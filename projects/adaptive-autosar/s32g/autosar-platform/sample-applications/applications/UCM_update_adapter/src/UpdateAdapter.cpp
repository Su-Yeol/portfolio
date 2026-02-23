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

#include "UpdateAdapter.h"

using namespace ara::ucm;

UpdateAdapter::UpdateAdapter(ara::core::InstanceSpecifier instance_spec, ara::core::String ucmId, bool itmMode)
    : Skeleton(instance_spec, ara::com::MethodCallProcessingMode::kEventSingleThread)
    , currentStatus_(PackageManagementStatusType::kIdle)
    , kvs_("update_adapter/UpdateAdapterSwc/SwPackageTransferStatusPRPort")
    , extractor_(fs_)
    , ucmId_(ucmId)
    , itmMode_(itmMode)
{ }

void UpdateAdapter::Init()
{
    logger_.LogInfo() << "UpdateAdapter with ID:" << ucmId_ << "initialized";

    // Register Field Getters.
    CurrentStatus.RegisterGetHandler(std::bind(&UpdateAdapter::getCurrentState, this));

    // Initialize Fields Values before Offering Service.
    CurrentStatus.Update(currentStatus_);

    OfferService();
}

void UpdateAdapter::Process()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);

    // State Machine mock of PackageManagementStatus
    switch (currentStatus_) {
    case PackageManagementStatusType::kProcessing:
        logger_.LogInfo() << "kProcessing --> kReady ";
        if (updateServer_.CheckForFinishedSwProcessing() || itmMode_) {
            SetCurrentStatus(PackageManagementStatusType::kReady);
            currentProcessedPackage_->SetProcessProgressValue(100);
            processingPromise_.set_value();
        }
        logger_.LogDebug() << "kProcessing --> kReady transition is over";
        break;
    case PackageManagementStatusType::kActivating:
        logger_.LogInfo() << "kActivating --> kVerifying ";
        if (updateServer_.CheckForFinishedSwActivation() || itmMode_) {
            SetCurrentStatus(PackageManagementStatusType::kVerifying);
            updateServer_.RemoveSoftwarePackages();
            activatingPromise_.set_value();
        }
        logger_.LogDebug() << "kActivating --> kVerifying transition is over";
        break;
    case PackageManagementStatusType::kVerifying:
        logger_.LogInfo() << "kVerifying --> kActivated ";
        SetCurrentStatus(PackageManagementStatusType::kActivated);
        logger_.LogDebug() << "kVerifying --> kActivated transition is over";
        break;
    default:
        break;
    }
}

void UpdateAdapter::CleanDirectory(ara::core::String pathToDir)
{
    logger_.LogInfo() << "Cleaning directory:" << pathToDir;
    boost::filesystem::path path_to_remove(pathToDir.c_str());
    for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it) {
        boost::filesystem::remove_all(it->path());
    }
}

void UpdateAdapter::SetCurrentStatus(ara::ucm::PackageManagementStatusType state)
{
    currentStatus_ = state;
    CurrentStatus.Update(state);
}

ara::core::Future<ara::ucm::skeleton::fields::CurrentStatus::value_type> UpdateAdapter::getCurrentState()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    logger_.LogInfo() << "getCurrentState: ";
    ara::core::Promise<ara::ucm::skeleton::fields::CurrentStatus::value_type> promise;
    promise.set_value(currentStatus_);
    return promise.get_future();
}

