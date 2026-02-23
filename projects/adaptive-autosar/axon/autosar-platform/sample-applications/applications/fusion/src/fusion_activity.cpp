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

#include "fusion_activity.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

// includes for used services
#include "ara/com/sample/radar_proxy.h"
using ara::com::sample::Position;
using ara::com::sample::FusionVariant;

#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"

#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

// <<operator implementation for logging custom types
// just another variant as also available in radar sources.
inline LogStream& operator<<(LogStream& out, const Position& value)
{
    std::ostringstream stream;
    stream << "x, y, z (" << value.x << "," << value.y << "," << value.z << ")";
    return (out << stream.str().c_str());
}

namespace fusion
{
FusionActivity::FusionActivity()
    : m_proxy(nullptr)
    , m_sync_count(0)
    , m_async_count(0)
    , m_rand_eng(m_rd())
    , m_ud_min_500_500(-500, 500)
    , m_ud_5_30(5, 30)
{
    m_logger.LogDebug() << "object address" << this;
}

void FusionActivity::init()
{
    m_logger.LogInfo() << "init() enter";

    ara::core::InstanceSpecifier portSpecifier{"fusion/fusion/radar_RPort"};
    m_logger.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        throw std::runtime_error{"No InstanceIdentifiers resolved from provided InstanceSpecifier"};
    }
    m_logger.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();

    auto res = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            FusionActivity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        portSpecifier);
    if (!res) {
        m_logger.LogError() << "StartFindService failed with error: " << res.Error();
        throw std::runtime_error{"StartFindService failed"};
    }

    m_logger.LogInfo() << "init() exit";
}

void FusionActivity::asyncMethodCall()
{
    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->Adjust);
    auto stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    LogStream logMsg{m_logger.LogVerbose()};

    logMsg << "Adjust E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    // Calling "Adjust" method asynchronously (GetResult() should not be used unless results are ready).
    // Set random target position from -500 to 500.
    Position target_position;
    target_position.x = m_ud_min_500_500(m_rand_eng);
    target_position.y = m_ud_min_500_500(m_rand_eng);
    target_position.z = m_ud_min_500_500(m_rand_eng);

    m_async_count++;

    auto adjust_future = m_proxy->Adjust(target_position);

    // Every 14 calls set a callback to the async call, no need to poll the results.
    bool poll_mode = (m_async_count % 14 == 0) ? false : true;

    if (poll_mode) {
        m_logger.LogInfo() << METHODS_HEADER << "Adjust() have been called, poll for completion";
        // Polling the results.
        while (!adjust_future.is_ready()) {
            // Results are not ready yet.
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            m_logger.LogInfo() << METHODS_HEADER << "Radar is adjusting position, please wait ...";
        }

        // GetResult will return immediately.
        auto r = adjust_future.GetResult();
        if (r.HasValue()) {
            auto adjust_output = r.Value();
            if (adjust_output.success) {
                m_logger.LogInfo() << METHODS_HEADER << "Adjusting position was successful, effective position is ("
                                   << adjust_output.effective_position;

            } else {
                m_logger.LogWarn() << METHODS_HEADER
                                   << "Adjusting position was not successful, deviation from the target position is ("
                                   << adjust_output.effective_position.x - target_position.x << ","
                                   << adjust_output.effective_position.y - target_position.y << ","
                                   << adjust_output.effective_position.z - target_position.z << ")";
            }
        } else {
            auto err = r.Error();
            m_logger.LogWarn() << ERROR_HEADER << "Code: " << err.Value();
            m_logger.LogWarn() << ERROR_HEADER << "Message: " << err.Message();
        }
    } else {
        // Registering a notification callback.
        // TODO: Current ara::com::future.then() doesn't support arguments to be passed to the Lambda
        // TODO: then output can't be retrieved. According to the explanatory, the use example is
        // TODO: then([] (Future<Calibrate::Output> f) { f.GetResult()}.
        m_logger.LogInfo() << METHODS_HEADER << "Radar is adjusting position, you will be notified when done ...";

        adjust_future.then([&]() {
            // it's safe to access the logger instance even if it's parent goes out of scope,
            // as long as the application process is alive, the reference is valid, because of
            // strong ownership inside the logging framework.
            m_logger.LogInfo() << METHODS_HEADER << "Notification! Adjusting position has finished";
        });
    }
}

