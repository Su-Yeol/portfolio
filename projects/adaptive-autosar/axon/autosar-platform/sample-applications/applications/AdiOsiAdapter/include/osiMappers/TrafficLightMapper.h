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

#ifndef ADI_OSI_ADAPTER_TRAFFICLIGHT_STATICOBJECTS_MAPPING_HPP_
#define ADI_OSI_ADAPTER_TRAFFICLIGHT_STATICOBJECTS_MAPPING_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/impl_type_trafficlight.h"

#include "osi3/osi_trafficlight.pb.h"

#include "GeneralHeaderMapper.h"

using TLClassificationIcon = osi3::TrafficLight_Classification_Icon;
using LSClassificationType = ::ara::adi::sensoritf::LightShapeClassificationType;

class TrafficLightMapper : public GeneralHeaderMapper
{
public:
    TrafficLightMapper() = default;

public:
    osi3::TrafficLight mapTrafficLight(const ara::adi::sensoritf::TrafficLight& trafficLight,
        const size_t& trafficLightSpotsIndex) const;

protected:
    osi3::BaseStationary mapBaseStationary(const ara::adi::sensoritf::TrafficLight& trafficLight,
        const std::size_t& trafficLightSpotsIndex) const;

    osi3::TrafficLight_Classification mapClassification(const ::ara::adi::sensoritf::TrafficLightSpot& tlSpot,
        const std::uint16_t& classIndex) const;

private:
    osi3::TrafficLight_Classification_Color mapColour(
        const ::ara::adi::sensoritf::ColourClassificationType& colourClassificationType) const;
    osi3::TrafficLight_Classification_Icon mapIcon(
        const ::ara::adi::sensoritf::LightShapeClassificationType& lightShapeClassificationType) const;
    osi3::TrafficLight_Classification_Mode mapMode(
        const ::ara::adi::sensoritf::LightModeClassificationType& lightModeClassificationType) const;

    static std::map<LSClassificationType, TLClassificationIcon> mapClassificationIcon;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("TLM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_TRAFFICLIGHT_STATICOBJECTS_MAPPING_HPP_
