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

#include "data_transfer.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

using ara::core::Future;
using ara::core::Promise;
using ara::ucm::PackageManagement;
using ara::ucm::TransferIdType;
using GetSwPackagesOutput = ara::ucm::PackageManagement::GetSwPackagesOutput;

namespace apd
{
namespace ucm
{
namespace pkgmgrsample
{

// helper methods
uint32_t CalculateBlockCount(uint32_t dataAmountInBytes, uint32_t blockSizeInBytes);
void InitDataBlock(const uint32_t startIndex,
    const uint32_t blockSize,
    const ara::core::Vector<uint8_t>& data,
    ara::core::Vector<uint8_t>& dataBlock);

template <class PackageManagementService>
DataTransfer<PackageManagementService>::DataTransfer(
    const std::shared_ptr<PackageManagementService>& packageManagementService)
    : packageManagementService_(packageManagementService)
    , blockSize_(0)
    , currentSession_(0)
    , logger_(ara::log::CreateLogger("DT", "DataTransfer", ara::log::LogLevel::kVerbose))
{ }

template <class PackageManagementService>
bool DataTransfer<PackageManagementService>::InitSession(uint32_t size)
{
    Future<PackageManagement::TransferStartOutput> transferStartFuture = packageManagementService_->TransferStart(size);

    const auto transferStartResult = transferStartFuture.GetResult();

    if (!transferStartResult) {
        errorCode_ = transferStartResult.Error();
        logger_.LogError() << "[Transfer] Error while initializing transfer session."
                           << "Error message:" << transferStartResult.Error().Message();
        return false;
    } else {
        currentSession_ = transferStartResult.Value().id;
        blockSize_ = transferStartResult.Value().blockSize;
        logger_.LogInfo() << "[Transfer] transfer id" << currentSession_ << "with block size" << blockSize_;
        return true;
    }
}
template <class PackageManagementService>
bool DataTransfer<PackageManagementService>::Transfer(const ara::core::Vector<uint8_t>& data)
{
    uint32_t blockCount = CalculateBlockCount(data.size(), blockSize_);

    uint32_t blocksSent = 0;
    std::size_t dataSent = 0;
    bool retValue = TestGetSwPackages(0, blocksSent);
    if (!retValue) {
        return retValue;
    }
    while (blocksSent < blockCount) {
        ara::core::Vector<uint8_t> dataBlock;
        InitDataBlock(blocksSent * blockSize_, blockSize_, data, dataBlock);

        auto transferDataFuture = packageManagementService_->TransferData(currentSession_, dataBlock, blocksSent + 1);

        const auto transferDataResult = transferDataFuture.GetResult();
        if (!transferDataResult) {
            errorCode_ = transferDataResult.Error();
            logger_.LogError() << "[Transfer] Failed to send data block" << blocksSent << "for transfer id"
                               << currentSession_ << ". Error:" << transferDataResult.Error().Message();
            return false;
        }
        ++blocksSent;
        dataSent += dataBlock.size();
        if (blocksSent == blockCount - 1) {
            retValue = TestGetSwPackages(dataSent, blocksSent);
            if (!retValue) {
                return retValue;
            }
        }
    }
    logger_.LogInfo() << "[Transfer] Transferred" << data.size() << "bytes of data in" << blocksSent
                      << "blocks, transfer id" << currentSession_;
    retValue = TestGetSwPackages(data.size(), blocksSent);

    if (retValue) {
        logger_.LogInfo() << "[Transfer] successfully checked the transfer status of transfer id" << currentSession_
                          << "with GetSwPackages";
    }

    return retValue;
}

template <class PackageManagementService>
bool DataTransfer<PackageManagementService>::ExitSession()
{
    const auto transferExitResult = packageManagementService_->TransferExit(currentSession_).GetResult();

    if (!transferExitResult) {
        errorCode_ = transferExitResult.Error();
        logger_.LogError() << "[Transfer] TransferExit failed for transfer id" << currentSession_
                           << ". Error:" << transferExitResult.Error().Message();
        return false;
    }
    return true;
}

template <class PackageManagementService>
bool DataTransfer<PackageManagementService>::DeleteSwPackage(const ara::ucm::TransferIdType transferId)
{
    Future<void> deleteTransferFuture = packageManagementService_->DeleteTransfer(transferId);

    const auto deleteTransferResult = deleteTransferFuture.GetResult();

    if (!deleteTransferResult) {
        errorCode_ = deleteTransferResult.Error();
        logger_.LogError() << "[Transfer] failed to delete software package for transfer id" << transferId
                           << ". Error messagge:" << deleteTransferResult.Error().Message();
        return false;
    } else {
        logger_.LogInfo() << "[Transfer] successfully deleted the transfer id" << transferId;
        return true;
    }
}

template <class PackageManagementService>
GetSwPackagesOutput DataTransfer<PackageManagementService>::GetSwPackages()
{
    Future<PackageManagement::GetSwPackagesOutput> getSwPackagesFuture = packageManagementService_->GetSwPackages();

    PackageManagement::GetSwPackagesOutput getSwPackagesOutput = getSwPackagesFuture.get();

    return getSwPackagesOutput;
}

template <class PackageManagementService>
TransferIdType DataTransfer<PackageManagementService>::GetTransferId()
{
    return currentSession_;
}

template <class PackageManagementService>
bool DataTransfer<PackageManagementService>::TestGetSwPackages(std::uint64_t bytesSent, std::uint64_t blocksSent)
{
    auto swPackages = GetSwPackages().packages;
    auto swPkg = std::find_if(swPackages.begin(), swPackages.end(), [=](const ::ara::ucm::SwPackageInfoType pkg) {
        return (pkg.transferId == currentSession_);
    });
    if (swPkg == swPackages.end()) {
        logger_.LogError() << "[Transfer] transfer id" << currentSession_ << "is not found by GetSwPackages()";
        return false;
    }
    // To report both errors test them separately
    bool success = true;
    if (swPkg->consecutiveBlocksReceived != blocksSent) {
        logger_.LogError() << "[Transfer] sent" << blocksSent
                           << "blocks, but the PackageManagement service instance reports"
                           << swPkg->consecutiveBlocksReceived << "blocks are received for transfer id"
                           << currentSession_;
        success = false;
    }
    if (swPkg->consecutiveBytesReceived != bytesSent) {
        logger_.LogError() << "[Transfer] sent" << bytesSent
                           << "bytes, but the PackageManagement service instance reports"
                           << swPkg->consecutiveBytesReceived << "bytes are received for transfer id"
                           << currentSession_;
        success = false;
    }
    return success;
}
template <class PackageManagementService>
ara::core::Optional<ara::core::ErrorCode> DataTransfer<PackageManagementService>::GetTransferError()
{
    return errorCode_;
}

uint32_t CalculateBlockCount(uint32_t dataAmountInBytes, uint32_t blockSizeInBytes)
{
    uint32_t blockCount = dataAmountInBytes / blockSizeInBytes;
    if (dataAmountInBytes % blockSizeInBytes != 0) {
        blockCount++;
    }
    return blockCount;
}

void InitDataBlock(const uint32_t startIndex,
    const uint32_t blockSize,
    const ara::core::Vector<uint8_t>& data,
    ara::core::Vector<uint8_t>& dataBlock)
{
    for (uint32_t i = startIndex; i < startIndex + blockSize; i++) {
        if (i < data.size()) {
            dataBlock.push_back(data[i]);
        } else {
            break;
        }
    }
}

template class DataTransfer<ara::ucm::proxy::PackageManagementProxy>;

}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd
