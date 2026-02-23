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

#ifndef ARA_UCM_PKGMGR_FLASHING_ADAPTER_H_
#define ARA_UCM_PKGMGR_FLASHING_ADAPTER_H_

#include <memory>
#include <mutex>

#include "ara/core/string.h"
#include "ara/log/logger.h"
#include "ara/per/key_value_storage.h"

#include "ara/ucm/packagemanagement_skeleton.h"
#include "transfer/map_of_serial_id_to_ptr.h"  // TransferIdToSoftwarePackageFactory
#include "transfer/per_transfer_status_storage.h"
#include "extraction/software_package_extractor.h"  // ara::ucm::libs::SoftwarePackageExtractor
#include "extraction/lib_poco_zip_extractor.h"  // LibPocoZipExtractor

#include "ecu_reprogramming_sequence.h"

class FlashingAdapter : public ara::ucm::skeleton::PackageManagementSkeleton
{
public:
    using Skeleton = ara::ucm::skeleton::PackageManagementSkeleton;

    FlashingAdapter(ara::core::InstanceSpecifier instance_spec, ara::core::String ucmId, bool execMode);

    /** \brief Init function of FlashingAdapter
     *
     *  Has to be called once before calling Process() the first time.
     */
    void Init();

    /** \brief Main routine of FlashingAdapter
     *
     * Has to be called cyclicly.
     *
     */
    void Process();

    ara::core::Future<void> Activate() override;
    ara::core::Future<void> Cancel(const ::ara::ucm::TransferIdType& id) override;
    ara::core::Future<void> DeleteTransfer(const ::ara::ucm::TransferIdType& id) override;
    ara::core::Future<void> Finish() override;
    using ara::ucm::PackageManagement::GetHistoryOutput;
    ara::core::Future<GetHistoryOutput> GetHistory(const std::uint64_t& timestampGE,
        const std::uint64_t& timestampLT) override;
    using ara::ucm::PackageManagement::GetIdOutput;
    ara::core::Future<GetIdOutput> GetId() override;
    using ara::ucm::PackageManagement::GetSwClusterChangeInfoOutput;
    ara::core::Future<GetSwClusterChangeInfoOutput> GetSwClusterChangeInfo() override;
    using ara::ucm::PackageManagement::GetSwClusterInfoOutput;
    ara::core::Future<GetSwClusterInfoOutput> GetSwClusterInfo() override;
    using ara::ucm::PackageManagement::GetSwPackagesOutput;
    ara::core::Future<GetSwPackagesOutput> GetSwPackages() override;
    using ara::ucm::PackageManagement::GetSwClusterManifestInfoOutput;
    ara::core::Future<GetSwClusterManifestInfoOutput> GetSwClusterManifestInfo(
        const ::ara::ucm::SwNameType& swName) override;
    using ara::ucm::PackageManagement::GetSwProcessProgressOutput;
    ara::core::Future<GetSwProcessProgressOutput> GetSwProcessProgress(const ::ara::ucm::TransferIdType& id) override;
    ara::core::Future<void> ProcessSwPackage(const ::ara::ucm::TransferIdType& id) override;
    ara::core::Future<void> RevertProcessedSwPackages() override;
    ara::core::Future<void> Rollback() override;
    ara::core::Future<void> TransferData(const ::ara::ucm::TransferIdType& id,
        const ::ara::ucm::ByteVectorType& data,
        const std::uint64_t& blockCounter) override;
    ara::core::Future<void> TransferExit(const ::ara::ucm::TransferIdType& id) override;
    using ara::ucm::PackageManagement::TransferStartOutput;
    ara::core::Future<TransferStartOutput> TransferStart(const std::uint64_t& size) override;

    ara::core::Future<ara::ucm::skeleton::fields::CurrentStatus::value_type> getCurrentState();

private:
    /** \brief Setter for currentStatus
     *
     * \param state state which has to be set
     *
     */
    void SetCurrentStatus(ara::ucm::PackageManagementStatusType state);

    /** \brief Verfiy activated sw packages
     *
     *  \return true if successful
     */
    bool Verify();

