#include "ControlModule/include/ControlModule.h"
#include "ControlModule/include/Communicator.h"
#include "ControlModule/include/PathManager.h"
#include "ControlModule/include/Controller.h"

// ------------------------------ (Global) Struct Variable ------------------- //
GPSVariable GPS;
VehicleVariable Vehicle;
GlobalPathVariable Global;
LocalPathVariable Local;
ControlVariable Control;

// ------------------------------ Loop Flag & Signal --------------------------------- //
bool MainFlag = true;
bool SocketFlag = true;
bool MCUSendSignal = false;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;
bool ViewerSendSignal = false;
bool PathErrorFlag = false;

//static PathConvert PathManager;
static ControlLogic Controller;

// ------------------------------ MAIN -------------------------------------- //
int main(int argc, const char*argv[])
{
    (void)(argc);
    (void)(argv);

    struct timeval FirstTime, SecondTime;
    thread TDkey, TDvehicle, TDibeo, TDgps, TDmcu, TDpath, TDview;
    uint16_t TimeGap;
    uint32_t MainCnt = 0;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], format[5] = ".txt";
    char GPSPath[100], PathPath[100], VehiclePath[100], ControlPath[100];
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d",  tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(GPSPath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer);
    sprintf(PathPath, "%s%s.txt", PathRecordPath.c_str(), TimeBuffer);
    sprintf(VehiclePath, "%s%s.txt", VehicleRecordPath.c_str(), TimeBuffer);    
    sprintf(ControlPath, "%s%s.txt", ControlRecordPath.c_str(), TimeBuffer);

    FILE* GPSFile;
    if (GPSRecord)
    {
        GPSFile = fopen(GPSPath, "w");
        if(GPSFile == NULL)
        {
            printf("<Error Opening GPSLogFile>\n");
            return 1;
        }
    }
    FILE* PathFile;
    if (PathRecord)
    {
        PathFile = fopen(PathPath, "w");
        if(PathFile == NULL)
        {
            printf("<Error Opening PathLogFile>\n");
            return 1;
        }
    }
    FILE* VehicleFile;
    if (VehicleRecord)
    {
        VehicleFile = fopen(VehiclePath, "w");
        if(VehicleFile == NULL)
        {
            printf("<Error Opening VehicleLogFile>\n");
            return 1;
        }
    }
    FILE* ControlFile;
    if (ControlRecord)
    {
        ControlFile = fopen(ControlPath, "w");
        if(ControlFile == NULL)
        {
            printf("<Error Opening ControlLogFile>\n");
            return 1;
        }
    }
    
    //-------thread for Communication---------//
    TDkey = thread(Key);
    TDvehicle = thread(VehicleReceiver);
    TDibeo = thread(IbeoReceiver);
    TDgps = thread(GPSParser);
    TDmcu = thread(MCUSender);

    gettimeofday(&FirstTime, NULL);
    while(Vehicle.MDPSmode != 3)
    {
        gettimeofday(&SecondTime, NULL);
        TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
        if( TimeGap > 500)
        {
            cout << "[ControlModule] ------------------ VehicleData updating...... MDPSMode : "<<(int)Vehicle.MDPSmode << endl;
            gettimeofday(&FirstTime, NULL);
        }
    }
    cout << "[ControlModule] ------------------ VehicleData update success! " << endl;

    gettimeofday(&FirstTime, NULL);
    while(GPS.Time == 0)
    {
        gettimeofday(&SecondTime, NULL);
        TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
        if( TimeGap > 500)
        {
            cout << "[ControlModule] ------------------ GPSData updating...... GPSTime : "<< (int)GPS.Time << endl;
            gettimeofday(&FirstTime, NULL);
        }
    }
    cout << "[ControlModule] ------------------ GPSData update success! " << endl;

    if (ViewerFlag)
        TDview = thread(ViewerSender);

    // -------Path Initialize------------//
    if (ReceivePathFlag)
    {
        TDpath = thread(PathReceiver);
        gettimeofday(&FirstTime, NULL);
        while(PathReceiveSignal == true)
        {
            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
            if(TimeGap > 500)
            {
                cout << "[ControlModule] ------------------ PathVertex updating...... " << endl;
                gettimeofday(&FirstTime, NULL);
            }
        }
    }
    else
    {
        PathManager.ImportFile(ReferenceFile.c_str());
        gettimeofday(&FirstTime, NULL);
        while (PathManager.WayPointNum == 0)
        {
            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
            if(TimeGap > 500)
            {
                cout << "[ControlModule] ------------------ PathVertex updating...... " << endl;
                gettimeofday(&FirstTime, NULL);
            }
        }
    }
    cout << "[ControlModule] ------------------ PathVertex update success! " << endl;

    cout << "[ControlModule] ------------------ ControlModule START ! " << endl;
    gettimeofday(&FirstTime, NULL);
    while (MainFlag)
    {
        try
        {
            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
            if (TimeGap >= MainCycle)
            {
                // Path
                if (ReceivePathFlag)
                {
                    if (PathReceiveSignal == false) 
                        PathManager.InitializePath();
                }

                if (PathErrorFlag == false)
                {
                    PathManager.GenerateLocalPath();
                    Controller.PurePursuit();
                    //printf("GAP:%.2lf  END:%d  LD:%d  Handle:%.2lf\n", Global.LocalizationGap, Local.Length, Control.LookAheadIdx, Control.Handle);
                }

                //test
                Control.Acceleration = 1023;

                MCUSendSignal = true;
                SRCSendSignal = true;
                PathReceiveSignal = true;
                ViewerSendSignal = true;

                if (GPSRecord) fprintf(GPSFile, "%d/%.7lf/%.7lf\n", MainCnt,GPS.Latitude, GPS.Longitude);
                if (PathRecord) 
                {
                    for (uint32_t i=0; i<128; i++)
                    {
                        fprintf(PathFile, "%d/%.7lf/%.7lf\n", MainCnt,Global.Latitude[i], Global.Longitude[i]);
                    }
                }
                MainCnt++;
                if (VehicleRecord) fprintf(VehicleFile, "%d/%.2lf/%.2lf/%.2lf/%.2lf/%.2lf/%.2lf\n", MainCnt, Vehicle.Velocity, Global.LocalizationGap, Control.Handle, Vehicle.YawRate, Vehicle.HandleAngle, Vehicle.HandleSpd);
                if (ControlRecord) fprintf(ControlFile, "%d/%.2lf/%d/%.2lf/%.2lf/%.2lf/%.2lf/%.2lf\n", MainCnt, Global.LocalizationGap, Local.Length, Control.Curvature, Control.LateralDeviation, Control.RelativeHeadingAngle, Control.Acceleration, Control.Handle);
                gettimeofday(&FirstTime, NULL);
            }
        }
        catch (out_of_range& e) {
            cout << "<MAIN> Out_of_range Error" << '\n';
        }
        catch (length_error& e) {
            cout << "<MAIN> Length Error" << '\n';
        }
        catch(exception& e){
            cout << "<MAIN> EXCEPTION " << '\n';
            cout << e.what() << '\n';
        }
    }
    TDvehicle.join();
    TDibeo.join();
    TDgps.join();
    TDmcu.join();

    if (ReceivePathFlag) TDpath.join();
    if (ViewerFlag) TDview.join();
    if (GPSRecord) fclose(GPSFile);
    if (PathRecord) fclose(PathFile);
    if (VehicleRecord) fclose(VehicleFile);
    if (ControlRecord) fclose(ControlFile);
    cout << "[ControlModule] ------------------ ControlModule END ! " << endl;
    TDkey.join();

    //cin.ignore(); // terminal typing dead block
    return 0;
}