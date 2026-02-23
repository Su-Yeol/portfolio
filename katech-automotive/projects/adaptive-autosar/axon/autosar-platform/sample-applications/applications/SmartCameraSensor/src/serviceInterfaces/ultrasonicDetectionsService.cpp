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

#include "serviceInterfaces/ultrasonicDetectionsService.h"

#include <thread>

UltrasonicDetectionsService::UltrasonicDetectionsService(ara::com::InstanceIdentifier instance_id)
    : UDSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&UltrasonicDetectionsService::ProcessRequests, this)
{ }

UltrasonicDetectionsService::~UltrasonicDetectionsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void UltrasonicDetectionsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize UltrasonicDetections Service";
    OfferService();
}

void UltrasonicDetectionsService::SendEvent()
{
    // allocate sample
    auto allocation_result = UltrasonicDetectionsInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "UltrasonicDetectionsInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleUltrasonicDetection = std::move(allocation_result).Value();
    l_sampleUltrasonicDetection = std::move(createUltrasonicDetection(std::move(l_sampleUltrasonicDetection)));

    // send sample
    auto send_result = UltrasonicDetectionsInterfaceEvent.Send(std::move(l_sampleUltrasonicDetection));
    if (send_result) {
        m_logger_.LogInfo() << "UltrasonicDetectionsInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "UltrasonicDetectionsInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto UltrasonicDetectionsService::UltrasonicDetectionsCapability()
    -> decltype(UDSSkeleton::UltrasonicDetectionsCapability())
{
    m_logger_.LogInfo() << "Call UltrasonicDetectionsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 UltrasonicDetectionsService Capability Vector
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

    UltrasonicDetectionsService::UltrasonicDetectionsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor UltrasonicDetectionsService capability: " << *it;
    }
    decltype(UDSSkeleton::UltrasonicDetectionsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void UltrasonicDetectionsService::ProcessRequests()
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

UltrasonicDetectionsService::ptrUltrasonicDetectionInterface UltrasonicDetectionsService::createUltrasonicDetection(
    UltrasonicDetectionsService::ptrUltrasonicDetectionInterface pUltrasonicDetectionInterface)
{
    // Fill the interface with dummy data
    DummyDataGenerator generator;
    pUltrasonicDetectionInterface->UltrasonicDetectionsInterfaceHeader = generator.getInterfaceHeader();
    // uint8_t
    pUltrasonicDetectionInterface->RecognizedDetectionsCap = 2;  // (optional)
    // RecognizedStatus
    pUltrasonicDetectionInterface->RecognizedDetectionsStatus
        = ara::adi::sensoritf::RecognizedStatus::kUnknown;  // 0x03 // (optional)
    // uint32_t
    pUltrasonicDetectionInterface->NoValidDetections = 666;
    // ValidUltrasonicDetectionVector
    ara::adi::sensoritf::ValidUltrasonicDetectionVector detectionVector
        = ara::adi::sensoritf::ValidUltrasonicDetectionVector();
    ara::adi::sensoritf::UltrasonicDetection detection;
    ara::adi::sensoritf::DetectionStatus detectionStatus;
    detectionStatus.ExistenceProbabilityDetectionLevel = 0.15;
    detectionStatus.ObjectID = 8;  // (optional)
    detectionStatus.FeatureID = 15;  // (optional)
    detectionStatus.TimeStampDifferenceDetectionLevel = 123456;
    detection.UltrasonicDetectionStatus = detectionStatus;

    ara::adi::sensoritf::UltrasonicDetectionsInformation detectionInformation;
    detectionInformation.SecondSensorIDReference = 3.7;
    detectionInformation.Reflectivity = 7.3;
    detection.UltrasonicDetectionsInformation = detectionInformation;

    detectionVector.push_back(detection);
    pUltrasonicDetectionInterface->ValidUltrasonicDetectionList = detectionVector;

    return pUltrasonicDetectionInterface;
}
