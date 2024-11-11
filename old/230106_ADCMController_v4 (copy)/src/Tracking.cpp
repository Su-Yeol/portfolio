#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <vector>

#include "Tracking.h"
#include "DecisionMaking.h"

using namespace std;

#ifdef MATPLOTLIBCPP
#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;
#endif

enum Tracking_Status
{
	T_INIT,
	T_FIRST_WAYPOINT,
	T_TRACKING,
	T_GOAL
};

enum Velocity_Status
{
	V_INIT,
	V_ACC,
	V_CV,
	V_DCC,
	V_STOP
};

bool global_viewer_stopflg = 0;

double global_current_velocity_prev;
double global_current_velocity;
double global_current_handle;
double global_heading;
int32_t global_waypoint_cnt;
int32_t global_waypoint_select;
int32_t global_nearpoint_select;
int32_t global_waypoint_goal;
int global_tracking_status;
int global_velocity_status;

double global_utc, global_azimuth, global_latitude, global_longitude;
char global_gpsstat;
double global_handle, global_vehicle_spd, global_target_spd = 30;
uint32_t target_cnt = 0;

double global_distance[REF_SIZE];
double REF_Utc[REF_SIZE], REF_Azi[REF_SIZE], REF_Lat[REF_SIZE], REF_Lon[REF_SIZE], REF_Spd[REF_SIZE];
char REF_Stat[REF_SIZE];

double diff_lati_m, diff_longi_m, azimuth_rad;
double Out_C0, Out_C1;
uint32_t my_count = 0;

std::vector<double> REF_Lat_plot, REF_Lon_plot;
std::vector<double> path_50m_origin_x, path_50m_origin_y;
std::vector<double> path_log_Longitude, path_log_Latitude;
std::vector<double> temp_x, temp_y;

std::vector<double> waypoint_x, waypoint_y;
std::vector<int> waypoint_velocity, waypoint_flag;
int wflag = 1, wcnt = 0;
double waypoint_dist[100] = {
	500,
};

// double path_50m_origin_x[REF_SIZE], path_50m_origin_y[REF_SIZE];
// double temp_x[REF_SIZE], temp_y[REF_SIZE];

double target_c0 = 0.0;
double target_c1 = 0.0;
double target_c2 = 0.0;
double target_c3 = 0.0;

double temp_lati0;
double temp_lati1;
double temp_lati2;
double temp_longi0;
double temp_longi1;
double temp_longi2;

double d1;
double d2;
double cx;
double cy;
double r;

gps_t destination_position;
gps_t next_destination_position;
gps_t nearlest_position;
gps_t next_near_position;
double current_near_distance, next_near_distance;
double look_ahead_distance;

int path_50m_end_trace = 0;
int path_50m_start_trace = 0;
int global_last_trace = 0;
int global_lookahead_cnt = 0, global_min_distance_cnt = 0;
double path_dencity = 0.8;
int path_log_len = 500; // 300 sec

double stanley_target_x = 0, stanley_target_y = 0, stanley_theta = 0;

// control param
double latitude_offset = 2.5;
double longitude_offset = 2.5;
double gain = 0.5;
double max_lookahead_distance = 100.0;
double front_path_distance = 50.0;
double global_cte = 0.0;

double global_curr_ay, global_pre_ay, global_vx;
double global_curvature = 0.0;

bool XStopFlg = 0;
uint8_t XStopDistance = 50;

int zoom_level = 15;

uint16_t global_lookahead_idx = 0;
double _current_distance = 0.0, _total_distance = 0.0, current_k = 0.0;
double FrontSCC_ObjDstVal = 0.0;

gps_t SpdFlg[2];
std::vector<double> turnsig_longi, turnsig_lati, turnsig_v;

void GPS_Tracking_init(void)
{
	global_current_handle = 0;
	global_waypoint_cnt = 1;
	global_waypoint_select = 0;
	global_tracking_status = T_INIT;
	global_velocity_status = V_INIT;
	global_current_velocity_prev = 0;
	global_current_velocity = 0;
	printf("Tracking init\n");
}

