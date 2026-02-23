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

#include "trigger_in.h"
#include "logger.h"
#include "ara/core/instance_specifier.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace
{

auto& GetLogger()
{
    static apd::smtool::Logger logger("TriggerIn", apd::smtool::LogLevel::kTrace);
    return logger;
}

}  // namespace

namespace apd
{
namespace smtool
{

TriggerIn::TriggerIn()
    : proxy_(nullptr)
{
    Init();
}

void TriggerIn::Init()
{
    GetLogger().LogDebug() << "Init TriggerIn Service\n";
    ara::core::InstanceSpecifier portSpecifier{"smtool/smtool/TriggerIn_RPort"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        GetLogger().LogError() << "InstanceIdentifier for TriggerIn port not resolved\n";
        return;
    }

    GetLogger().LogDebug() << "Service Discovery started.\n";

    TriggerInProxy::StartFindService(
        [this](
            ara::com::ServiceHandleContainer<TriggerInProxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            static_cast<void>(handler);
            serviceAvailabilityCallback(std::move(handles));
        },
        portSpecifier);
}

void TriggerIn::SendSetStateRequest(const ara::core::String& stateRequest)
{
    constexpr std::chrono::seconds kTimeout(10);
    constexpr std::chrono::milliseconds kSleepStep(100);
    std::chrono::milliseconds aggregatedSleepTime(0);

    while (aggregatedSleepTime < kTimeout) {
        if (proxy_ != nullptr) {
            break;
        }

        std::this_thread::sleep_for(kSleepStep);
        aggregatedSleepTime += kSleepStep;
    }

    if (proxy_ == nullptr) {
        GetLogger().LogError() << "Service Discovery Timeout.\n";
        return;
    }

    GetLogger().LogInfo() << "Send Request to StateManager: " << stateRequest << "\n";
    proxy_->Trigger.Set(stateRequest);
}

void TriggerIn::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<TriggerInProxy::HandleType> handles)
{
    GetLogger().LogDebug() << "Service TriggerIn found.\n";

    if (handles.size() == 0) {
        GetLogger().LogError() << "No handles found for the service TriggerIn.\n";
        return;
    }

    proxy_ = std::make_unique<TriggerInProxy>(handles[0]);
    GetLogger().LogDebug() << "Created TriggerIn proxy.\n";
}

}  // namespace smtool
}  // namespace apd
