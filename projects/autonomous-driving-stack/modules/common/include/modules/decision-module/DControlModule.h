#ifndef DCONTROLMODULE_H
#define DCONTROLMODULE_H

#include "../shared/ControlModuleCommon.h"

#define MainCycle 50
#define BufferSize 8192
#define PathSize 8192

MODULES_COMMON_USE_STD

extern const double FrontLength;
extern const double OffsetLatitude;
extern const double OffsetLongitude;
extern const double EarthRadius;
extern const double Lat2meter;
extern const double Lon2meter;
extern const double toRadian;
extern const double toDegree;

struct GPSStruct
{
    MODULES_SHARED_GPS_FIELDS
};

struct DecisionRadarField
{
    MODULES_SHARED_RADAR_CORE_FIELDS
    double X[16];
    double Y[16];
    double Latitude[16];
    double Longitude[16];
    double PathObjDist;
    double PedDistance[16];
    double WestMinPedDist;
    double EastMinPedDist;
    double MinPedDist;
    uint16_t MinPedIdx[16];
    uint16_t MinIdx;
    uint8_t PathObjectFlag;
};

struct VehicleStruct
{
    MODULES_SHARED_VEHICLE_MOTION_FIELDS
    MODULES_SHARED_TURN_SWITCH_FIELDS
    uint8_t MDPSmode;
    DecisionRadarField Radar;
};

struct GlobalPathStruct
{
    MODULES_SHARED_GLOBAL_PATH_FIELDS
    double LocalizationGap;
    uint8_t NowEnv;
    uint8_t PreEnv;
    double PreDist;
    double PathAngle;
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

struct MobileyeStruct
{
    double X[10];
    double Y[10];
    double Distance[10];
    double MinimumPedestrianDistance;
    uint16_t MinimumPedestrianIdx[10];
};

struct IbeoVariable
{
    uint8_t ObjectID;
    int16_t X;
    int16_t Y;
    int16_t Vx;
    int16_t Vy;
    int16_t BoxOrientation;
    int8_t Boxflag;
    uint16_t BoxSizeX;
    uint16_t BoxSizeY;
    int16_t Box[100];
    int16_t BoxCenterX;
    int16_t BoxCenterY;
    int Objectclassification;
    int ObjectCnt;
    double Object[100];
    double Latitude[30];
    double Longitude[30];
    double PathObjDist;
    double PedDistance[30];
    double WestMinPedDist;
    double EastMinPedDist;
    double MinPedDist;
    uint16_t PathObjIdx;
    uint16_t MinPedIdx[30];
    uint16_t MinIdx;
    uint8_t PathObjectFlag;
    uint8_t FaultCheckFlag;
    double FinalVertexDistance;
};

struct RadarStruct
{
    MODULES_SHARED_RADAR_CORE_FIELDS
    int ObjectCnt;
    double X[16];
    double Y[16];
    double Latitude[16];
    double Longitude[16];
    double PathObjDist;
    double PedDistance[16];
    double WestMinPedDist;
    double EastMinPedDist;
    double MinPedDist;
    uint16_t MinPedIdx[16];
    uint16_t MinIdx;
    uint8_t PathObjectFlag;
};

MODULES_COMMON_CONTROL_BASE_CONFIG
extern const bool PathRecord;
extern const string PathRecordPath;
extern const bool PedRecord;
extern const string PedDataPath;
extern const bool IbeoRecord;
extern const string IbeoDataPath;
extern const bool RadarRecord;
extern const string RadarDataPath;

extern bool MainFlag;
extern bool SocketFlag;
extern bool MCUSendSignal;
extern bool PathReceiveSignal;
extern bool PathErrorFlag;
extern int MobileyeFlag;
extern int IbeoFlag;
extern int RadarFlag;
extern bool ViewerSenderFlag;
extern char GPSRaw[100];

extern GPSStruct GPS;
extern GlobalPathStruct Global;
extern LocalPathStruct Local;
extern VehicleStruct Vehicle;
extern MobileyeStruct Mobileye;
extern IbeoVariable Ibeo;
extern RadarStruct Radar;

#endif
