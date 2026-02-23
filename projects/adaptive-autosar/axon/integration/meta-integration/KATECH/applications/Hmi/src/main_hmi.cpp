// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// This is just a test main to test the communication API
// The different components svl-brige, video, tester, ... are used in one
// application. This could be also different applications but we
// currently have no mechanism implemeted for inter-process-communication
// between applications. We also have no execution environment in use here
// I.e. this code as nothing to do with the communication or execution API
///////////////////////////////////////////////////////////////////////

#include <thread>
#include <chrono>
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <csignal>
#include <stdio.h>
#include <random>

#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>

#include <ara/log/logger.h>
#include "logger.h"
#include "ara/core/initialization.h"

#include "collect_fault_hmi_provider.h"
#include "can_data_subscriber.h"
#include "can_data_etc_subscriber.h"
#include "routing_search_provider.h"
#include "driving_trajectory_subscriber.h"
#include "local_map_subscriber.h"
#include "perception_obstacles_subscriber.h"
#include "planning_hmi_data_subscriber.h"
#include "routing_result_subscriber.h"
#include "traffic_light_subscriber.h"
#include "vehicle_location_subscriber.h"

// ======= Process RTT =======
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <iostream>

#include <unordered_map>
#include <mutex>
#include <cstring>
// ======= Process RTT =======

std::random_device m_rd;
std::default_random_engine m_rand_eng(m_rd());
std::uniform_real_distribution<double> m_ud_10000_10000(-10000, 10000);
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::uint8_t> m_ud_0_4(0, 4);

namespace Hmi
{

std::shared_ptr<katech::CollectFaultHmi_Provider> collectFaultHmi_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gEventReceived_Count_can_data{0};
std::atomic_uint gEventReceived_Count_can_data_etc{0};
std::atomic_uint gReceivedEvent_count_driving_trajectory{0};
std::atomic_uint gEventReceived_Count_local_map{0};
std::atomic_uint gEventReceived_Count_perception_obstacles{0};
std::atomic_uint gEventReceived_Count_planning_hmi_data{0};
std::atomic_uint gEventReceived_Count_routing_result{0};
std::atomic_uint gEventReceived_Count_traffic_light{0};
std::atomic_uint gEventReceived_Count_vehicle_location{0};
std::atomic_uint gMainthread_Loopcount{0};

std::atomic_uint64_t gFaultStatus{0};
void setFault(unsigned char faultID)
{
    gFaultStatus |= (static_cast<std::uint64_t>(1) << faultID);
}

void clearFault(unsigned char faultID)
{
    gFaultStatus &= ~((static_cast<std::uint64_t>(1) << faultID));
}

std::uint64_t getFaultStatus()
{
    return gFaultStatus;
}

void sendFault()
{
    static katech::collect_fault_hmi_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Hmi.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Hmi.push_back(i);
        }
    }

    collectFaultHmi_provider->send(gCollectFault);
    katech::Log::Verbose() << "[Hmi] send collectFault Event ";
}

void SigTermHandler(int signal)
{
    if(signal == SIGTERM) {
        // set atomic exit flag
        continueExecution = false;
    }
}

bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    // register signal handler
    if(sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }

    return true;
}

