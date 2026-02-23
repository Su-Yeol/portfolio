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

#include "package_management_app.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"
#include "ara/core/string.h"

#include <condition_variable>

using ara::com::ServiceHandleContainer;
using ara::core::Future;
using ara::core::String;
using ara::ucm::PackageManagement;
using ara::ucm::TransferIdType;
using ara::ucm::PackageManagementStatusType;
using GetSwPackagesOutput = ara::ucm::PackageManagement::GetSwPackagesOutput;
namespace apd
{
namespace ucm
{
namespace pkgmgrsample
{

namespace
{
void LogErrorCode(const int errorCode, const String& msg, ara::log::Logger& logger_)
{

    logger_.LogError() << msg << "Error code:" << errorCode;
}
}  // namespace

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Init(std::shared_ptr<PackageManagementService> service)
{
    service_ = service;

    if (service_) {
        return true;
    } else {
        logger_.LogError()
            << "FATAL: Expected the service, but it is not provided";  // this check is not needed, it checks the sample
                                                                       // application itself
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::IsCorrectStatus(PackageManagementStatusType askedStatus)
{
    if (currentStatus_ != askedStatus) {
        return false;
    }
    return true;
}

template <class PackageManagementService>
void PackageManagementApp<PackageManagementService>::OnStatusChange()
{
    std::unique_lock<std::mutex> lck(globalMutex);

    service_->CurrentStatus.GetNewSamples(
        [this](auto sample) {
            this->currentStatus_ = *sample;
            logger_.LogInfo() << "[service status update] changed to" << CurrentStatusToString(currentStatus_);
        },
        1);
    lck.unlock();
    this->cond.notify_one();
}

template <class PackageManagementService>
void PackageManagementApp<PackageManagementService>::Unsubscribe()
{
    if (service_->CurrentStatus.IsSubscribed()) {
        service_->CurrentStatus.Unsubscribe();
        service_->CurrentStatus.UnsetReceiveHandler();
        logger_.LogInfo() << "Unsubscribed status field callback";
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::UpdateStatus()
{
    if (service_->CurrentStatus.IsSubscribed()) {
        logger_.LogError()
            << "[Connect] Is already subscribed to currentStatus of PackageManagement when tried to subscribe to it.";
        return false;
    }

    // Register event receive callback
    service_->CurrentStatus.SetReceiveHandler(
        [this]() { PackageManagementApp<PackageManagementService>::OnStatusChange(); });

    // Subscribe to field
    const auto subscription_result = service_->CurrentStatus.Subscribe(1);
    if (subscription_result.HasValue()) {
        logger_.LogInfo() << "Callback registered.";
    } else {
        logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
        return false;
    }

    // Initial update of service.
    // This is necessary in case application is being re-attached to the service
    // after a crash.
    auto state = service_->CurrentStatus.Get();
    this->currentStatus_ = state.get();

    // service_ got initialized via callback.
    logger_.LogInfo() << "[Connect] subscribed to the status of the PackageManagement instance"
                      << service_->GetId().GetResult().Value().id;
    return true;
}

template <class PackageManagementService>
TransferReturn PackageManagementApp<PackageManagementService>::TransferSoftwarePackage(
    const ara::core::String& packagePath,
    DataTransfer<PackageManagementService>& dataTransfer)
{
    ara::core::Vector<uint8_t> buffer;
    ara::core::Optional<ara::core::ErrorCode> errorCode;
    const bool found = LoadBinaryFile(packagePath, buffer);
    if (!found) {
        logger_.LogError() << "[Transfer] software package not found:" << packagePath;
        return std::make_tuple(false, errorCode);
    }

    logger_.LogInfo() << "[Transfer] transferring" << packagePath;

    if (!dataTransfer.InitSession(buffer.size())) {
        errorCode = dataTransfer.GetTransferError();
        return std::make_tuple(false, errorCode);
    }

    lastTransferId_ = dataTransfer.GetTransferId();

    if (!dataTransfer.Transfer(buffer)) {
        errorCode = dataTransfer.GetTransferError();
        return std::make_tuple(false, errorCode);
    }

    if (!dataTransfer.ExitSession()) {
        errorCode = dataTransfer.GetTransferError();
        return std::make_tuple(false, errorCode);
    }
    return std::make_tuple(true, errorCode);
    ;
}

template <class PackageManagementService>
TransferReturn PackageManagementApp<PackageManagementService>::TransferSoftwarePackage(
    const ara::core::String& packagePath)
{
    DataTransfer<PackageManagementService> dataTransfer(service_);
    return this->TransferSoftwarePackage(packagePath, dataTransfer);
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::DeleteTransferredSoftwarePackages()
{
    DataTransfer<PackageManagementService> dataTransfer(service_);
    GetSwPackagesOutput getSwPackageResult = dataTransfer.GetSwPackages();

    ara::ucm::TransferIdType id;
    for (auto entry : getSwPackageResult.packages) {
        id = entry.transferId;
        const bool deleteTransferResult = dataTransfer.DeleteSwPackage(id);

        if (!deleteTransferResult) {
            LogErrorCode(static_cast<int>(deleteTransferResult), "Deleting transfer failed", logger_);
            logger_.LogWarn() << "[Delete] failed for transfer id" << id;
        }
    }

    getSwPackageResult = dataTransfer.GetSwPackages();
    if (getSwPackageResult.packages.size() != 0) {
        logger_.LogInfo() << "[Delete] failed. Remaining packages:"
                          << static_cast<int>(getSwPackageResult.packages.size());
        return false;
    }
    return true;
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Install()
{
    logger_.LogError() << "[ProcessSwPackage] for transfer id" << lastTransferId_;

    auto future = service_->ProcessSwPackage(lastTransferId_);
    future.wait();
    const auto result = future.GetResult();

    if (result.HasValue()) {
        std::unique_lock<std::mutex> lck(globalMutex);
        cond.wait_for(lck, std::chrono::seconds(10), [&] {
            return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kReady);
        });
        lck.unlock();
        cond.notify_one();
        return IsCorrectStatus(ara::ucm::PackageManagementStatusType::kReady);
    } else {
        logger_.LogError() << "[ProcessSwPackage] failed:" << result.Error();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Activate()
{
    logger_.LogError() << "[Activate] triggered";

    auto future = service_->Activate();
    const auto result = future.GetResult();
    if (result.HasValue()) {
        std::unique_lock<std::mutex> lck(globalMutex);
        cond.wait_for(lck, std::chrono::seconds(20), [&] {
            return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kActivated);
        });
        lck.unlock();
        cond.notify_one();
        return IsCorrectStatus(ara::ucm::PackageManagementStatusType::kActivated);
    } else {
        logger_.LogError() << "[Activate] failed with error:" << result.Error();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Finish()
{
    logger_.LogError() << "[Finish] triggered";

    auto future = service_->Finish();
    future.wait();
    const auto result = future.GetResult();
    if (result.HasValue()) {
        std::unique_lock<std::mutex> lck(globalMutex);
        cond.wait_for(lck, std::chrono::seconds(10), [&] {
            return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kIdle);
        });
        lck.unlock();
        cond.notify_one();
        return IsCorrectStatus(ara::ucm::PackageManagementStatusType::kIdle);
    } else {
        logger_.LogError() << "[Finish] failed:" << result.Error().Message();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Rollback()
{
    logger_.LogError() << "[Rollback] triggered";

    auto future = service_->Rollback();
    const auto result = future.GetResult();
    if (result.HasValue()) {
        std::unique_lock<std::mutex> lck(globalMutex);
        cond.wait_for(lck, std::chrono::seconds(10), [&] {
            return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRolledBack);
        });
        lck.unlock();
        cond.notify_one();
        return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRolledBack);
    } else {
        logger_.LogError() << "[Rollback] failed:" << result.Error().Message();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::Cancel(ara::ucm::TransferIdType processingTransferId)
{
    logger_.LogInfo() << "[Cancel] triggered for transfer id" << processingTransferId << "...";

    const auto result = service_->Cancel(processingTransferId).GetResult();
    if (result.HasValue()) {
        logger_.LogInfo() << "[Cancel] succeeded for transfer id" << processingTransferId;
        return true;
    } else {
        logger_.LogError() << "[Cancel] failed for transfer id" << processingTransferId
                           << "with error:" << result.Error();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::RevertProcessedSwPackages()
{
    logger_.LogInfo() << "[RevertProcessedSwPackages] triggered";

    const auto result = service_->RevertProcessedSwPackages().GetResult();
    if (result.HasValue()) {
        logger_.LogInfo() << "[RevertProcessedSwPackages] succeeded";
        return true;
    } else {
        logger_.LogError() << "[RevertProcessedSwPackages] failed:" << result.Error().Message();
        return false;
    }
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::AttemptStateRecovery()
{
    bool result = false;
    // the lock is already owned by the caller
    std::unique_lock<std::mutex> lck(globalMutex, std::adopt_lock);

    switch (currentStatus_) {

    case PackageManagementStatusType::kReady: {
        logger_.LogInfo() << "[State Recovery] trying kReady";
        return this->RevertProcessedSwPackages();
    }

    case PackageManagementStatusType::kProcessing: {
        logger_.LogInfo() << "[State Recovery] trying kProcessing";
        const ara::ucm::TransferIdType processingTransferId = GetProcessingTransferId();
        if (0ull == processingTransferId) {
            return false;
        }
        const bool cancelSucceeded = this->Cancel(processingTransferId);
        if (cancelSucceeded && (this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kReady);
            }))) {
            result = true;
        } else {
            result = false;
        }
    } break;

    case PackageManagementStatusType::kActivating:
        logger_.LogInfo() << "[State Recovery] trying kActivating";
        if (this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kVerifying);
            })) {
            result = this->AttemptStateRecovery();
        } else {
            result = false;
        }
        break;

    case PackageManagementStatusType::kVerifying:
        logger_.LogInfo() << "[State Recovery] trying kVerifying";
        if (this->Rollback() && this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRollingBack);
            })) {
            result = this->AttemptStateRecovery();
        } else {
            result = false;
        }
        break;

    case PackageManagementStatusType::kActivated:
        logger_.LogInfo() << "[State Recovery] trying kActivated";
        if (this->Rollback() && this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRollingBack);
            })) {
            result = this->AttemptStateRecovery();
        } else {
            result = false;
        }
        break;

    case PackageManagementStatusType::kRollingBack:
        logger_.LogInfo() << "[State Recovery] trying kRollingBack";
        if (this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kRolledBack);
            })) {
            result = this->AttemptStateRecovery();
        } else {
            result = false;
        }
        break;

