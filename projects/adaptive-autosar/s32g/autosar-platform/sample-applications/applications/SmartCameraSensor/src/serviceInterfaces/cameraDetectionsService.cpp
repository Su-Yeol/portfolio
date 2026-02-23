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

#include "serviceInterfaces/cameraDetectionsService.h"

#include <thread>

CameraDetectionsService::CameraDetectionsService(ara::com::InstanceIdentifier instance_id)
    : CDSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&CameraDetectionsService::ProcessRequests, this)
{ }

CameraDetectionsService::~CameraDetectionsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void CameraDetectionsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize CameraDetections Service";
    OfferService();
}

void CameraDetectionsService::SendEvent()
{
    // allocate sample
    auto allocation_result = CameraDetectionsEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "CameraDetectionsEvent allocation failed with error: " << allocation_result.Error();
        return;
    }

    auto l_sampleCameraDetection = std::move(allocation_result).Value();
    l_sampleCameraDetection = std::move(createCameraDetection(std::move(l_sampleCameraDetection)));

    // send sample
    auto send_result = CameraDetectionsEvent.Send(std::move(l_sampleCameraDetection));
    if (send_result) {
        m_logger_.LogInfo() << "CameraDetectionsEvent sent";
    } else {
        m_logger_.LogError() << "CameraDetectionsEvent.Send failed with error: " << send_result.Error();
    }
}

auto CameraDetectionsService::CameraDetectionsCapability() -> decltype(CDSSkeleton::CameraDetectionsCapability())
{
    m_logger_.LogInfo() << "Call CameraDetectionsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 CameraDetectionsService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 87 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    CameraDetectionsService::CameraDetectionsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor CameraDetectionsService capability: " << *it;
    }
    decltype(CDSSkeleton::CameraDetectionsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void CameraDetectionsService::ProcessRequests()
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

CameraDetectionsService::ptrCameraDetectionInterface CameraDetectionsService::createCameraDetection(
    CameraDetectionsService::ptrCameraDetectionInterface pCameraDetectionInterface)
{
    // Fill the interface with dummy data
    // InterfaceHeader
    DummyDataGenerator generator;
    pCameraDetectionInterface->CameraDetectionInterfaceHeader = generator.getInterfaceHeader();

    // uint32_t
    pCameraDetectionInterface->RecognizedDetectionsCap = 345;  // (optional)
    // RecognizedStatus
    pCameraDetectionInterface->RecognizedDetectionsStatus
        = ara::adi::sensoritf::RecognizedStatus::kUnknown;  // 0x03 // (optional)
    // uint32_t
    pCameraDetectionInterface->NumberOfValidShapes = 6;

    // ValidCameraShapeVector / vector <CameraShape>
    ara::adi::sensoritf::ValidCameraShapeVector cameraShapeVector = ara::adi::sensoritf::ValidCameraShapeVector();
    ara::adi::sensoritf::CameraShape cameraShape;

    ara::adi::sensoritf::DetectionStatus detectionStatus;
    detectionStatus.ExistenceProbabilityDetectionLevel = 0.45;
    detectionStatus.ObjectID = 2;  // (optional)
    detectionStatus.FeatureID = 45;  // (optional)
    detectionStatus.TimeStampDifferenceDetectionLevel = 123456;
    cameraShape.CameraShapesStatus = detectionStatus;

    ara::adi::sensoritf::CameraShapesShapeInformation shapesShapeInformation;
    shapesShapeInformation.FreeSpaceProbability = 0.8;  // (optional)
    shapesShapeInformation.NumberOfValidShapeClassificationsDetectionLevel = 3;
    ara::adi::sensoritf::ValidDetectionShapeClassificationVector classificationVector
        = ara::adi::sensoritf::ValidDetectionShapeClassificationVector();
    ara::adi::sensoritf::DetectionShapeClassification classification;
    classification.ShapeClassificationTypeDetectionLevel = ara::adi::sensoritf::ShapeClassificationType::kPedestrian;
    classification.ShapeClassificationTypeConfidenceDetectionLevel = 0.7;
    classificationVector.push_back(classification);
    shapesShapeInformation.ValidShapeClassificationsList = classificationVector;
    shapesShapeInformation.ShapeAmbiguityID = 23;  // (optional)
    cameraShape.CameraShapeInformation = shapesShapeInformation;

    ara::adi::sensoritf::ColourTone colourTone;
    colourTone.ValidColourModel.push_back(9.1);
    colourTone.ColourToneConfidenceObjectLevel = 0.8;  // (optional)
    cameraShape.CameraShapesShapeColourTone = colourTone;

    ara::adi::sensoritf::CameraShapesShapePoints shapePoints;
    shapePoints.ShapeTypeDetectionLevel = ara::adi::sensoritf::ShapeType::kPoint;
    shapePoints.NumberOfValidShapePointsDetectionLevel = 12;
    ara::adi::sensoritf::ValidShapePointDetectionLevelVector detectionLevelVector
        = ara::adi::sensoritf::ValidShapePointDetectionLevelVector();
    ara::adi::sensoritf::ShapePointDetectionLevel pointDetectionLevel;
    pointDetectionLevel.PointExistenceProbabilityDetectionLevel = 0.2;
    ara::adi::sensoritf::Position3DSpheric spheric;
    spheric.azimuth = 1;
    spheric.elevation = 2;
    spheric.distance = 3;  // (optional)
    pointDetectionLevel.Position = spheric;
    ara::adi::sensoritf::Position3DSphericError sphericError;
    sphericError.azimuth = 0.1;
    sphericError.elevation = 0.2;
    sphericError.distance = 0.3;  // (optional)
    pointDetectionLevel.PositionError = sphericError;
    detectionLevelVector.push_back(pointDetectionLevel);
    shapePoints.ValidShapePointsDetectionLevelList = detectionLevelVector;
    cameraShape.CameraShapePoints = shapePoints;

    ara::adi::sensoritf::CameraShapesShapeReferencePoints referencePoints;
    referencePoints.NumberOfValidShapeReferencePointsDetectionLevel = 45;
    ara::adi::sensoritf::ValidShapeReferencePointDetectionLevelVector referenceDetectionLevelVector
        = ara::adi::sensoritf::ValidShapeReferencePointDetectionLevelVector();
    ara::adi::sensoritf::ShapeReferencePointDetectionLevel referencePointDetectionLevel;
    referencePointDetectionLevel.PointExistenceProbabilityDetectionLevel = 0.99;
    referencePointDetectionLevel.Position = spheric;  // (optional)
    referencePointDetectionLevel.PositionError = sphericError;  // (optional)
    ara::adi::sensoritf::Point3D point;
    point.x = 4;
    point.y = 5;
    point.z = 6;  // (optional)
    referencePointDetectionLevel.TranslationRate = point;  // (optional)
    ara::adi::sensoritf::Point3DError pointError;
    pointError.xError = 7;
    pointError.yError = 8;
    pointError.zError = 9;  // (optional)
    referencePointDetectionLevel.TranslationRateError = pointError;  // (optional)
    referenceDetectionLevelVector.push_back(referencePointDetectionLevel);
    referencePoints.ValidShapeReferencePointsPointsDetectionLevelList = referenceDetectionLevelVector;
    cameraShape.CameraShapeReferencePoints = referencePoints;  // (optional)

    cameraShapeVector.push_back(cameraShape);
    pCameraDetectionInterface->ValidCameraDetectionList = cameraShapeVector;

    return pCameraDetectionInterface;
}
