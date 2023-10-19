#include "JCommunicator.h"

// ------------------------------ Config --------------------------------- //
CConfigParser Configuration("./config.ini");

const int MainCycle = Configuration.GetInt("MainCycle");
const int TargetSpeed = Configuration.GetInt("TargetSpeed"); // [kph]

// GPSParser
const bool GPSRecord = Configuration.GetBool("GPSRecord");
const string GPSRecordPath = Configuration.GetString("GPSRecordPath");

// PathReceiver
const bool ReceivePathFlag = Configuration.GetBool("ReceivePathFlag");
const string ReferenceFile = Configuration.GetString("ReferenceFile");

// IP
const string S32GIp = Configuration.GetString("S32GIp");           // GPSParser, PathReceiver
const string BroadCastIp = Configuration.GetString("BroadCastIp"); // GPSParser
// const string ExternalIp = Configuration.GetString("ExternalIp");

// Port
const int S32GPort = Configuration.GetInt("S32GPort");       // 3004, GPSParser
const int BackPort = Configuration.GetInt("BackPort");       // 3862, GPSParser
const int ForwardPort = Configuration.GetInt("ForwardPort"); // 1785, PathReceiver

// ControlModule - No use
const string MCUIp = Configuration.GetString("MCUIp");
const int MCUPort = Configuration.GetInt("MCUPort");

// Viewer - No use
const bool ViewerFlag = Configuration.GetBool("ViewerFlag");
const string ViewerIp = Configuration.GetString("ViewerIp");
const int ViewerPort = Configuration.GetInt("ViewerPort");

