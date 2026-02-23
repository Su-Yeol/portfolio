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
 * \file
 * \brief Specification of communication management validator subscriber activity.
 * \details Contains specification of communication management validator subscriber activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_SUBSCRIBER_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_SUBSCRIBER_H_

// includes for used services
#include "ara/com/sample/cmvalidatorpublisher_proxy.h"
#include "ara/log/logger.h"
#include "ara/com/e2e_helper.h"

#include <mutex>
#include <string>
#include <random>

namespace cmvalidatorsubscriber
{
// Header of Methods Output Messages.
constexpr auto kMethodsHeader = "METHODS: ";
constexpr auto kFieldsHeader = "FIELDS: ";
constexpr auto kEventsHeader = "EVENTS: ";
constexpr auto kErrorsHeader = "APPLICATION_ERROR: ";

/*!
 *  \brief Class implementing CMValidatorSubscriber activity.
 *
 *  CMValidatorSubscriber activity implementing function of data CMValidatorSubscriber.
 */
class CMValidatorSubscriberActivity
{
    using Proxy = ara::com::sample::proxy::CMValidatorPublisherProxy;

public:
    ara::com::SubscriptionState subField2 = {ara::com::SubscriptionState::kNotSubscribed};

    CMValidatorSubscriberActivity();

    /*!
     *  \brief Initializes CMValidatorSubscriber activity.
     *
     *  Initializes CMValidatorSubscriber activity. This is called during initialization of the runtime.
     */
    void Init();

    /*!
     *  \brief Runs CMValidatorSubscriber activity.
     *
     *  Executable unit triggered to perform CMValidatorSubscriber activity.
     */
    void Act();

    /*!
     *  \brief Callback to change to CMValidatorPublisher service offer changes.
     *
     *  Callback executed whenever a change for CMValidatorPublisher service offers happen.
     *
     */

    void ServiceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler)
    {
        for (auto it : handles) {
            logger_.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
        }
        if (handles.size() > 0) {
            std::lock_guard<std::mutex> lock(proxy__mutex);
            if (nullptr == proxy_) {
                proxy_ = std::make_shared<Proxy>(handles[0]);
                logger_.LogInfo() << "Created proxy from handle with instance: "
                                  << proxy_->GetHandle().GetInstanceId().ToString();
                // Construct some handles (implementation-specific).
                auto first_handle = handles[0];
                auto aux_handle = first_handle;
                for (auto current_handle : handles) {
                    // Call equality operator.
                    logger_.LogInfo() << "Check handle::operator==: "
                                      << static_cast<uint8_t>(aux_handle == current_handle);
                    // Call copy assignment operator.
                    aux_handle = current_handle;
                    // Call less-than operator.
                    logger_.LogInfo() << "Check handle::operator<: " << static_cast<uint8_t>(aux_handle < first_handle);
                }
            }
        }
    }

    void Event2Received()
    {
        ara::log::LogStream logMsg{logger_.LogVerbose()};

        auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(proxy_->Event2);
        bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

        logMsg << kEventsHeader << "Event2 E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
        logMsg.Flush();

        proxy_->Event2.GetNewSamples(
            [&logMsg](auto sample) {
                logMsg << kEventsHeader << "Callback: Event2 - CMValidatorPublisher:";

                if (!sample->active) {
                    logMsg << kEventsHeader << "NOT";
                }
                logMsg << kEventsHeader << "active";

                auto const& l_dataVector = sample->objectVector;

                if (!l_dataVector.empty()) {
                    logMsg << kEventsHeader << "Object:" << +l_dataVector[0];
                }

                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
                logMsg << kEventsHeader
                       << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
            },
            1);
    }

    /*!
     * \brief Callback Received when the Field1 is changed.
     *
     * Callback Received when the Field1 is changed.
     */
    void Field1Received()
    {
        ara::log::LogStream logMsg{logger_.LogVerbose()};

        proxy_->Field1.GetNewSamples(
            [&logMsg](
                auto sample) { logMsg << kFieldsHeader << "Callback: Field1 : " << static_cast<uint32_t>(*sample); },
            1);
    }

    /*!
     * \brief Callback Received when the Field2 is is changed.
     *
     * Callback Received when the Field2 is changed.
     */
    void Field2Received()
    {
        ara::log::LogStream logMsg{logger_.LogVerbose()};

        proxy_->Field2.GetNewSamples(
            [&logMsg](
                auto sample) { logMsg << kFieldsHeader << "Callback: Field2: " << static_cast<int16_t>(*sample); },
            1);
    }

protected:
    std::shared_ptr<Proxy> proxy_;
    std::mutex proxy__mutex;
    std::uint32_t sync_count_;
    std::uint32_t async_count_;
    std::uint32_t act_count_;
    std::random_device rd_;
    std::default_random_engine rand_eng_;
    std::uniform_int_distribution<std::int16_t> ud_min_500_500_;
    std::uniform_int_distribution<std::uint16_t> ud_5_30_;

    /*!
     * Test Method Calls.
     *
     * TODO: uptrace{SWS_CM_00197}, This is partially covered as ara::Future::then is not yet compelete.
     * \uptrace{SWS_CM_00191}
     * \uptrace{SWS_CM_00195}
     * \uptrace{SWS_CM_00193}
     * \uptrace{SWS_CM_00191}
     * \uptrace{SWS_CM_00195}
     * \uptrace{SWS_CM_00192}
     */
    void MethodCall();

    /*!
     * Test Fire And Forget Method Capability.
     *
     * \uptrace{SWS_CM_90434}
     */
    void FireAndForgetMethodCapability();

    /*!
     * Subscribe to Update Rate Field.
     */
    void Field1Subscription();

    /*!
     * Subscribe to Front Object Distance Field.
     */
    void Field2Subscription();

    /*!
     * Testing Field Getter.
     */
    void FieldGetter();

    /*!
     * Testing Field Setter.
     */
    void FieldSetter();

    // this class own logger
    ara::log::Logger& logger_{
        ara::log::CreateLogger("FACT", "CMValidatorSubscriberActivity class own context", ara::log::LogLevel::kInfo)};
};
}  // namespace cmvalidatorsubscriber

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_SUBSCRIBER_H_
