/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdSem.h
*
* DESCRIPTION:
*       Semaphore releated
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdSem_H
#define msdSem_H

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/***** Defines  ********************************************************/

#define OS_WAIT_FOREVER             0

#define OS_HW_SEMAPHORE_DEV_REG     0x1B
#define OS_HW_SEMAPHORE_REG         0x15

#define OS_MAX_TASKS                30
#define OS_MAX_TASK_NAME_LENGTH     10

#define OS_MAX_QUEUES               30
#define OS_MAX_QUEUE_NAME_LENGTH    10

#define OS_MAX_EVENTS               10

#define OS_MAX_SEMAPHORES           50

#define OS_EOF                      (-1)



/*******************************************************************************
* msdSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*        state - beginning state of the semaphore, either SEM_EMPTY or SEM_FULL
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_SEM if success. Otherwise, NULL
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_SEM msdSemCreate
(
    IN MSD_U8    devNum,
    IN MSD_SEM_BEGIN_STATE state
);

/*******************************************************************************
* msdSemDelete
*
* DESCRIPTION:
*       Delete semaphore.
*
* INPUTS:
*       smid - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemDelete
(
    IN MSD_U8		devNum,
    IN MSD_SEM       smid
);

/*******************************************************************************
* msdSemTake
*
* DESCRIPTION:
*       Wait for semaphore.
*
* INPUTS:
*       smid    - semaphore Id
*       timeOut - time out in miliseconds or 0 to wait forever
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       OS_TIMEOUT - on time out
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemTake
(
    IN MSD_U8	    devNum,
    IN MSD_SEM       smid,
    IN MSD_U32       timeOut
);

/*******************************************************************************
* msdSemGive
*
* DESCRIPTION:
*       release the semaphore which was taken previously.
*
* INPUTS:
*       smid    - semaphore Id
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdSemGive
(
    IN MSD_U8	    devNum,
    IN MSD_SEM       smid
);

#ifdef __cplusplus
}
#endif

#endif  /* __msdSemh */
/* Do Not Add Anything Below This Line */