// ------------------------------ Communicator Function ----------------------- //
void VehicleReceiver()
{
    // local //
    double WheelSpeedFL, WheelSpeedFR;
    float IbeoVehicleSpeed, IbeoSteerAngle, IbeoYawRate;

    VehicleStruct VehicleCache;
    CANClass VehicleCANFD;
    CANClass IbeoSend;
    VehicleCANFD.SetSocket("can0", 1);
    IbeoSend.SetSocket("can1", 0);

    std::cout << "[Communicator] ------------------- VehicleReceiver Thread start! " << endl;

    while (SocketFlag)
    {
        try
        {
            VehicleCANFD.ReceiveCANFD();

            switch (VehicleCANFD.FrameFd.can_id)
            {
            case 0x4A:                                                                                                                              // IMU Sensor
                VehicleCache.YawRate = ((VehicleCANFD.FrameFd.data[9] << 8) + VehicleCANFD.FrameFd.data[8]) * 0.005 - 163.84;                       // [deg/s]
                VehicleCache.LateralAccel = ((VehicleCANFD.FrameFd.data[11] << 8) + VehicleCANFD.FrameFd.data[10]) * 0.000127465 - 4.17677312;      // [g]
                VehicleCache.LongitudinalAccel = ((VehicleCANFD.FrameFd.data[13] << 8) + VehicleCANFD.FrameFd.data[12]) * 0.000127465 - 4.17677312; // [g]

                IbeoSend.InitFrame();
                IbeoYawRate = (VehicleCache.YawRate + 0.714700) / 0.000174;
                IbeoSend.Frame.can_id = 0x220;
                IbeoSend.Frame.can_dlc = 7;
                IbeoSend.Frame.data[5] = (int16_t)(IbeoVehicleSpeed);
                IbeoSend.Frame.data[6] = ((int16_t)(IbeoVehicleSpeed) >> 8) & 0x3F;
                IbeoSend.SendCAN();
                break;

            case 0xA0:
                WheelSpeedFL = (((VehicleCANFD.FrameFd.data[9] & 0x3F) << 8) + VehicleCANFD.FrameFd.data[8]) * 0.03125;   // [kph]
                WheelSpeedFR = (((VehicleCANFD.FrameFd.data[11] & 0x3F) << 8) + VehicleCANFD.FrameFd.data[10]) * 0.03125; // [kph]
                VehicleCache.Velocity = (WheelSpeedFL + WheelSpeedFR) / (2. * 3.6);                                       // [m/s]

                IbeoSend.InitFrame();
                IbeoVehicleSpeed = (VehicleCache.Velocity * 3.6) / 0.138889; // [kph]
                IbeoSend.Frame.can_id = 0x4F1;
                IbeoSend.Frame.can_dlc = 7;
                IbeoSend.Frame.data[1] = (uint16_t)(IbeoVehicleSpeed);
                IbeoSend.Frame.data[2] = (uint16_t)(IbeoVehicleSpeed) >> 8;
                IbeoSend.SendCAN();
                break;

            case 0x125:
                VehicleCache.HandleAngle = ((VehicleCANFD.FrameFd.data[4] << 8) + VehicleCANFD.FrameFd.data[3]) * 0.1; // [deg]
                VehicleCache.HandleSpd = (VehicleCANFD.FrameFd.data[5]) * 4;                                           // [deg/s]

                IbeoSend.InitFrame();
                IbeoSteerAngle = VehicleCache.HandleAngle / 0.001745;
                IbeoSend.Frame.can_id = 0x2B0;
                IbeoSend.Frame.can_dlc = 7;
                IbeoSend.Frame.data[0] = (int16_t)(IbeoSteerAngle);
                IbeoSend.Frame.data[1] = (int16_t)(IbeoSteerAngle) >> 8;
                IbeoSend.SendCAN();
                break;

            case 0x1A0: // SCC
                VehicleCache.Radar.Distance = (((VehicleCANFD.FrameFd.data[4] & 0x07) << 8) + VehicleCANFD.FrameFd.data[3]) * 0.1;
                VehicleCache.Radar.RelativeVelocity = (((VehicleCANFD.FrameFd.data[5] & 0x7F) << 5) + ((VehicleCANFD.FrameFd.data[4] & 0xF8) >> 3)) * 0.1 - 170;
                break;

            case 0x1AA:
                VehicleCache.ClusterVelocity = (((VehicleCANFD.FrameFd.data[9] & 0x01) << 8) + VehicleCANFD.FrameFd.data[8]) / 3.6; // [m/s]
                break;

            case 0x3C1:
                VehicleCache.LeftTurnSwitch = (VehicleCANFD.FrameFd.data[3] & 0xC0) >> 6;
                VehicleCache.RightTurnSwitch = VehicleCANFD.FrameFd.data[4] & 0x03;

            case 0xEA:
                VehicleCache.MDPSmode = VehicleCANFD.FrameFd.data[5] & 0x0F;

                // case 0x1016:
                //     IbeoRecv.Frame.data[0] = 1; // relative velocitiesb
                //     IbeoRecv.Frame.data[1] = 0; // object boxes
                //     break;
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
    IbeoSend.CloseSocket();
    std::cout << "[Communicator] ------------------- VehicleReceiver Socket Closed! " << endl;
}

void IbeoReceiver()
{
    // struct timeval startTime, endTime;
    // uint16_t TimeGap;

    CANClass IbeoRecv;
    IbeoVariable IbeoCache;
    IbeoRecv.SetSocket("can1", 0);
    uint8_t ObjectCnt = 0;

    std::cout << "[Communicator] ------------------- IbeoReceiver Thread start! " << endl;

    // gettimeofday(&startTime, NULL);
    while (SocketFlag)
    {
        try
        {
            // gettimeofday(&endTime, NULL);
            // TimeGap = (endTime.tv_sec - startTime.tv_sec) * 1000 + ((endTime.tv_usec - startTime.tv_usec) / 1000); // [ms]

            IbeoRecv.ReceiveCAN();
            switch (IbeoRecv.Frame.can_id)
            {
            case 0x500:
                IbeoCache.ObjectCnt = (int)(IbeoRecv.Frame.data[1]);
                /* bit 0
                    0 = absolute velocities
                    1 = relative velocities

                   bit 1
                    0 = object boxes
                    1 = bounding boxes */
                IbeoCache.Boxflag = IbeoRecv.Frame.data[4];
                ObjectCnt = 0;

                // printf("\nObject Count : %d\n", IbeoCache.ObjectCnt);
                break;

            case 0x502:
                // Boxsize
                IbeoCache.ObjectID = IbeoRecv.Frame.data[0]; // 트랙킹 ID 값
                // 기준 좌표계(cm)
                IbeoCache.X = (IbeoRecv.Frame.data[1] << 8) + IbeoRecv.Frame.data[2];
                IbeoCache.Y = (IbeoRecv.Frame.data[3] << 8) + IbeoRecv.Frame.data[4];
                // 기준 좌표게에서 물체속도(0.1m/s) - 0x800: an invalid veloctiy
                IbeoCache.Vx = (IbeoRecv.Frame.data[5] << 4) + ((IbeoRecv.Frame.data[6] & 0xF0) >> 4);
                IbeoCache.Vy = ((IbeoRecv.Frame.data[6] & 0x0F) << 8) + IbeoRecv.Frame.data[7];

                if (IbeoCache.Vx > 2048)
                    IbeoCache.Vx -= 4096;
                if (IbeoCache.Vy > 2048)
                    IbeoCache.Vy -= 4096;
                break;

            case 0x504:
                IbeoCache.Objectclassification = IbeoRecv.Frame.data[1];

                // 0: unclassified, 1: unknown small, 2: unknown big, 3: pedestrian, 4: bike, 5: car, 6: truck
                IbeoCache.Object[ObjectCnt * 3 + 1] = (int)IbeoCache.Objectclassification;
                IbeoCache.Object[ObjectCnt * 3 + 2] = ((double)IbeoCache.X / 100); // [m]
                IbeoCache.Object[ObjectCnt * 3 + 3] = ((double)IbeoCache.Y / 100); // [m]

                //IbeoCache.BoxCenterX = (IbeoRecv.Frame.data[4] << 8) + IbeoRecv.Frame.data[5];
                //IbeoCache.BoxCenterY = (IbeoRecv.Frame.data[6] << 8) + IbeoRecv.Frame.data[7];
                ObjectCnt += 1;

                // 1: Unknown big || 2: Unknown small || 3: 사람
                // if (IbeoCache.Objectclassification == 1 || IbeoCache.Objectclassification == 2 || IbeoCache.Objectclassification == 3)
                // {
                //     IbeoCache.Object[ObjectCnt * 3 + 1] = (int)IbeoCache.Objectclassification;
                //     IbeoCache.Object[ObjectCnt * 3 + 2] = ((double)IbeoCache.X / 100); // [m]
                //     IbeoCache.Object[ObjectCnt * 3 + 3] = ((double)IbeoCache.Y / 100); // [m]
                //     ObjectCnt += 1;
                // }
                // else
                // {
                //     IbeoCache.Object[ObjectCnt * 3 + 1] = 500;
                //     IbeoCache.Object[ObjectCnt * 3 + 2] = 500;
                //     IbeoCache.Object[ObjectCnt * 3 + 3] = 500;
                //     ObjectCnt += 1;
                // }
                break;

            case 0x505:
                // 0x8000 : an invalid orientation
                IbeoCache.BoxSizeX = (IbeoRecv.Frame.data[1] << 8) + IbeoRecv.Frame.data[2];
                IbeoCache.BoxSizeY = (IbeoRecv.Frame.data[3] << 8) + IbeoRecv.Frame.data[4];
                IbeoCache.BoxOrientation = (IbeoRecv.Frame.data[5] << 8) + IbeoRecv.Frame.data[6];

                // printf("Class : %d || Ibeo X : %.4lf || Ibeo Y : %.4lf  || ", IbeoCache.Objectclassification, ((double)IbeoCache.X / 100), ((double)IbeoCache.Y / 100));
                // printf("Box Flag : %d || Box Size X, Y : %d, %d || Box Orientation %d\n", IbeoCache.Boxflag, IbeoCache.BoxSizeX, IbeoCache.BoxSizeY, IbeoCache.BoxOrientation);

                // printf("%d\n", TimeGap);
                // gettimeofday(&startTime, NULL);
                break;
            }

            if (IbeoFlag)
            {
                Ibeo = IbeoCache;
                IbeoFlag = 0;
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<IbeoReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<IbeoReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<IbeoReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    IbeoRecv.CloseSocket();
    std::cout << "[Communicator] ------------------- IbeoReceiver Socket Closed! " << endl;
}

void GPSParser()
{
    UDPClass GPSBD, Back;
    GPSStruct GPSCache;
    VehicleStruct VehicleCache;

    GPSBD.SetSocket(BroadCastIp, S32GPort, 1); // .255, 3004
    Back.SetSocket(S32GIp, BackPort, 0);       // .99, 3862
    std::cout << "[Communicator] ------------------- GPSReceiver Thread start! " << endl;

    while (SocketFlag)
    {
        try
        {
            // GPS Raw data 저장: Receive(131) , for문, main에서 저장 파일형태 변경
            // GPSBD.Receive(131);
            GPSBD.Receive(24);
            GPSCache.Time = 0.001 * (uint32_t)((GPSBD.Buffer[8] << 24) + (GPSBD.Buffer[7] << 16) + (GPSBD.Buffer[6] << 8) + GPSBD.Buffer[5]);
            GPSCache.Latitude = 0.0000001 * (uint32_t)((GPSBD.Buffer[12] << 24) + (GPSBD.Buffer[11] << 16) + (GPSBD.Buffer[10] << 8) + GPSBD.Buffer[9]);
            GPSCache.Longitude = 0.0000001 * (uint32_t)((GPSBD.Buffer[16] << 24) + (GPSBD.Buffer[15] << 16) + (GPSBD.Buffer[14] << 8) + GPSBD.Buffer[13]);
            GPSCache.Azimuth = 0.01 * ((uint32_t)((GPSBD.Buffer[20] << 24) + (GPSBD.Buffer[19] << 16) + (GPSBD.Buffer[18] << 8) + GPSBD.Buffer[17]));
            GPSCache.State = GPSBD.Buffer[21];

            // 30~130 GPS Raw data
            /* for (uint8_t i = 0; i < 100; i++)
            {
                GPSRaw[i] = GPSBD.Buffer[i + 30];
            } */

            GPS = GPSCache;

            // TC KCITY Path 시 필요
            VehicleCache = Vehicle;
            Back.Buffer[0] = (uint32_t)(GPSCache.Time * 1000);
            Back.Buffer[1] = ((uint32_t)(GPSCache.Time * 1000)) >> 8;
            Back.Buffer[2] = ((uint32_t)(GPSCache.Time * 1000)) >> 16;
            Back.Buffer[3] = ((uint32_t)(GPSCache.Time * 1000)) >> 24;
            Back.Buffer[4] = (uint32_t)(GPSCache.Latitude * 10000000);
            Back.Buffer[5] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 8;
            Back.Buffer[6] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 16;
            Back.Buffer[7] = ((uint32_t)(GPSCache.Latitude * 10000000)) >> 24;
            Back.Buffer[8] = (uint32_t)(GPSCache.Longitude * 10000000);
            Back.Buffer[9] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 8;
            Back.Buffer[10] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 16;
            Back.Buffer[11] = ((uint32_t)(GPSCache.Longitude * 10000000)) >> 24;
            Back.Buffer[12] = (uint32_t)(GPSCache.Azimuth * 100);
            Back.Buffer[13] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 8;
            Back.Buffer[14] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 16;
            Back.Buffer[15] = ((uint32_t)(GPSCache.Azimuth * 100)) >> 24;
            Back.Buffer[16] = GPSCache.State;
            Back.Buffer[17] = (uint32_t)(VehicleCache.Velocity * 3.6 * 100);
            Back.Buffer[18] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 8;
            Back.Buffer[19] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 16;
            Back.Buffer[20] = ((uint32_t)(VehicleCache.Velocity * 3.6 * 100)) >> 24;
            Back.Buffer[21] = VehicleCache.LeftTurnSwitch;
            Back.Buffer[22] = VehicleCache.RightTurnSwitch;
            Back.Buffer[23] = 0;
            Back.Buffer[24] = 0;
            Back.Buffer[25] = 0;
            Back.Buffer[26] = 0;
            Back.Send(27);
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<GPSParser> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<GPSParser> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<GPSParser> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    GPSBD.CloseSocket();
    Back.CloseSocket();
    std::cout << "[Communicator] ------------------- GPSReceiver Socket Closed! " << endl;
}

/* KCITY 도로경로 받아오는 함수 */
void PathReceiver()
{
    UDPClass Forward;
    GlobalPathStruct GlobalCache;
    struct timeval FirstTime, SecondTime;
    Forward.SetSocket(S32GIp, ForwardPort, 1); // .99, 1785
    uint32_t RecvCnt = 0;
    double TimeGap;

    std::cout << "[Communicator] ------------------- PathReceiver Thread start! " << endl;

    while (SocketFlag)
    {
        try
        {
            uint32_t ErrorCnt = 0, TotalCnt = 0;
            gettimeofday(&FirstTime, NULL);

            Forward.Receive(BufferSize);

            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000); // [ms]
            RecvCnt++;

            memset(&GlobalCache.Latitude, 0, BufferSize);
            memset(&GlobalCache.Longitude, 0, BufferSize);

            for (uint32_t i = 0; i < BufferSize / 8; i++)
            {
                GlobalCache.Longitude[i] = 0.0000001 * (uint32_t)((Forward.Buffer[8 * i + 3] << 24) + (Forward.Buffer[8 * i + 2] << 16) + (Forward.Buffer[8 * i + 1] << 8) + Forward.Buffer[8 * i]);
                GlobalCache.Latitude[i] = 0.0000001 * (uint32_t)((Forward.Buffer[8 * i + 7] << 24) + (Forward.Buffer[8 * i + 6] << 16) + (Forward.Buffer[8 * i + 5] << 8) + Forward.Buffer[8 * i + 4]);

                if (GlobalCache.Longitude[i] == 0 || GlobalCache.Latitude[i] == 0)
                    ErrorCnt++;

                TotalCnt++;
            }
            ErrorCnt = 0;

            if (PathReceiveSignal)
            {
                Global = GlobalCache;
                PathReceiveSignal = false;
            }
        }
        catch (std::out_of_range &e)
        {
            std::cout << "<PathReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error &e)
        {
            std::cout << "<PathReceiver> Length Error" << '\n';
        }
        catch (std::exception &e)
        {
            std::cout << "<PathReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    Forward.CloseSocket();
    std::cout << "[Communicator] ------------------- PathReceiver Socket Closed!" << endl;
}

/* 보행자 데이터 수신 */
void MobileyeReceiver()
{
    /* Mobileye */
    CANClass PedestrianCANFD;
    MobileyeStruct MobileyeCache;

    // Y(종) - [cm]
    double MobileyeYFactor = 5.4054054;
    double MobileyeYOffset = -154;
    // X(횡) - [cm]
    double MobileyeXFactor = 1.3;
    double MobileyeXOffset = 0.0;

    PedestrianCANFD.SetSocket("can1", 1); // CAN FD - Mobileye: A-CAN, CAMERA: L-CAN
    std::cout << "[Communicator] ------------------- Mobileye Thread start! " << endl;

    while (SocketFlag)
    {
        try
        {
            PedestrianCANFD.ReceiveCANFD();

            switch (PedestrianCANFD.FrameFd.can_id) // 사람(차량, 자전거 등) 확인
            {
            case 0x180:
                if (PedestrianCANFD.FrameFd.data[7] == 0x50)
                {
                    MobileyeCache.Y[0] = (PedestrianCANFD.FrameFd.data[8] + ((PedestrianCANFD.FrameFd.data[9] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[0] = ((PedestrianCANFD.FrameFd.data[9] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[10] << 4);
                    if (MobileyeCache.X[0] > 2048) // 음수 확인
                        MobileyeCache.X[0] = MobileyeCache.X[0] - 4096;

                    MobileyeCache.X[0] = MobileyeCache.X[0] * MobileyeXFactor + MobileyeXOffset;

                    // [m]
                    MobileyeCache.X[0] = MobileyeCache.X[0] / 100;
                    MobileyeCache.Y[0] = MobileyeCache.Y[0] / 100; // 경로와 object 좌표값 고정
                }

                else
                {
                    MobileyeCache.X[0] = 0;
                    MobileyeCache.Y[0] = 0;
                }

                if (PedestrianCANFD.FrameFd.data[23] == 0x50)
                {
                    MobileyeCache.Y[1] = (PedestrianCANFD.FrameFd.data[24] + ((PedestrianCANFD.FrameFd.data[25] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[1] = ((PedestrianCANFD.FrameFd.data[25] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[26] << 4);
                    if (MobileyeCache.X[1] > 2048)
                        MobileyeCache.X[1] = MobileyeCache.X[1] - 4096;

                    MobileyeCache.X[1] = MobileyeCache.X[1] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[1] = MobileyeCache.X[1] / 100;
                    MobileyeCache.Y[1] = MobileyeCache.Y[1] / 100;
                }

                else
                {
                    MobileyeCache.X[1] = 0;
                    MobileyeCache.Y[1] = 0;
                }
                break;

            case 0x181:
                if (PedestrianCANFD.FrameFd.data[7] == 0x50)
                {
                    MobileyeCache.Y[2] = (PedestrianCANFD.FrameFd.data[8] + ((PedestrianCANFD.FrameFd.data[9] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[2] = ((PedestrianCANFD.FrameFd.data[9] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[10] << 4);
                    if (MobileyeCache.X[2] > 2048)
                        MobileyeCache.X[2] = MobileyeCache.X[2] - 4096;

                    MobileyeCache.X[2] = MobileyeCache.X[2] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[2] = MobileyeCache.X[2] / 100;
                    MobileyeCache.Y[2] = MobileyeCache.Y[2] / 100;
                }

                else
                {
                    MobileyeCache.X[2] = 0;
                    MobileyeCache.Y[2] = 0;
                }

                if (PedestrianCANFD.FrameFd.data[23] == 0x50)
                {
                    MobileyeCache.Y[3] = (PedestrianCANFD.FrameFd.data[24] + ((PedestrianCANFD.FrameFd.data[25] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[3] = ((PedestrianCANFD.FrameFd.data[25] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[26] << 4);
                    if (MobileyeCache.X[3] > 2048)
                        MobileyeCache.X[3] = MobileyeCache.X[3] - 4096;

                    MobileyeCache.X[3] = MobileyeCache.X[3] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[3] = MobileyeCache.X[3] / 100;
                    MobileyeCache.Y[3] = MobileyeCache.Y[3] / 100;
                }

                else
                {
                    MobileyeCache.X[3] = 0;
                    MobileyeCache.Y[3] = 0;
                }
                break;

            case 0x182:
                if (PedestrianCANFD.FrameFd.data[7] == 0x50)
                {
                    MobileyeCache.Y[4] = (PedestrianCANFD.FrameFd.data[8] + ((PedestrianCANFD.FrameFd.data[9] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[4] = ((PedestrianCANFD.FrameFd.data[9] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[10] << 4);
                    if (MobileyeCache.X[4] > 2048)
                        MobileyeCache.X[4] = MobileyeCache.X[4] - 4096;

                    MobileyeCache.X[4] = MobileyeCache.X[4] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[4] = MobileyeCache.X[4] / 100;
                    MobileyeCache.Y[4] = MobileyeCache.Y[4] / 100;
                }

                else
                {
                    MobileyeCache.X[4] = 0;
                    MobileyeCache.Y[4] = 0;
                }

                if (PedestrianCANFD.FrameFd.data[23] == 0x50)
                {
                    MobileyeCache.Y[5] = (PedestrianCANFD.FrameFd.data[24] + ((PedestrianCANFD.FrameFd.data[25] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[5] = ((PedestrianCANFD.FrameFd.data[25] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[26] << 4);
                    if (MobileyeCache.X[5] > 2048)
                        MobileyeCache.X[5] = MobileyeCache.X[5] - 4096;

                    MobileyeCache.X[5] = MobileyeCache.X[5] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[5] = MobileyeCache.X[5] / 100;
                    MobileyeCache.Y[5] = MobileyeCache.Y[5] / 100;
                }

                else
                {
                    MobileyeCache.X[5] = 0;
                    MobileyeCache.Y[5] = 0;
                }
                break;

            case 0x183:
                if (PedestrianCANFD.FrameFd.data[7] == 0x50)
                {
                    MobileyeCache.Y[6] = (PedestrianCANFD.FrameFd.data[8] + ((PedestrianCANFD.FrameFd.data[9] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[6] = ((PedestrianCANFD.FrameFd.data[9] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[10] << 4);
                    if (MobileyeCache.X[6] > 2048)
                        MobileyeCache.X[6] = MobileyeCache.X[6] - 4096;

                    MobileyeCache.X[6] = MobileyeCache.X[6] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[6] = MobileyeCache.X[6] / 100;
                    MobileyeCache.Y[6] = MobileyeCache.Y[6] / 100;
                }

                else
                {
                    MobileyeCache.X[6] = 0;
                    MobileyeCache.Y[6] = 0;
                }

                if (PedestrianCANFD.FrameFd.data[23] == 0x50)
                {
                    MobileyeCache.Y[7] = (PedestrianCANFD.FrameFd.data[24] + ((PedestrianCANFD.FrameFd.data[25] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[7] = ((PedestrianCANFD.FrameFd.data[25] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[26] << 4);
                    if (MobileyeCache.X[7] > 2048)
                        MobileyeCache.X[7] = MobileyeCache.X[7] - 4096;

                    MobileyeCache.X[7] = MobileyeCache.X[7] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[7] = MobileyeCache.X[7] / 100;
                    MobileyeCache.Y[7] = MobileyeCache.Y[7] / 100;
                }

                else
                {
                    MobileyeCache.X[7] = 0;
                    MobileyeCache.Y[7] = 0;
                }
                break;

            case 0x184:
                if (PedestrianCANFD.FrameFd.data[7] == 0x50)
                {
                    MobileyeCache.Y[8] = (PedestrianCANFD.FrameFd.data[8] + ((PedestrianCANFD.FrameFd.data[9] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[8] = ((PedestrianCANFD.FrameFd.data[9] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[10] << 4);
                    if (MobileyeCache.X[8] > 2048)
                        MobileyeCache.X[8] = MobileyeCache.X[8] - 4096;

                    MobileyeCache.X[8] = MobileyeCache.X[8] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[8] = MobileyeCache.X[8] / 100;
                    MobileyeCache.Y[8] = MobileyeCache.Y[8] / 100;
                }

                else
                {
                    MobileyeCache.X[8] = 0;
                    MobileyeCache.Y[8] = 0;
                }

                if (PedestrianCANFD.FrameFd.data[23] == 0x50)
                {
                    MobileyeCache.Y[9] = (PedestrianCANFD.FrameFd.data[24] + ((PedestrianCANFD.FrameFd.data[25] & 0x0F) << 8)) * MobileyeYFactor + MobileyeYOffset;
                    MobileyeCache.X[9] = ((PedestrianCANFD.FrameFd.data[25] & 0xF0) >> 4) + (PedestrianCANFD.FrameFd.data[26] << 4);
                    if (MobileyeCache.X[9] > 2048)
                        MobileyeCache.X[9] = MobileyeCache.X[9] - 4096;

                    MobileyeCache.X[9] = MobileyeCache.X[9] * MobileyeXFactor + MobileyeXOffset;

                    MobileyeCache.X[9] = MobileyeCache.X[9] / 100;
                    MobileyeCache.Y[9] = MobileyeCache.Y[9] / 100;
                }

                else
                {
                    MobileyeCache.X[9] = 0;
                    MobileyeCache.Y[9] = 0;
                }
                break;
            }

            if (MobileyeFlag)
            {
                Mobileye = MobileyeCache;
                MobileyeFlag = 0;
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
    std::cout << "[Communicator] ------------------- MobileyeReceiver Socket Closed! " << endl;
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
    buf.c_cc[VMIN] = 1;  // 입력을 기다리는 최소 문자 수를 1로 설정
    buf.c_cc[VTIME] = 0; // 입력을 기다리는 최대 시간을 0으로 설정

    tcsetattr(0, TCSAFLUSH, &buf); // buf 터미널 설정 변경하고 기존 입력 버퍼 삭제

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

void CANClass::InitFrame()
{
    Frame.can_id = 0x00;
    Frame.can_dlc = 8;
    for (uint8_t i = 0; i < Frame.can_dlc; i++)
    {
        Frame.data[i] = 0x00;
    }
}

void CANClass::CloseSocket()
{
    close(sock);
}

void UDPClass::SetSocket(const std::string &ip, const int port, const bool BindFlag)
{
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("<UDP> socket Open Error");

    memset(&Addr, 0x00, sizeof(Addr));
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = inet_addr(ip.c_str());
    Addr.sin_port = htons(port);

    if (BindFlag)
    {
        if (bind(sock, (struct sockaddr *)&Addr, sizeof(Addr)) < 0)
            perror("<UDP> bind Error");
    }
}

void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(Addr);
    if ((nbytes = recvfrom(sock, Buffer, buffersize, 0, (struct sockaddr *)&Addr, &addrlen)) < 0)
        perror("<UDP> Receive Error");
}

void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, Buffer, SendByte, 0, (struct sockaddr *)&Addr, sizeof(Addr)) < 0)
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