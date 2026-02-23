#include "sensor/gps/proj_converter.h"
#include <proj.h>
#include "logger.h"
#include <math.h>

namespace adcm
{
namespace sensor
{
namespace gps
{

ProjConverter::ProjConverter(UTM_Type type)
{
    // Custom projection parameters for Transverse Mercator
    // utm-k - Bessel // EPSG 5178  // https://epsg.org/crs_5178/Korean-1985-Unified-CS.html?sessionkey=iug5mxhyps
    // const PROJ_STR = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=bessel +units=m +no_defs +towgs84=-115.80,474.99,674.11,1.16,-2.31,-1.63,6.43";                             // �?��지리정보원 고시 ??002-433??    // const PROJ_STR = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=bessel +towgs84=-145.907,505.034,685.756,-1.162,2.347,1.592,6.342 +units=m +no_defs +type=crs"   // epsg.io // �?��지리정보원 고시 ??003-497??
    // utmk - GRS80 // EPSG 5179 // naver
    // const PROJ_STR = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=GRS80 +units=m +no_defs"
    // const PROJ_STR = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs"    // epsg.io
    // utm 52N // EPSG 32652
    // const PROJ_STR = "+proj=utm +zone=52 +ellps=WGS84 +datum=WGS84 +units=m +no_defs" // telecons
    // const PROJ_STR = "+proj=utm +zone=52 +datum=WGS84 +units=m +no_defs +type=crs"    // epsg.io, KATECH
    // Google Maps
    // const PROJ_STR = "+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +no_defs";
    proj_str_GNSS = "+proj=longlat +ellps=WGS84";
    std::string zone_str = "unknow";

    switch(type) {
    case UTM_Type::WATER_TREATMENT_PLANT:
    case UTM_Type::KAKAO:
    case UTM_Type::EPSG_5181:
        lat_0 = 38;
        lon_0 = 127;
        k = 1;
        x_0 = 200000;
        y_0 = 500000;
        zone_str = "EPSG_5181, KAKAO, WATER_TREATMENT_PLANT";
        proj_str_UTM = "+proj=tmerc +lat_0=38 +lon_0=127 +k=1 +x_0=200000 +y_0=500000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs +type=crs";
        offset_ssm_x = 0.333;
        offset_ssm_y = 4207592.517;
        break;

    case UTM_Type::UTM_K:
    case UTM_Type::EPSG_5179:
    case UTM_Type::K_CITY:
    case UTM_Type::NAVER:
        lat_0 = 38;
        lon_0 = 127.5;
        k = 0.9996;
        x_0 = 1000000;
        y_0 = 2000000;
        zone_str = "EPSG_5179, UTM_K, NAVER, K_CITY";
        proj_str_UTM = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=GRS80 +units=m +no_defs";
        offset_ssm_x = -1.007-1.76953;
        offset_ssm_y = 4205909.478+2.55;
        break;

    case UTM_Type::KATECH:
    case UTM_Type::UTM_54N:
    case UTM_Type::EPSG_32654:
        lat_0 = 0;
        lon_0 = 141;
        k = 0.9996;
        x_0 = 500000;
        y_0 = 0;
        zone_str = "EPSG_32654, UTM_54N, KATECH";
        proj_str_UTM = "+proj=utm +zone=54 +datum=WGS84 +units=m +no_defs +type=crs";
        offset_ssm_x = 1553.260;
        offset_ssm_y = 83.757;
        break;

    case UTM_Type::UTM_52N:
    case UTM_Type::EPSG_32652:
        lat_0 = 0;
        lon_0 = 129;
        k = 0.9996;
        x_0 = 500000;
        y_0 = 0;
        zone_str = "EPSG_32652, UTM_52N";
        //proj_str_UTM = "+proj=utm +zone=52 +datum=WGS84 +units=m +no_defs +type=crs";       // EPSG.org
        proj_str_UTM = "+proj=utm +zone=52 +ellps=WGS84 +datum=WGS84 +units=m +no_defs";    // telecons
        offset_ssm_x = 205.656;
        offset_ssm_y = 94.452;
        break;

    default:        // UTM_K, EPSG_5179
        lat_0 = 38;
        lon_0 = 127.5;
        k = 0.9996;
        x_0 = 1000000;
        y_0 = 2000000;
        zone_str = "EPSG_5179, UTM_K, NAVER, K_CITY";
        proj_str_UTM = "+proj=tmerc +lat_0=38 +lon_0=127.5 +k=0.9996 +x_0=1000000 +y_0=2000000 +ellps=GRS80 +units=m +no_defs";
        offset_ssm_x = -1.007;
        offset_ssm_y = 4205909.476;
        break;
    }

    INFO("Zone = %s", zone_str.c_str());
    INFO("PROJ_STR = %s", proj_str_UTM.c_str());
    mProjInstance = proj_create_crs_to_crs(PJ_DEFAULT_CTX, proj_str_GNSS.c_str(), proj_str_UTM.c_str(), NULL);

    if(mProjInstance == 0) {
        ERROR("proj instance has error (%d)", mProjInstance);

    } else {
        INFO("proj instance init complete!!");
    }

    double phi_0 = lat_0 * M_PI / 180.0;
    nu = R / sqrt(1 - e * e * sin(phi_0) * sin(phi_0));
    double eta_sq = (nu * nu - R * R) / (R * R);
}

void ProjConverter::convert_ssm(double& utm_x, double& utm_y, double& lon, double& lat)
{
    double phi = lat * M_PI / 180.0; // Convert latitude to radians
    double lambda = lon * M_PI / 180.0; // Convert longitude to radians
    double phi_0 = lat_0 * M_PI / 180.0; // Convert central latitude to radians
    double lambda_0 = lon_0 * M_PI / 180.0; // Convert central longitude to radians
    double nu_1 = R / sqrt(1 - e * e * sin(phi) * sin(phi));
    double rho = nu * (nu_1 / R);
    double eta_sq = (nu * nu - R * R) / (R * R);
    double A = (lambda - lambda_0) * cos(phi);
    double A2 = A * A;
    double A3 = A * A2;
    double A4 = A2 * A2;
    double A5 = A2 * A3;
    double A6 = A3 * A3;
    double m = R * (phi * (1 - e * e / 4 - 3 * e * e * e * e / 64 + 5 * e * e * e * e * e * e / 256) - sin(2 * phi) * (3 * e * e / 8 + 3 * e * e * e * e / 32 - 45 * e * e * e * e * e * e / 1024) + sin(4 * phi) * (15 * e * e * e / 256 - 45 * e * e * e * e * e * e / 1024) - sin(6 * phi) * (35 * e * e * e * e * e * e / 3072));
    utm_x = x_0 + k * nu * (A + A3 / 6 * (1 - tan(phi) * tan(phi) + eta_sq) + A5 / 120 * (5 - 18 * tan(phi) * tan(phi) + tan(phi) * tan(phi) * tan(phi) * tan(phi) + 72 * eta_sq - 58 * eta_sq * tan(phi) * tan(phi)));
    utm_y = y_0 + k * (m + nu_1 * tan(phi) * (A2 / 2 + A4 / 24 * (5 - tan(phi) * tan(phi) + 9 * eta_sq + 4 * eta_sq * eta_sq) + A6 / 720 * (61 - 58 * tan(phi) * tan(phi) + tan(phi) * tan(phi) * tan(phi) * tan(phi))));
    return;
}

void ProjConverter::convert(double& utm_x, double& utm_y, double& lon, double& lat)
{
    PJ_COORD c_in, c_out;
    c_in.lpzt.z = 0.0;
    c_in.lpzt.t = HUGE_VAL;
    c_in.lpzt.lam = lon;
    c_in.lpzt.phi = lat;
    c_out = proj_trans(mProjInstance, PJ_FWD, c_in);
    utm_x = c_out.xy.x;
    utm_y = c_out.xy.y;
    return;
}

void ProjConverter::inverse(double& utm_x, double& utm_y, double& lon, double& lat)
{
    PJ_COORD c_in, c_out;
    c_in.xy.x = utm_x;
    c_in.xy.y = utm_y;
    c_out = proj_trans(mProjInstance, PJ_INV, c_in);
    lon = c_out.lpzt.lam;
    lat = c_out.lpzt.phi;
    return;
}


#define _UTM_X_START_                                      237146.80                        // UTM52N
#define _UTM_X_END_                                                554434.80                         // UTM52N
#define _UTM_Y_START_                                      4277945.98                       // UTM52N
#define _UTM_Y_END_                                                3673121.98                        // UTM52N
#define _GRID_LENGTH_                                      250                                       // m
#define _CELL_LENGTH_                                      50                                        // m
#define _NUM_OF_GRID_X_                                    1272                             // Count
#define _NUM_OF_GRID_Y_                                    2424                             // Count
#define _IDX_OF_CELL_                                      5                                         // Count
#define _NUM_OF_CELL_IN_GRID_                     25                                        // Count



std::int32_t calculateGridID_UTM52N(double _dX, double _dY)

{
    int nGridID = (((int)((_dX - _UTM_X_START_) / _GRID_LENGTH_) * _NUM_OF_GRID_Y_)) + ((int)((_UTM_Y_START_ - _dY) / _GRID_LENGTH_));

    if(nGridID > 0) {
        return nGridID;

    } else {
        return -1;
    }
}



std::int32_t calculateCellID_UTM52N(double _dX, double _dY)

{
    std::int32_t nGridID = calculateGridID_UTM52N(_dX, _dY);
    int nCellID = (std::int32_t)((std::int32_t)(((_UTM_Y_START_ - (_GRID_LENGTH_ * (nGridID % _NUM_OF_GRID_Y_))) - _dY) / _CELL_LENGTH_)) +
                  ((std::int32_t)((_dX - (_UTM_X_START_ + (_GRID_LENGTH_ * (nGridID / _NUM_OF_GRID_Y_)))) / _CELL_LENGTH_) * _IDX_OF_CELL_);

    if(nCellID > 0) {
        return nCellID;

    } else {
        return -1;
    }
}

int ProjConverter::getGidCell(double utm_x, double utm_y, int& Grid, int& Cell)
{
    std::int32_t nGridID = calculateGridID_UTM52N(utm_x, utm_y);
    int nCellID = (std::int32_t)((std::int32_t)(((_UTM_Y_START_ - (_GRID_LENGTH_ * (nGridID % _NUM_OF_GRID_Y_))) - utm_y) / _CELL_LENGTH_)) +
                  ((std::int32_t)((utm_x - (_UTM_X_START_ + (_GRID_LENGTH_ * (nGridID / _NUM_OF_GRID_Y_)))) / _CELL_LENGTH_) * _IDX_OF_CELL_);

    if(nCellID > 0) {
        Grid = nGridID;
        Cell = nCellID;
        return 0;

    } else {
        return -1;
    }
}

}
}
}
