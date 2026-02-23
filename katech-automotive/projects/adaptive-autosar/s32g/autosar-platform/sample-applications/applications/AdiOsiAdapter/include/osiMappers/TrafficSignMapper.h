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

#ifndef ADI_OSI_ADAPTER_TRAFFICSIGN_STATICOBJECTS_MAPPING_HPP_
#define ADI_OSI_ADAPTER_TRAFFICSIGN_STATICOBJECTS_MAPPING_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/impl_type_trafficsign.h"

#include "osi3/osi_trafficsign.pb.h"

#include "GeneralHeaderMapper.h"

using osiTSValueUnit = osi3::TrafficSignValue_Unit;
using TSValueUnit = ::ara::adi::sensoritf::SignValueUnit;

class TrafficSignMapper : public GeneralHeaderMapper
{
public:
    TrafficSignMapper() = default;

public:
    osi3::TrafficSign mapTrafficSign(const ara::adi::sensoritf::TrafficSign& trafficSign) const;
    osi3::TrafficSign_MainSign_Classification_Type mapMainSignClassificationType(
        const ::ara::adi::sensoritf::SignClassificationType& signClassificationType) const;
    osi3::TrafficSignValue_Unit mapTrafficSignValueUnit(const ara::adi::sensoritf::SignValueUnit& signValueUnit) const;
    std::pair<osi3::TrafficSign_Variability, bool> mapTSVariabilityandServiceState(
        const ::ara::adi::sensoritf::SignState& signState) const;

protected:
    osi3::TrafficSign_MainSign_Classification mapTrafficSignMainClassification(
        const ara::adi::sensoritf::MainSignClassification& mainSignClassification) const;
    osi3::BaseStationary mapMainBaseStationary(const ara::adi::sensoritf::TrafficSign& trafficSign) const;
    osi3::TrafficSign_SupplementarySign_Classification mapTrafficSignSupplementaryClassification(
        const ara::adi::sensoritf::SupplementarySignClassification& supplSignClassification) const;
    osi3::BaseStationary mapSupplementaryBaseStationary(const ara::adi::sensoritf::TrafficSign& trafficSign,
        const std::size_t& index) const;

private:
    osi3::TrafficSign_MainSign mapTsMainSign(const ara::adi::sensoritf::TrafficSign& trafficSign) const;
    osi3::TrafficSign_SupplementarySign mapTsSupplementarySign(
        const ara::adi::sensoritf::TrafficSign& trafficSign) const;
    osi3::TrafficSign_SupplementarySign_Classification_Type mapSupplSignClassificationType(
        const ara::adi::sensoritf::SupplementarySignClassificationType& supplSignClassificationType) const;

    static std::map<TSValueUnit, osiTSValueUnit> TrafficSignValueUnitMap;
    ara::log::Logger& m_logger{ara::log::CreateLogger("TSM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_TRAFFICSIGN_STATICOBJECTS_MAPPING_HPP_
