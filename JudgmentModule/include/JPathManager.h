#ifndef JPathManager_H
#define JPathManager_H

#include "JControlModule.h"

using namespace std;

#define FrontLength 50 // [m] 전방거리
#define PathDencity 0.8 // 속도에 따라서 변경, 저속: 20cm, 고속: 1m
#define OffsetLatitude 2.5 // 위도 offset
#define OffsetLongitude 2.5 // 경도 offset`

#define Lat2meter 110979.309
#define Lon2meter 88907.949

class PathConverter
{
    public:
        void ImportFile(const char *file);
        void InitializePath();
        void GenerateLocalPath();

        void PedestrianDistance(); // Vertex와 보행자 간의 최소거리
        double FrontVertexDistance; // 보행자 인지 시 앞으로 남은 전방거리
        uint32_t WayPointNum;

    private:
        void UpdatePosition(GPSStruct* pos);
        double CalCulateDistance(GPSStruct* pos1, GPSStruct* pos2);
        void SetTargetVertex(uint32_t idx, GPSStruct* TargetPos);

        uint32_t EndVertex; // [t] frontPath End index
        uint32_t StartVertex; // [t] frontPath Start index
        uint32_t LastVertex; // [t-1] position on Path
        uint32_t MinimumDistanceIdx;
        uint32_t FrontPathIdx;

        double VertexDistance[PathSize]; // Start vertex ~ End vertex (50m)까지 전방거리
};

#endif
