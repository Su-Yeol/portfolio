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

#include "collect_fault_routing_provider.h"
#include "routing_result_provider.h"
#include "local_map_subscriber.h"
#include "matching_info_subscriber.h"
#include "routing_search_subscriber.h"

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
std::uniform_int_distribution<std::int8_t> m_ud_0_1(0, 1);
std::uniform_int_distribution<std::int32_t> m_ud_1000_1000(-1000, 1000);
std::uniform_int_distribution<std::int64_t> m_ud_1000_10000(-1000, 10000);

namespace Routing
{
std::shared_ptr<adcm::CollectFaultRouting_Provider> collectFault_provider;

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};
std::atomic_uint gEventReceived_Count_matching_info{0};
std::atomic_uint gEventReceived_Count_routing_search{0};
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
    static adcm::collect_fault_routing_Objects gCollectFault;
    gCollectFault.AP = 0;
    gCollectFault.Fault_Routing.clear();
    std::uint64_t temp = getFaultStatus();

    for(int i = 0; i < 64; i++) {
        if(temp & (static_cast<std::uint64_t>(1) << i)) {
            gCollectFault.Fault_Routing.push_back(i);
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



void ThreadReceiveMatchingInfo()
{
    adcm::Log::Info() << "Routing ThreadReceiveMatchingInfo Start!!";
    adcm::MatchingInfo_Subscriber matchingInfo_subscriber;
    matchingInfo_subscriber.init("Routing/Routing/RPort_matching_info");
    
    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Routing] ThreadReceiveMatchingInfo loop";
        bool matchingInfo_rxEvent = matchingInfo_subscriber.waitEvent(120);

        if(matchingInfo_rxEvent) {
            adcm::Log::Verbose() << "matchingInfo_rxEvent";

            while(!matchingInfo_subscriber.isEventQueueEmpty()) {
                auto data = matchingInfo_subscriber.getEvent();
                gEventReceived_Count_matching_info++;
                adcm::Log::Verbose() << "[EVENT] Routing Matching Info received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Routing matching_info Event Callback";
                    auto dStartX = data->dStartX;
                    auto dStartY = data->dStartY;
                    auto dEndX = data->dEndX;
                    auto dEndY = data->dEndY;
                    auto nS_ClosestRoadID = data->nS_ClosestRoadID;
                    auto nE_ClosestRoadID = data->nE_ClosestRoadID;
                    auto nS_ClosestLaneID = data->nS_ClosestLaneID;
                    auto nE_ClosestLaneID = data->nE_ClosestLaneID;
                    auto roadIdVector = data->roadIdVector;
                    adcm::Log::Verbose() << "MATCHING_INFO.dStartX : " << dStartX;
                    adcm::Log::Verbose() << "MATCHING_INFO.dStartY : " << dStartY;
                    adcm::Log::Verbose() << "MATCHING_INFO.dEndX : " << dEndX;
                    adcm::Log::Verbose() << "MATCHING_INFO.dEndY : " << dEndY;
                    adcm::Log::Verbose() << "MATCHING_INFO.nS_ClosestRoadID : " << nS_ClosestRoadID;
                    adcm::Log::Verbose() << "MATCHING_INFO.nE_ClosestRoadID : " << nE_ClosestRoadID;
                    adcm::Log::Verbose() << "MATCHING_INFO.nS_ClosestLaneID : " << nS_ClosestLaneID;
                    adcm::Log::Verbose() << "MATCHING_INFO.nE_ClosestLaneID : " << nE_ClosestLaneID;

                    if(!roadIdVector.empty()) {
                        adcm::Log::Verbose() << "=== roadId ===";
                        for(auto itr = roadIdVector.begin(); itr != roadIdVector.end(); ++itr) {
                            adcm::Log::Verbose() << *itr;
                        }
                    } else {
                        adcm::Log::Verbose() << "roadId Vector empty!!! ";
                    }
                }
            }
        }
    }
}

