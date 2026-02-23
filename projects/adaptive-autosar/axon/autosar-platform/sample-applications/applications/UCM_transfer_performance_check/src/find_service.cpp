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
#include <chrono>

#include "find_service.h"
#include "ara/log/logger.h"

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

static OptionalHandle FindServiceCallback(ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType> handles,
    ara::log::Logger& logger)
{
    if (handles.size() == 1) {
        // logger.LogInfo() << "Found one handle, instance:" << handles[0].GetInstanceId().ToString();
        return handles[0];
    } else {
        logger.LogError() << "Found" << handles.size() << "handles, but only one handle was expected";
        return {};
    }
}

}  // namespace internal

ara::core::Optional<ServiceProxyPtr> FindPackageManagementService(uint32_t timeoutInSeconds)
{

    std::promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger("FIND", "Find Service Callback context", ara::log::LogLevel::kVerbose);

    internal::OptionalHandle serviceInstanceHandle;

    const auto result = PackageManagementProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<PackageManagementProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            // logger.LogInfo() << "Find service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles, auto& stopHandle) {
                        serviceInstanceHandle = internal::FindServiceCallback(std::move(foundHandles), logger);
                        PackageManagementProxy::StopFindService(stopHandle);
                        // logger.LogInfo() << "Find service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findServiceHandle);
            }
        },
        ara::core::InstanceSpecifier(
            "transfer_performance_check/transfer_performance_check_root/PackageManagementRPort"));

    if (!result) {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
        return {};
    }

    if (signalFuture.wait_for(std::chrono::seconds(timeoutInSeconds)) != std::future_status::ready) {
        return {};
    }

    signalFuture.get();

    if (serviceInstanceHandle) {
        return std::make_shared<PackageManagementProxy>(*serviceInstanceHandle);
    } else {
        return {};
    }
}

}  // namespace pkgmgrsample
}  // namespace ucm
}  // namespace apd
