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

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <mutex>
#include <numeric>
#include <queue>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <arpa/inet.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <math.h>
#include <net/if.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <vsomeip/vsomeip.hpp>

#include "ADCM_KATECH.h"
#include "NatsConnManager.h"
#include "ara/core/initialization.h"
#include "bridge_control_provider.h"
#include "bridge_control_subscriber.h"
#include "can_data_etc_provider.h"
#include "can_data_provider.h"
#include "collect_fault_control_provider.h"
#include "control_bridge_provider.h"
#include "driving_trajectory_subscriber.h"
#include "logger.h"
#include "report_fault_subscriber.h"
#include "sensor/gps/proj_converter.h"
#include "sensor_radar_provider.h"
#include "udpClient.h"

KATECH::func KatechControl;

std::shared_ptr<adcm::sensor::gps::ProjConverter> gProj;

namespace Control
{

std::shared_ptr<adcm::CollectFaultControl_Provider> collectFault_provider;
std::shared_ptr<adcm::DrivingTrajectory_Subscriber> drivingTrajectory_subscriber;

std::atomic_bool continueExecution{true};
std::atomic_uint gReceivedEvent_count_driving_trajectory{0};
std::atomic_uint gReceivedEvent_count_report_fault{0};
std::atomic_uint gReceivedEvent_count_planning_path{0};
std::atomic_uint gMainthread_Loopcount{0};
std::atomic_uint gReceivedEvent_count_bridge_control{0};
std::atomic_uint RemoteEnable{0};

std::atomic_uint64_t gFaultStatus{0};
double RxAcc;
double RxBreak;
double RxSteer;
uint8_t RxEnable;
uint8_t RxTurn_signal;
std::atomic_uint gReceivedEvent_count_nats{0};

/**
 * @brief Set the fault bit for a specific fault ID.
 *
 * Updates the internal fault status bitmap by setting the bit at the
 * requested fault index.
 *
 * @param faultID Fault bit index to set.
 * @return void
 */
void setFault(unsigned char faultID)
{
    gFaultStatus |= (static_cast<std::uint64_t>(1) << faultID);
}

/**
 * @brief Clear the fault bit for a specific fault ID.
 *
 * Updates the internal fault status bitmap by clearing the bit at the
 * requested fault index.
 *
 * @param faultID Fault bit index to clear.
 * @return void
 */
void clearFault(unsigned char faultID)
{
    gFaultStatus &= ~((static_cast<std::uint64_t>(1) << faultID));
}

/**
 * @brief Get the current aggregated fault bitmap.
 *
 * Returns the current 64-bit fault mask that is managed by this process.
 *
 * @param None.
 * @return Current fault bitmap value.
 */
std::uint64_t getFaultStatus()
{
    return gFaultStatus;
}

/**
 * @brief Publish the current fault information through ara::com.
 *
 * Converts the internal bitmap representation into a fault list and sends
 * the list through the collect-fault provider port.
 *
 * @param None.
 * @return void
 */
void sendFault()
{
    static adcm::collect_fault_control_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Control.clear();
    std::uint64_t temp = getFaultStatus();

    for (int i = 0; i < 64; i++) {
        if (temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Control.push_back(i);
        }
    }

    collectFault_provider->send(gCollectFault);
    adcm::Log::Verbose() << "[Control] send collectFault Event ";
}

/**
 * @brief Handle process termination signal.
 *
 * Marks the global execution flag as false so all worker loops can finish
 * gracefully when SIGTERM or SIGINT is received.
 *
 * @param signal POSIX signal number.
 * @return void
 */
void SigTermHandler(int signal)
{
    if ((signal == SIGTERM) || (signal == SIGINT)) {
        continueExecution = false;
    }
}

/**
 * @brief Register process termination handlers for graceful shutdown.
 *
 * Configures signal action so the application can stop worker loops and exit
 * cleanly when the process is terminated.
 *
 * @param None.
 * @return True if registration succeeds, otherwise false.
 */
bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        return false;
    }
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        return false;
    }

    return true;
}

}  // namespace Control

