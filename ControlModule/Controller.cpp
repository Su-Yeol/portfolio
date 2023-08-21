#include "Controller.h"

//=========*
//  Asserts *
// =========
#ifndef utAssert
#if defined(DOASSERTS)
#if !defined(PRINT_ASSERTS)
#include <assert.h>
#define utAssert(exp)                  assert(exp)
#else
#include <stdio.h>

static void _assert(char *statement, char *file, int line)
{
    printf("%s in %s on line %d\n", statement, file, line);
}

#define utAssert(_EX)                  ((_EX) ? (void)0 : _assert(#_EX, __FILE__, __LINE__))
#endif

#else
#define utAssert(exp)                                            // do nothing
#endif
#endif

double TargetVelocity = TargetSpeed / 3.6; // [m/s]


struct Dynamics
{
    float m = 2060 + (75 + 2);  // Tare Mass [kg]
    float WB = 2.865;           // [m]
    float lf = 1.4;             // [m]
    float lr = WB - lf;         // [m]
    float wf = lr / (WB * m * Gravity);
    float wr = lf / (WB * m * Gravity);
    float cf = 1200 * 2;
    float cr = 1200 * 2;
} IONIC5;

// ModelPredictiveController Parameters
const double RMDscale{ 100.0 };
const double RMVscale{ 1.9230769230769229 };
const double Wdu{ 0.047817624989501852 };
const double Wu{ 0.0 };
const int32_t degrees{ 3 };
const int32_t p{ 10 };

double rtInf;
double rtMinusInf;
double rtNaN;
float rtInfF;
float rtMinusInfF;
float rtNaNF;

struct LittleEndianIEEEDouble {
    struct {
        uint32_t wordL;
        uint32_t wordH;
    } words;
};

struct IEEESingle {
    union {
        float wordLreal;
        uint32_t wordLuint;
    } wordL;
};

const ModelPredictiveControl::ConstP rtConstP{
    // Pooled Parameter (Mixed Expressions)
    //  Referenced by:
    //    '<S2>/Vehicle dynamics matrix A constant'
    //    '<S2>/Vehicle dynamics matrix C constant'
    //    '<S5>/DX Constant'
    //    '<S5>/X Constant'

    { 0.0, 0.0, 0.0, 0.0 },

    // Expression: lastPcov
    //  Referenced by: '<S11>/LastPcov'

    { 0.001387927720107651, 0.0014936516055697644, 4.5410965004370436E-6,
        3.0991556097227081E-6, 9.2074126036463637E-18, 0.0014936516055697644,
        0.0016074288930896874, 4.887009590384106E-6, 3.3352304196404587E-6,
        4.8907879594542963E-17, 4.5410965004370436E-6, 4.887009590384106E-6,
        0.00026015963098652343, 0.00025291581967499, -0.0025262408565442469,
        3.0991556097227081E-6, 3.3352304196404587E-6, 0.00025291581967499,
        0.00050213542880234293, -0.0049687281472600275, 9.2074126036463637E-18,
        4.8907879594542963E-17, -0.0025262408565442469, -0.0049687281472600275,
        0.10069730444687652 }
};
// end