void ThreadReceiveCanData()
{
    katech::Log::Info() << "Hmi ThreadReceiveCanData Start!!";
    katech::CanData_Subscriber canData_subscriber;
    canData_subscriber.init("Hmi/Hmi/RPort_can_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveCanData loop";
        bool canData_rxEvent = canData_subscriber.waitEvent(60); // wait event

        if(canData_rxEvent) {
            katech::Log::Verbose() << "canData_rxEvent";

            while(!canData_subscriber.isEventQueueEmpty()) {
                auto data = canData_subscriber.getEvent();
                gEventReceived_Count_can_data++; 
                katech::Log::Verbose() << "[EVENT] Hmi Can Data received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi can_data Event Callback";
                    
                    katech::Log::Verbose() << "WheelSpeed_FL : " << data->WheelSpeed_FL;
                    katech::Log::Verbose() << "WheelSpeed_FR : " << data->WheelSpeed_FR;
                    katech::Log::Verbose() << "WheelSpeed_RL : " << data->WheelSpeed_RL;
                    katech::Log::Verbose() << "WheelSpeed_RR : " << data->WheelSpeed_RR;
                    katech::Log::Verbose() << "ClusterSpeed : " << data->ClusterSpeed;
                    katech::Log::Verbose() << "GearState : " << data->GearState;
                    katech::Log::Verbose() << "LateralAcceleration : " << data->LateralAcceleration;
                    katech::Log::Verbose() << "LongitudinalAcceleration : " << data->LongitudinalAcceleration;
                    katech::Log::Verbose() << "Angular_Acceleration : " << data->Angular_Acceleration;
                    katech::Log::Verbose() << "Turn_Signal : " << data->Turn_Signal;
                    katech::Log::Verbose() << "Emergency_Light : " << data->Emergency_Light;
                    katech::Log::Verbose() << "SteeringWheelAngle : " << data->SteeringWheelAngle;
                    katech::Log::Verbose() << "LaneInfo.LanePosition.Left : " << data->LaneInfo.LanePosition.Left;
                    katech::Log::Verbose() << "LaneInfo.LanePosition.Right  : " << data->LaneInfo.LanePosition.Right;
                    katech::Log::Verbose() << "LaneInfo.LaneHeadingAngle.Left : " << data->LaneInfo.LaneHeadingAngle.Left;
                    katech::Log::Verbose() << "LaneInfo.LaneHeadingAngle.Right  : " << data->LaneInfo.LaneHeadingAngle.Right;
                    katech::Log::Verbose() << "LaneInfo.LaneCurvature.Left : " << data->LaneInfo.LaneCurvature.Left;
                    katech::Log::Verbose() << "LaneInfo.LaneCurvature.Right  : " << data->LaneInfo.LaneCurvature.Right;
                    katech::Log::Verbose() << "LaneInfo.LaneCurvatureRate.Left : " << data->LaneInfo.LaneCurvatureRate.Left;
                    katech::Log::Verbose() << "LaneInfo.LaneCurvatureRate.Right  : " << data->LaneInfo.LaneCurvatureRate.Right;
                    katech::Log::Verbose() << "WheelTick_FL : " << data->WheelTick_FL;
                    katech::Log::Verbose() << "WheelTick_FR : " << data->WheelTick_FR;
                    katech::Log::Verbose() << "WheelTick_RL : " << data->WheelTick_RL;
                    katech::Log::Verbose() << "WheelTick_RR : " << data->WheelTick_RR;

                    if(data->AutoDrivingMode)
                        katech::Log::Verbose() << "AutoDrivingMode On";
                    else
                        katech::Log::Verbose() << "AutoDrivingMode Off";

                    if(data->No_Autonomous_Driving)
                        katech::Log::Verbose() << "Disable Autonomous_Driving";
                    else
                        katech::Log::Verbose() << "Able Autonomous_Driving";
                }
            }
        }else{
            katech::Log::Verbose() << "Hmi can_data timeout...";
        }
    }
}

void ThreadReceiveCanDataEtc()
{
    katech::Log::Info() << "Hmi ThreadReceiveCanDataEtc Start!!";
    katech::CanDataEtc_Subscriber canDataEtc_subscriber;
    canDataEtc_subscriber.init("Hmi/Hmi/RPort_can_data_etc");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveCanDataEtc loop";
        bool canDataEtc_rxEvent = canDataEtc_subscriber.waitEvent(120); // wait event

        if(canDataEtc_rxEvent) {
            katech::Log::Verbose() << "canDataEtc_rxEvent";

            while(!canDataEtc_subscriber.isEventQueueEmpty()) {
                auto data = canDataEtc_subscriber.getEvent();
                gEventReceived_Count_can_data_etc++; 
                katech::Log::Verbose() << "[EVENT] Hmi Can Data Etc received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi can_data_etc Event Callback";
                    
                    katech::Log::Verbose() << "SeatBelt : " << data->SeatBelt;

                    if(data->Radar_Status)
                        katech::Log::Verbose() << "Radar_Status On";
                    else
                        katech::Log::Verbose() << "Radar_Status Off";

                    if(data->Head_Light)
                        katech::Log::Verbose() << "Head_Light On";
                    else
                        katech::Log::Verbose() << "Head_Light Off";

                    if(data->High_Beam)
                        katech::Log::Verbose() << "High_Beam On";
                    else
                        katech::Log::Verbose() << "High_Beam Off";
                    
                    if(data->Fog_Lights)
                        katech::Log::Verbose() << "Fog_Lights On";
                    else
                        katech::Log::Verbose() << "Fog_Lights Off";

                    if(data->Backup_Lamp)
                        katech::Log::Verbose() << "Backup_Lamp On";
                    else
                        katech::Log::Verbose() << "Backup_Lamp Off";

                    if(data->Wiper)
                        katech::Log::Verbose() << "Wiper On";
                    else
                        katech::Log::Verbose() << "Wiper Off";

                    if(data->Horn)
                        katech::Log::Verbose() << "Horn On";
                    else
                        katech::Log::Verbose() << "Horn Off";

                    katech::Log::Verbose() << "Battery_Status : " << data->Battery_Status;
                    katech::Log::Verbose() << "Battery_Gauge : " << data->Battery_Gauge;
                    katech::Log::Verbose() << "Odometer : " << data->Odometer;
                    katech::Log::Verbose() << "Driving_Distance : " << data->Driving_Distance;
                    katech::Log::Verbose() << "Avg_Elec_Consum : " << data->Avg_Elec_Consum;
                }
            }
        }else{
            katech::Log::Verbose() << "Hmi can_data_etc timeout...";
        }
    }
}

