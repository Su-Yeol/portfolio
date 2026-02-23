#pragma once

#include <arpa/inet.h>
#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <thread>
#include <unistd.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

#define MODULES_COMMON_USE_STD using namespace std;

#define MODULES_SHARED_GPS_FIELDS \
    double Time;                  \
    double Latitude;              \
    double Longitude;             \
    double Azimuth;               \
    char State;

#define MODULES_SHARED_VEHICLE_MOTION_FIELDS \
    double HandleAngle;                      \
    double HandleSpd;                        \
    double Velocity;                         \
    double ClusterVelocity;                  \
    double LateralAccel;                     \
    double LongitudinalAccel;                \
    double YawRate;

#define MODULES_SHARED_TURN_SWITCH_FIELDS \
    std::uint8_t LeftTurnSwitch;          \
    std::uint8_t RightTurnSwitch;

#define MODULES_SHARED_RADAR_CORE_FIELDS \
    double Distance;                     \
    double RelativeVelocity;

#define MODULES_SHARED_GLOBAL_PATH_FIELDS \
    double Latitude[PathSize];           \
    double Longitude[PathSize];          \
    double ValidLatitude[PathSize];      \
    double ValidLongitude[PathSize];     \
    double GapLatitude;                  \
    double GapLongitude;                 \
    double Heading;

#define MODULES_SHARED_LOCAL_PATH_FIELDS \
    double X[PathSize];                  \
    double Y[PathSize];                  \
    std::uint32_t Length;

#define MODULES_SHARED_CONTROL_GEOMETRY_FIELDS \
    double Curvature;                        \
    double LateralDeviation;                 \
    double RelativeHeadingAngle;

#define MODULES_SHARED_CONTROL_ACTUATOR_FIELDS \
    double Handle;                           \
    double Acceleration;

#define MODULES_COMMON_CONTROL_BASE_CONFIG \
    extern const int TargetSpeed;         \
    extern const bool ReceivePathFlag;    \
    extern const bool ViewerFlag;         \
    extern const bool GPSRecord;          \
    extern const string GPSRecordPath;    \
    extern const string ReferenceFile;