ara::core::Future<void> UpdateAdapter::Activate()
{
    ara::core::Promise<void> promise;
    activatingPromise_ = std::move(promise);
    auto future = activatingPromise_.get_future();
    logger_.LogInfo() << "Activate";

    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    SetCurrentStatus(PackageManagementStatusType::kActivating);

    if (itmMode_) {
        return future;
    }

    updateServer_.ActivateSoftwarePackages();

    return future;
}
ara::core::Future<void> UpdateAdapter::Cancel(const ::ara::ucm::TransferIdType& id)
{
    ara::core::Promise<void> promise;
    auto future = promise.get_future();
    logger_.LogInfo() << "Cancel of id: " << id;
    promise.set_value();
    return promise.get_future();
}
ara::core::Future<void> UpdateAdapter::DeleteTransfer(const ::ara::ucm::TransferIdType& id)
{
    logger_.LogInfo() << "DeleteTransfer of id: " << id;
    ara::core::Promise<void> promise;
    const auto getItemResult = packagesData_.GetItem(id);

    if (!getItemResult) {
        promise.SetError(UcmErrc::kInvalidTransferId);
        return promise.get_future();
    }

    const auto result = getItemResult->DeleteTransfer();

    if (result == libs::DeleteTransferReturnType::kSuccess) {
        packagesData_.DeleteItem(id);
    }

    promise.set_value();
    return promise.get_future();
}
ara::core::Future<void> UpdateAdapter::Finish()
{
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);
    ara::core::Promise<void> promise;

    logger_.LogInfo() << "Finish";
    if (currentStatus_ != PackageManagementStatusType::kActivated) {
        logger_.LogInfo() << "Finish: Not allowed in current state";
        promise.SetError(UcmErrc::kOperationNotPermitted);
        return promise.get_future();
    }
    SetCurrentStatus(PackageManagementStatusType::kCleaningUp);
    CleanDirectory(completedDirectory_);
    CleanDirectory(temporaryDirectory_);
    SetCurrentStatus(PackageManagementStatusType::kIdle);
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<PackageManagement::GetHistoryOutput> UpdateAdapter::GetHistory(const std::uint64_t& timestampGE,
    const std::uint64_t& timestampLT)
{
    (void)timestampGE;
    (void)timestampLT;
    ara::core::Promise<PackageManagement::GetHistoryOutput> promise;
    PackageManagement::GetHistoryOutput output;
    promise.set_value(output);
    logger_.LogInfo() << "GetHistory";
    return promise.get_future();
}
ara::core::Future<PackageManagement::GetIdOutput> UpdateAdapter::GetId()
{
    ara::core::Promise<PackageManagement::GetIdOutput> promise;
    PackageManagement::GetIdOutput output;
    output.id = ucmId_;
    promise.set_value(output);
    logger_.LogInfo() << "GetId:" << output.id;
    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwClusterChangeInfoOutput> UpdateAdapter::GetSwClusterChangeInfo()
{
    ara::core::Promise<PackageManagement::GetSwClusterChangeInfoOutput> promise;
    PackageManagement::GetSwClusterChangeInfoOutput output;
    promise.set_value(output);
    logger_.LogInfo() << "GetSwClusterChangeInfo";

    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwClusterInfoOutput> UpdateAdapter::GetSwClusterInfo()
{
    ara::core::Promise<PackageManagement::GetSwClusterInfoOutput> promise;

    PackageManagement::GetSwClusterInfoOutput output;

    promise.set_value(output);

    logger_.LogInfo() << "GetSwClusterInfo";

    return promise.get_future();
}
ara::core::Future<PackageManagement::GetSwPackagesOutput> UpdateAdapter::GetSwPackages()
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
ara::core::Future<PackageManagement::GetSwClusterManifestInfoOutput> UpdateAdapter::GetSwClusterManifestInfo(
    const ::ara::ucm::SwNameType& swName)
{
    ara::core::Promise<PackageManagement::GetSwClusterManifestInfoOutput> promise;
    PackageManagement::GetSwClusterManifestInfoOutput output;
    // todo: currently updateadapter does not have a database of installed items
    (void)swName;
    promise.set_value(output);
    logger_.LogInfo() << "GetSwClusterManifestInfo";
    return promise.get_future();
}

ara::core::Future<PackageManagement::GetSwProcessProgressOutput> UpdateAdapter::GetSwProcessProgress(
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

ara::core::Future<void> UpdateAdapter::ProcessSoftwarePackageImpl(libs::SoftwarePackageData& package,
    const TransferIdType& id)
{
    ara::core::Promise<void> promise;
    processingPromise_ = std::move(promise);

    auto path = package.GetPackageFilename();
    logger_.LogInfo() << "Processing software package at path" << path;

    if (!ExtractSwPackageToCompletedPath(path, id)) {
        logger_.LogError() << "Extraction of file with id" << std::to_string(id) << "failed";
        processingPromise_.SetError(UcmErrc::kPackageInconsistent);
        SetCurrentStatus(PackageManagementStatusType::kReady);
        return processingPromise_.get_future();
    }

    package.SetProcessProgressValue(50);

    // assuming extracted zip contains SwPackage with default name swPackageName_
    ara::core::String fileName = swPackageName_.c_str();
    ara::core::String pathToFile = GetPathToCompletedFile(path, swPackageName_).c_str();

    updateServer_.AddSoftwarePackage(id, fileName, pathToFile);
    updateServer_.ProcessSoftwarePackages();

    currentProcessedPackage_ = &package;
    return processingPromise_.get_future();
}

ara::core::Future<void> UpdateAdapter::ProcessSwPackage(const ::ara::ucm::TransferIdType& id)
{
    logger_.LogInfo() << "ProcessSwPackage of id: " << id;
    std::lock_guard<std::mutex> guard(accessCurrentStatus_);

    if (currentStatus_ == PackageManagementStatusType::kIdle || currentStatus_ == PackageManagementStatusType::kReady) {
        SetCurrentStatus(PackageManagementStatusType::kProcessing);
        auto package = packagesData_.GetItem(id);
        if (!package) {
            ara::core::Promise<void> promise;
            promise.SetError(UcmErrc::kInvalidTransferId);
            SetCurrentStatus(PackageManagementStatusType::kReady);
            return promise.get_future();
        } else {
            return ProcessSoftwarePackageImpl(*package, id);
        }
    }

    ara::core::Promise<void> promise;
    promise.SetError(UcmErrc::kServiceBusy);
    return promise.get_future();
}
ara::core::Future<void> UpdateAdapter::RevertProcessedSwPackages()
{
    ara::core::Promise<void> promise;
    auto future = promise.get_future();
    logger_.LogInfo() << "RevertProcessedSwPackages";
    promise.set_value();
    return future;
}
ara::core::Future<void> UpdateAdapter::Rollback()
{
    ara::core::Promise<void> promise;
    auto future = promise.get_future();
    logger_.LogInfo() << "Rollback";
    promise.set_value();
    return future;
}
ara::core::Future<void> UpdateAdapter::TransferData(const ::ara::ucm::TransferIdType& id,
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

ara::core::String UpdateAdapter::GetSwPackageFileName(ara::core::String pathToFile)
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

ara::core::String UpdateAdapter::GetPathToCompletedFile(ara::core::String extractDirName, ara::core::String packageName)
{
    return completedDirectory_ + "/" + GetSwPackageFileName(extractDirName) + "/" + packageName;
}

bool UpdateAdapter::ExtractSwPackageToCompletedPath(ara::core::String pathToFile, const TransferIdType& id)
{
    const ara::core::String extractionDir = completedDirectory_ + "/" + std::to_string(id);
    logger_.LogDebug() << "Extracting file with id:" << id;
    logger_.LogDebug() << "path to file: " << pathToFile;
    logger_.LogDebug() << "extraction dir:" << extractionDir;

    return extractor_.Extract(pathToFile, extractionDir, std::to_string(id));
}

ara::core::Future<void> UpdateAdapter::TransferExit(const ::ara::ucm::TransferIdType& id)
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
    case libs::TransferExitReturnType::kSuccess:
        packagePtr->SetState(SwPackageStateType::kTransferred);
        break;
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
    promise.set_value();
    return future;
}
ara::core::Future<PackageManagement::TransferStartOutput> UpdateAdapter::TransferStart(const std::uint64_t& size)
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

    auto package = *item;

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
