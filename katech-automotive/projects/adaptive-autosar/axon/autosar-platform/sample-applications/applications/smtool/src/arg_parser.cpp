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

#include "arg_parser.h"
#include "logger.h"

#include "ara/core/map.h"

namespace apd
{
namespace smtool
{

namespace
{

auto& GetLogger()
{
    static Logger logger("ArgParser", LogLevel::kTrace);
    return logger;
}

}  // namespace

ArgParser::ArgParser(int argc, char* argv[])
{
    constexpr size_t kCommandTypePosition = 1u;

    for (size_t argIndex = kCommandTypePosition; argIndex < static_cast<size_t>(argc); ++argIndex) {
        arguments_.emplace_back(argv[argIndex]);
    }
}

void ArgParser::HandleParsingError(const ara::core::String& errorDescription)
{
    parsingFailed_ = true;

    if (command_) {
        command_.reset();
    }

    GetLogger().LogError() << errorDescription;
}

void ArgParser::ParseArgs()
{
    static const ara::core::Map<ara::core::StringView, CommandType> commands = {{"setstate", CommandType::kSetState},
        {"getstate", CommandType::kGetState},
        {"getfgs", CommandType::kGetFunctionGroups}};

    if (command_ || parsingFailed_) {
        // parsing already done
        return;
    }

    if (!HasNextArgument()) {
        HandleParsingError("No command received\n");
        return;
    }

    const auto it = commands.find(GetNextArgument());
    if (it == commands.cend()) {
        HandleParsingError("Incorrect command received\n");
        return;
    }
    command_.emplace(it->second);

    while (HasNextArgument() && !parsingFailed_) {
        ara::core::StringView currentArgument = GetNextArgument();

        if (IsShortOption(currentArgument)) {
            ParseShortOption(currentArgument);
        } else {
            switch (command_->GetType()) {
            case CommandType::kSetState:
                command_->AddParameter(ara::core::String(currentArgument));
                break;

            default:
                HandleParsingError("Incorrect parameter(s) received\n");
                return;
            }
        }
    }

    if (!parsingFailed_ && !command_->IsValid()) {
        HandleParsingError("Incorrect option(s)/parameter(s) received\n");
    }
}

const ara::core::Optional<Command>& ArgParser::GetCommand() const noexcept
{
    return command_;
}

void ArgParser::ParseShortOption(const ara::core::StringView argument)
{
    constexpr size_t kShortOptionStringLength = 2u;
    constexpr size_t kShortOptionSymbolPosition = 1u;

    if (argument.size() != kShortOptionStringLength) {
        HandleParsingError("Incorrect option length\n");
        return;
    }

    const char shortOptionSymbolChar = argument[kShortOptionSymbolPosition];
    switch (shortOptionSymbolChar) {
    case static_cast<char>(ShortOptionSymbol::kVerboseOutput):
    case static_cast<char>(ShortOptionSymbol::kFunctionGroupIdentifier): {
        ShortOption shortOption;
        shortOption.optionSymbol = static_cast<ShortOptionSymbol>(shortOptionSymbolChar);

        if (Command::IsOptionParameterRequired(shortOption.optionSymbol)) {
            const ara::core::StringView nextArgument = GetNextArgument();
            if (!nextArgument.empty()) {
                shortOption.optionParameter.emplace(nextArgument);
            } else {
                HandleParsingError("Missing option parameter\n");
                break;
            }
        }
        command_->AddOption(std::move(shortOption));
        break;
    }

    default:
        HandleParsingError("Option received is not supported\n");
        break;
    }
}

bool ArgParser::IsShortOption(const ara::core::StringView argument) const
{
    constexpr char kShortOptionStart = '-';

    return (argument.front() == kShortOptionStart);
}

bool ArgParser::HasNextArgument() const noexcept
{
    return (currentPosition_ < arguments_.size());
}

ara::core::StringView ArgParser::GetNextArgument()
{
    if (HasNextArgument()) {
        return arguments_[currentPosition_++];
    }

    return "";
}

}  // namespace smtool
}  // namespace apd
