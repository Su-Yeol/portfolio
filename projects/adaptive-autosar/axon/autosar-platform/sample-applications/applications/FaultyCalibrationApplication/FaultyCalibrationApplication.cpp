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

#include "ara/core/initialization.h"
#include "ara/log/logger.h"

#include <chrono>
#include <thread>
#include <iostream>

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger
        = ara::log::CreateLogger("FCAP", "Faulty Calibration Application", ara::log::LogLevel::kInfo);
    return logger;
}

int main()
{
    if (!ara::core::Initialize()) {
        std::cerr << "Failed to initialize. Aborting." << std::endl;
        return 1;
    }
    while (1) {
        std::this_thread::sleep_for(std::chrono::seconds(3));
        GetLogger().LogInfo() << "Faulty app is here.";
    }

    if (!ara::core::Deinitialize()) {
        std::cerr << "Failed to deinitialize." << std::endl;
    }
}
