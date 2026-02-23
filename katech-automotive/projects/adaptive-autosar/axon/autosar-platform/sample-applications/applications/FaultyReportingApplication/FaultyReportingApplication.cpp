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

#include "ara/exec/execution_client.h"
#include "ara/core/initialization.h"
#include "ara/log/logger.h"

// this app shall try to report its ExecutionState::kRunning although it is marked as non-reporting.
// this serves as an integration test scenario for Execution Management.
int main()
{
    if (!ara::core::Initialize()) {
        ara::core::Abort("Initialization of FaultyReportingApplication failed.");
    }

    ara::log::Logger& logger
        = ara::log::CreateLogger("FRAP", "Faulty Reporting Application", ara::log::LogLevel::kInfo);

    ara::exec::ExecutionClient executionClient{};
    ara::core::Result<void> reportResult = executionClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);
    if (reportResult.HasValue()) {
        logger.LogError() << "Successfully reported ExecutionState::kRunning.";
    } else {
        logger.LogInfo() << "Report of ExecutionState::kRunning was not successful";
    }

    if (!ara::core::Deinitialize()) {
        ara::core::Abort("Deinitialization of FaultyReportingApplication failed.");
    }

    return 0;
}
