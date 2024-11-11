/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdIMP.c
*
* DESCRIPTION:
*       API definitions for IMP 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdIMP.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdImpRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - specified start address
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
MSD_STATUS msdImpRun
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  addr
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
		if (dev->SwitchDevObj.IMPObj.impRun != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impRun(dev, addr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdImpReset
*
* DESCRIPTION:
*       This routine is to reset imp
*
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
MSD_STATUS msdImpReset
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
		if (dev->SwitchDevObj.IMPObj.impReset != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impReset(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdImpWriteEEPROM
*
* DESCRIPTION:
*       This routine is to write data to EEPROM
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - EEPROM address
*       data - data to be written
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
MSD_STATUS msdImpWriteEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  addr,
    IN  MSD_U8  data
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
		if (dev->SwitchDevObj.IMPObj.impWriteEEPROM != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impWriteEEPROM(dev, addr, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdImpLoadToEEPROM
*
* DESCRIPTION:
*       This routine is to load data to EEPROM devices
*
*
* INPUTS:
*       devNum  - physical device number
*       datasize - data size
*       data - data to be written
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
MSD_STATUS msdImpLoadToEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  dataSize,
    IN  MSD_U8*  data
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
		if (dev->SwitchDevObj.IMPObj.impLoadToEEPROM != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impLoadToEEPROM(dev, dataSize, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdImpLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to RAM.
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - start address
*       datasize - data size
*       data - data to be written
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
MSD_STATUS msdImpLoadToRAM
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  addr,
    IN  MSD_U32  dataSize,
    IN  MSD_U8*  data
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
		if (dev->SwitchDevObj.IMPObj.impLoadToRAM != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impLoadToRAM(dev, addr, dataSize, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdImpStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
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
MSD_STATUS msdImpStop
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
		if (dev->SwitchDevObj.IMPObj.impStop != NULL)
		{
			retVal = dev->SwitchDevObj.IMPObj.impStop(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

