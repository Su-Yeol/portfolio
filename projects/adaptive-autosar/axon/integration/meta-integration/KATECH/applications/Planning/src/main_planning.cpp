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
// The different components radar, video, tester, ... are used in one
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

#include "collect_fault_planning_provider.h"
#include "driving_trajectory_provider.h"
#include "planning_hmi_data_provider.h"
#include "smart_cruise_control_provider.h"
#include "can_data_subscriber.h"
#include "can_data_etc_subscriber.h"
#include "local_map_subscriber.h"
#include "perception_obstacles_subscriber.h"
#include "report_fault_subscriber.h"
#include "routing_result_subscriber.h"
#include "sensor_radar_subscriber.h"
#include "traffic_light_subscriber.h"
#include "v2x_data_subscriber.h"
#include "vehicle_location_subscriber.h"

// ======= Process RTT =======
#include <vsomeip/vsomeip.hpp>
#include <memory>
#include <iostream>
// ======= Process RTT =======

std::random_device m_rd;
std::default_random_engine m_rand_eng(m_rd());
std::uniform_real_distribution<double> m_ud_10000_10000(-10000, 10000);
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);

namespace Planning
{

std::shared_ptr<katech::CollectFaultPlanning_Provider> collectFault_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gEventReceived_Count_can_data{0};
std::atomic_uint gEventReceived_Count_can_data_etc{0};
std::atomic_uint gEventReceived_Count_perception_obstacles{0};
std::atomic_uint gReceivedEvent_count_report_fault{0};
std::atomic_uint gEventReceived_Count_routing_result{0};
std::atomic_uint gEventReceived_Count_sensor_radar{0};
std::atomic_uint gEventReceived_Count_traffic_light{0};
std::atomic_uint gEventReceived_Count_v2x_data{0};
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
    static katech::collect_fault_planning_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Planning.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Planning.push_back(i);
        }
    }

    collectFault_provider->send(gCollectFault);
    katech::Log::Verbose() << "[Localization] send collectFault Event ";
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
    katech::Log::Info() << "Planning ThreadReceiveCanData Start!!";
    katech::CanData_Subscriber canData_subscriber;
    canData_subscriber.init("Planning/Planning/RPort_can_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveCanData loop";
        bool canData_rxEvent = canData_subscriber.waitEvent(60); // wait event

        if(canData_rxEvent) {
            katech::Log::Verbose() << "canData_rxEvent";

            while(!canData_subscriber.isEventQueueEmpty()) {
                auto data = canData_subscriber.getEvent();
                gEventReceived_Count_can_data++; 
                katech::Log::Verbose() << "[EVENT] Planning Can Data received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning can_data Event Callback";
                    
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
                    katech::Log::Verbose() << "ControlSteering : " << data->ControlSteering;
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
                    katech::Log::Verbose() << "RPM : " << data->RPM;
                    katech::Log::Verbose() << "Accel_Pedal : " << data->Accel_Pedal;
                    katech::Log::Verbose() << "Brake_Pedal : " << data->Brake_Pedal;

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
        } else{
            katech::Log::Verbose() << "Planning can_data timeout...";
        }
    }
}

void ThreadReceiveCanDataEtc()
{
    katech::Log::Info() << "Planning ThreadReceiveCanDataEtc Start!!";
    katech::CanDataEtc_Subscriber canDataEtc_subscriber;
    canDataEtc_subscriber.init("Planning/Planning/RPort_can_data_etc");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveCanDataEtc loop";
        bool canDataEtc_rxEvent = canDataEtc_subscriber.waitEvent(120); // wait event

        if(canDataEtc_rxEvent) {
            katech::Log::Verbose() << "canDataEtc_rxEvent";

            while(!canDataEtc_subscriber.isEventQueueEmpty()) {
                auto data = canDataEtc_subscriber.getEvent();
                gEventReceived_Count_can_data_etc++; 
                katech::Log::Verbose() << "[EVENT] Planning Can Data Etc received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning can_data_etc Event Callback";
                    
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
        }
    }
}

