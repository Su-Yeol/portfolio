#ifndef Controller_H
#define Controller_H

#include "ControlModule/include/ControlModule.h"

#define Gravity 9.81
#define LookAheadGain 0.6

extern const int TargetSpeed;

extern VehicleVariable Vehicle;
extern GlobalPathVariable Global;
extern LocalPathVariable Local;
extern ControlVariable Control;
extern GPSVariable GPS;

class ControlLogic
{
    public:

        // Backup Logic
        void PurePursuit();
        void SCC();

        void GetLateraldeviation();
        void GetRelativeYawAngle();
        void GetCurvature();

    private:
        
};

#endif