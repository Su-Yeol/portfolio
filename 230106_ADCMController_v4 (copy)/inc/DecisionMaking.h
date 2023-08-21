#pragma once
typedef std::pair<int,int> int_int_pair;

extern gps_t SchoolZone;
extern gps_t RoadJunction[4];
extern gps_t CrossWalk[3];
extern gps_t Roundabout[2];

extern bool SystemPriorityMode;


// Driver Override Warning System
struct DOWS{
	uint8_t level; // 1: traffic light, 2: cam_obj, 3: rdr_obj
    uint8_t msg;
};

/*
msg 1: Path Departure Warning System
msg 2: SchoolZone Warning System
msg 3: Functional failure
msg 4: Unable to enter System Priority Mode   
msg 5: Maximum speed limit Warning System

*/

DOWS chk_SchoolZone(gps_t *current_pos);
DOWS chk_PDWS(double EgoVehicle_cte);
DOWS chk_MSLWS(uint8_t tablet_spd);
DOWS chk_ODD(gps_t *current_pos);
DOWS DriverOverride_WarningSystem(gps_t *current_pos, double EgoVehicle_cte, uint8_t tablet_spd);


#define GO 1
#define STOP 2

#define stop_TL 1
#define stop_CAM 2
#define stop_RDR 3
struct Control{
	uint8_t sensor; // 1: traffic light, 2: cam_obj, 3: rdr_obj
    uint8_t mode;
    double stopdist;
};

extern uint8_t global_trafficlight_sig;
extern double stopline_dist;
extern std::vector<double> trafficlight_x, trafficlight_y, trafficlight_azi, trafficlight_dist;
extern std::vector<double> stopline_x, stopline_y;
extern vector<int_int_pair> stopline_idx;
extern uint32_t sl_cnt;

#define RDROBJ_SIZE 16
struct RadarYXV{
	double y;
	double x;
	int velocity;
	double cte;
};
extern RadarYXV RDR_Obj[RDROBJ_SIZE];

#define CAMOBJ_SIZE 20
struct CameraYXC{
	double y;
	double x;
	int c;
	int risk;
	double cte;
};
extern CameraYXC CAM_Obj[CAMOBJ_SIZE];

extern bool PDWS_flg;

void chk_RoundaboutVehicle(gps_t *current_pos);
void chk_RearLeftRDR(gps_t *current_pos);
void chk_CrosswalkPerson(gps_t *current_pos);
bool comparator ( const int_int_pair& l, const int_int_pair& r);
void TrafficLight_init(void);
void chk_TrafficLight(gps_t *current_pos);
Control DecisionMaking(gps_t *current_pos);