void RoadInfo_init(void)
{

	const int X = 0;
	const int L = 1;
	const int R = 2;

	// TurnSig 
	turnsig_longi.push_back(126.77334346);
	turnsig_lati.push_back(37.23950067);
	turnsig_v.push_back(R);

	turnsig_longi.push_back(126.77356752);
	turnsig_lati.push_back(37.23968524);
	turnsig_v.push_back(X);

	turnsig_longi.push_back(126.77368547);
	turnsig_lati.push_back(37.23964162);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77408008);
	turnsig_lati.push_back(37.23966885);
	turnsig_v.push_back(X);

	turnsig_longi.push_back(126.77427269);
	turnsig_lati.push_back(37.24044820);
	turnsig_v.push_back(R);

	turnsig_longi.push_back(126.77408733);
	turnsig_lati.push_back(37.24063501);
	turnsig_v.push_back(X);
	//---
	turnsig_longi.push_back(126.77450751);
	turnsig_lati.push_back(37.24199015);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77446195);
	turnsig_lati.push_back(37.24226546);
	turnsig_v.push_back(X);

	turnsig_longi.push_back(126.77445647);
	turnsig_lati.push_back(37.24241153);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77422413);
	turnsig_lati.push_back(37.24282625);
	turnsig_v.push_back(X);
	//---
	turnsig_longi.push_back(126.77378239);
	turnsig_lati.push_back(37.24296080);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77371141);
	turnsig_lati.push_back(37.24354479);
	turnsig_v.push_back(X);

	turnsig_longi.push_back(126.77400782);
	turnsig_lati.push_back(37.24382958);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77443140);
	turnsig_lati.push_back(37.24393724);
	turnsig_v.push_back(X);
	//----
	turnsig_longi.push_back(126.77322994);
	turnsig_lati.push_back(37.24540914);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77327176);
	turnsig_lati.push_back(37.24586817);
	turnsig_v.push_back(X);

	turnsig_longi.push_back(126.77334447);
	turnsig_lati.push_back(37.24592933);
	turnsig_v.push_back(R);

	turnsig_longi.push_back(126.77345203);
	turnsig_lati.push_back(37.24600707);
	turnsig_v.push_back(X);
	//---
	turnsig_longi.push_back(126.7750543);
	turnsig_lati.push_back(37.2452264);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.77514644);
	turnsig_lati.push_back(37.24381201);
	turnsig_v.push_back(X);

	// ---

	turnsig_longi.push_back(126.7753051);
	turnsig_lati.push_back(37.2397567);
	turnsig_v.push_back(L);

	turnsig_longi.push_back(126.7754051);
	turnsig_lati.push_back(37.2399358);
	turnsig_v.push_back(R);

	turnsig_longi.push_back(126.7751284);
	turnsig_lati.push_back(37.2387621);
	turnsig_v.push_back(X);


	// SpdFlg
	SpdFlg[0].longi = 126.77448073;
	SpdFlg[0].lati  = 37.24176716;
	SpdFlg[1].longi = 126.77445451;
	SpdFlg[1].lati  = 37.24249629;

	// [RoadJunction1] start point
	RoadJunction[0].longi = 126.7737803;
	RoadJunction[0].lati  = 37.2429554;
	// [RoadJunction1] end point
	RoadJunction[1].longi = 126.7737644;
	RoadJunction[1].lati  = 37.2433211;
	// [RoadJunction2] start point
	RoadJunction[2].longi = 126.7750492;
	RoadJunction[2].lati  = 37.2452882;
	// [RoadJunction2] end point
	RoadJunction[3].longi = 126.775104;
	RoadJunction[3].lati  = 37.2440959;

	// [Crossroad] start point
	CrossWalk[0].longi = 126.7744586;
	CrossWalk[0].lati  = 37.2423511;

	// [Crossroad] end point
	CrossWalk[1].longi = 126.7741809;
	CrossWalk[1].lati  = 37.2428248;

	// [CrossWalk_TrafficIsland] end point
	CrossWalk[2].longi = 126.773794;
	CrossWalk[2].lati  = 37.2428968;

	// [Roundabout] 
	Roundabout[0].longi = 126.7732427;
	Roundabout[0].lati  = 37.2451099;
	Roundabout[1].longi = 126.7732238;
	Roundabout[1].lati  = 37.2458168;

	// waypoint_y.push_back(36.743161);
	// waypoint_x.push_back(127.116264);
	// waypoint_velocity.push_back(10);
	// waypoint_flag.push_back(1);

	// trafficlight_y.push_back(37.2416087);
	// trafficlight_x.push_back(126.7744658);
	
	// trafficlight_y.push_back(37.2416087);
	// trafficlight_x.push_back(126.7744658);


	/* community road : Go */
	// stopline_y.push_back(37.2396299);
	// stopline_x.push_back(126.7734213);

	stopline_y.push_back(37.2399695);
	stopline_x.push_back(126.7736591);
	
	stopline_y.push_back(37.2404086);
	stopline_x.push_back(126.7739507);
	
	/* community road : Back */
	// stopline_y.push_back(37.2398407);
	// stopline_x.push_back(126.7734963);

	stopline_y.push_back(37.2403113);
	stopline_x.push_back(126.7738058);
	
	stopline_y.push_back(37.2406874);
	stopline_x.push_back(126.7740248);

	/* community road : Right */
	// stopline_y.push_back(37.2397937);
	// stopline_x.push_back(126.7741564);

	stopline_y.push_back(37.2404939);
	stopline_x.push_back(126.7741611);
	
	// stopline_y.push_back(37.240486);
	// stopline_x.push_back(126.7732925);

	// stopline_y.push_back(37.2397689);
	// stopline_x.push_back(126.7733769);

	/* community road : Left */
	stopline_y.push_back(37.2397204);
	stopline_x.push_back(126.7735798);

	stopline_y.push_back(37.2400721);
	stopline_x.push_back(126.7742947);
	
	stopline_y.push_back(37.2406025);
	stopline_x.push_back(126.7738176);

	/* community road : Center */
	stopline_y.push_back(37.2402137); // left-right
	stopline_x.push_back(126.7735336);
	// stopline_y.push_back(37.2403076); // left-left
	// stopline_x.push_back(126.773392);

	stopline_y.push_back(37.2399898); // right-right
	stopline_x.push_back(126.7740668);
	stopline_y.push_back(37.2400784); // right-left
	stopline_x.push_back(126.7739403);

	// stopline_y.push_back(37.2402276);
	// stopline_x.push_back(126.7731667);
	
	/* urban road : Go */
	// stopline_y.push_back(37.2415495);
	// stopline_x.push_back(126.7744763);
	stopline_y.push_back(37.2415256);
	stopline_x.push_back(126.7744952);
	
	stopline_y.push_back(37.2424991);
	stopline_x.push_back(126.7744735);
	
	stopline_y.push_back(37.2437394);
	stopline_x.push_back(126.7744395);

	/* urban road : Back */
	stopline_y.push_back(37.2439858);
	stopline_x.push_back(126.7743659);
	
	stopline_y.push_back(37.2430407);
	stopline_x.push_back(126.7743782);
	
	stopline_y.push_back(37.2418263);
	stopline_x.push_back(126.7743928);

	/* urban road : Right */
	stopline_y.push_back(37.242907);
	stopline_x.push_back(126.7736704);
	
	stopline_y.push_back(37.2416697);
	stopline_x.push_back(126.7742508);
	
	/* urban road : Left */
	stopline_y.push_back(37.241711);
	stopline_x.push_back(126.7746632);
	
	stopline_y.push_back(37.2438356);
	stopline_x.push_back(126.7742498);

	stopline_y.push_back(37.2426151);
	stopline_x.push_back(126.773753);

	/* urban road : Center */
	stopline_y.push_back(37.2427522); // left-right
	stopline_x.push_back(126.7741961);
	stopline_y.push_back(37.2427853); // left-left
	stopline_x.push_back(126.7739101);
	
	stopline_y.push_back(37.2428037); // right-right
	stopline_x.push_back(126.774691);
	stopline_y.push_back(37.2428033); // right-left
	stopline_x.push_back(126.7746924);

}

