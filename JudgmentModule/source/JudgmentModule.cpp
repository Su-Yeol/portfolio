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
bool PathErrorFlag = false;

// 30 ~ 130 byte low
/* ------------------------------- Main ------------------------------- */
int main(int argc, const char *argv[])
{
    (void)(argc); // 메인함수에 전달되는 정보의 갯수
    (void)(argv); // 메인함수에 전달되는 실질적인 정보로, 문자열의 배열

    thread KeyThread, GPSThread, PathThread, VehicleThread, MobileyeThread, IbeoThread;

    struct timeval startTime, endTime;
    uint16_t TimeGap;

    /* GPS Path Save File */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], format[5] = ".txt";
    char GPSPath[100];

    // Path name
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d", tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(GPSPath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer); // ./data/GPS/

    FILE *GPSFile;
    if (GPSRecord) // default: false -> GPS raw data를 위해서 1
    {
        GPSFile = fopen(GPSPath, "w");
        if (GPSFile == NULL)
        {
            printf("<Error Opening File>\n");
            return 1;
        }
    }

    /* thread of Communication */
    KeyThread = thread(Key);
    GPSThread = thread(GPSParser);             // GPS 정보 송신
    VehicleThread = thread(VehicleReceiver);   // MDPS 3 운전모드, 5 자율주행모드
    MobileyeThread = thread(MobileyeReceiver); // Mobileye 보행자 상대좌표
    IbeoThread = thread(IbeoReceiver);         // Ibeo Data 수신

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
    if (ReceivePathFlag) // config.ini 기본 False -> Path 실시간 true(K-CITY)
    {
        PathThread = thread(PathReceiver); // 경로 새로 탐색

        gettimeofday(&startTime, NULL);
        while (PathReceiveSignal == true)
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
    cout << "[ControlModule] ------------------ PathVertex update success! " << endl;

    cout << "------------------ JudgmentModule START ! ------------------" << endl;
    gettimeofday(&startTime, NULL);
    while (MainFlag)
    {
        try
        {
            gettimeofday(&endTime, NULL);
            TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]
            if (TimeGap >= MainCycle)
            {
                // Mobileye Pedestrian
                MobileyeFlag = true;

                if (ReceivePathFlag)
                {
                    if (PathReceiveSignal == false)
                        PathManager.InitializePath();
                }

                if (PathErrorFlag == false)
                {
                    PathManager.GenerateLocalPath();
                }

                /* Mobileye */
                PathManager.PedestrianDistance(); // 차량-보행자 최소거리

                SRCSendSignal = true;
                PathReceiveSignal = true;

                if (GPSRecord)
                {
                    fprintf(GPSFile, "%.7f/%.7f/", GPS.Latitude, GPS.Longitude);

                    for (uint8_t i = 0; i < 100; i++) // GPS raw 데이터 저장
                        fprintf(GPSFile, "%c", GPSRaw[i]);

                    fprintf(GPSFile, "\n");
                }

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

    GPSThread.join();
    VehicleThread.join();
    MobileyeThread.join();
    IbeoThread.join();
    if (ReceivePathFlag)
        PathThread.join();
    if (GPSRecord)
        fclose(GPSFile);

    cout << "------------------ JudgmentModule END ! ------------------" << endl;
    KeyThread.join();

    return 0;
}
