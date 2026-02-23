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

#include "hawkbit_config_file.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

// TODO : Verify the values of config file, Defaut values, Return error of config file

bool ConfigClass::LoadDeviceHashTable(boost::property_tree::ptree& pt)
{
    if (device != nullptr) {
        logger_.LogError() << "Something was  wrong during memory allocation";
        return false;
    }

    device.reset(new std::unordered_map<ara::core::String, ara::core::String>);

    (*device)["product"] = pt.get<std::string>("device.product");
    (*device)["model"] = pt.get<std::string>("device.model");
    (*device)["serialnumber"] = pt.get<std::string>("device.serialnumber");
    (*device)["hw_revision"] = pt.get<std::string>("device.hw_revision");
    (*device)["key1"] = pt.get<std::string>("device.key1");
    (*device)["key2"] = pt.get<std::string>("device.key2");

    logger_.LogError() << "Device was initialized successfully";

    return true;
}

void ConfigClass::LoadConfigFile(const char* configFile)
{

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini(configFile, pt);

    // TODO check if initialization was ok,if not return false

    hawkbitServer = pt.get<std::string>("client.hawkbit_server");
    ssl = pt.get<bool>("client.ssl");
    sslVerify = pt.get<bool>("client.ssl_verify");
    tenantId = pt.get<std::string>("client.tenant_id");
    targetName = pt.get<std::string>("client.target_name");
    authToken = pt.get<std::string>("client.auth_token");
    gatewayToken = pt.get<std::string>("client.gateway_token");
    downloadLocation = pt.get<std::string>("client.download_location");
    retryWait = pt.get<int>("client.retry_wait");
    connectTimeout = pt.get<uint32_t>("client.connect_timeout");
    timeout = pt.get<uint32_t>("client.timeout");

    if (!LoadDeviceHashTable(pt)) {
        logger_.LogError() << "Cannot initialize device";
        return;
    }

    isInitializedCorrectly = true;
}

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd
