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

#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include "ara/exec/state_client.h"
#include "ara/log/logger.h"
#include "ara/core/string.h"
#include <iostream>
#include <chrono>
#include <thread>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/program_options.hpp"

namespace
{
constexpr char const kInitFunctionGroupState[] = "Running";
}  // namespace

// Function fills provided variables with data loaded from command line.
// In case it failed to do so, it returns false, otherwise it returns true.
bool readProgramOptions(int ac, char** av, std::string& FGstate, std::string& Version)
{
    try {

        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()
            // general
            ("help", "Show this help and exit")
            // FunctionGroup State
            ("FGState",
                boost::program_options::value<std::string>(&FGstate)->default_value("Running"),
                "App Function Group State")(
                "Version", boost::program_options::value<std::string>(&Version)->default_value("1.0.0"), "App Version");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

        // in case help is requested, it is shown and no values are returned
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return false;
        }

        // update values and inform on success
        boost::program_options::notify(vm);
        return true;

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return false;  // return no values by default
}

int main(int ac, char** av)
{
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Failed to initialize. Terminating Updatable App." << std::endl;
        return 1;
    }
    ara::log::Logger& logger = ara::log::CreateLogger("UAPP", "Updatable App", ara::log::LogLevel::kInfo);

    // report application state
    ara::exec::ExecutionClient execClient;
    auto reportExecutionStateResult = execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);
    if (reportExecutionStateResult.HasValue()) {
        logger.LogInfo() << "Successfully reported ExecutionState::kRunning.";
    } else {
        logger.LogError() << "Failed to report ExecutionState::kRunning. Error code:"
                          << reportExecutionStateResult.Error();
        // return 1;
    }

    // Print FGState from prompt every 5s
    std::string fgstate, version;
    if (false == readProgramOptions(ac, av, fgstate, version)) {
        fgstate = kInitFunctionGroupState;
        version = "0.0.0";
        logger.LogInfo() << "Read from prompt failed";
    }

    while (1) {
        logger.LogInfo() << "Updatable application FGstate (from prompt):" << fgstate << ", Version:" << version;
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    logger.LogInfo() << "Terminating Updatable App.";
    if (!ara::core::Deinitialize().HasValue()) {
        std::cerr << "Failed to deinitialize Updatable App." << std::endl;
        return 1;
    }
    return 0;
}
