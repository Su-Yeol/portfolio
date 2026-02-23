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

#include "shmClient1_activity.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "ara/com/sample/healthinfo_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"

using namespace ara::log;

using ara::com::ComErrorDomainErrc;

namespace shmClient1
{
void shmClient1Imp::ProcessRequests()
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

shmClient1Activity::shmClient1Activity()
{
    ara::core::InstanceSpecifier instanceSpec{"shmClient1/shmClient1/shmClient1_PPort"};
    m_logger_ctx3.LogInfo() << "Port In Executable Ref:" << instanceSpec.ToString();

    m_skeleton = new shmClient1Imp(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);

    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);
    for (auto const& instanceId : instanceIDs) {
        m_logger_ctx3.LogInfo() << "Service Instance offered:" << instanceId.ToString();
    }

    m_logger_ctx3.LogDebug() << "object address" << this;
}

shmClient1Activity::~shmClient1Activity()
{
    delete m_skeleton;
}

void shmClient1Activity::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";

    m_skeleton->OfferService();
}

void shmClient1Activity::act()
{
    m_logger_ctx3.LogInfo() << "Health Data service active";

    /* NOTE: Global Supervision Status to be received from PHM */
    /*	NOTE: Sample values are considered for GlobalSupervisionStatus as an example. */
    if (GSS_Send_Counter < 100) {
        if (GSS_Send_Counter < 10) {
            GlobalSupervisionStatusValue = DEACTIVATED;
        } else if (GSS_Send_Counter > 25 && GSS_Send_Counter < 50) {
            GlobalSupervisionStatusValue = FAILED;
        } else {
            GlobalSupervisionStatusValue = OK;
        }
    } else {
        GSS_Send_Counter = 0;
    }

    // ASWS_HM_00501
    auto platform_HI = platform_HI_Determination(GlobalSupervisionStatusValue);

    /* Allocate sample */
    // ASWS_HM_00515
    auto HealthInfoData = m_skeleton->HealthInfoEvent.Allocate();
    auto l_sampleHealthInfoData = std::move(HealthInfoData).Value();
    // ASWS_HM_00517 (Partially implemented. Need to update the sub-element with a structure GlobalSupervisionInfo
    // instead of just GlobalSupervisionStatus value.)
    l_sampleHealthInfoData->GlobalSupervisionInfoVector.push_back(GlobalSupervisionStatusValue);

    /* Send the HealthInfo. */
    // ASWS_HM_00502
    m_skeleton->HealthInfoEvent.Send(std::move(l_sampleHealthInfoData));
    m_logger_ctx3.LogInfo() << "HealthInfo sent";

    /* Allocate sample for platform HealthIndicator */
    int8_t healthIndicator_Id = 1; /* Sample value for healthIndicator ID considered as 1. */
    // ASWS_HM_00511
    auto HealthIndicatorData = m_skeleton->HealthIndicatorEvent.Allocate();
    auto l_sampleplatformHealthIndicatorData = std::move(HealthIndicatorData).Value();
    l_sampleplatformHealthIndicatorData->HealthIndicatorID = healthIndicator_Id;
    l_sampleplatformHealthIndicatorData->Reliability = platform_HI;

    /* Send the platform HealthIndicator. */
    // ASWS_HM_00502
    m_skeleton->HealthIndicatorEvent.Send(std::move(l_sampleplatformHealthIndicatorData));
    m_logger_ctx3.LogInfo() << "platform HealthIndicator sent";

    /* Add GlobalSupervisionStatus to the GSS list */
    GSS_Provider_List.push_back(GlobalSupervisionStatusValue); /* Used for logging. */
    GSS_Provider_List.pop_front();

    /* Print GSS Provider List */
    std::cout << "\n The provided GSS values are : "; /* Used for logging. */
    for (std::list<int>::iterator n_index = GSS_Provider_List.begin(); n_index != GSS_Provider_List.end(); ++n_index)
        std::cout << *n_index << ' ';
    std::cout << std::endl;

    GSS_Send_Counter++;
}

int16_t shmClient1Activity::platform_HI_Determination(uint8_t globalSupervisionStatus)
{
    int16_t platform_HI = REL_FAILED;

    /* NOTE: Sample logic is written for determination of platform HealthIndicator. */
    if (globalSupervisionStatus == DEACTIVATED) {
        platform_HI = REL_FAILED;
    } else if (globalSupervisionStatus == FAILED) {
        platform_HI = REL_FAILED;
    } else if (globalSupervisionStatus == OK) {
        platform_HI = REL_GOOD;
    } else {
        /* Do Nothing */
    }

    return platform_HI;
}

}  // namespace shmClient1
