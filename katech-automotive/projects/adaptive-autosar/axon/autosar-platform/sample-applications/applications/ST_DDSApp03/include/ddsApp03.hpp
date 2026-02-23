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
 * \brief   Executor implementation of the DDSApp03 component
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup DDSApp03
 */

#ifndef DDSAPP03_H_
#define DDSAPP03_H_

#include <signal.h>
#include "execution_state.hpp"
#include "singleton.hpp"
#include <memory>
#include "ddsApp03_activity.hpp"
#include <ara/com/e2exf/status_handler.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include "ara/core/initialization.h"


void SignalHandler(int32_t signalnum);

/* Typedefs -------------------------------------------*/
/* Class definition ------------------------------------*/

/** The DDSApp03 is the implementation of the DDS fusion activity */
class DDSApp03
    : public APD::INTERFACES::ExecutionState
    , public APD::INTERFACES::Singleton<DDSApp03>
{
    friend class APD::INTERFACES::Singleton<DDSApp03>;

private:
    /* Constants -----------------------------------------------------*/

    std::shared_ptr<ddsApp03Activity> ddsApp03ActivityPtr;
    ara::exec::ExecutionClient exec_client;

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
    DDSApp03();

    /** Destructor
     *
     * \return          none
     */
    ~DDSApp03();

    /** Initializing of a DDSApp03 object
     *
     * \return          none
     */
    void Initializing() override;

    /** Terminating of a DDSApp03 object
     *
     * \return          none
     */
    void Terminating() override;

    /** Running of a DDSApp03 object
     *
     * \return          none
     */
    void Running() override;
    
};

#endif /* DDSAPP03_H_ */
