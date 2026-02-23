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

#include "adiMappers/TrafficSignsMapper.h"

using namespace ara::adi::sensoritf;

TrafficSigns TrafficSignsMapper::mapTrafficSigns(osi3::SensorData osiSensorData)
{
    int osiNumofTrafficSigns = osiSensorData.traffic_sign_size();
    // auto osiTrafficSign = osiSensorData.traffic_sign();

    TrafficSigns adiTrafficSigns{};
    ara::adi::sensoritf::TrafficSign adiTrafficSign{};
    InterfaceHeader interfaceHeader{};

    // NumberOfValidTrafficSigns
    adiTrafficSigns.NumberOfValidTrafficSigns = osiNumofTrafficSigns;

    // adiTrafficSigns.TrafficSignsList.at(0).ColourTone.ValidColourModel.
    ObjectStatus objectStatus = adiTrafficSign.TrafficSignsStatus;
    TrafficSignsInformation trafficSignInfo = adiTrafficSign.TrafficSignsInformation;
    TrafficSignsPosition trafficSignPosition = adiTrafficSign.TrafficSignsPosition;
    MainSignClassification mainSignClass{};

    TrafficSignsSupplementarySigns trafficSupplSigns{};
    TrafficSignsSupplementarySignVector trafficSupplSignVector{};
    TrafficSignsSupplementarySign trafficSupplSign{};
    TrafficSignsSupplementarySignsInformation trafficSupplSignInfo{};
    TrafficSignsSupplementarySignsPosition trafficSupplSignPos{};
    SubObjectStatus trafficSupplSignStatus{};

    SupplementarySignClassification supplSignClass{};

    for (int index = 0; index < osiNumofTrafficSigns; index++) {
        if (osiSensorData.traffic_sign(index).has_header()) {
            objectStatus = objectStatusMapper.mapObjectStatus(osiSensorData.traffic_sign(index).header());
        }

        if (osiSensorData.traffic_sign(index).has_main_sign()) {
            if (osiSensorData.traffic_sign(index).main_sign().has_base()) {
                if (osiSensorData.traffic_sign(index).main_sign().base().has_position()) {
                    trafficSignPosition.Position
                        = mapPositiontoADI(osiSensorData.traffic_sign(index).main_sign().base().position());
                }
            }

            if (osiSensorData.traffic_sign(index).main_sign().has_base_rmse()) {
                if (osiSensorData.traffic_sign(index).main_sign().base_rmse().has_position()) {
                    if (osiSensorData.traffic_sign(index).main_sign().base_rmse().has_position()) {
                        trafficSignPosition.PositionError
                            = mapPositionErrtoADI(osiSensorData.traffic_sign(index).main_sign().base_rmse().position());
                    }
                }
            }

            // trafficSignInfo.NumberOfValidLaneRelevanceClassifications;
            auto osiNumTrafficSignCandidate = osiSensorData.traffic_sign(index).main_sign().candidate_size();

            trafficSignInfo.NumberOfValidSignClassifications = static_cast<uint8_t>(osiNumTrafficSignCandidate);

            for (int cand = 0; cand < osiNumTrafficSignCandidate; cand++) {
                mainSignClass = maptoMainSignClass(osiSensorData.traffic_sign(index).main_sign(), cand);
                trafficSignInfo.ValidMainSignClassificationsList.push_back(mainSignClass);
            }

            adiTrafficSign.TrafficSignsInformation = trafficSignInfo;
            adiTrafficSign.TrafficSignsPosition = trafficSignPosition;
            adiTrafficSign.TrafficSignsStatus = objectStatus;
        }

        auto osiNumSupplTrafficSign = osiSensorData.traffic_sign(index).supplementary_sign_size();

        for (int supplSign = 0; supplSign < osiNumSupplTrafficSign; supplSign++) {

            // TODO: Cannot map RelativePosOrder
            // trafficSupplSignPos.RelativePosOrder;
            // auto osiSupplBasePosX =
            // osiSensorData.traffic_sign(index).supplementary_sign().at(supplSign).base().position().x();
            if (osiSensorData.traffic_sign(index).supplementary_sign(supplSign).has_base()) {
                if (osiSensorData.traffic_sign(index).supplementary_sign(supplSign).base().has_position()) {
                    if (osiSensorData.traffic_sign(index).supplementary_sign(supplSign).base().position().has_y()) {
                        auto osiSupplBasePosY
                            = osiSensorData.traffic_sign(index).supplementary_sign(supplSign).base().position().y();

                        if (osiSensorData.traffic_sign(index).supplementary_sign(supplSign).base().position().has_z()) {
                            auto osiSupplBasePosZ
                                = osiSensorData.traffic_sign(index).supplementary_sign(supplSign).base().position().z();

                            if (static_cast<double>(trafficSignPosition.Position.y) < osiSupplBasePosY) {
                                trafficSupplSignPos.SSRelativePosition = RelativePosition::kRight;
                            } else if (static_cast<double>(trafficSignPosition.Position.y) > osiSupplBasePosY) {
                                trafficSupplSignPos.SSRelativePosition = RelativePosition::kLeft;
                            } else if (static_cast<double>(trafficSignPosition.Position.z) > osiSupplBasePosZ) {
                                trafficSupplSignPos.SSRelativePosition = RelativePosition::kBelow;
                            } else if (static_cast<double>(trafficSignPosition.Position.z) < osiSupplBasePosZ) {
                                trafficSupplSignPos.SSRelativePosition = RelativePosition::kAbove;
                            }
                        }
                    }
                }
            }

            supplSignClass = maptoSupplSignClass(osiSensorData.traffic_sign(index).supplementary_sign(supplSign));
        }

        trafficSupplSign.TrafficSignsSupplementarySignsPos = trafficSupplSignPos;

        trafficSupplSignInfo.NumberOfValidSupplementarySignClassifications
            = static_cast<uint8_t>(osiNumSupplTrafficSign);
        trafficSupplSignInfo.ValidSupplementarySignClassifications.push_back(supplSignClass);
        trafficSupplSign.TrafficSignsSupplementarySignsInformation = trafficSupplSignInfo;

        // trafficSupplSign.TrafficSignsSupplementarySignsColourTone

        // trafficSupplSign.TrafficSignsSupplementarySignsStatus;
    }

    // TODO
    trafficSupplSignVector.push_back(trafficSupplSign);
    trafficSupplSigns.SSignList = trafficSupplSignVector;
    adiTrafficSign.TrafficSignsSupplementarySigns = trafficSupplSigns;

    return adiTrafficSigns;
}

