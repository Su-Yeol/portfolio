#ifndef ADCM_KATECH_H
#define ADCM_KATECH_H

#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <iostream>
#include <string.h>
#include <thread>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <memory.h>
#include <time.h>
#include <stdlib.h>
#include <cstdlib>
#include <wchar.h>
#include <stdint.h>
#include <stdbool.h>
#include <cstring>
#include <cxxabi.h>

#include "driving_trajectory_subscriber.h"
#include "katech_database.h"

using namespace std;

#define PathLength 30

// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ sy.kim ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

#define AP_LOCAL_ADDR 0x34200000UL  // AP
#define AP_REMOTE_ADDR 0x34100000UL  // MCU
#define SHM_BYTES 0x100000
#define CHANNEL_COUNT 8
#define SHM_SLOT_SIZE 256

typedef struct __attribute__((packed))
{
    volatile uint32_t head; // 0: empty, 1: full
    uint32_t len;           // payload 길이
    uint8_t data[SHM_SLOT_SIZE];
} ChannelBuffer_t;

typedef struct __attribute__((packed))
{
    ChannelBuffer_t ch[CHANNEL_COUNT];
} SharedMemory_t;


enum class RxPayloadType : uint8_t {
    V2A = 0,
    VEHICLE_INFO = 1 // test
    // 추후 확장시 추가
};

constexpr int get_channel_id(RxPayloadType type) {
    return static_cast<int>(type);
}

enum class TxPayloadType : uint8_t {
    A2V = 0,
    // 추후 확장시 추가
};

constexpr int get_channel_id(TxPayloadType type) {
    return static_cast<int>(type);
}

typedef struct
{
    union
    {
        KATECH_CONTROL_V2A v2a;
        KATECH_VEHICLE_INTERFACE_INFO info; // test
    } payload;
} KATECH_RX_Packet;


typedef struct
{
    union
    {
        KATECH_CONTROL_A2V a2v;
    } payload;
} KATECH_TX_Packet;
// ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ sy.kim ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ

namespace KATECH
{

struct UtmKStruct
{
    double x;
    double y;
    double yaw;
    double Interval;
};
struct LocalStruct
{
    double X;
    double Y;
};

class func
{
public:
    void Initialize(double X,
        double Y,
        double Yaw,
        TrajectoryPointArray Path,
        double Accel,
        bool drvmode,
        float LidarDist,
        float LidarRelSpd,
        float Tspd);
    void GenerateLocalPath();
    void PurePursuit();
    void SCC();
    void LatiControl();
    void LongiControl();
    void SendBuffer(uint8_t modeCmd, float ControlAx, float ControlSteer);
    void ReceiveBuffer();
    void DataLogger(bool operation);
    void SendBuffer_Remote(uint8_t RemoteMod, double RxAcc, double RxBreak, double RxSteer, uint8_t RxTurn_signal);

    double Gap;

private:
    double CalculateDistance(double x1, double x2, double y1, double y2);
    double GetLateraldeviation();
    double GetRelativeYawAngle();
    double Velocity;
    UtmKStruct Pos;
    std::vector<UtmKStruct> GlobalPath;
    std::vector<LocalStruct> LocalPath;

