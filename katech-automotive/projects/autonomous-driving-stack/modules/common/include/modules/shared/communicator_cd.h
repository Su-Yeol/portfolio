#pragma once

#include <fcntl.h>
#include <termios.h>

// cppcheck-suppress ctuOneDefinitionRuleViolation ; each module builds as independent binary
class CANClass
{
public:
    CANClass() : FrameFd{}, Frame{}, sock{-1}, nbytes{0}, addr{}, ifr{}, addrlen{0} {}

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
    ssize_t nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    socklen_t addrlen;
};

// cppcheck-suppress ctuOneDefinitionRuleViolation
class UDPClass
{
public:
    UDPClass() : Buffer{}, sock{-1}, Addr{}, JunkAddr{}, nbytes{0}, addrlen{0} {}

    void SetSocket(const std::string &ip, const int port, const bool BindFlag);
    void Receive(const uint16_t buffersize);
    void Send(const uint16_t SendByte);
    void CloseSocket();

    uint8_t Buffer[BufferSize];

private:
    int sock;
    struct sockaddr_in Addr, JunkAddr;
    ssize_t nbytes;
    socklen_t addrlen;
};

// cppcheck-suppress ctuOneDefinitionRuleViolation
class TCPClass
{
public:
    TCPClass() : SendBuffer{}, ServerSock{-1}, ClientSock{-1}, ServerAddr{}, ClientAddr{}, ClientAddrSize{0} {}

    void SetServerSocket(const std::string &ip, const uint16_t port);
    void Send(const uint16_t SendByte);
    void CloseSocket();

    uint8_t SendBuffer[BufferSize];

private:
    int ServerSock, ClientSock;
    struct sockaddr_in ServerAddr, ClientAddr;
    socklen_t ClientAddrSize;
};

