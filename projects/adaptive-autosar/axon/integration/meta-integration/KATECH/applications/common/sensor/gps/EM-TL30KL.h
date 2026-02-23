#ifndef __GPS_EM_TL30KL_H__
#define __GPS_EM_TL30KL_H__

#include <memory>
#include <thread>
#include <mutex>
#include <vector>
namespace katech
{
namespace sensor
{
namespace gps
{


class EM_TL30KL
{

public:
    enum class FixType {
        noFix = 0,
        deadReckonigOnly = 1,
        Fix2D = 2,
        Fix3D = 3,
        GNSS_Deadreckoing_Combined = 4,
        timeOnly = 5,

    };

    typedef struct __packed {
        unsigned int iTOW;

        unsigned short Year;
        unsigned char Month;
        unsigned char Day;

        unsigned char Hour;
        unsigned char Min;
        unsigned char Sec;
#if 1
        unsigned char valid;
#else
        unsigned char valid_resolved: 4;
        unsigned char valid_validMessage: 1;
        unsigned char valid_fullyResolved: 1;
        unsigned char valid_validTime: 1;
        unsigned char valid_validData: 1;

#endif
        unsigned int tAcc;

        unsigned int nano;

        unsigned char fixType;
#if 1
        unsigned char flags;
#else
        unsigned char flags1_carrSoln: 2;
        unsigned char flags1_headVehValid: 1;
        unsigned char flags1_psmState: 3;
        unsigned char flags1_diffSoln: 1;
        unsigned char flags1_gnssFixOK: 1;


#endif
#if 1
        unsigned char flags2;
#else
        unsigned char flags2_confirmedTime: 1;
        unsigned char flags2_confirmedDate: 1;
        unsigned char flags2_confirmedAvai: 1;
        unsigned char flags2_unused: 5;


#endif
        unsigned char numSV;


        unsigned int lon;
        unsigned int lat;
        unsigned int height;
        unsigned int hMSL;
        unsigned int hAcc;
        unsigned int vAcc;
        unsigned int velN;
        unsigned int velE;
        unsigned int velD;
        unsigned int gSpeed;
        unsigned int headMot;
        unsigned int sAcc;
        unsigned int headAcc;

        unsigned short pDOP;
#if 1
        unsigned char flags3;
#else
        unsigned char flags3_unused: 7;
        unsigned char flags3_invalidLlh: 1;
#endif
        unsigned char reserved0[5];

        unsigned int headVeh;

        unsigned short magDec;
        unsigned short magAcc;


    } UBX_NAV_PVT_01_07;

    typedef struct {
        //long time;
        double UTM_X;
        double UTM_Y;
        double heading;
        double speed;

    } LocalPosition;

    typedef struct {
        unsigned int hAcc;
        unsigned int vAcc;
        FixType fixType;
    } ModuleStatus;

    typedef void (*CB_Nmea)(std::shared_ptr<LocalPosition>);
    typedef void (*CB_Status)(std::shared_ptr<ModuleStatus>);
    typedef void (*CB_EM_TL30KL)(std::shared_ptr<UBX_NAV_PVT_01_07>);

    int devOpen(std::string device_name, bool deviceTypeUSB = true);
    void devClose();
    void registCb_NMEA(CB_Nmea cb);
    void registCb_Status(CB_Status cb);
    void registCb_LowData(CB_EM_TL30KL cb);
    std::shared_ptr<LocalPosition> getNMEA();
    std::shared_ptr<ModuleStatus> getStatus();
    std::shared_ptr<UBX_NAV_PVT_01_07>getLowData();
    static std::shared_ptr<EM_TL30KL> getInstance();
    static std::vector<std::string> split(const std::string& s, char delim);
    void setDumpMode(bool enable);

private:
    bool mOPT_DumpLog = false;
    CB_Nmea mCB_NMEA = NULL;
    CB_Status mCB_Status = NULL;
    CB_EM_TL30KL mCB_LowData = NULL;
    bool mRunning = false;
    bool mDeviceTypeUSB = true;
    std::shared_ptr<std::thread> mThread_DeviceRead;
    std::string mDevice;
    UBX_NAV_PVT_01_07 mCurrentGNSS_Data;
    std::mutex mMutex_AccessLock;

    static void thread_DeviceRead();
    static void thread_LogfileRead();
    static int splitData(char buffer[], int length);

};


}
}
}
#endif
