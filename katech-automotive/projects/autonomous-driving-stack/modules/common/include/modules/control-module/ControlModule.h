#ifndef CONTROLMODULE_H
#define CONTROLMODULE_H

#include "../shared/ControlModuleCommon.h"

#define BufferSize 1024
#define PathSize 8192

MODULES_COMMON_USE_STD

struct GPSVariable
{
    MODULES_SHARED_GPS_FIELDS
};

struct RadarVariable
{
    MODULES_SHARED_RADAR_CORE_FIELDS
};

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
};

struct VehicleVariable
{
    MODULES_SHARED_VEHICLE_MOTION_FIELDS
    MODULES_SHARED_TURN_SWITCH_FIELDS
    uint8_t MDPSmode;
    RadarVariable Radar;
    IbeoVariable Ibeo;
};

struct GlobalPathVariable
{
    MODULES_SHARED_GLOBAL_PATH_FIELDS
    double LocalizationGap;
};

struct LocalPathVariable
{
    MODULES_SHARED_LOCAL_PATH_FIELDS
};

struct ControlVariable
{
    MODULES_SHARED_CONTROL_GEOMETRY_FIELDS
    uint32_t LookAheadIdx;
    double K;
    double Handle;
    double MPCHandle;
    double Acceleration;
};

MODULES_COMMON_CONTROL_BASE_CONFIG
extern const bool PathRecord;
extern const string PathRecordPath;
extern const bool VehicleRecord;
extern const string VehicleRecordPath;
extern const bool ControlRecord;
extern const string ControlRecordPath;
extern const int MainCycle;

#endif
