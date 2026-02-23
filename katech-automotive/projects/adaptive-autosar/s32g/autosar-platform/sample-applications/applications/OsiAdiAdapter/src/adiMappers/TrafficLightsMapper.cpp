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

#include "ara/adi/sensoritf/staticobjectsservice_skeleton.h"

#include "osi3/osi_sensordata.pb.h"

#include "adiMappers/TrafficLightsMapper.h"

using namespace ara::adi::sensoritf;

TrafficLights TrafficLightsMapper::mapTrafficLights(osi3::SensorData osiSensorData)
{

    TrafficLights adiTrafficLights{};
    // adiTrafficLights.NumberOfValidTrafficLights; // NA
    // adiTrafficLights.RecognisedTrafficLightsCapability; // NA
    // adiTrafficLights.RecognisedTrafficLightsStatus; // NA

    TrafficLight adiTrafficLight{};
    // adiTrafficLight.StructureLightsInformation; // NA
    // adiTrafficLight.TrafficLightsBoundingBox; // M
    // adiTrafficLight.TrafficLightsPosition; // M

    TrafficLightSpot adiTrafficLightSpot{};
    // ColourClassification adiTrafficLightColorClass{};

    // adiTrafficLightSpot.TrafficLightsSpotsColour.
    //    ValidColourClassificationVectorList.push_back(adiTrafficLightColorClass); // M

    // LightModeClassification adiTrafficLightModeClass{};
    // adiTrafficLightSpot.TrafficLightsSpotsColour.
    //    ValidLightModeClassificationVectorList.push_back(adiTrafficLightModeClass); // M
    // LightShapeClassification adiTrafficLightShapeClass{};

    // adiTrafficLightSpot.TrafficLightsSpotsInformation.
    //    ValidLightShapeClassificationList.push_back(adiTrafficLightShapeClass);  // M

    LaneRelevanceClassification adiTLLaneRelevanceClass{};
    TrafficLightsSpotsPosition adiTrafficLightsSpotsPosition{};
    adiTrafficLightsSpotsPosition.ValidLaneRelevanceClassificationList.push_back(adiTLLaneRelevanceClass);
    adiTrafficLightSpot.TrafficLightsSpotsPosition = adiTrafficLightsSpotsPosition;
    // adiTrafficLightSpot.TrafficLightsSpotsStatus;

    auto adiTrafficLightSpots = adiTrafficLight.TrafficLightsSpots;
    adiTrafficLightSpots.ValidTrafficLightSpotList.push_back(adiTrafficLightSpot);
    // adiTrafficLight.TrafficLightsStatus;

    TrafficLightsInformation trafficLightsInfo{};
    // trafficLightsInfo.NumberOfValidStructureLightClassifications;
    // StructureLightClassificationType trafficLightClassType{};
    // trafficLightsInfo.ValidStructureLightClassificationsList.at(0);

    BoundingBox trafficLightsBBox{};
    DimensionBox BBoxDimension{};
    DimensionBoxError BBoxDimensionErr{};
    TrafficLightsPosition trafficLightsPos{};

    TrafficLightSpots trafficLightsSpots{};
    TrafficLightSpot trafficLightSpot{};
    TrafficLightsSpotsColour tlSpotCOlour{};
    ColourClassificationType tlSpotColorClassType{};
    TrafficLightsSpotsInformation tlSpotsInfo{};
    ColourClassification tlColorClass{};
    LightShapeClassification tlLightShapeClass{};
    LightShapeClassificationType tlLightShapeClassType{};
    LightModeClassification tlLightModeClass{};
    LightModeClassificationType tlLightModeClassType{};

    trafficLightsSpots.NumberOfValidTrafficLightSpots = osiSensorData.traffic_light_size();
    if (osiSensorData.traffic_light_size() != 0) {

        for (auto tlindex = 0; tlindex < osiSensorData.traffic_light_size(); tlindex++) {

            // TODO Verify
            // trafficLightsSpots.TotalNumberOfTrafficLightSpots
            // trafficLightsSpots.TotalNumberOfTrafficLightSpotsConfidence;
            // tlSpotCOlour.NumberOfValidColourClassifications;
            // tlSpotCOlour.NumberOfValidLightModeClassifications;
            // tlSpotCOlour.ValidLightModeClassificationVectorList;

            trafficLightSpot.TrafficLightsSpotsStatus
                = objectStatusMapper.mapSubObjectStatus(osiSensorData.traffic_light(tlindex).header());

            auto tlNumCandidate = osiSensorData.traffic_light(tlindex).candidate_size();

            for (auto tlcandIndex = 0; tlcandIndex < tlNumCandidate; tlcandIndex++) {
                if (osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).has_classification()) {
                    if (osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().has_color()) {
                        auto osiTLColor
                            = osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().color();
                        tlSpotColorClassType = mapTLColor(osiTLColor);
                        tlColorClass.ColourClassificationType = tlSpotColorClassType;
                        // tlColorClass.ColourClassificationTypeConfidence;
                        tlSpotCOlour.ValidColourClassificationVectorList.push_back(tlColorClass);
                    }

                    if (osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().has_icon()) {
                        auto osiTLIcon
                            = osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().icon();
                        tlLightShapeClassType = mapTLIcon(osiTLIcon);
                        tlLightShapeClass.LightShapeClassificationType = tlLightShapeClassType;
                        tlSpotsInfo.ValidLightShapeClassificationList.push_back(tlLightShapeClass);
                    }

                    if (osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().has_mode()) {
                        auto tlMode
                            = osiSensorData.traffic_light(tlindex).candidate(tlcandIndex).classification().mode();
                        tlLightModeClassType = mapTLMode(tlMode);
                        tlLightModeClass.LightModeClassificationType = tlLightModeClassType;
                        tlSpotCOlour.ValidLightModeClassificationVectorList.push_back(tlLightModeClass);
                    }
                }
            }

            trafficLightSpot.TrafficLightsSpotsInformation = tlSpotsInfo;
            trafficLightSpot.TrafficLightsSpotsColour = tlSpotCOlour;
            trafficLightsSpots.ValidTrafficLightSpotList.push_back(trafficLightSpot);

            adiTrafficLight.TrafficLightsSpots = trafficLightsSpots;
            // mapInterfaceHeader(osiSensorData.traffic_light_header());
            adiTrafficLight.StructureLightsInformation = trafficLightsInfo;

            if (osiSensorData.traffic_light(tlindex).has_base()) {
                if (osiSensorData.traffic_light(tlindex).base().has_dimension()) {
                    if (osiSensorData.traffic_light(tlindex).base().dimension().has_height()) {
                        BBoxDimension.Height = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base().dimension().height();
                    }
                    if (osiSensorData.traffic_light(tlindex).base().dimension().has_length()) {
                        BBoxDimension.Length = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base().dimension().length();
                    }
                    if (osiSensorData.traffic_light(tlindex).base().dimension().has_width()) {
                        BBoxDimension.Width = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base().dimension().width();
                    }

                    trafficLightsBBox.BoxDimension = BBoxDimension;
                }

                if (osiSensorData.traffic_light(tlindex).base().has_orientation()) {
                    trafficLightsPos.Orientation
                        = mapOrientationtoADI(osiSensorData.traffic_light(tlindex).base().orientation());
                }

                if (osiSensorData.traffic_light(tlindex).base().has_position()) {
                    trafficLightsPos.PositionObjectLevel
                        = mapPositiontoADI(osiSensorData.traffic_light(tlindex).base().position());
                }
            }

            if (osiSensorData.traffic_light(tlindex).has_base_rmse()) {
                if (osiSensorData.traffic_light(tlindex).base_rmse().has_dimension()) {
                    if (osiSensorData.traffic_light(tlindex).base_rmse().dimension().has_height()) {
                        BBoxDimensionErr.Height = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base_rmse().dimension().height();
                    }
                    if (osiSensorData.traffic_light(tlindex).base_rmse().dimension().has_length()) {
                        BBoxDimensionErr.Length = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base_rmse().dimension().length();
                    }
                    if (osiSensorData.traffic_light(tlindex).base_rmse().dimension().has_width()) {
                        BBoxDimensionErr.Width = (osiSensorData.traffic_light_size())
                            * osiSensorData.traffic_light(tlindex).base_rmse().dimension().width();
                    }

                    trafficLightsBBox.BoxError = BBoxDimensionErr;
                }

                if (osiSensorData.traffic_light(tlindex).base_rmse().has_orientation()) {
                    trafficLightsPos.OrientationError
                        = mapOrientationErrortoADI(osiSensorData.traffic_light(tlindex).base_rmse().orientation());
                }

                if (osiSensorData.traffic_light(tlindex).base_rmse().has_position()) {
                    trafficLightsPos.PositionObjectLevelError
                        = mapPositionErrtoADI(osiSensorData.traffic_light(tlindex).base_rmse().position());
                }
            }

            adiTrafficLight.TrafficLightsPosition = trafficLightsPos;
            adiTrafficLight.TrafficLightsBoundingBox = trafficLightsBBox;

            adiTrafficLights.TrafficLightList.push_back(adiTrafficLight);
        }
    }

    // TrafficLightsStatus Cannot be mapped  as OSI does not have traffic light, but only spots
    // adiTrafficLight.TrafficLightsStatus = trafficLightsStatus;

    return adiTrafficLights;
}

