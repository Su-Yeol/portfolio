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

RoadObjects::RoadObjects(SensorDataMapper* sensorDataMapper)
    : m_proxy_(nullptr)
    , sensorDataMapper_(sensorDataMapper)
{
    Init();
}

void RoadObjects::Act()
{
    // After pullCapabilityVector method is included, the if condition can be as follows:

    // if (m_proxy_is_initialized) {
    //    if (m_proxy_->RoadObjectInterfaceEvent.IsSubscribed()) {
    //        RoadObjectsInterfaceEventIsSubscribed();
    //    } else {
    //        RoadObjectsInterfaceEventIsNotSubscribed();
    //    }
    //} else {
    //    Init();
    //}

    if (nullptr != m_proxy_) {
        if (m_proxy_->RoadObjectInterfaceEvent.IsSubscribed()) {
            RoadObjectsInterfaceEventIsSubscribed();
        } else {
            RoadObjectsInterfaceEventIsNotSubscribed();
        }
    }
}

void RoadObjects::Init()
{
    m_logger_.LogDebug() << "Init RoadObjects Service\n";
    ara::core::InstanceSpecifier portSpecifier{"AdiOsiAdapter/AdiOsiAdapter/AdiOsiAdapter_RPort"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger_.LogError() << "InstanceIdentifier for RoadObjects port not resolved\n";
        return;
    }

    m_logger_.LogDebug() << "Service Discovery started.\n";

    auto res = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            static_cast<void>(handler);
            serviceAvailabilityCallback(std::move(handles));
        },
        portSpecifier);

    if (!res) {
        throw std::runtime_error{"StartFindService failed"};
    }

    // The pullCapablityVector would be included once the Capability vector is implemented
    // by the SmartCameraSensor app

    // bool capVectorPulled = pullCapablityVector();
    // if (capVectorPulled) {
    //    m_proxy_is_initialized = true;
    //    m_logger_.LogDebug() << "Finish RoadObjects Init()";
    //} else {
    //    m_logger_.LogDebug() << "RoadObjects Init failed";
    //}
}

void RoadObjects::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles)
{
    for (auto it : handles) {
        m_logger_.LogDebug() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() == 0) {
        m_logger_.LogDebug() << "handles.size() = " << handles.size();
        return;
    }

    std::lock_guard<std::mutex> lock(m_proxy_mutex_);
    if (nullptr != m_proxy_) {
        m_logger_.LogDebug() << "proxy is already instantiated";
        return;
    }

    m_proxy_ = std::make_shared<Proxy>(handles.at(0));
    m_logger_.LogDebug() << "Created proxy from handle with instance: "
                         << m_proxy_->GetHandle().GetInstanceId().ToString();
}

// The pullCapablityVector would be included once the Capability vector is implemented
// by the SmartCameraSensor app

// bool RoadObjects::pullCapablityVector()
//{
//    bool result = false;
//    int attempts_counter = 0;
//
//    while (m_proxy_ == nullptr) {
//        Proxy::FindService(ara::com::InstanceIdentifier::MakeAny());
//        m_logger_.LogDebug() << "m_proxy_ is empty";
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//        attempts_counter++;
//        if (attempts_counter >= NUMBER_OF_INIT_ATTEMPTS) {
//            return result;
//        }
//    }
//
//    m_logger_.LogDebug() << "m_proxy_ not empty";
//    auto capability_future = m_proxy_->RoadObjectsCapability();
//
//    while (!capability_future.is_ready()) {
//        // Results are not ready yet.
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        m_logger_.LogInfo() << "METHODS_HEADER: "
//                            << "Creating RoadObjects Capability Vector wait ...";
//    }
//
//    auto r = capability_future.GetResult();
//
//    if (!r.HasValue()) {
//        m_logger_.LogWarn() << "Warning: empty RoadObjects capability: ";
//        return result;
//    }
//
//    m_capVectorRoadObjects_ = r.Value().capVector;
//
//    for (auto it = m_capVectorRoadObjects_.begin(); it != m_capVectorRoadObjects_.end(); it++) {
//        m_logger_.LogDebug() << "SCS RoadObjects capability: " << *it;
//    }
//
//    result = true;
//    return result;
//}

void RoadObjects::RoadObjectsInterfaceEventIsSubscribed() const
{
    ReadRoadObjectsInterfaceEventData();
}

void RoadObjects::RoadObjectsInterfaceEventIsNotSubscribed() const
{
    m_logger_.LogInfo() << "not subscribed to RoadObjectInterfaceEvent yet";

    // subscribe to event
    auto subscription_result = m_proxy_->RoadObjectInterfaceEvent.Subscribe(1);
    if (subscription_result.HasValue()) {
        // m_proxy_->RoadObjectInterfaceEvent.SetReceiveHandler(
        //    [this]() { RoadObjects::RoadObjectsInterfaceEventReceived(); });
        m_logger_.LogInfo() << "Callback registered.";
        m_logger_.LogInfo() << "RoadObjectInterfaceEvent subscription complete";
    } else {
        m_logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
    }
}

// void RoadObjects::RoadObjectsInterfaceEventReceived() const
//{
//    ReadRoadObjectsInterfaceEventData();
//}

void RoadObjects::ReadRoadObjectsInterfaceEventData() const
{
    m_logger_.LogDebug() << "Event IsSubscribed";

    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_proxy_->RoadObjectInterfaceEvent);
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

    auto results = m_proxy_->RoadObjectInterfaceEvent.GetNewSamples(callback);

    if (results.HasValue()) {
        if (results.Value()) {
            // Insert into sensorDataMapper
            m_logger_.LogInfo() << "ROI NumOfRoadMarkings: "
                                << roadObjectInterface.RoadMarkingObjectList.NumberOfValidRoadMarkings;
            sensorDataMapper_->setRoadObjectInterface(roadObjectInterface);
        }
    }
}

// inline ara::log::LogStream& operator<<(ara::log::LogStream& out, const std::uint8_t& value)
//{
//    std::ostringstream stream;
//    stream << "RoadObjectLaneAssociation(" << value << ")";
//    return (out << stream.str().c_str());
//}
