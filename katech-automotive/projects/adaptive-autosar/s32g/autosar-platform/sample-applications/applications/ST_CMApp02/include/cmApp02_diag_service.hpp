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
 * \brief   Management of the skeleton which provides the diagnostics services offered by CMAPP02.
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup CMAPP02
 */

#ifndef CMAPP02DIAGSERVICE_H_
#define CMAPP02DIAGSERVICE_H_
#include <memory>
#include "cmApp02_data_identifier.hpp"


/* Class definition ------------------------------------*/

/** Class managing the service skeleton which offers the App1's diagnostic services. */
class CmApp02DiagService
{
private:

    /** skeleton offering DataIdentifier service */
    CMApp02DataIdentifier skeletonCMApp02DataIdentifier;

public:
    /** Constructor
     *
     * \return          none
     */
    CmApp02DiagService();

    /** Destructor
     *
     * \return          none
     */
    ~CmApp02DiagService();

    /** Initialization
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
    bool Init();

    /** Deinitialization
     *
     */
    void DeInit();

    /** Increment IncrementAuthorizedCounter
     * \param[in]       key Specify the key which readDidAuthorizedCounters is incremented
     *
     * \return          boolean
     * \retval          TRUE:         successful
     * \retval          FALSE:        error occurred
     */
};


#endif /* CMAPP02DIAGSERVICE_H_ */