void FusionActivity::syncMethodCall()
{
    m_sync_count++;

    FusionVariant variant;

    if ((m_sync_count % 3) == 0) {
        variant = FusionVariant::FV_CHINA;
    } else if ((m_sync_count % 5) == 0) {
        variant = FusionVariant::FV_USA;
    } else if ((m_sync_count % 7) == 0) {
        variant = FusionVariant::FV_RUSSIA;
    } else {
        variant = FusionVariant::FV_EUROPE;
    }

    // Prepare the calibration configuration string.
    String configuration = "calibration_config_" + std::to_string(m_sync_count);

    // Calling "Calibrate" method synchronously (GetResult() should be used).
    auto calibrate_future = m_proxy->Calibrate(configuration, variant);

    m_logger.LogInfo() << METHODS_HEADER << "Radar is calibrating, please wait ...";

    auto r = calibrate_future.GetResult();
    if (r.HasValue()) {
        auto calibrate_output = r.Value();
        (void)calibrate_output;
        m_logger.LogInfo() << METHODS_HEADER << "Calibration was successful";
    } else {
        auto err = r.Error();
        m_logger.LogWarn() << ERROR_HEADER << "Code: " << err.Value();
        m_logger.LogWarn() << ERROR_HEADER << "Message: " << err.Message();
        // reset the count and continue
        m_sync_count = 0;
    }
}

void FusionActivity::fireAndForgetMethodCall()
{
    String text = "Echo has been called.";

    m_proxy->Echo(text);
    m_logger.LogInfo() << METHODS_HEADER << text;
}

void FusionActivity::updateRateSubscription()
{
    m_logger.LogInfo() << "updateRateSubscription()";
    if (!m_proxy->UpdateRate.IsSubscribed()) {
        // m_proxy got initialized via callback.
        LogStream logMsg{m_logger.LogVerbose()};

        // Register event receive callback
        m_proxy->UpdateRate.SetReceiveHandler([this]() { FusionActivity::updateRateReceived(); });

        // subscribe to event
        auto subscription_result = m_proxy->UpdateRate.Subscribe(1);
        if (subscription_result.HasValue()) {
            m_logger.LogInfo() << "Callback registered.";
        } else {
            m_logger.LogError() << "Subscription failed with error: " << subscription_result.Error();
            return;
        }

        m_logger.LogInfo() << "done";
    } else {
        m_logger.LogInfo() << "already subscribed";
    }
}

void FusionActivity::frontObjectDistanceSubscription()
{
    m_logger.LogInfo() << "frontObjectDistanceSubscription()";
    if (!m_proxy->FrontObjectDistance.IsSubscribed()) {
        // m_proxy got initialized via callback.
        LogStream logMsg{m_logger.LogVerbose()};

        auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->FrontObjectDistance);
        auto stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

        logMsg << "FrontObjectDistance E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
        logMsg.Flush();

        // Register event receive callback
        m_proxy->FrontObjectDistance.SetReceiveHandler([this]() { FusionActivity::frontObjectDistanceReceived(); });

        // subscribe to event
        auto subscription_result = m_proxy->FrontObjectDistance.Subscribe(1);
        if (subscription_result.HasValue()) {
            m_logger.LogInfo() << "Callback registered.";
        } else {
            m_logger.LogError() << "Subscription failed with error: " << subscription_result.Error();
            return;
        }
        m_logger.LogInfo() << "done";
    } else {
        m_logger.LogInfo() << "already subscribed";
    }
}

