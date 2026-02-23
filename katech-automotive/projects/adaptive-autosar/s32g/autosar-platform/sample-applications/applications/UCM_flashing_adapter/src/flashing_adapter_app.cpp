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

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>  // boost::filesystem::exists boost::filesystem::create_directory

#include "ara/com/com_error_domain.h"
#include "ara/per/per_error_domain.h"

#include "flashing_adapter_app.h"

#include "ecu_reprogramming_sequence_factory.h"

using namespace ara::ucm;

FlashingAdapter::FlashingAdapter(ara::core::InstanceSpecifier instance_spec, ara::core::String ucmId, bool itmMode)
    : Skeleton(instance_spec, ara::com::MethodCallProcessingMode::kEventSingleThread)
    , currentStatus_(PackageManagementStatusType::kIdle)
    , kvs_("flashing_adapter/FlashingAdapterSwc/SwPackageTransferStatusPRPort")
    , kvsSwDesc_{ara::per::OpenKeyValueStorage(
          ara::core::InstanceSpecifier("flashing_adapter/FlashingAdapterSwc/SwClusterDescPRPort"))
                     .ValueOrThrow()}
    , extractor_(fs_)
    , ucmId_(ucmId)
    , itmMode_(itmMode)
{ }

void FlashingAdapter::Init()
{
    logger_.LogInfo() << "FlashingAdapter with ID:" << ucmId_ << "initialized";

    // Register Field Getters.
    CurrentStatus.RegisterGetHandler(std::bind(&FlashingAdapter::getCurrentState, this));

    // Initialize Fields Values before Offering Service.
    CurrentStatus.Update(currentStatus_);

    // TODO: Remove if ara::per is able to store and load complex data types
    InitializeSwClusterDecriptionDatabase();

    OfferService();
}

void FlashingAdapter::InitializeSwClusterDecriptionDatabase()
{
    const auto swClusterDescDatabaseResult
        = kvsSwDesc_->GetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>("SwClusterManifests");

    if (!swClusterDescDatabaseResult) {
        auto error = swClusterDescDatabaseResult.Error();
        logger_.LogInfo() << "Error:" << error.Message();

        ara::core::Vector<ara::ucm::SwClusterManifestInfoType> swClusterDescDatabase;

        ::ara::ucm::DependencyVectorType emptyDependency;
        swClusterDescDatabase.push_back({"demonstratorEcuSW",
            "1.0.0",
            "e0*42/42*1234*01",
            "AUTOSAR",
            "Release info demonstratorEcuSW",
            emptyDependency});

        kvsSwDesc_->SetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>(
            "SwClusterManifests", swClusterDescDatabase);
    }
    auto test = kvsSwDesc_->GetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>("SwClusterManifests")
                    .ValueOrThrow();
    for (auto const& swClusterManifestInfo : test) {
        logger_.LogInfo() << "Name: " << swClusterManifestInfo.name;
        logger_.LogInfo() << "Version: " << swClusterManifestInfo.version;
        logger_.LogInfo() << "Type approval: " << swClusterManifestInfo.typeApproval;
        logger_.LogInfo() << "License: " << swClusterManifestInfo.license;
        logger_.LogInfo() << "Release notes: " << swClusterManifestInfo.releaseNotes;
        // log dependencies?
    };
}

void FlashingAdapter::UpdateSwClusterDescriptionDatabase()
{
    logger_.LogInfo() << "UpdateSwClusterDescriptionDatabase";

    const auto swClusterDescDatabaseResult
        = kvsSwDesc_->GetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>("SwClusterManifests");

    if (!swClusterDescDatabaseResult) {
        auto error = swClusterDescDatabaseResult.Error();
        logger_.LogError() << "Error:" << error.Message();
        return;
    }
    auto swClusterDescDatabase = swClusterDescDatabaseResult.Value();

    for (auto const& ecuReprogrammingSequence : ecuReprogrammingContainer_) {
        auto name = ecuReprogrammingSequence.second->GetEcuSwName();
        auto version = ecuReprogrammingSequence.second->GetEcuSwVersion();

        bool clusterInfoAlreadyInDatabase = false;
        for (auto& swDesc : swClusterDescDatabase) {
            if (swDesc.name == name) {
                logger_.LogInfo() << "SwClusterDesc already in database, updating version info to version" << version;
                swDesc.version = version;
                clusterInfoAlreadyInDatabase = true;
            }
        }
        if (!clusterInfoAlreadyInDatabase) {
            ::ara::ucm::DependencyVectorType emptyDependency;
            logger_.LogInfo() << "SwClusterDesc not in database, create new one...";
            swClusterDescDatabase.push_back({name, version, "", "Autosar Demonstrator", "", emptyDependency});
        }
    }

    kvsSwDesc_->SetValue("SwClusterDescription", swClusterDescDatabase);
    // TODO: would call kvsSwDesc_->SyncToStorage() here, but ara::per currently cannot handle
    // serialization of complex types
}

