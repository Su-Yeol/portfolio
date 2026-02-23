#ifndef APathManager_H
#define APathManager_H

#include "AControlModule.h"
#include "../shared/ModuleInterfaceCommon.h"

#define FrontLength 0
#define PathDencity 1
#define OffsetLatitude 0
#define OffsetLongitude 0

#define Lat2meter 1
#define Lon2meter 1

extern GlobalPathStruct Global;
extern LocalPathStruct Local;
extern GPSStruct GPS;

class PathConverter
{
public:
    MODULES_COMMON_PATH_DECLARE_METHODS

private:
    MODULES_COMMON_PATH_DECLARE_INTERNALS(GPSStruct)

    uint32_t WayPointNum;
    MODULES_COMMON_PATH_DECLARE_FIELDS
};

#endif
