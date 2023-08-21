#include <iostream>
#include <math.h>
#include "Tracking.h"
#include "DecisionMaking.h"

gps_t SchoolZone;
gps_t RoadJunction[4];
gps_t CrossWalk[3];
gps_t Roundabout[2];

bool SystemPriorityMode = 0;

RadarYXV RDR_Obj[RDROBJ_SIZE];
CameraYXC CAM_Obj[CAMOBJ_SIZE];

uint8_t cam_stop_flg = GO;
double cam_stop_dist = 100.0;

uint8_t cam_Ra_stop_flg = GO;
double cam_Ra_stop_dist = 100.0;

uint8_t rdr_stop_flg = GO;
double rdr_stop_dist = 100.0;

uint8_t global_trafficlight_sig = GO;
double stopline_dist = 0.0;
std::vector<double> trafficlight_x, trafficlight_y, trafficlight_azi, trafficlight_dist(100, 0.0);
std::vector<double> stopline_x, stopline_y;
vector<int_int_pair> stopline_idx;
uint32_t sl_cnt = 0;

bool PDWS_flg = 0;


//****************************************************************
// Warning Msg
//****************************************************************

// Maximum Speed Limit Warning System
DOWS chk_MSLWS(uint8_t tablet_spd){
	DOWS DriverOverride;

	if(SystemPriorityMode){
		if(tablet_spd > 50){
			DriverOverride.level = 1;
			DriverOverride.msg = 5;
		}
		else{
			DriverOverride.level = 0;
			DriverOverride.msg = 0;
		}
	}
	else{
		DriverOverride.level = 0;
		DriverOverride.msg = 0;
	}

	return DriverOverride;
}

DOWS chk_SchoolZone(gps_t *current_pos){
	DOWS DriverOverride;

	// [KCity SchoolZone] center point
	SchoolZone.longi = 126.7741730;
	SchoolZone.lati  =  37.2399603;
	const int schoolzone_point = 36; // 36 m from the center point
	const int warning_point = 70; // 70 m from the center point

	double dist = get_distance(current_pos, &SchoolZone);

	if(SystemPriorityMode){
		if (dist <= schoolzone_point)
		{
			// SystemPriorityMode OFF & CarSpd < 30kph
			DriverOverride.level = 3;
			DriverOverride.msg = 4;
		}
		else if(dist <= warning_point){
			DriverOverride.level = 2;
			DriverOverride.msg = 2;
		}
		else{
			DriverOverride.level = 0;
			DriverOverride.msg = 0;
		}
	}
	else{
		DriverOverride.level = 0;
		DriverOverride.msg = 0;
	}

	return DriverOverride;
}

// Path Departure Warning System
DOWS chk_PDWS(double EgoVehicle_cte){
	DOWS DriverOverride;

	const float PDWS = 5.0; // Path Departure Warning System
	const float LDWS = 1.0; // Lane Departure Wanrning System

	if(SystemPriorityMode){
		if(EgoVehicle_cte >= PDWS){
			DriverOverride.level = 3;
			DriverOverride.msg = 4;
		}
		else if(EgoVehicle_cte >= LDWS){
			DriverOverride.level = 1;
			DriverOverride.msg = 1;
		}
		else{
			DriverOverride.level = 0;
			DriverOverride.msg = 0;
		}

		if(PDWS_flg){
			DriverOverride.level = 3;
			DriverOverride.msg = 4;
		}
	}
	else{
		DriverOverride.level = 0;
		DriverOverride.msg = 0;
	}

	return DriverOverride;
}

DOWS chk_ODD(gps_t *current_pos){
	DOWS DriverOverride;
	gps_t ODD;

	// [KCity] center point
	ODD.longi = 126.7744658;
	ODD.lati  =  37.2427811;
	const int odd_range = 550; // 550 m from the center point

	double dist = get_distance(current_pos, &ODD);

	if(SystemPriorityMode){
		if (dist >= odd_range)
		{
			// SystemPriorityMode OFF
			DriverOverride.level = 3;
			DriverOverride.msg = 4;
		}
		else{
			DriverOverride.level = 0;
			DriverOverride.msg = 0;
		}
	}
	else{
		DriverOverride.level = 0;
		DriverOverride.msg = 0;
	}

	return DriverOverride;
}


DOWS DriverOverride_WarningSystem(gps_t *current_pos, double EgoVehicle_cte, uint8_t tablet_spd){
	DOWS out[4];
	DOWS DriverOverride_out;

	out[0] = chk_PDWS(EgoVehicle_cte);
	out[1] = chk_SchoolZone(current_pos);
	out[2] = chk_MSLWS(tablet_spd);
	out[3] = chk_ODD(current_pos);

	DriverOverride_out.level = 0;
	DriverOverride_out.msg = 0;
	for(int i=0; i<4; i++){
		if(DriverOverride_out.level < out[i].level){
			DriverOverride_out.level = out[i].level;
			DriverOverride_out.msg   = out[i].msg;
		}
	}

	return DriverOverride_out;
}

