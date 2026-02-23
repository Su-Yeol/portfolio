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

#include "shmMaster_activity.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

// includes for used services
#include "ara/com/sample/healthinfo_proxy.h"

#include "ara/com/e2exf/types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"

#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

HealthIndicator HI_AEB = {0, 0, 0};  // Initialize HealthIndicator structure with 0.

namespace shmMaster
{
shmMasterActivity::shmMasterActivity()
    : m_proxy(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void shmMasterActivity::init()
{
    m_logger.LogInfo() << "init() enter";
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.
    ara::core::InstanceSpecifier portSpecifier{"shmMaster/shmMaster/HealthInfo_RPort"};
    m_logger.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger.LogError() << "shmMaster Activity No InstanceIdentifiers resolved from provided InstanceSpecifier";
    }
    m_logger.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();

    auto res = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            shmMasterActivity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        portSpecifier);
    if (!res) {
        m_logger.LogError() << "StartFindService failed with error: " << res.Error();
    }

    m_logger.LogInfo() << "init() exit";
}

void shmMasterActivity::act()
{
    /* Define variables for Global Supervision Status, Health Indicator ID and Reliability received from Events, to be
     * used for processing */
    uint8_t received_GlobalSupervisionStatus = DEACTIVATED;
    uint8_t received_HealthIndicatorID = DEACTIVATED;
    uint8_t received_Reliabiity = DEACTIVATED;

    m_logger.LogInfo() << "SHM Master alive";

    if (nullptr != m_proxy) {
        /* Check if HealthInfoEvent is already subscribed */
        if (m_proxy->HealthInfoEvent.IsSubscribed()) {
            auto e2eResult = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->HealthInfoEvent);
            bool stateResult = (e2eResult == ara::com::e2e::SMState::kNoData);

            LogStream logMsg{m_logger.LogVerbose()};

            logMsg << "HealthInfo E2E state is: " << (stateResult ? "ok (NoData)" : "not ok");
            logMsg.Flush();

            auto callback = [&received_GlobalSupervisionStatus](auto sample) {
                auto HealthInfoDataVector = sample->GlobalSupervisionInfoVector;

                if (!HealthInfoDataVector.empty()) {
                    /* Copy the GlobalSupervisionStatus from the HealthInfo Data  */
                    received_GlobalSupervisionStatus = HealthInfoDataVector[0];
                }
                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
            };
            // execute callback for every samples in the context of GetNewSamples
            m_proxy->HealthInfoEvent.GetNewSamples(callback);
        } else {
            m_logger.LogInfo() << "Not yet subscribed to HealthInfo";

            // m_proxy got initialized via callback.

            /* Subscribe to HealthInfo event */
            m_proxy->HealthInfoEvent.Subscribe(3);
            m_logger.LogInfo() << "HealthInfo subscription complete";
        }

        /*******************************************************************************************************************************/
        /* Check if HealthIndicatorEvent is already subscribed */
        if (m_proxy->HealthIndicatorEvent.IsSubscribed()) {
            auto e2eResult = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->HealthIndicatorEvent);
            bool stateResult = (e2eResult == ara::com::e2e::SMState::kNoData);

            LogStream logMsg{m_logger.LogVerbose()};

            logMsg << "platform HealthIndicator E2E state is: " << (stateResult ? "ok (NoData)" : "not ok");
            logMsg.Flush();

            auto callback = [&received_HealthIndicatorID, &received_Reliabiity](auto sample) {
                // always returns NotAvailable if E2E is disabled

                auto shmClient1_HealthIndicatorID = sample->HealthIndicatorID;
                auto shmClient1_Reliabiity = sample->Reliability;

                /* Copy the Platform Health Indicator and HealthIndicator ID from the HealthIndicator Data  */
                received_HealthIndicatorID = shmClient1_HealthIndicatorID;
                received_Reliabiity = shmClient1_Reliabiity;

                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
            };
            // execute callback for every samples in the context of GetNewSamples
            m_proxy->HealthIndicatorEvent.GetNewSamples(callback);
        } else {
            m_logger.LogInfo() << "Not yet subscribed to platform HealthIndicator";

            // m_proxy got initialized via callback.

            /* Subscribe to HealthIndicator event */
            m_proxy->HealthIndicatorEvent.Subscribe(3);
            m_logger.LogInfo() << "HealthIndicator subscription complete";
        }

        /**********************************************************************************************************************/

        /*// Add the received GlobalSupervisionStatus to the GSS list */
        GSS_Received_List.push_back(received_GlobalSupervisionStatus);
        GSS_Received_List.pop_front();

        /* Determination of Health Indicators by calling HIDetermination() function */
        auto result_HI_Determination
            = HI_Determination(received_GlobalSupervisionStatus, received_HealthIndicatorID, received_Reliabiity);
        if (result_HI_Determination != E_OK) {
            // Error in HI calculation
        } else {
            // Do nothing
            /* Health Indicators are communicated to SHM Clients */
        }
    }

    /* Printing Received GSS List */
    std::cout << "\n The received GSS values are : ";
    for (std::list<int>::iterator n_index = GSS_Received_List.begin(); n_index != GSS_Received_List.end(); ++n_index)
        std::cout << *n_index << ' ';
    std::cout << std::endl;

    /* Printing Health Indicator -> Reliabiliy List */
    std::cout << "\n The determined Performance values are : ";
    for (std::list<int>::iterator n_index = HI_Performance_List.begin(); n_index != HI_Performance_List.end();
         ++n_index)
        std::cout << *n_index << ' ';
    std::cout << std::endl;
}

