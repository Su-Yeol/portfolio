#ifndef PCONTROLMODULE_H
#define PCONTROLMODULE_H

#include <iostream> // = stdio.h, C++ 표준 입출력 스트림을 다루는 라이브러리
#include <string.h>
#include <thread>       // 멀티스레딩 지원 라이브러리, 병렬적인 작업을 수행하기 위해 스레드 생성 및 관리
#include <netinet/in.h> // 네트워크 프로그래밍에서 인터넷 주소 관련 구조체와 상수들을 제공
#include <arpa/inet.h>  //인터넷 주소 변환 함수를 제공하는 헤더 파일. IP 주소와 포트 번호를 변환하는 등의 작업을 수행
#include <unistd.h>
#include <net/if.h> // 네트워크 인터페이스 관련 기능을 다루는 헤더 파일

#include <sys/ioctl.h>     // 입출력 장치를 제어하는 ioctl() 함수와 관련된 기능을 제공
#include <sys/time.h>      // 시간과 시간 관련 작업을 다루는 헤더 파일. 시간 측정 등에 사용
#include <sys/types.h>     // 시스템 데이터 형식과 관련된 정의들을 포함
#include <sys/socket.h>    // 소켓 프로그래밍을 위한 시스템 레벨의 함수와 구조체들을 제공
#include <linux/can.h>     // Linux 환경에서 CAN(Controller Area Network) 통신을 위한 구조체와 상수들을 정의하는 헤더 파일
#include <linux/can/raw.h> // Linux CAN 프로토콜에서 사용되는 raw 소켓과 관련된 구조체와 상수들을 정의
#include <math.h>

#define BufferSize 8192
#define PathSize 1024

using namespace std;

struct GPSStruct
{
    /* GPS 정보를 저장하는 구조체
        GPS에서 수신한 위치와 시간관련 정보들을 저장 */
    double Time;
    double Latitude;  // GPS의 위도 정보
    double Longitude; // GPS의 경도 정보
    double Azimuth;   // GPS의 방위각
    char State;       // GPS 상태(정상, 오류, 수신없음)
};

// ------------------------------ Config ------------------------------------- //
extern const bool GPSRecord;       // GPS 데이터 기록 플래그 변수
extern const int MainCycle;        // 프로그램의 동작 속도를 조절
extern const string GPSRecordPath; //  GPS 데이터 기록 경로를 나타내는 문자열 상수

// ------------------------------ Struct ------------------------------------- //
extern GPSStruct GPS;

#endif