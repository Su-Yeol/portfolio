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

#ifndef ADI_OSI_ADAPTER_ROADOBJECTS_HPP_
#define ADI_OSI_ADAPTER_ROADOBJECTS_HPP_

#include <mutex>
#include <string>
#include <random>

#include "ara/adi/sensoritf/roadobjectsservice_proxy.h"
#include "ara/log/logger.h"

#include "osiMappers/SensorDataMapper.h"

class RoadObjects
{
    using Proxy = ara::adi::sensoritf::proxy::RoadObjectsServiceProxy;

public:
    /// @brief Start service discovery
    RoadObjects(SensorDataMapper* sensorDataMapper);

    /// @brief Check subscription to RoadObject Event
    void Act();

private:
    /// @brief Service Discovery on the specified port
    void Init();

    /// @brief  Check service instance is available or not and use the available proxy instance
    /// @param handles
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles);

    // bool pullCapablityVector();

    /// @brief Access the RoadObjectEvent contents
    void RoadObjectsInterfaceEventIsSubscribed() const;

    void RoadObjectsInterfaceEventIsNotSubscribed() const;

    // void RoadObjectsInterfaceEventReceived() const;

    void ReadRoadObjectsInterfaceEventData() const;

    std::shared_ptr<Proxy> m_proxy_;
    bool m_proxy_is_initialized{false};
    std::mutex m_proxy_mutex_;
    ara::core::Vector<bool> m_capVectorRoadObjects_{};
    static const std::uint16_t m_kSizeCapVectorRoadObjects_ = 173;
    ara::adi::sensoritf::RoadObjectInterface roadObjectsInterface_{};
    SensorDataMapper* sensorDataMapper_{};

    ara::log::Logger& m_logger_{
        ara::log::CreateLogger("AOAR", "AdiOsiAdapter Appl--> RoadObjectsService", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_ROADOBJECTS_HPP_
