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

#include "helper.h"
#include "logger.h"

namespace apd
{
namespace smtool
{

namespace
{

auto& GetLogger()
{
    static Logger logger("Helper", LogLevel::kTrace);
    return logger;
}

}  // namespace

void Helper::ShowHelp()
{
    static const std::string usageInfo = R"(Usage:
smtool setstate <StateName>                               # set state of MachineFG
smtool setstate -g <FunctionGroupIdentifier> <StateName>  # set state of a chosen function group
smtool getstate                                           # show state of MachineFG
smtool getstate -g <FunctionGroupIdentifier>              # show state of a chosen function group
smtool getfgs                                             # show all function groups
smtool getfgs -v                                          # show all function groups including their states
)";

    GetLogger().LogInfo() << usageInfo;
}

}  // namespace smtool
}  // namespace apd
