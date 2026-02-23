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

#include "ara/log/logger.h"

#include "osiMappers/TrafficSignMapper.h"
#include "osiMappers/GeneralHeaderMapper.h"

#include "osi3/osi_trafficsign.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_common.pb.h"

#include <algorithm>

std::map<TSValueUnit, osiTSValueUnit> TrafficSignMapper::TrafficSignValueUnitMap{
    {TSValueUnit::kDay, osiTSValueUnit::TrafficSignValue_Unit_UNIT_DAY},
    {TSValueUnit::kFeet, osiTSValueUnit::TrafficSignValue_Unit_UNIT_FEET},
    {TSValueUnit::kHour, osiTSValueUnit::TrafficSignValue_Unit_UNIT_OTHER},
    {TSValueUnit::kKilometrePerHour, osiTSValueUnit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR},
    {TSValueUnit::kKilometre, osiTSValueUnit::TrafficSignValue_Unit_UNIT_KILOMETER},
    {TSValueUnit::kLongTon, osiTSValueUnit::TrafficSignValue_Unit_UNIT_LONG_TON},
    {TSValueUnit::kMeter, osiTSValueUnit::TrafficSignValue_Unit_UNIT_METER},
    {TSValueUnit::kMetricTon, osiTSValueUnit::TrafficSignValue_Unit_UNIT_METRIC_TON},
    {TSValueUnit::kMile, osiTSValueUnit::TrafficSignValue_Unit_UNIT_MILE},
    {TSValueUnit::kMilePerHour, osiTSValueUnit::TrafficSignValue_Unit_UNIT_MILE_PER_HOUR},
    {TSValueUnit::kMinute, osiTSValueUnit::TrafficSignValue_Unit_UNIT_MINUTES},
    {TSValueUnit::kPercentage, osiTSValueUnit::TrafficSignValue_Unit_UNIT_PERCENTAGE},
    {TSValueUnit::kShortTon, osiTSValueUnit::TrafficSignValue_Unit_UNIT_SHORT_TON},
    {TSValueUnit::kUnknown, osiTSValueUnit::TrafficSignValue_Unit_UNIT_UNKNOWN},
    {TSValueUnit::kWeekday, osiTSValueUnit::TrafficSignValue_Unit_UNIT_NO_UNIT}};

osi3::TrafficSign TrafficSignMapper::mapTrafficSign(const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::TrafficSign osiTrafficSign;

    osi3::TrafficSign_SupplementarySign* ptrOsiTrafficSignSupplementarySign;

    // Map Identifier

    // TODO: Objectid type mismatch
    std::uint16_t trafficSignid = trafficSign.TrafficSignsStatus.ObjectID;
    osi3::Identifier* osiTsid{osiTrafficSign.mutable_id()};
    osiTsid->set_value(static_cast<uint64_t>(trafficSignid));

    // Map Main Traffic Sign
    osi3::TrafficSign_MainSign* osiTrafficSignMainSign{osiTrafficSign.mutable_main_sign()};
    *osiTrafficSignMainSign = mapTsMainSign(trafficSign);

    // Map Supplementary Sign
    ptrOsiTrafficSignSupplementarySign = osiTrafficSign.add_supplementary_sign();
    *ptrOsiTrafficSignSupplementarySign = mapTsSupplementarySign(trafficSign);

    return osiTrafficSign;
}

osi3::TrafficSign_MainSign TrafficSignMapper::mapTsMainSign(const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::TrafficSign_MainSign osiTrafficSignMainSign;

    // Map Main Base Stationary
    osi3::BaseStationary* osiBaseStationary{osiTrafficSignMainSign.mutable_base()};
    *osiBaseStationary = TrafficSignMapper::mapMainBaseStationary(trafficSign);

    // Map Main Traffic Sign Classification
    // TODO: ValidMainSignClassificationsList is a vector contained in one TrafficSign
    ara::adi::sensoritf::MainSignClassification mainSignClassification
        = trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList.at(0);
    osi3::TrafficSign_MainSign_Classification* osiTsMainSignClassification{
        osiTrafficSignMainSign.mutable_classification()};
    *osiTsMainSignClassification = TrafficSignMapper::mapTrafficSignMainClassification(mainSignClassification);

    return osiTrafficSignMainSign;
}

