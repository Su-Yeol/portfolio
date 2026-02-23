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

#include "collect_fault_perception_camera_provider.h"
#include "lane_detection_provider.h"
#include "traffic_light_provider.h"
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
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::uint8_t> m_ud_0_3(0, 3);


namespace PerceptionCamera
{

std::shared_ptr<katech::CollectFaultPerceptionCamera_Provider> collectFault_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
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
    static katech::collect_fault_perception_camera_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_PerceptionCamera.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_PerceptionCamera.push_back(i);
        }
    }

    collectFault_provider->send(gCollectFault);
    katech::Log::Verbose() << "[PerceptionCamera] send collectFault Event ";
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

void ThreadReceiveV2XData()
{
    katech::Log::Info() << "Perception_Camera Thread Start!!";
    katech::V2XData_Subscriber v2xData_subscriber;
    katech::Log::Info() << "Perception_Camera .init()";
    v2xData_subscriber.init("Perception_Camera/Perception_Camera/RPort_v2x_data");
    
    while(continueExecution) {
        gMainthread_Loopcount++;
        katech::Log::Verbose() << "[Perception_Camera] Application loop";
        bool v2xData_rxEvent = v2xData_subscriber.waitEvent(120);

        if(v2xData_rxEvent) {
            while(!v2xData_subscriber.isEventQueueEmpty()) {
                auto data = v2xData_subscriber.getEvent();
                gEventReceived_Count_v2x_data++;
                katech::Log::Verbose() << "[EVENT] Perception_Camera V2X Data received";

                if(data != nullptr) {
                    katech::Log::Verbose() << "[Enter] Perception_Camera v2x_data Event Callback";

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

void ThreadSendLaneDetection()
{
    katech::Log::Info() << "Perception_Camera ThreadSendLaneDetection";
    katech::LaneDetection_Provider laneDetection_provider;
    laneDetection_provider.init("Perception_Camera/Perception_Camera/PPort_lane_detection");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // write lane_detection
        katech::lane_detection_Objects laneDetection;
        
        katech::Vector2D temp;
        laneDetection.LaneLineDetection_LeftVector.clear();
        laneDetection.LaneLineDetection_RightVector.clear();
        temp.X = m_ud_10000_10000(m_rand_eng);
        temp.Y = m_ud_10000_10000(m_rand_eng);
        laneDetection.LaneLineDetection_LeftVector.push_back(temp);
        temp.X = m_ud_10000_10000(m_rand_eng);
        temp.Y = m_ud_10000_10000(m_rand_eng);
        laneDetection.LaneLineDetection_LeftVector.push_back(temp);
        temp.X = m_ud_10000_10000(m_rand_eng);
        temp.Y = m_ud_10000_10000(m_rand_eng);
        laneDetection.LaneLineDetection_RightVector.push_back(temp);
        temp.X = m_ud_10000_10000(m_rand_eng);
        temp.Y = m_ud_10000_10000(m_rand_eng);
        laneDetection.LaneLineDetection_RightVector.push_back(temp);
        
        // tx
        laneDetection_provider.send(laneDetection);
        katech::Log::Verbose() << "[Perception_Camera] send laneDetection Event ";
    }
}

void ThreadSendTrafficLight()
{
    katech::Log::Info() << "Perception_Camera ThreadSendTrafficLight";
    katech::TrafficLight_Provider trafficLight_provider;
    trafficLight_provider.init("Perception_Camera/Perception_Camera/PPort_traffic_light");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //write traffic_light
        katech::traffic_light_Objects trafficLight;
        katech::Perception_trafficlight trafficlight;
        trafficLight.PerceptionTrafficlightVector.clear();

        trafficlight.Id = "Perception_Camera";
        trafficlight.Color = katech::trafficLightColor::YELLOW;
        trafficlight.Position.Xmin = m_ud_10000_10000(m_rand_eng);
        trafficlight.Position.Ymin = m_ud_10000_10000(m_rand_eng);
        trafficlight.Position.Xmax = m_ud_10000_10000(m_rand_eng);
        trafficlight.Position.Ymax = m_ud_10000_10000(m_rand_eng);
        trafficlight.CameraID = m_ud_0_10000(m_rand_eng);

        trafficLight.PerceptionTrafficlightVector.push_back(trafficlight);
        trafficLight.Camera_Status = m_ud_0_3(m_rand_eng) % 2 == 0 ? true : false;
        trafficLight.TL_CAM_Status = m_ud_0_3(m_rand_eng) % 2 == 0 ? true : false;

        trafficLight_provider.send(trafficLight);
        katech::Log::Verbose() << "[Perception_Camera] send trafficLight Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(32);
        setFault(34);
        setFault(35);
        setFault(36);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(32);
        clearFault(34);
        clearFault(35);
        clearFault(36);
        sendFault();
        INFO("Clear fault");
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

            if(gEventReceived_Count_v2x_data != 0) {
                katech::Log::Info() << "v2x_data Received count = " << gEventReceived_Count_v2x_data;
                gEventReceived_Count_v2x_data = 0;

            } else {
                katech::Log::Info() << "v2x_data event timeout!!!";
            }
        }
    }
}

}  // namespace


// ========================================= Process RTT =========================================
// static std::shared_ptr<vsomeip::application> cam_app;
// static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
// static const vsomeip::instance_t INST = 0x0005;  // Instance ID
// static const vsomeip::method_t ECHO   = 0x0001;  // Echo Method ID

// void on_echo_request(const std::shared_ptr<vsomeip::message> &req) {
//     auto resp = vsomeip::runtime::get()->create_response(req);
//     resp->set_payload(req->get_payload());

//     auto pl = req->get_payload();
//     if (pl && pl->get_length() > 0) {
//         katech::Log::Info() << "[Pro-Camera] ECHO Request received. Payload size = " 
//                           << pl->get_length() << " bytes";
//     } else {
//         katech::Log::Info() << "[Pro-Camera] ECHO Request received (empty payload)";
//     }

//     cam_app->send(resp);
// }

// void vsomeip_server_thread() {
//     cam_app = vsomeip::runtime::get()->create_application("cam_echo");
//     cam_app->init();
//     cam_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
//     cam_app->offer_service(SVC, INST);
//     cam_app->start();
// }

static std::shared_ptr<vsomeip::application> cam_app;
static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
static const vsomeip::instance_t INST = 0x0005;  // Instance ID
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
        katech::Log::Info() << "[Pro-Camera] client=0x" << std::hex << cid << std::dec
                          << " recv=" << s.recv_total
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
        katech::Log::Verbose() << "[Pro-Camera] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    cam_app->send(resp);
}

void vsomeip_server_thread() {
    cam_app = vsomeip::runtime::get()->create_application("cam_echo");
    cam_app->init();
    cam_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    cam_app->offer_service(SVC, INST);
    cam_app->start();
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

    if(!PerceptionCamera::RegisterSigTermHandler()) {
        katech::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    katech::Log::Info() << "Perception_Camera: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    katech::Log::Info() << "Perception_Camera: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    katech::Log::Info() << "Ok, let's produce some Perception_Camera data...";

    {
        PerceptionCamera::collectFault_provider = std::make_shared<katech::CollectFaultPerceptionCamera_Provider>();
        PerceptionCamera::collectFault_provider->init("Perception_Camera/Perception_Camera/PPort_collect_fault_perception_camera");
        PerceptionCamera::sendFault();
    }

    // thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(PerceptionCamera::ThreadReceiveV2XData));
    thread_list.push_back(std::thread(PerceptionCamera::ThreadSendLaneDetection));
    thread_list.push_back(std::thread(PerceptionCamera::ThreadSendTrafficLight));
    thread_list.push_back(std::thread(PerceptionCamera::ThreadSendCollectFault));
    thread_list.push_back(std::thread(PerceptionCamera::ThreadMonitor));

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
