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
 * \brief   Implementation of the RoutineService service offered by CMApp10.
 * \note    Project: Adaptive Platform Test Applications
 *
 * \ingroup CMApp10
 */

#include "cmApp10_routine_service.hpp"
#include "cmApp10.hpp"
#include "cmApp10_utils.hpp"
#include <iostream>

using namespace ara::log;

CMApp10RoutineService::CMApp10RoutineService(ara::com::InstanceIdentifier instanceId)
    : Skeleton(instanceId)
{ }

auto CMApp10RoutineService::Start(const ByteArray& request) -> decltype(Skeleton::Start(request))
{
    auto& logger_diag = CreateLogger("DAPP", "Routine Service context", LogLevel::kVerbose);
    logger_diag.LogInfo() << "CMApp10RoutineService: Starting";
    decltype(Skeleton::Start(request))::PromiseType promise;

    ara::diag::RoutineService::StartOutput response;

    if (request.empty()) {
        /* request has wrong size, send back incorrect message size uds error. */
        // APD_UTILS_LOGDEBUG("CMApp10RoutineService::Start: request payload is
        // empty");
        promise.SetError(ara::diag::DiagnosticErrorsErrc::kIncorrectMessageLengthOrInvalidFormat);
    } else {
        auto cmapp10Runtime = CMApp10::instance();
        // APD::APP4::APP4StartRequestServices requestValue = static_cast<const
        // uint8_t>(request[0]);
        const uint8_t requestValue = static_cast<const uint8_t>(request[0]);

        switch (requestValue) {
        case CMAPP10StartRequestServices::UNSUBSCRIBE_CMSERVICE5: {
            // APD_UTILS_LOGDEBUG("CMAPP10StartRequestServices: Requesting start
            // service5");
            logger_diag.LogInfo() << "Debug: UNSUBSCRIBE_CMSERVICE5 ";
            cmapp10Runtime->cmService5ActivityPtr->unsubscribeCmService5();

            promise.set_value(response);
            break;
        }
        default: {
            std::cout << "CmApp10RoutineService: Request does not exist  not defined" << std::endl;
            // APD_UTILS_LOGDEBUG("CmApp04RoutineService: Requested value " <<
            // requestValue << " does not exist.");
            promise.SetError(ara::diag::DiagnosticErrorsErrc::kRequestOutOfRange);
        }
        }
    }
    return promise.get_future();
}

auto CMApp10RoutineService::RequestResults(const ByteArray& request) -> decltype(Skeleton::RequestResults(request))
{
    // APD_UTILS_LOGDEBUG("CmApp10RoutineService: Subfunciton RequestResult not
    // implemented.");
    std::cout << "CmApp10RoutineService: Subfunciton RequestResult not implemented" << std::endl;

    decltype(Skeleton::RequestResults(request))::PromiseType promise;
    promise.SetError(ara::diag::DiagnosticErrorsErrc::kSubfunctionNotSupported);

    return promise.get_future();
}

auto CMApp10RoutineService::Stop(const ByteArray& request) -> decltype(Skeleton::Stop(request))
{
    // APD_UTILS_LOGDEBUG("CMApp10RoutineService: Stop.");
    decltype(Skeleton::Stop(request))::PromiseType promise;
    StopOutput response;

    return promise.get_future();
}
