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

#include "adiMappers/RoadObjectsMapper.h"

using namespace ara::adi::sensoritf;

RoadObjectInterface RoadObjectsMapper::mapRoadObjects(const osi3::SensorData& osiSensorData)
{
    RoadObjectInterface adiRoadObjectInterface{};

    RoadBoundaries adiRoadBoundaries{};
    RoadSurface adiRoadSurface{};
    RoadMarkings adiRoadMarkings{};
    InterfaceHeader adiRMInterfaceHeader{};

    // adiRMInterfaceHeader = mapRMInterfaceHeader(osiSensorData);
    // adiRoadSurface = mapRoadSurface(osiSensorData);
    adiRoadMarkings = mapRoadMarkings(osiSensorData);
    adiRoadBoundaries = mapRoadBoundaries(osiSensorData);

    adiRoadObjectInterface.RoadBoundariesObjectList = adiRoadBoundaries;
    adiRoadObjectInterface.RoadMarkingObjectList = adiRoadMarkings;
    adiRoadObjectInterface.RoadObjectInterfaceHeader = adiRMInterfaceHeader;
    adiRoadObjectInterface.RoadSurfaceObjectList = adiRoadSurface;

    return adiRoadObjectInterface;
}

// InterfaceHeader RoadObjectsMapper::mapRMInterfaceHeader(const osi3::SensorData& osiSensorData)
// {
//     InterfaceHeader adiInterfaceHeader{};
//     // TODO: Cannot decide whether to map OSI LaneBoundary header to ADI RoadObject InterfaceHeader
//     // or map OSI RoadMarking header to  ADI RoadObject InterfaceHeader
//     return adiInterfaceHeader;
// }
//
// RoadSurface RoadObjectsMapper::mapRoadSurface(const osi3::SensorData& osiSensorData)
// {
//     // NA
// }