namespace
{

constexpr char kDefaultNatsServerUrl[] = "https://nats.beyless.com";
constexpr char kDefaultNatsRemoteControlSubject[] = "katech.remote.control";
constexpr char kDefaultNatsRemoteStatusSubject[] = "katech.remote.status";

struct RemoteControlCommand
{
    double steering{0.0};
    double acc{0.0};
    double brake{0.0};
    bool remoteEnable{false};
    bool enable{false};
    uint8_t turnSignal{0};
    unsigned long long timestamp{0};
};

std::shared_ptr<adcm::etc::NatsConnManager> gNatsSubscribeManager;
std::shared_ptr<adcm::etc::NatsConnManager> gNatsPublishManager;
std::mutex gNatsRemoteControlMutex;
std::queue<std::string> gNatsRemoteControlJsonQueue;
std::condition_variable gNatsRemoteControlCv;

const char* GetEnvOrDefault(const char* key, const char* defaultValue)
{
    const char* envValue = std::getenv(key);
    if ((envValue != nullptr) && (std::strlen(envValue) > 0)) {
        return envValue;
    }
    return defaultValue;
}

const char* GetNatsServerUrl()
{
    return GetEnvOrDefault("KATECH_NATS_SERVER_URL", kDefaultNatsServerUrl);
}

const char* GetNatsRemoteControlSubject()
{
    return GetEnvOrDefault("KATECH_NATS_REMOTE_CONTROL_SUBJECT", kDefaultNatsRemoteControlSubject);
}

const char* GetNatsRemoteStatusSubject()
{
    return GetEnvOrDefault("KATECH_NATS_REMOTE_STATUS_SUBJECT", kDefaultNatsRemoteStatusSubject);
}

bool ParseBooleanField(const Poco::Dynamic::Var& value)
{
    const std::string parsed = value.toString();
    return (parsed == "true") || (parsed == "1") || (parsed == "TRUE") || (parsed == "True");
}

uint8_t ParseTurnSignalField(const std::string& turnSignal)
{
    if (turnSignal == "L") {
        return 1;
    }
    if (turnSignal == "R") {
        return 2;
    }
    if (turnSignal == "E") {
        return 3;
    }
    return 0;
}

double ParseDoubleField(const Poco::JSON::Object::Ptr& jsonObject, const std::string& key, double defaultValue)
{
    if ((jsonObject == nullptr) || !jsonObject->has(key)) {
        return defaultValue;
    }
    try {
        return std::stod(jsonObject->get(key).toString());
    } catch (...) {
        return defaultValue;
    }
}

bool ParseBooleanField(const Poco::JSON::Object::Ptr& jsonObject, const std::string& key, bool defaultValue)
{
    if ((jsonObject == nullptr) || !jsonObject->has(key)) {
        return defaultValue;
    }
    try {
        return ParseBooleanField(jsonObject->get(key));
    } catch (...) {
        return defaultValue;
    }
}

uint8_t ParseTurnSignalField(const Poco::JSON::Object::Ptr& jsonObject, const std::string& key, uint8_t defaultValue)
{
    if ((jsonObject == nullptr) || !jsonObject->has(key)) {
        return defaultValue;
    }
    try {
        const std::string turnSignal = jsonObject->get(key).toString();
        return ParseTurnSignalField(turnSignal);
    } catch (...) {
        return defaultValue;
    }
}

unsigned long long ParseTimestampField(const Poco::JSON::Object::Ptr& jsonObject, unsigned long long defaultValue)
{
    if ((jsonObject == nullptr) || !jsonObject->has("timestamp")) {
        return defaultValue;
    }
    try {
        return static_cast<unsigned long long>(std::stoull(jsonObject->get("timestamp").toString()));
    } catch (...) {
        return defaultValue;
    }
}

void NatsAsyncErrorCallback(natsConnection* nc, natsSubscription* sub, natsStatus err, void* closure)
{
    (void)nc;
    (void)closure;
    adcm::Log::Error() << "[NATS] Async error: " << natsStatus_GetText(err);

    if ((gNatsSubscribeManager != nullptr) && (sub != nullptr)) {
        gNatsSubscribeManager->NatsSubscriptionGetDropped(
            sub, reinterpret_cast<int64_t*>(&gNatsSubscribeManager->dropped));
    }
}

void NatsMessageCallback(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure)
{
    (void)nc;
    (void)sub;
    (void)closure;

    {
        std::lock_guard<std::mutex> lock(gNatsRemoteControlMutex);
        gNatsRemoteControlJsonQueue.emplace(natsMsg_GetData(msg), natsMsg_GetDataLength(msg));
    }
    gNatsRemoteControlCv.notify_one();

    natsMsg_Destroy(msg);
}

bool EnsureNatsSubscribeConnection()
{
    if ((gNatsSubscribeManager != nullptr) && gNatsSubscribeManager->NatsIsConnected()) {
        return true;
    }

    const std::vector<const char*> subjects = {GetNatsRemoteControlSubject()};
    gNatsSubscribeManager = std::make_shared<adcm::etc::NatsConnManager>(GetNatsServerUrl(),
        subjects,
        NatsMessageCallback,
        NatsAsyncErrorCallback,
        adcm::etc::NatsConnManager::Mode::Default);

    const natsStatus status = gNatsSubscribeManager->NatsExecute();
    if (status != NATS_OK) {
        adcm::Log::Error() << "[NATS] Subscribe connection failed: " << natsStatus_GetText(status);
        gNatsSubscribeManager.reset();
        return false;
    }

    adcm::Log::Info() << "[NATS] Subscribe connection ready: " << GetNatsRemoteControlSubject();
    return true;
}

bool EnsureNatsPublishConnection()
{
    if ((gNatsPublishManager != nullptr) && gNatsPublishManager->NatsIsConnected()) {
        return true;
    }

    const std::vector<const char*> subjects = {GetNatsRemoteStatusSubject()};
    gNatsPublishManager = std::make_shared<adcm::etc::NatsConnManager>(
        GetNatsServerUrl(), subjects, adcm::etc::NatsConnManager::Mode::Publish_Only);

    const natsStatus status = gNatsPublishManager->NatsExecute();
    if (status != NATS_OK) {
        adcm::Log::Error() << "[NATS] Publish connection failed: " << natsStatus_GetText(status);
        gNatsPublishManager.reset();
        return false;
    }

    adcm::Log::Info() << "[NATS] Publish connection ready: " << GetNatsRemoteStatusSubject();
    return true;
}

}

float SCC_Lidar_ObjDistVal = 200;
float SCC_Lidar_ObjRelSpdVal = 200;
float SCC_Tspd = 10;

/**
 * @brief Execute the main KATECH control loop.
 *
 * Receives trajectory input, updates internal control state, runs lateral and
 * longitudinal control routines, and pushes results to the rest of the system.
 *
 * @param None.
 * @return void
 */
void KatechControlModule()
{
    uint64_t ADCMaliveCnt = 0, old_ADCMaliveCnt = 0, ADCMcntSame = 0;
    gProj = std::make_shared<adcm::sensor::gps::ProjConverter>(adcm::sensor::gps::ProjConverter::UTM_Type::UTM_52N);
    TrajectoryPointArray trajectoryPoint;
    adcm::Vector3DStruct Wgs_Position;
    double target_Accel = 1023;
    bool drive_Mode = 0;

    adcm::Log::Info() << "KatechControlModule start";
    auto end_time = std::chrono::steady_clock::now();
    SCC_Lidar_ObjDistVal = 200;
    SCC_Lidar_ObjRelSpdVal = 200;

    while (Control::continueExecution) {
        bool drivingTrajectory_rxEvent = Control::drivingTrajectory_subscriber->waitEvent(20);
        auto current_time = std::chrono::steady_clock::now();
        auto TimeGap = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - end_time).count();

        adcm::Log::Info() << "KatechControlModule Start";
        if ((TimeGap % 20) == 0) {
            KatechControl.ReceiveBuffer();
        }

        if (Control::RemoteEnable == 1) {
            KatechControl.SendBuffer_Remote(
                Control::RemoteEnable, Control::RxAcc, Control::RxBreak, Control::RxSteer, Control::RxTurn_signal);
        }

        if (drivingTrajectory_rxEvent) {
            ADCMaliveCnt++;
            while (!Control::drivingTrajectory_subscriber->isEventQueueEmpty()) {
                auto data = Control::drivingTrajectory_subscriber->getEvent();
                Control::gReceivedEvent_count_driving_trajectory++;
                {
                    trajectoryPoint.clear();
                    for (int i = 0; i < data->TrajectoryPointVector.size(); i++) {
                        double lon, lat;
                        auto temp = data->TrajectoryPointVector[i];
                        gProj->inverse(temp.Vector3D.x, temp.Vector3D.y, lon, lat);
                        temp.Vector3D.x = lon;
                        temp.Vector3D.y = lat;
                        trajectoryPoint.push_back(temp);
                    }
                }
                gProj->inverse(data->Position.x, data->Position.y, Wgs_Position.x, Wgs_Position.y);
                Wgs_Position.yaw = data->Position.yaw;
                target_Accel = data->Target_speed;
                drive_Mode = data->Drive_Mode;
                KATECH::Turn_Signal = data->Turn_Signal;
                (void)data->Emergency_acceleration;

                if (!Control::drivingTrajectory_subscriber->isEventQueueEmpty()) {
                    ERROR("Control processing time out!!!");
                }
            }
        } else {
            ERROR("Some/IP timeout!( %3dms )", TimeGap);
        }

        if (ADCMaliveCnt == old_ADCMaliveCnt) {
            ADCMcntSame++;
            if (ADCMcntSame > 10) {
                KATECH::ADCMFault = 0;
            }
            if (ADCMcntSame >= 30000)
                ADCMcntSame = 11;
        } else {
            ADCMcntSame = 0;
            KATECH::ADCMFault = 1;
        }
        old_ADCMaliveCnt = ADCMaliveCnt;
        end_time = std::chrono::steady_clock::now();
        KATECH::log::old_time = std::chrono::steady_clock::now();

        if (drivingTrajectory_rxEvent || (TimeGap > 50)) {
            KatechControl.Initialize(Wgs_Position.x,
                Wgs_Position.y,
                Wgs_Position.yaw,
                trajectoryPoint,
                target_Accel,
                drive_Mode,
                SCC_Lidar_ObjDistVal,
                SCC_Lidar_ObjRelSpdVal,
                SCC_Tspd);

            if (trajectoryPoint.size() > 10) {
                KatechControl.GenerateLocalPath();
                KatechControl.LongiControl();
                KatechControl.LatiControl();
                KATECH::log::current_time = std::chrono::steady_clock::now();
                KATECH::log::TimeGap = std::chrono::duration_cast<std::chrono::milliseconds>(
                    KATECH::log::current_time - KATECH::log::old_time)
                                           .count();
                KATECH::log::old_time = std::chrono::steady_clock::now();
            }
        }
    }
}

