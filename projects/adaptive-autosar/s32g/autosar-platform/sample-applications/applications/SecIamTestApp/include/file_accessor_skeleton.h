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
/// @brief Skeleton to send access control request to DemoFC

#ifndef IAM_TEST_APP_FILE_ACCESSOR_SKELETON_H_
#define IAM_TEST_APP_FILE_ACCESSOR_SKELETON_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/optional.h"
#include "ara/log/logger.h"
#include "ara/iam/ifc/ipc/ipc_client_adapter_impl.h"

namespace apd
{
namespace iamtest
{

/// @brief Class to send a request via IPC to access resources managed by Demo FC
///
/// The application skeleton class is used to send a request to access a resource of the
/// DemoFC using an IPC library provided by IAM. This step was fulfilled to overcome the
/// demonstrator limitations regarding to missing process separation and not complete
/// standardization of other FCs. This class uses the Linux SO_PEERCRED approach and
/// therefore, no credentials will be send by the skeleton.
class FileAccessorSkeleton
{
public:
    /// @brief Constructor which connects to server socket
    FileAccessorSkeleton();

    /// @brief Destructor which disconnects from server socket
    ~FileAccessorSkeleton();

    /// @brief Send request over IPC to access AUTOSAR Demo File element ressource
    ///
    /// @param instanceSpecifier AUTOSAR model element to access
    bool Send(const ara::core::InstanceSpecifier& instanceSpecifier);

private:
    /// @brief Connect to IPC socket as client
    bool Connect();

    /// @brief Serialize MetaModelIdentifier hold by InstanceSpecifier for IPC trasmission
    ara::core::Optional<std::size_t> SerializeMetaModelIdentifier(const ara::core::InstanceSpecifier& instanceSpecifier,
        char* buf,
        std::size_t buf_length) const;

    /// @brief Unix Domain Socket adapter to receive requested actions by Adaptive Applications
    ara::iam::ifc::ipc::IPCClientAdapterImpl mIPCClientAdapterImpl;

    /// @brief logger used in this class
    ara::log::Logger& mLogger_
        = ara::log::CreateLogger("FASK", "File Accessor Skeleton context", ara::log::LogLevel::kInfo);
};

}  // namespace iamtest
}  // namespace apd

#endif  // IAM_TEST_APP_FILE_ACCESSOR_SKELETON_H_
