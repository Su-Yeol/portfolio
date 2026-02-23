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

// includes for used services
#include "ara/com/sample/cmvalidatorpublisher_proxy.h"

#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"

#include "ara/log/logger.h"

#include "cm_validator_subscriber.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace ara::log;  // 'using' ara::log should be OK

namespace cmvalidatorsubscriber
{
CMValidatorSubscriberActivity::CMValidatorSubscriberActivity()
    : proxy_(nullptr)
    , sync_count_(0)
    , async_count_(0)
    , rand_eng_(rd_())
    , ud_min_500_500_(-500, 500)
    , ud_5_30_(5, 30)
{
    logger_.LogDebug() << "object address" << this;
}

void CMValidatorSubscriberActivity::Init()
{
    logger_.LogInfo() << "Init() enter";

    ara::core::InstanceSpecifier portSpecifier{
        "CMValidatorSubscriber/CMValidatorSubscriber/CMValidatorSubscriber_RPort"};
    logger_.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        throw std::runtime_error{"No InstanceIdentifiers resolved from provided InstanceSpecifier"};
    }
    logger_.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();

    auto res = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            CMValidatorSubscriberActivity::ServiceAvailabilityCallback(std::move(handles), handler);
        },
        portSpecifier);

    // validate SWS_CM_00122
    if (proxy_ == nullptr) {
        logger_.LogDebug() << "proxy_ is empty, use one-shot find request";
        Proxy::FindService(instanceIDs[0]);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (!res) {
        logger_.LogError() << "StartFindService failed with error: " << res.Error();
        throw std::runtime_error{"StartFindService failed"};
    }

    logger_.LogInfo() << "Init() exit";
}

void CMValidatorSubscriberActivity::MethodCall()
{
    sync_count_++;

    logger_.LogInfo() << kMethodsHeader << "MethodsCapability() called, poll for completion";

    // Calling "MethodsCapability" method
    auto method_capability_future = proxy_->MethodsCapability();

    while (!method_capability_future.is_ready()) {
        // Results are not ready yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        logger_.LogInfo() << kMethodsHeader << "CMValidatorPublisher is loading params, please wait ...";
    }

    logger_.LogInfo() << kMethodsHeader << "Results are not ready yet, please wait ...";

    auto r = method_capability_future.GetResult();
    if (r.HasValue()) {
        auto method_capability_output = r.Value();
        (void)method_capability_output;
        logger_.LogInfo() << kMethodsHeader << "Data loaded successful";
    } else {
        auto err = r.Error();
        logger_.LogWarn() << kErrorsHeader << "Code: " << err.Value();
        logger_.LogWarn() << kErrorsHeader << "Message: " << err.Message();
        // reset the count and continue
        sync_count_ = 0;
    }
}

void CMValidatorSubscriberActivity::FireAndForgetMethodCapability()
{
    String text = "fireAndForgetMethodCapability called.";

    proxy_->Echo(text);
    logger_.LogInfo() << kMethodsHeader << text;
}

void CMValidatorSubscriberActivity::Field1Subscription()
{
    logger_.LogInfo() << kFieldsHeader << "Field1Subscription()";
    if (!proxy_->Field1.IsSubscribed()) {
        // proxy_ got initialized via callback.
        LogStream logMsg{logger_.LogVerbose()};

        // Register event receive callback
        proxy_->Field1.SetReceiveHandler([this]() { CMValidatorSubscriberActivity::Field1Received(); });

        // subscribe to event
        auto subscription_result = proxy_->Field1.Subscribe(1);
        if (subscription_result.HasValue()) {
            logger_.LogInfo() << kFieldsHeader << "Callback registered.";
        } else {
            logger_.LogError() << kFieldsHeader << "Subscription failed with error: " << subscription_result.Error();
            return;
        }

        logger_.LogInfo() << kFieldsHeader << "done";
    } else {
        logger_.LogInfo() << kFieldsHeader << "already subscribed";
    }
}

void CMValidatorSubscriberActivity::Field2Subscription()
{
    logger_.LogInfo() << "Field2Subscription() called";

    if (!proxy_->Field2.IsSubscribed()) {
        // proxy_ got initialized via callback.
        LogStream logMsg{logger_.LogVerbose()};

        // Register event receive callback
        proxy_->Field2.SetReceiveHandler([this]() { CMValidatorSubscriberActivity::Field2Received(); });

        // set the subscription state change handler
        proxy_->Field2.SetSubscriptionStateChangeHandler(
            [this](ara::com::SubscriptionState substate) { subField2 = substate; });

        // subscribe to event
        auto subscription_result = proxy_->Field2.Subscribe(1);
        if (ara::com::SubscriptionState::kSubscribed == subField2) {
            logger_.LogInfo() << kFieldsHeader << "after Subscribe subField2 = kSubscribed";
        }

        if (subscription_result.HasValue()) {
            logger_.LogInfo() << kFieldsHeader << "Callback registered.";
        } else {
            logger_.LogError() << kFieldsHeader << "Subscription failed with error: " << subscription_result.Error();
            return;
        }
        logger_.LogInfo() << kFieldsHeader << "done";
    } else {
        // verify [SWS_CM_00334][SWS_CM_00026] Unset Subscription State change handler
        logger_.LogInfo() << kFieldsHeader << "already subscribed -> Call UnsetSubscriptionStateChangeHandler";
        proxy_->Field2.UnsetSubscriptionStateChangeHandler();
        if (ara::com::SubscriptionState::kSubscribed == subField2) {
            logger_.LogInfo() << kFieldsHeader << "after UnsetSubscriptionStateChangeHandler subField2 = kSubscribed";
        }
        logger_.LogInfo() << kFieldsHeader << "Set Unsubscribe";
        proxy_->Field2.Unsubscribe();
        // state is not updated after UnsetSubscriptionStateChangeHandler
        if (ara::com::SubscriptionState::kSubscribed == subField2) {
            logger_.LogInfo() << kFieldsHeader << "removed ChangeHandler and subField2 is still in kSubscribed status";
        }
    }
}

