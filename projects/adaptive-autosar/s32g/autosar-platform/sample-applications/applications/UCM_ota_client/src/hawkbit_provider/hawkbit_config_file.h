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

#ifndef CONFIG_FILE_H_
#define CONFIG_FILE_H_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "ara/log/logger.h"
#include "ara/core/string.h"
#include <memory>
#include <unordered_map>
#include <cstdint>

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

/**
 * @brief Class that contains the HawkBit configuration.
 */
struct ConfigClass
{
    ConfigClass()
        : logger_(ara::log::CreateLogger("Cnf", "Configuration file", ara::log::LogLevel::kVerbose))
    { }

    ~ConfigClass() = default;

    /**
     * @brief Struct containing the artifact specification
     */
    bool isInitializedCorrectly = false;

    /**
     * @brief hawkBit host or IP and port
     */
    ara::core::String hawkbitServer;

    /**
     * @brief use https or http
     */
    bool ssl = false;

    /**
     * @brief verify https certificate
     */
    bool sslVerify = false;

    /**
     * @brief hawkBit target security token
     */
    ara::core::String authToken;

    /**
     * @brief hawkBit gateway security token
     */
    ara::core::String gatewayToken;

    /**
     * @brief hawkBit tenant id
     */
    ara::core::String tenantId;

    /**
     * @brief hawkBit controller id
     */
    ara::core::String targetName;

    /**
     * @brief  file to download
     */
    ara::core::String downloadLocation;

    /**
     * @brief connection timeout
     */
    uint32_t connectTimeout = 0;

    /**
     * @brief reply timeout
     */
    uint32_t timeout = 0;

    /**
     * @brief wait between retries
     */
    uint32_t retryWait = 0;

    /**
     * @brief Additional attributes sent to hawkBit
     */
    std::shared_ptr<std::unordered_map<ara::core::String, ara::core::String>> device = nullptr;

    /**
     * @brief Loading the configuration file
     * @param configFile The configuration file
     */
    void LoadConfigFile(const char* configFile);

private:
    /**
     * @brief Initializes the additional attributes of the device
     * @param pt property tree to be read out
     * @return true if successful
     */
    bool LoadDeviceHashTable(boost::property_tree::ptree& pt);

    ara::log::Logger& logger_;
};

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // CONFIG_FILE_H_
