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

#include "adiMappers/PMObjectsMapper.h"
#include "osi3/osi_sensordata.pb.h"

#include <algorithm>

PotentiallyMovingObjectInterface PMObjectMapper::mapPMObjectInterface(const osi3::SensorData& osiSensorData)
{

    PotentiallyMovingObjectInterface adiPotentiallyMovingObjectInterface{};

    InterfaceHeader adiInterfaceHeader = mapPMOInterfaceHeader(osiSensorData);
    adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectInterfaceHeader = adiInterfaceHeader;

    adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectList = mapPMObjects(osiSensorData);

    return adiPotentiallyMovingObjectInterface;
}

InterfaceHeader PMObjectMapper::mapPMOInterfaceHeader(const osi3::SensorData& osiSensorData)
{

    InterfaceHeader interfaceHeader{};

    // Information Interface
    if (osiSensorData.has_moving_object_header()) {

        osi3::DetectedEntityHeader osiDetectedEntityHeader = osiSensorData.moving_object_header();

        interfaceHeader = mapInterfaceHeader(osiDetectedEntityHeader);
    }

    return interfaceHeader;
}

PotentiallyMovingObjects PMObjectMapper::mapPMObjects(const osi3::SensorData& osiSensorData)
{
    PotentiallyMovingObjects adiPMObjects{};

    // NumberOfValidPotentiallyMovingObjects
    adiPMObjects.NumberOfValidPotentiallyMovingObjects = osiSensorData.moving_object_size();

    // RecognisedPotentiallyMovingObjectsCapability
    // adiPMObjects.RecognisedPotentiallyMovingObjectsCapability;

    // RecognisedPotentiallyMovingObjectsStatus
    // adiPMObjects.RecognisedPotentiallyMovingObjectsStatus;

    // ValidPotentiallyMovingObjects
    auto numOsiMovingObjects = osiSensorData.moving_object_size();
    ValidPotentiallyMovingObject adiValidPMO{};

    // Cannot map
    // PotentiallyMovingObjectsLaneRelatedInformation adiPMOLaneInfo{};
    // PotentiallyMovingObjectsMotionRelatedInformation adiPMOMotionInfo{};

    for (auto i = 0; i < numOsiMovingObjects; i++) {

        adiValidPMO.PotentiallyMovingObjectsDynamics = mapPMODynamics(osiSensorData.moving_object(i));
        adiValidPMO.PotentiallyMovingObjectsBoundingBox = mapPMOBoundingBox(osiSensorData.moving_object(i));
        adiValidPMO.PotentiallyMovingObjectsStatus
            = objectStatusMapper.mapObjectStatus(osiSensorData.moving_object(i).header());
        adiValidPMO.PotentiallyMovingObjectsPosition = mapPMOPosition(osiSensorData.moving_object(i));

        for (int j = 0; j < osiSensorData.moving_object(i).candidate_size(); j++) {
            adiValidPMO.PotentiallyMovingObjectsInformation = mapPMOInformation(osiSensorData.moving_object(i), j);

            if (!(adiValidPMO.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications.empty())) {

                if (maxObject == 2) {
                    adiValidPMO.PotentiallyMovingObjectsPerson = mapPMOPerson(osiSensorData.moving_object(i), j);
                }

                else if (maxObject == 3) {
                    adiValidPMO.PotentiallyMovingObjectsLights = mapPMOLights(osiSensorData.moving_object(i), j);
                }
            }
        }

        adiPMObjects.ValidPotentiallyMovingObjects.push_back(adiValidPMO);
    }

    return adiPMObjects;
}

ObjectStatus PMObjectMapper::mapPMObjectStatus(const osi3::DetectedMovingObject& osiDMO)
{
    ObjectStatus adiObjectStatus{};

    if (osiDMO.has_header()) {
        adiObjectStatus = objectStatusMapper.mapObjectStatus(osiDMO.header());
    }

    return adiObjectStatus;
}

