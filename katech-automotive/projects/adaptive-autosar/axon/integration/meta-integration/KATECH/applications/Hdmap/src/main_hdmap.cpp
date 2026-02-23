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

#include "collect_fault_hdmap_provider.h"
#include "point_map_provider.h"
#include "local_map_provider.h"
#include "routing_search_subscriber.h"
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
std::uniform_real_distribution<float> m_ud_100_1000(-100, 1000);
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::int8_t> m_ud_100_100(-100, 100);
std::uniform_int_distribution<std::int16_t> m_ud_10_10(-10, 10);
std::uniform_int_distribution<std::int32_t> m_ud_1000_1000(-1000, 1000);
std::uniform_int_distribution<std::int64_t> m_ud_1000_10000(-1000, 10000);

namespace HDMap
{

std::shared_ptr<katech::LocalMap_Provider> localmap_provider;
std::shared_ptr<katech::CollectFaultHdmap_Provider> collectFault_Provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gEventReceived_Count_routing_search{0};
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
    static katech::collect_fault_hdmap_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Hdmap.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Hdmap.push_back(i);
        }
    }

    collectFault_Provider->send(gCollectFault);
    katech::Log::Verbose() << "[HDMap] send collectFault Event ";
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


void setMapData(const std::uint64_t& grid_id, const std::uint64_t& cell_id)
{
    katech::Log::Verbose() << "HDMAP >> setMapData function callback";

    // katech::local_map::GetMapDataOutput output;
    
    auto output = localmap_provider->getPtrOutput();

    // allocate sample

    katech::VertexStruct vertexLane;
    vertexLane.x = m_ud_10000_10000(m_rand_eng);
    vertexLane.y = m_ud_10000_10000(m_rand_eng);
    vertexLane.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexLine;
    vertexLine.x = m_ud_10000_10000(m_rand_eng);
    vertexLine.y = m_ud_10000_10000(m_rand_eng);
    vertexLine.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexCrossWalk;
    vertexCrossWalk.x = m_ud_10000_10000(m_rand_eng);
    vertexCrossWalk.y = m_ud_10000_10000(m_rand_eng);
    vertexCrossWalk.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexTrafficLight;
    vertexTrafficLight.x = m_ud_10000_10000(m_rand_eng);
    vertexTrafficLight.y = m_ud_10000_10000(m_rand_eng);
    vertexTrafficLight.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexStopSign;
    vertexStopSign.x = m_ud_10000_10000(m_rand_eng);
    vertexStopSign.y = m_ud_10000_10000(m_rand_eng);
    vertexStopSign.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexYieldSign;
    vertexYieldSign.x = m_ud_10000_10000(m_rand_eng);
    vertexYieldSign.y = m_ud_10000_10000(m_rand_eng);
    vertexYieldSign.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexSpeedBump;
    vertexSpeedBump.x = m_ud_10000_10000(m_rand_eng);
    vertexSpeedBump.y = m_ud_10000_10000(m_rand_eng);
    vertexSpeedBump.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexStopLine;
    vertexStopLine.x = m_ud_10000_10000(m_rand_eng);
    vertexStopLine.y = m_ud_10000_10000(m_rand_eng);
    vertexStopLine.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexBorder;
    vertexBorder.x = m_ud_10000_10000(m_rand_eng);
    vertexBorder.y = m_ud_10000_10000(m_rand_eng);
    vertexBorder.z = m_ud_10000_10000(m_rand_eng);

    katech::VertexStruct vertexParkingLot;
    vertexParkingLot.x = m_ud_10000_10000(m_rand_eng);
    vertexParkingLot.y = m_ud_10000_10000(m_rand_eng);
    vertexParkingLot.z = m_ud_10000_10000(m_rand_eng);

    //ParkingLot
    katech::ParkingLotStruct parkingLot;
    parkingLot.ID = m_ud_1000_10000(m_rand_eng);
    parkingLot.Type = m_ud_10_10(m_rand_eng);
    parkingLot.Geometry.clear();
    parkingLot.Geometry.push_back(vertexParkingLot);

    //Border
    katech::BorderStruct border;
    border.ID = m_ud_1000_10000(m_rand_eng);  
    border.Type = m_ud_10_10(m_rand_eng);  
    border.IsCentral = m_ud_10_10(m_rand_eng);
    border.LowHigh = m_ud_10_10(m_rand_eng);
    border.Ref_FID = m_ud_1000_10000(m_rand_eng);
    border.Geometry.clear();
    border.Geometry.push_back(vertexBorder);

    //StopLine
    katech::StopLineStruct stopLine;
    stopLine.ID = m_ud_1000_10000(m_rand_eng);  
    stopLine.Type = m_ud_10_10(m_rand_eng);  
    stopLine.Kind = m_ud_10_10(m_rand_eng);
    stopLine.R_LinkID = m_ud_1000_10000(m_rand_eng);
    stopLine.L_LinkID = m_ud_1000_10000(m_rand_eng);
    stopLine.Geometry.clear();
    stopLine.Geometry.push_back(vertexStopLine);

    //LaneLink
    katech::LaneLinkStruct laneLink;
    laneLink.From = m_ud_100_100(m_rand_eng);  
    laneLink.To = m_ud_100_100(m_rand_eng);  

    //Connection
    katech::ConnectionStruct connection;
    connection.ID = m_ud_100_100(m_rand_eng);  
    connection.IncomingRoadID = m_ud_1000_10000(m_rand_eng);  
    connection.ConnectingRoadID = m_ud_1000_10000(m_rand_eng);
    connection.ContactPoint = m_ud_100_100(m_rand_eng);
    connection.Type = m_ud_100_100(m_rand_eng);
    connection.LaneLinks.clear();
    connection.LaneLinks.push_back(laneLink);

    //Junction
    katech::JunctionStruct junction;
    junction.ID = m_ud_1000_10000(m_rand_eng);
    junction.Type = m_ud_100_100(m_rand_eng);
    junction.Connections.clear();
    junction.Connections.push_back(connection);

    //SpeedBump
    katech::SpeedBumpStruct speedBump;
    speedBump.ID = m_ud_1000_10000(m_rand_eng);  
    speedBump.Type = m_ud_10_10(m_rand_eng);
    speedBump.LinkID = m_ud_1000_10000(m_rand_eng);  
    speedBump.Ref_Lane = m_ud_10_10(m_rand_eng);
    speedBump.Geometry.clear();
    speedBump.Geometry.push_back(vertexSpeedBump);

    //YieldSign
    katech::YieldSignStruct yieldSign;
    yieldSign.ID = m_ud_1000_10000(m_rand_eng);  
    yieldSign.LinkID = m_ud_1000_10000(m_rand_eng);  
    yieldSign.PostID = m_ud_1000_10000(m_rand_eng);
    yieldSign.Ref_Lane = m_ud_10_10(m_rand_eng);
    yieldSign.Geometry.clear();
    yieldSign.Geometry.push_back(vertexYieldSign);

    //StopSign
    katech::StopSignStruct stopSign;
    stopSign.ID = m_ud_1000_10000(m_rand_eng);  
    stopSign.LinkID = m_ud_1000_10000(m_rand_eng);  
    stopSign.PostID = m_ud_1000_10000(m_rand_eng);
    stopSign.Ref_Lane = m_ud_10_10(m_rand_eng);
    stopSign.Geometry.clear();
    stopSign.Geometry.push_back(vertexStopSign);

    //TrafficLight
    katech::TrafficLightStruct trafficLight;
    trafficLight.ID = m_ud_1000_10000(m_rand_eng);  //64
    trafficLight.Type = m_ud_10_10(m_rand_eng);     //16
    trafficLight.LinkID = m_ud_1000_10000(m_rand_eng);  
    trafficLight.PostID = m_ud_1000_10000(m_rand_eng);
    trafficLight.Ref_Lane = m_ud_10_10(m_rand_eng);
    trafficLight.Geometry.clear();
    trafficLight.Geometry.push_back(vertexTrafficLight);

    //CrossWalk
    katech::CrossWalkStruct crossWalk;
    crossWalk.ID = m_ud_1000_10000(m_rand_eng);
    crossWalk.Type = m_ud_100_100(m_rand_eng);  //8
    crossWalk.Kind = m_ud_10_10(m_rand_eng);
    crossWalk.LinkID = m_ud_1000_10000(m_rand_eng);
    crossWalk.Geometry.clear();
    crossWalk.Geometry.push_back(vertexCrossWalk);

    //Line
    katech::LineStruct line;
    line.ID = m_ud_1000_10000(m_rand_eng);  //64
    line.Type = m_ud_10_10(m_rand_eng);  //16
    line.Kind = m_ud_10_10(m_rand_eng);  //16
    line.R_LinkID = m_ud_1000_10000(m_rand_eng);  //64
    line.L_LinkID = m_ud_1000_10000(m_rand_eng);  //64
    line.Geometry.clear();
    line.Geometry.push_back(vertexLine);

    //Lane
    katech::LaneStruct lane;
    lane.ID = m_ud_1000_10000(m_rand_eng);
    lane.LaneIndex = m_ud_100_100(m_rand_eng);
    lane.LaneNo = m_ud_100_100(m_rand_eng);
    lane.RoadRank = m_ud_100_100(m_rand_eng);
    lane.RoadType = m_ud_100_100(m_rand_eng);
    lane.LinkType = m_ud_100_100(m_rand_eng);
    lane.MaxSpeed = m_ud_100_100(m_rand_eng);
    lane.SectionKind = m_ud_100_100(m_rand_eng);
    lane.RoadDirection = m_ud_100_100(m_rand_eng);
    lane.Direction = m_ud_100_100(m_rand_eng);
    lane.R_LinkID = m_ud_1000_10000(m_rand_eng);
    lane.L_LinkID = m_ud_1000_10000(m_rand_eng);
    lane.Length = m_ud_10000_10000(m_rand_eng);
    lane.Predecessor.clear();
    lane.Predecessor.push_back(m_ud_1000_10000(m_rand_eng));
    lane.Successor.clear();
    lane.Successor.push_back(m_ud_1000_10000(m_rand_eng));
    lane.Geometry.clear();
    lane.Geometry.push_back(vertexLane);

    //Road
    katech::RoadStruct road;
    road.ID = m_ud_1000_10000(m_rand_eng);
    road.JunctionID = m_ud_1000_10000(m_rand_eng);
    road.Predecessor.clear();
    road.Predecessor.push_back(m_ud_1000_10000(m_rand_eng));
    road.Successor.clear();
    road.Successor.push_back(m_ud_1000_10000(m_rand_eng));
    road.Length = m_ud_10000_10000(m_rand_eng);
    road.LaneCnt = m_ud_100_100(m_rand_eng);
    road.Lanes.clear();
    road.Lanes.push_back(lane);
    road.Lines.clear();
    road.Lines.push_back(line);
    road.CrossWalks.clear();
    road.CrossWalks.push_back(crossWalk);
    road.TrafficLights.clear();
    road.TrafficLights.push_back(trafficLight);
    road.StopSigns.clear();
    road.StopSigns.push_back(stopSign);
    road.YieldSigns.clear();
    road.YieldSigns.push_back(yieldSign);
    road.SpeedBumps.clear();
    road.SpeedBumps.push_back(speedBump);

    output->map_data.GridID = grid_id;
    output->map_data.CellID = cell_id;
    output->map_data.Roads.clear();
    output->map_data.Roads.push_back(road);
    output->map_data.Junctions.clear();
    output->map_data.Junctions.push_back(junction);
    output->map_data.StopLines.clear();
    output->map_data.StopLines.push_back(stopLine);
    output->map_data.Borders.clear();
    output->map_data.Borders.push_back(border);
    output->map_data.ParkingLots.clear();
    output->map_data.ParkingLots.push_back(parkingLot);
    output->map_data.CrossWalks.clear();
    output->map_data.CrossWalks.push_back(crossWalk);
}