void Longitudinal::SCC()
{
    float Delta = 0;
    float SafetyDistance = 0;
    float Lambda = 0.6;
    float TimeToCollision = 2.2;

    GetCurvature();
    double PreviewAy = Vehicle.Velocity * Vehicle.Velocity * Curvature;
    double RefVelocity = sqrt(Curvature/5);
    printf("Curvature:%.lf  PreviewAy:%.lf  RefVel:%.lf\n", Curvature, PreviewAy, RefVelocity);
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
    else if(PreviewAy > 5)
    {
        Control.Acceleration = 1003 - (Vehicle.Velocity - RefVelocity)*4.25; //-0.2~-3.43m/s^2
    }
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

void Longitudinal::GetCurvature()
{
    Curvature = 0;
    double distance, theta, K;
    for (uint32_t i=0; i<Local.Length; i++)
    {
        distance = sqrt(pow(Local.Y[i], 2) + pow(Local.X[i], 2));
        theta = atan2(Local.Y[i], Local.X[i]);
        K = (2 * sin(theta)) / distance;
        
        if (isnan(K) == 0)
        {
            if (abs(K) > Curvature)
            {
                Curvature = K;
            }
        }
    }
}


void Lateral::PurPursuit()
{
    double CurrentDistance;
    double Handle = 0.0, Handle_1 = 0.0, Handle_2 = 0.0, Handle_3 = 0.0, Handle_4 = 0.0;
    double RefSpeed;
    
    RefSpeed = Vehicle.Velocity * 3.6;
    if (RefSpeed < 8) RefSpeed = 8;
    else if (RefSpeed > 70) RefSpeed = 70;

    LookAheadDistance = RefSpeed * LookAheadGain;

    if (Local.Length)
    {
        for (uint32_t i=0; i<Local.Length; i++)
        {
            CurrentDistance = sqrt(pow(Local.X[i],2) + pow(Local.Y[i],2));
            if(CurrentDistance < LookAheadDistance)
            {
                LookAheadIdx = i;
            }
        }
    }

    HeadingError = atan2(Local.Y[LookAheadIdx], Local.X[LookAheadIdx]);
    
    Curvature = LookAheadDistance / (2 * sin(HeadingError));

    Handle = 17.25 * ((IONIC5.WB/Curvature)*57.3 + ((Vehicle.Velocity*Vehicle.Velocity)/(Curvature*Gravity))*(IONIC5.wf/IONIC5.cf-IONIC5.wr/IONIC5.cr));

    if (Handle > 400) Handle = 400;
    else if (Handle <-400) Handle = -400;

    Handle_4 = Handle_3;
    Handle_3 = Handle_2;
    Handle_2 = Handle_1;
    Handle_1 = Handle;
    Control.Handle = ((Handle_1*4. + Handle_2*3. + Handle_3*2. + Handle_4) / 10.0);
}


double GetLateraldeviation()
{
    double Error;
    
    Error = sqrt(pow(Local.X[0], 2) + pow(Local.Y[0], 2));
    if (Local.Y[0] < 0) Error = Error * (-1);

    return Error;
}

double GetRelativeYawAngle()
{
    double Error;
    double PathDirection;
    double distance = 0;
    uint32_t NearestIdx;

    for (uint32_t i=0; i<Local.Length; i++)
    {
        distance = sqrt(pow((Global.ValidLatitude[i] - Global.ValidLatitude[0]), 2) + pow((Global.ValidLongitude[i] - Global.ValidLongitude[0]), 2));
        if (distance > 0) 
        {
        NearestIdx = i;
        break;
        }
    }

    PathDirection = atan2((Global.ValidLatitude[NearestIdx] - Global.ValidLatitude[0]),(Global.ValidLongitude[NearestIdx] - Global.ValidLongitude[0]));
    Error = GPS.Azimuth*(M_PI/180.) - PathDirection;

    return Error;
}


// Model step function
void ModelPredictiveControl::GetSteeringAngle()
{
    static const double d[30]{ 0.037254901960784313, 0.040092753623188405,
        0.00012189259027485707, 8.3187861103111493E-5, 0.0, 0.0, 0.0,
        -1.2500000000000002E-7, -0.0005, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.05, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

    static const double c[25]{ 3.3478172233201038E-85, 1.394552216603366E-85,
        0.00023204670732311508, 4.6687938036162162E-5, 0.0,
        1.8141707008020418E-85, 7.5570307556735628E-86, 6.0808644354296453E-5,
        0.00014524266225254362, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0005,
        1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0 };

    static const double t[20]{ 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

    static const double r[12]{ -1.0, -1.0, 1.0, 1.0, -0.0, -1.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0 };

    static const double q[9]{ 0.047823845508973471, 5.3248401564893519E-6, 0.0,
        5.3248401564893528E-6, 0.047822217098493117, 0.0, 0.0, 0.0, 100000.0 };

    static const double g[8]{ 0.52, 0.52, 0.52, 0.52, 0.01, 0.01, 0.01, 0.01 };

    static const double h[8]{ 0.5, 0.1, 0.5, 0.1, 0.5, 0.1, 0.5, 0.1 };

    static const double o[4]{ -1.0, -1.0, 1.0, 1.0 };

    static const double s[2]{ 0.20912791051825463, 0.0020912791051825464 };

    static const int32_t b_Mrows[4]{ 41, 42, 51, 52 };

    static const int8_t f[12]{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1 };

    static const int8_t e[10]{ 0, 0, 0, 0, 2, 0, 0, 10, 0, 1 };

    static const int8_t tmp[8]{ 0, 0, 0, 0, 1, 0, 0, 1 };

    static const int8_t c_a_2[6]{ 1, 0, 0, 1, 0, -1 };

    int8_t c_a;
    int8_t c_a_0;
    int8_t c_a_1;

    // MATLAB Function: '<S5>/DataTypeConversion_e1'
    DataTypeConversion_umax(0.0, &rtDW.y_gx);

    // MATLAB Function: '<S5>/DataTypeConversion_e2'
    DataTypeConversion_umax(0.0, &rtDW.y_o);

    // MATLAB Function: '<S5>/DataTypeConversion_Vx'
    DataTypeConversion_umax(0.0, &rtDW.y_j);

    // Product: '<S5>/Product' incorporates:
    //   MATLAB Function: '<S5>/CurvatureConversion'

    rtDW.Product[0] = 0.0 * rtDW.y_j;
    for (rtDW.i = 0; rtDW.i < 10; rtDW.i++) {
        rtDW.Product[rtDW.i + 1] = 0.0 * rtDW.y_j;
    }

    // End of Product: '<S5>/Product'

    // MATLAB Function: '<S3>/DataTypeConversion_umin' incorporates:
    //   Constant: '<S2>/Minimum steering angle constant'

    DataTypeConversion_umax(-0.26, &rtDW.y_a);

    // MATLAB Function: '<S3>/DataTypeConversion_umax' incorporates:
    //   Constant: '<S2>/Maximum steering angle constant'

    DataTypeConversion_umax(0.26, &rtDW.y_hh);

    // MATLAB Function: '<S5>/DataTypeConversion_Ts' incorporates:
    //   Constant: '<S5>/Sample time constant'

    DataTypeConversion_umax(0.05, &rtDW.y_h);

    // MATLAB Function: '<S51>/DataTypeConversion_rearstiff' incorporates:
    //   Constant: '<S51>/Vehicle rear tire cornering stiffness constant'

    DataTypeConversion_umax(33000.0, &rtDW.y);

    // MATLAB Function: '<S51>/DataTypeConversion_frontstiff' incorporates:
    //   Constant: '<S51>/Vehicle front tire cornering stiffness constant'

    DataTypeConversion_umax(19000.0, &rtDW.y_c);

    // MATLAB Function: '<S51>/DataTypeConversion_mass' incorporates:
    //   Constant: '<S51>/Vehicle mass constant'

    DataTypeConversion_umax(2210.0, &rtDW.y_m);

    // MATLAB Function: '<S51>/DataTypeConversion_rear' incorporates:
    //   Constant: '<S51>/Vehicle length to rear constant'

    DataTypeConversion_umax(1.465, &rtDW.y_k);

    // MATLAB Function: '<S51>/DataTypeConversion_front' incorporates:
    //   Constant: '<S51>/Vehicle length to front constant'

    DataTypeConversion_umax(1.4, &rtDW.y_g);

    // Product: '<S51>/Product1' incorporates:
    //   Gain: '<S51>/Gain1'
    //   Product: '<S51>/Divide1'

    rtDW.rtb_a4_tmp = rtDW.y_g * rtDW.y_c;

    // MATLAB Function: '<S51>/DataTypeConversion_inertia' incorporates:
    //   Constant: '<S51>/Vehicle yaw inertia constant'

    DataTypeConversion_umax(2875.0, &rtDW.y_f);

    // SignalConversion generated from: '<S51>/Matrix Concatenate' incorporates:
    //   Gain: '<S51>/Gain2'
    //   Product: '<S51>/Divide2'
    //   Sum: '<S51>/Sum'

    rtDW.Am[0] = (rtDW.y + rtDW.y_c) * -2.0 / rtDW.y_m / rtDW.y_j;

    // Product: '<S51>/Divide4' incorporates:
    //   Gain: '<S51>/Gain3'
    //   Product: '<S51>/Divide3'
    //   Product: '<S51>/Product'
    //   Product: '<S51>/Product1'
    //   Sum: '<S51>/Sum1'

    rtDW.rtb_Am_tmp = (rtDW.rtb_a4_tmp - rtDW.y_k * rtDW.y) * -2.0 / rtDW.y_j;

    // SignalConversion generated from: '<S51>/Matrix Concatenate' incorporates:
    //   Product: '<S51>/Divide4'

    rtDW.Am[1] = rtDW.rtb_Am_tmp / rtDW.y_f;

    // SignalConversion generated from: '<S51>/Matrix Concatenate' incorporates:
    //   Gain: '<S51>/Gain4'
    //   Product: '<S51>/Divide3'
    //   Product: '<S51>/Divide5'
    //   Product: '<S51>/Divide6'
    //   Product: '<S51>/Divide7'
    //   Sum: '<S51>/Sum2'
    //   Sum: '<S51>/Sum3'

    rtDW.Am[2] = rtDW.rtb_Am_tmp / rtDW.y_m - rtDW.y_j;
    rtDW.Am[3] = (rtDW.y_k * rtDW.y_k * rtDW.y + rtDW.y_g * rtDW.y_g * rtDW.y_c)
        * -2.0 / rtDW.y_j / rtDW.y_f;

    // MATLAB Function: '<S5>/DataTypeConversion_lag' incorporates:
    //   Constant: '<S5>/Transport lag constant'

    DataTypeConversion_umax(0.0, &rtDW.y);

    // MATLAB Function: '<S5>/Adaptive Model' incorporates:
    //   Concatenate: '<S51>/Matrix Concatenate'
    //   Constant: '<S51>/Cm Constant'
    //   Gain: '<S51>/Gain'
    //   Gain: '<S51>/Gain1'
    //   Product: '<S51>/Divide'
    //   Product: '<S51>/Divide1'

    rtDW.iv[0] = 1;
    rtDW.iv[1] = 0;
    rtDW.iv[2] = 0;
    rtDW.iv[3] = 0;
    rtDW.iv[4] = 1;
    rtDW.iv[5] = 0;
    for (rtDW.i = 0; rtDW.i < 2; rtDW.i++) {
        c_a = c_a_2[rtDW.i + 2];
        c_a_0 = c_a_2[rtDW.i];
        c_a_1 = c_a_2[rtDW.i + 4];
        for (rtDW.r2 = 0; rtDW.r2 < 2; rtDW.r2++) {
            rtDW.c_a[rtDW.i + (rtDW.r2 << 1)] = static_cast<int8_t>((rtDW.iv[3 *
                rtDW.r2 + 1] * c_a + rtDW.iv[3 * rtDW.r2] * c_a_0) + rtDW.iv[3 *
                rtDW.r2 + 2] * c_a_1);
            rtDW.a[rtDW.r2 + (rtDW.i << 2)] = rtDW.Am[(rtDW.i << 1) + rtDW.r2];
            rtDW.a[rtDW.r2 + ((rtDW.i + 2) << 2)] = 0.0;
        }
    }

    rtDW.a[10] = 0.0;
    rtDW.a[14] = rtDW.y_j;
    rtDW.b[0] = rtDW.y_c / rtDW.y_m * 2.0;
    rtDW.b[1] = rtDW.rtb_a4_tmp / rtDW.y_f * 2.0;
    rtDW.a[2] = rtDW.c_a[0];
    rtDW.a[3] = rtDW.c_a[1];
    rtDW.a[11] = 0.0;
    rtDW.b[4] = 0.0;
    rtDW.a[6] = rtDW.c_a[2];
    rtDW.a[7] = rtDW.c_a[3];
    rtDW.a[15] = 0.0;
    rtDW.b[5] = 0.0;
    rtDW.b[2] = 0.0;
    rtDW.b[3] = 0.0;
    rtDW.b[6] = 0.0;
    rtDW.b[7] = -1.0;
    if (rtDW.y_h > 0.0) {
        for (rtDW.i = 0; rtDW.i < 16; rtDW.i++) {
            rtDW.a_k[rtDW.i] = rtDW.a[rtDW.i] * rtDW.y_h;
        }

        expmNoLog2(rtDW.a_k, rtDW.A);
        rtDW.y_j = rtDW.y_h / 4.0;
        memset(&rtDW.Ai[0], 0, sizeof(double) << 4U);
        rtDW.Ai[0] = 1.0;
        rtDW.Ai[5] = 1.0;
        rtDW.Ai[10] = 1.0;
        rtDW.Ai[15] = 1.0;
        for (rtDW.i = 0; rtDW.i < 16; rtDW.i++) {
            rtDW.Ai[rtDW.i] += rtDW.A[rtDW.i];
        }

        rtDW.Coef = 2;
        for (rtDW.r2 = 0; rtDW.r2 < 3; rtDW.r2++) {
            if (rtDW.Coef == 2) {
                rtDW.Coef = 4;
            } else {
                rtDW.Coef = 2;
            }

            for (rtDW.i = 0; rtDW.i < 16; rtDW.i++) {
                rtDW.a_k[rtDW.i] = static_cast<double>(rtDW.r2 + 1) *
                    rtDW.a[rtDW.i] * rtDW.y_j;
            }

            expmNoLog2(rtDW.a_k, rtDW.dv2);
            for (rtDW.i = 0; rtDW.i < 16; rtDW.i++) {
                rtDW.Ai[rtDW.i] += static_cast<double>(rtDW.Coef) *
                    rtDW.dv2[rtDW.i];
            }
        }

        rtDW.y_j /= 3.0;
        for (rtDW.i = 0; rtDW.i < 4; rtDW.i++) {
            rtDW.y = rtDW.Ai[rtDW.i + 4];
            rtDW.y_c = rtDW.Ai[rtDW.i];
            rtDW.y_h = rtDW.Ai[rtDW.i + 8];
            rtDW.y_m = rtDW.Ai[rtDW.i + 12];
            for (rtDW.r2 = 0; rtDW.r2 < 2; rtDW.r2++) {
                rtDW.Coef = rtDW.r2 << 2;
                rtDW.rtb_y_j_g[rtDW.i + rtDW.Coef] = ((rtDW.y * rtDW.y_j *
                    rtDW.b[rtDW.Coef + 1] + rtDW.y_j * rtDW.y_c *
                    rtDW.b[rtDW.Coef]) + rtDW.y_h * rtDW.y_j * 0.0) + rtDW.y_m *
                    rtDW.y_j * rtDW.b[rtDW.Coef + 3];
            }
        }

        memcpy(&rtDW.b[0], &rtDW.rtb_y_j_g[0], sizeof(double) << 3U);
    } else {
        memcpy(&rtDW.A[0], &rtDW.a[0], sizeof(double) << 4U);
    }

    // MATLAB Function: '<S39>/FixedHorizonOptimizer' incorporates:
    //   Constant: '<S5>/DX Constant'
    //   MATLAB Function: '<S5>/Adaptive Model'
    //   Memory: '<S11>/LastPcov'
    //   Memory: '<S11>/last_x'
    //   Product: '<S5>/Product'

    memset(&rtDW.Bu[0], 0, 55U * sizeof(double));
    memset(&rtDW.Bv[0], 0, 110U * sizeof(double));
    memset(&rtDW.Dv[0], 0, 44U * sizeof(double));
    for (rtDW.i = 0; rtDW.i < 44; rtDW.i++) {
        rtDW.Dvm[rtDW.i] = 0;
    }

    memset(&rtDW.Cm[0], 0, 110U * sizeof(double));
    memcpy(&rtDW.b_A[0], &c[0], 25U * sizeof(double));
    memcpy(&rtDW.b_B[0], &d[0], 30U * sizeof(double));
    for (rtDW.i = 0; rtDW.i < 10; rtDW.i++) {
        rtDW.b_C[rtDW.i] = e[rtDW.i];
    }

    for (rtDW.i = 0; rtDW.i < 12; rtDW.i++) {
        rtDW.b_D_n[rtDW.i] = f[rtDW.i];
    }

    for (rtDW.i = 0; rtDW.i < 8; rtDW.i++) {
        rtDW.b[rtDW.i] *= g[rtDW.i];
    }

    for (rtDW.i = 0; rtDW.i < 4; rtDW.i++) {
        // MATLAB Function: '<S5>/Adaptive Model'
        rtDW.r2 = rtDW.i << 1;
        rtDW.b_C[rtDW.r2] = static_cast<double>(tmp[rtDW.r2]) / h[rtDW.r2];
        rtDW.b_C[rtDW.r2 + 1] = static_cast<double>(tmp[rtDW.r2 + 1]) /
            h[rtDW.r2 + 1];
        rtDW.r2 = rtDW.i << 2;
        rtDW.b_A[5 * rtDW.i] = rtDW.A[rtDW.r2];
        rtDW.b_A[5 * rtDW.i + 1] = rtDW.A[rtDW.r2 + 1];
        rtDW.b_A[5 * rtDW.i + 2] = rtDW.A[rtDW.r2 + 2];
        rtDW.b_A[5 * rtDW.i + 3] = rtDW.A[rtDW.r2 + 3];
        rtDW.b_B[rtDW.i] = rtDW.b[rtDW.i];
        rtDW.b_B[rtDW.i + 5] = rtDW.b[rtDW.i + 4];
    }

    rtDW.b_D_n[2] = 0;
    rtDW.b_D_n[3] = 0;
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        rtDW.Bu[rtDW.i] = rtDW.b_B[rtDW.i];
    }

    memcpy(&rtDW.Bv[0], &rtDW.b_B[5], 10U * sizeof(double));
    rtDW.Dvm[0] = 0;
    rtDW.Dvm[1] = 0;
    rtDW.Dvm[2] = rtDW.b_D_n[4];
    rtDW.Dvm[3] = rtDW.b_D_n[5];
    rtDW.UnknownIn[0] = 1;
    rtDW.UnknownIn[1] = 2;
    rtDW.UnknownIn[2] = 4;
    rtDW.UnknownIn[3] = 5;
    rtDW.UnknownIn[4] = 6;
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        rtDW.r2 = rtDW.i << 1;
        rtDW.Cm[rtDW.r2] = rtDW.b_C[rtDW.r2];
        rtDW.Cm[rtDW.r2 + 1] = rtDW.b_C[rtDW.r2 + 1];
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            rtDW.b_tmp[rtDW.r2 + 7 * rtDW.i] = rtDW.b_B[(rtDW.UnknownIn[rtDW.i]
                - 1) * 5 + rtDW.r2];
        }

        rtDW.r2 = (rtDW.UnknownIn[rtDW.i] - 1) << 1;
        rtDW.b_tmp[7 * rtDW.i + 5] = rtDW.b_D_n[rtDW.r2];
        rtDW.b_tmp[7 * rtDW.i + 6] = rtDW.b_D_n[rtDW.r2 + 1];
    }

    for (rtDW.i = 0; rtDW.i < 7; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 7; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.b_tmp[7 * rtDW.Coef + rtDW.i] * rtDW.b_tmp[7 *
                    rtDW.Coef + rtDW.r2];
            }

            rtDW.CovMat[rtDW.i + 7 * rtDW.r2] = rtDW.y_j;
        }
    }

    rtDW.Dv[0] = 0.0;
    rtDW.Dv[1] = 0.0;
    rtDW.Dv[2] = rtDW.b_D_n[4];
    rtDW.Dv[3] = rtDW.b_D_n[5];
    rtDW.Am[0] = 0.5;
    rtDW.Am[1] = 0.5;
    rtDW.Am[2] = 0.5;
    rtDW.Am[3] = 0.5;
    memset(&rtDW.b_utarget[0], 0, 10U * sizeof(double));
    rtDW.Bv[5] = 0.0;
    rtDW.Bv[6] = 0.0;
    rtDW.Bv[7] = 0.0;
    rtDW.Bv[8] = 0.0;
    memset(&rtDW.vseq[0], 0, 22U * sizeof(double));
    for (rtDW.r2 = 0; rtDW.r2 < 11; rtDW.r2++) {
        rtDW.vseq[(rtDW.r2 << 1) + 1] = 1.0;
    }

    for (rtDW.Coef = 0; rtDW.Coef < 10; rtDW.Coef++) {
        rtDW.i = rtDW.Coef << 1;
        rtDW.rseq[rtDW.i] = 0.0;
        rtDW.rseq[rtDW.i + 1] = 0.0;
    }

    for (rtDW.Coef = 0; rtDW.Coef < 11; rtDW.Coef++) {
        rtDW.vseq[rtDW.Coef << 1] = RMDscale * rtDW.Product[rtDW.Coef];
    }

    rtDW.y = rtDW.vseq[0];
    rtDW.y_c = rtDW.vseq[1];
    rtDW.c_a[0] = 0;
    rtDW.c_a[1] = 0;
    rtDW.c_a[2] = 0;
    rtDW.c_a[3] = 0;
    for (rtDW.Coef = 0; rtDW.Coef < 2; rtDW.Coef++) {
        rtDW.c_a[rtDW.Coef + (rtDW.Coef << 1)] = 1;
        for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
            rtDW.L_tmp_d = (rtDW.i << 1) + rtDW.Coef;
            rtDW.L[rtDW.i + 5 * rtDW.Coef] = rtDW.Cm[rtDW.L_tmp_d];
            rtDW.y_j = 0.0;
            for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
                rtDW.y_j += rtDW.Cm[(rtDW.r2 << 1) + rtDW.Coef] *
                    rtDW.LastPcov_PreviousInput[5 * rtDW.i + rtDW.r2];
            }

            rtDW.Cm_c[rtDW.L_tmp_d] = rtDW.y_j;
        }
    }

    for (rtDW.i = 0; rtDW.i < 2; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 2; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.Cm_c[(rtDW.Coef << 1) + rtDW.i] * rtDW.L[5 *
                    rtDW.r2 + rtDW.Coef];
            }

            rtDW.c_A[rtDW.i + (rtDW.r2 << 1)] = rtDW.CovMat[((rtDW.r2 + 5) * 7 +
                rtDW.i) + 5] + rtDW.y_j;
        }
    }

    if (std::abs(rtDW.c_A[1]) > std::abs(rtDW.c_A[0])) {
        rtDW.Coef = 1;
        rtDW.r2 = 0;
    } else {
        rtDW.Coef = 0;
        rtDW.r2 = 1;
    }

    rtDW.y_j = rtDW.c_A[rtDW.r2] / rtDW.c_A[rtDW.Coef];
    rtDW.y_m = rtDW.c_A[rtDW.Coef + 2];
    rtDW.y_h = rtDW.c_A[rtDW.r2 + 2] - rtDW.y_m * rtDW.y_j;
    rtDW.i = rtDW.Coef << 1;
    rtDW.Kinv[rtDW.i] = static_cast<double>(rtDW.c_a[0]) / rtDW.c_A[rtDW.Coef];
    rtDW.r2 <<= 1;
    rtDW.Kinv[rtDW.r2] = (static_cast<double>(rtDW.c_a[2]) - rtDW.Kinv[rtDW.i] *
                          rtDW.y_m) / rtDW.y_h;
    rtDW.Kinv[rtDW.i] -= rtDW.Kinv[rtDW.r2] * rtDW.y_j;
    rtDW.Kinv[rtDW.i + 1] = static_cast<double>(rtDW.c_a[1]) /
        rtDW.c_A[rtDW.Coef];
    rtDW.Kinv[rtDW.r2 + 1] = (static_cast<double>(rtDW.c_a[3]) -
        rtDW.Kinv[rtDW.i + 1] * rtDW.y_m) / rtDW.y_h;
    rtDW.Kinv[rtDW.i + 1] -= rtDW.Kinv[rtDW.r2 + 1] * rtDW.y_j;
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.b_A[5 * rtDW.Coef + rtDW.i] *
                    rtDW.LastPcov_PreviousInput[5 * rtDW.r2 + rtDW.Coef];
            }

            rtDW.L_tmp[rtDW.i + 5 * rtDW.r2] = rtDW.y_j;
        }

        for (rtDW.r2 = 0; rtDW.r2 < 2; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.L_tmp[5 * rtDW.Coef + rtDW.i] * rtDW.L[5 *
                    rtDW.r2 + rtDW.Coef];
            }

            rtDW.Cm_c[rtDW.i + 5 * rtDW.r2] = rtDW.CovMat[(rtDW.r2 + 5) * 7 +
                rtDW.i] + rtDW.y_j;
        }
    }

    rtDW.y_h = rtDW.Kinv[1];
    rtDW.y_m = rtDW.Kinv[0];
    rtDW.y_k = rtDW.Kinv[3];
    rtDW.y_g = rtDW.Kinv[2];
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        rtDW.y_j = rtDW.Cm_c[rtDW.i + 5];
        rtDW.rtb_a4_tmp = rtDW.Cm_c[rtDW.i];
        rtDW.L[rtDW.i] = rtDW.y_j * rtDW.y_h + rtDW.rtb_a4_tmp * rtDW.y_m;
        rtDW.L[rtDW.i + 5] = rtDW.y_j * rtDW.y_k + rtDW.rtb_a4_tmp * rtDW.y_g;
        rtDW.xk[rtDW.i] = rtDW.Bu[rtDW.i] * 0.0 +
            rtDW.last_x_PreviousInput[rtDW.i];
    }

    // SignalConversion generated from: '<S40>/ SFunction ' incorporates:
    //   MATLAB Function: '<S39>/FixedHorizonOptimizer'

    rtDW.rtb_y_gx_n[0] = rtDW.y_gx * 2.0;
    rtDW.rtb_y_gx_n[1] = rtDW.y_o * 10.0;

    // MATLAB Function: '<S39>/FixedHorizonOptimizer' incorporates:
    //   Memory: '<S11>/LastPcov'

    for (rtDW.i = 0; rtDW.i < 2; rtDW.i++) {
        rtDW.y_j = 0.0;
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            rtDW.y_j += rtDW.Cm[(rtDW.r2 << 1) + rtDW.i] * rtDW.xk[rtDW.r2];
        }

        rtDW.y_innov[rtDW.i] = rtDW.rtb_y_gx_n[rtDW.i] - ((static_cast<double>
            (rtDW.Dvm[rtDW.i + 2]) * rtDW.y_c + static_cast<double>
            (rtDW.Dvm[rtDW.i]) * rtDW.y) + rtDW.y_j);
    }

    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 2; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.LastPcov_PreviousInput[5 * rtDW.Coef + rtDW.i] *
                    rtDW.Cm[(rtDW.Coef << 1) + rtDW.r2];
            }

            rtDW.dv3[rtDW.i + 5 * rtDW.r2] = rtDW.y_j;
        }

        rtDW.y_j = rtDW.dv3[rtDW.i + 5];
        rtDW.y_gx = rtDW.dv3[rtDW.i];
        rtDW.xk_g[rtDW.i] = ((rtDW.y_j * rtDW.Kinv[1] + rtDW.y_gx * rtDW.Kinv[0])
                             * rtDW.y_innov[0] + (rtDW.y_j * rtDW.Kinv[3] +
                              rtDW.y_gx * rtDW.Kinv[2]) * rtDW.y_innov[1]) +
            rtDW.xk[rtDW.i];
    }

    memset(&rtDW.dv1[0], 0, 20U * sizeof(double));
    memset(&rtDW.dv[0], 0, 88U * sizeof(double));
    for (rtDW.i = 0; rtDW.i < 10; rtDW.i++) {
        rtDW.dv3[rtDW.i] = 1.0;
    }

    // Memory: '<S11>/Memory'
    for (rtDW.r2 = 0; rtDW.r2 < 4; rtDW.r2++) {
        rtDW.bv[rtDW.r2] = rtDW.Memory_PreviousInput[rtDW.r2];
    }

    // End of Memory: '<S11>/Memory'

    // MATLAB Function: '<S39>/FixedHorizonOptimizer'
    for (rtDW.i_l = 0; rtDW.i_l < 4; rtDW.i_l++) {
        rtDW.c_A[rtDW.i_l] = o[rtDW.i_l];
    }

    memcpy(&rtDW.q[0], &q[0], 9U * sizeof(double));
    memcpy(&rtDW.r[0], &r[0], 12U * sizeof(double));

    // Update for Memory: '<S11>/Memory' incorporates:
    //   Constant: '<S2>/Enable optimization constant'
    //   MATLAB Function: '<S39>/FixedHorizonOptimizer'
    //   MATLAB Function: '<S3>/DataTypeConversion_optsgn'
    //   UnitDelay: '<S11>/last_mv'

    mpcblock_optimizer(rtDW.rseq, rtDW.vseq, rtDW.y_a, rtDW.y_hh, 0, rtDW.xk_g,
                       rtDW.last_mv_DSTATE, rtDW.bv, rtDW.Am, rtDW.dv1, rtDW.c_A,
                       rtDW.dv, rtDW.b_utarget, 0.0, 0, rtDW.q, rtDW.r, s, t,
                       rtDW.dv3, rtDW.b_A, rtDW.Bu, rtDW.Bv, rtDW.b_C, rtDW.Dv,
                       b_Mrows, &rtDW.y_gx, rtDW.Product, &rtDW.y_o,
                       rtDW.Memory_PreviousInput);

    // MATLAB Function: '<S39>/FixedHorizonOptimizer'
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            rtDW.y_j = 0.0;
            for (rtDW.Coef = 0; rtDW.Coef < 5; rtDW.Coef++) {
                rtDW.y_j += rtDW.L_tmp[5 * rtDW.Coef + rtDW.i] * rtDW.b_A[5 *
                    rtDW.Coef + rtDW.r2];
            }

            rtDW.Coef = 5 * rtDW.r2 + rtDW.i;
            rtDW.L_tmp_m[rtDW.Coef] = rtDW.y_j;
            rtDW.L_tmp_c[rtDW.Coef] = rtDW.Cm_c[rtDW.i + 5] * rtDW.L[rtDW.r2 + 5]
                + rtDW.Cm_c[rtDW.i] * rtDW.L[rtDW.r2];
        }
    }

    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            rtDW.Coef = 5 * rtDW.i + rtDW.r2;
            rtDW.L_tmp[rtDW.Coef] = rtDW.CovMat[7 * rtDW.i + rtDW.r2] +
                (rtDW.L_tmp_m[rtDW.Coef] - rtDW.L_tmp_c[rtDW.Coef]);
        }
    }

    // MATLAB Function: '<S5>/DataTypeConversion_A' incorporates:
    //   Constant: '<S2>/Vehicle dynamics matrix A constant'

    DataTypeConversion_A(rtConstP.pooled4, rtDW.Am);

    // MATLAB Function: '<S5>/DataTypeConversion_C' incorporates:
    //   Constant: '<S2>/Vehicle dynamics matrix C constant'

    DataTypeConversion_A(rtConstP.pooled4, rtDW.Am);

    // Assertion: '<S4>/Assertion'
    utAssert(false);

    // MATLAB Function: '<S3>/DataTypeConversion_utrack' incorporates:
    //   Constant: '<S2>/External control signal constant'

    DataTypeConversion_umax(0.0, &rtDW.y_hh);

    // Update for UnitDelay: '<S11>/last_mv' incorporates:
    //   MATLAB Function: '<S39>/FixedHorizonOptimizer'

    rtDW.last_mv_DSTATE = rtDW.y_gx;
    for (rtDW.i = 0; rtDW.i < 5; rtDW.i++) {
        // MATLAB Function: '<S39>/FixedHorizonOptimizer'
        rtDW.y_j = 0.0;
        for (rtDW.r2 = 0; rtDW.r2 < 5; rtDW.r2++) {
            // MATLAB Function: '<S39>/FixedHorizonOptimizer'
            rtDW.Coef = 5 * rtDW.i + rtDW.r2;
            rtDW.L_tmp_d = 5 * rtDW.r2 + rtDW.i;

            // Update for Memory: '<S11>/LastPcov' incorporates:
            //   MATLAB Function: '<S39>/FixedHorizonOptimizer'

            rtDW.LastPcov_PreviousInput[rtDW.Coef] = (rtDW.L_tmp[rtDW.Coef] +
                rtDW.L_tmp[rtDW.L_tmp_d]) * 0.5;

            // MATLAB Function: '<S39>/FixedHorizonOptimizer'
            rtDW.y_j += rtDW.b_A[rtDW.L_tmp_d] * rtDW.xk[rtDW.r2];
        }

        // Update for Memory: '<S11>/last_x' incorporates:
        //   MATLAB Function: '<S39>/FixedHorizonOptimizer'

        rtDW.last_x_PreviousInput[rtDW.i] = ((rtDW.Bv[rtDW.i + 5] * rtDW.y_c +
            rtDW.Bv[rtDW.i] * rtDW.y) + (rtDW.Bu[rtDW.i] * rtDW.y_gx + rtDW.y_j))
            + (rtDW.L[rtDW.i + 5] * rtDW.y_innov[1] + rtDW.L[rtDW.i] *
               rtDW.y_innov[0]);
    }
}

