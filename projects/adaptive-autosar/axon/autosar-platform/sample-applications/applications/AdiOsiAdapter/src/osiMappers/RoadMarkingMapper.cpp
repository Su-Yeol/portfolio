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

#include "osiMappers/RoadMarkingMapper.h"

#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"

osi3::RoadMarking RoadMarkingMapper::mapToRoadMarking(const ara::adi::sensoritf::ColourModelType& colourModelType,
    const ara::adi::sensoritf::RoadMarking& roadMarking,
    const std::uint8_t classificationNumber) const
{
    osi3::RoadMarking osiRoadMarking;

    osi3::BaseStationary* base{osiRoadMarking.mutable_base()};
    ara::adi::sensoritf::Polylines polylines = roadMarking.RoadMarkingsPolylines;
    *base = mapToBaseStationary(polylines);

    if (roadMarking.RoadMarkingsInformation.ValidRoadMarkingClassificationsList.size() <= classificationNumber) {
        m_logger.LogWarn() << "Warning: classificationNumber in mapToRoadMarking is wrong, "
                           << "could not set classification and color_descripton!";
        return osiRoadMarking;
    }

    osi3::RoadMarking_Classification* osiClassification{osiRoadMarking.mutable_classification()};
    size_t signClassIndex = 0;
    ara::adi::sensoritf::RoadMarkingClassification classification
        = roadMarking.RoadMarkingsInformation.ValidRoadMarkingClassificationsList.at(classificationNumber);
    *osiClassification = mapToClassification(classification, signClassIndex);

    osi3::ColorDescription* colorDescription{osiRoadMarking.mutable_color_description()};
    *colorDescription = mapToColorDesription(colourModelType, classification.ColourTone);

    // adi has no id for RoadMaring
    //
    // adi has no ExternelReference type

    return osiRoadMarking;
}

osi3::BaseStationary RoadMarkingMapper::mapToBaseStationary(const ara::adi::sensoritf::Polylines& polylines) const
{
    osi3::BaseStationary base;

    // simple solution: take the first element of the first polyline and use it for the base position
    // more complex solutions are possible
    if (polylines.ValidPolylinesList.size() == 0) {
        return {};
    }

    if (polylines.ValidPolylinesList.at(0).ValidVerticesList.size() == 0) {
        return {};
    }

    auto positionOsi{base.mutable_position()};
    auto position = polylines.ValidPolylinesList.at(0).ValidVerticesList.at(0).VertexPoint;
    *positionOsi = mapPoint3D2Osi(position);

    return base;
}

osi3::RoadMarking_Classification RoadMarkingMapper::mapToClassification(
    const ara::adi::sensoritf::RoadMarkingClassification& classification,
    const size_t& signClassIndex) const
{
    if (signClassIndex >= classification.ValidSignClassificationsList.size()) {
        return {};
    }

    osi3::RoadMarking_Classification classificationOsi{};

    osi3::RoadMarking_Classification_Type value = mapRoadMarkingType(classification.RoadMarkingType);
    classificationOsi.set_type(value);

    // DONE add traffic sign type, if its implemented
    if (value == osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_PAINTED_TRAFFIC_SIGN
        || value == osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_SYMBOLIC_TRAFFIC_SIGN
        || value == osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_TEXTUAL_TRAFFIC_SIGN) {
        osi3::TrafficSign_MainSign_Classification_Type osiTSClassType = trafficSignMapper.mapMainSignClassificationType(
            classification.ValidSignClassificationsList.at(signClassIndex).SignClassificationType);
        classificationOsi.set_traffic_main_sign_type(osiTSClassType);
    }

    // The monochrome_color will not used Insted ColourDescription is used.

    osi3::TrafficSignValue signValue{};
    auto bestSignClassification = findBestValidSignClassifications(classification);
    signValue.set_value(static_cast<double>(bestSignClassification.SignValue));

    // DONE add the next line as soon as TrafficSign was implemented.
    osi3::TrafficSignValue* osiTSValue{classificationOsi.mutable_value()};
    osi3::TrafficSignValue_Unit signValueUnit = trafficSignMapper.mapTrafficSignValueUnit(
        classification.ValidSignClassificationsList.at(signClassIndex).SignValueUnit);
    signValue.set_value_unit(signValueUnit);
    *osiTSValue = signValue;

    // TODO implement the next line as soon as TrafficSign was implemented. Use SignState.kFullOutOfService andk
    // kPartllyOutOfService BD: Might not have to do with traffic sign. Bool needs to be set if the road marking is out
    // of service
    // classificationOsi.set_is_out_of_service(...);

    // value_tex and assigned_lane_id are not supported by adi yet.

    return classificationOsi;
}

ara::adi::sensoritf::SignClassification RoadMarkingMapper::findBestValidSignClassifications(
    const ara::adi::sensoritf::RoadMarkingClassification& roadMarkingClassifications) const
{
    auto list = roadMarkingClassifications.ValidSignClassificationsList;

    if (list.size() != roadMarkingClassifications.NumberOfValidSignClassifications) {
        m_logger.LogWarn() << "Warning: NumberOfValidSignClassifications has a wrong value!";
    }

    auto best = std::max_element(list.begin(),
        list.end(),
        [](const ara::adi::sensoritf::SignClassification& a, const ara::adi::sensoritf::SignClassification& b) {
            return a.SignClassificationTypeConfidence < b.SignClassificationTypeConfidence;
        });

    return *best.base();
}

osi3::RoadMarking_Classification_Type RoadMarkingMapper::mapRoadMarkingType(
    const ara::adi::sensoritf::RoadMarkingType& type) const
{

    switch (type) {
    case ara::adi::sensoritf::RoadMarkingType::kArrow: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kAttentionMarker: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kBottsDotsCatsEyes: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kBox: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kCentreLineDashedMarking: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kCentreLineDoubleLineDashed: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kCentreLineMultipleLineDashed: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kColouredArea: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kDoubleLineSolid: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kEdgeLineDashedMarking: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kEdgeLineMultipleLineDashed: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kGenericLine: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_GENERIC_LINE;
    }

    case ara::adi::sensoritf::RoadMarkingType::kGenericSymbol: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_GENERIC_SYMBOL;
    }

    case ara::adi::sensoritf::RoadMarkingType::kHatched: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kIShapeMarkingBegin: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kIShapeMarkingEnd: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kLShapeMarkingBegin: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kLShapeMarkingEnd: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kMultipleLineSolid: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kNets: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kParkingArea: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kSolid: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kTrafficSignOnLane: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_PAINTED_TRAFFIC_SIGN;
    }

    case ara::adi::sensoritf::RoadMarkingType::kTriangular: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kTShapeMarkingBegin: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kTShapeMarkingEnd: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    case ara::adi::sensoritf::RoadMarkingType::kUnknown: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_UNKNOWN;
    }

    case ara::adi::sensoritf::RoadMarkingType::kZebraCrossing: {
        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_OTHER;
    }

    default: {
        m_logger.LogWarn() << "Warning: Unknown type for ara::adi::sensoritf::RoadMarkingType.";

        return osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_UNKNOWN;

    } break;
    }
}