//****************************************************************
// Object (RDR, CAM)
//****************************************************************

// Overall Width  (mm) 1,890 (FRONT)
// Overall Height (mm) 1,605
// Overall Length (mm) 4,635

void chk_ObjOnPath(gps_t *current_pos){
    const int set_delay = 5; // 50ms * cnt
	const int y_range = 25;

	static uint8_t delay_cnt = 0;
    uint8_t t_idx = 0;

	if(delay_cnt-- == 0){
		delay_cnt = 1;
		cam_stop_dist = 100.0;
		cam_stop_flg = GO;
	}

	for(int k=0; k<CAMOBJ_SIZE; k++){
		double obj_y = CAM_Obj[k].y;
		double obj_x = CAM_Obj[k].x;
		uint8_t obj_class = CAM_Obj[k].c;
		uint8_t obj_stat = CAM_Obj[k].risk;
		double obj_cte = CAM_Obj[k].cte;

		if(!(obj_y == 0 && obj_x == 0)){
			if ((obj_cte <= 1.5) && (obj_y < y_range)){
				cam_stop_flg = STOP;
				delay_cnt = set_delay;
				if(cam_stop_dist >= obj_y){
					cam_stop_dist = obj_y;
					t_idx = k;
				}
			}
		}
	}
	// printf("[ObjOnPath] STOP:%d DIST: %3.2f CTE: %.2f  INFO(ID:%2d / %3.2f / %3.2f) (Delay: %2d)\n", cam_stop_flg, cam_stop_dist, CAM_Obj[t_idx].cte, CAM_Obj[t_idx].c, CAM_Obj[t_idx].y, CAM_Obj[t_idx].x, delay_cnt);
}
void chk_RoundaboutVehicle(gps_t *current_pos){
	const int set_delay = 5; // 50ms * cnt
	const int x_range = -13;
	const int x_range2 = 5;
	const int y_range = 20;

	static uint8_t delay_cnt = 0;
	static uint8_t Roundabout_cnt = 0;
    uint8_t t_idx = 0;

	double dist = get_distance(current_pos, &Roundabout[Roundabout_cnt]);
	if (dist < 2)
	{
		Roundabout_cnt++;
		if(Roundabout_cnt == sizeof(Roundabout) / sizeof(struct gps_t)){
			Roundabout_cnt = 0;
		}
	}
	if(Roundabout_cnt == 0){
		cam_Ra_stop_flg = GO;
	}
	if(Roundabout_cnt == 1){
		cam_Ra_stop_flg = STOP;
		cam_Ra_stop_dist = dist;

		if(cam_Ra_stop_dist <= 8.0){
			if(delay_cnt-- == 0){
				delay_cnt = 1;
				cam_Ra_stop_dist = 100.0;
				cam_Ra_stop_flg = GO;
			}

			for(int k=0; k<CAMOBJ_SIZE; k++){
				double obj_y = CAM_Obj[k].y;
				double obj_x = CAM_Obj[k].x;
				uint8_t obj_class = CAM_Obj[k].c;
				uint8_t obj_stat = CAM_Obj[k].risk;
				double obj_cte = CAM_Obj[k].cte;

				if(!(obj_y == 0 && obj_x == 0)){
					if ((obj_y < y_range) && (obj_x) > x_range && (obj_x) < x_range2){
						cam_Ra_stop_flg = STOP;
						delay_cnt = set_delay;
						t_idx = k;
					}
				}
			}
		}
		// printf("[Roundabout] STOP:%d DIST: %3.2f CTE: %.2f INFO(ID:%2d / %3.2f / %3.2f) (Delay: %2d)\n", cam_Ra_stop_flg, cam_Ra_stop_dist, CAM_Obj[t_idx].cte, CAM_Obj[t_idx].c, CAM_Obj[t_idx].y, CAM_Obj[t_idx].x, delay_cnt);
	}
}

