#include "ControlModule.h"
#include "Communicator.h"
#include "PathManager.h"

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;
// 30 ~ 130 byte low
/* ------------------------------- Main ------------------------------- */
int main()
{
    struct timeval startTime, endTime;
    thread PedestrianThread;
    thread KeyThread;
    thread GPSThread;
    thread VehicleThread;
    thread SRCThread;

    double MinimumPedestrianDistance = 0.0; // Vertex - 보행자 최소거리
    double CurrentPedestrianDistance = 0.0;

    /* GPS Path Save File */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], SavePath[100], format[5] = ".txt";

    // Path name
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d",  tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(SavePath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer); // ./data/GPS

    FILE* RecordFile;
    if (GPSRecord) // default: false
    {
        RecordFile = fopen(SavePath, "w");
        if(RecordFile == NULL)
        {
            printf("<Error Opening File>\n");
            return 1;
        }
    }

    /* thread of Communication */
    KeyThread = thread(Key);
    GPSThread = thread(GPSReceiver);
    VehicleThread = thread(VehicleReceiver);
    PedestrianThread = thread(PedestrianReceiver); // Mobileye 보행자 상대좌표

    /* Path Initialize */
    if (ReceivePathFlag) // config.ini 기본 False -> Path 실시간 true
        SRCThread = thread(SRCCommunication); // 경로 새로 탐색
    else
        PathManager.ImportFile(ReferenceFile.c_str()); // 저장되어 있는 경로

    std::cout << "------------------ Project START ! ------------------" << endl;
    gettimeofday(&startTime, NULL);
    // thread update 대기 코드
    while (MainFlag)
    {
        try
        {
            gettimeofday(&endTime, NULL);
            double TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]

            if (TimeGap >= MainCycle)
            {
                // Path
                PathReceiveSignal = false;
                if (ReceivePathFlag)
                    PathManager.InitializePath();

                PathManager.GenerateLocalPath();

                /* Mobileye */
                PathManager.PedestrianDistance(); // 차량-보행자 최소거리
                
                // 경로 상에 보행자가 있다면 Vertex 전방거리 반환
                if (Pedestrian.MinimumPedestrianDistance <= 1.8 )
                {
                    cout << "[Communicator]----------- Vertex Front Distance : " << PathManager.FrontVertexDistance << "[m]-----------" << endl;
                    
                }
                
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

    PedestrianThread.join();
    GPSThread.join();
    VehicleThread.join();
    if (ReceivePathFlag)
        SRCThread.join();
    cout << "------------------ Pedestrian Distance Module END ! ------------------" << endl;
    fclose(RecordFile);
    KeyThread.join();

    return 1;
}
