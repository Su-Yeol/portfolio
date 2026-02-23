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
// or by any means, without permission in writing to the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/*!
 * \file
 * \brief Specification of control activity.
 * \details Contains specification of control activity.
 * Stub for an activity, can be completely generated to the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef __LOCAL_MAP_SUBSCRIBER_H__
#define __LOCAL_MAP_SUBSCRIBER_H__

#include <mutex>
#include <string>
#include <random>
#include <queue>
#include <chrono>
#include <utility>

// includes for used services
#include "katech/local_map_proxy.h"
#include "logger.h"

namespace katech
{

// Header of Methods Output Messages.

/*!
 *  \brief Class implementing control activity.
 *
 *  Control activity implementing control of data control.
 */
class LocalMap_Subscriber
{
    using Proxy = katech::proxy::local_mapProxy;

public:
    LocalMap_Subscriber();

    /*!
     *  \brief Initializes control activity.
     *
     *  Initializes control activity. This is called during initialization of the runtime.
     */
    void init(std::string instance);
    bool waitEvent(int deadLine);
    bool isEventQueueEmpty();

    std::shared_ptr<local_map_Objects> getEvent();

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler);

    void receivedCallback_localMap();

    std::shared_ptr<local_map_Objects> MapData(const std::uint64_t& grid_id, const std::uint64_t& cell_id, const std::uint64_t deadLine);

    void receivedCallback_serviceFlag();
    String fieldGetter_ServiceFlag();

#if defined(R19_11_1)
    void fieldSetter(katech::proxy::local_map::fields::ServiceFlag::value_type& value);
#else
    void fieldSetter(katech::proxy::fields::ServiceFlag::value_type& value);
#endif

    local_map_Objects getMapData();

protected:
    std::shared_ptr<Proxy> m_proxy;
    std::mutex m_proxy_mutex;
    std::shared_ptr<ara::core::Promise<bool>> m_promise_waitEvent;
    std::mutex m_Mutex_eventQueue;
    std::queue<std::shared_ptr<local_map_Objects>> m_Queue_event;

    void enQueue(std::shared_ptr<local_map_Objects> item);
    std::shared_ptr<local_map_Objects> deQueue();

    local_map_Objects mapData;
    
    int getQueueSize();
    auto setTrigger();

    void subscribe_localMap();

    /*!
     * Subscribe to Update Rate Field.
     */
    void subscribe_serviceFlag();
};

}  // namespace katech

#endif  // __LOCAL_MAP_SUBSCRIBER_H__
