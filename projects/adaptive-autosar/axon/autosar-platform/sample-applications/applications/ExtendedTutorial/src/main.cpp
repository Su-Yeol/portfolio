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

#include "demofunction_component.h"

#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include <thread>
#include <chrono>
#include <csignal>
#include <exception>
#include <atomic>

namespace  // contained elements for signal handling only to be seen & used locally
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

static void DemoFunctionThread()
{
    DemoFunctionComponent demoFunction;
    demoFunction.init();
    while (continueExecution) {
        demoFunction.act();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    demoFunction.shutdown();
}

int main()
{
    ara::core::Result<void> initSuccess = ara::core::Initialize();
    if (!initSuccess) {
        return 1;
    }

    ara::log::Logger& logger = ara::log::CreateLogger("ETrl", "Extended Tutorial", ara::log::LogLevel::kVerbose);
    logger.LogInfo() << "Starting ExtendedTutorial...";

    try {
        ara::exec::ExecutionClient execClient;
        if (!execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning)) {
            logger.LogError() << "Unable to report 'Running'";
        }

        if (!RegisterSigTermHandler()) {
            logger.LogError() << "Unable to register signal handler";
        }

        std::thread demoFunctionThread(DemoFunctionThread);
        demoFunctionThread.join();
    } catch (const std::exception& ex) {
        logger.LogError() << ex.what();
    } catch (...) {
        logger.LogError() << "An exception (not of type std::exception) occurred";
    }

    logger.LogInfo() << "Shutting down ExtendedTutorial...";
    ara::core::Result<void> shutdownSuccess = ara::core::Deinitialize();
    if (!shutdownSuccess) {
        return 1;
    }
    return 0;
}
