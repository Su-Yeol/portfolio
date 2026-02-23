#ifndef __UDP_PACKET_H__
#define __UDP_PACKET_H__
// #include "katech/driving_trajectory_proxy.h"

    typedef struct __packed{
        katech::TrajectoryPoint TrajectoryPointVector[50];
        katech::Vector3DStruct Position;
        double Target_speed;
        bool Drive_Mode;
        double Emergency_acceleration;
        std::uint8_t Turn_Signal;
        std::uint8_t sizeof_trajectory;
    } xDrivingTrajectory_UdpPacket;


#endif