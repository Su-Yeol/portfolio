/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* msdSem.c
*
* DESCRIPTION:
*       Semaphore related routines
*
* DEPENDENCIES:
*       OS Dependent.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

static MSD_U32 refCnt = 0;

/*******************************************************************************
* msdSemCreate
*
* DESCRIPTION:
*       Create semaphore.
*
* INPUTS:
*        state - beginning state of the semaphore, either MSD_SEM_EMPTY or MSD_SEM_FULL
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
)
{
	MSD_SEM msdSem;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		msdSem = 1;
	}
	else
	{
		if (dev->semCreate)
		{
			msdSem = dev->semCreate(state);
		}
		else
		{
			msdSem = 1;
		}
	}

	return msdSem; /* should return any value other than 0 to let it keep going */
}

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
    IN MSD_U8    devNum,
    IN MSD_SEM smid
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		if ((dev->semDelete) && (smid))
		{
			retVal = dev->semDelete(smid);
		}
		else
		{
			retVal = MSD_FAIL;
		}
	}

	return retVal;
}


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
    IN MSD_U8  devNum,
    IN MSD_SEM smid, 
    IN MSD_U32 timeOut
)
{
	MSD_STATUS retVal;
	MSD_U16 tmpSem = 0;
	MSD_BOOL flag = MSD_TRUE;

	/* initial device structure */
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->HWSemaphore != MSD_HW_SEM_DISABLE)
		{
			/* read back semaphore value from hardware G1, offset 0x15 */
			retVal = msdGetAnyReg(devNum, OS_HW_SEMAPHORE_DEV_REG, OS_HW_SEMAPHORE_REG, &tmpSem);
			if (retVal != MSD_OK)
			{
				return retVal;
			}

			/* if read back value match semaphore value, means already take hardware semaphore successfully */
			if (tmpSem == dev->HWSemaphore)
			{
				/*  recursive semaphore count increment */
				refCnt += 1U;
			}
			else
			{
				/* wait until semaphore take success */
				while (flag == MSD_TRUE)
				{
					/* write semaphore value to hardware G1, offset 0x15 */
					retVal = msdSetAnyReg(devNum, OS_HW_SEMAPHORE_DEV_REG, OS_HW_SEMAPHORE_REG, dev->HWSemaphore);
					if (retVal != MSD_OK)
					{
						return retVal;
					}

					/* read back semaphore value from hardware G1, offset 0x15 */
					retVal = msdGetAnyReg(devNum, OS_HW_SEMAPHORE_DEV_REG, OS_HW_SEMAPHORE_REG, &tmpSem);
					if (retVal != MSD_OK)
					{
						return retVal;
					}

					/* if read back value equal to 0x0 or semaphore value, means take hardware semaphore successfully */
					if (tmpSem == 0U || tmpSem == dev->HWSemaphore)
					{
						/*  recursive semaphore count increment */
						refCnt += 1U;
						break;
					}
				}
			}
		}

		/* software semaphore takes */
		if (dev->semTake)
		{
			retVal = dev->semTake(smid, timeOut);
		}
		else
		{
			retVal = MSD_FAIL;
		}
	}

	return retVal;
}

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
    IN MSD_U8		devNum,
    IN MSD_SEM       smid
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->HWSemaphore != MSD_HW_SEM_DISABLE)
		{
			/*  recursive semaphore count decrement*/
			refCnt -= 1U;

			/* write 0x0 to hardware G1, offset 0x15 semaphore register, release semaphore */
			if (refCnt == 0U)
			{
				retVal = msdSetAnyReg(devNum, OS_HW_SEMAPHORE_DEV_REG, OS_HW_SEMAPHORE_REG, 0x0);
				if (retVal != MSD_OK)
				{
					return retVal;
				}
			}
		}

		/* software semaphore release */
		if (dev->semGive)
		{
			retVal = dev->semGive(smid);
		}
		else
		{
			retVal = MSD_FAIL;
		}
	}

	return retVal;
}