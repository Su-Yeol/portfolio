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

#include <mutex>

#include "find_service.h"
#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

namespace ara
{
namespace ucm
{
namespace vsm
{

using ara::vucm::proxy::VehicleStateManagerInterfaceProxy;

namespace internal
{

using OptionalHandle = ara::core::Optional<VehicleStateManagerInterfaceProxy::HandleType>;

static OptionalHandle FindServiceCallback(
    ara::com::ServiceHandleContainer<VehicleStateManagerInterfaceProxy::HandleType> handles,
    ara::log::Logger& logger)
{
    if (handles.size() == 1) {
        logger.LogInfo() << "Found one handle, instance:" << handles[0].GetInstanceId().ToString();
        return handles[0];
    } else {
        logger.LogError() << "Found" << handles.size() << "handles, but only one handle was expected";
        return {};
    }
}

}  // namespace internal

ara::core::Optional<ServiceProxyPtr> FindVSMApplicationService(const ara::core::InstanceSpecifier& ucmMasterInstance)
{

    ara::core::Promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger("FIND", "Find VSM Service Callback context", ara::log::LogLevel::kVerbose);

    internal::OptionalHandle serviceInstanceHandle;

    auto result = VehicleStateManagerInterfaceProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<VehicleStateManagerInterfaceProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            logger.LogInfo() << "Find VSM service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No VSM service handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles, auto& stopHandle) {
                        serviceInstanceHandle = internal::FindServiceCallback(std::move(foundHandles), logger);
                        VehicleStateManagerInterfaceProxy::StopFindService(stopHandle);
                        logger.LogInfo() << "Find VSM service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findServiceHandle);
            }
        },
        ucmMasterInstance);
    if (!result) {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
        return {};
    }
    signalFuture.get();

    if (serviceInstanceHandle) {
        return std::make_shared<VehicleStateManagerInterfaceProxy>(*serviceInstanceHandle);
    } else {
        return ara::core::nullopt;
    }
}

}  // namespace vsm
}  // namespace ucm
}  // namespace ara
