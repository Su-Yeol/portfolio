#include "ControlModule.h"
#include "Communicator.h"
#include "Controller.h"
#include "PathManager.h"

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;
bool MCUSendSignal = false;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;

/* (Global) Struct Variable */
GPSStruct GPS;
VehicleStruct Vehicle;
GlobalPathStruct Global;
LocalPathStruct Local;
ControlStruct Control;
PedestrianStruct Pedestrian;

/* Loop Flag & Signal */
bool MainFlag = true;
bool SocketFlag = true;
bool MCUSendSignal = false;
bool SRCSendSignal = false;
bool PathReceiveSignal = true;
bool ViewerSendSignal = false;

/* ------------------------------- Main ------------------------------- */
int main()
{
    struct timeval startTime, endTime;
    thread PedestrianThread;
    thread KeyThread; thread GPSThread;
    thread VehicleThread; thread SRCThread;
    
    double MinimumPedestrianDistance = 0.0; // Vertex - 보행자 최소거리
    int MinimumPedestrianidx = 0; // 최소거리 index
    char PedestrianClass; // 최소거리 Class

    PathConverter PathManager; // 경로 데이터

    /* thread of Communication */
    KeyThread = thread(Key);
    PedestrianThread = thread(PedestrianReceiver); // Mobileye 보행자 상대좌표
    GPSThread = thread(GPSReceiver);
    VehicleThread = thread(VehicleReceiver);

    /* Path Initialize */
    if (ReceivePathFlag)
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
                MinimumPedestrianidx, PedestrianClass, MinimumPedestrianDistance = PathManager.PedestrianDistance(); // 차량-보행자 최소거리

                MCUSendSignal = true;
                SRCSendSignal = true;
                PathReceiveSignal = true;

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
    cout << "------------------ ControlModule END ! ------------------" << endl;
    KeyThread.join();

    return 1;
}