void ThreadReceiveVehicleLocataion()
{
    katech::Log::Info() << "Hdmap ThreadReceiveVehicleLocataion Start!!";
    katech::VehicleLocation_Subscriber vehicleLocation_subscriber;
    vehicleLocation_subscriber.init("Hdmap/Hdmap/RPort_vehicle_location");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hdmap] ThreadReceiveVehicleLocataion loop";
        bool vehicleLocation_rxEvent = vehicleLocation_subscriber.waitEvent(120); // wait event

        if(vehicleLocation_rxEvent) {
            while(!vehicleLocation_subscriber.isEventQueueEmpty()) {
                katech::Log::Verbose() << "[EVENT] Hdmap Vehicle Location received";
                auto data = vehicleLocation_subscriber.getEvent();
                gEventReceived_Count_vehicle_location++;

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hdmap vehicle_location Event Callback";
                    
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
            katech::Log::Verbose() << "Hdmap vehicle_location timeout...";
        }
    }
}

void ThreadReceiveRoutingSearch()
{
    katech::Log::Info() << "Hdmap ThreadReceiveRoutingSearch Start!!";
    katech::RoutingSearch_Subscriber routingSearch_subscriber;
    katech::Log::Info() << "Hdmap .init()";
    routingSearch_subscriber.init("Hdmap/Hdmap/RPort_routing_search");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Hdmap] ThreadReceiveRoutingSearch loop";
        bool routingSearch_rxEvent = routingSearch_subscriber.waitEvent(120); // wait event

        if(routingSearch_rxEvent) {
            katech::Log::Verbose() << "routingSearch_rxEvent";

            while(!routingSearch_subscriber.isEventQueueEmpty()) {
                auto data = routingSearch_subscriber.getEvent();
                gEventReceived_Count_routing_search++;
                katech::Log::Verbose() << "[EVENT] Hdmap Routing Search received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Hdmap routing_search Event Callback";
                    auto preset = data->preset;
                    auto StartPoint = data->StartPoint;
                    auto EndPoint = data->EndPoint;
                    auto StopPoint = data->StopPoint;

                    katech::Log::Verbose() << "preset : " << preset;
                    if(preset == 0 || preset == 1) {
                        katech::Log::Verbose() << "StartPoint.X :  " << StartPoint.X;
                        katech::Log::Verbose() << "StartPoint.X :  " << StartPoint.Y;
                        katech::Log::Verbose() << "EndPoint.X :  " << EndPoint.X;
                        katech::Log::Verbose() << "EndPoint.Y :  " << EndPoint.Y;

                        if(preset == 1){
                            if(!StopPoint.empty()) {
                                katech::Log::Verbose() << "=== StopPoint ===";
                                for(auto itr = StopPoint.begin(); itr != StopPoint.end(); ++itr) {
                                    katech::Log::Verbose() << "StopPoint.X : " << itr->X;
                                    katech::Log::Verbose() << "StopPoint.Y : " << itr->Y;
                                }
                            } else {
                                katech::Log::Info() << "StopPoint Vector empty!!! ";
                            }
                        }
                    } else {
                        katech::Log::Verbose() << "preseted routing path!!! ";
                    }
                }
            }
        }
    }
}

