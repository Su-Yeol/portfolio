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

#include "vsm_interface_swc.h"
#include "rest_payload_helpers.h"

#include <fstream>

using namespace apd::rest;
constexpr static const char* const kNoSafetyConditionsTag = "NONE";

static ara::core::String CampaignStateType2String(const ara::vucm::CampaignStateType campaignState)
{
    switch (campaignState) {
    case ara::vucm::CampaignStateType::kIdle: {
        return "kIdle";
    }
    case ara::vucm::CampaignStateType::kSyncing: {
        return "kSyncing";
    }
    case ara::vucm::CampaignStateType::kVehiclePackageTransferring: {
        return "kVehiclePackageTransferring";
    }
    case ara::vucm::CampaignStateType::kSoftwarePackage_Transferring: {
        return "kSoftwarePackage_Transferring";
    }
    case ara::vucm::CampaignStateType::kProcessing: {
        return "kProcessing";
    }
    case ara::vucm::CampaignStateType::kActivating: {
        return "kActivating";
    }
    case ara::vucm::CampaignStateType::kVehicleChecking: {
        return "kVehicleChecking";
    }
    case ara::vucm::CampaignStateType::kCancelling: {
        return "kCancelling";
    }
    default: {
        return "Unknown campaign state: " + static_cast<std::underlying_type_t<decltype(campaignState)>>(campaignState);
    }
    }
    return ara::core::String{};
}

VsmInterfaceSwc::VsmInterfaceSwc(std::shared_ptr<ara::vucm::proxy::VehicleStateManagerInterfaceProxy>& proxy,
    ara::log::Logger& logger)
    : proxy_(std::move(proxy))
    , logger_{logger}
{
    restServer_ = std::make_unique<apd::rest::Server>("VSMIFRestServer", CreateRouter());
    restServer_->Start(apd::rest::StartupPolicy::kDetached);

    EstablishSubscriptions();
}

VsmInterfaceSwc::~VsmInterfaceSwc()
{
    logger_.LogInfo() << "Stopping REST server...";
    restServer_->Stop(apd::rest::ShutdownPolicy::kGraceful);

    logger_.LogInfo() << "Unsubscribing from campaign state field...";
    proxy_->CampaignState.UnsetReceiveHandler();
    proxy_->CampaignState.UnsetSubscriptionStateChangeHandler();
    proxy_->CampaignState.Unsubscribe();

    logger_.LogInfo() << "Unsubscribing from safety conditions field...";
    proxy_->VehicleConditionCollection.UnsetReceiveHandler();
    proxy_->VehicleConditionCollection.UnsetSubscriptionStateChangeHandler();
    proxy_->VehicleConditionCollection.Unsubscribe();
}

void VsmInterfaceSwc::RespondApprovalRequest()
{
    if (!safetyConditions_.empty()) {
        logger_.LogInfo() << "valid safetyConditions received (ITM Mode)";
        ara::vucm::VehicleConditionCollectionType copySafetyConditions;
        copySafetyConditions.clear();
        for (auto condition : safetyConditions_) {
            condition.state = ara::vucm::SafetyStateType::Safe;
            copySafetyConditions.push_back(condition);
        }
        proxy_->PublishSafetyState(copySafetyConditions);
        safetyConditions_.clear();
    }
}

