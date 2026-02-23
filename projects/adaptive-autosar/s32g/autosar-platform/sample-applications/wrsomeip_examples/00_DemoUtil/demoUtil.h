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

/*
DESCRIPTION

This file is a common header for multiple demo applications containing useful
definitions and utility functions.
*/

#ifndef __INCdemoUtilh
#define __INCdemoUtilh

/* includes */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */

#define DEMO_SERV 0x1234 /* demo service id */
#define DEMO_SERV_RANGE 1 /* demo services range */
#define DEMO_INST 0x5678 /* demo instance id */
#define DEMO_INST_RANGE 1 /* demo instances range */
#define DEMO_METH 0x0421 /* demo method id */
#define DEMO_METH_RANGE 1 /* demo method range */
#define DEMO_EV 0x8778
#define DEMO_EV_RANGE 1 /* demo event range */
#define DEMO_EVGR 0x4465
#define DEMO_EVGR_RANGE 1 /* demo event group range */
#define DEMO_VER_MAJOR 0x00
#define DEMO_VER_MINOR 0x00000000
#define DEMO_CYCLE 20000 /* app cycle time (ms) */
#define DEFAULT_STOP_DELAY 20000 /* stop offer delay (ms) */
#define NOTIF_PLD_SIZE_MAX 10
#define NOTIF_PLD_SIZE_INIT 1
#define NOTIF_TS FALSE
#define NOTIF_TS_SZ (14 + 1 + 6 + 1) /* "sec:usec" */
#define NOTIF_TS_FMT "%014ld:%06ld"

#define USEC_PER_MSEC (1000)
#define USEC_PER_SEC (1000 * 1000)

/* typedefs */

/* timestamp */

typedef struct demoTs
{
    unsigned long int sec;
    unsigned long int usec;
} DEMO_TS;

/* function declarations */

DEMO_TS* demoUtilTsAlloc(int nr);
void demoUtilTsSet(DEMO_TS* pTs);
int demoUtilTsDif(DEMO_TS* pTs, DEMO_TS* pDif);
void demoUtilTsFree(DEMO_TS* pTs);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCdemoUtilh */
