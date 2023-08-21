#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>

#include <termios.h>
#include <stdbool.h>
#include <signal.h>

#include <time.h>
#include <sys/time.h>

#include "ibeolidar.h"

#define CAN_INTERFACE_0 "can0"
#define CAN_INTERFACE_1 "can1"

int sockfd , s2;
struct sockaddr_can addr , addr1;
struct ifreq ifr, ifr1;
struct canfd_frame can0_frame;
struct can_frame frame1, test;

struct sockaddr_can src_addr;
socklen_t src_addr_len;
int nbytes,nbytes1, nbytes2;

//=============================================
float AvgWheelSpeed;
float WheelSpdFL;
float WheelSpdFR;
float WheelSpdRL;
float WheelSpdRR;

int SAS_Ang_VAL3;
int SAS_Ang_VAL4;

float StrAngle;
float iBeoStrAngle;
float iBeoVehicleSpeed;

int YRSVal8;
int YRSVal9;

float yaw_rate;
float iBeoYawrate;

bool egomotion_flag;

uint8_t ObjectID;

int16_t Positionx1;
int16_t Positionx2;
int16_t Positiony1;
int16_t Positiony2;
int16_t Positionx;
int16_t Positiony;

uint16_t velocityx1;
uint16_t velocityx2;
int16_t velocityy1;
int16_t velocityy2;

int16_t velocityx;
int16_t velocityy;


int16_t BoxOrientation;
int BoxSizeX;
int BoxSizeY;

int Objectclassification;

int ObjectID4;
int prtflag;

int dsj;
//=============================================
void init_frame1(){
    frame1.can_id = 0x00;
    frame1.can_dlc = 8;
    frame1.data[0] = 0x00;
    frame1.data[1] = 0x00;
    frame1.data[2] = 0x00;
    frame1.data[3] = 0x00;
    frame1.data[4] = 0x00;
    frame1.data[5] = 0x00;
    frame1.data[6] = 0x00;
    frame1.data[7] = 0x00;
}