void FusionActivity::fieldGetter()
{
    m_logger.LogInfo() << FIELDS_HEADER << "Trying to get values for UpdateRate and RearObjectDistance";
    LogStream logMsgUpdate{m_logger.LogVerbose()};
    auto getUpdateRateFuture = m_proxy->UpdateRate.Get();
    auto rUpdateRate = getUpdateRateFuture.GetResult();
    if (rUpdateRate.HasValue()) {
        auto value = rUpdateRate.Value();
        logMsgUpdate << FIELDS_HEADER << " Current Update Rate is " << value;
    } else {
        // while field getters itself do not return errors, networking errors are still possible
        auto err = rUpdateRate.Error();
        logMsgUpdate << ERROR_HEADER << "Code: " << err.Value();
        logMsgUpdate << ERROR_HEADER << "Message: " << err.Message();
    }

    LogStream logMsgDistance{m_logger.LogVerbose()};
    auto getRearObjectDistanceFuture = m_proxy->RearObjectDistance.Get();
    auto rRearObjectDistance = getRearObjectDistanceFuture.GetResult();
    if (rRearObjectDistance.HasValue()) {
        auto value = rRearObjectDistance.Value();
        logMsgDistance << FIELDS_HEADER << " Current Rear Object Distance is " << value;
    } else {
        // while field getters itself do not return errors, networking errors are still possible
        auto err = rRearObjectDistance.Error();
        logMsgDistance << ERROR_HEADER << "Code: " << err.Value();
        logMsgDistance << ERROR_HEADER << "Message: " << err.Message();
    }
}

void FusionActivity::fieldSetter()
{
    m_logger.LogInfo() << FIELDS_HEADER << "Trying to set UpdateRate and ObjectDetectionLimit";
    if (m_act_count % 7 == 0) {
        std::uint32_t valueToSet = 0;
        auto setResultFuture = m_proxy->UpdateRate.Set(valueToSet);
        auto r = setResultFuture.GetResult();
        if (r.HasValue()) {
            auto valueWhichWasSet = r.Value();
            m_logger.LogInfo() << FIELDS_HEADER << "Set was successful: " << (valueWhichWasSet == valueToSet);
        } else {
            // while field setters itself do not return errors, networking errors are still possible
            auto err = r.Error();
            m_logger.LogWarn() << ERROR_HEADER << "Code: " << err.Value();
            m_logger.LogWarn() << ERROR_HEADER << "Message: " << err.Message();
        }
    } else {
        m_logger.LogInfo() << FIELDS_HEADER << "UpdateRate setting skipped";
    }

    if (m_act_count % 5 == 0) {
        std::uint16_t distance = m_ud_5_30(m_rand_eng);
        auto setResultFuture = m_proxy->ObjectDetectionLimit.Set(distance);
        auto r = setResultFuture.GetResult();
        if (r.HasValue()) {
            auto distanceWhichWasSet = r.Value();
            m_logger.LogInfo() << FIELDS_HEADER << "Set was successful: " << (distanceWhichWasSet == distance);
        } else {
            // while field setters itself do not return errors, networking errors are still possible
            auto err = r.Error();
            m_logger.LogWarn() << ERROR_HEADER << "Code: " << err.Value();
            m_logger.LogWarn() << ERROR_HEADER << "Message: " << err.Message();
        }
    } else {
        m_logger.LogInfo() << FIELDS_HEADER << "ObjectDetectionLimit setting skipped";
    }
}