/**
 * @brief Receive driving trajectory data from ara::com.
 *
 * Subscribes to the driving trajectory port and parses incoming trajectory
 * events for diagnostics and fault-state handling.
 *
 * @param None.
 * @return void
 */
void ThreadReceiveDrivingTrajectory()
{
    adcm::Log::Info() << "Control ThreadReceiveDrivingTrajectory Start!!";
    adcm::DrivingTrajectory_Subscriber drivingTrajectory_subscriber;
    drivingTrajectory_subscriber.init("Control/Control/RPort_driving_trajectory");

    while (Control::continueExecution) {
        Control::gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Control] ThreadReceiveDrivingTrajectory loop";
        bool drivingTrajectory_rxEvent = drivingTrajectory_subscriber.waitEvent(120);

        if (drivingTrajectory_rxEvent) {
            adcm::Log::Verbose() << "[EVENT] Control Driving Trajectory received";

            while (!drivingTrajectory_subscriber.isEventQueueEmpty()) {
                auto data = drivingTrajectory_subscriber.getEvent();

                if (data != nullptr) {
                    Control::gReceivedEvent_count_driving_trajectory++;
                    adcm::Log::Verbose() << "[Enter] Control driving_trajectory Event Callback";
                    auto trajectoryPoint = data->TrajectoryPointVector;
                    auto position = data->Position;
                    auto targetSpeed = data->Target_speed;
                    auto driveMode = data->Drive_Mode;
                    auto emergencyAcceleration = data->Emergency_acceleration;

                    if (!trajectoryPoint.empty()) {
                        adcm::Log::Verbose() << "trajectoryPoint: ";

                        for (typename std::vector<adcm::TrajectoryPoint>::iterator itr = trajectoryPoint.begin();
                            itr != trajectoryPoint.end();
                            ++itr) {
                            adcm::Log::Verbose() << "TrajectoryPoint.Vector3D.X : " << itr->Vector3D.x;
                            adcm::Log::Verbose() << "TrajectoryPoint.Vector3D.Y : " << itr->Vector3D.y;
                            adcm::Log::Verbose() << "TrajectoryPoint.Vector3D.Z : " << itr->Vector3D.yaw;
                        }

                    } else {
                        adcm::Log::Error() << "trajectoryPoint vector empty!!! ";
                    }

                    adcm::Log::Verbose() << "Position.x :  " << position.x;
                    adcm::Log::Verbose() << "Position.y :  " << position.y;
                    adcm::Log::Verbose() << "Position.yaw :  " << position.yaw;
                    adcm::Log::Verbose() << "Target_speed :  " << targetSpeed;

                    if (driveMode) {
                        adcm::Log::Verbose() << "Drive_Mode : true";

                    } else {
                        adcm::Log::Verbose() << "Drive_Mode : false";
                    }

                    adcm::Log::Verbose() << "Emergency_acceleration :  " << emergencyAcceleration;
                }

                Control::clearFault(4);
            }

        } else {
            adcm::Log::Error() << "Control driving_trajectory timeout...";
            Control::setFault(4);
        }
    }
}

/**
 * @brief Receive fault reports from ara::com services.
 *
 * Subscribes to the report-fault port and prints incoming fault payloads for
 * monitoring and diagnosis.
 *
 * @param None.
 * @return void
 */
void ThreadReceiveReportFault()
{
    adcm::Log::Info() << "Control ThreadReceiveReportFault Start!!";
    adcm::ReportFault_Subscriber reportFault_subscriber;
    reportFault_subscriber.init("Control/Control/RPort_report_fault");

    while (Control::continueExecution) {
        Control::gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Control] ThreadReceiveReportFault loop";
        bool reportFault_rxEvent = reportFault_subscriber.waitEvent(120);

        if (reportFault_rxEvent) {
            adcm::Log::Verbose() << "[EVENT] Control Report Fault received";

            while (!reportFault_subscriber.isEventQueueEmpty()) {
                auto data = reportFault_subscriber.getEvent();
                Control::gReceivedEvent_count_report_fault++;
                adcm::Log::Verbose() << "[Enter] Control report_fault Event Callback";
                auto AP = data->AP;
                auto FaultList = data->FaultList;
                adcm::Log::Info() << "AP num : " << AP;

                if (!FaultList.empty()) {
                    adcm::Log::Info() << "=== Fault Values ===";

                    for (auto itr = FaultList.begin(); itr != FaultList.end(); ++itr) {
                        adcm::Log::Info() << *itr;
                    }

                } else {
                    adcm::Log::Info() << "FaultList Vector empty!!! ";
                }
            }
        }
    }
}

/**
 * @brief Publish periodic CAN data to ara::com.
 *
 * Builds the CAN payload from current KATECH runtime variables and publishes
 * it on a fixed cycle to the CAN data provide port.
 *
 * @param None.
 * @return void
 */
