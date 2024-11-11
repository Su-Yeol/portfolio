/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Oak_msdEEPROM.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdEEPROM.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Oak_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
);
static MSD_STATUS Oak_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
);
static MSD_STATUS Oak_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT	MSD_U16	*data
);
static MSD_STATUS Oak_EEPROM_Chip_Select_Set
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
);
static MSD_STATUS Oak_EEPROM_Chip_Select_Get
(
IN  MSD_QD_DEV *dev,
OUT MSD_U8  *chipSelect
);
/*******************************************************************************
* Oak_EEPROMWrite
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
MSD_STATUS Oak_EEPROMWrite
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Oak_EEPROMWrite Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Oak_EEPROM_Write(dev, addr, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_EEPROMWrite returned: %d.\n", msdDisplayStatus(retVal)));
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Oak_EEPROMWrite Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_EEPROMRead
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
MSD_STATUS Oak_EEPROMRead
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT MSD_U16  *data
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Oak_EEPROMRead Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Oak_EEPROM_Read(dev, addr, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_EEPROMRead returned: %d.\n", msdDisplayStatus(retVal)));
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Oak_EEPROMRead Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Oak_EEPROMChipSelSet
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
MSD_STATUS Oak_EEPROMChipSelSet
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Oak_EEPROMChipSelSet Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Oak_EEPROM_Chip_Select_Set(dev, chipSelect);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_EEPROMChipSelSet returned: %d.\n", msdDisplayStatus(retVal)));
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Oak_EEPROMChipSelSet Exit.\n"));
	return retVal;
}
/*******************************************************************************
* Oak_EEPROMChipSelGet
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
MSD_STATUS Oak_EEPROMChipSelGet
(
IN  MSD_QD_DEV *dev,
OUT  MSD_U8  *chipSelect
)
{
	MSD_STATUS retVal;
	MSD_DBG_INFO(("Oak_EEPROMChipSelGet Called.\n"));

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Oak_EEPROM_Chip_Select_Get(dev, chipSelect);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_EEPROMChipSelGet returned: %d.\n", msdDisplayStatus(retVal)));
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	MSD_DBG_INFO(("Oak_EEPROMChipSelGet Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Oak_waitBusyBitSelfClear
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U16 data = (MSD_U16)1;

	while (data == (MSD_U16)1)
	{
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)20, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Oak_EEPROM_Write
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
IN	MSD_U16	data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15;
	
	retVal = Oak_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		reg15 = addr;
		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x15, reg15);
		if (retVal != MSD_OK)
		{
			retVal = MSD_FAIL;
		}
		else
		{
			reg14 = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)3 << 12) | (MSD_U16)((MSD_U16)1 << 10) | (data & (MSD_U16)0xff);

			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, reg14);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				retVal = Oak_waitBusyBitSelfClear(dev);
				if (retVal != MSD_OK)
				{
					retVal = MSD_FAIL;
				}
			}
		}
	}

	return retVal;
}

static MSD_STATUS Oak_EEPROM_Read
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	addr,
OUT MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, reg15, tmpData;
	reg15 = addr;
	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x15, reg15);
	if (retVal != MSD_OK)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		retVal = Oak_waitBusyBitSelfClear(dev);
		if (retVal != MSD_OK)
		{
			retVal = MSD_FAIL;
		}
		else
		{
			reg14 = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)4 << 12);

			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, reg14);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				retVal = Oak_waitBusyBitSelfClear(dev);
				if (retVal != MSD_OK)
				{
					retVal = MSD_FAIL;
				}
				else
				{
					retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, &tmpData);
					*data = tmpData & (MSD_U16)0xFF;
				}
			}
		}
	}

	return retVal;

}

static MSD_STATUS Oak_EEPROM_Chip_Select_Set
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  chipSelect
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14;
	retVal = Oak_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		reg14 = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)1 << 12) | (MSD_U16)((MSD_U16)1 << 10) | (chipSelect & (MSD_U16)0x7);
		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, reg14);
	}

	return retVal;
}

static MSD_STATUS Oak_EEPROM_Chip_Select_Get
(
IN  MSD_QD_DEV *dev,
OUT MSD_U8  *chipSelect
)
{
	MSD_STATUS retVal;
	MSD_U16 reg14, tmpData;

	retVal = Oak_waitBusyBitSelfClear(dev);
	if (retVal != MSD_OK)
	{
		retVal = MSD_FAIL;
	}
	else
	{
		reg14 = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)5 << 12);
		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, reg14);
		if (retVal != MSD_OK)
		{
			retVal = MSD_FAIL;
		}
		else
		{
			retVal = Oak_waitBusyBitSelfClear(dev);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)0x14, &tmpData);
				*chipSelect = (MSD_U8)(tmpData & (MSD_U16)0x07);
			}
		}
	}

	return retVal;
}