RoadMarkings RoadObjectsMapper::mapRoadMarkings(const osi3::SensorData& osiSensorData)
{
    RoadMarkings adiRoadMarkings{};
    RoadMarking adiRoadMarking{};
    RoadMarkingVector adiRoadMarkingList{};
    Polylines adiRoadMarkingPolylines{};
    ValidPolyline adiValidPolyline{};
    ValidPolylineVector adiValidPolylineList{};
    ValidVerticeVector adiValidVerticeList{};
    ValidVertice adiValidVertice{};

    RoadMarkingsInformation adiRoadMarkingInfo{};
    RoadMarkingClassificationVector adiRoadMarkingClassList{};
    RoadMarkingClassification adiRoadMarkingClass{};
    RoadMarkingType adiRoadMarkingType{};

    auto osiNumRoadMarkings = osiSensorData.road_marking_size();
    for (auto index = 0; index < osiNumRoadMarkings; index++) {
        if (osiSensorData.road_marking(index).has_header()) {
            adiRoadMarking.RoadMarkingsStatus
                = objectStatusMapper.mapObjectStatus(osiSensorData.road_marking(index).header());
        }

        if (osiSensorData.road_marking(index).has_base()) {
            if (osiSensorData.road_marking(index).base().has_position()) {
                adiValidVertice.VertexPoint = mapPositiontoADI(osiSensorData.road_marking(index).base().position());
            }

            if (osiSensorData.road_marking(index).base().has_dimension()) {
                if (osiSensorData.road_marking(index).base().dimension().has_height()) {
                    adiValidVertice.HeightVertex = osiSensorData.road_marking(index).base().dimension().height();
                }
                if (osiSensorData.road_marking(index).base().dimension().has_width()) {
                    adiValidVertice.WidthVertex = osiSensorData.road_marking(index).base().dimension().width();
                }
            }
        }
        if (osiSensorData.road_marking(index).has_base_rmse()) {
            if (osiSensorData.road_marking(index).base_rmse().has_dimension()) {
                if (osiSensorData.road_marking(index).base_rmse().dimension().has_height()) {
                    adiValidVertice.HeightVertexError
                        = osiSensorData.road_marking(index).base_rmse().dimension().height();
                }
                if (osiSensorData.road_marking(index).base_rmse().dimension().has_width()) {
                    adiValidVertice.WidthVertexError
                        = osiSensorData.road_marking(index).base_rmse().dimension().width();
                }
            }
        }

        adiValidVerticeList.push_back(adiValidVertice);
        adiValidPolyline.ValidVerticesList = adiValidVerticeList;
        adiValidPolylineList.push_back(adiValidPolyline);

        adiRoadMarkingPolylines.ValidPolylinesList = adiValidPolylineList;
        adiRoadMarking.RoadMarkingsPolylines = adiRoadMarkingPolylines;

        auto osiNumRMCandidate = osiSensorData.road_marking(index).candidate_size();
        adiRoadMarkingInfo.NumberOfValidRoadMarkingClassifications = static_cast<uint8_t>(osiNumRMCandidate);
        for (auto candIndex = 0; candIndex < osiNumRMCandidate; candIndex++) {

            if (osiSensorData.road_marking(index).candidate(candIndex).has_classification()) {

                if (osiSensorData.road_marking(index).candidate(candIndex).classification().has_type()) {
                    auto osiRMClassType
                        = osiSensorData.road_marking(index).candidate(candIndex).classification().type();
                    switch (osiRMClassType) {
                    case osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_GENERIC_LINE: {
                        adiRoadMarkingType = RoadMarkingType::kGenericLine;
                        break;
                    }
                    case osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_GENERIC_SYMBOL: {
                        adiRoadMarkingType = RoadMarkingType::kGenericSymbol;
                        break;
                    }
                    case osi3::RoadMarking_Classification_Type::
                        RoadMarking_Classification_Type_TYPE_PAINTED_TRAFFIC_SIGN: {
                        adiRoadMarkingType = RoadMarkingType::kTrafficSignOnLane;
                        break;
                    }
                    default: {
                        adiRoadMarkingType = RoadMarkingType::kUnknown;
                        break;
                    }
                    }
                }
            }
        }

        adiRoadMarkingClass.RoadMarkingType = adiRoadMarkingType;
        adiRoadMarkingClassList.push_back(adiRoadMarkingClass);
        adiRoadMarkingInfo.ValidRoadMarkingClassificationsList = adiRoadMarkingClassList;
        adiRoadMarking.RoadMarkingsInformation = adiRoadMarkingInfo;

        adiRoadMarkingList.push_back(adiRoadMarking);
    }

    adiRoadMarkings.ValidRoadMarkings = adiRoadMarkingList;

    return adiRoadMarkings;
}

