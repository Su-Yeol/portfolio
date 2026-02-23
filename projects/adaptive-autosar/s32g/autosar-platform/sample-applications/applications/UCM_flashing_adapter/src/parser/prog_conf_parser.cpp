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
#include "ara/core/vector.h"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/detail/file_parser_error.hpp"
#include "boost/property_tree/json_parser.hpp"

#include "prog_conf_parser.h"

using boost::property_tree::ptree;
using boost::property_tree::ptree_error;
using boost::property_tree::file_parser_error;

template <typename T>
ara::core::Vector<T> ProgConfParser::as_vector(boost::property_tree::ptree const& pt,
    boost::property_tree::ptree::key_type const& key)
{
    logger_.LogInfo() << "as_vector";
    ara::core::Vector<T> r;
    for (auto& item : pt.get_child(key))
        r.push_back(item.second.get_value<T>());
    return r;
}

uint8_t ProgConfParser::stringToHexU8(ara::core::String s)
{
    const uint8_t value = std::stoul(s.c_str(), nullptr, 16);
    return value;
}

uint16_t ProgConfParser::stringToHexU16(ara::core::String s)
{
    const uint16_t value = std::stoul(s.c_str(), nullptr, 16);
    return value;
}

ara::core::Vector<uint8_t> ProgConfParser::as_byte_vector(boost::property_tree::ptree const& pt,
    boost::property_tree::ptree::key_type const& key)
{
    ara::core::Vector<uint8_t> r;
    for (auto& item : pt.get_child(key))
        r.push_back(stringToHexU8(item.second.get_value<std::string>()));
    return r;
}

ReprogrammingSequenceType ProgConfParser::parseReprogrammingSequence(boost::property_tree::ptree const& pt)
{
    const ara::core::String typeAsString = pt.get<ara::core::String>("ecuReprogrammingSequenceType");

    if (typeAsString == "singleBankEcu") {
        logger_.LogInfo() << "singleBankEcu";
        return ReprogrammingSequenceType::SingleBankEcu;
    } else if (typeAsString == "dualBankEcu") {
        logger_.LogInfo() << "dualBankEcu";
        return ReprogrammingSequenceType::DualBankEcu;
    } else {
        return ReprogrammingSequenceType::UnknownEcuType;
    }
}

std::unique_ptr<ProgConf> ProgConfParser::Parse(const boost::property_tree::ptree& progConfTree)
{
    try {

        ReprogrammingSequenceType type = parseReprogrammingSequence(progConfTree);
        ara::core::Vector<ara::core::String> fileList = as_vector<ara::core::String>(progConfTree, "programmingFiles");
        ara::core::Vector<ara::core::String> rollbackfileList;
        if (type == ReprogrammingSequenceType::SingleBankEcu) {
            rollbackfileList = as_vector<ara::core::String>(progConfTree, "rollbackFiles");
        }
        const uint32_t canRequestId = stringToHexU16(progConfTree.get<std::string>("canRequestId"));
        const uint32_t canResponseId = stringToHexU16(progConfTree.get<std::string>("canReponseId"));
        logger_.LogInfo() << "canRequestId" << canRequestId;
        logger_.LogInfo() << "canResponseId" << canResponseId;
        ProgConf::UdsCommandMap udsCommandMap;

        auto name = progConfTree.get<std::string>("name");
        auto version = progConfTree.get<std::string>("version");

        for (auto& item : progConfTree.get_child("udsCommands")) {
            udsCommandMap.insert(std::make_pair<ara::core::String, UdsCommand>(
                item.first, {as_byte_vector(item.second, "command"), as_byte_vector(item.second, "positiveResponse")}));
        }

        if (type == ReprogrammingSequenceType::SingleBankEcu) {
            return std::make_unique<ProgConf>(
                fileList, rollbackfileList, type, udsCommandMap, canRequestId, canResponseId, name, version);
        } else if (type == ReprogrammingSequenceType::DualBankEcu) {
            return std::make_unique<ProgConf>(
                fileList, type, udsCommandMap, canRequestId, canResponseId, name, version);
        } else {
            return nullptr;
        }

    } catch (const ptree_error& parseError) {
        logger_.LogError() << "Error parsing programming configuration" << parseError.what();
        return nullptr;
    }
}

std::unique_ptr<ProgConf> ProgConfParser::ParseFromFile(const ara::core::String& progConfPath)
{
    boost::property_tree::ptree progConfTree;

    try {
        boost::property_tree::read_json(progConfPath.c_str(), progConfTree);
    } catch (const boost::property_tree::file_parser_error& error) {
        const ara::core::String msg = ara::core::String("cannot parse json file ") + ara::core::String(error.what());
        logger_.LogError() << msg;
    }

    return Parse(progConfTree);
}
