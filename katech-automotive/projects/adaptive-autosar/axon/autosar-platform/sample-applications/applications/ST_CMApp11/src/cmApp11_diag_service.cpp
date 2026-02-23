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
 * \brief   Management of the skeleton which provides the diagnostics services
 * offered by CMAPP11. \note    Project: Adaptive Platform Test Applications
 *
 * \ingroup CMAPP11
 */

#include "cmApp11_diag_service.hpp"
#include "cmApp11_utils.hpp"

CmApp11DiagService::CmApp11DiagService()
    : skeletonCMApp11DataIdentifier(ara::com::InstanceIdentifier(DIAGCONFIG::CMAPP11_DATAIDENTIFIER_INSTANCE_ID))
    , skeletonCMApp11RoutineService(ara::com::InstanceIdentifier(DIAGCONFIG::CMAPP11_ROUTINESERVICE_INSTANCE_ID))
{ }

CmApp11DiagService::~CmApp11DiagService()
{ }

bool CmApp11DiagService::Init()
{
    bool rv = false;
    // APD_UTILS_LOGDEBUG("Init()");
    try {
        // Make DataIdentifier service available to diagnostic manager.
        skeletonCMApp11DataIdentifier.OfferService();
        skeletonCMApp11RoutineService.OfferService();
        rv = true;
    } catch (std::exception& e) {
        // APD_UTILS_LOGDEBUG("Standard exception: " << e.what());
        rv = false;
    }
    return rv;
}

void CmApp11DiagService::DeInit()
{
    // APD_UTILS_LOGDEBUG("Stop Offer RoutineService");
    skeletonCMApp11DataIdentifier.StopOfferService();
    skeletonCMApp11RoutineService.StopOfferService();
}