void ThreadReceiveDrivingTrajectory()
{
    katech::Log::Info() << "Hmi ThreadReceiveDrivingTrajectory Start!!";
    katech::DrivingTrajectory_Subscriber drivingTrajectory_subscriber;
    drivingTrajectory_subscriber.init("Hmi/Hmi/RPort_driving_trajectory");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveDrivingTrajectory loop";
        bool drivingTrajectory_rxEvent = drivingTrajectory_subscriber.waitEvent(120); // wait event

        if(drivingTrajectory_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Hmi Driving Trajectory received";

            while(!drivingTrajectory_subscriber.isEventQueueEmpty()) {
                auto data = drivingTrajectory_subscriber.getEvent();
                gReceivedEvent_count_driving_trajectory++;
                katech::Log::Verbose() << "[Enter] Hmi driving_trajectory Event Callback";

                auto trajectoryPoint = data->TrajectoryPointVector;
                auto position = data->Position;
                auto targetSpeed = data->Target_speed;
                auto driveMode = data->Drive_Mode;
                auto emergencyAcceleration = data->Emergency_acceleration;

                if(!trajectoryPoint.empty()) {
                    katech::Log::Verbose() << "trajectoryPoint: ";

                    for(typename std::vector<katech::TrajectoryPoint>::iterator itr = trajectoryPoint.begin(); itr != trajectoryPoint.end(); ++itr) {
                        katech::Log::Verbose() << "TrajectoryPoint.Vector3D.X : " << itr->Vector3D.x;
                        katech::Log::Verbose() << "TrajectoryPoint.Vector3D.Y : " << itr->Vector3D.y;
                        katech::Log::Verbose() << "TrajectoryPoint.Vector3D.Z : " << itr->Vector3D.yaw;
                    }

                } else {
                    katech::Log::Error() << "trajectoryPoint vector empty!!! ";
                }

                katech::Log::Verbose() << "Position.x :  " << position.x;
                katech::Log::Verbose() << "Position.y :  " << position.y;
                katech::Log::Verbose() << "Position.yaw :  " << position.yaw;

                katech::Log::Verbose() << "Target_speed :  " << targetSpeed;
                if(driveMode)
                    katech::Log::Verbose() << "Drive_Mode : true";
                else
                    katech::Log::Verbose() << "Drive_Mode : false";
                katech::Log::Verbose() << "Emergency_acceleration :  " << emergencyAcceleration;
            }
        }
    }
}

