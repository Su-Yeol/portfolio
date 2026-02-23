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
 * @file
 * @brief implementation of the EMOApp
 * @note Project: Adaptive Platform Test Applications 
 * @ingroup EMOApp 
 */

#pragma once
#include <atomic>
#include <csignal>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <ara/core/string_view.h>

/** @brief EMO App Code
 */
extern ara::core::StringView appCtxID;
extern ara::core::StringView filename;
extern ara::core::StringView invoked;

/**
 * @brief EMOApp Class 
 * Using singleton design pattern 
 * because SignalHandler and EMOApp instance must be one-to-one correspondance.
 * 
 */
class ST_EMOApp
{
private:
    static ST_EMOApp* instance;
    std::atomic_bool continueExecution;
    std::atomic_int32_t recv_signal_num;
    ST_EMOApp() {};
    ~ST_EMOApp() {
        delete instance;
    };
public:
    ara::exec::ExecutionClient exec_client;
    ara::log::Logger& m_logger_est = ara::log::CreateLogger(appCtxID, "context for EMOApp", ara::log::LogLevel::kVerbose);
    ara::log::Logger& m_logger_est2 = ara::log::CreateLogger("FORK", "context for forked EMOApp", ara::log::LogLevel::kVerbose);

    struct sigaction act;

    ST_EMOApp(const ST_EMOApp&) = delete;
    ST_EMOApp& operator=(const ST_EMOApp&) = delete;
    ST_EMOApp(ST_EMOApp&&) = delete;
    ST_EMOApp& operator=(ST_EMOApp&&) = delete;

    static int CreateInstance(){
        if (!instance) {
            instance = new ST_EMOApp;
            if(!instance) {
                std::cerr << "Failed to allocate memory" << std::endl;
                return EXIT_FAILURE;
            }
            instance->continueExecution = false;
        }
        return EXIT_SUCCESS;
    }

    static ST_EMOApp &Instance(){
        return *instance;
    };

    void Initializing();
    void Terminating();
    void Running();

    static void SignalHandler(int32_t signalnum) {
        if (instance) {
            instance->continueExecution = false;
            instance->recv_signal_num = signalnum;
        }
        return;
    };

};


