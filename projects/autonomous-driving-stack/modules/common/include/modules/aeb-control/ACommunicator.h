#ifndef ACOMMUNICATOR_H
#define ACOMMUNICATOR_H

#include "AControlModule.h"
#include "AConfigParser.h"
#include "../shared/ModuleInterfaceCommon.h"

MODULES_COMMON_COMM_EXTERN_FLAGS
MODULES_COMMON_COMM_EXTERN_STATE(GPSStruct, VehicleStruct, GlobalPathStruct, LocalPathStruct, ControlStruct)

class CANClass
{

public:
    void SetSocket(const std::string &ifname, const int canfd);

    void ReceiveCANFD();

    void ReceiveCAN();

    void SendCANFD();

    void SendCAN();

    void CloseSocket();

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
    void SetServerSocket(const std::string &ip, const uint16_t port);

    void SetClientSocket(const std::string &ip, const uint16_t port);

    void Receive(const uint16_t buffersize);

    void Send(const uint16_t SendByte);

    void CloseSocket();

    uint8_t ReceiveBuffer[BufferSize];

    uint8_t SendBuffer[BufferSize];

private:
    int sock;

    struct sockaddr_in ServerAddr, ClientAddr;

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

MODULES_COMMON_COMM_DECLARE_BASE_HANDLERS
void GPSReceiver();
void SRCCommunication();

#endif
