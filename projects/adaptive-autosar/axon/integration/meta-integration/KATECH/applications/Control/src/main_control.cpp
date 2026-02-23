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
// or by any means, without permission in writing to the publisher.
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

#include "can_data_provider.h"
#include "can_data_etc_provider.h"
#include "collect_fault_control_provider.h"
#include "control_bridge_provider.h"
#include "sensor_radar_provider.h"
#include "bridge_control_subscriber.h"
#include "driving_trajectory_subscriber.h"
#include "report_fault_subscriber.h"
#include "smart_cruise_control_subscriber.h"
#include "vehicle_location_subscriber.h"

std::random_device m_rd;
std::default_random_engine m_rand_eng(m_rd());
std::uniform_real_distribution<double> m_ud_10000_10000(-10000, 10000);
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::int64_t> m_ud_0_100(0, 100);
std::uniform_int_distribution<std::uint8_t> m_ud_0_4(0, 4);
std::uniform_int_distribution<std::int32_t> m_ud_400_400(-400, 400);

namespace Control
{

std::shared_ptr<katech::CollectFaultControl_Provider> collectFault_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gReceivedEvent_count_bridge_control{0};
std::atomic_uint gReceivedEvent_count_driving_trajectory{0};
std::atomic_uint gReceivedEvent_count_report_fault{0};
std::atomic_uint gReceivedEvent_count_smart_cruise_control{0};
std::atomic_uint gReceivedEvent_count_vehicle_location{0};
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
    static katech::collect_fault_control_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Control.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Control.push_back(i);
        }
    }

    collectFault_provider->send(gCollectFault);
    katech::Log::Verbose() << "[Control] send collectFault Event ";
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

void ThreadReceiveControlBridge()
{
    katech::Log::Info() << "Control ThreadReceiveControlBridge Start!!";
    katech::BridgeControl_Subscriber bridgeControl_subscriber;
    bridgeControl_subscriber.init("Control/Control/RPort_bridge_control");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Control] ThreadReceiveControlBridge loop";
        bool bridgeControl_rxEvent = bridgeControl_subscriber.waitEvent(120); // wait event

        if(bridgeControl_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Control Bridge Control received";

            while(!bridgeControl_subscriber.isEventQueueEmpty()) {
                auto data = bridgeControl_subscriber.getEvent();
                gReceivedEvent_count_bridge_control++;

                auto Acc = data->Acc;
                auto Break = data->Break;
                auto Steer = data->Steer;
                auto Enable = data->Enable;
                auto Turn_signal = data->Turn_signal;

                katech::Log::Verbose() << "Acc : " << Acc;
                katech::Log::Verbose() << "Break : " << Break;
                katech::Log::Verbose() << "Steer : " << Steer;

                switch(Turn_signal){
                case 'L':
                    katech::Log::Verbose() << "Turn_signal : L";
                    break;
                case 'R':
                    katech::Log::Verbose() << "Turn_signal : R";
                    break;
                case 'E':
                    katech::Log::Verbose() << "Turn_signal : E";
                    break;
                case 'O':
                    katech::Log::Verbose() << "Turn_signal : O";
                    break;
                }

                if(Enable)
                    katech::Log::Verbose() << "Enable is true";
                else
                    katech::Log::Verbose() << "Enable is false";
            }
        }
    }
}

void ThreadReceiveDrivingTrajectory()
{
    katech::Log::Info() << "Control ThreadReceiveDrivingTrajectory Start!!";
    katech::DrivingTrajectory_Subscriber drivingTrajectory_subscriber;
    drivingTrajectory_subscriber.init("Control/Control/RPort_driving_trajectory");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Control] ThreadReceiveDrivingTrajectory loop";
        bool drivingTrajectory_rxEvent = drivingTrajectory_subscriber.waitEvent(120); // wait event

        if(drivingTrajectory_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Control Driving Trajectory received";

            while(!drivingTrajectory_subscriber.isEventQueueEmpty()) {
                auto data = drivingTrajectory_subscriber.getEvent();

                if(data != nullptr) {
                    gReceivedEvent_count_driving_trajectory++;
                    katech::Log::Verbose() << "[Enter] Control driving_trajectory Event Callback";
                    auto trajectoryPoint = data->TrajectoryPointVector;
                    auto position = data->Position;
                    auto targetSpeed = data->Target_speed;
                    auto driveMode = data->Drive_Mode;
                    auto emergencyAcceleration = data->Emergency_acceleration;
                    auto turnSignal = data->Turn_Signal;
                    auto highWay = data->High_Way;

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

                    if(driveMode) {
                        katech::Log::Verbose() << "Drive_Mode : true";
                    } else {
                        katech::Log::Verbose() << "Drive_Mode : false";
                    }

                    katech::Log::Verbose() << "Emergency_acceleration :  " << emergencyAcceleration;
                    katech::Log::Verbose() << "Turn_Signal :  " << turnSignal;
                    katech::Log::Verbose() << "High_Way :  " << highWay;
                }
            } 
        } else{
            katech::Log::Verbose() << "Control driving_trajectory timeout...";
        }
    }
}