// Model initialize function
void ModelPredictiveControl::initialize()
{
    // Registration code

    // initialize non-finites
    rt_InitInfAndNaN(sizeof(double));

    // InitializeConditions for Memory: '<S11>/LastPcov'
    memcpy(&rtDW.LastPcov_PreviousInput[0],
                &rtConstP.LastPcov_InitialCondition[0], 25U * sizeof(double));
}

// Constructor
ModelPredictiveControl::ModelPredictiveControl() :
    rtDW(),
    rtM()
{
    // Currently there is no constructor body generated.
}

// Destructor
// Currently there is no destructor body generated.
ModelPredictiveControl::~ModelPredictiveControl() = default;

// Real-Time Model get method
ModelPredictiveControl::RT_MODEL * ModelPredictiveControl::getRTM()
{
    return (&rtM);
}

void ModelPredictiveControl::DataTypeConversion_umax(double rtu_u, double *rty_y)
{
    *rty_y = rtu_u;
}

//
// Output and update for atomic system:
//    '<S5>/DataTypeConversion_A'
//    '<S5>/DataTypeConversion_C'
//
void ModelPredictiveControl::DataTypeConversion_A(const double rtu_u[4], double
    rty_y[4])
{
    rty_y[0] = rtu_u[0];
    rty_y[1] = rtu_u[1];
    rty_y[2] = rtu_u[2];
    rty_y[3] = rtu_u[3];
}

// Function for MATLAB Function: '<S5>/Adaptive Model'
void ModelPredictiveControl::PadeApproximantOfDegree(const double A[16], uint8_t
    m, double F[16])
{
    int32_t F_tmp;
    int32_t b_ix;
    int32_t ipiv_tmp;
    int32_t iy;
    int32_t jj;
    int8_t ipiv[4];
    for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
        for (int32_t e_k{0}; e_k < 4; e_k++) {
            jj = ipiv_tmp << 2;
            rtDW.A2[e_k + jj] = ((A[jj + 1] * A[e_k + 4] + A[jj] * A[e_k]) +
                                 A[jj + 2] * A[e_k + 8]) + A[jj + 3] * A[e_k +
                12];
        }
    }

    if (m == 3) {
        memcpy(&F[0], &rtDW.A2[0], sizeof(double) << 4U);
        F[0] += 60.0;
        F[5] += 60.0;
        F[10] += 60.0;
        F[15] += 60.0;
        for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
            double A2;
            F_tmp = ipiv_tmp << 2;
            rtDW.d = F[F_tmp + 1];
            rtDW.s_l = F[F_tmp];
            rtDW.A2_d = F[F_tmp + 2];
            A2 = F[F_tmp + 3];
            for (int32_t e_k{0}; e_k < 4; e_k++) {
                rtDW.A4_l[e_k + F_tmp] = ((A[e_k + 4] * rtDW.d + rtDW.s_l *
                    A[e_k]) + A[e_k + 8] * rtDW.A2_d) + A[e_k + 12] * A2;
            }
        }

        for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
            F[ipiv_tmp] = rtDW.A4_l[ipiv_tmp];
            rtDW.V[ipiv_tmp] = 12.0 * rtDW.A2[ipiv_tmp];
        }

        rtDW.d = 120.0;
    } else {
        for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
            for (int32_t e_k{0}; e_k < 4; e_k++) {
                jj = ipiv_tmp << 2;
                rtDW.A3[e_k + jj] = ((rtDW.A2[jj + 1] * rtDW.A2[e_k + 4] +
                                      rtDW.A2[jj] * rtDW.A2[e_k]) + rtDW.A2[jj +
                                     2] * rtDW.A2[e_k + 8]) + rtDW.A2[jj + 3] *
                    rtDW.A2[e_k + 12];
            }
        }

        if (m == 5) {
            for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                F[ipiv_tmp] = 420.0 * rtDW.A2[ipiv_tmp] + rtDW.A3[ipiv_tmp];
            }

            F[0] += 15120.0;
            F[5] += 15120.0;
            F[10] += 15120.0;
            F[15] += 15120.0;
            for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                double A2;
                F_tmp = ipiv_tmp << 2;
                rtDW.d = F[F_tmp + 1];
                rtDW.s_l = F[F_tmp];
                rtDW.A2_d = F[F_tmp + 2];
                A2 = F[F_tmp + 3];
                for (int32_t e_k{0}; e_k < 4; e_k++) {
                    rtDW.A4_l[e_k + F_tmp] = ((A[e_k + 4] * rtDW.d + rtDW.s_l *
                        A[e_k]) + A[e_k + 8] * rtDW.A2_d) + A[e_k + 12] * A2;
                }
            }

            for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                F[ipiv_tmp] = rtDW.A4_l[ipiv_tmp];
                rtDW.V[ipiv_tmp] = 30.0 * rtDW.A3[ipiv_tmp] + 3360.0 *
                    rtDW.A2[ipiv_tmp];
            }

            rtDW.d = 30240.0;
        } else {
            double A2;
            for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                jj = ipiv_tmp << 2;
                rtDW.d = rtDW.A2[jj + 1];
                rtDW.s_l = rtDW.A2[jj];
                rtDW.A2_d = rtDW.A2[jj + 2];
                A2 = rtDW.A2[jj + 3];
                for (int32_t e_k{0}; e_k < 4; e_k++) {
                    rtDW.A4[e_k + jj] = ((rtDW.A3[e_k + 4] * rtDW.d + rtDW.s_l *
                                          rtDW.A3[e_k]) + rtDW.A3[e_k + 8] *
                                         rtDW.A2_d) + rtDW.A3[e_k + 12] * A2;
                }
            }

            switch (m) {
              case 7:
                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    F[ipiv_tmp] = (1512.0 * rtDW.A3[ipiv_tmp] + rtDW.A4[ipiv_tmp])
                        + 277200.0 * rtDW.A2[ipiv_tmp];
                }

                F[0] += 8.64864E+6;
                F[5] += 8.64864E+6;
                F[10] += 8.64864E+6;
                F[15] += 8.64864E+6;
                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    F_tmp = ipiv_tmp << 2;
                    rtDW.d = F[F_tmp + 1];
                    rtDW.s_l = F[F_tmp];
                    rtDW.A2_d = F[F_tmp + 2];
                    A2 = F[F_tmp + 3];
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        rtDW.A4_l[e_k + F_tmp] = ((A[e_k + 4] * rtDW.d +
                            rtDW.s_l * A[e_k]) + A[e_k + 8] * rtDW.A2_d) + A[e_k
                            + 12] * A2;
                    }
                }

                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    F[ipiv_tmp] = rtDW.A4_l[ipiv_tmp];
                    rtDW.V[ipiv_tmp] = (56.0 * rtDW.A4[ipiv_tmp] + 25200.0 *
                                        rtDW.A3[ipiv_tmp]) + 1.99584E+6 *
                        rtDW.A2[ipiv_tmp];
                }

                rtDW.d = 1.729728E+7;
                break;

              case 9:
                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    jj = ipiv_tmp << 2;
                    rtDW.d = rtDW.A2[jj + 1];
                    rtDW.s_l = rtDW.A2[jj];
                    rtDW.A2_d = rtDW.A2[jj + 2];
                    A2 = rtDW.A2[jj + 3];
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        rtDW.V[e_k + jj] = ((rtDW.A4[e_k + 4] * rtDW.d +
                                             rtDW.s_l * rtDW.A4[e_k]) +
                                            rtDW.A4[e_k + 8] * rtDW.A2_d) +
                            rtDW.A4[e_k + 12] * A2;
                    }
                }

                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    F[ipiv_tmp] = ((3960.0 * rtDW.A4[ipiv_tmp] + rtDW.V[ipiv_tmp])
                                   + 2.16216E+6 * rtDW.A3[ipiv_tmp]) +
                        3.027024E+8 * rtDW.A2[ipiv_tmp];
                }

                F[0] += 8.8216128E+9;
                F[5] += 8.8216128E+9;
                F[10] += 8.8216128E+9;
                F[15] += 8.8216128E+9;
                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    F_tmp = ipiv_tmp << 2;
                    rtDW.d = F[F_tmp + 1];
                    rtDW.s_l = F[F_tmp];
                    rtDW.A2_d = F[F_tmp + 2];
                    A2 = F[F_tmp + 3];
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        rtDW.A4_l[e_k + F_tmp] = ((A[e_k + 4] * rtDW.d +
                            rtDW.s_l * A[e_k]) + A[e_k + 8] * rtDW.A2_d) + A[e_k
                            + 12] * A2;
                    }
                }

                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    F[ipiv_tmp] = rtDW.A4_l[ipiv_tmp];
                    rtDW.V[ipiv_tmp] = ((90.0 * rtDW.V[ipiv_tmp] + 110880.0 *
                                         rtDW.A4[ipiv_tmp]) + 3.027024E+7 *
                                        rtDW.A3[ipiv_tmp]) + 2.0756736E+9 *
                        rtDW.A2[ipiv_tmp];
                }

                rtDW.d = 1.76432256E+10;
                break;

              default:
                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    rtDW.s_l = rtDW.A4[ipiv_tmp];
                    rtDW.A2_d = rtDW.A3[ipiv_tmp];
                    rtDW.d = rtDW.A2[ipiv_tmp];
                    F[ipiv_tmp] = (3.352212864E+10 * rtDW.s_l +
                                   1.05594705216E+13 * rtDW.A2_d) +
                        1.1873537964288E+15 * rtDW.d;
                    rtDW.V[ipiv_tmp] = (16380.0 * rtDW.A2_d + rtDW.s_l) +
                        4.08408E+7 * rtDW.d;
                }

                F[0] += 3.238237626624E+16;
                F[5] += 3.238237626624E+16;
                F[10] += 3.238237626624E+16;
                F[15] += 3.238237626624E+16;
                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    rtDW.s_l = rtDW.A4[ipiv_tmp + 4];
                    rtDW.d = rtDW.A4[ipiv_tmp];
                    rtDW.A2_d = rtDW.A4[ipiv_tmp + 8];
                    A2 = rtDW.A4[ipiv_tmp + 12];
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        jj = e_k << 2;
                        iy = jj + ipiv_tmp;
                        rtDW.A4_l[iy] = (((rtDW.V[jj + 1] * rtDW.s_l + rtDW.V[jj]
                                           * rtDW.d) + rtDW.V[jj + 2] *
                                          rtDW.A2_d) + rtDW.V[jj + 3] * A2) +
                            F[iy];
                    }
                }

                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    jj = ipiv_tmp << 2;
                    rtDW.s_l = rtDW.A4_l[jj + 1];
                    rtDW.d = rtDW.A4_l[jj];
                    rtDW.A2_d = rtDW.A4_l[jj + 2];
                    A2 = rtDW.A4_l[jj + 3];
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        F[e_k + jj] = ((A[e_k + 4] * rtDW.s_l + rtDW.d * A[e_k])
                                       + A[e_k + 8] * rtDW.A2_d) + A[e_k + 12] *
                            A2;
                    }
                }

                for (ipiv_tmp = 0; ipiv_tmp < 16; ipiv_tmp++) {
                    rtDW.A4_l[ipiv_tmp] = (182.0 * rtDW.A4[ipiv_tmp] + 960960.0 *
                                           rtDW.A3[ipiv_tmp]) + 1.32324192E+9 *
                        rtDW.A2[ipiv_tmp];
                }

                for (ipiv_tmp = 0; ipiv_tmp < 4; ipiv_tmp++) {
                    for (int32_t e_k{0}; e_k < 4; e_k++) {
                        jj = e_k << 2;
                        iy = jj + ipiv_tmp;
                        rtDW.V[iy] = (((((rtDW.A4_l[jj + 1] * rtDW.A4[ipiv_tmp +
                                          4] + rtDW.A4_l[jj] * rtDW.A4[ipiv_tmp])
                                         + rtDW.A4_l[jj + 2] * rtDW.A4[ipiv_tmp
                                         + 8]) + rtDW.A4_l[jj + 3] *
                                        rtDW.A4[ipiv_tmp + 12]) + rtDW.A4[iy] *
                                       6.704425728E+11) + rtDW.A3[iy] *
                                      1.29060195264E+14) + rtDW.A2[iy] *
                            7.7717703038976E+15;
                    }
                }

                rtDW.d = 6.476475253248E+16;
                break;
            }
        }
    }

    rtDW.V[0] += rtDW.d;
    rtDW.V[5] += rtDW.d;
    rtDW.V[10] += rtDW.d;
    rtDW.V[15] += rtDW.d;
    for (int32_t e_k{0}; e_k < 16; e_k++) {
        rtDW.d = F[e_k];
        rtDW.V[e_k] -= rtDW.d;
        F[e_k] = 2.0 * rtDW.d;
    }

    ipiv[0] = 1;
    ipiv[1] = 2;
    ipiv[2] = 3;
    ipiv[3] = 4;
    for (int32_t e_k{0}; e_k < 3; e_k++) {
        jj = e_k * 5;
        iy = 4 - e_k;
        b_ix = 0;
        rtDW.d = std::abs(rtDW.V[jj]);
        for (int32_t ix{2}; ix <= iy; ix++) {
            rtDW.s_l = std::abs(rtDW.V[(jj + ix) - 1]);
            if (rtDW.s_l > rtDW.d) {
                b_ix = ix - 1;
                rtDW.d = rtDW.s_l;
            }
        }

        if (rtDW.V[jj + b_ix] != 0.0) {
            if (b_ix != 0) {
                ipiv_tmp = e_k + b_ix;
                ipiv[e_k] = static_cast<int8_t>(ipiv_tmp + 1);
                rtDW.d = rtDW.V[e_k];
                rtDW.V[e_k] = rtDW.V[ipiv_tmp];
                rtDW.V[ipiv_tmp] = rtDW.d;
                rtDW.d = rtDW.V[e_k + 4];
                rtDW.V[e_k + 4] = rtDW.V[ipiv_tmp + 4];
                rtDW.V[ipiv_tmp + 4] = rtDW.d;
                rtDW.d = rtDW.V[e_k + 8];
                rtDW.V[e_k + 8] = rtDW.V[ipiv_tmp + 8];
                rtDW.V[ipiv_tmp + 8] = rtDW.d;
                rtDW.d = rtDW.V[e_k + 12];
                rtDW.V[e_k + 12] = rtDW.V[ipiv_tmp + 12];
                rtDW.V[ipiv_tmp + 12] = rtDW.d;
            }

            iy = (jj - e_k) + 4;
            for (b_ix = jj + 2; b_ix <= iy; b_ix++) {
                rtDW.V[b_ix - 1] /= rtDW.V[jj];
            }
        }

        iy = 2 - e_k;
        b_ix = jj + 6;
        for (int32_t ix{0}; ix <= iy; ix++) {
            rtDW.d = rtDW.V[((ix << 2) + jj) + 4];
            if (rtDW.d != 0.0) {
                ipiv_tmp = (b_ix - e_k) + 2;
                for (F_tmp = b_ix; F_tmp <= ipiv_tmp; F_tmp++) {
                    rtDW.V[F_tmp - 1] += rtDW.V[((jj + F_tmp) - b_ix) + 1] *
                        -rtDW.d;
                }
            }

            b_ix += 4;
        }
    }

    for (int32_t e_k{0}; e_k < 3; e_k++) {
        int8_t ipiv_0;
        ipiv_0 = ipiv[e_k];
        if (e_k + 1 != ipiv_0) {
            rtDW.d = F[e_k];
            F[e_k] = F[ipiv_0 - 1];
            F[ipiv_0 - 1] = rtDW.d;
            rtDW.d = F[e_k + 4];
            F[e_k + 4] = F[ipiv_0 + 3];
            F[ipiv_0 + 3] = rtDW.d;
            rtDW.d = F[e_k + 8];
            F[e_k + 8] = F[ipiv_0 + 7];
            F[ipiv_0 + 7] = rtDW.d;
            rtDW.d = F[e_k + 12];
            F[e_k + 12] = F[ipiv_0 + 11];
            F[ipiv_0 + 11] = rtDW.d;
        }
    }

    for (int32_t e_k{0}; e_k < 4; e_k++) {
        jj = e_k << 2;
        for (iy = 0; iy < 4; iy++) {
            b_ix = iy << 2;
            ipiv_tmp = iy + jj;
            if (F[ipiv_tmp] != 0.0) {
                for (int32_t ix{iy + 2}; ix < 5; ix++) {
                    F_tmp = (ix + jj) - 1;
                    F[F_tmp] -= rtDW.V[(ix + b_ix) - 1] * F[ipiv_tmp];
                }
            }
        }
    }

    for (int32_t e_k{0}; e_k < 4; e_k++) {
        jj = e_k << 2;
        for (iy = 3; iy >= 0; iy--) {
            b_ix = iy << 2;
            ipiv_tmp = iy + jj;
            rtDW.d = F[ipiv_tmp];
            if (rtDW.d != 0.0) {
                F[ipiv_tmp] = rtDW.d / rtDW.V[iy + b_ix];
                for (int32_t ix{0}; ix < iy; ix++) {
                    F_tmp = ix + jj;
                    F[F_tmp] -= rtDW.V[ix + b_ix] * F[ipiv_tmp];
                }
            }
        }
    }

    F[0]++;
    F[5]++;
    F[10]++;
    F[15]++;
}