    case PackageManagementStatusType::kRolledBack:
        logger_.LogInfo() << "[State Recovery] trying kRolledBack";
        if (this->Finish() && this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
                return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kCleaningUp);
            })) {
            result = this->AttemptStateRecovery();
        } else {
            result = false;
        }
        break;

    case PackageManagementStatusType::kCleaningUp:
        logger_.LogInfo() << "[State Recovery] trying kCleaningUp";
        result = this->cond.wait_for(lck, std::chrono::milliseconds(5000), [&] {
            return this->IsCorrectStatus(ara::ucm::PackageManagementStatusType::kReady);
        });
        break;

    default:
        result = false;
        break;
    }

    return result;
}

template <class PackageManagementService>
bool PackageManagementApp<PackageManagementService>::AreChangesPending()
{
    Future<PackageManagement::GetSwClusterChangeInfoOutput> futureChangeInfoOutput = service_->GetSwClusterChangeInfo();
    PackageManagement::GetSwClusterChangeInfoOutput changeInfoOutput = futureChangeInfoOutput.get();
    return !(changeInfoOutput.swInfo.empty());
}

template <class PackageManagementService>
ara::ucm::TransferIdType PackageManagementApp<PackageManagementService>::GetProcessingTransferId()
{
    Future<PackageManagement::GetSwPackagesOutput> futureGetPackagesOutput = service_->GetSwPackages();
    PackageManagement::GetSwPackagesOutput GetPackagesOutput = futureGetPackagesOutput.get();

    // In case no packages are being processed, then return an error.
    // ToDo Error to be implemented with the implementation of error domains
    ara::ucm::TransferIdType activeTransferId = 0;

    auto pIt = std::find_if(GetPackagesOutput.packages.cbegin(),
        GetPackagesOutput.packages.cend(),
        [](const ara::ucm::SwPackageInfoType& package) {
            return package.state == ara::ucm::SwPackageStateType::kProcessing;
        });
    if (pIt != GetPackagesOutput.packages.end()) {
        activeTransferId = pIt->transferId;
    }

    return activeTransferId;
}

