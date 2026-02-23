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

#include "tc01.h"

#include "apd/wgres/modeledmsgs/messages.h"
#include <ara/core/initialization.h>
#include <ara/log/logger.h>

#include <chrono>
#include <thread>

namespace
{

ara::log::Logger& get_logger()
{
    static ara::log::Logger& logger = ara::log::CreateLogger("TC01", "WG-RES TC01", ara::log::LogLevel::kVerbose);
    return logger;
}
void emit_log_message()
{
    get_logger().Log(apd::wgres::modeledmsgs::Test1Msg, 0);
}
void log_periodic()
{
    auto time{std::chrono::steady_clock::now()};
    for (int i = 0; i < 5; ++i) {
        using std::chrono::operator""ms;
        time += 1000ms;
        emit_log_message();
        std::this_thread::sleep_until(time);
    }
}
std::unique_ptr<std::thread> start_log_periodic()
{
    return std::make_unique<std::thread>([&]() { log_periodic(); });
}
}  // namespace
void testcase1()
{
    get_logger().LogInfo() << "test case 01";

    std::unique_ptr<std::thread> logThread = start_log_periodic();
    if (logThread != nullptr) {
        logThread->join();
        logThread.reset();
    }
}