void FlashingAdapter::Process()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);

    // State Machine mock of PackageManagementStatus
    switch (currentStatus_) {
    case PackageManagementStatusType::kVerifying:
        logger_.LogInfo() << "kVerifying --> kActivated ";

        if (Verify()) {
            SetCurrentStatus(PackageManagementStatusType::kActivated);
            logger_.LogInfo() << "kActivated";
        } else {
            SetCurrentStatus(PackageManagementStatusType::kRollingBack);
            logger_.LogInfo() << "kRollingBack";
            RollbackImpl();
            SetCurrentStatus(PackageManagementStatusType::kRolledBack);
            logger_.LogInfo() << "kRolledBack";
        }
        break;
    default:
        break;
    }
}

bool FlashingAdapter::Verify()
{
    for (auto const& ecuReprogrammingSequence : ecuReprogrammingContainer_) {
        logger_.LogInfo() << "Verify id" << ecuReprogrammingSequence.first;
        const auto result = ecuReprogrammingSequence.second->Verify().GetResult();

        if (result.HasValue()) {
            logger_.LogInfo() << "[Verify] succeeded for transfer id" << ecuReprogrammingSequence.first;
        } else {
            logger_.LogError() << "[Verify] failed for transfer id" << ecuReprogrammingSequence.first
                               << "with error:" << result.Error() << result.Error().Message();
            return false;
        }
    }
    return true;
}

void FlashingAdapter::CleanDirectory(ara::core::String pathToDir)
{
    logger_.LogInfo() << "Cleaning directory:" << pathToDir;
    boost::filesystem::path path_to_remove(pathToDir.c_str());
    for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it) {
        boost::filesystem::remove_all(it->path());
    }
}

void FlashingAdapter::CleaningUp()
{
    CleanDirectory(completedDirectory_);
    CleanDirectory(temporaryDirectory_);
    ecuReprogrammingContainer_.clear();
}

void FlashingAdapter::SetCurrentStatus(ara::ucm::PackageManagementStatusType state)
{
    currentStatus_ = state;
    CurrentStatus.Update(state);
}
ara::core::Future<ara::ucm::skeleton::fields::CurrentStatus::value_type> FlashingAdapter::getCurrentState()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    logger_.LogInfo() << "getCurrentState: ";
    ara::core::Promise<ara::ucm::skeleton::fields::CurrentStatus::value_type> promise;
    promise.set_value(currentStatus_);
    return promise.get_future();
}

bool FlashingAdapter::ActivateImpl()
{
    for (auto const& ecuReprogrammingSequence : ecuReprogrammingContainer_) {
        logger_.LogInfo() << "Activating id" << ecuReprogrammingSequence.first;
        const auto result = ecuReprogrammingSequence.second->Activate().GetResult();

        if (result.HasValue()) {
            logger_.LogInfo() << "[Activate] succeeded for transfer id" << ecuReprogrammingSequence.first;
        } else {
            logger_.LogError() << "[Activate] failed for transfer id" << ecuReprogrammingSequence.first
                               << "with error:" << result.Error() << result.Error().Message();
            return false;
        }
    }
    return true;
}

