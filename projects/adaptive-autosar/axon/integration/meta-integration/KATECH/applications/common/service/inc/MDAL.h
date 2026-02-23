#pragma once
#include "MapStructs.h"

//******************************************************************
//		  KATECH HDMAP API
//******************************************************************
void setMapFilePath(const char* pPath);  												// 21년 1차 제공 API
LOCAL_MAP requestMapDataInGrid_ID(int nGridID);  										// 21년 1차 제공 API
// LOCAL_MAP requestMapDataInGrid_WGS84(double dLongitude, double dLatitude);
// LOCAL_MAP requestMapDataInGrid_UTM52N(double dX, double dY);

// LOCAL_MAP requestMapDataInCell_ID(int nGridID, int nCellID, int nCnt);
// LOCAL_MAP requestMapDataInCell_WGS84(double dLongitude, double dLatitude, int nCnt);
// LOCAL_MAP requestMapDataInCell_UTM52N(double dX, double dY, int nCnt);

void transWGStoUTM(double* pX_out, double* pY_out, double* pLon_in, double* pLat_in);  	// 21년 1차 제공 API
void transUTMtoWGS(double* pX_in, double* pY_in, double* pLon_out, double* pLat_out);  	// 21년 1차 제공 API
int requestGridID_WGS84(double dLongitude, double dLatitude);  							// 21년 1차 제공 API
int requestGridID_UTM52N(double dX, double dY);
