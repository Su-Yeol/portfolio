/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID Translation Unit
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdBrgStu.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdStuEntryAdd
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on user input.
*
* INPUTS:
*       stuEntry  - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdStuEntryAdd
(
    IN MSD_U8 devNum,
    IN MSD_STU_ENTRY *stuEntry
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.STUObj.gstuAddEntry != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuAddEntry(dev, stuEntry);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStuEntryNextGet
*
* DESCRIPTION:
*       Gets next valid STU entry from the specified SID.
*
* INPUTS:
*       sid  - the SID to start the search
*
* OUTPUTS:
*       stuEntry - next valid stu entry
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*       MSD_NO_SUCH - no more entries
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdStuEntryNextGet
(
	IN  MSD_U8 devNum,
	IN  MSD_U16 sid,
	OUT MSD_STU_ENTRY *stuEntry
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		stuEntry->sid = sid;
		if (dev->SwitchDevObj.STUObj.gstuGetEntryNext != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuGetEntryNext(dev, stuEntry);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStuEntryDelete
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       devNum - physical device number
*       sid    - SID ID, valid value is 0 ~ 63
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdStuEntryDelete
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 sid
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.STUObj.gstuDelEntry != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuDelEntry(dev, sid);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStuEntryFind
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       devNum - physical device number
*       sid    - SID ID, valid value is 0 ~ 63
*
* OUTPUTS:
*       stuEntry - pointer to the STU entry structure
*       found    - 1/0 for found / not found
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdStuEntryFind
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.STUObj.gstuFindSidEntry != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuFindSidEntry(dev, sid, stuEntry, found);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStuEntryCountGet
*
* DESCRIPTION:
*       Gets number of valid entries in the STU table
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       count  - number of valid entries
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdStuEntryCountGet
(
    IN  MSD_U8  devNum,
    OUT MSD_U16 *count
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.STUObj.gstuGetEntryCount != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuGetEntryCount(dev, count);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStuDump
*
* DESCRIPTION:
*       Finds all STU Entries and print it out.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdStuDump
(
	IN  MSD_U8  devNum
)
{
	MSD_STATUS retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.STUObj.gstuDump != NULL)
		{
			retVal = dev->SwitchDevObj.STUObj.gstuDump(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}