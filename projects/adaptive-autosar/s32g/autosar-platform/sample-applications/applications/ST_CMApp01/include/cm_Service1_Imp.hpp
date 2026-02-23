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

#ifndef CMSERVICE1_IMP_HPP_
#define CMSERVICE1_IMP_HPP_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "apd/testsuite/cm/cmservice1_skeleton.h"



/*!
 *  \brief Class implementing skeleton methods.
 *
 *  Radar's implementation.
 *
 *  \uptrace{SWS_CM_00191}
 */
class cmService1Imp : public apd::testsuite::cm::skeleton::CMService1Skeleton
{
    using Skeleton = apd::testsuite::cm::skeleton::CMService1Skeleton;

public:
    cmService1Imp(ara::com::InstanceIdentifier instance)
        : Skeleton(instance)
    {}

    virtual ~cmService1Imp()
    {
        m_finished = true;
        //m_worker.join();
    }
    
    void ProcessRequests();

private:

    std::atomic<bool> m_finished{false};

    ara::log::Logger& m_logger_ctx1{ara::log::CreateLogger("CTX1", "context for CMService1", ara::log::LogLevel::kVerbose)};
};
#endif  // CMSERVICE1_IMP_HPP_
