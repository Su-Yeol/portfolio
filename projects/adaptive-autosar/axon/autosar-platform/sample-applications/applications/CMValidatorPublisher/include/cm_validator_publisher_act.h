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

/*!
 *  \file
 *  \brief Specification of communication management validator publisher.
 *
 *  \details Contains specification of communication management validator publisher.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_ACT_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_ACT_H_

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "ara/com/sample/cmvalidatorpublisher_skeleton.h"

#include <string>
#include <random>

namespace cmvalidatorpublisher
{
// Header of Methods Output Messages.
constexpr auto kMethodsHeader = "METHODS: ";
constexpr auto kFieldsHeader = "FIELDS: ";
constexpr auto kEventsHeader = "EVENTS: ";
constexpr auto kErrorsHeader = "APPLICATION_ERROR: ";

/*!
 *  \brief Class implementing CMValidatorPublisher activity.
 *
 *  CMValidatorPublisher activity implementing function of CMValidatorPublisher.
 */
class CMValidatorPublisherActivity
{
public:
    CMValidatorPublisherActivity();
    ~CMValidatorPublisherActivity();

    /*!
     *  \brief Initializes CMValidatorPublisher activity.
     *
     *  Initializes CMValidatorPublisher activity. This is called during initialization of the runtime.
     */
    void Init();

    /*!
     *  \brief Runs CMValidatorPublisher activity.
     *
     *  Executable unit triggered to perform CMValidatorPublisher activity.
     */
    void Act();

protected:
    /*!
     * Override the Field1 Getter, to be registered by RegisterGet.
     * {SWS_CM_00112}
     */
    ara::core::Future<ara::com::sample::skeleton::fields::Field1::value_type> GetField1();

    /*!
     * Override the Field1 Setter, to be registered by RegisterSet.
     * {SWS_CM_00113}
     */
    ara::core::Future<std::uint32_t> SetField1(std::uint32_t field);

    /*!
     *  \brief Send Events to subscribers.
     *  {SWS_CM_99033}
     *  {SWS_CM_90437}
     *  Allocate and Send mechanism.
     */
    void SendEvents();

    /*!
     * Update mechanisam for fields.
     * {SWS_CM_00120}
     */
    void UpdateFields();

    /*!
     * \brief A pointer to the skeleton object.
     */
    ara::com::sample::skeleton::CMValidatorPublisherSkeleton* skeleton_;
    std::uint32_t field_1_;
    std::random_device rd_;
    std::default_random_engine randEng_;
    std::uniform_int_distribution<std::uint16_t> ud_0_100_;
    std::uniform_int_distribution<std::uint16_t> ud_0_50_;

    enum class internalStates
    {
        kReady,
        kNotReady
    };
    internalStates internal_state_for_update_rate_set_handler_ = internalStates::kReady;

    ara::log::Logger& logger_{ara::log::CreateLogger("CMVPA", "context for activity", ara::log::LogLevel::kInfo)};

    ara::log::Logger& loggerCtxEvent_{ara::log::CreateLogger("CMVPE", "context for event", ara::log::LogLevel::kInfo)};

    ara::log::Logger& loggerCtxField_{ara::log::CreateLogger("CMVPF", "context for field", ara::log::LogLevel::kInfo)};
};
}  // namespace cmvalidatorpublisher

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_ACT_H_
