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
#include <chrono>
#include <condition_variable>
#include <cstdlib>
#include <memory>

#include "ara/com/internal/runtime.h"
#include "ara/core/initialization.h"
#include "ara/core/string.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "ultrasonicsensor_app.h"

#ifdef PICAR
#    include <thread>
void test(ara::log::Logger& logger);

void test(ara::log::Logger& logger)
{
    std::unique_ptr<UltrasonicSensor> sensor = UltrasonicSensor::CreateSensor();
    for (int i = 0; i < 100; i++) {
        logger.LogInfo() << "Distance: " << sensor->GetDistance();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
#endif

int main(int ac, char** av)
{

    ara::core::Result<void> initSuccess = ara::core::Initialize();
    if (!initSuccess) {
        return 1;
    }

    auto& logger = ara::log::CreateLogger("MAIN", "UltraSonic sensor main", ara::log::LogLevel::kVerbose);

    logger.LogInfo() << "ULTRASONIC SENSOR READER STARTS";
    logger.LogInfo() << "===========================";

    if (ac == 2 && strncmp(av[1], "--test", 6) == 0) {
#ifdef PICAR
        test(logger);
#else
        logger.LogError() << "Test mode is only supported for picar machine";
#endif
        return 0;
    }

    // report application state
    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    logger.LogInfo() << "Starting UltraSonic Sensor Provider Application";

    apd::picar::ultrasonicsensor::UltraSonicSensorApp app;
    app.Execute();

    logger.LogInfo() << "Terminating UltraSonic Sensor Provider Application.";
    ara::core::Deinitialize();

    return 0;
}
