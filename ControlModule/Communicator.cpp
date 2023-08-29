#include "ControlModule/include/Communicator.h"

// ------------------------------ Config --------------------------------- //
CConfigParser Configuration("./config.ini");

const int TargetSpeed = Configuration.GetInt("TargetSpeed"); // [kph]

const bool GPSRecord = Configuration.GetBool("GPSRecord");
const string GPSRecordPath = Configuration.GetString("GPSRecordPath");
const bool PathRecord = Configuration.GetBool("PathRecord");
const string PathRecordPath = Configuration.GetString("PathRecordPath");
const bool VehicleRecord = Configuration.GetBool("VehicleRecord");
const string VehicleRecordPath = Configuration.GetString("VehicleRecordPath");
const bool ControlRecord = Configuration.GetBool("ControlRecord");
const string ControlRecordPath = Configuration.GetString("ControlRecordPath");

const bool ReceivePathFlag = Configuration.GetBool("ReceivePathFlag");
const string ReferenceFile = Configuration.GetString("ReferenceFile");
const string ExternalIp = Configuration.GetString("ExternalIp");
const int ForwardPort = Configuration.GetInt("ForwardPort");
const int BackPort = Configuration.GetInt("BackPort");

const int MainCycle = Configuration.GetInt("MainCycle");

const string S32GIp = Configuration.GetString("S32GIp");
const string BroadCastIp = Configuration.GetString("BroadCastIp");
const int GPSPort = Configuration.GetInt("GPSPort");
const string MCUIp = Configuration.GetString("MCUIp");
const int MCUPort = Configuration.GetInt("MCUPort");

const bool ViewerFlag = Configuration.GetBool("ViewerFlag");
const string ViewerIp = Configuration.GetString("ViewerIp");
const int ViewerPort = Configuration.GetInt("ViewerPort");


