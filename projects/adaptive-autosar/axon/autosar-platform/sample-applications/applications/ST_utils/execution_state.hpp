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
 * \brief   Interface description "Logic"
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Interface
 */

#ifndef EXECUTION_STATE_HPP_
#define EXECUTION_STATE_HPP_

namespace APD
{
namespace INTERFACES
{
/** Abstract Class ExecutionState */
class ExecutionState
{
public:
    /** Destructor of class ExecutionState
     */
    virtual ~ExecutionState()
    {}

    /** Initializing of a ExecutionState object
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    virtual void Initializing(void) = 0;

    /** DeInitializing of a ExecutionState object
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    virtual void Terminating(void) = 0;

    /** Processing of a ExecutionState object
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    virtual void Running(void) = 0;
};
}  // namespace INTERFACES
}  // namespace APD

#endif /* EXECUTION_STATE_HPP_ */
