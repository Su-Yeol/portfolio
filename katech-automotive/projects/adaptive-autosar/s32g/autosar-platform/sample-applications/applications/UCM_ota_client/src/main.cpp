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
#include <thread>
#include "boost/program_options.hpp"

#include "ara/core/string.h"
#include "ara/com/internal/runtime.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "ara/log/ifc/logging.h"
#include "ara/core/initialization.h"

#include "find_service.h"
#include "vpackage_management_app.h"
#include "hawkbit_config_file.h"
#include "hawkbit_client.h"
#include "update_client.h"

using apd::ucm::ota::hawkbitClient::HawkbitClient;
using apd::ucm::ota::updateClient::UpdateClient;
using ara::vucm::proxy::VehiclePackageManagementProxy;
using apd::ucm::ota::VehiclePackageManagementApp;
using namespace std::chrono;

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
    ara::core::String const& testDataLocation,
    ara::core::String const& vehiclepackage)
{
    logger.LogInfo() << "Transferring" << testDataLocation + "/" + vehiclepackage;
    return app.TransferVehiclePackage(testDataLocation + "/" + vehiclepackage);
}

static void TransferSoftwarePackages(VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    ara::log::Logger& logger,
    ara::core::String const& testDataLocation,
    std::atomic_bool& continueExecution)
{
    logger.LogInfo() << "Transfering software packages in dedicated thread";
    while (continueExecution) {
        const auto packages = app.GetRequestedPackages();
        if (packages.size() == 0) {
            logger.LogInfo() << "No package requests";
        }
        for (auto& swPackage : packages) {
            logger.LogInfo() << "Transferring" << testDataLocation + "/" + swPackage.name + "_" + swPackage.version;
            app.TransferSoftwarePackage(testDataLocation, swPackage.name, swPackage.version);
        }
    }
    logger.LogInfo() << "Closing transfer software packages thread ";
}

