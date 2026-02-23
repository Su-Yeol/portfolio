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
 * \brief Specification of service3 field activity.
 * \details Contains specification of service3 field activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points .
 */

#ifndef DDSSERVICE3_IMP_HPP_
#define DDSSERVICE3_IMP_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cmdds/ddsservice3_proxy.h"
#include "ara/com/e2e_helper.h"
#include "ara/log/logger.h"
#include "ddsApp03_utils.hpp"

constexpr auto FIELDS_HEADER = "FIELDS: ";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR:";
/*!
 *  \brief Class implementing ddsService3 activity.
 *
 *  ddsService3 activity implementing.
 */
class ddsService3Imp
{
    using DDSService3Proxy = apd::testsuite::cmdds::proxy::ddsservice3Proxy;

public:
    ddsService3Imp();

    /*!
     *  \brief Callback to change to ddsApp01 service offer changes.
     *
     *  Callback executed whenever a change for ddsApp01 service offers happen.
     *
     */
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService3Proxy::HandleType> handles, ara::com::FindServiceHandle handler);

    /*!
     *  \brief Start Find Service3
     */
    void StartFindService3();

    /*!
     *  \brief Stop Find Service3
     */
    void StopFindService3();

    /*!
     *  \brief Callback FiledA value
     */
    void FieldAReceived();

    /*!
     *  \brief Start Subscribe FieldA
     */
    void FieldASubscription();

    /*!
     *  \brief Get to Field_A value 
     */
    void FieldGetter();

    /*!
     *  \brief Set to Field_A value
     */
    void FieldSetter();

    /*!
     *  \brief Send Response to tester request.
     *
     *  Send Response testcase3 result to tester request.
     *
     */
    std::string SendRequestResult3();

    /* Pointer to proxy */
    std::shared_ptr<DDSService3Proxy> service3_proxy;

protected:
    std::uint32_t responseValue;
    std::mutex m_proxy_mutex;

    ara::com::FindServiceHandle handle_;


};

#endif  // DDSAPP03_IMP_HPP_
