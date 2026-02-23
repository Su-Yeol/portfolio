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

/// @file
#include "ara/com/internal/runtime.h"
#include "ara/core/initialization.h"
#include "ara/core/string.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "controller_activity.h"
#include "find_service.h"
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include <string>

#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"

using namespace std;

int main(int ac, char** av)
{

    ara::core::Result<void> initSuccess = ara::core::Initialize();
    if (!initSuccess) {
        return 1;
    }

    auto& logger = ara::log::CreateLogger("MAIN", "Controller main", ara::log::LogLevel::kVerbose);

    logger.LogInfo() << "CONTROLLER STARTS";
    logger.LogInfo() << "===========================";

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    logger.LogInfo() << "Starting Controller Application";

    apd::picar::controller::ControllerActivity actController;
    if (ac == 2 && strncmp(av[1], "--rest", 6) == 0) {
        actController.init(true);
    } else {
        actController.init(false);
    }
    actController.act();
    logger.LogInfo() << "Terminating Controller Application.";
    ara::core::Deinitialize();

    return 0;
}
