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

#ifndef ADCM_SENSOR_RADAR_SKELETON_H_
#define ADCM_SENSOR_RADAR_SKELETON_H_

#include "ara/com/internal/skeleton/ara_skeleton_base.h"

#include "ara/com/illegal_state_exception.h"
#include "sensor_radar_common.h"

namespace adcm
{
namespace skeleton
{

namespace events
{
/// @uptrace{SWS_CM_00003, b9c1ea1f304c4608e91d4b112aeded7f03b7122e}
using sensorRadarEvent = ara::com::internal::skeleton::EventDispatcher<::adcm::sensor_radar_Objects>;
}  // namespace events

/// @uptrace{SWS_CM_00002, 4d3a2b51c78573d1d34cd3d2aff93527b4a97d63}
class sensor_radarSkeleton
    : public adcm::sensor_radar
    , public ara::com::internal::skeleton::TypedServiceImplBase<sensor_radarSkeleton>
{
public:
    /// @uptrace{SWS_CM_00130, 4e955786ada8287ec07a290cebafd2be29ee3fde}
    sensor_radarSkeleton(ara::com::InstanceIdentifier instanceID,
        ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : ara::com::internal::skeleton::TypedServiceImplBase<sensor_radarSkeleton>(instanceID, mode)
    { }

    /// @uptrace{SWS_CM_00152, 1259be4e527aa8726c310aa14e83ed368c964fed}
    sensor_radarSkeleton(ara::core::InstanceSpecifier instanceSpec,
        ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : ara::com::internal::skeleton::TypedServiceImplBase<sensor_radarSkeleton>(std::move(instanceSpec), mode)
    { }

    /// @uptrace{SWS_CM_00153, 0b96a5306e0c4ffe31c40ccd91e8a33ed50fe2bc}
    sensor_radarSkeleton(ara::com::InstanceIdentifierContainer instanceIDs,
        ara::com::MethodCallProcessingMode mode = ara::com::MethodCallProcessingMode::kEvent)
        : ara::com::internal::skeleton::TypedServiceImplBase<sensor_radarSkeleton>(std::move(instanceIDs), mode)
    { }

    virtual ~sensor_radarSkeleton()
    {
        StopOfferService();
    }

    /// @brief Skeleton shall be move constructable.
    ///
    /// @uptrace{SWS_CM_00135, 3e590d73cd77c5afbc592cec60094e43c8dfbf97}
    explicit sensor_radarSkeleton(sensor_radarSkeleton&&) = default;

    /// @brief Skeleton shall be move assignable.
    ///
    /// @uptrace{SWS_CM_00135, 3e590d73cd77c5afbc592cec60094e43c8dfbf97}
    sensor_radarSkeleton& operator=(sensor_radarSkeleton&&) = default;

    /// @brief Skeleton shall not be copy constructable.
    ///
    /// @uptrace{SWS_CM_00134, 988e39db973396943ac710bbeb3a6e4af033b727}
    explicit sensor_radarSkeleton(const sensor_radarSkeleton&) = delete;

    /// @brief Skeleton shall not be copy assignable.
    ///
    /// @uptrace{SWS_CM_00134, 988e39db973396943ac710bbeb3a6e4af033b727}
    sensor_radarSkeleton& operator=(const sensor_radarSkeleton&) = delete;

    void OfferService()
    {

        ara::com::internal::skeleton::TypedServiceImplBase<sensor_radarSkeleton>::OfferService();
    }

    events::sensorRadarEvent sensorRadarEvent;
};

}  // namespace skeleton
}  // namespace adcm

#endif  // ADCM_SENSOR_RADAR_SKELETON_H_