MainSignClassification TrafficSignsMapper::maptoMainSignClass(
    const osi3::DetectedTrafficSign_DetectedMainSign& osiMainSign,
    int cand)
{

    MainSignClassification adiMainSignClass{};
    osi3::TrafficSign_MainSign_Classification osiMainSignClass = osiMainSign.candidate(cand).classification();

    using TsMsClassificationType = osi3::TrafficSign_MainSign_Classification_Type;
    if (osiMainSignClass.has_type()) {
        switch (osiMainSignClass.type()) {
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_TRAFFIC_LIGHT_GREEN_ARROW: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kGreenArrowSign;
            break;
        }
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_VEHICLES_EXCESS_HEIGHT_PROHIBITED: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kHeightLimitSign;
            break;
        }
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_SPEED_LIMIT_BEGIN: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kSpeedLimitSign;
            break;
        }
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_STOP: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kStopSign;
            break;
        }
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_UNKNOWN: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kUnknown;
            break;
        }
        case TsMsClassificationType::TrafficSign_MainSign_Classification_Type_TYPE_GIVE_WAY: {
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kYieldSign;
            break;
        }
        default: {
            m_logger.LogError() << "Error: maptoMainSignClass: Unknown enum entry"
                                   "mapped to SignClassificationType::kUnknown";
            adiMainSignClass.MainSignClassificationType = SignClassificationType::kUnknown;
            break;
        }
        }
    }

    // Main sign geometry

    if (osiMainSign.has_geometry()) {
        osi3::DetectedTrafficSign_DetectedMainSign_Geometry osiTSGeometry = osiMainSign.geometry();
        using osiTSMainSignGeometry = osi3::DetectedTrafficSign_DetectedMainSign_Geometry;
        switch (osiTSGeometry) {
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_ARROW_LEFT: {
            adiMainSignClass.MSignGeometry = SignGeometry::kArrowLeft;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_ARROW_RIGHT: {
            adiMainSignClass.MSignGeometry = SignGeometry::kArrowRight;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_CIRCLE: {
            adiMainSignClass.MSignGeometry = SignGeometry::kCircle;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_OCTAGON: {
            adiMainSignClass.MSignGeometry = SignGeometry::kOctagon;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_PLATE: {
            adiMainSignClass.MSignGeometry = SignGeometry::kPlate;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_POLE: {
            adiMainSignClass.MSignGeometry = SignGeometry::kPole;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_RECTANGLE: {
            adiMainSignClass.MSignGeometry = SignGeometry::kRectangle;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_SQUARE: {
            adiMainSignClass.MSignGeometry = SignGeometry::kSquare;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_TRIANGLE_DOWN: {
            adiMainSignClass.MSignGeometry = SignGeometry::kTriangleDown;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_TRIANGLE_TOP: {
            adiMainSignClass.MSignGeometry = SignGeometry::kTriangleTop;
            break;
        }
        case osiTSMainSignGeometry::DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_UNKNOWN: {
            adiMainSignClass.MSignGeometry = SignGeometry::kUnknown;
            break;
        }
        default: {
            m_logger.LogError() << "Error: maptoMainSignClass: Unknown enum entry"
                                   "mapped to SignGeometry::kUnknown";
            adiMainSignClass.MSignGeometry = SignGeometry::kUnknown;
            break;
        }
        }
    }

    // MSignUnit
    if (osiMainSignClass.has_value()) {
        if (osiMainSignClass.value().has_value_unit()) {
            auto osiSignValueUnit = osiMainSignClass.value().value_unit();
            adiMainSignClass.MSignUnit = mapSignValueUnit(osiSignValueUnit);
        }

        if (osiMainSignClass.value().has_value()) {
            // SignValue
            adiMainSignClass.SignValue = static_cast<float>(osiMainSignClass.value().value());
        }
    }

    // SignClassificationTypeConfidence
    if (osiMainSign.candidate(cand).has_probability()) {
        adiMainSignClass.SignClassificationTypeConfidence = osiMainSign.candidate(cand).probability();
    }

    // SignState
    // adiMainSignClass.SignState;

    return adiMainSignClass;
}

SupplementarySignClassification TrafficSignsMapper::maptoSupplSignClass(
    const osi3::DetectedTrafficSign_DetectedSupplementarySign& osiSupplSign)
{

    SupplementarySignClassification supplSignClass{};

    auto osiNumSupplSignCand = osiSupplSign.candidate_size();

    for (auto cand = 0; cand < osiNumSupplSignCand; cand++) {
        if (osiSupplSign.candidate(cand).has_classification()) {
            auto osiNumSupplSignVal = osiSupplSign.candidate(cand).classification().value_size();

            if (osiNumSupplSignVal != 0) {

                // SSignValue
                // TODo: Checking only the first value
                if (osiSupplSign.candidate(cand).classification().value(0).has_value()) {
                    supplSignClass.SSignValue = osiSupplSign.candidate(cand).classification().value(0).value();
                }

                // SSignUnit
                if (osiSupplSign.candidate(cand).classification().value(0).has_value_unit()) {
                    auto osiSupplSignValueUnit = osiSupplSign.candidate(cand).classification().value(0).value_unit();
                    supplSignClass.SSignUnit = mapSignValueUnit(osiSupplSignValueUnit);
                }

                // SupplementarySignClassificationType
                if (osiSupplSign.candidate(cand).classification().has_type()) {
                    auto osiSupplSignClassType = osiSupplSign.candidate(cand).classification().type();

                    switch (osiSupplSignClassType) {
                    case osi3::TrafficSign_SupplementarySign_Classification_Type::
                        TrafficSign_SupplementarySign_Classification_Type_TYPE_RAIN: {
                        // TODO Warning
                        supplSignClass.SupplementarySignClassificationType
                            = SupplementarySignClassificationType ::kFrost;
                        break;
                    }
                    case osi3::TrafficSign_SupplementarySign_Classification_Type::
                        TrafficSign_SupplementarySign_Classification_Type_TYPE_CONSTRAINED_TO: {
                        // TODO Warning
                        supplSignClass.SupplementarySignClassificationType
                            = SupplementarySignClassificationType ::kLimitation;
                        break;
                    }
                    case osi3::TrafficSign_SupplementarySign_Classification_Type::
                        TrafficSign_SupplementarySign_Classification_Type_TYPE_WET: {
                        // TODO Warning
                        supplSignClass.SupplementarySignClassificationType
                            = SupplementarySignClassificationType ::kWetRoad;
                        break;
                    }
                    default: {
                        m_logger.LogError() << "Error: maptoSupplSignClass: Unknown enum entry"
                                               "mapped to SupplementarySignClassificationType::kUnknown";
                        supplSignClass.SupplementarySignClassificationType
                            = SupplementarySignClassificationType ::kUnknown;
                        break;
                    }
                    }
                }

                // SignState
                osi3::TrafficSign_Variability osiTSVariability{};
                if (osiSupplSign.candidate(cand).classification().has_variability()) {
                    osiTSVariability = osiSupplSign.candidate(cand).classification().variability();
                }

                bool osiTSOutOfService = osiSupplSign.candidate(cand).classification().is_out_of_service();

                if (osiTSOutOfService) {
                    supplSignClass.SignState = SignState::kFullOutOfService;
                } else {
                    switch (osiTSVariability) {
                    case osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_FIXED: {
                        supplSignClass.SignState = SignState::kStatic;
                        break;
                    }
                    case osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_VARIABLE: {
                        supplSignClass.SignState = SignState::kVariable;
                        break;
                    }
                    default: {
                        m_logger.LogError() << "Error: maptoSupplSignClass SignState: Unknown enum entry"
                                               "mapped to SupplementarySignClassificationType::kUnknown";
                        supplSignClass.SignState = SignState::kUnknown;
                        break;
                    }
                    }
                }
            }
        }
        // SupplementarySignClassificationTypeConfidence
        if (osiSupplSign.candidate(cand).has_probability()) {
            supplSignClass.SupplementarySignClassificationTypeConfidence = osiSupplSign.candidate(cand).probability();
        }
    }
    return supplSignClass;
}

SignValueUnit TrafficSignsMapper::mapSignValueUnit(const osi3::TrafficSignValue_Unit& osiTSValueUnit)
{

    // auto osiSignValueUnit = osiMainSignClass.value().value_unit();
    SignValueUnit adiSignValueUnit{};

    switch (osiTSValueUnit) {
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_DAY: {
        adiSignValueUnit = SignValueUnit ::kDay;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_FEET: {
        adiSignValueUnit = SignValueUnit ::kFeet;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER: {
        adiSignValueUnit = SignValueUnit ::kKilometre;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR: {
        adiSignValueUnit = SignValueUnit ::kKilometrePerHour;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_LONG_TON: {
        adiSignValueUnit = SignValueUnit ::kLongTon;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER: {
        adiSignValueUnit = SignValueUnit ::kMeter;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METRIC_TON: {
        adiSignValueUnit = SignValueUnit ::kMetricTon;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE: {
        adiSignValueUnit = SignValueUnit ::kMile;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MILE_PER_HOUR: {
        adiSignValueUnit = SignValueUnit ::kMilePerHour;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_MINUTES: {
        adiSignValueUnit = SignValueUnit ::kMinute;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_PERCENTAGE: {
        adiSignValueUnit = SignValueUnit ::kPercentage;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_SHORT_TON: {
        adiSignValueUnit = SignValueUnit ::kShortTon;
        break;
    }
    case osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_UNKNOWN: {
        adiSignValueUnit = SignValueUnit ::kUnknown;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapSignValueUnit TrafficSignValue_Unit: Unknown enum entry"
                               "mapped to SignValueUnit::kUnknown";
        adiSignValueUnit = SignValueUnit ::kUnknown;
        break;
    }
    }
    return adiSignValueUnit;
}
