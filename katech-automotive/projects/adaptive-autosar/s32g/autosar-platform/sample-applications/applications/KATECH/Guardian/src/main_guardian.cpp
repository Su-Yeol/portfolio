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

#include "report_fault_provider.h"
#include "collect_fault_control_subscriber.h"
#include "collect_fault_hdmap_subscriber.h"
#include "collect_fault_hmi_subscriber.h"
#include "collect_fault_localization_subscriber.h"
#include "collect_fault_perception_camera_subscriber.h"
#include "collect_fault_perception_lidar_subscriber.h"
#include "collect_fault_planning_subscriber.h"
#include "collect_fault_routing_subscriber.h"
#include "collect_fault_v2x_subscriber.h"

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
std::uniform_int_distribution<std::uint8_t> m_ud_0_46(0, 46);

namespace Guardian
{

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_bool report_fault_flag{false};

std::atomic_uint gReceivedEvent_count_collect_fault_control{0};
std::atomic_uint gReceivedEvent_count_collect_fault_hdmap{0};
std::atomic_uint gReceivedEvent_count_collect_fault_hmi{0};
std::atomic_uint gReceivedEvent_count_collect_fault_localization{0};
std::atomic_uint gReceivedEvent_count_collect_fault_perception_camera{0};
std::atomic_uint gReceivedEvent_count_collect_fault_perception_lidar{0};
std::atomic_uint gReceivedEvent_count_collect_fault_planning{0};
std::atomic_uint gReceivedEvent_count_collect_fault_routing{0};
std::atomic_uint gReceivedEvent_count_collect_fault_v2x{0};

std::atomic_uint64_t gFaultStatus{0};
adcm::report_fault_Objects reportFault;

void thread_check_Control();
void thread_check_Hdmap();
void thread_check_Hmi();
void thread_check_Localization();
void thread_check_PerceptionCamera();
void thread_check_PerceptionLidar();
void thread_check_Planning();
void thread_check_Routing();
void thread_check_V2X();

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

void thread_check_Control()
{
    adcm::CollectFaultControl_Subscriber collectFaultControl_subscriber;
    collectFaultControl_subscriber.init("Guardian/Guardian/RPort_collect_fault_control");

    while(true) {
        bool collectFaultControl_rxEvent = collectFaultControl_subscriber.waitEvent(1200);

        if(collectFaultControl_rxEvent) {
            // INFO("Collect Fault Control Event");

            while(!collectFaultControl_subscriber.isEventQueueEmpty()) {
                auto data = collectFaultControl_subscriber.getEvent();
                gReceivedEvent_count_collect_fault_control++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_Control;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_Hdmap()
{
    adcm::CollectFaultHdmap_Subscriber collectFaultHdmap_subscriber;
    collectFaultHdmap_subscriber.init("Guardian/Guardian/RPort_collect_fault_hdmap");

    while(true) {
        bool rxEvent = collectFaultHdmap_subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Hdmap Event");

            while(!collectFaultHdmap_subscriber.isEventQueueEmpty()) {
                auto data = collectFaultHdmap_subscriber.getEvent();
                gReceivedEvent_count_collect_fault_hdmap++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_Hdmap;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_Hmi()
{
    adcm::CollectFaultHmi_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_hmi");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Hmi Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_hmi++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_Hmi;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_Localization()
{
    adcm::CollectFaultLocalization_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_localization");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Localization Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_localization++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : %d", AP);
                auto fault_list = data->Fault_Localization;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }

                } else {
                    // INFO("All Green");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_PerceptionCamera()
{
    adcm::CollectFaultPerceptionCamera_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_perception_camera");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Perception_Camera Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_perception_camera++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_PerceptionCamera;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_PerceptionLidar()
{
    adcm::CollectFaultPerceptionLidar_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_perception_lidar");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Perception_Lidar Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_perception_lidar++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_PerceptionLidar;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}


void thread_check_Planning()
{
    adcm::CollectFaultPlanning_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_planning");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Planning Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_planning++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_Planning;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("Vector empty!!! ");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}

void thread_check_Routing()
{
    adcm::CollectFaultRouting_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_routing");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault Routing Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_routing++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_Routing;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("All green");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}


void thread_check_V2X()
{
    adcm::CollectFaultV2X_Subscriber subscriber;
    subscriber.init("Guardian/Guardian/RPort_collect_fault_v2x");

    while(true) {
        bool rxEvent = subscriber.waitEvent(1200);

        if(rxEvent) {
            // INFO("Collect Fault V2X Event");

            while(!subscriber.isEventQueueEmpty()) {
                auto data = subscriber.getEvent();
                gReceivedEvent_count_collect_fault_v2x++;
                // INFO("Collect Fault received");
                auto AP = data->AP;
                // INFO("AP num : ", AP);
                auto fault_list = data->Fault_V2X;

                if(!fault_list.empty()) {
                    report_fault_flag = true;
                    // INFO("=== Fault Values ===");

                    for(int i = 0; i < fault_list.size();i++) {
                        // todo something
                        // INFO("value = %d", fault_list[i]);
                        reportFault.FaultList.push_back(fault_list[i]);
                    }
                } else {
                    // INFO("All green");
                }
            }

        } else {
            // INFO("Time Out");
        }
    }
}


void thread_check_Guardian()
{
    while(true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // do something(add fault value)..
        reportFault.FaultList.push_back(20);
        reportFault.FaultList.push_back(21);
        reportFault.FaultList.push_back(22);
    }
}


void ThreadSendReportFault()
{
    adcm::Log::Info() << "Guardian ThreadReceiveEvent Start!!";
    adcm::ReportFault_Provider reportFault_provider;
    adcm::Log::Info() << "Guardian .init()";
    reportFault_provider.init("Guardian/Guardian/PPort_report_fault");
    //report_fault
    
    std::uint8_t fault_value;
    reportFault.AP = 1;

    while(continueExecution) {
        adcm::Log::Verbose() << "[Guardian] Application loop";
        reportFault.FaultList.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        std::sort(reportFault.FaultList.begin(), reportFault.FaultList.end());
        reportFault.FaultList.erase(unique(reportFault.FaultList.begin(), reportFault.FaultList.end()), reportFault.FaultList.end());
        reportFault_provider.send(reportFault);
        report_fault_flag = false;
        adcm::Log::Verbose() << "[Guardian] send reportFault Event ";
    }
}

void ThreadMonitor()
{
    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(gReceivedEvent_count_collect_fault_control != 0) {
            adcm::Log::Info() << "collect_fault_control Received count = " << gReceivedEvent_count_collect_fault_control;
            gReceivedEvent_count_collect_fault_control = 0;

        } else {
            adcm::Log::Info() << "collect_fault_control event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_hdmap != 0) {
            adcm::Log::Info() << "collect_fault_hdmap Received count = " << gReceivedEvent_count_collect_fault_hdmap;
            gReceivedEvent_count_collect_fault_hdmap = 0;

        } else {
            adcm::Log::Info() << "collect_fault_hdmap event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_hmi != 0) {
            adcm::Log::Info() << "collect_fault_hmi Received count = " << gReceivedEvent_count_collect_fault_hmi;
            gReceivedEvent_count_collect_fault_hmi = 0;

        } else {
            adcm::Log::Info() << "collect_fault_hmi event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_localization != 0) {
            adcm::Log::Info() << "collect_fault_localization Received count = " << gReceivedEvent_count_collect_fault_localization;
            gReceivedEvent_count_collect_fault_localization = 0;

        } else {
            adcm::Log::Info() << "collect_fault_localization event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_perception_camera != 0) {
            adcm::Log::Info() << "collect_fault_perception_camera Received count = " << gReceivedEvent_count_collect_fault_perception_camera;
            gReceivedEvent_count_collect_fault_perception_camera = 0;

        } else {
            adcm::Log::Info() << "collect_fault_perception_camera event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_perception_lidar != 0) {
            adcm::Log::Info() << "collect_fault_perception_lidar Received count = " << gReceivedEvent_count_collect_fault_perception_lidar;
            gReceivedEvent_count_collect_fault_perception_lidar = 0;

        } else {
            adcm::Log::Info() << "collect_fault_perception_lidar event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_planning != 0) {
            adcm::Log::Info() << "collect_fault_planning Received count = " << gReceivedEvent_count_collect_fault_planning;
            gReceivedEvent_count_collect_fault_planning = 0;

        } else {
            adcm::Log::Info() << "collect_fault_planning event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_routing != 0) {
            adcm::Log::Info() << "collect_fault_routing Received count = " << gReceivedEvent_count_collect_fault_routing;
            gReceivedEvent_count_collect_fault_routing = 0;

        } else {
            adcm::Log::Info() << "collect_fault_routing event timeout!!!";
        }

        if(gReceivedEvent_count_collect_fault_v2x != 0) {
            adcm::Log::Info() << "collect_fault_v2x Received count = " << gReceivedEvent_count_collect_fault_v2x;
            gReceivedEvent_count_collect_fault_v2x = 0;

        } else {
            adcm::Log::Info() << "collect_fault_v2x event timeout!!!";
        }
    }
}


}  // namespace


// ========================================= Process RTT =========================================
static std::shared_ptr<vsomeip::application> guardian_app;
static const vsomeip::service_t SVC = 0x7A01;
static const vsomeip::instance_t INST = 0x0001; // guardian
static const vsomeip::method_t ECHO = 0x0001;

#pragma pack(push, 1)
struct Payload {
    uint64_t seq;   // 전체 송신/응답 매칭
    uint32_t cnt;   // 페이로드 카운터
    uint8_t  padding[56];
};
#pragma pack(pop)

struct RecvStats {
    uint64_t recv_total = 0;    // 수신 개수
    uint32_t last_cnt   = 0;    // 마지막 카운터
    uint64_t lost_total = 0;    // 손실
};

static std::mutex g_mu;
static std::unordered_map<vsomeip::client_t, RecvStats> g_stats;

static inline void log_stats_periodically(vsomeip::client_t cid, const RecvStats& s) {
    if (s.recv_total % 1000 == 0) {
        adcm::Log::Info() << "[Pro-Guardian] recv=" << s.recv_total
                          << " last_cnt=" << s.last_cnt
                          << " lost=" << s.lost_total;
    }
}

/* SOME/IP가 ECHO 요청을 받으면 runtime이 자동으로 호출하는 콜백 함수 */
void on_echo_request(const std::shared_ptr<vsomeip::message> &req) {
    auto resp = vsomeip::runtime::get()->create_response(req);  // 메시지 객체 생성

    Payload p{};
    auto pl = req->get_payload();
    if (pl && pl->get_length() >= sizeof(Payload)) {
        std::memcpy(&p, pl->get_data(), sizeof(Payload));
    }

    {
        std::lock_guard<std::mutex> lk(g_mu);
        auto cid = req->get_client();   // 요청을 보낸 vsomeip client id를 확인
        auto &st = g_stats[cid];        // 해당 클라이언트의 stats 객체 가져옴
        st.recv_total++;                // 수신 count 증가

        if (p.cnt > st.last_cnt + 1) {
            st.lost_total += (p.cnt - (st.last_cnt + 1)); // 중간 구간 유실 추정
        }
        if (p.cnt > st.last_cnt)
            st.last_cnt = p.cnt;

        log_stats_periodically(cid, st);
    }

    if (pl && pl->get_length() > 0)
        adcm::Log::Verbose() << "[Pro-Guardian] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    guardian_app->send(resp);
}

void vsomeip_server_thread() {
    guardian_app = vsomeip::runtime::get()->create_application("guardian_echo");
    guardian_app->init();
    guardian_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    guardian_app->offer_service(SVC, INST); // 이 어플리케이션이 서비스 서버 역할로 등록
    guardian_app->start();
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

    if(!Guardian::RegisterSigTermHandler()) {
        adcm::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    adcm::Log::Info() << "Guardian: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    adcm::Log::Info() << "Guardian: e2e configuration " << (success ? "succeeded" : "failed");
#endif 
    adcm::Log::Info() << "Ok, let's produce some Guardian data...";

    thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT
    
    thread_list.push_back(std::thread(Guardian::thread_check_Control));
    thread_list.push_back(std::thread(Guardian::thread_check_Hdmap));
    thread_list.push_back(std::thread(Guardian::thread_check_Hmi));
    thread_list.push_back(std::thread(Guardian::thread_check_Localization));
    thread_list.push_back(std::thread(Guardian::thread_check_PerceptionCamera));
    thread_list.push_back(std::thread(Guardian::thread_check_PerceptionLidar));
    thread_list.push_back(std::thread(Guardian::thread_check_Planning));
    thread_list.push_back(std::thread(Guardian::thread_check_Routing));
    thread_list.push_back(std::thread(Guardian::thread_check_V2X));
    thread_list.push_back(std::thread(Guardian::thread_check_Guardian));
    thread_list.push_back(std::thread(Guardian::ThreadSendReportFault));
    thread_list.push_back(std::thread(Guardian::ThreadMonitor));
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