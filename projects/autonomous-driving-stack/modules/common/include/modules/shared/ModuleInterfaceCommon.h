#pragma once

#define MODULES_COMMON_COMM_EXTERN_FLAGS   \
    extern bool MainFlag;                  \
    extern bool SocketFlag;                \
    extern bool MCUSendSignal;             \
    extern bool SRCSendSignal;             \
    extern bool PathReceiveSignal;         \
    extern bool ViewerSendSignal;

#define MODULES_COMMON_COMM_EXTERN_STATE(GPS_TYPE, VEHICLE_TYPE, GLOBAL_TYPE, LOCAL_TYPE, CONTROL_TYPE) \
    extern GPS_TYPE GPS;                                                                                   \
    extern VEHICLE_TYPE Vehicle;                                                                           \
    extern GLOBAL_TYPE Global;                                                                             \
    extern LOCAL_TYPE Local;                                                                               \
    extern CONTROL_TYPE Control;

#define MODULES_COMMON_COMM_DECLARE_BASE_HANDLERS \
    void VehicleReceiver();                       \
    void Key();                                   \
    void MCUSender();                             \
    void ViewerSender();

#define MODULES_COMMON_CONTROLLER_TARGET_SPEED extern const int TargetSpeed;

#define MODULES_COMMON_CONTROLLER_STATE(VEHICLE_TYPE, GLOBAL_TYPE, LOCAL_TYPE, CONTROL_TYPE, GPS_TYPE) \
    extern VEHICLE_TYPE Vehicle;                                                                       \
    extern GLOBAL_TYPE Global;                                                                         \
    extern LOCAL_TYPE Local;                                                                           \
    extern CONTROL_TYPE Control;                                                                       \
    extern GPS_TYPE GPS;

#define MODULES_COMMON_PATH_DECLARE_METHODS \
    void ImportFile(const char *file);      \
    void InitializePath();                   \
    void GenerateLocalPath();

#define MODULES_COMMON_PATH_DECLARE_INTERNALS(GPS_TYPE) \
    void UpdatePosition(GPS_TYPE *pos);                \
    double CalCulateDistance(GPS_TYPE *pos1, GPS_TYPE *pos2); \
    void SetTargetVertex(uint32_t idx, GPS_TYPE *TargetPos);

#define MODULES_COMMON_PATH_DECLARE_FIELDS \
    uint32_t EndVertex;                    \
    uint32_t StartVertex;                  \
    uint32_t LastVertex;                   \
    uint32_t MinimumDistanceIdx;           \
    uint32_t FrontPathIdx;                 \
    double VertexDistance[PathSize];
