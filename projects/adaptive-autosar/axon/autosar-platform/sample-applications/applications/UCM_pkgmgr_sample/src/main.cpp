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

#include "ara/core/string.h"
#include "ara/com/internal/runtime.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "ara/log/ifc/logging.h"
#include "find_service.h"
#include "package_management_app.h"
#include <chrono>
#include <memory>
#include <condition_variable>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/program_options.hpp"
#include "ara/core/initialization.h"

using ara::ucm::proxy::PackageManagementProxy;
using apd::ucm::pkgmgrsample::PackageManagementApp;

inline void trace(const ara::core::String& str)
{
    std::cout << "pkgApp: " << str << std::endl;
}

static void ReportIntegrationTestResult(bool result)
{
    auto& logger = ara::log::CreateLogger("RES", "RESULT context", ara::log::LogLevel::kVerbose);

    logger.LogInfo() << "Reporting Integration Test Result";
    if (result) {
        trace("INTEGRATION TEST <TC_PKGMGR_PackageManagement> {SUCCESS}");
    } else {
        trace("INTEGRATION TEST <TC_PKGMGR_PackageManagement> {FAIL}");
    }
}

static bool TransferAndActivatePackages(PackageManagementApp<PackageManagementProxy>& app,
    ara::log::Logger& logger,
    ara::core::String const& testDataLocation,
    ara::core::Vector<ara::core::String> const& packages)
{
    logger.LogInfo() << "performing" << __func__;
    bool result;
    ara::core::Optional<ara::core::ErrorCode> errorCode;

    for (const ara::core::String& package : packages) {
        logger.LogInfo() << "---------- Transferring" << testDataLocation + package << "----------";
        std::tie(result, errorCode) = app.TransferSoftwarePackage(testDataLocation + package);
        if (result) {
            logger.LogInfo() << "---------- Installing" << testDataLocation + package << "----------";
            result &= app.Install();
            if (!result) {
                logger.LogInfo() << "Install failed";
                break;
            }
        } else {
            break;
        }
    }

    logger.LogInfo() << "---------- Deleting transfers ----------";
    result &= app.DeleteTransferredSoftwarePackages();

    if (result) {
        logger.LogInfo() << "---------- Activating ----------";
        result &= app.Activate();

        if (!result) {
            logger.LogInfo() << "---------- Rolling back ----------";
            result &= app.Rollback();
        }

        if (app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRolledBack)
            || app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kActivated)) {
            logger.LogInfo() << "---------- Finishing ----------";
            result &= app.Finish();
        } else {
            result = false;
        }
    }

    return result;
}

static bool TransferAndActivateAndRollback(PackageManagementApp<PackageManagementProxy>& app,
    ara::log::Logger& logger,
    ara::core::String const& testDataLocation,
    ara::core::Vector<ara::core::String> const& packages)
{
    // OPEARTE ON THE FIRST PACKAGE ONLY TO AVOID DIFFICULTIES WITH AN UPDATE
    logger.LogInfo() << "performing" << __func__;
    bool result;
    ara::core::Optional<ara::core::ErrorCode> errorCode;

    const ara::core::String& package = packages[0];
    logger.LogInfo() << "---------- Transferring" << testDataLocation + package << "----------";
    std::tie(result, errorCode) = app.TransferSoftwarePackage(testDataLocation + package);
    if (result) {
        logger.LogInfo() << "---------- Installing" << testDataLocation + package << "----------";
        result &= app.Install();
        if (!result) {
            logger.LogInfo() << "Install failed";
        }
    }

    logger.LogInfo() << "---------- Deleting transfers ----------";
    result &= app.DeleteTransferredSoftwarePackages();

    if (result) {
        logger.LogInfo() << "---------- Activating ----------";
        result &= app.Activate();

        logger.LogInfo() << "---------- Rolling back ----------";
        result &= app.Rollback();

        if (app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRolledBack)
            || app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kActivated)) {
            logger.LogInfo() << "---------- Finishing ----------";
            result &= app.Finish();
        } else {
            result = false;
        }
    }

    return result;
}