    double Heading;
    double HeadingError;
    double LookaheadLength;
};

int katech_ipcf_init(void);
void katech_ipcf_signal_io_fun(int signum);
void katech_ipcf_signal_int_fun(int signum);
// void katech_ipcf_read(KATECH_CONTROL_V2A *data);
// void katech_ipcf_send(KATECH_CONTROL_A2V *data);
void katech_ipcf_read(KATECH_RX_Packet* rx, RxPayloadType type);
void katech_ipcf_send(KATECH_TX_Packet* tx, TxPayloadType type);

// OUTPUT
extern double WheelSpeed_FL;  // [kph]
extern double WheelSpeed_FR;  // [kph]
extern double WheelSpeed_RL;  // [kph]
extern double WheelSpeed_RR;  // [kph]
extern double ClusterSpeed;  // [kph]
extern uint8_t GearState;  // PRND : 1234
extern uint8_t HandleState;  // 0=driver  1=automode  2=fault
extern double LateralAcceleration;  // [m/s^2]
extern double LongitudinalAcceleration;  // [m/s^2]
extern double Angular_Acceleration;  // Yawrate [rad/s]
extern uint8_t Turn_Signal;  // OFF:0 , LeftON:1, RightON:2
extern uint8_t Turn_SignalReport;
extern bool AutoDrivingMode;  // OFF:0, ON:1
extern uint8_t Emergency_Light;  // OFF:0 , ON:1
extern bool No_Autonomous_Driving;  // Auto:0 , NO_Auto:1
extern double SteeringWheelAngle;  // [degree]
extern double LanePosition_Left;  // [m]
extern double LanePosition_Right;  // [m]
extern double LaneHeadingAngle_Left;  // [rad]
extern double LaneHeadingAngle_Right;  // [rad]
extern double LaneCurvature_Left;  // [1/m]
extern double LaneCurvature_Right;  // [1/m]
extern double LaneCurvatureRate_Left;  // [1/m^2]
extern double LaneCurvatureRate_Right;  // [1/m^2]

extern float Lidar_ObjDistVal_Front;  // [m]
extern float Lidar_ObjRelSpdVal_Front;  // [m/s]
extern float SCC_TargetVelocity;  // [m/s]

extern float Radar_ObjDistVal_Front;
extern float Radar_ObjRelSpdVal_Front;

extern float CornerRadar_x1;
extern float CornerRadar_y1;
extern float CornerRadar_x2;
extern float CornerRadar_y2;
extern float CornerRadar_x3;
extern float CornerRadar_y3;
extern float CornerRadar_x4;
extern float CornerRadar_y4;

extern float CornerRadar2_x1;
extern float CornerRadar2_y1;
extern float CornerRadar2_x2;
extern float CornerRadar2_y2;
extern float CornerRadar2_x3;
extern float CornerRadar2_y3;
extern float CornerRadar2_x4;
extern float CornerRadar2_y4;

extern float CornerRadar3_x1;
extern float CornerRadar3_y1;
extern float CornerRadar3_x2;
extern float CornerRadar3_y2;
extern float CornerRadar3_x3;
extern float CornerRadar3_y3;
extern float CornerRadar3_x4;
extern float CornerRadar3_y4;

extern float CornerRadar4_x1;
extern float CornerRadar4_y1;
extern float CornerRadar4_x2;
extern float CornerRadar4_y2;
extern float CornerRadar4_x3;
extern float CornerRadar4_y3;
extern float CornerRadar4_x4;
extern float CornerRadar4_y4;

extern uint8_t NearSensor;

extern double ControlHandle;
extern double ControlAx;
extern double ControlHandle_Log;
extern double ControlAx_Log;

extern float WheelPulse_FL;
extern float WheelPulse_FR;
extern float WheelPulse_RL;
extern float WheelPulse_RR;
extern float AccelCylPrs;
extern float brakecylPrs;

extern uint8_t ADCMFault;
extern uint8_t VCUFault;
extern uint16_t vcualivecnt;
extern uint8_t MobileyeFault;
extern uint8_t LCANFault;
extern uint8_t FrontRadarFault;
extern uint8_t HMIFault;
extern uint8_t SeatBeltCheck;
extern uint8_t LongiError;

namespace log
{
inline constexpr std::size_t kLogPathBufferSize = SHM_SLOT_SIZE;

extern FILE* StateFile;
extern FILE* GlobalPathFile;
extern FILE* LocalPathFile;
extern char StatePath[kLogPathBufferSize], GlobalPathPath[kLogPathBufferSize], LocalPathPath[kLogPathBufferSize];
extern uint64_t maincnt;

extern std::chrono::steady_clock::time_point current_time;
extern std::chrono::steady_clock::time_point old_time;
extern long long TimeGap;
}  // namespace log

}  // namespace KATECH

#endif  // ADCM_KATECH_H