bool LoadBinaryFile(const ara::core::String& filePath, ara::core::Vector<uint8_t>& binaryPackage)
{
    bool fileFound = false;
    std::ifstream ifs;
    ifs.open(filePath.c_str(), std::ios::binary | std::ios::in);
    if (ifs.is_open()) {
        char c;
        while (ifs.get(c)) {
            binaryPackage.push_back(c);
        }

        fileFound = true;
    }
    ifs.close();

    return fileFound;
}

const ara::core::String CurrentStatusToString(PackageManagementStatusType status)
{
    switch (status) {
    case PackageManagementStatusType::kIdle:
        return "Idle";

    case PackageManagementStatusType::kProcessing:
        return "Processing";

    case PackageManagementStatusType::kReady:
        return "Ready";

    case PackageManagementStatusType::kActivating:
        return "Activating";

    case PackageManagementStatusType::kVerifying:
        return "Verifying";

    case PackageManagementStatusType::kActivated:
        return "Activated";

    case PackageManagementStatusType::kRollingBack:
        return "Rolling Back";

    case PackageManagementStatusType::kRolledBack:
        return "Rolled Back";

    case PackageManagementStatusType::kCleaningUp:
        return "Cleaning Up";

    default:
        return "Unknown";
    }
}

template class PackageManagementApp<ara::ucm::proxy::PackageManagementProxy>;
}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd
