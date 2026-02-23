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

namespace apd
{
namespace ucm
{
namespace vda
{

using ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy;

namespace internal
{

using OptionalHandle = ara::core::Optional<VehicleDriverApplicationInterfaceProxy::HandleType>;

static OptionalHandle FindServiceCallback(
    ara::com::ServiceHandleContainer<VehicleDriverApplicationInterfaceProxy::HandleType> handles,
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

ara::core::Optional<ServiceProxyPtr> FindVehicleDriverApplicationInterfaceService(
    const ara::core::InstanceSpecifier& ucmMasterInstance)
{

    ara::core::Promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger("FIND", "Find VDA Service Callback context", ara::log::LogLevel::kVerbose);

    internal::OptionalHandle serviceInstanceHandle;

    const auto result = VehicleDriverApplicationInterfaceProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<VehicleDriverApplicationInterfaceProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            static_cast<void>(findServiceHandle);  // unused
            logger.LogInfo() << "Find VDA service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No VDA service handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles) {
                        serviceInstanceHandle = internal::FindServiceCallback(std::move(foundHandles), logger);
                        logger.LogInfo() << "Find VDA service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles));
            }
        },
        ucmMasterInstance);
    if (!result) {
        logger.LogError() << "StartFindService failed with error: " << result.Error();
        return {};
    }
    signalFuture.get();
    VehicleDriverApplicationInterfaceProxy::StopFindService(*result);

    if (serviceInstanceHandle) {
        return std::make_shared<VehicleDriverApplicationInterfaceProxy>(*serviceInstanceHandle);
    } else {
        return ara::core::nullopt;
    }
}

}  // namespace vda
}  // namespace ucm
}  // namespace apd
