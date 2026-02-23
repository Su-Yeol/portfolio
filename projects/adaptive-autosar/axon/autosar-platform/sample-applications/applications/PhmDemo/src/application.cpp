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

// this file is just for the purpose of the demonstration, they are not needed in production code
#include <chrono>
#include <thread>
#include <typeinfo>
#include <ara/log/logger.h>
#include "ara/phm/supervised_entity.h"
#include "ara/core/abort.h"
#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"

#include "supervised_entities/engine.h"
#include "supervised_entities/wheel.h"
#include "supervised_entities/motor.h"

// namespace with non-generated phm code
using namespace ara::phm;

// namespaces with the generated code
using namespace ara::phm::supervised_entities;
using namespace ara::log;

constexpr auto kLongLine = "---------------------------------------------------------------------";

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (!ara::core::Initialize()) {
        ara::core::Abort("PhmDemo initialization failed.");
    };
    ara::log::Logger& logger = ara::log::CreateLogger("PHMDemo", "PHM demo Context", ara::log::LogLevel::kVerbose);

    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    logger.LogInfo() << "PHM Demo";
    logger.LogInfo()
        << "All three Supervisions and its LocalSupevisionStatus are demonstrated by sending respective checkpoints";

    SupervisedEntity<engine::Checkpoints> engine0(engine::engineInstance);

    // example 1: single prototype of SE (engine0) with 3 checkpoints
    logger.LogInfo() << kLongLine;
    logger.LogInfo() << "example 1: single prototype of SE (engine) with 3 checkpoints";
    logger.LogInfo() << "Logical Supervision use case";

    engine0.ReportCheckpoint(engine::Checkpoints::kInitializing);
    engine0.ReportCheckpoint(engine::Checkpoints::kStartupTest);
    engine0.ReportCheckpoint(engine::Checkpoints::kInitializingFinished);

    // example 2: four prototypes of the same SE, each with 2 checkpoints
    logger.LogInfo() << kLongLine;
    logger.LogInfo() << "example 2: four prototypes of the same SE (wheel), each with 4 checkpoints";
    logger.LogInfo() << "Deadline Supervision use case: valid time frame 0ms - 100ms";

    SupervisedEntity<wheel::Checkpoints> wheel0(wheel::wheel0Instance);

    logger.LogInfo() << "- prototype 0 - 30ms";
    wheel0.ReportCheckpoint(wheel::Checkpoints::kStarted);
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    wheel0.ReportCheckpoint(wheel::Checkpoints::kFinished);

    SupervisedEntity<wheel::Checkpoints> wheel1(wheel::wheel1Instance);

    logger.LogInfo() << "- prototype 1 - 50ms";
    wheel1.ReportCheckpoint(wheel::Checkpoints::kStarted);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    wheel1.ReportCheckpoint(wheel::Checkpoints::kFinished);

    SupervisedEntity<wheel::Checkpoints> wheel2(wheel::wheel2Instance);

    logger.LogInfo() << "- prototype 2 - 70ms";
    wheel2.ReportCheckpoint(wheel::Checkpoints::kStarted);
    std::this_thread::sleep_for(std::chrono::milliseconds(70));
    wheel2.ReportCheckpoint(wheel::Checkpoints::kFinished);

    SupervisedEntity<wheel::Checkpoints> wheel3(wheel::wheel3Instance);

    logger.LogInfo() << "Alive Supervision use case - 6 cycles";
    wheel3.ReportCheckpoint(wheel::Checkpoints::kFinished);
    for (int i = 0; i < 6; i++) {
        logger.LogInfo() << "Cycle" << (i + 1) << "- cycle time:" << 8 << "ms";
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
        wheel3.ReportCheckpoint(wheel::Checkpoints::kFinished);
    }

    // Deadline supervision use case to check Recovery action via application functionality
    logger.LogInfo() << kLongLine;
    logger.LogInfo() << "Single prototype of SE (motor) with 2 checkpoints";
    logger.LogInfo() << "Deadline supervision use case: valid time frame 0ms - 50ms";

    SupervisedEntity<motor::Checkpoints> motor(motor::motorInstance);

    motor.ReportCheckpoint(motor::Checkpoints::kStart);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    motor.ReportCheckpoint(motor::Checkpoints::kFinish);

    if (!ara::core::Deinitialize()) {
        ara::core::Abort("PhmDemo deinitialization failed.");
    }

    return 0;
}
