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

#ifndef ARA_UCM_VSM_SAMPLE_REST_PAYLOAD_HELPERS_H_
#define ARA_UCM_VSM_SAMPLE_REST_PAYLOAD_HELPERS_H_

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
#include "ara/vucm/impl_type_vehicleconditioncollectiontype.h"

namespace vsm
{
namespace rest
{
constexpr static const char* const kVehicleCheckResolutionField = "VehicleCheckResolution";
constexpr static const char* const kSafetyStatesField = "SafetyStates";
constexpr static const char* const kConditionField = "Condition";
constexpr static const char* const kStateField = "State";
const ara::core::String kVsmRoute = "/api/vsm";
const ara::core::String kVehicleCheckRelRoute = "/vehicle_check";
const ara::core::String kSafetyStatesRelRoute = "/safety_states";
}  // namespace rest
}  // namespace vsm

ara::core::Optional<bool> GetVehicleCheck(const apd::rest::ServerRequest& request, ara::log::Logger& logger);
ara::core::Optional<ara::vucm::VehicleConditionCollectionType> GetSafetyStates(const apd::rest::ServerRequest& request,
    ara::log::Logger& logger);
ara::core::String SafetyStateToString(const ara::vucm::SafetyStateType safetyState);
ara::vucm::SafetyStateType StringToSafetyState(const ara::core::String& safetyState);
apd::rest::Pointer<apd::rest::ogm::Object> SafetyStatesToOgm(
    const ara::vucm::VehicleConditionCollectionType& safetyStates);
apd::rest::Pointer<apd::rest::ogm::Object> VehicleCheckResToOgm(const bool vehCheckRes);
std::map<uint32_t, ara::vucm::VehicleConditionCollectionType> ReadSafetyStatesFromFile(
    const ara::core::String& fileName,
    ara::log::Logger& logger);
ara::core::Optional<bool> ReadVehCheckResFromFile(const ara::core::String& fileName, ara::log::Logger& logger);

#endif  // ARA_UCM_VSM_SAMPLE_REST_PAYLOAD_HELPERS_H_