void ThreadReceivePerceptionObstacles()
{
    katech::Log::Info() << "Hmi ThreadReceivePerceptionObstacles Start!!";
    katech::PerceptionObstacles_Subscriber perceptionObstacles_subscriber;
    perceptionObstacles_subscriber.init("Hmi/Hmi/RPort_perception_obstacles");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceivePerceptionObstacles loop";
        bool perceptionObstacles_rxEvent = perceptionObstacles_subscriber.waitEvent(120);

        if(perceptionObstacles_rxEvent) {
            while(!perceptionObstacles_subscriber.isEventQueueEmpty()) {
                auto data = perceptionObstacles_subscriber.getEvent();
                gEventReceived_Count_perception_obstacles++;
                katech::Log::Verbose() << "[EVENT] Hmi Perception Obstacles received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi perception_obstacles Event Callback";
                    auto perceptionObstacleVector = data->PerceptionObstacleVector;
                    auto Lidar_Status = data->Lidar_Status;

                    if(Lidar_Status)
                        katech::Log::Verbose() << "Lidar_Status : On";
                    else
                        katech::Log::Verbose() << "Lidar_Status : Off";

                    if(!perceptionObstacleVector.empty()) {
                        katech::Log::Verbose() << "Obstacles: ";

                        for(typename std::vector<katech::Perception_obstacles>::iterator itr = perceptionObstacleVector.begin(); itr != perceptionObstacleVector.end(); ++itr) {
                            katech::Log::Verbose() << "Id : " << itr->Id;
                            katech::Log::Verbose() << "Type : " << itr->Type;
                            katech::Log::Verbose() << "Position(X) : " << itr->Position.X;
                            katech::Log::Verbose() << "Position(Y) : " << itr->Position.Y;
                            katech::Log::Verbose() << "Length : " << itr->Length;
                            katech::Log::Verbose() << "Width : " << itr->Width;
                            katech::Log::Verbose() << "Timestamp : " << itr->Timestamp;
                            katech::Log::Verbose() << "Velocity_x : " << itr->Velocity_x;
                            katech::Log::Verbose() << "Velocity_y : " << itr->Velocity_y;
                            katech::Log::Verbose() << "Heading : " << itr->Heading;
                        }
                    } else {
                        katech::Log::Verbose() << "Obstacles empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceivePlanningHmiData()
{
    katech::Log::Info() << "Hmi ThreadReceivePerceptionObstacles Start!!";
    katech::PlanningHmiData_Subscriber planningHmiData_subscriber;
    planningHmiData_subscriber.init("Hmi/Hmi/RPort_planning_hmi_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceivePerceptionObstacles loop";
        bool planningHmiData_rxEvent = planningHmiData_subscriber.waitEvent(120);

        if(planningHmiData_rxEvent) {
            while(!planningHmiData_subscriber.isEventQueueEmpty()) {
                auto data = planningHmiData_subscriber.getEvent();
                gEventReceived_Count_planning_hmi_data++;
                katech::Log::Verbose() << "[EVENT] Hmi Planning Hmi Data received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi perception_obstacles Event Callback";
                    auto No_Autonomous_Driving = data->No_Autonomous_Driving;
                    auto Noti_Autonomous_Driving_Mode = data->Noti_Autonomous_Driving_Mode;
                    auto AutoDrivingMode = data->AutoDrivingMode;

                    if(No_Autonomous_Driving)
                        katech::Log::Verbose() << "No_Autonomous_Driving : On";
                    else
                        katech::Log::Verbose() << "No_Autonomous_Driving : Off";

                    katech::Log::Verbose() << "Noti_Autonomous_Driving_Mode : " << Noti_Autonomous_Driving_Mode;

                    if(AutoDrivingMode)
                        katech::Log::Verbose() << "AutoDrivingMode : On";
                    else
                        katech::Log::Verbose() << "AutoDrivingMode : Off";
                }
            }
        }
    }
}

void ThreadReceiveRoutingResult()
{
    katech::Log::Info() << "Hmi ThreadReceiveRoutingResult Start!!";
    katech::RoutingResult_Subscriber routingResult_subscriber;
    routingResult_subscriber.init("Hmi/Hmi/RPort_routing_result");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveRoutingResult loop";
        bool routingResult_rxEvent = routingResult_subscriber.waitEvent(120);

        if(routingResult_rxEvent) {
            while(!routingResult_subscriber.isEventQueueEmpty()) {
                auto data = routingResult_subscriber.getEvent();
                gEventReceived_Count_routing_result++;
                katech::Log::Verbose() << "[EVENT] Hmi Routing Result received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi routing_result Event Callback";
                    
                    auto vRoute = data->vRoute;

                    if(!vRoute.empty()) {
                        katech::Log::Verbose() << "vRoute: ";

                        for(typename std::vector<katech::ROAD_LIST>::iterator itr = vRoute.begin(); itr != vRoute.end(); ++itr) {
                            katech::Log::Verbose() << "nRoadID : " << itr->nRoadID;
                            
                            auto vLinks = itr->vLinks;

                            if(!vLinks.empty()){
                                for(typename std::vector<katech::LINK_LIST>::iterator itr = vLinks.begin(); itr != vLinks.end(); ++itr) {
                                    katech::Log::Verbose() << "nBest : " << itr->nBest;
                                    katech::Log::Verbose() << "nLinkID : " << itr->nLinkID;
                                }
                            } else {
                                katech::Log::Verbose() << "vLinks vector empty!!! ";
                            }
                        }
                    } else {
                        katech::Log::Verbose() << "vRoute vector empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveTrafficLight()
{
    katech::Log::Info() << "Hmi ThreadReceiveTrafficLight Start!!";
    katech::TrafficLight_Subscriber trafficLight_subscriber;
    trafficLight_subscriber.init("Hmi/Hmi/RPort_traffic_light");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveTrafficLight loop";
        bool trafficLight_rxEvent = trafficLight_subscriber.waitEvent(120);

        if(trafficLight_rxEvent) {
            while(!trafficLight_subscriber.isEventQueueEmpty()) {
                auto data = trafficLight_subscriber.getEvent();
                gEventReceived_Count_traffic_light++;
                katech::Log::Verbose() << "[EVENT] Hmi Perception Obstacles received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi traffic_light Event Callback";
                    auto perceptionTrafficlightVector = data->PerceptionTrafficlightVector;
                    auto Camera_Status = data->Camera_Status;
                    auto TL_CAM_Status = data->TL_CAM_Status;

                    if(Camera_Status)
                        katech::Log::Verbose() << "Camera_Status : On";
                    else
                        katech::Log::Verbose() << "Camera_Status : Off";

                    if(TL_CAM_Status)
                        katech::Log::Verbose() << "TL_CAM_Status : On";
                    else
                        katech::Log::Verbose() << "TL_CAM_Status : Off";

                    if(!perceptionTrafficlightVector.empty()) {
                        katech::Log::Verbose() << "Traffic Light: ";

                        for(typename std::vector<katech::Perception_trafficlight>::iterator itr = perceptionTrafficlightVector.begin(); itr != perceptionTrafficlightVector.end(); ++itr) {
                            katech::Log::Verbose() << "Id : " << itr->Id;
                            auto color = static_cast<std::uint16_t>(itr->Color);

                            if(color == 82) {
                                katech::Log::Verbose() << "Color : RED";

                            } else if(color == 85) {
                                katech::Log::Verbose() << "Color : YELLOW";

                            } else if(color == 80) {
                                katech::Log::Verbose() << "Color : GREEN";

                            } else if(color == 81) {
                                katech::Log::Verbose() << "Color : GREEN_LEFT_GREEN_ARROW";

                            } else if(color == 83) {
                                katech::Log::Verbose() << "Color : RED_LEFT_GREEN_ARROW";

                            } else if(color == 90) {
                                katech::Log::Verbose() << "Color : LEFT_GREEN_ARROW";

                            } else {
                                katech::Log::Verbose() << "Color : UNKNOWN";
                            }

                            katech::Log::Verbose() << "Position(Xmin) : " << itr->Position.Xmin;
                            katech::Log::Verbose() << "Position(Ymin) : " << itr->Position.Ymin;
                            katech::Log::Verbose() << "Position(Xmax) : " << itr->Position.Xmax;
                            katech::Log::Verbose() << "Position(Ymax) : " << itr->Position.Ymax;
                            katech::Log::Verbose() << "CameraID : " << itr->CameraID;
                        }

                    } else {
                        katech::Log::Verbose() << "Traffic Light empty!!! ";
                    }
                }
            }
        }
    }
}
        
void ThreadReceiveVehicleLocation()
{
    katech::Log::Info() << "Hmi ThreadReceiveVehicleLocation Start!!";
    katech::VehicleLocation_Subscriber vehicleLocation_subscriber;
    vehicleLocation_subscriber.init("Hmi/Hmi/RPort_vehicle_location");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hmi] ThreadReceiveVehicleLocation loop";
        bool vehicleLocation_rxEvent = vehicleLocation_subscriber.waitEvent(120); // wait event

        if(vehicleLocation_rxEvent) {
            while(!vehicleLocation_subscriber.isEventQueueEmpty()) {
                auto data = vehicleLocation_subscriber.getEvent();
                gEventReceived_Count_vehicle_location++;
                katech::Log::Verbose() << "[EVENT] Hmi Vehicle Location received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hmi vehicle_location Event Callback";
                    auto MeasurementTime = data->MeasurementTime;
                    auto CurrentLane_id = data->CurrentLane_id;
                    auto PositionX = data->PositionX;
                    auto PositionY = data->PositionY;
                    auto PositionZ = data->PositionZ;
                    auto GNSS = data->GNSS;
                    auto IMU = data->IMU;
                    auto GridID = data->GridID;
                    auto CellID = data->CellID;
                    auto Accuracy = data->Accuracy;
                    auto Fix_type = data->Fix_type;
                    auto GPS_Status = data->GPS_Status;

                    katech::Log::Verbose() << "MeasurementTime :  " << MeasurementTime;
                    katech::Log::Verbose() << "CurrentLane_id :  " << CurrentLane_id;
                    katech::Log::Verbose() << "PositionX :  " << PositionX;
                    katech::Log::Verbose() << "PositionY :  " << PositionY;
                    katech::Log::Verbose() << "PositionZ :  " << PositionZ;
                    katech::Log::Verbose() << "GNSS.Heading :  " << GNSS.Heading;
                    katech::Log::Verbose() << "GNSS.Speed :  " << GNSS.Speed;
                    katech::Log::Verbose() << "IMU.Roll :  " << IMU.Roll;
                    katech::Log::Verbose() << "IMU.Pitch :  " << IMU.Pitch;
                    katech::Log::Verbose() << "IMU.Yaw :  " << IMU.Yaw;
                    katech::Log::Verbose() << "IMU.Linear_acceleration :  " << IMU.Linear_acceleration;
                    katech::Log::Verbose() << "IMU.Angular_velocity :  " << IMU.Angular_velocity;
                    katech::Log::Verbose() << "GridID :  " << GridID;
                    katech::Log::Verbose() << "CellID :  " << CellID;
                    katech::Log::Verbose() << "Accuracy :  " << Accuracy;
                    katech::Log::Verbose() << "Fix_type :  " << Fix_type;

                    if(GPS_Status)
                        katech::Log::Verbose() << "GPS_Status : On";
                    else
                        katech::Log::Verbose() << "GPS_Status : Off";
                }
            }
        } else{
            katech::Log::Verbose() << "Hmi vehicle_location timeout...";
        }
    }
}


void ThreadSendRoutingSearch()
{
    katech::Log::Info() << "Hmi ThreadSendRoutingSearch Start!!";
    katech::RoutingSearch_Provider routingSearch_provider;
    routingSearch_provider.init("Hmi/Hmi/PPort_routing_search");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // write routing_search
        katech::routing_search_Objects routingSearch;
        katech::PointStruct stopPoint;

        routingSearch.preset = m_ud_0_4(m_rand_eng);

        if(routingSearch.preset == 0 || routingSearch.preset == 1){
            routingSearch.StartPoint.X = m_ud_10000_10000(m_rand_eng);
            routingSearch.StartPoint.Y = m_ud_10000_10000(m_rand_eng);

            routingSearch.EndPoint.X = m_ud_10000_10000(m_rand_eng);
            routingSearch.EndPoint.Y = m_ud_10000_10000(m_rand_eng);
            if(routingSearch.preset == 1){
                routingSearch.StopPoint.clear();
                for (int i = 0; i < 3; ++i){
                    stopPoint.X = m_ud_10000_10000(m_rand_eng);
                    stopPoint.Y = m_ud_10000_10000(m_rand_eng);
                    routingSearch.StopPoint.push_back(stopPoint);
                }
            }
        }

        routingSearch_provider.send(routingSearch);
        katech::Log::Verbose() << "[Hmi] send routingSearch Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(13);
        setFault(14);
        setFault(15);
        setFault(16);
        setFault(17);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(13);
        clearFault(14);
        clearFault(15);
        clearFault(16);
        clearFault(17);
        sendFault();
        INFO("Clear fault");
    }
}

void ThreadMethodCallTest()
{
    katech::LocalMap_Subscriber localMap_subscriber;
    localMap_subscriber.init("Hmi/Hmi/RPort_local_map");

    std::random_device m_rd;
    std::default_random_engine m_rand_eng(m_rd());
    std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);

    String serviceFlag;
    std::uint32_t gridID = 0;
    std::uint32_t cellID = 0;
    std::uint64_t deadLine = 0;

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000)); // wait 3s
        serviceFlag = localMap_subscriber.fieldGetter_ServiceFlag();

        if(serviceFlag == "On"){
            katech::Log::Info() << "[Hmi][localMap_subscriber] GetMapData Method Call Test(mapdata)";
            gridID = m_ud_0_10000(m_rand_eng);
            cellID = m_ud_0_10000(m_rand_eng);
            deadLine = 1000;    //millisecond

            auto mapdataPtr = localMap_subscriber.MapData(gridID, cellID, deadLine);

            if(mapdataPtr != NULL){
                auto GridID = mapdataPtr->GridID;
                auto CellID = mapdataPtr->CellID;
                katech::Log::Verbose() << "mapdata.GridID : " << GridID;
                katech::Log::Verbose() << "mapdata.CellID : " << CellID;
            }else{
                katech::Log::Info() << "mapdata is NULL ...";
            }

            gEventReceived_Count_local_map++;
            
        }else{
            katech::Log::Info() << "local_map service is not provided...";
        }
    }
}