// crossroad -> crosswalk
// -5 ~ 2.8m
void chk_CrosswalkPerson(gps_t *current_pos){
    const int set_delay = 5; // 50ms * cnt
	const int x_range = 5;
	const int y_range = 15;
	const uint8_t PEDESTRIAN_ID = 80;
	const uint8_t CYCLIST_ID1 = 64; // 64~66
	const uint8_t CYCLIST_ID2 = 65; // 64~66
	const uint8_t CYCLIST_ID3 = 66; // 64~66

	static uint8_t delay_cnt = 0;
	static uint8_t CrossWalk_cnt = 0;
    uint8_t t_idx = 0;

	double dist = get_distance(current_pos, &CrossWalk[CrossWalk_cnt]);
	if (dist < 2)
	{
		CrossWalk_cnt++;
		if(CrossWalk_cnt == sizeof(CrossWalk) / sizeof(struct gps_t)){
			CrossWalk_cnt = 0;
		}
	}
	if(CrossWalk_cnt == 0){
		cam_stop_flg = GO;
	}
	if(CrossWalk_cnt == 1){ // section(CrossWalk1, CrossWalk2 )
        if(delay_cnt-- == 0){
			delay_cnt = 1;
            cam_stop_dist = 100.0;
			cam_stop_flg = GO;
		}

		for(int k=0; k<CAMOBJ_SIZE; k++){
			double obj_y = CAM_Obj[k].y;
			double obj_x = CAM_Obj[k].x;
			uint8_t obj_class = CAM_Obj[k].c;
			uint8_t obj_stat = CAM_Obj[k].risk;
			double obj_cte = CAM_Obj[k].cte;

			if(!(obj_y == 0 && obj_x == 0)){
				if((obj_class == PEDESTRIAN_ID) || (obj_class == CYCLIST_ID1) || (obj_class == CYCLIST_ID2) || (obj_class == CYCLIST_ID3)){
					if ((obj_y < y_range) && (abs(obj_x) < x_range)){
						cam_stop_flg = STOP;
						delay_cnt = set_delay;
						// if(obj_y >= 2){ // 221205 update
						// 	obj_y =- 2;
						// }

						if(cam_stop_dist >= obj_y){
							cam_stop_dist = obj_y;
							t_idx = k;
						}
					}
				}
			}
		}
		// printf("[CrossWalk] STOP:%d DIST: %3.2f CTE: %.2f  INFO(ID:%2d / %3.2f / %3.2f) (Delay: %2d)\n", cam_stop_flg, cam_stop_dist, CAM_Obj[t_idx].cte, CAM_Obj[t_idx].c, CAM_Obj[t_idx].y, CAM_Obj[t_idx].x, delay_cnt);
	}
	if(CrossWalk_cnt == 2){ // TrafficIsland
		if(delay_cnt-- == 0){
			delay_cnt = 1;
            cam_stop_dist = 100.0;
			cam_stop_flg = GO;
		}
		for(int k=0; k<CAMOBJ_SIZE; k++){
			double obj_y = CAM_Obj[k].y;
			double obj_x = CAM_Obj[k].x;
			uint8_t obj_class = CAM_Obj[k].c;
			uint8_t obj_stat = CAM_Obj[k].risk;
			double obj_cte = CAM_Obj[k].cte;

			if(!(obj_y == 0 && obj_x == 0)){
				if((obj_class == PEDESTRIAN_ID) || (obj_class == CYCLIST_ID1) || (obj_class == CYCLIST_ID2) || (obj_class == CYCLIST_ID3)){
					if ((obj_stat == 1) && (obj_y < y_range)){
						cam_stop_flg = STOP;
						delay_cnt = set_delay;
						if(cam_stop_dist >= obj_y){
							cam_stop_dist = obj_y;
							t_idx = k;
						}
					}
				}
			}

		}
		// printf("[TrafficIsland] STOP:%d DIST: %3.2f CTE: %.2f  INFO(ID:%2d / %3.2f / %3.2f) (Delay: %2d)\n", cam_stop_flg, cam_stop_dist, CAM_Obj[t_idx].cte, CAM_Obj[t_idx].c, CAM_Obj[t_idx].y, CAM_Obj[t_idx].x, delay_cnt);
	}
}

