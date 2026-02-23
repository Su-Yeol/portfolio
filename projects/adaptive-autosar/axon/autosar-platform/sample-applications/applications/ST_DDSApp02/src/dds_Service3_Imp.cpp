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

#include "dds_Service3_Imp.hpp"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

// includes for used services

#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"
#include "ara/log/logger.h"
using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

ddsService3Imp::ddsService3Imp()
    : service3_proxy(nullptr)
{ }

void ddsService3Imp::StartFindService3()
{
    GetLogger().LogInfo() << "startFindService3() enter";

    auto res = DDSService3Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<DDSService3Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            ddsService3Imp::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier("DDS:31")));
    if (!res) {
        GetLogger().LogError() << "StartFindService faild with error:" << res.Error();
        throw std::runtime_error{"StartFindService faild"};
    }
}

void ddsService3Imp::StopFindService3()
{
    GetLogger().LogInfo() << "StopFindService3() enter";
    DDSService3Proxy::StopFindService(this->handle_);
}

void ddsService3Imp::FieldAReceived()
{
    ara::log::LogStream logMsg{GetLogger().LogVerbose()};
    static uint32_t resVal;
    this->service3_proxy->FieldA.GetNewSamples(
        [&logMsg](auto sample) {
            logMsg << FIELDS_HEADER << "Callback:  Field A; " << static_cast<uint32_t>(*sample);
            resVal = static_cast<uint32_t>(*sample);
        },
        3);
    responseValue = resVal;
}

void ddsService3Imp::FieldASubscription()
{
    GetLogger().LogInfo() << "FieldA Subscription()";
    if (!this->service3_proxy->FieldA.IsSubscribed()) {

        LogStream logMsg{GetLogger().LogVerbose()};
        this->service3_proxy->FieldA.SetReceiveHandler([this]() { ddsService3Imp::FieldAReceived(); });

        // subscribe
        auto subscription_result = this->service3_proxy->FieldA.Subscribe(3);
        if (subscription_result.HasValue()) {
            GetLogger().LogInfo() << "Callback registered.";
        } else {
            GetLogger().LogError() << "Subscription faild with error: " << subscription_result.Error();
            return;
        }
        GetLogger().LogInfo() << "done";
    } else {
        GetLogger().LogInfo() << " already subscribed";
    }
}

void ddsService3Imp::FieldGetter()
{
    GetLogger().LogInfo() << FIELDS_HEADER << "Trying to get values FieldA";
    LogStream logMsgFieldA{GetLogger().LogVerbose()};
    auto getFieldAFuture = this->service3_proxy->FieldA.Get();
    auto rFieldA = getFieldAFuture.GetResult();
    GetLogger().LogInfo() << FIELDS_HEADER << "Hasvalue";
    if (rFieldA.HasValue()) {
        auto value = rFieldA.Value();
        responseValue = value;
        logMsgFieldA << FIELDS_HEADER << " Current FiledA value is " << value;
    } else {
        // while field getters itself do not return errors, networking errors are still possible
        GetLogger().LogInfo() << "FieldA-err";
        auto err = rFieldA.Error();
        logMsgFieldA << "Code: " << err.Value();
        logMsgFieldA << "Message: " << err.Message();
    }
}

void ddsService3Imp::FieldSetter()
{
    GetLogger().LogInfo() << FIELDS_HEADER << "Trying to set FieldA";
    std::uint32_t valueToSet = 2000;
    auto setResultFuture = this->service3_proxy->FieldA.Set(valueToSet);
    auto r = setResultFuture.GetResult();
    if (r.HasValue()) {
        auto valueWhichWasSet = r.Value();
        GetLogger().LogInfo() << FIELDS_HEADER << "Set was successful: " << (valueWhichWasSet == valueToSet);
        responseValue = valueToSet;
    } else {
        // while field setters itself do not return errors, networking errors are still possible
        auto err = r.Error();
        GetLogger().LogWarn() << "Code: " << err.Value();
        GetLogger().LogWarn() << "Message: " << err.Message();
    }
}

void ddsService3Imp::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService3Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    for (auto it : handles) {
        GetLogger().LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == service3_proxy) {
            service3_proxy = std::make_shared<DDSService3Proxy>(handles[0]);
            GetLogger().LogInfo() << "Created proxy from handle with instance: "
                                  << service3_proxy->GetHandle().GetInstanceId().ToString();
            // Construct some handles (implementation-specific).
            auto first_handle = handles[0];
            auto aux_handle = first_handle;
            for (auto current_handle : handles) {
                // Call equality operator.
                GetLogger().LogInfo() << "Check handle::operator==: "
                                      << static_cast<uint8_t>(aux_handle == current_handle);
                // Call copy assignment operator.
                aux_handle = current_handle;
                // Call less-than operator.
                GetLogger().LogInfo() << "Check handle::operator<: " << static_cast<uint8_t>(aux_handle < first_handle);
            }
        }
    }
}

std::string ddsService3Imp::SendRequestResult3()
{
    GetLogger().LogInfo() << " RequestResult3";

    std::string response;

    response = std::to_string(responseValue);
    GetLogger().LogInfo() << "response value " << response;

    return response;
}
