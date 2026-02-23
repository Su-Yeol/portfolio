#ifndef ACONTROLMODULE_H
#define ACONTROLMODULE_H

#include "../shared/ControlModuleCommon.h"

#define BufferSize 8192
#define PathSize 8192

MODULES_COMMON_USE_STD

struct GPSStruct
{
    MODULES_SHARED_GPS_FIELDS
};

struct AebRadarStruct
{
    MODULES_SHARED_RADAR_CORE_FIELDS
};

struct VehicleStruct
{
    MODULES_SHARED_VEHICLE_MOTION_FIELDS
    MODULES_SHARED_TURN_SWITCH_FIELDS
    AebRadarStruct Radar;
};

struct GlobalPathStruct
{
    MODULES_SHARED_GLOBAL_PATH_FIELDS
};

struct LocalPathStruct
{
    MODULES_SHARED_LOCAL_PATH_FIELDS
};

struct ControlStruct
{
    MODULES_SHARED_CONTROL_GEOMETRY_FIELDS
    MODULES_SHARED_CONTROL_ACTUATOR_FIELDS
};

MODULES_COMMON_CONTROL_BASE_CONFIG
extern const int MainCycle;

#endif
