#include "JCommunicator.h"
#include "JControlModule.h"
#include "JPathManager.h"

/* Struct */
GPSStruct GPS;
GlobalPathStruct Global;
LocalPathStruct Local;
VehicleStruct Vehicle;
MobileyeStruct Mobileye;

/* Class */
PathConverter PathManager;

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;
bool MobileyeFlag = false;
// 30 ~ 130 byte low
/* ------------------------------- Main ------------------------------- */
int main()
{
    struct timeval startTime, endTime;
    thread MobileyeThread;
    thread KeyThread;
    thread GPSThread;
    thread VehicleThread;
    thread SRCThread;

    double TimeGap;
    /* PathManager.FrontVertexDistance = 0.0;
    Pedestrian.MinimumPedestrianDistance = 0.0;
    int MinimumPedestrianIdx = 0; // 보행자와 최소거리인 Vertex 위치 */

    /* GPS Path Save File */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], SavePath[100], format[5] = ".txt";

    // Path name
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d", tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(SavePath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer); // ./data/GPS

    FILE *RecordFile;
    if (GPSRecord) // default: false
    {
        RecordFile = fopen(SavePath, "w");
        if (RecordFile == NULL)
        {
            printf("<Error Opening File>\n");
            return 1;
        }
    }

    /* thread of Communication */
    KeyThread = thread(Key);
    GPSThread = thread(GPSReceiver);
    VehicleThread = thread(VehicleReceiver);   // MDPS 3 운전모드, 5 자율주행모드
    MobileyeThread = thread(MobileyeReceiver); // Mobileye 보행자 상대좌표

    gettimeofday(&startTime, NULL);
    while (GPS.Time == 0)
    {
        gettimeofday(&endTime, NULL);
        TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
        if (TimeGap > 500)
        {
            cout << "[ControlModule] ------------------ GPSData updating...... GPSTime : " << (int)GPS.Time << endl;
            gettimeofday(&startTime, NULL);
        }
    }
    cout << "[ControlModule] ------------------ GPSData update success! " << endl;

    /* Path Initialize */
    if (ReceivePathFlag) // config.ini 기본 False -> Path 실시간 true
    {
        SRCThread = thread(SRCCommunication); // 경로 새로 탐색

        gettimeofday(&startTime, NULL);
        while (ReceivePathFlag == true)
        {
            gettimeofday(&endTime, NULL);
            TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
            if (TimeGap > 500)
            {
                cout << "[ControlModule] ------------------ PathVertex updating...... " << endl;
                gettimeofday(&startTime, NULL);
            }
        }
    }
    else
    {
        PathManager.ImportFile(ReferenceFile.c_str()); // 저장되어 있는 경로

        gettimeofday(&startTime, NULL);
        while (PathManager.WayPointNum == 0)
        {
            gettimeofday(&endTime, NULL);
            TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
            if (TimeGap > 500)
            {
                cout << "[ControlModule] ------------------ PathVertex updating...... " << endl;
                gettimeofday(&startTime, NULL);
            }
        }
    }

    std::cout << "------------------ Project START ! ------------------" << endl;
    gettimeofday(&startTime, NULL);

    while (MainFlag)
    {
        try
        {
            gettimeofday(&endTime, NULL);
            TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]

            if (TimeGap >= MainCycle)
            {
                // Path
                PathReceiveSignal = false;

                // Mobileye Pedestrian
                MobileyeFlag = true;

                if (ReceivePathFlag)
                    PathManager.InitializePath();

                PathManager.GenerateLocalPath();

                /* Mobileye */
                PathManager.PedestrianDistance(); // 차량-보행자 최소거리

                SRCSendSignal = true;
                PathReceiveSignal = true;

                // Save File - 저장할 때만 사용
                //fprintf(RecordFile, "%.7f/%.7f\n", GPS.Latitude, GPS.Longitude);

                gettimeofday(&startTime, NULL);
            }
        }

        catch (out_of_range &e)
        {
            cout << "<MAIN> Out_of_range Error" << '\n';
        }
        catch (length_error &e)
        {
            cout << "<MAIN> Length Error" << '\n';
        }
        catch (exception &e)
        {
            cout << "<MAIN> EXCEPTION " << '\n';
            cout << e.what() << '\n';
        }
    }

    MobileyeThread.join();
    GPSThread.join();
    VehicleThread.join();
    if (ReceivePathFlag)
        SRCThread.join();
    cout << "------------------ Pedestrian Distance Module END ! ------------------" << endl;
    fclose(RecordFile);
    KeyThread.join();

    return 1;
}
