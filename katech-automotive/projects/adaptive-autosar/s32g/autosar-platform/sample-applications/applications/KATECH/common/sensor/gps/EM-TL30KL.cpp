#include "sensor/gps/EM-TL30KL.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <math.h>
#include "logger.h"
#include <fstream>
namespace adcm
{
namespace sensor
{
namespace gps
{

int EM_TL30KL::devOpen(std::string device_name, bool deviceTypeUSB)
{
    std::string dev_path = "/dev";
    mDevice = device_name;
    mRunning = true;
    mDeviceTypeUSB = deviceTypeUSB;
    int result = dev_path.compare(0, 4, device_name, 0, 4);

    if(result == 0) {
        INFO("real Device file open (%s)", device_name.c_str());
        mThread_DeviceRead = std::make_shared<std::thread>(thread_DeviceRead);

    } else {
        INFO("simulation file open (%s)", device_name.c_str());
        mThread_DeviceRead = std::make_shared<std::thread>(thread_LogfileRead);
    }

    return 0;
}

void EM_TL30KL::devClose()
{
    mRunning = false;
    mThread_DeviceRead->join();
}
void EM_TL30KL::registCb_NMEA(CB_Nmea cb)
{
    mCB_NMEA = cb;
}
void EM_TL30KL::registCb_Status(CB_Status cb)
{
    mCB_Status = cb;
}
std::shared_ptr<EM_TL30KL::LocalPosition> EM_TL30KL::getNMEA()
{
    UBX_NAV_PVT_01_07 temp;
    std::shared_ptr<LocalPosition> result = std::make_shared<LocalPosition>();
    {
        std::lock_guard<std::mutex> guard(mMutex_AccessLock);
        temp = mCurrentGNSS_Data;
    }
    result->UTM_X = (double)temp.lon / 10000000;
    result->UTM_Y = (double)temp.lat / 10000000;
    result->heading = (double)temp.headVeh / 100000;
    result->speed = (double)temp.gSpeed / 1000;
    return result;
}
std::shared_ptr<EM_TL30KL::ModuleStatus> EM_TL30KL::getStatus()
{
    std::shared_ptr<ModuleStatus> result = std::make_shared<ModuleStatus>();
    {
        std::lock_guard<std::mutex> guard(mMutex_AccessLock);
        result->hAcc = mCurrentGNSS_Data.hAcc;
        result->vAcc = mCurrentGNSS_Data.vAcc;
        result->fixType = static_cast<FixType>(mCurrentGNSS_Data.fixType);
    }
    return result;
}

std::shared_ptr<EM_TL30KL::UBX_NAV_PVT_01_07> EM_TL30KL::getLowData()
{
    UBX_NAV_PVT_01_07 temp;
    {
        std::lock_guard<std::mutex> guard(mMutex_AccessLock);
        temp = mCurrentGNSS_Data;
    }
    std::shared_ptr<UBX_NAV_PVT_01_07> result = std::make_shared<UBX_NAV_PVT_01_07>(temp);
    return result;
}

void EM_TL30KL::registCb_LowData(CB_EM_TL30KL cb)
{
    mCB_LowData = cb;
}

std::shared_ptr<EM_TL30KL> EM_TL30KL::getInstance()
{
    static std::shared_ptr<EM_TL30KL> instance = std::make_shared<EM_TL30KL>();
    return instance;
}

int find_ch_count(char* buf, int size, char ch)
{
    int i;
    int result = 0;
    INFO("------------------------------");

    for(i = 0; i < size; i++) {
        if(buf[i] == ch) {
            result++;
            INFO(" > %s", buf + i);
        }
    }

    INFO("------------------------------");
    return result;
}

std::vector<std::string> EM_TL30KL::split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while(getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

double ddmm2dd(double ddmm)
{
    int degrees = static_cast<int>(ddmm / 100);
    double minutes = ddmm - degrees * 100;
    double decimalDegrees = degrees + minutes / 60.0;
    return decimalDegrees;
}

int EM_TL30KL::splitData(char buffer[], int length)
{
    int result = -1;
    char pasing_buf[4096];
    int index;
    int i = 0;

    // INFO("rx[%d] = %s", length ,buffer);
    // for(i = 0 ; i < length ; i++) {
    if((buffer[i] == 0xb5) && (buffer[i + 1] == 0x62) && (buffer[i + 2] == 0x01) && (buffer[i + 3] == 0x07)) {
        // INFO("Pass");
        if(length < sizeof(UBX_NAV_PVT_01_07) + 6) {
            ERROR("!!! received length error (%d < %d) ", length, sizeof(UBX_NAV_PVT_01_07) + 6);
            return result;
        }

        if(1) {   // mutex guard block
            std::lock_guard<std::mutex> guard(getInstance()->mMutex_AccessLock);
            memcpy(&getInstance()->mCurrentGNSS_Data, buffer + 6, 92);
        }

#if 1

        if(getInstance()->mCB_LowData != NULL) {
            std::shared_ptr<UBX_NAV_PVT_01_07> temp
                = std::make_shared<UBX_NAV_PVT_01_07>(getInstance()->mCurrentGNSS_Data);
            getInstance()->mCB_LowData(temp);
        }

        result = 0;
#endif
#if 0
        {
            UBX_NAV_PVT_01_07 temp;
            temp = getInstance()->mCurrentGNSS_Data;
            INFO("time = %d-%d-%d %d:%d:%d:%03d",
                 temp.Year,
                 temp.Month,
                 temp.Day,
                 temp.Hour,
                 temp.Min,
                 temp.Sec,
                 temp.nano / 1000000);
            INFO("Satellites = %d, fixType = %d", temp.numSV, temp.fixType);
            INFO("Flags = %02X %02X %02X", temp.flags, temp.flags2, temp.flags3);
            INFO("gps(lon/lat/heading/speed) %lf / %lf / %lfdegree / %dmm/s ",
                 (double)temp.lon / 10000000,
                 (double)temp.lat / 10000000,
                 (double)temp.headVeh / 100000,
                 temp.gSpeed);
            INFO("Accuracy(vertical/horizontal/Speed) = %dmm / %dmm / %dmm/s", temp.vAcc, temp.hAcc, temp.sAcc);
        }
#endif
        // i = i + 92 + 2 + 6;
        // continue;
        return result;
        //} else if(buffer[i] == '$' && buffer[i + 1] == 'G' && buffer[i + 2] == 'N') {
        //} else {
        //    ERROR("unknow packet[%d/%d] [%02X %02X %02X %02X %02X %02X]", i, length, buffer[i + 0], buffer[i + 1],
        //    buffer[i + 2], buffer[i + 3], buffer[i + 4], buffer[i + 5]);

    } else if(buffer[0] == '$') {
        buffer[length] = 0;

        // INFO("GNSS = %s", buffer);
        if(memcmp(buffer, "$GNRMC", 6) == 0) {
            buffer[length] = 0;
            // INFO("GNSS = %s", buffer);
            std::string temp(buffer);
            std::vector<std::string> tmp = split(temp, ',');

            // INFO("tmp [%d] %s / %s / %s", tmp.size(), tmp[5].c_str(), tmp[3].c_str(), tmp[8].c_str());
            if(tmp.size() < 9) {
                ERROR("PAcket error (split count = %d)", tmp.size());
                return result;
            }

            double lat = 0;

            if(tmp[3].length() != 0) {
                lat = ddmm2dd(std::stod(tmp[3]));
            }

            double lon = 0;

            if(tmp[5].length() != 0) {
                lon = ddmm2dd(std::stod(tmp[5]));
            }

            int time = 0;

            if(tmp[1].length() != 0) {
                time = static_cast<int>(std::stod(tmp[1]) * 1000);
            }

            int ms = time % 1000;
            time /= 1000;
            int ss = time % 100;
            time /= 100;
            int mm = time % 100;
            int hh = time / 100;
            // INFO("%s / %02d%02d%02d%03d", tmp[1].c_str(), hh, mm, ss, ms);
            int ddmmyy = 0;

            // INFO("tmp[0] = %s befor", tmp[0].c_str());
            // INFO("tmp[1] = %s befor", tmp[1].c_str());
            // INFO("tmp[2] = %s befor", tmp[2].c_str());
            // INFO("tmp[3] = %s befor", tmp[3].c_str());
            // INFO("tmp[4] = %s befor", tmp[4].c_str());
            // INFO("tmp[5] = %s befor", tmp[5].c_str());
            // INFO("tmp[6] = %s befor", tmp[6].c_str());
            // INFO("tmp[7] = %s befor", tmp[7].c_str());
            // INFO("tmp[8] = %s befor", tmp[8].c_str());
            // INFO("tmp[9] = %s befor", tmp[9].c_str());
            if(tmp[9].length() != 0) {
                try{
                ddmmyy = std::stoi(tmp[9]);

                }catch(std::exception e)
                {
                    ERROR("error : %s",e.what());
                }
            }
            // INFO("tmp[9] = %s after ", tmp[9].c_str());
            int year = ddmmyy % 100;
            ddmmyy /= 100;
            int month = ddmmyy % 100;
            int day = ddmmyy / 100;
            // INFO("%s / %02d%02d%02d", tmp[9].c_str(), year, month, day);
            {
                std::lock_guard<std::mutex> guard(getInstance()->mMutex_AccessLock);

                if(tmp[7].length() != 0) {
                    // double yaw = (-std::stod(tmp[7]) + 90) * M_PI / 180.0;
                    double speed = std::stod(tmp[7]);
                    getInstance()->mCurrentGNSS_Data.gSpeed = (unsigned int)(speed * 1000);
                }

                if(tmp[8].length() != 0) {
                    // double yaw = (-std::stod(tmp[8])) * M_PI / 180.0;
                    double yaw = std::stod(tmp[8]);
                    getInstance()->mCurrentGNSS_Data.headVeh = yaw * 100000;
                }

                getInstance()->mCurrentGNSS_Data.Hour = hh;
                getInstance()->mCurrentGNSS_Data.Min = mm;
                getInstance()->mCurrentGNSS_Data.Sec = ss;
                getInstance()->mCurrentGNSS_Data.nano = ms * 1000000;
                getInstance()->mCurrentGNSS_Data.Year = year;
                getInstance()->mCurrentGNSS_Data.Month = month;
                getInstance()->mCurrentGNSS_Data.Day = day;
                getInstance()->mCurrentGNSS_Data.lon = lon * 10000000;
                getInstance()->mCurrentGNSS_Data.lat = lat * 10000000;
            }

            if(getInstance()->mCB_LowData != NULL) {
                std::shared_ptr<UBX_NAV_PVT_01_07> temp
                    = std::make_shared<UBX_NAV_PVT_01_07>(getInstance()->mCurrentGNSS_Data);
                getInstance()->mCB_LowData(temp);
            }

            result = 0;
#if 0

        } else if(memcmp(buffer, "$GNGGA", 6) == 0) {
            // INFO("GNSS = %s", buffer);
            std::string temp(buffer);
            std::vector<std::string> tmp = split(temp, ',');

            if(tmp.size() < 7) {
                ERROR("PAcket error (split count = %d)", tmp.size());
                return result;
            }

            int fixed = 0;

            if(tmp[6].length() != 0) {
                fixed = std::stoi(tmp[6]);
            }

            // double HDOP = std::stod(tmp[8]);
            int num_sv = 0;

            if(tmp[7].length() != 0) {
                num_sv = std::stoi(tmp[7]);
            }

            // INFO("fixed = %d ", fixed);
            {
                std::lock_guard<std::mutex> guard(getInstance()->mMutex_AccessLock);
                getInstance()->mCurrentGNSS_Data.fixType = fixed;
                // getInstance()->mCurrentGNSS_Data.pDOP = HDOP;
                getInstance()->mCurrentGNSS_Data.numSV = num_sv;
            }
#endif
        }

    } else if(length > 1) {
        // INFO("unknow[%d] = %02X %02X %02X %02X %02X %02X", length,
        // buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],buffer[5]);
        for(i = 0; i < length; i++) {
            if((buffer[i] == 0xb5) && (buffer[i + 1] == 0x62) && (buffer[i + 2] == 0x01) && (buffer[i + 3] == 0x07)) {
                // INFO("finding !!! %d", i);
            } else if(buffer[i] == '$') {
                // INFO("Find in %d [%s]", i , buffer + i);
                result = splitData(buffer + i, length - i);
            }
        }
    }

    //}
    return result;
}
void EM_TL30KL::thread_DeviceRead()
{
#define LOW_BUFFER_SIZE 4096

    if(!getInstance()->mDeviceTypeUSB) {
        INFO("Serial device Open");
        std::shared_ptr<EM_TL30KL> instance = getInstance();
        int fd;
        int result;
        char buffer[LOW_BUFFER_SIZE];
        int index = 0;
        char buffer_low[LOW_BUFFER_SIZE];
        int index_low = 0;
        char buffer_gnss[LOW_BUFFER_SIZE];
        int index_gnss = 0;
        char buffer_pvt[LOW_BUFFER_SIZE] = "";
        int index_pvt = 0;
        bool found_gnss = false;
        std::fstream fs;
        char filename[64] = "";

        if(instance->mOPT_DumpLog) {
            sprintf(filename, "/home/root/gnss_log_%014lld.bin", adcm::Log::getUptime());
            fs.open(filename, std::ios::out | std::ios::binary);
        }

        fd = open(instance->mDevice.c_str(), O_RDONLY | O_NOCTTY);   // remove O_NONBLOCK option , i don't know why...
        {
            struct termios tty;
            INFO("tcget = %d", tcgetattr(fd, &tty));
            tty.c_cflag &= ~PARENB;
            tty.c_cflag &= ~CSTOPB;
            tty.c_cflag &= ~CSIZE;
            tty.c_cflag |= CS8;
            tty.c_cflag &= ~CRTSCTS;
            tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)
            tty.c_lflag &= ~ICANON;
            tty.c_lflag &= ~ECHO; // Disable echo
            tty.c_lflag &= ~ECHOE; // Disable erasure
            tty.c_lflag &= ~ECHONL; // Disable new-line echo
            tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
            tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
            tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL); // Disable any special handling of received bytes
            tty.c_cc[VTIME] = 0;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
            tty.c_cc[VMIN] = 0;
            cfsetispeed(&tty, B115200);
            tcsetattr(fd, TCSANOW, &tty);
        }

        if(fd > 0) {
            INFO("Device %s Open!!!", instance->mDevice.c_str());
            int index = 0;
            int targLength = 10;

            while(instance->mRunning) {
#if 0
                result = read(fd, buffer, LOW_BUFFER_SIZE);

                // INFO("read count = %d)", result);
                if(result == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                } else {
                    if(instance->mOPT_DumpLog) {
                        fs.write(reinterpret_cast<const char*>(buffer), result);
                    }

                    splitData(buffer, result);
                }

#else
                char ch;
                result = read(fd, buffer, LOW_BUFFER_SIZE);

                if(result == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                } else {
                    // INFO("result = %d / %s", result, buffer);
                    for(index = 0; index < result; index++) {
                        ch = buffer[index];
                        buffer_pvt[index_pvt++] = ch;
#if 1       // 0 : RMC, 1 : PVT

                        if(buffer_pvt[0] != 0xb5) {
                            index_pvt = 0;

                        } else {
                            if((buffer_pvt[0] == 0xb5)  && (buffer[1] == 0x62) && (buffer[ 2] == 0x01) && (buffer[ 3] == 0x07)) {
                                if(index_pvt >= sizeof(UBX_NAV_PVT_01_07) + 6) {
                                    splitData(buffer_pvt, index_pvt);
                                    index_pvt = 0;
                                }
                            }
                        }

#else

                        if(buffer_pvt[0] != '$') {
                            index_pvt = 0;

                        } else {
                            // INFO("Find $, %d / %s", index_pvt, buffer_pvt);
                            if(index_pvt > 5 && (buffer_pvt[0] == '$')  && (buffer[1] == 'G') && (buffer[ 2] == 'N') && (buffer[ 3] == 'R') && (buffer[ 4] == 'M') && (buffer[ 5] == 'C')) {
                                // INFO("index pvt = %d / %s", index_pvt,buffer_pvt);
                                if(buffer_pvt[index_pvt - 1] == '\n') {
                                    splitData(buffer_pvt, index_pvt);
                                    index_pvt = 0;
                                }

                            } else if(index_pvt > 5 && ((buffer_pvt[0] != '$')  && (buffer[1] != 'G') || (buffer[ 2] != 'N') || (buffer[ 3] != 'R') || (buffer[ 4] != 'M') || (buffer[ 5] != 'C'))) {
                                index_pvt = 0;
                                // memset(buffer_pvt,0,sizeof(buffer_pvt));
                            }
                        }

#endif
                    }
                }

#endif
            }

        } else {
            ERROR("Device %s Open error!!!", instance->mDevice.c_str());
        }

    } else {
        INFO("USB device Open");
        std::shared_ptr<EM_TL30KL> instance = getInstance();
        int fd;
        int result;
        char buffer[LOW_BUFFER_SIZE];
        std::fstream fs;
        char filename[64] = "";

        if(instance->mOPT_DumpLog) {
            sprintf(filename, "/home/root/gnss_log_%014lld.bin", adcm::Log::getUptime());
            fs.open(filename, std::ios::out | std::ios::binary);
        }

        fd = open(instance->mDevice.c_str(), O_RDONLY | O_NOCTTY);  // remove O_NONBLOCK option , i don't know why...

        if(fd > 0) {
            INFO("Device %s Open!!!", instance->mDevice.c_str());
            struct termios tty;

            while(instance->mRunning) {
                result = read(fd, buffer, LOW_BUFFER_SIZE);

                // INFO("read count = %d)", result);
                if(result == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));

                } else {
                    if(instance->mOPT_DumpLog) {
                        fs.write(reinterpret_cast<const char*>(buffer), result);
                    }

                    splitData(buffer, result);
                }
            }

        } else {
            ERROR("Device %s Open error!!!", instance->mDevice.c_str());
        }
    }