void ThreadReceivePerceptionObstacles()
{
    katech::Log::Info() << "Planning ThreadReceivePerceptionObstacles Start!!";
    katech::PerceptionObstacles_Subscriber perceptionObstacles_subscriber;
    perceptionObstacles_subscriber.init("Planning/Planning/RPort_perception_obstacles");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceivePerceptionObstacles loop";
        bool perceptionObstacles_rxEvent = perceptionObstacles_subscriber.waitEvent(120);

        if(perceptionObstacles_rxEvent) {
            while(!perceptionObstacles_subscriber.isEventQueueEmpty()) {
                auto data = perceptionObstacles_subscriber.getEvent();
                gEventReceived_Count_perception_obstacles++;
                katech::Log::Verbose() << "[EVENT] Planning Perception Obstacles received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning perception_obstacles Event Callback";
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
        } else{
            katech::Log::Verbose() << "Planning perception_obstacles timeout...";
        }
    }
}

void ThreadReceiveReportFault()
{
    katech::Log::Info() << "Planning ThreadReceiveReportFault Start!!";
    katech::ReportFault_Subscriber reportFault_subscriber;
    reportFault_subscriber.init("Planning/Planning/RPort_report_fault");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveReportFault loop";
        bool reportFault_rxEvent = reportFault_subscriber.waitEvent(120); // wait event

        if(reportFault_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Planning Report Fault received";

            while(!reportFault_subscriber.isEventQueueEmpty()) {
                auto data = reportFault_subscriber.getEvent();
                gReceivedEvent_count_report_fault++;
                katech::Log::Verbose() << "[Enter] Planning report_fault Event Callback";
                auto AP = data->AP;
                auto FaultList = data->FaultList;
                katech::Log::Verbose() << "AP num : " << AP;

                if(!FaultList.empty()) {
                    katech::Log::Verbose() << "=== Fault Values ===";

                    for(auto itr = FaultList.begin(); itr != FaultList.end(); ++itr) {
                        katech::Log::Verbose() << *itr;
                    }

                } else {
                    katech::Log::Verbose() << "FaultList Vector empty!!! ";
                }
            }
        }
    }
}