LightModeClassificationType TrafficLightsMapper::mapTLMode(const osi3::TrafficLight_Classification_Mode& osiTLClassMode)
{
    LightModeClassificationType tlLightModeClassType;
    switch (osiTLClassMode) {
    case osi3::TrafficLight_Classification_Mode_MODE_FLASHING: {
        tlLightModeClassType = LightModeClassificationType::kBlinking;
        break;
    }
    case osi3::TrafficLight_Classification_Mode_MODE_CONSTANT: {
        tlLightModeClassType = LightModeClassificationType::kContinuous;
        break;
    }
    case osi3::TrafficLight_Classification_Mode_MODE_COUNTING: {
        tlLightModeClassType = LightModeClassificationType::kCounting;
        break;
    }
    case osi3::TrafficLight_Classification_Mode_MODE_OFF: {
        tlLightModeClassType = LightModeClassificationType::kTurnedOff;
        break;
    }
    case osi3::TrafficLight_Classification_Mode_MODE_UNKNOWN: {
        tlLightModeClassType = LightModeClassificationType::kUnknown;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapTLMode: Unknown enum entry"
                               "mapped to LightModeClassificationType::kUnknown";
        tlLightModeClassType = LightModeClassificationType::kUnknown;
        break;
    }
    }

    return tlLightModeClassType;
}

