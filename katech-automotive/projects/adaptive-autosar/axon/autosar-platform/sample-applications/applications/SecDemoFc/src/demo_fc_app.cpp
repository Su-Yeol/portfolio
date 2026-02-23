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

#include "demo_fc_app.h"
#include "file_accessor_proxy.h"
#include "ara/core/instance_specifier.h"
#include "ara/log/ifc/logging.h"

#include <chrono>
#include <csignal>
#include <functional>
#include <thread>
#include <system_error>
#include <memory>

namespace apd
{
namespace demofc
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

DemoFCApp::DemoFCApp()
    : mIpcServer(std::make_unique<ara::iam::ifc::ipc::IPCServerAdapterImpl>())
    , mExitRequested(false)
{
    SetupSignalHandling();
}

void DemoFCApp::Initialize()
{
    if (!FileAccessorProxy::InitServerAdapter(*mIpcServer)) {
        mLogger_.LogInfo() << "Unable to init server adapter.";
        throw std::system_error(ENOENT, std::generic_category(), "Unable to init server adapter.");
    }

    mLogger_.LogInfo() << "Starting Demo FC";
}

int DemoFCApp::Run()
{
    try {
        RunApplication();
        mLogger_.LogInfo() << "Stopping Demo FC";
        return EXIT_SUCCESS;
    } catch (const std::system_error& e) {
        mLogger_.LogInfo() << "Stopping Demo FC because of:" << e.what();
        return EXIT_FAILURE;
    }
}

void DemoFCApp::RunApplication()
{
    FileAccessorProxy fileAccessorProxy(std::move(mIpcServer));

    try {
        std::chrono::milliseconds aggregatedTime = std::chrono::milliseconds(0);
        do {
            if (aggregatedTime.count() == 0) {
                fileAccessorProxy.HandleAccessFileRequest();
            }

            std::this_thread::sleep_for(kSleepTime);
            aggregatedTime
                = std::chrono::milliseconds((aggregatedTime.count() + kSleepTime.count()) % kCycleTime.count());
        } while (!mExitRequested);
    } catch (...) {
        mLogger_.LogInfo() << "Caught Exception in RunApplication()!";
        throw;
    }
}

void DemoFCApp::SetupSignalHandling()
{
    gExitHandler = std::bind(&DemoFCApp::HandleSignal, this);

    if (std::signal(SIGTERM, SignalHandler) == SIG_ERR || std::signal(SIGINT, SignalHandler) == SIG_ERR
        || std::signal(SIGABRT, SignalHandler) == SIG_ERR || std::signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        throw std::system_error(errno, std::generic_category(), "std::signal() failed");
    }
}

void DemoFCApp::HandleSignal()
{
    mExitRequested = true;
}

}  // namespace demofc
}  // namespace apd