// Function fills provided variables with data loaded from command line.
// In case it failed to do so, it returns false, otherwise it returns true.
static bool readProgramOptions(int ac,
    char** av,
    ara::core::String& dataLocation,
    ara::core::Vector<ara::core::String>& campaigns,
    ara::core::String& testDownGradeProtection,
    ara::core::String& ucmId)
{
    try {

        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()
            // general
            ("help", "Show this help and exit")
            // the processing of software packages
            ("campaign",
                boost::program_options::value<ara::core::Vector<ara::core::String>>(&campaigns)->composing(),
                "Quoted list of packages separated with semicolon like 'package1.zip;package2.zip;package3.zip'."
                "There can be multiple campaigns. In that case every campaign is followed by ACTIVATE call."
                "Do not confuse with VPM campaigns! Here the campaign is a set of Software Packages which are"
                "to be transferred and then installed (activated) in the provided order."
                "Please note that this behaviour is not \"reboot-friendly\".")  // packages
            ("path",
                boost::program_options::value<ara::core::String>(&dataLocation)
                    ->default_value("/usr/share/apdtest/ucm/"),
                "Location of software packages")  // location,
            ("test-downgrade",
                boost::program_options::value<ara::core::String>(&testDownGradeProtection)->default_value(""),
                "Option to enable the DownGradeProtection Testing")  // DownGradeProtection
            ("ucmId",
                boost::program_options::value<ara::core::String>(&ucmId)->default_value(""),
                "UCMIdentifier to connect to")  // UCMIdentifier
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
        std::cerr << "Error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return false;  // return no values by default
}

static bool TestGetSwClusterInfo(apd::ucm::pkgmgrsample::ServiceProxyPtr& service, ara::log::Logger& logger)

{
    using namespace std::string_literals;
    // calling GetSwClusterInfo
    constexpr int index = 0;
    const ara::core::Vector<ara::core::String> presentSwclShortNames{
        "SWCL_BASE", "ControllerAppSwcl", "ActorAppSwcl", "LinesensorAppSwcl"};

    ara::core::String expectedSwcls = std::accumulate(presentSwclShortNames.cbegin(),
        presentSwclShortNames.cend(),
        ara::core::String(),
        [](ara::core::String accumulated, ara::core::String item) { return std::move(accumulated) + item; });
    logger.LogInfo() << "SWCL check: expecting "s + expectedSwcls;

    auto swclInfo = service->GetSwClusterInfo().get().swInfo;
    for (auto info : swclInfo) {
        logger.LogInfo() << "SWCL check: SWCL "s + std::to_string(index) + ": " + info.name + ", " + info.version + ", "
                + [=](ara::ucm::SwClusterStateType state) {
                      switch (state) {
                      case ara::ucm::SwClusterStateType::kPresent:
                          return "kPresent"s;
                      case ara::ucm::SwClusterStateType::kAdded:
                          return "kAdded"s;
                      case ara::ucm::SwClusterStateType::kUpdating:
                          return "kUpdating"s;
                      case ara::ucm::SwClusterStateType::kRemoved:
                          return "kRemoved"s;
                      default:
                          return "Unknown"s;
                      }
                  }(info.state);
    }
    for (auto swclName : presentSwclShortNames) {
        if (std::find_if(swclInfo.begin(),
                swclInfo.end(),
                [&](ara::ucm::SwClusterInfoType info) { return (info.name == swclName); })
            == swclInfo.end()) {
            logger.LogError() << "SWCL check: error:" << swclName << "is not available";
            return false;
        }
    }
    return true;
}

static void InitializeFSM(PackageManagementApp<PackageManagementProxy>& app, ara::log::Logger& logger)
{  // Initialization of FSM: go to initial state or recover previous one
    {  // a block which sets the mutex lifetime
        std::unique_lock<std::mutex> lck(app.globalMutex);
        if ((!app.cond.wait_for(lck,
                std::chrono::milliseconds(5000),
                [&] {
                    return app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kIdle)
                        || app.IsCorrectStatus(ara::ucm::PackageManagementStatusType::kReady);
                }))
            || app.AreChangesPending()) {
            if (app.AttemptStateRecovery()) {
                logger.LogInfo() << "State Recovery Successful ";
            } else {
                logger.LogInfo() << "State Recovery Failed ";
            }
        } else {
            logger.LogInfo() << "No state recovery detected, starting from the default state";
        }
    }
}

static void ExecuteCampaigns(PackageManagementApp<PackageManagementProxy>& app,
    ara::log::Logger& logger,
    const ara::core::String& dataLocation,
    ara::core::Vector<ara::core::String>& campaigns,
    bool& result)

