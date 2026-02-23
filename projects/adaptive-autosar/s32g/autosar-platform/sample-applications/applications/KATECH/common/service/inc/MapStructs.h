#pragma once
#pragma pack(1)

#include <vector>
#include <stdint.h>

typedef unsigned char byte;
typedef unsigned short WORD;
typedef unsigned int UINT;
typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int UINT;

#if !(defined(_WIN64) || defined(_WIN32))
typedef int8_t __int8;
typedef int16_t __int16;
typedef int32_t __int32;
typedef int64_t __int64;
#endif

//------------------------------------------------------------------------------
//			도로 타입
//------------------------------------------------------------------------------
enum ObjTypeInRoad
{
    OTR_UNKWON,  // 0 : 미정의 객체
    OTR_PREVROAD,  // 1 : 진입도로 Road ID
    OTR_NEXTROAD,  // 2 : 진출도로 Road ID
    OTR_LANE,  // 3 : 차로 Lane ID
    OTR_LINE,  // 4 : 차선 Line ID
    OTR_CROSSWALK,  // 5 : 횡단보도 CrossWalk ID
    OTR_SPEEDBUMP,  // 6 : 과속방지턱 SpeedBump ID
    OTR_TRAFFICLIGHT,  // 7 : 신호등 TrafficLight ID
    OTR_STOPSIGN,  // 8 : 정지표지판 StopSign ID
    OTR_YIELDSIGN,  // 9 : 양보표지판 YieldSign ID
    OTR_LEFTBORDER,  // 10 : 좌측경계 Border ID
    OTR_RIGHTBORDER,  // 11 : 우측경계 Border ID
    OTR_NUMBER,  // 12 : 객체종류 개수
};

//------------------------------------------------------------------------------
//			보간점 좌표값
//------------------------------------------------------------------------------
typedef struct
{
    double dx;
    double dy;
    double dz;
} VERTEX_INFO;  // 24

//------------------------------------------------------------------------------
//			추출용 구조체
//------------------------------------------------------------------------------

typedef struct
{
    __int8 nRoadRank;  // 도로 등급
    __int8 nRoadType;  // 도로 유형
    __int8 nLinkType;  // 차로 유형
    __int8 nMaxSpeed;  // 최고제한속도
    __int8 nLaneNo;  // 차로 번호
    __int16 nVerticesNum;  // 버텍스 개수
    float fLength;  // 길이
    __int64 nID;  // 차로ID (정수형)
    __int64 nR_LaneID;  // 우측차로 ID (정수형)
    __int64 nL_LaneID;  // 좌측차로 ID (정수형)
    __int64 nFromNodeID;  // 시점노드 ID (정수형)
    __int64 nToNodeID;  // 종점노드 ID (정수형)
    __int64 nSectionID;  // 구간/영역 ID (정수형)
} LANE_INFO;

//				A5_PARKINGLOT
typedef struct
{
    __int16 nVerticesNum;  // 버텍스 개수
    __int16 nType;  // 주차장 유형
    __int64 nID;  // 주차장ID (정수형)
    __int64 nSelectID;  // 구간 ID (정수형)
} PARKINGLOT_INFO;

//				B1_SURFACEMAKR
typedef struct
{
    __int16 nRefLane;  // 참조 차로 수
    __int64 nID;  // 안전표지ID (정수형)
    __int64 nLinkID;  // 링크 ID (정수형)
    __int64 nPostID;  // 지주 ID (정수형)
} SIGN_INFO;

//				B2_SURFACELINEMAKR
typedef struct
{
    __int16 nVerticesNum;  // 버텍스 개수
    __int16 nType;  // 선 모양, 색상 정보
    __int16 nKind;  // 중앙선, 버스전용차선 등 종류 정보
    __int64 nID;  // 차선ID (정수형)
    __int64 nR_LinkID;  // 우측차로 ID  (정수형)
    __int64 nL_LinkID;  // 좌측차로 ID  (정수형)
} LINE_INFO;

//				B3_SURFACEMAKR
typedef struct
{
    __int8 nType;  // 표시 형태
    __int16 nKind;  // 표시 종류
    __int16 nVerticesNum;  // 버텍스 개수
    __int64 nID;  // 건널목ID (정수형)
    __int64 nLinkID;  // 인접 차로 ID (정수형)
} CROSSWALK_INFO;

