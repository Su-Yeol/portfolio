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

#include "apd/rest/serialize/serialize.h"

#include "json_helper.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

using namespace apd::rest;

static apd::rest::Pointer<apd::rest::ogm::Field> JsonExtractValue(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const ara::core::String& path)
{
    ara::core::String copyPath(path);
    const ara::core::String delimiter = ".";
    std::size_t pos = 0;
    Pointer<ogm::Object> value = Copy(root);
    Pointer<ogm::Field> field = NULL;

    auto newValue = value->Find((copyPath.substr(0, pos)).c_str());

    while ((pos = copyPath.find(delimiter)) != ara::core::String::npos) {
        newValue = value->Find((copyPath.substr(0, pos)).c_str());

        if (newValue == value->GetFields().end()) {
            return nullptr;
        }
        copyPath.erase(0, pos + delimiter.length());

        ogm::Visit(&((*newValue).GetValue()),
            [&value](const ogm::Object* object_with_in_field) { value = Copy(object_with_in_field); });
    }

    newValue = value->Find(copyPath.c_str());

    if (newValue == value->GetFields().end()) {
        return nullptr;
    }

    return apd::rest::ogm::Copy(&(*newValue));
}

apd::rest::Pointer<apd::rest::ogm::String> JsonGetString(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const ara::core::String& path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::String> content;

    if (field == nullptr) {
        return nullptr;
    }

    ogm::Visit(&field->GetValue(), [&content](ogm::String* str) { content = Copy(str); });
    return Copy(content);
}

int64_t JsonGetInt(apd::rest::Pointer<apd::rest::ogm::Object>& root, const char* path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    int64_t value = -1;
    ogm::Visit(&field->GetValue(), [&value](apd::rest::ogm::Int* integer) { value = integer->GetValue(); });

    return value;
}

apd::rest::Pointer<apd::rest::ogm::Array> JsonGetArray(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const char* path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::Array> array = nullptr;
    ogm::Visit(&field->GetValue(), [&array](ogm::Array* arrayField) { array = Copy(arrayField); });

    return array;
}

bool JsonContains(apd::rest::Pointer<apd::rest::ogm::Object>& root, const char* key)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, key);

    if (field == nullptr)
        return false;

    return true;
}

apd::rest::Pointer<apd::rest::ogm::Object> JsonArrayGetElement(apd::rest::Pointer<apd::rest::ogm::Array>& array,
    uint index)
{
    Pointer<ogm::Object> content = nullptr;

    if (index >= array->GetSize())
        return nullptr;

    ogm::Visit(&array->GetValue(index), [&content](ogm::Object* objectInField) { content = Copy(objectInField); });

    return content;
}

/// @brief Get polling sleep time from hawkBit JSON response.
std::chrono::seconds JsonGetSleeptime(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const std::chrono::seconds& defaultTimeSec)
{
    // TODO define all consts in unique file
    const char* valueTime = JsonGetString(root, "config.polling.sleep")->GetValue().c_str();

    if (valueTime != nullptr) {
        struct tm time;
        strptime(valueTime, "%T", &time);
        std::chrono::seconds pollSleepTime(time.tm_sec + (time.tm_min * 60) + (time.tm_hour * 60 * 60));
        return pollSleepTime;
    }
    return defaultTimeSec;
}

/// Build JSON status request.
/// have a look at :
/// https://www.eclipse.org/hawkbit/rest-api/rootcontroller-api-guide/#_post_tenant_controller_v1_controllerid_deploymentbase_actionid_feedback
void JsonBuildStatus(apd::rest::Pointer<apd::rest::ogm::Object>& obj_node,
    const ara::core::String& id,
    const ara::core::String& detail,
    const ara::core::String& finished,
    const ara::core::String& execution,
    const std::unordered_map<ara::core::String, ara::core::String>& data,
    int progress)
{
    // TODO Check if file was created
    // TODO define all using in unique file
    time_t currentTime;
    struct tm timeInfo;
    // TODO Check if 16 enough
    char timeString[16];

    Pointer<ogm::Object> statusJson = ogm::Object::Make();
    Pointer<ogm::Object> resultJson = ogm::Object::Make();
    Pointer<ogm::Object> progressJson = ogm::Object::Make();
    Pointer<ogm::Object> dataJson = ogm::Object::Make();
    Pointer<ogm::Array> detailsJson = ogm::Array::Make();

    // Get current time in UTC
    time(&currentTime);
    gmtime_r(&currentTime, &timeInfo);
    strftime(timeString, sizeof(timeString), "%Y%m%dT%H%M%S", &timeInfo);
    if (!id.empty())
        obj_node->Insert(ogm::Field::Make("id", ogm::String::Make(id.c_str())));

    obj_node->Insert(ogm::Field::Make("time", ogm::String::Make(String(timeString))));

    if (execution.compare("proceeding") == 0) {
        progressJson->Insert(ogm::Field::Make("of", ogm::Int::Make(3)));
        progressJson->Insert(ogm::Field::Make("cnt", ogm::Int::Make(progress)));
        resultJson->Insert(ogm::Field::Make("progress", std::move(progressJson)));
    }

    resultJson->Insert(ogm::Field::Make("finished", ogm::String::Make(finished.c_str())));

    statusJson->Insert(ogm::Field::Make("result", std::move(resultJson)));
    statusJson->Insert(ogm::Field::Make("execution", ogm::String::Make(execution.c_str())));

    if (!detail.empty()) {

        detailsJson->Append(ogm::String::Make(detail.c_str()));
        statusJson->Insert(ogm::Field::Make("details", std::move(detailsJson)));
    }

    obj_node->Insert(ogm::Field::Make("status", std::move(statusJson)));

    if (!data.empty()) {

        for (const auto& pair : data)
            dataJson->Insert(ogm::Field::Make(pair.first.c_str(), ogm::String::Make(pair.second.c_str())));

        obj_node->Insert(ogm::Field::Make("data", std::move(dataJson)));
    }
}

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd
