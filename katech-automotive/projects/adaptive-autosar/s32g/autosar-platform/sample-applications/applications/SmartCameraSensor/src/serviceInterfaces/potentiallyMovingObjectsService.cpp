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

#include "serviceInterfaces/potentiallyMovingObjectsService.h"

#include <thread>

PotentiallyMovingObjectsService::PotentiallyMovingObjectsService(ara::com::InstanceIdentifier instance_id)
    : PMOSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&PotentiallyMovingObjectsService::ProcessRequests, this)
{ }

PotentiallyMovingObjectsService::~PotentiallyMovingObjectsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void PotentiallyMovingObjectsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize StaticObjects Service";
    OfferService();
}

void PotentiallyMovingObjectsService::SendEvent()
{
    // allocate sample
    auto allocation_result = PotentiallyMovingObjectInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "PotentiallyMovingObjectInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_samplePMObjects = std::move(allocation_result).Value();
    l_samplePMObjects = std::move(createPMObjects(std::move(l_samplePMObjects)));

    // send sample
    auto send_result = PotentiallyMovingObjectInterfaceEvent.Send(std::move(l_samplePMObjects));
    if (send_result) {
        m_logger_.LogInfo() << "PotentiallyMovingObjectInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "PotentiallyMovingObjectInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto PotentiallyMovingObjectsService::PotentiallyMovingObjectsCapability()
    -> decltype(PMOSSkeleton::PotentiallyMovingObjectsCapability())
{
    m_logger_.LogInfo() << "Call PotentiallyMovingObjectsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.1 PMObjectsService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 134 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    PotentiallyMovingObjectsService::PotentiallyMovingObjectsCapabilityOutput output;
    output.CapVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor PotentiallyMovingObjectsService capability: " << *it;
    }
    decltype(PMOSSkeleton::PotentiallyMovingObjectsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void PotentiallyMovingObjectsService::ProcessRequests()
{
    while (!m_finished_) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            m_logger_.LogFatal() << "Request took too long :S";
        } else {
            if (!m_finished_) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

PotentiallyMovingObjectsService::ptrPotentiallyMovingObjectsInterface PotentiallyMovingObjectsService::createPMObjects(
    PotentiallyMovingObjectsService::ptrPotentiallyMovingObjectsInterface pPotentiallyMovingObjectsInterface)
{
    // Fill the interface with dummy data
    DummyDataGenerator generator;
    pPotentiallyMovingObjectsInterface->PotentiallyMovingObjectInterfaceHeader = generator.getInterfaceHeader();
    // PotentiallyMovingObjectList / PotentiallyMovingObjects
    ara::adi::sensoritf::PotentiallyMovingObjects movingObjects;
    movingObjects.RecognizedPotentiallyMovingObjectsCapability = 43;  // (optional)
    movingObjects.RecognizedPotentiallyMovingObjectsStatus
        = ara::adi::sensoritf::RecognizedStatus::kNormal;  // (optional)
    movingObjects.NumberOfValidPotentiallyMovingObjects = 3;
    ara::adi::sensoritf::ValidPotentiallyMovingObjectVector movingObjectVector
        = ara::adi::sensoritf::ValidPotentiallyMovingObjectVector();
    ara::adi::sensoritf::ValidPotentiallyMovingObject movingObject;

    ara::adi::sensoritf::ObjectStatus objectStatus;
    objectStatus.ExistenceProbabilityObjectLevel = 0.77;
    objectStatus.ObjectID = 3;
    objectStatus.GroupingObjectID = 4;  // (optional)
    objectStatus.Age = 123456;
    objectStatus.NumberOfValidObservationsObjectLevel = 3;  // (optional)
    ara::adi::sensoritf::ValidObservationVector observationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation observation;
    observation.TimeStampReferenceObjectLevel = 234567;
    observation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationTrue;
    observationVector.push_back(observation);
    objectStatus.ValidObservations = observationVector;  // optional
    objectStatus.TrackQuality = 78;  // (optional)
    objectStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kPredictedOccluded;
    movingObject.PotentiallyMovingObjectsStatus = objectStatus;

    ara::adi::sensoritf::PotentiallyMovingObjectsInformation objectInformation;
    objectInformation.NumberOfValidPotentiallyMovingObjectClassifications = 3;
    ara::adi::sensoritf::ValidPotentiallyMovingObjectClassificationVector classificationVector
        = ara::adi::sensoritf::ValidPotentiallyMovingObjectClassificationVector();
    ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification classification;
    classification.PotentiallyMovingObjectClassificationType
        = ara::adi::sensoritf::PotentiallyMovingObjectClassificationType::kTrailer;
    classification.PotentiallyMovingObjectClassificationTypeConfidence = 0.2;
    classificationVector.push_back(classification);
    objectInformation.PotentiallyMovingObjectClassifications = classificationVector;
    movingObject.PotentiallyMovingObjectsInformation = objectInformation;

    ara::adi::sensoritf::PotentiallyMovingObjectsPosition objectPosition;
    ara::adi::sensoritf::Point3D positionPoint;
    positionPoint.x = 1;
    positionPoint.y = 2;
    positionPoint.z = 3;
    objectPosition.PositionObjectLevel = positionPoint;
    ara::adi::sensoritf::Point3DError positionPointError;
    positionPointError.xError = 0.1;
    positionPointError.yError = 0.2;
    positionPointError.zError = 0.3;
    objectPosition.PositionObjectLevelError = positionPointError;
    ara::adi::sensoritf::Orientation3D positionOrientation;
    positionOrientation.Pitch = 9.1;
    positionOrientation.Roll = 1.1;
    positionOrientation.Yaw = 4.6;
    objectPosition.Orientation = positionOrientation;  // (optional)
    ara::adi::sensoritf::Orientation3DError positionOrientationError;
    positionOrientationError.PitchError = 0.1;
    positionOrientationError.RollError = 0.2;
    positionOrientationError.YawError = 0.3;
    objectPosition.OrientationError = positionOrientationError;  // (optional)
    objectPosition.ReferencePoint = ara::adi::sensoritf::ReferencePoint::kFrontRightBottom;  // (optional)
    objectPosition.RoadLevel = ara::adi::sensoritf::RoadLevel::kRoadLevelAbove;  // (optional)
    movingObject.PotentiallyMovingObjectsPosition = objectPosition;

    ara::adi::sensoritf::PotentiallyMovingObjectsBoundingBox objectBoundingBox;
    ara::adi::sensoritf::DimensionBox dimension;
    dimension.Width = 3;
    dimension.Length = 1;
    dimension.Height = 4;  // (optional)
    objectBoundingBox.BoundingBoxExtent = dimension;
    ara::adi::sensoritf::DimensionBoxError dimensionError;
    dimensionError.Width = 3;
    dimensionError.Length = 1;
    dimensionError.Height = 4;  // (optional)
    objectBoundingBox.BoundingBoxExtentError = dimensionError;  // (optional)
    objectBoundingBox.BoundingBoxGroundClearance = 0.3;  // (optional)
    objectBoundingBox.IncludedGeometricStructures
        = ara::adi::sensoritf::IncludedGeometricStructures::kWithMirrors;  // (optional)
    movingObject.PotentiallyMovingObjectsBoundingBox = objectBoundingBox;  // (optional)

    ara::adi::sensoritf::PotentiallyMovingObjectsDynamics objectDynamics;
    ara::adi::sensoritf::Point3D dynamicPoint;
    dynamicPoint.x = 1;
    dynamicPoint.y = 2;
    dynamicPoint.z = 3;
    objectDynamics.VelocityObjectLevel = dynamicPoint;
    ara::adi::sensoritf::Point3DError dynamicPointError;
    dynamicPointError.xError = 0.1;
    dynamicPointError.yError = 0.2;
    dynamicPointError.zError = 0.3;
    objectDynamics.VelocityObjectLevelError = dynamicPointError;  // (optional)
    objectDynamics.Acceleration = dynamicPoint;  // (optional)
    objectDynamics.AccelerationError = dynamicPointError;  // (optional)
    ara::adi::sensoritf::Point2D dynamicPoint2d;
    dynamicPoint2d.x = 1;
    dynamicPoint2d.y = 2;
    objectDynamics.InstantaneousCentreOfRotation = dynamicPoint2d;  // (optional)
    ara::adi::sensoritf::Point2DError dynamicPoint2dError;
    dynamicPoint2dError.xError = 0.1;
    dynamicPoint2dError.yError = 0.2;
    objectDynamics.InstantaneousCentreOfRotationError = dynamicPoint2dError;  // (optional)
    objectDynamics.RotationRateAtInstantaneousCentreOfRotationYaw = 0.4;  // (optional)
    objectDynamics.RotationRateAtInstantaneousCentreOfRotationYawError = 0.1;  // (optional)
    objectDynamics.MovementStatus = ara::adi::sensoritf::MovementStatus::kStoppedMoving;  // (optional)
    movingObject.PotentiallyMovingObjectsDynamics = objectDynamics;

    ara::adi::sensoritf::PotentiallyMovingObjectsLights objectLights;
    objectLights.NumberOfValidLights = 6;
    ara::adi::sensoritf::PotentiallyMovingObjectsLightVector objectLightVector
        = ara::adi::sensoritf::PotentiallyMovingObjectsLightVector();
    ara::adi::sensoritf::PotentiallyMovingObjectsLight objectLight;
    objectLight.PMOLightType = ara::adi::sensoritf::LightType::kRightBrakeLight;
    objectLight.PMOLightStatus = ara::adi::sensoritf::LightStatus::kFlash;
    objectLightVector.push_back(objectLight);
    objectLights.PotentiallyMovingObjectsLightList = objectLightVector;
    movingObject.PotentiallyMovingObjectsLights = objectLights;  // (optional)

    ara::adi::sensoritf::PotentiallyMovingObjectsPerson objectPerson;
    objectPerson.NumberOfValidPersonSPoses = 2;
    ara::adi::sensoritf::PersonPoseVector personVector = ara::adi::sensoritf::PersonPoseVector();
    ara::adi::sensoritf::ValidPersonPose person;
    person.PoseType = ara::adi::sensoritf::PersonPoseType::kRightLowerLeg;
    ara::adi::sensoritf::Orientation3D personOrientation;
    personOrientation.Pitch = 3;
    personOrientation.Roll = 2;
    personOrientation.Yaw = 1;
    person.PersonPoseInfo = personOrientation;
    ara::adi::sensoritf::Orientation3DError personOrientationError;
    personOrientationError.PitchError = 0.3;
    personOrientationError.RollError = 0.2;
    personOrientationError.YawError = 0.1;
    person.PersonPoseError = personOrientationError;  // (optional)
    personVector.push_back(person);
    objectPerson.PersonPoselist = personVector;
    movingObject.PotentiallyMovingObjectsPerson = objectPerson;  // (optional)

    ara::adi::sensoritf::PotentiallyMovingObjectsLaneRelatedInformation objectLaneRelated;
    objectLaneRelated.LaneAssociation = ara::adi::sensoritf::ObjectLaneAssociation::kEgoRightLane;
    ara::adi::sensoritf::AngleBetweenObjectEdgeAndLaneRightEdgeLeftLane angleLane;
    angleLane.LeftEdgeRightLane = 10.2;
    angleLane.RightEdgeLeftLane = 0.2;
    objectLaneRelated.AngleBetweenObjectEdgeAndLane = angleLane;  // (optional)
    ara::adi::sensoritf::AngleBetweenObjectEdgeAndLaneRightEdgeLeftLaneError angleLaneError;
    angleLaneError.LeftEdgeRightLane = 0.3;
    angleLaneError.RightEdgeLeftLane = 4.1;
    objectLaneRelated.AngleBetweenObjectEdgeAndLaneError = angleLaneError;  // (optional)
    ara::adi::sensoritf::PercentageSideLane sideLane;
    sideLane.Left = 45;
    sideLane.Right = 30;
    objectLaneRelated.PercentageSideLane = sideLane;  // (optional)
    movingObject.PotentiallyMovingObjectsLaneRelatedInformation = objectLaneRelated;  // (optional)

    ara::adi::sensoritf::PotentiallyMovingObjectsMotionRelatedInformation objectMotionRelated;
    objectMotionRelated.AngularPositionAzimuth = 4.5;
    objectMotionRelated.AngularVelocityAzimuth = 10.2;
    movingObject.PotentiallyMovingObjectsMotionInformation = objectMotionRelated;  // (optional)

    ara::adi::sensoritf::PotentiallyMovingObjectsCameraSensorTechnologySpecific objectCamera;
    objectCamera.ScaleChangeObjectLevel = 3;
    movingObject.CameraSensorSpecific = objectCamera;  // (optional)
    ara::adi::sensoritf::PotentiallyMovingObjectsRadarSensorTechnologySpecific objectRadar;
    objectRadar.EntityRadarCrossSection = 0.9;
    movingObject.RadarSensorSpecific = objectRadar;  // (optional)
    ara::adi::sensoritf::PotentiallyMovingObjectsLidarSensorTechnologySpecific objectLidar;
    objectLidar.EntityLidarReflectivity = 9.4;
    movingObject.LidarSensorSpecific = objectLidar;  // (optional)

    movingObjectVector.push_back(movingObject);
    movingObjects.ValidPotentiallyMovingObjects = movingObjectVector;
    pPotentiallyMovingObjectsInterface->PotentiallyMovingObjectList = movingObjects;

    return pPotentiallyMovingObjectsInterface;
}
