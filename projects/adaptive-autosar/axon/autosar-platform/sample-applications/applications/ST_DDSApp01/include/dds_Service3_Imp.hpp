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
 *  \brief Specification of ddsApp01 service3 activity.
 *
 *  \details Contains specification of ddsApp01 service3 activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (field getter/setter, ...).
 */

#ifndef DDSSERVICE3_IMP_HPP_
#define DDSSERVICE3_IMP_HPP_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "apd/testsuite/cmdds/ddsservice3_skeleton.h"

constexpr auto FIELDS_HEADER = "FIELDS:";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR:";

/*!
 *  \brief Class implementing skeleton Fields.
 *
 *  DDSApp03's implementation.
 */
class ddsService3Imp :  public apd::testsuite::cmdds::skeleton::ddsservice3Skeleton
{
    using DDSService3Skeleton = apd::testsuite::cmdds::skeleton::ddsservice3Skeleton;

public:
   ddsService3Imp(ara::com::InstanceIdentifier instance, ara::com::MethodCallProcessingMode mode)
        : DDSService3Skeleton(instance,mode)
        , m_worker(&ddsService3Imp::ProcessRequests,this)
    { }

    virtual ~ddsService3Imp()
    {
        m_finished = true;
    }

    /*!
     *  \brief ddsApp01 service activity .
     *
     *  Setting ddsApp01 field service activity.
     */
    ara::core::Future<std::uint32_t> SetFieldA(std::uint32_t field);
 
    /*!
     *  \brief ddsApp01 service activity .
     *
     *  Getting ddsApp01 field service activity.
     */
    ara::core::Future<apd::testsuite::cmdds::skeleton::fields::FieldA::value_type> GetFieldA();

    /*!
     *  \brief ddsApp01 service activity .
     *
     *  Initializing ddsApp01 field_A service activity.
     */
    void FieldInitialization();

    /*!
     *  \brief ddsApp01 service activity .
     *
     *  Updating ddsApp01 field_A service activity.
     */
    void UpdateFieldA();

    /*!
     *  \brief ddsApp01 service offer .
     *
     *  Executable unit triggered to offer ddsApp01 service.
     */ 
    void offerDdsService3();

    /*!
     *  \brief ddsApp01 stop service offer .
     *
     *  Executable unit triggered to stop offer ddsApp01 service.
     */ 
    void stopOfferDdsService3();


private:
    /*!
     * \brief Defines how the incoming service method invocations are processed.
     */
    void ProcessRequests();

    std::thread m_worker;
    std::atomic<bool> isOfferDdsService3{false};
    std::uint32_t update_count;
    std::atomic<bool> m_finished{false};
    std::uint32_t field_a;

};
#endif  // DDSSERVICE03_IMP_HPP_
