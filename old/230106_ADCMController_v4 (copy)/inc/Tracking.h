#include <vector>
#include <algorithm>

using namespace std;

typedef struct gps_t{
	double longi;
	double lati;
	double azimuth;
} gps_t;
typedef int			int32_t;

extern bool exit_flag;

extern bool global_viewer_stopflg;

extern double global_utc, global_azimuth, global_latitude, global_longitude;
extern char global_gpsstat;
extern double global_handle, global_vehicle_spd, global_target_spd;
extern uint32_t target_cnt;

#define REF_SIZE 100000 // 221130 size upp 
extern double global_distance[REF_SIZE];
extern double REF_Utc[REF_SIZE], REF_Azi[REF_SIZE], REF_Lat[REF_SIZE], REF_Lon[REF_SIZE], REF_Spd[REF_SIZE];

extern char REF_Stat[REF_SIZE];

extern double diff_lati_m, diff_longi_m, azimuth_rad, diff_azimuth_rad;
extern double global_cte;
// extern double path_50m_origin_x[REF_SIZE], path_50m_origin_y[REF_SIZE];
// extern double temp_x[REF_SIZE], temp_y[REF_SIZE];

extern std::vector<double> REF_Lat_plot, REF_Lon_plot;
extern std::vector<double> path_50m_origin_x, path_50m_origin_y;
extern std::vector<double> path_log_Longitude, path_log_Latitude;
extern std::vector<double> temp_x, temp_y;

extern std::vector<double> trafficlight_x, trafficlight_y, trafficlight_dist;

extern bool trafficlight_chk;
extern uint8_t trafficlight_signal, Objdist;
extern uint16_t trafficlight_cnt;
extern uint8_t pedestrian_stop;


extern std::vector<double> waypoint_x, waypoint_y;
extern std::vector<int> waypoint_velocity;

extern double temp_lati0;
extern double temp_lati1;
extern double temp_lati2;
extern double temp_longi0;
extern double temp_longi1;
extern double temp_longi2;
extern double d1;
extern double d2;
extern double cx;
extern double cy;
extern double r;

extern double target_c0, target_c1, target_c2, target_c3;

extern int32_t global_waypoint_select;
extern int32_t global_nearpoint_select;
extern double global_heading;
extern int global_tracking_status;
extern int global_waypoint_cnt;

extern gps_t destination_position;
extern int path_50m_end_trace;
extern int path_50m_start_trace;
extern int global_last_trace;
extern int global_lookahead_cnt, global_min_distance_cnt;
extern double look_ahead_distance;
extern double Out_C0, Out_C1;
extern uint32_t my_count;
extern int zoom_level;

extern double gain;
extern double max_lookahead_distance;
extern double front_path_distance;
extern double latitude_offset;
extern double longitude_offset;

extern uint8_t TurnSignal;
extern int SpdControl;
extern double stanley_theta;
extern double global_curvature;

extern uint8_t longi_target_spd;
extern double global_curr_ay, global_pre_ay, global_vx;

extern bool XStopFlg;
extern uint8_t XStopDistance;
extern double FrontSCC_ObjDstVal;

extern gps_t SpdFlg[2];
extern std::vector<double> turnsig_longi, turnsig_lati, turnsig_v;

void RoadInfo_init(void);
void SpdProfile(gps_t* current_pos);
void get_first_path_50m(gps_t* current_pos);
int get_path_50m(gps_t *current_position);
void get_current_gps(gps_t* current_position);
void get_destination_from_file(int  waypoint_select, gps_t* destination_position);
double get_lookahead_distance(double current_velocity);
void get_initial_destination(gps_t* current_position);
double get_distance(gps_t* current_position, gps_t* destination_position);
double get_obj_distance(double y, double x, gps_t *destination_position);
double get_handle(double handle_radius, double look_ahead_distance);
void GPS_Tracking_init(void);
double GPS_Tracking(void);
void Viewer(void);
void TargetSpd_Control(double cur_k, int path_cnt);