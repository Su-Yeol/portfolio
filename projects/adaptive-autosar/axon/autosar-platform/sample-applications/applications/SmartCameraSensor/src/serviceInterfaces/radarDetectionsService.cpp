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

#include "serviceInterfaces/radarDetectionsService.h"

#include <thread>

RadarDetectionsService::RadarDetectionsService(ara::com::InstanceIdentifier instance_id)
    : RDSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&RadarDetectionsService::ProcessRequests, this)
{ }

RadarDetectionsService::~RadarDetectionsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void RadarDetectionsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize UltrasonicFeature Service";
    OfferService();
}

void RadarDetectionsService::SendEvent()
{
    // allocate sample
    auto allocation_result = RadarDetectionsInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "RadarDetectionsInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleRadarDetection = std::move(allocation_result).Value();
    l_sampleRadarDetection = std::move(createRadarDetection(std::move(l_sampleRadarDetection)));

    // send sample
    auto send_result = RadarDetectionsInterfaceEvent.Send(std::move(l_sampleRadarDetection));
    if (send_result) {
        m_logger_.LogInfo() << "RadarDetectionsInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "RadarDetectionsInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto RadarDetectionsService::RadarDetectionsCapability() -> decltype(RDSSkeleton::RadarDetectionsCapability())
{
    m_logger_.LogInfo() << "Call RadarDetectionsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 RadarDetectionsService Capability Vector
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

    RadarDetectionsService::RadarDetectionsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor RadarDetectionsService capability: " << *it;
    }
    decltype(RDSSkeleton::RadarDetectionsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void RadarDetectionsService::ProcessRequests()
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

RadarDetectionsService::ptrRadarDetectionsInterface RadarDetectionsService::createRadarDetection(
    RadarDetectionsService::ptrRadarDetectionsInterface pRadarDetectionsInterface)
{
    // TODO fill the interface with dummy data
    DummyDataGenerator generator;
    pRadarDetectionsInterface->RadarDetectionInterfaceHeader = generator.getInterfaceHeader();

    // RecognizedDetectionsCapability / uint32_t (optional)
    pRadarDetectionsInterface->RecognizedDetectionsCapability = 27;

    // RecognizedDetectionsStatus / RecognizedStatus (optional) / uint8_t
    pRadarDetectionsInterface->RecognizedDetectionsStatus = ara::adi::sensoritf::RecognizedStatus::kUnknown;  // 0X03

    // NumberOfValidDetections / uint32_t
    pRadarDetectionsInterface->NumberOfValidDetections = 21;

    // ValidRadarDetectionsList / ValidRadarDetectionVector / vector <RadarDetection>
    ara::adi::sensoritf::ValidRadarDetectionVector detectionVector = ara::adi::sensoritf::ValidRadarDetectionVector();
    ara::adi::sensoritf::RadarDetection detection;

    ara::adi::sensoritf::DetectionStatus detectionStatus;
    detectionStatus.ExistenceProbabilityDetectionLevel = 0.99;
    detectionStatus.ObjectID = 7;  // (optional)
    detectionStatus.FeatureID = 23;  // (optional)
    detectionStatus.TimeStampDifferenceDetectionLevel = 123456;
    detection.RadarDetectionsStatus = detectionStatus;

    ara::adi::sensoritf::RadarDetectionsInformation detectionInformation;
    detectionInformation.RadarCrossSection = 9.2;
    detectionInformation.RadarCrossSectionError = 0.2;  // (optional)
    detectionInformation.SignalToNoiseRatioDetectionLevel = 3.4;
    detectionInformation.SignalToNoiseRatioDetectionLevelError = 4.1;  // (optional)
    detectionInformation.MultiTargetProbability = 0.1;  // (optional)
    detectionInformation.AmbiguityID = 3;
    detectionInformation.DetectionAmbiguityProbability = 0.2;  // (optional)
    detectionInformation.FreeSpaceProbability = 0.5;  // (optional)
    detectionInformation.NumberOfValidDetectionClassifications = 2;  // (optional)
    ara::adi::sensoritf::ValidDetectionClassificationVector detectionClassificationVector
        = ara::adi::sensoritf::ValidDetectionClassificationVector();
    ara::adi::sensoritf::DetectionClassification detectionClassification;
    detectionClassification.DetectionClassificationType
        = ara::adi::sensoritf::DetectionClassificationType::kOverdrivable;  // (optional)
    detectionClassification.DetectionClassificationTypeConfidence = 0.4;  // (optional)
    detectionClassificationVector.push_back(detectionClassification);
    detectionInformation.ValidDetectionClassificationList = detectionClassificationVector;  // (optional)
    detection.RadarDetectionsInformation = detectionInformation;

    ara::adi::sensoritf::DetectionsPosition detectionPosition;
    ara::adi::sensoritf::Position3DSpheric spheric;
    spheric.azimuth = 2.3;
    spheric.elevation = 10.2;
    spheric.distance = 3;  // (optional)
    detectionPosition.DetectionPosition = spheric;
    ara::adi::sensoritf::Position3DSphericError sphericError;
    sphericError.azimuth = 2.3;
    sphericError.elevation = 10.2;
    sphericError.distance = 3;  // (optional)
    detectionPosition.DetectionPositionError = sphericError;
    detection.RadarDetectionsPosition = detectionPosition;

    ara::adi::sensoritf::DetectionsDynamics detectionDynamics;
    detectionDynamics.RelativeVelocityRadialDistance = 76.1;
    detectionDynamics.RelativeVelocityRadialDistanceError = 2.4;  // (optional)
    detection.RadarDetectionsDynamics = detectionDynamics;

    detectionVector.push_back(detection);
    pRadarDetectionsInterface->ValidRadarDetectionsList = detectionVector;

    return pRadarDetectionsInterface;
}
