#ifndef __UDP_PACKET_H__
#define __UDP_PACKET_H__
// #include "adcm/driving_trajectory_proxy.h"

    typedef struct __packed{
        adcm::TrajectoryPoint TrajectoryPointVector[50];
        adcm::Vector3DStruct Position;
        double Target_speed;
        bool Drive_Mode;
        double Emergency_acceleration;
        std::uint8_t Turn_Signal;
        std::uint8_t sizeof_trajectory;
    } xDrivingTrajectory_UdpPacket;


#endif