void FusionActivity::setServiceStateChangeHandler()
{
    if (nullptr != m_proxy) {
        static bool isHandlerAlreadySet = false;
        if (!isHandlerAlreadySet) {
            ara::core::Result<void> res = m_proxy->SetServiceStateChangeHandler(
                [this](ara::com::ServiceState state) { FusionActivity::radarServiceStateChangeHandler(state); });
            if (res) {
                m_logger.LogInfo() << "ServiceStateChangeHandler Set Successfully";
                isHandlerAlreadySet = true;
            } else {
                m_logger.LogError() << "SetServiceStateChangeHandler failed with error: " << res.Error();
            }
        }
    }
}
void FusionActivity::act()
{
    m_logger.LogInfo() << "fusion alive";

    setServiceStateChangeHandler();

    if (nullptr != m_proxy && m_proxy->GetServiceState() == ara::com::ServiceState::kAvailable) {
        if (m_proxy->brakeEvent.IsSubscribed()) {

            auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy->brakeEvent);
            bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

            LogStream logMsg{m_logger.LogVerbose()};

            logMsg << "BrakeEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
            logMsg.Flush();

            auto callback = [&logMsg](auto sample) {
                logMsg << "Polling: BrakeEvent - Radar:";
                // always returns NotAvailable if E2E is disabled

                if (!sample->active) {
                    logMsg << "NOT";
                }
                logMsg << "active";

                auto const& l_dataVector = sample->objectVector;

                if (!l_dataVector.empty()) {
                    logMsg << " data: ";
                    ara::core::Span<std::uint8_t const> sp_object_vector(l_dataVector);
                    logMsg << ara::core::as_bytes(sp_object_vector);
                }

                auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
                bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
                logMsg << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
                logMsg.Flush();
            };
            // execute callback for every samples in the context of GetNewSamples
            m_proxy->brakeEvent.GetNewSamples(callback);
        } else {
            m_logger.LogInfo() << "not subscribed to brakeEvent yet";

            // m_proxy got initialized via callback.

            // Register subscription state change callback
            auto set_subscription_state_handler_result = m_proxy->brakeEvent.SetSubscriptionStateChangeHandler(
                [this](ara::com::SubscriptionState state) { FusionActivity::brakeEventSubscriptionState(state); });

            if (set_subscription_state_handler_result.HasValue()) {
                m_logger.LogInfo() << "Subscription state change handler registered for brakeEvent.";
            } else {
                m_logger.LogError()
                    << "Subscription state change handler registration failed for brakeEvent with error: "
                    << set_subscription_state_handler_result.Error();
                return;
            }

            // subscribe to event
            auto subscription_result = m_proxy->brakeEvent.Subscribe(3);
            if (subscription_result.HasValue()) {
                m_logger.LogInfo() << "Subscription successful for brakeEvent.";
            } else {
                m_logger.LogError() << "Subscription failed with error: " << subscription_result.Error();
                return;
            }
            m_logger.LogInfo() << "brakeEvent subscription complete";

            // Register event receive callback
            auto receive_handler_result = m_proxy->parkingBrakeEvent.SetReceiveHandler(
                [this]() { FusionActivity::parkingBrakeEventReceived(); });
            if (receive_handler_result.HasValue()) {
                m_logger.LogInfo() << "Event receive handler registered for parkingBrakeEvent.";
            } else {
                m_logger.LogError() << "Event receive handler registration failed for parkingBrakeEvent with error: "
                                    << receive_handler_result.Error();
                return;
            }
            m_logger.LogInfo() << "parkingBrakeEvent SetReceiveHandler() complete";

            // Register subscription state change callback
            set_subscription_state_handler_result = m_proxy->parkingBrakeEvent.SetSubscriptionStateChangeHandler(
                [this](
                    ara::com::SubscriptionState state) { FusionActivity::parkingBrakeEventSubscriptionState(state); });

            if (set_subscription_state_handler_result.HasValue()) {
                m_logger.LogInfo() << "Subscription state change handler registered for parkingBrakeEvent.";
            } else {
                m_logger.LogError()
                    << "Subscription state change handler registration failed for parkingBrakeEvent with error: "
                    << set_subscription_state_handler_result.Error();
                return;
            }

            // subscribe to event
            // kNewN -> kNewestN
            subscription_result = m_proxy->parkingBrakeEvent.Subscribe(1);
            if (subscription_result.HasValue()) {
                m_logger.LogInfo() << "Subscription successful for parkingBrakeEvent.";
            } else {
                m_logger.LogError() << "Subscription failed with error: " << subscription_result.Error();
                return;
            }
            m_logger.LogInfo() << "parkingBrakeEvent subscription complete";
        }

        m_logger.LogInfo() << "Subscribe to Update Rate Field";
        this->updateRateSubscription();

        m_logger.LogInfo() << "Subscribe to Front Object Distance Field.";
        this->frontObjectDistanceSubscription();

        m_logger.LogInfo() << "Testing Field Getter.";
        this->fieldGetter();

        m_logger.LogInfo() << "Testing Field Setter.";
        this->fieldSetter();

        m_logger.LogInfo() << "Testing Synchronous Method Call.";
        this->syncMethodCall();

        m_logger.LogInfo() << "Testing Asynchronous Methods Call.";
        this->asyncMethodCall();

        m_logger.LogInfo() << "Testing Fire And Forget Method Call.";
        this->fireAndForgetMethodCall();
    }
    m_act_count++;
}
}  // namespace fusion