void ThreadSendPointMap()
{
    katech::Log::Info() << "Hdmap ThreadSendPointMap Start!!";

    katech::PointMap_Provider pointMap_provider;
    pointMap_provider.init("Hdmap/Hdmap/PPort_point_map");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //point_map
        katech::point_map_Objects pointMap;

        katech::VertexStruct vertexLane;
        vertexLane.x = m_ud_10000_10000(m_rand_eng);
        vertexLane.y = m_ud_10000_10000(m_rand_eng);
        vertexLane.z = m_ud_10000_10000(m_rand_eng);

        katech::VertexStruct vertexLine;
        vertexLine.x = m_ud_10000_10000(m_rand_eng);
        vertexLine.y = m_ud_10000_10000(m_rand_eng);
        vertexLine.z = m_ud_10000_10000(m_rand_eng);

        //Line
        katech::LineStruct line;
        line.ID = m_ud_1000_10000(m_rand_eng);  //64
        line.Type = m_ud_10_10(m_rand_eng);  //16
        line.Kind = m_ud_10_10(m_rand_eng);  //16
        line.R_LinkID = m_ud_1000_10000(m_rand_eng);  //64
        line.L_LinkID = m_ud_1000_10000(m_rand_eng);  //64
        line.Geometry.clear();
        line.Geometry.push_back(vertexLine);

        //Lane
        katech::LaneStruct lane;
        lane.ID = m_ud_1000_10000(m_rand_eng);
        lane.LaneIndex = m_ud_100_100(m_rand_eng);
        lane.LaneNo = m_ud_100_100(m_rand_eng);
        lane.RoadRank = m_ud_100_100(m_rand_eng);
        lane.RoadType = m_ud_100_100(m_rand_eng);
        lane.LinkType = m_ud_100_100(m_rand_eng);
        lane.MaxSpeed = m_ud_100_100(m_rand_eng);
        lane.SectionKind = m_ud_100_100(m_rand_eng);
        lane.RoadDirection = m_ud_100_100(m_rand_eng);
        lane.Direction = m_ud_100_100(m_rand_eng);
        lane.R_LinkID = m_ud_1000_10000(m_rand_eng);
        lane.L_LinkID = m_ud_1000_10000(m_rand_eng);
        lane.Length = m_ud_10000_10000(m_rand_eng);
        lane.Predecessor.clear();
        lane.Predecessor.push_back(m_ud_1000_10000(m_rand_eng));
        lane.Successor.clear();
        lane.Successor.push_back(m_ud_1000_10000(m_rand_eng));
        lane.Geometry.clear();
        lane.Geometry.push_back(vertexLane);

        //MMRoadStruct
        katech::MMRoadStruct mmroad;
        mmroad.ID = m_ud_1000_10000(m_rand_eng);
        mmroad.Predecessor.clear();
        mmroad.Predecessor.push_back(m_ud_1000_10000(m_rand_eng));
        mmroad.Successor.clear();
        mmroad.Successor.push_back(m_ud_1000_10000(m_rand_eng));
        mmroad.Lanes.clear();
        mmroad.Lanes.push_back(lane);
        mmroad.Lines.clear();
        mmroad.Lines.push_back(line);

        //pointinfo
        katech::pointinfo pointInfo;
        pointInfo.ID = m_ud_1000_10000(m_rand_eng);
        pointInfo.Roads.clear(); 
        pointInfo.Roads.push_back(mmroad);

        //PointMap
        pointMap.PointMap.clear();
        pointMap.PointMap.push_back(pointInfo);

        pointMap_provider.send(pointMap);
        katech::Log::Verbose() << "[Hdmap] send pointMap Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(8);
        setFault(9);
        setFault(10);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(8);
        clearFault(9);
        clearFault(10);
        sendFault();
        INFO("Clear fault");
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

            if(gEventReceived_Count_routing_search != 0) {
                katech::Log::Info() << "routing_search Received count = " << gEventReceived_Count_routing_search;
                gEventReceived_Count_routing_search = 0;

            } else {
                katech::Log::Info() << "routing_search event timeout!!!";
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
static std::shared_ptr<vsomeip::application> hdmap_app;
static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
static const vsomeip::instance_t INST = 0x0002;  // Instance ID
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
        katech::Log::Info() << "[Pro-Hdmap] recv=" << s.recv_total
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
        katech::Log::Verbose() << "[Pro-Hdmap] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    hdmap_app->send(resp);
}

void vsomeip_server_thread() {
    hdmap_app = vsomeip::runtime::get()->create_application("hdmap_echo");
    hdmap_app->init();
    hdmap_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    hdmap_app->offer_service(SVC, INST);
    hdmap_app->start();
}

// ========================================= Process RTT =========================================


int main(int argc, char* argv[])
{
    std::vector<std::thread> thread_list;
    UNUSED(argc);
    UNUSED(argv);

    if(!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        std::cout << "!ara::core::Initialize()";
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if(!HDMap::RegisterSigTermHandler()) {
        katech::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    katech::Log::Info() << "Hdmap: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    katech::Log::Info() << "Hdmap: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    katech::Log::Info() << "Ok, let's produce some Hdmap data...";

    {
        HDMap::localmap_provider = std::make_shared<katech::LocalMap_Provider>();
        HDMap::localmap_provider->init("Hdmap/Hdmap/PPort_local_map");
        HDMap::localmap_provider->setCallback(HDMap::setMapData);
    }

    {
        HDMap::collectFault_Provider = std::make_shared<katech::CollectFaultHdmap_Provider>();
        HDMap::collectFault_Provider->init("Hdmap/Hdmap/PPort_collect_fault_hdmap");
        HDMap::sendFault();
    }

    thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(HDMap::ThreadReceiveVehicleLocataion));
    thread_list.push_back(std::thread(HDMap::ThreadReceiveRoutingSearch));
    thread_list.push_back(std::thread(HDMap::ThreadSendCollectFault));
    thread_list.push_back(std::thread(HDMap::ThreadSendPointMap));
    thread_list.push_back(std::thread(HDMap::ThreadMonitor));

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
