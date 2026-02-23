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

#include "cmService5_activity.hpp"

#include <cassert>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <iostream>
#include <thread>
#include <stdint.h>
#include "cmApp10_utils.hpp"

#include "ara/com/e2e_helper.h"
#include "ara/com/e2exf/types.h"
#include "ara/log/logger.h"
using namespace ara::log;  // 'using' ara::log should be OK

uint8_t cmService5Activity::cmApp10_Service5_subscribed;
bool cmService5Activity::cmApp10_Service5_received;

cmService5Activity::cmService5Activity()
    : service5Proxy(nullptr)
{
    m_logger.LogDebug() << "object address" << this;
}

void cmService5Activity::init()
{
    m_logger.LogDebug() << "enter init()";
    startFindService5();
}

void cmService5Activity::deinit()
{
    CMService5Proxy::StopFindService(this->handle_);
}

void cmService5Activity::startFindService5()
{
    // Polling version of FindService does not work for ANY instance.
    // Using callback-based one.

    auto result = CMService5Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<CMService5Proxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            cmService5Activity::serviceAvailabilityCallback(std::move(handles), handler);
        },
        (ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE5_INSTANCE_ID)));

    this->handle_ = result.ValueOrThrow();
}

void cmService5Activity::serviceAvailabilityCallback(
    ara::com::ServiceHandleContainer<CMService5Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    m_logger.LogInfo() << "serviceAvailabilityCallback";
    for (auto it : handles) {
        m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);
        if (nullptr == this->service5Proxy) {
            this->service5Proxy = std::make_shared<CMService5Proxy>(handles[0]);
            m_logger.LogInfo() << "Created proxy from handle with instance: "
                               << this->service5Proxy->GetHandle().GetInstanceId().ToString();
            this->SetUpProxyService5();
        }
    }
}

void cmService5Activity::SetUpProxyService5()
{
    m_logger.LogInfo() << "call SetUpProxyService5";
    if (this->service5Proxy->Event5.GetSubscriptionState() == ara::com::SubscriptionState::kNotSubscribed) {
        setService5SubscribedValue();
        m_logger.LogInfo() << "GetSubscriptionState(): " << cmApp10_Service5_subscribed;
        this->service5Proxy->Event5.Subscribe(1);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    setService5SubscribedValue();
    m_logger.LogInfo() << "GetSubscriptionState(): " << cmApp10_Service5_subscribed;
}

void cmService5Activity::act()
{
    if (nullptr != this->service5Proxy) {
        service5Proxy->Event5.GetNewSamples(
            [this](auto sample) {
                m_logger.LogInfo() << static_cast<int16_t>(*sample);
                auto event5 = static_cast<int16_t>(*sample);
                if (event5 == 1)
                    setService5ReceivedValue();
            },
            1);
    }
}

void cmService5Activity::unsubscribeCmService5()
{
    this->service5Proxy->Event5.Unsubscribe();
    setService5SubscribedValue();
    m_logger.LogInfo() << "IsSubscribed(): " << cmApp10_Service5_subscribed;
}

void cmService5Activity::setService5SubscribedValue()
{
    cmService5Activity::cmApp10_Service5_subscribed
        = static_cast<uint8_t>(this->service5Proxy->Event5.GetSubscriptionState());
}

uint8_t cmService5Activity::getService5SubscribedValue()
{
    return cmService5Activity::cmApp10_Service5_subscribed;
}

void cmService5Activity::setService5ReceivedValue()
{
    cmService5Activity::cmApp10_Service5_received = true;
}

bool cmService5Activity::getService5ReceivedValue()
{
    return cmService5Activity::cmApp10_Service5_received;
}
