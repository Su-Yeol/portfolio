#include "../../common/include/modules/remote-control/remote.h"

const std::string LTEIp = "10.167.216.215";
const int LTEPort = 5200;

/**
 * @brief Short description of `main`.
 *
 * Detailed explanation of what `main` does,
 * including major steps and responsibilities.
 *
 * @param argc Input parameter.
 * @param argv Input parameter.
 * @return Output value from the function.
 */
int main(int argc, const char *argv[])
{
    (void)(argc);
    (void)(argv);

    UDPClass s32g;
    s32g.Buffer[0] = 0;
    s32g.Buffer[1] = 1;
    s32g.Buffer[2] = 2;
    s32g.Buffer[3] = 3;

    s32g.SetSocket(LTEIp, LTEPort);
    printf("Send UDP\n");
    while (true)
    {
        try
        {
            s32g.Send(4);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }

    s32g.CloseSocket();
    return 0;
}

/**
 * @brief Short description of `CANClass::SetSocket`.
 *
 * Detailed explanation of what `CANClass::SetSocket` does,
 * including major steps and responsibilities.
 *
 * @param ifname Input parameter.
 * @param canfd Input parameter.
 * @return void
 */
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
    if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) < 0)
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

/**
 * @brief Short description of `CANClass::RecieveCANFD`.
 *
 * Detailed explanation of what `CANClass::RecieveCANFD` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::RecieveCANFD()
{
    addrlen = sizeof(addr);
    if ((nbytes = recvfrom(sock, &FrameFd, sizeof(FrameFd), 0, reinterpret_cast<struct sockaddr *>(&addr), &addrlen)) < 0)
        perror("<CANFD> Read Error");
}

/**
 * @brief Short description of `CANClass::SendCANFD`.
 *
 * Detailed explanation of what `CANClass::SendCANFD` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::SendCANFD()
{
    if (write(sock, &FrameFd, sizeof(FrameFd)) != sizeof(struct canfd_frame))
        perror("<CANFD> Send Error");
}

/**
 * @brief Short description of `CANClass::CloseSocket`.
 *
 * Detailed explanation of what `CANClass::CloseSocket` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void CANClass::CloseSocket()
{
    close(sock);
}

/**
 * @brief Short description of `UDPClass::SetSocket`.
 *
 * Detailed explanation of what `UDPClass::SetSocket` does,
 * including major steps and responsibilities.
 *
 * @param ip Input parameter.
 * @param port Input parameter.
 * @return void
 */
void UDPClass::SetSocket(const std::string &ip, const int port)
{
    (void)ip;  // Receiver binds to INADDR_ANY, ip unused
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("<UDP> socket Open Error");

    memset(&Addr, 0, sizeof(Addr));
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = htonl(INADDR_ANY);
    Addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(sock, reinterpret_cast<struct sockaddr *>(&Addr), sizeof(Addr)) < 0)
        perror("<UDP> bind Error");
}

/**
 * @brief Short description of `UDPClass::Receive`.
 *
 * Detailed explanation of what `UDPClass::Receive` does,
 * including major steps and responsibilities.
 *
 * @param buffersize Input parameter.
 * @return void
 */
void UDPClass::Receive(const uint16_t buffersize)
{
    addrlen = sizeof(Addr);
    if ((nbytes = recvfrom(sock, Buffer, buffersize, 0, reinterpret_cast<struct sockaddr *>(&Addr), &addrlen)) < 0)
        perror("<UDP> Receive Error");
}

/**
 * @brief Short description of `UDPClass::Send`.
 *
 * Detailed explanation of what `UDPClass::Send` does,
 * including major steps and responsibilities.
 *
 * @param SendByte Input parameter.
 * @return void
 */
void UDPClass::Send(const uint16_t SendByte)
{
    if (sendto(sock, Buffer, SendByte, 0, reinterpret_cast<struct sockaddr *>(&Addr), sizeof(Addr)) < 0)
        perror("<UDP> Send Error");
}

/**
 * @brief Short description of `UDPClass::CloseSocket`.
 *
 * Detailed explanation of what `UDPClass::CloseSocket` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
void UDPClass::CloseSocket()
{
    close(sock);
}
