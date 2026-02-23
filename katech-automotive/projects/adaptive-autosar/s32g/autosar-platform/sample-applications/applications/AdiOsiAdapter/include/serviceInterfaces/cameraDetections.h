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

#ifndef ADI_OSI_ADAPTER_CAMERADETECTIONS_H_
#define ADI_OSI_ADAPTER_CAMERADETECTIONS_H_

#include <mutex>
#include <string>
#include <random>

#include "ara/adi/sensoritf/cameradetectionsservice_proxy.h"
#include "ara/log/logger.h"

#include "osiMappers/CameraFeatureDataMapper.h"
#include "osiMappers/SensorDataMapper.h"

#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_datarecording.pb.h"
#include "osi3/osi_featuredata.pb.h"

class CameraDetections
{
    using CDProxy = ara::adi::sensoritf::proxy::CameraDetectionsServiceProxy;

public:
    /// @brief Start service discovery
    CameraDetections(SensorDataMapper* sensorDataMapper);

    ara::adi::sensoritf::CameraDetectionsInterface getCameraDetectionInterfaceValue() const;

    /// @brief Store the received cameraDetectionInterface value
    /// @param cameraDetectionsInterface
    void setCameraDetectionInterfaceValue(
        const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface);

    /// @brief Check subscription to CameraDetections Event
    void Act();

private:
    /// @brief Service Discovery on the specified port
    void Init();

    /// @brief  Check service instance is available or not and use the available proxy instance
    /// @param handles
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<CDProxy::HandleType> handles);

    // bool pullCapablityVector();

    /// @brief Access the StaticObjectEvent contents
    void CameraDetectionsEventIsSubscribed();

    void CameraDetectionsEventIsNotSubscribed();

    // void CameraDetectionsEventReceived();

    void ReadCameraDetectionsEventData();

private:
    ara::adi::sensoritf::CameraDetectionsInterface cameraDetectionsInterface_;
    std::shared_ptr<CDProxy> m_cd_proxy_;
    bool m_cd_proxy_is_initialized{false};
    std::mutex m_proxy_mutex_;
    ara::core::Vector<bool> m_capVectorCameraDetections_{};
    static const std::uint16_t m_kSizeCapVectorCameraDetections_ = 87;
    SensorDataMapper* sensorDataMapper_{};

    ara::log::Logger& m_logger_{
        ara::log::CreateLogger("AOAD", "AdiOsiAdapter Appl--> CameraDetectionsService", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_CAMERADETECTIONS_H_