//				C1_TRAFFICLIGHT
typedef struct
{
    __int16 nType;  // 교통 신호등 유형
    __int16 nRefLane;  // 참조 차로 수
    __int64 nID;  // 교통 신호등ID (정수형)
    __int64 nLinkID;  // 링크 ID (정수형)
    __int64 nPostID;  // 지주 ID (정수형)
} SIGNAL_INFO;

//				C3_VEHICLEPROTECTIONSAFTY
typedef struct
{
    __int16 nVerticesNum;  // 버텍스 개수
    __int16 nType;  // 시설유형
    __int16 nIsCentral;  // 중앙분리대
    __int16 nLowHigh;  // 상단, 하단
    __int64 nID;  // 차량방호시설ID (정수형)
    __int64 nRef_FID;  // 상하단 ID (정수형)
} BORDER_INFO;

//				C4_SPEEDBUMP
typedef struct
{
    __int16 nVerticesNum;  // 버텍스 개수
    __int16 nType;  // 과속방지턱 유형
    __int16 nRef_Lane;  // 참조 차로 수
    __int64 nID;  // 과속방지턱ID (정수형)
    __int64 nLinkID;  // 링크ID (정수형)
} SPEEDBUMP_INFO;

//------------------------------------------------------------------------------
//			(객체 정보 + 선형 정보) 구조체
//------------------------------------------------------------------------------
typedef struct
{
    LANE_INFO sInfo;  //
    std::vector<VERTEX_INFO> vGeometry;  // 가변
} LANE_VTX;  // 가변

typedef struct
{
    PARKINGLOT_INFO sInfo;  //
    std::vector<VERTEX_INFO> vGeometry;  // 가변
} PARKINGLOT_VTX;  // 가변

typedef struct
{
    SIGN_INFO sInfo;  //
    VERTEX_INFO sGeometry;  // 24
} SIGN_VTX;  //

typedef struct
{
    LINE_INFO sInfo;  //
    std::vector<VERTEX_INFO> vGeometry;  // 가변
} LINE_VTX;  // 가변

typedef struct
{
    CROSSWALK_INFO sInfo;  //
    std::vector<VERTEX_INFO> vGeometry;  // 가변
} CROSSWALK_VTX;  // 가변

typedef struct
{
    SIGNAL_INFO sInfo;  //
    VERTEX_INFO sGeometry;  // 24
} SIGNAL_VTX;  //

typedef struct
{
    BORDER_INFO sInfo;  //
    std::vector<VERTEX_INFO> vGeometry;  // 가변
} BORDER_VTX;  // 가변

typedef struct
{
    SPEEDBUMP_INFO sInfo;
    std::vector<VERTEX_INFO> vGeometry;
} SPEEDBUMP_VTX;

//------------------------------------------------------------------------------
//			데이터 제공용 구조체
//------------------------------------------------------------------------------
typedef struct
{
    __int64 nID;
    __int64 nJunctionID;
    std::vector<__int64> nPredecessorID;
    std::vector<__int64> nSuccessorID;
    double dLength;
    __int8 nLaneCnt;
    std::vector<LANE_VTX> vLanes;
    std::vector<LINE_VTX> vLines;
    std::vector<CROSSWALK_VTX> vCrosswalks;
    std::vector<SIGNAL_VTX> vSignals;
    std::vector<SIGN_VTX> vStopsigns;
    std::vector<SIGN_VTX> vYieldsigns;
    std::vector<SPEEDBUMP_VTX> vSpeedbumps;
} ROAD;

typedef struct
{
    __int8 nFrom;
    __int8 nTo;
} LANELINK;

typedef struct
{
    __int8 nID;
    __int64 nIncomingRoadID;
    __int64 nConnectingRoadID;
    __int8 nContactPoint;
    __int8 nType;
    std::vector<LANELINK> vLaneLinkInfo;
} CONNECTION;

typedef struct
{
    __int64 nID;
    __int8 nType;
    std::vector<CONNECTION> vConnections;
} JUNCTION;

typedef struct
{
    std::vector<ROAD> vRoads;
    std::vector<JUNCTION> vJunctions;
} ROAD_JUNCTION_LIST;

typedef struct
{
    __int64 nID;
    std::vector<ROAD> vRoads;
    std::vector<JUNCTION> vJunctions;
    std::vector<LINE_VTX> vStopLines;
    std::vector<BORDER_VTX> vBorders;
    std::vector<PARKINGLOT_VTX> vParkinglots;
    std::vector<CROSSWALK_VTX> vCrosswalks_out;
} LOCAL_MAP;
