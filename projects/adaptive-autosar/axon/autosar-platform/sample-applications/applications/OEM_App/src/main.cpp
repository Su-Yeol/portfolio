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
#include <cstdint>
#include <chrono>
#include <signal.h>
#include <ctime>
#include "ara/fw/firewall_state.h"

using namespace std;
using namespace ara::fw;

bool exit_requested{false};

void handle_sigint(int sig)
{
    ara::exec::ExecutionClient execClient;
    printf("Caught signal %d\n", sig);

    exit_requested = true;
    if (!(execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning).HasValue())) {
        std::cout << "OEM APP successfully reported ExecutionState::kTerminating.";
        //    return 1;
    }
}
const ara::core::String kState{"--state"};

ara::core::String ParseCommandLineArgs(int argc, char const* argv[])
{
    ara::core::String state{""};
    if (argc >= 2) {
        if (argv[1] == kState) {
            state = argv[2];
        }
    }
    return state;
}

FirewallState ConvertStringToEnum(const ara::core::String& state)
{
    if (state == "Driving") {
        return FirewallState::Driving;
    } else if (state == "Parking") {
        return FirewallState::Parking;
    }
    return FirewallState::Unknown;
}

int main(int argc, char const* argv[])
{
    if (!ara::core::Initialize().HasValue()) {
        std::cerr << "Failed to initialize. Terminating OEM App." << std::endl;
        return 1;
    }
    ara::log::Logger& logger = ara::log::CreateLogger("UAPP", "OEM App", ara::log::LogLevel::kInfo);

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    ara::core::String state = ParseCommandLineArgs(argc, argv);
    if (state.empty()) {
        // deinit
        return 0;
    }
    FirewallState stateEnum = ConvertStringToEnum(state);

    ara::core::InstanceSpecifier obj1("oem_app/rootsw_oem/oem_app/fwport");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ara::fw::FirewallStateSwitchInterface<ara::fw::FirewallState> fwObj(obj1);

    ara::core::Future<void> fut = fwObj.SwitchFirewallState(stateEnum);

    ara::core::future_status status = fut.wait_for(std::chrono::milliseconds(kMaxWaitTimeForResponse));
    if (ara::core::future_status::timeout == status) {
        logger.LogInfo() << "Firewall string Return Value is Timeout";
    } else {
        try {
            logger.LogInfo() << "Firewall status is returned ";
        } catch (const std::exception& e) {
            logger.LogInfo() << " caught exception " << e.what() << ", f.valid() == " << fut.valid() << "\n";
        }
    }
    logger.LogInfo() << "FirewallState Switched:" << state << static_cast<int>(stateEnum);
    return 0;
}