void SpdProfile(gps_t *current_pos)
{
	static uint8_t SpdFlg_cnt = 0;

	double dist = get_distance(current_pos, &SpdFlg[SpdFlg_cnt]);
	if (dist < 2)
	{
		SpdFlg_cnt++;
		if(SpdFlg_cnt == sizeof(SpdFlg) / sizeof(struct gps_t)){
			SpdFlg_cnt = 0;
		}
	}

	if(SpdFlg_cnt == 1){
		global_target_spd = 10;
	}

}

double GPS_Tracking(void)
{
	gps_t curr_pos;
	
	double target_distance;
	double next_target_distance;
	double diff_x, diff_y;
	double handle_radius;
	double theta_e = 0.0;
	double efa = 0.0;
	double target_azimuth, weight, alpha;
	float handle = 0.0;

	double rotate_theta = 0.0, rotate_theta_rad = 0.0;
	int _lookahead_cnt = 0, Pre_cnt = 0,_PreLookahead_cnt = 0;
	double _current_distance = 0.0;
	double _total_distance = 0.0, _PreTotal_distance = 0.0, current_R = 0.0, pre_R = 0.0, current_Ay = 0.0, pre_Ay = 0.0;; //Longitudinal control
	double pre_azi_rad, pre_diff_lati_m, pre_diff_longi_m, pre_stanley_x, pre_stanley_y, pre_stanley_theta;

	get_current_gps(&curr_pos);
	look_ahead_distance = get_lookahead_distance(global_vehicle_spd);

	azimuth_rad = (curr_pos.azimuth) * M_PI / 180.;
	diff_lati_m = (curr_pos.lati) * 111000.;
	diff_longi_m = (curr_pos.longi) * 88800.;

	// 230106 JDS MODIFIED -------------------------- 
	// diff_lati_m += latitude_offset * sin(azimuth_rad);
	// diff_longi_m += longitude_offset * cos(azimuth_rad);
	// curr_pos.lati  = diff_lati_m / 111000.;
	// curr_pos.longi = diff_longi_m / 88800.;
	// -----------------------------------------------

	get_initial_destination(&curr_pos);
	int path_50m_cnt = get_path_50m(&curr_pos);
	if (path_50m_cnt == 0){
		printf("<get_first_path_50m>\n");
		PDWS_flg = 1;
		get_first_path_50m(&curr_pos);
	}
	else PDWS_flg = 0;
	// cout << "path_50m_cnt : " << path_50m_cnt << endl;

	// Get PathCurvature
	global_curvature = 0.0;
	double max_curvature = 0.0;
	if(path_50m_cnt > 0){
		double sum_cur = 0.0;
		uint32_t sum_cnt = 0;
		for(int i=0; i<path_50m_cnt-2; i++){

			double a = path_50m_origin_y[i+1] - path_50m_origin_y[i];
			double b = path_50m_origin_x[i+1] - path_50m_origin_x[i];
			
			double c = path_50m_origin_y[i+2] - path_50m_origin_y[i];
			double d = path_50m_origin_x[i+2] - path_50m_origin_x[i];

			double e = path_50m_origin_y[i+1] - path_50m_origin_y[i+2];
			double f = path_50m_origin_x[i+1] - path_50m_origin_x[i+2];
			
			// triangleArea
			double triangleArea = ((b)*(c)-(d)*(a))/2.0;

			// L = A*B*C 
			// curvature = 4*triangleArea/(sideLength0*sideLength1*sideLength2)
			// https://stackoverflow.com/questions/41144224/calculate-curvature-for-3-points-x-y
			double fDist12 = sqrt(pow((double) fabs(a),2.0)+pow((double) fabs(b),2.0));
			double fDist23 = sqrt(pow((double) fabs(c),2.0)+pow((double) fabs(d),2.0));
			double fDist13 = sqrt(pow((double) fabs(e),2.0)+pow((double) fabs(f),2.0));
			double L = (fDist12*fDist23*fDist13);
		
			if(L == 0) sum_cur += 0.0;
			else
			{
				double curr_curvature = (4*triangleArea)/L;
				max_curvature = max(max_curvature, curr_curvature);
				sum_cur += curr_curvature;
				sum_cnt++;
			}
		}
		global_curvature = sum_cur / sum_cnt; // Avg
		global_curvature *= 10000;
		// cout << path_50m_cnt << "/" << path_50m_origin_y.size() << endl;
	}
	
	// 50m 까지의 경로를 (0, 0, Azimuth) 기준으로 회전
	rotate_theta = curr_pos.azimuth * -1;
	rotate_theta_rad = rotate_theta * M_PI / 180.;

	temp_x.resize(path_50m_cnt);
	temp_y.resize(path_50m_cnt);

	if (path_50m_cnt > 0)
	{
		for (int index = 0; index < path_50m_cnt; index++)
		{
			temp_x[index] = (path_50m_origin_x[index] - diff_longi_m) * cos(rotate_theta_rad) + (path_50m_origin_y[index] - diff_lati_m) * -1 * sin(rotate_theta_rad);
			temp_y[index] = (path_50m_origin_x[index] - diff_longi_m) * sin(rotate_theta_rad) + (path_50m_origin_y[index] - diff_lati_m) * cos(rotate_theta_rad);
		}
		for (int index = 0; index < path_50m_cnt; index++)
		{
			_current_distance = sqrt(pow(temp_x[index], 2) + pow(temp_y[index], 2));
			//printf("%.2f/", _current_distance);

			if (_current_distance < look_ahead_distance)
			{
				_lookahead_cnt = index;
				_total_distance = _current_distance;
			}
		}
		global_lookahead_idx = _lookahead_cnt;
		//printf("\n LD: %.2f -> Lcnt: %d  L: %.2f\n", look_ahead_distance, _lookahead_cnt, _total_distance);
		//printf("\n");
		
		stanley_target_x = temp_x[_lookahead_cnt];
		stanley_target_y = temp_y[_lookahead_cnt];
		stanley_theta = atan2(stanley_target_y, stanley_target_x);

		current_k = abs((2*sin(stanley_theta)) / _total_distance);  // k = 1/r

		TargetSpd_Control(current_k, path_50m_cnt);
	}

	// 차량 이동 궤적 생성 - 붉은선
	if (diff_lati_m != 0)
	{
		path_log_Latitude.push_back(diff_lati_m);
		path_log_Longitude.push_back(diff_longi_m);
	}
	if (path_log_Latitude.size() > path_log_len)
	{
		path_log_Latitude.erase(path_log_Latitude.begin());
		path_log_Longitude.erase(path_log_Longitude.begin());
	}

	return get_handle(-sin(stanley_theta), 3); // look ahead = 3
}

