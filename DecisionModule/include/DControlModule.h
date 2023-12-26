#ifndef DCONTROLMODULE_H
#define DCONTROLMODULE_H

#include <iostream> // = stdio.h, C++ 표준 입출력 스트림을 다루는 라이브러리
#include <string.h>
#include <thread>       // 멀티스레딩 지원 라이브러리, 병렬적인 작업을 수행하기 위해 스레드 생성 및 관리
#include <netinet/in.h> // 네트워크 프로그래밍에서 인터넷 주소 관련 구조체와 상수들을 제공
#include <arpa/inet.h>  //인터넷 주소 변환 함수를 제공하는 헤더 파일. IP 주소와 포트 번호를 변환하는 등의 작업을 수행
#include <unistd.h>
#include <net/if.h>        // 네트워크 인터페이스 관련 기능을 다루는 헤더 파일
#include <sys/ioctl.h>     // 입출력 장치를 제어하는 ioctl() 함수와 관련된 기능을 제공
#include <sys/time.h>      // 시간과 시간 관련 작업을 다루는 헤더 파일. 시간 측정 등에 사용
#include <sys/types.h>     // 시스템 데이터 형식과 관련된 정의들을 포함
#include <sys/socket.h>    // 소켓 프로그래밍을 위한 시스템 레벨의 함수와 구조체들을 제공
#include <linux/can.h>     // Linux 환경에서 CAN(Controller Area Network) 통신을 위한 구조체와 상수들을 정의하는 헤더 파일
#include <linux/can/raw.h> // Linux CAN 프로토콜에서 사용되는 raw 소켓과 관련된 구조체와 상수들을 정의
#include <time.h>
#include <math.h>
#include <cmath>

#define BufferSize 8192 // 1024
#define PathSize 8192   // 8192

#define EarthRadius 6378137 // WGS84
// #define Lat2meter 110979.309
// #define Lon2meter 88907.949
#define Lat2meter 111319.49079327358
#define Lon2meter 88903.69830789097

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

struct VehicleStruct
{
    /* 차량 정보를 저장하는 구조체
        차량의 운전 상태 및 다양한 센서로부터 수집된 정보를 저장 */
    double HandleAngle;       // [deg], 차량의 조향각
    double HandleSpd;         // [deg/s], 차량의 조향속도
    double Velocity;          // [m/s], 차량의 속도
    double ClusterVelocity;   // 차량 그룹의 속도, 다른 차량과의 상대적인 속도를 표현
    double LateralAccel;      // 차량의 측방(pitch, x축) 가속도, 차량이 좌우 방향으로 가속할 때의 가속도
    double LongitudinalAccel; // 차량의 전방(roll, y축) 가속도. 차량이 전진하거나 감속할 때의 가속도
    double YawRate;           // 차량의 z축 회전각, 차량의 방향이 얼마나 빨리 변하는지를 나타내며, '도/초' 단위로 표시
    // 좌우 방향지시등의 상태, 방향지시등이 켜져 있는지 여부를 표현
    uint8_t LeftTurnSwitch;
    uint8_t RightTurnSwitch;

    uint8_t MDPSmode;

    struct RadarStruct
    {
        /* 레이더 센서로부터 수신된 정보를 저장하는 구조체 */
        double Distance;         // 차량과 다른 물체 간의 거리
        double RelativeVelocity; // 다른 물체와의 상대적인 속도

        double X[16];
        double Y[16];
        double Latitude[16];
        double Longitude[16];

        // 보행자 판단
        double PathObjDist;
        double PedDistance[16];
        double WestMinPedDist;
        double EastMinPedDist;
        double MinPedDist;

        uint16_t MinPedIdx[16];
        uint16_t MinIdx;
        uint8_t PathObjectFlag;
    } Radar;
};

struct GlobalPathStruct
{
    /* TELECONS에서 제공해주는 차량의 주행 경로를 나타내는 구조체
        주행 경로의 좌표 및 관련 정보를 저장 */
    // 주행 경로의 모든 지점의 위도와 경도 값을 배열로 저장
    double Latitude[PathSize];
    double Longitude[PathSize];
    // 유효한 경로 지점의 위도와 경도를 배열로 저장
    double ValidLatitude[PathSize];
    double ValidLongitude[PathSize];
    // 경로 지점들 사이의 간격을 나타냄. 주행 경로 지점 간의 거리나 간격을 나타내어 경로의 밀도나 간격을 조절하는데 사용
    double GapLatitude;
    double GapLongitude;
    double Heading; // 주행 경로의 방향(pre, next 위경도를 이용하여 차의 방향(0~360도)을 나타냄.
    // 일반적으로 각도로 표현, GPS 방위각(azimous) - error를 구해서 wheel 각도 구하는데 사용
    double LocalizationGap;

    uint8_t NowEnv;
    uint8_t PreEnv;
    double PreDist;
    double PathAngle;
};

