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

#include "serviceInterfaces/staticObjectsServiceImp.h"

#include <thread>

StaticObjectsServiceImp::StaticObjectsServiceImp(ara::com::InstanceIdentifier instance_id)
    : SOSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&StaticObjectsServiceImp::ProcessRequests, this)
{ }

StaticObjectsServiceImp::~StaticObjectsServiceImp()
{
    m_finished_ = true;
    m_worker_.join();
}

void StaticObjectsServiceImp::InitializeService()
{
    m_logger_.LogInfo() << "Initialize StaticObjects Service";
    OfferService();
}

void StaticObjectsServiceImp::SendEvent()
{
    // allocate sample
    auto allocation_result = StaticObjectInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "StaticObjectInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleStaticObjects = std::move(allocation_result).Value();
    l_sampleStaticObjects = std::move(createStaticObjects(std::move(l_sampleStaticObjects)));

    // send sample
    auto send_result = StaticObjectInterfaceEvent.Send(std::move(l_sampleStaticObjects));
    if (send_result) {
        m_logger_.LogInfo() << "StaticObjectInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "StaticObjectInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto StaticObjectsServiceImp::StaticObjectsCapability() -> decltype(SOSSkeleton::StaticObjectsCapability())
{
    m_logger_.LogInfo() << "Call StaticObjectsServiceImp::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 SObjectsService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 155 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    StaticObjectsServiceImp::StaticObjectsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor StaticObjectsServiceImp capability: " << *it;
    }
    decltype(SOSSkeleton::StaticObjectsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void StaticObjectsServiceImp::ProcessRequests()
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

StaticObjectsServiceImp::ptrStaticObjectsInterface StaticObjectsServiceImp::createStaticObjects(
    StaticObjectsServiceImp::ptrStaticObjectsInterface pStaticObjectsInterface)
{
    // Fill the interface with dummy data

    // uint8_t
    osi3::SensorData osiSensorData = createOsiDummyData();
    *pStaticObjectsInterface = adiStaticObjectsMapper.mapStaticObjectInterface(osiSensorData);

    return pStaticObjectsInterface;
}