void TargetSpd_Control(double cur_k, int path_cnt) 
{
	int value = 0, set_num = 8;

	double current_Ay, current_azimuth, calibration_Xm, calibration_Ym, current_distance;
	double prev_Ay, max_K,lon1,lon2,lat1,lat2;
	double tmp_x[path_cnt] = {0.0}, tmp_y[path_cnt] = {0.0};
	int prev_start_cnt[set_num] = {0}, prev_lookahead_cnt[set_num] = {0};
	double prev_total_distance[set_num] = {0.0}, prev_stanley_theta[set_num] = {0.0}, prev_K[set_num] = {0.0}; 
	bool flag = 1;

	current_Ay = (10*10) * cur_k; 
	max_K = 0;
	//current_azimuth = (REF_Azi[path_25m_start_trace] * (-1) + 90) * M_PI/180.;
	lon1 = path_50m_origin_x[0] * M_PI/180.;
	lon2 = path_50m_origin_x[1] * M_PI/180.;
	lat1 = path_50m_origin_y[0] * M_PI/180.;
	lat2 = path_50m_origin_y[1] * M_PI/180.;

	double x,y;
	y = sin(lon2-lon1) * cos(lat2);
	x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lon2-lon1);

	current_azimuth = ((atan2(y,x) * 180./M_PI) * (-1) + 90) * M_PI/180.;

	//printf("row_azi:%lf   test_azi:%lf\n", current_azimuth, test_azi); 

	calibration_Xm = path_50m_origin_x[0] + longitude_offset * cos(current_azimuth);
	calibration_Ym = path_50m_origin_y[0] + latitude_offset * sin(current_azimuth);
	//current_azimuth = (-1) * current_azimuth;
	current_azimuth = global_azimuth * (-1) * M_PI/180.;

	for (int idx=0; idx<path_cnt; idx++) {
		tmp_x[idx] = (path_50m_origin_x[idx] - calibration_Xm) * cos(current_azimuth) + (path_50m_origin_y[idx] - calibration_Ym) * (-1) * sin(current_azimuth);
		tmp_y[idx] = (path_50m_origin_x[idx] - calibration_Xm) * sin(current_azimuth) + (path_50m_origin_y[idx] - calibration_Ym) * cos(current_azimuth);
		current_distance = sqrt(pow(tmp_x[idx], 2) + pow(tmp_y[idx], 2));
		
		if (current_distance < (value+2)*look_ahead_distance) {
			if(flag == 1) {
				prev_start_cnt[value] == idx;
				flag = 0;
			}
			prev_lookahead_cnt[value] = idx;
			prev_total_distance[value] = current_distance - (value+1) * look_ahead_distance;
		}
		else {
			value++;
			flag = 1;
		}

		if (value == set_num) break;
	}

	
	for (int num=0; num<set_num; num++) {
		int start = prev_start_cnt[num], end = prev_lookahead_cnt[num];
		prev_stanley_theta[num] = atan2((tmp_y[end] - tmp_y[start]), (tmp_x[end]-tmp_x[start]));

		if (prev_total_distance == 0) continue;
		prev_K[num] = abs((2*sin(prev_stanley_theta[num])) / prev_total_distance[num]);

		if (prev_K[num] > max_K) max_K = prev_K[num];
	}

	prev_Ay = (10*10) * max_K; 

	if (isnan(prev_Ay) == 0) {
		if (current_Ay > 5) global_target_spd = 10;
		else  {
			if (prev_Ay > 40) global_target_spd = 10;
			else if (prev_Ay > 30) global_target_spd = 12;
			else if (prev_Ay > 20) global_target_spd = 14;
			else if (prev_Ay > 10) global_target_spd = 16;
			else if (prev_Ay > 5) global_target_spd = 20;
			else global_target_spd = 30;
		}
	}
	else global_target_spd = 0;
	// printf("curAy:%.2lf   Curve:%.2lf   prev_Ay:%.2lf   Tspd:%d \n", current_Ay, global_curvature, prev_Ay, (int)global_target_spd);
}

