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

#include "collect_fault_localization_provider.h"
#include "matching_info_provider.h"
#include "vehicle_location_provider.h"
#include "can_data_subscriber.h"
#include "can_data_etc_subscriber.h"
#include "lane_detection_subscriber.h"
#include "local_map_subscriber.h"
#include "point_map_subscriber.h"
#include "perception_obstacles_subscriber.h"
#include "routing_result_subscriber.h"
#include "routing_search_subscriber.h"
#include "traffic_light_subscriber.h"
#include "v2x_data_subscriber.h"

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
std::uniform_real_distribution<float> m_ud_100_1000(-100, 1000);
std::uniform_int_distribution<std::uint64_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::int64_t> m_ud_1000_10000(-1000, 10000);
std::uniform_int_distribution<std::uint8_t> m_ud_0_10(0, 10);

namespace Localization
{

std::shared_ptr<adcm::CollectFaultLocalization_Provider> collectFault_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gEventReceived_Count_can_data{0};
std::atomic_uint gEventReceived_Count_can_data_etc{0};
std::atomic_uint gEventReceived_Count_lane_detection{0};
std::atomic_uint gEventReceived_Count_point_map{0};
std::atomic_uint gEventReceived_Count_perception_obstacles{0};
std::atomic_uint gEventReceived_Count_routing_result{0};
std::atomic_uint gEventReceived_Count_routing_search{0};
std::atomic_uint gEventReceived_Count_traffic_light{0};
std::atomic_uint gEventReceived_Count_v2x_data{0};
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
    static adcm::collect_fault_localization_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Localization.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Localization.push_back(i);
        }
    }

    collectFault_provider->send(gCollectFault);
    adcm::Log::Verbose() << "[Localization] send collectFault Event ";
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
    adcm::Log::Info() << "Localization ThreadReceiveCanData Start!!";
    adcm::CanData_Subscriber canData_subscriber;
    canData_subscriber.init("Localization/Localization/RPort_can_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveCanData loop";
        bool canData_rxEvent = canData_subscriber.waitEvent(60); // wait event

        if(canData_rxEvent) {
            adcm::Log::Verbose() << "canData_rxEvent";

            while(!canData_subscriber.isEventQueueEmpty()) {
                auto data = canData_subscriber.getEvent();
                gEventReceived_Count_can_data++;
                adcm::Log::Verbose() << "[EVENT] Localization Can Data received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization can_data Event Callback";
                    adcm::Log::Verbose() << "WheelSpeed_FL : " << data->WheelSpeed_FL;
                    adcm::Log::Verbose() << "WheelSpeed_FR : " << data->WheelSpeed_FR;
                    adcm::Log::Verbose() << "WheelSpeed_RL : " << data->WheelSpeed_RL;
                    adcm::Log::Verbose() << "WheelSpeed_RR : " << data->WheelSpeed_RR;
                    adcm::Log::Verbose() << "ClusterSpeed : " << data->ClusterSpeed;
                    adcm::Log::Verbose() << "GearState : " << data->GearState;
                    adcm::Log::Verbose() << "LateralAcceleration : " << data->LateralAcceleration;
                    adcm::Log::Verbose() << "LongitudinalAcceleration : " << data->LongitudinalAcceleration;
                    adcm::Log::Verbose() << "Angular_Acceleration : " << data->Angular_Acceleration;
                    adcm::Log::Verbose() << "Turn_Signal : " << data->Turn_Signal;
                    adcm::Log::Verbose() << "Emergency_Light : " << data->Emergency_Light;
                    adcm::Log::Verbose() << "SteeringWheelAngle : " << data->SteeringWheelAngle;
                    adcm::Log::Verbose() << "ControlSteering : " << data->ControlSteering;
                    adcm::Log::Verbose() << "LaneInfo.LanePosition.Left : " << data->LaneInfo.LanePosition.Left;
                    adcm::Log::Verbose() << "LaneInfo.LanePosition.Right  : " << data->LaneInfo.LanePosition.Right;
                    adcm::Log::Verbose() << "LaneInfo.LaneHeadingAngle.Left : " << data->LaneInfo.LaneHeadingAngle.Left;
                    adcm::Log::Verbose() << "LaneInfo.LaneHeadingAngle.Right  : " << data->LaneInfo.LaneHeadingAngle.Right;
                    adcm::Log::Verbose() << "LaneInfo.LaneCurvature.Left : " << data->LaneInfo.LaneCurvature.Left;
                    adcm::Log::Verbose() << "LaneInfo.LaneCurvature.Right  : " << data->LaneInfo.LaneCurvature.Right;
                    adcm::Log::Verbose() << "LaneInfo.LaneCurvatureRate.Left : " << data->LaneInfo.LaneCurvatureRate.Left;
                    adcm::Log::Verbose() << "LaneInfo.LaneCurvatureRate.Right  : " << data->LaneInfo.LaneCurvatureRate.Right;
                    adcm::Log::Verbose() << "WheelTick_FL : " << data->WheelTick_FL;
                    adcm::Log::Verbose() << "WheelTick_FR : " << data->WheelTick_FR;
                    adcm::Log::Verbose() << "WheelTick_RL : " << data->WheelTick_RL;
                    adcm::Log::Verbose() << "WheelTick_RR : " << data->WheelTick_RR;
                    adcm::Log::Verbose() << "RPM : " << data->RPM;
                    adcm::Log::Verbose() << "Accel_Pedal : " << data->Accel_Pedal;
                    adcm::Log::Verbose() << "Brake_Pedal : " << data->Brake_Pedal;

                    if(data->AutoDrivingMode) {
                        adcm::Log::Verbose() << "AutoDrivingMode On";

                    } else {
                        adcm::Log::Verbose() << "AutoDrivingMode Off";
                    }

                    if(data->No_Autonomous_Driving) {
                        adcm::Log::Verbose() << "Disable Autonomous_Driving";

                    } else {
                        adcm::Log::Verbose() << "Able Autonomous_Driving";
                    }
                }
            }
        } else{
            adcm::Log::Verbose() << "Localization can_data timeout...";
        }
    }
}

