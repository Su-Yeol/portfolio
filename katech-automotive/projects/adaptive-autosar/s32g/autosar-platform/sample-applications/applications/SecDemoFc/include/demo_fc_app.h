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
/// @brief Application functionality for Demo FC

#ifndef DEMO_FC_DEMO_FC_APP_H_
#define DEMO_FC_DEMO_FC_APP_H_

#include "ara/log/logger.h"
#include "ara/iam/ifc/ipc/ipc_server_adapter_impl.h"

#include <atomic>
#include <memory>

namespace apd
{
namespace demofc
{

/// @brief This class represents the Demo FC platform application
///
/// This class manages the lifecycle of Demo Functional Cluster application
/// and implements the application main loop.
class DemoFCApp
{
public:
    /// @brief Constructor to create object and to setup signal handling.
    DemoFCApp();

    /// @brief Destructor to manage shutdown of application.
    ///
    /// Managing the shutdown of the application.
    ~DemoFCApp() = default;

    /// @brief Initialization of the application.
    ///
    /// Initialization includes logging and notification of Execution State to EM.
    void Initialize();

    /// @brief This method runs the main application.
    ///
    /// This method blocks until the application is shutting down by a signal.
    ///
    /// @returns EXIT_SUCCESS in case of success, EXIT_FAILURE otherwise
    int Run();

private:
    /// @brief This method implements the main application loop logic.
    ///
    /// In the main application loop the FileAccessorProxy waits for an AA request to  access a ressource
    /// of the DemoFC. It is assumed that the method only returns if mExitRequested was set to true.
    void RunApplication();

    /// @brief Setup signal handling for the platform application.
    ///
    /// Signals will be handled by HandleSignal() method
    ///
    /// @throws std::system_error if error occurs
    void SetupSignalHandling();

    /// @brief Handles signal by setting mExitRequested to true
    void HandleSignal();

    /// @brief
    std::unique_ptr<ara::iam::ifc::ipc::IPCServerAdapterImpl> mIpcServer;

    /// @brief Flag for shutdown required by signal handling.
    std::atomic<bool> mExitRequested;

    /// @brief logger used in this class
    ara::log::Logger& mLogger_
        = ara::log::CreateLogger("DEFC", "Demo FC application context", ara::log::LogLevel::kInfo);
};

}  // namespace demofc
}  // namespace apd

#endif  // DEMO_FC_DEMO_FC_APP_H_
