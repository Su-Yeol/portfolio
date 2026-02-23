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

#ifndef GENERICADASAPPLICATION_RADARDETECTIONS_H_
#define GENERICADASAPPLICATION_RADARDETECTIONS_H_

#include <mutex>
#include <string>
#include <random>

#include "ara/adi/sensoritf/radardetectionsservice_proxy.h"
#include "ara/log/logger.h"

class RadarDetections
{
    using RDProxy = ara::adi::sensoritf::proxy::RadarDetectionsServiceProxy;

public:
    /// @brief Start service discovery
    RadarDetections();

    ara::adi::sensoritf::RadarDetectionsInterface radarDetectionsInterfaceValue;

    /// @brief Store the received radarDetectionsInterface value
    /// @param radarDetectionsInterface
    void setRadarDetectionsInterfaceValue(
        const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface);

    /// @brief Check subscription to RadarDetections Event
    void Act();

private:
    /// @brief Service Discovery on the specified port
    void Init();

    /// @brief  Check service instance is available or not and use the available proxy instance
    /// @param handles
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<RDProxy::HandleType> handles);

#ifdef CAPABILITY_VECTOR
    bool pullCapablityVector();
#endif

    /// @brief Access the StaticObjectEvent contents
    void RadarDetectionsInterfaceEventIsSubscribed();

    void RadarDetectionsInterfaceEventIsNotSubscribed();

    void ReadRadarDetectionsInterfaceEventData();

private:
    std::shared_ptr<RDProxy> m_rd_proxy_;
    bool m_rd_proxy_is_initialized{false};
    std::mutex m_proxy_mutex_;
    ara::core::Vector<bool> m_capVectorRadarDetections_{};
    static const std::uint16_t m_kSizeCapVectorRadarDetections_ = 87;

    ara::log::Logger& m_logger_{ara::log::CreateLogger("GARD",
        "GenericAdasApplication Appl--> RadarDetectionsService",
        ara::log::LogLevel::kVerbose)};
};

#endif  // GENERICADASAPPLICATION_RADARDETECTIONS_H_
