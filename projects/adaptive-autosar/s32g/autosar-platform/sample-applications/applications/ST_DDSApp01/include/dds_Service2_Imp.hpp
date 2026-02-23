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
 *  \brief Specification of ddsApp01 service2 activity.
 *
 *  \details Contains specification of ddsApp01 service2 activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef DDSSERVICE2_IMP_HPP_
#define DDSSERVICE2_IMP_HPP_

#include <string>
#include <random>

#include "ara/log/logger.h"

// includes for used services
#include "apd/testsuite/cmdds/ddsservice2_skeleton.h"

/*!
 *  \brief Class implementing skeleton Events.
 *
 *  ddsService2 implementation.
 */
class ddsService2Imp : public apd::testsuite::cmdds::skeleton::ddsservice2Skeleton
{
    using DDSService2Skeleton = apd::testsuite::cmdds::skeleton::ddsservice2Skeleton;

public:
    ddsService2Imp(ara::com::InstanceIdentifier instance, ara::com::MethodCallProcessingMode mode)
        : DDSService2Skeleton(instance, mode)
    { }

    virtual ~ddsService2Imp()
    { }

    /*!
     *  \brief ddsApp01 service activity .
     *
     *  Setting ddsApp01 event service activity.
     */
    void SendEventC(uint8_t i);

    /*!
     *  \brief ddsApp01 service offer .
     *
     *  Executable unit triggered to offer ddsApp01 service.
     */
    void offerDdsService2();

    /*!
     *  \brief ddsApp01 stop service offer .
     *
     *  Executable unit triggered to stop offer ddsApp01 service.
     */
    void stopOfferDdsService2();

private:

    std::atomic<bool> isOfferDdsService2{false};

};

#endif  // DDSSERVICE2_IMP_HPP_