void ThreadReceiveRoutingResult()
{
    katech::Log::Info() << "Planning ThreadReceiveRoutingResult Start!!";
    katech::RoutingResult_Subscriber routingResult_subscriber;
    routingResult_subscriber.init("Planning/Planning/RPort_routing_result");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveRoutingResult loop";
        bool routingResult_rxEvent = routingResult_subscriber.waitEvent(120);

        if(routingResult_rxEvent) {
            while(!routingResult_subscriber.isEventQueueEmpty()) {
                auto data = routingResult_subscriber.getEvent();
                gEventReceived_Count_routing_result++;
                katech::Log::Verbose() << "[EVENT] Planning Routing Result received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning routing_result Event Callback";
                    
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

void ThreadReceiveSensorRadar()
{
    katech::Log::Info() << "Planning ThreadReceiveSensorRadar Start!!";
    katech::SensorRadar_Subscriber sensorRadar_subscriber;
    sensorRadar_subscriber.init("Planning/Planning/RPort_sensor_radar");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveSensorRadar loop";
        bool sensorRadar_rxEvent = sensorRadar_subscriber.waitEvent(120);

        if(sensorRadar_rxEvent) {
            while(!sensorRadar_subscriber.isEventQueueEmpty()) {
                auto data = sensorRadar_subscriber.getEvent();
                gEventReceived_Count_sensor_radar++;
                katech::Log::Verbose() << "[EVENT] Planning Sensor Radar received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning sensor_radar Event Callback";
                    
                    auto Front = data->Front;
                    auto Side = data->Side;

                    katech::Log::Verbose() << "Front.length : " << Front.length;
                    katech::Log::Verbose() << "Front.RelativeVelocity : " << Front.RelativeVelocity;

                    if(!Side.empty()) {
                        katech::Log::Verbose() << "=== Side ===";
                        for(auto itr = Side.begin(); itr != Side.end(); ++itr) {
                            katech::Log::Verbose() << "Side.X : " << itr->X;
                            katech::Log::Verbose() << "Side.Y : " << itr->Y;
                        }
                    } else {
                        katech::Log::Info() << "Side Vector empty!!! ";
                    }
                }
            }
        }
    }
}

 void ThreadReceiveTrafficLight()
{
    katech::Log::Info() << "Planning ThreadReceiveTrafficLight Start!!";
    katech::TrafficLight_Subscriber trafficLight_subscriber;
    trafficLight_subscriber.init("Planning/Planning/RPort_traffic_light");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveTrafficLight loop";
        bool trafficLight_rxEvent = trafficLight_subscriber.waitEvent(120);

        if(trafficLight_rxEvent) {
            while(!trafficLight_subscriber.isEventQueueEmpty()) {
                auto data = trafficLight_subscriber.getEvent();
                gEventReceived_Count_traffic_light++;
                katech::Log::Verbose() << "[EVENT] Planning Traffic Light received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning traffic_light Event Callback";
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
        } else{
            katech::Log::Verbose() << "Planning traffic_light timeout...";
        }
    }
}

void ThreadReceiveV2XData()
{
    katech::Log::Info() << "Planning ThreadReceiveV2XData Start!!";
    katech::V2XData_Subscriber v2xData_subscriber;
    v2xData_subscriber.init("Planning/Planning/RPort_v2x_data");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveV2XData loop";
        bool v2xData_rxEvent = v2xData_subscriber.waitEvent(120);

        if(v2xData_rxEvent) {
            while(!v2xData_subscriber.isEventQueueEmpty()) {
                auto data = v2xData_subscriber.getEvent();
                gEventReceived_Count_v2x_data++;
                katech::Log::Verbose() << "[EVENT] Planning V2X Data received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning v2x_data Event Callback";

                    auto intersections = data->intersections;

                    katech::Log::Verbose() << "id : " << intersections.id;
                    katech::Log::Verbose() << "revision : " << intersections.revision;
                    katech::Log::Verbose() << "status : " << intersections.status;
                    katech::Log::Verbose() << "moy : " << intersections.moy;
                    katech::Log::Verbose() << "timeStamp : " << intersections.timeStamp;
                    
                    auto MovementState = intersections.states.MovementState;

                    if(!MovementState.empty()) {
                        katech::Log::Verbose() << "MovementState: ";

                        for(typename std::vector<katech::movementState>::iterator itr = MovementState.begin(); itr != MovementState.end(); ++itr) {
                            
                            katech::Log::Verbose() << "movementName : " << itr->movementName;
                            katech::Log::Verbose() << "signalGroup : " << itr->signalGroup;
                            katech::Log::Verbose() << "eventState : " << itr->eventState;
                            katech::Log::Verbose() << "timing_minEndTime : " << itr->timing_minEndTime;
                            katech::Log::Verbose() << "timing_maxEndTime : " << itr->timing_maxEndTime;
                        }

                    } else {
                        katech::Log::Verbose() << "MovementState empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveVehicleLocation()
{
    katech::Log::Info() << "Planning ThreadReceiveVehicleLocation Start!!";
    katech::VehicleLocation_Subscriber vehicleLocation_subscriber;
    vehicleLocation_subscriber.init("Planning/Planning/RPort_vehicle_location");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Planning] ThreadReceiveVehicleLocation loop";
        bool vehicleLocation_rxEvent = vehicleLocation_subscriber.waitEvent(120);

        if(vehicleLocation_rxEvent) {
            while(!vehicleLocation_subscriber.isEventQueueEmpty()) {
                auto data = vehicleLocation_subscriber.getEvent();
                gEventReceived_Count_vehicle_location++;
                katech::Log::Verbose() << "[EVENT] Planning Vehicle Location received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Planning vehicle_location Event Callback";
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
            katech::Log::Verbose() << "Planning vehicle_location timeout...";
        }
    }
}

void ThreadSendDrivingTrajectory()
{
    katech::Log::Info() << "Planning ThreadSendDrivingTrajectory Start!!";
    katech::DrivingTrajectory_Provider drivingTrajectory_provider;
    drivingTrajectory_provider.init("Planning/Planning/PPort_driving_trajectory");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        katech::driving_trajectory_Objects drivingTrajectory;
        // write data
        katech::TrajectoryPoint trajectoryPoint;
        trajectoryPoint.Vector3D.x = m_ud_10000_10000(m_rand_eng);
        trajectoryPoint.Vector3D.y = m_ud_10000_10000(m_rand_eng);
        trajectoryPoint.Vector3D.yaw = m_ud_10000_10000(m_rand_eng);        
        drivingTrajectory.TrajectoryPointVector.clear();
        drivingTrajectory.TrajectoryPointVector.push_back(trajectoryPoint);

        drivingTrajectory.Position.x = m_ud_10000_10000(m_rand_eng);
        drivingTrajectory.Position.y = m_ud_10000_10000(m_rand_eng);
        drivingTrajectory.Position.yaw = m_ud_10000_10000(m_rand_eng);

        drivingTrajectory.Target_speed = m_ud_10000_10000(m_rand_eng);
        drivingTrajectory.Drive_Mode = m_ud_0_10000(m_rand_eng)%2 == 0 ? true : false;
        drivingTrajectory.Emergency_acceleration = m_ud_10000_10000(m_rand_eng);
        drivingTrajectory.Turn_Signal = m_ud_0_10000(m_rand_eng)%3;

        drivingTrajectory_provider.send(drivingTrajectory);
        katech::Log::Verbose() << "[Planning] send drivingTrajectory Event ";
    }
}

void ThreadSendPlanningHmiData()
{
    katech::Log::Info() << "Planning ThreadSendPlanningHmiData Start!!";
    katech::PlanningHmiData_Provider planningHmiData_provider;
    planningHmiData_provider.init("Planning/Planning/PPort_planning_hmi_data");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        katech::planning_hmi_data_Objects planningHmiData;
        // write data
        planningHmiData.No_Autonomous_Driving = false;
        planningHmiData.Noti_Autonomous_Driving_Mode = m_ud_0_10000(m_rand_eng)%3;
        planningHmiData.AutoDrivingMode = true;

        planningHmiData_provider.send(planningHmiData);
        katech::Log::Verbose() << "[Planning] send planningHmiData Event ";
    }
}

void ThreadSendSmartCruiseControl()
{
    katech::Log::Info() << "Planning ThreadSendSmartCruiseControl Start!!";
    katech::SmartCruiseControl_Provider smartCruiseControl_provider;
    smartCruiseControl_provider.init("Planning/Planning/PPort_smart_cruise_control");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        katech::smart_cruise_control_Objects sccData;
        // write data
        sccData.TargetVelocity = m_ud_10000_10000(m_rand_eng);
        sccData.ObjectRelSpeed = m_ud_10000_10000(m_rand_eng);
        sccData.ObjectDistance = m_ud_10000_10000(m_rand_eng);

        smartCruiseControl_provider.send(sccData);
        katech::Log::Verbose() << "[Planning] send smartCruiseControl Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        setFault(46);
        setFault(47);
        setFault(48);
        setFault(49);
        setFault(50);
        setFault(51);
        setFault(52);
        sendFault();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        clearFault(46);
        clearFault(47);
        clearFault(48);
        clearFault(49);
        clearFault(50);
        clearFault(51);
        clearFault(52);
        sendFault();
    }
}

void ThreadMethodCallTest()
{
    katech::LocalMap_Subscriber localMap_subscriber;
    localMap_subscriber.init("Planning/Planning/RPort_local_map");

    std::random_device m_rd;
    std::default_random_engine m_rand_eng(m_rd());
    std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);

    String serviceFlag;
    std::uint32_t gridID = 0;
    std::uint32_t cellID = 0;
    std::uint64_t deadLine = 0;

    while(continueExecution){
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        serviceFlag = localMap_subscriber.fieldGetter_ServiceFlag();

        if(serviceFlag == "On")
        {
            katech::Log::Verbose() << "[Planning][localMap_subscriber] GetMapData Method Call Test";
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
            katech::Log::Error() << "Main thread Timeout!!!";

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

            if(gEventReceived_Count_perception_obstacles != 0) {
                katech::Log::Info() << "perception_obstacles Received count = " << gEventReceived_Count_perception_obstacles;
                gEventReceived_Count_perception_obstacles = 0;

            } else {
                katech::Log::Info() << "perception_obstacles event timeout!!!";
            }

            if(gReceivedEvent_count_report_fault != 0) {
                katech::Log::Info() << "report_fault Received count = " << gReceivedEvent_count_report_fault;
                gReceivedEvent_count_report_fault = 0;

            } else {
                katech::Log::Info() << "report_fault event timeout!!!";
            }

            if(gEventReceived_Count_routing_result != 0) {
                katech::Log::Info() << "routing_result Received count = " << gEventReceived_Count_routing_result;
                gEventReceived_Count_routing_result = 0;

            } else {
                katech::Log::Info() << "routing_result event timeout!!!";
            }

            if(gEventReceived_Count_sensor_radar != 0) {
                katech::Log::Info() << "sensor_radar Received count = " << gEventReceived_Count_sensor_radar;
                gEventReceived_Count_sensor_radar = 0;

            } else {
                katech::Log::Info() << "sensor_radar event timeout!!!";
            }

            if(gEventReceived_Count_traffic_light != 0) {
                katech::Log::Info() << "traffic_light Received count = " << gEventReceived_Count_traffic_light;
                gEventReceived_Count_traffic_light = 0;

            } else {
                katech::Log::Info() << "traffic_light event timeout!!!";
            }

            if(gEventReceived_Count_v2x_data != 0) {
                katech::Log::Info() << "v2x_data Received count = " << gEventReceived_Count_v2x_data;
                gEventReceived_Count_v2x_data = 0;

            } else {
                katech::Log::Info() << "v2x_data event timeout!!!";
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

} // namespace


// ========================================= Process RTT =========================================
// static std::shared_ptr<vsomeip::application> plan_app;
// static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
// static const vsomeip::instance_t INST = 0x0007;  // Instance ID
// static const vsomeip::method_t ECHO   = 0x0001;  // Echo Method ID

// void on_echo_request(const std::shared_ptr<vsomeip::message> &req) {
//     auto resp = vsomeip::runtime::get()->create_response(req);
//     resp->set_payload(req->get_payload());

//     auto pl = req->get_payload();
//     if (pl && pl->get_length() > 0) {
//         katech::Log::Info() << "[Pro-Planning] ECHO Request received. Payload size = " 
//                           << pl->get_length() << " bytes";
//     } else {
//         katech::Log::Info() << "[Pro-Planning] ECHO Request received (empty payload)";
//     }

//     plan_app->send(resp);
// }

// void vsomeip_server_thread() {
//     plan_app = vsomeip::runtime::get()->create_application("plan_echo");
//     plan_app->init();
//     plan_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
//     plan_app->offer_service(SVC, INST);
//     plan_app->start();
// }
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

    if(!Planning::RegisterSigTermHandler()) {
        katech::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    katech::Log::Info() << "Planning: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    katech::Log::Info() << "Planning: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    katech::Log::Info() << "Ok, let's produce some Planning data...";

    {
        Planning::collectFault_provider = std::make_shared<katech::CollectFaultPlanning_Provider>();
        Planning::collectFault_provider->init("Planning/Planning/PPort_collect_fault_planning");
        Planning::sendFault();
    }

    // thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(Planning::ThreadReceiveCanData));
    thread_list.push_back(std::thread(Planning::ThreadReceiveCanDataEtc));
    thread_list.push_back(std::thread(Planning::ThreadReceivePerceptionObstacles));
    thread_list.push_back(std::thread(Planning::ThreadReceiveReportFault));
    thread_list.push_back(std::thread(Planning::ThreadReceiveRoutingResult));
    thread_list.push_back(std::thread(Planning::ThreadReceiveSensorRadar));
    thread_list.push_back(std::thread(Planning::ThreadReceiveTrafficLight));
    thread_list.push_back(std::thread(Planning::ThreadReceiveV2XData));
    thread_list.push_back(std::thread(Planning::ThreadReceiveVehicleLocation));
    thread_list.push_back(std::thread(Planning::ThreadSendDrivingTrajectory));
    thread_list.push_back(std::thread(Planning::ThreadSendPlanningHmiData));
    thread_list.push_back(std::thread(Planning::ThreadSendSmartCruiseControl));
    // thread_list.push_back(std::thread(Planning::ThreadMethodCallTest));
    thread_list.push_back(std::thread(Planning::ThreadSendCollectFault));
    thread_list.push_back(std::thread(Planning::ThreadMonitor));

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