int ibeo(void) {

    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sockfd < 0) {
        perror("Error while opening socket");
        exit(EXIT_FAILURE);
    }

    strcpy(ifr.ifr_name, CAN_INTERFACE_0);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("Error with SIOCGIFINDEX ioctl");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    int enable_canfd = 1;

    if (setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd))) {
        perror("Error enabling CAN FD support");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    //======================================

    s2 = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s2 < 0) {
        perror("Error while opening socket");
        exit(EXIT_FAILURE);
    }

    strcpy(ifr1.ifr_name, CAN_INTERFACE_1);
    if (ioctl(s2, SIOCGIFINDEX, &ifr1) < 0) {
        perror("Error with SIOCGIFINDEX ioctl");
        close(s2);
        exit(EXIT_FAILURE);
    }

    addr1.can_family = AF_CAN;
    addr1.can_ifindex = ifr1.ifr_ifindex;

    if (bind(s2, (struct sockaddr *)&addr1, sizeof(addr1)) < 0) {
        perror("Error in socket bind");
        close(s2);
        exit(EXIT_FAILURE);
    }


    if (setsockopt(s2, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_canfd, sizeof(enable_canfd))) {
        perror("Error enabling CAN FD support");
        close(s2);
        exit(EXIT_FAILURE);
    }

    // struct can_filter filter[1];
    // filter[0].can_id = 0xA0; // 처리할 CAN 프레임의 ID 설정
    // filter[0].can_mask = CAN_SFF_MASK;
    // setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
    
    while (1) {
        src_addr_len = sizeof(src_addr);
        nbytes = recvfrom(sockfd, &can0_frame, sizeof(can0_frame), 0, (struct sockaddr *)&src_addr, &src_addr_len);
        if (nbytes < 0) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        
        if(can0_frame.can_id == 0xA0){
            WheelSpdFL = (can0_frame.data[8]+((can0_frame.data[9]&0x3F)<<8))*0.03125;
			WheelSpdFR = (can0_frame.data[10]+((can0_frame.data[11]&0x3F)<<8))*0.03125;
			WheelSpdRL = (can0_frame.data[12]+((can0_frame.data[13]&0x3F)<<8))*0.03125;
			WheelSpdRR = (can0_frame.data[14]+((can0_frame.data[15]&0x3F)<<8))*0.03125;

            //3276.8보다 크면 6553.5를  뺴줘야함
            //strangle 사양 : 승용차 450~ -450 트럭 1080~ -1080
            AvgWheelSpeed = ((WheelSpdFL + WheelSpdFR + WheelSpdRL + WheelSpdRR) / 4);
            // printf("\nAvgWheelSpeed : %f", AvgWheelSpeed);

            iBeoVehicleSpeed = AvgWheelSpeed / 0.138889;
            init_frame1();
            frame1.can_id = 0x4F1;
            frame1.can_dlc = 7;
            frame1.data[1] = (u_int16_t)(iBeoVehicleSpeed);
            frame1.data[2] = (u_int16_t)(iBeoVehicleSpeed) >> 8;

            write(s2, &frame1, sizeof(struct can_frame));
        }

        if(can0_frame.can_id == 0x125){

            SAS_Ang_VAL3 = can0_frame.data[3];
			SAS_Ang_VAL4 = can0_frame.data[4]<<8;

            StrAngle = (SAS_Ang_VAL3+SAS_Ang_VAL4)*0.1;


            // if(StrAngle >= 3276.8){
            //     printf("\nStrAngle : %f", StrAngle-6553.5);
            // }
            // else{
            //     printf("\nStrAngle : %f", StrAngle);
            // }

            iBeoStrAngle = StrAngle/0.001745;

            init_frame1();
            frame1.can_id = 0x2B0;
            frame1.can_dlc = 7;
            
            frame1.data[0] = (int16_t)iBeoStrAngle;
            frame1.data[1] = (int16_t)iBeoStrAngle>>8;
            
            //nbytes1 = write(s2, &frame1, sizeof(struct can_frame));
            
            // init_frame1();
            // frame1.can_id = 0x50A;
            // frame1.can_dlc = 8;
            
            // frame1.data[0] = 0x11;
            // frame1.data[1] = 0x00;
            // frame1.data[2] = 0x10;
            // frame1.data[3] = 0x12;
            // frame1.data[4] = 0x00;
            // frame1.data[5] = 0x00;
            // frame1.data[6] = 0x00;
            // frame1.data[7] = 0x00;
            
            //50F를 출력
            nbytes1 = write(s2, &frame1, sizeof(struct can_frame));
        }



        if(can0_frame.can_id == 0x4A){
            YRSVal8 = can0_frame.data[8];
            YRSVal9 = can0_frame.data[9]<<8;

            yaw_rate = (YRSVal8 +YRSVal9)*0.005-163.84;
            iBeoYawrate = (yaw_rate+0.714700)/0.000174;


            init_frame1();
            frame1.can_id = 0x220;
            frame1.can_dlc = 7;

            frame1.data[5] = (int16_t)iBeoYawrate;
            frame1.data[6] = ((int16_t)iBeoYawrate >>8)&0x3F;

            nbytes1 = write(s2, &frame1, sizeof(struct can_frame));
            
        }

        nbytes2 = recvfrom(s2, &frame1, sizeof(frame1), 0, (struct sockaddr *)&src_addr, &src_addr_len);
        if (nbytes < 0) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }
        if(frame1.can_id == 0x500){
            printf("===============================================\n");
            printf("NumberOfObject : %d \n", frame1.data[1]);
            printf("%d \n", frame1.data[4]);
        }

        else if( frame1.can_id == 0x502){
            test = frame1;
            ObjectID = frame1.data[0];
            Positionx1 = frame1.data[1]<<8;
            Positionx2 = frame1.data[2];
            Positiony1 = frame1.data[3]<<8;
            Positiony2 = frame1.data[4];
            Positionx = Positionx1 + Positionx2;
            Positiony = Positiony1 + Positiony2;
            velocityx1 = frame1.data[5] << 4;
            velocityx2 = (frame1.data[6]&0xF0) >> 4;
            velocityy1 = (frame1.data[6]&0x0F) << 8;
            velocityy2 = frame1.data[7];
            velocityx = velocityx1 + velocityx2;
            if(velocityx > 2048){
                velocityx -= 4096;
            }
            
            velocityy = velocityy1 + velocityy2;
            if(velocityy > 2048){
                velocityy -= 4096;
               
            }
            prtflag =1;
            // printf("\n");
            //  printf("x:%d , y:%d\n",Positionx,Positiony);
            // printf("\n");
        }
        else if(frame1.can_id == 0x503){
            // printf("vel %d\n", frame1.data[6]);
        }
        else if(frame1.can_id == 0x504){

            ObjectID4 = frame1.data[0];
            Objectclassification = frame1.data[1];
            if(prtflag == 1){
                // printf("\n");
                // printf(" class : %d, ID : %d, position x : %d, y :%d, BO : %d, BoxSize : %d, %d, Vel : %d, %d", Objectclassification,ObjectID, Positionx,Positiony,BoxOrientation,BoxSizeX, BoxSizeY,velocityx,velocityy);
                // printf("\n");
                // if( Objectclassification == 5){
                // printf("\n");
                // printf(" class : %d,position x : %d, y :%d", Objectclassification,Positionx,Positiony);
                // printf("\n");
                // }
                // if( Objectclassification == 3){
                //     // for(int i=0; i<8; ++i){
                //     //     printf("%X ", test.data[i]);
                //     // }
                //     // printf("\n");

                //     printf("\n");
                //     printf(" class : %d, ID : %d, position x : %d, y :%d, BO : %d, BoxSize : %d, %d, Vel : %d, %d", Objectclassification,ObjectID, Positionx,Positiony,BoxOrientation,BoxSizeX, BoxSizeY,velocityx,velocityy);
                //     printf("\n");
                //     }
                // else if( Objectclassification == 4){
                // printf("\n");
                // printf(" class : %d,position x : %5d, y :%5d", Objectclassification,Positionx,Positiony);
                // printf("\n");
                // }
                // else if( Objectclassification == 6){
                // printf("\n");
                // printf(" class : %d,position x : %5d, y :%5d", Objectclassification,Positionx,Positiony);
                // printf("\n");
                // }
                prtflag = 0;
            }

        }

        else if(frame1.can_id == 0x505){
            BoxOrientation = (frame1.data[5] << 8) + (frame1.data[6]);
            BoxSizeX = (frame1.data[1] << 8) + (frame1.data[2]);
            BoxSizeY = (frame1.data[3] << 8) + (frame1.data[4]);
            printf("\n");
            printf(" class : %d, ID : %d, position x : %d, y :%d, BO : %d, BoxSize : %d, %d, Vel : %d, %d", Objectclassification,ObjectID, Positionx,Positiony,BoxOrientation,BoxSizeX, BoxSizeY,velocityx,velocityy);
            printf("\n");
            
        }
        

        // if(frame1.can_id == 0x50B){
        //     //printf("Received CAN FD frame with ID 0x%x and %d bytes of data from interface index %d\n", frame1.can_id, frame1.can_dlc, src_addr.can_ifindex);
        //     //for (int i = 0; i < frame1.can_dlc; i++) { printf("%02x ", frame1.data[i]); }

        //     if(frame1.data[4] == 1){
        //         printf("\negomotion : true");
        //     }
        //     else{
        //         printf("\negomotion : false");
        //     }
        //     printf("\n");
        // }
        

        // printf("Received CAN FD frame with ID 0x%x and %d bytes of data from interface index %d\n", can0_frame.can_id, can0_frame.len, src_addr.can_ifindex);
        // for (int i = 0; i < can0_frame.len; i++) { printf("%02x ", can0_frame.data[i]); }
        // printf("\n");
    }

    close(sockfd);
    return 0;
}

