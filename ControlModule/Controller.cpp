#include "Controller.h"

double TargetVelocity = TargetSpeed / 3.6; // [m/s]
double Handle = 0.0, Handle_1 = 0.0, Handle_2 = 0.0, Handle_3 = 0.0, Handle_4 = 0.0; //PurePursuit

struct Dynamics
{
    float m = 2060 + (75 * 2);  // Tare Mass [kg]
    float WB = 2.865;           // [m]
    float lf = 1.4;             // [m]
    float lr = WB - lf;         // [m]
    float wf = lr / (WB * m * Gravity);
    float wr = lf / (WB * m * Gravity);
    float cf = 1200 * 2; // N/rad 20000~30000
    float cr = 1200 * 2;
} IONIC5;



void ControlLogic::SCC()
{
    float Delta = 0;
    float SafetyDistance = 0;
    float Lambda = 0.6;
    float TimeToCollision = 2.2;

    //GetCurvature();
    // double PreviewAy = Vehicle.Velocity * Vehicle.Velocity * Control.Curvature;
    // double RefVelocity = sqrt(5/Control.Curvature);
    if(Vehicle.Radar.Distance < 70)
    {
        SafetyDistance = 4.635 + (TimeToCollision * Vehicle.Velocity) + 2;
        Delta = -Vehicle.Radar.Distance + SafetyDistance;
        if(Vehicle.Radar.Distance <= 5)
        {
            Control.Acceleration = 680;
        }
        else
        {
            Control.Acceleration = ((-1 * (((-1 * Vehicle.Radar.RelativeVelocity) + (Lambda * (Delta/20))) / TimeToCollision))+10.23)*100; 

            if((Control.Acceleration > 1023) && (Vehicle.Velocity >= TargetVelocity)) 
                Control.Acceleration = 1023 - ((Vehicle.Velocity - TargetVelocity) * 1.5);
        }
    }
    // else if(RefVelocity < TargetVelocity)
    // {
    //     if (Vehicle.Velocity < RefVelocity)
    //     {
    //         Control.Acceleration = 1033 + (RefVelocity - Vehicle.Velocity)*3;
    //     }
    //     else if (Vehicle.Velocity > RefVelocity)
    //     {
    //         Control.Acceleration = 1003 - (Vehicle.Velocity - RefVelocity)*4.25; //-0.2~-3.43m/s^2
    //     }
    //     else
    //     {
    //         Control.Acceleration = 1023;
    //     }
    // }
    else 
    {
        if (Vehicle.ClusterVelocity < TargetVelocity)
        {
            Control.Acceleration = 1033 + (TargetVelocity - Vehicle.ClusterVelocity)*3; //0.1~0.9m/s^2
        }
        else if (Vehicle.ClusterVelocity > TargetVelocity)
        {
            Control.Acceleration = 1003 - (Vehicle.ClusterVelocity - TargetVelocity)*4.25; //-0.2~-3.43m/s^2
        }
        else 
            Control.Acceleration = 1023; // 0 m/s^2
    }
    if(Control.Acceleration > 1113) Control.Acceleration = 1113; 
    if(Control.Acceleration < 680) Control.Acceleration = 680;
}