void ThreadReceiveRoutingSearch()
{
    adcm::Log::Info() << "Routing ThreadReceiveRoutingSearch Start!!";
    adcm::RoutingSearch_Subscriber routingSearch_subscriber;
    routingSearch_subscriber.init("Routing/Routing/RPort_routing_search");
    
    while(continueExecution) {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Routing] ThreadReceiveRoutingSearch loop";
        bool routingSearch_rxEvent = routingSearch_subscriber.waitEvent(120); // wait event

        if(routingSearch_rxEvent) {
            adcm::Log::Verbose() << "routingSearch_rxEvent";

            while(!routingSearch_subscriber.isEventQueueEmpty()) {
                auto data = routingSearch_subscriber.getEvent();
                gEventReceived_Count_routing_search++;
                adcm::Log::Verbose() << "[EVENT] Routing Routing Search received";

                if(data != nullptr) {
                    adcm::Log::Verbose() << "[Enter] Routing routing_search Event Callback";
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

                        if(preset == 1){
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

void ThreadSendRoutingResult()
{
    adcm::Log::Info() << "Routing ThreadSendRoutingResult Start!!";

    adcm::RoutingResult_Provider routingResult_provider;
    routingResult_provider.init("Routing/Routing/PPort_routing_result");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        //write routing_result
        adcm::routing_result_Objects routingResult;

        //LINK_LIST
        adcm::LINK_LIST linkList;
        linkList.nBest = m_ud_0_1(m_rand_eng);
        linkList.nLinkID = m_ud_1000_10000(m_rand_eng);

        //ROAD_LIST
        adcm::ROAD_LIST roadList;
        roadList.nRoadID = m_ud_1000_10000(m_rand_eng);
        roadList.vLinks.clear();
        roadList.vLinks.push_back(linkList);

        //ROUTE_RESULT
        routingResult.vRoute.clear();
        routingResult.vRoute.push_back(roadList);

        routingResult_provider.send(routingResult);
        adcm::Log::Verbose() << "[Routing] send routingResult Event ";
    }
}

void ThreadSendCollectFault()
{
    INFO("ThreadSendCollectFault Start!!");

    while(continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // set fault
        setFault(56);
        setFault(57);
        sendFault();
        INFO("Set fault");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        // clear fault
        clearFault(56);
        clearFault(57);
        sendFault();
        INFO("Clear fault");
    }
}

void ThreadMethodCallTest()
{
    adcm::LocalMap_Subscriber localMap_subscriber;
    localMap_subscriber.init("Routing/Routing/RPort_local_map");

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

        if(serviceFlag == "On"){
            adcm::Log::Verbose() << "[Routing][localMap_subscriber] GetMapData Method Call Test";
            gridID = m_ud_0_10000(m_rand_eng);
            cellID = m_ud_0_10000(m_rand_eng);
            deadLine = 1000;    //millisecond

            auto mapdataPtr = localMap_subscriber.MapData(gridID, cellID, deadLine);

            if(mapdataPtr != NULL){
                auto GridID = mapdataPtr->GridID;
                auto CellID = mapdataPtr->CellID;
                adcm::Log::Verbose() << "mapdata.GridID : " << GridID;
                adcm::Log::Verbose() << "mapdata.CellID : " << CellID;
            }else{
                adcm::Log::Info() << "mapdata is NULL ...";
            }
        }else{
            adcm::Log::Info() << "Hdmap Process is not running...";
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

            if(gEventReceived_Count_matching_info != 0) {
                adcm::Log::Info() << "matching_info Received count = " << gEventReceived_Count_matching_info;
                gEventReceived_Count_matching_info = 0;

            } else {
                adcm::Log::Info() << "matching_info event timeout!!!";
            }

            if(gEventReceived_Count_routing_search != 0) {
                adcm::Log::Info() << "routing_search Received count = " << gEventReceived_Count_routing_search;
                gEventReceived_Count_routing_search = 0;

            } else {
                adcm::Log::Info() << "routing_search event timeout!!!";
            }            
        }
    }
}

}  // namespace


// ========================================= Process RTT =========================================
// static std::shared_ptr<vsomeip::application> route_app;
// static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
// static const vsomeip::instance_t INST = 0x0008;  // Instance ID
// static const vsomeip::method_t ECHO   = 0x0001;  // Echo Method ID

// void on_echo_request(const std::shared_ptr<vsomeip::message> &req) {
//     auto resp = vsomeip::runtime::get()->create_response(req);
//     resp->set_payload(req->get_payload());

//     auto pl = req->get_payload();
//     if (pl && pl->get_length() > 0) {
//         adcm::Log::Info() << "[Pro-Routing] ECHO Request received. Payload size = " 
//                           << pl->get_length() << " bytes";
//     } else {
//         adcm::Log::Info() << "[Pro-Routing] ECHO Request received (empty payload)";
//     }

//     route_app->send(resp);
// }

// void vsomeip_server_thread() {
//     route_app = vsomeip::runtime::get()->create_application("route_echo");
//     route_app->init();
//     route_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
//     route_app->offer_service(SVC, INST);
//     route_app->start();
// }

static std::shared_ptr<vsomeip::application> route_app;
static const vsomeip::service_t SVC  = 0x7A01;   // HDMAP 고유 Service ID
static const vsomeip::instance_t INST = 0x0008;  // Instance ID
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
        adcm::Log::Info() << "[Pro-Routing] client=0x" << std::hex << cid << std::dec
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
        adcm::Log::Verbose() << "[Pro-Routing] ECHO req: len=" << pl->get_length()
                             << " seq=" << p.seq << " cnt=" << p.cnt;

    resp->set_payload(pl);
    route_app->send(resp);
}

void vsomeip_server_thread() {
    route_app = vsomeip::runtime::get()->create_application("route_echo");
    route_app->init();
    route_app->register_message_handler(SVC, INST, ECHO, on_echo_request);
    route_app->offer_service(SVC, INST);
    route_app->start();
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

    if(!Routing::RegisterSigTermHandler()) {
        adcm::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    adcm::Log::Info() << "Routing: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    adcm::Log::Info() << "Routing: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    adcm::Log::Info() << "Ok, let's produce some Routing data...";
    
    {
        Routing::collectFault_provider = std::make_shared<adcm::CollectFaultRouting_Provider>();
        Routing::collectFault_provider->init("Routing/Routing/PPort_collect_fault_routing");
        Routing::sendFault();
    }

    // thread_list.push_back(std::thread(vsomeip_server_thread)); // Process RTT

    thread_list.push_back(std::thread(Routing::ThreadReceiveMatchingInfo));
    thread_list.push_back(std::thread(Routing::ThreadReceiveRoutingSearch));
    thread_list.push_back(std::thread(Routing::ThreadSendRoutingResult));
    // thread_list.push_back(std::thread(Routing::ThreadMethodCallTest));
    thread_list.push_back(std::thread(Routing::ThreadSendCollectFault));
    thread_list.push_back(std::thread(Routing::ThreadMonitor));

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
