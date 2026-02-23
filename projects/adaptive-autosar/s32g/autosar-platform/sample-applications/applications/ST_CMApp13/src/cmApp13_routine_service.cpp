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
 * \brief   Implementation of the RoutineService service offered by CMApp13.
 * \note    Project: Adaptive Platform Test Applications
 *
 * \ingroup CMApp13
 */

#include "cmApp13_routine_service.hpp"
#include "cmApp13.hpp"
#include "cmApp13_utils.hpp"
#include <iostream>

using namespace ara::log;

CMApp13RoutineService::CMApp13RoutineService(ara::com::InstanceIdentifier instanceId)
    : Skeleton(instanceId)
{ }

auto CMApp13RoutineService::Start(const ByteArray& request) -> decltype(Skeleton::Start(request))
{
    auto& logger_diag = CreateLogger("DAPP", "Routine Service context", LogLevel::kVerbose);
    logger_diag.LogInfo() << "CMApp13RoutineService: Starting";
    decltype(Skeleton::Start(request))::PromiseType promise;

    ara::diag::RoutineService::StartOutput response;

    if (request.empty()) {
        /* request has wrong size, send back incorrect message size uds error. */
        // APD_UTILS_LOGDEBUG("CMApp13RoutineService::Start: request payload is
        // empty");
        promise.SetError(ara::diag::DiagnosticErrorsErrc::kIncorrectMessageLengthOrInvalidFormat);
    } else {
        auto cmapp13Runtime = CMApp13::instance();
        // APD::APP4::APP4StartRequestServices requestValue = static_cast<const
        // uint8_t>(request[0]);
        const uint8_t requestValue = static_cast<const uint8_t>(request[0]);

        switch (requestValue) {
        case CMAPP13StartRequestServices::UNSUBSCRIBE_CMSERVICE6: {
            // APD_UTILS_LOGDEBUG("CMAPP13StartRequestServices: Requesting start
            // service6");
            logger_diag.LogInfo() << "Debug: UNSUBSCRIBE_CMSERVICE6 ";
            cmapp13Runtime->cmService6ActivityPtr->unsubscribeCmService6();

            promise.set_value(response);
            break;
        }
        default: {
            std::cout << "CmApp13RoutineService: Request does not exist  not defined" << std::endl;
            // APD_UTILS_LOGDEBUG("CmApp04RoutineService: Requested value " <<
            // requestValue << " does not exist.");
            promise.SetError(ara::diag::DiagnosticErrorsErrc::kRequestOutOfRange);
        }
        }
    }
    return promise.get_future();
}

auto CMApp13RoutineService::RequestResults(const ByteArray& request) -> decltype(Skeleton::RequestResults(request))
{
    // APD_UTILS_LOGDEBUG("CmApp13RoutineService: Subfunciton RequestResult not
    // implemented.");
    std::cout << "CmApp13RoutineService: Subfunciton RequestResult not implemented" << std::endl;

    decltype(Skeleton::RequestResults(request))::PromiseType promise;
    promise.SetError(ara::diag::DiagnosticErrorsErrc::kSubfunctionNotSupported);

    return promise.get_future();
}

auto CMApp13RoutineService::Stop(const ByteArray& request) -> decltype(Skeleton::Stop(request))
{
    // APD_UTILS_LOGDEBUG("CMApp13RoutineService: Stop.");
    decltype(Skeleton::Stop(request))::PromiseType promise;
    StopOutput response;

    return promise.get_future();
}
