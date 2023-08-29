#ifndef PathManager_H
#define PathManager_H

#include "ControlModule/include/ControlModule.h"

using namespace std;

#define FrontLength 50 // [m]

#define OffsetLatitude 2.5
#define OffsetLongitude 2.5

#define Lat2meter 110979.309
#define Lon2meter 88907.949

extern GlobalPathVariable Global;
extern LocalPathVariable Local;
extern VehicleVariable Vehicle;
extern GPSVariable GPS;

extern bool PathErrorFlag;

class PathConvert
{
    public:
        void ImportFile(const char *file);
        void InitializePath();
        void GenerateLocalPath();
        uint32_t WayPointNum;

    private:
        void UpdatePosition(GPSVariable* pos);
        double CalCulateDistance(GPSVariable* pos1, GPSVariable* pos2);
        void SetTargetVertex(uint32_t idx, GPSVariable* TargetPos);

        uint32_t EndVertex; // [t] frontPath End index
        uint32_t StartVertex; // [t] frontPath Start index
        uint32_t LastVertex; // [t-1] position on Path
        uint32_t MinimumDistanceIdx;
        uint32_t FrontPathIdx;

        double VertexDistance[PathSize];
};

#endif