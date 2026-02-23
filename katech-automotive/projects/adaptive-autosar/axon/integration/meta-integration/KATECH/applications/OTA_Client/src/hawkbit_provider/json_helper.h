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

#ifndef JSON_HELPER_H_
#define JSON_HELPER_H_

#include <string>
#include <unordered_map>

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

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

/**
 * \brief Extraction function to get a string from a given path of a given json object
 * \param root root object of the json
 * \param path path to the json field where the string shall be read out
 * \return pointer to string or nullptr if path was not found in json
 */
ara::rest::Pointer<ara::rest::ogm::String> JsonGetString(ara::rest::Pointer<ara::rest::ogm::Object>& root,
    const std::string& path);

/**
 * \brief Extraction function to get a int from a given path of a given json object
 * \param root root object of the json
 * \param path path to the json field where the string shall be read out
 * \return int value. -1 if path was not found in json
 */
int64_t JsonGetInt(ara::rest::Pointer<ara::rest::ogm::Object>& root, const char* path);

/**
 * \brief Extraction function to get an array from a given path of a given json object
 * \param root root object of the json
 * \param path path to the json field where the string shall be read out
 * \return pointer to the array object or nullptr if path was not found in json
 */
ara::rest::Pointer<ara::rest::ogm::Array> JsonGetArray(ara::rest::Pointer<ara::rest::ogm::Object>& root,
    const char* path);

/**
 * \brief Checking function to check for the presence of a key in a json.
 * \param root root object of the json
 * \param key key to check for
 * \return true if key is present, false if not
 */
bool JsonContains(ara::rest::Pointer<ara::rest::ogm::Object>& root, const char* key);

/**
 * \brief Extraction function to get an object of an array at a specific index.
 * \param array array object
 * \param index index from which the object shall be read out
 * \return pointer to object or nullptr if index is out of bounds
 */
ara::rest::Pointer<ara::rest::ogm::Object> JsonArrayGetElement(ara::rest::Pointer<ara::rest::ogm::Array>& array,
    uint index);

/**
 * \brief Extraction function to get the sleeping time from a json object
 * \param root root object of the json
 * \param defaultTimeSec default time if no sleep time has been found in given json
 * \return sleeping time
 */
std::chrono::seconds JsonGetSleeptime(ara::rest::Pointer<ara::rest::ogm::Object>& root,
    const std::chrono::seconds& defaultTimeSec);

/**
 * \brief Function to build hawkbit feedback json
 * \param obj_node root object of the json
 * \param id path to the json field where the string shall be read out
 * \param detail path to the json field where the string shall be read out
 * \param finished path to the json field where the string shall be read out
 * \param execution path to the json field where the string shall be read out
 * \param progress path to the json field where the string shall be read out
 */
void JsonBuildStatus(ara::rest::Pointer<ara::rest::ogm::Object>& obj_node,
    const std::string& id,
    const std::string& detail,
    const std::string& finished,
    const std::string& execution,
    const std::unordered_map<std::string, std::string>& data,
    int progress);

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // JSON_HELPER_H_