LightShapeClassificationType TrafficLightsMapper::mapTLIcon(
    const osi3::TrafficLight_Classification_Icon& osiTLClassIcon)
{

    LightShapeClassificationType tlLightShapeClassType{};
    switch (osiTLClassIcon) {
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_CROSS: {
        tlLightShapeClassType = LightShapeClassificationType::kCross;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_LEFT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowDiagLeft;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DIAG_RIGHT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowDiagRight;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowDown;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN_LEFT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowDownLeft;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_DOWN_RIGHT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowDownLeft;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowLeft;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT_RIGHT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowLeftRight;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_RIGHT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowRight;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowStraightAhead;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_LEFT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowStraightAheadLeft;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_STRAIGHT_AHEAD_RIGHT: {
        tlLightShapeClassType = LightShapeClassificationType::kArrowStraightAheadRight;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BICYCLE: {
        tlLightShapeClassType = LightShapeClassificationType::kBicycle;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BUS: {
        tlLightShapeClassType = LightShapeClassificationType::kBus;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_BUS_AND_TRAM: {
        tlLightShapeClassType = LightShapeClassificationType::kBusAndTram;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_COUNTDOWN_PERCENT: {
        tlLightShapeClassType = LightShapeClassificationType::kCountdownPercent;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_COUNTDOWN_SECONDS: {
        tlLightShapeClassType = LightShapeClassificationType::kCountdownSecond;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_DONT_WALK: {
        tlLightShapeClassType = LightShapeClassificationType::kDontWalk;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_NONE: {
        m_logger.LogWarn()
            << "In mapTLIcon: No perfect mapping to TrafficLight_Classification_Icon_ICON_NONE available."
               "Mapped to LightShapeClassificationType::kNoShape";
        tlLightShapeClassType = LightShapeClassificationType::kNoShape;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_OTHER: {
        m_logger.LogWarn()
            << "In mapTLIcon: No perfect mapping to TrafficLight_Classification_Icon_ICON_OTHER available."
               "Mapped to LightShapeClassificationType::kUnknown";
        tlLightShapeClassType = LightShapeClassificationType::kUnknown;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN: {
        tlLightShapeClassType = LightShapeClassificationType::kPedestrian;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_PEDESTRIAN_AND_BICYCLE: {
        tlLightShapeClassType = LightShapeClassificationType::kPedestrianAndBicycle;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_TRAM: {
        tlLightShapeClassType = LightShapeClassificationType::kTram;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_UNKNOWN: {
        tlLightShapeClassType = LightShapeClassificationType::kUnknown;
        break;
    }
    case osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_WALK: {
        tlLightShapeClassType = LightShapeClassificationType::kWalk;
        break;
    }
    default: {
        tlLightShapeClassType = LightShapeClassificationType::kUnknown;
        m_logger.LogError() << "Error: mapTLIcon: Unknown enum entry"
                               "mapped to LightShapeClassificationType::kUnknown";
        break;
    }
    }

    return tlLightShapeClassType;
}

ColourClassificationType TrafficLightsMapper::mapTLColor(const osi3::TrafficLight_Classification_Color& osiTLColor)
{
    ColourClassificationType tlSpotColorClassType;
    switch (osiTLColor) {
    case osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN: {
        tlSpotColorClassType = ColourClassificationType::kGreen;
        break;
    }
    case osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_RED: {
        tlSpotColorClassType = ColourClassificationType::kRed;
        break;
    }
    case osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_WHITE: {
        tlSpotColorClassType = ColourClassificationType::kWhite;
        break;
    }
    case osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_YELLOW: {
        tlSpotColorClassType = ColourClassificationType::kYellow;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapTLColor: Unknown enum entry"
                               "mapped to ColourClassificationType::kUnknown";
        tlSpotColorClassType = ColourClassificationType::kUnknown;
        break;
    }
    }

    return tlSpotColorClassType;
}