void ThreadReceiveReportFault()
{
    katech::Log::Info() << "Control ThreadReceiveReportFault Start!!";
    katech::ReportFault_Subscriber reportFault_subscriber;
    reportFault_subscriber.init("Control/Control/RPort_report_fault");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Control] ThreadReceiveReportFault loop";
        bool reportFault_rxEvent = reportFault_subscriber.waitEvent(120); // wait event

        if(reportFault_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Control Report Fault received";

            while(!reportFault_subscriber.isEventQueueEmpty()) {
                auto data = reportFault_subscriber.getEvent();
                gReceivedEvent_count_report_fault++;
                katech::Log::Verbose() << "[Enter] Control report_fault Event Callback";
                auto AP = data->AP;
                auto FaultList = data->FaultList;
                katech::Log::Verbose() << "AP num : " << AP;

                if(!FaultList.empty()) {
                    katech::Log::Verbose() << "=== Fault Values ===";

                    for(auto itr = FaultList.begin(); itr != FaultList.end(); ++itr) {
                        katech::Log::Verbose() << *itr;
                    }

                } else {
                    katech::Log::Info() << "FaultList Vector empty!!! ";
                }
            }
        }
    }
}

void ThreadReceiveSmartCruiseControl()
{
    katech::Log::Info() << "Control ThreadReceiveSmartCruiseControl Start!!";
    katech::SmartCruiseControl_Subscriber smartCruiseControl_subscriber;
    smartCruiseControl_subscriber.init("Control/Control/RPort_smart_cruise_control");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Control] ThreadReceiveSmartCruiseControl loop";
        bool smartCruiseControl_rxEvent = smartCruiseControl_subscriber.waitEvent(120); // wait event

        if(smartCruiseControl_rxEvent) {
            katech::Log::Verbose() << "[EVENT] Control Smart Cruise Control received";

            while(!smartCruiseControl_subscriber.isEventQueueEmpty()) {
                auto data = smartCruiseControl_subscriber.getEvent();
                gReceivedEvent_count_smart_cruise_control++;

                katech::Log::Verbose() << "[Enter] Control smart_cruise_control Event Callback";

                auto targetVelocity = data->TargetVelocity;
                auto objectRelSpeed = data->ObjectRelSpeed;
                auto objectDistance = data->ObjectDistance;

                katech::Log::Verbose() << "TargetVelocity : " << targetVelocity;
                katech::Log::Verbose() << "ObjectRelSpeed : " << objectRelSpeed;
                katech::Log::Verbose() << "ObjectDistance : " << objectDistance;
            }
        }
    }
}

void ThreadReceiveVehicleLocation()
{
    katech::Log::Info() << "Control ThreadReceiveVehicleLocation Start!!";
    katech::VehicleLocation_Subscriber vehicleLocation_subscriber;
    vehicleLocation_subscriber.init("Control/Control/RPort_vehicle_location");

    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Control] ThreadReceiveVehicleLocation loop";
        bool vehicleLocation_rxEvent = vehicleLocation_subscriber.waitEvent(120);

        if(vehicleLocation_rxEvent) {
            while(!vehicleLocation_subscriber.isEventQueueEmpty()) {
                auto data = vehicleLocation_subscriber.getEvent();
                gReceivedEvent_count_vehicle_location++;
                katech::Log::Verbose() << "[EVENT] Control Vehicle Location received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Control vehicle_location Event Callback";
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
            katech::Log::Verbose() << "Control vehicle_location timeout...";
        }
    }
}