void ThreadMonitor()
{
    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(gMainthread_Loopcount == 0) {
            ERROR("Main thread Timeout!!!");

        } else {
            gMainthread_Loopcount = 0;

            if(gEventReceived_Count_can_data != 0) {
                katech::Log::Info() << "can_data Received count = " << gEventReceived_Count_can_data;
                gEventReceived_Count_can_data = 0;

            } else {
                katech::Log::Info() << "can_data event timeout!!!";
            }

            if(gEventReceived_Count_can_data_etc != 0) {
                katech::Log::Info() << "can_data_etc Received count = " << gEventReceived_Count_can_data_etc;
                gEventReceived_Count_can_data_etc = 0;

            } else {
                katech::Log::Info() << "can_data_etc event timeout!!!";
            }

            if(gReceivedEvent_count_driving_trajectory != 0) {
                katech::Log::Info() << "driving_trajectory Received count = " << gReceivedEvent_count_driving_trajectory;
                gReceivedEvent_count_driving_trajectory = 0;

            } else {
                katech::Log::Info() << "driving_trajectory event timeout!!!";
            }

            if(gEventReceived_Count_local_map != 0) {
                katech::Log::Info() << "local_map Received count = " << gEventReceived_Count_local_map;
                gEventReceived_Count_local_map = 0;

            } else {
                katech::Log::Info() << "local_map event timeout!!!";
            }

            if(gEventReceived_Count_perception_obstacles != 0) {
                katech::Log::Info() << "perception_obstacles Received count = " << gEventReceived_Count_perception_obstacles;
                gEventReceived_Count_perception_obstacles = 0;

            } else {
                katech::Log::Info() << "perception_obstacles event timeout!!!";
            }

            if(gEventReceived_Count_planning_hmi_data != 0) {
                katech::Log::Info() << "planning_hmi_data Received count = " << gEventReceived_Count_planning_hmi_data;
                gEventReceived_Count_planning_hmi_data = 0;

            } else {
                katech::Log::Info() << "planning_hmi_data event timeout!!!";
            }

            if(gEventReceived_Count_routing_result != 0) {
                katech::Log::Info() << "routing_result Received count = " << gEventReceived_Count_routing_result;
                gEventReceived_Count_routing_result = 0;

            } else {
                katech::Log::Info() << "routing_result event timeout!!!";
            }

            if(gEventReceived_Count_traffic_light != 0) {
                katech::Log::Info() << "traffic_light Received count = " << gEventReceived_Count_traffic_light;
                gEventReceived_Count_traffic_light = 0;

            } else {
                katech::Log::Info() << "traffic_light event timeout!!!";
            }

            if(gEventReceived_Count_vehicle_location != 0) {
                katech::Log::Info() << "vehicle_location Received count = " << gEventReceived_Count_vehicle_location;
                gEventReceived_Count_vehicle_location = 0;

            } else {
                katech::Log::Info() << "vehicle_location event timeout!!!";
            }
        }
    }
}

}  // namespace


