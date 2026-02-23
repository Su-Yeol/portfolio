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

/*!
 *  \file
 *  \brief Specification of perception activity.
 *
 *  \details Contains specification of perception activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef __LOCAL_MAP_PROVIDER_H__
#define __LOCAL_MAP_PROVIDER_H__

#include <string>
#include <random>

#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "adcm/local_map_skeleton.h"

namespace adcm
{

typedef void (*MapDataCallback)(const std::uint64_t& grid_id, const std::uint64_t& cell_id);
static MapDataCallback mCallback = nullptr;
static std::shared_ptr<adcm::local_map::GetMapDataOutput> output = std::make_shared<adcm::local_map::GetMapDataOutput>();

/*!
 *  \brief Class implementing skeleton methods.
 *
 *  local_map's implementation.
 *
 *  \uptrace{SWS_CM_00191}
 */
class LocalMapImp : public adcm::skeleton::local_mapSkeleton
{
    using Skeleton = adcm::skeleton::local_mapSkeleton;

public:
    LocalMapImp(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
        , m_worker(&LocalMapImp::ProcessRequests, this)
    { }
    virtual ~LocalMapImp()
    {
        m_finished = true;
        m_worker.join();
    }

    virtual auto GetMapData(const std::uint64_t& grid_id, const std::uint64_t& cell_id)
        -> decltype(Skeleton::GetMapData(grid_id, cell_id)) override;

private:
    void ProcessRequests();

    std::atomic<bool> m_finished{false};
    std::thread m_worker;
};

class LocalMap_Provider
{
public:
    LocalMap_Provider();
    ~LocalMap_Provider();

    static void setCallback(MapDataCallback cb);
    static std::shared_ptr<adcm::local_map::GetMapDataOutput> getPtrOutput();
    
    void init(std::string instance);
    void send(local_map_Objects& data);

#if defined(R19_11_1)
    void update(adcm::skeleton::local_map::fields::ServiceFlag::value_type& data);
#else
    void update(adcm::skeleton::fields::ServiceFlag::value_type& data);
#endif

protected:
    adcm::skeleton::local_mapSkeleton* m_skeleton;

    std::uint32_t m_update_rate;
    double m_object_distance;
    String m_service_flag;

#if defined(R19_11_1)
    ara::core::Future<adcm::skeleton::local_map::fields::ServiceFlag::value_type> getServiceFlag();
    ara::core::Future<adcm::skeleton::local_map::fields::ServiceFlag::value_type> setServiceFlag(
        adcm::skeleton::local_map::fields::ServiceFlag::value_type field);
#else
    ara::core::Future<adcm::skeleton::fields::ServiceFlag::value_type> getServiceFlag();
    ara::core::Future<adcm::skeleton::fields::ServiceFlag::value_type> setServiceFlag(
        adcm::skeleton::fields::ServiceFlag::value_type field);
    
#endif
};

}  // namespace adcm
#endif  // __LOCAL_MAP_PROVIDER_H__
