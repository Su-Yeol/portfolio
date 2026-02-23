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

#include "serviceInterfaces/cameraFeaturesService.h"

#include <thread>

CameraFeaturesService::CameraFeaturesService(ara::com::InstanceIdentifier instance_id)
    : CFSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&CameraFeaturesService::ProcessRequests, this)
{ }

CameraFeaturesService::~CameraFeaturesService()
{
    m_finished_ = true;
    m_worker_.join();
}

void CameraFeaturesService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize CameraDetections Service";
    OfferService();
}

void CameraFeaturesService::SendEvent()
{
    // allocate sample
    auto allocation_result = CameraFeatureInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "CameraFeatureInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleCameraFeature = std::move(allocation_result).Value();
    l_sampleCameraFeature = std::move(createCameraFeature(std::move(l_sampleCameraFeature)));

    // send sample
    auto send_result = CameraFeatureInterfaceEvent.Send(std::move(l_sampleCameraFeature));
    if (send_result) {
        m_logger_.LogInfo() << "CameraFeatureInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "CameraFeatureInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto CameraFeaturesService::CameraFeaturesCapability() -> decltype(CFSSkeleton::CameraFeaturesCapability())
{
    m_logger_.LogInfo() << "Call CameraFeaturesService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.2.1 CameraFeaturesService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 106 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    CameraFeaturesService::CameraFeaturesCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor CameraFeaturesService capability: " << *it;
    }
    decltype(CFSSkeleton::CameraFeaturesCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void CameraFeaturesService::ProcessRequests()
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

CameraFeaturesService::ptrCameraFeatureInterface CameraFeaturesService::createCameraFeature(
    CameraFeaturesService::ptrCameraFeatureInterface pCameraFeatureInterface)
{
    // Fill the interface with dummy data
    DummyDataGenerator generator;
    pCameraFeatureInterface->CameraFeatureInterfaceHeader = generator.getInterfaceHeader();
    // uint32_t
    pCameraFeatureInterface->RecognizedFeaturesCapability = 987;  // (optional)
    // RecognizedStatus
    pCameraFeatureInterface->RecognizedFeaturesStatus
        = ara::adi::sensoritf::RecognizedStatus::kLimited;  // 0x02 // (optional)
    // uint32_t
    pCameraFeatureInterface->NumberOfValidFeatures = 5;
    // ValidCameraFeatureVector vector<CameraFeature>
    ara::adi::sensoritf::ValidCameraFeatureVector featureVector = ara::adi::sensoritf::ValidCameraFeatureVector();
    ara::adi::sensoritf::CameraFeature cameraFeature;
    // FeatureStatus
    ara::adi::sensoritf::FeatureStatus featureStatus;
    featureStatus.ExistenceProbabilityFeatureLevel = 0.8;
    featureStatus.FeatureID = 9;  // (optional)
    featureStatus.FeatureGroupingID = 7;  // (optional)
    featureStatus.ObjectIDReferenceFeatureLevel = 13;  // (optional)
    featureStatus.TimeStampDifferenceFeatureLevel = 123456;  // (optional)
    featureStatus.NumberOfValidObservationsFeatureLevel = 7;  // (optional)
    ara::adi::sensoritf::ValidObservationVector validObservationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation validObservation;
    validObservation.TimeStampReferenceObjectLevel = 123456;
    validObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kUnknown;
    validObservationVector.push_back(validObservation);
    featureStatus.ValidObservations = validObservationVector;  // (optional)
    cameraFeature.CameraFeaturesStatus = featureStatus;

    ara::adi::sensoritf::CameraFeaturesShapeInformation featureInformation;
    featureInformation.NumberOfValidShapeClassificationsFeatureLevel = 4;
    ara::adi::sensoritf::ValidShapeClassificationsVector shapeVector
        = ara::adi::sensoritf::ValidShapeClassificationsVector();
    ara::adi::sensoritf::ShapeClassification shape;
    shape.ShapeClassificationType = ara::adi::sensoritf::ShapeClassificationType::kRoad;
    shape.ShapeClassificationTypeConfidence = 0.9;
    shapeVector.push_back(shape);
    featureInformation.ValidShapeClassificationsList = shapeVector;
    cameraFeature.CameraFeaturesShapeInformation = featureInformation;

    ara::adi::sensoritf::ColourTone colourTone;
    colourTone.ValidColourModel.push_back(110);
    colourTone.ColourToneConfidenceObjectLevel = 0.7;  // (optional)
    cameraFeature.CameraFeaturesShapeColourTone = colourTone;

    ara::adi::sensoritf::ShapePoints shapePoints;
    shapePoints.ShapeType = ara::adi::sensoritf::ShapeType::kPolyline;
    shapePoints.NumberOfValidShapePoints = 3;
    ara::adi::sensoritf::ValidShapePointVector shapePointVector = ara::adi::sensoritf::ValidShapePointVector();
    ara::adi::sensoritf::ShapePoint shapePoint;
    shapePoint.PointExistenceProbability = 0.5;
    ara::adi::sensoritf::Point3D point;
    point.x = 1;
    point.y = 2;
    point.z = 3;
    shapePoint.Position = point;
    ara::adi::sensoritf::Point3DError pointError;
    pointError.xError = 0.1;
    pointError.yError = 0.2;
    pointError.zError = 0.3;
    shapePoint.PositionError = pointError;
    shapePointVector.push_back(shapePoint);
    shapePoints.ValidShapePointsList = shapePointVector;
    cameraFeature.CameraFeaturesShapePoints = shapePoints;

    ara::adi::sensoritf::ShapeReferencePoints referencePoints;
    referencePoints.NumberOfValidShapeReferencePointsFeatureLevel = 4;
    ara::adi::sensoritf::ValidShapeReferencePointVector referenceVector
        = ara::adi::sensoritf::ValidShapeReferencePointVector();
    ara::adi::sensoritf::ShapeReferencePoint referencePoint;
    referencePoint.PointExistenceProbability = 0.5;
    ara::adi::sensoritf::Point3D pointRef;
    pointRef.x = 4;
    pointRef.y = 5;
    pointRef.z = 6;
    referencePoint.Position = pointRef;
    ara::adi::sensoritf::Point3DError pointErrorRef;
    pointErrorRef.xError = 0.1;
    pointErrorRef.yError = 0.2;
    pointErrorRef.zError = 0.3;
    referencePoint.PositionError = pointErrorRef;
    referencePoint.ShapeSurfaceNormal = pointRef;  // (optional)
    referencePoint.ShapeSurfaceNormalError = pointErrorRef;  // (optional)
    referencePoint.TranslationRate = pointRef;  // (optional)
    referencePoint.TranslationRateError = pointErrorRef;  // (optional)
    ara::adi::sensoritf::Orientation3D orientationRef;
    orientationRef.Pitch = 2;
    orientationRef.Roll = 3;
    orientationRef.Yaw = 4.4;
    referencePoint.RotationRate = orientationRef;  // (optional)
    ara::adi::sensoritf::Orientation3DError orientationRefError;
    orientationRefError.PitchError = 2;
    orientationRefError.RollError = 3;
    orientationRefError.YawError = 4.4;
    referencePoint.RotationRateError = orientationRefError;  // (optional)
    referencePoint.ScaleChange = 5.5;  // (optional)
    referencePoint.ScaleChangeError = 0.5;  // (optional)

    referenceVector.push_back(referencePoint);
    referencePoints.ShapeReferencePointsList = referenceVector;
    cameraFeature.CameraFeaturesShapeReferencePoints = referencePoints;  // (optional)

    featureVector.push_back(cameraFeature);
    pCameraFeatureInterface->ValidCameraFeaturesList = featureVector;

    return pCameraFeatureInterface;
}