// ------------------------------ Communicator Function ----------------------- //
void VehicleReceiver()
{
    // local //
    double WheelSpeedFL, WheelSpeedFR;
    VehicleVariable VehicleCache;
    CANClass VehicleCANFD;
    CANClass IbeoSend;
    VehicleCANFD.SetSocket("can0", 1);
    IbeoSend.SetSocket("can1", 0);
    float IbeoVehicleSpeed, IbeoSteerAngle, IbeoYawRate;
    cout<<"[Communicator] ------------------- VehicleReceiver Thread start! "<<endl;
    while (SocketFlag)
    {
        try
        {
            VehicleCANFD.ReceiveCANFD();

            switch (VehicleCANFD.FrameFd.can_id)
            {
                case 0x4A:
                    VehicleCache.YawRate = ((VehicleCANFD.FrameFd.data[9]<<8) + VehicleCANFD.FrameFd.data[8]) * 0.005 - 163.84; // [deg/s]
                    VehicleCache.LateralAccel = ((VehicleCANFD.FrameFd.data[11]<<8) + VehicleCANFD.FrameFd.data[10]) * 0.000127465 - 4.17677312; // [g]
                    VehicleCache.LongitudinalAccel = ((VehicleCANFD.FrameFd.data[13]<<8) + VehicleCANFD.FrameFd.data[12]) * 0.000127465 - 4.17677312; // [g]

                    IbeoSend.InitFrame();
                    IbeoYawRate =  (VehicleCache.YawRate + 0.714700) / 0.000174;
                    IbeoSend.Frame.can_id = 0x220;
                    IbeoSend.Frame.can_dlc = 7;
                    IbeoSend.Frame.data[5] = (int16_t)(IbeoVehicleSpeed);
                    IbeoSend.Frame.data[6] = ((int16_t)(IbeoVehicleSpeed) >> 8)&0x3F;
                    IbeoSend.SendCAN();                    
                    break;

                case 0xA0:
                    WheelSpeedFL = (((VehicleCANFD.FrameFd.data[9]&0x3F)<<8) + VehicleCANFD.FrameFd.data[8])*0.03125; // [kph]
                    WheelSpeedFR = (((VehicleCANFD.FrameFd.data[11]&0x3F)<<8) + VehicleCANFD.FrameFd.data[10])*0.03125; // [kph]
                    VehicleCache.Velocity = (WheelSpeedFL+WheelSpeedFR)/(2. * 3.6); // [m/s]
                    
                    IbeoSend.InitFrame();
                    IbeoVehicleSpeed = (VehicleCache.Velocity * 3.6) / 0.138889; // [kph]
                    IbeoSend.Frame.can_id = 0x4F1;
                    IbeoSend.Frame.can_dlc = 7;
                    IbeoSend.Frame.data[1] = (uint16_t)(IbeoVehicleSpeed);
                    IbeoSend.Frame.data[2] = (uint16_t)(IbeoVehicleSpeed) >> 8;
                    IbeoSend.SendCAN();
                    break;

                case 0x125:
                    VehicleCache.HandleAngle = ((VehicleCANFD.FrameFd.data[4]<<8) + VehicleCANFD.FrameFd.data[3])*0.1; // [deg]
                    VehicleCache.HandleSpd = (VehicleCANFD.FrameFd.data[5])*4; // [deg/s]

                    IbeoSend.InitFrame();
                    IbeoSteerAngle = VehicleCache.HandleAngle / 0.001745;
                    IbeoSend.Frame.can_id = 0x2B0;
                    IbeoSend.Frame.can_dlc = 7;
                    IbeoSend.Frame.data[0] = (int16_t)(IbeoSteerAngle);
                    IbeoSend.Frame.data[1] = (int16_t)(IbeoSteerAngle) >> 8;
                    IbeoSend.SendCAN();               
                    break;

                case 0x1A0:
                    VehicleCache.Radar.Distance = (((VehicleCANFD.FrameFd.data[4]&0x07)<<8)+VehicleCANFD.FrameFd.data[3])*0.1;
                    VehicleCache.Radar.RelativeVelocity = (((VehicleCANFD.FrameFd.data[5]&0x7F)<<5)+((VehicleCANFD.FrameFd.data[4]&0xF8)>>3))*0.1 -  170;
                    break;

                case 0x1AA:
                    VehicleCache.ClusterVelocity = (((VehicleCANFD.FrameFd.data[9]&0x01)<<8) + VehicleCANFD.FrameFd.data[8]);
                    break;

                case 0x3C1:
                    VehicleCache.LeftTurnSwitch = (VehicleCANFD.FrameFd.data[3]&0xC0)>>6;
                    VehicleCache.RightTurnSwitch = VehicleCANFD.FrameFd.data[4]&0x03;

                case 0xEA:
                    VehicleCache.MDPSmode = VehicleCANFD.FrameFd.data[5]&0x0F;
            }
            Vehicle = VehicleCache;
        }
        catch (std::out_of_range& e) {
            std::cout << "<VehicleReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<VehicleReceiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<VehicleReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    VehicleCANFD.CloseSocket();
    IbeoSend.CloseSocket();
    cout<<"[Communicator] ------------------- VehicleReceiver Socket Closed! "<<endl;
}


void IbeoReceiver()
{
    CANClass IbeoRecv;
    VehicleVariable VehicleCache;
    IbeoRecv.SetSocket("can1", 0);
    uint8_t ObjectCnt = 0;
    
    cout<<"[Communicator] ------------------- IbeoReceiver Thread start! "<<endl;
    while(SocketFlag)
    {
        try
        {
            //printf("%x\n", IbeoRecv.Frame.can_id);
            IbeoRecv.ReceiveCAN();
            switch (IbeoRecv.Frame.can_id)
            {
                case 0x500:
                    VehicleCache.Ibeo.Object[0] = ObjectCnt;
                    //최신화 타이밍
                    Vehicle.Ibeo = VehicleCache.Ibeo;
                    // printf("%d\n", ObjectCnt);
                    ObjectCnt = 0;
                    VehicleCache.Ibeo.Object[0] = (int)(IbeoRecv.Frame.data[1]);
                    break;
                
                case 0x502:
                    VehicleCache.Ibeo.ObjectID = IbeoRecv.Frame.data[0];
                    VehicleCache.Ibeo.X = (IbeoRecv.Frame.data[1]<<8) + IbeoRecv.Frame.data[2];
                    VehicleCache.Ibeo.Y = (IbeoRecv.Frame.data[3]<<8) + IbeoRecv.Frame.data[4];
                    VehicleCache.Ibeo.Vx = (IbeoRecv.Frame.data[5]<<4) + ((IbeoRecv.Frame.data[6]&0xF0)>>4);
                    VehicleCache.Ibeo.Vy = ((IbeoRecv.Frame.data[6]&0x0F)<<8) + IbeoRecv.Frame.data[7];

                    if(VehicleCache.Ibeo.Vx > 2048) VehicleCache.Ibeo.Vx -= 4096;
                    if(VehicleCache.Ibeo.Vy > 2048) VehicleCache.Ibeo.Vy -= 4096;
                    break;
                
                case 0x504:
                    //VehicleCache.Ibeo.ObjectID4 = IbeoRecv.Frame.data[0];
                    VehicleCache.Ibeo.Objectclassification = IbeoRecv.Frame.data[1];
                    VehicleCache.Ibeo.Object[ObjectCnt*3+1] = (int)VehicleCache.Ibeo.Objectclassification;
                    VehicleCache.Ibeo.Object[ObjectCnt*3+2] = (int)VehicleCache.Ibeo.X;
                    VehicleCache.Ibeo.Object[ObjectCnt*3+3] = (int)VehicleCache.Ibeo.Y;
                    ObjectCnt += 1;
                    // printf(" class : %d, position x : %d, y :%d", VehicleCache.Ibeo.Objectclassification, VehicleCache.Ibeo.X,VehicleCache.Ibeo.Y);
                    // printf("\n");
                    break;
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<IbeoReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<IbeoReceiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<IbeoReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    IbeoRecv.CloseSocket();
    cout<<"[Communicator] ------------------- IbeoReceiver Socket Closed! "<<endl;
}


void GPSParser()
{
    UDPClass GPSBD, Back;
    GPSVariable GPSCache;
    VehicleVariable VehicleCache;

    GPSBD.SetSocket(BroadCastIp, GPSPort, 1);
    Back.SetSocket(S32GIp, BackPort, 0);
    cout<<"[Communicator] ------------------- GPSReceiver Thread start! "<<endl;
    while (SocketFlag)
    {
        try
        {
            GPSBD.Receive(24);
            GPSCache.Time = 0.001*(uint32_t)((GPSBD.Buffer[8]<<24)+(GPSBD.Buffer[7]<<16)+(GPSBD.Buffer[6]<<8)+GPSBD.Buffer[5]);
            GPSCache.Latitude = 0.0000001*(uint32_t)((GPSBD.Buffer[12]<<24)+(GPSBD.Buffer[11]<<16)+(GPSBD.Buffer[10]<<8)+GPSBD.Buffer[9]);
            GPSCache.Longitude = 0.0000001*(uint32_t)((GPSBD.Buffer[16]<<24)+(GPSBD.Buffer[15]<<16)+(GPSBD.Buffer[14]<<8)+GPSBD.Buffer[13]);
            GPSCache.Azimuth = 0.01*((uint32_t)((GPSBD.Buffer[20]<<24)+(GPSBD.Buffer[19]<<16)+(GPSBD.Buffer[18]<<8)+GPSBD.Buffer[17]));
            GPSCache.State = GPSBD.Buffer[21];

            GPS = GPSCache;
            VehicleCache = Vehicle;
            Back.Buffer[0] = (uint32_t)(GPSCache.Time*1000);
            Back.Buffer[1] = ((uint32_t)(GPSCache.Time*1000))>>8;
            Back.Buffer[2] = ((uint32_t)(GPSCache.Time*1000))>>16;
            Back.Buffer[3] = ((uint32_t)(GPSCache.Time*1000))>>24;
            Back.Buffer[4] = (uint32_t)(GPSCache.Latitude*10000000);
            Back.Buffer[5] = ((uint32_t)(GPSCache.Latitude*10000000))>>8;
            Back.Buffer[6] = ((uint32_t)(GPSCache.Latitude*10000000))>>16;
            Back.Buffer[7] = ((uint32_t)(GPSCache.Latitude*10000000))>>24;
            Back.Buffer[8] = (uint32_t)(GPSCache.Longitude*10000000);
            Back.Buffer[9] = ((uint32_t)(GPSCache.Longitude*10000000))>>8;
            Back.Buffer[10] = ((uint32_t)(GPSCache.Longitude*10000000))>>16;
            Back.Buffer[11] = ((uint32_t)(GPSCache.Longitude*10000000))>>24;
            Back.Buffer[12] = (uint32_t)(GPSCache.Azimuth*100);
            Back.Buffer[13] = ((uint32_t)(GPSCache.Azimuth*100))>>8;
            Back.Buffer[14] = ((uint32_t)(GPSCache.Azimuth*100))>>16;
            Back.Buffer[15] = ((uint32_t)(GPSCache.Azimuth*100))>>24;
            Back.Buffer[16] = GPSCache.State;
            Back.Buffer[17] = (uint32_t)(VehicleCache.Velocity*3.6*100);
            Back.Buffer[18] = ((uint32_t)(VehicleCache.Velocity*3.6*100))>>8;
            Back.Buffer[19] = ((uint32_t)(VehicleCache.Velocity*3.6*100))>>16;
            Back.Buffer[20] = ((uint32_t)(VehicleCache.Velocity*3.6*100))>>24;
            Back.Buffer[21] = VehicleCache.LeftTurnSwitch;
            Back.Buffer[22] = VehicleCache.RightTurnSwitch;
            Back.Send(23);
        }
        catch (std::out_of_range& e) {
            std::cout << "<GPSParser> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<GPSParser> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<GPSParser> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    GPSBD.CloseSocket();
    Back.CloseSocket();
    cout<<"[Communicator] ------------------- GPSReceiver Socket Closed! "<<endl;
}

void MCUSender()
{
    UDPClass MCU;
    MCU.SetSocket(MCUIp, MCUPort, 0);
    uint8_t AliveCnt = 0;
    int handle, Ax;
    cout<<"[Communicator] ------------------- MCUSender Thread start! "<<endl; 
    while (SocketFlag)
    {   
        try
        {
            if (MCUSendSignal)
            {
                //handle = (int)(Control.MPCHandle * 10. + 3600.);
                handle = (int)(Control.Handle * 10. + 3600.);
                Ax = (int)Control.Acceleration;

                //old version
                MCU.Buffer[0] = 0x01;
                MCU.Buffer[1] = 0x02;
                MCU.Buffer[2] = 0x03;
                MCU.Buffer[3] = 0x04;
                MCU.Buffer[4] = AliveCnt;
                MCU.Buffer[5] = AliveCnt>>8;
                MCU.Buffer[6] = 0;
                MCU.Buffer[7] = 0;
                MCU.Buffer[8] = 0;
                MCU.Buffer[9] = 0;
                MCU.Buffer[10] = 0;
                MCU.Buffer[11] = 0;
                MCU.Buffer[12] = TargetSpeed;
                MCU.Buffer[15] = 0; // PathFollower
                MCU.Buffer[16] = handle;
                MCU.Buffer[17] = handle >> 8;
                MCU.Buffer[18] = handle >> 16;
                MCU.Buffer[19] = handle >> 24;
                MCU.Buffer[20] = Ax;
                MCU.Buffer[21] = Ax >> 8;
                MCU.Send(22);
                // 주행주인 차선의 정지선까지 거리. 자동차전용도로 같이, 정지선이 멀면 맥스값으로 주기.  ex) 60m.항상 나오고 있어야 함.
                // 깜빡이를 언제 넣을지. (좌회전 할 건지, 우회전 할 건지, 경로 내 차선변경 포함) bool 값. // 1이 유지되는 공안 깜빡이 넣는다.
                // 우회전 정지. 
                // TargetSpeed. 어린이보호구역(30,30), 도심로(60,30), 자동차전용도로(60,60)에 따라 바뀌어야 함. 클러스터에 띄우는 속도랑, 실제 타겟스피드랑 다르게.

                //new verison
                // MCU.Buffer[0] = 4;
                // MCU.Buffer[1] = 9;
                // MCU.Buffer[2] = 1;
                // MCU.Buffer[3] = 2;
                // MCU.Buffer[4] = AliveCnt;
                // MCU.Buffer[5] = AliveCnt>>8;
                // MCU.Buffer[6] = TargetSpeed;
                // MCU.Buffer[7] = handle;
                // MCU.Buffer[8] = handle>>8;
                // MCU.Buffer[9] = handle>>16;
                // MCU.Buffer[10] = handle>>24;

                // for(uint8_t i=0; i<100; i++)
                // {
                //     MCU.Buffer[i+11] = Vehicle.Ibeo.Object[i];
                // }

                // MCU.Send(111);

                // if (AliveCnt == 255) AliveCnt = 0;
                // else AliveCnt++;

                MCUSendSignal = false;
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<MCUSender> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<MCUSender> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<MCUSender> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    MCU.CloseSocket();
    cout<<"[Communicator] ------------------- MCUSender Socket Closed! "<<endl;
}


void PathReceiver()
{
    UDPClass Forward;
    GlobalPathVariable GlobalCache;
    struct timeval FirstTime, SecondTime;
    Forward.SetSocket(S32GIp, ForwardPort, 1);
    uint32_t RecvCnt = 0;
    double TimeGap;


    cout<<"[Communicator] ------------------- PathReceiver Thread start! "<<endl;
    while(SocketFlag)
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
            for(uint32_t i=0; i<BufferSize/8; i++)
            {
                GlobalCache.Longitude[i] = 0.0000001*(uint32_t)((Forward.Buffer[8*i+3]<<24)+(Forward.Buffer[8*i+2]<<16)+(Forward.Buffer[8*i+1]<<8)+Forward.Buffer[8*i]);
                GlobalCache.Latitude[i] = 0.0000001*(uint32_t)((Forward.Buffer[8*i+7]<<24)+(Forward.Buffer[8*i+6]<<16)+(Forward.Buffer[8*i+5]<<8)+Forward.Buffer[8*i+4]);
                if (GlobalCache.Longitude[i] == 0 || GlobalCache.Latitude[i] == 0) ErrorCnt++;
                TotalCnt++;
            }
            //cout<<"[Communicator/PathReceiver]  ReceiveFromCNT:"<<RecvCnt<<" Time:"<<(int)TimeGap<<"  PathVertex Totalnum:"<<TotalCnt<<"  PathVertex ErrorCnt:"<<ErrorCnt<<endl;
            ErrorCnt = 0;
            if (PathReceiveSignal)
            {
                Global = GlobalCache;
                PathReceiveSignal = false;
            }
            
        }
        catch (std::out_of_range& e) {
            std::cout << "<PathReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<PathReceiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<PathReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    Forward.CloseSocket();
    cout<<"[Communicator] ------------------- PathReceiver Socket Closed!"<<endl;
}


void ViewerSender()
{
    UDPClass Viewer;
    Viewer.SetSocket(ViewerIp, ViewerPort, 0);
    cout<<"[Communicator] ------------------- ViewerSender Thread start! "<<endl;
    
    while(SocketFlag)
    {
        try
        {
            if (ViewerSendSignal)
            {
                //Path
                for (uint32_t i=0; i<120; i++)
                {
                    Viewer.Buffer[8*i] = (uint32_t)(Local.X[i]);
                    Viewer.Buffer[8*i+1] = ((uint32_t)(Local.X[i]))>>8;
                    Viewer.Buffer[8*i+2] = ((uint32_t)(Local.X[i]))>>16;
                    Viewer.Buffer[8*i+3] = ((uint32_t)(Local.X[i]))>>24;
                    Viewer.Buffer[8*i+4] = (uint32_t)(Local.Y[i]);
                    Viewer.Buffer[8*i+5] = ((uint32_t)(Local.Y[i]))>>8;
                    Viewer.Buffer[8*i+6] = ((uint32_t)(Local.Y[i]))>>16;
                    Viewer.Buffer[8*i+7] = ((uint32_t)(Local.Y[i]))>>24;                
                }

                //Vehicle
                Viewer.Buffer[960] = (int)(Vehicle.Velocity*3.6);
                Viewer.Buffer[961] = (int)(Vehicle.LongitudinalAccel*100);
                Viewer.Buffer[962] = ((int)(Vehicle.LongitudinalAccel*100))>>8;
                Viewer.Buffer[963] = (int)(Vehicle.LateralAccel*100);
                Viewer.Buffer[964] = ((int)(Vehicle.LateralAccel*100))>>8;
                Viewer.Buffer[965] = (int)(Vehicle.YawRate*100);
                Viewer.Buffer[966] = ((int)(Vehicle.YawRate*100))>>8;
                Viewer.Buffer[967] = (uint32_t)(Vehicle.HandleAngle);
                Viewer.Buffer[968] = ((uint32_t)(Vehicle.HandleAngle))>>8;
                Viewer.Buffer[969] = ((uint32_t)(Vehicle.HandleAngle))>>16;
                Viewer.Buffer[970] = ((uint32_t)(Vehicle.HandleAngle))>>24;

                //Control Input
                Viewer.Buffer[971] = (uint32_t)(Control.Curvature*10000);
                Viewer.Buffer[972] = ((uint32_t)(Control.Curvature*10000))>>8;
                Viewer.Buffer[973] = ((uint32_t)(Control.Curvature*10000))>>16;
                Viewer.Buffer[974] = ((uint32_t)(Control.Curvature*10000))>>24;
                Viewer.Buffer[975] = (int)(Control.LateralDeviation*100);
                Viewer.Buffer[976] = ((int)(Control.LateralDeviation*100))>>8;
                Viewer.Buffer[977] = (int)(Control.RelativeHeadingAngle*100);
                Viewer.Buffer[978] = ((int)(Control.RelativeHeadingAngle*100))>>8;

                //Control Output
                Viewer.Buffer[979] = (uint32_t)(Control.Handle*100);
                Viewer.Buffer[32] = ((uint32_t)(Control.Handle*100))>>8;
                Viewer.Buffer[33] = ((uint32_t)(Control.Handle*100))>>16;
                Viewer.Buffer[34] = ((uint32_t)(Control.Handle*100))>>24;
                Viewer.Buffer[35] = (int)(Control.Acceleration*100);
                Viewer.Buffer[36] = ((int)(Control.Acceleration*100))>>8;

                Viewer.Send(1024);

                ViewerSendSignal = false;
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<ViewerSender> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<ViewerSender> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<ViewerSender> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    Viewer.CloseSocket();
    cout<<"[Communicator] ------------------- ViewerSender Socket Closed! "<<endl;
}


int getch(void)  
{  
  int ch;  
  struct termios buf;  
  struct termios save;  
  
   tcgetattr(0, &save);  
   buf = save;  
   buf.c_lflag &= ~(ICANON|ECHO);  
   buf.c_cc[VMIN] = 1;  
   buf.c_cc[VTIME] = 0;  
   tcsetattr(0, TCSAFLUSH, &buf);  
   ch = getchar();  
   tcsetattr(0, TCSAFLUSH, &save);  
   return ch;  
}

void Key()
{
    try {
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
    catch (std::out_of_range& e) {
        std::cout << "<Key> Out_of_range Error" << '\n';
    }
    catch (std::length_error& e) {
        std::cout << "<Key> Length Error" << '\n';
    }
    catch(std::exception& e){
        std::cout << "<Key> EXCEPTION " << '\n';
        std::cout << e.what() << '\n';
    }
}




// ------------------------------- class Function ------------------------------------------- //
void CANClass::SetSocket(const std::string& ifname, const int canfd)
{
    if((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1) perror("<CAN> socket open error");
    
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
    if (nbytes < 0) perror("<CANFD> Read Error");
}

void CANClass::ReceiveCAN()
{
    addrlen = sizeof(addr);
    nbytes = recvfrom(sock, &Frame, sizeof(Frame), 0, (struct sockaddr *)&addr, &addrlen);
    if (nbytes < 0) perror("<CAN> Read Error");
}

void CANClass::SendCANFD()
{
    if (write(sock, &FrameFd, sizeof(FrameFd)) != sizeof(struct canfd_frame)) perror("<CANFD> Send Error");
}

void CANClass::SendCAN()
{
    if (write(sock, &Frame, sizeof(Frame)) != sizeof(struct can_frame)) perror("<CAN> Send Error");
}

void CANClass::InitFrame()
{
    Frame.can_id = 0x00;
    Frame.can_dlc = 8;
    for (uint8_t i=0; i<Frame.can_dlc; i++)
    {
        Frame.data[i] = 0x00;
    }
}

void CANClass::CloseSocket()
{
    close(sock);
}

void UDPClass::SetSocket(const std::string& ip, const int port, const bool BindFlag)
{
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) perror("<UDP> socket Open Error");

    memset(&Addr, 0x00, sizeof(Addr));
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = inet_addr(ip.c_str());
    Addr.sin_port = htons(port);

    if (BindFlag)
    {
        if (bind(sock, (struct sockaddr *)&Addr, sizeof(Addr)) < 0) perror("<UDP> bind Error");
    }
}

void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(JunkAddr);
    if ((nbytes = recvfrom(sock, Buffer, buffersize, 0, (struct sockaddr *)&JunkAddr, &addrlen)) < 0) perror("<UDP> Receive Error");
}

void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, Buffer, SendByte, 0, (struct sockaddr *)&Addr, sizeof(Addr)) < 0) perror("<UDP> Send Error");
}

void UDPClass::CloseSocket()
{
    close(sock);
}

void TCPClass::SetServerSocket(const std::string& ip, const uint16_t port)
{
    if((ServerSock = socket(AF_INET, SOCK_STREAM, 0)) == 0) perror("<TCP> socket Open Error");

    memset(&ServerAddr, 0, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    ServerAddr.sin_port = htons(port);
    
    if (bind(ServerSock, (struct sockaddr*)&ServerAddr, sizeof(ServerAddr))<0) perror("<TCP> Bind Error");

    if (listen(ServerSock, 5)<0) perror("<TCP> Listen Error");

    ClientAddrSize = sizeof(ClientAddr);
    if ((ClientSock = accept(ServerSock, (struct sockaddr*)&ClientAddr, &ClientAddrSize))<0) perror("<TCP> Accept Error");
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