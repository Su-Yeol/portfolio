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

#ifndef __DRIVING_TRAJECTORY_SUBSCRIBER_H__
#define __DRIVING_TRAJECTORY_SUBSCRIBER_H__

#include <mutex>
#include <string>
#include <random>
#include <queue>
#include <chrono>
#include <utility>

#include "adcm/driving_trajectory_proxy.h"
#include "logger.h"
#include "etc/udpClient.h"

#include "etc/udpPacket.h"

namespace adcm
{

// Header of Methods Output Messages.

/*!
 *  \brief Class implementing control activity.
 *
 *  Control activity implementing control of data control.
 */
class DrivingTrajectory_Subscriber
{
#define QUEUE_LENGTH 16

    using Proxy = adcm::proxy::driving_trajectoryProxy;

public:
    DrivingTrajectory_Subscriber();

    /*!
     *  \brief Initializes control activity.
     *
     *  Initializes control activity. This is called during initialization of the runtime.
     */
    void init(std::string instance);
    bool waitEvent(int deadLine);
    std::shared_ptr<driving_trajectory_Objects> getEvent();
    std::shared_ptr<driving_trajectory_Objects> getEvent_Last();
    bool isEventQueueEmpty();

    /*!
     *  \brief Callback to change to radar service offer changes.
     *
     *  Callback executed whenever a change for radar service offers happen.
     *
     */

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
                                     ara::com::FindServiceHandle handler);

    /*!
     * \brief Callback Received when the Field Update Rate is changed.
     *
     * Callback Received when the Field Update Rate is changed.
     */

    void receivedCallback_driving();
    void receivedCallback_trajectory();
    void receivedCallback_driving_trajectory();

protected:
    static DrivingTrajectory_Subscriber* mInstance;
    std::shared_ptr<Proxy> m_proxy;
    std::mutex m_proxy_mutex;
    std::mutex m_Mutex_eventQueue;
    //std::queue<std::shared_ptr<driving_trajectory_Objects>> m_Queue_event;
    std::shared_ptr<driving_trajectory_Objects> m_Queue_event[QUEUE_LENGTH];
    int mIndex_head = 0;
    int mIndex_tail = 0;
    std::shared_ptr<ara::core::Promise<bool>> m_promise_waitEvent;
    bool mEnableUDP = false;
    int mUdp_port = 10001;
    std::shared_ptr<adcm::etc::udpClient> mUdpClient;

    static void udpClientCallback(int port, std::shared_ptr<adcm::etc::udpClient::udpBuffer> buffer);
    void enQueue(std::shared_ptr<driving_trajectory_Objects> item);
    std::shared_ptr<driving_trajectory_Objects> deQueue();
    std::shared_ptr<driving_trajectory_Objects> popQueue();
    std::shared_ptr<driving_trajectory_Objects> peekQueue();
    void flashQueue();
    int getQueueSize();
    auto setTrigger();

    void subscribe_drivingTrajectory();

};

}  // namespace adcm

#endif  // __DRIVING_TRAJECTORY_SUBSCRIBER_H__
