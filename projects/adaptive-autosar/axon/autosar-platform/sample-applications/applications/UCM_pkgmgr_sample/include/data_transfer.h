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

#ifndef APD_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_
#define APD_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_

#include "ara/ucm/packagemanagement_proxy.h"
#include "ara/log/logger.h"
#include <fstream>
#include <iostream>
#include <memory>
#include "ara/core/vector.h"

namespace apd
{
namespace ucm
{
namespace pkgmgrsample
{

template <class PackageManagementService>
class DataTransfer
{
public:
    /// @brief Initializes this object with a reference to the PackageManagement
    /// service
    ///
    /// @param packageManagementService The instance of the PackageManagement
    /// service to be used
    explicit DataTransfer(const std::shared_ptr<PackageManagementService>& packageManagementService);
    virtual ~DataTransfer() = default;

    /// @brief Initializes a new session
    ///
    /// @param size The size of the data that should be transmitted
    ///
    /// @return Boolean indicating whether the session was successfully initiated
    virtual bool InitSession(uint32_t size);

    /// @brief Transfers data to UCM sending the provided collection of bytes
    /// to UCM, possibly by using multiple smaller data blocks according to the
    /// configured block size.
    ///
    /// @param data The data that should be sent to UCM
    ///
    /// @return Boolean indicating whether the data was successfully sent
    virtual bool Transfer(const ara::core::Vector<uint8_t>& data);

    /// @brief Exits the current session
    ///
    /// @return Boolean indicating whether this session was successfully exited
    virtual bool ExitSession();

    /// @brief Delete SoftwarePackage
    ///
    /// @param transferId Id of the package to be deleted
    ///
    /// @returns Enum indicating whether deleting package was successful
    virtual bool DeleteSwPackage(const ara::ucm::TransferIdType transferId);

    /// @brief Get SoftwarePackages
    ///
    /// @returns List of all Software Packages that have successfully transferred
    virtual ara::ucm::PackageManagement::GetSwPackagesOutput GetSwPackages();

    /// @brief Returns the session id of the current session.
    ///
    /// @return The id of the current session
    virtual ara::ucm::TransferIdType GetTransferId();

    /// @brief Test GetSwPackages
    ///
    ///	@param bytesSent No of transferred-bytes
    ///	@param blocksSent No of transferred-blocks
    ///
    /// @return Boolean indicating whether the transferred-bytes,transferred-blocks successfully received by ucm
    virtual bool TestGetSwPackages(std::uint64_t bytesSent, std::uint64_t blocksSent);

    ///@brief Returns the error of the current session.
    ///
    ///@return The error of the current session.
    ara::core::Optional<ara::core::ErrorCode> GetTransferError();

private:
    /// @brief The instance of the PackageManagement service used
    std::shared_ptr<PackageManagementService> packageManagementService_;

    /// @brief The configured block size for transmitting data
    uint32_t blockSize_;

    /// @brief The id of the current transfer
    ara::ucm::TransferIdType currentSession_;

    /// @brief Stores the error code
    ara::core::Optional<ara::core::ErrorCode> errorCode_;

    /// @brief For logging errors during transferring.
    ara::log::Logger& logger_;
};

}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd

#endif  // APD_UCM_PKGMGR_SAMPLE_DATA_TRANSFER_H_
