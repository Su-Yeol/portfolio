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
#include "ucm_accessor.h"

using ara::core::Future;
using ara::core::Promise;
using ara::ucm::TransferIdType;

namespace apd
{
namespace ucm
{
namespace ota
{

// helper methods
static uint64_t CalculateBlockCount(uint64_t dataAmountInBytes, uint64_t blockSizeInBytes)
{
    uint64_t blockCount = dataAmountInBytes / blockSizeInBytes;
    if (dataAmountInBytes % blockSizeInBytes != 0) {
        blockCount++;
    }
    return blockCount;
}

static void InitDataBlock(const uint64_t startIndex,
    const uint64_t blockSize,
    const std::vector<uint8_t>& data,
    std::vector<uint8_t>& dataBlock)
{
    for (uint64_t i = startIndex; i < startIndex + blockSize; i++) {
        if (i < data.size()) {
            dataBlock.push_back(data[i]);
        } else {
            break;
        }
    }
}

template <class VehiclePackageManagementService>
bool DataTransfer<VehiclePackageManagementService>::InitSession(uint64_t size)
{
    #if 1
    CheckGrant();
    #endif

    auto transferStartResult = vehiclePackageManagementService_->TransferVehiclePackage(size).GetResult();

    if (!transferStartResult) {
        logger_.LogError() << "Error while initializing vehicle package transfer session"
                           << "Error Code:" << transferStartResult.Error();
        return false;
    } else {
        currentSession_ = transferStartResult.Value().id;
        blockSize_ = transferStartResult.Value().blockSize;
        logger_.LogInfo() << "InitSession was successful for software package session" << currentSession_
                          << "with received block size:" << blockSize_;
        return true;
    }
}

template <class VehiclePackageManagementService>
bool DataTransfer<VehiclePackageManagementService>::InitSession(const ara::core::String& name)
{
    #if 1
    // IAM Check
    CheckGrant();
    #endif

    auto transferStartResult = vehiclePackageManagementService_->TransferStart(name).GetResult();

    if (!transferStartResult) {
        logger_.LogError() << "Error while initializing software package transfer session"
                           << "Error Code:" << transferStartResult.Error();
        return false;
    } else {
        currentSession_ = transferStartResult.Value().id;
        blockSize_ = transferStartResult.Value().blockSize;
        logger_.LogInfo() << "InitSession was successful for software package session" << currentSession_
                          << "with received block size:" << blockSize_;
        return true;
    }
}

template <class VehiclePackageManagementService>
bool DataTransfer<VehiclePackageManagementService>::Transfer(const std::vector<uint8_t>& data)
{
    uint64_t blockCount = CalculateBlockCount(data.size(), blockSize_);

    uint64_t blocksSent = 0;

    while (blocksSent < blockCount) {
        std::vector<uint8_t> dataBlock;
        InitDataBlock(blocksSent * blockSize_, blockSize_, data, dataBlock);
        #if 1
        // IAM Check
        CheckGrant();
        #endif
        auto transferDataResult
            = vehiclePackageManagementService_->TransferData(currentSession_, dataBlock, blocksSent + 1).GetResult();
        if (!transferDataResult) {
            logger_.LogError() << "Error while sending data block number" << blocksSent << "in session"
                               << currentSession_ << ". Error:" << transferDataResult.Error().Message();
            return false;
        }
        ++blocksSent;
    }
    logger_.LogInfo() << "Successful transfer of data of size" << data.size() << "with block count" << blocksSent
                      << "in session" << currentSession_;
    return true;
}

template <class VehiclePackageManagementService>
bool DataTransfer<VehiclePackageManagementService>::ExitSession()
{
    #if 1
    CheckGrant();
    #endif

    auto transferExitResult = vehiclePackageManagementService_->TransferExit(currentSession_).GetResult();

    if (!transferExitResult) {
        logger_.LogError() << "Error while exiting session" << currentSession_
                           << ". Error:" << transferExitResult.Error().Message();
        return false;
    }
    logger_.LogInfo() << "Session exited successfully for session" << currentSession_ << ".";
    return true;
}

template <class VehiclePackageManagementService>
TransferIdType DataTransfer<VehiclePackageManagementService>::GetTransferId() const
{
    return currentSession_;
}

template <class VehiclePackageManagementService>
void DataTransfer<VehiclePackageManagementService>::CheckGrant() {
    // IAM Check
    static apd::iamtest::UcmAccessor ucmAccessor;
    const ara::core::InstanceSpecifier vpmRport(
        "ara/apd/OTA_Client/software_components/OTA/VehiclePackageManagementRPort");
    ucmAccessor.AccessUcm(vpmRport);
}


template class DataTransfer<ara::ucm::proxy::VehiclePackageManagementProxy>;

}  // namespace ota
}  // namespace ucm
}  // namespace apd
