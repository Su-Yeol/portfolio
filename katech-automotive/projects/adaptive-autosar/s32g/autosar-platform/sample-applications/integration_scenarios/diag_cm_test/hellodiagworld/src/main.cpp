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

/**
 * \brief Main entry point of the hellodiagworld test application.
 * \details This is the main.cpp file containing the main function.
 */
#include "ara/diag/diagnostic_manager/diagnosticstatus_proxy.h"
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <ara/log/ifc/logging.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <future>

using ara::com::ServiceHandleContainer;
using ara::com::FindServiceHandle;
using ara::diag::diagnostic_manager::proxy::DiagnosticStatusProxy;

using namespace ara::log;

#define TRACE(str) (std::cout << "hellodiagworld: " << str << std::endl)

void IntegrationTest(ara::com::ServiceHandleContainer<DiagnosticStatusProxy::HandleType> handles)
{
    TRACE("Integration Test. Found handles: " << handles.size());
    if (handles.size() > 1) {
        TRACE("Error: More than one DiagnosticStatus service interface found.");
        TRACE("INTEGRATION TEST <TC_DIAG_DiagnosticStatus> {FAIL}");
    } else if (handles.size() == 1) {
        auto proxy = std::make_unique<DiagnosticStatusProxy>(handles[0]);

        TRACE("Handle instance identifier: " << proxy->GetHandle().GetInstanceId().ToString());

        auto curr_active_protocols = proxy->CurrentActiveProtocols.Get();
        TRACE("Current active protocols: " << curr_active_protocols.get().size());
        TRACE("INTEGRATION TEST <TC_DIAG_DiagnosticStatus> {SUCCESS}");
    } else {
        TRACE("No proxy could be found for service handle DiagnosticStatus!");
        TRACE("INTEGRATION TEST <TC_DIAG_DiagnosticStatus> {FAIL}");
    }
}

int main(int argc, char** argv)
{
    ara::log::ifc::InitLogging(
        "DCIS", "diag_cm. hellodiagworld", LogLevel::kVerbose, (LogMode::kRemote | LogMode::kConsole));
    auto& logger = CreateLogger("DFLT", "Default logger", LogLevel::kVerbose);

    logger.LogInfo() << "Starting";

    // report application state
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    // start diagnostic status service discovery
    std::promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();

    ara::com::ServiceHandleContainer<DiagnosticStatusProxy::HandleType> foundHandles;

    auto result = DiagnosticStatusProxy::StartFindService(
        [&signalPromise, &foundHandles, &logger](
            ara::com::ServiceHandleContainer<DiagnosticStatusProxy::HandleType> handles,
            ara::com::FindServiceHandle findHandle) {
            static std::once_flag flag;
            if (handles.empty()) {
                logger.LogInfo() << "No handles are available yet";
            } else {
                std::call_once(
                    flag,
                    [&signalPromise, &foundHandles, &logger](auto&& handles, auto& stopHandle) {
                        DiagnosticStatusProxy::StopFindService(stopHandle);
                        logger.LogInfo() << "Handles are available; finding was stopped";
                        foundHandles = std::move(handles);
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findHandle);
            }
        },
        ara::com::InstanceIdentifier::MakeAny());
    if (result) {
        signalFuture.wait();

        IntegrationTest(std::move(foundHandles));
    } else {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
    }
    logger.LogInfo() << "Terminating";

    return 0;
}
