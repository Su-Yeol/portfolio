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

#include <string>
#include "rest_payload_helpers.h"
#include <boost/tokenizer.hpp>
#include <fstream>

// rapidjson
#include "rapidjson/document.h"

ara::core::Optional<bool> GetVehicleCheck(const apd::rest::ServerRequest& request, ara::log::Logger& logger)
{
    apd::rest::String content{""};
    logger.LogInfo() << "Enter GetVehicleCheck function";
    const auto& rootObj = request.GetObject().get();
    if (!rootObj.HasField(vsm::rest::kVehicleCheckResolutionField)) {  // "VehicleCheckResolution"
        return {};
    }

    auto it = rootObj.Find(vsm::rest::kVehicleCheckResolutionField);
    auto& value = (*it).GetValue();
    apd::rest::ogm::Visit(&value, [&content](apd::rest::ogm::String* str) { content = str->GetValue(); });
    if (content == "true")
        return true;
    if (content == "false")
        return false;
    return {};
}

ara::core::Optional<ara::vucm::VehicleConditionCollectionType> GetSafetyStates(const apd::rest::ServerRequest& request,
    ara::log::Logger& logger)
{
    ara::vucm::VehicleConditionCollectionType safetyStates;
    const auto& rootObj = request.GetObject().get();
    if (!rootObj.HasField(vsm::rest::kSafetyStatesField)) {
        return {};
    }
    auto it = rootObj.Find(vsm::rest::kSafetyStatesField);  // "SafetyStates"
    std::size_t receivedArraySize{};
    auto& value = (*it).GetValue();
    apd::rest::ogm::Visit(&value, [&safetyStates, &receivedArraySize, &logger](apd::rest::ogm::Array* array) {
        const auto safetyStatesArr = array->GetValues();
        receivedArraySize = array->GetSize();
        for (auto safetyStateIter = safetyStatesArr.begin(); safetyStateIter != safetyStatesArr.end();
             safetyStateIter++) {
            ara::vucm::VehicleConditionType cond;
            apd::rest::ogm::Visit(&(*safetyStateIter), [&cond, &logger](apd::rest::ogm::Object* safetyObj) {
                if (safetyObj->HasField(vsm::rest::kConditionField)) {
                    auto it_ = safetyObj->Find(vsm::rest::kConditionField);
                    auto& value_ = (*it_).GetValue();
                    apd::rest::ogm::Visit(
                        &value_, [&cond](apd::rest::ogm::String* str) { cond.condition = str->GetValue(); });
                }
                if (safetyObj->HasField(vsm::rest::kStateField)) {
                    auto it_ = safetyObj->Find(vsm::rest::kStateField);
                    auto& value_ = (*it_).GetValue();
                    apd::rest::ogm::Visit(&value_,
                        [&cond](apd::rest::ogm::String* str) { cond.state = StringToSafetyState(str->GetValue()); });
                }
            });
            safetyStates.emplace_back(cond);
        }
    });
    if ((safetyStates.size()) != receivedArraySize) {
        safetyStates.clear();
        logger.LogError() << "Size Mismatch";
        return {};
    }
    return safetyStates;
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

ara::vucm::SafetyStateType StringToSafetyState(const ara::core::String& safetyState)
{
    if (safetyState == "Safe") {
        return ara::vucm::SafetyStateType::Safe;
    } else if (safetyState == "NotSafe") {
        return ara::vucm::SafetyStateType::NotSafe;
    } else if (safetyState == "NotSupported") {
        return ara::vucm::SafetyStateType::NotSupported;
    }
    return ara::vucm::SafetyStateType::NotSupported;
}

apd::rest::Pointer<apd::rest::ogm::Object> SafetyStatesToOgm(
    const ara::vucm::VehicleConditionCollectionType& safetyStates)
{
    auto safetyStatesArray = apd::rest::ogm::Array::Make();
    for (const auto& safetyState : safetyStates) {
        safetyStatesArray->Append(apd::rest::ogm::String::Make(SafetyStateToString(safetyState.state).c_str()));
    }
    apd::rest::Pointer<apd::rest::ogm::Field> safetyStatesField(
        new apd::rest::ogm::Field(vsm::rest::kSafetyStatesField, std::move(safetyStatesArray)));
    return apd::rest::ogm::Object::Make(std::move(safetyStatesField));
}

apd::rest::Pointer<apd::rest::ogm::Object> VehicleCheckResToOgm(const bool vehCheckRes)
{
    apd::rest::Pointer<apd::rest::ogm::Field> vehCheckResField(new apd::rest::ogm::Field(
        vsm::rest::kVehicleCheckResolutionField, apd::rest::ogm::String::Make(vehCheckRes ? "true" : "false")));
    return apd::rest::ogm::Object::Make(std::move(vehCheckResField));
}

std::map<uint32_t, ara::vucm::VehicleConditionCollectionType> ReadSafetyStatesFromFile(
    const ara::core::String& fileName,
    ara::log::Logger& logger)
{
    rapidjson::Document jsonDoc;
    std::ifstream ifs(fileName.c_str());
    if (!ifs.is_open() || !ifs.good()) {
        ifs.close();
        logger.LogError() << "Failed to read from file:" << fileName;
        return {};
    }

    const auto safetyStatesBuffer = ara::core::String(std::istreambuf_iterator<char>(ifs), {});
    ifs.close();

    if (safetyStatesBuffer.empty()) {
        logger.LogError() << "Safety states file is empty";
        return {};
    }

    if (jsonDoc.Parse(safetyStatesBuffer.c_str()).HasParseError()) {
        logger.LogError() << "Invalid file format";
        return {};
    }

    assert(jsonDoc.IsObject());
    std::map<uint32_t, ara::vucm::VehicleConditionCollectionType> safetyStates;
    for (const auto& vehicleRolloutStep : jsonDoc.GetObject()) {
        if (!vehicleRolloutStep.value.IsArray()) {
            logger.LogError() << "Invalid safety states format";
            return {};
        }

        for (const auto& safetyState : vehicleRolloutStep.value.GetArray()) {
            if (!safetyState.IsString()) {
                logger.LogError() << "Invalid safety state format for vehicle rollout step number:"
                                  << vehicleRolloutStep.name.GetUint();
                return {};
            }
            ara::vucm::VehicleConditionType cond = {"EMPTY", StringToSafetyState(safetyState.GetString())};
            safetyStates[std::stoi(vehicleRolloutStep.name.GetString())].emplace_back(cond);
        }
    }

    return safetyStates;
}

ara::core::Optional<bool> ReadVehCheckResFromFile(const ara::core::String& fileName, ara::log::Logger& logger)
{
    rapidjson::Document jsonDoc;
    std::ifstream ifs(fileName.c_str());
    if (!ifs.is_open() || !ifs.good()) {
        ifs.close();
        logger.LogError() << "Failed to read from file:" << fileName;
        return {};
    }

    const auto vehCheckResBuffer = ara::core::String(std::istreambuf_iterator<char>(ifs), {});
    ifs.close();

    if (vehCheckResBuffer.empty()) {
        logger.LogError() << "Vehicle check resolution file is empty";
        return {};
    }

    if (jsonDoc.Parse(vehCheckResBuffer.c_str()).HasParseError()) {
        logger.LogError() << "Invalid file format";
        return {};
    }

    assert(jsonDoc.IsObject());
    if (!jsonDoc.GetObject().HasMember(vsm::rest::kVehicleCheckResolutionField)) {
        logger.LogError() << vsm::rest::kVehicleCheckResolutionField << "tag is not present";
        return {};
    }

    const auto itr = jsonDoc.GetObject().FindMember(vsm::rest::kVehicleCheckResolutionField);
    const auto vehCheckResString = itr->value.GetString();
    if (strcmp(vehCheckResString, "true") == 0)
        return true;
    if (strcmp(vehCheckResString, "false") == 0)
        return false;

    return {};
}
