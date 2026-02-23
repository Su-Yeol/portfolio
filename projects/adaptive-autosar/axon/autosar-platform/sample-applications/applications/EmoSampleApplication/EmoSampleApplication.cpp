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

#include "ara/core/abort.h"
#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include <atomic>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <chrono>
#include <thread>

namespace
{

ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger{
        ara::log::CreateLogger("EMAP", "EMO Sample Application", ara::log::LogLevel::kInfo)};
    return logger;
}

std::atomic_bool continueExecution{true};

void SignalHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        continueExecution = false;
    }
}

}  // namespace

int main(int argc, const char** argv)
{
    if (!ara::core::Initialize()) {
        ara::core::Abort("EMO Sample Application failed to initialize.");
    }

    if (argc != 2) {
        ara::core::Abort("EMO Sample Application: Wrong number of arguments.");
    }

    signal(SIGTERM, SignalHandler);

    ara::exec::ExecutionClient execClient;
    ara::core::Result<void> executionStateReportResult
        = execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);
    if (executionStateReportResult.HasValue()) {
        GetLogger().LogInfo() << "EMO Sample Application successfully reported ExecutionState Running.";
    } else {
        GetLogger().LogWarn() << "EMO Sample Application's report of Execution State Running failed with error"
                              << executionStateReportResult.Error();
    }

    GetLogger().LogInfo() << "EMO Sample Application is running.";

    ara::core::String processArgument = argv[1];
    std::uint64_t counter = 0;

    while (continueExecution) {
        if (processArgument == "Parking" && counter == 250) {
            break;
        }

        if (counter % 4 == 0) {
            GetLogger().LogInfo() << "EMO Sample Application running -- counter" << counter;
        }

        ++counter;
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    GetLogger().LogInfo() << "Terminating EMO Sample Application.";

    if (!ara::core::Deinitialize()) {
        ara::core::Abort("EMO Sample Application: Failed to deinitialize.");
    }

    return EXIT_SUCCESS;
}