double get_handle(double lateral_offset, double look_ahead_distance)
{
	if (lateral_offset == 0)
		return 0;

	/*  [IONIQ5 Parameter]
		WheelBase = 3.000 m
		
		If : distance between front axle and center of tractor mass
		Ir : distance between rear axle and center of tractor mass
	*/

	float handle = 0.;
	float m = 2060 + (75 * 2); // Tare Mass [kg]
	float WheelBase = 2.865; // [m]
	float ff,fr, wf, wr, cf, cr;
	float If = 1.4; // [m]
	float Ir = WheelBase - If; // [m]
	wf = Ir / (WheelBase * m * 9.81); // 앞바퀴에서 발생하는 무게
	wr = If / (WheelBase * m * 9.81); 
	cf = cr = 1200 * 2; 
	
	float r_tmp = (look_ahead_distance * look_ahead_distance + (lateral_offset * lateral_offset)) / (2 * lateral_offset);
	float delta_c = -1 * (WheelBase / r_tmp * 57.3 + global_vehicle_spd * global_vehicle_spd / (r_tmp * 9.81) * (wf / cf - wr / cr)) * 17.25;
	// printf("[%lf] %lf/%lf\n", lateral_offset, r_tmp, delta_c);
	if (delta_c > 400)
	{
		handle = 400;
	}
	else if (delta_c < -400)
	{
		handle = -400;
	}
	else
	{
		handle = delta_c;
	}
	return handle;
}


void get_initial_destination(gps_t *current_position)
{
	int i, j, k;
	double current_distance = 500.0;
	double min_distance = 500.0;
	int min_distance_cnt = 0;
	int lookahead_cnt = 0;
	gps_t select_position;

	double look_ahead_distance = get_lookahead_distance(global_vehicle_spd);
	for (i = global_last_trace; i < path_50m_end_trace; i++)
	{
		/// calculate distance
		get_destination_from_file(i, &select_position);
		current_distance = get_distance(current_position, &select_position);
		if (current_distance < min_distance)
		{
			min_distance = current_distance;
			min_distance_cnt = i;
		}
	}
	// printf("current_distance: %f / %f \n ", current_distance, min_distance);
	// printf("<get_initial_destination2> %d / %f / %d \n", global_last_trace, min_distance, min_distance_cnt );

	for (j = min_distance_cnt; j < path_50m_end_trace; j++)
	{
		/// calculate distance
		get_destination_from_file(j, &select_position);
		current_distance = get_distance(current_position, &select_position);
		if (current_distance < look_ahead_distance)
		{
			lookahead_cnt = j;
		}
		else
		{
			break;
		}
	}

	get_destination_from_file(lookahead_cnt, &select_position);
	current_distance = get_distance(current_position, &select_position);
	if (current_distance > look_ahead_distance)
	{
		lookahead_cnt = global_last_trace;
	}
	if (lookahead_cnt + 1 < global_waypoint_cnt)
	{
		lookahead_cnt += 1;
	}
	global_lookahead_cnt = lookahead_cnt;
	global_min_distance_cnt = min_distance_cnt;
}

void get_first_path_50m(gps_t *current_position){
	double x1, x2, y1, y2;

	// find first path_50m_end_trace
    x1 = (current_position->longi) * 88800.;
    y1 = (current_position->lati) * 111000.;
    double min_distance = 500.0;
    int min_distance_cnt = 0;
    int i = 0;
    for (i = 0; i < global_waypoint_cnt; i++)
    {
        x2 = REF_Lon[i] * 88800.;
        y2 = REF_Lat[i] * 111000.;
        double dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        if (dist < min_distance)
        {
            min_distance = dist;
            min_distance_cnt = i;
        }
    }

    // printf("min_distance: %lf, cnt: %d\n", min_distance, min_distance_cnt);

    global_min_distance_cnt = min_distance_cnt;
    if (global_min_distance_cnt == global_waypoint_cnt)
    {
        global_min_distance_cnt -= 1;
    }
    global_last_trace = global_min_distance_cnt;
    path_50m_start_trace = global_min_distance_cnt;
    path_50m_end_trace = global_min_distance_cnt + 1;

    for (int i = path_50m_end_trace; i < global_waypoint_cnt - 1; i++)
    {
        x2 = REF_Lon[i] * 88800.;
        y2 = REF_Lat[i] * 111000.;
        double dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
        if (dist < front_path_distance)
        {
            path_50m_end_trace++;
        }
        else
        {
            break;
        }
    }
}

