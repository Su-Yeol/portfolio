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

#include <thread>
using std::this_thread::sleep_for;
#include <chrono>
using namespace std::literals::chrono_literals;

#ifndef NO_ARAEXEC
#    include <ara/exec/execution_client.h>
#endif

#include "ara/diag/diagnostic_manager/securityaccess_skeleton.h"

using Skeleton = ara::diag::diagnostic_manager::skeleton::SecurityAccessSkeleton;
using GetSeedOutput = ara::diag::diagnostic_manager::SecurityAccess::GetSeedOutput;
using CompareKeyOutput = ara::diag::diagnostic_manager::SecurityAccess::CompareKeyOutput;
using ara::diag::KeyCompareResultType;
using ara::diag::MetaInfoType;
using ara::diag::ByteVectorType;

class SeedKeyImpl : public Skeleton
{
public:
    SeedKeyImpl()
        : Skeleton(::ara::com::InstanceIdentifier("SOME/IP:11"),
            ::ara::com::MethodCallProcessingMode::kEventSingleThread)
    { }
    auto GetSeed(const ByteVectorType& dataRecord, const MetaInfoType& metaInfo)
        -> decltype(Skeleton::GetSeed(dataRecord, metaInfo)) override;
    auto CompareKey(const ByteVectorType& key, const MetaInfoType& metaInfo)
        -> decltype(Skeleton::CompareKey(key, metaInfo)) override;
    auto Cancel(const MetaInfoType& metaInfo) -> decltype(Skeleton::Cancel(metaInfo)) override;
    using Skeleton::OfferService;
};

auto SeedKeyImpl::GetSeed(const ByteVectorType& dataRecord, const MetaInfoType& metaInfo)
    -> decltype(Skeleton::GetSeed(dataRecord, metaInfo))
{
    decltype(Skeleton::GetSeed(dataRecord, metaInfo))::PromiseType promise;

    GetSeedOutput output{{'t', 'e', 's', 't'}};
    promise.set_value(output);
    return promise.get_future();
}

auto SeedKeyImpl::CompareKey(const ByteVectorType& key, const MetaInfoType& metaInfo)
    -> decltype(Skeleton::CompareKey(key, metaInfo))
{
    decltype(Skeleton::CompareKey(key, metaInfo))::PromiseType promise;

    ByteVectorType expected{'p', 'a', 's', 's', 'e', 'd'};
    CompareKeyOutput output{key == expected ? KeyCompareResultType::kKeyValid : KeyCompareResultType::kKeyInvalid};
    promise.set_value(output);
    return promise.get_future();
}

auto SeedKeyImpl::Cancel(const MetaInfoType& metaInfo) -> decltype(Skeleton::Cancel(metaInfo))
{
    decltype(Skeleton::Cancel(metaInfo))::PromiseType promise;

    promise.set_value();
    return promise.get_future();
}

int main()
{
#ifndef NO_ARAEXEC
    // report application state
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);
#endif

    SeedKeyImpl impl;
    impl.OfferService();

    while (true)
        sleep_for(1s);
}