/* Determination of Health Indicators */
Result_Enum shmMasterActivity::HI_Determination(std::uint8_t globalSupervisionStatus,
    std::uint8_t HealthIndicatorID,
    std::uint8_t received_Reliability)
{
    auto GSS = globalSupervisionStatus;
    auto HID = HealthIndicatorID;
    auto Rlt = received_Reliability;

    /*	NOTE: Sample logic for determination of Health Indicator -> Reliability based on the Global Supervision Status.
     */
    if (HID == 1) {
        if (GSS == DEACTIVATED && Rlt == DEACTIVATED) {
            HI_AEB.Performance = PER_INIT;
        } else if (Rlt == REL_RECOVERING || GSS == FAILED) {
            HI_AEB.Performance = PER_LOW;
        } else if (GSS == OK && Rlt == REL_RECOVERING) {
            HI_AEB.Performance = PER_OK;
        } else if (GSS == EXPIRED || GSS == STOPPED) {
            HI_AEB.Performance = PER_VERY_LOW;
        } else if (GSS == OK || Rlt == REL_GOOD) {
            HI_AEB.Performance = PER_EXCELLENT;
        } else {
            /* Do Nothing. */
        }
    }

    // Update the list with Performance values
    HI_Performance_List.push_back(HI_AEB.Performance);
    HI_Performance_List.pop_front();

    return E_OK;
}

}  // namespace shmMaster

/* ****************************************************************** Namespace for Health Indicator
 * ****************************************************/
namespace shmMaster_healthindicator
{

void healthindicatorImp::ProcessRequests()
{
    while (!m_finished) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            m_logger_ctx1.LogFatal() << "Request took too long :S";
        } else {
            if (!m_finished) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

healthindicatorActivity::healthindicatorActivity()
{
    ara::core::InstanceSpecifier instanceSpecHI{"shmMaster/shmMaster/HealthIndicator_PPort"};
    m_logger_ctx3.LogInfo() << "Port In Executable Ref:" << instanceSpecHI.ToString();

    m_skeleton_HI = new healthindicatorImp(instanceSpecHI, ara::com::MethodCallProcessingMode::kPoll);

    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpecHI);
    for (auto const& instanceId : instanceIDs) {
        m_logger_ctx3.LogInfo() << "Service Instance offered:" << instanceId.ToString();
    }

    m_logger_ctx3.LogDebug() << "object address" << this;
}

healthindicatorActivity::~healthindicatorActivity()
{
    delete m_skeleton_HI;
}

void healthindicatorActivity::init()
{
    m_logger_ctx3.LogDebug() << "Enter init() for HealthIndicator";

    m_skeleton_HI->OfferService();
}

void healthindicatorActivity::act()
{
    m_logger_ctx3.LogInfo() << "Health Indicator active";

    /* Allocate sample */
    // ASWS_HM_00511
    auto HealthIndicatorData = m_skeleton_HI->HealthIndicatorEvent.Allocate();
    auto l_sampleHealthIndicatorData = std::move(HealthIndicatorData).Value();
    l_sampleHealthIndicatorData->Performance = HI_AEB.Performance;

    /* Send the HealthIndicator. */
    // ASWS_HM_00509
    m_skeleton_HI->HealthIndicatorEvent.Send(std::move(l_sampleHealthIndicatorData));
    m_logger_ctx3.LogInfo() << "HealthIndicator sent";
}

}  // namespace shmMaster_healthindicator
