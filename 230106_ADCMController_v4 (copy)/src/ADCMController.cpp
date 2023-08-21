#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <string.h> // strtok 함수가 선언된 헤더 파일

#include <thread>
#include <sys/time.h>
#include <math.h>
#include <exception> // 예외처리

// user-include
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "Tracking.h"
#include "ConfigParser.h"
#include "Key.h"
#include "DecisionMaking.h"
#include "Simulation.h"
#include "ibeolidar.h"

// key thread
#include <term.h>
#include <termios.h>
#include <unistd.h>  /* For open(), creat() */
#include <fcntl.h>   /* For O_RDWR */


using namespace std;

CConfigParser UserCfg("./config.ini");

const int ADCMRxPort1 = UserCfg.GetInt("ADCMRxPort1"); // Lat/Lon/Azi
const int ADCMRxPort2 = UserCfg.GetInt("ADCMRxPort2"); // Car velocity
const int ADCMTxPort1 = UserCfg.GetInt("ADCMTxPort1"); // SteerAng ConrolVal
const int TeleconsRxPort = UserCfg.GetInt("TeleconsRxPort"); // Path

string TeleconsAddr = UserCfg.GetString("TeleconsAddr");
string NE5Addr = UserCfg.GetString("NE5Addr");
string REF_FILE = UserCfg.GetString("REF_FILE");

bool RECORD_ON = UserCfg.GetInt("RECORD_ON");
string RECORD_PATH = UserCfg.GetString("RECORD_PATH");

bool GPS_RECORD_ON = UserCfg.GetInt("GPS_RECORD_ON");
string GPS_RECORD_PATH = UserCfg.GetString("GPS_RECORD_PATH");

bool SIMUL_ON = UserCfg.GetInt("SIMUL_ON");
bool Tablet_ON = UserCfg.GetInt("Tablet_ON");

uint16_t SIMUL_CYCLE_MS = UserCfg.GetInt("SIMUL_CYCLE_MS");
uint16_t CONTROL_CYCLE_MS = UserCfg.GetInt("CONTROL_CYCLE_MS");
uint16_t GPS_CYCLE_MS = UserCfg.GetInt("GPS_CYCLE_MS");

uint8_t RefVelocity = UserCfg.GetInt("RefVelocity");



gps_t egovehicle_pos;
bool exit_flag = false;
int SpdControl = RefVelocity;
uint8_t Tablet_SpdControl;

bool LeftAllowed=0, RightAllowed=0;

bool PathFollower = 1;

uint8_t longi_target_spd = 5;
string traffic_sig = "";

bool RDR_L_Obj=0, RDR_R_Obj=0;
bool FL_Obj=0, FR_Obj=0, L_Obj=0, R_Obj=0, RL_Obj=0, RR_Obj=0;
bool keyboard_spdflg = 0;
uint8_t global_delay_cnt = 0;
bool CAM_FLG = 0, RDR_FLG = 0;
bool TurnLeftsys = 0, TurnRightsys = 0; 

#define RX_BUFSIZE 1024

double stod_u(string str){
  const char* s = str.c_str();
  double rez = 0, fact = 1;
  if (*s == '-'){
    s++;
    fact = -1;
  };
  for (int point_seen = 0; *s; s++){
    if (*s == '.'){
      point_seen = 1; 
      continue;
    };
    int d = *s - '0';
    if (d >= 0 && d <= 9){
      if (point_seen) fact /= 10.0f;
      rez = rez * 10.0f + (double)d;
    };
  };
  return rez * fact;
};

