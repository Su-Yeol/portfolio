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
 * \brief   Executor implementation of the CMApp10 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMApp10
 */

#include "cmApp10.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ara/core/initialization.h"
#include "ara/log/logger.h"
#include "ara/log/ifc/logging.h"

using namespace ara::log;

/* Public function definitions --------------------------------- */

CMApp10::CMApp10()
    : cmService5ActivityPtr(std::make_shared<cmService5Activity>())
    , diag(std::make_shared<CmApp10DiagService>())
{ }

CMApp10::~CMApp10()
{ }

void CMApp10::Initializing()
{
    // =====================================================================================
    //  Register a signal handler
    // =====================================================================================

    memset(&this->act, '\0', sizeof(act));
    act.sa_handler = &SignalHandler;
    logger.LogInfo() << "Initialization succeeded";
    if (sigaction(SIGINT, &act, NULL) < 0) {
        logger.LogInfo() << "CMApp10: Error: sigaction";
    } else if (sigaction(SIGTERM, &act, NULL) < 0) {
        logger.LogInfo() << "CMApp10: Error: sigaction";
    }

    // =====================================================================================
    //  configure e2e protection
    // =====================================================================================

    logger.LogInfo() << "CMApp10: configure e2e protection";

    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
        ara::com::e2exf::ConfigurationFormat::JSON,
        "./etc/e2e_statemachines.json",
        ara::com::e2exf::ConfigurationFormat::JSON);

    logger.LogInfo() << "CMApp10: e2e configuration " << (success ? "succeeded" : "failed");

    // =====================================================================================
    //  Initialization
    // =====================================================================================

    if (true != this->diag->Init()) {
        logger.LogInfo() << "CMApp10: DIAG Init failed";
    } else {
        logger.LogInfo() << "CMApp10: DIAG started";
    }

    this->cmService5ActivityPtr->init();
}

void CMApp10::Running()
{
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    while (false == this->shutdown) {
        this->cmService5ActivityPtr->act();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void CMApp10::Terminating()
{
    this->cmService5ActivityPtr->deinit();
    this->diag->DeInit();
}

// =====================================================================================
//  SignalHandler for SIGRTMIN and SIGINT
// =====================================================================================
void SignalHandler(int32_t signalnum)
{
    std::string signal_name;
    auto& logger = CreateLogger("DFLT", "Default logger", LogLevel::kVerbose);
    switch (signalnum) {
    case SIGTERM: {
        signal_name = "SIGTERM";
        logger.LogInfo() << "Fusion: Received signal: " << signal_name << " --> Shutting down...";
        CMApp10::instance()->shutdown = true;
        break;
    }
    case SIGINT: {
        signal_name = "SIGINT";
        logger.LogInfo() << "Fusion: Received signal: " << signal_name << " --> without handle";
        break;
    }
    default: {
        logger.LogInfo() << "Fusion: Received signal: " << signal_name << " --> without handle";
        break;
    }
    }
}
