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

#include "vpackage_management_app.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"
#include "ara/core/string.h"

#include <thread>

using ara::com::ServiceHandleContainer;
using ara::core::Future;
using ara::core::String;
using ara::vucm::VehiclePackageManagement;
using ara::vucm::TransferStateType;
using ara::vucm::UCMMasterResolutionType;

namespace apd
{
namespace ucm
{
namespace ota
{

template <class VehiclePackageManagementService>
ara::vucm::UCMMasterResolutionVectorType
VehiclePackageManagementApp<VehiclePackageManagementService>::GetCampaignResolution(const std::uint64_t timestampGE,
    const std::uint64_t timestampLT)
{
    auto out = (service_->GetCampaignHistory(timestampGE, timestampLT).get());
    logger_.LogInfo() << "Campaign history";
    int c{0};
    for (auto campaign : out.campaignHistory) {
        logger_.LogInfo() << "  CampaignResult" << c++;
        int r{0};
        for (auto resolution : campaign.campaignResult.campaignResolution) {
            logger_.LogInfo() << "    Resolution" << r++ << ": " << unsigned(resolution);
        }
        int e{0};
        for (auto stepError : campaign.campaignResult.UCMStepError) {
            logger_.LogInfo() << "    Error" << e++ << ":";
            logger_.LogInfo() << "      UCM Id:" << stepError.ucmId;
            logger_.LogInfo() << "      Step:" << unsigned(stepError.softwarePackageStep);
            logger_.LogInfo() << "      Error:" << stepError.returnedError;
        }
        logger_.LogInfo() << "    Start time:" << campaign.campaignResult.campaignStartTime;
        logger_.LogInfo() << "    Resolution time:" << campaign.campaignResult.campaignResolutionTime;
        logger_.LogInfo() << "    Driver notified:" << campaign.campaignResult.driverNotified;
        logger_.LogInfo() << "    URI: " << campaign.repository;
        int u{0};
        for (auto ucmSub : campaign.historyVector) {
            logger_.LogDebug() << "    UCM Id: " << ucmSub.ucmId << "(#" << u++ << ")";
            int a{0};
            for (auto ucmHistory : ucmSub.historyVector) {
                logger_.LogDebug() << "      Entry" << a++;
                logger_.LogDebug() << "        Time" << ucmHistory.time;
                logger_.LogDebug() << "        Name" << ucmHistory.name;
                logger_.LogDebug() << "        Version" << ucmHistory.version;
                logger_.LogDebug() << "        Action" << unsigned(ucmHistory.action);
                logger_.LogDebug() << "        Resolution" << unsigned(ucmHistory.resolution);
                logger_.LogDebug() << "        FailureError" << ucmHistory.failureError;
            }
        }
    }
    return {out.campaignHistory[0].campaignResult.campaignResolution};
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::GetRequiredSwInfo(
    ara::ucm::SwClusterInfoVectorType& SwInfo)
{
    SwInfo = (service_->GetSwClusterInfo().get()).swInfo;
    // Return value should be false
    // when it couldn't get information of installed Software Clusters
    return true;
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::IsCorrectStatus(
    ara::vucm::TransferStateType askedStatus)
{
    while (CurrentStatusToString(currentStatus_).size() == 0) {
        logger_.LogInfo() << "Waiting for status field.";
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
    }

    if (currentStatus_ != askedStatus) {
        logger_.LogError() << "Current status" << CurrentStatusToString(currentStatus_) << "incorrect; should be"
                           << CurrentStatusToString(askedStatus);
        return false;
    }
    return true;
}

template <class VehiclePackageManagementService>
void VehiclePackageManagementApp<VehiclePackageManagementService>::OnStatusChange()
{
    std::unique_lock<std::mutex> lck(transferMutex_);
    service_->TransferState.GetNewSamples(
        [this](auto sample) {
            this->currentStatus_ = *sample;

            if (CurrentStatusToString(currentStatus_).size() > 0) {
                logger_.LogInfo() << "Transfer state transition notification :"
                                  << CurrentStatusToString(currentStatus_);
                statusBuffer_.push_back(CurrentStatusToString(currentStatus_));
            } else {
                logger_.LogError() << "Empty status field.";
            }
        },
        1);
    lck.unlock();
    campaignCond_.notify_one();
}

template <class VehiclePackageManagementService>
ara::core::Vector<ara::core::String> VehiclePackageManagementApp<VehiclePackageManagementService>::GetStatusBuffer()
{
    std::unique_lock<std::mutex> lck(transferMutex_);
    if (statusBuffer_.empty()) {
        logger_.LogInfo() << "Status buffer empty.";
        return {};
    }
    return statusBuffer_;
}

template <class VehiclePackageManagementService>
void VehiclePackageManagementApp<VehiclePackageManagementService>::FlushStatusBuffer()
{
    if (statusBuffer_.empty()) {
        logger_.LogInfo() << "Status buffer empty.";
        return;
    }
    statusBuffer_.clear();
}

template <class VehiclePackageManagementService>
std::shared_ptr<VehiclePackageManagementService>
VehiclePackageManagementApp<VehiclePackageManagementService>::GetService()
{
    return service_;
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::UpdateStatus()
{
    if (service_->TransferState.IsSubscribed()) {
        logger_.LogError() << "TransferState field callback already subscribed";
        return false;
    }

    // Register event receive callback
    service_->TransferState.SetReceiveHandler(
        [this]() { VehiclePackageManagementApp<VehiclePackageManagementService>::OnStatusChange(); });

    // Subscribe to field
    const auto subscription_result = service_->TransferState.Subscribe(1);
    if (subscription_result.HasValue()) {
        logger_.LogInfo() << "Callback registered.";
    } else {
        logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
        return false;
    }

    // Initial update of service.
    // This is necessary in case application is being re-attached to the service
    // after a crash.
    auto state = service_->TransferState.Get();
    this->currentStatus_ = state.get();

    // service_ got initialized via callback.
    logger_.LogInfo() << "Transfer status field callbacks registered.";
    return true;
}

template <class VehiclePackageManagementService>
void VehiclePackageManagementApp<VehiclePackageManagementService>::OnPackageRequested()
{
    std::unique_lock<std::mutex> lck(requestMutex_);
    service_->RequestedPackage.GetNewSamples(
        [this](auto sample) {
            this->requestedPackage_ = *sample;

            if (requestedPackage_.name.size() > 0) {
                logger_.LogInfo() << "Requested Package full name:" << requestedPackage_.name
                                  << " Version: " << requestedPackage_.version;
                std::size_t last_path_separator_position(requestedPackage_.name.find_last_of("/"));
                if (last_path_separator_position != ara::core::String::npos) {
                    requestedPackage_.name
                        = requestedPackage_.name.substr(last_path_separator_position + 1, ara::core::String::npos);
                }
                requestedPackageList_.push_back(requestedPackage_);
            } else {
                logger_.LogError() << "Empty RequestedPackage field.";
            }
        },
        1);
    lck.unlock();
    requestCond_.notify_one();
}

template <class VehiclePackageManagementService>
ara::vucm::SwNameVersionVectorType VehiclePackageManagementApp<VehiclePackageManagementService>::GetRequestedPackages()
{
    std::unique_lock<std::mutex> lck(requestMutex_);
    if (requestedPackageList_.empty()) {
        logger_.LogInfo() << "Requested packages is not available yet, waiting for it...";
        // use timed wait to reenter main thread
        bool timedout
            = requestCond_.wait_for(lck, std::chrono::seconds(1), [this] { return !requestedPackageList_.empty(); });
        logger_.LogInfo() << "Requested packages wait timeout " << timedout;
    } else {
        logger_.LogInfo() << "Requested packages already has" << requestedPackageList_.size() << "items.";
    }
    auto copy = requestedPackageList_;
    requestedPackageList_.clear();
    return copy;
}

template <class VehiclePackageManagementService>
void VehiclePackageManagementApp<VehiclePackageManagementService>::FlushRequestedPackageList()
{
    std::unique_lock<std::mutex> lck(requestMutex_);
    if (requestedPackageList_.empty()) {
        logger_.LogInfo() << "Requested package list empty.";
        return;
    }
    requestedPackageList_.clear();
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::UpdateRequestedPackage()
{
    if (service_->RequestedPackage.IsSubscribed()) {
        logger_.LogError() << "RequestedPackage field callback already subscribed";
        return false;
    }

    // Register event receive callback
    service_->RequestedPackage.SetReceiveHandler(
        [this]() { VehiclePackageManagementApp<VehiclePackageManagementService>::OnPackageRequested(); });

    // Subscribe to field
    const auto subscription_result = service_->RequestedPackage.Subscribe(1);
    if (subscription_result.HasValue()) {
        logger_.LogInfo() << "Callback registered.";
    } else {
        logger_.LogError() << "Subscription failed with error: " << subscription_result.Error();
        return false;
    }

    // Initial update of service.
    // This is necessary in case application is being re-attached to the service
    // after a crash.
    auto package = service_->RequestedPackage.Get();
    this->requestedPackage_ = package.get();

    // service_ got initialized via callback.
    logger_.LogInfo() << "Requested package field callback registered.";
    return true;
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::TransferVehiclePackage(
    const ara::core::String& packagePath,
    DataTransfer<VehiclePackageManagementService>& dataTransfer)
{
    ara::core::Vector<uint8_t> buffer;
    const bool found = LoadBinaryFile(packagePath, buffer);
    if (!found) {
        logger_.LogError() << "Transfer failed. Vehicle Package not found at path" << packagePath;
        return false;
    }
    requestedPackageList_.clear();

    if (!dataTransfer.InitSession(buffer.size())) {
        logger_.LogError() << "Initialization of transfer with size " + std::to_string(buffer.size()) + " failed";
        return false;
    }

    lastTransferId_ = dataTransfer.GetTransferId();

    if (!dataTransfer.Transfer(buffer)) {
        logger_.LogError() << "Transfer of Vehicle package associated with TransferId" << lastTransferId_ << "failed";
        return false;
    }

    if (!dataTransfer.ExitSession()) {
        logger_.LogError() << "Exiting Vehicle package transfer associated with TransferId" << lastTransferId_
                           << "failed";
        return false;
    }
    logger_.LogInfo() << "Transfer of Vehicle Package was successful for session" << lastTransferId_;
    return true;
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::TransferSoftwarePackage(
    const ara::core::String& testDataLocation,
    const ara::core::String& swPackage,
    const ara::ucm::StrongRevisionLabelString& swPackageVersion,
    DataTransfer<VehiclePackageManagementService>& dataTransfer)
{
    ara::core::Vector<uint8_t> buffer;
    const ara::core::String path = testDataLocation + "/" + swPackage + "_" + swPackageVersion + ".zip";
    const bool found = LoadBinaryFile(path, buffer);
    if (!found) {
        logger_.LogError() << "Transfer failed. Software Package not found at path " << path;
        return false;
    }

    if (!dataTransfer.InitSession(swPackage)) {
        logger_.LogError() << "Initialization of Software Package transfer with name" << swPackage << "failed";
        return false;
    }

    lastTransferId_ = dataTransfer.GetTransferId();

    if (!dataTransfer.Transfer(buffer)) {
        logger_.LogError() << "Transfer of Software package associated with TransferId" << lastTransferId_ << "failed";
        return false;
    }

    if (!dataTransfer.ExitSession()) {
        logger_.LogError() << "Exiting Software package transfer associated with TransferId" << lastTransferId_
                           << "failed";
        return false;
    }
    logger_.LogInfo() << "Transfer of Software Package was successful for session" << lastTransferId_;
    return true;
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::TransferVehiclePackage(
    const ara::core::String& packagePath)
{
    DataTransfer<VehiclePackageManagementService> dataTransfer(service_);
    return this->TransferVehiclePackage(packagePath, dataTransfer);
}

template <class VehiclePackageManagementService>
bool VehiclePackageManagementApp<VehiclePackageManagementService>::TransferSoftwarePackage(
    const ara::core::String& testDataLocation,
    const ara::core::String& swPackage,
    const ara::ucm::StrongRevisionLabelString& swPackageVersion)
{
    DataTransfer<VehiclePackageManagementService> dataTransfer(service_);
    return this->TransferSoftwarePackage(testDataLocation, swPackage, swPackageVersion, dataTransfer);
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

const ara::core::String CurrentStatusToString(ara::vucm::TransferStateType status)
{
    switch (status) {
    case ara::vucm::TransferStateType::kIdle:
        return "Idle";

    case ara::vucm::TransferStateType::kSyncing:
        return "Syncing";

    case ara::vucm::TransferStateType::kTransferring:
        return "Transferring";

    case ara::vucm::TransferStateType::kUpdating:
        return "Updating";

    case ara::vucm::TransferStateType::kCancelling:
        return "Cancelling";

    default:
        return "Unknown";
    }
}

template class VehiclePackageManagementApp<ara::vucm::proxy::VehiclePackageManagementProxy>;
}  // namespace ota
}  // namespace ucm
}  // namespace apd
