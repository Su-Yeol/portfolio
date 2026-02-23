#ifndef DPathManager_H
#define DPathManager_H

#include "DControlModule.h"
#include "../shared/ModuleInterfaceCommon.h"

class PathConverter
{
public:
    MODULES_COMMON_PATH_DECLARE_METHODS

    void PedestrianDistance();
    double FrontVertexDistance;
    double PreFrontVertexDistance;

    void IbeoPedestrianDistance();
    double IbeoFrontVertexDistance;
    double IbeoPreFrontVertexDistance;

    void PathHeadingAngle();
    uint32_t WayPointNum;

private:
    MODULES_COMMON_PATH_DECLARE_INTERNALS(GPSStruct)
    MODULES_COMMON_PATH_DECLARE_FIELDS
};
#endif
