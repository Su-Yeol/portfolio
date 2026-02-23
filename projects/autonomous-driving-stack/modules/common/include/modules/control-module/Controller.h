#ifndef Controller_H
#define Controller_H

#include "ControlModule.h"
#include "../shared/ModuleInterfaceCommon.h"

extern const double kControlGravity;
extern const double kControlLookAheadGain;
extern const double kControlTargetVelocityScale;

MODULES_COMMON_CONTROLLER_TARGET_SPEED
MODULES_COMMON_CONTROLLER_STATE(VehicleVariable, GlobalPathVariable, LocalPathVariable, ControlVariable, GPSVariable)

class ControlLogic
{
public:
    void PurePursuit();
    void SCC();

    void GetLateraldeviation();
    void GetRelativeYawAngle();
    void GetCurvature();

private:
};

#endif