void ThreadSendCanData()
{
    katech::Log::Info() << "Control ThreadSendCanData Start!!";
    katech::CanData_Provider canData_provider;
    canData_provider.init("Control/Control/PPort_can_data");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        //write can_data for 50 ms cycles
        katech::can_data_Objects canData;
        canData.WheelSpeed_FL = m_ud_10000_10000(m_rand_eng);
        canData.WheelSpeed_FR = m_ud_10000_10000(m_rand_eng);
        canData.WheelSpeed_RL = m_ud_10000_10000(m_rand_eng);
        canData.WheelSpeed_RR = m_ud_10000_10000(m_rand_eng);
        canData.ClusterSpeed = m_ud_10000_10000(m_rand_eng);
        canData.GearState = m_ud_0_4(m_rand_eng);
        canData.LateralAcceleration = m_ud_10000_10000(m_rand_eng);
        canData.LongitudinalAcceleration = m_ud_10000_10000(m_rand_eng);
        canData.Angular_Acceleration = m_ud_10000_10000(m_rand_eng);
        canData.Turn_Signal = m_ud_0_4(m_rand_eng);
        canData.AutoDrivingMode = true;
        canData.Emergency_Light = m_ud_0_4(m_rand_eng) % 2;
        canData.No_Autonomous_Driving = false;
        canData.SteeringWheelAngle = m_ud_10000_10000(m_rand_eng);
        canData.ControlSteering = m_ud_400_400(m_rand_eng);
        canData.LaneInfo.LanePosition.Left = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LanePosition.Right = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneHeadingAngle.Left = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneHeadingAngle.Right = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneCurvature.Left = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneCurvature.Right = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneCurvatureRate.Left = m_ud_10000_10000(m_rand_eng);
        canData.LaneInfo.LaneCurvatureRate.Right = m_ud_10000_10000(m_rand_eng);
        canData.WheelTick_FL = m_ud_0_10000(m_rand_eng);
        canData.WheelTick_FR = m_ud_0_10000(m_rand_eng);
        canData.WheelTick_RL = m_ud_0_10000(m_rand_eng);
        canData.WheelTick_RR = m_ud_0_10000(m_rand_eng);

        canData_provider.send(canData);
        katech::Log::Verbose() << "[Control] send canData Event";
    }
}

void ThreadSendCanEtcData()
{
    katech::Log::Info() << "Control ThreadSendCanEtcData Start!!";
    katech::CanDataEtc_Provider canDataEtc_provider;
    canDataEtc_provider.init("Control/Control/PPort_can_data_etc");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //write can_data_etc
        katech::can_data_etc_Objects canDataEtc;
        canDataEtc.SeatBelt = m_ud_0_4(m_rand_eng);
        canDataEtc.Radar_Status = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Head_Light = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.High_Beam = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Fog_Lights = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Backup_Lamp = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Wiper = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Horn = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        canDataEtc.Battery_Status = m_ud_0_4(m_rand_eng);
        canDataEtc.Battery_Gauge = m_ud_0_4(m_rand_eng);
        canDataEtc.Odometer = m_ud_0_100(m_rand_eng);
        canDataEtc.Driving_Distance = m_ud_0_100(m_rand_eng);
        canDataEtc.Avg_Elec_Consum = m_ud_10000_10000(m_rand_eng);
        canDataEtc_provider.send(canDataEtc);
        katech::Log::Verbose() << "[Control] send canDataEtc Event";
    }
}

void ThreadSendSensorRadar()
{
    katech::Log::Info() << "Control ThreadSendSensorRadar Start!!";
    katech::SensorRadar_Provider sensorRadar_provider;
    sensorRadar_provider.init("Control/Control/PPort_sensor_radar");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //write sensor_radar
        katech::sensor_radar_Objects sensorRadar;
        katech::PointStruct sideRadarPoint;

        sensorRadar.Front.length = m_ud_10000_10000(m_rand_eng);
        sensorRadar.Front.RelativeVelocity = m_ud_10000_10000(m_rand_eng);

        sensorRadar.Side.clear();

        for (int i = 0; i < 15; ++i)
        {
            sideRadarPoint.X = m_ud_10000_10000(m_rand_eng);
            sideRadarPoint.Y = m_ud_10000_10000(m_rand_eng);

            sensorRadar.Side.push_back(sideRadarPoint);
        }

        sensorRadar_provider.send(sensorRadar);
        katech::Log::Verbose() << "[Control] send sensorRadar Event";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(1);
        setFault(2);
        setFault(4);
        setFault(5);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(1);
        clearFault(2);
        clearFault(4);
        clearFault(5);
        sendFault();
        INFO("Clear fault");
    }
}