void VsmInterfaceSwc::RespondVehicleCheck()
{
    if (campaignState_ == ara::vucm::CampaignStateType::kVehicleChecking) {
        proxy_->VehicleCheck(true);
        logger_.LogInfo() << "Published the vehicle check resolution";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

ara::vucm::CampaignStateType VsmInterfaceSwc::GetCampaignState()
{
    return campaignState_;
}

#ifdef PICAR

bool VsmInterfaceSwc::StartMyVSMILoopThread()
{
    myVSMILoopThread = std::thread([this]() { this->MyVSMILoopThreadFunction(); });
    return true;
}

void VsmInterfaceSwc::MyVSMILoopThreadFunction()
{
    const int continueExecution = 1;

    const int gpioPIN = 19;
    const int ledPIN = 26;

    static bool parkingModeSet = false;

    ParkingBrake parkingBrakeObj(gpioPIN, ledPIN);

    while (continueExecution) {
        if (false == parkingModeSet) {
            auto brakeStatus = parkingBrakeObj.brakeMonitor();
            if (brakeStatus) {
                std::thread([this]() {
                    std::string smParking = "/usr/sbin/smtool setstate Parking";
                    int returnCode = system(smParking.c_str());
                    if (returnCode) {
                        logger_.LogError() << "returnCode: " << returnCode << ", Erorr while setting to Parking";
                    } else {
                        logger_.LogInfo() << "SMTool sucessfully set to Parking";
                    }
                }).detach();
                parkingModeSet = true;
            }
        } else {
            auto brakeStatus = parkingBrakeObj.brakeMonitor();
            if (brakeStatus) {
                std::thread([this]() {
                    std::string smParking = "/usr/sbin/smtool setstate Driving";
                    int returnCode = system(smParking.c_str());
                    if (returnCode) {
                        logger_.LogError() << "returnCode: " << returnCode << ", Erorr while setting to Driving";
                    } else {
                        logger_.LogInfo() << "SMTool sucessfully set to Driving";
                    }
                }).detach();
                parkingModeSet = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1500));
    }
}

#endif

apd::rest::Router VsmInterfaceSwc::CreateRouter()
{
    std::string vehicleCheckPatternStr(ara::core::String{vsm::rest::kVsmRoute + vsm::rest::kVehicleCheckRelRoute}
                                           .c_str());  //  /api/vsm//vehicle_check
    Pattern vehicleCheckPattern(vehicleCheckPatternStr);

    Route vehcielCheckPostRoute{RequestMethod::kPost,
        vehicleCheckPattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return VehicleCheckHandler(route, request, reply);
        }};

    std::string safetyStatesPatternStr(ara::core::String{vsm::rest::kVsmRoute + vsm::rest::kSafetyStatesRelRoute}
                                           .c_str());  //  /api/vsm//safety_states
    Pattern safetyStatesPattern(safetyStatesPatternStr);

    Route safetyStatesPostRoute{RequestMethod::kPost,
        safetyStatesPattern,
        [this](const Route& route, const ServerRequest& request, ServerReply& reply) -> Route::Upshot {
            return SafetyStatesHandler(route, request, reply);
        }};
    return Router{safetyStatesPostRoute, vehcielCheckPostRoute};
}

apd::rest::Route::Upshot VsmInterfaceSwc::VehicleCheckHandler(const apd::rest::Route&,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply&)
{
    logger_.LogInfo() << "Vehicle check resolution received";
    std::lock_guard<std::mutex> lock{mutexVehicleCheck_};
    ara::core::Optional<bool> check = GetVehicleCheck(request, logger_);
    if (!check.has_value()) {
        logger_.LogError() << "Could not parse the request!";
        return Route::Upshot::Accept;
    }

    if (!proxy_) {
        logger_.LogError() << "Proxy not available!";
        return Route::Upshot::Accept;
    }

    if (campaignState_ != ara::vucm::CampaignStateType::kVehicleChecking) {
        logger_.LogError() << "Failed to send vehicle check resolution, wrong campaign state:"
                           << CampaignStateType2String(campaignState_);
        return Route::Upshot::Accept;
    }

    proxy_->VehicleCheck(*check);
    logger_.LogInfo() << "Published the vehicle check resolution!";
    return Route::Upshot::Accept;
}

apd::rest::Route::Upshot VsmInterfaceSwc::SafetyStatesHandler(const apd::rest::Route&,
    const apd::rest::ServerRequest& request,
    apd::rest::ServerReply& reply)
{
    logger_.LogInfo() << "Safety states received";
    std::lock_guard<std::mutex> lock{mutexSafetyStates_};
    ara::core::Optional<ara::vucm::VehicleConditionCollectionType> safetyStates = GetSafetyStates(request, logger_);
    if (!safetyStates.has_value() || (*safetyStates).empty()) {
        logger_.LogError() << "Could not parse the request!";
        return Route::Upshot::Accept;
    }
    if ((*safetyStates).size() != safetyConditions_.size()) {
        logger_.LogError() << "Safety states do not correspond to the number of required safety conditions!";
        logger_.LogError() << "Computed safety states:" << (*safetyStates).size()
                           << ", Required number of safety conditions:" << safetyConditions_.size();
        return Route::Upshot::Accept;
    }
    for (size_t i = 0; i < safetyConditions_.size(); i++) {
        if ((*safetyStates)[i].condition != safetyConditions_[i].condition) {
            logger_.LogError() << "Provided values do not correspond to the required safety conditions!";
            return Route::Upshot::Accept;
        }
    }
    if (!proxy_) {
        logger_.LogError() << "Proxy not available!";
        return Route::Upshot::Accept;
    }
    proxy_->PublishSafetyState(*safetyStates);
    safetyConditions_.clear();
    logger_.LogInfo() << "Published the safety states!";
    reply.Send(apd::rest::ogm::Object::Make());
    return Route::Upshot::Accept;
}

void VsmInterfaceSwc::EstablishSubscriptions()
{
    // initial update
    {
        std::lock_guard<std::mutex> guard(mutexSafetyConditions_);
        safetyConditions_ = proxy_->VehicleConditionCollection.Get().get();
    }

    {
        std::lock_guard<std::mutex> guard(mutexCampaignState_);
        campaignState_ = proxy_->CampaignState.Get().get();
    }

    proxy_->VehicleConditionCollection.SetReceiveHandler([this] {
        logger_.LogInfo() << "Safety Conditions Receive Handler";
        const auto result = proxy_->VehicleConditionCollection.GetNewSamples([this](auto sample) {
            std::lock_guard<std::mutex> guard(mutexSafetyConditions_);
            ++rolloutStepNumber_;
            logger_.LogInfo() << "rolloutStepNumber_ = " << rolloutStepNumber_;
            safetyConditions_ = *sample;
        });

        if (!result) {
            logger_.LogError() << "Could not retrieve the new safety conditions";
            return;
        }

        if ((safetyConditions_).empty() || safetyConditions_[0].condition == kNoSafetyConditionsTag) {
            logger_.LogInfo() << "No safety conditions required";
            return;
        }

        // for(auto it : safetyConditions_)
        // {
        //     logger_.LogInfo() << "GK: SetReceiveHandler safetyConditions_ " << "condition: "<< it.condition <<
        //     "state: "<< static_cast<int>(it.state);
        // }

        std::lock_guard<std::mutex> guard(mutexSafetyConditions_);
        areSafetyConditionsAvailable_ = true;
    });

    proxy_->CampaignState.SetReceiveHandler([this] {
        logger_.LogInfo() << "Campaign State Handler";
        const auto result = proxy_->CampaignState.GetNewSamples([this](auto sample) {
            logger_.LogInfo() << "Campaign state change:" << CampaignStateType2String(campaignState_) << "->"
                              << CampaignStateType2String(*sample);
            std::lock_guard<std::mutex> guard(mutexCampaignState_);
            campaignState_ = *sample;
        });

        if (!result) {
            logger_.LogError() << "Could not retrieve the new campaign state";
        }

        // if (campaignState_ == ara::vucm::CampaignStateType::kVehicleChecking) {
        //     std::lock_guard<std::mutex> guard(mutexCampaignState_);
        //     isVehCheckResNeeded_ = true;
        // }
    });

    proxy_->VehicleConditionCollection.SetSubscriptionStateChangeHandler([&](ara::com::SubscriptionState substate) {
        switch (substate) {
        case ara::com::SubscriptionState::kNotSubscribed: {
            logger_.LogInfo() << "Safety conditions field is not subscribed";
            break;
        }
        case ara::com::SubscriptionState::kSubscriptionPending: {
            logger_.LogInfo() << "Safety conditions field subscription pending...";
            break;
        }
        case ara::com::SubscriptionState::kSubscribed: {
            logger_.LogInfo() << "Safety conditions field subscribed!";
            break;
        }

        default:
            logger_.LogInfo() << "Default: VehicleConditionCollection.SetSubscriptionStateChangeHandler";
            break;
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

    proxy_->VehicleConditionCollection.Subscribe(1);
    proxy_->CampaignState.Subscribe(1);
}
