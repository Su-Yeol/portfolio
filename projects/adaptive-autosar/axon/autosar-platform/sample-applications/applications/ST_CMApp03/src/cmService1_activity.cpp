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

#include "cmService1_activity.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <thread>
#include <stdint.h>
#include "cmApp03_utils.hpp"

#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"
#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

uint8_t cmService1Activity::cmApp03_Service1_subscribed;

cmService1Activity::cmService1Activity()
    : service1Proxy(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void cmService1Activity::init()
{
    m_logger.LogDebug() << "enter init()";
    startFindService1();
}

void cmService1Activity::deinit()
{
    CMService1Proxy::StopFindService(this->handle_);
}

void cmService1Activity::startFindService1()
{
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.

    auto result = CMService1Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<CMService1Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            cmService1Activity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE1_INSTANCE_ID)));

    this->handle_ = result.ValueOrThrow();
}

void cmService1Activity::serviceAvailabilityCallback(
    ara::com::ServiceHandleContainer<CMService1Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    m_logger.LogInfo() << "serviceAvailabilityCallback";
    for (auto it : handles) {
        m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == this->service1Proxy) {
            this->service1Proxy = std::make_shared<CMService1Proxy>(handles[0]);
            m_logger.LogInfo() << "Created proxy from handle with instance: "
                               << this->service1Proxy->GetHandle().GetInstanceId().ToString();
            this->SetUpProxyService1();
        }
    }
}

void cmService1Activity::SetUpProxyService1()
{
    m_logger.LogInfo() << "call SetUpProxyService1";
    if (this->service1Proxy->Event1.GetSubscriptionState() == ara::com::SubscriptionState::kNotSubscribed) {
        setService1SubscribedValue();
        m_logger.LogInfo() << "GetSubscriptionState(): " << cmApp03_Service1_subscribed;
        this->service1Proxy->Event1.Subscribe(1);
        this->service1Proxy->Event1.SetReceiveHandler([this]() { cmService1Activity::HandleEventService1(); });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    setService1SubscribedValue();
    m_logger.LogInfo() << "GetSubscriptionState(): " << cmApp03_Service1_subscribed;
}

void cmService1Activity::HandleEventService1()
{
    m_logger.LogInfo() << "received Event1";
    service1Proxy->Event1.GetNewSamples(
        [this](auto sample) { m_logger.LogInfo() << static_cast<int16_t>(*sample); }, 1);
}

void cmService1Activity::act()
{
    if (nullptr != this->service1Proxy) {
        m_logger.LogInfo() << "cmService1Activity";
    }
}

void cmService1Activity::setService1SubscribedValue()
{
    cmService1Activity::cmApp03_Service1_subscribed
        = static_cast<uint8_t>(this->service1Proxy->Event1.GetSubscriptionState());
}

uint8_t cmService1Activity::getService1SubscribedValue()
{
    return cmService1Activity::cmApp03_Service1_subscribed;
}
