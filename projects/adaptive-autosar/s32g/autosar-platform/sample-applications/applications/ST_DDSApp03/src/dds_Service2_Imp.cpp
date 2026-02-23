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

#include "dds_Service2_Imp.hpp"

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

ddsService2Imp::ddsService2Imp()
    : service2_proxy(nullptr)
{ }

void ddsService2Imp::StartFindService2()
{
    GetLogger().LogInfo() << "startFindService2() enter";

    auto res = DDSService2Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<DDSService2Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            ddsService2Imp::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier("DDS:30")));

    if (!res) {
        GetLogger().LogError() << "StartFindService faild with error:" << res.Error();
        throw std::runtime_error{"StartFindService faild"};
    }
    GetLogger().LogInfo() << "startFindService2() exit";
}

void ddsService2Imp::StopFindService2()
{
    GetLogger().LogInfo() << "StopFindService2() enter";
    DDSService2Proxy::StopFindService(this->handle_);
}

void ddsService2Imp::StopSendingService2()
{
    GetLogger().LogInfo() << "Unscubscribe EventC";
    service2_proxy->EventC.Unsubscribe();
}

void ddsService2Imp::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService2Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    service2_state = DDSAPP03ServiceState::NotAvailable;
    for (auto it : handles) {
        GetLogger().LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
        service2_state = DDSAPP03ServiceState::Available;
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == service2_proxy) {
            service2_proxy = std::make_shared<DDSService2Proxy>(handles[0]);
            GetLogger().LogInfo() << "Created proxy from handle with instance: "
                                  << service2_proxy->GetHandle().GetInstanceId().ToString();
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

void ddsService2Imp::SetEventCHandler()
{
    // Resister event receive callback
    service2_proxy->EventC.SetReceiveHandler([this]() { ddsService2Imp::EventCReceived(); });
    GetLogger().LogInfo() << "EventC SetReceiveHandler() complete";

    // subscribe to event
    // knewN -> kNewestN
    auto subscription_result = service2_proxy->EventC.Subscribe(3);
    if (subscription_result.HasValue()) {
        GetLogger().LogInfo() << "Callback registered.";
    } else {
        GetLogger().LogInfo() << "Subscription failed with error: " << subscription_result.Error();
        return;
    }
    GetLogger().LogInfo() << "EventC subscription complete";
}

void ddsService2Imp::EventCReceived()
{
    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(service2_proxy->EventC);
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);
    static unsigned char getData = 0;

    ara::log::LogStream logMsg{GetLogger().LogVerbose()};

    logMsg << "EventC E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    service2_proxy->EventC.GetNewSamples(
        [&logMsg](auto sample) {
            logMsg << "Polling: EventC - Service2:";
            // always returns NotAvailable if E2E is disabled

            if (!sample->active) {
                logMsg << "NOT";
            }
            logMsg << "active";

            auto const& l_dataVector = sample->objectVector;

            if (!l_dataVector.empty()) {
                logMsg << " data: ";
                ara::core::Span<std::uint8_t const> sp_object_vector(l_dataVector);
                logMsg << ara::core::as_bytes(sp_object_vector);
                getData = l_dataVector[0];
            }

            auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
            bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
            logMsg << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
        },
        3);
    if (getData == 5) {
        getCallback = 5;
    } else {
        getCallback = "failed";
    }
}

std::string ddsService2Imp::SendRequestResult1()
{
    GetLogger().LogInfo() << "SendRequestResult1";

    std::string response;

    if (service2_state == DDSAPP03ServiceState::Available) {
        response = "Available";
        GetLogger().LogInfo() << "response value " << response;
    } else if (service2_state == DDSAPP03ServiceState::NotAvailable) {
        response = "NotAvailable";
        GetLogger().LogInfo() << "response value " << response;
    } else {
        GetLogger().LogInfo() << "no ServiceState Parameter ";
        response = "NotDefine";
        GetLogger().LogInfo() << "response value" << response;
    }
    return response;
}

std::string ddsService2Imp::SendRequestResult2()
{
    GetLogger().LogInfo() << "SendRequestResult2";

    std::string response;

    if (this->service2_proxy->EventC.IsSubscribed()) {
        response = "kSubscribed";
        GetLogger().LogInfo() << "response value" << response;
    } else if (!this->service2_proxy->EventC.IsSubscribed()) {
        response = "kNotSubscribed";
        GetLogger().LogInfo() << " response value " << response;
    } else {
        response = "NotDefine";
        GetLogger().LogInfo() << "response value" << response;
    }
    return response;
}

std::string ddsService2Imp::SendRequestResult2_2()
{
    GetLogger().LogInfo() << " SendRequestResult2-2";

    std::string response;

    if (getCallback.empty()) {
        response = "No Callback";
    } else {
        response = "EventC data:" + getCallback;
    }

    return response;
}