void CMValidatorSubscriberActivity::FieldGetter()
{
    logger_.LogInfo() << kFieldsHeader << "Trying to get values for Field1";
    LogStream logMsgUpdate{logger_.LogVerbose()};
    auto getField1Future = proxy_->Field1.Get();
    auto rField1 = getField1Future.GetResult();
    if (rField1.HasValue()) {
        auto value = rField1.Value();
        logMsgUpdate << kFieldsHeader << " Current Field1 is " << value;
    } else {
        // while field getters itself do not return errors, networking errors are still possible
        auto err = rField1.Error();
        logMsgUpdate << kErrorsHeader << "Code: " << err.Value();
        logMsgUpdate << kErrorsHeader << "Message: " << err.Message();
    }
}

void CMValidatorSubscriberActivity::FieldSetter()
{
    logger_.LogInfo() << kFieldsHeader << "Trying to set Field1";
    if (act_count_ % 7 == 0) {
        std::uint32_t valueToSet = 0;
        auto setResultFuture = proxy_->Field1.Set(valueToSet);
        auto r = setResultFuture.GetResult();
        if (r.HasValue()) {
            auto valueWhichWasSet = r.Value();
            logger_.LogInfo() << kFieldsHeader << "Set was successful: " << (valueWhichWasSet == valueToSet);
        } else {
            // while field setters itself do not return errors, networking errors are still possible
            auto err = r.Error();
            logger_.LogWarn() << kErrorsHeader << "Code: " << err.Value();
            logger_.LogWarn() << kErrorsHeader << "Message: " << err.Message();
        }
    } else {
        logger_.LogInfo() << kFieldsHeader << "Field1 setting skipped";
    }
}

void CMValidatorSubscriberActivity::Act()
{
    logger_.LogInfo() << "CMValidatorSubscriber alive";

    if (nullptr != proxy_) {
        if (proxy_->Event1.IsSubscribed()) {

            auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(proxy_->Event1);
            bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

            LogStream logMsg{logger_.LogVerbose()};

            logMsg << "Event1 E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
            logMsg.Flush();

            auto callback = [&logMsg](auto sample) {
                logMsg << "Polling: Event1 - Radar:";
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
            proxy_->Event1.GetNewSamples(callback);
        } else {
            logger_.LogInfo() << "not subscribed to Event1 yet";

            // proxy_ got initialized via callback.

            // subscribe to event
            auto subscription_result = proxy_->Event1.Subscribe(3);
            if (subscription_result.HasValue()) {
                logger_.LogInfo() << "Callback registered.";
            } else {
                logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
                return;
            }
            logger_.LogInfo() << "Event1 subscription complete";

            // Register event receive callback
            // validate [SWS_CM_00309]{DRAFT} Event Receive Handler
            // std::function<void()> provided as parameter
            proxy_->Event2.SetReceiveHandler([this]() { CMValidatorSubscriberActivity::Event2Received(); });
            logger_.LogInfo() << "Event2 SetReceiveHandler() complete";
            // subscribe to event
            // kNewN -> kNewestN
            subscription_result = proxy_->Event2.Subscribe(1);
            if (subscription_result.HasValue()) {
                logger_.LogInfo() << "Callback registered.";
            } else {
                logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
                return;
            }
            logger_.LogInfo() << "Event2 subscription complete";
        }

        logger_.LogInfo() << "Subscribe to Field1";
        this->Field1Subscription();

        logger_.LogInfo() << "Subscribe to Field2";
        this->Field2Subscription();

        logger_.LogInfo() << "Testing Field Getter.";
        this->FieldGetter();

        logger_.LogInfo() << "Testing Field Setter.";
        this->FieldSetter();

        // TODO: improve MethodCall
        // logger_.LogInfo() << "Testing Method Call.";
        // this->MethodCall();

        // TODO: improve FireAndForgetMethodCapability
        // logger_.LogInfo() << "Testing Fire And Forget Method Call.";
        // this->FireAndForgetMethodCapability();
    }
    act_count_++;
}
}  // namespace cmvalidatorsubscriber
