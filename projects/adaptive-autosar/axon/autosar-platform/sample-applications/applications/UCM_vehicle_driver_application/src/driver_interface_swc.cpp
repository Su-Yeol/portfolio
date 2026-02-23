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

#include "driver_interface_swc.h"
#include "rest_payload_helpers.h"

using namespace apd::rest;

DriverInterfaceSwc::DriverInterfaceSwc(std::shared_ptr<ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy>& proxy)
    : proxy_(proxy)
{
    restServer_ = std::make_unique<apd::rest::Server>("DriverIfRestServer", CreateRouter());
    restServer_->Start(apd::rest::StartupPolicy::kDetached);

    EstablishSubscriptions();
}

DriverInterfaceSwc::~DriverInterfaceSwc()
{
    restServer_->Stop(apd::rest::ShutdownPolicy::kGraceful);
}

apd::rest::Router DriverInterfaceSwc::CreateRouter()
{
    ara::core::String p("/api/arasoftwareupdate");
    Pattern softwareUpdateResourcePattern(p.c_str());

    Route getRoute{RequestMethod::kGet,
        softwareUpdateResourcePattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return GetSoftwareUpdateHandler(route, request, reply);
        }};

    ara::core::String approveStatusPatternStr("/api/arasoftwareupdate/approvestatus/*");
    Pattern approveStatusResourcePattern(approveStatusPatternStr.c_str());

    Route postRoute{RequestMethod::kPost,
        approveStatusResourcePattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return PostApprovalStatusHandler(route, request, reply);
        }};

    ara::core::String progressInformation("/api/arasoftwareupdate/progress");
    Pattern progressInformationPattern(progressInformation.c_str());

    Route getprogressRoute{RequestMethod::kGet,
        progressInformationPattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return GetprogressInformationHandler(route, request, reply);
        }};

    ara::core::String packageInformation("/api/arasoftwareupdate/package");
    Pattern packageInformationPattern(packageInformation.c_str());

    Route getpackageRoute{RequestMethod::kGet,
        packageInformationPattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return GetpackageInformationHandler(route, request, reply);
        }};

    ara::core::String historyInformation("/api/arasoftwareupdate/history");
    Pattern campaignHistoryPattern(historyInformation.c_str());

    Route gethistoryRoute{RequestMethod::kGet,
        campaignHistoryPattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return GetCampaignHistoryHandler(route, request, reply);
        }};

    return Router{getRoute, postRoute, getprogressRoute, getpackageRoute, gethistoryRoute};
}

void DriverInterfaceSwc::RequestLoopThreadFunction()
{
    const auto continueExecution = 1;

    while (continueExecution) {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        logger_.LogInfo() << "VDI worker Thread";

        if (IsCorrectCampaignState(ara::vucm::CampaignStateType::kIdle)) {
            RequestSwClusterDescriptions();
            RequestCampignHistory();
            // SW Package Descritpion is valid only when there is an active campaign
            // Adding this workaround in VDI to clear the info.
            {
                std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
                packageInfo_.swPackageDescVec.clear();
            }
        } else {
            RequestSwPackageDescriptions();
        }
        RequestProgressInformation();
        RequestTransferInformation();
    }
}

bool DriverInterfaceSwc::StartMyrequestLoopThread()
{
    requestLoopThread = std::thread([this]() { this->RequestLoopThreadFunction(); });
    return true;
}

Route::Upshot DriverInterfaceSwc::GetSoftwareUpdateHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    static_cast<void>(route);
    static_cast<void>(request);
    logger_.LogInfo() << "Get request has been received";
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        reply.Send(AraSoftwareUpdateToOgm(updateInfo_, progressInfo_, packageInfo_));
    }
    ara_software_update_cond_.notify_one();
    return Route::Upshot::Accept;
}

Route::Upshot DriverInterfaceSwc::PostApprovalStatusHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    static_cast<void>(route);
    static_cast<void>(reply);
    auto approval = GetVehicleDriverApproval(request);

    if (approval) {
        loggerDriverApproval_.LogInfo() << "Driver Approval Received: " << ((*approval) ? "approved" : "cancelled");
        proxy_->DriverApproval(*approval);
    }
    reply.Send(apd::rest::ogm::Object::Make());
    return Route::Upshot::Accept;
}

