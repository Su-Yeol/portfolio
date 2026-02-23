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

// needs to be included before logging.
// it contains the template specializations that are used by the logger.
//#include "trace_switch_spec.hpp"
#include <iostream>
#include <atomic>
#include <csignal>
#include <thread>
#include <chrono>
#include <string>
#include <condition_variable>
#include <future>
#include <stdexcept>

#include "ara/core/initialization.h"
#include "ara/exec/state_client.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include "tit_client.h"

#include "tc02_client.h"
#include "tc03.h"
#include "tc05_01_app2.h"
#include "tc07.h"
#include "tc08.h"
#include "tc10.h"

namespace
{
std::atomic_bool continueExecution{true};
std::mutex mtx;
std::condition_variable cv;

static ara::log::Logger& GetLogger() noexcept
{
    static ara::log::Logger& logger = ara::log::CreateLogger("TIT", "WG-RES TIT", ara::log::LogLevel::kVerbose);
    return logger;
}

void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        std::cerr << "SIGTERM received" << std::endl;
        GetLogger().LogInfo() << "SIGTERM received";
        continueExecution = false;
        cv.notify_all();
    }
}

bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to register SIGTERM handler" << std::endl;
        return false;
    }
    return true;
}
}  // namespace

static void thread_func_tit_client()
{
    std::unique_ptr<apd::wgres::tit::TitClient> titclient = std::make_unique<apd::wgres::tit::TitClient>();

    if (titclient->FindService()) {
        titclient->Run();

        std::unique_lock<std::mutex> lk(mtx);
        cv.wait(lk, [] { return !continueExecution.load(); });

        GetLogger().LogInfo() << "Stopping TitClient...";
        titclient->Stop();
        GetLogger().LogInfo() << "TitClient stopped.";
    } else {
        GetLogger().LogError() << "TitClient service not found.";
    }

    titclient.reset();
    GetLogger().LogInfo() << "TitClient thread exiting.";
}

// Define an enumeration
enum class MyEnum
{
    Value1,
    Value2,
    Value3,
    Value5,
    Value7,
    Value8,
    Value10
};

// Function to convert string to MyEnum
MyEnum stringToEnum(const std::string& str)
{
    if (str == "tc-02-client") {
        return MyEnum::Value2;
    } else if (str == "tc-03") {
        return MyEnum::Value3;
    } else if (str == "tc-05-01-app2") {
        return MyEnum::Value5;
    } else if (str == "tc-07") {
        return MyEnum::Value7;
    } else if (str == "tc-08") {
        return MyEnum::Value8;
    } else if (str == "tc-10") {
        return MyEnum::Value10;
    } else {
        // Handle unrecognized strings, you might throw an exception or return a default value
        throw std::invalid_argument("Invalid string for MyEnum");
    }
}

// int main(int, char** /*argv*/)
int main(int argc, char* argv[])
{
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Failed to initialize. Terminating TitClientComponent." << std::endl;
        return 1;
    }

    ara::log::Logger& logger = ara::log::CreateLogger("TIT", "WG-RES TIT", ara::log::LogLevel::kVerbose);

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    } else {
        logger.LogInfo() << "Signal handler registered successfully";
    }

    logger.LogInfo() << "TitClientComponent starting.";

    logger.LogInfo() << "argument: " << argc;
    // Use the result
    if (argc >= 2) {
        std::string inputString(argv[1]);
        MyEnum resultEnum = stringToEnum(inputString);
        switch (resultEnum) {
        case MyEnum::Value2:
            logger.LogInfo() << "Got Value2";
            testcase2_client();
            break;
        case MyEnum::Value3:
            logger.LogInfo() << "Got Value3";
            testcase3();
            break;
        case MyEnum::Value5:
            logger.LogInfo() << "Got Value5";
            testcase5_01_app2();
            break;
        case MyEnum::Value7:
            logger.LogInfo() << "Got Value7";
            testcase7();
            break;
        case MyEnum::Value8:
            logger.LogInfo() << "Got Value8";
            testcase8();
            break;
        case MyEnum::Value10:
            logger.LogInfo() << "Got Value10";
            testcase10();
            break;
        default:
            logger.LogInfo() << "Invalid test case name";
            break;
        }
    } else {
        // if no argument is given then use the defaul behavior and
        // execute the HpProcessor
        std::thread td_titclient(thread_func_tit_client);

        {
            std::unique_lock<std::mutex> lk(mtx);
            cv.wait(lk, [] { return !continueExecution.load(); });
        }

        logger.LogInfo() << "Waiting for TitClient thread to finish.";

        auto start = std::chrono::steady_clock::now();
        while (td_titclient.joinable()) {
            if (std::chrono::steady_clock::now() - start > std::chrono::seconds(5)) {
                logger.LogError() << "Timeout waiting for TitClient thread to finish.";
                td_titclient.detach();
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        logger.LogInfo() << "TitClientComponent Terminating";

        if (!ara::core::Deinitialize()) {
            std::cerr << "core deinitialization failed" << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
