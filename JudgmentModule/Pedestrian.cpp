#include "ControlModule.h"
#include "Communicator.h"
#include "PathManager.h"

/* (Global) Struct Variable */
GPSStruct GPS;
VehicleStruct Vehicle;
GlobalPathStruct Global;
LocalPathStruct Local;
PedestrianStruct Pedestrian;

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;

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

    PathConverter PathManager; // 경로 데이터

    /* GPS Path Save File */
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char TimeBuffer[50], SavePath[100], format[5] = ".txt";

    // Path name
    sprintf(TimeBuffer, "%02d.%02d.%02d-%02d:%02d:%02d",  tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(SavePath, "%s%s.txt", GPSRecordPath.c_str(), TimeBuffer);

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
    if (ReceivePathFlag) // 기본 False
        SRCThread = thread(SRCCommunication);
    else
        PathManager.ImportFile(ReferenceFile.c_str());

    std::cout << "------------------ Project START ! ------------------" << endl;
    gettimeofday(&startTime, NULL);

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

                /* for (uint32_t p = 0; p < Local.Length; p++) // Vertex
                {
                    for (uint32_t r = 0; r < 10; r++) // Mobileye Object count = 10
                    {
                        CurrentPedestrianDistance = sqrt(pow((Local.X[p] - Pedestrian.X[r]), 2) + pow((Local.Y[p] - Pedestrian.Y[r]), 2));

                        if (CurrentPedestrianDistance < MinimumPedestrianDistance)
                        {
                            MinimumPedestrianDistance = CurrentPedestrianDistance;
                            Pedestrian.MinimumPedestrianDistance = MinimumPedestrianDistance;
                        }
                    }
                }  */

                std::cout << "[Communicator]----------- Pedestrian Distance" << Pedestrian.MinimumPedestrianDistance << "-----------" << endl;

                SRCSendSignal = true;
                PathReceiveSignal = true;
                
                // Save File
                fprintf(RecordFile, "%.7f/%.7f\n", GPS.Latitude, GPS.Longitude);

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