Route::Upshot DriverInterfaceSwc::GetprogressInformationHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    static_cast<void>(route);
    static_cast<void>(request);

    logger_.LogInfo() << "Progress information";
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        reply.Send(progressInformationToOgm(progressInfo_, packageInfo_));
    }
    ara_software_update_cond_.notify_one();

    return Route::Upshot::Accept;
}

Route::Upshot DriverInterfaceSwc::GetpackageInformationHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    static_cast<void>(route);
    static_cast<void>(request);

    logger_.LogInfo() << "Package information from the downloading data";
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        reply.Send(packageInformationToOgm(packageInfo_));
    }
    ara_software_update_cond_.notify_one();

    return Route::Upshot::Accept;
}

Route::Upshot DriverInterfaceSwc::GetCampaignHistoryHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    static_cast<void>(route);
    static_cast<void>(request);

    logger_.LogInfo() << "Campaign History";
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        reply.Send(historyInformationToOgm(historyInfo_));
    }
    ara_software_update_cond_.notify_one();

    return Route::Upshot::Accept;
}

bool DriverInterfaceSwc::RespondApprovalRequest(bool driverResponse)
{
    const auto result = proxy_->DriverApproval(driverResponse).GetResult();
    if (result.HasValue()) {
        logger_.LogInfo() << "Driver response has been acknowledged";
        return true;
    }
    return false;
}

bool DriverInterfaceSwc::RequestSwPackageDescriptions()
{
    using namespace std::string_literals;

    auto swPackageDescVec = proxy_->GetSwPackageDescription().get().packages;
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        packageInfo_.swPackageDescVec.clear();
        packageInfo_.swPackageDescVec = swPackageDescVec;
    }
    ara_software_update_cond_.notify_one();

    if (packageInfo_.swPackageDescVec.empty()) {
        logger_.LogWarn() << "No SwPackages";
        return false;
    }
    return true;
}

bool DriverInterfaceSwc::RequestVehiclePackageDescriptions()
{
    auto vehPackageDecVec = proxy_->GetVehiclePackageDescription().get().vehiclePackageDescription;
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        packageInfo_.vehPackageDecVec = vehPackageDecVec;
    }
    ara_software_update_cond_.notify_one();

    logger_.LogDebug() << "Estimate Duration of Campaign: " << vehPackageDecVec.estimatedDurationOfCampaign;
    logger_.LogDebug() << "            "
                       << "Purpose: " << vehPackageDecVec.purposeOfUpdate;

    return true;
}

bool DriverInterfaceSwc::RequestSwClusterDescriptions()
{
    auto swClusterInfoVec = proxy_->GetSwClusterInfo().get().swInfo;
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        updateInfo_.swClusterInfoVec.clear();
        updateInfo_.swClusterInfoVec = swClusterInfoVec;
    }
    ara_software_update_cond_.notify_one();

    if (updateInfo_.swClusterInfoVec.empty()) {
        logger_.LogWarn() << "No SwClusters installed";
        return false;
    }

    return true;
}

bool DriverInterfaceSwc::RequestProgressInformation()
{
    const auto processProgress = proxy_->GetSwProcessProgress().get().progress;
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        progressInfo_.processProgress = processProgress;
    }
    ara_software_update_cond_.notify_one();

    if (processProgress == 255) {
        logger_.LogInfo() << "No SW Progress information available";
        return true;
    }

    logger_.LogInfo() << "Progress of campaign processing:" << processProgress;
    return true;
}

bool DriverInterfaceSwc::RequestTransferInformation()
{
    const auto transferProgress = proxy_->GetSwTransferToUCMMasterProgress().get().progress;
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        progressInfo_.transferProgress = transferProgress;
    }
    ara_software_update_cond_.notify_one();

    if (transferProgress == 255) {
        logger_.LogInfo() << "No Transfer Progress Information available";
        return true;
    }

    logger_.LogInfo() << "Progress of campaign transferring:" << transferProgress;
    return true;
}

