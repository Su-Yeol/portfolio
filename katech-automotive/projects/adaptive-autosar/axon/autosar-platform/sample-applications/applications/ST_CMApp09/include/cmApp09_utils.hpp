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
 * \brief   CMApp09 typedefs used in CMApp09 components
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */

#ifndef CMAPP09_TYPES_H_
#define CMAPP09_TYPES_H_

namespace VSOMEIPCONFIG
{
    /** instance id of service 1*/
    static constexpr auto AP_SERVICE5_INSTANCE_ID = "SOME/IP:25";
}  

namespace DIAGCONFIG
{
    /** instance id of cmapp09's RoutineService service skeleton */
    static constexpr auto CMAPP09_ROUTINESERVICE_INSTANCE_ID = "SOME/IP:1730";
} 

/* Typedefs -------------------------------------------*/

/**
 * \brief   enum UDS Start methode Request type
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */
enum CMAPP09StartRequestServices : uint8_t
{
    OFFER_CMSERVICE5 = 1u,
};

#endif /* CMAPP01_TYPES_H_ */
