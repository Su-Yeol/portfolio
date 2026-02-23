#ifndef PROJ_CONVERTER_H
#define PROJ_CONVERTER_H

#include <cmath>
#include <string>
#include <proj.h>

namespace adcm
{
namespace sensor
{
namespace gps
{

class ProjConverter
{

public:
    static constexpr double SUNGNAM_WATER_LAT_MIN = 37.45678787;
    static constexpr double SUNGNAM_WATER_LAT_MAX = 37.46729672;
    static constexpr double SUNGNAM_WATER_LON_MIN = 127.11715937;
    static constexpr double SUNGNAM_WATER_LON_MAX = 127.12591410;


    static constexpr double KCITY_LAT_MIN = 37.22728723;
    static constexpr double KCITY_LAT_MAX = 37.24922182;
    static constexpr double KCITY_LON_MIN = 126.76664829;
    static constexpr double KCITY_LON_MAX = 126.77677631;

    enum class UTM_Type {
        UTM_K,
        UTM_52N,
        UTM_54N,
        EPSG_5179,
        EPSG_5181,
        EPSG_32652,
        EPSG_32654,        
        KATECH,
        WATER_TREATMENT_PLANT,
        K_CITY,
        KAKAO,
        NAVER,
        UTM_UNKNOW
    };

    ProjConverter(UTM_Type type);

    void convert_ssm(double& utm_x, double& utm_y, double& lon, double& lat);
	void inverse_ssm(double& lat, double& lon, double& utm_x, double& utm_y);

    void convert(double& utm_x, double& utm_y, double& lon, double& lat);
    void inverse(double& utm_x, double& utm_y, double& lon, double& lat);
    int getGidCell(double utm_x, double utm_y, int& Grid, int& Cell);

    double offset_ssm_x;
    double offset_ssm_y;
private:
    std::string proj_str_GNSS;
    std::string proj_str_UTM;
    PJ *mProjInstance;


    const double R = 6378137; // Earth's radius (meters) for GRS80 ellipsoid
    const double e = 0.08181919104281579; // Eccentricity for GRS80 ellipsoid

    double lat_0; // Central latitude of Transverse Mercator projection
    double lon_0; // Central longitude of Transverse Mercator projection
    double k; // Scale factor for Transverse Mercator projection
    double x_0; // False easting for Transverse Mercator projection
    double y_0; // False northing for Transverse Mercator projection
    double nu; // Precomputed constant for the projection
    double eta_sq; // Square of the seconed eccentricity
};
}
}
}
#endif //PROJ_CONVERTER_H
