/* 전처리기: #define, #if, #ifdef, #ifndef
    실질적인 컴파일 이전에 미리 처리되는 문장(선행처리기)
    디버깅에 도움을 주며 헤더 파일의 중복 포함도 방지 */

/* #ifndef ~ #endif
    구조체 중복 정의로 인해 발생하는 오류 방지 */
#ifndef JCOMMUNICATOR_H
#define JCOMMUNICATOR_H

// key
/* termios.h
    기본 입력, 출력, 제어 및 줄 관리 모드를 정의
    POSIX(OS간 호환성을 위한 IEEE 표준 규격) 호환성을 위한 터미널 인터페이스를 제공 */
#include <termios.h>
/* fcntl.h
    파일을 열고, 잠금 및 다른 작업 가능
    읽기 잠금을 통해 여러 프로세스가 공유 가능한 읽기 잠금
    쓰기 잠금을 통해 한 프로세스만이 가질 수 있는 쓰기 잠금

   unistd.h - write(), read(), close() */
#include <fcntl.h>
#include <stdlib.h>

#include "JControlModule.h"
#include "JConfigParser.h"

extern bool MainFlag;
extern bool SocketFlag;

extern bool MCUSendSignal;
extern bool SRCSendSignal;
extern bool PathReceiveSignal;
extern bool ViewerSendSignal;

/* 클래스 생성
    class 클래스 이름{
        접근 제어 지시자:
            멤버변수:
            멤버함수
    } */
class CANClass
{
    /* 통신을 위한 소켓 설정을 수행하는 역할 */
public:
    /* std::string
        std:: std 네임스페이스에 속해 있다라는 의미
        C++ 에서 제공하는 문자열을 다루는 클래스
        문자열 데이터를 저장하고 처리하는데 사용
        문자열이 길어졌을 때 내부 버퍼를 다시 할당하는 비용을 절약하는 트릭
       const
        변수나 매개변수가 변경되지 않도록 보장 */
    // CAN 통신을 위해 지정된 네트워크 인터페이스와 CAN FD를 위한 소켓 설정
    void SetSocket(const std::string &ifname, const int canfd);
    // CAN FD 프레임 수신
    void ReceiveCANFD();
    // CAN 표준 프레임 수신
    void ReceiveCAN();
    // CAN FD 프레임 전송
    void SendCANFD();
    // CAN 표준 프레임 전송
    void SendCAN();
    // 소켓 닫기
    void CloseSocket();
    void InitFrame();
    // CAN FD 프레임을 저장하는 구조체
    struct canfd_frame FrameFd;
    // CAN 표준 프레임을 저장하는 구조체
    struct can_frame Frame;

private:
    // 소켓 파일 디스크립터
    int sock;
    // 수신한 바이트 수
    uint16_t nbytes;
    // CAN 소켓 주소 정보
    struct sockaddr_can addr;
    /* 네트워크 인터페이스 정보
        linux에서 네트워크 장치의 설정과 관련된 대부분의 ioctl() 호출에 3번째 인자로 사용하는 구조체
            ioctl() 함수란 하드웨어의 제어와 상태 정보를 얻기 위해 제공되는 함수 */
    struct ifreq ifr;
    // 소켓 주소 길이
    socklen_t addrlen;
};

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

class TCPClass // 신뢰성 >> 속도
{
    /* TCP(Transmission Control Protocol): 전송을 제어하는 프로토콜
        연결 지향 프로토콜: 서버가 연결된 상태에서 데이터를 주고 받는 프로토콜
        송수신 응답이 존재 - 신뢰성이 높음 - 클라이언트와 서버, Socket 통신 등에 사용
        ex) Local path, HMI(갤탭 - 장애물 등) */
public:
    /* 지정된 IP 주소와 포트 번호를 사용하여 소켓 설정
       클라이언 연결 요청을 수신하고 데이터 통신을 위한 연결을 생성 */
    void SetServerSocket(const std::string &ip, const uint16_t port);
    void Send(const uint16_t SendByte);
    void CloseSocket();
    uint8_t SendBuffer[BufferSize];

private:
    int ServerSock, ClientSock;
    // 서버 주소 정보와 클라이언트 주소 정보
    struct sockaddr_in ServerAddr, ClientAddr;
    // 클라이언트 주소 정보의 크기
    socklen_t ClientAddrSize;
};

void Key();
void GPSParser();
void PathReceiver();
void VehicleReceiver();
void MobileyeReceiver();
void IbeoReceiver();
void MCUSender();
void ViewerSender();
#endif