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

#include "serviceInterfaces/roadObjects.h"
#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"
#include <thread>

// static int NUMBER_OF_INIT_ATTEMPTS = 3;

RoadObjects::RoadObjects()
    : m_ro_proxy_(nullptr)
{
    Init();
}

void RoadObjects::Act()
{
    // TODO: Define the CAPABILITY_VECTOR once it is fully implemented

#ifndef CAPABILITY_VECTOR
    if (nullptr != m_ro_proxy_) {
        if (m_ro_proxy_->RoadObjectInterfaceEvent.IsSubscribed()) {
            RoadObjectsEventIsSubscribed();
        } else {
            RoadObjectsEventIsNotSubscribed();
        }
    }
#else
    if (m_ro_proxy_is_initialized) {
        if (m_ro_proxy_->RoadObjectInterfaceEvent.IsSubscribed()) {
            RoadObjectsEventIsSubscribed();
        } else {
            RoadObjectsEventIsNotSubscribed();
        }
    } else {
        Init();
    }
#endif
}

void RoadObjects::Init()
{
    m_logger_.LogDebug() << "Init RoadObjects Service\n";
    ara::core::InstanceSpecifier portSpecifier{
        "GenericAdasApplication/GenericAdasApplication/GenericAdasApplication_RPort"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger_.LogError() << "InstanceIdentifier for RoadObjects port not resolved\n";
        return;
    }

    m_logger_.LogDebug() << "Service Discovery started.\n";

    auto res = ROSProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<ROSProxy::HandleType> handles, ara::com::FindServiceHandle handler) {
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
        m_ro_proxy_is_initialized = true;
        m_logger_.LogDebug() << "Finish RoadObjects Init()";
    } else {
        m_logger_.LogDebug() << "RoadObjects Init failed";
    }
#endif
}

void RoadObjects::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<ROSProxy::HandleType> handles)
{
    for (auto it : handles) {
        m_logger_.LogDebug() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() == 0) {
        m_logger_.LogDebug() << "handles.size() = " << handles.size();
        return;
    }

    std::lock_guard<std::mutex> lock(m_proxy_mutex_);
    if (nullptr != m_ro_proxy_) {
        m_logger_.LogDebug() << "proxy is already instantiated";
        return;
    }

    m_ro_proxy_ = std::make_shared<ROSProxy>(handles.at(0));
    m_logger_.LogDebug() << "Created ROSProxy from handle with instance: "
                         << m_ro_proxy_->GetHandle().GetInstanceId().ToString();
}

// TODO: Define the CAPABILITY_VECTOR once it is fully implemented

#ifdef CAPABILITY_VECTOR
bool RoadObjects::pullCapablityVector()
{
    bool result = false;
    int attempts_counter = 0;

    while (m_ro_proxy_ == nullptr) {
        ROSProxy::FindService(ara::com::InstanceIdentifier::MakeAny());
        m_logger_.LogDebug() << "m_ro_proxy_ is empty";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        attempts_counter++;
        if (attempts_counter >= NUMBER_OF_INIT_ATTEMPTS) {
            return result;
        }
    }

    m_logger_.LogDebug() << "m_ro_proxy_ not empty";
    auto capability_future = m_ro_proxy_->RoadObjectsCapability();

    while (!capability_future.is_ready()) {
        // Results are not ready yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_logger_.LogInfo() << "METHODS_HEADER: "
                            << "Creating RoadObjects Capability Vector wait ...";
    }

    auto r = capability_future.GetResult();

    if (!r.HasValue()) {
        m_logger_.LogWarn() << "Warning: empty RoadObjects capability: ";
        return result;
    }

    m_capVectorRoadObjects_ = r.Value().capVector;

    for (auto it = m_capVectorRoadObjects_.begin(); it != m_capVectorRoadObjects_.end(); it++) {
        m_logger_.LogDebug() << "SCS RoadObjects capability: " << *it;
    }

    result = true;
    return result;
}
#endif

void RoadObjects::RoadObjectsEventIsSubscribed()
{
    ReadRoadObjectsEventData();
}

void RoadObjects::RoadObjectsEventIsNotSubscribed()
{
    m_logger_.LogInfo() << "not subscribed to RoadObjectInterfaceEvent yet";

    // subscribe to event
    auto subscription_result = m_ro_proxy_->RoadObjectInterfaceEvent.Subscribe(1);

    if (subscription_result.HasValue()) {
        m_logger_.LogInfo() << "Callback registered.";
        m_logger_.LogInfo() << "RoadObjectInterfaceEvent subscription complete";
    } else {
        m_logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
    }
}

void RoadObjects::ReadRoadObjectsEventData()
{
    m_logger_.LogDebug() << "Event IsSubscribed";

    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_ro_proxy_->RoadObjectInterfaceEvent);
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    ara::log::LogStream logMsg{m_logger_.LogVerbose()};

    logMsg << "RoadObjectInterfaceEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    ara::adi::sensoritf::RoadObjectInterface roadObjectInterface{};
    auto callback = [&logMsg, &roadObjectInterface](auto sample) {
        logMsg << "Polling: RoadObjectInterfaceEvent - camera:";
        // always returns NotAvailable if E2E is disabled

        if (0 == sample->RoadObjectInterfaceHeader.InformationInterface.NumberOfValidServingSensors) {
            logMsg << "NOT";
        }
        logMsg << "active";

        roadObjectInterface = *sample;

        auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
        bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
        logMsg << " E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
        logMsg.Flush();
    };
    // execute callback for every samples in the context of GetNewSamples

    auto results = m_ro_proxy_->RoadObjectInterfaceEvent.GetNewSamples(callback);

    if (results.HasValue()) {
        if (results.Value()) {
            setRoadObjectsInterfaceValue(roadObjectInterface);
            // logMsg << "ROI NumOfRoadMarkings: " <<
            // roadObjectInterface.RoadMarkingObjectList.NumberOfValidRoadMarkings;
        }
    }
}

void RoadObjects::setRoadObjectsInterfaceValue(const ara::adi::sensoritf::RoadObjectInterface& roadObjectInterface)
{
    m_logger_.LogInfo() << "Setting RoadObjectsInterfaceValue";
    this->roadObjectsInterfaceValue = roadObjectInterface;
}