// ========================================= Process RTT =========================================
static std::shared_ptr<vsomeip::application> hmi_app;
static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
static const vsomeip::instance_t INST = 0x0003;  // Instance ID
static const vsomeip::method_t ECHO   = 0x0001;  // Echo Method ID

#pragma pack(push, 1)
struct Payload {
    uint64_t seq;
    uint32_t cnt;
    uint8_t  padding[56];
};
#pragma pack(pop)

struct RecvStats {
    uint64_t recv_total = 0;
    uint32_t last_cnt   = 0;
    uint64_t lost_total = 0; // cnt 기준 유실 추정
};

static std::mutex g_mu;
static std::unordered_map<vsomeip::client_t, RecvStats> g_stats;

static inline void log_stats_periodically(vsomeip::client_t cid, const RecvStats& s) {
    if (s.recv_total % 1000 == 0) {
        katech::Log::Info() << "[Pro-Hmi] recv=" << s.recv_total
                          << " last_cnt=" << s.last_cnt
                          << " lost=" << s.lost_total;
    }
}


void on_echo_request(const std::shared_ptr<vsomeip::message> &req) {

    auto resp = vsomeip::runtime::get()->create_response(req);

    Payload p{};
    auto pl = req->get_payload();
    if (pl && pl->get_length() >= sizeof(Payload)) {
        std::memcpy(&p, pl->get_data(), sizeof(Payload));
    }

    {
        std::lock_guard<std::mutex> lk(g_mu);
        auto cid = req->get_client();   // vsomeip client id
        auto &st = g_stats[cid];
        st.recv_total++;

        if (p.cnt > st.last_cnt + 1) {
            st.lost_total += (p.cnt - (st.last_cnt + 1)); // 중간 구간 유실 추정
        }
        if (p.cnt > st.last_cnt)
            st.last_cnt = p.cnt;

        log_stats_periodically(cid, st);
    }

    if (pl && pl->get_length() > 0)
        katech::Log::Verbose() << "[Pro-Hmi] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    hmi_app->send(resp);
}

