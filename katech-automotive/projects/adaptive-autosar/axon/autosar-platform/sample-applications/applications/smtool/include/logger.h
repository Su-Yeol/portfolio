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

#ifndef APD_SMTOOL_LOGGER_H_
#define APD_SMTOOL_LOGGER_H_

#include "ara/core/string.h"
#include "ara/core/string_view.h"

#include <iostream>

namespace apd
{
namespace smtool
{

enum class LogLevel
{
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kError = 3,
    kLogDisabled = 4
};

class Logger
{
public:
    using TOutputStream = std::ostream;

    Logger(ara::core::StringView identifier, LogLevel logLevel);
    ~Logger() = default;

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    TOutputStream& LogTrace();
    TOutputStream& LogDebug();
    TOutputStream& LogInfo();
    TOutputStream& LogError();

private:
    TOutputStream& PrepareStream(LogLevel logLevel, ara::core::StringView logLevelStr);

    const ara::core::String identifier_;
    const LogLevel logLevel_;
    TOutputStream nullStream_;
    TOutputStream& outputStream_ = std::cerr;
};

}  // namespace smtool
}  // namespace apd

#endif  // APD_SMTOOL_LOGGER_H_
