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

#include "cmd_handler.h"
#include "logger.h"
#include "smtool_definitions.h"
#include "trigger_in.h"
#include "trigger_out.h"

#include <algorithm>
#include <exception>
#include <cstdint>
#include <string>

namespace apd
{
namespace smtool
{

namespace
{

auto& GetLogger()
{
    static Logger logger("CommandHandler", LogLevel::kTrace);
    return logger;
}

}  // namespace

void CommandHandler::HandleCommand(const Command& command)
{
    switch (command.GetType()) {
    case CommandType::kSetState:
        GetLogger().LogDebug() << "Received setstate command\n";
        handleSetStateCmd(command.GetOptions(), command.GetParameters());
        break;
    case CommandType::kGetState:
        GetLogger().LogDebug() << "Received getstate command\n";
        handleGetStateCmd(command.GetOptions());
        break;

    case CommandType::kGetFunctionGroups:
        GetLogger().LogDebug() << "Received getfgs command\n";
        handleGetFunctionGroupsCmd(command.GetOptions());
        break;

    default:
        GetLogger().LogError() << "Received incorrect command\n";
        break;
    }
}

void CommandHandler::handleSetStateCmd(const Command::TCmdOptions& options, const Command::TCmdParameters& parameters)
{
    GetLogger().LogTrace() << __FUNCTION__ << " called\n";

    if (parameters.empty()) {
        GetLogger().LogError() << "Malformed command received: missing state name\n";
        return;
    }

    ara::core::String message;
    message += GetFunctionGroupId(options);
    message += ".";
    message += parameters.front();

    TriggerIn triggerIn;
    TriggerOut triggerOut;

    triggerIn.SendSetStateRequest(message);

    constexpr std::chrono::seconds kTimeout{35};
    if (triggerOut.WaitForStateChange(message, kTimeout)) {
        GetLogger().LogInfo() << "State changed successfully\n";
    } else {
        GetLogger().LogError() << "State change failure.\n";
    }
}

void CommandHandler::handleGetStateCmd(const Command::TCmdOptions& options)
{
    GetLogger().LogTrace() << __FUNCTION__ << " called\n";

    ara::core::String message;
    message += ara::core::String(std::to_string(static_cast<std::uint32_t>(SmtoolCommand::kGetState)));
    message += kSmtoolMessageDelimiter;

    message += GetFunctionGroupId(options);
    message += kSmtoolMessageDelimiter;

    if (!smtoolClient_.SendRequest(message)) {
        // request could not be sent - no use in waiting for a response
        GetLogger().LogError() << "Failed to obtain function group state\n";
        return;
    }

    ara::core::String response = smtoolClient_.ReceiveResponse();
    if (!response.empty()) {
        GetLogger().LogInfo() << "Function group state: " << response << "\n";
    } else {
        GetLogger().LogError() << "Failed to obtain function group state\n";
    }
}

void CommandHandler::handleGetFunctionGroupsCmd(const Command::TCmdOptions& options)
{
    GetLogger().LogTrace() << __FUNCTION__ << " called\n";

    ara::core::String message;

    const auto& verboseOutputOption = std::find_if(options.cbegin(), options.cend(), [](const ShortOption& option) {
        return (option.optionSymbol == ShortOptionSymbol::kVerboseOutput);
    });

    if ((verboseOutputOption != options.cend())) {
        message += ara::core::String(std::to_string(static_cast<std::uint32_t>(SmtoolCommand::kGetFgsStates)));
    } else {
        message += ara::core::String(std::to_string(static_cast<std::uint32_t>(SmtoolCommand::kGetFunctionGroups)));
    }

    message += kSmtoolMessageDelimiter;
    smtoolClient_.SendRequest(message);
}

ara::core::String CommandHandler::GetFunctionGroupId(const Command::TCmdOptions& options)
{
    static const ara::core::String kDefaultFunctionGroupId = "MachineFG";

    const auto& functionGroupIdOption = std::find_if(options.cbegin(), options.cend(), [](const ShortOption& option) {
        return (option.optionSymbol == ShortOptionSymbol::kFunctionGroupIdentifier);
    });

    if ((functionGroupIdOption != options.cend()) && functionGroupIdOption->optionParameter) {
        return *(functionGroupIdOption->optionParameter);
    }

    return kDefaultFunctionGroupId;
}

}  // namespace smtool
}  // namespace apd
