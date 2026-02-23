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

#include "iam_test_app.h"
#include "file_accessor.h"
#include "ara/core/instance_specifier.h"

#include <chrono>
#include <csignal>
#include <functional>
#include <thread>
#include <system_error>

namespace apd
{
namespace iamtest
{

namespace
{

constexpr std::chrono::milliseconds kCycleTime = std::chrono::milliseconds(5000);
constexpr std::chrono::milliseconds kSleepTime = std::chrono::milliseconds(5);

std::function<void()> gExitHandler;

extern "C" void SignalHandler(int /*sig*/)
{
    gExitHandler();
}

}  // namespace

IamTestApp::IamTestApp()
    : mExitRequested(false)
{
    SetupSignalHandling();
}

void IamTestApp::Initialize()
{
    mLogger_.LogInfo() << "Starting IAM Test Application";
}

int IamTestApp::Run()
{
    try {
        RunApplication();
        mLogger_.LogInfo() << "Stopping IAM Test App";
        return EXIT_SUCCESS;
    } catch (const ara::core::CoreException& e) {
        mLogger_.LogError() << "Wrong InstanceSpecifier ShortName format:" << e.what();
        return EXIT_FAILURE;
    } catch (const std::system_error& e) {
        mLogger_.LogInfo() << "Stopping IAM Test App due to:" << e.what();
        return EXIT_FAILURE;
    }
}

void IamTestApp::RunApplication()
{
    FileAccessor fileAccessor;

    // ######## PortPrototype InstanceSpecifier to test ##############
    const ara::core::InstanceSpecifier demoFile(
        "iamTestAppExe/iamTestAppRootSwc/demoFileAccessorSwc/accessDemoFileRPort");
    // ########

    try {
        std::chrono::milliseconds aggregatedTime = std::chrono::milliseconds(0);
        do {
            if (aggregatedTime.count() == 0) {
                fileAccessor.AccessFile(demoFile);
            }

            std::this_thread::sleep_for(kSleepTime);
            aggregatedTime
                = std::chrono::milliseconds((aggregatedTime.count() + kSleepTime.count()) % kCycleTime.count());
        } while (!mExitRequested);
    } catch (const std::exception& e) {
        mLogger_.LogError() << "std::exception type thrown:" << e.what();
        throw;
    } catch (...) {
        throw;
    }
}

void IamTestApp::SetupSignalHandling()
{
    gExitHandler = std::bind(&IamTestApp::HandleSignal, this);

    if (std::signal(SIGTERM, SignalHandler) == SIG_ERR || std::signal(SIGINT, SignalHandler) == SIG_ERR
        || std::signal(SIGABRT, SignalHandler) == SIG_ERR || std::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        throw std::system_error(errno, std::generic_category(), "std::signal() failed");
    }
}

void IamTestApp::HandleSignal()
{
    mExitRequested = true;
}

}  // namespace iamtest
}  // namespace apd