void ThreadSendControlBridge()
{
    katech::Log::Info() << "Control ThreadSendCanEtcData Start!!";

    katech::ControlBridge_Provider controlBridge_provider;
    controlBridge_provider.init("Control/Control/PPort_control_bridge");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //write controlBridge data
        katech::control_bridge_Objects controlBridge;

        controlBridge.Acc = m_ud_10000_10000(m_rand_eng);
        controlBridge.Break = m_ud_10000_10000(m_rand_eng);
        controlBridge.Steer = m_ud_10000_10000(m_rand_eng);

        controlBridge.Enable = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        controlBridge.Turn_signal = m_ud_0_10000(m_rand_eng) % 2 == 0 ? 'L' : 
                                    m_ud_0_10000(m_rand_eng) % 3 == 0 ? 'R' :
                                    m_ud_0_10000(m_rand_eng) % 5 == 0 ? 'E' : 
                                    'O';

        controlBridge_provider.send(controlBridge);
        katech::Log::Verbose() << "[Control] send controlBridge Event";
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

            if(gReceivedEvent_count_bridge_control != 0) {
                katech::Log::Info() << "bridge_control Received count = " << gReceivedEvent_count_bridge_control;
                gReceivedEvent_count_bridge_control = 0;

            } else {
                katech::Log::Info() << "bridge_control event timeout!!!";
            }

            if(gReceivedEvent_count_driving_trajectory != 0) {
                katech::Log::Info() << "driving_trajectory Received count = " << gReceivedEvent_count_driving_trajectory;
                gReceivedEvent_count_driving_trajectory = 0;

            } else {
                katech::Log::Info() << "driving_trajectory event timeout!!!";
            }

            if(gReceivedEvent_count_report_fault != 0) {
                katech::Log::Info() << "report_fault Received count = " << gReceivedEvent_count_report_fault;
                gReceivedEvent_count_report_fault = 0;

            } else {
                katech::Log::Info() << "report_fault event timeout!!!";
            }

            if(gReceivedEvent_count_smart_cruise_control != 0) {
                katech::Log::Info() << "smart_cruise_control Received count = " << gReceivedEvent_count_smart_cruise_control;
                gReceivedEvent_count_smart_cruise_control = 0;

            } else {
                katech::Log::Info() << "smart_cruise_control event timeout!!!";
            }

            if(gReceivedEvent_count_vehicle_location != 0) {
                katech::Log::Info() << "vehicle_location Received count = " << gReceivedEvent_count_vehicle_location;
                gReceivedEvent_count_vehicle_location = 0;

            } else {
                katech::Log::Info() << "vehicle_location event timeout!!!";
            }
        }
    }
}

}  // namespace

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

    if(!Control::RegisterSigTermHandler()) {
        katech::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    katech::Log::Info() << "Control: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
                   ara::com::e2exf::ConfigurationFormat::JSON,
                   "./etc/e2e_statemachines.json",
                   ara::com::e2exf::ConfigurationFormat::JSON);
    katech::Log::Info() << "Control: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    katech::Log::Info() << "Ok, let's produce some Control data...";

    {
        Control::collectFault_provider = std::make_shared<katech::CollectFaultControl_Provider>();
        Control::collectFault_provider->init("Control/Control/PPort_collect_fault_control");
        Control::sendFault();
    }
    
    thread_list.push_back(std::thread(Control::ThreadReceiveControlBridge));
    thread_list.push_back(std::thread(Control::ThreadReceiveDrivingTrajectory));
    thread_list.push_back(std::thread(Control::ThreadReceiveSmartCruiseControl));
    thread_list.push_back(std::thread(Control::ThreadReceiveReportFault));
    thread_list.push_back(std::thread(Control::ThreadReceiveVehicleLocation));
    thread_list.push_back(std::thread(Control::ThreadSendCanData));
    thread_list.push_back(std::thread(Control::ThreadSendCanEtcData));
    thread_list.push_back(std::thread(Control::ThreadSendSensorRadar));
    thread_list.push_back(std::thread(Control::ThreadSendCollectFault));
    thread_list.push_back(std::thread(Control::ThreadSendControlBridge));
    thread_list.push_back(std::thread(Control::ThreadMonitor));

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

