/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdEEPROM.c
*
* DESCRIPTION:
*       API definitions for EEPROM 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdEEPROM.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdEEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write data to EEPROM
*
*
* INPUTS:
*      addr - EEPROM address
*      data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEEPROMWrite
(
	IN  MSD_U8		devNum,
	IN  MSD_U16		addr,
	IN  MSD_U16		data
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
		if (dev->SwitchDevObj.EEPROMObj.geepromWrite != NULL)
		{
			retVal = dev->SwitchDevObj.EEPROMObj.geepromWrite(dev, addr, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdEEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one data from EEPROM
*
*
* INPUTS:
*      addr - EEPROM address
*
* OUTPUTS:
*      data - EEPROM Data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEEPROMRead
(
	IN  MSD_U8		devNum,
	IN  MSD_U16		addr,
	OUT MSD_U16		*data
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
		if (dev->SwitchDevObj.EEPROMObj.geepromRead != NULL)
		{
			retVal = dev->SwitchDevObj.EEPROMObj.geepromRead(dev, addr, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdEEPROMChipSelSet
*
* DESCRIPTION:
*       This routine is used to select one external EERPOM chip to operation
*
*
* INPUTS:
*      chipSelect - selected EEPROM chip number
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEEPROMChipSelSet
(
	IN  MSD_U8  devNum,
	IN  MSD_U8  chipSelect
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
		if (dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel != NULL)
		{
			retVal = dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel(dev, chipSelect);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdEEPROMChipSelGet
*
* DESCRIPTION:
*       This routine is used to get which external EERPOM chip is actively been operated on
*
*
* INPUTS:
*      None
*
* OUTPUTS:
*      chipSelect - selected EEPROM chip number
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdEEPROMChipSelGet
(
	IN  MSD_U8		devNum,
	OUT  MSD_U8		*chipSelect
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
		if (dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel != NULL)
		{
			retVal = dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel(dev, chipSelect);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
