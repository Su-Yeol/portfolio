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



#ifndef DDSSERVICE4_IMP_HPP_
#define DDSSERVICE4_IMP_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cmdds/ddsservice4_proxy.h"
#include "ara/log/logger.h"


/*!
 *  \brief Class implementing service4 method activity.
 *
 *  ddsService4 implementing function.
 */
class ddsService4Imp
{
    using DDSService4Proxy = apd::testsuite::cmdds::proxy::ddsservice4Proxy;

public:
    ddsService4Imp();

    std::shared_ptr<DDSService4Proxy> service4_proxy;
    std::mutex m_proxy_mutex;
    std::uint32_t m_sync_count;

    std::uint8_t send_request_count;
    std::string get_method_data_a;
    std::string get_method_data_b;
    /// @brief handle to call StopFindService therewith in the destructor.
    ara::com::FindServiceHandle handle_;

    void StartFindService4();

    void StopFindService4();

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<DDSService4Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler);

    /*!
     * Test Synchrnous Method Calls.
     *
     */
    void syncMethodACall();
    void syncMethodBCall();

    std::string SendRequestResult4();

};


#endif  // DDSSERVICE4_IMP_HPP_
