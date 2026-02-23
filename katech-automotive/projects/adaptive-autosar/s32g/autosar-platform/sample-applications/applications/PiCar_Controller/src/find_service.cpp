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

#include "ara/core/future.h"
#include "ara/core/promise.h"
#include "ara/log/logger.h"
#include "find_service.h"

namespace apd
{
namespace picar
{
namespace controller
{

using apd::picar::proxy::ActorInterfaceProxy;
using apd::picar::proxy::LineSensorInterfaceProxy;
#ifdef ENABLE_ULTRASONIC
using apd::picar::proxy::UltraSonicSensorInterfaceProxy;
#endif

namespace internal
{

using LineSensorOptionalHandle = ara::core::Optional<LineSensorInterfaceProxy::HandleType>;

#ifdef ENABLE_ULTRASONIC
using UltraSonicSensorOptionalHandle = ara::core::Optional<UltraSonicSensorInterfaceProxy::HandleType>;
#endif
using ActorOptionalHandle = ara::core::Optional<ActorInterfaceProxy::HandleType>;

static LineSensorOptionalHandle FindLineSensorServiceCallback(
    ara::com::ServiceHandleContainer<LineSensorInterfaceProxy::HandleType> handles,
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

#ifdef ENABLE_ULTRASONIC
static UltraSonicSensorOptionalHandle FindUltraSonicSensorServiceCallback(
    ara::com::ServiceHandleContainer<UltraSonicSensorInterfaceProxy::HandleType> handles,
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
#endif

static ActorOptionalHandle FindActorServiceCallback(
    ara::com::ServiceHandleContainer<ActorInterfaceProxy::HandleType> handles,
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

ara::core::Optional<ActorServiceProxyPtr> FindActorService()
{

    ara::core::Promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger
        = ara::log::CreateLogger("FIND", "Find PiCar Actor Service Callback context", ara::log::LogLevel::kVerbose);

    internal::ActorOptionalHandle serviceInstanceHandle;

    ActorInterfaceProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<ActorInterfaceProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            logger.LogInfo() << "Find actor service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No actor service handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles, auto& stopHandle) {
                        serviceInstanceHandle = internal::FindActorServiceCallback(std::move(foundHandles), logger);
                        ActorInterfaceProxy::StopFindService(stopHandle);
                        logger.LogInfo() << "Find actor service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findServiceHandle);
            }
        },
        ara::com::InstanceIdentifier::MakeAny());

    signalFuture.get();

    if (serviceInstanceHandle) {
        return std::make_shared<ActorInterfaceProxy>(*serviceInstanceHandle);
    } else {
        return ara::core::nullopt;
    }
}

ara::core::Optional<LineSensorServiceProxyPtr> FindLineSensorService()
{

    ara::core::Promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger(
        "FIND", "Find PiCar Line Sensor Service Callback context", ara::log::LogLevel::kVerbose);

    internal::LineSensorOptionalHandle serviceInstanceHandle;

    LineSensorInterfaceProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<LineSensorInterfaceProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            logger.LogInfo() << "Find Line Sensor service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No Line Sensor service handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles, auto& stopHandle) {
                        serviceInstanceHandle
                            = internal::FindLineSensorServiceCallback(std::move(foundHandles), logger);
                        LineSensorInterfaceProxy::StopFindService(stopHandle);
                        logger.LogInfo() << "Find Line Sensor service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findServiceHandle);
            }
        },
        ara::com::InstanceIdentifier::MakeAny());

    signalFuture.get();

    if (serviceInstanceHandle) {
        return std::make_shared<LineSensorInterfaceProxy>(*serviceInstanceHandle);
    } else {
        return ara::core::nullopt;
    }
}

#ifdef ENABLE_ULTRASONIC
ara::core::Optional<UltraSonicSensorServiceProxyPtr> FindUltraSonicSensorService()
{

    ara::core::Promise<void> signalPromise;
    auto signalFuture = signalPromise.get_future();
    auto& logger = ara::log::CreateLogger(
        "FIND", "Find PiCar UltraSonic Sensor Service Callback context", ara::log::LogLevel::kVerbose);

    // internal::

    internal::UltraSonicSensorOptionalHandle serviceInstanceHandle;

    UltraSonicSensorInterfaceProxy::StartFindService(
        [&signalPromise, &logger, &serviceInstanceHandle](
            ara::com::ServiceHandleContainer<UltraSonicSensorInterfaceProxy::HandleType> handles,
            ara::com::FindServiceHandle findServiceHandle) {
            static std::once_flag flag;
            logger.LogInfo() << "Find UltraSonic Sensor service handler is called";
            if (handles.empty()) {
                logger.LogInfo() << "No UltraSonic Sensor service handles are available";
            } else {
                std::call_once(
                    flag,
                    [&serviceInstanceHandle, &signalPromise, &logger](auto foundHandles, auto& stopHandle) {
                        serviceInstanceHandle
                            = internal::FindUltraSonicSensorServiceCallback(std::move(foundHandles), logger);
                        UltraSonicSensorInterfaceProxy::StopFindService(stopHandle);
                        logger.LogInfo() << "Find UltraSonic Sensor service stopped";
                        signalPromise.set_value();
                    },
                    std::move(handles),
                    findServiceHandle);
            }
        },
        ara::com::InstanceIdentifier::MakeAny());

    signalFuture.get();

    if (serviceInstanceHandle) {
        return std::make_shared<UltraSonicSensorInterfaceProxy>(*serviceInstanceHandle);
    } else {
        return ara::core::nullopt;
    }
}
#endif

}  // namespace controller
}  // namespace picar
}  // namespace apd
