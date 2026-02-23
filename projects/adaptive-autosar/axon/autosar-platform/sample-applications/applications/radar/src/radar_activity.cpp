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

///////////////////////////////////////////////////////////////////////
// Activity specific implementation, skeleton can be generated from the model
// Discovery of services and sending/receiving of data according
// to the communication API
///////////////////////////////////////////////////////////////////////

#include "radar_activity.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>

#include "ara/com/sample/radar_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/com/internal/skeleton/mutable_field_dispatcher.h"

using ara::com::sample::Position;
using ara::com::sample::FusionVariant;

using namespace ara::log;

using ara::com::sample::SpecificErrorsErrc;
using ara::com::ComErrorDomainErrc;
using ara::com::sample::skeleton::fields::UpdateRate;
using ara::com::sample::skeleton::fields::RearObjectDistance;

// <<operator implementation for logging custom types
// similar handler is also available in fusion sources, so for real projects it make sense to
// have some common place where custom type log-handlers are provided.
inline LogStream& operator<<(LogStream& out, const Position& value)
{
    return (out << "x, y, z (" << value.x << "," << value.y << "," << value.z << ")");
}

namespace radar
{
auto radarImp::Adjust(const Position& target_position) -> decltype(Skeleton::Adjust(target_position))
{
    radarImp::AdjustOutput output;

    m_adjust_count++;

    m_logger_ctx1.LogInfo() << METHODS_HEADER << "Target position is" << target_position << ", adjusting position ...";

    if ((m_adjust_count % 7) == 0) {
        // Emulate error condition -- Adjusting position took too long
        std::this_thread::sleep_for(std::chrono::seconds(10));

        // set success to false & deviate the effective position.
        output.success = false;
        output.effective_position.x = target_position.x + m_ud_0_100(m_rand_eng);
        output.effective_position.y = target_position.y + m_ud_0_100(m_rand_eng);
        output.effective_position.z = target_position.z + m_ud_0_100(m_rand_eng);

        m_logger_ctx1.LogWarn() << METHODS_HEADER << "Adjusting position took too long time, there is something wrong";
        m_logger_ctx1.LogWarn() << METHODS_HEADER << "Effective position is" << output.effective_position;

    } else {
        // Adjusting position was successful, set success to true and show the effective position.
        output.success = true;
        output.effective_position = target_position;

        m_logger_ctx1.LogInfo() << METHODS_HEADER
                                << "Adjusting position was successful, effective position equals target position";
        m_logger_ctx1.LogWarn() << METHODS_HEADER << "Effective position is" << output.effective_position;
    }

    decltype(Skeleton::Adjust(target_position))::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

auto radarImp::Calibrate(const String& configuration, const FusionVariant& variant)
    -> decltype(Skeleton::Calibrate(configuration, variant))
{
    radarImp::CalibrateOutput output;

    m_calibrate_count++;

    m_logger_ctx2.LogInfo() << METHODS_HEADER << "Calibration configuration string is " << configuration
                            << ", Fusion variant is " << (std::uint16_t)variant << ", calibrating ...";

    if ((m_calibrate_count % 8) == 0) {
        // Emulate error condition -- Calibration took too long
        std::this_thread::sleep_for(std::chrono::seconds(10));
        output.result = false;
        m_logger_ctx2.LogWarn() << METHODS_HEADER << "Calibration took very long time, there is something wrong";
    } else {
        // Calibration was successful, set result to true.
        output.result = true;
        m_logger_ctx2.LogInfo() << METHODS_HEADER << "Calibration was successful";
    }

    // Set the promise value (or error in case calibration went wrong).
    decltype(Skeleton::Calibrate(configuration, variant))::PromiseType promise;
    if (!output.result) {
        m_logger_ctx2.LogWarn() << ERROR_HEADER << "Configuration string was invalid.";
        promise.SetError(ara::core::ErrorCode(SpecificErrorsErrc::kInvalidConfigString));
    } else {
        promise.set_value(std::move(output));
    }

    return promise.get_future();
}

void radarImp::Echo(const String& text)
{
    m_logger_ctx5.LogInfo() << METHODS_HEADER << text;
}

void radarImp::ProcessRequests()
{
    while (!m_finished) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            m_logger_ctx1.LogFatal() << "Request took too long :S";
        } else {
            if (!m_finished) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

RadarActivity::RadarActivity()
    : m_rand_eng(m_rd())
    , m_ud_0_100(0, 100)
    , m_ud_0_50(0, 50)
{
    ara::core::InstanceSpecifier instanceSpec{"radar/radar/radar_PPort"};
    m_logger_ctx4.LogInfo() << "Port In Executable Ref:" << instanceSpec.ToString();

    m_skeleton = new radarImp(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);

    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);
    for (auto const& instanceId : instanceIDs) {
        m_logger_ctx4.LogInfo() << "Service Instance offered:" << instanceId.ToString();
    }

    m_logger_ctx4.LogDebug() << "object address" << this;
}

RadarActivity::~RadarActivity()
{
    delete m_skeleton;
}

ara::core::Future<ara::com::sample::skeleton::fields::UpdateRate::value_type> RadarActivity::getUpdateRate()
{
    ara::core::Promise<ara::com::sample::skeleton::fields::UpdateRate::value_type> promise;
    // Field value is always accessible as per current spec
    m_update_rate = m_ud_0_100(m_rand_eng);
    m_logger_ctx3.LogInfo() << FIELDS_HEADER << "Getting the field Update Rate value :" << m_update_rate;
    promise.set_value(std::move(m_update_rate));
    return promise.get_future();
}

ara::core::Future<ara::com::sample::skeleton::fields::RearObjectDistance::value_type>
RadarActivity::getRearObjectDistance()
{
    std::uint16_t distance = m_ud_0_50(m_rand_eng);

    if (distance > m_object_limit) {
        distance = -1;
    }

    m_logger_ctx3.LogInfo() << FIELDS_HEADER << "Getting the Rear Object Distance Distance :" << (std::int16_t)distance;

    ara::core::Promise<ara::com::sample::skeleton::fields::RearObjectDistance::value_type> promise;
    promise.set_value(std::move(distance));
    return promise.get_future();
}

ara::core::Future<uint32_t> RadarActivity::setUpdateRate(uint32_t field)
{
    ara::core::Promise<uint32_t> promise;
    switch (m_internal_state_for_update_rate_set_handler) {
    // field setters indicates all errors by keeping previous value of field as per current spec
    case internalStates::READY: {
        m_update_rate = field;
        m_logger_ctx3.LogInfo() << FIELDS_HEADER << "Setting the field Update Rate value to " << m_update_rate;
        promise.set_value(std::move(m_update_rate));
        m_internal_state_for_update_rate_set_handler = internalStates::NOT_READY;
        break;
    }
    case internalStates::NOT_READY: {
        m_logger_ctx3.LogInfo() << FIELDS_HEADER << "Simulating unreadiness, keeping previous value " << m_update_rate;
        promise.set_value(std::move(m_update_rate));
        m_internal_state_for_update_rate_set_handler = internalStates::READY;
        break;
    }
    }
    return promise.get_future();
}

ara::core::Future<uint16_t> RadarActivity::setObjectDetectionLimit(uint16_t field)
{
    m_object_limit = field;
    m_logger_ctx3.LogInfo() << FIELDS_HEADER << "Setting the Object Detection Distance Limit to " << m_object_limit;
    ara::core::Promise<uint16_t> promise;
    promise.set_value(std::move(m_object_limit));
    return promise.get_future();
}

void RadarActivity::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";

    // Init cached version of the Update Rate field.
    this->m_update_rate = 0;

    // Register Field Getters.
    m_skeleton->UpdateRate.RegisterGetHandler(std::bind(&RadarActivity::getUpdateRate, this));
    m_skeleton->RearObjectDistance.RegisterGetHandler(std::bind(&RadarActivity::getRearObjectDistance, this));

    // Register Field Setters.
    m_skeleton->UpdateRate.RegisterSetHandler(std::bind(&RadarActivity::setUpdateRate, this, std::placeholders::_1));

    m_skeleton->ObjectDetectionLimit.RegisterSetHandler(
        std::bind(&RadarActivity::setObjectDetectionLimit, this, std::placeholders::_1));

    // Initialize Fields Values before Offering Service.
    m_skeleton->UpdateRate.Update(0);
    m_skeleton->FrontObjectDistance.Update(0);
    m_skeleton->RearObjectDistance.Update(0);
    m_skeleton->ObjectDetectionLimit.Update(0);

    ara::core::Result<void> handler_result = m_skeleton->OfferService();
    if (!handler_result.HasValue()) {
        m_logger_ctx3.LogError() << "Get handler not registered:" << handler_result.Error();
    } else {
        m_logger_ctx3.LogDebug() << "Get handler registered";
    }
}

void RadarActivity::act()
{
    m_logger_ctx3.LogInfo() << "radar active";

    static uint8_t i = 0;

    // allocate sample
    auto allocation_result = m_skeleton->brakeEvent.Allocate();
    if (!allocation_result) {
        m_logger_ctx3.LogError() << "brakeEvent allocation failed with error: " << allocation_result.Error();
    } else {
        auto l_sampleRadar = std::move(allocation_result).Value();

        // write data
        if (i % 5 == 0) {
            l_sampleRadar->active = true;
        }
        l_sampleRadar->objectVector.push_back(0x10);
        l_sampleRadar->objectVector.push_back(0x20);
        l_sampleRadar->objectVector.push_back(0x30);
        l_sampleRadar->objectVector.push_back(0x40);
        l_sampleRadar->objectVector.push_back(i);

        // send sample
        auto send_result = m_skeleton->brakeEvent.Send(std::move(l_sampleRadar));
        if (send_result) {
            m_logger_ctx3.LogInfo() << "brakeEvent sent";
        } else {
            m_logger_ctx3.LogError() << "brakeEvent.Send failed with error: " << send_result.Error();
        }
    }

    // same for parking brake
    auto pb_allocation_result = m_skeleton->parkingBrakeEvent.Allocate();
    if (!pb_allocation_result) {
        m_logger_ctx3.LogError() << "parkingBrakeEvent allocation failed with error: " << pb_allocation_result.Error();
    } else {
        auto l_sampleParkingBrake = std::move(pb_allocation_result).Value();

        if (i % 5 != 0) {
            l_sampleParkingBrake->active = true;
        }
        l_sampleParkingBrake->objectVector.push_back(255 - i);

        // FIX for possible threading problem in vSomeIP which led to SEGFAULT
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // send sample
        auto send_result = m_skeleton->parkingBrakeEvent.Send(std::move(l_sampleParkingBrake));
        if (send_result) {
            m_logger_ctx3.LogInfo() << "parkingBrakeEvent sent";
        } else {
            m_logger_ctx3.LogError() << "parkingBrakeEvent.Send failed with error: " << send_result.Error();
        }
    }
    // Update Field Value and send notification.
    if (i % 5 == 0) {
        m_update_rate = i * 1000;
        auto update_result = m_skeleton->UpdateRate.Update(m_update_rate);
        if (update_result) {
            m_logger_ctx3.LogInfo() << "UpdateRate updated";
        } else {
            m_logger_ctx3.LogError() << "UpdateRate.Update failed with error: " << update_result.Error();
        }
    }

    std::uint16_t distance = m_ud_0_50(m_rand_eng);
    if (distance < m_object_limit) {
        auto update_result = m_skeleton->FrontObjectDistance.Update(distance);
        if (update_result) {
            m_logger_ctx3.LogInfo() << "FrontObjectDistance updated " << distance;
        } else {
            m_logger_ctx3.LogError() << "FrontObjectDistance.Update failed with error: " << update_result.Error();
        }
    } else {
        auto update_result = m_skeleton->FrontObjectDistance.Update(-1);
        if (update_result) {
            m_logger_ctx3.LogInfo() << "FrontObjectDistance updated: -1";
        } else {
            m_logger_ctx3.LogError() << "FrontObjectDistance.Update failed with error: " << update_result.Error();
        }
    }
    i++;
}
}  // namespace radar
