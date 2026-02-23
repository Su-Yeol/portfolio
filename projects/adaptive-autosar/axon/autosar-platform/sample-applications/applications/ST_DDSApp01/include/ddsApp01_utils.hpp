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
 * \brief   DDSApp01 typedefs used in DDSApp01 components
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */

#ifndef DDSAPP01_TYPES_H_
#define DDSAPP01_TYPES_H_ 

/* Typedefs -------------------------------------------*/

/**
 * \brief   enum UDS find Request type
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */
enum DDSAPP01RequestServices : uint8_t
{
    OFFER_DDSSERVICE1,
    OFFER_DDSSERVICE2,
    OFFER_DDSSERVICE3,
    OFFER_DDSSERVICE4,
    OFFER_DDSSERVICE5,
    SEND_DDSSERVICE1,
    SEND_DDSSERVICE2,
    STOP_DDSSERVICE1,
    STOP_DDSSERVICE2,
    STOP_DDSSERVICE3,
    STOP_DDSSERVICE4,
    STOP_DDSSERVICE5,
    STOPSENDING_DDSSERVICE1,
    STOPSENDING_DDSSERVICE2,
    UPDATE_DDSSERVICE3,
};

#endif /* DDSAPP01_TYPES_H_ */
