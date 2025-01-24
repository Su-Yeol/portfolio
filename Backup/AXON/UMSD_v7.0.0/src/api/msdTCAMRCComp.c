/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdTCAMRCComp.c
*
* DESCRIPTION:
*       API definitions for TCAM range check comparator
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdTCAMRCComp.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdTcamRCCompAdd
*
* DESCRIPTION:
*       This routine configures a Range Check Compatator for the specified port.
*
* INPUTS:
*       devNum			- physical device number
*       portNum			- Logical port number to set
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS msdTcamRCCompAdd
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
IN  MSD_TCAM_RC_COMP_DATA *tcamRCCompData
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
		if (dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad(dev, portNum, tcamRCCompData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCCompAllDelete
*
* DESCRIPTION:
*       This routine is to flush all Port Range Comparators.Based on the function
*		of Entry Delete. Set TCAM Page2 Block6 Reg1 to 0x1F.
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
MSD_STATUS msdTcamRCCompAllDelete
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
		if (dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCCompFlushPort
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum			- Logical port number to set
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
MSD_STATUS msdTcamRCCompFlushPort
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum
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
		if (dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTcamRCCompRead
*
* DESCRIPTION:
*       This routine reads a Range Check Compatator for the specified port.
*
* INPUTS:
*       devNum			- physical device number
*       portNum			- Logical port number to set
*
* OUTPUTS:
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS msdTcamRCCompRead
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
OUT MSD_TCAM_RC_COMP_DATA *tcamRCCompData
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
		if (dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead != NULL)
		{
			retVal = dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead(dev, portNum, tcamRCCompData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}