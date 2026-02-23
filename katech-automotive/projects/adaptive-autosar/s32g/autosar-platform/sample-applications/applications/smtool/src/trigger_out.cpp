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

#include "trigger_out.h"
#include "logger.h"
#include "ara/core/instance_specifier.h"
#include <chrono>
#include <iostream>
#include <thread>

namespace
{

auto& GetLogger()
{
    static apd::smtool::Logger logger("TriggerOut", apd::smtool::LogLevel::kTrace);
    return logger;
}

}  // namespace

namespace apd
{
namespace smtool
{

TriggerOut::TriggerOut()
    : proxy_(nullptr)
{
    Init();
}

void TriggerOut::Init()
{
    GetLogger().LogDebug() << "Init TriggerOut Service\n";
    ara::core::InstanceSpecifier portSpecifier{"smtool/smtool/TriggerOut_RPort"};

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        GetLogger().LogError() << "InstanceIdentifier for TriggerOut port not resolved\n";
        return;
    }

    GetLogger().LogDebug() << "Service Discovery started.\n";

    TriggerOutProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<TriggerOutProxy::HandleType> handles,
            ara::com::FindServiceHandle handler) {
            static_cast<void>(handler);
            serviceAvailabilityCallback(std::move(handles));
        },
        portSpecifier);
}

ara::core::String TriggerOut::GetSetStateRequestResponse()
{
    auto future = proxy_->Notifier.Get();
    future.wait();
    auto result = future.GetResult();
    if (result) {
        return result.Value();
    }

    return "";
}

bool TriggerOut::WaitForStateChange(ara::core::StringView stateChange, const std::chrono::seconds timeout)
{
    constexpr std::chrono::milliseconds kSleepStep(100);
    std::chrono::milliseconds aggregatedSleepTime(0);

    while (aggregatedSleepTime < timeout) {
        if (stateChange == GetSetStateRequestResponse()) {
            return true;
        }

        std::this_thread::sleep_for(kSleepStep);
        aggregatedSleepTime += kSleepStep;
    }

    return false;
}

void TriggerOut::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<TriggerOutProxy::HandleType> handles)
{
    GetLogger().LogDebug() << "Service TriggerOut found.\n";

    if (handles.size() == 0) {
        GetLogger().LogError() << "No handles found for the service TriggerOut.\n";
        return;
    }

    proxy_ = std::make_unique<TriggerOutProxy>(handles[0]);
    GetLogger().LogDebug() << "Created TriggerOut proxy.\n";
}

}  // namespace smtool
}  // namespace apd
