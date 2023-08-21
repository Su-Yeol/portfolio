#include "Communicator.h"

// ------------------------------ Config --------------------------------- //
CConfigParser Configuration("./config.ini");

const int TargetSpeed = Configuration.GetInt("TargetSpeed"); // [kph]

const bool GPSRecord = Configuration.GetBool("GPSRecord");
const string GPSRecordPath = Configuration.GetString("GPSRecordPath");

const bool ReceivePathFlag = Configuration.GetBool("ReceivePathFlag");
const string ReferenceFile = Configuration.GetString("ReferenceFile");
const string ExternalIp = Configuration.GetString("ExternalIp");
const int ExternalPortReceive = Configuration.GetInt("ExternalPortReceive");
const int ExternalPortSend = Configuration.GetInt("ExternalPortSend");

const int MainCycle = Configuration.GetInt("MainCycle");
const int MCUReceiveCycle = Configuration.GetInt("MCUReceiveCycle");

const string S32GIp = Configuration.GetString("S32GIp");
const int S32GPort = Configuration.GetInt("S32GPort");
const string MCUIp = Configuration.GetString("MCUIp");
const int MCUPort = Configuration.GetInt("MCUPort");

const bool ViewerFlag = Configuration.GetBool("ViewerFlag");
const string ViewerIp = Configuration.GetString("ViewerIp");
const int ViewerPortClient = Configuration.GetInt("ViewerPortClient");
const int ViewerPortServer = Configuration.GetInt("ViewerPortServer");