// Function for MATLAB Function: '<S5>/Adaptive Model'
void ModelPredictiveControl::expmNoLog2(double A[16], double F[16])
{
    static const double c[5]{ 0.01495585217958292, 0.253939833006323,
        0.95041789961629319, 2.097847961257068, 5.3719203511481517 };

    static const uint8_t d[5]{ 3U, 5U, 7U, 9U, 13U };

    int32_t F_tmp;
    int32_t b_i;
    int32_t b_j;
    int32_t b_s_tmp;
    int32_t i;
    bool exitg1;
    rtDW.normA = 0.0;
    b_j = 0;
    exitg1 = false;
    while ((!exitg1) && (b_j < 4)) {
        b_s_tmp = b_j << 2;
        rtDW.b_s = ((std::abs(A[b_s_tmp + 1]) + std::abs(A[b_s_tmp])) + std::abs
                    (A[b_s_tmp + 2])) + std::abs(A[b_s_tmp + 3]);
        if (std::isnan(rtDW.b_s)) {
            rtDW.normA = (rtNaN);
            exitg1 = true;
        } else {
            if (rtDW.b_s > rtDW.normA) {
                rtDW.normA = rtDW.b_s;
            }

            b_j++;
        }
    }

    if (rtDW.normA <= 5.3719203511481517) {
        b_j = 0;
        exitg1 = false;
        while ((!exitg1) && (b_j < 5)) {
            if (rtDW.normA <= c[b_j]) {
                PadeApproximantOfDegree(A, d[b_j], F);
                exitg1 = true;
            } else {
                b_j++;
            }
        }
    } else {
        if (std::isinf(rtDW.normA) || std::isnan(rtDW.normA)) {
            rtDW.normA = 0.0;
        } else {
            rtDW.b_s = rtDW.normA / 5.3719203511481517;
            rtDW.normA = std::floor(std::log(rtDW.b_s) / 0.69314718055994529);
            rtDW.b_s *= rt_powd_snf(2.0, -rtDW.normA);
            if (rtDW.b_s < 0.5) {
                rtDW.normA--;
                rtDW.b_s *= 2.0;
            } else if (rtDW.b_s >= 1.0) {
                rtDW.normA++;
                rtDW.b_s *= 0.5;
            }

            if (rtDW.b_s == 0.5) {
                rtDW.normA--;
            }
        }

        rtDW.b_s = rt_powd_snf(2.0, rtDW.normA);
        for (b_s_tmp = 0; b_s_tmp < 16; b_s_tmp++) {
            A[b_s_tmp] /= rtDW.b_s;
        }

        PadeApproximantOfDegree(A, 13, F);
        b_j = static_cast<int32_t>(rtDW.normA);
        for (b_i = 0; b_i < b_j; b_i++) {
            for (b_s_tmp = 0; b_s_tmp < 4; b_s_tmp++) {
                for (i = 0; i < 4; i++) {
                    F_tmp = i << 2;
                    rtDW.F[b_s_tmp + F_tmp] = ((F[F_tmp + 1] * F[b_s_tmp + 4] +
                        F[F_tmp] * F[b_s_tmp]) + F[F_tmp + 2] * F[b_s_tmp + 8])
                        + F[F_tmp + 3] * F[b_s_tmp + 12];
                }
            }

            memcpy(&F[0], &rtDW.F[0], sizeof(double) << 4U);
        }
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
int32_t ModelPredictiveControl::xpotrf(double b_A[9])
{
    int32_t info;
    int32_t j;
    bool exitg1;
    info = 0;
    j = 0;
    exitg1 = false;
    while ((!exitg1) && (j < 3)) {
        double c;
        double ssq;
        int32_t idxAjj;
        idxAjj = j * 3 + j;
        ssq = 0.0;
        if (j >= 1) {
            for (int32_t b_k{0}; b_k < j; b_k++) {
                c = b_A[b_k * 3 + j];
                ssq += c * c;
            }
        }

        ssq = b_A[idxAjj] - ssq;
        if (ssq > 0.0) {
            ssq = std::sqrt(ssq);
            b_A[idxAjj] = ssq;
            if (j + 1 < 3) {
                int32_t jm1;
                if (j != 0) {
                    int32_t b_iy;
                    b_iy = ((j - 1) * 3 + j) + 2;
                    for (int32_t b_k{j + 2}; b_k <= b_iy; b_k += 3) {
                        int32_t d;
                        jm1 = b_k - j;
                        c = -b_A[div_nde_s32_floor(jm1 - 2, 3) * 3 + j];
                        d = jm1 + 1;
                        for (jm1 = b_k; jm1 <= d; jm1++) {
                            int32_t tmp;
                            tmp = ((idxAjj + jm1) - b_k) + 1;
                            b_A[tmp] += b_A[jm1 - 1] * c;
                        }
                    }
                }

                ssq = 1.0 / ssq;
                jm1 = (idxAjj - j) + 3;
                for (int32_t b_k{idxAjj + 2}; b_k <= jm1; b_k++) {
                    b_A[b_k - 1] *= ssq;
                }
            }

            j++;
        } else {
            b_A[idxAjj] = ssq;
            info = j + 1;
            exitg1 = true;
        }
    }

    return info;
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
double ModelPredictiveControl::minimum(const double x[3])
{
    double ex;
    int32_t idx;
    int32_t k;
    if (!std::isnan(x[0])) {
        idx = 1;
    } else {
        bool exitg1;
        idx = 0;
        k = 2;
        exitg1 = false;
        while ((!exitg1) && (k < 4)) {
            if (!std::isnan(x[k - 1])) {
                idx = k;
                exitg1 = true;
            } else {
                k++;
            }
        }
    }

    if (idx == 0) {
        ex = x[0];
    } else {
        ex = x[idx - 1];
        for (k = idx + 1; k < 4; k++) {
            double x_0;
            x_0 = x[k - 1];
            if (ex > x_0) {
                ex = x_0;
            }
        }
    }

    return ex;
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::trisolve(const double b_A[9], double b_B[9])
{
    for (int32_t j{0}; j < 3; j++) {
        int32_t jBcol;
        jBcol = 3 * j;
        for (int32_t b_k{0}; b_k < 3; b_k++) {
            double b_B_0;
            int32_t b_B_tmp;
            int32_t kAcol;
            kAcol = 3 * b_k;
            b_B_tmp = b_k + jBcol;
            b_B_0 = b_B[b_B_tmp];
            if (b_B_0 != 0.0) {
                b_B[b_B_tmp] = b_B_0 / b_A[b_k + kAcol];
                for (int32_t i{b_k + 2}; i < 4; i++) {
                    int32_t tmp;
                    tmp = (i + jBcol) - 1;
                    b_B[tmp] -= b_A[(i + kAcol) - 1] * b_B[b_B_tmp];
                }
            }
        }
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
double ModelPredictiveControl::norm(const double x[3])
{
    double absxk;
    double scale;
    double t;
    double y;
    scale = 3.3121686421112381E-170;
    absxk = std::abs(x[0]);
    if (absxk > 3.3121686421112381E-170) {
        y = 1.0;
        scale = absxk;
    } else {
        t = absxk / 3.3121686421112381E-170;
        y = t * t;
    }

    absxk = std::abs(x[1]);
    if (absxk > scale) {
        t = scale / absxk;
        y = y * t * t + 1.0;
        scale = absxk;
    } else {
        t = absxk / scale;
        y += t * t;
    }

    absxk = std::abs(x[2]);
    if (absxk > scale) {
        t = scale / absxk;
        y = y * t * t + 1.0;
        scale = absxk;
    } else {
        t = absxk / scale;
        y += t * t;
    }

    return scale * std::sqrt(y);
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
double ModelPredictiveControl::maximum(const double x[3])
{
    double ex;
    int32_t idx;
    int32_t k;
    if (!std::isnan(x[0])) {
        idx = 1;
    } else {
        bool exitg1;
        idx = 0;
        k = 2;
        exitg1 = false;
        while ((!exitg1) && (k < 4)) {
            if (!std::isnan(x[k - 1])) {
                idx = k;
                exitg1 = true;
            } else {
                k++;
            }
        }
    }

    if (idx == 0) {
        ex = x[0];
    } else {
        ex = x[idx - 1];
        for (k = idx + 1; k < 4; k++) {
            double x_0;
            x_0 = x[k - 1];
            if (ex < x_0) {
                ex = x_0;
            }
        }
    }

    return ex;
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
double ModelPredictiveControl::xnrm2(int32_t n, const double x[9], int32_t ix0)
{
    double y;
    y = 0.0;
    if (n >= 1) {
        if (n == 1) {
            y = std::abs(x[ix0 - 1]);
        } else {
            int32_t kend;
            rtDW.scale = 3.3121686421112381E-170;
            kend = (ix0 + n) - 1;
            for (int32_t k{ix0}; k <= kend; k++) {
                rtDW.absxk = std::abs(x[k - 1]);
                if (rtDW.absxk > rtDW.scale) {
                    rtDW.t_g = rtDW.scale / rtDW.absxk;
                    y = y * rtDW.t_g * rtDW.t_g + 1.0;
                    rtDW.scale = rtDW.absxk;
                } else {
                    rtDW.t_g = rtDW.absxk / rtDW.scale;
                    y += rtDW.t_g * rtDW.t_g;
                }
            }

            y = rtDW.scale * std::sqrt(y);
        }
    }

    return y;
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::xgemv(int32_t b_m, int32_t n, const double b_A[9],
    int32_t ia0, const double x[9], int32_t ix0, double y[3])
{
    if ((b_m != 0) && (n != 0)) {
        int32_t b;
        if (n - 1 >= 0) {
            memset(&y[0], 0, static_cast<uint32_t>(n) * sizeof(double));
        }

        b = (n - 1) * 3 + ia0;
        for (int32_t b_iy{ia0}; b_iy <= b; b_iy += 3) {
            int32_t d;
            int32_t iyend;
            rtDW.c = 0.0;
            d = b_iy + b_m;
            for (iyend = b_iy; iyend < d; iyend++) {
                rtDW.c += x[((ix0 + iyend) - b_iy) - 1] * b_A[iyend - 1];
            }

            iyend = div_nde_s32_floor(b_iy - ia0, 3);
            y[iyend] += rtDW.c;
        }
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::xgerc(int32_t b_m, int32_t n, double alpha1,
    int32_t ix0, const double y[3], double b_A[9], int32_t ia0)
{
    if (!(alpha1 == 0.0)) {
        int32_t jA;
        jA = ia0;
        for (int32_t j{0}; j < n; j++) {
            rtDW.temp = y[j];
            if (rtDW.temp != 0.0) {
                int32_t b;
                rtDW.temp *= alpha1;
                b = b_m + jA;
                for (int32_t ijA{jA}; ijA < b; ijA++) {
                    b_A[ijA - 1] += b_A[((ix0 + ijA) - jA) - 1] * rtDW.temp;
                }
            }

            jA += 3;
        }
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::KWIKfactor(const double b_Ac[12], const int32_t iC
    [4], int32_t nA, const double b_Linv[9], double b_D[9], double b_H[9],
    int32_t n, double RLinv[9], double *Status)
{
    int32_t b_coltop;
    int32_t b_lastv;
    int32_t coltop;
    int32_t exitg1;
    int32_t ii;
    int32_t k_i;
    int32_t knt;
    bool exitg2;
    *Status = 1.0;
    memset(&RLinv[0], 0, 9U * sizeof(double));
    for (k_i = 0; k_i < nA; k_i++) {
        b_lastv = iC[k_i];
        for (b_coltop = 0; b_coltop < 3; b_coltop++) {
            RLinv[b_coltop + 3 * k_i] = (b_Ac[b_lastv - 1] * b_Linv[b_coltop] +
                b_Linv[b_coltop + 3] * b_Ac[b_lastv + 3]) + b_Linv[b_coltop + 6]
                * b_Ac[b_lastv + 7];
        }
    }

    memcpy(&rtDW.b_A_f[0], &RLinv[0], 9U * sizeof(double));
    rtDW.tau[0] = 0.0;
    rtDW.work[0] = 0.0;
    rtDW.tau[1] = 0.0;
    rtDW.work[1] = 0.0;
    rtDW.tau[2] = 0.0;
    rtDW.work[2] = 0.0;
    for (k_i = 0; k_i < 3; k_i++) {
        ii = k_i * 3 + k_i;
        if (k_i + 1 < 3) {
            rtDW.atmp = rtDW.b_A_f[ii];
            b_lastv = ii + 2;
            rtDW.tau[k_i] = 0.0;
            rtDW.beta1 = xnrm2(2 - k_i, rtDW.b_A_f, ii + 2);
            if (rtDW.beta1 != 0.0) {
                rtDW.b_A_d = rtDW.b_A_f[ii];
                rtDW.beta1 = rt_hypotd_snf(rtDW.b_A_d, rtDW.beta1);
                if (rtDW.b_A_d >= 0.0) {
                    rtDW.beta1 = -rtDW.beta1;
                }

                if (std::abs(rtDW.beta1) < 1.0020841800044864E-292) {
                    knt = 0;
                    coltop = (ii - k_i) + 3;
                    do {
                        knt++;
                        for (b_coltop = b_lastv; b_coltop <= coltop; b_coltop++)
                        {
                            rtDW.b_A_f[b_coltop - 1] *= 9.9792015476736E+291;
                        }

                        rtDW.beta1 *= 9.9792015476736E+291;
                        rtDW.atmp *= 9.9792015476736E+291;
                    } while ((std::abs(rtDW.beta1) < 1.0020841800044864E-292) &&
                             (knt < 20));

                    rtDW.beta1 = rt_hypotd_snf(rtDW.atmp, xnrm2(2 - k_i,
                        rtDW.b_A_f, ii + 2));
                    if (rtDW.atmp >= 0.0) {
                        rtDW.beta1 = -rtDW.beta1;
                    }

                    rtDW.tau[k_i] = (rtDW.beta1 - rtDW.atmp) / rtDW.beta1;
                    rtDW.atmp = 1.0 / (rtDW.atmp - rtDW.beta1);
                    for (b_coltop = b_lastv; b_coltop <= coltop; b_coltop++) {
                        rtDW.b_A_f[b_coltop - 1] *= rtDW.atmp;
                    }

                    for (b_lastv = 0; b_lastv < knt; b_lastv++) {
                        rtDW.beta1 *= 1.0020841800044864E-292;
                    }

                    rtDW.atmp = rtDW.beta1;
                } else {
                    rtDW.tau[k_i] = (rtDW.beta1 - rtDW.b_A_d) / rtDW.beta1;
                    rtDW.atmp = 1.0 / (rtDW.b_A_d - rtDW.beta1);
                    b_coltop = (ii - k_i) + 3;
                    for (knt = b_lastv; knt <= b_coltop; knt++) {
                        rtDW.b_A_f[knt - 1] *= rtDW.atmp;
                    }

                    rtDW.atmp = rtDW.beta1;
                }
            }

            rtDW.b_A_f[ii] = 1.0;
            if (rtDW.tau[k_i] != 0.0) {
                b_lastv = 3 - k_i;
                knt = (ii - k_i) + 2;
                while ((b_lastv > 0) && (rtDW.b_A_f[knt] == 0.0)) {
                    b_lastv--;
                    knt--;
                }

                knt = 2 - k_i;
                exitg2 = false;
                while ((!exitg2) && (knt > 0)) {
                    b_coltop = ((knt - 1) * 3 + ii) + 3;
                    coltop = b_coltop;
                    do {
                        exitg1 = 0;
                        if (coltop + 1 <= b_coltop + b_lastv) {
                            if (rtDW.b_A_f[coltop] != 0.0) {
                                exitg1 = 1;
                            } else {
                                coltop++;
                            }
                        } else {
                            knt--;
                            exitg1 = 2;
                        }
                    } while (exitg1 == 0);

                    if (exitg1 == 1) {
                        exitg2 = true;
                    }
                }
            } else {
                b_lastv = 0;
                knt = 0;
            }

            if (b_lastv > 0) {
                xgemv(b_lastv, knt, rtDW.b_A_f, ii + 4, rtDW.b_A_f, ii + 1,
                      rtDW.work);
                xgerc(b_lastv, knt, -rtDW.tau[k_i], ii + 1, rtDW.work,
                      rtDW.b_A_f, ii + 4);
            }

            rtDW.b_A_f[ii] = rtDW.atmp;
        } else {
            rtDW.tau[2] = 0.0;
        }
    }

    for (k_i = 0; k_i < 3; k_i++) {
        for (ii = 0; ii <= k_i; ii++) {
            rtDW.R[ii + 3 * k_i] = rtDW.b_A_f[3 * k_i + ii];
        }

        for (ii = k_i + 2; ii < 4; ii++) {
            rtDW.R[(ii + 3 * k_i) - 1] = 0.0;
        }

        rtDW.work[k_i] = 0.0;
    }

    for (k_i = 2; k_i >= 0; k_i--) {
        b_lastv = (k_i * 3 + k_i) + 4;
        if (k_i + 1 < 3) {
            rtDW.b_A_f[b_lastv - 4] = 1.0;
            if (rtDW.tau[k_i] != 0.0) {
                knt = 3 - k_i;
                b_coltop = b_lastv - k_i;
                while ((knt > 0) && (rtDW.b_A_f[b_coltop - 2] == 0.0)) {
                    knt--;
                    b_coltop--;
                }

                b_coltop = 2 - k_i;
                exitg2 = false;
                while ((!exitg2) && (b_coltop > 0)) {
                    coltop = (b_coltop - 1) * 3 + b_lastv;
                    ii = coltop;
                    do {
                        exitg1 = 0;
                        if (ii <= (coltop + knt) - 1) {
                            if (rtDW.b_A_f[ii - 1] != 0.0) {
                                exitg1 = 1;
                            } else {
                                ii++;
                            }
                        } else {
                            b_coltop--;
                            exitg1 = 2;
                        }
                    } while (exitg1 == 0);

                    if (exitg1 == 1) {
                        exitg2 = true;
                    }
                }
            } else {
                knt = 0;
                b_coltop = 0;
            }

            if (knt > 0) {
                xgemv(knt, b_coltop, rtDW.b_A_f, b_lastv, rtDW.b_A_f, b_lastv -
                      3, rtDW.work);
                xgerc(knt, b_coltop, -rtDW.tau[k_i], b_lastv - 3, rtDW.work,
                      rtDW.b_A_f, b_lastv);
            }

            b_coltop = (b_lastv - k_i) - 1;
            for (knt = b_lastv - 2; knt <= b_coltop; knt++) {
                rtDW.b_A_f[knt - 1] *= -rtDW.tau[k_i];
            }
        }

        rtDW.b_A_f[b_lastv - 4] = 1.0 - rtDW.tau[k_i];
        for (knt = 0; knt < k_i; knt++) {
            rtDW.b_A_f[(b_lastv - knt) - 5] = 0.0;
        }
    }

    for (k_i = 0; k_i < 3; k_i++) {
        rtDW.Q[3 * k_i] = rtDW.b_A_f[3 * k_i];
        b_lastv = 3 * k_i + 1;
        rtDW.Q[b_lastv] = rtDW.b_A_f[b_lastv];
        b_lastv = 3 * k_i + 2;
        rtDW.Q[b_lastv] = rtDW.b_A_f[b_lastv];
    }

    k_i = 0;
    do {
        exitg1 = 0;
        if (k_i <= nA - 1) {
            if (std::abs(rtDW.R[3 * k_i + k_i]) < 1.0E-12) {
                *Status = -2.0;
                exitg1 = 1;
            } else {
                k_i++;
            }
        } else {
            for (k_i = 0; k_i < n; k_i++) {
                for (ii = 0; ii < n; ii++) {
                    rtDW.TL[k_i + 3 * ii] = (b_Linv[3 * k_i + 1] * rtDW.Q[3 * ii
                        + 1] + b_Linv[3 * k_i] * rtDW.Q[3 * ii]) + b_Linv[3 *
                        k_i + 2] * rtDW.Q[3 * ii + 2];
                }
            }

            memset(&RLinv[0], 0, 9U * sizeof(double));
            for (k_i = nA; k_i >= 1; k_i--) {
                b_coltop = (k_i - 1) * 3;
                knt = (k_i + b_coltop) - 1;
                RLinv[knt] = 1.0;
                for (ii = k_i; ii <= nA; ii++) {
                    coltop = ((ii - 1) * 3 + k_i) - 1;
                    RLinv[coltop] /= rtDW.R[knt];
                }

                if (k_i > 1) {
                    for (ii = 0; ii <= k_i - 2; ii++) {
                        for (b_lastv = k_i; b_lastv <= nA; b_lastv++) {
                            knt = (b_lastv - 1) * 3;
                            coltop = knt + ii;
                            RLinv[coltop] -= RLinv[(knt + k_i) - 1] *
                                rtDW.R[b_coltop + ii];
                        }
                    }
                }
            }

            for (k_i = 0; k_i < n; k_i++) {
                for (ii = k_i + 1; ii <= n; ii++) {
                    b_coltop = (ii - 1) * 3 + k_i;
                    b_H[b_coltop] = 0.0;
                    for (b_lastv = nA + 1; b_lastv <= n; b_lastv++) {
                        knt = (b_lastv - 1) * 3;
                        b_H[b_coltop] -= rtDW.TL[(knt + ii) - 1] * rtDW.TL[knt +
                            k_i];
                    }

                    b_H[(ii + 3 * k_i) - 1] = b_H[b_coltop];
                }
            }

            for (k_i = 0; k_i < nA; k_i++) {
                for (ii = 0; ii < n; ii++) {
                    b_coltop = 3 * k_i + ii;
                    b_D[b_coltop] = 0.0;
                    for (b_lastv = k_i + 1; b_lastv <= nA; b_lastv++) {
                        knt = (b_lastv - 1) * 3;
                        b_D[b_coltop] += rtDW.TL[knt + ii] * RLinv[knt + k_i];
                    }
                }
            }

            exitg1 = 1;
        }
    } while (exitg1 == 0);
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::DropConstraint(int32_t kDrop, bool iA[4], int32_t
    *nA, int32_t iC[4])
{
    if (kDrop > 0) {
        iA[iC[kDrop - 1] - 1] = false;
        if (kDrop < *nA) {
            for (int32_t i{kDrop}; i < *nA; i++) {
                iC[i - 1] = iC[i];
            }
        }

        iC[*nA - 1] = 0;
        (*nA)--;
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::qpkwik(const double b_Linv[9], const double b_Hinv
    [9], const double f[3], const double b_Ac[12], const double b[4], bool iA[4],
    int32_t maxiter, double FeasTol, double x[3], double lambda[4], int32_t
    *status)
{
    int32_t b_exponent;
    int32_t exitg1;
    int32_t exitg3;
    int32_t iC;
    bool ColdReset;
    bool DualFeasible;
    bool cTolComputed;
    bool exitg2;
    bool exitg4;
    bool guard1;
    bool guard2;
    x[0] = 0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    lambda[0] = 0.0;
    lambda[1] = 0.0;
    lambda[2] = 0.0;
    lambda[3] = 0.0;
    *status = 1;
    rtDW.r_m[0] = 0.0;
    rtDW.r_m[1] = 0.0;
    rtDW.r_m[2] = 0.0;
    rtDW.rMin = 0.0;
    cTolComputed = false;
    rtDW.cTol[0] = 1.0;
    rtDW.iC[0] = 0;
    rtDW.cTol[1] = 1.0;
    rtDW.iC[1] = 0;
    rtDW.cTol[2] = 1.0;
    rtDW.iC[2] = 0;
    rtDW.cTol[3] = 1.0;
    rtDW.iC[3] = 0;
    rtDW.nA = 0;
    if (iA[0]) {
        rtDW.nA = 1;
        rtDW.iC[0] = 1;
    }

    if (iA[1]) {
        rtDW.nA++;
        rtDW.iC[rtDW.nA - 1] = 2;
    }

    if (iA[2]) {
        rtDW.nA++;
        rtDW.iC[rtDW.nA - 1] = 3;
    }

    if (iA[3]) {
        rtDW.nA++;
        rtDW.iC[rtDW.nA - 1] = 4;
    }

    guard1 = false;
    if (rtDW.nA > 0) {
        for (rtDW.kNext = 0; rtDW.kNext < 6; rtDW.kNext++) {
            rtDW.Opt[rtDW.kNext] = 0.0;
        }

        rtDW.Rhs[0] = f[0];
        rtDW.Rhs[3] = 0.0;
        rtDW.Rhs[1] = f[1];
        rtDW.Rhs[4] = 0.0;
        rtDW.Rhs[2] = f[2];
        rtDW.Rhs[5] = 0.0;
        DualFeasible = false;
        ColdReset = false;
        do {
            exitg3 = 0;
            if ((!DualFeasible) && (rtDW.nA > 0) && (*status <= maxiter)) {
                KWIKfactor(b_Ac, rtDW.iC, rtDW.nA, b_Linv, rtDW.b_D, rtDW.b_H,
                           degrees, rtDW.RLinv, &rtDW.Xnorm0);
                if (rtDW.Xnorm0 < 0.0) {
                    if (ColdReset) {
                        *status = -2;
                        exitg3 = 2;
                    } else {
                        rtDW.nA = 0;
                        iA[0] = false;
                        rtDW.iC[0] = 0;
                        iA[1] = false;
                        rtDW.iC[1] = 0;
                        iA[2] = false;
                        rtDW.iC[2] = 0;
                        iA[3] = false;
                        rtDW.iC[3] = 0;
                        ColdReset = true;
                    }
                } else {
                    for (rtDW.kNext = 0; rtDW.kNext < rtDW.nA; rtDW.kNext++) {
                        rtDW.Rhs[rtDW.kNext + 3] = b[rtDW.iC[rtDW.kNext] - 1];
                        for (rtDW.kDrop = rtDW.kNext + 1; rtDW.kDrop <= rtDW.nA;
                             rtDW.kDrop++) {
                            iC = (3 * rtDW.kNext + rtDW.kDrop) - 1;
                            rtDW.U[iC] = 0.0;
                            for (rtDW.iSave = 0; rtDW.iSave < rtDW.nA;
                                    rtDW.iSave++) {
                                rtDW.U[iC] += rtDW.RLinv[(3 * rtDW.iSave +
                                    rtDW.kDrop) - 1] * rtDW.RLinv[3 * rtDW.iSave
                                    + rtDW.kNext];
                            }

                            rtDW.U[rtDW.kNext + 3 * (rtDW.kDrop - 1)] =
                                rtDW.U[iC];
                        }
                    }

                    for (rtDW.kNext = 0; rtDW.kNext < 3; rtDW.kNext++) {
                        rtDW.Opt[rtDW.kNext] = (rtDW.b_H[rtDW.kNext + 3] *
                                                rtDW.Rhs[1] +
                                                rtDW.b_H[rtDW.kNext] * rtDW.Rhs
                                                [0]) + rtDW.b_H[rtDW.kNext + 6] *
                            rtDW.Rhs[2];
                        for (rtDW.kDrop = 0; rtDW.kDrop < rtDW.nA; rtDW.kDrop++)
                        {
                            rtDW.Opt[rtDW.kNext] += rtDW.b_D[3 * rtDW.kDrop +
                                rtDW.kNext] * rtDW.Rhs[rtDW.kDrop + 3];
                        }
                    }

                    for (rtDW.kNext = 0; rtDW.kNext < rtDW.nA; rtDW.kNext++) {
                        rtDW.Opt[rtDW.kNext + 3] = (rtDW.b_D[3 * rtDW.kNext + 1]
                            * rtDW.Rhs[1] + rtDW.b_D[3 * rtDW.kNext] * rtDW.Rhs
                            [0]) + rtDW.b_D[3 * rtDW.kNext + 2] * rtDW.Rhs[2];
                        for (rtDW.kDrop = 0; rtDW.kDrop < rtDW.nA; rtDW.kDrop++)
                        {
                            rtDW.Opt[rtDW.kNext + 3] += rtDW.U[3 * rtDW.kDrop +
                                rtDW.kNext] * rtDW.Rhs[rtDW.kDrop + 3];
                        }
                    }

                    rtDW.Xnorm0 = -1.0E-12;
                    rtDW.kDrop = -1;
                    for (rtDW.kNext = 0; rtDW.kNext < rtDW.nA; rtDW.kNext++) {
                        rtDW.cMin = rtDW.Opt[rtDW.kNext + 3];
                        lambda[rtDW.iC[rtDW.kNext] - 1] = rtDW.cMin;
                        if ((rtDW.cMin < rtDW.Xnorm0) && (rtDW.kNext + 1 <=
                                rtDW.nA)) {
                            rtDW.kDrop = rtDW.kNext;
                            rtDW.Xnorm0 = rtDW.cMin;
                        }
                    }

                    if (rtDW.kDrop + 1 <= 0) {
                        DualFeasible = true;
                        x[0] = rtDW.Opt[0];
                        x[1] = rtDW.Opt[1];
                        x[2] = rtDW.Opt[2];
                    } else {
                        (*status)++;
                        if (*status > 5) {
                            rtDW.nA = 0;
                            iA[0] = false;
                            rtDW.iC[0] = 0;
                            iA[1] = false;
                            rtDW.iC[1] = 0;
                            iA[2] = false;
                            rtDW.iC[2] = 0;
                            iA[3] = false;
                            rtDW.iC[3] = 0;
                            ColdReset = true;
                        } else {
                            lambda[rtDW.iC[rtDW.kDrop] - 1] = 0.0;
                            DropConstraint(rtDW.kDrop + 1, iA, &rtDW.nA, rtDW.iC);
                        }
                    }
                }
            } else {
                if (rtDW.nA <= 0) {
                    lambda[0] = 0.0;
                    lambda[1] = 0.0;
                    lambda[2] = 0.0;
                    lambda[3] = 0.0;
                    rtDW.Xnorm0 = f[1];
                    rtDW.cMin = f[0];
                    rtDW.cVal = f[2];
                    for (rtDW.kNext = 0; rtDW.kNext < 3; rtDW.kNext++) {
                        x[rtDW.kNext] = (-b_Hinv[rtDW.kNext + 3] * rtDW.Xnorm0 +
                                         -b_Hinv[rtDW.kNext] * rtDW.cMin) +
                            -b_Hinv[rtDW.kNext + 6] * rtDW.cVal;
                    }
                }

                exitg3 = 1;
            }
        } while (exitg3 == 0);

        if (exitg3 == 1) {
            guard1 = true;
        }
    } else {
        rtDW.Xnorm0 = f[1];
        rtDW.cMin = f[0];
        rtDW.cVal = f[2];
        for (rtDW.kNext = 0; rtDW.kNext < 3; rtDW.kNext++) {
            x[rtDW.kNext] = (-b_Hinv[rtDW.kNext + 3] * rtDW.Xnorm0 +
                             -b_Hinv[rtDW.kNext] * rtDW.cMin) +
                -b_Hinv[rtDW.kNext + 6] * rtDW.cVal;
        }

        guard1 = true;
    }

    if (guard1) {
        rtDW.Xnorm0 = norm(x);
        exitg2 = false;
        while ((!exitg2) && (*status <= maxiter)) {
            rtDW.cMin = -FeasTol;
            rtDW.kNext = -1;
            for (rtDW.kDrop = 0; rtDW.kDrop < 4; rtDW.kDrop++) {
                if (!cTolComputed) {
                    rtDW.z[0] = std::abs(b_Ac[rtDW.kDrop] * x[0]);
                    rtDW.z[1] = std::abs(b_Ac[rtDW.kDrop + 4] * x[1]);
                    rtDW.z[2] = std::abs(b_Ac[rtDW.kDrop + 8] * x[2]);
                    rtDW.cTol[rtDW.kDrop] = std::fmax(rtDW.cTol[rtDW.kDrop],
                        maximum(rtDW.z));
                }

                if (!iA[rtDW.kDrop]) {
                    rtDW.cVal = (((b_Ac[rtDW.kDrop + 4] * x[1] + b_Ac[rtDW.kDrop]
                                   * x[0]) + b_Ac[rtDW.kDrop + 8] * x[2]) -
                                 b[rtDW.kDrop]) / rtDW.cTol[rtDW.kDrop];
                    if (rtDW.cVal < rtDW.cMin) {
                        rtDW.cMin = rtDW.cVal;
                        rtDW.kNext = rtDW.kDrop;
                    }
                }
            }

            cTolComputed = true;
            if (rtDW.kNext + 1 <= 0) {
                exitg2 = true;
            } else if (*status == maxiter) {
                *status = 0;
                exitg2 = true;
            } else {
                do {
                    exitg1 = 0;
                    if ((rtDW.kNext + 1 > 0) && (*status <= maxiter)) {
                        guard2 = false;
                        if (rtDW.nA == 0) {
                            for (iC = 0; iC < 3; iC++) {
                                rtDW.z[iC] = (b_Hinv[iC + 3] * b_Ac[rtDW.kNext +
                                              4] + b_Hinv[iC] * b_Ac[rtDW.kNext])
                                    + b_Hinv[iC + 6] * b_Ac[rtDW.kNext + 8];
                            }

                            guard2 = true;
                        } else {
                            KWIKfactor(b_Ac, rtDW.iC, rtDW.nA, b_Linv, rtDW.b_D,
                                       rtDW.b_H, degrees, rtDW.RLinv, &rtDW.cMin);
                            if (rtDW.cMin <= 0.0) {
                                *status = -2;
                                exitg1 = 1;
                            } else {
                                for (iC = 0; iC < 9; iC++) {
                                    rtDW.U[iC] = -rtDW.b_H[iC];
                                }

                                for (iC = 0; iC < 3; iC++) {
                                    rtDW.z[iC] = (rtDW.U[iC + 3] *
                                                  b_Ac[rtDW.kNext + 4] +
                                                  rtDW.U[iC] * b_Ac[rtDW.kNext])
                                        + rtDW.U[iC + 6] * b_Ac[rtDW.kNext + 8];
                                }

                                for (rtDW.kDrop = 0; rtDW.kDrop < rtDW.nA;
                                        rtDW.kDrop++) {
                                    rtDW.r_m[rtDW.kDrop] = (rtDW.b_D[3 *
                                                            rtDW.kDrop + 1] *
                                                            b_Ac[rtDW.kNext + 4]
                                                            + rtDW.b_D[3 *
                                                            rtDW.kDrop] *
                                                            b_Ac[rtDW.kNext]) +
                                        rtDW.b_D[3 * rtDW.kDrop + 2] *
                                        b_Ac[rtDW.kNext + 8];
                                }

                                guard2 = true;
                            }
                        }

                        if (guard2) {
                            rtDW.kDrop = 0;
                            rtDW.cMin = 0.0;
                            DualFeasible = true;
                            ColdReset = true;
                            if (rtDW.nA > 0) {
                                rtDW.iSave = 0;
                                exitg4 = false;
                                while ((!exitg4) && (rtDW.iSave <= rtDW.nA - 1))
                                {
                                    if (rtDW.r_m[rtDW.iSave] >= 1.0E-12) {
                                        ColdReset = false;
                                        exitg4 = true;
                                    } else {
                                        rtDW.iSave++;
                                    }
                                }
                            }

                            if ((rtDW.nA != 0) && (!ColdReset)) {
                                for (rtDW.iSave = 0; rtDW.iSave < rtDW.nA;
                                        rtDW.iSave++) {
                                    rtDW.cVal = rtDW.r_m[rtDW.iSave];
                                    if (rtDW.cVal > 1.0E-12) {
                                        rtDW.cVal = lambda[rtDW.iC[rtDW.iSave] -
                                            1] / rtDW.cVal;
                                        if ((rtDW.kDrop == 0) || (rtDW.cVal <
                                                rtDW.rMin)) {
                                            rtDW.rMin = rtDW.cVal;
                                            rtDW.kDrop = rtDW.iSave + 1;
                                        }
                                    }
                                }

                                if (rtDW.kDrop > 0) {
                                    rtDW.cMin = rtDW.rMin;
                                    DualFeasible = false;
                                }
                            }

                            rtDW.t = b_Ac[rtDW.kNext + 4];
                            rtDW.cVal_tmp = b_Ac[rtDW.kNext + 8];
                            rtDW.cVal = (rtDW.t * rtDW.z[1] + rtDW.z[0] *
                                         b_Ac[rtDW.kNext]) + rtDW.cVal_tmp *
                                rtDW.z[2];
                            if (rtDW.cVal <= 0.0) {
                                rtDW.cVal = 0.0;
                                ColdReset = true;
                            } else {
                                rtDW.cVal = (b[rtDW.kNext] - ((rtDW.t * x[1] +
                                               b_Ac[rtDW.kNext] * x[0]) +
                                              rtDW.cVal_tmp * x[2])) / rtDW.cVal;
                                ColdReset = false;
                            }

                            if (DualFeasible && ColdReset) {
                                *status = -1;
                                exitg1 = 1;
                            } else {
                                if (ColdReset) {
                                    rtDW.t = rtDW.cMin;
                                } else if (DualFeasible) {
                                    rtDW.t = rtDW.cVal;
                                } else if (rtDW.cMin < rtDW.cVal) {
                                    rtDW.t = rtDW.cMin;
                                } else {
                                    rtDW.t = rtDW.cVal;
                                }

                                for (rtDW.iSave = 0; rtDW.iSave < rtDW.nA;
                                        rtDW.iSave++) {
                                    iC = rtDW.iC[rtDW.iSave];
                                    lambda[iC - 1] -= rtDW.t *
                                        rtDW.r_m[rtDW.iSave];
                                    if ((iC <= 4) && (lambda[iC - 1] < 0.0)) {
                                        lambda[iC - 1] = 0.0;
                                    }
                                }

                                lambda[rtDW.kNext] += rtDW.t;
                                std::frexp(1.0, &rtDW.exponent);
                                if (std::abs(rtDW.t - rtDW.cMin) <
                                        2.2204460492503131E-16) {
                                    DropConstraint(rtDW.kDrop, iA, &rtDW.nA,
                                                   rtDW.iC);
                                }

                                if (!ColdReset) {
                                    x[0] += rtDW.t * rtDW.z[0];
                                    x[1] += rtDW.t * rtDW.z[1];
                                    x[2] += rtDW.t * rtDW.z[2];
                                    std::frexp(1.0, &b_exponent);
                                    if (std::abs(rtDW.t - rtDW.cVal) <
                                            2.2204460492503131E-16) {
                                        if (rtDW.nA == degrees) {
                                            *status = -1;
                                            exitg1 = 1;
                                        } else {
                                            rtDW.nA++;
                                            rtDW.iC[rtDW.nA - 1] = rtDW.kNext +
                                                1;
                                            rtDW.kDrop = rtDW.nA - 1;
                                            exitg4 = false;
                                            while ((!exitg4) && (rtDW.kDrop + 1 >
                                                    1)) {
                                                iC = rtDW.iC[rtDW.kDrop - 1];
                                                if (rtDW.iC[rtDW.kDrop] > iC) {
                                                    exitg4 = true;
                                                } else {
                                                    rtDW.iSave =
                                                        rtDW.iC[rtDW.kDrop];
                                                    rtDW.iC[rtDW.kDrop] = iC;
                                                    rtDW.iC[rtDW.kDrop - 1] =
                                                        rtDW.iSave;
                                                    rtDW.kDrop--;
                                                }
                                            }

                                            iA[rtDW.kNext] = true;
                                            rtDW.kNext = -1;
                                            (*status)++;
                                        }
                                    } else {
                                        (*status)++;
                                    }
                                } else {
                                    (*status)++;
                                }
                            }
                        }
                    } else {
                        rtDW.cMin = norm(x);
                        if (std::abs(rtDW.cMin - rtDW.Xnorm0) > 0.001) {
                            rtDW.Xnorm0 = rtDW.cMin;
                            rtDW.cTol[0] = std::fmax(std::abs(b[0]), 1.0);
                            rtDW.cTol[1] = std::fmax(std::abs(b[1]), 1.0);
                            rtDW.cTol[2] = std::fmax(std::abs(b[2]), 1.0);
                            rtDW.cTol[3] = std::fmax(std::abs(b[3]), 1.0);
                            cTolComputed = false;
                        }

                        exitg1 = 2;
                    }
                } while (exitg1 == 0);

                if (exitg1 == 1) {
                    exitg2 = true;
                }
            }
        }
    }
}

// Function for MATLAB Function: '<S39>/FixedHorizonOptimizer'
void ModelPredictiveControl::mpcblock_optimizer(const double rseq[20], const
    double vseq[22], double umin, double umax, int32_t switch_in, const double
    x[5], double old_u, const bool iA[4], const double b_Mlim[4], double b_Mx[20],
    double b_Mu1[4], double b_Mv[88], const double b_utarget[10], double b_uoff,
    int32_t b_enable_value, double b_H[9], double b_Ac[12], const double b_Wy[2],
    const double b_Jm[20], const double b_I1[10], const double b_A[25], const
    double Bu[55], const double Bv[110], const double b_C[10], const double Dv
    [44], const int32_t b_Mrows[4], double *u, double useq[11], double *status,
    bool iAout[4])
{
    static const int8_t c_A[100]{ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
        0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };

    int16_t ixw;
    int8_t rows[2];
    int8_t kidx;
    int8_t rows_0;
    bool exitg1;
    bool exitg2;
    bool guard1;
    bool guard2;
    bool umax_incr_flag;
    bool umin_incr_flag;
    memset(&useq[0], 0, 11U * sizeof(double));
    *status = 1.0;
    iAout[0] = false;
    iAout[1] = false;
    iAout[2] = false;
    iAout[3] = false;
    if (switch_in != b_enable_value) {
        *u = old_u + b_uoff;
        for (rtDW.i_o = 0; rtDW.i_o < 11; rtDW.i_o++) {
            useq[rtDW.i_o] = *u;
        }
    } else {
        for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
            rtDW.Sum_p[rtDW.i1] = 0.0;
            for (rtDW.Tries = 0; rtDW.Tries < 5; rtDW.Tries++) {
                rtDW.normH = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 5; rtDW.CA_tmp++) {
                    rtDW.normH += b_C[(rtDW.CA_tmp << 1) + rtDW.i1] * b_A[5 *
                        rtDW.Tries + rtDW.CA_tmp];
                }

                rtDW.CA_tmp = (rtDW.Tries << 1) + rtDW.i1;
                rtDW.CA_b[rtDW.CA_tmp] = rtDW.normH;
                rtDW.Sum_p[rtDW.i1] += b_C[rtDW.CA_tmp] * Bu[rtDW.Tries];
            }

            for (rtDW.Tries = 0; rtDW.Tries < 2; rtDW.Tries++) {
                rtDW.normH = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 5; rtDW.CA_tmp++) {
                    rtDW.normH += b_C[(rtDW.CA_tmp << 1) + rtDW.i1] * Bv[5 *
                        rtDW.Tries + rtDW.CA_tmp];
                }

                rtDW.Bc[rtDW.i1 + (rtDW.Tries << 1)] = rtDW.normH;
            }
        }

        rtDW.b_Hv[0] = rtDW.Bc[0];
        rtDW.b_Hv[40] = Dv[0];
        rtDW.b_Hv[1] = rtDW.Bc[1];
        rtDW.b_Hv[41] = Dv[1];
        rtDW.b_Hv[20] = rtDW.Bc[2];
        rtDW.b_Hv[60] = Dv[2];
        rtDW.b_Hv[21] = rtDW.Bc[3];
        rtDW.b_Hv[61] = Dv[3];
        for (rtDW.i1 = 0; rtDW.i1 < 18; rtDW.i1++) {
            rtDW.Tries = (rtDW.i1 + 4) * 20;
            rtDW.b_Hv[rtDW.Tries] = 0.0;
            rtDW.b_Hv[rtDW.Tries + 1] = 0.0;
        }

        for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
            memset(&rtDW.b_Hv[rtDW.i1 * 20 + 2], 0, 18U * sizeof(double));
        }

        for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
            rtDW.Tries = rtDW.i1 << 1;
            rtDW.b_Sx[20 * rtDW.i1] = rtDW.CA_b[rtDW.Tries];
            rtDW.b_Sx[20 * rtDW.i1 + 1] = rtDW.CA_b[rtDW.Tries + 1];
        }

        rtDW.b_Su1[0] = rtDW.Sum_p[0];
        rtDW.b_Su1[1] = rtDW.Sum_p[1];
        for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
            memset(&rtDW.b_Sx[rtDW.i1 * 20 + 2], 0, 18U * sizeof(double));
        }

        memset(&rtDW.b_Su1[2], 0, 18U * sizeof(double));
        rtDW.Su[0] = rtDW.Sum_p[0];
        rtDW.Su[1] = rtDW.Sum_p[1];
        for (rtDW.i1 = 0; rtDW.i1 < 9; rtDW.i1++) {
            rtDW.Tries = (rtDW.i1 + 1) * 20;
            rtDW.Su[rtDW.Tries] = 0.0;
            rtDW.Su[rtDW.Tries + 1] = 0.0;
        }

        for (rtDW.i1 = 0; rtDW.i1 < 10; rtDW.i1++) {
            memset(&rtDW.Su[rtDW.i1 * 20 + 2], 0, 18U * sizeof(double));
        }

        for (rtDW.i_o = 0; rtDW.i_o < 9; rtDW.i_o++) {
            kidx = static_cast<int8_t>(((rtDW.i_o + 1) << 1) + 1);
            for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
                rows_0 = static_cast<int8_t>(rtDW.i1 + kidx);
                rows[rtDW.i1] = rows_0;
                rtDW.normH = 0.0;
                for (rtDW.Tries = 0; rtDW.Tries < 5; rtDW.Tries++) {
                    rtDW.normH += rtDW.CA_b[(rtDW.Tries << 1) + rtDW.i1] *
                        Bu[rtDW.Tries];
                }

                rtDW.normH += rtDW.Sum_p[rtDW.i1];
                rtDW.Sum_p[rtDW.i1] = rtDW.normH;
                rtDW.b_Su1[rows_0 - 1] = rtDW.normH;
                rtDW.Sum[rtDW.i1] = rtDW.normH;
            }

            rows_0 = rows[0];
            kidx = rows[1];
            for (rtDW.i1 = 0; rtDW.i1 < 9; rtDW.i1++) {
                rtDW.Tries = (rtDW.i1 + 1) << 1;
                rtDW.Sum[rtDW.Tries] = rtDW.Su[(20 * rtDW.i1 + rows_0) - 3];
                rtDW.Sum[rtDW.Tries + 1] = rtDW.Su[(20 * rtDW.i1 + kidx) - 3];
            }

            rows_0 = rows[0];
            kidx = rows[1];
            for (rtDW.i1 = 0; rtDW.i1 < 10; rtDW.i1++) {
                rtDW.Tries = rtDW.i1 << 1;
                rtDW.Su[(rows_0 + 20 * rtDW.i1) - 1] = rtDW.Sum[rtDW.Tries];
                rtDW.Su[(kidx + 20 * rtDW.i1) - 1] = rtDW.Sum[rtDW.Tries + 1];
            }

            for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
                for (rtDW.Tries = 0; rtDW.Tries < 2; rtDW.Tries++) {
                    rtDW.normH = 0.0;
                    for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 5; rtDW.CA_tmp++) {
                        rtDW.normH += rtDW.CA_b[(rtDW.CA_tmp << 1) + rtDW.i1] *
                            Bv[5 * rtDW.Tries + rtDW.CA_tmp];
                    }

                    rtDW.Bc[rtDW.i1 + (rtDW.Tries << 1)] = rtDW.normH;
                }
            }

            rtDW.CA[0] = rtDW.Bc[0];
            rtDW.CA[1] = rtDW.Bc[1];
            rtDW.CA[2] = rtDW.Bc[2];
            rtDW.CA[3] = rtDW.Bc[3];
            rows_0 = rows[0];
            kidx = rows[1];
            for (rtDW.i1 = 0; rtDW.i1 < 20; rtDW.i1++) {
                rtDW.CA_tmp = (rtDW.i1 + 2) << 1;
                rtDW.CA[rtDW.CA_tmp] = rtDW.b_Hv[(20 * rtDW.i1 + rows_0) - 3];
                rtDW.CA[rtDW.CA_tmp + 1] = rtDW.b_Hv[(20 * rtDW.i1 + kidx) - 3];
            }

            rows_0 = rows[0];
            kidx = rows[1];
            for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
                rtDW.Tries = rtDW.i1 << 1;
                rtDW.b_Hv[(rows_0 + 20 * rtDW.i1) - 1] = rtDW.CA[rtDW.Tries];
                rtDW.b_Hv[(kidx + 20 * rtDW.i1) - 1] = rtDW.CA[rtDW.Tries + 1];
            }

            for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
                for (rtDW.Tries = 0; rtDW.Tries < 5; rtDW.Tries++) {
                    rtDW.normH = 0.0;
                    for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 5; rtDW.CA_tmp++) {
                        rtDW.normH += rtDW.CA_b[(rtDW.CA_tmp << 1) + rtDW.i1] *
                            b_A[5 * rtDW.Tries + rtDW.CA_tmp];
                    }

                    rtDW.CA_p[rtDW.i1 + (rtDW.Tries << 1)] = rtDW.normH;
                }
            }

            memcpy(&rtDW.CA_b[0], &rtDW.CA_p[0], 10U * sizeof(double));
            rows_0 = rows[0];
            kidx = rows[1];
            for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
                rtDW.Tries = rtDW.i1 << 1;
                rtDW.b_Sx[(rows_0 + 20 * rtDW.i1) - 1] = rtDW.CA_b[rtDW.Tries];
                rtDW.b_Sx[(kidx + 20 * rtDW.i1) - 1] = rtDW.CA_b[rtDW.Tries + 1];
            }
        }

        for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
            for (rtDW.Tries = 0; rtDW.Tries < 20; rtDW.Tries++) {
                rtDW.s = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 10; rtDW.CA_tmp++) {
                    rtDW.s += rtDW.Su[20 * rtDW.CA_tmp + rtDW.Tries] * b_Jm[10 *
                        rtDW.i1 + rtDW.CA_tmp];
                }

                rtDW.b_SuJm[rtDW.Tries + 20 * rtDW.i1] = rtDW.s;
            }
        }

        if (b_Mrows[0] > 0) {
            rtDW.i_o = 0;
            exitg1 = false;
            while ((!exitg1) && (rtDW.i_o < 4)) {
                if (b_Mrows[rtDW.i_o] <= 20) {
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    b_Ac[rtDW.i_o] = -rtDW.b_SuJm[rtDW.Tries - 1];
                    b_Ac[rtDW.i_o + 4] = -rtDW.b_SuJm[rtDW.Tries + 19];
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
                        b_Mx[rtDW.i_o + (rtDW.i1 << 2)] = -rtDW.b_Sx[(20 *
                            rtDW.i1 + rtDW.Tries) - 1];
                    }

                    b_Mu1[rtDW.i_o] = -rtDW.b_Su1[b_Mrows[rtDW.i_o] - 1];
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
                        b_Mv[rtDW.i_o + (rtDW.i1 << 2)] = -rtDW.b_Hv[(20 *
                            rtDW.i1 + rtDW.Tries) - 1];
                    }

                    rtDW.i_o++;
                } else if (b_Mrows[rtDW.i_o] <= 40) {
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    b_Ac[rtDW.i_o] = rtDW.b_SuJm[rtDW.Tries - 21];
                    b_Ac[rtDW.i_o + 4] = rtDW.b_SuJm[rtDW.Tries - 1];
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
                        b_Mx[rtDW.i_o + (rtDW.i1 << 2)] = rtDW.b_Sx[(20 *
                            rtDW.i1 + rtDW.Tries) - 21];
                    }

                    b_Mu1[rtDW.i_o] = rtDW.b_Su1[b_Mrows[rtDW.i_o] - 21];
                    rtDW.Tries = b_Mrows[rtDW.i_o];
                    for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
                        b_Mv[rtDW.i_o + (rtDW.i1 << 2)] = rtDW.b_Hv[(20 *
                            rtDW.i1 + rtDW.Tries) - 21];
                    }

                    rtDW.i_o++;
                } else {
                    exitg1 = true;
                }
            }
        }

        rtDW.i_o = -1;
        for (rtDW.Tries = 0; rtDW.Tries < 10; rtDW.Tries++) {
            for (rtDW.i1 = 0; rtDW.i1 < 10; rtDW.i1++) {
                rtDW.a_b[(rtDW.i_o + rtDW.i1) + 1] = c_A[10 * rtDW.Tries +
                    rtDW.i1];
            }

            rtDW.i_o += 10;
        }

        for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
            for (rtDW.Tries = 0; rtDW.Tries < 10; rtDW.Tries++) {
                rtDW.normH = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 10; rtDW.CA_tmp++) {
                    rtDW.normH += static_cast<double>(rtDW.a_b[10 * rtDW.CA_tmp
                        + rtDW.Tries]) * b_Jm[10 * rtDW.i1 + rtDW.CA_tmp];
                }

                rtDW.Sum[rtDW.Tries + 10 * rtDW.i1] = rtDW.normH;
            }
        }

        ixw = 1;
        for (rtDW.i_o = 0; rtDW.i_o < 20; rtDW.i_o++) {
            rtDW.normH = b_Wy[ixw - 1];
            rtDW.WySuJm[rtDW.i_o] = rtDW.normH * rtDW.b_SuJm[rtDW.i_o];
            rtDW.WySuJm[rtDW.i_o + 20] = rtDW.b_SuJm[rtDW.i_o + 20] * rtDW.normH;
            ixw = static_cast<int16_t>(ixw + 1);
            if (ixw > 2) {
                ixw = 1;
            }
        }

        for (rtDW.i_o = 0; rtDW.i_o < 10; rtDW.i_o++) {
            rtDW.WuI2Jm[rtDW.i_o] = Wu * rtDW.Sum[rtDW.i_o];
            rtDW.WduJm[rtDW.i_o] = Wdu * b_Jm[rtDW.i_o];
            rtDW.WuI2Jm[rtDW.i_o + 10] = rtDW.Sum[rtDW.i_o + 10] * Wu;
            rtDW.WduJm[rtDW.i_o + 10] = b_Jm[rtDW.i_o + 10] * Wdu;
        }

        for (rtDW.i1 = 0; rtDW.i1 < 2; rtDW.i1++) {
            for (rtDW.Tries = 0; rtDW.Tries < 2; rtDW.Tries++) {
                rtDW.s = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 20; rtDW.CA_tmp++) {
                    rtDW.s += rtDW.b_SuJm[20 * rtDW.i1 + rtDW.CA_tmp] *
                        rtDW.WySuJm[20 * rtDW.Tries + rtDW.CA_tmp];
                }

                rtDW.normH = 0.0;
                rtDW.b_Jm = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 10; rtDW.CA_tmp++) {
                    rtDW.i_o = 10 * rtDW.i1 + rtDW.CA_tmp;
                    rtDW.b_Jm_tmp = 10 * rtDW.Tries + rtDW.CA_tmp;
                    rtDW.b_Jm += b_Jm[rtDW.i_o] * rtDW.WduJm[rtDW.b_Jm_tmp];
                    rtDW.normH += rtDW.Sum[rtDW.i_o] * rtDW.WuI2Jm[rtDW.b_Jm_tmp];
                }

                b_H[rtDW.i1 + 3 * rtDW.Tries] = (rtDW.s + rtDW.b_Jm) +
                    rtDW.normH;
            }

            rtDW.normH = 0.0;
            for (rtDW.Tries = 0; rtDW.Tries < 20; rtDW.Tries++) {
                rtDW.normH += rtDW.WySuJm[20 * rtDW.i1 + rtDW.Tries] *
                    rtDW.b_Su1[rtDW.Tries];
            }

            rtDW.s = 0.0;
            for (rtDW.Tries = 0; rtDW.Tries < 10; rtDW.Tries++) {
                rtDW.s += rtDW.WuI2Jm[10 * rtDW.i1 + rtDW.Tries] *
                    b_I1[rtDW.Tries];
            }

            rtDW.Sum_p[rtDW.i1] = rtDW.normH + rtDW.s;
        }

        for (rtDW.i1 = 0; rtDW.i1 < 20; rtDW.i1++) {
            rtDW.WuI2Jm[rtDW.i1] = -rtDW.WuI2Jm[rtDW.i1];
        }

        for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
            for (rtDW.Tries = 0; rtDW.Tries < 2; rtDW.Tries++) {
                rtDW.normH = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 20; rtDW.CA_tmp++) {
                    rtDW.normH += rtDW.b_Sx[20 * rtDW.i1 + rtDW.CA_tmp] *
                        rtDW.WySuJm[20 * rtDW.Tries + rtDW.CA_tmp];
                }

                rtDW.CA_b[rtDW.i1 + 5 * rtDW.Tries] = rtDW.normH;
            }
        }

        for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
            for (rtDW.Tries = 0; rtDW.Tries < 2; rtDW.Tries++) {
                rtDW.s = 0.0;
                for (rtDW.CA_tmp = 0; rtDW.CA_tmp < 20; rtDW.CA_tmp++) {
                    rtDW.s += rtDW.b_Hv[20 * rtDW.i1 + rtDW.CA_tmp] *
                        rtDW.WySuJm[20 * rtDW.Tries + rtDW.CA_tmp];
                }

                rtDW.CA[rtDW.i1 + 22 * rtDW.Tries] = rtDW.s;
            }
        }

        for (rtDW.i1 = 0; rtDW.i1 < 40; rtDW.i1++) {
            rtDW.WySuJm[rtDW.i1] = -rtDW.WySuJm[rtDW.i1];
        }

        rtDW.i_o = 0;
        memcpy(&rtDW.L_c[0], &b_H[0], 9U * sizeof(double));
        rtDW.Tries = xpotrf(rtDW.L_c);
        guard1 = false;
        if (rtDW.Tries == 0) {
            rtDW.varargin_1[0] = rtDW.L_c[0];
            rtDW.varargin_1[1] = rtDW.L_c[4];
            rtDW.varargin_1[2] = rtDW.L_c[8];
            if (minimum(rtDW.varargin_1) > 1.4901161193847656E-7) {
            } else {
                guard1 = true;
            }
        } else {
            guard1 = true;
        }

        if (guard1) {
            rtDW.normH = 0.0;
            rtDW.Tries = 0;
            exitg2 = false;
            while ((!exitg2) && (rtDW.Tries < 3)) {
                rtDW.s = (std::abs(b_H[rtDW.Tries + 3]) + std::abs
                          (b_H[rtDW.Tries])) + std::abs(b_H[rtDW.Tries + 6]);
                if (std::isnan(rtDW.s)) {
                    rtDW.normH = (rtNaN);
                    exitg2 = true;
                } else {
                    if (rtDW.s > rtDW.normH) {
                        rtDW.normH = rtDW.s;
                    }

                    rtDW.Tries++;
                }
            }

            if (rtDW.normH >= 1.0E+10) {
                rtDW.i_o = 2;
            } else {
                rtDW.Tries = 0;
                exitg1 = false;
                while ((!exitg1) && (rtDW.Tries <= 4)) {
                    rtDW.normH = rt_powd_snf(10.0, static_cast<double>
                        (rtDW.Tries)) * 1.4901161193847656E-7;
                    for (rtDW.i1 = 0; rtDW.i1 < 9; rtDW.i1++) {
                        rtDW.b_b[rtDW.i1] = 0;
                    }

                    rtDW.b_b[0] = 1;
                    rtDW.b_b[4] = 1;
                    rtDW.b_b[8] = 1;
                    for (rtDW.i1 = 0; rtDW.i1 < 9; rtDW.i1++) {
                        rtDW.s = rtDW.normH * static_cast<double>
                            (rtDW.b_b[rtDW.i1]) + b_H[rtDW.i1];
                        b_H[rtDW.i1] = rtDW.s;
                        rtDW.L_c[rtDW.i1] = rtDW.s;
                    }

                    rtDW.i_o = xpotrf(rtDW.L_c);
                    guard2 = false;
                    if (rtDW.i_o == 0) {
                        rtDW.varargin_1[0] = rtDW.L_c[0];
                        rtDW.varargin_1[1] = rtDW.L_c[4];
                        rtDW.varargin_1[2] = rtDW.L_c[8];
                        if (minimum(rtDW.varargin_1) > 1.4901161193847656E-7) {
                            rtDW.i_o = 1;
                            exitg1 = true;
                        } else {
                            guard2 = true;
                        }
                    } else {
                        guard2 = true;
                    }

                    if (guard2) {
                        rtDW.i_o = 3;
                        rtDW.Tries++;
                    }
                }
            }
        }

        if (rtDW.i_o > 1) {
            *u = old_u + b_uoff;
            for (rtDW.i_o = 0; rtDW.i_o < 11; rtDW.i_o++) {
                useq[rtDW.i_o] = *u;
            }

            *status = -2.0;
        } else {
            for (rtDW.i1 = 0; rtDW.i1 < 9; rtDW.i1++) {
                rtDW.b_b[rtDW.i1] = 0;
            }

            rtDW.b_b[0] = 1;
            rtDW.b_b[4] = 1;
            rtDW.b_b[8] = 1;
            for (rtDW.i_o = 0; rtDW.i_o < 3; rtDW.i_o++) {
                b_H[3 * rtDW.i_o] = rtDW.b_b[3 * rtDW.i_o];
                rtDW.i1 = 3 * rtDW.i_o + 1;
                b_H[rtDW.i1] = rtDW.b_b[rtDW.i1];
                rtDW.i1 = 3 * rtDW.i_o + 2;
                b_H[rtDW.i1] = rtDW.b_b[rtDW.i1];
            }

            trisolve(rtDW.L_c, b_H);
            for (rtDW.i1 = 0; rtDW.i1 < 4; rtDW.i1++) {
                rtDW.normH = 0.0;
                for (rtDW.Tries = 0; rtDW.Tries < 5; rtDW.Tries++) {
                    rtDW.normH += b_Mx[(rtDW.Tries << 2) + rtDW.i1] *
                        x[rtDW.Tries];
                }

                rtDW.s = 0.0;
                for (rtDW.Tries = 0; rtDW.Tries < 22; rtDW.Tries++) {
                    rtDW.s += b_Mv[(rtDW.Tries << 2) + rtDW.i1] *
                        vseq[rtDW.Tries];
                }

                rtDW.Bc[rtDW.i1] = -(((b_Mlim[rtDW.i1] + rtDW.normH) +
                                      b_Mu1[rtDW.i1] * old_u) + rtDW.s);
            }

            umax_incr_flag = false;
            rtDW.normH = 0.0;
            umin_incr_flag = false;
            rtDW.s = 0.0;
            if (b_Mrows[0] > 0) {
                rtDW.i_o = 0;
                exitg1 = false;
                while ((!exitg1) && (rtDW.i_o < 4)) {
                    if ((b_Mrows[rtDW.i_o] <= 20) || (b_Mrows[rtDW.i_o] <= 40))
                    {
                        rtDW.i_o++;
                    } else if (b_Mrows[rtDW.i_o] <= 50) {
                        if (!umax_incr_flag) {
                            rtDW.normH = -(RMVscale * umax - b_uoff) -
                                (-b_Mlim[rtDW.i_o]);
                            umax_incr_flag = true;
                        }

                        rtDW.Bc[rtDW.i_o] += rtDW.normH;
                        rtDW.i_o++;
                    } else if (b_Mrows[rtDW.i_o] <= 60) {
                        if (!umin_incr_flag) {
                            rtDW.s = (RMVscale * umin - b_uoff) -
                                (-b_Mlim[rtDW.i_o]);
                            umin_incr_flag = true;
                        }

                        rtDW.Bc[rtDW.i_o] += rtDW.s;
                        rtDW.i_o++;
                    } else {
                        exitg1 = true;
                    }
                }
            }

            rtDW.varargin_1[0] = 0.0;
            rtDW.varargin_1[1] = 0.0;
            rtDW.varargin_1[2] = 0.0;
            for (rtDW.i_o = 0; rtDW.i_o < 2; rtDW.i_o++) {
                rtDW.normH = 0.0;
                for (rtDW.i1 = 0; rtDW.i1 < 5; rtDW.i1++) {
                    rtDW.normH += rtDW.CA_b[5 * rtDW.i_o + rtDW.i1] * x[rtDW.i1];
                }

                rtDW.b_Jm = 0.0;
                for (rtDW.i1 = 0; rtDW.i1 < 20; rtDW.i1++) {
                    rtDW.b_Jm += rtDW.WySuJm[20 * rtDW.i_o + rtDW.i1] *
                        rseq[rtDW.i1];
                }

                rtDW.s = 0.0;
                for (rtDW.i1 = 0; rtDW.i1 < 22; rtDW.i1++) {
                    rtDW.s += rtDW.CA[22 * rtDW.i_o + rtDW.i1] * vseq[rtDW.i1];
                }

                rtDW.WuI2Jm_j = 0.0;
                for (rtDW.i1 = 0; rtDW.i1 < 10; rtDW.i1++) {
                    rtDW.WuI2Jm_j += rtDW.WuI2Jm[10 * rtDW.i_o + rtDW.i1] *
                        b_utarget[rtDW.i1];
                }

                rtDW.varargin_1[rtDW.i_o] = (((rtDW.normH + rtDW.b_Jm) +
                    rtDW.Sum_p[rtDW.i_o] * old_u) + rtDW.s) + rtDW.WuI2Jm_j;
            }

            iAout[0] = iA[0];
            iAout[1] = iA[1];
            iAout[2] = iA[2];
            iAout[3] = iA[3];
            for (rtDW.i1 = 0; rtDW.i1 < 3; rtDW.i1++) {
                for (rtDW.Tries = 0; rtDW.Tries < 3; rtDW.Tries++) {
                    rtDW.L_c[rtDW.i1 + 3 * rtDW.Tries] = (b_H[3 * rtDW.i1 + 1] *
                        b_H[3 * rtDW.Tries + 1] + b_H[3 * rtDW.i1] * b_H[3 *
                        rtDW.Tries]) + b_H[3 * rtDW.i1 + 2] * b_H[3 * rtDW.Tries
                        + 2];
                }
            }

            qpkwik(b_H, rtDW.L_c, rtDW.varargin_1, b_Ac, rtDW.Bc, iAout, 120,
                   1.0E-6, rtDW.zopt, rtDW.a__1, &rtDW.i_o);
            if ((rtDW.i_o < 0) || (rtDW.i_o == 0)) {
                rtDW.zopt[0] = 0.0;
            }

            *status = rtDW.i_o;
            *u = (old_u + rtDW.zopt[0]) + b_uoff;
        }
    }
}


