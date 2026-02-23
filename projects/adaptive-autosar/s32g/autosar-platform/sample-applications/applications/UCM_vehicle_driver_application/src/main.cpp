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

#include <csignal>
#include <cstdlib>
#include <thread>
#include <atomic>
#include <chrono>
#include "ara/core/string.h"
#include "boost/program_options.hpp"

#include "ara/log/logger.h"
#include "ara/exec/execution_client.h"
#include "ara/core/initialization.h"

#include "driver_interface_swc.h"
#include "find_service.h"

using ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy;

namespace
{

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};

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

static void RunItm(DriverInterfaceSwc& dif, ara::log::Logger& logger, bool& start)
{
    std::unique_lock<std::mutex> lck(dif.ara_software_update_mutex_, std::defer_lock);
    logger.LogInfo() << "Waiting for approval required...";
    lck.lock();
    dif.ara_software_update_cond_.wait(lck, [&] { return dif.GetApprovalRequired(); });
    dif.SetApprovalRequired(false);
    lck.unlock();
    logger.LogInfo() << "VPM is asking for approval.";

    logger.LogInfo() << "Driver approves.";
    dif.RespondApprovalRequest(true);

    if (!start) {
        logger.LogInfo() << "Waiting for kVehiclePackageTransferring or kSoftwarePackage_Transferring";
        lck.lock();
        dif.ara_software_update_cond_.wait(lck, [&] {
            return dif.IsCorrectCampaignState(ara::vucm::CampaignStateType::kVehiclePackageTransferring)
                || dif.IsCorrectCampaignState(ara::vucm::CampaignStateType::kSoftwarePackage_Transferring);
        });
        lck.unlock();
        logger.LogInfo() << "Driver is cancelling campaign";
        dif.GetAppProxy().CancelCampaign(false);
        logger.LogInfo() << "After cancel";
        start = true;
    }

    logger.LogInfo() << "Requesting Progress info...";
    dif.RequestProgressInformation();
    dif.RequestTransferInformation();
}

static void RunVehicleDriverApp(const bool itmMode,
    ara::log::Logger& logger,
    std::shared_ptr<ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy>& vdaAppProxy)
{
    DriverInterfaceSwc dif(vdaAppProxy);
    constexpr auto interval = std::chrono::milliseconds(2000);
    bool start = false;

    if (!itmMode) {
        dif.StartMyrequestLoopThread();
    }

    logger.LogInfo() << "Running Rest proxy Driver Application";
    logger.LogInfo() << "itmMode: " << (itmMode ? "enabled" : "disabled");

    while (continueExecution) {
        std::this_thread::sleep_for(interval);
        logger.LogInfo() << "VDI Application is alive";
        if (itmMode) {
            RunItm(dif, logger, start);
        }
    }

    if (!itmMode) {
        dif.requestLoopThread.join();
    }
}

static bool readProgramOptions(int ac, char** av, ara::core::String& vdiAppMode, ara::log::Logger& logger)
{
    try {
        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()
            // general
            ("help", "Show this help and exit")("itm-mode",
                boost::program_options::value<ara::core::String>(&vdiAppMode)->default_value("enabled"),
                "Mode of the vehicle driver application");

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
        logger.LogError() << "Error: " << e.what();
    } catch (...) {
        logger.LogError() << "Exception of unknown type!";
    }

    return false;  // return no values by default
}

int main(int ac, char** av)
{
    using namespace ara::log;

    if (!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    auto& logger{CreateLogger("MAIN", "Driver Interface Application main context", ara::log::LogLevel::kInfo)};

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    ara::core::String itmMode;
    if (!readProgramOptions(ac, av, itmMode, logger)) {
        return EXIT_FAILURE;
    } else {
        logger.LogInfo() << "ITM mode:" << itmMode;
    }

    auto foundService = apd::ucm::vda::FindVehicleDriverApplicationInterfaceService(
        ara::core::InstanceSpecifier("vehicle_driver_application/app_root/VehicleDriverApplicationInterfaceRPort"));

    if (foundService) {
        RunVehicleDriverApp(itmMode == "enabled" ? true : false, logger, *foundService);
    }
    logger.LogInfo() << "Application successfully exits";

    if (!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
