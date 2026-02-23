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

#include "command.h"
#include "logger.h"

#include <algorithm>

namespace apd
{
namespace smtool
{

namespace
{

auto& GetLogger()
{
    static Logger logger("Command", LogLevel::kTrace);
    return logger;
}

}  // namespace

const ara::core::Map<CommandType, CommandRestrictions> Command::supportedCommands_
    = {{CommandType::kSetState, {1u, {ShortOptionSymbol::kFunctionGroupIdentifier}}},
        {CommandType::kGetState, {0u, {ShortOptionSymbol::kFunctionGroupIdentifier}}},
        {CommandType::kGetFunctionGroups, {0u, {ShortOptionSymbol::kVerboseOutput}}}};

Command::Command(CommandType type)
    : type_(type)
{ }

CommandType Command::GetType() const noexcept
{
    return type_;
}

const Command::TCmdOptions& Command::GetOptions() const noexcept
{
    return options_;
}

const Command::TCmdParameters& Command::GetParameters() const noexcept
{
    return parameters_;
}

void Command::AddOption(ShortOption&& option)
{
    options_.push_back(option);
}

void Command::AddParameter(ara::core::String&& parameter)
{
    parameters_.push_back(parameter);
}

bool Command::IsValid() const
{
    const auto& commandIt = supportedCommands_.find(type_);
    if (commandIt == supportedCommands_.cend()) {
        GetLogger().LogError() << "Received command is not supported\n";
        return false;
    }

    const CommandRestrictions& restrictions = commandIt->second;
    const ara::core::Vector<ShortOptionSymbol>& supportedOptions = restrictions.supportedOptions;

    if (parameters_.size() != restrictions.parametersCount) {
        GetLogger().LogError() << "Command invalid: incorrect parameters count\n";
        return false;
    }

    if (options_.size() > supportedOptions.size()) {
        GetLogger().LogError() << "Command invalid: incorrect options count\n";
        return false;
    }

    for (const ShortOption& option : options_) {
        const auto supportedOptionIt
            = std::find(supportedOptions.cbegin(), supportedOptions.cend(), option.optionSymbol);

        if (supportedOptionIt == supportedOptions.cend()) {
            GetLogger().LogError() << "Command invalid: option is not supported\n";
            return false;
        }

        if (IsOptionParameterRequired(option.optionSymbol) != option.optionParameter.has_value()) {
            GetLogger().LogError() << "Command invalid: option parameter is missing or not supported\n";
            return false;
        }
    }

    return true;
}

bool Command::IsOptionParameterRequired(ShortOptionSymbol optionSymbol)
{
    return (optionSymbol == ShortOptionSymbol::kFunctionGroupIdentifier);
}

}  // namespace smtool
}  // namespace apd