{
    for (size_t i = 0; i < campaigns.size(); i++) {
        ara::core::Vector<ara::core::String> packages;
        // campaign is a list of packages
        boost::split(packages, campaigns[i], boost::is_any_of(";"), boost::algorithm::token_compress_on);
        logger.LogInfo() << "Installing the set of Software Clusters. The set is" << i + 1 << "out of"
                         << campaigns.size();
        result &= TransferAndActivateAndRollback(app, logger, dataLocation, packages);
        result &= TransferAndActivatePackages(app, logger, dataLocation, packages);
        if (result) {
            logger.LogInfo() << "SUCCESSFULLY installed the set of Software Clusters" << i + 1 << "out of"
                             << campaigns.size();
        } else {
            logger.LogInfo() << "FAILED to install the set of Software Clusters" << i + 1 << "out of"
                             << campaigns.size();
        }

        // campaigns may depend on previous campaigns therefore in case of error program exits immediately
        if (!result) {
            break;
        }
    }
}
static bool TestDownGradeProtection(PackageManagementApp<PackageManagementProxy>& app,
    ara::log::Logger& logger,
    const ara::core::String& testDataLocation,
    const ara::core::String& package)

{
    logger.LogInfo() << "Transferring" << testDataLocation + package;
    bool transferStatus;
    ara::core::Optional<ara::core::ErrorCode> errorCode;
    std::tie(transferStatus, errorCode) = app.TransferSoftwarePackage(testDataLocation + package);

    if ((!transferStatus) && (errorCode.has_value() && (*errorCode) == ara::ucm::UcmErrc::kOldVersion)) {
        logger.LogInfo() << "Test of Downgrade Protection is Success";
        return true;
    }
    logger.LogWarn() << "Test of Downgrade Protection is Failed";
    return false;
}
int main(int ac, char** av)
{
    if (!ara::core::Initialize()) {
        std::cerr << "core initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    // main() connects to the UCM service and sets the lifetime of the proxy instance
    PackageManagementApp<PackageManagementProxy> app;
    apd::ucm::pkgmgrsample::ServiceProxyPtr service = nullptr;

    std::cout << std::endl << std::endl;
    std::cout << "UCM SAMPLES DEMO STARTS" << std::endl;
    std::cout << "===========================" << std::endl;

    // report application state
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger = ara::log::CreateLogger("MAIN", "MAIN context", ara::log::LogLevel::kVerbose);
    logger.LogInfo() << "Starting PackageManagement Sample Application";

    ara::core::String dataLocation;
    ara::core::Vector<ara::core::String> campaigns;
    ara::core::String testDownGradeProtection{};
    ara::core::String ucmId{};

    if (readProgramOptions(ac, av, dataLocation, campaigns, testDownGradeProtection, ucmId)) {
        // log all input:
        logger.LogInfo() << "Location of Software Packages:" << dataLocation;
        if (campaigns.size() == 1) {
            logger.LogInfo() << "There is 1 set of Software Packages to be installed";
        } else {
            logger.LogInfo() << campaigns.size() << "sets of Software Packages provided:";
            for (std::size_t i = 0; i < campaigns.size(); i++) {
                logger.LogInfo() << "Software Packages of the set" << i + 1 << "are" << campaigns[i];
            }
        }
        logger.LogInfo() << "Test downgrade protection:" << testDownGradeProtection;
        logger.LogInfo() << "UCMIdentifier: " << ucmId;

        auto foundService = apd::ucm::pkgmgrsample::FindPackageManagementService(
            ara::core::InstanceSpecifier("pkgmgr_sample/pkgmgr_app_root/PackageManagementRPort"), ucmId);

        // the flag is used for final report, it is updated further
        bool result = foundService.has_value();

        if (result) {
            service = *foundService;

            if (app.Init(service) && app.UpdateStatus()) {
                logger.LogInfo() << "[Connect] connection to the PackageManagement service is established";

                InitializeFSM(app, logger);
                // Software cluster should not be listed before processing
                result &= TestGetSwClusterInfo(
                    service, logger);  // original swcl: SWCL_BASE, Actor, Controller, Linesensor, UpdatableApp
                if (result) {
                    ExecuteCampaigns(app, logger, dataLocation, campaigns, result);
                    if (result) {
                        logger.LogInfo() << "Final check of the SWCL count";
                        result &= TestGetSwClusterInfo(service, logger);
                    }
                }
            }
        } else {
            logger.LogInfo() << "[Connect] Unable to connect to the PackageManagement service, exiting...";
        }
        if (result && !(testDownGradeProtection.empty())) {
            result &= TestDownGradeProtection(app, logger, dataLocation, testDownGradeProtection);
        }
        // not really necessary though
        app.Unsubscribe();
        ReportIntegrationTestResult(result);
    } else {
        logger.LogInfo() << "Failed to read the command line parameters, exiting...";
    }

    logger.LogInfo() << "Terminating PackageManagement Sample app.";

    if (!ara::core::Deinitialize()) {
        std::cerr << "core deinitialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    return 0;
}
