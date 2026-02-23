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

#include "ara/log/logger.h"

#include "ara/core/string.h"
#include "ara/core/initialization.h"
#include "ara/core/instance_specifier.h"

#include "ara/exec/execution_client.h"

#include "ara/per/key_value_storage.h"
#include "ara/per/kvstype/kvs_type.h"
#include "ara/per/manifestaccess/manifest_helper.h"

using namespace ara::per;

using ara::core::String;
using ara::per::kvstype::KvsType;

constexpr auto kDatabasePerPort
    = "PersistencyRedundancyDemo/PersistencyRedundancyDemo_RootSwComp/PersistencyRedundancyDemo_PRPort";

static void TraverseKVSDatabase(SharedHandle<KeyValueStorage> db, ara::log::Logger& logger)
{
    auto allKeysResult = db->GetAllKeys();
    if (!allKeysResult) {
        logger.LogError() << "Failed to get all keys!";
        return;
    }

    auto allKeys = std::move(allKeysResult).Value();

    for (const auto& key : allKeys) {
        logger.LogInfo() << "key: " << key;

        if (key == "key-negative-int") {
            auto int_result = db->GetValue<int16_t>("key-negative-int");
            if (int_result)
                logger.LogInfo() << "Value: " << int_result.Value();
        } else if (key == "key-float") {
            float double_value = db->GetValue<float>(key).ValueOr(0);
            logger.LogInfo() << "Value: " << double_value;
        } else if (key == "key-string") {
            auto String_value = db->GetValue<ara::core::String>(key);
            if (String_value)
                logger.LogInfo() << "Value: " << String_value.Value();
        } else if (key == "key-uint64") {
            auto uint64_t_value = db->GetValue<uint64_t>(key).ValueOr(0);
            logger.LogInfo() << "Value: " << uint64_t_value;
        } else if (key == "key-bool") {
            auto bool_value = db->GetValue<bool>(key);
            if (bool_value)
                logger.LogInfo() << "Value: " << bool_value.Value();
        }
    }
}

int main()
{
    if (!ara::core::Initialize()) {
        ara::core::Abort("Demo Persistency Redundancy initialization failed.");
    };

    ara::log::Logger& logger = ara::log::CreateLogger("PER", "Demo Redundancy Persistency", ara::log::LogLevel::kInfo);

    logger.LogInfo() << "KVS redundancy demo starts";

    ara::exec::ExecutionClient execClient;
    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto kvs = OpenKeyValueStorage(ara::core::InstanceSpecifier(kDatabasePerPort));
    if (kvs) {
        TraverseKVSDatabase(std::move(kvs).Value(), logger);
    } else {
        logger.LogError() << "Failed to open database due to: " << kvs.Error().Message();
    }

    logger.LogInfo() << "End of KVS redundancy demo";

    if (!ara::core::Deinitialize()) {
        ara::core::Abort("Demo Persistency Redundancy deinitialization failed.");
    }

    return 0;
}
