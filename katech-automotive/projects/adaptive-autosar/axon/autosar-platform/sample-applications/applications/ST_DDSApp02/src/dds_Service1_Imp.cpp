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

#include "dds_Service1_Imp.hpp"

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

ddsService1Imp::ddsService1Imp()
    : service1_proxy(nullptr)
{ }

void ddsService1Imp::StartFindService1()
{
    GetLogger().LogInfo() << "startFindService1() enter";

    auto res = DDSService1Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<DDSService1Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            ddsService1Imp::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier("DDS:29")));
    if (!res) {
        GetLogger().LogError() << "StartFindService faild with error:" << res.Error();
        throw std::runtime_error{"StartFindService faild"};
    }
}

void ddsService1Imp::StopFindService1()
{
    GetLogger().LogInfo() << "StopFindService1() enter";
    service1_proxy->StopFindService(this->handle_);
}

void ddsService1Imp::StopSendingService1()
{
    GetLogger().LogInfo() << "Unsubscribe EventA and B";
    service1_proxy->EventA.UnsetReceiveHandler();
    service1_proxy->EventB.UnsetReceiveHandler();

    service1_proxy->EventA.Unsubscribe();
    service1_proxy->EventB.Unsubscribe();
}

void ddsService1Imp::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService1Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    service1_state = DDSAPP02ServiceState::NotAvailable;
    for (auto it : handles) {
        GetLogger().LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
        service1_state = DDSAPP02ServiceState::Available;
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == service1_proxy) {
            service1_proxy = std::make_shared<DDSService1Proxy>(handles[0]);
            GetLogger().LogInfo() << "Created proxy from handle with instance: "
                                  << service1_proxy->GetHandle().GetInstanceId().ToString();
            //  Construct some handles (implementation-specific).
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

void ddsService1Imp::SetEventAHandler()
{
    service1_proxy->EventA.SetReceiveHandler([this]() { ddsService1Imp::EventAReceived(); });

    GetLogger().LogInfo() << "EventA SetReceiveHandler() complete";

    // subscribe to event
    // kNewN -> kNewestN
    auto subscription_result = service1_proxy->EventA.Subscribe(3);
    if (subscription_result.HasValue()) {
        GetLogger().LogInfo() << "Callback registered.";
    } else {
        GetLogger().LogError() << "Subscription failed with error: " << subscription_result.Error();
        return;
    }
    GetLogger().LogInfo() << "EventA subscription complete";
}

void ddsService1Imp::SetEventBHandler()
{
    // Register event receive callback
    service1_proxy->EventB.SetReceiveHandler([this]() { ddsService1Imp::EventBReceived(); });
    GetLogger().LogInfo() << "EventB SetReceiveHandler() complete";

    // subscribe to event
    // kNewN -> kNewestN
    auto subscription_result = service1_proxy->EventB.Subscribe(3);
    if (subscription_result.HasValue()) {
        GetLogger().LogInfo() << "Callback registered.";
    } else {
        GetLogger().LogError() << "Subscription failed with error: " << subscription_result.Error();
        return;
    }
    GetLogger().LogInfo() << "EventB subscription complete";
}

void ddsService1Imp::EventAReceived()
{
    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(service1_proxy->EventA);
    static unsigned char getData = 0;
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    LogStream logMsg{GetLogger().LogVerbose()};
    logMsg << "EventA E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    auto callback = [&logMsg](auto sample) {
        logMsg << "Polling: EventA - Service1:";
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
        logMsg.Flush();
    };
    // execute callback for every samples in the context of GetNewSamples
    service1_proxy->EventA.GetNewSamples(callback);
    GetLogger().LogInfo() << "getdata :" << getData;

    if (getData == 10) {
        getCallbackA = "10";
    } else {
        getCallbackA = "failed";
    }

    GetLogger().LogInfo() << "EventA subscription complete";
}

void ddsService1Imp::EventBReceived()
{
    ara::log::LogStream logMsg{GetLogger().LogVerbose()};

    static unsigned char getData = 0;
    auto e2eState = ara::com::e2e::internal::GetE2EStateMachineState(service1_proxy->EventB);
    bool stateResult = (e2eState == ara::com::e2e::SMState::kNoData);

    logMsg << "EventB E2E state:" << (stateResult ? "ok (NoData)" : "not ok");
    logMsg.Flush();

    service1_proxy->EventB.GetNewSamples(
        [&logMsg](auto sample) {
            logMsg << "Callback: EventB - Service1:";

            if (!sample->active) {
                logMsg << "NOT";
            }
            logMsg << "active";

            auto const& l_dataVector = sample->objectVector;

            if (!l_dataVector.empty()) {
                logMsg << "Object:" << +l_dataVector[0];
                getData = l_dataVector[0];
            }

            auto e2eCheckStatus = ara::com::e2e::internal::GetProfileCheckStatus(sample);
            bool sampleCheckStatusResult = (e2eCheckStatus == ara::com::e2e::ProfileCheckStatus::kNotAvailable);
            logMsg << "E2E checkStatus:" << (sampleCheckStatusResult ? "ok (NotAvailable)" : "not ok");
        },
        3);
    GetLogger().LogInfo() << "getdata :" << getData;
    if (getData == 250) {
        getCallbackB = "250";
    } else {
        getCallbackB = "failed";
    }
}

std::string ddsService1Imp::SendRequestResult1()
{
    GetLogger().LogInfo() << "RequestResult1";

    std::string response;

    if (service1_state == DDSAPP02ServiceState::Available) {
        response = "Available";
        GetLogger().LogInfo() << "response value " << response;
    } else if (service1_state == DDSAPP02ServiceState::NotAvailable) {
        response = "NotAvailable";
        GetLogger().LogInfo() << "response value " << response;
    } else {
        GetLogger().LogInfo() << "no ServiceState Parameter";
        response = "NotDefine";
        GetLogger().LogInfo() << "response value " << response;
    }

    return response;
}

std::string ddsService1Imp::SendRequestResult2()
{
    GetLogger().LogInfo() << " RequestResult2";

    std::string response;

    if (this->service1_proxy->EventA.IsSubscribed() && this->service1_proxy->EventB.IsSubscribed()) {
        response = "kSubscribed";
        GetLogger().LogInfo() << "response value" << response;
    } else if (!(this->service1_proxy->EventA.IsSubscribed() && this->service1_proxy->EventB.IsSubscribed())) {
        response = "kNotSubscribed";
        GetLogger().LogInfo() << " response value " << response;
    } else {
        response = "NotDefine";
        GetLogger().LogInfo() << "response value" << response;
    }

    return response;
}

std::string ddsService1Imp::SendRequestResult2_2()
{
    GetLogger().LogInfo() << "SendRequestResult2-2";

    std::string response;

    if ((send_request_count % 2) == 0) {
        if (getCallbackA.empty()) {
            response = "No Callback";
        } else {
            response = "EventA data:" + getCallbackA;
        }
    } else {
        if (getCallbackB.empty()) {
            response = "No Callback";
        } else {
            response = "EventB data:" + getCallbackB;
        }
    }

    send_request_count++;

    return response;
}