int get_path_50m(gps_t *current_position)
{
	struct timeval t1, t2;
	gps_t select_position;
	double current_distance, front_distance = 0.0;
	int front_cnt = 0;
	int path_50m_cnt = 0;
	double obj_cte[10];
	double rdr_cte[16];
	uint32_t p_cnt = 0;
	
	if (global_waypoint_cnt - 1 > path_50m_end_trace)
	{
		for (int i = path_50m_end_trace; i < global_waypoint_cnt - 1; i++)
		{
			get_destination_from_file(i, &select_position);
			current_distance = get_distance(current_position, &select_position);
			if (current_distance < front_path_distance)
			{
				path_50m_end_trace += 1;
			}
			else
			{
				break;
			}
		}
	}
	if (global_min_distance_cnt > path_50m_start_trace && path_50m_end_trace > path_50m_start_trace)
	{
		path_50m_start_trace = global_min_distance_cnt;
		global_last_trace = global_min_distance_cnt;
	}

	front_distance = 0;
	for (int i = path_50m_start_trace; i < path_50m_end_trace; i++)
	{
		p_cnt++;
		front_distance += global_distance[i];
	}
	
	front_cnt = (int)(front_distance / path_dencity);

	path_50m_origin_x.resize(front_cnt, 0);
	path_50m_origin_y.resize(front_cnt, 0);


	CAM_Obj[0].y = 10;
	CAM_Obj[1].y = 20;
	CAM_Obj[2].y = 30;
	CAM_Obj[3].y = 40;
	CAM_Obj[4].y = 50;
	CAM_Obj[5].y = 60;
	CAM_Obj[6].y = 70;
	CAM_Obj[7].y = 80;
	CAM_Obj[8].y = 90;
	CAM_Obj[9].y = 100;
	CAM_Obj[10].x = 10;
	CAM_Obj[11].x = 20;
	CAM_Obj[12].x = 30;
	CAM_Obj[13].x = 40;
	CAM_Obj[14].x = 50;
	CAM_Obj[15].x = 60;
	CAM_Obj[16].x = 70;
	CAM_Obj[17].x = 80;
	CAM_Obj[18].x = 90;
	CAM_Obj[19].x = 100;



    //---------------------------------------------------
	if (front_cnt > 0)
	{
		gettimeofday(&t1, NULL);
    
		get_destination_from_file(path_50m_start_trace, &select_position);
		path_50m_origin_x[0] = select_position.longi * 88800.;
		path_50m_origin_y[0] = select_position.lati * 111000.;
		for (int i = 1; i < front_cnt; i++)
		{
			front_distance = 0;
			double cte = 50.0;
			for (int j = path_50m_start_trace; j < path_50m_end_trace; j++)
			{
				front_distance += global_distance[j];
				if (front_distance < path_dencity * i)
				{
					get_destination_from_file(j, &select_position);
					path_50m_origin_x[i] = select_position.longi * 88800.;
					path_50m_origin_y[i] = select_position.lati * 111000.;

					double dist = get_distance(current_position, &select_position);
					cte = min(cte, dist);

				}
				else
				{
					break;
				}
			}
			global_cte = cte;
		}

		memset(&obj_cte, 100, sizeof(obj_cte));
		for(int k=0; k<CAMOBJ_SIZE; k++){
			double obj_y = CAM_Obj[k].y;
			double obj_x = CAM_Obj[k].x;

			if(obj_y== 0 && obj_x == 0){
				obj_cte[k] = 100;
				continue;
			}

			for (int i = 0; i < front_cnt; i++)
			{
				gps_t sel_pos;
				sel_pos.lati  = path_50m_origin_y[i] / 111000.;
				sel_pos.longi = path_50m_origin_x[i] / 88800.;

				double obj_dist = get_obj_distance(obj_y, obj_x, &sel_pos);
				obj_cte[k] = min(obj_cte[k], obj_dist);
			}
		}

		memset(&rdr_cte, 100, sizeof(rdr_cte));
		for(int k=0; k<RDROBJ_SIZE; k++){
			double obj_y = RDR_Obj[k].y;
			double obj_x = RDR_Obj[k].x;

			if(obj_y== 0 && obj_x == 0){
				rdr_cte[k] = 100;
				continue;
			}

			for (int i = 0; i < front_cnt; i++)
			{
				gps_t sel_pos;
				sel_pos.lati  = path_50m_origin_y[i];
				sel_pos.longi = path_50m_origin_x[i];


				double obj_dist = get_obj_distance(obj_y, obj_x, &sel_pos);
				rdr_cte[k] = min(rdr_cte[k], obj_dist);
			}
		}

		
		for(int k=0; k<CAMOBJ_SIZE; k++){
			CAM_Obj[k].cte = obj_cte[k];

			printf("%.2f/",obj_cte[k]);
			if(CAM_Obj[k].cte < 2.0){
				CAM_Obj[k].risk = 1;
			}
			else CAM_Obj[k].risk = 0;
		}
		printf("\n");

		for(int k=0; k<RDROBJ_SIZE; k++){
			RDR_Obj[k].cte = rdr_cte[k];
		}

		gettimeofday(&t2, NULL);
		double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);

		printf("[%d/%d] %.5f ms\n", p_cnt, front_cnt, diffTime_ms);
	}

	path_50m_cnt = front_cnt;
	return path_50m_cnt;
}

double get_distance(gps_t *current_position, gps_t *destination_position)
{
	double _distance = sqrt(pow(((current_position->longi - destination_position->longi) * 88800.), 2) + pow(((current_position->lati - destination_position->lati) * 111000.), 2));
	return _distance;
}

