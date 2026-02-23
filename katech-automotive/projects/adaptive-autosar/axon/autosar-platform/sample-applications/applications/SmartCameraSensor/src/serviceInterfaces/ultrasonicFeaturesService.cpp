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

#include "serviceInterfaces/ultrasonicFeaturesService.h"

#include <thread>

UltrasonicFeaturesService::UltrasonicFeaturesService(ara::com::InstanceIdentifier instance_id)
    : UFSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&UltrasonicFeaturesService::ProcessRequests, this)
{ }

UltrasonicFeaturesService::~UltrasonicFeaturesService()
{
    m_finished_ = true;
    m_worker_.join();
}

void UltrasonicFeaturesService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize UltrasonicFeature Service";
    OfferService();
}

void UltrasonicFeaturesService::SendEvent()
{
    // allocate sample
    auto allocation_result = UltrasonicFeatureInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "UltrasonicFeatureInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleUltrasonicFeature = std::move(allocation_result).Value();
    l_sampleUltrasonicFeature = std::move(createUltrasonicFeature(std::move(l_sampleUltrasonicFeature)));

    // send sample
    auto send_result = UltrasonicFeatureInterfaceEvent.Send(std::move(l_sampleUltrasonicFeature));
    if (send_result) {
        m_logger_.LogInfo() << "UltrasonicFeatureInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "UltrasonicFeatureInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto UltrasonicFeaturesService::UltrasonicFeaturesCapability() -> decltype(UFSSkeleton::UltrasonicFeaturesCapability())
{
    m_logger_.LogInfo() << "Call UltrasonicFeaturesService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 UltrasonicFeaturesService Capability Vector
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

    UltrasonicFeaturesService::UltrasonicFeaturesCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor UltrasonicFeaturesService capability: " << *it;
    }
    decltype(UFSSkeleton::UltrasonicFeaturesCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void UltrasonicFeaturesService::ProcessRequests()
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

UltrasonicFeaturesService::ptrUltrasonicFeatureInterface UltrasonicFeaturesService::createUltrasonicFeature(
    UltrasonicFeaturesService::ptrUltrasonicFeatureInterface pUltrasonicFeaturesInterface)
{
    // TODO fill the interface with dummy data
    DummyDataGenerator generator;
    pUltrasonicFeaturesInterface->UltrasonicFeatureInterfaceHeader = generator.getInterfaceHeader();

    // RecognizedFeaturesCapability / uint32_t (optional)
    pUltrasonicFeaturesInterface->RecognizedFeaturesCapability = 169;  // (optional)

    // RecognizedFeaturesStatus / RecognizedStatus (optional) / uint8_t
    pUltrasonicFeaturesInterface->RecognizedFeaturesStatus
        = ara::adi::sensoritf::RecognizedStatus::kLimited;  // 0x02 // (optional)

    // NumberOfValidFeatures / uint32_t
    pUltrasonicFeaturesInterface->NumberOfValidFeatures = 22;

    // ValidUltrasonicFeaturesList / ValidUltrasonicFeatureVector / vector <UltrasonicFeature>
    ara::adi::sensoritf::ValidUltrasonicFeatureVector featureVector
        = ara::adi::sensoritf::ValidUltrasonicFeatureVector();
    ara::adi::sensoritf::UltrasonicFeature usFeature;
    // FeatureStatus
    ara::adi::sensoritf::FeatureStatus featureStatus;
    featureStatus.ExistenceProbabilityFeatureLevel = 0.2;
    featureStatus.FeatureID = 12;  // (optional)
    featureStatus.FeatureGroupingID = 9;  // (optional)
    featureStatus.ObjectIDReferenceFeatureLevel = 76;  // (optional)
    featureStatus.TimeStampDifferenceFeatureLevel = 123456;  // (optional)
    featureStatus.NumberOfValidObservationsFeatureLevel = 2;  // (optional)
    ara::adi::sensoritf::ValidObservationVector validObservationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation validObservation;
    validObservation.TimeStampReferenceObjectLevel = 123456;
    validObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationTrue;
    validObservationVector.push_back(validObservation);
    featureStatus.ValidObservations = validObservationVector;  // (optional)
    usFeature.UltrasonicFeaturesStatus = featureStatus;

    ara::adi::sensoritf::UltrasonicSegmentInformation segmentInformation;
    segmentInformation.NumberOfValidUltrasonicFeatureClassifications = 45;
    ara::adi::sensoritf::ValidUltrasonicFeatureClassificationVector featureClassificationVector
        = ara::adi::sensoritf::ValidUltrasonicFeatureClassificationVector();
    ara::adi::sensoritf::UltrasonicFeatureClassification featureClassification;
    featureClassification.UltrasonicFeatureClassificationType
        = ara::adi::sensoritf::UltrasonicFeatureClassificationType::kUnknown;
    featureClassification.UltrasonicFeatureClassificationTypeConfidence = 0.9;
    featureClassificationVector.push_back(featureClassification);
    segmentInformation.ValidUltrasonicFeatureClassificationsList = featureClassificationVector;
    usFeature.UltrasonicFeaturesSegmentInformation = segmentInformation;

    ara::adi::sensoritf::UltrasonicSegmentPoints segmentPoints;
    segmentPoints.NumberOfValidPoints = 12;
    ara::adi::sensoritf::ValidSegmentPointVector segmentPointVector = ara::adi::sensoritf::ValidSegmentPointVector();
    ara::adi::sensoritf::SegmentPoint segmentPoint;
    ara::adi::sensoritf::Point3D point;
    point.x = 1;
    point.y = 2;
    point.z = 3;  // (optional)
    segmentPoint.Position = point;
    ara::adi::sensoritf::Point3DError pointError;
    pointError.xError = 0.1;
    pointError.yError = 0.2;
    pointError.zError = 0.3;
    segmentPoint.PositionError = pointError;
    segmentPoint.OrientationPitch = 0.5;  // (optional)
    segmentPoint.OrientationPitchError = 0.1;  // (optional)
    segmentPoint.Height = 11;  // (optional)
    segmentPoint.HeightError = 2.1;  // (optional)
    ara::adi::sensoritf::Point2D point2d;
    point2d.x = 10;
    point2d.y = 11;
    segmentPoint.VelocityUltrasonic = point2d;  // (optional)
    ara::adi::sensoritf::Point2DError point2dError;
    point2d.x = 0.1;
    point2d.y = 0.2;
    segmentPoint.VelocityUltrasonicError = point2dError;  // (optional)
    segmentPoint.TrilaterationStatus = ara::adi::sensoritf::TrilaterationStatus::kNotTrilaterated;
    segmentPoint.MeasurementStatusFeatureLevel
        = ara::adi::sensoritf::MeasurementStatusFeature::kMeasured;  // (optional)
    segmentPointVector.push_back(segmentPoint);
    segmentPoints.ValidSegmentPointsList = segmentPointVector;
    usFeature.UltrasonicFeaturesSegmentPoints = segmentPoints;
    featureVector.push_back(usFeature);
    pUltrasonicFeaturesInterface->ValidUltrasonicFeaturesList = featureVector;

    return pUltrasonicFeaturesInterface;
}