    getInstance()->mRunning = false;
}

int readLogData(int fd, char* buffer)
{
    int result;
    bool loop = true;
    int find_header = 0;
    int index = 0;
    char ch;
    int retValue = 0;

    while(loop) {
        result = read(fd, &ch, 1);

        if(result == 0) {
            loop = false;

        } else {
            // INFO("ch = 0x%02X(%c))", ch,ch);
            switch(find_header) {
            case 0:  // header finding
                if(ch == '$') {
                    find_header = 1;
                    buffer[0] = ch;
                    index = 1;
                    // INFO("GNSS first char = 0x%02X", ch);

                } else if(ch == 0xB5) {
                    find_header = 2;
                    buffer[0] = ch;
                    index = 1;
                    // INFO("ublux first char = 0x%02X", ch);

                } else {
                    // ERROR("unknow first char = 0x%02X", ch);
                }

                break;

            case 1:
                if(ch == 0x0A) {
                    loop = false;
                    retValue = 1;
                    // INFO("end GNSS packet");
                }

                buffer[index++] = ch;
                break;

            case 2:
                buffer[index++] = ch;

                if(index > (0x64 - 1)) {
                    retValue = 2;
                    loop = false;
                    // INFO("end ublux packet");
                }

                break;
            }
        }
    }

    // INFO("read count = %d", index);
    return retValue;
}

void EM_TL30KL::thread_LogfileRead()
{
#define LOW_BUFFER_SIZE 4096
    int fd;
    int result;
    char buffer[LOW_BUFFER_SIZE];
    fd = open(getInstance()->mDevice.c_str(), O_RDONLY);

    if(fd > 0) {
        INFO("Device %s Open!!!", getInstance()->mDevice.c_str());

        while(getInstance()->mRunning) {
            result = readLogData(fd, buffer);

            if(result == 0) {
                INFO("EOF found repeet");
                // getInstance()->mRunning = false;
                lseek(fd, 0, SEEK_SET);
                std::this_thread::sleep_for(std::chrono::milliseconds(50));

            } else {
                result = 0x64;
                result = splitData(buffer, result);

                // result = 2;
                if(result == 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(30));
                }
            }
        }

    } else {
        ERROR("Device %s Open error!!!", getInstance()->mDevice.c_str());
    }

    getInstance()->mRunning = false;
}

void EM_TL30KL::setDumpMode(bool enable)
{
    mOPT_DumpLog = enable;
}

}  // namespace gps
}  // namespace sensor
}  // namespace adcm
