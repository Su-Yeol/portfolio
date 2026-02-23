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

#include "osi3/osi_detectedobject.pb.h"
#include "osi3/osi_sensordata.pb.h"

#include "ara/adi/sensoritf/impl_type_staticobjectinterface.h"

#include "osiMappers/DetectedStationaryObjectMapper.h"
#include "osiMappers/DetectedItemHeaderMapper.h"

osi3::DetectedStationaryObject DetectedStationaryObjectMapper::mapDetectedStationaryObject(
    const ara::adi::sensoritf::GeneralLandmark& generalLandmark) const
{
    osi3::DetectedStationaryObject osiDetectedStationaryObj{};

    // base Mapping
    osi3::BaseStationary* osiBase{osiDetectedStationaryObj.mutable_base()};
    *osiBase = mapBaseStationary(generalLandmark);

    // base rmse Mapping
    osi3::BaseStationary* osiBaseError{osiDetectedStationaryObj.mutable_base_rmse()};
    *osiBaseError = mapBaseRmse(generalLandmark);

    // TODO: Unavailable color description Mapping
    // osiDetectedStationaryObj.set_allocated_color_description();

    // header Mapping
    osi3::DetectedItemHeader* osiDetectedItemHeader{osiDetectedStationaryObj.mutable_header()};
    *osiDetectedItemHeader = detectedItemHeaderMapper.mapToDetectedItemHeader(generalLandmark.GeneralLandmarksStatus);

    // candidate Mapping
    // osi3::DetectedStationaryObject_CandidateStationaryObject* dStyObjCandidate;
    std::size_t numLandmark = generalLandmark.GeneralLandmarksInformation.LandmarkTypelist.size();

    if (numLandmark == 0) {
        return {};
    }

    if (numLandmark != generalLandmark.GeneralLandmarksInformation.NumberOfValidGeneralLandmarkClassifications) {
        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidGeneralLandmarkClassifications: "
                           << generalLandmark.GeneralLandmarksInformation.NumberOfValidGeneralLandmarkClassifications
                           << " and numLandmark: "
                           << generalLandmark.GeneralLandmarksInformation.LandmarkTypelist.size();
    }

    for (std::size_t i = 0; i < numLandmark; i++) {
        osi3::DetectedStationaryObject_CandidateStationaryObject* dStyObjCandidate
            = osiDetectedStationaryObj.add_candidate();

        // Classification Mapping
        osi3::StationaryObject_Classification* styObjClassification{dStyObjCandidate->mutable_classification()};

        auto landmarkType{generalLandmark.GeneralLandmarksInformation.LandmarkTypelist.at(i)};
        *styObjClassification = mapClassification(landmarkType);

        // Probability Mapping
        auto landmarkTypeProbability{
            generalLandmark.GeneralLandmarksInformation.LandmarkTypelist.at(i).LandmarkClassProbability};
        dStyObjCandidate->set_probability(static_cast<double>(landmarkTypeProbability));
    }

    return osiDetectedStationaryObj;
}

osi3::BaseStationary DetectedStationaryObjectMapper::mapBaseRmse(
    const ara::adi::sensoritf::GeneralLandmark& generalLandmark) const
{
    osi3::BaseStationary osiBaseError{};

    osi3::Dimension3d* osiDimError{osiBaseError.mutable_dimension()};
    auto dimError{generalLandmark.GeneralLandmarksBoundingBox.BoxError};
    *osiDimError = mapToDimension3dError(dimError);

    osi3::Vector3d* osiPosError{osiBaseError.mutable_position()};
    auto posError{generalLandmark.GeneralLandmarksPos.PositionError};
    *osiPosError = mapPoint3DError2Osi(posError);

    osi3::Orientation3d* osiOrientError{osiBaseError.mutable_orientation()};
    auto orientError{generalLandmark.GeneralLandmarksPos.OrientationError};
    *osiOrientError = mapToOrientation3dError(orientError);

    return osiBaseError;
}