void ThreadReceiveCanDataEtc()
{
    adcm::Log::Info() << "Localization ThreadReceiveCanDataEtc Start!!";
    adcm::CanDataEtc_Subscriber canDataEtc_subscriber;
    canDataEtc_subscriber.init("Localization/Localization/RPort_can_data_etc");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveCanDataEtc loop";
        bool canDataEtc_rxEvent = canDataEtc_subscriber.waitEvent(120); // wait event

        if(canDataEtc_rxEvent) {
            adcm::Log::Verbose() << "canDataEtc_rxEvent";

            while(!canDataEtc_subscriber.isEventQueueEmpty()) {
                auto data = canDataEtc_subscriber.getEvent();
                gEventReceived_Count_can_data_etc++;
                adcm::Log::Verbose() << "[EVENT] Localization Can Data Etc received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization can_data_etc Event Callback";
                    adcm::Log::Verbose() << "SeatBelt : " << data->SeatBelt;

                    if(data->Radar_Status) {
                        adcm::Log::Verbose() << "Radar_Status On";

                    } else {
                        adcm::Log::Verbose() << "Radar_Status Off";
                    }

                    if(data->Head_Light) {
                        adcm::Log::Verbose() << "Head_Light On";

                    } else {
                        adcm::Log::Verbose() << "Head_Light Off";
                    }

                    if(data->High_Beam) {
                        adcm::Log::Verbose() << "High_Beam On";

                    } else {
                        adcm::Log::Verbose() << "High_Beam Off";
                    }

                    if(data->Fog_Lights) {
                        adcm::Log::Verbose() << "Fog_Lights On";

                    } else {
                        adcm::Log::Verbose() << "Fog_Lights Off";
                    }

                    if(data->Backup_Lamp) {
                        adcm::Log::Verbose() << "Backup_Lamp On";

                    } else {
                        adcm::Log::Verbose() << "Backup_Lamp Off";
                    }

                    if(data->Wiper) {
                        adcm::Log::Verbose() << "Wiper On";

                    } else {
                        adcm::Log::Verbose() << "Wiper Off";
                    }

                    if(data->Horn) {
                        adcm::Log::Verbose() << "Horn On";

                    } else {
                        adcm::Log::Verbose() << "Horn Off";
                    }

                    adcm::Log::Verbose() << "Battery_Status : " << data->Battery_Status;
                    adcm::Log::Verbose() << "Battery_Gauge : " << data->Battery_Gauge;
                    adcm::Log::Verbose() << "Odometer : " << data->Odometer;
                    adcm::Log::Verbose() << "Driving_Distance : " << data->Driving_Distance;
                    adcm::Log::Verbose() << "Avg_Elec_Consum : " << data->Avg_Elec_Consum;
                }
            }
        } else{
            adcm::Log::Verbose() << "Localization can_data_etc timeout...";
        }
    }
}

