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

#include "ara/rest/ogm/base.h"
#include "ara/rest/ogm/node.h"
#include "ara/rest/ogm/field.h"
#include "ara/rest/ogm/object.h"
#include "ara/rest/ogm/array.h"
#include "ara/rest/ogm/string.h"
#include "ara/rest/ogm/int.h"
#include "ara/rest/ogm/real.h"
#include "ara/rest/ogm/visit.h"
#include "ara/rest/ogm/copy.h"

#include "json_helper.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace updateClient
{

using namespace ara::rest;

static ara::rest::Pointer<ara::rest::ogm::Field> JsonExtractValue(ara::rest::Pointer<ara::rest::ogm::Object>& root,
    const std::string& path)
{
    std::string copyPath(path);
    std::string delimiter = ".";
    std::size_t pos = 0;
    Pointer<ogm::Object> value = Copy(root);
    Pointer<ogm::Field> field = NULL;

    auto newValue = value->Find(copyPath.substr(0, pos));

    while ((pos = copyPath.find(delimiter)) != std::string::npos) {
        newValue = value->Find(copyPath.substr(0, pos));

        if (newValue == value->GetFields().end()) {
            return nullptr;
        }
        copyPath.erase(0, pos + delimiter.length());

        ogm::Visit(&((*newValue).GetValue()),
            [&value](const ogm::Object* object_with_in_field) { value = Copy(object_with_in_field); });
    }

    newValue = value->Find(copyPath);

    if (newValue == value->GetFields().end()) {
        return nullptr;
    }

    return Copy(&(*newValue));
}

static ara::rest::Pointer<ara::rest::ogm::Array> JsonGetArray(ara::rest::Pointer<ara::rest::ogm::Object>& root,
    const char* path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::Array> array = nullptr;
    ogm::Visit(&field->GetValue(), [&array](ogm::Array* arrayField) { array = Copy(arrayField); });

    return array;
}

static std::string GetString(ara::rest::Pointer<ara::rest::ogm::Object>& root, const std::string& path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::String> content;

    if (field == nullptr) {
        return nullptr;
    }

    ogm::Visit(&field->GetValue(), [&content](ogm::String* str) { content = Copy(str); });
    if (content == nullptr) {
        return std::string("");
    }
    return std::string(content->GetValue());
}

static ara::rest::Pointer<ara::rest::ogm::Object> JsonArrayGetElement(ara::rest::Pointer<ara::rest::ogm::Array>& array,
    uint index)
{
    Pointer<ogm::Object> content = nullptr;

    if (index >= array->GetSize())
        return nullptr;

    ogm::Visit(&array->GetValue(index), [&content](ogm::Object* objectInField) { content = Copy(objectInField); });

    return content;
}

SoftwarePackageInformation GetSoftwarePackageInformation(ara::rest::Pointer<ara::rest::ogm::Object>& root)
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

void JsonBuildRequiredSoftwarePackages(ara::rest::Pointer<ara::rest::ogm::Object>& obj_node,
    ara::ucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    obj_node = ogm::Object::Make();
    Pointer<ogm::Array> reqSwPackageArr = ogm::Array::Make();

    for (auto info : requiredSoftwarePackages) {
        Pointer<ogm::Object> requiredSoftwareObj = ogm::Object::Make();
        requiredSoftwareObj->Insert(ogm::Field::Make("name", ogm::String::Make(info.Name.c_str())));
        requiredSoftwareObj->Insert(ogm::Field::Make("version", ogm::String::Make(info.Version.c_str())));
        reqSwPackageArr->Append(std::move(requiredSoftwareObj));
    }
    obj_node->Insert(ogm::Field::Make("required_software_packages", std::move(reqSwPackageArr)));
}

}  // namespace updateClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd
