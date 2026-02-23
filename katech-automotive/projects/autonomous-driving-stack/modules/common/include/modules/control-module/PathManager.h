#ifndef PathManager_H
#define PathManager_H

#include "ControlModule.h"
#include "../shared/ModuleInterfaceCommon.h"

extern const double FrontLength;
extern const double OffsetLatitude;
extern const double OffsetLongitude;
extern const double Lat2meter;
extern const double Lon2meter;

extern GlobalPathVariable Global;
extern LocalPathVariable Local;
extern VehicleVariable Vehicle;
extern GPSVariable GPS;

extern bool PathErrorFlag;

class PathConvert
{
public:
    MODULES_COMMON_PATH_DECLARE_METHODS
    uint32_t WayPointNum;

private:
    MODULES_COMMON_PATH_DECLARE_INTERNALS(GPSVariable)
    MODULES_COMMON_PATH_DECLARE_FIELDS
};

#endif