void ControlLogic::PurePursuit()
{
    double CurrentDistance;
    double RefSpeed;
    double HeadingError;
    double LookAheadDistance;
    double RadiusGain;
    uint32_t idx;

    double R; //Radius of path. VehicleCoordinate [0 -> Lookaheadidx]

    RefSpeed = Vehicle.Velocity * 3.6;
    if (RefSpeed < 3) RefSpeed = 3; // 원래는 1,1
    LookAheadDistance = RefSpeed * LookAheadGain;
    
    if (Local.Length)
    {
        for (uint32_t i=0; i<Local.Length; i++)
        {
            CurrentDistance = sqrt(pow(Local.X[i],2) + pow(Local.Y[i],2));

            if(CurrentDistance < LookAheadDistance)
            {
                idx = i;
            }
        }
        Control.LookAheadIdx = idx;

        if (idx >= Local.Length)
        {
            HeadingError = 0;
            cout << "[Controller/PurePursuit] ------------------ LookAheadPoint is not in a Local Path. LookaheadIndex : "<<Control.LookAheadIdx<<" LocalPathLength : "<< Local.Length << endl;
            return;
        }
        else HeadingError = atan2(Local.Y[idx], Local.X[idx]);

        if(Vehicle.Velocity < 0.25)   //if(Vehicle.ClusterVelocity < 2)
        {
            if (Handle > 0) Handle -= 0.1;
            else if (Handle < 0) Handle += 0.1;
            cout << "[Controller/PurePursuit] ------------------ Vehicle Stop State. making handle zero"<<endl;
            
        }
        else if(Vehicle.ClusterVelocity <= 31) // 도심로
        {
            double Vx = Vehicle.Velocity;
            double Gap = Global.LocalizationGap;
            double HandleGain;
            if(Vehicle.Velocity == 0) Vx = 0.01;
            if (Gap > 0.8) Gap = 0.8;

            // 깜빡이 ON : 둔감 -> 민감
            if (Vehicle.ClusterVelocity <= 13) 
            {
                GetLateraldeviation();
                RadiusGain = 3.2 + (0.8-Gap)*0.6;
                R = (RadiusGain * RadiusGain + (sin(HeadingError) * sin(HeadingError))) / (2*sin(HeadingError));

                Handle = 17.25 * (IONIC5.WB/R) * (180.0/M_PI); 
                
                if (Handle < 0) 
                {
                    if (Handle < -300 || Vehicle.YawRate < -6)  HandleGain = -4.0;  // 우회전할때 반응성 빠르게
                    else if (Handle < -225)                     HandleGain = -3.0; 
                    else if (Handle < -150)                     HandleGain = -1.5;
                    else if (Handle < -50)                      HandleGain = 2.0;
                    else                                        HandleGain = 1.0;
                }
                else 
                {
                    if (Handle > 300)       HandleGain = 4.0;
                    else if (Handle > 225)  HandleGain = 3.0;
                    else if (Handle > 150)  HandleGain = 1.5;
                    else if(Handle > 50)    HandleGain = -2.0;
                    else                    HandleGain = -1.0;
                }

            }
            // S자 구간에서 핸들 둔감하게
            else 
            {
                RadiusGain = 3 + (RefSpeed * 0.05);
                R = (RadiusGain * RadiusGain + (sin(HeadingError) * sin(HeadingError))) / (2*sin(HeadingError));

                Handle = 17.25 * (IONIC5.WB/R) * (180.0/M_PI); 
                
                if (Handle < 0) HandleGain = 1.0;
                else            HandleGain = -1.0;
            }
            
            if (Gap == 0) Handle = Handle_1;

            Handle = Handle + HandleGain * (Gap/(Vx))*(180.0/M_PI);
            if (Handle > 400) Handle = 400;
            else if (Handle <-400) Handle = -400;
            printf("[Controller/PurePursuit][PP+stanley] Handle:%d  Gap:%.2lf  END:%d  LD:%d  YawRate:%.2lf\n", (int)Handle, Gap, Local.Length, Control.LookAheadIdx, Vehicle.YawRate);
        }
        else // 자동차전용도로
        {
            R = LookAheadDistance / (2 * sin(HeadingError));
            Control.K = ((Vehicle.Velocity*Vehicle.Velocity)/(R*Gravity))*(IONIC5.wf/IONIC5.cf-IONIC5.wr/IONIC5.cr); // 23.08.07 . 부족 조향 구배는 값이 들어가고 있지 않았음. 코너링강성, 동적하중배분이 제대로 계산되지 않음.
            Handle = 17.25 * ((IONIC5.WB/R)*(180.0/M_PI) + Control.K);
            if (Handle > 400) Handle = 400;
            else if (Handle <-400) Handle = -400;
            printf("[Controller/PurePursuit][PP] Handle:%d  GAP:%.2lf  END:%d  LD:%d\n", (int)Handle, Global.LocalizationGap, Local.Length, Control.LookAheadIdx);
        }
    }
    else 
    {
        cout << "[Controller/PurePursuit] ------------------ LocalPath is invalid." << endl;
        Handle = 0.0;
    }
    Handle_4 = Handle_3;
    Handle_3 = Handle_2;
    Handle_2 = Handle_1;
    Handle_1 = Handle;

    Control.Handle = ((Handle_1*4. + Handle_2*3. + Handle_3*2. + Handle_4*1.) / 10.0);
}

void ControlLogic::GetCurvature()
{
    double distance, theta, K;
    Control.Curvature = 0;
    for (uint32_t i=1; i<Local.Length; i++)
    {
        //distance = sqrt(pow((Global.ValidLatitude[i] - Global.ValidLatitude[0]),2) - pow((Global.ValidLongitude[i] - Global.ValidLongitude[0]),2));
        //theta = atan2(Global.ValidLatitude[i], Global.ValidLongitude[i]);
        distance = sqrt(pow(Local.Y[i], 2) + pow(Local.X[i], 2));
        theta = atan2(Local.Y[i], Local.X[i]);
        K = (2 * sin(theta)) / distance;
        
        if (isnan(K) == 0)
        {
            if (abs(K) > Control.Curvature)
            {
                Control.Curvature = K;
            }
        }
    }
}

void ControlLogic::GetLateraldeviation()
{
    double Error;
    
    Error = Global.LocalizationGap;
    //Error = sqrt(pow((Global.ValidLatitude[0] - GPS.Latitude*110979.309),2) + pow((Global.ValidLongitude[0] - GPS.Longitude*88907.949),2));
    //Error = sqrt(pow(Local.X[0], 2) + pow(Local.Y[0], 2));
    if (Local.Y[0] < 0) Error = Error * (-1);
    Control.LateralDeviation = Error;

}

void ControlLogic::GetRelativeYawAngle()
{
    double Error;
    double PathDirection;
    double distance = 0;
    uint32_t NearestIdx;

    for (uint32_t i=0; i<Local.Length; i++)
    {
        distance = sqrt(pow((Global.ValidLatitude[i] - Global.ValidLatitude[0]), 2) + pow((Global.ValidLongitude[i] - Global.ValidLongitude[0]), 2));
        if (distance > 0.5) 
        {
            NearestIdx = i;
            break;
        }
    }
    //printf("IDX:%d  ", NearestIdx);
    PathDirection = atan2((Global.ValidLatitude[NearestIdx] - Global.ValidLatitude[0]),(Global.ValidLongitude[NearestIdx] - Global.ValidLongitude[0]));
    PathDirection = PathDirection*(180./M_PI);
    if(PathDirection > 90) PathDirection = PathDirection - 360;
    Error = (GPS.Azimuth * (-1) + 90)*(M_PI/180.); - PathDirection*(M_PI/180.);
    //printf("Azi[deg]:%.2lf  PathDir:%.2lf\n", (GPS.Azimuth * (-1) + 90), PathDirection);
    Error = Error * (-1);
    if (Error > 2.5) Error = 2.5;
    else if (Error < -2.5) Error = -2.5;
    Control.RelativeHeadingAngle = Error;
}