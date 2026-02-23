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

#include "tc02_remote.h"
#include "tit_service.h"
#include <ara/log/logger.h>

#include <chrono>
#include <thread>

#include <ara/core/instance_specifier.h>
#include <atomic>

extern const ara::core::InstanceSpecifier ServicePortSpecifier;
// Atomic flag for exit after SIGTERM caught
extern std::atomic_bool continueExecution;

static ara::log::Logger& GetLogger() noexcept
{
    static ara::log::Logger& logger = ara::log::CreateLogger("TC02", "WG-RES TC02", ara::log::LogLevel::kVerbose);
    return logger;
}

static void thread_func_start_service()
{
    GetLogger().LogInfo() << "Tit Service registering.";
    ara::com::InstanceIdentifierContainer instanceIDs = ara::com::runtime::ResolveInstanceIDs(ServicePortSpecifier);
    if (instanceIDs.empty()) {
        GetLogger().LogError()
            << "At least one InstanceIdentifiers not resolved from provided InstanceSpecifier -- Init failed";
        return;
    }
    GetLogger().LogInfo() << "Tit Service starting.";
    std::unique_ptr<apd::wgres::tit::TitServiceImp> service
        = std::make_unique<apd::wgres::tit::TitServiceImp>(instanceIDs[0]);

    GetLogger().LogInfo() << "TitService starts to offer service";
    service->OfferService();

    while (continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    service->StopOfferService();
    service.reset();
}

void testcase2_remote()
{
    // ara::log::Logger& logger = ara::log::CreateLogger("TC01", "WG-RES TC01", ara::log::LogLevel::kOff);
    ara::log::Logger& logger = GetLogger();
    logger.LogInfo() << "test case 02-remote";

    std::thread td_tc02(thread_func_start_service);
    td_tc02.join();
}
