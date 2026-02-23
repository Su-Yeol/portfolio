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

#ifndef APD_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_
#define APD_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_

#include "data_transfer.h"
#include "ara/core/string.h"
#include <fstream>
#include <iostream>
#include <memory>
#include "ara/core/vector.h"
#include <functional>

namespace apd
{
namespace ucm
{
namespace pkgmgrsample
{
using TransferReturn = std::tuple<bool, ara::core::Optional<ara::core::ErrorCode>>;
template <class PackageManagementService>
class PackageManagementApp
{
public:
    std::mutex globalMutex;
    std::condition_variable cond;
    /// @brief Initializes this object with the given service.
    ///
    /// @return True if initialization is successful, else false
    bool Init(std::shared_ptr<PackageManagementService> service);

    /// @brief Transfers a software package that is located at the given path
    ///
    /// @param packagePath The full file path to a software package archive
    ///
    /// @return True if the package has been successfully transferred, else false
    TransferReturn TransferSoftwarePackage(const ara::core::String& packagePath);

    /// @brief Transfers a software package that is located at the given path by
    /// using the given DataTransfer object
    ///
    /// @param packagePath The full file path to a software package archive
    /// @param dataTransfer An instance of the DataTransfer object that will be
    /// used
    ///
    /// @return True if the package has been successfully transferred, else false
    TransferReturn TransferSoftwarePackage(const ara::core::String& packagePath,
        DataTransfer<PackageManagementService>& dataTransfer);

    /// @brief Delete transferred software packages
    ///
    /// @return True if the packages has been successfully deleted, else false
    bool DeleteTransferredSoftwarePackages();

    /// @brief Installs the latest software package that has been transferred.
    /// If no software package has been transferred, then this method will do
    /// nothing.
    ///
    /// @return True If the installation was successful, else false.
    bool Install();
    /// @brief Calls Activate
    ///
    /// @return True if the activation was successful, else false.
    bool Activate();
    ///@brief Finish to finalize the installation process.
    ///
    /// @return True If the finalization was successful, else false.
    bool Finish();
    /// @brief Rollbacks the latest software package that has been install and set
    /// to be Activated.
    ///
    /// @return True If the rollback was successful, else false.
    bool Rollback();
    /// @brief Cancel the current processing
    ///
    /// @param processingTransferId Id of the package being processed
    ///
    /// @return CancelReturnType.
    bool Cancel(ara::ucm::TransferIdType processingTransferId);
    /// @brief Revert processed SW packages
    ///
    /// @returns True If the RevertProcessedSwPackages was successful, else false.
    bool RevertProcessedSwPackages();
    /// @brief Is the callback to handle status change notification.
    void OnStatusChange();
    /// @brief Unsubscribe the status field notification.
    ///
    void Unsubscribe();
    /// @brief It registers the status field notification.
    ///
    /// @return True if successfully registered, else false.
    bool UpdateStatus();
    /// @brief Check the expected status.
    ///
    /// @return True if the current status is equal to askedStatus, else false.
    bool IsCorrectStatus(ara::ucm::PackageManagementStatusType askedStatus);
    /// @brief Tries to recover state.
    ///
    /// @return True if state recovery is successful and current state is ready, else false.
    bool AttemptStateRecovery();
    /// @brief Checks if there are pending changes to the set of SoftwareClusters.
    ///
    /// @return True if the there are changes, else false.
    bool AreChangesPending();
    /// @brief Gets the transfer id of the package being processed.
    ///
    /// @return TransferIdType.
    ara::ucm::TransferIdType GetProcessingTransferId();

private:
    ///@brief Used to store current value of UCM server state
    ara::ucm::PackageManagementStatusType currentStatus_;

    /// @brief the instance of the PackageManagement service that is used by this
    /// app
    std::shared_ptr<PackageManagementService> service_;

    // @brief TransferId of the last successful transfer
    ara::ucm::TransferIdType lastTransferId_;

    /// @brief For logging errors related with ucm service.
    ara::log::Logger& logger_ = ara::log::CreateLogger("PMA", "PackageManagementApp", ara::log::LogLevel::kVerbose);
};

/// @brief Loads a binary file into memory
///
/// @param[in] filePath The full file path to the file that should be loaded
/// @param[out] binaryPackage The output buffer vector that will contain the
/// result
///
/// @return True if the file was found and loaded, else false
bool LoadBinaryFile(const ara::core::String& filePath, ara::core::Vector<uint8_t>& binaryPackage);
/// @brief Convert current status to string.
/// @return Status string
const ara::core::String CurrentStatusToString(ara::ucm::PackageManagementStatusType status);

}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd

#endif  // APD_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_
