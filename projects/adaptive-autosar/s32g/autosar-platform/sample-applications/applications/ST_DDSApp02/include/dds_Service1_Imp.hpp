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
 * \file
 * \brief Specification of service1 event activity.
 * \details Contains specification of service1 activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef DDSSERVICE1_IMP_HPP_
#define DDSSERVICE1_IMP_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cmdds/ddsservice1_proxy.h"
#include "ara/log/logger.h"
#include "ddsApp02_utils.hpp"

/*!
 *  \brief Class implementing Service1 activity.
 *
 *  Service1 activity implementing.
 */
class ddsService1Imp
{
    using DDSService1Proxy = apd::testsuite::cmdds::proxy::ddsservice1Proxy;

public:
    ddsService1Imp();

   /* pointer to ddsService1 proxy */
    std::shared_ptr<DDSService1Proxy> service1_proxy;

    /*!
     *  \brief Callback to change to ddsApp01 service offer changes.
     *
     *  Callback executed whenever a change for ddsApp01 service offers happen.
     *
     */
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService1Proxy::HandleType> handles, ara::com::FindServiceHandle handler);

    /*! 
     * \brief Set EventA Handler
     */
    void SetEventAHandler();

    /*! 
     * \brief Set EventB Handler
     */
    void SetEventBHandler();

    /*! 
     * \brief Callback EventA
     */
    void EventAReceived();

    /*! 
     * \brief Callback EventB
     */
    void EventBReceived();

    /*!
     *  \brief Start Find Service2
     */
    void StartFindService1();

    /*!
     *  \brief Stop Find Service2
     */
    void StopFindService1();

    /*!
     *  \brief Stop Sending Service2
     *
     * Executing Unsubscribe EventC
     *
     */
    void StopSendingService1();

    /*!
     *  \brief Send Response to tester request.
     *
     *  Send Response testcase1 result to tester request.
     *
     */
    std::string SendRequestResult1();

    /*!
     *  \brief Send Response to tester request.
     *
     *  Send Response testcase2 result to tester request.
     *
     */
    std::string SendRequestResult2();
    std::string SendRequestResult2_2();

protected:
    std::mutex m_proxy_mutex;
    std::uint32_t m_act_count;
    std::uint8_t send_request_count;
    std::string getCallbackA;
    std::string getCallbackB;
    DDSAPP02ServiceState service1_state = {DDSAPP02ServiceState::NotAvailable};
    ara::com::FindServiceHandle handle_;

};

#endif  // DDSAPP02_ACTIVITY_HPP_