double get_obj_distance(double x, double y, gps_t *destination_position)
{
	double obj_x = diff_longi_m + (x * cos(azimuth_rad)) - (y * sin(-azimuth_rad));
	double obj_y = diff_lati_m  + (x * sin(azimuth_rad)) - (y * cos(azimuth_rad));
	double diff_x = (obj_x - (destination_position->longi * 88800.));
	double diff_y = (obj_y - (destination_position->lati * 111000.));

	double _distance = sqrt(pow(diff_x,2) + pow(diff_y, 2));
	return _distance;
}

void get_destination_from_file(int waypoint_select, gps_t *destination_position)
{
	destination_position->azimuth = REF_Azi[waypoint_select];
	destination_position->lati = REF_Lat[waypoint_select];
	destination_position->longi = REF_Lon[waypoint_select];
}

void get_current_gps(gps_t *current_position)
{
	double _lati, _longi;
	_longi = global_longitude; // + cos(current_position->azimuth / 180 * PI) * 2 / 88800.;
	_lati = global_latitude;   // + sin(current_position->azimuth / 180 * PI) * 2 / 111000.;
	current_position->lati = _lati;
	current_position->longi = _longi;
	current_position->azimuth = global_azimuth;
}

// 8kph  -> 8 * 0.5 = 4
// 30kph -> 30 * 0.5 = 15
// 50kph -> 50 * 0.5 = 25
double get_lookahead_distance(double current_velocity)
{
	double output = 0.0;

	if (current_velocity < 8.0)
		output = 8.0;
	else if (current_velocity > max_lookahead_distance)
		output = max_lookahead_distance;
	else
		output = current_velocity;

	return output * gain;
}