void ThreadSendCanData()
{
    adcm::Log::Info() << "Control ThreadSendCanData Start!!";
    adcm::CanData_Provider canData_provider;
    canData_provider.init("Control/Control/PPort_can_data");

    auto end_time = std::chrono::steady_clock::now();

    while (Control::continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        adcm::can_data_Objects canData;

        canData.WheelSpeed_FL = KATECH::WheelSpeed_FL;
        canData.WheelSpeed_FR = KATECH::WheelSpeed_FR;
        canData.WheelSpeed_RL = KATECH::WheelSpeed_RL;
        canData.WheelSpeed_RR = KATECH::WheelSpeed_RR;
        canData.ClusterSpeed = KATECH::ClusterSpeed;
        canData.GearState = KATECH::GearState;
        canData.LateralAcceleration = KATECH::LateralAcceleration;
        canData.LongitudinalAcceleration = KATECH::LongitudinalAcceleration;
        canData.Angular_Acceleration = KATECH::Angular_Acceleration;
        canData.Turn_Signal = KATECH::Turn_SignalReport;
        canData.AutoDrivingMode = KATECH::AutoDrivingMode;
        canData.Emergency_Light = KATECH::Emergency_Light;
        canData.No_Autonomous_Driving = KATECH::No_Autonomous_Driving;
        canData.SteeringWheelAngle = KATECH::SteeringWheelAngle;
        canData.ControlSteering = KATECH::ControlHandle_Log;
        canData.LaneInfo.LanePosition.Left = KATECH::LanePosition_Left;
        canData.LaneInfo.LanePosition.Right = KATECH::LanePosition_Right;
        canData.LaneInfo.LaneHeadingAngle.Left = KATECH::LaneHeadingAngle_Left;
        canData.LaneInfo.LaneHeadingAngle.Right = KATECH::LaneHeadingAngle_Right;
        canData.LaneInfo.LaneCurvature.Left = KATECH::LaneCurvature_Left;
        canData.LaneInfo.LaneCurvature.Right = KATECH::LaneCurvature_Right;
        canData.LaneInfo.LaneCurvatureRate.Left = KATECH::LaneCurvatureRate_Left;
        canData.LaneInfo.LaneCurvatureRate.Right = KATECH::LaneCurvatureRate_Right;
        canData.WheelTick_FL = KATECH::WheelPulse_FL;
        canData.WheelTick_FR = KATECH::WheelPulse_FR;
        canData.WheelTick_RL = KATECH::WheelPulse_RL;
        canData.WheelTick_RR = KATECH::WheelPulse_RR;
        canData.RPM = KATECH::HandleState;
        canData.Accel_Pedal = KATECH::AccelCylPrs;
        canData.Brake_Pedal = KATECH::brakecylPrs;
        canData_provider.send(canData);

        auto current_time = std::chrono::steady_clock::now();
        (void)std::chrono::duration_cast<std::chrono::milliseconds>(current_time - end_time).count();
        end_time = current_time;
    }

    adcm::Log::Info() << "[CANSEND] continueExecution" << Control::continueExecution;
}

/**
 * @brief Publish radar sensor objects to ara::com.
 *
 * Builds radar object vectors from KATECH runtime values and publishes the
 * assembled radar payload at a fixed period.
 *
 * @param None.
 * @return void
 */
void ThreadSendSensorRadar()
{
    adcm::Log::Info() << "Control ThreadSendSensorRadar Start!!";
    adcm::SensorRadar_Provider sensorRadar_provider;
    sensorRadar_provider.init("Control/Control/PPort_sensor_radar");
    auto end_time = std::chrono::steady_clock::now();

    while (Control::continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        adcm::sensor_radar_Objects sensorRadar;
        adcm::PointStruct radarPointFL;
        adcm::PointStruct radarPointFR;
        adcm::PointStruct radarPointRL;
        adcm::PointStruct radarPointRR;
        adcm::PointStruct radarPointFL2;
        adcm::PointStruct radarPointFR2;
        adcm::PointStruct radarPointRL2;
        adcm::PointStruct radarPointRR2;
        adcm::PointStruct radarPointFL3;
        adcm::PointStruct radarPointFR3;
        adcm::PointStruct radarPointRL3;
        adcm::PointStruct radarPointRR3;
        adcm::PointStruct radarPointFL4;
        adcm::PointStruct radarPointFR4;
        adcm::PointStruct radarPointRL4;
        adcm::PointStruct radarPointRR4;
        adcm::PointStruct nearSensor;

        sensorRadar.Front.length = KATECH::Radar_ObjDistVal_Front;
        sensorRadar.Front.RelativeVelocity = KATECH::Radar_ObjRelSpdVal_Front;
        sensorRadar.Side.clear();

        if (KATECH::NearSensor == 0) {
            nearSensor.X = 0;
            nearSensor.Y = 0;
        } else if (KATECH::NearSensor == 1) {
            nearSensor.X = 1;
            nearSensor.Y = 0;
        } else if (KATECH::NearSensor == 2) {
            nearSensor.X = 0;
            nearSensor.Y = 1;
        } else if (KATECH::NearSensor == 3) {
            nearSensor.X = 1;
            nearSensor.Y = 1;
        }

        radarPointFL.X = KATECH::CornerRadar_x1;
        radarPointFL.Y = KATECH::CornerRadar_y1;
        radarPointFR.X = KATECH::CornerRadar_x2;
        radarPointFR.Y = KATECH::CornerRadar_y2;
        radarPointRL.X = KATECH::CornerRadar_x3;
        radarPointRL.Y = KATECH::CornerRadar_y3;
        radarPointRR.X = KATECH::CornerRadar_x4;
        radarPointRR.Y = KATECH::CornerRadar_y4;
        radarPointFL2.X = KATECH::CornerRadar2_x1;
        radarPointFL2.Y = KATECH::CornerRadar2_y1;
        radarPointFR2.X = KATECH::CornerRadar2_x2;
        radarPointFR2.Y = KATECH::CornerRadar2_y2;
        radarPointRL2.X = KATECH::CornerRadar2_x3;
        radarPointRL2.Y = KATECH::CornerRadar2_y3;
        radarPointRR2.X = KATECH::CornerRadar2_x4;
        radarPointRR2.Y = KATECH::CornerRadar2_y4;
        radarPointFL3.X = KATECH::CornerRadar3_x1;
        radarPointFL3.Y = KATECH::CornerRadar3_y1;
        radarPointFR3.X = KATECH::CornerRadar3_x2;
        radarPointFR3.Y = KATECH::CornerRadar3_y2;
        radarPointRL3.X = KATECH::CornerRadar3_x3;
        radarPointRL3.Y = KATECH::CornerRadar3_y3;
        radarPointRR3.X = KATECH::CornerRadar3_x4;
        radarPointRR3.Y = KATECH::CornerRadar3_y4;
        radarPointFL4.X = KATECH::CornerRadar4_x1;
        radarPointFL4.Y = KATECH::CornerRadar4_y1;
        radarPointFR4.X = KATECH::CornerRadar4_x2;
        radarPointFR4.Y = KATECH::CornerRadar4_y2;
        radarPointRL4.X = KATECH::CornerRadar4_x3;
        radarPointRL4.Y = KATECH::CornerRadar4_y3;
        radarPointRR4.X = KATECH::CornerRadar4_x4;
        radarPointRR4.Y = KATECH::CornerRadar4_y4;

        sensorRadar.Side.push_back(nearSensor);

        sensorRadar.Side.push_back(radarPointFL);
        sensorRadar.Side.push_back(radarPointFR);
        sensorRadar.Side.push_back(radarPointRL);
        sensorRadar.Side.push_back(radarPointRR);
        sensorRadar.Side.push_back(radarPointFL2);
        sensorRadar.Side.push_back(radarPointFR2);
        sensorRadar.Side.push_back(radarPointRL2);
        sensorRadar.Side.push_back(radarPointRR2);
        sensorRadar.Side.push_back(radarPointFL3);
        sensorRadar.Side.push_back(radarPointFR3);
        sensorRadar.Side.push_back(radarPointRL3);
        sensorRadar.Side.push_back(radarPointRR3);
        sensorRadar.Side.push_back(radarPointFL4);
        sensorRadar.Side.push_back(radarPointFR4);
        sensorRadar.Side.push_back(radarPointRL4);
        sensorRadar.Side.push_back(radarPointRR4);
        sensorRadar_provider.send(sensorRadar);
        auto current_time = std::chrono::steady_clock::now();
        (void)std::chrono::duration_cast<std::chrono::milliseconds>(current_time - end_time).count();
        end_time = current_time;
        adcm::Log::Verbose() << "[Control] send sensorRadar Event";
    }
}

