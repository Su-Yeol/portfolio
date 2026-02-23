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
 *  \brief Specification of ddsApp01 service4 activity.
 *
 *  \details Contains specification of ddsApp01 service4 activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef DDSSERVICE4_IMP_HPP_
#define DDSSERVICE4_IMP_HPP_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "apd/testsuite/cmdds/ddsservice4_skeleton.h"

constexpr auto METHODS_HEADER = "METHODS: ";

using apd::testsuite::cmdds::DDSFusionVariant;
using apd::testsuite::cmdds::DDSVehicleState;

/*!
 *  \brief Class implementing skeleton methods.
 *
 *  DDSApp01's implementation.
 *
 *  \uptrace{SWS_CM_00191}
 */
class ddsService4Imp : public apd::testsuite::cmdds::skeleton::ddsservice4Skeleton
{
    using DDSService4Skeleton = apd::testsuite::cmdds::skeleton::ddsservice4Skeleton;

public:
    ddsService4Imp(ara::com::InstanceIdentifier instance, ara::com::MethodCallProcessingMode mode)
        : DDSService4Skeleton(instance, mode)
        , m_worker(&ddsService4Imp::ProcessRequests, this)
    {}

    virtual ~ddsService4Imp()
    {
        m_finished = true;
        m_worker.join();
    }

    virtual auto MethodA(const String& configuration, const DDSVehicleState& vehiclestate)
        -> decltype(DDSService4Skeleton::MethodA(configuration, vehiclestate)) override;


    virtual auto MethodB(const String& configuration, const DDSFusionVariant& variant)
        -> decltype(DDSService4Skeleton::MethodB(configuration, variant)) override;
    
    /*!
     * \brief Defines how the incoming service method invocations are processed.
     *
     */
    void ProcessRequests();

    /*!
     *  \brief ddsApp01 service offer .
     *
     *  Executable unit triggered to offer ddsApp01 service.
     */
    void offerDdsService4();

    /*!
     *  \brief ddsApp01 stop service offer .
     *
     *  Executable unit triggered to stop offer ddsApp01 service.
     */    
    void stopOfferDdsService4();

private:
    std::uint16_t m_sync_count; 
    std::atomic<bool> m_finished{false};
    std::atomic<bool> isOfferDdsService4{false};
    std::thread m_worker;

};
#endif  // DDSSERVICE4_IMP_HPP_
