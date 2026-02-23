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
// Activity specific implementation, skeleton can be generated from the model
// Discovery of services and sending/receiving of data according
// to the communication API
///////////////////////////////////////////////////////////////////////

#include "shmClient2_activity.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>

// includes for used services
#include "ara/com/sample/healthindicator_proxy.h"

#include "ara/com/e2exf/types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"

#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

namespace shmClient2
{
shmClient2Activity::shmClient2Activity()
    : m_proxy(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void shmClient2Activity::init()
{
    m_logger.LogInfo() << "init() enter";
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.
    ara::core::InstanceSpecifier portSpecifier{"shmClient2/shmClient2/HealthIndicator_RPort"};
    m_logger.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger.LogError() << "No InstanceIdentifiers resolved from provided InstanceSpecifier";
    }

    m_logger.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();

    auto res = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            shmClient2Activity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        portSpecifier);
    if (!res) {
        m_logger.LogError() << "StartFindService failed with error: " << res.Error();
    }

    m_logger.LogInfo() << "init() exit";
}

void shmClient2Activity::act()
{
    m_logger.LogInfo() << "shmClient2 alive";
    /* Define a variable for Performance received from Event, to be used for processing */
    static uint8_t Performance_Value;

    if (nullptr != m_proxy) {

        if (m_proxy->HealthIndicatorEvent.IsSubscribed()) {
            auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->HealthIndicatorEvent);
            bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

            LogStream logMsg{m_logger.LogVerbose()};

            logMsg << "HealthIndicatorEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
            logMsg.Flush();

            auto callback = [&logMsg](auto sample) {
                logMsg << "Polling: HealthIndicator - Performance:";
                // always returns NotAvailable if E2E is disabled

                auto HealthIndicatorData = sample->Performance;

                /* Copy the Performance from the HealthIndicator Data  */
                Performance_Value = HealthIndicatorData;

                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
                logMsg << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
                logMsg.Flush();
            };
            // execute callback for every samples in the context of GetNewSamples
            m_proxy->HealthIndicatorEvent.GetNewSamples(callback);
        } else {
            m_logger.LogInfo() << "Not yet subscribed to HealthIndicator";

            // m_proxy got initialized via callback.

            /* Subscribe to HealthIndicator event */
            m_proxy->HealthIndicatorEvent.Subscribe(3);
            m_logger.LogInfo() << "HealthIndicator subscription complete";
        }
    }

    /* Add the received Performance to the list */
    HI_Performance_List.push_back(Performance_Value);
    HI_Performance_List.pop_front();

    // Use HI values And Update Output as required
    Result_Enum ret_Val = HI_Processing(Performance_Value);

    /* Printing Reliabiliy List */
    std::cout << "\n The received Performance values are : ";
    for (std::list<int>::iterator n_index = HI_Performance_List.begin(); n_index != HI_Performance_List.end();
         ++n_index)
        std::cout << *n_index << ' ';
    std::cout << std::endl;
}

Result_Enum shmClient2Activity::HI_Processing(std::int16_t Performance)
{
    /*
     *		Health Indicator Processing Logic - Implementation Specific
     */
    return E_OK;
}

}  // namespace shmClient2
