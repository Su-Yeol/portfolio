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
 * offered by CMAPP01.
 * \note    Project: Adaptive Platform Test Applications
 *
 * \ingroup CMAPP01
 */

//#include "utils/inc/logging_conf.hpp"
#include "cmApp01_diag_service.hpp"
#include "cmApp01_utils.hpp"
#include "cmApp01.hpp"

using namespace ara::log;

CmApp01DiagService::CmApp01DiagService()
{ }

CmApp01DiagService::~CmApp01DiagService()
{
    if (skeletonRoutineService) {
        delete this->skeletonRoutineService;
        this->skeletonRoutineService = nullptr;
    }
}

bool CmApp01DiagService::Init()
{
    bool rv = false;
    auto& logger_debug = CreateLogger("DAP", "Diag Service context", LogLevel::kVerbose);
    logger_debug.LogInfo() << "CMApp01RoutineService: Starting";

    try {
        // Make DataIdentifier service available to diagnostic manager.
        this->skeletonRoutineService
            = new CMApp01RoutineService(ara::com::InstanceIdentifier(DIAGCONFIG::CMAPP01_ROUTINESERVICE_INSTANCE_ID));
        skeletonRoutineService->OfferService();
        rv = true;

    } catch (std::exception& e) {
        // APD_UTILS_LOGDEBUG("Standard exception: " << e.what());
        rv = false;
    }
    return rv;
}

void CmApp01DiagService::DeInit()
{
    // APD_UTILS_LOGDEBUG("Stop Offer RoutineService");
    skeletonRoutineService->StopOfferService();
}
