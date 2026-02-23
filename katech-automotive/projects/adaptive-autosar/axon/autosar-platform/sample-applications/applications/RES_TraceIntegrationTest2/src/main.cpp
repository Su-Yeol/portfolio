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

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <csignal>
#include <thread>
#include <chrono>
#include <string>

#include "ara/core/initialization.h"
#include "ara/exec/state_client.h"
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>

// //#include "ExecutionManagerTraceMessages/messages.h"

#include "tc01.h"
#include "tc02_remote.h"
#include "tc05_01_app1.h"
#include "tit_service.h"

#include <ara/core/instance_specifier.h>
#include <atomic>

extern const ara::core::InstanceSpecifier ServicePortSpecifier;
// Atomic flag for exit after SIGTERM caught
extern std::atomic_bool continueExecution;

const ara::core::InstanceSpecifier ServicePortSpecifier{
    "RES_TraceIntegrationTest2/RES_TraceIntegrationTest2/SensorData_ActuatorPort"};
// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};

namespace
{

void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        continueExecution = false;
    }
}

bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

}  // namespace

static ara::log::Logger& GetLogger() noexcept
{
    static ara::log::Logger& logger = ara::log::CreateLogger("TIT2", "WG-RES TIT2", ara::log::LogLevel::kVerbose);
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
    service->StartSendingData();

    while (continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }
    service->StopSendingData();
    service->StopOfferService();
    service.reset();
}

// Define an enumeration
enum class MyEnum
{
    Value1,
    Value2,
    Value5
};

// Function to convert string to MyEnum
MyEnum stringToEnum(const std::string& str)
{
    if (str == "tc-01") {
        return MyEnum::Value1;
    } else if (str == "tc-02-remote") {
        return MyEnum::Value2;
    } else if (str == "tc-05-01") {
        return MyEnum::Value5;
    } else {
        // Handle unrecognized strings, you might throw an exception or return a default value
        throw std::invalid_argument("Invalid string for MyEnum");
    }
}

// int main(int, char** /*argv*/)
int main(int argc, char* argv[])
{
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Failed to initialize. Terminating TraceIntegrationTestApp." << std::endl;
        return 1;
    }

    ara::log::Logger& logger = GetLogger();

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    logger.LogInfo() << "Tit Service Component starting.";

    logger.LogInfo() << "argument: " << argc;
    // Use the result
    for (int i = 1; i < argc; i++) {
        if (argc >= 2) {
            std::string inputString(argv[i]);
            MyEnum resultEnum = stringToEnum(inputString);
            switch (resultEnum) {
            case MyEnum::Value1:
                logger.LogInfo() << "Got Value1";
                testcase1();
                break;
            case MyEnum::Value2:
                logger.LogInfo() << "Got Value2";
                testcase2_remote();
                break;
            case MyEnum::Value5:
                logger.LogInfo() << "Got Value5";
                testcase5_01_app1();
                break;
            default:
                logger.LogInfo() << "Invalid test case name";
                break;
            }
        } else {
            // if no argument is given then use the defaul behavior and
            // execute a dummy thread
            std::thread td_dummy(thread_func_start_service);
            td_dummy.join();
        }

        logger.LogInfo() << "Tit Service Component terminating.";

        if (!ara::core::Deinitialize()) {
            std::cerr << "core deinitialization failed" << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
