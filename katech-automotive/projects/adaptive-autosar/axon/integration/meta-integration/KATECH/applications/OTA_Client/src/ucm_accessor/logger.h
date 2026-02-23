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

/// @file
/// @brief Logger functionality to simplify logger calls

#ifndef IAM_TEST_APP_LOGGER_H_
#define IAM_TEST_APP_LOGGER_H_

#include "ara/log/logging.h"

namespace apd
{
namespace iamtest
{

/// @brief Access global Demo FC logger instance
ara::log::Logger& GetIamTestAppLogger();

#if defined(LOGGING_NO_FILE_LINE)
#    define FILE_LINE ' '
#else
#    define FILE_LINE "[" << __FILE__ << "-" << __LINE__ << "] "
#endif

// Makro for DLT logging at log level "Fatal".
#define IAMTESTAPP_LOGFATAL(x) GetIamTestAppLogger().LogFatal() << FILE_LINE << x;
// Makro for DLT logging at log level "Error".
#define IAMTESTAPP_LOGERROR(x) GetIamTestAppLogger().LogError() << FILE_LINE << x;
// Makro for DLT logging at log level "Warn".
#define IAMTESTAPP_LOGWARN(x) GetIamTestAppLogger().LogWarn() << FILE_LINE << x;
// Makro for DLT logging at log level "Info".
#define IAMTESTAPP_LOGINFO(x) GetIamTestAppLogger().LogInfo() << FILE_LINE << x;
// Makro for DLT logging at log level "Debug".
#define IAMTESTAPP_LOGDEBUG(x) GetIamTestAppLogger().LogDebug() << FILE_LINE << x;
// Makro for DLT logging at log level "Verbose".
#define IAMTESTAPP_LOGVERBOSE(x) GetIamTestAppLogger().LogVerbose() << FILE_LINE << x;

}  // namespace iamtest
}  // namespace apd

#endif  // IAM_TEST_APP_LOGGER_H_
