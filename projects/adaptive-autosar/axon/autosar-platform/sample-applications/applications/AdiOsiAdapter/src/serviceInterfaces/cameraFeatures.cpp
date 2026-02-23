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

#include "serviceInterfaces/cameraFeatures.h"
#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"

#include <thread>

// static int NUMBER_OF_INIT_ATTEMPTS = 3;

CameraFeatures::CameraFeatures(SensorDataMapper* sensorDataMapper)
    : m_cf_proxy_(nullptr)
    , sensorDataMapper_{sensorDataMapper}
{
    Init();
}

void CameraFeatures::Act()
{
    // After pullCapabilityVector method is included, the if condition can be as follows:

    // if (m_cf_proxy_is_initialized) {
    //     if (m_cf_proxy_->CameraFeatureInterfaceEvent.IsSubscribed()) {
    //         CameraFeaturesInterfaceEventIsSubscribed();
    //     } else {
    //         CameraFeaturesInterfaceEventIsNotSubscribed();
    //     }
    // } else {
    //     Init();
    // }

    if (m_cf_proxy_ != nullptr) {
        if (m_cf_proxy_->CameraFeatureInterfaceEvent.IsSubscribed()) {
            CameraFeaturesInterfaceEventIsSubscribed();
        } else {
            CameraFeaturesInterfaceEventIsNotSubscribed();
        }
    }
}

void CameraFeatures::Init()
{
    m_logger_.LogDebug() << "Init CameraFeatures Service\n";
    ara::core::InstanceSpecifier portSpecifier{
        "AdiOsiAdapter/AdiOsiAdapter/"
        "AdiOsiAdapter_RPort4"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        m_logger_.LogError() << "InstanceIdentifier for CameraFeatures port not resolved\n";
        return;
    }

    m_logger_.LogDebug() << "Service Discovery started.\n";

    auto res = CFProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<CFProxy::HandleType> handles, ara::com::FindServiceHandle handler) {
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
    //    m_cf_proxy_is_initialized = true;
    //    m_logger_.LogDebug() << "Finish CameraFeatures Init()";
    //} else {
    //    m_logger_.LogDebug() << "CameraFeatures Init failed";
    //}
}

void CameraFeatures::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<CFProxy::HandleType> handles)
{
    for (auto it : handles) {
        m_logger_.LogDebug() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() == 0) {
        m_logger_.LogDebug() << "handles.size() = " << handles.size();
        return;
    }

    std::lock_guard<std::mutex> lock(m_proxy_mutex_);
    if (nullptr != m_cf_proxy_) {
        m_logger_.LogDebug() << "proxy is already instantiated";
        return;
    }

    m_cf_proxy_ = std::make_shared<CFProxy>(handles.at(0));
    m_logger_.LogDebug() << "Created proxy from handle with instance: "
                         << m_cf_proxy_->GetHandle().GetInstanceId().ToString();
}

// The pullCapablityVector would be included once the Capability vector is implemented
// by the SmartCameraSensor app

// bool CameraFeatures::pullCapablityVector()
//{
//    bool result = false;
//    int attempts_counter = 0;
//
//    while (m_cf_proxy_ == nullptr) {
//        CFProxy::FindService(ara::com::InstanceIdentifier::MakeAny());
//        m_logger_.LogDebug() << "m_cf_proxy_ is empty";
//        std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//        attempts_counter++;
//        if (attempts_counter >= NUMBER_OF_INIT_ATTEMPTS) {
//            return result;
//        }
//    }
//
//    m_logger_.LogDebug() << "m_cf_proxy_ not empty";
//    auto capability_future = m_cf_proxy_->CameraFeaturesCapability();
//
//    while (!capability_future.is_ready()) {
//        // Results are not ready yet.
//        std::this_thread::sleep_for(std::chrono::milliseconds(500));
//        m_logger_.LogInfo() << "METHODS_HEADER: "
//                            << "Creating CameraFeatures Capability Vector wait ...";
//    }
//
//    auto r = capability_future.GetResult();
//
//    if (!r.HasValue()) {
//        m_logger_.LogWarn() << "Warning: empty CameraFeatures capability: ";
//        return result;
//    }
//
//    m_capVectorCameraFeatures_ = r.Value().capVector;
//
//    for (auto it = m_capVectorCameraFeatures_.begin(); it != m_capVectorCameraFeatures_.end(); it++) {
//
//        m_logger_.LogDebug() << "SCS CameraFeatures capability: " << *it;
//    }
//
//    result = true;
//    return result;
//}

void CameraFeatures::CameraFeaturesInterfaceEventIsSubscribed() const
{
    ReadCameraFeaturesInterfaceEventData();
}

void CameraFeatures::CameraFeaturesInterfaceEventIsNotSubscribed() const
{
    m_logger_.LogInfo() << "not subscribed to CameraFeatureInterfaceEvent yet";

    // subscribe to event
    auto subscription_result = m_cf_proxy_->CameraFeatureInterfaceEvent.Subscribe(1);
    if (subscription_result.HasValue()) {
        // m_cf_proxy_->CameraFeatureInterfaceEvent.SetReceiveHandler(
        //    [this]() { CameraFeatures::CameraFeaturesInterfaceEventReceived(); });
        m_logger_.LogInfo() << "Callback registered.";
        m_logger_.LogInfo() << "CameraFeatureInterfaceEvent subscription complete";
    } else {
        m_logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
    }
}

// void CameraFeatures::CameraFeaturesInterfaceEventReceived() const
//{
//    ReadCameraFeaturesInterfaceEventData();
//}

void CameraFeatures::ReadCameraFeaturesInterfaceEventData() const
{
    m_logger_.LogDebug() << "Event IsSubscribed";

    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(m_cf_proxy_->CameraFeatureInterfaceEvent);
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    ara::log::LogStream logMsg{m_logger_.LogVerbose()};

    logMsg << "CameraFeatureInterfaceEvent E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    ara::adi::sensoritf::CameraFeatureInterface cameraFeatureInterface{};
    auto callback = [&logMsg, &cameraFeatureInterface](auto sample) {
        logMsg << "Polling: CameraFeatureInterfaceEvent - camera:";
        // always returns NotAvailable if E2E is disabled

        if (sample->CameraFeatureInterfaceHeader.InformationInterface.NumberOfValidServingSensors == 0) {
            logMsg << "NOT";
        }
        logMsg << "active";

        cameraFeatureInterface = *sample;

        auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
        bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
        logMsg << " E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
        logMsg.Flush();
    };
    // execute callback for every samples in the context of GetNewSamples
    auto results = m_cf_proxy_->CameraFeatureInterfaceEvent.GetNewSamples(callback);

    if (results.HasValue()) {
        if (results.Value()) {
            m_logger_.LogInfo() << "NumberOfValidCameraFeatures: " << cameraFeatureInterface.NumberOfValidFeatures;

            // insert sensorDataMapper
            sensorDataMapper_->setCameraFeatureInterface(cameraFeatureInterface);
        }
    }
}

// inline ara::log::LogStream& operator<<(ara::log::LogStream& out, const std::uint8_t& value)
//{
//    std::ostringstream stream;
//    stream << "RoadObjectLaneAssociation(" << value << ")";
//    return (out << stream.str().c_str());
//}