/**
 * @brief Publish aggregated fault states periodically.
 *
 * Monitors fault-related runtime counters and publishes fault bitmap updates
 * using the collect-fault provider port.
 *
 * @param None.
 * @return void
 */
void ThreadSendCollectFault()
{
    uint64_t old_vcualivecnt = 0, vcucntSame = 0;
    INFO("ThreadSendCollectFault Start!!");

    while (Control::continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        if (KATECH::vcualivecnt == old_vcualivecnt) {
            vcucntSame++;
            if (vcucntSame > 10)
                KATECH::VCUFault = 0;
            if (vcucntSame >= 30000)
                vcucntSame = 11;
        } else {
            vcucntSame = 0;
            KATECH::VCUFault = 1;
        }
        old_vcualivecnt = KATECH::vcualivecnt;

        if (KATECH::VCUFault == 0) {
            Control::setFault(4);
            Control::sendFault();
            INFO("vcufault!!");
        } else {
            Control::clearFault(4);
            Control::sendFault();
        }

        if ((KATECH::FrontRadarFault == 0) || (KATECH::LCANFault == 0)) {
            Control::setFault(5);
            Control::sendFault();
            INFO("FrontRadarfault!!");
        } else {
            Control::clearFault(5);
            Control::sendFault();
        }

        if (KATECH::MobileyeFault == 0) {
            Control::setFault(3);
            Control::sendFault();
            INFO("Mobileyefault!!");
        } else {
            Control::clearFault(3);
            Control::sendFault();
        }

        if (KATECH::LongiError == 1) {
            Control::setFault(6);
            Control::sendFault();
        } else {
            Control::clearFault(6);
            Control::sendFault();
        }
    }
}

/**
 * @brief Handle UDP callback data for SCC inputs.
 *
 * Decodes the fixed binary packet and updates KATECH shared values used by
 * longitudinal control logic.
 *
 * @param port UDP source port used by callback.
 * @param buffer Raw UDP payload buffer.
 * @return void
 */
void udpCallback_4SCC(int port, std::shared_ptr<adcm::etc::udpClient::udpBuffer> buffer)
{
    (void)port;
    double temp;
    unsigned char temp_b1;
    memcpy(&temp, &(*buffer)[0], sizeof(double));
    KATECH::SCC_TargetVelocity = (float)temp;
    memcpy(&temp, &(*buffer)[0] + sizeof(double), sizeof(double));
    KATECH::Lidar_ObjRelSpdVal_Front = (float)temp;
    memcpy(&temp, &(*buffer)[0] + sizeof(double) * 2, sizeof(double));
    KATECH::Lidar_ObjDistVal_Front = (float)temp;
    memcpy(&temp_b1, &(*buffer)[0] + sizeof(double) * 3, sizeof(unsigned char));
    KATECH::HMIFault = (uint8_t)temp_b1;
}

/**
 * @brief Run the top-level monitor thread.
 *
 * Starts support threads and periodically reports runtime receive counts while
 * the process stays alive.
 *
 * @param None.
 * @return void
 */
void ThreadMonitor()
{
    adcm::Log::Info() << "ThreadMonitor Start";

    int loop_count = 0;
    std::thread katech_thread_MAIN(&KatechControlModule);
    std::shared_ptr<adcm::etc::udpClient> udp_instance = std::make_shared<adcm::etc::udpClient>(udpCallback_4SCC, 9898);
    while (Control::continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        loop_count++;

        if (loop_count > 10) {
            loop_count = 0;
            INFO("driving count = %d", (int)Control::gReceivedEvent_count_driving_trajectory);
            Control::gReceivedEvent_count_driving_trajectory = 0;
        }
    }

    (void)udp_instance;
    if (katech_thread_MAIN.joinable()) {
        katech_thread_MAIN.join();
    }
}

/**
 * @brief Receive remote bridge control commands from ara::com.
 *
 * Waits for bridge-control events and updates shared remote-control values
 * used by the control execution path.
 *
 * @param None.
 * @return void
 */
void ThreadReceiveControlBridge()
{
    adcm::Log::Info() << "Control ThreadReceiveControlBridge Start!!";
    adcm::BridgeControl_Subscriber bridgeControl_subscriber;
    bridgeControl_subscriber.init("Control/Control/RPort_bridge_control");

    while (Control::continueExecution) {
        Control::gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Control] ThreadReceiveControlBridge loop";
        bool bridgeControl_rxEvent = bridgeControl_subscriber.waitEvent(120);

        if (bridgeControl_rxEvent) {
            adcm::Log::Verbose() << "[EVENT] Control Bridge Control received";

            while (!bridgeControl_subscriber.isEventQueueEmpty()) {
                auto data = bridgeControl_subscriber.getEvent();
                Control::gReceivedEvent_count_bridge_control++;

                Control::RxAcc = data->Acc;
                Control::RxBreak = data->Break;
                Control::RxSteer = data->Steer;
                Control::RemoteEnable = (uint8_t)(data->RemoteEnable);
                auto Enable = data->Enable;
                Control::RxTurn_signal = data->Turn_signal;

                adcm::Log::Info() << "[From AXON] RxAcc: " << Control::RxAcc;
                adcm::Log::Info() << "[From AXON] RxBreak: " << Control::RxBreak;
                adcm::Log::Info() << "[From AXON] RxSteer: " << Control::RxSteer;
                adcm::Log::Info() << "[From AXON] RxRemoteEnable: " << Control::RemoteEnable;
                adcm::Log::Info() << "[From AXON] RxEnable: " << Enable;
                adcm::Log::Info() << "[From AXON] RxTurn_signal: " << Control::RxTurn_signal;

                if (Enable) {
                    adcm::Log::Verbose() << "Enable is true";
                    Control::RxEnable = 1;
                } else {
                    adcm::Log::Verbose() << "Enable is false";
                    Control::RxEnable = 0;
                }
            }
        }
    }
}

