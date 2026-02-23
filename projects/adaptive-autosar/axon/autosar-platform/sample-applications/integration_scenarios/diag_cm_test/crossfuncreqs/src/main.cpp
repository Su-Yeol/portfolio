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

#include "ara/diag/diagnostic_manager/diagnosticevent_proxy.h"
#include "ara/diag/impl_type_dtcformattype.h"
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <ara/log/ifc/logging.h>

#include <mutex>
#include <memory>
#include <utility>
#include <iostream>
#include <iomanip>
#include <unistd.h>

using ara::com::ServiceHandleContainer;
using ara::com::FindServiceHandle;
using ara::diag::DTCFormatType;
using ara::diag::diagnostic_manager::proxy::DiagnosticEventProxy;

using namespace ara::log;

#define TRACE(str) (std::cout << "crossfuncreqs: " << str << std::endl)

std::mutex callbackMutex;

void DiagnosticEventCallback(ServiceHandleContainer<DiagnosticEventProxy::HandleType> handles)
{
    std::lock_guard<std::mutex> lock(callbackMutex);

    TRACE("DiagnosticEventCallback here. Found handles: " << handles.size());
    int handle_counter = 0;
    for (auto handle : handles) {
        auto proxy = std::make_shared<DiagnosticEventProxy>(handle);

        TRACE("Handle No. " << ++handle_counter << ":");
        TRACE("DiagnosticEvent::HandleType.GetInstanceId(): " << handle.GetInstanceId().ToString());

        if (proxy != nullptr) {
            auto debouncingOfEvent = proxy->GetDebouncingOfEvent();
            TRACE("DiagnosticEvent.GetDebouncingOfEvent: 0x"
                << std::hex << static_cast<unsigned int>(debouncingOfEvent.get().debouncingState) << std::dec);

            auto faultDetectionCounter = proxy->FaultDetectionCounter();
            TRACE("DiagnosticEvent.FaultDetectionCounter: " << static_cast<int>(
                      faultDetectionCounter.get().faultDetectionCounter));

            DTCFormatType kDTCFormatUDS = DTCFormatType::kDTCFormatUDS;
            auto dtcOfEvent = proxy->GetDTCOfEvent(kDTCFormatUDS);
            TRACE("DiagnosticEvent.DTCOfEvent: " << dtcOfEvent.get().DTCOfEvent);

            TRACE("INTEGRATION TEST <TC_DIAG_DiagnosticEvent> {SUCCESS}");
        } else {
            TRACE("No proxy could be found for service handle!");
        }
    }
}

int main(int argc, char** argv)
{
    ara::log::ifc::InitLogging(
        "DCIS", "diag_cm. crossfuncreqs", LogLevel::kVerbose, (LogMode::kRemote | LogMode::kConsole));
    auto& logger = CreateLogger("DFLT", "Default logger", LogLevel::kVerbose);

    logger.LogInfo() << "Starting";

    // report application state
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    TRACE("StartFindService for DiagnosticEvent");
    auto result = DiagnosticEventProxy::StartFindService(
        [](ServiceHandleContainer<DiagnosticEventProxy::HandleType> handles, FindServiceHandle) {
            DiagnosticEventCallback(std::move(handles));
        },
        ara::com::InstanceIdentifier::MakeAny());

    if (result) {
        sleep(15);
        DiagnosticEventProxy::StopFindService(*result);
    } else {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
    }

    logger.LogInfo() << "Terminating";

    return 0;
}
