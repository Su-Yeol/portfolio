/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdTCAMRC.c
*
* DESCRIPTION:
*       API definitions for TCAM range check
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdTCAMRC.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdTcamRCEntryAdd
*
* DESCRIPTION:
*       This routine loads a Range Check entry.
*
* INPUTS:
*       devNum	     - Device number
*		rcEntryNum   - Pointer to the desired entry of RC table
*		rcData       - Range Check entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCEntryAdd
(
IN  MSD_U8  devNum,
IN  MSD_U32  rcEntryNum,
IN  MSD_TCAM_RC_DATA *rcData
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry(dev, rcEntryNum, rcData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCAllDelete
*
* DESCRIPTION:
*       This routine is to flush all TCAM Range Check entries.Based on the function 
*		of Entry Delete. Set TCAM Page2 Block7 Reg1 to 0x1F.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCAllDelete
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCEntryDelete
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry. 
*
*
* INPUTS:
*       devNum      - physical device number
*       rcEntryNum  - pointer to the desired entry of RC table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCEntryDelete
(
IN  MSD_U8  devNum,
IN  MSD_U32 rcEntryNum
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry(dev, rcEntryNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCEntryRead
*
* DESCRIPTION:
*       This routine reads the TCAM RC entry.
*
*
* INPUTS:
*       devNum      - physical device number
*       rcEntryNum  - pointer to the desired entry of RC table
*
* OUTPUTS:
*       rcData     - RC entry Data
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCEntryRead
(
IN  MSD_U8  devNum,
IN  MSD_U32 rcEntryNum,
OUT MSD_TCAM_RC_DATA  *rcData
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry(dev, rcEntryNum, rcData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdTcamRCEntryGetNext
*
* DESCRIPTION:
*       This routine finds the next higher RC Entry number that is valid starting 
*		from the specified RC entry.
*
* INPUTS:
*       devNum      - physical device number
*       rcEntryNum  - pointer to the desired entry of RC table
*
* OUTPUTS:
*       rcEntryNum  - pointer to the desired entry of RC table
*       rcData      - RC entry Data
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no valid entry exist
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCEntryGetNext
(
IN    MSD_U8  devNum,
INOUT MSD_U32 *rcEntryNum,
OUT   MSD_TCAM_RC_DATA *rcData
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry(dev, rcEntryNum, rcData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdTcamRCEntryFind
*
* DESCRIPTION:
*       Finds the specified valid RC entry in ingress RC table.
*
* INPUTS:
*       devNum      - physical device number
*       rcEntryNum  - pointer to the desired entry of RC table
*
* OUTPUTS:
*       rcData      - RC entry Data
*       found    - MSD_TRUE, if the appropriate entry exists.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS msdTcamRCEntryFind
(
IN  MSD_U8  devNum,
IN	MSD_U32 rcEntryNum,
OUT	MSD_TCAM_RC_DATA  *rcData,
OUT	MSD_BOOL *found
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry(dev, rcEntryNum, rcData, found);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCDump
*
* DESCRIPTION:
*       Finds all valid RC entries and print it out.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTcamRCDump
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
		if (dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}