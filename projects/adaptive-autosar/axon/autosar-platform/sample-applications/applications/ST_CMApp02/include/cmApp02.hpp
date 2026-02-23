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
 * \brief   Executor implementation of the CMApp02 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMApp02
 */

#ifndef CMAPP02_H_
#define CMAPP02_H_

#include <signal.h>
#include "execution_state.hpp"
#include "singleton.hpp"
#include <memory>
#include "cmService1_activity.hpp"
#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>

#include "cmApp02_diag_service.hpp"
#include "cmApp02_utils.hpp"
#include "ara/core/initialization.h"


void SignalHandler(int32_t signalnum);

/* Typedefs -------------------------------------------*/
/* Class definition ------------------------------------*/

/** The CMApp02 is the implementation of the Someip event Communication */
class CMApp02
    : public APD::INTERFACES::ExecutionState
    , public APD::INTERFACES::Singleton<CMApp02>
{
    friend class APD::INTERFACES::Singleton<CMApp02>;

private:
    /* Constants -----------------------------------------------------*/

    /** Pointer to the component of the APP1 responsible for diagnostic services. */
    std::shared_ptr<cmService1Activity> cmService1ActivityPtr;
    
    std::shared_ptr<CmApp02DiagService> diag;
    
    ara::exec::ExecutionClient exec_client;
    ara::log::Logger& logger{ara::log::CreateLogger("DFLT", "Default context", ara::log::LogLevel::kVerbose)};

public:

    struct sigaction act;

    /** Flag indicating if the executor shall shutdown */
    volatile bool shutdown = false;


    /* Public object data -------------------------------------------*/
    /* Public function declarations -------------------------------------------*/

    /** Constructor
     *
     * \return          none
     */
    CMApp02();

    /** Destructor
     *
     * \return          none
     */
    ~CMApp02();

    /** Initializing of a CMApp01 object
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    void Initializing() override;

    /** Terminating of a CMApp01 object
     *
     */
    void Terminating() override;

    /** Running of a CMApp01 object
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    void Running() override;
    
};

#endif /* CMAPP02_H_ */
