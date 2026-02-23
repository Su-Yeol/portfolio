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

#include "bridge_control_provider.h"
#include "control_bridge_subscriber.h"

std::random_device m_rd;
std::default_random_engine m_rand_eng(m_rd());
std::uniform_real_distribution<double> m_ud_10000_10000(-10000, 10000);
std::uniform_int_distribution<std::uint32_t> m_ud_0_10000(0, 10000);
std::uniform_int_distribution<std::uint8_t> m_ud_0_4(0, 4);

std::atomic_bool continueExecution{true};
std::atomic_uint gReceivedEvent_count_control_bridge{0};
std::atomic_uint gReceivedEvent_count_nats{0};
std::atomic_uint gMainthread_Loopcount{0};

namespace Katech_Bridge
{

// Atomic flag for exit after SIGTERM caught
// std::atomic_bool continueExecution{true};
// std::atomic_uint gReceivedEvent_count_control_bridge{0};
// std::atomic_uint gMainthread_Loopcount{0};

void SigTermHandler(int signal)
{
    if(signal == SIGTERM)
    {
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
    if(sigaction(SIGTERM, &sa, NULL) == -1)
    {
        // Could not register a SIGTERM signal handler
        return false;
    }

    return true;
}

void ThreadReceiveControlBridge()
{
    adcm::Log::Info() << "Katech_Bridge ThreadReceiveControlBridge Start!!";
    adcm::ControlBridge_Subscriber controlBridge_subscriber;
    controlBridge_subscriber.init("Katech_Bridge/Katech_Bridge/RPort_control_bridge");

    while(continueExecution)
    {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Katech_Bridge] ThreadReceiveControlBridge loop";
        bool controlBridge_rxEvent = controlBridge_subscriber.waitEvent(120); // wait event

        if(controlBridge_rxEvent)
        {
            adcm::Log::Verbose() << "[EVENT] Katech_Bridge Control Bridge received";

            while(!controlBridge_subscriber.isEventQueueEmpty())
            {
                auto data = controlBridge_subscriber.getEvent();
                gReceivedEvent_count_control_bridge++;
                auto Acc = data->Acc;
                auto Break = data->Break;
                auto Steer = data->Steer;
                auto Enable = data->Enable;
                auto Turn_signal = data->Turn_signal;
                adcm::Log::Verbose() << "Acc : " << Acc;
                adcm::Log::Verbose() << "Break : " << Break;
                adcm::Log::Verbose() << "Steer : " << Steer;

                switch(Turn_signal)
                {
                    case 'L':
                        adcm::Log::Verbose() << "Turn_signal : L";
                        break;

                    case 'R':
                        adcm::Log::Verbose() << "Turn_signal : R";
                        break;

                    case 'E':
                        adcm::Log::Verbose() << "Turn_signal : E";
                        break;

                    case 'O':
                        adcm::Log::Verbose() << "Turn_signal : O";
                        break;
                }

                if(Enable)
                {
                    adcm::Log::Verbose() << "Enable is true";
                }
                else
                {
                    adcm::Log::Verbose() << "Enable is false";
                }
            }
        }
    }
}

void ThreadSendBridgeControl()
{
    adcm::Log::Info() << "Control ThreadSendCanEtcData Start!!";
    adcm::BridgeControl_Provider bridgeControl_provider;
    bridgeControl_provider.init("Katech_Bridge/Katech_Bridge/PPort_bridge_control");

    while(continueExecution)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //write brigeControl data
        adcm::bridge_control_Objects bridgeControl;
        bridgeControl.Acc = m_ud_10000_10000(m_rand_eng);
        bridgeControl.Break = m_ud_10000_10000(m_rand_eng);
        bridgeControl.Steer = m_ud_10000_10000(m_rand_eng);
        bridgeControl.Enable = m_ud_0_4(m_rand_eng) % 2 == 0 ? true : false;
        bridgeControl.Turn_signal = m_ud_0_10000(m_rand_eng) % 2 == 0 ? 'L' :
                                    m_ud_0_10000(m_rand_eng) % 3 == 0 ? 'R' :
                                    m_ud_0_10000(m_rand_eng) % 5 == 0 ? 'E' :
                                    'O';
        bridgeControl_provider.send(bridgeControl);
        adcm::Log::Verbose() << "[Control] send bridgeControl Event";
    }
}

void ThreadMonitor()
{
    while(continueExecution)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        if(gMainthread_Loopcount == 0)
        {
            INFO("Main thread Timeout!!!");
        }
        else
        {
            gMainthread_Loopcount = 0;

            if(gReceivedEvent_count_control_bridge != 0)
            {
                INFO("control_bridge Received count = %d", (unsigned int)gReceivedEvent_count_control_bridge);
                gReceivedEvent_count_control_bridge = 0;
            }
            else
            {
                INFO("control_bridge event timeout!!!");
            }

            if(gReceivedEvent_count_nats != 0)
            {
                INFO("nats Received count = %d", (unsigned int)gReceivedEvent_count_nats);
                gReceivedEvent_count_nats = 0;
            }
            else
            {
                INFO("nats event timeout!!!");
            }
        }
    }
}

}  // namespace

extern int main_thread();

int main(int argc, char* argv[])
{
    std::vector<std::thread> thread_list;
    UNUSED(argc);
    UNUSED(argv);

    if(!ara::core::Initialize())
    {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if(!Katech_Bridge::RegisterSigTermHandler())
    {
        adcm::Log::Error() << "Unable to register signal handler";
    }

#ifndef R19_11_1
    adcm::Log::Info() << "Katech_Bridge: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure(
                       "./etc/e2e_dataid_mapping.json", ara::com::e2exf::ConfigurationFormat::JSON,
                       "./etc/e2e_statemachines.json", ara::com::e2exf::ConfigurationFormat::JSON);
    adcm::Log::Info() << "Katech_Bridge: e2e configuration " << (success ? "succeeded" : "failed");
#endif
    adcm::Log::Info() << "Ok, let's produce some Katech_Bridge data...";
    // thread_list.push_back(std::thread(Katech_Bridge::ThreadReceiveControlBridge));
    // thread_list.push_back(std::thread(Katech_Bridge::ThreadSendBridgeControl));
    thread_list.push_back(std::thread(Katech_Bridge::ThreadMonitor));
    thread_list.push_back(std::thread(main_thread));
    adcm::Log::Info() << "Thread join";

    for(int i = 0; i < static_cast<int>(thread_list.size()); i++)
    {
        thread_list[i].join();
    }

    adcm::Log::Info() << "done.";

    if(!ara::core::Deinitialize())
    {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
