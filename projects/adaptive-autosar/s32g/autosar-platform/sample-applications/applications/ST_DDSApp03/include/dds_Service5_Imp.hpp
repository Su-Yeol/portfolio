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



#ifndef DDSSERVICE5_IMP_HPP_
#define DDSSERVICE5_IMP_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cmdds/ddsservice5_proxy.h"
#include "ara/log/logger.h"



/*!
 *  \brief Class implementing service5 activity.
 *
 *  ddsService5 implementing function.
 */
class ddsService5Imp
{
    using DDSService5Proxy = apd::testsuite::cmdds::proxy::ddsservice5Proxy;

public:
    ddsService5Imp();

    std::shared_ptr<DDSService5Proxy> service5_proxy;
    std::mutex m_proxy_mutex;
    std::uint32_t m_async_count;

    std::uint32_t send_request_count;
    std::string get_method_data_c;
    std::string get_method_data_d;

    /// @brief handle to call StopFindService therewith in the destructor.
    ara::com::FindServiceHandle handle_;
    


    void StartFindService5();
    void StopFindService5();

    /*!
     *  \brief Callback to change to radar service offer changes.
     *
     *  Callback executed whenever a change for radar service offers happen.
     *
     */
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService5Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler);


    /*!
     * Test Asynchrnous Method Calls.
     *
     */
    void asyncMethodCCall();

    /*!
     * Test Asynchrnous Method Calls(fire and forgots).
     *
     */
    void asyncMethodDCall();

    std::string SendRequestResult4();

};


#endif  // DDSSERVICE5_IMP_HPP_
