#include "PCommunicator.h"

// ------------------------------ Config --------------------------------- //
CConfigParser Configuration("./config.ini");

const bool GPSRecord = Configuration.GetBool("GPSRecord");
const string GPSRecordPath = Configuration.GetString("GPSRecordPath");

const int MainCycle = Configuration.GetInt("MainCycle");
const int S32GPort = Configuration.GetInt("S32GPort");
const string BroadCastIp = Configuration.GetString("BroadCastIp");

// ------------------------------ Communicator Function ----------------------- //
void GPSParser()
{
    UDPClass GPSBD;
    GPSStruct GPSCache;

    GPSBD.SetSocket(BroadCastIp, S32GPort, 1);
    cout << "[Communicator] ------------------- GPSReceiver Thread start! " << endl;

    while (SocketFlag)
    {
        try
        {
            GPSBD.Receive(24);
            GPSCache.Time = 0.001 * (uint32_t)((GPSBD.Buffer[8] << 24) + (GPSBD.Buffer[7] << 16) + (GPSBD.Buffer[6] << 8) + GPSBD.Buffer[5]);
            GPSCache.Latitude = 0.0000001 * (uint32_t)((GPSBD.Buffer[12] << 24) + (GPSBD.Buffer[11] << 16) + (GPSBD.Buffer[10] << 8) + GPSBD.Buffer[9]);
            GPSCache.Longitude = 0.0000001 * (uint32_t)((GPSBD.Buffer[16] << 24) + (GPSBD.Buffer[15] << 16) + (GPSBD.Buffer[14] << 8) + GPSBD.Buffer[13]);
            GPSCache.Azimuth = 0.01 * ((uint32_t)((GPSBD.Buffer[20] << 24) + (GPSBD.Buffer[19] << 16) + (GPSBD.Buffer[18] << 8) + GPSBD.Buffer[17]));
            GPSCache.State = GPSBD.Buffer[21];

            GPS = GPSCache;
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
    cout << "[Communicator] ------------------- GPSReceiver Socket Closed! " << endl;
}

/* Key 입력 */
int getch(void)
{
    int ch;
    struct termios buf;
    struct termios save;

    tcgetattr(0, &save);
    buf = save;
    buf.c_lflag &= ~(ICANON | ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;

    tcsetattr(0, TCSAFLUSH, &buf);

    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);

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
    addrlen = sizeof(ClientAddr);
    if ((nbytes = recvfrom(sock, Buffer, buffersize, 0, (struct sockaddr *)&ClientAddr, &addrlen)) < 0)
        perror("<UDP> Receive Error");
}

void UDPClass::CloseSocket()
{
    close(sock);
}
