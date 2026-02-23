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

#include <iostream>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <csignal>
#include <thread>
#include <chrono>

#include "ara/core/initialization.h"
#include "ara/exec/state_client.h"
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include "ara/com/instance_identifier.h"

#include "sensor_actuator_service.h"

const ara::core::InstanceSpecifier ServicePortSpecifier{
    "RES_TimingRefServiceApp/RES_TimingRefServiceApp/SensorData_ActuatorPort"};

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

ara::log::Logger& GetLogger() noexcept
{
    static ara::log::Logger& logger
        = ara::log::CreateLogger("TRSM", "WG-RES Timing Reference Service - main()", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

void ThreadStartService()
{
    GetLogger().LogInfo() << "AP Sensor Actuator Service registering.";
    ara::com::InstanceIdentifierContainer instanceIDs = ara::com::runtime::ResolveInstanceIDs(ServicePortSpecifier);
    if (instanceIDs.empty()) {
        GetLogger().LogError()
            << "At least one InstanceIdentifiers not resolved from provided InstanceSpecifier -- Init failed";
        return;
    }
    GetLogger().LogInfo() << "AP Sensor Service starting.";
    std::unique_ptr<apd::wgres::tra::SensorActuatorServiceImp> service
        = std::make_unique<apd::wgres::tra::SensorActuatorServiceImp>(instanceIDs[0]);

    GetLogger().LogInfo() << "SensorActuatorService starts to offer service";
    service->OfferService();
    service->StartSendingData();

    while (continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    service->StopSendingData();
    service->StopOfferService();
    service.reset();
}

int main(int, char** /*argv*/)
{
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Failed to initialize. Terminating DataPreprocessor App." << std::endl;
        return 1;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (!RegisterSigTermHandler()) {
        GetLogger().LogError() << "Unable to register signal handler";
    }

    std::thread tdservice(ThreadStartService);
    tdservice.join();

    GetLogger().LogInfo() << "Terminating DataPreprocessor App";

    if (!ara::core::Deinitialize()) {
        std::cerr << "core deinitialization failed" << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
