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

#include "osiMappers/TrafficLightMapper.h"
#include "osiMappers/GeneralHeaderMapper.h"

#include "osi3/osi_trafficlight.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_common.pb.h"

#include <algorithm>

std::map<LSClassificationType, TLClassificationIcon> TrafficLightMapper::mapClassificationIcon{
    {LSClassificationType::kArrowDiagLeft, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT},
    {LSClassificationType::kArrowDiagRight,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT},
    {LSClassificationType::kArrowDown, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN},
    {LSClassificationType::kArrowDownLeft, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN_LEFT},
    {LSClassificationType::kArrowDownRight,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN_RIGHT},
    {LSClassificationType::kArrowLeft, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT},
    {LSClassificationType::kArrowLeftRight,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT_RIGHT},
    {LSClassificationType::kArrowRight, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT},
    {LSClassificationType::kArrowStraightAhead,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD},
    {LSClassificationType::kArrowStraightAheadLeft,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT},
    {LSClassificationType::kArrowStraightAheadRight,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT},
    {LSClassificationType::kBicycle, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_BICYCLE},
    {LSClassificationType::kBus, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_BUS},
    {LSClassificationType::kBusAndTram, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_BUS_AND_TRAM},
    {LSClassificationType::kCountdownPercent,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_COUNTDOWN_PERCENT},
    {LSClassificationType::kCountdownSecond,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_COUNTDOWN_SECONDS},
    {LSClassificationType::kCross, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_ARROW_CROSS},
    {LSClassificationType::kDontWalk, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_DONT_WALK},
    {LSClassificationType::kNoShape, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_NONE},
    {LSClassificationType::kPedestrian, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN},
    {LSClassificationType::kPedestrianAndBicycle,
        TLClassificationIcon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE},
    {LSClassificationType::kTram, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_TRAM},
    {LSClassificationType::kUnknown, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_UNKNOWN},
    {LSClassificationType::kWalk, TLClassificationIcon::TrafficLight_Classification_Icon_ICON_WALK}};

// Map osi single bulb traffic light
osi3::TrafficLight TrafficLightMapper::mapTrafficLight(const ara::adi::sensoritf::TrafficLight& trafficLight,
    const size_t& trafficLightSpotsIndex) const
{
    osi3::TrafficLight osiTrafficLight;

    // Map Identifier

    // TODO: Objectid for Traffic Light Spot not available(xcel correction). Mapping Traffic Light id for now.
    // TODO: Objectid type mismatch-->uint16_t to uint64
    // std::uint16_t trafficLightid = trafficLight.TrafficLightsStatus.ObjectID;
    // osi3::Identifier osiTlid;
    // osiTlid.set_value(& trafficLightid);
    // ositrafficLight.set_allocated_id(&osiTlid);

    // Map Base Stationary(xcel correction)
    // TODO: ValidTrafficLightSpotList is a vector contained in one TrafficLight
    ara::adi::sensoritf::TrafficLightSpot tlSpot
        = trafficLight.TrafficLightsSpots.ValidTrafficLightSpotList.at(trafficLightSpotsIndex);

    osi3::BaseStationary* baseOsi{osiTrafficLight.mutable_base()};
    *baseOsi = TrafficLightMapper::mapBaseStationary(trafficLight, trafficLightSpotsIndex);

    // Map Traffic Light Classification
    // ToDo: Loop through all spot class
    osi3::TrafficLight_Classification* osiTlClassification{osiTrafficLight.mutable_classification()};
    *osiTlClassification = TrafficLightMapper::mapClassification(tlSpot, 0);

    return osiTrafficLight;
}

osi3::TrafficLight_Classification TrafficLightMapper::mapClassification(
    const ara::adi::sensoritf::TrafficLightSpot& tlSpot,
    const std::uint16_t& classIndex) const
{
    // initialize osi parameters
    osi3::TrafficLight_Classification osiTlClassification{};
    osi3::TrafficLight_Classification_Color osiTlColor{osi3::TrafficLight_Classification_Color_COLOR_UNKNOWN};
    osi3::TrafficLight_Classification_Icon osiTlIcon{osi3::TrafficLight_Classification_Icon_ICON_UNKNOWN};
    osi3::TrafficLight_Classification_Mode osiTlMode{osi3::TrafficLight_Classification_Mode_MODE_UNKNOWN};

    // map and assign values to osi
    // TODO: Assign classification parameters based on probability

    // colour
    osiTlColor = mapColour(
        tlSpot.TrafficLightsSpotsColour.ValidColourClassificationVectorList.at(classIndex).ColourClassificationType);
    osiTlClassification.set_color(std::move(osiTlColor));

    // icon
    osiTlIcon = mapIcon(tlSpot.TrafficLightsSpotsInformation.ValidLightShapeClassificationList.at(classIndex)
                            .LightShapeClassificationType);
    osiTlClassification.set_icon(std::move(osiTlIcon));

    // mode
    osiTlMode = mapMode(tlSpot.TrafficLightsSpotsColour.ValidLightModeClassificationVectorList.at(classIndex)
                            .LightModeClassificationType);
    osiTlClassification.set_mode(osiTlMode);

    // counter
    osiTlClassification.set_counter(static_cast<double>(
        tlSpot.TrafficLightsSpotsInformation.ValidLightShapeClassificationList.at(classIndex).LightShapeValue));

    // assigned lane id: NA
    osiTlClassification.add_assigned_lane_id();

    // is out of service
    if (osiTlMode == osi3::TrafficLight_Classification_Mode::TrafficLight_Classification_Mode_MODE_OFF) {
        osiTlClassification.is_out_of_service();
    }

    return osiTlClassification;
}