bool DriverInterfaceSwc::RequestCampignHistory()
{

    // ara::vucm::CampaignHistoryVectorType campaignHistory;
    //  timestampGE – Earliest timestamp (inclusive).
    //  timestampLT – Latest timestamp (exclusive)

    std::uint64_t timestampGE = 0;
    std::uint64_t timestampLT
        = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
              .count();

    const auto out = proxy_->GetCampaignHistory(timestampGE, timestampLT).get();
    {
        std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
        historyInfo_.campaignHistoryInfo_ = out;
        historyInfo_.timestampGE = timestampGE;
        historyInfo_.timestampLT = timestampLT;
    }
    ara_software_update_cond_.notify_one();

    // ara::vucm::VehicleDriverApplicationInterface::GetCampaignHistoryOutput

    logger_.LogDebug() << "Campaign history";
    int c{0};
    for (auto campaign : out.history) {
        logger_.LogDebug() << "  CampaignResult" << c++;
        int r{0};
        for (auto resolution : campaign.campaignResult.campaignResolution) {
            logger_.LogDebug() << "    Resolution" << r++ << ": " << unsigned(resolution);
        }
        int e{0};
        for (auto stepError : campaign.campaignResult.UCMStepError) {
            logger_.LogDebug() << "    Error" << e++ << ":";
            logger_.LogDebug() << "      UCM Id:" << stepError.ucmId;
            logger_.LogDebug() << "      Step:" << unsigned(stepError.softwarePackageStep);
            logger_.LogDebug() << "      Error:" << stepError.returnedError;
        }
        logger_.LogDebug() << "    Start time:" << campaign.campaignResult.campaignStartTime;
        logger_.LogDebug() << "    Resolution time:" << campaign.campaignResult.campaignResolutionTime;
        logger_.LogDebug() << "    Driver notified:" << campaign.campaignResult.driverNotified;
        logger_.LogDebug() << "    URI: " << campaign.repository;
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

    return true;
}

ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy& DriverInterfaceSwc::GetAppProxy()
{
    return *proxy_;
}

bool DriverInterfaceSwc::GetApprovalRequired() const
{
    logger_.LogInfo() << __func__ << packageInfo_.approvalRequired_;
    return packageInfo_.approvalRequired_;
}

void DriverInterfaceSwc::SetApprovalRequired(bool approvalRequired)
{
    logger_.LogDebug() << __func__ << approvalRequired;
    packageInfo_.approvalRequired_ = approvalRequired;
}

bool DriverInterfaceSwc::IsCorrectCampaignState(const ara::vucm::CampaignStateType askedStatus) const
{
    if (progressInfo_.campaignState_ != askedStatus) {
        logger_.LogDebug() << "Current status: " << CampaignStateToString(progressInfo_.campaignState_)
                           << ", NOT: " << CampaignStateToString(askedStatus);
        return false;
    }
    return true;
}

void DriverInterfaceSwc::OnApprovalRequiredChanged()
{
    proxy_->ApprovalRequired.GetNewSamples(
        [this](auto sample) {
            logger_.LogInfo() << "ApprovalRequired transition notification :" << *sample;
            {
                std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
                packageInfo_.approvalRequired_ = *sample;
            }
            ara_software_update_cond_.notify_one();
        },
        1);
}

void DriverInterfaceSwc::OnCampaignStatusChanged()
{
    proxy_->CampaignState.GetNewSamples(
        [this](auto sample) {
            const auto campaignStateString = CampaignStateToString(*sample);
            if (campaignStateString.size() > 0) {
                logger_.LogInfo() << "Campaign state transition notification :" << campaignStateString;
            } else {
                logger_.LogError() << "Empty status field.";
                return;
            }

            {
                std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
                progressInfo_.campaignState_ = *sample;
                // campaignStateUpdated = true;
            }
            ara_software_update_cond_.notify_one();
        },
        1);
}

void DriverInterfaceSwc::EstablishSubscriptions()
{
    // initial update
    {
        std::lock_guard<std::mutex> guard(ara_software_update_mutex_);
        packageInfo_.vehicleConditions_ = proxy_->VehicleConditionCollection.Get().get();
        progressInfo_.campaignState_ = proxy_->CampaignState.Get().get();
        packageInfo_.approvalRequired_ = proxy_->ApprovalRequired.Get().get();
        updateInfo_.approvalId_ = 0;
    }

    RequestVehiclePackageDescriptions();

    proxy_->ApprovalRequired.SetReceiveHandler([this]() {
        logger_.LogInfo() << "Approval Required Receive Handler";
        OnApprovalRequiredChanged();
    });

    proxy_->CampaignState.SetReceiveHandler([this]() {
        logger_.LogInfo() << "Campaign State Receive Handler";
        OnCampaignStatusChanged();
    });

    proxy_->VehicleConditionCollection.SetReceiveHandler([this] {
        logger_.LogInfo() << "Safety Conditions Receive Handler";
        const auto result = proxy_->VehicleConditionCollection.GetNewSamples([this](auto sample) {
            std::lock_guard<std::mutex> lck(ara_software_update_mutex_);
            packageInfo_.vehicleConditions_ = *sample;
        });

        if (!result) {
            logger_.LogError() << "Could not retrieve the new vehicle conditions";
        }
        ara::vucm::VehicleConditionCollectionType unsupportedSafetyConditions;
        ara::vucm::VehicleConditionCollectionType::const_iterator vehicleConditionsIt;

        for (vehicleConditionsIt = packageInfo_.vehicleConditions_.begin();
             vehicleConditionsIt != packageInfo_.vehicleConditions_.end();
             ++vehicleConditionsIt) {
            if (vehicleConditionsIt->state == ara::vucm::SafetyStateType::NotSupported) {
                unsupportedSafetyConditions.push_back(
                    packageInfo_.vehicleConditions_[vehicleConditionsIt - packageInfo_.vehicleConditions_.begin()]);
            }
        }

        if (!unsupportedSafetyConditions.empty()) {
            proxy_->ReportUnsupportedSafetyConditions(unsupportedSafetyConditions);
            logger_.LogInfo() << "Sent unsupported safety conditions!";
        }
    });

    proxy_->CampaignState.SetSubscriptionStateChangeHandler([&](ara::com::SubscriptionState substate) {
        switch (substate) {
        case ara::com::SubscriptionState::kNotSubscribed: {
            logger_.LogInfo() << "Campaign state field is not subscribed";
            break;
        }
        case ara::com::SubscriptionState::kSubscriptionPending: {
            logger_.LogInfo() << "Campaign state field subscription pending...";
            break;
        }
        case ara::com::SubscriptionState::kSubscribed: {
            logger_.LogInfo() << "Campaign state field subscribed!";
            break;
        }

        default:
            break;
        }
    });

    proxy_->ApprovalRequired.SetSubscriptionStateChangeHandler([&](ara::com::SubscriptionState substate) {
        switch (substate) {
        case ara::com::SubscriptionState::kNotSubscribed: {
            logger_.LogInfo() << "Approval required field is not subscribed";
            break;
        }
        case ara::com::SubscriptionState::kSubscriptionPending: {
            logger_.LogInfo() << "Approval required field subscription pending...";
            break;
        }
        case ara::com::SubscriptionState::kSubscribed: {
            logger_.LogInfo() << "Approval required field subscribed!";
            break;
        }

        default:
            break;
        }
    });

    proxy_->VehicleConditionCollection.SetSubscriptionStateChangeHandler([&](ara::com::SubscriptionState substate) {
        switch (substate) {
        case ara::com::SubscriptionState::kNotSubscribed: {
            logger_.LogInfo() << "Vehicle conditions field is not subscribed";
            break;
        }
        case ara::com::SubscriptionState::kSubscriptionPending: {
            logger_.LogInfo() << "Vehicle conditions field subscription pending...";
            break;
        }
        case ara::com::SubscriptionState::kSubscribed: {
            logger_.LogInfo() << "Vehicle conditions field subscribed!";
            break;
        }

        default:
            break;
        }
    });

    proxy_->VehicleConditionCollection.Subscribe(1);
    proxy_->ApprovalRequired.Subscribe(1);
    proxy_->CampaignState.Subscribe(1);
}
