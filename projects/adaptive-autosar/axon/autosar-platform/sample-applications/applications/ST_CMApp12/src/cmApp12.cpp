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
 * \brief   Executor implementation of the CMApp12 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMApp12
 */

#include "cmApp12.hpp"
#include <chrono>
#include <exception>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <thread>

using namespace ara::log;

/* Public function definitions --------------------------------- */

CMApp12::CMApp12()
    : serviceActivity(std::make_shared<cmApp12Activity>())
    , diag(std::make_shared<CmApp12DiagService>())
{ }

CMApp12::~CMApp12()
{ }

void CMApp12::Initializing()
{

    // =====================================================================================
    //  Register a signal handler
    // =====================================================================================

    memset(&this->act, '\0', sizeof(act));
    act.sa_handler = &SignalHandler;

    if (sigaction(SIGINT, &act, NULL) < 0) {
        m_logger_ctx4.LogInfo() << "MAIN: "
                                << "Error: sigaction";
    } else if (sigaction(SIGTERM, &act, NULL) < 0) {
        m_logger_ctx4.LogInfo() << "MAIN: "
                                << "Error: sigaction";
    }

    // =====================================================================================
    //  configure e2e protection
    // =====================================================================================

    m_logger_ctx4.LogInfo() << "MAIN: "
                            << "CMApp12: configure e2e protection";

    bool success = ara::com::e2exf::StatusHandler::Configure("./etc/e2e_dataid_mapping.json",
        ara::com::e2exf::ConfigurationFormat::JSON,
        "./etc/e2e_statemachines.json",
        ara::com::e2exf::ConfigurationFormat::JSON);

    m_logger_ctx4.LogInfo() << "CMApp12: e2e configuration " << (success ? "succeeded" : "failed");

    // =====================================================================================
    //  Initialization
    // =====================================================================================

    if (true != this->diag->Init()) {
        m_logger_ctx4.LogInfo() << "MAIN: "
                                << "DIAG Init failed";
    } else {
        m_logger_ctx4.LogInfo() << "MAIN: "
                                << "DIAG Init started";
    }

    // this->serviceActivity->init();
}

void CMApp12::Running()
{
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);
    // wait for CMAPP10 and CMAPP11 to start
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // send Service6
    while (false == this->shutdown) {
        this->serviceActivity->act();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void CMApp12::Terminating()
{
    this->serviceActivity->deinit();
    this->diag->DeInit();
}

// =====================================================================================
//  SignalHandler for SIGRTMIN and SIGINT
// =====================================================================================
void SignalHandler(int32_t signalnum)
{
    std::string signal_name;
    auto& logger = CreateLogger("DFLT", "Default context", LogLevel::kVerbose);
    switch (signalnum) {
    case SIGTERM: {
        signal_name = "SIGTERM";
        logger.LogInfo() << "CMApp12: Received signal: " << signal_name << " --> Shutting down...";
        CMApp12::instance()->shutdown = true;
        break;
    }
    case SIGINT: {
        signal_name = "SIGINT";
        logger.LogInfo() << "CMApp12: Received signal: " << signal_name << " --> without handle";
        break;
    }
    default: {
        logger.LogInfo() << "CMApp12: Received signal: " << signal_name << " --> without handle";
        break;
    }
    }
}
