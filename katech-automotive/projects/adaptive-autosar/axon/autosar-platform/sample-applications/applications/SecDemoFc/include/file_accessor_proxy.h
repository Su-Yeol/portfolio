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
/// @brief Proxy to receive IPC messages from AA

#ifndef DEMO_FC_FILE_ACCESSOR_PROXY_H_
#define DEMO_FC_FILE_ACCESSOR_PROXY_H_

#include "ara/core/string.h"
#include "ara/log/logger.h"
#include "ara/iam/ifc/ipc/ipc_server_adapter_impl.h"
#include "pep.h"

namespace apd
{
namespace demofc
{

/// @brief This class handles the AA request to access resources
///
/// This class handles the IPC communication to the AA, deserializes the IPC messages, contains the
/// PEP to enforce the request and plot the access control decision received by the PDP
class FileAccessorProxy
{
public:
    /// @brief Constructor to create object and establish IPC communication to the AA.
    FileAccessorProxy(std::unique_ptr<ara::iam::ifc::ipc::IPCServerAdapterImpl> serverAdapter);

    /// @brief Destructor to destroy object and close IPC socket.
    ~FileAccessorProxy();

    /// @brief Initialize the IPCServerAdapterInterface with the default parameters
    ///
    /// @param server  the IPCServerAdapterInterface which is initialized
    /// @returns true if successful, false otherwise
    static bool InitServerAdapter(ara::iam::ifc::ipc::IPCServerAdapterInterface& serverAdapter);

    /// @brief Handles access request of AA
    ///
    /// Identifies AA via Linux credential mechanism (SO_PASSCRED) and EM IFC, and enforces the access
    /// request using the PEP. The decision will be logged.
    void HandleAccessFileRequest();

private:
    /// @brief Deserialized IPC message to retrieve the transmitted InstanceSpecifier
    ///
    /// @param buf  the buffer to store the deserialized data
    /// @param buf_length  the size of the buffer to store the data
    /// @returns InstanceSpecifier is successful, ErrorCode if not
    ara::core::Result<ara::core::InstanceSpecifier> DeserializeIPCMessage(const char* buf,
        const std::size_t buf_length);

    /// @brief Plots the result of IAM access control.
    void PlotAccessEnforcement(bool accessGranted);

    /// @brief Provides access to IPC operations receiving requested actions by Adaptive Applications
    std::unique_ptr<ara::iam::ifc::ipc::IPCServerAdapterImpl> mIPCServerAdapterImpl;

    /// @brief PEP for IAM access control
    PEP pep;

    /// @brief File descriptor of socket
    int mClientSocketDescriptor = -1;

    /// @brief logger used in this class
    ara::log::Logger& mLogger_
        = ara::log::CreateLogger("FAPR", "File Accessor Proxy context", ara::log::LogLevel::kInfo);
};

}  // namespace demofc
}  // namespace apd

#endif  // DEMO_FC_FILE_ACCESSOR_PROXY_H_
