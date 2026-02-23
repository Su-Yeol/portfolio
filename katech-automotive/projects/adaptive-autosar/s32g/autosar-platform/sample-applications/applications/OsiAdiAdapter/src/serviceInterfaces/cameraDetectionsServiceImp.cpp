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

#include "serviceInterfaces/cameraDetectionsServiceImp.h"
#include "osi3/osi_sensordata.pb.h"
#include "createOsiDummyData.h"
#include <thread>

CameraDetectionsServiceImp::CameraDetectionsServiceImp(ara::com::InstanceIdentifier instance_id)
    : CDSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&CameraDetectionsServiceImp::ProcessRequests, this)
{ }

CameraDetectionsServiceImp::~CameraDetectionsServiceImp()
{
    m_finished_ = true;
    m_worker_.join();
}

void CameraDetectionsServiceImp::InitializeService()
{
    m_logger_.LogInfo() << "Initialize CameraDetections Service";
    OfferService();
}

void CameraDetectionsServiceImp::SendEvent()
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

auto CameraDetectionsServiceImp::CameraDetectionsCapability() -> decltype(CDSSkeleton::CameraDetectionsCapability())
{
    m_logger_.LogInfo() << "Call CameraDetectionsServiceImp::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 CameraDetectionsServiceImp Capability Vector
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

    CameraDetectionsServiceImp::CameraDetectionsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor CameraDetectionsServiceImp capability: " << *it;
    }
    decltype(CDSSkeleton::CameraDetectionsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void CameraDetectionsServiceImp::ProcessRequests()
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

CameraDetectionsServiceImp::ptrCameraDetectionInterface CameraDetectionsServiceImp::createCameraDetection(
    CameraDetectionsServiceImp::ptrCameraDetectionInterface pCameraDetectionInterface)
{
    // TODO: Fill the interface with OSI to ADI dummy data
    osi3::SensorData osiSensorData = createOsiDummyData();
    *pCameraDetectionInterface = cameraDetectionsMapper.mapCameraDetectionsInterface(osiSensorData);

    return pCameraDetectionInterface;
}