osi3::TrafficSign_MainSign_Classification TrafficSignMapper::mapTrafficSignMainClassification(
    const ara::adi::sensoritf::MainSignClassification& mainSignClassification) const
{
    // initialize osi parameters
    osi3::TrafficSign_MainSign_Classification osiTsMainSignClassification{};
    osi3::TrafficSign_MainSign_Classification_Type osiTsMainSignClassificationType{};
    osi3::TrafficSign_Variability osiTsVariability{};
    bool osiTsIsOutOfService{};
    osi3::TrafficSignValue* osiTsValue{osiTsMainSignClassification.mutable_value()};
    osi3::TrafficSignValue_Unit osiTsValueUnit{};

    // map and assign values to osi

    // type
    osiTsMainSignClassificationType = mapMainSignClassificationType(mainSignClassification.MainSignClassificationType);

    osiTsMainSignClassification.set_type(std::move(osiTsMainSignClassificationType));

    // variability and is_out_of_service
    std::tie(osiTsVariability, osiTsIsOutOfService) = mapTSVariabilityandServiceState(mainSignClassification.SignState);
    osiTsMainSignClassification.set_variability(std::move(osiTsVariability));
    osiTsMainSignClassification.set_is_out_of_service(osiTsIsOutOfService);

    // value
    // ToDo: float adi to double osi
    osiTsValue->set_value(static_cast<double>(mainSignClassification.SignValue));
    // value unit
    osiTsValueUnit = mapTrafficSignValueUnit(mainSignClassification.MSignUnit);
    osiTsValue->set_value_unit(std::move(osiTsValueUnit));

    return osiTsMainSignClassification;
}

