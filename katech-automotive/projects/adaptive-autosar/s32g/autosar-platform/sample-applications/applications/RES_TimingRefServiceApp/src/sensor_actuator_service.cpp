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

#include "sensor_actuator_service.h"

#include <chrono>
#include <thread>

#include "ara/log/logger.h"
#include "ara/com/instance_identifier.h"

namespace apd
{
namespace wgres
{
namespace tra
{

SensorActuatorServiceImp::SensorActuatorServiceImp(ara::com::InstanceIdentifier instance_id)
    : Skeleton(instance_id, ara::com::MethodCallProcessingMode::kEvent)
    , periodMs_(250)
    , terminate_(false)
{ }

auto SensorActuatorServiceImp::Action(const std::uint32_t& intensity) -> decltype(Skeleton::Action(intensity))
{
    logger_.LogInfo() << "SensorActuatorService::action called with intensity " << intensity;

    uint32_t resultValue = intensity / 5;

    SensorActuatorService::ActionOutput result;
    result.return_value = resultValue;

    ara::core::Promise<SensorActuatorService::ActionOutput> retPromise;
    retPromise.set_value(std::move(result));

    // FIX for possible threading problem in vSomeIP which led to SEGFAULT
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    logger_.LogInfo() << "SensorActuator::action will return " << resultValue;
    return retPromise.get_future();
}

void SensorActuatorServiceImp::StartSendingData()
{
    terminate_ = false;
    sendThread_ = std::make_unique<std::thread>([&]() { this->SendPeriodic(); });
}

void SensorActuatorServiceImp::StopSendingData()
{
    terminate_ = true;
    if (sendThread_ != nullptr) {
        sendThread_->join();
        sendThread_.reset();
    }
}

void SensorActuatorServiceImp::SendPeriodic()
{
    int sensorValue = 0;
    while (!terminate_) {
        if (sensorValue > 110) {
            sensorValue = 0;
        }
        sensorData.Send(sensorValue++);
        std::this_thread::sleep_for(std::chrono::milliseconds(periodMs_));
    }
}

} /* namespace tra */
} /* namespace wgres */
} /* namespace apd */