// ------------------------------ Communicator Function ----------------------- //
void VehicleReceiver()
{
    // local //
    double WheelSpeedFL, WheelSpeedFR;
    VehicleStruct VehicleCache;
    CANClass VehicleCANFD;
    VehicleCANFD.SetSocket("can0", 1);

    cout << "[Communicator]-----------VehicleReceiver Thread start!-------------" << endl;
    while (SocketFlag)
    {
        try
        {
            VehicleCANFD.ReceiveCANFD();

            switch (VehicleCANFD.FrameFd.can_id)
            {
            case 0x4A:
                VehicleCache.YawRate = ((VehicleCANFD.FrameFd.data[9] << 8) + VehicleCANFD.FrameFd.data[8]) * 0.005 - 163.84;                       // [deg/s]
                VehicleCache.LateralAccel = ((VehicleCANFD.FrameFd.data[11] << 8) + VehicleCANFD.FrameFd.data[10]) * 0.000127465 - 4.17677312;      // [g]
                VehicleCache.LongitudinalAccel = ((VehicleCANFD.FrameFd.data[13] << 8) + VehicleCANFD.FrameFd.data[12]) * 0.000127465 - 4.17677312; // [g]
                break;

            case 0xA0:
                WheelSpeedFL = (((VehicleCANFD.FrameFd.data[9] & 0x3F) << 8) + VehicleCANFD.FrameFd.data[8]) * 0.03125;   // [kph]
                WheelSpeedFR = (((VehicleCANFD.FrameFd.data[11] & 0x3F) << 8) + VehicleCANFD.FrameFd.data[10]) * 0.03125; // [kph]
                VehicleCache.Velocity = (WheelSpeedFL + WheelSpeedFR) / (2. * 3.6);                                       // [m/s]
                break;

            case 0x125:
                VehicleCache.HandleAngle = ((VehicleCANFD.FrameFd.data[4] << 8) + VehicleCANFD.FrameFd.data[3]) * 0.1; // [deg]
                VehicleCache.HandleSpd = (VehicleCANFD.FrameFd.data[5]) * 4;                                           // [deg/s]
                break;

            case 0x1A0:
                VehicleCache.Radar.Distance = (((VehicleCANFD.FrameFd.data[4] & 0x07) << 8) + VehicleCANFD.FrameFd.data[3]) * 0.1;
                VehicleCache.Radar.RelativeVelocity = (((VehicleCANFD.FrameFd.data[5] & 0x7F) << 5) + ((VehicleCANFD.FrameFd.data[4] & 0xF8) >> 3)) * 0.1 - 170;
                break;

            case 0x1AA:
                VehicleCache.ClusterVelocity = (((VehicleCANFD.FrameFd.data[9] & 0x01) << 8) + VehicleCANFD.FrameFd.data[8]) / 3.6; // [m/s]
                break;

            case 0x3C1:
                VehicleCache.LeftTurnSwitch = (VehicleCANFD.FrameFd.data[3] & 0xC0) >> 6;
                VehicleCache.RightTurnSwitch = VehicleCANFD.FrameFd.data[4] & 0x03;
            }

            Vehicle = VehicleCache;
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<VehicleReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<VehicleReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<VehicleReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    VehicleCANFD.CloseSocket();
    cout << "[Communicator]-----------VehicleReceiver Socket Closed!-------------" << endl;
}

void GPSReceiver()
{
    UDPClass GPSBD;
    GPSStruct GPSCache;
    GPSBD.SetServerSocket("192.168.100.255", S32GPort); // S32G
    struct timeval FirstTime, SecondTime;
    double TimeGap;

    cout << "[Communicator]-----------GPSReceiver Thread start!-------------" << endl;
    gettimeofday(&FirstTime, NULL);
    while (SocketFlag)
    {
        try
        {
            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000);
            if (TimeGap >= MCUReceiveCycle)
            {
                GPSBD.Receive(24);
                GPSCache.Time = 0.001 * (uint32_t)((GPSBD.ReceiveBuffer[8] << 24) + (GPSBD.ReceiveBuffer[7] << 16) + (GPSBD.ReceiveBuffer[6] << 8) + GPSBD.ReceiveBuffer[5]);
                GPSCache.Latitude = 0.0000001 * (uint32_t)((GPSBD.ReceiveBuffer[12] << 24) + (GPSBD.ReceiveBuffer[11] << 16) + (GPSBD.ReceiveBuffer[10] << 8) + GPSBD.ReceiveBuffer[9]);
                GPSCache.Longitude = 0.0000001 * (uint32_t)((GPSBD.ReceiveBuffer[16] << 24) + (GPSBD.ReceiveBuffer[15] << 16) + (GPSBD.ReceiveBuffer[14] << 8) + GPSBD.ReceiveBuffer[13]);
                GPSCache.Azimuth = 0.01 * ((uint32_t)((GPSBD.ReceiveBuffer[20] << 24) + (GPSBD.ReceiveBuffer[19] << 16) + (GPSBD.ReceiveBuffer[18] << 8) + GPSBD.ReceiveBuffer[17]));
                GPSCache.State = GPSBD.ReceiveBuffer[21];

                GPS = GPSCache;
                gettimeofday(&FirstTime, NULL);
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<GPSReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<GPSReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<GPSReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    GPSBD.CloseSocket();
    cout << "[Communicator]-----------GPSReceiver Socket Closed!-------------" << endl;
}

/* 보행자 데이터 수신 */
/* void PedestrianReceiver()
{
    // 주형연권님
    UDPClass PedestrianUDP;
    PedestrianStruct PedestrianCashe;
    struct timeval startTime, endTime;

    PedestrianUDP.SetServerSocket(S32GIp, ExternalPortReceive); // ip, port 선정 ft.주형 연구원님
    cout << "[Communicator]-----------Pedestrian Thread start!-------------" << endl;

    gettimeofday(&startTime, NULL);

    while (SocketFlag)
    {
        try
        {
            gettimeofday(&endTime, NULL);
            double timeGap = ((endTime.tv_sec - startTime.tv_sec) * 1000) + ((endTime.tv_usec - startTime.tv_usec) / 1000); // 1ms

            if (timeGap >= MCUReceiveCycle)
            {
                PedestrianUDP.Receive(8); // [left camera, class, X, Y, right camera, class, X, Y]
                PedestrianCashe.Direction[0] = PedestrianUDP.ReceiveBuffer[0];
                PedestrianCashe.Direction[1] = PedestrianUDP.ReceiveBuffer[4];
                PedestrianCashe.Class[0] = PedestrianUDP.ReceiveBuffer[1];
                PedestrianCashe.Class[1] = PedestrianUDP.ReceiveBuffer[5];
                PedestrianCashe.X[0] = PedestrianUDP.ReceiveBuffer[2];
                PedestrianCashe.Y[0] = PedestrianUDP.ReceiveBuffer[3];
                PedestrianCashe.X[1] = PedestrianUDP.ReceiveBuffer[6];
                PedestrianCashe.Y[1] = PedestrianUDP.ReceiveBuffer[7];

                Pedestrian = PedestrianCashe;

                gettimeofday(&startTime, NULL);
            }
        }

        catch (std::out_of_range &e)
        {
            std::cout << "<PedestrianReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<PedestrianReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<PedestrianReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    PedestrianUDP.CloseSocket();
    cout << "[Communicator]-----------PedestrianUDPReceiver Socket Closed!-------------" << endl;
} */

/* 보행자 데이터 수신 */
void PedestrianReceiver()
{
    /* Mobileye */
    CANClass PedestrianCANFD;
    PedestrianStruct PedestrianCashe;
    double MobileyeFactor = 1.838e-4;
    double MobileyeOffset = -0.305;

    PedestrianCANFD.SetSocket("can1", 1); // CAN FD
    cout << "[Communicator]-----------Pedestrian Thread start!-------------" << endl;

    while (SocketFlag)
    {
        try
        {
            //return 객체 1개당 x, y, distance
            switch (PedestrianCANFD.FrameFd.can_id)
            {
            case 0x180:
                /* Change index */
                if (PedestrianCANFD.FrameFd.data[1] == 0x50)
                {
                    // Left
                    PedestrianCashe.X[0] = ((PedestrianCANFD.FrameFd.data[2] << 8) + PedestrianCANFD.FrameFd.data[3]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[0] = ((PedestrianCANFD.FrameFd.data[4] << 8) + PedestrianCANFD.FrameFd.data[5]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[0] = '180L';
                    // Right
                    PedestrianCashe.X[1] = ((PedestrianCANFD.FrameFd.data[10] << 8) + PedestrianCANFD.FrameFd.data[11]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[1] = ((PedestrianCANFD.FrameFd.data[12] << 8) + PedestrianCANFD.FrameFd.data[13]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[1] = '180R';
                }
                break;

            case 0x181:
                if (PedestrianCANFD.FrameFd.data[1] == 0x50)
                {
                    // Left
                    PedestrianCashe.X[2] = ((PedestrianCANFD.FrameFd.data[2] << 8) + PedestrianCANFD.FrameFd.data[3]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[2] = ((PedestrianCANFD.FrameFd.data[4] << 8) + PedestrianCANFD.FrameFd.data[5]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[2] = '181L';
                    // Right
                    PedestrianCashe.X[3] = ((PedestrianCANFD.FrameFd.data[10] << 8) + PedestrianCANFD.FrameFd.data[11]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[3] = ((PedestrianCANFD.FrameFd.data[12] << 8) + PedestrianCANFD.FrameFd.data[13]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[3] = '181R';
                }
                break;

            case 0x182:
                if (PedestrianCANFD.FrameFd.data[1] == 0x50)
                {
                    // Left
                    PedestrianCashe.X[4] = ((PedestrianCANFD.FrameFd.data[2] << 8) + PedestrianCANFD.FrameFd.data[3]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[4] = ((PedestrianCANFD.FrameFd.data[4] << 8) + PedestrianCANFD.FrameFd.data[5]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[4] = '182L';
                    // Right
                    PedestrianCashe.X[5] = ((PedestrianCANFD.FrameFd.data[10] << 8) + PedestrianCANFD.FrameFd.data[11]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[5] = ((PedestrianCANFD.FrameFd.data[12] << 8) + PedestrianCANFD.FrameFd.data[13]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[5] = '182R';
                }
                break;

            case 0x183:
                if (PedestrianCANFD.FrameFd.data[1] == 0x50)
                {
                    // Left
                    PedestrianCashe.X[6] = ((PedestrianCANFD.FrameFd.data[2] << 8) + PedestrianCANFD.FrameFd.data[3]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[6] = ((PedestrianCANFD.FrameFd.data[4] << 8) + PedestrianCANFD.FrameFd.data[5]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[6] = '183L';
                    // Right
                    PedestrianCashe.X[7] = ((PedestrianCANFD.FrameFd.data[10] << 8) + PedestrianCANFD.FrameFd.data[11]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[7] = ((PedestrianCANFD.FrameFd.data[12] << 8) + PedestrianCANFD.FrameFd.data[13]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[7] = '183R';
                }
                break;

            case 0x184:
                if (PedestrianCANFD.FrameFd.data[1] == 0x50)
                {
                    // Left
                    PedestrianCashe.X[8] = ((PedestrianCANFD.FrameFd.data[2] << 8) + PedestrianCANFD.FrameFd.data[3]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[8] = ((PedestrianCANFD.FrameFd.data[4] << 8) + PedestrianCANFD.FrameFd.data[5]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[8] = '184L';
                    // Right
                    PedestrianCashe.X[9] = ((PedestrianCANFD.FrameFd.data[10] << 8) + PedestrianCANFD.FrameFd.data[11]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Y[9] = ((PedestrianCANFD.FrameFd.data[12] << 8) + PedestrianCANFD.FrameFd.data[13]) * MobileyeFactor + MobileyeOffset;
                    PedestrianCashe.Class[9] = '184R';
                }
                break;

            default:
                break;
            }
        }

        catch (std::out_of_range &e)
        {
            std::cout << "<PedestrianReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<PedestrianReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<PedestrianReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }

    PedestrianCANFD.CloseSocket();
    cout << "[Communicator]-----------PedestrianUDPReceiver Socket Closed!-------------" << endl;
}

/* S32G -> VCU */
void MCUSender()
{
    UDPClass MCU;
    MCU.SetServerSocket(S32GIp, S32GPort);
    MCU.SetClientSocket(MCUIp, MCUPort);
    uint8_t AliveCnt = 0;
    int handle, Ax;

    cout << "[Communicator]-----------MCUSender Thread start!-------------" << endl;
    while (SocketFlag)
    {
        try
        {
            if (MCUSendSignal)
            {
                handle = (int)(Control.Handle * 10. + 3600.);
                Ax = (int)Control.Acceleration;

                MCU.SendBuffer[0] = 0x01;
                MCU.SendBuffer[1] = 0x02;
                MCU.SendBuffer[2] = 0x03;
                MCU.SendBuffer[3] = 0x04;
                MCU.SendBuffer[4] = AliveCnt;
                MCU.SendBuffer[5] = AliveCnt >> 8;
                MCU.SendBuffer[6] = 0;
                MCU.SendBuffer[7] = 0;
                MCU.SendBuffer[8] = 0;
                MCU.SendBuffer[9] = 0;
                MCU.SendBuffer[10] = 0;
                MCU.SendBuffer[12] = TargetSpeed;
                MCU.SendBuffer[15] = 0; // PathFollower
                MCU.SendBuffer[16] = handle;
                MCU.SendBuffer[17] = handle >> 8;
                MCU.SendBuffer[18] = handle >> 16;
                MCU.SendBuffer[19] = handle >> 24;
                MCU.SendBuffer[20] = Ax;
                MCU.SendBuffer[21] = Ax >> 8;

                if (AliveCnt == 255)
                    AliveCnt = 0;
                else
                    AliveCnt++;
                MCU.Send(22);

                MCUSendSignal = false;
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<MCUSender> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<MCUSender> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<MCUSender> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    MCU.CloseSocket();
    cout << "[Communicator]-----------MCUSender Socket Closed!-------------" << endl;
}

void SRCCommunication()
{
    UDPClass SRC;
    GlobalPathStruct GlobalCache;
    GPSStruct GPSCache;
    VehicleStruct VehicleCache;
    SRC.SetServerSocket(S32GIp, ExternalPortReceive);
    SRC.SetClientSocket(ExternalIp, ExternalPortReceive);

    cout << "[Communicator]-----------SRCCommunication Thread start!-------------" << endl;
    while (SocketFlag)
    {
        try
        {
            if (SRCSendSignal)
            {
                GPSCache = GPS;
                VehicleCache = Vehicle;
                SRC.SendBuffer[0] = (uint32_t)(GPSCache.Time * 1000);
                SRC.SendBuffer[1] = ((uint32_t)(GPSCache.Time * 1000)) >> 8;
                SRC.SendBuffer[2] = ((uint32_t)(GPSCache.Time * 1000)) >> 16;
                SRC.SendBuffer[3] = ((uint32_t)(GPSCache.Time * 1000)) >> 24;
                SRC.SendBuffer[4] = (uint32_t)(GPSCache.Latitude * 10000000);
                SRC.SendBuffer[5] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 8;
                SRC.SendBuffer[6] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 16;
                SRC.SendBuffer[7] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 24;
                SRC.SendBuffer[8] = (uint32_t)(GPSCache.Longitude * 10000000);
                SRC.SendBuffer[9] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 8;
                SRC.SendBuffer[10] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 16;
                SRC.SendBuffer[11] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 24;
                SRC.SendBuffer[12] = (uint32_t)(GPSCache.Azimuth * 100);
                SRC.SendBuffer[13] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 8;
                SRC.SendBuffer[14] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 16;
                SRC.SendBuffer[15] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 24;
                SRC.SendBuffer[16] = GPS.State;
                SRC.SendBuffer[17] = (uint32_t)(VehicleCache.Velocity * 3.6 * 100);
                SRC.SendBuffer[18] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 8;
                SRC.SendBuffer[19] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 16;
                SRC.SendBuffer[20] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 24;
                SRC.SendBuffer[21] = VehicleCache.LeftTurnSwitch;
                SRC.SendBuffer[22] = VehicleCache.RightTurnSwitch;
                SRC.Send(23);
                SRCSendSignal = false;
            }

            SRC.Receive(PathSize);
            memset(&GlobalCache.Latitude, 0, PathSize);
            memset(&GlobalCache.Longitude, 0, PathSize);
            for (uint32_t i = 0; i < PathSize / 8; i++)
            {
                GlobalCache.Longitude[i] = 0.0000001 * (uint32_t)((SRC.ReceiveBuffer[8 * i + 3] << 24) + (SRC.ReceiveBuffer[8 * i + 2] << 16) + (SRC.ReceiveBuffer[8 * i + 1] << 8) + SRC.ReceiveBuffer[8 * i]);
                GlobalCache.Latitude[i] = 0.0000001 * (uint32_t)((SRC.ReceiveBuffer[8 * i + 7] << 24) + (SRC.ReceiveBuffer[8 * i + 6] << 16) + (SRC.ReceiveBuffer[8 * i + 5] << 8) + SRC.ReceiveBuffer[8 * i + 4]);
            }

            if (PathReceiveSignal)
            {
                Global = GlobalCache;
                PathReceiveSignal = false;
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<SRCCommunication> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<SRCCommunication> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<SRCCommunication> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    SRC.CloseSocket();
    cout << "[Communicator]-----------SRCCommunication Thread end!-------------" << endl;
}

void ViewerSender()
{
    TCPClass Viewer;

    Viewer.SetServerSocket(S32GIp, ViewerPortClient);
    cout << "[Communicator]-----------ViewerSender Thread start!-------------" << endl;

    while (SocketFlag)
    {
        try
        {
            if (ViewerSendSignal)
            {
                printf("%lf    %lf\n", Global.GapLatitude, Global.GapLongitude);
                // Location
                Viewer.SendBuffer[0] = (uint32_t)(Global.GapLatitude);
                Viewer.SendBuffer[1] = ((uint32_t)(Global.GapLatitude)) >> 8;
                Viewer.SendBuffer[2] = ((uint32_t)(Global.GapLatitude)) >> 16;
                Viewer.SendBuffer[3] = ((uint32_t)(Global.GapLatitude)) >> 24;
                Viewer.SendBuffer[4] = (uint32_t)(Global.GapLongitude);
                Viewer.SendBuffer[5] = ((uint32_t)(Global.GapLongitude)) >> 8;
                Viewer.SendBuffer[6] = ((uint32_t)(Global.GapLongitude)) >> 16;
                Viewer.SendBuffer[7] = ((uint32_t)(Global.GapLongitude)) >> 24;
                Viewer.SendBuffer[8] = (uint32_t)(Global.Heading * 100);
                Viewer.SendBuffer[9] = ((uint32_t)(Global.Heading * 100)) >> 8;
                Viewer.SendBuffer[10] = ((uint32_t)(Global.Heading * 100)) >> 16;
                Viewer.SendBuffer[11] = ((uint32_t)(Global.Heading * 100)) >> 24;

                // Vehicle
                Viewer.SendBuffer[12] = (int)(Vehicle.Velocity * 3.6);
                Viewer.SendBuffer[13] = (int)(Vehicle.LongitudinalAccel * 100);
                Viewer.SendBuffer[14] = ((int)(Vehicle.LongitudinalAccel * 100)) >> 8;
                Viewer.SendBuffer[15] = (int)(Vehicle.LateralAccel * 100);
                Viewer.SendBuffer[16] = ((int)(Vehicle.LateralAccel * 100)) >> 8;
                Viewer.SendBuffer[17] = (int)(Vehicle.YawRate * 100);
                Viewer.SendBuffer[18] = ((int)(Vehicle.YawRate * 100)) >> 8;
                Viewer.SendBuffer[19] = (uint32_t)(Vehicle.HandleAngle);
                Viewer.SendBuffer[20] = ((uint32_t)(Vehicle.HandleAngle)) >> 8;
                Viewer.SendBuffer[21] = ((uint32_t)(Vehicle.HandleAngle)) >> 16;
                Viewer.SendBuffer[22] = ((uint32_t)(Vehicle.HandleAngle)) >> 24;

                // Control Input
                Viewer.SendBuffer[23] = (uint32_t)(Control.Curvature * 10000);
                Viewer.SendBuffer[24] = ((uint32_t)(Control.Curvature * 10000)) >> 8;
                Viewer.SendBuffer[25] = ((uint32_t)(Control.Curvature * 10000)) >> 16;
                Viewer.SendBuffer[26] = ((uint32_t)(Control.Curvature * 10000)) >> 24;
                Viewer.SendBuffer[27] = (int)(Control.LateralDeviation * 100);
                Viewer.SendBuffer[28] = ((int)(Control.LateralDeviation * 100)) >> 8;
                Viewer.SendBuffer[29] = (int)(Control.RelativeHeadingAngle * 100);
                Viewer.SendBuffer[30] = ((int)(Control.RelativeHeadingAngle * 100)) >> 8;

                // Control Output
                Viewer.SendBuffer[31] = (uint32_t)(Control.Handle * 100);
                Viewer.SendBuffer[32] = ((uint32_t)(Control.Handle * 100)) >> 8;
                Viewer.SendBuffer[33] = ((uint32_t)(Control.Handle * 100)) >> 16;
                Viewer.SendBuffer[34] = ((uint32_t)(Control.Handle * 100)) >> 24;
                Viewer.SendBuffer[35] = (int)(Control.Acceleration * 100);
                Viewer.SendBuffer[36] = ((int)(Control.Acceleration * 100)) >> 8;

                // Path
                Viewer.SendBuffer[37] = Local.Length;
                Viewer.SendBuffer[38] = Local.Length >> 8;
                for (uint32_t i = 0; i < Local.Length; i++)
                {
                    Viewer.SendBuffer[8 * i + 39] = (uint32_t)(Global.ValidLatitude[i]);
                    Viewer.SendBuffer[8 * i + 40] = ((uint32_t)(Global.ValidLatitude[i])) >> 8;
                    Viewer.SendBuffer[8 * i + 41] = ((uint32_t)(Global.ValidLatitude[i])) >> 16;
                    Viewer.SendBuffer[8 * i + 42] = ((uint32_t)(Global.ValidLatitude[i])) >> 24;
                    Viewer.SendBuffer[8 * i + 43] = (uint32_t)(Global.ValidLongitude[i]);
                    Viewer.SendBuffer[8 * i + 44] = ((uint32_t)(Global.ValidLongitude[i])) >> 8;
                    Viewer.SendBuffer[8 * i + 45] = ((uint32_t)(Global.ValidLongitude[i])) >> 16;
                    Viewer.SendBuffer[8 * i + 46] = ((uint32_t)(Global.ValidLongitude[i])) >> 24;
                }
                Viewer.Send(BufferSize * 4);

                ViewerSendSignal = false;
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<ViewerSender> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<ViewerSender> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<ViewerSender> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    Viewer.CloseSocket();
    cout << "[Communicator]-----------ViewerSender Socket Closed!-------------" << endl;
}

/* Key 입력 */
int getch(void)
{
    /* 터미널 입력을 비동기적으로 받아오기 위한 함수
        터미널에서 키보드 입력을 받아오는 동작을 수행하며, 입력을 처리한 후 해당 입력값을 반환 */
    int ch;

    /* struct termios
        POSIX가 지정하는 표준 인터페이스
        인터페이스 제어: 5가지 모드(입력, 출력, 제어, 로컬, 특수 제어문자)로 구분 */
    struct termios buf;
    struct termios save;

    /* 터미널의 속성을 변경하기 위해서는 현재 터미널의 속성을 알아야한다
       그 후, 변경하고픈 터미널의 속성값을 비트연산을 사용하여 on/off 시킨다
       현재 터미널 설정을 읽는 함수: int tcgetattr(int fd, struct termios *termios_p)
                                                    ㄴfd: 속성을 알기위한 open file */
    tcgetattr(0, &save);
    buf = save;
    /* ICANON(정규모드), ECHO(입력 에코) 플래그를 비트 마스크를 통해 제거하여 비정규 모드 설정
       비정규 모드에서는 문자가 입력될 때마다 즉시 반환 */
    buf.c_lflag &= ~(ICANON | ECHO);
    /* 문자를 입력하면 즉시 반환 */
    buf.c_cc[VMIN] = 1;             // 입력을 기다리는 최소 문자 수를 1로 설정
    buf.c_cc[VTIME] = 0;            // 입력을 기다리는 최대 시간을 0으로 설정
    
    tcsetattr(0, TCSAFLUSH, &buf);  // buf 터미널 설정 변경하고 기존 입력 버퍼 삭제
    
    ch = getchar();                 // 키보드 입력받은 문자 저장
    tcsetattr(0, TCSAFLUSH, &save); // 입력 받은 후, 원래의 터미널 설정 복원
    
    return ch;
}

void Key()
{
    try
    {
        char key;
        for (; !(key == '\n');)
        {
            key = getch();
            if (key == 'q')
            {
                MainFlag = false;
                SocketFlag = false;
                break;
            }
        }
    }
    catch (std::out_of_range &e)
    {
        std::cout << "<Key> Out_of_range Error" << '\n';
    }
    catch (std::length_error &e)
    {
        std::cout << "<Key> Length Error" << '\n';
    }
    catch (std::exception &e)
    {
        std::cout << "<Key> EXCEPTION " << '\n';
        std::cout << e.what() << '\n';
    }
}

// ------------------------------- class Function ------------------------------------------- //
void CANClass::SetSocket(const std::string &ifname, const int canfd)
{
    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
        perror("<CAN> socket open error");

    strcpy(ifr.ifr_name, ifname.c_str());
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0)
    {
        perror("<CAN> Error with SIOCGIFINDEX ioctl");
        close(sock);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("<CAN> Error in socket bind");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (canfd)
    {
        if (setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &canfd, sizeof(canfd)))
        {
            perror("<CAN> Error enabling CAN FD support");
            close(sock);
            exit(EXIT_FAILURE);
        }
    }
}

void CANClass::ReceiveCANFD()
{
    addrlen = sizeof(addr);
    nbytes = recvfrom(sock, &FrameFd, sizeof(FrameFd), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0)
        perror("<CANFD> Read Error");
}

void CANClass::ReceiveCAN()
{
    addrlen = sizeof(addr);
    nbytes = recvfrom(sock, &Frame, sizeof(Frame), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0)
        perror("<CAN> Read Error");
}

void CANClass::SendCANFD()
{
    // FramFd : id, length, data[64] --> 20ms마다 : stop하면 다시 원래값으로 변경
    if (write(sock, &FrameFd, sizeof(FrameFd)) != sizeof(struct canfd_frame))
        perror("<CANFD> Send Error");
}

void CANClass::SendCAN()
{
    if (write(sock, &Frame, sizeof(Frame)) != sizeof(struct can_frame))
        perror("<CANFD> Send Error");
}

void CANClass::CloseSocket()
{
    close(sock);
}

void UDPClass::SetServerSocket(const std::string &ip, const uint16_t port)
{
    /* 소켓 생성 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("<UDP> socket Open Error");

    /* 소켓 주소 구조체 설정 */
    memset(&ServerAddr, 0x00, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;                    // AF_INET(IP용 Address Family), IPv4 주소 체계 사용
    ServerAddr.sin_addr.s_addr = inet_addr(ip.c_str()); // ip 네트워크 인터페이스로부터 수신
    ServerAddr.sin_port = htons(port);                  // htons: host-to-network short, 포트 번호 설정

    /* 소켓 바인딩 */
    if (bind(sock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
        perror("<UDP> bind Error");
}

void UDPClass::SetClientSocket(const std::string &ip, const uint16_t port)
{
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    ClientAddr.sin_port = htons(port);
}

void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(ClientAddr);
    if ((nbytes = recvfrom(sock, ReceiveBuffer, buffersize, 0, (struct sockaddr *)&ClientAddr, &addrlen)) < 0)
        perror("<UDP> Receive Error");
}

void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, SendBuffer, SendByte, 0, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr)) < 0)
        perror("<UDP> Send Error");
}

void UDPClass::CloseSocket()
{
    close(sock);
}

void TCPClass::SetServerSocket(const std::string &ip, const uint16_t port)
{
    if ((ServerSock = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        perror("<TCP> socket Open Error");

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    ServerAddr.sin_port = htons(port);

    if (bind(ServerSock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0)
        perror("<TCP> Bind Error");

    if (listen(ServerSock, 5) < 0)
        perror("<TCP> Listen Error");

    ClientAddrSize = sizeof(ClientAddr);
    if ((ClientSock = accept(ServerSock, (struct sockaddr *)&ClientAddr, &ClientAddrSize)) < 0)
        perror("<TCP> Accept Error");
}

void TCPClass::Send(const uint16_t SendByte)
{
    write(ClientSock, SendBuffer, SendByte);
}

void TCPClass::CloseSocket()
{
    close(ServerSock);
    close(ClientSock);
}