void ThreadReceiveLaneDetection()
{
    adcm::Log::Info() << "Localization ThreadReceiveLaneDetection Start!!";
    adcm::LaneDetection_Subscriber laneDetection_subscriber;
    laneDetection_subscriber.init("Localization/Localization/RPort_lane_detection");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveLaneDetection loop";
        bool laneDetection_rxEvent = laneDetection_subscriber.waitEvent(120); // wait event

        if(laneDetection_rxEvent) {
            adcm::Log::Verbose() << "laneDetection_rxEvent";

            while(!laneDetection_subscriber.isEventQueueEmpty()) {
                auto data = laneDetection_subscriber.getEvent();
                gEventReceived_Count_lane_detection++;
                adcm::Log::Verbose() << "[EVENT] Localization Lane Detection received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization lane_detection Event Callback";
                    auto LaneLineDetection_LeftVector = data->LaneLineDetection_LeftVector;
                    auto LaneLineDetection_RightVector = data->LaneLineDetection_RightVector;

                    if(!LaneLineDetection_LeftVector.empty()) {
                        adcm::Log::Verbose() << "LaneLineDetection_Left: ";

                        for(typename std::vector<adcm::Vector2D>::iterator itr = LaneLineDetection_LeftVector.begin(); itr != LaneLineDetection_LeftVector.end(); ++itr) {
                            adcm::Log::Verbose() << "X : " << itr->X;
                            adcm::Log::Verbose() << "Y : " << itr->Y;
                        }

                    } else {
                        adcm::Log::Verbose() << "LaneLineDetection_Left empty!!! ";
                    }

                    if(!LaneLineDetection_RightVector.empty()) {
                        adcm::Log::Verbose() << "LaneLineDetection_Right: ";

                        for(typename std::vector<adcm::Vector2D>::iterator itr = LaneLineDetection_RightVector.begin(); itr != LaneLineDetection_RightVector.end(); ++itr) {
                            adcm::Log::Verbose() << "X : " << itr->X;
                            adcm::Log::Verbose() << "Y : " << itr->Y;
                        }

                    } else {
                        adcm::Log::Verbose() << "LaneLineDetection_Right empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceivePerceptionObstacles()
{
    adcm::Log::Info() << "Localization ThreadReceivePerceptionObstacles Start!!";
    adcm::PerceptionObstacles_Subscriber perceptionObstacles_subscriber;
    perceptionObstacles_subscriber.init("Localization/Localization/RPort_perception_obstacles");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceivePerceptionObstacles loop";
        bool perceptionObstacles_rxEvent = perceptionObstacles_subscriber.waitEvent(120); // wait event

        if(perceptionObstacles_rxEvent) {
            adcm::Log::Verbose() << "perceptionObstacles_rxEvent";

            while(!perceptionObstacles_subscriber.isEventQueueEmpty()) {
                auto data = perceptionObstacles_subscriber.getEvent();
                gEventReceived_Count_perception_obstacles++;
                adcm::Log::Verbose() << "[EVENT] Localization Perception Obstacles received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization perception_obstacles Event Callback";
                    auto perceptionObstacleVector = data->PerceptionObstacleVector;
                    auto Lidar_Status = data->Lidar_Status;

                    if(Lidar_Status) {
                        adcm::Log::Verbose() << "Lidar_Status : On";

                    } else {
                        adcm::Log::Verbose() << "Lidar_Status : Off";
                    }

                    if(!perceptionObstacleVector.empty()) {
                        adcm::Log::Verbose() << "Obstacles: ";

                        for(typename std::vector<adcm::Perception_obstacles>::iterator itr = perceptionObstacleVector.begin(); itr != perceptionObstacleVector.end(); ++itr) {
                            adcm::Log::Verbose() << "Id : " << itr->Id;
                            adcm::Log::Verbose() << "Type : " << itr->Type;
                            adcm::Log::Verbose() << "Position(X) : " << itr->Position.X;
                            adcm::Log::Verbose() << "Position(Y) : " << itr->Position.Y;
                            adcm::Log::Verbose() << "Length : " << itr->Length;
                            adcm::Log::Verbose() << "Width : " << itr->Width;
                            adcm::Log::Verbose() << "Timestamp : " << itr->Timestamp;
                            adcm::Log::Verbose() << "Velocity_x : " << itr->Velocity_x;
                            adcm::Log::Verbose() << "Velocity_y : " << itr->Velocity_y;
                            adcm::Log::Verbose() << "Heading : " << itr->Heading;
                        }

                    } else {
                        adcm::Log::Verbose() << "Obstacles empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceivePointMap()
{
    adcm::Log::Info() << "Localization ThreadReceivePointMap Start!!";
    adcm::PointMap_Subscriber pointMap_subscriber;
    pointMap_subscriber.init("Localization/Localization/RPort_point_map");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceivePointMap loop";
        bool pointMap_rxEvent = pointMap_subscriber.waitEvent(120); // wait event

        if(pointMap_rxEvent) {
            adcm::Log::Verbose() << "pointMap_rxEvent";

            while(!pointMap_subscriber.isEventQueueEmpty()) {
                auto data = pointMap_subscriber.getEvent();
                gEventReceived_Count_point_map++;
                adcm::Log::Verbose() << "[EVENT] Localization Point Map received";
                auto PointMap = data->PointMap;

                if(!PointMap.empty()) {
                    adcm::Log::Verbose() << "PointMap: ";

                    for(typename std::vector<adcm::pointinfo>::iterator itr = PointMap.begin(); itr != PointMap.end(); ++itr) {
                        adcm::Log::Verbose() << "pointInfo.ID : " << itr->ID;
                        auto Roads = itr->Roads;

                        if(!Roads.empty()) {
                            for(typename std::vector<adcm::MMRoadStruct>::iterator itr = Roads.begin(); itr != Roads.end(); ++itr) {
                                adcm::Log::Verbose() << "mmroad.ID : " << itr->ID;
                                auto Predecessor = itr->Predecessor;

                                if(!Predecessor.empty()) {
                                    for(auto itr = Predecessor.begin(); itr != Predecessor.end(); ++itr) {
                                        adcm::Log::Verbose() << "=== Road.Predecessor ===";
                                        adcm::Log::Verbose() << *itr;
                                    }

                                } else {
                                    adcm::Log::Verbose() << "Road Predecessor Vector empty!!! ";
                                }

                                auto Successor = itr->Successor;

                                if(!Successor.empty()) {
                                    for(auto itr = Successor.begin(); itr != Successor.end(); ++itr) {
                                        adcm::Log::Verbose() << "=== Road.Successor ===";
                                        adcm::Log::Verbose() << *itr;
                                    }

                                } else {
                                    adcm::Log::Verbose() << "Road Successor Vector empty!!! ";
                                }

                                auto Lanes = itr->Lanes;

                                if(!Lanes.empty()) {
                                    for(auto itr = Lanes.begin(); itr != Lanes.end(); ++itr) {
                                        adcm::Log::Verbose() << "=== Road.Lane ===";
                                        adcm::Log::Verbose() << "Lane.ID : " << itr->ID;
                                        adcm::Log::Verbose() << "Lane.LaneIndex : " << itr->LaneIndex;
                                        adcm::Log::Verbose() << "Lane.LaneNo : " << itr->LaneNo;
                                        adcm::Log::Verbose() << "Lane.RoadRank : " << itr->RoadRank;
                                        adcm::Log::Verbose() << "Lane.RoadType : " << itr->RoadType;
                                        adcm::Log::Verbose() << "Lane.LinkType : " << itr->LinkType;
                                        adcm::Log::Verbose() << "Lane.MaxSpeed : " << itr->MaxSpeed;
                                        adcm::Log::Verbose() << "Lane.SectionKind : " << itr->SectionKind;
                                        adcm::Log::Verbose() << "Lane.RoadDirection : " << itr->RoadDirection;
                                        adcm::Log::Verbose() << "Lane.Direction : " << itr->Direction;
                                        adcm::Log::Verbose() << "Lane.R_LinkID : " << itr->R_LinkID;
                                        adcm::Log::Verbose() << "Lane.L_LinkID : " << itr->L_LinkID;
                                        adcm::Log::Verbose() << "Lane.Length : " << itr->Length;
                                        auto Predecessor = itr->Predecessor;

                                        if(!Predecessor.empty()) {
                                            for(auto itr = Predecessor.begin(); itr != Predecessor.end(); ++itr) {
                                                adcm::Log::Verbose() << "=== Lane.Predecessor ===";
                                                adcm::Log::Verbose() << *itr;
                                            }

                                        } else {
                                            adcm::Log::Verbose() << "Road Lane Predecessor Vector empty!!! ";
                                        }

                                        auto Successor = itr->Successor;

                                        if(!Successor.empty()) {
                                            for(auto itr = Successor.begin(); itr != Successor.end(); ++itr) {
                                                adcm::Log::Verbose() << "=== Lane.Successor ===";
                                                adcm::Log::Verbose() << *itr;
                                            }

                                        } else {
                                            adcm::Log::Verbose() << "Road Lane Successor Vector empty!!! ";
                                        }

                                        auto Geometry = itr->Geometry;

                                        if(!Geometry.empty()) {
                                            for(auto itr = Geometry.begin(); itr != Geometry.end(); ++itr) {
                                                adcm::Log::Verbose() << "=== Lane.Geometry ===";
                                                adcm::Log::Verbose() << "Lane.Geometry.x : " << itr->x;
                                                adcm::Log::Verbose() << "Lane.Geometry.y : " << itr->y;
                                                adcm::Log::Verbose() << "Lane.Geometry.z : " << itr->z;
                                            }

                                        } else {
                                            adcm::Log::Verbose() << "Road Lane Geometry Vector empty!!! ";
                                        }
                                    }

                                } else {
                                    adcm::Log::Verbose() << "Road Lane Vector empty!!! ";
                                }

                                auto Lines = itr->Lines;

                                if(!Lines.empty()) {
                                    for(auto itr = Lines.begin(); itr != Lines.end(); ++itr) {
                                        adcm::Log::Verbose() << "=== Road.Line ===";
                                        adcm::Log::Verbose() << "Line.ID : " << itr->ID;
                                        adcm::Log::Verbose() << "Line.Type : " << itr->Type;
                                        adcm::Log::Verbose() << "Line.Kind : " << itr->Kind;
                                        adcm::Log::Verbose() << "Line.R_LinkID : " << itr->R_LinkID;
                                        adcm::Log::Verbose() << "Line.L_LinkID : " << itr->L_LinkID;
                                        auto Geometry = itr->Geometry;

                                        if(!Geometry.empty()) {
                                            for(auto itr = Geometry.begin(); itr != Geometry.end(); ++itr) {
                                                adcm::Log::Verbose() << "=== Line.Geometry ===";
                                                adcm::Log::Verbose() << "Line.Geometry.x : " << itr->x;
                                                adcm::Log::Verbose() << "Line.Geometry.y : " << itr->y;
                                                adcm::Log::Verbose() << "Line.Geometry.z : " << itr->z;
                                            }

                                        } else {
                                            adcm::Log::Verbose() << "Road Line Geometry Vector empty!!! ";
                                        }
                                    }

                                } else {
                                    adcm::Log::Verbose() << "Road Line Vector empty!!! ";
                                }
                            }

                        } else {
                            adcm::Log::Verbose() << "Roads vector empty!!! ";
                        }
                    }

                } else {
                    adcm::Log::Verbose() << "PointMap vector empty!!! ";
                }
            }
        }
    }
}

void ThreadReceiveRoutingResult()
{
    adcm::Log::Info() << "Localization ThreadReceiveRoutingResult Start!!";
    adcm::RoutingResult_Subscriber routingResult_subscriber;
    routingResult_subscriber.init("Localization/Localization/RPort_routing_result");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveRoutingResult loop";
        bool routingResult_rxEvent = routingResult_subscriber.waitEvent(120);

        if(routingResult_rxEvent) {
            while(!routingResult_subscriber.isEventQueueEmpty()) {
                auto data = routingResult_subscriber.getEvent();
                gEventReceived_Count_routing_result++;
                adcm::Log::Verbose() << "[EVENT] Localization Routing Result received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization routing_result Event Callback";
                    
                    auto vRoute = data->vRoute;

                    if(!vRoute.empty()) {
                        adcm::Log::Verbose() << "vRoute: ";

                        for(typename std::vector<adcm::ROAD_LIST>::iterator itr = vRoute.begin(); itr != vRoute.end(); ++itr) {
                            adcm::Log::Verbose() << "nRoadID : " << itr->nRoadID;
                            
                            auto vLinks = itr->vLinks;

                            if(!vLinks.empty()){
                                for(typename std::vector<adcm::LINK_LIST>::iterator itr = vLinks.begin(); itr != vLinks.end(); ++itr) {
                                    adcm::Log::Verbose() << "nBest : " << itr->nBest;
                                    adcm::Log::Verbose() << "nLinkID : " << itr->nLinkID;
                                }
                            } else {
                                adcm::Log::Verbose() << "vLinks vector empty!!! ";
                            }
                        }
                    } else {
                        adcm::Log::Verbose() << "vRoute vector empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveRoutingSearch()
{
    adcm::Log::Info() << "Localization ThreadReceiveRoutingSearch Start!!";
    adcm::RoutingSearch_Subscriber routingSearch_subscriber;
    routingSearch_subscriber.init("Localization/Localization/RPort_routing_search");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveRoutingSearch loop";
        bool routingSearch_rxEvent = routingSearch_subscriber.waitEvent(120); // wait event

        if(routingSearch_rxEvent) {
            adcm::Log::Verbose() << "routingSearch_rxEvent";

            while(!routingSearch_subscriber.isEventQueueEmpty()) {
                auto data = routingSearch_subscriber.getEvent();
                gEventReceived_Count_routing_search++;
                adcm::Log::Verbose() << "[EVENT] Localization Routing Search received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization routing_search Event Callback";
                    auto preset = data->preset;
                    auto StartPoint = data->StartPoint;
                    auto EndPoint = data->EndPoint;
                    auto StopPoint = data->StopPoint;
                    adcm::Log::Verbose() << "preset : " << preset;

                    if(preset == 0 || preset == 1) {
                        adcm::Log::Verbose() << "StartPoint.X :  " << StartPoint.X;
                        adcm::Log::Verbose() << "StartPoint.X :  " << StartPoint.Y;
                        adcm::Log::Verbose() << "EndPoint.X :  " << EndPoint.X;
                        adcm::Log::Verbose() << "EndPoint.Y :  " << EndPoint.Y;

                        if(preset == 1) {
                            if(!StopPoint.empty()) {
                                adcm::Log::Verbose() << "=== StopPoint ===";

                                for(auto itr = StopPoint.begin(); itr != StopPoint.end(); ++itr) {
                                    adcm::Log::Verbose() << "StopPoint.X : " << itr->X;
                                    adcm::Log::Verbose() << "StopPoint.Y : " << itr->Y;
                                }

                            } else {
                                adcm::Log::Info() << "StopPoint Vector empty!!! ";
                            }
                        }

                    } else {
                        adcm::Log::Verbose() << "preseted routing path!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveTrafficLight()
{
    adcm::Log::Info() << "Localization ThreadReceiveTrafficLight Start!!";
    adcm::TrafficLight_Subscriber trafficLight_subscriber;
    trafficLight_subscriber.init("Localization/Localization/RPort_traffic_light");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveTrafficLight loop";
        bool trafficLight_rxEvent = trafficLight_subscriber.waitEvent(120); // wait event

        if(trafficLight_rxEvent) {
            adcm::Log::Verbose() << "trafficLight_rxEvent";

            while(!trafficLight_subscriber.isEventQueueEmpty()) {
                auto data = trafficLight_subscriber.getEvent();
                gEventReceived_Count_traffic_light++;
                adcm::Log::Verbose() << "[EVENT] Localization Perception Obstacles received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization perception_obstacles Event Callback";
                    auto perceptionTrafficlightVector = data->PerceptionTrafficlightVector;
                    auto Camera_Status = data->Camera_Status;
                    auto TL_CAM_Status = data->TL_CAM_Status;

                    if(Camera_Status) {
                        adcm::Log::Verbose() << "Camera_Status : On";

                    } else {
                        adcm::Log::Verbose() << "Camera_Status : Off";
                    }

                    if(TL_CAM_Status) {
                        adcm::Log::Verbose() << "TL_CAM_Status : On";

                    } else {
                        adcm::Log::Verbose() << "TL_CAM_Status : Off";
                    }

                    if(!perceptionTrafficlightVector.empty()) {
                        adcm::Log::Verbose() << "Traffic Light: ";

                        for(typename std::vector<adcm::Perception_trafficlight>::iterator itr = perceptionTrafficlightVector.begin(); itr != perceptionTrafficlightVector.end(); ++itr) {
                            adcm::Log::Verbose() << "Id : " << itr->Id;
                            auto color = static_cast<std::uint16_t>(itr->Color);

                            if(color == 80) {
                                adcm::Log::Verbose() << "Color : GREEN";

                            } else if(color == 81) {
                                adcm::Log::Verbose() << "Color : GREEN_LEFT_GREEN_ARROW";

                            } else if(color == 82) {
                                adcm::Log::Verbose() << "Color : RED";

                            } else if(color == 83) {
                                adcm::Log::Verbose() << "Color : RED_LEFT_GREEN_ARROW";

                            } else if(color == 85) {
                                adcm::Log::Verbose() << "Color : YELLOW";

                            } else if(color == 90) {
                                adcm::Log::Verbose() << "Color : OFF";

                            } else {
                                adcm::Log::Verbose() << "Color : UNKNOWN";
                            }

                            adcm::Log::Verbose() << "Position(Xmin) : " << itr->Position.Xmin;
                            adcm::Log::Verbose() << "Position(Ymin) : " << itr->Position.Ymin;
                            adcm::Log::Verbose() << "Position(Xmax) : " << itr->Position.Xmax;
                            adcm::Log::Verbose() << "Position(Ymax) : " << itr->Position.Ymax;
                            adcm::Log::Verbose() << "CameraID : " << itr->CameraID;
                        }

                    } else {
                        adcm::Log::Verbose() << "Traffic Light empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveV2XData()
{
    adcm::Log::Info() << "Localization ThreadReceiveV2XData Start!!";
    adcm::V2XData_Subscriber v2xData_subscriber;
    v2xData_subscriber.init("Localization/Localization/RPort_v2x_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Localization] ThreadReceiveV2XData loop";
        bool v2xData_rxEvent = v2xData_subscriber.waitEvent(120);

        if(v2xData_rxEvent) {
            while(!v2xData_subscriber.isEventQueueEmpty()) {
                auto data = v2xData_subscriber.getEvent();
                gEventReceived_Count_v2x_data++;
                adcm::Log::Verbose() << "[EVENT] Localization V2X Data received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Localization v2x_data Event Callback";
                    auto intersections = data->intersections;
                    adcm::Log::Verbose() << "id : " << intersections.id;
                    adcm::Log::Verbose() << "revision : " << intersections.revision;
                    adcm::Log::Verbose() << "status : " << intersections.status;
                    adcm::Log::Verbose() << "moy : " << intersections.moy;
                    adcm::Log::Verbose() << "timeStamp : " << intersections.timeStamp;
                    auto MovementState = intersections.states.MovementState;

                    if(!MovementState.empty()) {
                        adcm::Log::Verbose() << "MovementState: ";

                        for(typename std::vector<adcm::movementState>::iterator itr = MovementState.begin(); itr != MovementState.end(); ++itr) {
                            adcm::Log::Verbose() << "movementName : " << itr->movementName;
                            adcm::Log::Verbose() << "signalGroup : " << itr->signalGroup;
                            adcm::Log::Verbose() << "eventState : " << itr->eventState;
                            adcm::Log::Verbose() << "timing_minEndTime : " << itr->timing_minEndTime;
                            adcm::Log::Verbose() << "timing_maxEndTime : " << itr->timing_maxEndTime;
                        }

                    } else {
                        adcm::Log::Verbose() << "MovementState empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadSendVehicleLocation()
{
    adcm::Log::Info() << "Localization ThreadSendVehicleLocation Start!!";
    adcm::VehicleLocation_Provider vehicleLocation_provider;
    vehicleLocation_provider.init("Localization/Localization/PPort_vehicle_location");
    adcm::Log::Info() << "nmea Thread start";

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // write data
        adcm::vehicle_location_Objects POSITION_INFO;
        POSITION_INFO.MeasurementTime = m_ud_100_1000(m_rand_eng);
        POSITION_INFO.CurrentLane_id = m_ud_1000_10000(m_rand_eng);
        POSITION_INFO.PositionX = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.PositionY = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.PositionZ = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.GNSS.Heading = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.GNSS.Speed = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.IMU.Roll = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.IMU.Pitch = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.IMU.Yaw = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.IMU.Linear_acceleration = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.IMU.Angular_velocity = m_ud_10000_10000(m_rand_eng);
        POSITION_INFO.GridID = m_ud_0_10000(m_rand_eng);
        POSITION_INFO.CellID = m_ud_0_10000(m_rand_eng);
        POSITION_INFO.Accuracy = m_ud_0_10000(m_rand_eng);
        POSITION_INFO.Fix_type = m_ud_0_10(m_rand_eng);
        POSITION_INFO.GPS_Status = m_ud_0_10(m_rand_eng) % 2 == 0 ? true : false;
        vehicleLocation_provider.send(POSITION_INFO);
        adcm::Log::Verbose() << "[Localization] send vehicleLocation Event ";
    }
}

void ThreadSendMatchingInfo()
{
    adcm::Log::Info() << "Localization ThreadSendMatchingInfo Start!!";
    adcm::MatchingInfo_Provider matchingInfo_provider;
    matchingInfo_provider.init("Localization/Localization/PPort_matching_info");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // allocate sample
        adcm::matching_info_Objects MATCHING_INFO;
        // write data
        MATCHING_INFO.dStartX = m_ud_10000_10000(m_rand_eng);
        MATCHING_INFO.dStartY = m_ud_10000_10000(m_rand_eng);
        MATCHING_INFO.dEndX = m_ud_10000_10000(m_rand_eng);
        MATCHING_INFO.dEndY = m_ud_10000_10000(m_rand_eng);
        MATCHING_INFO.nS_ClosestRoadID = m_ud_0_10000(m_rand_eng);
        MATCHING_INFO.nE_ClosestRoadID = m_ud_0_10000(m_rand_eng);
        MATCHING_INFO.nS_ClosestLaneID = m_ud_0_10000(m_rand_eng);
        MATCHING_INFO.nE_ClosestLaneID = m_ud_0_10000(m_rand_eng);
        MATCHING_INFO.roadIdVector.clear();
        MATCHING_INFO.roadIdVector.push_back(m_ud_0_10000(m_rand_eng));
        MATCHING_INFO.roadIdVector.push_back(m_ud_0_10000(m_rand_eng));
        MATCHING_INFO.roadIdVector.push_back(m_ud_0_10000(m_rand_eng));
        matchingInfo_provider.send(MATCHING_INFO);
        adcm::Log::Verbose() << "[Localization] send matchingInfo Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(25);
        setFault(26);
        setFault(27);
        setFault(28);
        setFault(29);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(25);
        clearFault(26);
        clearFault(27);
        clearFault(28);
        clearFault(29);
        sendFault();
        INFO("Clear fault");
    }
}

void ThreadMethodCallTest()
{
    adcm::LocalMap_Subscriber localMap_subscriber;
    localMap_subscriber.init("Localization/Localization/RPort_local_map");
    std::random_device m_rd;
    std::default_random_engine m_rand_eng(m_rd());
    std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
    String serviceFlag;
    std::uint32_t gridID = 0;
    std::uint32_t cellID = 0;
    std::uint64_t deadLine = 0;

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        serviceFlag = localMap_subscriber.fieldGetter_ServiceFlag();

        if(serviceFlag == "On") {
            adcm::Log::Verbose() << "[Localization][localMap_subscriber] GetMapData Method Call Test";
            gridID = m_ud_0_10000(m_rand_eng);
            cellID = m_ud_0_10000(m_rand_eng);
            deadLine = 1000;    //millisecond
            auto mapdataPtr = localMap_subscriber.MapData(gridID, cellID, deadLine);

            if(mapdataPtr != NULL) {
                auto GridID = mapdataPtr->GridID;
                auto CellID = mapdataPtr->CellID;
                adcm::Log::Verbose() << "mapdata.GridID : " << GridID;
                adcm::Log::Verbose() << "mapdata.CellID : " << CellID;

            } else {
                adcm::Log::Info() << "mapdata is NULL ...";
            }

        } else {
            adcm::Log::Info() << "local_map service is not provided...";
        }
    }
}

void ThreadMonitor()
{
    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(gMainthread_Loopcount == 0) {
            adcm::Log::Error() << "Main thread Timeout!!!";

        } else {
            gMainthread_Loopcount = 0;

            if(gEventReceived_Count_can_data != 0) {
                adcm::Log::Info() << "can_data Received count = " << gEventReceived_Count_can_data;
                gEventReceived_Count_can_data = 0;

            } else {
                adcm::Log::Info() << "can_data event timeout!!!";
            }

            if(gEventReceived_Count_can_data_etc != 0) {
                adcm::Log::Info() << "can_data_etc Received count = " << gEventReceived_Count_can_data_etc;
                gEventReceived_Count_can_data_etc = 0;

            } else {
                adcm::Log::Info() << "can_data_etc event timeout!!!";
            }

            if(gEventReceived_Count_lane_detection != 0) {
                adcm::Log::Info() << "lane_detection Received count = " << gEventReceived_Count_lane_detection;
                gEventReceived_Count_lane_detection = 0;

            } else {
                adcm::Log::Info() << "lane_detection event timeout!!!";
            }

            if(gEventReceived_Count_perception_obstacles != 0) {
                adcm::Log::Info() << "perception_obstacles Received count = " << gEventReceived_Count_perception_obstacles;
                gEventReceived_Count_perception_obstacles = 0;

            } else {
                adcm::Log::Info() << "perception_obstacles event timeout!!!";
            }

            if(gEventReceived_Count_point_map != 0) {
                adcm::Log::Info() << "point_map Received count = " << gEventReceived_Count_point_map;
                gEventReceived_Count_point_map = 0;

            } else {
                adcm::Log::Info() << "point_map event timeout!!!";
            }

            if(gEventReceived_Count_routing_result != 0) {
                adcm::Log::Info() << "routing_result Received count = " << gEventReceived_Count_routing_result;
                gEventReceived_Count_routing_result = 0;

            } else {
                adcm::Log::Info() << "routing_result event timeout!!!";
            }
            
            if(gEventReceived_Count_routing_search != 0) {
                adcm::Log::Info() << "routing_search Received count = " << gEventReceived_Count_routing_search;
                gEventReceived_Count_routing_search = 0;

            } else {
                adcm::Log::Info() << "routing_search event timeout!!!";
            }

            if(gEventReceived_Count_traffic_light != 0) {
                adcm::Log::Info() << "traffic_light Received count = " << gEventReceived_Count_traffic_light;
                gEventReceived_Count_traffic_light = 0;

            } else {
                adcm::Log::Info() << "traffic_light event timeout!!!";
            }

            if(gEventReceived_Count_v2x_data != 0) {
                adcm::Log::Info() << "v2x_data Received count = " << gEventReceived_Count_v2x_data;
                gEventReceived_Count_v2x_data = 0;

            } else {
                adcm::Log::Info() << "v2x_data event timeout!!!";
            }
        }
    }
}

}  // namespace


// ========================================= Process RTT =========================================
static std::shared_ptr<vsomeip::application> local_app;
static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
static const vsomeip::instance_t INST = 0x0004;  // Instance ID
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
        adcm::Log::Info() << "[Pro-Localization] recv=" << s.recv_total
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
        adcm::Log::Verbose() << "[Pro-Localization] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    local_app->send(resp);
}

void vsomeip_server_thread() {
    local_app = vsomeip::runtime::get()->create_application("local_echo");
    local_app->init();
    local_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    local_app->offer_service(SVC, INST);
    local_app->start();
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

    if(!Localization::RegisterSigTermHandler()) {
        adcm::Log::Error() << "Unable to register signal handler";
    }
#ifndef R19_11_1
    adcm::Log::Info() << "Localization: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    adcm::Log::Info() << "Localization: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    adcm::Log::Info() << "Ok, let's produce some Localization data...";

    {
        Localization::collectFault_provider = std::make_shared<adcm::CollectFaultLocalization_Provider>();
        Localization::collectFault_provider->init("Localization/Localization/PPort_collect_fault_localization");
        Localization::sendFault();
    }

    thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(Localization::ThreadReceiveCanData));
    thread_list.push_back(std::thread(Localization::ThreadReceiveCanDataEtc));
    thread_list.push_back(std::thread(Localization::ThreadReceiveLaneDetection));
    thread_list.push_back(std::thread(Localization::ThreadReceivePerceptionObstacles));
    thread_list.push_back(std::thread(Localization::ThreadReceivePointMap));
    thread_list.push_back(std::thread(Localization::ThreadReceiveRoutingResult));
    thread_list.push_back(std::thread(Localization::ThreadReceiveRoutingSearch));
    thread_list.push_back(std::thread(Localization::ThreadReceiveTrafficLight));
    thread_list.push_back(std::thread(Localization::ThreadReceiveV2XData));
    thread_list.push_back(std::thread(Localization::ThreadSendVehicleLocation));
    thread_list.push_back(std::thread(Localization::ThreadSendMatchingInfo));
    // thread_list.push_back(std::thread(Localization::ThreadMethodCallTest));
    thread_list.push_back(std::thread(Localization::ThreadSendCollectFault));
    thread_list.push_back(std::thread(Localization::ThreadMonitor));

    adcm::Log::Info() << "Thread join";
    for(int i = 0; i < static_cast<int>(thread_list.size()); i++) {
        thread_list[i].join();
    }

    adcm::Log::Info() << "done.";
    if(!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