::PotentiallyMovingObjectsInformation PMObjectMapper::mapPMOInformation(const osi3::DetectedMovingObject& osiDMO,
    const int& index)
{
    ::PotentiallyMovingObjectsInformation adiPMObjectsInfo{};

    // TODO: Check this
    // Set 1: Vehicle, 2: Pedestrian

    auto osiDMOCandidate = osiDMO.candidate(index);

    if (osiDMOCandidate.has_type()) {

        static float objProbability{};

        osi3::MovingObject_Type osiMOType = osiDMOCandidate.type();

        adiPMObjectsInfo.NumberOfValidPotentiallyMovingObjectClassifications = osiDMO.candidate_size();

        ValidPotentiallyMovingObjectClassification adiPMOClassification{};

        switch (osiMOType) {
        case osi3::MovingObject_Type::MovingObject_Type_TYPE_ANIMAL: {
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kAnimal;
            if (static_cast<float>(osiDMOCandidate.probability()) > objProbability) {
                objProbability = static_cast<float>(osiDMOCandidate.probability());
                maxObject = 1;
            }
            break;
        }
        case osi3::MovingObject_Type::MovingObject_Type_TYPE_OTHER: {
            m_logger.LogWarn() << "In mapPMOInformation: No perfect mapping to MovingObject_Type_TYPE_OTHER available."
                                  "Mapped to PotentiallyMovingObjectClassificationType::kUnknown";
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kUnknown;
            if (static_cast<float>(osiDMOCandidate.probability()) > objProbability) {
                objProbability = static_cast<float>(osiDMOCandidate.probability());
                maxObject = 1;
            }
            break;
        }
        case osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN: {
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kPedestrian;
            if (static_cast<float>(osiDMOCandidate.probability()) > objProbability) {
                objProbability = static_cast<float>(osiDMOCandidate.probability());
                maxObject = 2;
            }
            break;
        }
        case osi3::MovingObject_Type::MovingObject_Type_TYPE_UNKNOWN: {
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kUnknown;
            if (static_cast<float>(osiDMOCandidate.probability()) > objProbability) {
                objProbability = static_cast<float>(osiDMOCandidate.probability());
                maxObject = 1;
            }
            break;
        }
        case osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE: {
            m_logger.LogWarn()
                << "In mapPMOInformation: No perfect mapping to MovingObject_Type_TYPE_VEHICLE available."
                   "Mapped to PotentiallyMovingObjectClassificationType::kOtherVehicle";
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kOtherVehicle;
            if (static_cast<float>(osiDMOCandidate.probability()) > objProbability) {
                objProbability = static_cast<float>(osiDMOCandidate.probability());
                maxObject = 3;
            }
            break;
        }

        default:
            m_logger.LogError() << "Error: mapPMOInformation: Unknown enum entry"
                                   "mapped to PotentiallyMovingObjectClassificationType::kUnknown";
            adiPMOClassification.PotentiallyMovingObjectClassificationType
                = PotentiallyMovingObjectClassificationType::kUnknown;
            break;
        }

        adiPMOClassification.PotentiallyMovingObjectClassificationTypeConfidence
            = static_cast<float>(osiDMOCandidate.probability());

        adiPMObjectsInfo.PotentiallyMovingObjectClassifications.push_back(adiPMOClassification);
    }

    return adiPMObjectsInfo;
}

::PotentiallyMovingObjectsPosition PMObjectMapper::mapPMOPosition(const osi3::DetectedMovingObject& osiDMObject)
{
    ::PotentiallyMovingObjectsPosition adiPMOPosition{};

    if (osiDMObject.has_base()) {
        if (osiDMObject.base().has_orientation()) {
            adiPMOPosition.Orientation = mapOrientationtoADI(osiDMObject.base().orientation());
        }

        if (osiDMObject.base().has_position()) {
            adiPMOPosition.PositionObjectLevel = mapPositiontoADI(osiDMObject.base().position());
        }
    }

    if (osiDMObject.has_base_rmse()) {
        if (osiDMObject.base_rmse().has_orientation()) {
            adiPMOPosition.OrientationError = mapOrientationErrortoADI(osiDMObject.base_rmse().orientation());
        }

        if (osiDMObject.base_rmse().has_position()) {
            adiPMOPosition.PositionObjectLevelError = mapPositionErrtoADI(osiDMObject.base_rmse().position());
        }
    }

    // adiPMOPosition.ReferencePoint
    // adiPMOPosition.RoadLevel

    return adiPMOPosition;
}

