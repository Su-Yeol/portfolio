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

#ifndef APD_UCM_VDA_SAMPLE_REST_PAYLOAD_HELPERS_H_
#define APD_UCM_VDA_SAMPLE_REST_PAYLOAD_HELPERS_H_

#include "ara_software_update.h"
#include "apd/rest/server.h"
#include "apd/rest/ogm/base.h"
#include "apd/rest/ogm/node.h"
#include "apd/rest/ogm/field.h"
#include "apd/rest/ogm/object.h"
#include "apd/rest/ogm/array.h"
#include "apd/rest/ogm/string.h"
#include "apd/rest/ogm/int.h"
#include "apd/rest/ogm/real.h"
#include "apd/rest/ogm/visit.h"

#include "ara/core/optional.h"

apd::rest::Pointer<apd::rest::ogm::Object> AraSoftwareUpdateToOgm(const AraSoftwareUpdate& upd,
    const progressInformationStruct& progress_upd,
    const packageInformationStruct& package_upd);
apd::rest::Pointer<apd::rest::ogm::Object> progressInformationToOgm(const progressInformationStruct& upd,
    const packageInformationStruct& package_upd);
apd::rest::Pointer<apd::rest::ogm::Object> packageInformationToOgm(const packageInformationStruct& upd);
apd::rest::Pointer<apd::rest::ogm::Object> historyInformationToOgm(const campaignHistoryStruct& historyInfo);
apd::rest::Pointer<apd::rest::ogm::Object> getcampaignresult(const ara::vucm::CampaignResultType campaignResult);
apd::rest::Pointer<apd::rest::ogm::Array> CreatecampaignResolutionArray(
    const ara::vucm::UCMMasterResolutionVectorType& campaignResolutionVec);
apd::rest::Pointer<apd::rest::ogm::Array> processcampaignhistoryarray(
    const ara::vucm::CampaignHistoryVectorType& campaignHistoryInfoVec);
apd::rest::Pointer<apd::rest::ogm::Array> CreateUCMStepErrorArray(
    const ara::vucm::UCMStepErrorVectorType& UCMStepErrorVec);
ara::core::String softwarePackageSteptoString(const ara::vucm::SoftwarePackageStepType& softwarePackageStep);
apd::rest::Pointer<apd::rest::ogm::Array> uCMHistoryectorarray(
    const ara::vucm::UCMHistoryVectorType& UCMhistoryVectorVec);
apd::rest::Pointer<apd::rest::ogm::Array> historyVectorArray(const ara::ucm::HistoryVectorType& historyVectorVec);

ara::core::Optional<bool> GetVehicleDriverApproval(const apd::rest::ServerRequest& request);
ara::core::String SafetyStateToString(const ara::vucm::SafetyStateType safetyState);
apd::rest::Pointer<apd::rest::ogm::Field> VehicleConditionsToOgm(
    const ara::vucm::VehicleConditionCollectionType& vehicleConditions);
apd::rest::Pointer<apd::rest::ogm::Object> CreateVehPackageDescritpionInfo(
    const ara::vucm::VehiclePackageDescriptionType& vehPackageDescritpion);
apd::rest::Pointer<apd::rest::ogm::Object> CreateProgressInformation(const progressInformationStruct& upd);
const ara::core::String CampaignStateToString(const ara::vucm::CampaignStateType status);

#endif  // APD_UCM_VDA_SAMPLE_REST_PAYLOAD_HELPERS_H_
