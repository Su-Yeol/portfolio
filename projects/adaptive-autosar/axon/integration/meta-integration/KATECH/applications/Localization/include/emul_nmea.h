#ifndef __EMUL_NMEA_H__
#define __EMUL_NMEA_H__

/*
############################################################
#░                         ░       ░                       #
#  ▄▄▄▄   ░▓████▒██   ██▓ ▓█▓    ▒▓████   ██████   ██████  #
# ▓█████▄░▓█   ▀ ▓██  ██▒▒██▒   ░▓█   ▀ ▒██    ▒ ▒██    ▒  #
# ▒██▒ ▄██▒███    ▒██ ██░▒██░    ▒███   ░ ▓██▄   ░ ▓██▄    #
# ▒██░█▀  ▒▓█  ▄  ░ ▐██▓░▒██░    ▒▓█  ▄   ▒   ██▒  ▒   ██▒ #
# ░▓█  ▀█▓░▒████▒ ░ ██▒▓░░██████▒░▒████▒▒██████▒▒▒██████▒▒ #
# ░▒▓███▀▒░░ ▒░ ░  ██▒▒▒ ░ ▒░▓  ░░░ ▒░ ░▒ ▒▓▒ ▒ ░▒ ▒▓▒ ▒ ░ #
# ▒░▒   ░  ░ ░  ░▓██ ░▒░ ░ ░ ▒  ░ ░ ░  ░░ ░▒  ░ ░░ ░▒  ░ ░ #
# ░    ░    ░   ▒ ▒ ░░    ░ ░      ░   ░  ░  ░  ░  ░  ░    #
# ░         ░  ░░ ░         ░  ░   ░  ░      ░        ░    #
############################################################
*/

#include <vector>
#include <string>
#include <thread>
#include <mutex>

class EmulNMEA
{
public:
    typedef struct
    {
        unsigned long mTimeStemp;
        double m_utmX;
        double m_utmY;
        double m_Speed_Km;
        double m_dSpeed;
        double m_Angle;
        double m_dAngle;
    } xGPS_Position;

    EmulNMEA();
    ~EmulNMEA();
    // int getCurrentPosition_WGS84(double& lon, double& lat);
    int getCurrentPosition(double& utm_x,
        double& utm_y,
        double& angle,
        double& delta_angle,
        double& speed,
        double& delta_speed);
    static std::shared_ptr<EmulNMEA> getInstance();
    void init(std::string nmeaFilePath);
    void logPosition(xGPS_Position& position);

protected:
    void update_CurrentPosition();
    xGPS_Position getPositionHistory(int index);

private:
    std::vector<xGPS_Position> mPositionHistory;
    std::mutex mMutex_AccessLock;
    xGPS_Position mCurrentPosition = {0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
};

#endif
