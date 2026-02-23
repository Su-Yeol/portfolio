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

#include "osiMappers/LaneBoundaryMapper.h"

#include "osi3/osi_sensordata.pb.h"

osi3::LaneBoundary LaneBoundaryMapper::mapLaneBoundary(
    const ara::adi::sensoritf::RoadObjectInterface& roadObjectInterface) const
{
    ara::adi::sensoritf::RoadBoundaries roadBoundaries = roadObjectInterface.RoadBoundariesObjectList;
    osi3::LaneBoundary osiLaneBoundary{};

    // BoundaryPoint Mapping
    if (roadBoundaries.RoadBoundaryList.size() == 0) {
        return {};
    }

    if (roadBoundaries.RoadBoundaryList.size() != roadBoundaries.NumberOfValidRoadBoundaries) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidRoadBoundaries: "
                           << roadBoundaries.NumberOfValidRoadBoundaries
                           << " and roadBoundaries.RoadBoundaryList.size(): " << roadBoundaries.RoadBoundaryList.size();
    }

    for (size_t index = 0; index < roadBoundaries.RoadBoundaryList.size(); index++) {

        ara::adi::sensoritf::ValidRoadBoundary roadBoundary = roadBoundaries.RoadBoundaryList.at(index);

        // Identifier Mapping ToDo: Type Mismatch
        osi3::Identifier roadBoundaryID{};
        roadBoundaryID.set_value(static_cast<uint64_t>(roadBoundary.RoadBoundariesStatus.ObjectID));

        auto numValidPolylines{roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.size()};

        if (numValidPolylines == 0) {
            return {};
        }

        if (numValidPolylines != roadBoundary.RoadBoundariesPolylines.NumberOfValidPolylines) {

            m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidPolylines: "
                               << roadBoundary.RoadBoundariesPolylines.NumberOfValidPolylines
                               << " and roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.size(): "
                               << roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.size();
        }

        for (size_t polylineIndex = 0; polylineIndex < numValidPolylines; polylineIndex++) {

            ara::adi::sensoritf::ValidPolyline roadPolyline
                = roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polylineIndex);

            if (roadPolyline.ValidVerticesList.size() == 0) {
                return {};
            }

            if (roadPolyline.ValidVerticesList.size() != roadPolyline.NumberOfValidVertices) {

                m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidVertices: "
                                   << roadPolyline.NumberOfValidVertices
                                   << " and roadPolyline."
                                      "ValidVerticesList.size(): "
                                   << roadPolyline.ValidVerticesList.size();
            }

            for (size_t vertice = 0; vertice < roadPolyline.ValidVerticesList.size(); vertice++) {
                osi3::LaneBoundary_BoundaryPoint* ptrOsiLaneBoundaryPoint = osiLaneBoundary.add_boundary_line();
                *ptrOsiLaneBoundaryPoint = mapLaneBoundaryLine(roadPolyline.ValidVerticesList.at(vertice));
            }
        }

        // Classification Mapping
        auto numRoadBoundaryClass{roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.size()};

        if (numRoadBoundaryClass == 0) {
            return {};
        }

        if (numRoadBoundaryClass != roadBoundary.RoadBoundariesInformation.NumberOfValidRoadBoundaryClassifications) {
            m_logger.LogWarn() << "Warning: Mismatch between "
                                  "NumberOfValidRoadBoundaryClassifications: "
                               << roadBoundary.RoadBoundariesInformation.NumberOfValidRoadBoundaryClassifications
                               << " and roadBoundary.RoadBoundariesInformation."
                                  "ValidRoadBoundaryClassificationsList.size(): "
                               << roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.size();
        }

        for (size_t classIndex = 0; classIndex < numRoadBoundaryClass; classIndex++) {
            osi3::LaneBoundary_Classification* osiLaneBoundaryClassification{osiLaneBoundary.mutable_classification()};
            *osiLaneBoundaryClassification = mapLaneBoundaryClassification(
                roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.at(classIndex));

            // ColorDescription Mapping
            osi3::ColorDescription* osi3ColorDescription{osiLaneBoundary.mutable_color_description()};
            *osi3ColorDescription = mapToColorDesription(
                roadObjectInterface.RoadObjectInterfaceHeader.InterfaceExtension.ColourModelType,
                roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.at(classIndex).ColourTone);
        }
    }

    return osiLaneBoundary;
}