::PotentiallyMovingObjectsBoundingBox PMObjectMapper::mapPMOBoundingBox(const osi3::DetectedMovingObject& osiDMObject)
{
    ::PotentiallyMovingObjectsBoundingBox adiPMOBBox{};

    if (osiDMObject.has_base()) {
        if (osiDMObject.base().has_dimension()) {
            adiPMOBBox.BoundingBoxExtent = mapDimensiontoADI(osiDMObject.base().dimension());
        }
    }

    if (osiDMObject.has_base_rmse()) {
        if (osiDMObject.base_rmse().has_dimension()) {
            adiPMOBBox.BoundingBoxExtentError = mapToDimension3dError(osiDMObject.base_rmse().dimension());
            // adiPMOBBox.BoundingBoxGroundClearance = osiDMObject.base().
            // adiPMOBBox.IncludedGeometricStructures
        }
    }

    return adiPMOBBox;
}

::PotentiallyMovingObjectsDynamics PMObjectMapper::mapPMODynamics(const osi3::DetectedMovingObject& osiDMObject)
{

    ::PotentiallyMovingObjectsDynamics adiPMODynamics{};

    if (osiDMObject.has_base()) {
        if (osiDMObject.base().has_acceleration()) {
            adiPMODynamics.Acceleration = mapPositiontoADI(osiDMObject.base().acceleration());
        }

        if (osiDMObject.base().has_velocity()) {
            adiPMODynamics.VelocityObjectLevel = mapPositiontoADI(osiDMObject.base().velocity());
        }
    }

    if (osiDMObject.has_base_rmse()) {
        if (osiDMObject.base_rmse().has_acceleration()) {
            adiPMODynamics.AccelerationError = mapPositionErrtoADI(osiDMObject.base_rmse().acceleration());
        }
        if (osiDMObject.base_rmse().has_velocity()) {
            adiPMODynamics.VelocityObjectLevelError = mapPositionErrtoADI(osiDMObject.base_rmse().velocity());
        }
    }

    return adiPMODynamics;
}

::PotentiallyMovingObjectsLights PMObjectMapper::mapPMOLights(const osi3::DetectedMovingObject& osiDMObject,
    const int& index)
{
    ::PotentiallyMovingObjectsLights adiPMOLights{};
    ara::adi::sensoritf::PotentiallyMovingObjectsLightVector adiPMOVector;
    ara::adi::sensoritf::PotentiallyMovingObjectsLight adiPMOLight;

    // TODO: Might have to check has_classification and has_light_state
    if (osiDMObject.candidate(index).vehicle_classification().light_state().has_brake_light_state()) {
        osi3::MovingObject_VehicleClassification_LightState_BrakeLightState osiBrakeLightState
            = osiDMObject.candidate(index).vehicle_classification().light_state().brake_light_state();

        // TODO: Mapping all to Center Brake Light

        switch (osiBrakeLightState) {
        case osi3::MovingObject_VehicleClassification_LightState::BRAKE_LIGHT_STATE_NORMAL: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kCentreBrakeLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOn;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState::BRAKE_LIGHT_STATE_OFF: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kCentreBrakeLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOff;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState::BRAKE_LIGHT_STATE_OTHER: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kCentreBrakeLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
            // case osi3::MovingObject_VehicleClassification_LightState::BRAKE_LIGHT_STATE_STRONG:
            //{
            //    adiPMOLights.PotentiallyMovingObjectsLightList.at(0).PMOLightStatus =
            //        ara::adi::sensoritf::LightStatus::kFlash;
            //    break;
            //}

        case osi3::MovingObject_VehicleClassification_LightState::BRAKE_LIGHT_STATE_UNKNOWN: {
            // TODO: Print warning

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kCentreBrakeLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        default: {
            m_logger.LogError() << "Error: mapPMOLights: Unknown enum entry"
                                   "mapped to LightStatus::kOther for LightType::kCentreBrakeLight";
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kCentreBrakeLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        }
    }

    // TODO: Might have to check has_classification and has_light_state
    // Push indicator light state in same vector
    if (osiDMObject.candidate(index).vehicle_classification().light_state().has_indicator_state()) {
        osi3::MovingObject_VehicleClassification_LightState_IndicatorState osiIndicatorState
            = osiDMObject.candidate(index).vehicle_classification().light_state().indicator_state();

        switch (osiIndicatorState) {
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kFlash;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF: {
            // TODO: Flash light check
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOff;
            adiPMOVector.push_back(adiPMOLight);

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOff;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OTHER: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kFlash;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_UNKNOWN: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }
        case osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kWarning;
            adiPMOVector.push_back(adiPMOLight);

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kWarning;
            adiPMOVector.push_back(adiPMOLight);
            break;
        }

        default: {
            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kLeftFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);

            adiPMOLight.PMOLightType = ara::adi::sensoritf::LightType::kRightFlashLight;
            adiPMOLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kOther;
            adiPMOVector.push_back(adiPMOLight);
            m_logger.LogError() << "Error: mapPMOLights: Unknown enum entry for IndicatorState"
                                   "mapped to LightStatus::kOther for LightType::kLeftFlashLight and kRightFlashLight";
            break;
        }
        }
    }

    return adiPMOLights;
}

