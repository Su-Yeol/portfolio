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

#include "cmService6_activity.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <stdint.h>
#include "cmApp14_utils.hpp"

#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"
#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

uint8_t cmService6Activity::cmApp14_Service6_subscribed;
bool cmService6Activity::cmApp14_Service6_received;

cmService6Activity::cmService6Activity()
    : service6Proxy(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void cmService6Activity::init()
{
    m_logger.LogDebug() << "enter init()";
    startFindService6();
}

void cmService6Activity::deinit()
{
    CMService6Proxy::StopFindService(this->handle_);
}

void cmService6Activity::startFindService6()
{
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.

    auto result = CMService6Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<CMService6Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            cmService6Activity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE6_INSTANCE_ID)));

    this->handle_ = result.ValueOrThrow();
}

void cmService6Activity::serviceAvailabilityCallback(
    ara::com::ServiceHandleContainer<CMService6Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    m_logger.LogInfo() << "serviceAvailabilityCallback";
    for (auto it : handles) {
        m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == this->service6Proxy) {
            this->service6Proxy = std::make_shared<CMService6Proxy>(handles[0]);
            m_logger.LogInfo() << "Created proxy from handle with instance: "
                               << this->service6Proxy->GetHandle().GetInstanceId().ToString();
            this->SetUpProxyService6();
        }
    }
}

void cmService6Activity::SetUpProxyService6()
{
    m_logger.LogInfo() << "call SetUpProxyService6";
    if (this->service6Proxy->Event6.GetSubscriptionState() == ara::com::SubscriptionState::kNotSubscribed) {
        setService6SubscribedValue();
        m_logger.LogInfo() << "IsSubscribed(): " << cmApp14_Service6_subscribed;
        this->service6Proxy->Event6.Subscribe(1);
        this->service6Proxy->Event6.SetReceiveHandler([this]() { cmService6Activity::HandleEventService6(); });
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    setService6SubscribedValue();
    m_logger.LogInfo() << "GetSubscriptionState(): " << cmApp14_Service6_subscribed;
}

void cmService6Activity::HandleEventService6()
{
    m_logger.LogInfo() << "received Event6";
    setService6ReceivedValue();
    service6Proxy->Event6.GetNewSamples(
        [this](auto sample) { m_logger.LogInfo() << static_cast<int16_t>(*sample); }, 1);
}

void cmService6Activity::act()
{
    if (nullptr != this->service6Proxy) {
        m_logger.LogInfo() << "cmService6Activity";
    }
}

void cmService6Activity::unsubscribeCmService6()
{
    this->service6Proxy->Event6.Unsubscribe();
    setService6SubscribedValue();
    m_logger.LogInfo() << "IsSubscribed(): " << cmApp14_Service6_subscribed;
}

void cmService6Activity::setService6SubscribedValue()
{
    cmService6Activity::cmApp14_Service6_subscribed
        = static_cast<uint8_t>(this->service6Proxy->Event6.GetSubscriptionState());
}

uint8_t cmService6Activity::getService6SubscribedValue()
{
    return cmService6Activity::cmApp14_Service6_subscribed;
}

void cmService6Activity::setService6ReceivedValue()
{
    cmService6Activity::cmApp14_Service6_received = true;
}

bool cmService6Activity::getService6ReceivedValue()
{
    return cmService6Activity::cmApp14_Service6_received;
}
