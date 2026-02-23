
#include <thread>
#include <chrono>
#include "logger.h"
#include "emul_nmea.h"
#include <fstream>
#include <iostream>

#include <cmath>

#include "proj_converter.h"

double degree2radian(double degree)
{
    degree *= M_PI;
    degree /= 180;
    return degree;
}

double wgs2double(std::string temp)
{
    double result;
    std::string subStr;
    int index;
    //INFO("input = %s", temp.c_str());
    index = temp.find('.');
    index -= 2;
    subStr = temp.substr(0, index);
    //INFO("subStr = %s", subStr.c_str());
    temp = temp.substr(index);
    //INFO("input = %s", temp.c_str());
    result = std::stod(temp);
    //katech::Log::Info() << "result = " << result;
    result /= 60;
    //katech::Log::Info() << "result = " << result;
    result += std::stod(subStr);
    //katech::Log::Info() << "result = " << result;
    return result;
}

void parserRMC(std::string& in_rmc, double& out_time, double& out_latitude, double& out_longitude, double& out_angle, double& out_speed_km)
{
    int searchIndex;
    int foundCount;
    int startIndex;
    std::string temp;

    for(searchIndex = 0, foundCount = 0; searchIndex < in_rmc.size(); searchIndex++) {
        if(in_rmc[searchIndex] == ',') {
            foundCount++;

            switch(foundCount) {
            case 1:     // start of time
                startIndex = searchIndex + 1;
                break;

            case 2:     // end of out_time
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "time = " << temp;
                out_time = std::stod(temp);
                break;

            case 3:     // start of lat
                startIndex = searchIndex + 1;
                break;

            case 4:     // endof lat
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "Lon = " << temp;
                out_latitude = wgs2double(temp);
                break;

            case 5:     // start of lon
                startIndex = searchIndex + 1;
                break;

            case 6:     // end of lon
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "Lat = " << temp;
                out_longitude = wgs2double(temp);
                break;

            case 7:     // start of speed over ground ( knots )
                startIndex = searchIndex + 1;
                break;

            case 8:     // end of speed over ground / start of track angle
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "Speed = " << temp;
                {
                    out_speed_km = std::stod(temp);
                    out_speed_km *= 1.852;
                    //katech::Log::Info() << "Speed = " << out_speed_km << "km/h";
                }
                startIndex = searchIndex + 1;
                break;

            case 9:     // endof track angle / start of date
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "Angle = " << temp;
                out_angle = std::stod(temp);
                startIndex = searchIndex + 1;
                break;

            case 10:    // end of date
                temp = in_rmc.substr(startIndex, searchIndex - startIndex);
                //katech::Log::Info() << "Date = " << temp;
                break;

            default:
                searchIndex = in_rmc.size();   // set break condition
                break;
            }
        }
    }
}

std::shared_ptr<EmulNMEA> EmulNMEA::getInstance()
{
    static std::shared_ptr<EmulNMEA> instance = std::make_shared<EmulNMEA>();
    return instance;
}

EmulNMEA::EmulNMEA()
{
}

void EmulNMEA::logPosition(xGPS_Position& position)
{
    katech::Log::Info() << "time = " << position.mTimeStemp << " | UTM = " << position.m_utmX << " / " << position.m_utmY << " | Angle = " << position.m_Angle << "dgree" << " | Speed = " << position.m_Speed_Km << "km/h";
}

void EmulNMEA::init(std::string nmeaFilePath)
{
    double lat, lon;
    double time;
    double speed_km;
    double angle;
    xGPS_Position sample;
    katech::sensor::gps::ProjConverter proj = katech::sensor::gps::ProjConverter(katech::sensor::gps::ProjConverter::UTM_Type::UTM_52N);

    katech::Log::Info() << "NMEA Dummy : " << nmeaFilePath;
    mPositionHistory.clear();
    // read File
    std::ifstream openFile(nmeaFilePath.data());

    if(openFile.is_open()) {
        std::string nmea_data;
        katech::Log::Info() << "load NMEA Data...";
        while(getline(openFile, nmea_data)) {
            //katech::Log::Info() << nmea_data ;
            std::string tag = nmea_data.substr(3, 3);

            if(tag == "RMC") {
                parserRMC(nmea_data, time, lat, lon, angle, speed_km);
                angle = degree2radian(angle);
                sample.mTimeStemp = time;
                // transWGStoUTM(&sample.m_utmX, &sample.m_utmY, &lon, &lat);
                proj.convert(sample.m_utmX, sample.m_utmY, lon, lat);
                //katech::Log::Info() << "lon/lat : " << lon << " / " << lat << " >>> " << "utm = " << sample.m_utmX << " / " << sample.m_utmY;
                sample.m_Angle = angle;
                sample.m_Speed_Km = speed_km;
                mPositionHistory.push_back(sample);
            }
        }

        openFile.close();
#if 0

        for(int i = 0; i < mPositionHistory.size(); i++) {
            logPosition(mPositionHistory[i]);
        }

#endif
    }
}

EmulNMEA::~EmulNMEA()
{
}

int EmulNMEA::getCurrentPosition(double& utm_x, double& utm_y, double& angle, double& delta_angle, double& speed, double& delta_speed)
{
    std::lock_guard<std::mutex> guard(mMutex_AccessLock);
    static int index = 0;
    static int subIndex = 0;
    static xGPS_Position current;
    static xGPS_Position next;
    double delta_x;
    double delta_y;

    if(subIndex == 0) {
        current = mPositionHistory[index];
        index++;
        index %= mPositionHistory.size();
        next = mPositionHistory[index];
    }

    delta_angle = next.m_Angle - current.m_Angle;
    delta_speed = next.m_Speed_Km - current.m_Speed_Km;
    delta_x = next.m_utmX - current.m_utmX;
    delta_y = next.m_utmY - current.m_utmY;
    utm_x = current.m_utmX + ((delta_x * subIndex) / 10);
    utm_y = current.m_utmY + ((delta_y * subIndex) / 10);
    angle = current.m_Angle + ((delta_angle * subIndex) / 10);     // radian
    speed = current.m_Speed_Km + ((delta_speed * subIndex) / 10);  // km
    //INFO("X = %lf(%lf), Y = %lf(%lf), A = %lf(%lf), S = %lf(%lf)",utm_x, delta_x, utm_y, delta_y ,angle, delta_angle, speed,delta_speed);
    subIndex++;
    subIndex %= 10;
    return 0;
}


void EmulNMEA::update_CurrentPosition(EmulNMEA::xGPS_Position base, EmulNMEA::xGPS_Position next, double rate)
{
    std::lock_guard<std::mutex> guard(mMutex_AccessLock);
    mCurrentPosition.m_utmX = 0;
    mCurrentPosition.m_utmY = 0;
}

EmulNMEA::xGPS_Position EmulNMEA::getPositionHistory(int index)
{
    return mPositionHistory[index];
}