static bool readProgramOptions(int ac,
    char** av,
    ara::core::String& providerMode,
    ara::core::String& testDataLocation,
    ara::core::String& vehiclepackage,
    ara::core::String& configFile,
    ara::log::Logger& logger)
{
    try {
        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()
            // general
            ("help", "Show this help and exit")("provider-mode",
                boost::program_options::value<ara::core::String>(&providerMode)->default_value("localProvider"),
                "Provider mode of the ota client")
            // the processing of vehicle/software packages
            ("vehiclepackage",
                boost::program_options::value<ara::core::String>(&vehiclepackage)->default_value("ucm_vpm_simple.arvp"),
                "Name of vehicle package")("datapath",
                boost::program_options::value<ara::core::String>(&testDataLocation)
                    ->default_value("/usr/share/apdtest/ucm/vpm"),
                "Location of vehicle/software packages to be stored")  // location
            ("c",
                boost::program_options::value<ara::core::String>(&configFile),
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
    ara::core::String const& testDataLocation,
    ara::core::String const& vehiclepackage)
{
    bool result = true;
    std::unique_lock<std::mutex> lock(app.campaignMutex_);
    logger.LogInfo() << "Running local OTA Client provider";
    logger.LogInfo() << "testDataLocation: " << testDataLocation;
    logger.LogInfo() << "vehiclepackage: " << vehiclepackage;
    using namespace std::chrono;

    logger.LogInfo() << "Starting the campaign test case 1/2...";
    result = TransferVehiclePackage(app, logger, testDataLocation, vehiclepackage);

    if (!result) {
        logger.LogInfo() << "Vehicle package transfer to UCM Master failed";
    }

    // TODO: this seems very unstable and might not get executed every run?
    auto buff = app.GetStatusBuffer();
    if (std::find(buff.begin(), buff.end(), "Cancelling") != buff.end()) {

        logger.LogInfo() << "VPM has passed through kCancelling";
        // Check in Campaign History if the campaign was cancelled
        const auto checkPoint1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();

        logger.LogInfo() << "Waiting for VPM to go back to Idle state";
        const bool resultBool = app.campaignCond_.wait_for(
            lock, milliseconds(5000), [&] { return app.IsCorrectStatus(ara::vucm::TransferStateType::kIdle); });

        if (!resultBool) {
            logger.LogInfo() << "The campaign test case 1/2 failed because it's hanging in un unexpected state.";
            ReportIntegrationTestResult(resultBool);
            return;
        }

        logger.LogInfo() << "Asking VPM for campaign resolution";
        const auto historyResult = app.GetCampaignResolution(0, checkPoint1);

        if (historyResult.empty()) {
            logger.LogError() << "The campaign history is empty";
            ReportIntegrationTestResult(false);
            return;
        } else {
            logger.LogInfo() << "The campaign history has" << historyResult.size() << "entries.";
        }

        logger.LogInfo() << "The campaign finished with resolution code:" << unsigned(historyResult[0]);

        if (historyResult[0] == ara::vucm::UCMMasterResolutionType::kCampaignCancelled) {
            logger.LogInfo() << "The campaign was cancelled";
            logger.LogInfo() << "The campaign test case 1/2 suceeded";
        } else {
            logger.LogInfo() << "The campaign failed with resolution code:" << unsigned(historyResult[0]);
            logger.LogInfo() << "The campaign test case 1/2 failed";
            ReportIntegrationTestResult(false);
            return;
        }

        app.FlushStatusBuffer();
        app.FlushRequestedPackageList();
    }

    logger.LogInfo() << "Wait for VPM to come back to Idle state, before starting a new campaign";
    app.campaignCond_.wait(lock, [&] { return app.IsCorrectStatus(ara::vucm::TransferStateType::kIdle); });

    logger.LogInfo() << "Starting the campaign test case 2/2...";
    const auto checkPoint2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    result = TransferVehiclePackage(app, logger, testDataLocation, vehiclepackage);

    std::thread transferSwpThread(
        TransferSoftwarePackages, std::ref(app), std::ref(logger), testDataLocation, std::ref(continueExecution));

    // Wait until VPM has finished the campaign, then check results
    if (!app.campaignCond_.wait_for(
            lock, milliseconds(70000), [&] { return app.IsCorrectStatus(ara::vucm::TransferStateType::kIdle); })) {
        logger.LogInfo() << "The campaign test case 2/2 failed because it's hanging in un unexpected state.";
        continueExecution = false;
        transferSwpThread.join();
        ReportIntegrationTestResult(false);
        return;
    }

    const auto historyResult1 = app.GetCampaignResolution(
        checkPoint2, duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());

    if (historyResult1.empty()) {
        logger.LogError() << "The campaign history is empty";
        continueExecution = false;
        transferSwpThread.join();
        ReportIntegrationTestResult(false);
        return;
    } else {
        logger.LogInfo() << "The campaign history has " << historyResult1.size() << " entries.";
    }

    if (historyResult1[0] != ara::vucm::UCMMasterResolutionType::kSuccess) {
        result = false;
        logger.LogInfo() << "Campaign failed with code:" << unsigned(historyResult1[0]);
        logger.LogInfo() << "The campaign test case 2/2 failed";
        continueExecution = false;
        transferSwpThread.join();
        ReportIntegrationTestResult(result);
        return;
    }

    logger.LogInfo() << "The campaign test case 2/2 suceeded";
    app.FlushStatusBuffer();
    continueExecution = false;
    transferSwpThread.join();
    ReportIntegrationTestResult(result);
}

static void RunHawkbitProvider(ara::log::Logger& logger,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    ara::core::String const& configFile)
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
    ara::core::String const& configFile)
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
    ara::core::String const& providerMode,
    VehiclePackageManagementApp<VehiclePackageManagementProxy>& app,
    ara::core::String const& configFile,
    ara::core::String const& testDataLocation,
    ara::core::String const& vehiclepackage)
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

    if (!ara::core::Initialize()) {
        // No interaction with ARA is possible here since initialization failed
        return EXIT_FAILURE;
    }

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger = ara::log::CreateLogger("MAIN", "OTA main context", ara::log::LogLevel::kVerbose);
    logger.LogInfo() << "Starting OTA Sample Application";

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "Unable to register signal handler";
    }

    ara::core::String testDataLocation;
    ara::core::String vehiclepackage;
    ara::core::String configFile;
    ara::core::String providerMode;

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
                logger.LogInfo() << "providerMode: " << providerMode;
                logger.LogInfo() << "configFile: " << configFile;
                logger.LogInfo() << "testDataLocation: " << testDataLocation;
                logger.LogInfo() << "vehiclepackage: " << vehiclepackage;
            }
        }
    }

    logger.LogInfo() << "Terminating OTA Sample app.";

    if (!ara::core::Deinitialize()) {
        // No interaction with ARA is possible here since some ARA resources can be destroyed already
        return EXIT_FAILURE;
    }

    return 0;
}