ara::core::Future<void> FlashingAdapter::Activate()
{
    ara::core::Promise<void> promise;

    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    logger_.LogInfo() << "Activate";

    if (currentStatus_ != PackageManagementStatusType::kReady) {
        logger_.LogInfo() << "Activate: Not allowed in current state";
        promise.SetError(UcmErrc::kOperationNotPermitted);
        return promise.get_future();
    }

    SetCurrentStatus(PackageManagementStatusType::kActivating);

    if (ActivateImpl()) {
        SetCurrentStatus(PackageManagementStatusType::kVerifying);
        promise.set_value();
    } else {
        SetCurrentStatus(PackageManagementStatusType::kReady);
        promise.SetError(UcmErrc::kPrepareUpdateFailed);
    }

    return promise.get_future();
}
ara::core::Future<void> FlashingAdapter::Cancel(const ::ara::ucm::TransferIdType& id)
{
    ara::core::Promise<void> promise;
    auto future = promise.get_future();
    logger_.LogInfo() << "Cancel of id: " << id;
    promise.set_value();
    return promise.get_future();
}
ara::core::Future<void> FlashingAdapter::DeleteTransfer(const ::ara::ucm::TransferIdType& id)
{
    logger_.LogInfo() << "DeleteTransfer of id: " << id;
    ara::core::Promise<void> promise;
    auto getItemResult = packagesData_.GetItem(id);

    if (!getItemResult) {
        promise.SetError(UcmErrc::kInvalidTransferId);
        return promise.get_future();
    }

    auto result = getItemResult->DeleteTransfer();

    if (result == libs::DeleteTransferReturnType::kSuccess) {
        packagesData_.DeleteItem(id);
    }

    promise.set_value();
    return promise.get_future();
}
ara::core::Future<void> FlashingAdapter::Finish()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    ara::core::Promise<void> promise;

    logger_.LogInfo() << "Finish";
    if (currentStatus_ != PackageManagementStatusType::kActivated
        && currentStatus_ != PackageManagementStatusType::kRolledBack) {
        logger_.LogInfo() << "Finish: Not allowed in current state";
        promise.SetError(UcmErrc::kOperationNotPermitted);
        return promise.get_future();
    }

    if (currentStatus_ == PackageManagementStatusType::kActivated) {
        logger_.LogInfo() << "Successfully updated ECUs:";
        UpdateSwClusterDescriptionDatabase();
        for (auto const& ecuReprogrammingSequence : ecuReprogrammingContainer_) {
            auto name = ecuReprogrammingSequence.second->GetEcuSwName();
            auto version = ecuReprogrammingSequence.second->GetEcuSwVersion();
            logger_.LogInfo() << name << "Version" << version;
        }
    }

    SetCurrentStatus(PackageManagementStatusType::kCleaningUp);
    CleaningUp();
    SetCurrentStatus(PackageManagementStatusType::kIdle);
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<PackageManagement::GetHistoryOutput> FlashingAdapter::GetHistory(const std::uint64_t& timestampGE,
    const std::uint64_t& timestampLT)
{
    (void)timestampGE;
    (void)timestampLT;
    ara::core::Promise<PackageManagement::GetHistoryOutput> promise;
    PackageManagement::GetHistoryOutput output;
    promise.set_value(output);
    logger_.LogWarn() << "GetHistory is not implemented";
    return promise.get_future();
}
ara::core::Future<PackageManagement::GetIdOutput> FlashingAdapter::GetId()
{
    ara::core::Promise<PackageManagement::GetIdOutput> promise;
    PackageManagement::GetIdOutput output;
    output.id = ucmId_;
    promise.set_value(output);
    logger_.LogInfo() << "GetId:" << output.id;

    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwClusterChangeInfoOutput> FlashingAdapter::GetSwClusterChangeInfo()
{
    ara::core::Promise<PackageManagement::GetSwClusterChangeInfoOutput> promise;
    PackageManagement::GetSwClusterChangeInfoOutput output;
    promise.set_value(output);
    logger_.LogWarn() << "GetSwClusterChangeInfo is not implemented";

    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwClusterInfoOutput> FlashingAdapter::GetSwClusterInfo()
{
    ara::core::Promise<PackageManagement::GetSwClusterInfoOutput> promise;

    PackageManagement::GetSwClusterInfoOutput output;

    auto swClusters = kvsSwDesc_->GetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>("SwClusterManifests")
                          .ValueOrThrow();
    std::for_each(swClusters.begin(), swClusters.end(), [&output](auto& swCluster) {
        output.swInfo.push_back({swCluster.name, swCluster.version, ara::ucm::SwClusterStateType::kPresent, 42 * 1024});
    });

    promise.set_value(output);

    logger_.LogInfo() << "GetSwClusterInfo";

    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwPackagesOutput> FlashingAdapter::GetSwPackages()
{
    PackageManagement::GetSwPackagesOutput out;

    packagesData_.IterateItems([&](const std::pair<TransferIdType, std::shared_ptr<libs::SoftwarePackageData>> pair) {
        SwPackageInfoType info;

        info.transferId = pair.first;
        info.state = pair.second->GetState();
        info.consecutiveBytesReceived = pair.second->GetReceivedBytes();
        info.consecutiveBlocksReceived = pair.second->GetReceivedBlocks();
        // The package in state of "Transferring" has no unpacked manifests
        if (info.state != SwPackageStateType::kTransferring) {
            info.name = GetSwPackageFileName(pair.second->GetPackageFilename());
            info.version = "1.0";
        }
        out.packages.push_back(info);
    });

    ara::core::Promise<PackageManagement::GetSwPackagesOutput> promise;
    promise.set_value(out);
    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwClusterManifestInfoOutput> FlashingAdapter::GetSwClusterManifestInfo(
    const SwNameType& swName)
{
    ara::core::Promise<PackageManagement::GetSwClusterManifestInfoOutput> promise;
    PackageManagement::GetSwClusterManifestInfoOutput output;
    auto swClusters
        = kvsSwDesc_->GetValue<ara::core::Vector<ara::ucm::SwClusterManifestInfoType>>("SwClusterManifests");
    if (swClusters) {
        logger_.LogInfo() << "SwClusterDescription database successfully read";
        for (auto swCluster : swClusters.Value()) {
            if (swCluster.name == swName) {
                output.swInfo = swCluster;
            }
        }
    } else {
        auto error = swClusters.Error();
        logger_.LogError() << "Error:" << error.Message();
    }
    promise.set_value(output);
    logger_.LogInfo() << "GetSwClusterDescription";

    return promise.get_future();
}

ara::core::Future<PackageManagement::GetSwProcessProgressOutput> FlashingAdapter::GetSwProcessProgress(
    const TransferIdType& id)
{

    logger_.LogInfo() << "GetSwProcessProgress of id: " << id;
    ara::core::Promise<PackageManagement::GetSwProcessProgressOutput> promise;

    auto package = packagesData_.GetItem(id);
    if (!package) {
        promise.SetError(UcmErrc::kInvalidTransferId);
    } else {
        promise.set_value({package->GetProcessProgressValue()});
    }
    return promise.get_future();
}

ara::core::Future<void> FlashingAdapter::ProcessSoftwarePackageImpl(libs::SoftwarePackageData& package,
    const TransferIdType& id)
{
    ara::core::Promise<void> promise;

    auto path = package.GetPackageFilename();
    logger_.LogInfo() << "Processing software package at path" << path;

    package.SetProcessProgressValue(50);

    if (ecuReprogrammingContainer_.count(id)) {
        const auto result = ecuReprogrammingContainer_.at(id)->Install().GetResult();

        if (result.HasValue()) {
            logger_.LogInfo() << "[Install] succeeded for transfer id" << id;
            promise.set_value();
        } else {
            logger_.LogError() << "[Install] failed for transfer id" << id << "with error:" << result.Error()
                               << result.Error().Message();
            promise.SetError(UcmErrc::kPackageInconsistent);
        }
    } else {
        logger_.LogError() << "Invalid transfer ID!";
        promise.SetError(UcmErrc::kInvalidTransferId);
    }

    package.SetProcessProgressValue(100);
    {
        std::lock_guard<std::mutex> guard(accessCurrentStatus_);
        SetCurrentStatus(PackageManagementStatusType::kReady);
    }
    return promise.get_future();
}

ara::core::Future<void> FlashingAdapter::ProcessSwPackage(const ::ara::ucm::TransferIdType& id)
{
    logger_.LogInfo() << "ProcessSwPackage of id: " << id;

    if (currentStatus_ == PackageManagementStatusType::kIdle || currentStatus_ == PackageManagementStatusType::kReady) {
        {
            std::lock_guard<std::mutex> guard(accessCurrentStatus_);
            SetCurrentStatus(PackageManagementStatusType::kProcessing);
        }
        auto package = packagesData_.GetItem(id);
        if (!package) {
            ara::core::Promise<void> promise;
            promise.SetError(UcmErrc::kInvalidTransferId);
            {
                std::lock_guard<std::mutex> guard(accessCurrentStatus_);
                SetCurrentStatus(PackageManagementStatusType::kReady);
            }
            return promise.get_future();
        } else {
            return ProcessSoftwarePackageImpl(*package, id);
        }
    }

    ara::core::Promise<void> promise;
    promise.SetError(UcmErrc::kServiceBusy);
    return promise.get_future();
}
ara::core::Future<void> FlashingAdapter::RevertProcessedSwPackages()
{
    ara::core::Promise<void> promise;
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    logger_.LogInfo() << "RevertProcessedSwPackages";
    if (currentStatus_ != PackageManagementStatusType::kReady) {
        logger_.LogInfo() << "RevertProcessedSwPackages: Not allowed in current state";
        promise.SetError(UcmErrc::kOperationNotPermitted);
        return promise.get_future();
    }

    SetCurrentStatus(PackageManagementStatusType::kCleaningUp);
    CleaningUp();
    SetCurrentStatus(PackageManagementStatusType::kIdle);
    promise.set_value();
    return promise.get_future();
}

bool FlashingAdapter::RollbackImpl()
{
    for (auto const& ecuReprogrammingSequence : ecuReprogrammingContainer_) {
        logger_.LogInfo() << "Rollback id" << ecuReprogrammingSequence.first;
        const auto result = ecuReprogrammingSequence.second->Rollback().GetResult();

        if (result.HasValue()) {
            logger_.LogInfo() << "[Rollback] succeeded for transfer id" << ecuReprogrammingSequence.first;
        } else {
            logger_.LogError() << "[Rollback] failed for transfer id" << ecuReprogrammingSequence.first
                               << "with error:" << result.Error() << result.Error().Message();
            return false;
        }
    }
    return true;
}

ara::core::Future<void> FlashingAdapter::Rollback()
{
    ara::core::Promise<void> promise;
    auto future = promise.get_future();
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);

    logger_.LogInfo() << "Rollback";
    if (currentStatus_ != PackageManagementStatusType::kActivated) {
        logger_.LogInfo() << "Rollback: Not allowed in current state";
        promise.SetError(UcmErrc::kOperationNotPermitted);
        return promise.get_future();
    }

    SetCurrentStatus(PackageManagementStatusType::kRollingBack);

    promise.set_value();
    if (RollbackImpl()) {
        SetCurrentStatus(PackageManagementStatusType::kRolledBack);
    } else {
        SetCurrentStatus(PackageManagementStatusType::kRollingBackFailed);
    }

    return future;
}
ara::core::Future<void> FlashingAdapter::TransferData(const ::ara::ucm::TransferIdType& id,
    const ByteVectorType& data,
    const std::uint64_t& blockCounter)
{
    logger_.LogInfo() << "TransferData of id: " << id << " blockCounter: " << blockCounter;
    ara::core::Promise<void> promise;
    auto future = promise.get_future();

    const auto getItemResult = packagesData_.GetItem(id);

    if (!getItemResult) {
        promise.SetError(UcmErrc::kInvalidTransferId);
        return future;
    } else {
        switch (getItemResult->TransferData(data, blockCounter)) {
        case libs::TransferDataReturnType::kSuccess:
            promise.set_value();
            break;
        case libs::TransferDataReturnType::kIncorrectBlock:
            promise.SetError(UcmErrc::kIncorrectBlock);
            break;
        case libs::TransferDataReturnType::kIncorrectSize:
            promise.SetError(UcmErrc::kIncorrectSize);
            break;
        case libs::TransferDataReturnType::kInsufficientMemory:
            promise.SetError(UcmErrc::kInsufficientMemory);
            break;
        case libs::TransferDataReturnType::kInvalidTransferId:
            promise.SetError(UcmErrc::kInvalidTransferId);
            break;
        default:
            promise.SetError(UcmErrc::kOperationNotPermitted);
        }
    }

    return future;
}

ara::core::String FlashingAdapter::GetSwPackageFileName(ara::core::String pathToFile)
{
    // Remove directory if present.
    // Do this before extension removal incase directory has a period character.
    const size_t last_slash_idx = pathToFile.find_last_of("\\/");
    if (ara::core::String::npos != last_slash_idx) {
        pathToFile.erase(0, last_slash_idx + 1);
    }

    // Remove extension if present.
    const size_t period_idx = pathToFile.rfind('.');
    if (ara::core::String::npos != period_idx) {
        pathToFile.erase(period_idx);
    }
    return pathToFile;
}

ara::core::String FlashingAdapter::GetPathToCompletedFile(ara::core::String extractDirName,
    ara::core::String packageName)
{
    return completedDirectory_ + "/" + GetSwPackageFileName(extractDirName) + "/" + packageName;
}

bool FlashingAdapter::ExtractSwPackageToCompletedPath(ara::core::String& pathToFile,
    const TransferIdType& id,
    const ara::core::String& extractionDir)
{
    logger_.LogDebug() << "Extracting file with id:" << id;
    logger_.LogDebug() << "path to file: " << pathToFile;
    logger_.LogDebug() << "extraction dir:" << extractionDir;

    return extractor_.Extract(pathToFile, extractionDir, std::to_string(id));
}

ara::core::Future<void> FlashingAdapter::TransferExit(const ::ara::ucm::TransferIdType& id)
{
    logger_.LogInfo() << "TransferExit of id: " << id;
    ara::core::Promise<void> promise;
    auto future = promise.get_future();

    auto packagePtr = packagesData_.GetItem(id);

    if (!packagePtr) {
        promise.SetError(UcmErrc::kInvalidTransferId);
        return future;
    }

    switch (packagePtr->TransferExit()) {
    case libs::TransferExitReturnType::kSuccess: {
        packagePtr->SetState(SwPackageStateType::kTransferred);

        auto path = packagePtr->GetPackageFilename();
        const ara::core::String extractionDir = completedDirectory_ + "/" + std::to_string(id);

        if (!ExtractSwPackageToCompletedPath(path, id, extractionDir)) {
            logger_.LogError() << "Extraction of file with id" << std::to_string(id) << "failed";
            promise.SetError(UcmErrc::kPackageInconsistent);
        } else {
            auto pathToConfFile = GetPathToCompletedFile(path, progConfName_.c_str());

            EcuReprogrammingSequenceFactory ecuReprogrammingSequenceFactory(
                pduApiHandle_, extractionDir, pathToConfFile);
            auto reprogrammingSequence = ecuReprogrammingSequenceFactory.Create();

            if (reprogrammingSequence) {
                ecuReprogrammingContainer_.insert(std::make_pair(id, std::move(reprogrammingSequence)));
                promise.set_value();
            } else {
                logger_.LogError() << "Creation of reprogramming sequence failed, prog conf invalid";
                promise.SetError(UcmErrc::kInvalidPackageManifest);
            }
        }
        break;
    }
    case libs::TransferExitReturnType::kInsufficientData:
        promise.SetError(UcmErrc::kInsufficientData);
        break;
    case libs::TransferExitReturnType::kPackageInconsistent:
        promise.SetError(UcmErrc::kPackageInconsistent);
        break;
    case libs::TransferExitReturnType::kInvalidTransferId:
        promise.SetError(UcmErrc::kInvalidTransferId);
        break;
    case libs::TransferExitReturnType::kOperationNotPermitted:
        promise.SetError(UcmErrc::kOperationNotPermitted);
        break;
    default:
        promise.SetError(UcmErrc::kOperationNotPermitted);
    }

    return future;
}
ara::core::Future<PackageManagement::TransferStartOutput> FlashingAdapter::TransferStart(const std::uint64_t& size)
{
    ara::core::Promise<PackageManagement::TransferStartOutput> promise;
    auto future = promise.get_future();

    libs::SoftwarePackageDataFactory factory{kvs_, logger_, fs_};
    libs::SerialIDGenerator<TransferIdType> idGenerator;
    auto item = packagesData_.AddNewItemWithId(idGenerator, factory, temporaryDirectory_);

    if (!item) {
        logger_.LogError() << "TransferStart for" << size << "bytes failed to create package data.";
        promise.SetError(UcmErrc::kInsufficientMemory);
        return future;
    }

    const auto package = *item;

    const auto result = package.second->TransferStart(size);

    if (result.transferStartSuccess != libs::TransferStartSuccessType::kSuccess) {

        packagesData_.DeleteItem(package.first);
        logger_.LogError() << "TransferStart for" << size << "bytes failed to start transfer.";
        promise.SetError(UcmErrc::kInsufficientMemory);
        return future;
    }

    package.second->SetState(SwPackageStateType::kTransferring);
    promise.set_value({result.transferId, UCM_DATA_BLOCK_SIZE});
    return future;
}