void vsomeip_server_thread() {
    hmi_app = vsomeip::runtime::get()->create_application("hmi_echo");
    hmi_app->init();
    hmi_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    hmi_app->offer_service(SVC, INST);
    hmi_app->start();
}
// ========================================= Process RTT =========================================


int main(int argc, char* argv[])
{
    std::vector<std::thread> thread_list;
    UNUSED(argc);
    UNUSED(argv);

    if(!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if(!Hmi::RegisterSigTermHandler()) {
        katech::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    katech::Log::Info() << "Hmi: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    katech::Log::Info() << "Hmi: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    katech::Log::Info() << "Ok, let's produce some Hmi data...";

    {
        Hmi::collectFaultHmi_provider = std::make_shared<katech::CollectFaultHmi_Provider>();
        Hmi::collectFaultHmi_provider->init("Hmi/Hmi/PPort_collect_fault_hmi");
        Hmi::sendFault();
    }

    thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(Hmi::ThreadReceiveCanData));
    thread_list.push_back(std::thread(Hmi::ThreadReceiveCanDataEtc));
    thread_list.push_back(std::thread(Hmi::ThreadReceiveDrivingTrajectory));
    thread_list.push_back(std::thread(Hmi::ThreadReceivePerceptionObstacles));
    thread_list.push_back(std::thread(Hmi::ThreadReceivePlanningHmiData));    
    thread_list.push_back(std::thread(Hmi::ThreadReceiveRoutingResult));
    thread_list.push_back(std::thread(Hmi::ThreadReceiveTrafficLight));
    thread_list.push_back(std::thread(Hmi::ThreadReceiveVehicleLocation));
    thread_list.push_back(std::thread(Hmi::ThreadSendRoutingSearch));
    // thread_list.push_back(std::thread(Hmi::ThreadMethodCallTest));
    thread_list.push_back(std::thread(Hmi::ThreadSendCollectFault));
    thread_list.push_back(std::thread(Hmi::ThreadMonitor));

    katech::Log::Info() << "Thread join";
    for(int i = 0; i < static_cast<int>(thread_list.size()); i++) {
        thread_list[i].join();
    }

    katech::Log::Info() << "done.";

    if(!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
