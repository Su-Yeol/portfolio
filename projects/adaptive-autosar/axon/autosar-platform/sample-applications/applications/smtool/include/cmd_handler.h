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

#ifndef APD_SMTOOL_CMD_HANDLER_H_
#define APD_SMTOOL_CMD_HANDLER_H_

#include "command.h"
#include "smtool_client.h"

#include "ara/core/string.h"

namespace apd
{
namespace smtool
{

/// @brief Class used for execution of user requests
class CommandHandler
{
public:
    CommandHandler() = default;
    ~CommandHandler() = default;

    CommandHandler(const CommandHandler&) = delete;
    CommandHandler(CommandHandler&&) = delete;
    CommandHandler& operator=(const CommandHandler&) = delete;
    CommandHandler& operator=(CommandHandler&&) = delete;

    void HandleCommand(const Command& command);

private:
    void handleSetStateCmd(const Command::TCmdOptions& options, const Command::TCmdParameters& parameters);
    void handleGetStateCmd(const Command::TCmdOptions& options);
    void handleGetFunctionGroupsCmd(const Command::TCmdOptions& options);
    ara::core::String GetFunctionGroupId(const Command::TCmdOptions& options);

    SmtoolClient smtoolClient_;
};

}  // namespace smtool
}  // namespace apd

#endif  // APD_SMTOOL_CMD_HANDLER_H_