osi3::TrafficSign_MainSign_Classification_Type TrafficSignMapper::mapMainSignClassificationType(
    const ::ara::adi::sensoritf::SignClassificationType& signClassificationType) const
{
    using TsMsClassificationType = osi3::TrafficSign_MainSign_Classification_Type;
    TsMsClassificationType osiTsMsClassificationType{
        TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_UNKNOWN};

    switch (signClassificationType) {
    case ::ara::adi::sensoritf::SignClassificationType::kGreenArrowSign: {
        osiTsMsClassificationType
            = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_TRAFFIC_LIGHT_GREEN_ARROW;
        break;
    }
    case ::ara::adi::sensoritf::SignClassificationType::kHeightLimitSign: {
        osiTsMsClassificationType
            = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_VEHICLES_EXCESS_HEIGHT_PROHIBITED;
        break;
    }
    case ::ara::adi::sensoritf::SignClassificationType::kSpeedLimitSign: {
        osiTsMsClassificationType
            = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN;
        break;
    }
    case ::ara::adi::sensoritf::SignClassificationType::kStopSign: {
        osiTsMsClassificationType = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_STOP;
        break;
    }
    case ::ara::adi::sensoritf::SignClassificationType::kUnknown: {
        osiTsMsClassificationType = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    case ::ara::adi::sensoritf::SignClassificationType::kYieldSign: {
        osiTsMsClassificationType = TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_GIVE_WAY;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapMainSignClassificationType: Unknown enum entry: "
                            << static_cast<std::uint8_t>(signClassificationType);
        break;
    }
    }

    return osiTsMsClassificationType;
}

std::pair<osi3::TrafficSign_Variability, bool> TrafficSignMapper::mapTSVariabilityandServiceState(
    const ::ara::adi::sensoritf::SignState& signState) const
{
    std::pair<osi3::TrafficSign_Variability, bool> osiVariabilityandServiceState{
        osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_UNKNOWN, false};
    osi3::TrafficSign_Variability& osiTsVariability = osiVariabilityandServiceState.first;
    bool& osiTsIsOutOfService = osiVariabilityandServiceState.second;

    switch (signState) {
    case ::ara::adi::sensoritf::SignState::kVariable: {
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_VARIABLE;
        osiTsIsOutOfService = false;
        break;
    }
    case ::ara::adi::sensoritf::SignState::kUnknown: {
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_UNKNOWN;
        osiTsIsOutOfService = false;
        break;
    }
    case ::ara::adi::sensoritf::SignState::kStatic: {
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_FIXED;
        osiTsIsOutOfService = false;
        break;
    }
    case ::ara::adi::sensoritf::SignState::kOutOfView: {
        m_logger.LogWarn() << "No perfect mapping was possible: SignState::kOutOfView "
                              "-> osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER";
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER;
        osiTsIsOutOfService = false;
        break;
    }
    case ::ara::adi::sensoritf::SignState::kFullOutOfService: {
        m_logger.LogWarn() << "Mapping might be inaccurate: SignState::kFullOutOfService"
                              "-> osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER; "
                              "-> osiTsIsOutOfService = true";
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER;
        osiTsIsOutOfService = true;
        break;
    }
    case ::ara::adi::sensoritf::SignState::kPartlyOutOfService: {
        m_logger.LogWarn() << "Mapping might be inaccurate: SignState::kPartlyOutOfService"
                              "-> osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER; "
                              "-> osiTsIsOutOfService = true";
        osiTsVariability = osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_OTHER;
        osiTsIsOutOfService = true;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapTSVariabilityandServiceState: Unknown enum entry: "
                            << static_cast<std::uint8_t>(signState);
        break;
    }
    }

    return osiVariabilityandServiceState;
}

osi3::TrafficSignValue_Unit TrafficSignMapper::mapTrafficSignValueUnit(
    const ara::adi::sensoritf::SignValueUnit& signValueUnit) const
{
    osi3::TrafficSignValue_Unit osiTsValueUnit{osiTSValueUnit::TrafficSignValue_Unit_UNIT_UNKNOWN};
    if (TrafficSignValueUnitMap.find(signValueUnit) != TrafficSignValueUnitMap.end()) {
        if (signValueUnit == TSValueUnit::kHour || signValueUnit == TSValueUnit::kDay
            || signValueUnit == TSValueUnit::kWeekday) {
            if (signValueUnit == TSValueUnit::kHour) {
                m_logger.LogWarn() << "No perfect mapping was possible: SignValueUnit::kHour "
                                      "-> osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_OTHER";
            } else if (signValueUnit == TSValueUnit::kDay) {
                m_logger.LogInfo() << "No perfect mapping was possible: SignValueUnit::kDay "
                                      "-> osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_DAY";
            } else if (signValueUnit == TSValueUnit::kWeekday) {
                m_logger.LogWarn() << "No perfect mapping was possible: SignValueUnit::kWeekday "
                                      "-> osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_NO_UNIT";
            }
        }
        osiTsValueUnit = TrafficSignValueUnitMap[signValueUnit];
    } else {
        m_logger.LogError() << "Error: mapTrafficSignValueUnit: Unknown enum entry: "
                            << static_cast<std::uint8_t>(signValueUnit);
    }

    return osiTsValueUnit;
}

osi3::BaseStationary TrafficSignMapper::mapMainBaseStationary(const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::BaseStationary osiMainBaseStationary{};
    // Position
    osi3::Vector3d* position{osiMainBaseStationary.mutable_position()};
    *position = TrafficSignMapper::mapPoint3D2Osi(trafficSign.TrafficSignsPosition.Position);

    return osiMainBaseStationary;
}

osi3::TrafficSign_SupplementarySign TrafficSignMapper::mapTsSupplementarySign(
    const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::TrafficSign_SupplementarySign osiTsSupplementarySign;

    // Map Supplementary Base Stationary
    // TODO: Absolute supplementary traffic signs position in ISO not available. Calculated using relative traffic signs
    // position
    osi3::BaseStationary* osiSupplBaseStationary{osiTsSupplementarySign.mutable_base()};
    *osiSupplBaseStationary = TrafficSignMapper::mapSupplementaryBaseStationary(trafficSign, 0);

    // Map Supplementary Sign Classification
    // TODO: SSignList is a vector contained in one TrafficSignsSupplementarySigns
    ara::adi::sensoritf::SupplementarySignClassification supplSignClassification
        = trafficSign.TrafficSignsSupplementarySigns.SSignList.at(0)
              .TrafficSignsSupplementarySignsInformation.ValidSupplementarySignClassifications.at(0);
    osi3::TrafficSign_SupplementarySign_Classification* osiTsSupplSignClassification{
        osiTsSupplementarySign.mutable_classification()};
    *osiTsSupplSignClassification
        = TrafficSignMapper::mapTrafficSignSupplementaryClassification(supplSignClassification);

    return osiTsSupplementarySign;
}

osi3::TrafficSign_SupplementarySign_Classification TrafficSignMapper::mapTrafficSignSupplementaryClassification(
    const ara::adi::sensoritf::SupplementarySignClassification& supplSignClassification) const
{
    // initialize osi parameters
    osi3::TrafficSign_SupplementarySign_Classification osiTsSupplSignClassification{};
    osi3::TrafficSign_SupplementarySign_Classification_Type osiTsSupplSignClassificationType{};
    osi3::TrafficSign_Variability osiTsVariability{};
    bool osiTsIsOutOfService{};
    osi3::TrafficSignValue* ptrOsiTsValue{};
    osi3::TrafficSignValue_Unit osiTsValueUnit{};

    // map and assign values to osi

    // type
    osiTsSupplSignClassificationType
        = mapSupplSignClassificationType(supplSignClassification.SupplementarySignClassificationType);
    osiTsSupplSignClassification.set_type(std::move(osiTsSupplSignClassificationType));

    // variability and is_out_of_service

    std::tie(osiTsVariability, osiTsIsOutOfService)
        = mapTSVariabilityandServiceState(supplSignClassification.SignState);
    osiTsSupplSignClassification.set_variability(std::move(osiTsVariability));
    osiTsSupplSignClassification.set_is_out_of_service(osiTsIsOutOfService);

    // value
    // ToDo: float adi to double osi
    ptrOsiTsValue = osiTsSupplSignClassification.add_value();
    (*ptrOsiTsValue).set_value(static_cast<double>(supplSignClassification.SSignValue));
    // value unit
    osiTsValueUnit = mapTrafficSignValueUnit(supplSignClassification.SSignUnit);
    (*ptrOsiTsValue).set_value_unit(std::move(osiTsValueUnit));

    return osiTsSupplSignClassification;
}

osi3::TrafficSign_SupplementarySign_Classification_Type TrafficSignMapper::mapSupplSignClassificationType(
    const ara::adi::sensoritf::SupplementarySignClassificationType& supplSignClassificationType) const
{
    using tsSupplSignClassificationType = osi3::TrafficSign_SupplementarySign_Classification_Type;
    tsSupplSignClassificationType osiTsSupplSignClassificationType{
        tsSupplSignClassificationType::TrafficSign_SupplementarySign_Classification_Type_TYPE_UNKNOWN};

    switch (supplSignClassificationType) {
    case ara::adi::sensoritf::SupplementarySignClassificationType::kFrost: {
        osiTsSupplSignClassificationType
            = tsSupplSignClassificationType::TrafficSign_SupplementarySign_Classification_Type_TYPE_SNOW;
        break;
    }
    case ara::adi::sensoritf::SupplementarySignClassificationType::kWetRoad: {
        osiTsSupplSignClassificationType
            = tsSupplSignClassificationType::TrafficSign_SupplementarySign_Classification_Type_TYPE_WET;
        break;
    }
    case ara::adi::sensoritf::SupplementarySignClassificationType::kDistance: {
        osiTsSupplSignClassificationType
            = tsSupplSignClassificationType::TrafficSign_SupplementarySign_Classification_Type_TYPE_VALID_FOR_DISTANCE;
        break;
    }
    case ara::adi::sensoritf::SupplementarySignClassificationType::kUnknown: {
        osiTsSupplSignClassificationType
            = tsSupplSignClassificationType::TrafficSign_SupplementarySign_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapSupplSignClassificationType: "
                               "Unknown enum entry: "
                            << static_cast<std::uint8_t>(supplSignClassificationType);
        break;
    }
    }

    return osiTsSupplSignClassificationType;
}

osi3::BaseStationary TrafficSignMapper::mapSupplementaryBaseStationary(
    const ara::adi::sensoritf::TrafficSign& trafficSign,
    const std::size_t& index) const
{
    osi3::BaseStationary osiSupplBaseStationary;

    // Absolute supplementary traffic signs position in ISO not available.
    // Calculated using relative traffic signs position
    ::ara::adi::sensoritf::RelativePosition relativePosition
        = trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
              .TrafficSignsSupplementarySignsPos.SSRelativePosition;
    u_int8_t relativePositionOrder = trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
                                         .TrafficSignsSupplementarySignsPos.RelativePosOrder;

    const ara::adi::sensoritf::Point3D& tsMainPosition = trafficSign.TrafficSignsPosition.Position;

    // Position
    // IMP NOTE: All coordinates and orientations from detected objects are relative to the host vehicle frame
    // (see: Vehicle vehicle reference point)
    // From ego vehicle center: x-->front, y-->left, z-->top
    osi3::Vector3d* supplAbsoluteposition{osiSupplBaseStationary.mutable_position()};

    switch (relativePosition) {
    case ::ara::adi::sensoritf::RelativePosition::kAbove: {
        supplAbsoluteposition->set_x(tsMainPosition.x);
        supplAbsoluteposition->set_y(tsMainPosition.y);
        // 30cm above if relativePostionOrder is 1
        // 60cm above if relativePostionOrder is 2
        supplAbsoluteposition->set_z(tsMainPosition.z + static_cast<float>(0.3 * relativePositionOrder));
        break;
    }
    case ::ara::adi::sensoritf::RelativePosition::kBelow: {
        supplAbsoluteposition->set_x(tsMainPosition.x);
        supplAbsoluteposition->set_y(tsMainPosition.y);
        supplAbsoluteposition->set_z(tsMainPosition.z - static_cast<float>(0.3 * relativePositionOrder));
        break;
    }
    case ::ara::adi::sensoritf::RelativePosition::kLeft: {
        supplAbsoluteposition->set_x(tsMainPosition.x);
        supplAbsoluteposition->set_y(tsMainPosition.y + static_cast<float>(0.3 * relativePositionOrder));
        supplAbsoluteposition->set_z(tsMainPosition.z);
        break;
    }
    case ::ara::adi::sensoritf::RelativePosition::kRight: {
        supplAbsoluteposition->set_x(tsMainPosition.x);
        supplAbsoluteposition->set_y(tsMainPosition.y - static_cast<float>(0.3 * relativePositionOrder));
        supplAbsoluteposition->set_z(tsMainPosition.z);
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapSupplBaseStationary: Unknown Position enum entry: "
                            << static_cast<std::uint8_t>(relativePosition);
        break;
    }
    }

    return osiSupplBaseStationary;
}
