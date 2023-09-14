#ifndef PCOMMUNICATOR_H
#define PCOMMUNICATOR_H

#include <termios.h>
#include <fcntl.h>
#include <stdlib.h>

#include "PControlModule.h"
#include "PConfigParser.h"

extern bool MainFlag;
extern bool SocketFlag;

class UDPClass // 속도 >> 신뢰성
{
    /* UDP(User Datagram Protocol) 통신에 관련된 기능 구현
        비연결 지향 프로토콜: 연결 절차 없이 발신자가 일방적으로 데이터를 발산하는 방식
        ex) VCU(5748 - GPS 정보) -> S32G  */
public:
    // 지정된 IP 주소와 포트 번호를 사용하여 소켓 설정
    void SetSocket(const std::string& ip, const int port, const bool BindFlag);
    // 데이터 수신
    void Receive(const uint16_t buffersize);
    // 데이터 송신
    void Send(const uint16_t SendByte);
    // 소켓 닫기
    void CloseSocket();
    // 데이터 송수신을 위한 버퍼
    uint8_t Buffer[BufferSize];

private:
    // UDP 통신을 위한 소켓 파일
    int sock;
    // 서버, 클라이언트 주소 정보를 저장하는 구조체 변수
    struct sockaddr_in Addr, JunkAddr, ServerAddr, ClientAddr;
    // 수신한 바이트 수를 저장하는 변수
    uint16_t nbytes;
    // 소켓 주소 길이를 저장하는 변수
    socklen_t addrlen;
};

void Key();
void GPSParser();
#endif