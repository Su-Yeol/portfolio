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

#include <csignal>
#include <cstdlib>
#include <thread>
#include <chrono>

#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "ara/core/initialization.h"

#include "flashing_adapter_app.h"

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

const ara::core::String ucmIDArg{"--ucmId"};
const ara::core::String execModeArg{"--itm"};

void GetCommandLineArgs(int argc, char const* argv[], ara::core::String& str, bool& mode)
{
    // this requires ucmId to always be supplied, otherwise setting itm is impossible
    // however, since we always do that should not cause issues
    if (argc >= 3) {
        if (argv[1] == ucmIDArg) {
            str = argv[2];
        }
    }
    if (argc >= 4) {
        if (argv[3] == execModeArg) {
            mode = true;
        }
    }
}

}  // namespace

int main(int argc, char const* argv[])
{
    using namespace ara::log;
    uint32_t aliveCtr = 0;

    if (!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger{CreateLogger("MAIN", "Flashing Adapter main context", ara::log::LogLevel::kVerbose)};

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    // Default command line arguments
    ara::core::String ucmId{"FLAD"};
    bool itmMode = false;

    GetCommandLineArgs(argc, argv, ucmId, itmMode);

    FlashingAdapter flashingAdapter(
        ara::core::InstanceSpecifier("flashing_adapter/FlashingAdapterSwc/PackageManagementProvided"), ucmId, itmMode);

    flashingAdapter.Init();
    logger.LogInfo() << "Application successfully exits";

    while (continueExecution) {
        if (aliveCtr % 10 == 0) {
            logger.LogInfo() << "Flashing Adapter is alive";
        }
        aliveCtr++;
        flashingAdapter.Process();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    if (!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
