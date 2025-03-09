/*
 * remoteClient.cpp
 *
 *  Modified on: 2025. 03. 08
 *  Author: sy.kim
 *  Server
 *      name: katech-networkap
 *      pwd: F4*U2uPuP+Tn2e
 *      IP : 223.130.136.21
 */

#include "remote.h"

const std::string LTEIp = "223.130.136.21";
const int LTEPort = 5200;

int main(int argc, const char *argv[])
{
    (void)(argc);
    (void)(argv);
    
    UDPClass client;
    client.SetSocket(LTEIp, LTEPort);
    printf("Wait to receive data\n");
    
    while(true)
    {
        try
        {
            client.Receive(4);
            for(int i=0; i<4; i++)
            {
                printf("Buffer: %d ", client.Buffer[i]);
            }
            printf("\n");

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
    }

    client.CloseSocket();

    return 0;
}

/**************************************************************************************************
 *                                           CAN FUNCTION
 **************************************************************************************************/
void CANClass::SetSocket(const std::string &ifname, const int canfd)
{
    // socket generate
    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) == -1)
        perror("<CAN> socket open error");

    // bind
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

    // can(fd) frame activate
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

void CANClass::RecieveCANFD()
{
    addrlen = sizeof(addr);
    if ((nbytes = recvfrom(sock, &FrameFd, sizeof(FrameFd), 0, (struct sockaddr *)&addr, &addrlen)) < 0)
        perror("<CANFD> Read Error");
}

void CANClass::SendCANFD()
{
    if (write(sock, &FrameFd, sizeof(FrameFd)) != sizeof(struct canfd_frame))
        perror("<CANFD> Send Error");
}

void CANClass::CloseSocket()
{
    close(sock);
}

/**************************************************************************************************
 *                                           UDP FUNCTION
 **************************************************************************************************/
void UDPClass::SetSocket(const std::string &ip, const int port)
{
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("<UDP> socket Open Error");

    memset(&Addr, 0x00, sizeof(Addr));
    Addr.sin_family = AF_INET;
    Addr.sin_addr.s_addr = INADDR_ANY;//inet_addr(ip.c_str());
    Addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&Addr, sizeof(Addr)) < 0)
        perror("<UDP> bind Error");
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
