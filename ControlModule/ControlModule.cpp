#include "ControlModule.h"
#include "Communicator.h"
#include "Controller.h"
#include "PathManager.h"


// ------------------------------ (Global) Struct Variable ------------------- //
GPSStruct GPS;
VehicleStruct Vehicle;
GlobalPathStruct Global;
LocalPathStruct Local;
ControlStruct Control;


// ------------------------------ Loop Flag & Signal --------------------------------- //
bool MainFlag = true;
bool SocketFlag = true;
bool MCUSendSignal = false;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;
bool ViewerSendSignal = false;

// ------------------------------ MAIN -------------------------------------- //
int main(void)
{
    struct timeval FirstTime, SecondTime;
    thread TDkey, TDcan, TDgps, TDmcu, TDsrc, TDview;
    double TimeGap;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], SavePath[100], format[5] = ".txt";
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d",  tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(SavePath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer);

    FILE* RecordFile;
    if (GPSRecord)
    {
        RecordFile = fopen(SavePath, "w");
        if(RecordFile == NULL)
        {
            printf("<Error Opening File>\n");
            return 1;
        }
    }

    PathConverter PathManager;
    Lateral LateralControl;
    Longitudinal LongitudinalControl;
    
    //-------thread of Communication---------//
    TDkey = thread(Key);
    TDcan = thread(VehicleReceiver);
    TDgps = thread(GPSReceiver);
    TDmcu = thread(MCUSender);

    if (ViewerFlag)
        TDview = thread(ViewerSender);

    // -------Path Initialize------------//
    if (ReceivePathFlag)
        TDsrc = thread(SRCCommunication);
    else
        PathManager.ImportFile(ReferenceFile.c_str());


    cout << "------------------ ControlModule START ! ------------------" << endl;
    
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
                PathReceiveSignal = false;
                if (ReceivePathFlag) 
                    PathManager.InitializePath();
                
                PathManager.GenerateLocalPath();

                // Control
                LateralControl.PurPursuit();
                LongitudinalControl.SCC();

                MCUSendSignal = true;
                SRCSendSignal = true;
                PathReceiveSignal = true;
                ViewerSendSignal = true;

                if (GPSRecord) fprintf(RecordFile, "%.7f/%.7f\n", GPS.Latitude, GPS.Longitude);
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
    TDcan.join();
    TDgps.join();
    TDmcu.join();

    if (ReceivePathFlag) TDsrc.join();
    if (ViewerFlag) TDview.join();
    if (GPSRecord) fclose(RecordFile);
    cout << "------------------ ControlModule END ! ------------------" << endl;
    TDkey.join();

    //cin.ignore(); // terminal typing dead block
    return 1;
}