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
 * \brief   Executor implementation of the CMApp01 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMApp01
 */

#ifndef CMAPP01_H_
#define CMAPP01_H_

#include <signal.h>
#include "execution_state.hpp"
#include "singleton.hpp"
#include <memory>
#include "cmApp01_activity.hpp"
#include "cmApp01_diag_service.hpp"

#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include "ara/core/initialization.h"


void SignalHandler(int32_t signalnum);

/* Typedefs -------------------------------------------*/
/* Class definition ------------------------------------*/

/** The CMApp01 is the implementation of the Someip event Communication */
class CMApp01
    : public APD::INTERFACES::ExecutionState
    , public APD::INTERFACES::Singleton<CMApp01>
{
    friend class APD::INTERFACES::Singleton<CMApp01>;

    public:
        /* Constants -----------------------------------------------------*/

        /** Pointer to the component of the APP1 responsible for diagnostic services. */
        std::shared_ptr<cmApp01Activity> serviceActivity;
        std::shared_ptr<CmApp01DiagService> diag;
        ara::exec::ExecutionClient exec_client;
        ara::log::Logger& m_logger_ctx4{ara::log::CreateLogger("CTX4", "context for main module", ara::log::LogLevel::kVerbose)};

        struct sigaction act;

        /** Flag indicating if the executor shall shutdown */
        volatile bool shutdown = false;

        /* Public object data -------------------------------------------*/
        /* Public function declarations -------------------------------------------*/

        /** Constructor
         *
         * \return          none
         */
        CMApp01();

        /** Destructor
         *
         * \return          none
         */
        ~CMApp01();

        /** Initializing of a CMApp01 object
         *
         * \return          none
         */
        void Initializing() override;

        /** Terminating of a CMApp01 object
         *
         * \return          none
         */
        void Terminating() override;

        /** Running of a CMApp01 object
         *
         * \return          none
         */
        void Running() override;
    
};

#endif /* CMAPP01_H_ */
