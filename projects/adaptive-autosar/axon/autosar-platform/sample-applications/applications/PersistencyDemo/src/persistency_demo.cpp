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
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include "ara/per/key_value_storage.h"
#include "ara/per/per_error_domain.h"

#include "ara/core/instance_specifier.h"

using namespace ara::per;

constexpr auto kDatabasePerPort = "PersistencyDemo/PersistencyDemo/PersistencyKvsDemo_RPPort";

// This function shows how the KeyValueStorage can be used
static void writeToKvsStore(ara::log::Logger& logger)
{
    auto result = OpenKeyValueStorage(ara::core::InstanceSpecifier(kDatabasePerPort));

    if (!result) {
        logger.LogError() << "Failed to create key-value-storage due to: " << result.Error().Message();
        return;
    }

    auto db = std::move(result).Value();
    db->RemoveAllKeys();

    // Basic type usage
    // It's not needed to create a temporary object, it's also possible just to add it on the fly.
    db->SetValue("hello-key-value-storage", 12345);
    db->SetValue("hello-key-negative-int", -12345);
    db->SetValue("hello-key-float", static_cast<float>(1.234567));
    db->SetValue("hello-key-double", static_cast<double>(1.234567));
    db->SetValue("hello-key-string", ara::core::String("hello-adaptive!"));
    db->SetValue("hello-key-negative-int64", -12345123412341234);
    db->SetValue("hello-key-int64", 12345123412341234);
    db->SetValue("hello-key-bool", bool(true));
    db->SetValue("hello-key-uint64", static_cast<uint64_t>(18446744073709551615UL));

    db->SyncToStorage();
}

static void readFromKvsStore(ara::log::Logger& logger)
{
    auto result = OpenKeyValueStorage(ara::core::InstanceSpecifier(kDatabasePerPort));
    if (!result) {
        logger.LogError() << "Failed to create key-value-storage due to: " << result.Error().Message();
        return;
    }

    // now the db object contains the data that was written there in the past
    auto db = std::move(result).Value();

    logger.LogInfo() << "KEY VALUE STORAGE READ DEMO STARTS";

    // Simple case
    auto int_result = db->GetValue<int16_t>("hello-key-value-storage");

    // Things to check
    if (int_result) {
        logger.LogInfo() << "The read value is: "
                         << "hello-key-value-storage"
                         << " : " << int_result.Value();
    }

    // the data can of course be retrieved also without any checks if
    // you know what to expect.
    double double_value = db->GetValue<double>("hello-key-double").ValueOr(0);
    logger.LogInfo() << "read value with no checks : "
                     << "hello-key-double"
                     << " : " << double_value;

    auto missing_result = db->GetValue<bool>("This key does not exist");
    if (!missing_result && missing_result.CheckError(PerErrc::kKeyNotFound)) {
        logger.LogInfo() << "read value "
                         << "This key does not exist"
                         << " : "
                         << "the value is missing";
    }

    logger.LogInfo() << "END OF KEY VALUE STORAGE READ DEMO";
}

static void OtherFunctions(ara::log::Logger& logger)
{
    logger.LogInfo() << "KeyValueStorage other functions demo start";

    auto result = OpenKeyValueStorage(ara::core::InstanceSpecifier(kDatabasePerPort));
    if (!result) {
        logger.LogError() << "Failed to create key-value-storage due to: " << result.Error().Message();
        return;
    }

    auto db = std::move(result).Value();

    if (db->KeyExists("hello-key-string").Value()) {
        logger.LogInfo() << "removing key : "
                         << "hello-key-string";
        db->RemoveKey("hello-key-string");
    }

    if (db->KeyExists("hello-key-string").Value()) {
        logger.LogInfo() << "key wasn't removed?!?";
    } else {
        logger.LogInfo() << "Key was successfully removed.";
    }
    db->SyncToStorage();

    logger.LogInfo() << "KeyValueStorage - trying to open a non-existing db";

    auto expected_failure = OpenKeyValueStorage(ara::core::InstanceSpecifier("tmp/this_does_not_exist"));

    if (expected_failure) {
        logger.LogInfo() << "Not expected: the database was opened!?";
    } else {
        auto error = expected_failure.Error();
        logger.LogInfo() << "Expected error:" << error.Message();
    }

    logger.LogInfo() << "KeyValueStorage other functions demo ends";
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    ara::core::Result<void> initSuccess = ara::core::Initialize();
    if (!initSuccess) {
        return 1;
    }
    ara::log::Logger& logger = ara::log::CreateLogger("PER", "Persistency Demo", ara::log::LogLevel::kVerbose);

    logger.LogInfo() << "Starting Persistency Demo...";

    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    writeToKvsStore(logger);
    readFromKvsStore(logger);
    OtherFunctions(logger);

    logger.LogInfo() << "Shutting down Demo Persistency...";
    ara::core::Result<void> shutdownSuccess = ara::core::Deinitialize();
    if (!shutdownSuccess) {
        return 1;
    }

    return 0;
}