/**
 * @brief Receive remote control commands from NATS and update control state.
 *
 * Connects to the NATS server, subscribes to the configured remote-control
 * subject, waits for JSON messages, parses fields, and stores validated values
 * into shared control variables used by the control loop.
 *
 * @param None.
 * @return void
 */
void ThreadReceiveRemoteControlFromNats()
{
    adcm::Log::Info() << "Control ThreadReceiveRemoteControlFromNats Start!!";
    adcm::BridgeControl_Provider bridgeControl_provider;
    bridgeControl_provider.init("Control/Control/PPort_bridge_control");

    Poco::JSON::Parser parser;
    RemoteControlCommand lastCommand;

    while (Control::continueExecution) {
        if (!EnsureNatsSubscribeConnection()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        std::string jsonStr;
        {
            std::unique_lock<std::mutex> lock(gNatsRemoteControlMutex);
            const bool hasEvent = gNatsRemoteControlCv.wait_for(lock, std::chrono::milliseconds(120), [] {
                return !gNatsRemoteControlJsonQueue.empty() || !Control::continueExecution;
            });

            if (!Control::continueExecution) {
                break;
            }
            if (!hasEvent) {
                continue;
            }

            while (!gNatsRemoteControlJsonQueue.empty()) {
                jsonStr = gNatsRemoteControlJsonQueue.front();
                gNatsRemoteControlJsonQueue.pop();
            }
        }

        try {
            Poco::Dynamic::Var result = parser.parse(jsonStr);
            Poco::JSON::Object::Ptr jsonObject = result.extract<Poco::JSON::Object::Ptr>();

            if (jsonObject == nullptr) {
                adcm::Log::Error() << "[NATS] Invalid payload object";
                continue;
            }

            RemoteControlCommand command;
            command.steering = ParseDoubleField(jsonObject, "steering", lastCommand.steering);
            command.acc = ParseDoubleField(jsonObject, "acc", lastCommand.acc);
            command.brake = ParseDoubleField(jsonObject, "break", lastCommand.brake);
            command.remoteEnable = ParseBooleanField(jsonObject, "remote_enable", lastCommand.remoteEnable);
            command.enable = ParseBooleanField(jsonObject, "enable", lastCommand.enable);
            command.turnSignal = ParseTurnSignalField(jsonObject, "turnsignal", lastCommand.turnSignal);
            command.timestamp = ParseTimestampField(jsonObject, lastCommand.timestamp);

            if (!jsonObject->has("steering") || !jsonObject->has("acc") || !jsonObject->has("break")
                || !jsonObject->has("turnsignal") || !jsonObject->has("remote_enable") || !jsonObject->has("enable")) {
                adcm::Log::Error() << "[NATS] Payload field missing, fallback defaults applied";
            }

            Control::RxSteer = command.steering;
            Control::RxAcc = command.acc;
            Control::RxBreak = command.brake;
            Control::RemoteEnable = static_cast<uint8_t>(command.remoteEnable);
            Control::RxEnable = static_cast<uint8_t>(command.enable);
            Control::RxTurn_signal = command.turnSignal;
            Control::gReceivedEvent_count_nats++;
            lastCommand = command;

            adcm::bridge_control_Objects bridgeControl;
            bridgeControl.Acc = command.acc;
            bridgeControl.Break = command.brake;
            bridgeControl.Steer = command.steering;
            bridgeControl.RemoteEnable = command.remoteEnable;
            bridgeControl.Enable = command.enable;
            bridgeControl.Turn_signal = command.turnSignal;
            bridgeControl_provider.send(bridgeControl);

            adcm::Log::Info() << "[From NATS] RxAcc: " << Control::RxAcc;
            adcm::Log::Info() << "[From NATS] RxBreak: " << Control::RxBreak;
            adcm::Log::Info() << "[From NATS] RxSteer: " << Control::RxSteer;
            adcm::Log::Info() << "[From NATS] RxRemoteEnable: " << Control::RemoteEnable;
            adcm::Log::Info() << "[From NATS] RxEnable: " << Control::RxEnable;
            adcm::Log::Info() << "[From NATS] RxTurn_signal: " << Control::RxTurn_signal;
            adcm::Log::Info() << "[From NATS] bridge_control sent to ara::com";
        } catch (const std::exception& e) {
            adcm::Log::Error() << "[NATS] Payload parse failed: " << e.what();
        }
    }

    gNatsSubscribeManager.reset();
}


/**
 * @brief Send control-bridge status to NATS periodically.
 *
 * Runs a dedicated publish loop that keeps a NATS publish connection alive,
 * builds the current control status payload, and publishes it to the
 * configured remote-status subject at a fixed interval.
 *
 * @param None.
 * @return void
 */
void ThreadSendControlBridgeToNats()
{
    adcm::Log::Info() << "Control ThreadSendControlBridgeToNats Start!!";

    while (Control::continueExecution) {
        if (!EnsureNatsPublishConnection()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            continue;
        }

        gNatsPublishManager->ClearJsonData();
        gNatsPublishManager->addJsonData("acc", KATECH::ClusterSpeed);
        gNatsPublishManager->addJsonData("break", 0);
        gNatsPublishManager->addJsonData("steer", KATECH::SteeringWheelAngle);
        gNatsPublishManager->addJsonData("remote_enable", KATECH::AutoDrivingMode);
        gNatsPublishManager->addJsonData("enable", KATECH::AutoDrivingMode);
        gNatsPublishManager->addJsonData("turn_signal", KATECH::Turn_SignalReport);
        gNatsPublishManager->addJsonData("timestamp",
            std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                               std::chrono::system_clock::now().time_since_epoch())
                               .count()));

        const natsStatus status = gNatsPublishManager->NatsPublishJson(GetNatsRemoteStatusSubject());
        if (status != NATS_OK) {
            adcm::Log::Error() << "[NATS] Publish failed: " << natsStatus_GetText(status);
            gNatsPublishManager.reset();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    gNatsPublishManager.reset();
}


/**
 * @brief Publish current control state to the bridge output port.
 *
 * Builds a bridge-control payload from the latest runtime state and sends it
 * at a fixed period.
 *
 * @param None.
 * @return void
 */
void ThreadSendControlBridge()
{
    adcm::Log::Info() << "Control ThreadSendControlBridge Start!!";

    adcm::ControlBridge_Provider controlBridge_provider;
    controlBridge_provider.init("Control/Control/PPort_control_bridge");

    while (Control::continueExecution) {
        adcm::Log::Verbose() << "[Control] ThreadSendControlBridge loop";

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        adcm::control_bridge_Objects controlBridge;

        controlBridge.Acc = KATECH::ClusterSpeed;
        controlBridge.Break = 0;
        controlBridge.Steer = KATECH::SteeringWheelAngle;
        controlBridge.Enable = KATECH::AutoDrivingMode;
        controlBridge.RemoteEnable = KATECH::AutoDrivingMode;
        controlBridge.Turn_signal = KATECH::Turn_SignalReport;

        controlBridge_provider.send(controlBridge);

        adcm::Log::Info() << "[To AXON] TxAcc: " << controlBridge.Acc;
        adcm::Log::Info() << "[To AXON] TxBreak: " << controlBridge.Break;
        adcm::Log::Info() << "[To AXON] TxSteer: " << controlBridge.Steer;
        adcm::Log::Info() << "[To AXON] TxRemoteEnable: " << controlBridge.RemoteEnable;
        adcm::Log::Info() << "[To AXON] TxEnable: " << controlBridge.Enable;
        adcm::Log::Info() << "[To AXON] TxTurn_signal: " << controlBridge.Turn_signal;

        adcm::Log::Verbose() << "[Control] send controlBridge Event";
    }

    gNatsPublishManager.reset();
}

/**
 * @brief Execute system initialization shell commands.
 *
 * Runs predefined startup commands required by the runtime environment and
 * logs per-command success or failure.
 *
 * @param None.
 * @return void
 */
void init_script()
{
    int result;
    std::vector<std::string> command_list
        = {"insmod /lib/modules/`uname -r`/extra/ipc-shm-dev.ko && insmod /lib/modules/`uname "
           "-r`/extra/katech-ipcf-can.ko"};
    INFO("Start Init script..");

    for (size_t i = 0; i < command_list.size(); i++) {
        result = system(command_list[i].c_str());

        if (result == 0) {
            INFO("Execute[%zu] : Success ( %s )", i, command_list[i].c_str());

        } else {
            FATAL("Execute[%zu] : Failed( %s, Error = %d )", i, command_list[i].c_str(), result);
        }
    }

    INFO("Complete Init script..");
}

using clk = std::chrono::steady_clock;
static std::shared_ptr<vsomeip::application> app;

struct Target
{
    uint16_t svc;
    uint16_t inst;
    std::string name;
};

std::vector<Target> all_targets = {{0x7A01, 0x0001, "guardian"},
    {0x7A01, 0x0002, "hdmap"},
    {0x7A01, 0x0003, "hmi"},
    {0x7A01, 0x0004, "localization"}};

struct Payload
{
    uint64_t seq;
    uint32_t cnt;
    uint8_t padding[56];
} __attribute__((packed));

static const vsomeip::method_t ECHO = 0x0001;
static std::mutex m;
static std::condition_variable cv;

static std::unordered_map<uint64_t, std::chrono::time_point<clk>> send_times;
static std::unordered_map<uint64_t, double> rtts;
static std::unordered_map<uint64_t, int> seq_to_target;
static uint64_t seq = 0;

static std::vector<uint32_t> per_target_cnt;
static std::unordered_map<uint32_t, int> svcinst_to_index;
static std::vector<std::vector<uint8_t>> recv_bitmap;
static int g_COUNT = 0;

/**
 * @brief Summarize missing packet indices from a bitmap.
 *
 * Converts a per-target receive bitmap into a compact range string such as
 * "3-5, 9, 12-15".
 *
 * @param bm_1_to_COUNT Bitmap indexed by packet count (index 0 unused).
 * @return Comma-separated missing index ranges.
 */
static std::string summarize_missing_ranges(const std::vector<uint8_t>& bm_1_to_COUNT)
{
    std::ostringstream oss;
    bool first = true;
    const int n = static_cast<int>(bm_1_to_COUNT.size()) - 1;
    int i = 1;

    while (i <= n) {
        while (i <= n && bm_1_to_COUNT[i])
            ++i;
        if (i > n)
            break;
        int start = i;
        while (i <= n && !bm_1_to_COUNT[i])
            ++i;
        int end = i - 1;

        if (!first)
            oss << ", ";
        first = false;
        if (start == end)
            oss << start;
        else
            oss << start << "-" << end;
    }
    return oss.str();
}

/**
 * @brief Handle incoming SOME/IP echo responses.
 *
 * Parses response payloads, computes request-response RTT for each sequence,
 * and updates per-target receive bitmaps.
 *
 * @param resp Received SOME/IP response message.
 * @return void
 */
void on_echo_response(const std::shared_ptr<vsomeip::message>& resp)
{
    auto now = clk::now();
    auto pl = resp->get_payload();
    if (!pl || pl->get_length() < sizeof(Payload))
        return;

    Payload p{};
    std::memcpy(&p, pl->get_data(), sizeof(Payload));

    {
        std::unique_lock<std::mutex> lk(m);
        auto it = send_times.find(p.seq);
        if (it == send_times.end())
            return;
        double delta_us = std::chrono::duration<double, std::micro>(now - it->second).count();
        rtts[p.seq] = delta_us;
        send_times.erase(it);
        cv.notify_all();
    }

    uint16_t svc = resp->get_service();
    uint16_t inst = resp->get_instance();
    uint32_t key = (uint32_t(svc) << 16) | inst;

    auto it_idx = svcinst_to_index.find(key);
    if (it_idx == svcinst_to_index.end())
        return;
    int t_idx = it_idx->second;

    if (p.cnt <= static_cast<uint32_t>(g_COUNT)) {
        recv_bitmap[t_idx][p.cnt] = 1;
    }
}

/**
 * @brief Send request packets to a specific SOME/IP target.
 *
 * Creates and transmits a sequence of request messages to one target at a
 * fixed interval while recording per-sequence send timestamps.
 *
 * @param t Target service/instance descriptor.
 * @param count Number of packets to send.
 * @param interval_ms Delay between packets in milliseconds.
 * @param target_index Index of target in the target table.
 * @return void
 */
void send_to_target(const Target& t, int count, int interval_ms, int target_index)
{
    for (int i = 0; i < count; i++) {
        auto req = vsomeip::runtime::get()->create_request();
        req->set_service(t.svc);
        req->set_instance(t.inst);
        req->set_method(ECHO);

        Payload p{};
        p.seq = ++seq;
        p.cnt = ++per_target_cnt[target_index];

        auto payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(reinterpret_cast<vsomeip::byte_t*>(&p), sizeof(p));
        req->set_payload(payload);

        {
            std::lock_guard<std::mutex> lk(m);
            send_times[p.seq] = clk::now();
            seq_to_target[p.seq] = target_index;
        }
        app->send(req);

        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
    }
}

/**
 * @brief Execute SOME/IP latency measurement workflow.
 *
 * Starts a temporary SOME/IP client, sends echo requests to configured
 * targets, and logs RTT and packet-loss statistics.
 *
 * @param None.
 * @return void
 */
void vsomeip_latency_test()
{
    app = vsomeip::runtime::get()->create_application("control_latency");
    app->init();

    for (auto& t : all_targets)
        app->register_message_handler(t.svc, t.inst, ECHO, on_echo_response);

    std::thread vsomeip_thread([] { app->start(); });

    const int COUNT = 10000;
    const int INTERVAL_MS = 10;
    g_COUNT = COUNT;

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    {
        std::lock_guard<std::mutex> lk(m);
        send_times.clear();
        rtts.clear();
        seq_to_target.clear();
    }

    per_target_cnt.assign(all_targets.size(), 0);

    svcinst_to_index.clear();
    for (int i = 0; i < (int)all_targets.size(); ++i) {
        uint32_t key = (uint32_t(all_targets[i].svc) << 16) | all_targets[i].inst;
        svcinst_to_index[key] = i;
    }

    recv_bitmap.assign(all_targets.size(), std::vector<uint8_t>(COUNT + 1, 0));

    std::vector<std::thread> threads;
    threads.reserve(all_targets.size());
    for (int i = 0; i < (int)all_targets.size(); i++)
        threads.emplace_back(send_to_target, all_targets[i], COUNT, INTERVAL_MS, i);

    for (auto& th : threads)
        th.join();

    {
        std::unique_lock<std::mutex> lk(m);
        cv.wait_for(lk, std::chrono::seconds(2), [] { return send_times.empty(); });
    }

    std::vector<double> data;
    data.reserve(rtts.size());
    for (auto& kv : rtts)
        data.push_back(kv.second);
    std::sort(data.begin(), data.end());

    if (data.empty()) {
        adcm::Log::Info() << "[Pro-Control] No data received";
    } else {
        double avg = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        adcm::Log::Info() << "[Pro-Control] samples=" << data.size() << " min=" << data.front() << "us"
                          << " avg=" << avg << "us"
                          << " max=" << data.back() << "us";
    }

    std::vector<int> replies_per_target(all_targets.size(), 0);
    for (auto& kv : rtts) {
        auto it = seq_to_target.find(kv.first);
        if (it != seq_to_target.end()) {
            int t_idx = it->second;
            if (0 <= t_idx && t_idx < (int)all_targets.size())
                replies_per_target[t_idx]++;
        }
    }
    for (int i = 0; i < (int)all_targets.size(); i++) {
        adcm::Log::Info() << "[Pro-Control] Target '" << all_targets[i].name << "' Reply " << replies_per_target[i]
                          << "/" << COUNT << " (" << (100.0 * replies_per_target[i] / COUNT) << "%)";
    }

    for (int i = 0; i < (int)all_targets.size(); ++i) {
        int lost_cnt = 0;
        for (int k = 1; k <= COUNT; ++k)
            if (!recv_bitmap[i][k])
                ++lost_cnt;

        if (lost_cnt == 0) {
            adcm::Log::Info() << "[Pro-Control] Target '" << all_targets[i].name << "' Loss: 0/" << COUNT
                              << " (no gaps)";
        } else {
            std::string gaps = summarize_missing_ranges(recv_bitmap[i]);
            adcm::Log::Error() << "[Pro-Control] Target '" << all_targets[i].name << "' Lost ranges (" << lost_cnt
                               << "/" << COUNT << "): " << gaps;
        }
    }

    app->stop();
    if (vsomeip_thread.joinable())
        vsomeip_thread.join();
}

/**
 * @brief Start Control application runtime and worker threads.
 *
 * Initializes ARA runtime resources, configures providers/subscribers,
 * launches control worker threads, and waits for graceful termination.
 *
 * @param argc Command line argument count.
 * @param argv Command line argument vector.
 * @return `EXIT_SUCCESS` on normal shutdown, otherwise `EXIT_FAILURE`.
 */
int main(int argc, char* argv[])
{
    std::vector<std::thread> thread_list;
    std::thread latency_thread;
    UNUSED(argc);
    UNUSED(argv);

    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    int ret = KATECH::katech_ipcf_init();
    if (ret < 0) {
        adcm::Log::Info() << "[MAIN] katech_ipcf_init failed, exiting!";
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (!Control::RegisterSigTermHandler()) {
        adcm::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    adcm::Log::Info() << "Control: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
        ara::com::e2exf::ConfigurationFormat::JSON,
        "./etc/e2e_statemachines.json",
        ara::com::e2exf::ConfigurationFormat::JSON);
    adcm::Log::Info() << "Control: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    adcm::Log::Info() << "Ok, let's produce some Control data...";
    {
        Control::collectFault_provider = std::make_shared<adcm::CollectFaultControl_Provider>();
        Control::collectFault_provider->init("Control/Control/PPort_collect_fault_control");
        Control::sendFault();
    }

    latency_thread = std::thread(vsomeip_latency_test);

    Control::drivingTrajectory_subscriber = std::make_shared<adcm::DrivingTrajectory_Subscriber>();
    Control::drivingTrajectory_subscriber->init("Control/Control/RPort_driving_trajectory");
    thread_list.push_back(std::thread(ThreadMonitor));
    thread_list.push_back(std::thread(ThreadSendCanData));
    // thread_list.push_back(std::thread(ThreadReceiveControlBridge));
    // thread_list.push_back(std::thread(ThreadSendControlBridge));
    thread_list.push_back(std::thread(ThreadReceiveRemoteControlFromNats));
    thread_list.push_back(std::thread(ThreadSendControlBridgeToNats));

    adcm::Log::Info() << "Thread join";

    for (int i = 0; i < static_cast<int>(thread_list.size()); i++) {
        thread_list[i].join();
    }

    if (latency_thread.joinable()) {
        latency_thread.join();
    }

    adcm::Log::Info() << "done.";

    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
