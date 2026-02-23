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

#include <future>
#include <mutex>

#include "find_service.h"
#include "ara/log/logger.h"
#include "ara/core/map.h"
#include <condition_variable>

namespace apd
{
namespace ucm
{
namespace pkgmgrsample
{

using ara::ucm::proxy::PackageManagementProxy;

namespace internal
{

using OptionalHandle = ara::core::Optional<PackageManagementProxy::HandleType>;

static OptionalHandle MatchServiceCallback(ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType> handles,
    const ara::core::String& identifier,
    ara::log::Logger& logger)
{
    OptionalHandle ret;
    for (const auto& handle : handles) {
        const auto subordinatePtr = std::make_shared<PackageManagementProxy>(handle);
        const auto subordinateId = subordinatePtr->GetId().get().id;
        logger.LogInfo() << "[Connect] PackageManagement instance " << handle.GetInstanceId().ToString()
                         << " with UCM Id:" << subordinateId;
        if (subordinateId.compare(identifier) == 0) {
            logger.LogDebug() << "[Connect] It's a match! <3";
            ret = handle;
            break;
        }
    }
    return ret;
}

static void CompareIdThread(ara::log::Logger& logger,
    const ara::core::String identifier,
    std::promise<void>& signalPromise,
    std::condition_variable& cond,
    bool& continueRunning,
    std::mutex& serviceInstancesAccessMutex,
    ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType>& compareHandles,
    OptionalHandle& matchingHandle)
{
    std::unique_lock<std::mutex> lk(serviceInstancesAccessMutex);
    while (continueRunning) {
        logger.LogInfo() << "connect: waiting for instances";
        cond.wait(
            lk, [&logger, &continueRunning, &compareHandles] { return !continueRunning || !compareHandles.empty(); });
        // search id in currently available instances
        matchingHandle = MatchServiceCallback(compareHandles, identifier, logger);
        if (matchingHandle) {
            signalPromise.set_value();
            continueRunning = false;
        } else {
            logger.LogDebug() << "[Connect] This is not the PackageManagement service you are looking for...";
        }
        logger.LogInfo() << "connect: Clearing service instances";
        compareHandles.clear();
    }
    logger.LogInfo() << "connect: stopping the id compare thread";
    lk.unlock();
}

}  // namespace internal

ara::core::Optional<ServiceProxyPtr> FindPackageManagementService(const ara::core::InstanceSpecifier& ucmInstance,
    const ara::core::String& ucmIdentifier)
{

    std::promise<void> signalPromise;
    std::condition_variable cond;
    bool continueRunning(true);
    std::mutex serviceInstancesAccessMutex;
    ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType> foundHandles;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger("FIND", "Find Service Callback context", ara::log::LogLevel::kVerbose);

    internal::OptionalHandle serviceInstanceHandle;

    logger.LogInfo() << "[Connect] connecting to the PackageManagement service...";

    const auto result = PackageManagementProxy::StartFindService(
        [&signalPromise, &logger, &ucmIdentifier, &serviceInstancesAccessMutex, &foundHandles, &cond](
            ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static_cast<void>(findServiceHandle);  // unused
            if (handles.empty()) {
                logger.LogInfo() << "[Connect] Searching for PackageManagement service...";
            } else {
                std::lock_guard<std::mutex> guard(serviceInstancesAccessMutex);
                foundHandles = std::move(handles);
                cond.notify_one();
            }
        },
        ucmInstance);

    if (!result) {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
        return {};
    }

    std::thread compareIdThread(internal::CompareIdThread,
        std::ref(logger),
        ucmIdentifier,
        std::ref(signalPromise),
        std::ref(cond),
        std::ref(continueRunning),
        std::ref(serviceInstancesAccessMutex),
        std::ref(foundHandles),
        std::ref(serviceInstanceHandle));

    signalFuture.get();
    compareIdThread.join();

    if (serviceInstanceHandle) {
        logger.LogInfo() << "[Connect] The search for PackageManagement service is over";
        PackageManagementProxy::StopFindService(*result);
        return std::make_shared<PackageManagementProxy>(*serviceInstanceHandle);
    } else {
        logger.LogInfo() << "[Connect] No service found. :(";
        return {};
    }
}

}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd
