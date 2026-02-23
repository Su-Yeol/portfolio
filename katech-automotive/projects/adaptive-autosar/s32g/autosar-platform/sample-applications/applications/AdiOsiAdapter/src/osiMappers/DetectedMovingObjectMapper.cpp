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

#include "ara/adi/sensoritf/impl_type_potentiallymovingobjectinterface.h"

#include "osiMappers/DetectedMovingObjectMapper.h"
#include "osiMappers/DetectedItemHeaderMapper.h"

std::map<adiDMORefPt, osiDMORefPt> DetectedMovingObjectMapper::referencePointMap{
    {adiDMORefPt::kMidsideMidwidthMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_CENTER},
    {adiDMORefPt::kFrontLeftMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_FRONT_LEFT},
    {adiDMORefPt::kFrontMidwidthMidheight,
        osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_FRONT_MIDDLE},
    {adiDMORefPt::kFrontRightMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_FRONT_RIGHT},
    {adiDMORefPt::kMidsideLeftMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_MIDDLE_LEFT},
    {adiDMORefPt::kMidsideRightMidheight,
        osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_MIDDLE_RIGHT},
    //{adiDMORefPt::kMidsideMidwidthMidheight,osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_OTHER},
    {adiDMORefPt::kRearLeftMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_REAR_LEFT},
    {adiDMORefPt::kRearMidwidthMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_REAR_MIDDLE},
    {adiDMORefPt::kRearRightMidheight, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_REAR_RIGHT},
    {adiDMORefPt::kUnknown, osiDMORefPt::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_UNKNOWN}};

osi3::DetectedMovingObject DetectedMovingObjectMapper::mapDetectedMovingObject(
    const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    osi3::DetectedMovingObject osiDetectedMovingObj{};

    // header Mapping
    osi3::DetectedItemHeader* osiDetectedItemHeader{osiDetectedMovingObj.mutable_header()};
    *osiDetectedItemHeader
        = detectedItemHeaderMapper.mapToDetectedItemHeader(potentiallyMovingObject.PotentiallyMovingObjectsStatus);

    // base Mapping
    osi3::BaseMoving* osiBase{osiDetectedMovingObj.mutable_base()};
    *osiBase = mapBaseMoving(potentiallyMovingObject);

    // base rmse Mapping
    osi3::BaseMoving* osiBaseError{osiDetectedMovingObj.mutable_base_rmse()};
    *osiBaseError = mapBaseRmse(potentiallyMovingObject);

    // Reference Point Mapping
    osi3::DetectedMovingObject_ReferencePoint osiDMOReferencePoint = mapReferencePoint(potentiallyMovingObject);
    osiDetectedMovingObj.set_reference_point(osiDMOReferencePoint);

    // Movement State Mapping
    osi3::DetectedMovingObject_MovementState osiDMOMovementState = mapMovementState(potentiallyMovingObject);
    osiDetectedMovingObj.set_movement_state(osiDMOMovementState);

    // percentage_side_lane_left Mapping: Deprecated

    // percentage_side_lane_right Mapping: Deprecated

    // candidate Mapping

    osi3::StationaryObject_Classification styObjClassification;
    std::size_t numDMOCandidate
        = potentiallyMovingObject.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications.size();

    if (numDMOCandidate == 0) {
        return {};
    }

    if (numDMOCandidate
        != potentiallyMovingObject.PotentiallyMovingObjectsInformation
               .NumberOfValidPotentiallyMovingObjectClassifications) {

        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidPotentiallyMovingObjectClassifications: "
                           << potentiallyMovingObject.PotentiallyMovingObjectsInformation
                                  .NumberOfValidPotentiallyMovingObjectClassifications
                           << " and numDMOCandidate: " << numDMOCandidate;
    }

    for (std::size_t i = 0; i < numDMOCandidate; i++) {
        osi3::DetectedMovingObject_CandidateMovingObject* osiDMOCandidate{osiDetectedMovingObj.add_candidate()};
        *osiDMOCandidate = mapDMOCandidate(potentiallyMovingObject, i);
    }

    // TODO: Unavailable color description Mapping
    // osiDetectedMovingObj.set_allocated_color_description();

    // Camera specific information: Not available in OSI currently
    m_logger.LogInfo() << "End mapDetectedMovingObject";

    return osiDetectedMovingObj;
}

osi3::BaseMoving DetectedMovingObjectMapper::mapBaseRmse(
    const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    m_logger.LogInfo() << "mapBaseRmse";
    osi3::BaseMoving osiBaseError{};

    auto bboxDimError{potentiallyMovingObject.PotentiallyMovingObjectsBoundingBox.BoundingBoxExtentError};
    osi3::Dimension3d* osiDimError{osiBaseError.mutable_dimension()};

    *osiDimError = mapToDimension3dError(bboxDimError);

    auto posError{potentiallyMovingObject.PotentiallyMovingObjectsPosition.PositionObjectLevelError};
    osi3::Vector3d* osiPosError{osiBaseError.mutable_position()};

    *osiPosError = mapPoint3DError2Osi(posError);

    auto orientationError{potentiallyMovingObject.PotentiallyMovingObjectsPosition.OrientationError};
    osi3::Orientation3d* osiOrientError{osiBaseError.mutable_orientation()};

    *osiOrientError = mapToOrientation3dError(orientationError);

    return osiBaseError;
}