// road junction
void chk_RearLeftRDR(gps_t *current_pos){
	const int set_delay = 10; // 50ms * cnt
	const double x_range = -4.5;
	const int y_range = -40;

	static uint8_t delay_cnt = 0;
	static uint8_t RoadJunction_cnt = 0;

	double dist = get_distance(current_pos, &RoadJunction[RoadJunction_cnt]);
	if (dist < 2)
	{
		RoadJunction_cnt++;
		if(RoadJunction_cnt == sizeof(RoadJunction) / sizeof(struct gps_t)){
			RoadJunction_cnt = 0;
		}
	}

	if(RoadJunction_cnt == 0 || RoadJunction_cnt == 2){
		rdr_stop_flg = GO;
	}
	if(RoadJunction_cnt == 1 || RoadJunction_cnt == 3){
		uint8_t t_idx = 0;
		if(delay_cnt-- == 0){
			delay_cnt = 1;
			rdr_stop_flg = GO;
		}

		for(int k=0; k<RDROBJ_SIZE; k++){
			double obj_y = RDR_Obj[k].y;
			double obj_x = RDR_Obj[k].x;
			int obj_spd = RDR_Obj[k].velocity;

			double min_dist = 100.0;
			if(obj_y < 0 && obj_x < 0){ // Rear + Left
				if ((obj_y > y_range) && (obj_x > x_range) && (obj_spd > 0)){
					rdr_stop_flg = STOP;
					rdr_stop_dist = dist;
					delay_cnt = set_delay; 

					if(min_dist >= obj_y){
						min_dist = obj_y;
						t_idx = k;
					}
				}
			}
		}
	
		// printf("[RoadJunction %d] STOP:%d DIST: %3.2f INFO(Spd:%2d / %3.2f / %3.2f) (Delay: %2d) \n", RoadJunction_cnt, rdr_stop_flg, rdr_stop_dist, RDR_Obj[t_idx].velocity, RDR_Obj[t_idx].y, RDR_Obj[t_idx].x, delay_cnt);
	}
}


//****************************************************************
// TraficLight
//****************************************************************

bool comparator ( const int_int_pair& l, const int_int_pair& r)
{
	return l.first < r.first;
}

void TrafficLight_init(void)
{
	double x1,x2,y1,y2;
	sl_cnt = 0;
	double dist = 0.0;
	int pram = 3;

	int stopline_size = stopline_x.size();

	stopline_idx.clear();
	for(int n=0; n<stopline_size; n++){

		stopline_idx.push_back(int_int_pair(0,0));

		x2 = stopline_x[n] * 88800;
		y2 = stopline_y[n] * 111000;

		for (int i = 0; i < global_waypoint_cnt - 2; i++)
        {
            x1 = REF_Lon[i] * 88800.;
            y1 = REF_Lat[i] * 111000.;
            dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
			if(dist < pram){
				stopline_idx[n].first = i;
				stopline_idx[n].second = n;
				break; 
			}
        }
	}

	if(stopline_idx.size() > 0)
		stable_sort(stopline_idx.begin(),stopline_idx.end(), comparator);

	printf("TLIdx: ");
	for(int i=0; i<stopline_idx.size(); i++){
		printf("%d/%d/", stopline_idx[i].first, stopline_idx[i].second);
	}
	printf("\n");
}

void chk_TrafficLight(gps_t *current_pos)
{
	gps_t stopline_pos;
	int dist_flg = 2;

	if(stopline_idx.size() > 0 && stopline_idx[sl_cnt].first > 0){
		stopline_pos.longi = stopline_x[stopline_idx[sl_cnt].second];
		stopline_pos.lati  = stopline_y[stopline_idx[sl_cnt].second];
		stopline_dist = get_distance(current_pos, &stopline_pos);

		if (stopline_dist < dist_flg)
		{
			sl_cnt++;
		}
	}
	else{
		sl_cnt++;
		// return -1;
	}

	
	if(stopline_idx.size() <= sl_cnt) {
		sl_cnt = 0;
    }
}


//****************************************************************
// DecisionMaking Output
//****************************************************************

Control DecisionMaking(gps_t *current_pos){
    Control move;
    chk_TrafficLight(current_pos);
	chk_RoundaboutVehicle(current_pos);
	chk_CrosswalkPerson(current_pos);
    chk_RearLeftRDR(current_pos);
	// chk_ObjOnPath(current_pos);
    
    if(cam_stop_flg == STOP && global_trafficlight_sig == STOP){
        move.mode = STOP;
        if(cam_stop_dist < stopline_dist){
			move.sensor = 2;
            move.stopdist = cam_stop_dist;
        }
        else{
			move.sensor = 1;
            move.stopdist = stopline_dist;
        }  
    }
    else if(cam_stop_flg == STOP && global_trafficlight_sig == GO){
        move.mode = STOP;
		move.sensor = 2;
        move.stopdist = cam_stop_dist;
    }
    else if(cam_stop_flg == GO && global_trafficlight_sig == STOP){
        move.mode = STOP;
		move.sensor = 1;
        move.stopdist = stopline_dist;
    }
    else{ // GO & GO
        move.mode = GO;
		move.sensor = 0;
        move.stopdist = stopline_dist;
    }

	if(cam_Ra_stop_flg == STOP){
		move.mode = STOP;
		move.sensor = 2;
        move.stopdist = cam_Ra_stop_dist;
	}

    if(rdr_stop_flg == STOP){
        move.mode = STOP;
		move.sensor = 3;
        move.stopdist = rdr_stop_dist;
    }

    return move;
}