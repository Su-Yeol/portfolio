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

#ifndef APD_SMTOOL_COMMAND_H_
#define APD_SMTOOL_COMMAND_H_

#include "ara/core/map.h"
#include "ara/core/optional.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"

namespace apd
{
namespace smtool
{

enum class CommandType
{
    kSetState,
    kGetState,
    kGetFunctionGroups,
    kUndefined
};

enum class ShortOptionSymbol : char
{
    kInvalid = '\0',
    kFunctionGroupIdentifier = 'g',
    kVerboseOutput = 'v'
};

struct ShortOption
{
    ShortOptionSymbol optionSymbol;
    ara::core::Optional<ara::core::String> optionParameter;
};

struct CommandRestrictions
{
    size_t parametersCount;
    ara::core::Vector<ShortOptionSymbol> supportedOptions;
};

class Command
{
public:
    using TCmdOptions = ara::core::Vector<ShortOption>;
    using TCmdParameters = ara::core::Vector<ara::core::String>;

    explicit Command(CommandType type);
    ~Command() = default;

    Command(const Command&) = delete;
    Command(Command&&) = delete;
    Command& operator=(const Command&) = delete;
    Command& operator=(Command&&) = delete;

    void AddOption(ShortOption&& option);
    void AddParameter(ara::core::String&& parameter);
    CommandType GetType() const noexcept;
    const TCmdOptions& GetOptions() const noexcept;
    const TCmdParameters& GetParameters() const noexcept;
    bool IsValid() const;

    static bool IsOptionParameterRequired(ShortOptionSymbol optionSymbol);

private:
    CommandType type_;
    TCmdOptions options_;
    TCmdParameters parameters_;
    static const ara::core::Map<CommandType, CommandRestrictions> supportedCommands_;
};

}  // namespace smtool
}  // namespace apd

#endif  // APD_SMTOOL_COMMAND_H_
