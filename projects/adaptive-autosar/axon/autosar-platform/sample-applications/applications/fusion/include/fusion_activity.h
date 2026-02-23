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
 * \brief Specification of fusion activity.
 * \details Contains specification of fusion activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_
#define CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "ara/com/sample/radar_proxy.h"
#include "ara/log/logger.h"
#include "ara/com/e2e_helper.h"

namespace fusion
{
// Header of Methods Output Messages.
constexpr auto METHODS_HEADER = "METHODS: ";
constexpr auto FIELDS_HEADER = "FIELDS: ";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR: ";

/*!
 *  \brief Class implementing fusion activity.
 *
 *  Fusion activity implementing function of data fusion.
 */
class FusionActivity
{
    using Proxy = ara::com::sample::proxy::radarProxy;

public:
    FusionActivity();

    /*!
     *  \brief Initializes fusion activity.
     *
     *  Initializes fusion activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs fusion activity.
     *
     *  Executable unit triggered to perform fusion activity.
     */
    void act();

    /*!
     *  \brief Callback to change to radar service offer changes.
     *
     *  Callback executed whenever a change for radar service offers happen.
     *
     */

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler)
    {
        for (auto it : handles) {
            m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
        }
        if (handles.size() > 0) {
            std::lock_guard<std::mutex> lock(m_proxy_mutex);
            if (nullptr == m_proxy) {
                m_proxy = std::make_shared<Proxy>(handles[0]);
                m_logger.LogInfo() << "Created proxy from handle with instance: "
                                   << m_proxy->GetHandle().GetInstanceId().ToString();
                // Construct some handles (implementation-specific).
                auto first_handle = handles[0];
                auto aux_handle = first_handle;
                for (auto current_handle : handles) {
                    // Call equality operator.
                    m_logger.LogInfo() << "Check handle::operator==: "
                                       << static_cast<uint8_t>(aux_handle == current_handle);
                    // Call copy assignment operator.
                    aux_handle = current_handle;
                    // Call less-than operator.
                    m_logger.LogInfo() << "Check handle::operator<: "
                                       << static_cast<uint8_t>(aux_handle < first_handle);
                }
            }
        }
    }

    /*!
     *  \brief ServiceStateChangeHandler
     *
     *  Callback executed whenever the state of radar Service changes. i.e from kAvailable to kNotAvailable or vice
     * versa.
     *
     */

    void radarServiceStateChangeHandler(ara::com::ServiceState serviceState)
    {
        m_logger.LogInfo() << "Radar Service State Changed! The new State is -> " << static_cast<bool>(serviceState);
    }

    void parkingBrakeEventReceived()
    {
        ara::log::LogStream logMsg{m_logger.LogVerbose()};

        auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->parkingBrakeEvent);
        bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

        logMsg << "ParkingBrakeEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
        logMsg.Flush();

        m_proxy->parkingBrakeEvent.GetNewSamples(
            [&logMsg](auto sample) {
                logMsg << "Callback: ParkingBrakeEvent - Radar:";

                if (!sample->active) {
                    logMsg << "NOT";
                }
                logMsg << "active";

                auto const& l_dataVector = sample->objectVector;

                if (!l_dataVector.empty()) {
                    logMsg << "Object:" << +l_dataVector[0];
                }

                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
                logMsg << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
            },
            1);
    }

    /*!
     * \brief Callback Received when the Field Update Rate is changed.
     *
     * Callback Received when the Field Update Rate is changed.
     */
    void updateRateReceived()
    {
        ara::log::LogStream logMsg{m_logger.LogVerbose()};

        m_proxy->UpdateRate.GetNewSamples(
            [&logMsg](auto sample) {
                logMsg << FIELDS_HEADER << "Callback: UpdateRate Field: " << static_cast<uint32_t>(*sample);
            },
            1);
    }

    /*!
     * \brief Callback Received when the Front Object Distance is is changed.
     *
     * Callback Received when the Field Front Object Distance is changed.
     */
    void frontObjectDistanceReceived()
    {
        ara::log::LogStream logMsg{m_logger.LogVerbose()};

        m_proxy->FrontObjectDistance.GetNewSamples(
            [&logMsg](auto sample) {
                logMsg << FIELDS_HEADER << "Callback: Front Object Distance Field: " << static_cast<int16_t>(*sample);
            },
            1);
    }

    /*!
     * \brief Callback Received when the subscription state of brakeEvent is changed.
     *
     * Callback Received when the subscription state of brakeEvent is changed.
     */
    void brakeEventSubscriptionState(ara::com::SubscriptionState state)
    {
        ara::log::LogStream logMsg{m_logger.LogVerbose()};
        logMsg << "brakeEvent state changed to" << static_cast<uint8_t>(state);
    }

    /*!
     * \brief Callback Received when the subscription state of parkingBrakeEvent is changed.
     *
     * Callback Received when the subscription state of parkingBrakeEvent is changed.
     */
    void parkingBrakeEventSubscriptionState(ara::com::SubscriptionState state)
    {
        ara::log::LogStream logMsg{m_logger.LogVerbose()};
        logMsg << "parkingBrakeEvent state changed to" << static_cast<uint8_t>(state);
    }

    void setServiceStateChangeHandler();

protected:
    std::shared_ptr<Proxy> m_proxy;
    std::mutex m_proxy_mutex;
    std::uint32_t m_sync_count;
    std::uint32_t m_async_count;
    std::uint32_t m_act_count;
    std::random_device m_rd;
    std::default_random_engine m_rand_eng;
    std::uniform_int_distribution<std::int16_t> m_ud_min_500_500;
    std::uniform_int_distribution<std::uint16_t> m_ud_5_30;

    /*!
     * Test Asynchrnous Method Calls.
     *
     * TODO: uptrace{SWS_CM_00197}, This is partially covered as ara::Future::then is not yet compelete.
     * \uptrace{SWS_CM_00191}
     * \uptrace{SWS_CM_00195}
     * \uptrace{SWS_CM_00193}
     */
    void asyncMethodCall();

    /*!
     * Test Synchrnous Method Calls.
     *
     * \uptrace{SWS_CM_00191}
     * \uptrace{SWS_CM_00195}
     * \uptrace{SWS_CM_00192}
     */
    void syncMethodCall();

    /*!
     * Test Fire And Forget Method Calls.
     *
     * \uptrace{SWS_CM_90434}
     */
    void fireAndForgetMethodCall();

    /*!
     * Subscribe to Update Rate Field.
     */
    void updateRateSubscription();

    /*!
     * Subscribe to Front Object Distance Field.
     */
    void frontObjectDistanceSubscription();

    /*!
     * Testing Field Getter.
     */
    void fieldGetter();

    /*!
     * Testing Field Setter.
     */
    void fieldSetter();

    // this class own logger
    ara::log::Logger& m_logger{
        ara::log::CreateLogger("FACT", "FusionActivity class own context", ara::log::LogLevel::kVerbose)};
};
}  // namespace fusion

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_
