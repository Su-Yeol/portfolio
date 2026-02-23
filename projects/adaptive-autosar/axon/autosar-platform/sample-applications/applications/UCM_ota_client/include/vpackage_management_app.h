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
#include <cstdint>

namespace apd
{
namespace ucm
{
namespace ota
{

template <class VehiclePackageManagementService>
class VehiclePackageManagementApp
{
public:
    std::mutex transferMutex_;
    std::condition_variable campaignCond_;
    std::mutex requestMutex_;
    std::condition_variable requestCond_;
    std::mutex campaignMutex_;
    /// @brief Initializes this object with the given service.
    ///
    /// @return True if initialization is successful, else false
    explicit VehiclePackageManagementApp(std::shared_ptr<VehiclePackageManagementService> service)
        : currentStatus_(ara::vucm::TransferStateType::kIdle)

        , service_(service)
        , lastTransferId_(0)
        , logger_(ara::log::CreateLogger("VPMA", "VehiclePackageManagementApp", ara::log::LogLevel::kVerbose))
    {
        isCampaignCancelled_ = false;
    }

    ~VehiclePackageManagementApp() = default;

    /// @brief Retrieve the campaign resolution code from the service
    ///
    /// @param timestampGE The starting timestamp
    ///
    /// @param timestampLT The latest timestamp
    ///
    /// @return The campaign resolution code
    ara::vucm::UCMMasterResolutionVectorType GetCampaignResolution(const std::uint64_t timestampGE,
        const std::uint64_t timestampLT);

    /// @brief Get Vehicle Package from OTA Server (2-2 Push pattern)
    ///
    /// @param packagePath The full file path to a software package archive
    ///
    /// @return True if the package has been successfully transferred, else false
    bool GetRequiredSwInfo(ara::ucm::SwClusterInfoVectorType& SwInfo);

    /// @brief Transfers a software package that is located at the given path
    ///
    /// @param packagePath The full file path to a vehicle software package archive
    ///
    /// @return True if the package has been successfully transferred, else false
    bool TransferVehiclePackage(const ara::core::String& packagePath);

    /// @brief Transfers a software package that is located at the given path
    ///
    /// @param testDataLocation The full directory path storing software package archive
    ///
    /// @param swPackage The file name of software package archive
    ///
    /// @return True if the package has been successfully transferred, else false
    bool TransferSoftwarePackage(const ara::core::String& testDataLocation,
        const ara::core::String& swPackage,
        const ara::ucm::StrongRevisionLabelString& swPackageVersion);

    /// @brief Transfers package that is located at the given path by
    /// using the given DataTransfer object
    ///
    /// @param packagePath The full file path to a software package archive
    /// @param dataTransfer An instance of the DataTransfer object that will be
    /// used
    ///
    /// @return True if the package has been successfully transferred, else false
    bool TransferVehiclePackage(const ara::core::String& packagePath,
        DataTransfer<VehiclePackageManagementService>& dataTransfer);

    /// @brief Transfers package that is located at the given path by
    /// using the given DataTransfer object
    ///
    /// @param testDataLocation The full directory path storing software package archive
    /// @param swPackage The file name of software package archive
    /// @param dataTransfer An instance of the DataTransfer object that will be
    /// used
    ///
    /// @return True if the package has been successfully transferred, else false
    bool TransferSoftwarePackage(const ara::core::String& testDataLocation,
        const ara::core::String& swPackage,
        const ara::ucm::StrongRevisionLabelString& version,
        DataTransfer<VehiclePackageManagementService>& dataTransfer);

    /// @brief Is the callback to handle status change notification.
    void OnStatusChange();

    /// @brief Retrieve the buffer of transfer state of the campaign
    ///
    /// @return A vector of transfer states converted to string
    ara::core::Vector<ara::core::String> GetStatusBuffer();

    /// @brief Clear the vector of buffered transfer states
    void FlushStatusBuffer();

    /// @brief Clear the vector of requested software packages
    void FlushRequestedPackageList();

    /// @brief Check the expected status.
    ///
    /// @param askedStatus a status it should be
    /// @return True if the current status is equal to askedStatus, else false.
    bool IsCorrectStatus(ara::vucm::TransferStateType askedStatus);

    /// @brief It registers the status field notification.
    ///
    /// @return True if successfully registered, else false.
    bool UpdateStatus();

    /// @brief Is the callback to handle Package request notification.
    void OnPackageRequested();

    /// @brief It provides the requested package names.
    ///
    /// @return Requested package names.
    ara::vucm::SwNameVersionVectorType GetRequestedPackages();

    /// @brief It registers the RequestedPackage field notification.
    ///
    /// @return True if successfully registered, else false.
    bool UpdateRequestedPackage();

    /// @brief Gets the instance of the PackageManagement service that is used by this app
    ///
    /// @return std::shared_ptr<VehiclePackageManagementService>.
    std::shared_ptr<VehiclePackageManagementService> GetService();

    ///@brief Flag of campaign transit to Cancelling state
    bool isCampaignCancelled_;

private:
    ///@brief The size of bytes for transmitting data blocks to UCM
    static constexpr uint64_t kBlockSize = 32768;

    ///@brief Used to store current value of campaign state
    ara::vucm::TransferStateType currentStatus_;

    /// @brief the instance of the VehiclePackageManagement service that is used by this app
    std::shared_ptr<VehiclePackageManagementService> service_;

    /// @brief TransferId of the last successful transfer
    ara::ucm::TransferIdType lastTransferId_;

    /// @brief For logging errors related with ucm service.
    ara::log::Logger& logger_;

    ///@brief Used to store current value of requested package
    ara::ucm::SwNameVersionType requestedPackage_;

    ///@brief Used to store the list of requested package names for this campaign
    ara::vucm::SwNameVersionVectorType requestedPackageList_;

    ///@brief Used to buffer the transfer state that the campaign crossed
    ara::core::Vector<ara::core::String> statusBuffer_;
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
const ara::core::String CurrentStatusToString(ara::vucm::TransferStateType status);

}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // APD_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_