#ifdef MATPLOTLIBCPP
void Viewer(void){
	// plt::figure_size(1200, 780);
	vector<double> theta, circle_a, circle_b;
	
	vector<double> curr_pos_x(1), curr_pos_y(1);
	vector<double> curr_tar_x(1), curr_tar_y(1);
	vector<double> waypoint_pos_x, waypoint_pos_y;
	for(int i=0; i<waypoint_x.size(); i++){
		waypoint_pos_x.push_back(waypoint_x[i] * 88800.);
		waypoint_pos_y.push_back(waypoint_y[i] * 111000.);
	}

	vector<double> trafficlight_pos_x, trafficlight_pos_y;
	for(int i=0; i<trafficlight_x.size(); i++){
		trafficlight_pos_x.push_back(trafficlight_x[i] * 88800.);
		trafficlight_pos_y.push_back(trafficlight_y[i] * 111000.);
	}

	vector<double> sl_pos_x, sl_pos_y;
	for(int i=0; i<stopline_x.size(); i++){
		sl_pos_x.push_back(stopline_x[i] * 88800.);
		sl_pos_y.push_back(stopline_y[i] * 111000.);
	}

	vector<double> test_x, test_y;
	for(int i=0; i<sizeof(CrossWalk) / sizeof(struct gps_t); i++){
		test_x.push_back(CrossWalk[i].longi * 88800.);
		test_y.push_back(CrossWalk[i].lati * 111000.);
	}
	for(int i=0; i<sizeof(RoadJunction) / sizeof(struct gps_t); i++){
		test_x.push_back(RoadJunction[i].longi * 88800.);
		test_y.push_back(RoadJunction[i].lati * 111000.);
	}
	for(int i=0; i<sizeof(Roundabout) / sizeof(struct gps_t); i++){
		test_x.push_back(Roundabout[i].longi * 88800.);
		test_y.push_back(Roundabout[i].lati * 111000.);
	}

	vector<double> turn_x, turn_y;
	for(int i=0; i<turnsig_lati.size(); i++){
		turn_x.push_back(turnsig_longi[i] * 88800.);
		turn_y.push_back(turnsig_lati[i] * 111000.);
	}
	

	vector<double> curr_stopline_pos_x(1), curr_stopline_pos_y(1);
	vector<double> dsl_pos_x, dsl_pos_y;
	vector<double> obj_pos_x, obj_pos_y, d_obj_pos_x, d_obj_pos_y;
	vector<double> rdr_obj_pos_x, rdr_obj_pos_y;

	while(1){
		try{
			plt::clf();
	
			plt::xlim(diff_longi_m - (zoom_level *20), diff_longi_m + (zoom_level *20));
			plt::ylim(diff_lati_m - (zoom_level *20), diff_lati_m + (zoom_level *20));

			// Add graph title
			plt::title("KATECH");

			// [PATH]
			plt::plot(REF_Lon_plot, REF_Lat_plot, {{"color", "black"}, {"linestyle", "--"}, {"label", "global_path"}}); // Global
			plt::plot(path_50m_origin_x, path_50m_origin_y, {{"color", "green"}, {"linestyle", "-"}, {"linewidth", "3"}, {"label", "local_path"}}); // Local
			plt::plot(path_log_Longitude, path_log_Latitude, {{"color", "red"}, {"linestyle", "-"}, {"label", "trajectory"}}); // Position

			// [Obstacle]
			obj_pos_x.clear();
			obj_pos_y.clear();
			d_obj_pos_x.clear();
			d_obj_pos_y.clear();
			for(int k=0; k<CAMOBJ_SIZE; k++){
				double y = CAM_Obj[k].y;
				double x = CAM_Obj[k].x;
				uint8_t c = CAM_Obj[k].c;
				bool d = CAM_Obj[k].risk;
				if((x == 0 && y == 0))
					continue;
				
				if(d == 0){
					obj_pos_x.push_back(diff_longi_m + (y * cos(azimuth_rad)) - (x * sin(-azimuth_rad))); 
					obj_pos_y.push_back(diff_lati_m  + (y * sin(azimuth_rad)) - (x * cos(azimuth_rad))); 
				}
				else{
					d_obj_pos_x.push_back(diff_longi_m + (y * cos(azimuth_rad)) - (x * sin(-azimuth_rad)));
					d_obj_pos_y.push_back(diff_lati_m  + (y * sin(azimuth_rad)) - (x * cos(azimuth_rad))); 
				}
				// printf("[ID:%d] Y: %.3f X: %.3f  PATHflg: %d \n", c,y,x,d);
			}
			plt::scatter(d_obj_pos_x, d_obj_pos_y,  50, {{"color", "red"}, {"marker", "x"}});
			plt::scatter(obj_pos_x, obj_pos_y,  30, {{"color", "black"}, {"marker", "x"}, {"label", "Obstacle"}});

			rdr_obj_pos_x.clear();
			rdr_obj_pos_y.clear();

			for(int k=0; k<RDROBJ_SIZE; k++){
				double obj_y = RDR_Obj[k].y;
				double obj_x = RDR_Obj[k].x;
				int obj_spd = RDR_Obj[k].velocity;
				if((obj_x == 0 && obj_y == 0))
					continue;
				
				// printf("[v:%d] Y: %.3f X: %.3f \n", obj_spd,obj_y,obj_x);

				rdr_obj_pos_x.push_back(diff_longi_m + (obj_y * cos(azimuth_rad)) - (obj_x * sin(-azimuth_rad))); 
				rdr_obj_pos_y.push_back(diff_lati_m  + (obj_y * sin(azimuth_rad)) - (obj_x * cos(azimuth_rad))); 
			}
			plt::scatter(rdr_obj_pos_x, rdr_obj_pos_y,  30, {{"color", "blue"}, {"marker", "x"}, {"label", "RDRObstacle"}});

			// [Vehicle]
			curr_pos_x[0] = diff_longi_m;
			curr_pos_y[0] = diff_lati_m;
			plt::scatter(curr_pos_x, curr_pos_y, 50, {{"color", "black"}, {"marker", "D"}, {"linestyle", "-"}});
			plt::arrow(diff_longi_m, diff_lati_m, 7 * cos(azimuth_rad), 7 * sin(azimuth_rad), "r","k",4.0,2.0);


			curr_tar_x[0] = path_50m_origin_x[global_lookahead_idx];
			curr_tar_y[0] = path_50m_origin_y[global_lookahead_idx];
			plt::scatter(curr_tar_x, curr_tar_y, 50, {{"color", "red"}, {"marker", "o"}, {"linestyle", "-"}});

			// [TrafficLight : StopLine]
			if(stopline_idx.size() > 0){
				dsl_pos_x.clear();
				dsl_pos_y.clear();
				for(int i=0; i<stopline_idx.size(); i++){
					if(stopline_idx[i].first > 0){
						dsl_pos_x.push_back(stopline_x[stopline_idx[i].second] * 88800.);
						dsl_pos_y.push_back(stopline_y[stopline_idx[i].second] * 111000.);
					}
				}
				plt::scatter(dsl_pos_x, dsl_pos_y, 70, {{"color", "black"}, {"marker", "*"}});

				curr_stopline_pos_x[0] = stopline_x[stopline_idx[sl_cnt].second] * 88800;
				curr_stopline_pos_y[0] = stopline_y[stopline_idx[sl_cnt].second] * 111000;
				plt::scatter(curr_stopline_pos_x, curr_stopline_pos_y, 70, {{"color", "magenta"}, {"marker", "*"}, {"label", "TrafficLight"}});
			}

			// Test point
			plt::scatter(test_x, test_y, 20, {{"color", "red"}, {"marker", "o"}, {"label", "TEST"}});
			plt::scatter(turn_x, turn_y, 20, {{"color", "blue"}, {"marker", "o"}, {"label", "TurnSig"}});

			// [속도에 따른 lookahead]
			circle_a.clear();
			circle_b.clear();
			double radius = look_ahead_distance;
			for(int i = 0; i< 100; i++){
				theta.push_back((2.0*M_PI/100.0)*(double)i);
				circle_a.push_back(diff_longi_m+radius*cos(theta[i]));
				circle_b.push_back(diff_lati_m +radius*sin(theta[i]));
			}
			plt::plot(circle_a, circle_b, "k--");

			// [Text]
			char buf[50], buf1[50];
			sprintf(buf, "%.2f/%c/%3d/%.2f/%.2f", global_utc, global_gpsstat, my_count-target_cnt, global_azimuth, global_vehicle_spd);
			plt::text(diff_longi_m, diff_lati_m - 20, buf);

			sprintf(buf, "CTE: %lf m / Cur: %.6f", global_cte, global_curvature);
			plt::text(diff_longi_m, diff_lati_m - 40, buf);

			// Display plot continuously
			plt::legend();
			plt::pause(0.1);

			if(exit_flag){
				break;
			}
            
        }
        catch (std::out_of_range& e) {
		    std::cout << "<Viewer> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<Viewer> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<Viewer> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
			plt::detail::_interpreter::kill();
			// Py_Finalize();
        }
	}
}
#endif
