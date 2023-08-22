#include "Communicator.h"

// ------------------------------ Config --------------------------------- //
CConfigParser Configuration("./config.ini");

const bool GPSRecord = Configuration.GetBool("GPSRecord");
const string GPSRecordPath = Configuration.GetString("GPSRecordPath");

const string S32GIp = Configuration.GetString("S32GIp");
const int S32GPort = Configuration.GetInt("S32GPort");

// ------------------------------ Communicator Function ----------------------- //
void GPSReceiver()
{
    UDPClass GPSBD;
    GPSStruct GPSCache;
    GPSBD.SetServerSocket("192.168.100.255", S32GPort); // S32G
    struct timeval FirstTime, SecondTime;
    double TimeGap;

    cout<<"[Communicator]-----------GPSReceiver Thread start!-------------"<<endl;
    gettimeofday(&FirstTime, NULL);
    while (1)
    {
        try
        {
            gettimeofday(&SecondTime, NULL);
            TimeGap = (SecondTime.tv_sec - FirstTime.tv_sec) * 1000 + ((SecondTime.tv_usec - FirstTime.tv_usec) / 1000);
            if(TimeGap >= 50)
            {
                GPSBD.Receive(24);
                GPSCache.Time = 0.001*(uint32_t)((GPSBD.ReceiveBuffer[8]<<24)+(GPSBD.ReceiveBuffer[7]<<16)+(GPSBD.ReceiveBuffer[6]<<8)+GPSBD.ReceiveBuffer[5]);
                GPSCache.Latitude = 0.0000001*(uint32_t)((GPSBD.ReceiveBuffer[12]<<24)+(GPSBD.ReceiveBuffer[11]<<16)+(GPSBD.ReceiveBuffer[10]<<8)+GPSBD.ReceiveBuffer[9]);
                GPSCache.Longitude = 0.0000001*(uint32_t)((GPSBD.ReceiveBuffer[16]<<24)+(GPSBD.ReceiveBuffer[15]<<16)+(GPSBD.ReceiveBuffer[14]<<8)+GPSBD.ReceiveBuffer[13]);
                GPSCache.Azimuth = 0.01*((uint32_t)((GPSBD.ReceiveBuffer[20]<<24)+(GPSBD.ReceiveBuffer[19]<<16)+(GPSBD.ReceiveBuffer[18]<<8)+GPSBD.ReceiveBuffer[17]));
                GPSCache.State = GPSBD.ReceiveBuffer[21];

                GPS = GPSCache;
                gettimeofday(&FirstTime, NULL);
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<GPSReceiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<GPSReceiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<GPSReceiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    GPSBD.CloseSocket();
    cout<<"[Communicator]-----------GPSReceiver Socket Closed!-------------"<<endl;
}


// ------------------------------- class Function ------------------------------------------- //
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

void CANClass::SendCAN()
{
    if (write(sock, &Frame, sizeof(Frame)) != sizeof(struct can_frame)) perror("<CANFD> Send Error");
}

void CANClass::CloseSocket()
{
    close(sock);
}

void UDPClass::SetServerSocket(const std::string& ip, const uint16_t port)
{
    /* 소켓 생성 */
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) perror("<UDP> socket Open Error");

    /* 소켓 주소 구조체 설정 */
    memset(&ServerAddr, 0x00, sizeof(ServerAddr));
    ServerAddr.sin_family = AF_INET; // AF_INET(IP용 Address Family), IPv4 주소 체계 사용
    ServerAddr.sin_addr.s_addr = inet_addr(ip.c_str()); // ip 네트워크 인터페이스로부터 수신
    ServerAddr.sin_port = htons(port); // htons: host-to-network short, 포트 번호 설정

    /* 소켓 바인딩 */
    if (bind(sock, (struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0) perror("<UDP> bind Error");
}

void UDPClass::SetClientSocket(const std::string& ip, const uint16_t port)
{
    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    ClientAddr.sin_port = htons(port);
}

void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(ClientAddr);
    if ((nbytes = recvfrom(sock, ReceiveBuffer, buffersize, 0, (struct sockaddr *)&ClientAddr, &addrlen)) < 0) perror("<UDP> Receive Error");
}

void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, SendBuffer, SendByte, 0, (struct sockaddr *)&ClientAddr, sizeof(ClientAddr)) < 0) perror("<UDP> Send Error");
}

void UDPClass::CloseSocket()
{
    close(sock);
}
