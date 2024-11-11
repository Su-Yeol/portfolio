extern uint16_t SIMUL_CYCLE_MS;
extern double global_utc, global_azimuth, global_latitude, global_longitude;
extern char global_gpsstat;

#define REF_SIZE 100000 // 221130 size upp 
extern double global_distance[REF_SIZE];
extern double REF_Utc[REF_SIZE], REF_Azi[REF_SIZE], REF_Lat[REF_SIZE], REF_Lon[REF_SIZE], REF_Spd[REF_SIZE];
extern int simul_cnt;
extern bool set_pos;
extern bool TurnSigLeft, TurnSigRight;


void SIMULATION(void);