struct LocalPathStruct
{
    /* TELECONS에서 제공해주는 GPS 값을 튜닝하여 X, Y 좌표값(m)으로 표현 */
    double X[PathSize];
    double Y[PathSize];
    uint32_t Length; // 로컬 경로에 포함된 지점의 개수 - 저속: 20cm, 고속: 1m - 200개 지점
};

struct ControlStruct
{
    /* 제어 정보를 저장하는 구조체 */
    double Curvature;            // 차량의 곡률(주어진 지점에서의 도로의 곡률), 차량의 움직임을 계획
    double LateralDeviation;     // 차량의 측방 편차(주어진 경로에서 얼마나 벗어났는가), 제어 및 주행 상태 평가에 사용
    double RelativeHeadingAngle; // 다른 지점이나 차량과의 상대적인 방향을 나타냄.

    double Handle;       // 조향 휠의 회전각도
    double Acceleration; // 가속도
};

struct MobileyeStruct // A-CAN
{
    // Class(idx = 7, idx = 23 1byte) : start bit 56, 사람 0x50, 차량 0x22
    // X(L idx = 9>>4 + 10<<4 (76~87), R idx 25>>4 + 26<<4 1.5byte) : start bit 64
    // Y(L idx = 8 + 9<<4 (64~75), R idx 24 + 25<<8 1.5byte) : start bit 76
    double X[10];
    double Y[10];
    double Distance[10];
    double MinimumPedestrianDistance;
    uint16_t MinimumPedestrianIdx[10];
};

struct IbeoVariable
{
    uint8_t ObjectID;
    // Position
    int16_t X;
    int16_t Y;
    // Relative Velocity(0.1 m/s)
    int16_t Vx;
    int16_t Vy;
    // Box data
    int16_t BoxOrientation; // Object box orientation in the reference coordinate system in 1/100
    int8_t Boxflag;         // 0: object boxes, 1: bounding boxes
    uint16_t BoxSizeX;
    uint16_t BoxSizeY;
    int16_t Box[100];
    int16_t BoxCenterX;
    int16_t BoxCenterY;

    int Objectclassification; // object class
    int ObjectCnt;            // Object detection count
    double Object[100];       // Data(class, x, y, ..., class30, x30, y30)
    double Latitude[30];
    double Longitude[30];
    // 보행자 판단
    double PathObjDist;     // 경로상 장애물 전방거리
    double PedDistance[30]; // 경로~장애물 거리
    double WestMinPedDist;
    double EastMinPedDist;
    double MinPedDist;      // 경로~장애물 최소거리
    uint16_t PathObjIdx;    // 경로상 장애물 index
    uint16_t MinPedIdx[30]; // 보행자와의 최소거리 vertex
    uint16_t MinIdx;        // 최소 index
    uint8_t PathObjectFlag;
    // Fault
    uint8_t FaultCheckFlag;
    // Vertex 간격
    double FinalVertexDistance;
};

struct RadarStruct
{
    /* 레이더 센서로부터 수신된 정보를 저장하는 구조체 */
    double Distance;         // 차량과 다른 물체 간의 거리
    double RelativeVelocity; // 다른 물체와의 상대적인 속도

    int ObjectCnt;
    double X[16];
    double Y[16];
    double Latitude[16];
    double Longitude[16];

    // 보행자 판단
    double PathObjDist;
    double PedDistance[16];
    double WestMinPedDist;
    double EastMinPedDist;
    double MinPedDist;

    uint16_t MinPedIdx[16];
    uint16_t MinIdx;
    uint8_t PathObjectFlag;
};

// ------------------------------ Config ------------------------------------- //
extern const int TargetSpeed;      // 차량의 목표 속도 설정
extern const bool ReceivePathFlag; // TELECONS 시뮬레이션 경로 확인용
extern const bool ViewerFlag;      //
extern const bool GPSRecord;       // GPS 데이터 기록 플래그 변수
extern const string GPSRecordPath; //  GPS 데이터 기록 경로를 나타내는 문자열 상수
extern const bool PathRecord;
extern const string PathRecordPath;
extern const bool PedRecord;
extern const string PedDataPath;
extern const bool IbeoRecord;
extern const string IbeoDataPath;
extern const bool RadarRecord;
extern const string RadarDataPath;
extern const string ReferenceFile; // 참조 파일 경로를 나타내는 문자열 상수
extern const int MainCycle;        // 프로그램의 동작 속도를 조절
extern bool MCUSendSignal;
// ------------------------------ Sensor ------------------------------------- //
extern int MobileyeFlag;
extern int IbeoFlag;
extern int RadarFlag;
extern bool ViewerSenderFlag;
extern char GPSRaw[100]; // GPS Raw 데이터 저장
extern double toRadian;
extern double toDegree;
// ------------------------------ Struct ------------------------------------- //
extern GPSStruct GPS;
extern GlobalPathStruct Global;
extern LocalPathStruct Local;
extern VehicleStruct Vehicle;
extern MobileyeStruct Mobileye;
extern IbeoVariable Ibeo;
extern RadarStruct Radar;

#endif