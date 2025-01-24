/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdEEPROM.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdEEPROM.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Amethyst_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
);
static MSD_STATUS Amethyst_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
);
static MSD_STATUS Amethyst_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT	MSD_U16	*data
);
static MSD_STATUS Amethyst_EEPROM_Chip_Select_Set
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
);
static MSD_STATUS Amethyst_EEPROM_Chip_Select_Get
(
IN  MSD_QD_DEV *dev,
OUT MSD_U8  *chipSelect
);
/*******************************************************************************
* Amethyst_EEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write one byte EEPROM data
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
MSD_STATUS Amethyst_EEPROMWrite
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Amethyst_EEPROMWrite Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_EEPROM_Write(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_EEPROMWrite returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_EEPROMWrite Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Amethyst_EEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one byte EEPROM data
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
MSD_STATUS Amethyst_EEPROMRead
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT MSD_U16  *data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Amethyst_EEPROMRead Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_EEPROM_Read(dev, addr, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_EEPROMRead returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_EEPROMRead Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Amethyst_EEPROMChipSelSet
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
MSD_STATUS Amethyst_EEPROMChipSelSet
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Amethyst_EEPROMChipSelSet Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_EEPROM_Chip_Select_Set(dev, chipSelect);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_EEPROMChipSelSet returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_EEPROMChipSelSet Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Amethyst_EEPROMChipSelGet
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
MSD_STATUS Amethyst_EEPROMChipSelGet
(
IN  MSD_QD_DEV *dev,
OUT  MSD_U8  *chipSelect
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Amethyst_EEPROMChipSelGet Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_EEPROM_Chip_Select_Get(dev, chipSelect);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_EEPROMChipSelGet returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_EEPROMChipSelGet Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Amethyst_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 data = 1;

	while (data == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 20, 15, 1, &data);
		if (retVal != MSD_OK)
			return MSD_FAIL;
	}

	return MSD_OK;
}
static MSD_STATUS Amethyst_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;
	
	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (3 << 12) | (1 << 10) | (data & 0xff);

	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return retVal;
}
static MSD_STATUS Amethyst_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15, tmpData;
	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x15, reg15);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (4 << 12);

	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}
	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}
	retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, &tmpData);
	*data = tmpData & 0xFF;
	return retVal;

}
static MSD_STATUS Amethyst_EEPROM_Chip_Select_Set
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14;
	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}
	reg14 = 0x8000 | (1 << 12) | (1 << 10) | (chipSelect & 0x7);
	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, reg14);
	return retVal;
}
static MSD_STATUS Amethyst_EEPROM_Chip_Select_Get
(
IN  MSD_QD_DEV *dev,
OUT MSD_U8  *chipSelect
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, tmpData;

	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	reg14 = 0x8000 | (5 << 12);
	retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, reg14);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = Amethyst_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, 0x14, &tmpData);
	*chipSelect = tmpData & 0x07;
	return retVal;
}