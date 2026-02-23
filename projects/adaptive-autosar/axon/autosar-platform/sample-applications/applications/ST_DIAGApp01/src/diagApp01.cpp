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
 * \brief   Executor implementation of the DIAGApp01 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup DIAGApp01
 */

#include "diagApp01.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ara/core/initialization.h"

using namespace ara::log;

/* Public function definitions --------------------------------- */

DIAGApp01::DIAGApp01()
    : diag(std::make_shared<DIAGApp01DiagService>())
{ }

DIAGApp01::~DIAGApp01()
{ }

void DIAGApp01::Initializing()
{

    // =====================================================================================
    //  Register a signal handler
    // =====================================================================================

    memset(&this->act, '\0', sizeof(act));
    act.sa_handler = &SignalHandler;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        m_logger_dst1.LogInfo() << "MAIN: "
                                << "Error: sigaction";
    } else if (sigaction(SIGTERM, &act, NULL) < 0) {
        m_logger_dst1.LogInfo() << "MAIN: "
                                << "Error: sigaction";
    }

    // =====================================================================================
    //  Initialization
    // =====================================================================================

    if (true != this->diag->Init()) {
        m_logger_dst1.LogInfo() << "MAIN: "
                                << "DIAG Init failed";
    } else {
        m_logger_dst1.LogInfo() << "DIAGApp01 starts";
    }
}

void DIAGApp01::Running()
{
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    while (false == this->shutdown) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void DIAGApp01::Terminating()
{
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
        logger.LogInfo() << "DIAGApp01: Received signal: " << signal_name << " --> Shutting down...";
        DIAGApp01::instance()->shutdown = true;
        break;
    }
    case SIGINT: {
        signal_name = "SIGINT";
        logger.LogInfo() << "DIAGApp01: Received signal: " << signal_name << " --> without handle";
        break;
    }
    default: {
        logger.LogInfo() << "DIAGApp01: Received signal: " << signal_name << " --> without handle";
        break;
    }
    }
}
