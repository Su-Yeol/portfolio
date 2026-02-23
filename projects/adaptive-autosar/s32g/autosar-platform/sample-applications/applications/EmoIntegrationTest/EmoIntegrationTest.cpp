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
#include "ara/core/vector.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include <unistd.h>

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger{ara::log::CreateLogger("EMIT", "EMO Integration Test", ara::log::LogLevel::kInfo)};
    return logger;
}

static const ara::core::Vector<ara::core::String> expected_args{"", "Argument 1", "Argument 2", "Argument 3"};

int main(int argc, char** argv)
{
    if (!ara::core::Initialize()) {
        ara::core::Abort("ara::core::Initialize() failed for EmoIntegrationTest.");
    }

    ara::exec::ExecutionClient executionClient;
    ara::core::Result<void> executionStateResponseResult
        = executionClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (executionStateResponseResult) {
        GetLogger().LogInfo() << "EmoIntegrationTest successfully reported its ExecutionState.";
    } else {
        ara::core::Abort("EmoIntegrationTest failed to report its ExecutionState.");
    }

    bool failure = false;

    GetLogger().LogInfo() << "Start checking process arguments.";

    ara::core::Vector<ara::core::String> actual_args{argv, argv + argc};
    if (actual_args.size() == expected_args.size()) {
        GetLogger().LogDebug() << "Actual number of arguments matches the expected number.";
        for (std::size_t i = 1; i < actual_args.size(); ++i) {
            if (actual_args[i] == expected_args[i]) {
                GetLogger().LogDebug() << "Argument" << actual_args[i] << "matches.";
            } else {
                GetLogger().LogError() << "Actual argument" << actual_args[i] << "does not match the expected one"
                                       << expected_args[i];
                failure = true;
            }
        }
    } else {
        GetLogger().LogError() << "Actual number of arguments" << actual_args.size()
                               << "does not match the expected number" << expected_args.size();
        failure = true;
    }

    GetLogger().LogInfo() << "Finished checking process arguments.";
    GetLogger().LogInfo() << "Start checking core-affinity.";

    cpu_set_t cpuSet;
    if (sched_getaffinity(0, sizeof(cpuSet), &cpuSet)) {
        GetLogger().LogError() << "sched_getaffinity failed with errno:" << errno;
        failure = true;
    } else {
        for (std::size_t i = 0; i < 4; i++) {
            if (CPU_ISSET(i, &cpuSet)) {
                GetLogger().LogInfo() << "EmoIntegrationTest's core affinity is set to:" << i;
                if (i >= 2) {
                    failure = true;
                }
            }
        }
    }

    GetLogger().LogInfo() << "Finished checking core-affinity.";
    GetLogger().LogInfo() << "Starting checking scheduling parameters";

    pid_t emoIntegrationTestPid = getpid();
    struct sched_param schedParams
    {
        - 1
    };
    int schedPolicy = sched_getscheduler(emoIntegrationTestPid);
    sched_getparam(emoIntegrationTestPid, &schedParams);

    GetLogger().LogInfo() << "EmoIntegrationTest's scheduling policy is" << schedPolicy;
    GetLogger().LogInfo() << "EmoIntegrationTest's scheduling priority is" << schedParams.sched_priority;
    if (schedPolicy != 2 || schedParams.sched_priority != 12) {
        failure = true;
    }

    GetLogger().LogInfo() << "Finished checking scheduling parameters.";

    // As this app is currently used for the integration test, it shall print a dedicated success/failure message.
    ara::core::String resultString = "INTEGRATION TEST <emo_integration_test>";
    ara::core::String successString = failure ? "{FAILURE}" : "{SUCCESS}";

    GetLogger().LogInfo() << resultString << successString;

    if (!ara::core::Deinitialize()) {
        ara::core::Abort("ara::core::Deinitialize() failed for EmoIntegrationTest.");
    }

    return EXIT_SUCCESS;
}