::PotentiallyMovingObjectsPerson PMObjectMapper::mapPMOPerson(const osi3::DetectedMovingObject& osiDMObject,
    const int& index)
{
    ::PotentiallyMovingObjectsPerson adiPMOPerson{};

    ara::adi::sensoritf::ValidPersonPose adiValidPersonPose;

    if (osiDMObject.candidate(index).has_type()) {
        if (osiDMObject.candidate(index).type() == osi3::MovingObject_Type::MovingObject_Type_TYPE_PEDESTRIAN) {

            Orientation3D personPoseInfo{};
            if (osiDMObject.candidate(index).has_head_pose()) {
                personPoseInfo = mapOrientationtoADI(osiDMObject.candidate(index).head_pose());
                adiValidPersonPose.PersonPoseInfo = personPoseInfo;
                adiValidPersonPose.PoseType = ara::adi::sensoritf::PersonPoseType::kHead;

                adiPMOPerson.PersonPoselist.push_back(adiValidPersonPose);
            }

            // TODO: Check this mapping
            if (osiDMObject.candidate(index).has_upper_body_pose()) {
                personPoseInfo = mapOrientationtoADI(osiDMObject.candidate(index).upper_body_pose());
                adiValidPersonPose.PersonPoseInfo = personPoseInfo;
                adiValidPersonPose.PoseType = ara::adi::sensoritf::PersonPoseType::kUpperBody;

                adiPMOPerson.PersonPoselist.push_back(adiValidPersonPose);
            }
        }
    }

    // adiPMOPerson.NumberOfValidPersonSPoses

    // adiPMOPerson.PersonPoselist.at(0).PersonPoseError

    return adiPMOPerson;
}

// ::PotentiallyMovingObjectsLaneRelatedInformation PMObjectMapper::mapPMOLaneRelatedInformation(
//     const osi3::DetectedMovingObject& osiDMObject,
//     const int& index)
// {

// Exact Data unavailable
//::PotentiallyMovingObjectsLaneRelatedInformation adiPMORelatedInfo{};
//
// adiPMORelatedInfo.AngleBetweenObjectEdgeAndLane.LeftEdgeRightLane;
// adiPMORelatedInfo.AngleBetweenObjectEdgeAndLane.RightEdgeLeftLane;
//
// adiPMORelatedInfo.AngleBetweenObjectEdgeAndLaneError;
//
// adiPMORelatedInfo.LaneAssociation;
// adiPMORelatedInfo.PercentageSideLane.Left;
//
// osiDMObject.candidate(index).moving_object_classification().assigned_lane_id();
// osiDMObject.candidate(index).moving_object_classification().assigned_lane_id_size();
// osiDMObject.candidate(index).moving_object_classification().assigned_lane_percentage().
//
//
// return adiPMORelatedInfo;
// }

::PotentiallyMovingObjectsMotionRelatedInformation PMObjectMapper::mapPMOMotionInformation()
{
    ::PotentiallyMovingObjectsMotionRelatedInformation adiPMOMotionInfo{};

    // adiPMOMotionInfo.AngularPositionAzimuth
    return adiPMOMotionInfo;
}

//::PotentiallyMovingObjectsCameraSensorTechnologySpecific PMObjectMapper::mapCameraSensorSpecific() {
//    ::PotentiallyMovingObjectsCameraSensorTechnologySpecific adi
//}
//::PotentiallyMovingObjectsRadarSensorTechnologySpecific PMObjectMapper::mapRadarSensorSpecific() {
//
//}
// PotentiallyMovingObjectsLidarSensorTechnologySpecific PMObjectMapper::mapLidarSensorSpecific() {
//
//}
