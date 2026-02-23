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

#include "ara/core/string.h"
#include "rest_payload_helpers.h"

namespace
{
using ara::vucm::CampaignStateType;

apd::rest::Pointer<apd::rest::ogm::Array> CreateSwPkgDescArray(const ara::vucm::SwPackageDescVectorType& swPkgDescVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    auto mylamdaaction = [](const ara::ucm::ActionType& packageAction) -> ara::core::String {
        switch (packageAction) {
        case ara::ucm::ActionType::kUpdate:
            return "kUpdate";

        case ara::ucm::ActionType::kInstall:
            return "kInstall";

        case ara::ucm::ActionType::kRemove:
            return "kRemove";

        default:
            return "ERROR";
        }
    };

    for (const auto& swPkgDesc : swPkgDescVec) {

        apd::rest::Pointer<apd::rest::ogm::Field> nameField(
            new apd::rest::ogm::Field("name", apd::rest::ogm::String::Make(swPkgDesc.swDesc.name.c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> versionField(
            new apd::rest::ogm::Field("version", apd::rest::ogm::String::Make(swPkgDesc.swDesc.version.c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> durationField(
            new apd::rest::ogm::Field("duration", apd::rest::ogm::String::Make(std::to_string(swPkgDesc.duration))));
        apd::rest::Pointer<apd::rest::ogm::Field> actionField(new apd::rest::ogm::Field(
            "action", apd::rest::ogm::String::Make(mylamdaaction(swPkgDesc.packageAction).c_str())));

        auto descEntity = apd::rest::ogm::Object::Make(
            std::move(nameField), std::move(versionField), std::move(durationField), std::move(actionField));

        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Array> CreateSwClInfoArray(const ara::ucm::SwClusterInfoVectorType& swClInfoVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    auto mylamdastate_string = [](const ara::ucm::SwClusterStateType& state) -> ara::core::String {
        switch (state) {
        case ara::ucm::SwClusterStateType::kPresent:
            return "kPresent";

        case ara::ucm::SwClusterStateType::kAdded:
            return "kAdded";

        case ara::ucm::SwClusterStateType::kUpdating:
            return "kAdded";

        case ara::ucm::SwClusterStateType::kRemoved:
            return "kAdded";

        default:
            return "ERROR";
        }
    };

    for (const auto& swclInfo : swClInfoVec) {

        apd::rest::Pointer<apd::rest::ogm::Field> nameField(
            new apd::rest::ogm::Field("name", apd::rest::ogm::String::Make(swclInfo.name.c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> versionField(
            new apd::rest::ogm::Field("version", apd::rest::ogm::String::Make(swclInfo.version.c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> stateField(new apd::rest::ogm::Field(
            "state", apd::rest::ogm::String::Make(mylamdastate_string(swclInfo.state).c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> sizeField(
            new apd::rest::ogm::Field("size", apd::rest::ogm::String::Make(std::to_string(swclInfo.size))));

        auto descEntity = apd::rest::ogm::Object::Make(
            std::move(nameField), std::move(versionField), std::move(stateField), std::move(sizeField));

        descArray->Append(std::move(descEntity));
    }
    return descArray;
}
}  // namespace

apd::rest::Pointer<apd::rest::ogm::Object> AraSoftwareUpdateToOgm(const AraSoftwareUpdate& upd,
    const progressInformationStruct& progress_upd,
    const packageInformationStruct& package_upd)
{
    auto ptr = apd::rest::ogm::Object::Make(
        std::make_unique<apd::rest::ogm::Field>(
            "campaign_state", apd::rest::ogm::String::Make(CampaignStateToString(progress_upd.campaignState_).c_str())),
        std::make_unique<apd::rest::ogm::Field>("software_clusters", CreateSwClInfoArray(upd.swClusterInfoVec)),
        std::make_unique<apd::rest::ogm::Field>(
            "software_packages", CreateSwPkgDescArray(package_upd.swPackageDescVec)),
        std::make_unique<apd::rest::ogm::Field>(
            "vehicle_package_descriptions", CreateVehPackageDescritpionInfo(package_upd.vehPackageDecVec)),
        VehicleConditionsToOgm(package_upd.vehicleConditions_));

    const ara::core::String approvalRequiredKey("approval_required");
    apd::rest::Pointer<apd::rest::ogm::Field> approvalRequiredField(new apd::rest::ogm::Field(
        approvalRequiredKey.c_str(), apd::rest::ogm::String::Make((package_upd.approvalRequired_) ? "true" : "false")));

    const ara::core::String feedbackUrlKey("feedback_url");
    const ara::core::String feedbackUrl("/api/arasoftwareupdate/approvestatus/" + std::to_string(upd.approvalId_));

    apd::rest::Pointer<apd::rest::ogm::Field> feedbackUrlField(
        new apd::rest::ogm::Field(feedbackUrlKey.c_str(), apd::rest::ogm::String::Make(feedbackUrl.c_str())));

    ptr->Insert(std::move(approvalRequiredField));
    ptr->Insert(std::move(feedbackUrlField));

    return ptr;
}

apd::rest::Pointer<apd::rest::ogm::Object> progressInformationToOgm(const progressInformationStruct& upd,
    const packageInformationStruct& package_upd)
{
    auto ptr = apd::rest::ogm::Object::Make(
        std::make_unique<apd::rest::ogm::Field>(
            "campaign_state", apd::rest::ogm::String::Make(CampaignStateToString(upd.campaignState_).c_str())),
        std::make_unique<apd::rest::ogm::Field>("progress_information", CreateProgressInformation(upd)),
        VehicleConditionsToOgm(package_upd.vehicleConditions_));

    const ara::core::String approvalRequiredKey("approval_required");
    apd::rest::Pointer<apd::rest::ogm::Field> approvalRequiredField(new apd::rest::ogm::Field(
        approvalRequiredKey.c_str(), apd::rest::ogm::String::Make((package_upd.approvalRequired_) ? "true" : "false")));

    ptr->Insert(std::move(approvalRequiredField));

    return ptr;
}

apd::rest::Pointer<apd::rest::ogm::Object> packageInformationToOgm(const packageInformationStruct& upd)
{
    auto ptr = apd::rest::ogm::Object::Make(
        std::make_unique<apd::rest::ogm::Field>("software_packages", CreateSwPkgDescArray(upd.swPackageDescVec)),
        std::make_unique<apd::rest::ogm::Field>(
            "vehicle_package_descriptions", CreateVehPackageDescritpionInfo(upd.vehPackageDecVec)),
        VehicleConditionsToOgm(upd.vehicleConditions_));

    const ara::core::String approvalRequiredKey("approval_required");
    apd::rest::Pointer<apd::rest::ogm::Field> approvalRequiredField(new apd::rest::ogm::Field(
        approvalRequiredKey.c_str(), apd::rest::ogm::String::Make((upd.approvalRequired_) ? "true" : "false")));

    ptr->Insert(std::move(approvalRequiredField));

    return ptr;
}

apd::rest::Pointer<apd::rest::ogm::Array> CreatecampaignResolutionArray(
    const ara::vucm::UCMMasterResolutionVectorType& campaignResolutionVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    auto mylamdaCampaignresolutiontostring
        = [](const ara::vucm::UCMMasterResolutionType& campaignResolution) -> ara::core::String {
        switch (campaignResolution) {
        case ara::vucm::UCMMasterResolutionType::kSuccess:
            return "kSuccess";

        case ara::vucm::UCMMasterResolutionType::kUCMError:
            return "kUCMError";

        case ara::vucm::UCMMasterResolutionType::kInvalidVehiclePackageManifest:
            return "kInvalidVehiclePackageManifest";

        case ara::vucm::UCMMasterResolutionType::kSubordinateNotAvailableOnTheNetwork:
            return "kSubordinateNotAvailableOnTheNetwork";

        case ara::vucm::UCMMasterResolutionType::kVehicleStateManagerCommunicationError:
            return "kVehicleStateManagerCommunicationError";

        case ara::vucm::UCMMasterResolutionType::kCampaignCancelled:
            return "kCampaignCancelled";

        case ara::vucm::UCMMasterResolutionType::kUnsupportedSafetyCondition:
            return "kUnsupportedSafetyCondition";

        case ara::vucm::UCMMasterResolutionType::kRollingBackFailed:
            return "kRollingBackFailed";

        default:
            return "UNKNOWN ERROR";
        }
    };

    for (const auto& campaignResolution : campaignResolutionVec) {
        apd::rest::Pointer<apd::rest::ogm::Field> campaignResolutionField(
            new apd::rest::ogm::Field("campaignResolution",
                apd::rest::ogm::String::Make(mylamdaCampaignresolutiontostring(campaignResolution).c_str())));

        auto descEntity = apd::rest::ogm::Object::Make(std::move(campaignResolutionField));
        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Array> CreateUCMStepErrorArray(
    const ara::vucm::UCMStepErrorVectorType& UCMStepErrorVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    auto mylamdasoftwarePackageStep
        = [](const ara::vucm::SoftwarePackageStepType& softwarePackageStep) -> ara::core::String {
        switch (softwarePackageStep) {
        case ara::vucm::SoftwarePackageStepType::kTransfer:
            return "kTransfer";

        case ara::vucm::SoftwarePackageStepType::kProcess:
            return "kProcess";

        case ara::vucm::SoftwarePackageStepType::kActivate:
            return "kActivate";

        default:
            return "ERROR";
        }
    };

    for (const auto& UCMStepError : UCMStepErrorVec) {
        apd::rest::Pointer<apd::rest::ogm::Field> ucmIdField(
            new apd::rest::ogm::Field("ucmId", apd::rest::ogm::String::Make(UCMStepError.ucmId.c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> softwarePackageStepField(
            new apd::rest::ogm::Field("softwarePackageStep",
                apd::rest::ogm::String::Make(mylamdasoftwarePackageStep(UCMStepError.softwarePackageStep).c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> returnedErrorField(new apd::rest::ogm::Field("returnedError",
            apd::rest::ogm::String::Make(std::to_string(static_cast<int>(UCMStepError.returnedError)))));

        auto descEntity = apd::rest::ogm::Object::Make(
            std::move(ucmIdField), std::move(softwarePackageStepField), std::move(returnedErrorField));

        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Object> getcampaignresult(const ara::vucm::CampaignResultType campaignResult)
{
    auto ptr = apd::rest::ogm::Object::Make(std::make_unique<apd::rest::ogm::Field>("campaignResolution",
                                                CreatecampaignResolutionArray(campaignResult.campaignResolution)),

        std::make_unique<apd::rest::ogm::Field>("UCMStepError", CreateUCMStepErrorArray(campaignResult.UCMStepError)),

        std::make_unique<apd::rest::ogm::Field>(
            "campaignStartTime", apd::rest::ogm::String::Make(std::to_string(campaignResult.campaignStartTime))),

        std::make_unique<apd::rest::ogm::Field>("campaignResolutionTime",
            apd::rest::ogm::String::Make(std::to_string(campaignResult.campaignResolutionTime))),

        std::make_unique<apd::rest::ogm::Field>(
            "driverNotified", apd::rest::ogm::String::Make((campaignResult.driverNotified) ? "true" : "false")));
    return ptr;
}

apd::rest::Pointer<apd::rest::ogm::Array> historyVectorArray(const ara::ucm::HistoryVectorType& historyVectorVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    auto mylamdahistoryVectoraction = [](const ara::ucm::ActionType& action) -> ara::core::String {
        switch (action) {
        case ara::ucm::ActionType::kUpdate:
            return "kUpdate";

        case ara::ucm::ActionType::kInstall:
            return "kInstall";

        case ara::ucm::ActionType::kRemove:
            return "kRemove";

        default:
            return "ERROR";
        }
    };

    auto mylamdaresolution = [](const ara::ucm::ResultType& resolution) -> ara::core::String {
        switch (resolution) {
        case ara::ucm::ResultType::kActivated:
            return "kActivated";

        case ara::ucm::ResultType::kActivatedAndRolledBack:
            return "kActivatedAndRolledBack";

        case ara::ucm::ResultType::kVerificationFailed:
            return "kVerificationFailed";

        default:
            return "ERROR INVALID VALUE";
        }
    };

    for (const auto& historyVector : historyVectorVec) {
        apd::rest::Pointer<apd::rest::ogm::Field> timeField(
            new apd::rest::ogm::Field("time", apd::rest::ogm::String::Make(std::to_string(historyVector.time))));

        apd::rest::Pointer<apd::rest::ogm::Field> nameField(
            new apd::rest::ogm::Field("name", apd::rest::ogm::String::Make(historyVector.name.c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> versionField(
            new apd::rest::ogm::Field("version", apd::rest::ogm::String::Make(historyVector.version.c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> actionField(new apd::rest::ogm::Field(
            "action", apd::rest::ogm::String::Make(mylamdahistoryVectoraction(historyVector.action).c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> resolutionField(new apd::rest::ogm::Field(
            "resolution", apd::rest::ogm::String::Make(mylamdaresolution(historyVector.resolution).c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> failureErrorField(new apd::rest::ogm::Field(
            "failureError", apd::rest::ogm::String::Make(std::to_string(historyVector.failureError))));

        auto descEntity = apd::rest::ogm::Object::Make(std::move(timeField),
            std::move(nameField),
            std::move(versionField),
            std::move(actionField),
            std::move(resolutionField),
            std::move(failureErrorField));
        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Array> uCMHistoryectorarray(
    const ara::vucm::UCMHistoryVectorType& UCMhistoryVectorVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    for (const auto& UCMhistoryVector : UCMhistoryVectorVec) {
        apd::rest::Pointer<apd::rest::ogm::Field> ucmIdField(
            new apd::rest::ogm::Field("ucmId", apd::rest::ogm::String::Make(UCMhistoryVector.ucmId.c_str())));

        apd::rest::Pointer<apd::rest::ogm::Field> historyVectorField(
            new apd::rest::ogm::Field("historyVector", historyVectorArray(UCMhistoryVector.historyVector)));

        auto descEntity = apd::rest::ogm::Object::Make(std::move(ucmIdField), std::move(historyVectorField));
        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Array> processcampaignhistoryarray(
    const ara::vucm::CampaignHistoryVectorType& campaignHistoryInfoVec)
{
    auto descArray = apd::rest::ogm::Array::Make();

    for (const auto& campaignHistory : campaignHistoryInfoVec) {
        apd::rest::Pointer<apd::rest::ogm::Field> campaignResultField(
            new apd::rest::ogm::Field("campaignResult", getcampaignresult(campaignHistory.campaignResult)));

        apd::rest::Pointer<apd::rest::ogm::Field> historyVectorField(
            new apd::rest::ogm::Field("historyVector", uCMHistoryectorarray(campaignHistory.historyVector)));

        apd::rest::Pointer<apd::rest::ogm::Field> repositoryField(
            new apd::rest::ogm::Field("repository", apd::rest::ogm::String::Make(campaignHistory.repository.c_str())));

        auto descEntity = apd::rest::ogm::Object::Make(
            std::move(campaignResultField), std::move(historyVectorField), std::move(repositoryField));
        descArray->Append(std::move(descEntity));
    }
    return descArray;
}

apd::rest::Pointer<apd::rest::ogm::Object> historyInformationToOgm(const campaignHistoryStruct& historyInfo)
{
    auto ptr = apd::rest::ogm::Object::Make(

        std::make_unique<apd::rest::ogm::Field>(
            "Result_Last_Refreshed_At", apd::rest::ogm::String::Make(std::to_string(historyInfo.timestampLT)))

            ,
        std::make_unique<apd::rest::ogm::Field>(
            "campaign_history", processcampaignhistoryarray(historyInfo.campaignHistoryInfo_.history)));

    return ptr;
}

ara::core::Optional<bool> GetVehicleDriverApproval(const apd::rest::ServerRequest& request)
{
    ara::core::String content{""};

    const auto& rootObj = request.GetObject().get();
    if (rootObj.HasField("status")) {
        auto it = rootObj.Find("status");
        auto& value = (*it).GetValue();
        apd::rest::ogm::Visit(&value, [&content](apd::rest::ogm::String* str) { content = str->GetValue(); });
        if (content == "approved")
            return true;
        if (content == "cancelled")
            return false;
    }
    return {};
}

ara::core::String SafetyStateToString(const ara::vucm::SafetyStateType safetyState)
{
    switch (safetyState) {
    case ara::vucm::SafetyStateType::NotSafe: {
        return "NotSafe";
    }
    case ara::vucm::SafetyStateType::Safe: {
        return "Safe";
    }
    case ara::vucm::SafetyStateType::NotSupported: {
        return "NotSupported";
    }
    default:
        return "Unknown";
    }
}

apd::rest::Pointer<apd::rest::ogm::Object> CreateVehPackageDescritpionInfo(
    const ara::vucm::VehiclePackageDescriptionType& vehPackageDescritpion)
{
    apd::rest::Pointer<apd::rest::ogm::Field> durationField(new apd::rest::ogm::Field("estimatedDurationOfCampaign",
        apd::rest::ogm::String::Make(std::to_string(vehPackageDescritpion.estimatedDurationOfCampaign))));

    apd::rest::Pointer<apd::rest::ogm::Field> purposeField(new apd::rest::ogm::Field(
        "purposeOfUpdate", apd::rest::ogm::String::Make(vehPackageDescritpion.purposeOfUpdate.c_str())));

    auto vehiclePackageDescritpion = apd::rest::ogm::Object::Make(std::move(durationField), std::move(purposeField));

    return vehiclePackageDescritpion;
}

apd::rest::Pointer<apd::rest::ogm::Object> CreateProgressInformation(const progressInformationStruct& upd)
{
    apd::rest::Pointer<apd::rest::ogm::Field> software_process_progressField(
        new apd::rest::ogm::Field("software_process_progress",
            apd::rest::ogm::String::Make(std::to_string(static_cast<int>(upd.processProgress)))));

    apd::rest::Pointer<apd::rest::ogm::Field> software_transferToUCMMaster_progressField(
        new apd::rest::ogm::Field("software_transferToUCMMaster_progress",
            apd::rest::ogm::String::Make(std::to_string(static_cast<int>(upd.processProgress)))));

    auto progressInformation = apd::rest::ogm::Object::Make(
        std::move(software_process_progressField), std::move(software_transferToUCMMaster_progressField));

    return progressInformation;
}

apd::rest::Pointer<apd::rest::ogm::Field> VehicleConditionsToOgm(
    const ara::vucm::VehicleConditionCollectionType& vehicleConditions)
{
    auto vehicleConditionsArray = apd::rest::ogm::Array::Make();
    for (const auto& vehicleCondition : vehicleConditions) {
        apd::rest::Pointer<apd::rest::ogm::Field> conditionField(
            new apd::rest::ogm::Field("Condition", apd::rest::ogm::String::Make(vehicleCondition.condition.c_str())));
        apd::rest::Pointer<apd::rest::ogm::Field> stateField(new apd::rest::ogm::Field(
            "State", apd::rest::ogm::String::Make(SafetyStateToString(vehicleCondition.state).c_str())));

        auto safetyEntity = apd::rest::ogm::Object::Make(std::move(conditionField), std::move(stateField));

        vehicleConditionsArray->Append(std::move(safetyEntity));
    }
    return std::make_unique<apd::rest::ogm::Field>("vehicle_conditions", std::move(vehicleConditionsArray));
}

const ara::core::String CampaignStateToString(const CampaignStateType status)
{
    switch (status) {
    case CampaignStateType::kIdle:
        return "Idle";

    case CampaignStateType::kSyncing:
        return "Syncing";

    case CampaignStateType::kVehiclePackageTransferring:
        return "VehiclePackageTransferring";

    case CampaignStateType::kSoftwarePackage_Transferring:
        return "SoftwarePackage_Transferring";

    case CampaignStateType::kProcessing:
        return "Processing";

    case CampaignStateType::kActivating:
        return "Activating";

    case CampaignStateType::kVehicleChecking:
        return "VehicleChecking";

    case CampaignStateType::kCancelling:
        return "Cancelling";

    default:
        return "Unknown";
    }
}