void GPS_Receiver()
{
    UDPSocket GPS2ADCM;
    GPS2ADCM.setLocalPort(ADCMRxPort1);

    double tmp_utc = 0.0;
    double tmp_lat = 0.0, tmp_lon = 0.0, tmp_azi = 0.0;
    char tmp_stat;

    double heading_offset = 90;
    
    //uint8_t buf0[RX_BUFSIZE];
    char buf0[RX_BUFSIZE]; // GPS char!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    int recvlen = 0;
    int comma_cnt = 0;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char t_buf[50], gps_save_path[100], format[5] = ".txt";
    sprintf(t_buf, "%02d%02d%02d-%02d%02d%02d", tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(gps_save_path, "%s%s.txt", GPS_RECORD_PATH.c_str(), t_buf);

    FILE *f_gps_record;
    if (GPS_RECORD_ON)
    {
        f_gps_record = fopen(gps_save_path, "w");
        if (f_gps_record == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
    }

    string str_lat, str_lon;

    struct timeval t1, t2;
    gettimeofday(&t1, NULL);   
    while (1)
    {
        try{
            recvlen = GPS2ADCM.receive((char *)buf0);
            // $GNRMC,012443.62,A,3644.44411,N,12707.19776,E,0.022,241.57,131022,8.34,W,R,V*51
            // "4092/KST/Lat/Lon/Azi/Stat/"
            if(buf0[0] == '4' && buf0[1] == '0' && buf0[2] == '9' && buf0[3] == '2'){
                int comma_cnt = 0;
                string str, output;
                char *ptr = strtok((char *)buf0, "/");
                while (ptr != NULL)
                {
                    if(comma_cnt == 1){
                        str = ptr;
                        tmp_utc = stod(str);
                    }
                    else if(comma_cnt == 2){
                        str = ptr;
                        if(str.size() > 0){
                            tmp_lat = stod(str.substr(0, 2)) + stod(str.substr(2)) / 60.0;
                        }
                    }
                    else if(comma_cnt == 3){
                        str = ptr;
                        if(str.size() > 0){
                            tmp_lon = stod(str.substr(0, 3)) + stod(str.substr(3)) / 60.0;
                        }
                    }
                    else if(comma_cnt == 4){
                        str = ptr;
                        if(str.size() > 0){
                            tmp_azi = stod(str);
                        }
                    }
                    else if(comma_cnt == 5){
                        str = ptr;
                        if(str.size() > 0){
                            tmp_stat = ptr[0];
                        }
                    }
                    
                    ptr = strtok(NULL, "/"); // 다음 문자열을 잘라서 포인터를 반환
                    comma_cnt++;
                }
                // global_output = output;
                // printf("%s\n", buf0);
                // tmp_utc  = ((buf0[4] << 24) + (buf0[5] << 16) + (buf0[6] << 8) + buf0[7]) * 0.01;
                // tmp_azi  = ((buf0[8] << 24) + (buf0[9] << 16) + (buf0[10] << 8) + buf0[11]) * 0.01 - 270.;
                // tmp_lat  = ((buf0[12] << 24) + (buf0[13] << 16) + (buf0[14] << 8) + buf0[15]) * 0.00001;
                // tmp_lon  = ((buf0[16] << 24) + (buf0[17] << 16) + (buf0[18] << 8) + buf0[19]) * 0.00001;
                // tmp_stat = buf0[20];

                // str_lat = to_string(tmp_lat);
                // str_lon = to_string(tmp_lon);

                // printf("%.10f/%s/%.10f/%s\n", tmp_lat, str_lat.c_str(), tmp_lon, str_lon.c_str());
            }
            
            if (GPS_RECORD_ON){
                fprintf(f_gps_record, "%.2f/%.2f/%.7f/%.7f/%c\n", tmp_utc, tmp_azi, tmp_lat, tmp_lon, tmp_stat);
            }
                
            gettimeofday(&t2, NULL);
            double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);
            if (diffTime_ms >= GPS_CYCLE_MS)
            {
                gettimeofday(&t1, NULL);

                global_utc = tmp_utc;
                global_azimuth = tmp_azi * -1 + heading_offset; // 체크필요
                global_latitude = tmp_lat;
                global_longitude = tmp_lon;
                global_gpsstat = tmp_stat;
            }

            // std::cout << global_utc << '/' <<  global_azimuth << '/' <<\
            // global_latitude << '/' << global_longitude << '/' << global_gpsstat << std::endl;

            if(exit_flag){
                break;
            }
        }
        catch (std::out_of_range& e) {
		    std::cout << "<GPS_Receiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<GPS_Receiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<GPS_Receiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    if (GPS_RECORD_ON)
     fclose(f_gps_record);
    return;
}

void CAM_Receiver()
{
    UDPSocket CAM2ADCM;
    CAM2ADCM.setLocalPort(4910);

    int recvlen = 0;
    char buf1[RX_BUFSIZE];
    uint8_t trafficlight_sig;

    int turnleft_ref = 150;
    int turnright_ref = -150;

    while (1)
    {
        try{
            recvlen = CAM2ADCM.receive((char *)buf1);
            if (buf1[0] == 0x04 && buf1[1] == 0x09 && buf1[2] == 0x01 && buf1[3] == 0x00)
            {
               trafficlight_sig = buf1[4];

               if(global_curvature < turnright_ref){ // TURN RIGHT 
                    trafficlight_sig = GO;
               }
               else {
                if(trafficlight_sig == 1){
                    trafficlight_sig = GO; // green
                    traffic_sig = "G";

                    if(global_curvature > turnleft_ref){ 
                        trafficlight_sig = STOP; 
                    }   
                }
                else if(trafficlight_sig == 2) {
                    trafficlight_sig = STOP; // red
                    traffic_sig = "R";
                }
                else if(trafficlight_sig == 3) {
                    trafficlight_sig = STOP; // orange
                    traffic_sig = "O";
                }
                else if(trafficlight_sig == 4) {
                    trafficlight_sig = GO; // g + left
                    traffic_sig = "GL";
                }
                else if(trafficlight_sig == 5) {
                    trafficlight_sig = GO; // g + orange
                    traffic_sig = "GO";
                }
                else if(trafficlight_sig == 6) // r + left
                {
                    traffic_sig = "RL";
                    if(global_curvature > turnleft_ref){
                        trafficlight_sig = GO; // left
                    }
                    else{
                        trafficlight_sig = STOP; // red(stop) 
                    }   
                }
                else if(trafficlight_sig == 7) {
                    traffic_sig = "RO";
                    trafficlight_sig = STOP; // r + orange
                }
               }
               global_trafficlight_sig = trafficlight_sig;
            //    printf("global_trafficlight_sig: %d\n", global_trafficlight_sig);
            }

            if(exit_flag){
                break;
            }
        }
        catch (std::out_of_range& e) {
		    std::cout << "<CAM_Receiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<CAM_Receiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<CAM_Receiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    return;
}

void NE5_Receiver()
{
    UDPSocket NE2ADCM;
    NE2ADCM.setLocalPort(ADCMRxPort2);

    struct timeval t1, t2;

    double precision_speed = 0.0;
    int recvlen = 0;
    uint8_t buf1[RX_BUFSIZE];
    int16_t buf_yxc[10][3];
    int16_t buf_rdr_yxv[16][3];
    
    const int set_delay = 20; // 50ms * cnt
    static uint8_t delay_cnt = 0;

    std::cout << "Start NE5_Receiver Func!" << std::endl;
    std::cout << "NE5_Receiver_RxPort : " << ADCMRxPort2 << std::endl;

    gettimeofday(&t1, NULL);
    while (1)
    {
        try{
            recvlen = NE2ADCM.receive((char *)buf1);
            if (buf1[0] == 0x00 && buf1[1] == 0x00 && buf1[2] == 0x04 && buf1[3] == 0x0F)
            {
                target_cnt = (buf1[5] << 8) + (buf1[4] << 0);
                SystemPriorityMode = (buf1[7]);
                precision_speed = (uint32_t)((buf1[8] << 24) + (buf1[9] << 16) + (buf1[10] << 8) + (buf1[11] << 0)) / 100000.0 - 100.0;
                global_vehicle_spd = precision_speed;
            
                // trafficlight_signal = buf1[12];
                // Objdist = buf1[13];
                // pedestrian_stop = buf1[14];
                // LeftAllowed = buf1[15];
                // RightAllowed = buf1[16];
                
                // FL_Obj = (buf1[17] >> 3) & 0x01;
                // FR_Obj = (buf1[17] >> 2) & 0x01;
                RL_Obj = (buf1[17] >> 1) & 0x01;
                RR_Obj = (buf1[17] >> 0) & 0x01;
                
                uint8_t RDRFusion = (buf1[17]>>4) & 0x0F;
                if(RDRFusion == 2 || RDRFusion == 3)
                    L_Obj  = 1;
                else L_Obj  = 0;

                if(RDRFusion == 1 || RDRFusion == 3)
                    R_Obj  = 1;
                else R_Obj  = 0;


                FrontSCC_ObjDstVal = (buf1[19] << 8) + (buf1[18] << 0) / 10;

                FL_Obj = 0;
                FR_Obj = 0;
                for(int k=0; k<CAMOBJ_SIZE; k++){
                    buf_yxc[k][0] = (buf1[20+(k*5+1)]<<8) + (buf1[20+(k*5+0)]);
                    buf_yxc[k][1] = (buf1[20+(k*5+3)]<<8) + (buf1[20+(k*5+2)]);
                    buf_yxc[k][2] = (buf1[20+(k*5+4)]);

                    if (buf_yxc[k][1] > 2048) // unsigned -> signed
				        buf_yxc[k][1] = buf_yxc[k][1] - 4096;

                    // CAM_Obj[k].y = buf_yxc[k][0] * 0.041265;
                    CAM_Obj[k].y = buf_yxc[k][0] * 0.0477725; // factor
                    CAM_Obj[k].x = buf_yxc[k][1] * 0.009484;
                    CAM_Obj[k].c = buf_yxc[k][2];

                    CAM_Obj[k].x *= -1;

                    
                    // if(CAM_Obj[k].y >= 0 && CAM_Obj[k].y <= 25 && CAM_Obj[k].x < 0 && CAM_Obj[k].x >= -7 ){
                    //     FL_Obj = 1;
                    // }

                    // if(CAM_Obj[k].y >= 0 && CAM_Obj[k].y <= 25 && CAM_Obj[k].x > 0 && CAM_Obj[k].x <= 7 ){
                    //     FR_Obj = 1;
                    // }
                }

                RDR_L_Obj = 0;
                RDR_R_Obj = 0;
                for(int k=0; k<RDROBJ_SIZE; k++){
                    buf_rdr_yxv[k][0] = (buf1[70+(k*5+1)]<<8) + (buf1[70+(k*5+0)]);
                    buf_rdr_yxv[k][1] = (buf1[70+(k*5+3)]<<8) + (buf1[70+(k*5+2)]);
                    buf_rdr_yxv[k][2] = (buf1[70+(k*5+4)]);

                    // if (buf_rdr_yxv[k][1] > 2048) // unsigned -> signed
				    //     buf_rdr_yxv[k][1] = buf_yxc[k][1] - 4096;;

                    RDR_Obj[k].y = buf_rdr_yxv[k][0] * 0.01; // factor
                    RDR_Obj[k].x = buf_rdr_yxv[k][1] * 0.01;

                    // obj_velocity = ego vehicle velocity + obj relative velocity
                    RDR_Obj[k].velocity = global_vehicle_spd + buf_rdr_yxv[k][2];

                    RDR_Obj[k].x *= -1;

                    if(RDR_Obj[k].y >= -30 && RDR_Obj[k].y <= 25 && RDR_Obj[k].x < -1.8 && RDR_Obj[k].x >= -4.5 ){
                        RDR_L_Obj = 1;
                    }

                    if(RDR_Obj[k].y >= -30 && RDR_Obj[k].y <= 25 && RDR_Obj[k].x > 1.8 && RDR_Obj[k].x <= 4.5 ){
                        RDR_R_Obj = 1;
                    }
                }
            }

            gettimeofday(&t2, NULL);
            double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);
            if (diffTime_ms >= 50)
            {
                gettimeofday(&t1, NULL);

                if(delay_cnt-- == 0){
                    delay_cnt = 1;

                    LeftAllowed = 1;
                    RightAllowed = 1;
                }

                if ((RL_Obj == 1) || (L_Obj == 1) || (FL_Obj == 1) || (RDR_L_Obj == 1)){
                    LeftAllowed = 0;
                    delay_cnt = set_delay;
                }
            
                if ((RR_Obj == 1) || (R_Obj == 1) || (FR_Obj == 1) || (RDR_R_Obj == 1)){
                    RightAllowed = 0;
                    delay_cnt = set_delay; 
                }

                global_delay_cnt = delay_cnt;
            }

            if(exit_flag){
                break;
            }
        }
        catch (std::out_of_range& e) {
		    std::cout << "<NE5_Receiver> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<NE5_Receiver> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<NE5_Receiver> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }
    return;
}

void GetREFFile(const char *file)
{
    double x1, y1, x2, y2;

    FILE *ref = fopen(file, "ro");
    char line[1024], c;

    global_waypoint_cnt = 0;
    fseek(ref, 0, SEEK_SET);
    while (1)
    {
        fgets(line, 1024, ref);
        c = strlen(line);
        if (c == 0)
        {
            break;
        }
        global_waypoint_cnt++;
        line[0] = '\0';
    }
    fseek(ref, 0, SEEK_SET);

    std::cout << "global_waypoint_cnt :" << global_waypoint_cnt << std::endl;
    printf("Trajectory Generate ... %d line\n", global_waypoint_cnt);

    memset(&REF_Utc, 0, REF_SIZE);
    memset(&REF_Azi, 0, REF_SIZE);
    memset(&REF_Lat, 0, REF_SIZE);
    memset(&REF_Lon, 0, REF_SIZE);
    memset(&REF_Stat, 0, REF_SIZE);
    memset(&global_distance, 0, REF_SIZE);
    REF_Lat_plot.clear();
    REF_Lon_plot.clear();
    path_50m_origin_x.clear();
    path_50m_origin_y.clear();
    path_log_Longitude.clear();
    path_log_Latitude.clear();
    temp_x.clear();
    temp_y.clear();
    path_50m_end_trace = 0;
    path_50m_start_trace = 0;
    global_last_trace = 0;
    global_lookahead_cnt = 0;
    global_min_distance_cnt = 0;

    for (int i = 0; i < global_waypoint_cnt - 1; i++)
    {
        fscanf(ref, "%lf/%lf/%lf/%lf/%c", &REF_Utc[i], &REF_Azi[i], &REF_Lat[i], &REF_Lon[i], &REF_Stat[i]);
        REF_Lat_plot.push_back(REF_Lat[i] * 111000.);
        REF_Lon_plot.push_back(REF_Lon[i] * 88800.);
    }

    if (global_waypoint_cnt >= 1)
    {
        for (int i = 0; i < global_waypoint_cnt - 2; i++)
        {
            x1 = REF_Lon[i] * 88800.;
            x2 = REF_Lon[i + 1] * 88800.;
            y1 = REF_Lat[i] * 111000.;
            y2 = REF_Lat[i + 1] * 111000.;
            global_distance[i] = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

            // 230109 MODIFIED BY JDS
            double lon_x1 = REF_Lon[i] * M_PI/180.;
            double lon_x2 = REF_Lon[i + 1] * M_PI/180.;
            double lat_y1 = REF_Lat[i] * M_PI/180.;
            double lat_y2 = REF_Lat[i + 1] * M_PI/180.;

	        double cal_y = sin(lon_x2-lon_x1) * cos(lat_y2);
	        double cal_x = cos(lat_y1) * sin(lat_y2) - sin(lat_y1) * cos(lat_y2) * cos(lon_x2-lon_x1);
            REF_Azi[i] = (atan2(cal_y,cal_x) * 180./M_PI);
        }
    }
    global_distance[global_waypoint_cnt + 1] = 0.0;

    gps_t curr_pos;
    get_current_gps(&curr_pos);
    get_first_path_50m(&curr_pos);
}

void key_thread()
{
    try {
        char key;
        for (; !(key == '\n');)
        {
            key = getch();
            if (key == 'q') exit_flag = true;
            if (key == '1' || key == '-') zoom_level++;
            if (key == '2' || key == '+') zoom_level--;
            // if (key == '1')
            // {
            //     printf("%.2f/%.2f/%.7f/%.7f/%c\n", global_utc, global_azimuth, global_latitude, global_longitude, global_gpsstat);
            // }
            if (key == '3'){
                PathFollower ^= 1;
                cout << "[PathFollower] " << PathFollower << endl;
            }
            if (key == '4'){
                keyboard_spdflg ^= 1;
                cout << "[keyboard_spdflg] " << keyboard_spdflg << endl;
            }
            if (key == '5'){
                CAM_FLG ^= 1;
            }
            if (key == '6'){
                RDR_FLG ^= 1;
            }
            if (key == '7'){
                global_azimuth += 10;
                printf("global_azimuth : %lf\n", global_azimuth);
            }
            if (key == '8'){
                global_azimuth -= 10;
                printf("global_azimuth : %lf\n", global_azimuth);
            }
            if (key == '9'){
                CConfigParser UserCfg("./config.ini");
                double set_lat = UserCfg.GetFloat("set_lat");
                double set_lon = UserCfg.GetFloat("set_lon");

                global_latitude = set_lat;
                global_longitude = set_lon;
                simul_cnt = 0;
                set_pos ^= 1;

                //printf("[set_pos : %d] %lf / %lf \n", set_pos, global_latitude, global_longitude);
            }
            if (key == 'o'){
                TurnSigLeft ^= 1;
                printf("TurnSigLeft : %d\n", TurnSigLeft);
            }
            if (key == 'p'){
                TurnSigRight ^= 1;
                printf("TurnSigRight : %d\n", TurnSigRight);
            }
            if (key == '.'){
                SpdControl += 5;
                cout << "[SpdControl] " << SpdControl << endl;
            }
            if (key == ','){
                SpdControl -= 5;
                cout << "[SpdControl] " << SpdControl << endl;
            }
            if (key == 'm'){
                front_path_distance += 10.;
                cout << "[front_path_distance] " << front_path_distance << endl;
            }
            if (key == 'n'){
                front_path_distance -= 10.;
                cout << "[front_path_distance] " << front_path_distance << endl;
            }
            if (key == 'l'){ 
                max_lookahead_distance += 10.;
                cout << "[max_lookahead_distance] " << max_lookahead_distance << endl;
            }
            if (key == 'k'){
                max_lookahead_distance -= 10.;
                cout << "[max_lookahead_distance] " << max_lookahead_distance << endl;
            }
            if (key == 'j'){ 
                gain += 0.01;
                cout << "[gain] " << gain << endl;
            }
            if (key == 'h'){
                gain -= 0.01;
                cout << "[gain] " << gain << endl;
            }
            
            if (key == '0'){
                REF_FILE = UserCfg.GetString("REF_FILE");
                GetREFFile(REF_FILE.c_str());
                simul_cnt = 0;
                get_current_gps(&egovehicle_pos);
                bool res = get_path_50m(&egovehicle_pos);
            }
        }
	}
    catch (std::out_of_range& e) {
        std::cout << "<key_thread> Out_of_range Error" << '\n';
    }
    catch (std::length_error& e) {
        std::cout << "<key_thread> Length Error" << '\n';
    }
    catch(std::exception& e){
        std::cout << "<key_thread> EXCEPTION " << '\n';
        std::cout << e.what() << '\n';
    }
}

int main(int argc, char *argv[])
{
    struct timeval t1, t2, t3, t4, t5, t6;
    double x1, y1, x2, y2;
    double _handle1, _handle2, _handle3, _handle4;
    int cnt = 0, thetaCurv_cnt = 0, thetaZero_cnt = 0;
    char p_buf[50];

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char t_buf[50], save_path[100], format[5] = ".txt";
    sprintf(t_buf, "%02d%02d%02d-%02d%02d%02d", tm.tm_year % 100, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    sprintf(save_path, "%s%s.txt", RECORD_PATH.c_str(), t_buf);

    uint16_t turnsig_delay = 0;

    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "<ADCMController User Config>-----------------------" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "[RECORD_ON] " << RECORD_ON << std::endl;
    std::cout << "[RECORD_PATH] " << save_path << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "[SIMUL_ON] " << SIMUL_ON << std::endl;
    std::cout << "[Tablet_ON] " << Tablet_ON << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    // std::cout << "[ADCMRxPort1] " << ADCMRxPort1 << std::endl;
    std::cout << "[ADCMRxPort2] " << ADCMRxPort2 << std::endl;
    std::cout << "[ADCMTxPort1] " << ADCMTxPort1 << std::endl;
    std::cout << "[NE5Addr] " << NE5Addr << std::endl;
    std::cout << "[REF_FILE] " << REF_FILE << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    // sleep(3);

    FILE *f_record;
    if (RECORD_ON)
    {
        f_record = fopen(save_path, "w");
        if (f_record == NULL)
        {
            printf("Error opening file!\n");
            exit(1);
        }
    }

    uint8_t UDPTxBuffer[50] = {
        0,
    };

    UDPSocket ADCM2NE;
    bool enable_broadcast = false;
    ADCM2NE.setDestination(NE5Addr, ADCMTxPort1, enable_broadcast);

    RoadInfo_init();
    GPS_Tracking_init();
    
    // Thread
    std::thread t_GPS, t_NE5, t_VIEWER, t_SIMUL, t_KEY, t_IBEO;
    std::thread t_CAM;

    //t_CAM = std::thread(CAM_Receiver);

    if (SIMUL_ON) t_SIMUL = std::thread(SIMULATION);
    else{
        t_GPS = std::thread(GPS_Receiver);
        t_NE5 = std::thread(NE5_Receiver);
    }

    //t_IBEO = std::thread(ibeo);
    t_KEY = std::thread(key_thread);
    

    #ifdef MATPLOTLIBCPP
    
    t_VIEWER = std::thread(Viewer);
    #endif

    PathVertexVct* pMainPath = new PathVertexVct();
    TCPSocket PathManager;
    if(Tablet_ON){
        PathManager.addr = TeleconsAddr;
        PathManager.port = TeleconsRxPort;
        PathManager.Start_Thread();
    }
    // sleep(3);

    GetREFFile(REF_FILE.c_str());

    get_current_gps(&egovehicle_pos);
    bool res = get_path_50m(&egovehicle_pos);

    gettimeofday(&t1, NULL);
    gettimeofday(&t3, NULL);

    TrafficLight_init();

    double Control_ms = 0.0;
    while (1)
    {
        try{
            gettimeofday(&t2, NULL);
            double diffTime_ms = (t2.tv_sec - t1.tv_sec) * 1000 + ((t2.tv_usec - t1.tv_usec) / 1000);
            if (diffTime_ms >= CONTROL_CYCLE_MS)
            {
                gettimeofday(&t1, NULL);

                // C0 -> right가 플러스 left가 마이너스 방향
                global_handle = GPS_Tracking();
                get_current_gps(&egovehicle_pos);

                // Decision-Making
                Control moveout = DecisionMaking(&egovehicle_pos);

                // FCA
                float Avg_WhlSpd = global_vehicle_spd / 3.6; // km/h to m/s
                float Smin = (Avg_WhlSpd * Avg_WhlSpd) / 3.43 * 0.5;
                float ttc = FrontSCC_ObjDstVal/Avg_WhlSpd;
                // printf("Smin: %.2f / ObjDst: %.2f -> %d \n", Smin, FrontSCC_ObjDstVal, Smin > FrontSCC_ObjDstVal);

                // Control
                _handle4 = _handle3;
                _handle3 = _handle2;
                _handle2 = _handle1;
                _handle1 = global_handle;
                double CarHandle = ((_handle1 * 4. + _handle2 * 3. + _handle3 * 2. + _handle4 * 1.) / 10.0);
                int Out_Handle = (int)(CarHandle * 10. + 3600.);

                UDPTxBuffer[0] = 0x01;
                UDPTxBuffer[1] = 0x02;
                UDPTxBuffer[2] = 0x03;
                UDPTxBuffer[3] = 0x04;
                UDPTxBuffer[4] = (my_count & 0x00FF);
                UDPTxBuffer[5] = (my_count & 0xFF00) >> 8;
                UDPTxBuffer[6] = XStopFlg;
                UDPTxBuffer[7] = XStopDistance;
                UDPTxBuffer[8] = (LeftAllowed<<4) + RightAllowed;

                DOWS DriverOverride = DriverOverride_WarningSystem(&egovehicle_pos, global_cte, PathManager.m_tSpd);

                string str1, str2;
                if(moveout.mode == GO) str1 = "GO";
                else str1 = "STOP";

                if(moveout.sensor == 1) str2 = "TL";
                else if (moveout.sensor == 2) str2 = "CAM";
                else if (moveout.sensor == 3) str2 = "RDR";
                else str2 = "X";
                
                UDPTxBuffer[9] = DriverOverride.level;
                UDPTxBuffer[10] = DriverOverride.msg;

                // UDPTxBuffer[11] = PathManager.m_tTurnSig;
                if(PathManager.m_tSpdflg){
                    Tablet_SpdControl = PathManager.m_tSpd;
                    if(Tablet_SpdControl >= 50){
                        Tablet_SpdControl = 50;
                    }
                    
                    UDPTxBuffer[11] = Tablet_SpdControl;
                }
                else{
                    if(keyboard_spdflg){
                        UDPTxBuffer[11] = SpdControl;
                    }
                    else{
                        SpdProfile(&egovehicle_pos);
                        UDPTxBuffer[11] = (int)global_target_spd;
                    }  
                }

                UDPTxBuffer[12] = moveout.mode;

                double sdist = moveout.stopdist*10;
                UDPTxBuffer[13] = ((int)(sdist))&0xFF;
                UDPTxBuffer[14] = ((int)(sdist)>>8) & 0xFF;

                UDPTxBuffer[15] = PathFollower; // PathFollower
                UDPTxBuffer[16] = Out_Handle;
                UDPTxBuffer[17] = Out_Handle >> 8;
                UDPTxBuffer[18] = Out_Handle >> 16;
                UDPTxBuffer[19] = Out_Handle >> 24;
                UDPTxBuffer[20] = (PathManager.m_tTCPStatus<<4)+(TurnRightsys<<3)+(TurnLeftsys<<2)+(CAM_FLG<<1)+(RDR_FLG);

                gps_t TurnSig_pos;
                for(int i=0; i<turnsig_lati.size(); i++){
                    TurnSig_pos.longi = turnsig_longi[i];
                    TurnSig_pos.lati  = turnsig_lati[i];
                    double dist = get_distance(&egovehicle_pos, &TurnSig_pos);
                    if(dist < 1){
                        if(turnsig_v[i] == 1){
                            TurnLeftsys = 1;
                        }
                        else if(turnsig_v[i] == 2){
                            TurnRightsys = 1;
                        }
                        else{
                            TurnLeftsys = 0;
                            TurnRightsys = 0;
                        }
                    }
                }
                
                if(PathManager.m_tTurnSig > 0){
                    printf("PathManager.m_tTurnSig : %d \n", PathManager.m_tTurnSig);
                }

                // if(!LeftAllowed || !RightAllowed)
                // printf("[%d][L: %d R: %d] OBj:%d/%d/%d/%d/%d/%d  L-CAN: %d/%d \n", global_delay_cnt,LeftAllowed, RightAllowed, FL_Obj, FR_Obj, L_Obj, R_Obj, RL_Obj, RR_Obj, RDR_L_Obj, RDR_R_Obj);


                gettimeofday(&t4, NULL);
                double print_cycle_ms = (t4.tv_sec - t3.tv_sec) * 1000 + ((t4.tv_usec - t3.tv_usec) / 1000);
                if( print_cycle_ms >= 100 ){
                    //printf("EthErr:%5d T:%s -> %d TD:%.2f Ay:%lf/%lf H:%.2f\n", my_count-target_cnt, traffic_sig.c_str(), global_trafficlight_sig, stopline_dist, global_curr_ay, global_pre_ay, CarHandle);
                    // printf("[%dms] EthErr:%5d T:%s -> %d TD:%.2f Ay:%lf/%lf TSpd:%d Cur:%lf\n", (int)diffTime_ms, my_count-target_cnt, traffic_sig.c_str(), global_trafficlight_sig, stopline_dist, global_curr_ay, global_pre_ay, longi_target_spd, global_curvature);
                    // printf("EthErr:%5d T:%s -> %d TD:%.2f OBJ:%d/%d/%d/%d/%d/%d L/R: %d/%d Ay:%lf/%lf TSpd:%d\n", my_count-target_cnt, traffic_sig.c_str(), global_trafficlight_sig, stopline_dist, FL_Obj, FR_Obj, L_Obj, R_Obj, RL_Obj, RR_Obj, LeftAllowed, RightAllowed, global_curr_ay, global_pre_ay, longi_target_spd);
                    // printf("[%.2f][%s] [M: %s  S: %s  D: %lf] [lv: %d msg: %d] [L: %d R: %d D:%d] [C: %.2f] \n", global_utc, traffic_sig.c_str(), str1.c_str(), str2.c_str(), moveout.stopdist, DriverOverride.level, DriverOverride.msg, LeftAllowed, RightAllowed, global_delay_cnt, global_curvature);
                    gettimeofday(&t3, NULL);
                }
                
                my_count++;
                
                ADCM2NE.send(UDPTxBuffer, 21); // TX_BUF_SIZE = 4096 체크.
                if (RECORD_ON){
                    //fprintf(f_record, "%.2f/%.7f/%.7f/%.2f/%c/%d/%.2f/%d/%d\n", global_utc, global_latitude, global_longitude, global_azimuth, global_gpsstat, my_count, CarHandle, (int)global_target_spd, UDPTxBuffer[13]);
                    // TIME, Lat, Lon, Azimuth, CurrSpd, CTE, HANDLE,
                }
                    
            }

            if(PathManager.m_DataSendConfirm == true){
                // printf("SAVE\n");
                // printf("PathManager.DataSendConfirm: %d\n", PathManager.m_DataSendConfirm);
                PathVertexVct* pTempPath = new PathVertexVct();
                
                bool res = PathManager.GetPath(pTempPath, global_longitude, global_latitude);
                if (res)
                {
                    pMainPath->DeleteAll();
                    pMainPath->swap(*pMainPath);
                    pMainPath->Move(pTempPath);

                    sprintf(p_buf, "./data/Path/path_%d.txt", cnt);
                    printf("FileName: %s\n", p_buf);
                    FILE *f_MainPath;
                    f_MainPath = fopen(p_buf, "w");
                    if (f_MainPath == NULL)
                    {
                        printf("Error opening file!\n");
                        exit(1);
                    }

                    for (int i=0; i<pMainPath->size() - 1; i++)
                    {
                        fprintf(f_MainPath, "0/0/%.7f/%.7f/0\n", pMainPath->at(i)->y, pMainPath->at(i)->x);
                    }

                    REF_FILE = p_buf;
                    GetREFFile(REF_FILE.c_str());
                    simul_cnt = 0;
                    get_current_gps(&egovehicle_pos);
                    res = get_path_50m(&egovehicle_pos);

                    TrafficLight_init();
                    
                    cnt++;
                }
                
                delete pTempPath;

                PathManager.m_DataSendConfirm = false;
                // std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }

            if(exit_flag){
                break;
            }
        }
        catch (std::out_of_range& e) {
            std::cout << "<MAIN> Out_of_range Error" << '\n';
        }
        catch (std::length_error& e) {
            std::cout << "<MAIN> Length Error" << '\n';
        }
        catch(std::exception& e){
            std::cout << "<MAIN> EXCEPTION " << '\n';
            std::cout << e.what() << '\n';
        }
    }

    #ifdef MATPLOTLIBCPP
    t_VIEWER.join();
    #endif

    //t_IBEO.join();
    t_KEY.join();
    

    if (SIMUL_ON)  t_SIMUL.join();
    else{
        t_NE5.join();
        t_GPS.join();
    }

    if (RECORD_ON) fclose(f_record);
    return 0;
}
