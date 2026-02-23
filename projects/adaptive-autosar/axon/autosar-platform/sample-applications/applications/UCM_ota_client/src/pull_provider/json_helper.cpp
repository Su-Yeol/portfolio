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

#include "apd/rest/ogm/base.h"
#include "apd/rest/ogm/node.h"
#include "apd/rest/ogm/field.h"
#include "apd/rest/ogm/object.h"
#include "apd/rest/ogm/array.h"
#include "apd/rest/ogm/string.h"
#include "apd/rest/ogm/int.h"
#include "apd/rest/ogm/real.h"
#include "apd/rest/ogm/visit.h"
#include "apd/rest/ogm/copy.h"

#include "json_helper.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace updateClient
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

    return Copy(&(*newValue));
}

static apd::rest::Pointer<apd::rest::ogm::Array> JsonGetArray(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const char* path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::Array> array = nullptr;
    ogm::Visit(&field->GetValue(), [&array](ogm::Array* arrayField) { array = Copy(arrayField); });

    return array;
}

static ara::core::String GetString(apd::rest::Pointer<apd::rest::ogm::Object>& root, const ara::core::String& path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::String> content;

    if (field == nullptr) {
        return nullptr;
    }

    ogm::Visit(&field->GetValue(), [&content](ogm::String* str) { content = Copy(str); });
    if (content == nullptr) {
        return ara::core::String("");
    }
    return ara::core::String(content->GetValue());
}

static apd::rest::Pointer<apd::rest::ogm::Object> JsonArrayGetElement(apd::rest::Pointer<apd::rest::ogm::Array>& array,
    uint index)
{
    Pointer<ogm::Object> content = nullptr;

    if (index >= array->GetSize())
        return nullptr;

    ogm::Visit(&array->GetValue(index), [&content](ogm::Object* objectInField) { content = Copy(objectInField); });

    return content;
}

SoftwarePackageInformation GetSoftwarePackageInformation(apd::rest::Pointer<apd::rest::ogm::Object>& root)
{
    SoftwarePackageInformation infos;
    Pointer<ogm::Array> jsonChunks = nullptr;
    Pointer<ogm::Object> jsonChunk = nullptr;

    jsonChunks = JsonGetArray(root, "software_packages");

    if (jsonChunks == nullptr || jsonChunks->GetSize() == 0) {
        return infos;
    }

    for (uint i = 0; (jsonChunk = JsonArrayGetElement(jsonChunks, i)) != nullptr; i++) {
        SoftwarePackage softwarePackage;

        softwarePackage.fqn = GetString(jsonChunk, "fqn");
        softwarePackage.name = GetString(jsonChunk, "name");
        softwarePackage.softwareCluster.fqn = GetString(jsonChunk, "software_cluster.fqn");
        softwarePackage.softwareCluster.name = GetString(jsonChunk, "software_cluster.name");
        softwarePackage.softwareCluster.version = GetString(jsonChunk, "software_cluster.version");
        softwarePackage.downloadUrl = GetString(jsonChunk, "download_url");

        infos.push_back(softwarePackage);
    }

    return infos;
}

void JsonBuildRequiredSoftwarePackages(apd::rest::Pointer<apd::rest::ogm::Object>& obj_node,
    ara::vucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    obj_node = ogm::Object::Make();
    Pointer<ogm::Array> reqSwPackageArr = ogm::Array::Make();

    for (auto info : requiredSoftwarePackages) {
        Pointer<ogm::Object> requiredSoftwareObj = ogm::Object::Make();
        requiredSoftwareObj->Insert(ogm::Field::Make("name", ogm::String::Make(info.name.c_str())));
        requiredSoftwareObj->Insert(ogm::Field::Make("version", ogm::String::Make(info.version.c_str())));
        reqSwPackageArr->Append(std::move(requiredSoftwareObj));
    }
    obj_node->Insert(ogm::Field::Make("required_software_packages", std::move(reqSwPackageArr)));
}

}  // namespace updateClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd
