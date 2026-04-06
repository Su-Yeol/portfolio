#ifndef REMOTE_H
#define REMOTE_H

#include <linux/can.h>
#include <linux/can/raw.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define BufferSize 8192


// cppcheck-suppress ctuOneDefinitionRuleViolation ; each module builds as independent binary
class CANClass
{
    public:
        CANClass() : FrameFd{}, sock{-1}, nbytes{0}, addr{}, ifr{}, addrlen{0} {}

        void SetSocket(const std::string &ifname, const int canfd);
        void RecieveCANFD();
        void SendCANFD();
        void CloseSocket();
        struct canfd_frame FrameFd;
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
    UDPClass() : Buffer{}, sock{-1}, Addr{}, nbytes{0}, addrlen{0} {}

    void SetSocket(const std::string &ip, const int port);
    void Receive(const uint16_t buffersize);
    void Send(const uint16_t SendByte);
    void CloseSocket();
    uint8_t Buffer[BufferSize];

private:
    int sock;
    struct sockaddr_in Addr;
    ssize_t nbytes;
    socklen_t addrlen;
};


#endif