double rt_powd_snf(double u0, double u1)
{
    double y;
    if (std::isnan(u0) || std::isnan(u1)) {
        y = (rtNaN);
    } else {
        double tmp;
        double tmp_0;
        tmp = std::abs(u0);
        tmp_0 = std::abs(u1);
        if (std::isinf(u1)) {
            if (tmp == 1.0) {
                y = 1.0;
            } else if (tmp > 1.0) {
                if (u1 > 0.0) {
                    y = (rtInf);
                } else {
                    y = 0.0;
                }
            } else if (u1 > 0.0) {
                y = 0.0;
            } else {
                y = (rtInf);
            }
        } else if (tmp_0 == 0.0) {
            y = 1.0;
        } else if (tmp_0 == 1.0) {
            if (u1 > 0.0) {
                y = u0;
            } else {
                y = 1.0 / u0;
            }
        } else if (u1 == 2.0) {
            y = u0 * u0;
        } else if ((u1 == 0.5) && (u0 >= 0.0)) {
            y = std::sqrt(u0);
        } else if ((u0 < 0.0) && (u1 > std::floor(u1))) {
            y = (rtNaN);
        } else {
            y = std::pow(u0, u1);
        }
    }

    return y;
}

double rt_hypotd_snf(double u0, double u1)
{
    double a;
    double b;
    double y;
    a = std::abs(u0);
    b = std::abs(u1);
    if (a < b) {
        a /= b;
        y = std::sqrt(a * a + 1.0) * b;
    } else if (a > b) {
        b /= a;
        y = std::sqrt(b * b + 1.0) * a;
    } else if (std::isnan(b)) {
        y = (rtNaN);
    } else {
        y = a * 1.4142135623730951;
    }

    return y;
}

