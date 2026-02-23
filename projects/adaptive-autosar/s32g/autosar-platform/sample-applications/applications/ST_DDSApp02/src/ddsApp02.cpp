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

/**
 * \brief   Executor implementation of the DDSApp2 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup DDSApp02
 */

#include "ddsApp02.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ara/core/initialization.h"
#include "ara/log/logger.h"

using namespace ara::log;

namespace
{

ara::log::Logger& GetLogger() noexcept
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS App Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

/* Public function definitions --------------------------------- */

DDSApp02::DDSApp02()
    : ddsApp02ActivityPtr(std::make_shared<ddsApp02Activity>())
{ }

DDSApp02::~DDSApp02()
{ }

void DDSApp02::Initializing()
{
    // =====================================================================================
    //  Initialization for app
    // =====================================================================================
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Initialization failed. Terminating DDSApp02." << std::endl;
    }

    // =====================================================================================
    //  Register a signal handler
    // =====================================================================================

    memset(&this->act, '\0', sizeof(act));
    act.sa_handler = &SignalHandler;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        GetLogger().LogInfo() << "DDSApp02: Error: sigaction";
    } else if (sigaction(SIGTERM, &act, NULL) < 0) {
        GetLogger().LogInfo() << "DDSApp02: Error: sigaction";
    }

    // =====================================================================================
    //  configure e2e protection
    // =====================================================================================

    GetLogger().LogInfo() << "DDSApp02: configure e2e protection";

    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
        ara::com::e2exf::ConfigurationFormat::JSON,
        "./etc/e2e_statemachines.json",
        ara::com::e2exf::ConfigurationFormat::JSON);

    GetLogger().LogInfo() << "DDSApp02: e2e configuration " << (success ? "succeeded" : "failed");

    // =====================================================================================
    //  Initialization
    // =====================================================================================

    this->ddsApp02ActivityPtr->init();
}

void DDSApp02::Running()
{
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    while (false == this->shutdown) {
        this->ddsApp02ActivityPtr->act();
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
}

void DDSApp02::Terminating()
{
    this->ddsApp02ActivityPtr->deinit();
}

// =====================================================================================
//  SignalHandler for SIGRTMIN and SIGINT
// =====================================================================================
void SignalHandler(int32_t signalnum)
{
    std::string signal_name;
    switch (signalnum) {
    case SIGTERM: {
        signal_name = "SIGTERM";
        GetLogger().LogInfo() << "DDSApp02: Received signal: " << signal_name << " --> Shutting down...";
        DDSApp02::instance()->shutdown = true;
        break;
    }
    case SIGINT: {
        signal_name = "SIGINT";
        GetLogger().LogInfo() << "DDSApp02: Received signal: " << signal_name << " --> without handle";
        break;
    }
    default: {
        GetLogger().LogInfo() << "DDSApp02: Received signal: " << signal_name << " --> without handle";
        break;
    }
    }
}
