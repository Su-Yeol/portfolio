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

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <csignal>
#include <thread>
#include <chrono>

#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>

#include "ara/core/initialization.h"

#include "radar_activity.h"

using namespace ara::log;  // 'using' is OK for ara::log

namespace
{

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};

void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
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
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

}  // namespace

void ThreadAct1()
{
    radar::RadarActivity actRadar;
    actRadar.init();

    while (continueExecution) {
        actRadar.act();
        // sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
}

int main(int argc, char* argv[])
{
    if (!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger = CreateLogger("DFLT", "Default logger", LogLevel::kVerbose);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    logger.LogInfo() << "Radar: configure e2e protection";
    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
        ara::com::e2exf::ConfigurationFormat::JSON,
        "./etc/e2e_statemachines.json",
        ara::com::e2exf::ConfigurationFormat::JSON);
    logger.LogInfo() << "Radar: e2e configuration " << (success ? "succeeded" : "failed");

    logger.LogInfo() << "Ok, let's produce some radar data...";
    std::thread act1(ThreadAct1);

    act1.join();

    logger.LogInfo() << "done.";

    if (!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
