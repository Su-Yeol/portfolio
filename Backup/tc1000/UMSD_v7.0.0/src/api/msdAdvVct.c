/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdAdvVct.c
*
* DESCRIPTION:
*       API definitions for Virtual Cable Test
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdAdvVct.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdVctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
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
MSD_STATUS msdVctGetAdvCableDiag
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_ADV_VCT_MODE  mode,
	OUT MSD_ADV_CABLE_STATUS  *cableStatus
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
		if (dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag != NULL)
		{
			retVal = dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag(dev, portNum, mode, cableStatus);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdVctGetAdvExtendStatus
*
* DESCRIPTION:
*       This routine retrieves extended cable status, such as Pair Poloarity,
*       Pair Swap, and Pair Skew. Note that this routine will be success only
*       if 1000Base-T Link is up.
*       DSP based cable length is also provided.
*
* INPUTS:
*       devNum  - physical device number
*       dev     - pointer to MSD driver structure returned from msdLoadDriver
*       portNum - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdVctGetAdvExtendStatus
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	OUT MSD_ADV_EXTENDED_STATUS  *extendedStatus
)
{
	MSD_STATUS  retVal;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus != NULL)
		{
			retVal = dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus(dev, portNum, extendedStatus);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
