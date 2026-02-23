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

#include "serviceInterfaces/staticObjects.h"
#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"
#include <thread>

// static int NUMBER_OF_INIT_ATTEMPTS = 3;

StaticObjects::StaticObjects()
    : m_static_proxy_(nullptr)
{
    Init();
}

void StaticObjects::Act()
{
    // TODO: Define the CAPABILITY_VECTOR once it is fully implemented

#ifndef CAPABILITY_VECTOR
    if (nullptr != m_static_proxy_) {
        if (m_static_proxy_->StaticObjectInterfaceEvent.IsSubscribed()) {
            StaticObjectsInterfaceEventIsSubscribed();
        } else {
            StaticObjectsInterfaceEventIsNotSubscribed();
        }
    }
#else
    if (m_static_proxy_is_initialized) {
        if (m_static_proxy_->StaticObjectInterfaceEvent.IsSubscribed()) {
            StaticObjectsInterfaceEventIsSubscribed();
        } else {
            StaticObjectsInterfaceEventIsNotSubscribed();
        }
    } else {
        Init();
    }
#endif
}

void StaticObjects::Init()
{
    m_logger_.LogDebug() << "Init StaticObjects Service\n";
    ara::core::InstanceSpecifier portSpecifier{
        "GenericAdasApplication/GenericAdasApplication/GenericAdasApplication_RPort2"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger_.LogError() << "InstanceIdentifier for StaticObjects port not resolved\n";
        return;
    }

    m_logger_.LogDebug() << "Service Discovery started.\n";

    auto res = StaticProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<StaticProxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            static_cast<void>(handler);
            serviceAvailabilityCallback(std::move(handles));
        },
        portSpecifier);

    if (!res) {
        throw std::runtime_error{"StartFindService failed"};
    }

    // TODO: Define the CAPABILITY_VECTOR once it is fully implemented

#ifdef CAPABILITY_VECTOR
    bool capVectorPulled = pullCapablityVector();
    if (capVectorPulled) {
        m_static_proxy_is_initialized = true;
        m_logger_.LogDebug() << "Finish StaticObjects Init()";
    } else {
        m_logger_.LogDebug() << "StaticObjects Init failed";
    }
#endif
}

void StaticObjects::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<StaticProxy::HandleType> handles)
{
    for (auto it : handles) {
        m_logger_.LogDebug() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() == 0) {
        m_logger_.LogDebug() << "handles.size() = " << handles.size();
        return;
    }

    std::lock_guard<std::mutex> lock(m_proxy_mutex_);
    if (nullptr != m_static_proxy_) {
        m_logger_.LogDebug() << "proxy is already instantiated";
        return;
    }

    m_static_proxy_ = std::make_shared<StaticProxy>(handles.at(0));
    m_logger_.LogDebug() << "Created proxy from handle with instance: "
                         << m_static_proxy_->GetHandle().GetInstanceId().ToString();
}

// TODO: Define the CAPABILITY_VECTOR once it is fully implemented

#ifdef CAPABILITY_VECTOR
bool StaticObjects::pullCapablityVector()
{
    bool result = false;
    int attempts_counter = 0;

    while (m_static_proxy_ == nullptr) {
        StaticProxy::FindService(ara::com::InstanceIdentifier::MakeAny());
        m_logger_.LogDebug() << "m_static_proxy_ is empty";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        attempts_counter++;
        if (attempts_counter >= NUMBER_OF_INIT_ATTEMPTS) {
            return result;
        }
    }

    m_logger_.LogDebug() << "m_static_proxy_ not empty";
    auto capability_future = m_static_proxy_->StaticObjectsCapability();

    while (!capability_future.is_ready()) {
        // Results are not ready yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_logger_.LogInfo() << "METHODS_HEADER: "
                            << "Creating StaticObjects Capability Vector wait ...";
    }

    auto r = capability_future.GetResult();

    if (!r.HasValue()) {
        m_logger_.LogWarn() << "Warning: empty StaticObjects capability: ";
        return result;
    }

    m_capVectorStaticObjects_ = r.Value().capVector;

    for (auto it = m_capVectorStaticObjects_.begin(); it != m_capVectorStaticObjects_.end(); it++) {
        m_logger_.LogDebug() << "GAA StaticObjects capability: " << *it;
    }

    result = true;
    return result;
}
#endif

void StaticObjects::StaticObjectsInterfaceEventIsSubscribed()
{
    ReadStaticObjectsInterfaceEventData();
}

void StaticObjects::StaticObjectsInterfaceEventIsNotSubscribed()
{
    m_logger_.LogInfo() << "not subscribed to StaticObjectInterfaceEvent yet";

    // subscribe to event
    auto subscription_result = m_static_proxy_->StaticObjectInterfaceEvent.Subscribe(1);
    if (subscription_result.HasValue()) {
        m_logger_.LogInfo() << "Callback registered.";
        m_logger_.LogInfo() << "StaticObjectInterfaceEvent subscription complete";
    } else {
        m_logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
    }
}

void StaticObjects::ReadStaticObjectsInterfaceEventData()
{
    m_logger_.LogDebug() << "Event IsSubscribed";

    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_static_proxy_->StaticObjectInterfaceEvent);
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    ara::log::LogStream logMsg{m_logger_.LogVerbose()};

    logMsg << "StaticObjectInterfaceEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    ara::adi::sensoritf::StaticObjectInterface staticObjectsInterface{};
    auto callback = [&logMsg, &staticObjectsInterface](auto sample) {
        logMsg << "Polling: StaticObjectInterfaceEvent - camera:";
        // always returns NotAvailable if E2E is disabled

        // RoadObjectInterfaceHeader for StaticObjects as well
        if (sample->StaticObjectInterfaceHeader.InformationInterface.NumberOfValidServingSensors == 0) {
            logMsg << "NOT";
        }
        logMsg << "active";

        staticObjectsInterface = *sample;

        auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
        bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
        logMsg << " E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
        logMsg.Flush();
    };
    // execute callback for every samples in the context of GetNewSamples
    auto results = m_static_proxy_->StaticObjectInterfaceEvent.GetNewSamples(callback);

    if (results.HasValue()) {
        if (results.Value()) {
            setStaticObjectInterfaceValue(staticObjectsInterface);
        }
    }
}

void StaticObjects::setStaticObjectInterfaceValue(
    const ara::adi::sensoritf::StaticObjectInterface& staticObjectInterface)
{
    m_logger_.LogInfo() << "Setting StaticObjectInterfaceValue";
    this->staticObjectInterfaceValue = staticObjectInterface;
}
