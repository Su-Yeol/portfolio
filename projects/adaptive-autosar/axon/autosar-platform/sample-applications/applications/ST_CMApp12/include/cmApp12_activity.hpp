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
 *  \brief Specification of radar activity.
 *
 *  \details Contains specification of radar activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CMAPP12_ACTIVITY_HPP_
#define CMAPP12_ACTIVITY_HPP_

#include <string>
#include <random>
#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"
// includes for used services
#include "apd/testsuite/cm/cmservice6_skeleton.h"
#include "cmService6_Imp.hpp"
#include "cmApp12_utils.hpp"

class cmApp12Activity
{
    private:
        int send_count = 0;
    public: 
        std::atomic<bool> isOfferCmService6{false};
        std::atomic<bool> isOfferDiag{false};
        std::atomic<bool> isService6ProcessingRequests{false};

        /*!
         * \brief A pointer to the skeleton object.
         */
        cmService6Imp* cmService6Skeleton;

        ara::log::Logger& m_logger_ctx3{ara::log::CreateLogger("CTX3", "context for update rate", ara::log::LogLevel::kVerbose)};
    
    
        cmApp12Activity();
        ~cmApp12Activity();

        /*!
        *  \brief Initializes radar activity.
        *
        *  Initializes radar activity. This is called during initialization of the runtime.
        */
        void init();
        void deinit();
        void offerCmService6();
        void stopOfferCmService6();
        void enableService6ProcessingRequests();
        void disableService6ProcessingRequests();

        /*!
        *  \brief Runs radar activity.
        *
        *  Executable unit triggered to perform radar activity.
        */
        void act();
};


#endif  // CMAPP12_ACTIVITY_HPP_