osi3::DetectedMovingObject_ReferencePoint DetectedMovingObjectMapper::mapReferencePoint(
    const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    m_logger.LogInfo() << "mapReferencePoint";
    osiDMORefPt osiDMOReferencePoint{};
    ara::adi::sensoritf::ReferencePoint refPoint
        = potentiallyMovingObject.PotentiallyMovingObjectsPosition.ReferencePoint;

    if (referencePointMap.find(refPoint) != referencePointMap.end()) {
        osiDMOReferencePoint = referencePointMap[refPoint];
    } else {
        osiDMOReferencePoint
            = osi3::DetectedMovingObject_ReferencePoint::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_OTHER;
        m_logger.LogWarn() << "Warning: mapReferencePoint: Other enum-entry: " << static_cast<std::uint8_t>(refPoint);
    }

    return osiDMOReferencePoint;
}

osi3::DetectedMovingObject_MovementState DetectedMovingObjectMapper::mapMovementState(
    const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    m_logger.LogInfo() << "mapMovementState";
    osi3::DetectedMovingObject_MovementState osiDMOMovementState{};
    ara::adi::sensoritf::MovementStatus PMOMovementStatus{
        potentiallyMovingObject.PotentiallyMovingObjectsDynamics.MovementStatus};

    // OTHER state not available in adi
    switch (PMOMovementStatus) {
    case ara::adi::sensoritf::MovementStatus::kMoving: {
        osiDMOMovementState
            = osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_MOVING;
        break;
    }
    case ara::adi::sensoritf::MovementStatus::kStationary: {
        osiDMOMovementState
            = osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_STATIONARY;
        break;
    }
    case ara::adi::sensoritf::MovementStatus::kStoppedMoving: {
        osiDMOMovementState
            = osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_STOPPED;
        break;
    }
    case ara::adi::sensoritf::MovementStatus::kUnknow: {
        osiDMOMovementState
            = osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_UNKNOWN;
        break;
    }
    default: {
        osiDMOMovementState
            = osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_UNKNOWN;
        m_logger.LogError() << "Error: mapMovementState: Other enum-entry: "
                            << static_cast<std::uint8_t>(PMOMovementStatus);
        break;
    }
    }

    return osiDMOMovementState;
}

osi3::DetectedMovingObject_CandidateMovingObject DetectedMovingObjectMapper::mapDMOCandidate(
    const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject,
    const size_t& classIndex) const
{
    m_logger.LogInfo() << "mapDMOCandidate";
    osi3::DetectedMovingObject_CandidateMovingObject osiDMOCandidate{};
    auto PMOClassifications{
        potentiallyMovingObject.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications};

    if (PMOClassifications.size() == 0) {
        return {};
    }

    auto PMOClass{PMOClassifications.at(classIndex)};

    // Classification Mapping
    // styObjClassification = mapClassification(generalLandmark.GeneralLandmarksInformation.LandmarkTypelist.at(i));
    // osiDMOObjCandidate->set_allocated_classification(&styObjClassification);

    // Probability Mapping: Confidence of Class type mapped
    osiDMOCandidate.set_probability(static_cast<double>(PMOClass.PotentiallyMovingObjectClassificationTypeConfidence));

    // Moving object type Mapping
    osiMOType osiMovingObjType = mapType(PMOClass);
    osiDMOCandidate.set_type(osiMovingObjType);

    // Moving object vehicle classification
    if (osiMovingObjType == osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE) {

        osi3::MovingObject_VehicleClassification* osiMOVehicleClassification{
            osiDMOCandidate.mutable_vehicle_classification()};
        *osiMOVehicleClassification = mapVehicleClassification(potentiallyMovingObject, classIndex);
    }

    if (osiMovingObjType == osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN) {

        // person pose calculated at 1st pose
        size_t personIndex = 0;
        auto personPoselist{potentiallyMovingObject.PotentiallyMovingObjectsPerson.PersonPoselist};

        if (personPoselist.size() == 0) {
            return {};
        }

        // Pedestrian head pose Mapping
        osi3::Orientation3d* osiPedestrianOrientation{osiDMOCandidate.mutable_head_pose()};

        *osiPedestrianOrientation = mapOrientation3D2Osi(
            potentiallyMovingObject.PotentiallyMovingObjectsPerson.PersonPoselist.at(personIndex).PersonPoseInfo);

        // Pedestrian upper body pose Mapping
        osi3::Orientation3d* osiUpperBodyPose{osiDMOCandidate.mutable_upper_body_pose()};

        *osiUpperBodyPose = *osiPedestrianOrientation;

        auto poseType{personPoselist.at(personIndex).PoseType};

        if (poseType == ara::adi::sensoritf::PersonPoseType::kHead) {
            m_logger.LogWarn() << "Warning: mapDMOCandidate: Pose type "
                                  "head_pose available in adi. Mapping"
                                  " upper_body_pose to same value";
        } else if (poseType == ara::adi::sensoritf::PersonPoseType::kUpperBody) {
            m_logger.LogWarn() << "Warning: mapDMOCandidate: Pose type "
                                  "upper_body_pose available in adi. Mapping"
                                  " head_pose to same value";
        } else {
            m_logger.LogWarn() << "Warning: mapDMOCandidate: Pose type "
                                  "upper_body_pose and head_pose not "
                                  "available in adi. Mapping them to other available value";
        }
    }

    // Moving object classification: not available in correct format in adi

    return osiDMOCandidate;
}
