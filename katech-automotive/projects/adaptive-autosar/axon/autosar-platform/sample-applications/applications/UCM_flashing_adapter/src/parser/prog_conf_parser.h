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

#ifndef ARA_UCM_PKGMGR_PROG_CONF_PARSER_H_
#define ARA_UCM_PKGMGR_PROG_CONF_PARSER_H_

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/log/logger.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

#include "prog_conf.h"
/**
 * @brief Parser for reprogramming sequence configuration file
 */
class ProgConfParser
{
public:
    ProgConfParser() = default;
    /** \brief Parses given programming sequence configuration file
     *  \param path to configuration file
     *  \return pointer to generated reprogramming sequence object
     */
    std::unique_ptr<ProgConf> ParseFromFile(const ara::core::String& progConfPath);

private:
    /** \brief Parses given json tree to reprogramming sequence configuration
     *  \param progConfTree the json tree
     *  \return pointer to generated reprogramming sequence configuration
     */
    std::unique_ptr<ProgConf> Parse(const boost::property_tree::ptree& progConfTree);
    ara::core::Vector<uint8_t> as_byte_vector(boost::property_tree::ptree const& pt,
        boost::property_tree::ptree::key_type const& key);
    uint8_t stringToHexU8(ara::core::String s);
    uint16_t stringToHexU16(ara::core::String s);
    template <typename T>
    ara::core::Vector<T> as_vector(boost::property_tree::ptree const& pt,
        boost::property_tree::ptree::key_type const& key);
    ReprogrammingSequenceType parseReprogrammingSequence(boost::property_tree::ptree const& pt);
    ara::log::Logger& logger_{
        ara::log::CreateLogger("FPCP", "FlashingadapterProgConfParser", ara::log::LogLevel::kVerbose)};
};

#endif  // ARA_UCM_PKGMGR_PROG_CONF_PARSER_H_
