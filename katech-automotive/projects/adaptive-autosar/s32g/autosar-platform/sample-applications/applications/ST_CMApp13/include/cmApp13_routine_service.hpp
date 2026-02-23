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

/**
 * \brief   Implementation of the RoutineService service offered by CMAPP13.
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMAPP13
 */

#ifndef CMAPP13ROUTINESERVICE_H_
#define CMAPP13ROUTINESERVICE_H_

#include "ara/diag/routineservice_skeleton.h"

/** Implementation of the RoutineService service skeleton */
class CMApp13RoutineService : public ara::diag::skeleton::RoutineServiceSkeleton
{
    /** Abstract skeleton class which is implemented here. */
    using Skeleton = ara::diag::skeleton::RoutineServiceSkeleton;

public:
    /** Constructor
     *
     * \return          none
     */
    explicit CMApp13RoutineService(ara::com::InstanceIdentifier);

    /** UDS Start methode definition
     *
     * \param[in]       request      Request payload bytes.
     *
     * \return          ara::com::Future
     */
    auto Start(const ByteArray& request) -> decltype(Skeleton::Start(request)) override;

    /** UDS RequestResults methode definition
     *
     * \param[in]       request      Request payload bytes.
     *
     * \return          ara::com::Future
     */
    auto RequestResults(const ByteArray& request) -> decltype(Skeleton::RequestResults(request)) override;

    /** UDS Stop methode definition
     *
     * \param[in]       request      unused
     *
     * \return          ara::com::Future
     */
    auto Stop(const ByteArray& request) -> decltype(Skeleton::Stop(request)) override;
};

#endif  // CMAPP13ROUTINESERVICE_H_
