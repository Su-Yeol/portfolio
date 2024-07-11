#ifndef DCOMMUNICATOR_H
#define DCOMMUNICATOR_H

#include "DControlModule.h"
#include "DConfigParser.h"

class CANClass
{
public:
    void SetSocket(const std::string &ifname, const int canfd);
    void ReceiveCANFD();
    void ReceiveCAN();
    void SendCANFD();
    void SendCAN();
    void CloseSocket();
    void InitFrame();
    struct canfd_frame FrameFd;
    struct can_frame Frame;

private:
    int sock;
    uint16_t nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    socklen_t addrlen;
};

class UDPClass
{
public:
    void SetSocket(const std::string &ip, const int port, const bool BindFlag);
    void Receive(const uint16_t buffersize);
    void Send(const uint16_t SendByte);
    void CloseSocket();
    uint8_t Buffer[BufferSize];

private:
    int sock;
    struct sockaddr_in Addr;
    uint16_t nbytes;
    socklen_t addrlen;
};

class TCPClass
{
public:
    void SetServerSocket(const std::string &ip, const uint16_t port);
    void Send(const uint16_t SendByte);
    void CloseSocket();
    uint8_t SendBuffer[BufferSize];

private:
    int ServerSock, ClientSock;
    struct sockaddr_in ServerAddr, ClientAddr;
    socklen_t ClientAddrSize;
};

void Key();
void GPSParser();
void PathReceiver();
void VehicleReceiver();
void MobileyeReceiver();
void IbeoReceiver();
void RadarReceiver();
void MCUSender();
void ViewerSender();
#endif