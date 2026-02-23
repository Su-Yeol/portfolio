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

/* includes */

#include "demoUtil.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

/*******************************************************************************
 *
 * demoUtilTsAlloc - alloc a table of timestamps
 *
 * This routine is used to allocated memory for an array of specified number of
 * timestamps elements.
 *
 * RETURNS:
 * - NULL - could not alloc memory
 * - pointer to the allocated memory
 */

DEMO_TS* demoUtilTsAlloc(int nelem)
{
    DEMO_TS* pTsArr;

    pTsArr = calloc(nelem, sizeof(DEMO_TS));

    return pTsArr;
}

/*******************************************************************************
 *
 * demoUtilTsFree - free a table of timestamps
 *
 * This routine is used to free allocated memory for an array of timestamps
 * elements.
 *
 * RETURNS: N/A
 */

void demoUtilTsFree(DEMO_TS* pTs)
{
    free(pTs);
}

/*******************************************************************************
 *
 * demoUtilTsSet - set timestamp
 *
 * Set the current time in the specified timestamp structure.
 *
 * RETURNS: N/A
 */

void demoUtilTsSet(DEMO_TS* pTs)
{
    struct timeval tv;
    (void)gettimeofday(&tv, 0);
    pTs->sec = tv.tv_sec;
    pTs->usec = tv.tv_usec;
}

/*******************************************************************************
 *
 * demoUtilTsDif - calculate timestamps difference
 *
 * Calculate the difference between 2 timestamps and normalize the result.
 *
 * RETURNS: N/A
 */

int demoUtilTsDif(DEMO_TS* pTs, DEMO_TS* pDif)
{
    struct timeval ts;
    struct timeval tv;
    struct timeval dif;
    int us;

    (void)gettimeofday(&tv, 0);
    ts.tv_sec = (time_t)pTs->sec;
    ts.tv_usec = (suseconds_t)pTs->usec;

    dif.tv_sec = tv.tv_sec - ts.tv_sec;
    dif.tv_usec = tv.tv_usec - ts.tv_usec;
    if (dif.tv_usec < 0) {
        --dif.tv_sec;
        dif.tv_usec += USEC_PER_SEC;
    }

    us = (int)(dif.tv_usec + dif.tv_sec * USEC_PER_SEC);

    if (NULL != pDif) {
        pDif->sec = dif.tv_sec;
        pDif->usec = dif.tv_usec;
    }
    return us;
}