    /** \brief Rollback internal function
     *
     *  \return true if successful
     */
    bool RollbackImpl();

    /** \brief Activate internal function
     *
     *  \return true if successful
     */
    bool ActivateImpl();

    /** \brief Process SW package internal function
     *
     * \param package SW package which has to be processed
     * \param id id of the SW package
     *
     */
    ara::core::Future<void> ProcessSoftwarePackageImpl(ara::ucm::libs::SoftwarePackageData& package,
        const ara::ucm::TransferIdType& id);

    /** \brief File copy function
     *
     * Extracts received Sw package zip archive to the path of completed file transfers
     *
     * \param pathToFile path to file which has to be copyed
     * \param id id of sw package
     * \param extractionDir dir to extract to
     * \return true if successful
     */
    bool ExtractSwPackageToCompletedPath(ara::core::String& pathToFile,
        const ara::ucm::TransferIdType& id,
        const ara::core::String& extractionDir);

    /** \brief Cleans given directory by deleting all files in that path
     *
     * \param pathToDir path to directory
     *
     */
    void CleanDirectory(ara::core::String pathToDir);

    void CleaningUp();

    /** \brief Gets filename of given path to file without file ending
     *
     * \param pathToFile path to file
     *
     */
    ara::core::String GetSwPackageFileName(ara::core::String pathToFile);

    /** \brief Composes the path to the sw package file
     *
     * \param extractDirName name of extraction directory
     * \param packageName software package name
     */
    ara::core::String GetPathToCompletedFile(ara::core::String extractDirName, ara::core::String packageName);

    /** \brief Update SwClusterDescription Database
     *
     */
    void UpdateSwClusterDescriptionDatabase();

    /** \brief Initialize SwClusterDescription Database
     * Fills the sw cluster description database with demo data, because ara::per currently does not support
     * serializing (and loading) complex data types to (from) storage. To be replaced with default kvs data
     * when available.
     */
    void InitializeSwClusterDecriptionDatabase();

    ara::log::Logger& logger_{ara::log::CreateLogger("FAMA", "FlashingAdapterManager", ara::log::LogLevel::kVerbose)};
    /**
     * \brief Current status of the PackageManager
     */
    ara::ucm::PackageManagementStatusType currentStatus_;
    /**
     * \brief Mutex for access control of currentStatus
     */
    std::mutex accessCurrentStatus_;

    /**
     * \brief Promise for processing
     */
    ara::core::Promise<void> processingPromise_;

    /**
     * \brief Promise for activating
     */
    ara::core::Promise<void> activatingPromise_;

    /**
     * \brief Software Package Data for storing data about software package in work
     */
    ara::ucm::libs::TransferIdToSoftwarePackageFactory packagesData_;

    /**
     * \brief Transfer Status database
     */
    ara::ucm::libs::TransferStatusKvs kvs_;

    /// @brief A shared handle to access key-value storage
    ara::per::SharedHandle<ara::per::KeyValueStorage> kvsSwDesc_;

    ara::ucm::libs::Filesystem fs_;

    ara::ucm::libs::LibPocoZipExtractor extractor_;

    ara::core::Map<ara::ucm::TransferIdType, std::unique_ptr<EcuReprogrammingSequence>> ecuReprogrammingContainer_;

    const ara::ucm::UCMIdentifierType ucmId_;
    const bool itmMode_;

    std::shared_ptr<PduApiHandle> pduApiHandle_ = std::make_shared<PduApiHandle>();

    const ara::core::String version_{"1.0.0"};
    const ara::core::String completedDirectory_{"var/completed"};
    const ara::core::String temporaryDirectory_{"var/temp"};
    const ara::core::String swPackageManifestName_{"SWPKG_MANIFEST.json"};
    const ara::core::String swclPackageManifestName_{"SWCL_MANIFEST.json"};
    const ara::core::String progConfName_{"PROG_CONF.json"};
};

#endif  // ARA_UCM_PKGMGR_FLASHING_ADAPTER_H_
