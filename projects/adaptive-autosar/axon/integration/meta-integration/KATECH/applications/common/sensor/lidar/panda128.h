#ifndef __LIDAR_PANDA128__
#define __LIDAR_PANDA128__

#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace Lidar
{

typedef struct {
    float x;
    float y;
    float z;
    float intensity;
} xPoint3D;

typedef std::vector<xPoint3D> xPCD;

class Panda128
{

public:
    typedef struct __attribute__((packed))
    {
        unsigned char SOP[2]; // start of packet, 0xEE, 0xFF
        unsigned char Version[2];
        unsigned char reserved[2];
        unsigned char LaserNum;	// 0x80
        unsigned char BlockNum; // 0x02
        unsigned char FirstBlockReturn; // resolved
        unsigned char DisUnit;
        unsigned char ReturnNum;
        unsigned char Flag_UDP_Sequence: 1;
        unsigned char Flag_IMU: 1;
        unsigned char Flag_FunctinalSafety: 1;
        unsigned char Flag_DigitalSignature: 1;

        unsigned char Flags_resolved: 4;
    } xHeader;

    typedef struct __attribute__((packed))
    {
        unsigned short Distance;
        unsigned char Reflectivity;
    } xChannel_XX;

    typedef struct __attribute__((packed))
    {
        unsigned short Azimuth;
        xChannel_XX Channel[128];
    } xBlock;

    typedef struct __attribute__((packed))
    {
        xBlock Block[2];
        unsigned int CRC1;
    } xBody;


    typedef struct __attribute__((packed))
    {
        unsigned char FS_Version;
				unsigned char RollingCounter:3;
		unsigned char FaultCodeType:2;

		unsigned char LidarState:3;
        unsigned char FaultCodeID:4;
        
        unsigned char TotalFaultCodeNum:4;
        unsigned short FaultCode;
        unsigned char Reserved[8];
        unsigned int CRC2;
    } xFunctinalsafety;

    
    typedef struct __attribute__((packed))
        {
            unsigned char Year;
            unsigned char Month;
            unsigned char Day;
            unsigned char Hour;
            unsigned char Min;
            unsigned char Sec;
            unsigned int us;
        } xDateTime;

    typedef struct __attribute__((packed))
        {
        	unsigned short resolved:12;
			unsigned short Block2:2;
			unsigned short Block1:2;
        } xAzimuthState;

    typedef struct __attribute__((packed))
    {
        unsigned char Resolved[9];
        xAzimuthState AzimuthState;
        unsigned char OperationalState;
        unsigned char ReturnMode; // 0x33 : First / 0x37 : Strongest / 0x38 : Last / 0x39 :Last, Strongest / 0x3B : Last, First / 0x3C : First, Strongest
        unsigned short MotorSpeed;
        xDateTime DateTime;
        unsigned char FactoryInformation; // 0x42
        unsigned int UDPSequence;
        unsigned short IMU_Temperature;
        unsigned short IMU_Acceleration_Unit;
        unsigned short IMU_AngularVelocityUnit;
        unsigned int IMU_Timestamp;
        unsigned short IMU_X_Acceleration;
        unsigned short IMU_Y_Acceleration;
        unsigned short IMU_Z_Acceleration;
        unsigned short IMU_X_AngularVelocity;
        unsigned short IMU_Y_AngularVelocity;
        unsigned short IMU_Z_AngularVelocity;
        unsigned int CRC3;
    } xTail;

    typedef struct __attribute__((packed))
    {
        unsigned char Signature[32];
    } xCyberSecurity;


    typedef struct __attribute__((packed))
    {
        xHeader Header;
        xBody Body;
        xFunctinalsafety Functinalsafety;
        xTail Tail;
        //xCyberSecurity CyberSecurity;
    } xPacket;

    typedef void (*xCallback)(std::vector<std::shared_ptr<xPCD>>&);

    Panda128(xCallback callback, unsigned short port, int timeOut_);
    unsigned short Calculate_CRC_RPF(const unsigned char* buffer, int size, unsigned char AC);

    ~Panda128();

    void printPacket(std::shared_ptr<xPacket> packet);
    void mergePacket(std::shared_ptr<xPacket> packet);
    std::shared_ptr<xPCD> getPCDData(std::shared_ptr<std::vector<xBlock>> pcd_input);

private:
    /// @brief Ports
    unsigned short mPort;

    /// @brief Threads for receiving data with sockets. Size is equal to mPorts.size()
    std::unique_ptr<std::thread> mUdpReceiver;

    /// @brief Sockets for receiving lidar data. Size equal to mPorts.size()
    int mSocket;

    /// @brief bool for thread while loop. Size equal to mPorts.size()
    bool mThreadRunning;

    std::shared_ptr<std::vector<xBlock>> mPcdInput;

    unsigned int mLastReceived_Index;

    std::queue<std::shared_ptr<xPacket>> mPacket;

    bool mPacketProcessorRunning;

    std::unique_ptr<std::thread> mPacketProcessor;

    std::unique_ptr<std::mutex> mTimeoutLock;

    std::mutex mCallbackLock;

    std::unique_ptr<std::condition_variable> mCv;

    xCallback mCallback;

    static Panda128* mInstance;
    void ThreadCallbackWrapper();
    void Thread_UdpReceiver();
    void ThreadPacketProcessor();

    int mTimeOut;

    int index;
};

static int mCount = 0;

static unsigned char mOperationalState;
static unsigned char mAzimuthState_0;
static unsigned char mAzimuthState_1;

static unsigned short mMotorSpeed;
static unsigned short mDisUnit;

/// @brief queue for pcd data. Size is equal to mPorts.size()
static std::vector<std::queue<std::shared_ptr<xPCD>>> mPcds;

/// @brief Thread for callback
static std::unique_ptr<std::thread> mCallbackWrapper;

/// @brief bool for thread wrapper running
static bool mWrapperRunning = false;

static std::condition_variable mCallbackCv;

}  // namespace Lidar

#endif
