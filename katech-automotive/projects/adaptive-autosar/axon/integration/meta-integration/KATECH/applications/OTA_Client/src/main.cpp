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

/// @file
/// Main entry point of the Adaptive Autosar OTA.
#include <csignal>
#include "boost/program_options.hpp"

#include "ara/core/string.h"
#include "ara/com/internal/runtime.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logging.h"
#include "ara/log/logmanager.h"

#include "find_service.h"
#include "vpackage_management_app.h"
#include "hawkbit_config_file.h"
#include "hawkbit_client.h"
#include "update_client.h"

#include "ara/core/instance_specifier.h"
#include "ara/core/exception.h"
#include "logger.h"
#include "ucm_accessor.h"

using apd::ucm::ota::hawkbitClient::HawkbitClient;
using apd::ucm::ota::updateClient::UpdateClient;
using ara::ucm::proxy::VehiclePackageManagementProxy;
using apd::ucm::ota::VehiclePackageManagementApp;

static void ReportIntegrationTestResult(bool result)
{
    auto& logger = ara::log::CreateLogger("RES", "RESULT context", ara::log::LogLevel::kVerbose);

    if (result) {
        logger.LogInfo() << "Reporting Integration Test Result success";
        logger.LogInfo() << "OTAClient: "
                         << "INTEGRATION TEST <TC_VPKGMGR_OTAClient> {SUCCESS}";
    } else {
        logger.LogWarn() << "Reporting Integration Test Result failed";
        logger.LogInfo() << "OTAClient: "
                         << "INTEGRATION TEST <TC_VPKGMGR_OTAClient> {FAIL}";
    }
}

static bool TransferVehiclePackage(VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    ara::log::Logger& logger,
    std::string const& testDataLocation,
    std::string const& vehiclepackage)
{
    logger.LogInfo() << "Transferring" << testDataLocation + "/" + vehiclepackage;
    return app.TransferVehiclePackage(testDataLocation + "/" + vehiclepackage);
}

static bool TransferSoftwarePackages(VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    ara::log::Logger& logger,
    std::string const& testDataLocation)
{
    bool result = true;

    const auto packages = app.GetRequestedPackages();
    for (auto& swPackage : packages) {
        logger.LogInfo() << "Transferring" << testDataLocation + "/" + swPackage;
        result &= app.TransferSoftwarePackage(testDataLocation, swPackage);
    }
    return result;
}

static bool readProgramOptions(int ac,
    char** av,
    std::string& providerMode,
    std::string& testDataLocation,
    std::string& vehiclepackage,
    std::string& configFile,
    ara::log::Logger& logger)
{
    try {
        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()
            // general
            ("help", "Show this help and exit")("provider-mode",
                boost::program_options::value<std::string>(&providerMode)->default_value("localProvider"),
                "Provider mode of the ota client")
            // the processing of vehicle/software packages
            ("vehiclepackage",
                boost::program_options::value<std::string>(&vehiclepackage)->default_value("VP_TEST.zip"),
                "Name of vehicle package")("datapath",
                boost::program_options::value<std::string>(&testDataLocation)
                    ->default_value("/usr/share/apdtest/ucm/vpm"),
                "Location of vehicle/software packages to be stored")  // location
            ("c",
                boost::program_options::value<std::string>(&configFile),
                "Configuration file")  // provider config-file
            ;

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

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};

static void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        continueExecution = false;
    }
}

static bool RegisterSigTermHandler()
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

static void RunLocalProvider(ara::log::Logger& logger,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    std::string const& testDataLocation,
    std::string const& vehiclepackage)
{
    bool result = true;
    logger.LogInfo() << "Running local provider";

    result &= TransferVehiclePackage(app, logger, testDataLocation, vehiclepackage);
    result &= TransferSoftwarePackages(app, logger, testDataLocation);
    logger.LogInfo() << "All software packages have been transferred to UCM Master";

    ReportIntegrationTestResult(result);
}

static void RunHawkbitProvider(ara::log::Logger& logger,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    std::string const& configFile)
{
    logger.LogInfo() << "Running hawkbit provider";
    apd::ucm::ota::hawkbitClient::ConfigClass hawkbitConfig;
    hawkbitConfig.LoadConfigFile(configFile.c_str());
    if (!hawkbitConfig.isInitializedCorrectly) {
        return;
    }

    HawkbitClient hawkbitClient(app);
    if (!hawkbitClient.Init(hawkbitConfig)) {
        logger.LogError() << "Error while initialization of hawkbit provider";
    } else {
        hawkbitClient.StartServiceSync();
        logger.LogInfo() << "hawkbit provider running";
    }

    while (continueExecution) {
        logger.LogInfo() << "OTA Sample app. is alive";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    hawkbitClient.StopServiceSync();
}

static void RunPullProvider(ara::log::Logger& logger,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    std::string const& configFile)
{
    logger.LogInfo() << "Running pull provider";
    (void)configFile;
    UpdateClient updateClientBackend("KEY1", app);
    updateClientBackend.StartServiceSync();

    while (continueExecution) {
        logger.LogInfo() << "OTA Sample app. is alive";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
    updateClientBackend.StopServiceSync();
}

static void RunProvider(ara::log::Logger& logger,
    std::string const& providerMode,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    std::string const& configFile,
    std::string const& testDataLocation,
    std::string const& vehiclepackage)
{
    logger.LogInfo() << "Starting provider:" << providerMode;
    if (providerMode == "hawkbitProvider") {
        RunHawkbitProvider(logger, app, configFile);
    } else if (providerMode == "pullProvider") {
        RunPullProvider(logger, app, configFile);
    } else if (providerMode == "localProvider") {
        RunLocalProvider(logger, app, testDataLocation, vehiclepackage);
    } else {
        logger.LogInfo() << "Invalid run mode!";
    }
}

int main(int ac, char** av)
{
    std::cout << std::endl << std::endl;
    std::cout << "OTA SAMPLES DEMO STARTS" << std::endl;
    std::cout << "===========================" << std::endl;

    ara::log::InitLogging(
        "OTA", "OTA App", ara::log::LogLevel::kVerbose, ara::log::LogMode::kConsole | ara::log::LogMode::kRemote);

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger = ara::log::CreateLogger("MAIN", "OTA main context", ara::log::LogLevel::kVerbose);
    logger.LogInfo() << "Starting OTA Sample Application";

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    std::string testDataLocation;
    std::string vehiclepackage;
    std::string configFile;
    std::string providerMode;

    std::shared_ptr<VehiclePackageManagementProxy> service;
    std::shared_ptr<VehiclePackageManagementApp<VehiclePackageManagementProxy>> app;
    auto foundService = apd::ucm::ota::FindVehiclePackageManagementService();

    if (readProgramOptions(ac, av, providerMode, testDataLocation, vehiclepackage, configFile, logger)) {
        if (foundService.has_value()) {
            service = *foundService;
            app = std::make_unique<VehiclePackageManagementApp<VehiclePackageManagementProxy>>(service);

            if (app->UpdateStatus() && app->UpdateRequestedPackage()) {
                logger.LogInfo() << "Created Vehicle PackageManagementApp";
                RunProvider(logger, providerMode, *app, configFile, testDataLocation, vehiclepackage);
            }
        }
    }

    logger.LogInfo() << "Terminating OTA Sample app.";

    return 0;
}
