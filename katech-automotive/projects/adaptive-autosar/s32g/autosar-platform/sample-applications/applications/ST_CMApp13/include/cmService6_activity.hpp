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



#ifndef CMSERVICE6_ACTIVITY_HPP_
#define CMSERVICE6_ACTIVITY_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cm/cmservice6_proxy.h"
#include "ara/log/logger.h"


/*!
 *  \brief Class implementing fusion activity.
 *
 *  cmService6Activity implementing function.
 */
class cmService6Activity
{
    using CMService6Proxy = apd::testsuite::cm::proxy::CMService6Proxy;

private:
    static uint8_t cmApp13_Service6_subscribed;//memo subscribestate passcriteria value
    static bool cmApp13_Service6_received;//memo passcritetia value

public:
    std::shared_ptr<CMService6Proxy> service6Proxy;
    std::mutex m_proxy_mutex;

    /// @brief handle to call StopFindService therewith in the destructor.
    ara::com::FindServiceHandle handle_;
    
    cmService6Activity();

    /*!
     *  \brief Initializes fusion activity.
     *
     *  Initializes fusion activity. This is called during initialization of the runtime.
     */
    void init();
    
    void deinit();

    /*!
     *  \brief Runs fusion activity.
     *
     *  Executable unit triggered to perform fusion activity.
     */
    void act();

    /*!
     *  \brief Callback to change to radar service offer changes.
     *
     *  Callback executed whenever a change for radar service offers happen.
     *
     */

    void startFindService6();
    //StopFindService
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<CMService6Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler);
    void SetUpProxyService6();
    void unsubscribeCmService6();
    void setService6SubscribedValue();
    uint8_t getService6SubscribedValue();
    void setService6ReceivedValue();
    bool getService6ReceivedValue();
    void HandleEventService6();

    // this class own logger
    ara::log::Logger& m_logger{ara::log::CreateLogger("CMApp13", "cmService6Activity class own context", ara::log::LogLevel::kVerbose)};
};


#endif  // CMSERVICE6_ACTIVITY_HPP_
