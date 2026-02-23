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

#include "osiMappers/DetectedLaneBoundaryMapper.h"
#include "osiMappers/DetectedItemHeaderMapper.h"

osi3::DetectedLaneBoundary DetectedLaneBoundaryMapper::mapDetectedLaneBoundary(
    const ara::adi::sensoritf::ValidRoadBoundary& roadBoundary) const
// ara::adi::sensoritf::ColourModelType colourModelType) const
{
    osi3::DetectedLaneBoundary osiDetectedLaneBoundary{};

    uint16_t numPolylines = roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.size();
    if (numPolylines == 0) {
        return {};
    }

    if (numPolylines != roadBoundary.RoadBoundariesPolylines.NumberOfValidPolylines) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidPolylines: "
                           << roadBoundary.RoadBoundariesPolylines.NumberOfValidPolylines
                           << " and numPolylines: " << numPolylines;
    }

    // Candidate probaility and classification Mapping
    uint8_t numRoadClassification = roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.size();

    if (numRoadClassification == 0) {
        return {};
    }

    if (numRoadClassification != roadBoundary.RoadBoundariesInformation.NumberOfValidRoadBoundaryClassifications) {

        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidRoadBoundaryClassifications: "
                           << roadBoundary.RoadBoundariesInformation.NumberOfValidRoadBoundaryClassifications
                           << " and numROadClassification: " << numRoadClassification;
    }

    for (size_t classIndex = 0; classIndex < numRoadClassification; classIndex++) {
        osi3::DetectedLaneBoundary_CandidateLaneBoundary* ptrOsiDLBCandidate = osiDetectedLaneBoundary.add_candidate();

        osi3::LaneBoundary_Classification* osiLaneBoundaryClass{ptrOsiDLBCandidate->mutable_classification()};

        auto laneBoundaryClass{
            roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.at(classIndex)};
        *osiLaneBoundaryClass = mapLaneBoundaryClassification(laneBoundaryClass);

        auto laneBoundaryTypeConfidence{
            roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.at(classIndex)
                .RoadBoundaryTypeConfidence};
        ptrOsiDLBCandidate->set_probability(laneBoundaryTypeConfidence);

        // TODO ColorDescription Mapping: not sure if this should be mapped
        // here or consider each candidate's color description mapping

        // osi3::ColorDescription osi3ColorDescription = mapToColorDesription(colourModelType,
        //    roadBoundary.RoadBoundariesInformation.ValidRoadBoundaryClassificationsList.at(classIndex).ColourTone);
        // osiDetectedLaneBoundary.set_allocated_color_description(&osi3ColorDescription);
    }

    // Item Header Mapping
    osi3::DetectedItemHeader* osiDetectedItemHeader{osiDetectedLaneBoundary.mutable_header()};

    *osiDetectedItemHeader = detectedItemHeaderMapper.mapToDetectedItemHeader(roadBoundary.RoadBoundariesStatus);

    for (size_t polyLineIndex = 0; polyLineIndex < numPolylines; polyLineIndex++) {
        auto validVertices{roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex).ValidVerticesList};
        uint16_t numVertices = validVertices.size();

        if (numVertices == 0) {
            return {};
        }

        if (numVertices
            != roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex).NumberOfValidVertices) {
            m_logger.LogWarn()
                << "Warning: Mismatch between "
                   "NumberOfValidVertices: "
                << roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex).NumberOfValidVertices
                << " and numROadClassification: " << numVertices;
        }

        for (size_t verticeIndex = 0; verticeIndex < numVertices; verticeIndex++) {
            // boundary_line Mapping
            osi3::LaneBoundary_BoundaryPoint* ptrOsiLaneBoundaryPoint = osiDetectedLaneBoundary.add_boundary_line();
            *ptrOsiLaneBoundaryPoint = mapLaneBoundaryLine(validVertices.at(verticeIndex));

            // boundary_line_rmse Mapping
            osi3::LaneBoundary_BoundaryPoint* ptrOsiLaneBoundaryPointRmse
                = osiDetectedLaneBoundary.add_boundary_line_rmse();
            osi3::Vector3d* osiBoundaryLineRmse{ptrOsiLaneBoundaryPointRmse->mutable_position()};
            *osiBoundaryLineRmse = mapPoint3DError2Osi(validVertices.at(verticeIndex).VertexPointError);

            auto heightVertexError{roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex)
                                       .ValidVerticesList.at(verticeIndex)
                                       .HeightVertexError};
            auto widthVertexError{roadBoundary.RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex)
                                      .ValidVerticesList.at(verticeIndex)
                                      .WidthVertexError};

            ptrOsiLaneBoundaryPointRmse->set_height(static_cast<double>(heightVertexError));
            ptrOsiLaneBoundaryPointRmse->set_width(static_cast<double>(widthVertexError));

            // NA
            // osiLaneBoundaryPointRmse->set_dash();

            // boundary_line confidence : NA
            // osiDetectedLaneBoundary.add_boundary_line_confidences(roadObjectInterface.RoadBoundariesObjectList.
            //    RoadBoundaryList.at(index).RoadBoundariesPolylines.ValidPolylinesList.at(polyLineIndex).
            //    ValidVerticesList.at(verticeIndex).VertexPointConfidence);
        }
    }

    return osiDetectedLaneBoundary;
    // osiDetectedLaneBoundary.set_boundary_line_confidences();
}
