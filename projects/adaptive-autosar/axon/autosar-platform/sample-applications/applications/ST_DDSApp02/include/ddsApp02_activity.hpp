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
 * \brief Specification of ddsApp02 activity.
 * \details Contains specification of ddsApp02 activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef DDSSERVICE1_ACTIVITY_HPP_
#define DDSSERVICE1_ACTIVITY_HPP_

#include <mutex>
#include <string>
#include <random>

// includes for used services
#include "apd/testsuite/cmdds/ddsservice1_proxy.h"
#include "apd/testsuite/cmdds/ddsservice3_proxy.h"
#include "apd/testsuite/cmdds/ddsservice4_proxy.h"
#include "ara/com/e2e_helper.h"
#include "ara/log/logger.h"
#include "ddsApp02_TcpServer.hpp"
#include "ddsApp02_utils.hpp"
#include "dds_Service1_Imp.hpp"
#include "dds_Service3_Imp.hpp"
#include "dds_Service4_Imp.hpp"

/*!
 *  \brief Class implementing ddsApp02 activity.
 *
 *  ddsApp02  activity implementing.
 */
class ddsApp02Activity
{

public:
    ddsApp02Activity();

    /*!
     *  \brief Initializes ddsApp02 activity.
     *
     *  Initializes ddsApp02 activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs ddsApp02 activity.
     *
     *  Executable unit triggered to perform ddsApp02 activity.
     */
    void act();

    /*!
     *  \brief Deinit ddsApp02 activity.
     *
     *  Deinitializing unit triggered to perform ddsApp02 activity.
     */
    void deinit();


protected:
    ddsService1Imp* ddsService1Proxy;
    ddsService3Imp* ddsService3Proxy;
    ddsService4Imp* ddsService4Proxy;

    ddsApp02TcpServer* tcpServer;

    /*!
     *  \brief Receive Command
     *
     *  Deal with tester command and execute some offer operation.
     *
     */
    void Command(std::string request);

};

#endif  // DDSAPP02_ACTIVITY_HPP_