osi3::LaneBoundary_BoundaryPoint LaneBoundaryMapper::mapLaneBoundaryLine(
    const ara::adi::sensoritf::ValidVertice& roadVertice) const
{
    osi3::LaneBoundary_BoundaryPoint laneBoundaryPoint{};

    osi3::Vector3d* osiLanePosition{laneBoundaryPoint.mutable_position()};

    *osiLanePosition = mapPoint3D2Osi(roadVertice.VertexPoint);

    // NA
    // laneBoundaryPoint.set_dash();

    laneBoundaryPoint.set_height(static_cast<double>(roadVertice.HeightVertex));

    laneBoundaryPoint.set_width(static_cast<double>(roadVertice.WidthVertex));

    return laneBoundaryPoint;
}

osi3::LaneBoundary_Classification LaneBoundaryMapper::mapLaneBoundaryClassification(
    const ara::adi::sensoritf::ValidRoadBoundaryClassification& validRoadBoundaryClassification) const
{
    osi3::LaneBoundary_Classification osiLaneBoundaryClassification{};

    // NA
    // osiLaneBoundaryClassification.add_limiting_structure_id();

    // NA
    // osiLaneBoundaryClassification.set_color();

    osi3::LaneBoundary_Classification_Type osiLaneBoundaryClassType
        = mapLaneBoundaryClassType(validRoadBoundaryClassification.RoadBoundaryType);

    osiLaneBoundaryClassification.set_type(osiLaneBoundaryClassType);

    return osiLaneBoundaryClassification;
}

osi3::LaneBoundary_Classification_Type LaneBoundaryMapper::mapLaneBoundaryClassType(
    const ara::adi::sensoritf::RoadBoundaryType& roadBoundaryType) const
{
    osi3::LaneBoundary_Classification_Type osiLaneBoundaryClassType{};

    switch (roadBoundaryType) {
    case ara::adi::sensoritf::RoadBoundaryType::kBarrier: {
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_BARRIER;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kCurb: {
        osiLaneBoundaryClassType = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_CURB;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kFence: {
        m_logger.LogWarn() << "No perfect mapping was possible: "
                              "RoadBoundaryType::kFence "
                              "-> osi3::LaneBoundary_Classification_Type::LaneBoundary_"
                              "Classification_Type_TYPE_STRUCTURE";
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_STRUCTURE;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kGuardrail: {
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_GUARD_RAIL;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kRoadEdge: {
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_ROAD_EDGE;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kTensionCableSystem: {
        m_logger.LogWarn() << "No perfect mapping was possible: "
                              "RoadBoundaryType::kTensionCableSystem "
                              "-> osi3::LaneBoundary_Classification_Type::"
                              "LaneBoundary_Classification_Type_TYPE_OTHER";
        osiLaneBoundaryClassType = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_OTHER;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kUnclassifiedElevated: {
        m_logger.LogWarn() << "No perfect mapping was possible: "
                              "RoadBoundaryType::kUnclassifiedElevated "
                              "-> osi3::LaneBoundary_Classification_Type::"
                              "LaneBoundary_Classification_Type_TYPE_OTHER";
        osiLaneBoundaryClassType = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_OTHER;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kWand: {
        m_logger.LogWarn() << "No perfect mapping was possible: "
                              "RoadBoundaryType::kWand "
                              "-> osi3::LaneBoundary_Classification_Type::"
                              "LaneBoundary_Classification_Type_TYPE_OTHER";
        osiLaneBoundaryClassType = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_OTHER;
        break;
    }
    case ara::adi::sensoritf::RoadBoundaryType::kUnknown: {
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapLaneBoundaryType: "
                               "Unknown enum entry: ";
        osiLaneBoundaryClassType
            = osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    }

    return osiLaneBoundaryClassType;
}