static int32_t div_nde_s32_floor(int32_t numerator, int32_t denominator)
{
    return (((numerator < 0) != (denominator < 0)) && (numerator % denominator
             != 0) ? -1 : 0) + numerator / denominator;
}

static void rt_InitInfAndNaN(size_t realSize)
{
    (void) (realSize);
    rtNaN = rtGetNaN();
    rtNaNF = rtGetNaNF();
    rtInf = rtGetInf();
    rtInfF = rtGetInfF();
    rtMinusInf = rtGetMinusInf();
    rtMinusInfF = rtGetMinusInfF();
}

//
// Initialize rtNaN needed by the generated code.
// NaN is initialized as non-signaling. Assumes IEEE.
//
static double rtGetNaN(void)
{
    size_t bitsPerReal{ sizeof(double) * (NumBitsPerChar) };

    double nan{ 0.0 };

    if (bitsPerReal == 32U) {
        nan = rtGetNaNF();
    } else {
        union {
            LittleEndianIEEEDouble bitVal;
            double fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0xFFF80000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        nan = tmpVal.fltVal;
    }

    return nan;
}

//
// Initialize rtNaNF needed by the generated code.
// NaN is initialized as non-signaling. Assumes IEEE.
//
static float rtGetNaNF(void)
{
    IEEESingle nanF{ { 0.0F } };

    nanF.wordL.wordLuint = 0xFFC00000U;
    return nanF.wordL.wordLreal;
}

static double rtGetInf(void)
{
    size_t bitsPerReal{ sizeof(double) * (NumBitsPerChar) };

    double inf{ 0.0 };

    if (bitsPerReal == 32U) {
        inf = rtGetInfF();
    } else {
        union {
            LittleEndianIEEEDouble bitVal;
            double fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0x7FF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        inf = tmpVal.fltVal;
    }

    return inf;
}

//
// Initialize rtInfF needed by the generated code.
// Inf is initialized as non-signaling. Assumes IEEE.
//
static float rtGetInfF(void)
{
    IEEESingle infF;
    infF.wordL.wordLuint = 0x7F800000U;
    return infF.wordL.wordLreal;
}

//
// Initialize rtMinusInf needed by the generated code.
// Inf is initialized as non-signaling. Assumes IEEE.
//
static double rtGetMinusInf(void)
{
    size_t bitsPerReal{ sizeof(double) * (NumBitsPerChar) };

    double minf{ 0.0 };

    if (bitsPerReal == 32U) {
        minf = rtGetMinusInfF();
    } else {
        union {
            LittleEndianIEEEDouble bitVal;
            double fltVal;
        } tmpVal;

        tmpVal.bitVal.words.wordH = 0xFFF00000U;
        tmpVal.bitVal.words.wordL = 0x00000000U;
        minf = tmpVal.fltVal;
    }

    return minf;
}

//
// Initialize rtMinusInfF needed by the generated code.
// Inf is initialized as non-signaling. Assumes IEEE.
//
static float rtGetMinusInfF(void)
{
    IEEESingle minfF;
    minfF.wordL.wordLuint = 0xFF800000U;
    return minfF.wordL.wordLreal;
}