osi3::TrafficLight_Classification_Color TrafficLightMapper::mapColour(
    const ::ara::adi::sensoritf::ColourClassificationType& colourClassificationType) const
{
    using TLClassificationColour = osi3::TrafficLight_Classification_Color;
    TLClassificationColour osiTLClassificationColour{};
    switch (colourClassificationType) {
    case ::ara::adi::sensoritf::ColourClassificationType::kGreen: {
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_GREEN;
        break;
    }
    case ::ara::adi::sensoritf::ColourClassificationType::kRed: {
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_RED;
        break;
    }
    case ::ara::adi::sensoritf::ColourClassificationType::kWhite: {
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_WHITE;
        break;
    }
    case ::ara::adi::sensoritf::ColourClassificationType::kYellow: {
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_YELLOW;
        break;
    }
    case ::ara::adi::sensoritf::ColourClassificationType::kUnknown: {
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_UNKNOWN;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapColour: Unknown enum entry: "
                               "mapped to TLClassificationColour::"
                               "TrafficLight_Classification_Color_COLOR_UNKNOWN";
        osiTLClassificationColour = TLClassificationColour::TrafficLight_Classification_Color_COLOR_UNKNOWN;
        break;
    }
    }

    return osiTLClassificationColour;
}

osi3::TrafficLight_Classification_Icon TrafficLightMapper::mapIcon(
    const ::ara::adi::sensoritf::LightShapeClassificationType& lightShapeClassificationType) const
{
    m_logger.LogInfo() << "mapIcon";
    TLClassificationIcon osiTLClassificationIcon{TLClassificationIcon::TrafficLight_Classification_Icon_ICON_UNKNOWN};

    if (mapClassificationIcon.find(lightShapeClassificationType) != mapClassificationIcon.end())
        osiTLClassificationIcon = mapClassificationIcon[lightShapeClassificationType];
    else {
        m_logger.LogError() << "Error: mapIcon: Invalid enum entry in mapIcon ";
    }

    return osiTLClassificationIcon;
}

osi3::TrafficLight_Classification_Mode TrafficLightMapper::mapMode(
    const ::ara::adi::sensoritf::LightModeClassificationType& lightModeClassificationType) const
{
    using TLClassificationMode = osi3::TrafficLight_Classification_Mode;
    TLClassificationMode osiTLClassificationMode{};

    switch (lightModeClassificationType) {
    case ::ara::adi::sensoritf::LightModeClassificationType::kBlinking: {
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_FLASHING;
        break;
    }
    case ::ara::adi::sensoritf::LightModeClassificationType::kContinuous: {
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_CONSTANT;
        break;
    }
    case ::ara::adi::sensoritf::LightModeClassificationType::kCounting: {
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_COUNTING;
        break;
    }
    case ::ara::adi::sensoritf::LightModeClassificationType::kTurnedOff: {
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_OFF;
        break;
    }
    case ::ara::adi::sensoritf::LightModeClassificationType::kUnknown: {
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_UNKNOWN;
        break;
    }
    default: {
        TrafficLightMapper::m_logger.LogError() << "Error: mapMode: Unknown enum entry: ";
        osiTLClassificationMode = TLClassificationMode::TrafficLight_Classification_Mode_MODE_UNKNOWN;
        break;
    }
    }
    return osiTLClassificationMode;
}

osi3::BaseStationary TrafficLightMapper::mapBaseStationary(const ara::adi::sensoritf::TrafficLight& trafficLight,
    const std::size_t& trafficLightSpotsIndex) const
{
    osi3::BaseStationary osiBase;

    // Position: Position for single bulb traffic light spot available
    osi3::Vector3d* position{osiBase.mutable_position()};
    *position = TrafficLightMapper::mapPoint3D2Osi(
        trafficLight.TrafficLightsSpots.ValidTrafficLightSpotList.at(trafficLightSpotsIndex)
            .TrafficLightsSpotsPosition.PositionObjectLevel);

    // Orientation: Orientation same as the entire trafficLight
    osi3::Orientation3d* orientation{osiBase.mutable_orientation()};
    *orientation = TrafficLightMapper::mapOrientation3D2Osi(trafficLight.TrafficLightsPosition.Orientation);

    TrafficLightMapper::m_logger.LogWarn() << "Orientation mapping for single traffic light not available."
                                           << "Mapping the orientation of collective traffic light";

    // Dimension: Dimension of single bulb traffic light spot needs to be calculated from traffic light bounding box

    // TotalNumberOfTrafficLightSpots chosen over NumberOfValidTrafficLightSpots
    std::uint8_t numOfTLSpots = trafficLight.TrafficLightsSpots.TotalNumberOfTrafficLightSpots;
    ::ara::adi::sensoritf::DimensionBox dimensionTLSpot{};
    dimensionTLSpot.Height = (trafficLight.TrafficLightsBoundingBox.BoxDimension.Height) / numOfTLSpots;

    // Traffic Light Spot Length and Width same as the entire single TrafficLight
    dimensionTLSpot.Width = trafficLight.TrafficLightsBoundingBox.BoxDimension.Width;
    dimensionTLSpot.Length = trafficLight.TrafficLightsBoundingBox.BoxDimension.Length;

    TrafficLightMapper::m_logger.LogWarn() << "Dimension mapping for single traffic light not available."
                                           << "Deriving the values from the collective traffic light dimensions";

    osi3::Dimension3d* dimension{osiBase.mutable_dimension()};
    *dimension = TrafficLightMapper::mapDimension3D2Osi(dimensionTLSpot);

    return osiBase;
}