RoadBoundaries RoadObjectsMapper::mapRoadBoundaries(const osi3::SensorData& osiSensorData)
{
    RoadBoundaries adiRoadBoundaries{};
    ValidRoadBoundary adiValidRoadBoundary{};

    uint16_t adiNumValidBoundary = static_cast<uint16_t>(osiSensorData.lane_boundary_size());

    RoadBoundariesInformation adiRBInfo{};
    ValidRoadBoundaryClassificationVector adiRBClassList{};
    ValidRoadBoundaryClassification adiRBClass{};
    ColourTone adiColorTone{};
    RoadBoundaryType adiRoadBoundaryType{};
    ProbabilityPercentage adiRoadBoundaryTypeConfidence{};

    ObjectStatus adiRoadBoundariesStatus{};

    ara::adi::sensoritf::ValidPolylineVector adiValidPolylineVec{};
    ValidPolyline adiValidPolyline{};
    ara::adi::sensoritf::ValidVerticeVector adiRBValidVerticeList{};

    ValidVertice adiValidVertex{};

    for (int index = 0; index < adiNumValidBoundary; index++) {

        if (osiSensorData.lane_boundary(index).has_header()) {
            adiRoadBoundariesStatus = objectStatusMapper.mapObjectStatus(osiSensorData.lane_boundary(index).header());
        }
        adiValidRoadBoundary.RoadBoundariesStatus = adiRoadBoundariesStatus;

        if (osiSensorData.lane_boundary(index).has_color_description()) {
            // adiColorTone = mapToColorToneADI(osiSensorData.lane_boundary(index).color_description());
        }

        int osiNumBoundaryLine = osiSensorData.lane_boundary(index).boundary_line_size();
        for (int boundIndex = 0; boundIndex < osiNumBoundaryLine; boundIndex++) {
            adiValidVertex = mapRBVertex(osiSensorData.lane_boundary(index).boundary_line(boundIndex));
            adiRBValidVerticeList.push_back(adiValidVertex);
        }

        adiValidPolyline.ValidVerticesList = adiRBValidVerticeList;
        adiValidPolylineVec.push_back(adiValidPolyline);

        int osiNumCand = osiSensorData.lane_boundary(index).candidate_size();
        for (int candIndex = 0; candIndex < osiNumCand; candIndex++) {
            if (osiSensorData.lane_boundary(index).candidate(candIndex).has_classification()) {
                if (osiSensorData.lane_boundary(index).candidate(candIndex).classification().has_type()) {
                    auto osiLaneType = osiSensorData.lane_boundary(index).candidate(candIndex).classification().type();
                    adiRoadBoundaryType = mapRoadBoundaryType(osiLaneType);
                }
            }

            if (osiSensorData.lane_boundary(index).candidate(candIndex).has_probability()) {
                adiRoadBoundaryTypeConfidence
                    = static_cast<float>(osiSensorData.lane_boundary(index).candidate(candIndex).probability());
            }

            adiRBClass.RoadBoundaryType = adiRoadBoundaryType;
            adiRBClass.RoadBoundaryTypeConfidence = adiRoadBoundaryTypeConfidence;
        }

        adiRBClass.ColourTone = adiColorTone;

        adiRBClassList.push_back(adiRBClass);
    }

    adiRBInfo.ValidRoadBoundaryClassificationsList = adiRBClassList;
    adiValidRoadBoundary.RoadBoundariesPolylines.ValidPolylinesList = adiValidPolylineVec;

    adiRoadBoundaries.RoadBoundaryList.push_back(adiValidRoadBoundary);
    adiRoadBoundaries.NumberOfValidRoadBoundaries = adiNumValidBoundary;

    return adiRoadBoundaries;
}

RoadBoundaryType RoadObjectsMapper::mapRoadBoundaryType(const osi3::LaneBoundary_Classification_Type& osiLaneType)
{

    RoadBoundaryType adiRoadBOundaryType{};

    switch (osiLaneType) {
    case osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_BARRIER: {
        adiRoadBOundaryType = RoadBoundaryType::kBarrier;
        break;
    }
    case osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_CURB: {
        adiRoadBOundaryType = RoadBoundaryType::kCurb;
        break;
    }
    case osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_GUARD_RAIL: {
        adiRoadBOundaryType = RoadBoundaryType::kGuardrail;
        break;
    }
    case osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_ROAD_EDGE: {
        adiRoadBOundaryType = RoadBoundaryType::kRoadEdge;
        break;
    }
    default: {
        // TODO Warning
        adiRoadBOundaryType = RoadBoundaryType::kUnknown;
        break;
    }
    }

    return adiRoadBOundaryType;
}

ValidVertice RoadObjectsMapper::mapRBVertex(const osi3::LaneBoundary_BoundaryPoint& osiLBPoint)
{

    ValidVertice adiValidVertice{};
    if (osiLBPoint.has_height()) {
        adiValidVertice.HeightVertex = static_cast<float>(osiLBPoint.height());
    }
    if (osiLBPoint.has_width()) {
        adiValidVertice.WidthVertex = static_cast<float>(osiLBPoint.width());
    }
    if (osiLBPoint.has_position()) {
        adiValidVertice.VertexPoint = mapPositiontoADI(osiLBPoint.position());
    }

    return adiValidVertice;
}
