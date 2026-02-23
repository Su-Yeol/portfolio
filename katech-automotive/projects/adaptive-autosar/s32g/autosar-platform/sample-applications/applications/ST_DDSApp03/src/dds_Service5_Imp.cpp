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

#include "dds_Service5_Imp.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <thread>
#include <stdint.h>
#include "ddsApp03_utils.hpp"

// includes for used services
using apd::shared::Position;

#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"
#include "ara/log/logger.h"

using namespace ara::log;  // 'using' ara::log should be OK

// <<operator implementation for logging custom types
// just another variant as also available in radar sources.
inline LogStream& operator<<(LogStream& out, const Position& value)
{
    std::ostringstream stream;
    stream << "x, y, z (" << value.x << "," << value.y << "," << value.z << ")";
    return (out << stream.str().c_str());
}

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

ddsService5Imp::ddsService5Imp()
    : service5_proxy(nullptr)
    , m_async_count(0)
{ }

void ddsService5Imp::StartFindService5()
{
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.

    auto res = DDSService5Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<DDSService5Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            ddsService5Imp::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier("DDS:33")));

    this->handle_ = res.ValueOrThrow();
}

void ddsService5Imp::StopFindService5()
{
    GetLogger().LogInfo() << "StopFindService5() enter";
    DDSService5Proxy::StopFindService(this->handle_);
}

void ddsService5Imp::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService5Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    for (auto it : handles) {
        GetLogger().LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == this->service5_proxy) {
            this->service5_proxy = std::make_shared<DDSService5Proxy>(handles[0]);
            GetLogger().LogInfo() << "Created proxy from handle with instance: "
                                  << this->service5_proxy->GetHandle().GetInstanceId().ToString();
        }
    }
}

void ddsService5Imp::asyncMethodCCall()
{
    Position target_position;
    target_position.x = 123;
    target_position.y = 456;
    target_position.z = 789;

    auto count = 0;

    // Increment the counter.
    m_async_count++;
    // Get future.
    auto carposition_future = this->service5_proxy->MethodC(target_position);
    // Polling the results.
    while (!carposition_future.is_ready()) {
        // Results are not ready yet.
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        GetLogger().LogInfo() << "METHODS: CarPosition"
                              << ", please wait ...";
        if (count == 100) {
            GetLogger().LogInfo() << "too long wait;skip act";
            get_method_data_c = "No CallBack";
            return;
        }
        count++;
        GetLogger().LogInfo() << "count:" << count;
    }

    // Get carposition result.

    auto carposition_output = carposition_future.get();

    if (carposition_output.success) {
        GetLogger().LogInfo() << "METHODS: "
                              << "car position was successful, effective position is ("
                              << carposition_output.effective_position;
        auto getValx = (std::uint16_t)carposition_output.effective_position.x;
        auto getValy = (std::uint16_t)carposition_output.effective_position.y;
        auto getValz = (std::uint16_t)carposition_output.effective_position.z;
        get_method_data_c = "METHOD_C: target position is " + std::to_string(getValx) + "," + std::to_string(getValy)
            + "," + std::to_string(getValz);
    } else {
        GetLogger().LogWarn() << "METHODS: "
                              << "car position was not successful, deviation from the target "
                                 "position is ("
                              << carposition_output.effective_position.x - target_position.x << ","
                              << carposition_output.effective_position.y - target_position.y << ","
                              << carposition_output.effective_position.z - target_position.z << ")";
        auto getValWarnx = (std::uint16_t)carposition_output.effective_position.x;
        auto getValWarny = (std::uint16_t)carposition_output.effective_position.y;
        auto getValWarnz = (std::uint16_t)carposition_output.effective_position.z;
        get_method_data_c = "METHOD_C: target position is " + std::to_string(getValWarnx) + ","
            + std::to_string(getValWarny) + "," + std::to_string(getValWarnz);
    }
}

void ddsService5Imp::asyncMethodDCall()
{
    std::string text = "MethodD has been called";

    this->service5_proxy->MethodD(text);
    GetLogger().LogInfo() << "METHODS:" << text;
    get_method_data_d = text;
}

std::string ddsService5Imp::SendRequestResult4()
{
    GetLogger().LogInfo() << "SendRequestResult4";
    std::string response;

    if (send_request_count == 0) {
        GetLogger().LogInfo() << "Send Value:" << get_method_data_c;
        response = get_method_data_c;
    } else {
        GetLogger().LogInfo() << "Send Value:" << get_method_data_d;
        response = get_method_data_d;
    }

    send_request_count++;

    return response;
}
