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

#include "serviceInterfaces/ultrasonicFeaturesServiceImp.h"

#include <thread>

UltrasonicFeaturesServiceImp::UltrasonicFeaturesServiceImp(ara::com::InstanceIdentifier instance_id)
    : UFSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&UltrasonicFeaturesServiceImp::ProcessRequests, this)
{ }

UltrasonicFeaturesServiceImp::~UltrasonicFeaturesServiceImp()
{
    m_finished_ = true;
    m_worker_.join();
}

void UltrasonicFeaturesServiceImp::InitializeService()
{
    m_logger_.LogInfo() << "Initialize UltrasonicFeature Service";
    OfferService();
}

void UltrasonicFeaturesServiceImp::SendEvent()
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

auto UltrasonicFeaturesServiceImp::UltrasonicFeaturesCapability()
    -> decltype(UFSSkeleton::UltrasonicFeaturesCapability())
{
    m_logger_.LogInfo() << "Call UltrasonicFeaturesServiceImp::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 UltrasonicFeaturesServiceImp Capability Vector
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

    UltrasonicFeaturesServiceImp::UltrasonicFeaturesCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor UltrasonicFeaturesServiceImp capability: " << *it;
    }
    decltype(UFSSkeleton::UltrasonicFeaturesCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void UltrasonicFeaturesServiceImp::ProcessRequests()
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

UltrasonicFeaturesServiceImp::ptrUltrasonicFeatureInterface UltrasonicFeaturesServiceImp::createUltrasonicFeature(
    UltrasonicFeaturesServiceImp::ptrUltrasonicFeatureInterface pUltrasonicFeaturesInterface)
{
    // TODO fill the interface with dummy data

    osi3::SensorData osiSensorData = createOsiDummyData();
    *pUltrasonicFeaturesInterface = ultrasonicFeaturesMapper.mapUltrasonicFeatureInterface(osiSensorData);

    return pUltrasonicFeaturesInterface;
}
