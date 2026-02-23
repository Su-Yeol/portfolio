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

#include "dds_Service4_Imp.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdint.h>

// includes for used services

using apd::testsuite::cmdds::DDSVehicleState;
using apd::testsuite::cmdds::DDSFusionVariant;

#include "ara/core/instance_specifier.h"
#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"
#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace
// <<operator implementation for logging custom types
// just another variant as also available in radar sources.

ddsService4Imp::ddsService4Imp()
    : service4_proxy(nullptr)
    , m_sync_count(0)
{ }

void ddsService4Imp::StartFindService4()
{
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.
    GetLogger().LogInfo() << "StartFindService4() enter";

    auto res = DDSService4Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<DDSService4Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            ddsService4Imp::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier("DDS:32")));

    if (!res) {
        GetLogger().LogError() << "StartFindService failed with error:" << res.Error();
        throw std::runtime_error{"StartFindService failed"};
    }

    this->handle_ = res.ValueOrThrow();
}

void ddsService4Imp::StopFindService4()
{
    GetLogger().LogInfo() << "StopFindService4() enter";
    DDSService4Proxy::StopFindService(this->handle_);
}

void ddsService4Imp::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService4Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    for (auto it : handles) {
        GetLogger().LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == this->service4_proxy) {
            this->service4_proxy = std::make_shared<DDSService4Proxy>(handles[0]);
            GetLogger().LogInfo() << "Created proxy from handle with instance: "
                                  << this->service4_proxy->GetHandle().GetInstanceId().ToString();
        }
    }
}

void ddsService4Imp::syncMethodACall()
{
    // Increment the counter.
    m_sync_count++;
    // Prepare the Vehicle State.
    DDSVehicleState state;

    if ((m_sync_count % 3) == 0) {
        state = DDSVehicleState::DRIVING;
    } else if ((m_sync_count % 5) == 0) {
        state = DDSVehicleState::STARTUP;
    } else if ((m_sync_count % 7) == 0) {
        state = DDSVehicleState::PARKING;
    } else {
        state = DDSVehicleState::STARTUP;
    }

    // Prepare the calibration configuration string.
    String configuration = "carstate_config_" + std::to_string(m_sync_count);
    // Calling "carstate" method synchronously (get() should be used).
    auto carstate_future = this->service4_proxy->MethodA(configuration, state);
    GetLogger().LogInfo() << "METHODS: "
                          << "call Service4, please wait ...";

    auto r = carstate_future.GetResult();
    if (r.HasValue()) {
        auto carstate_output = r.Value();
        (void)carstate_output;
        auto getVal = (std::uint16_t)state;
        get_method_data_a = "METHOD_A:VehicleState-" + std::to_string(getVal);
        GetLogger().LogInfo() << "METHODS: "
                              << "Service 4 call was successful:" << get_method_data_a;
    } else {
        auto err = r.Error();
        GetLogger().LogWarn() << "APPLICATION_ERROR: "
                              << "Code: " << err.Value();
        GetLogger().LogWarn() << "APPLICATION_ERROR: "
                              << "Message: " << err.Message();
        // reset the count and continue
        m_sync_count = 0;
    }
}

void ddsService4Imp::syncMethodBCall()
{
    m_sync_count++;

    DDSFusionVariant variant;

    if ((m_sync_count % 3) == 0) {
        variant = DDSFusionVariant::FV_CHINA;
    } else if ((m_sync_count % 5) == 0) {
        variant = DDSFusionVariant::FV_USA;
    } else if ((m_sync_count % 7) == 0) {
        variant = DDSFusionVariant::FV_RUSSIA;
    } else {
        variant = DDSFusionVariant::FV_EUROPE;
    }

    // Prepare the calibration configuration string.
    String configuration = "calibration_config_" + std::to_string(m_sync_count);

    // Calling "Calibrate" method synchronously (GetResult() should be used).
    auto calibrate_future = service4_proxy->MethodB(configuration, variant);

    GetLogger().LogInfo() << "METHODS: "
                          << "Radar is calibrating, please wait ...";

    auto r = calibrate_future.GetResult();
    if (r.HasValue()) {
        auto calibrate_output = r.Value();
        (void)calibrate_output;
        auto getVal = (std::uint16_t)variant;
        get_method_data_b = "METHOD_B:Variant-" + std::to_string(getVal);
        GetLogger().LogInfo() << "METHODS: "
                              << "Calibration was successful" << get_method_data_b;
    } else {
        auto err = r.Error();
        GetLogger().LogWarn() << "APPLICATION_ERROR: "
                              << "Code: " << err.Value();
        GetLogger().LogWarn() << "APPLICATION_ERROR: "
                              << "Message: " << err.Message();
        // reset the count and continue
        m_sync_count = 0;
    }
}

std::string ddsService4Imp::SendRequestResult4()
{
    GetLogger().LogInfo() << "SendRequestResult4";
    std::string response;

    if (send_request_count == 0) {
        GetLogger().LogInfo() << "Send Value:" << get_method_data_a;
        response = get_method_data_a;
    } else {
        GetLogger().LogInfo() << "Send Value:" << get_method_data_b;
        response = get_method_data_b;
    }

    send_request_count++;

    return response;
}
