#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H

#include <stdio.h>
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
#include <linux/can.h>
#include <linux/can/raw.h>

#include <math.h>


#define BufferSize 1024
#define PathSize 8192

using namespace std;

struct GPSVariable
{
    double Time;
    double Latitude;
    double Longitude;
    double Azimuth;
    char State;
};

struct VehicleVariable
{
    double HandleAngle; // [deg]
    double HandleSpd; // [deg/s]
    double Velocity;
    double ClusterVelocity; //[kph]
    double LateralAccel;
    double LongitudinalAccel;
    double YawRate;
    uint8_t LeftTurnSwitch;
    uint8_t RightTurnSwitch;
    uint8_t MDPSmode;
    struct RadarVariable
    {
        double Distance;
        double RelativeVelocity;
    } Radar;

    // struct Ibeo
    struct IbeoVariable
    {
        uint8_t ObjectID;
        int16_t X;
        int16_t Y;
        int16_t Vx;
        int16_t Vy;
        int16_t BoxOrientation;

        int BoxSizeX;
        int BoxSizeY;
        int ObjectID4;
        int Objectclassification;      
        int Object[100];
    } Ibeo;

    // struct Mobileye
};

struct GlobalPathVariable
{
    double Latitude[PathSize];
    double Longitude[PathSize];
    double ValidLatitude[PathSize];
    double ValidLongitude[PathSize];
    double GapLatitude;
    double GapLongitude;
    double Heading;
    double LocalizationGap;
};

struct LocalPathVariable
{
    double X[PathSize];
    double Y[PathSize];
    uint32_t Length;
};

struct ControlVariable
{
    double Curvature;
    double LateralDeviation;
    double RelativeHeadingAngle;
    uint32_t LookAheadIdx;
    double K;

    double Handle;
    double MPCHandle;
    double Acceleration;
};


// ------------------------------ Config ------------------------------------- //
extern const int TargetSpeed;
extern const bool ReceivePathFlag;
extern const bool ViewerFlag;
extern const bool GPSRecord;
extern const string GPSRecordPath;
extern const bool PathRecord;
extern const string PathRecordPath;
extern const bool VehicleRecord;
extern const string VehicleRecordPath;
extern const bool ControlRecord;
extern const string ControlRecordPath;
extern const string ReferenceFile;
extern const int MainCycle;

#endif