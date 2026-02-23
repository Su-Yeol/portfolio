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
 * \brief   Config Values
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */

#ifndef DDSAPP03_TYPES_H_
#define DDSAPP03_TYPES_H_


/* Typedefs -------------------------------------------*/


namespace TESTER_REQUESTS
{
    constexpr uint8_t REQUEST_RESULTS_TC1 = 20u;
    constexpr uint8_t REQUEST_RESULTS_TC2_1 = 21u;
    constexpr uint8_t REQUEST_RESULTS_TC2_2 = 31u;
    constexpr uint8_t REQUEST_RESULTS_TC3 = 22u;
    constexpr uint8_t REQUEST_RESULTS_TC4 = 23u;
};
/**
 * \brief   enum UDS find Request type
 * \note    Project: Adaptive Platform Test Applications
 * \ingroup Utils
 */
enum DDSAPP03RequestServices : uint8_t
{
    FIND_DDSSERVICE1,
    FIND_DDSSERVICE2,
    FIND_DDSSERVICE3,
    FIND_DDSSERVICE4,
    FIND_DDSSERVICE5,
    CALL_DDSSERVICE4,
    CALL_DDSSERVICE5,
    STOP_DDSSERVICE1,
    STOP_DDSSERVICE2,
    STOP_DDSSERVICE3,
    STOP_DDSSERVICE4,
    STOP_DDSSERVICE5,
    STOPSENDING_DDSSERVICE1,
    STOPSENDING_DDSSERVICE2,
    SUBSCRIBE_DDSSERVICE1,
    SUBSCRIBE_DDSSERVICE2,
    SUBSCRIBE_DDSSERVICE3,
    GETVALUE_DDSSERVICE3,
    SETVALUE_DDSSERVICE3,
};

enum class DDSAPP03ServiceState
{
    Available,
    NotAvailable,
};

#endif /* DDSAPP03_TYPES_H_ */
