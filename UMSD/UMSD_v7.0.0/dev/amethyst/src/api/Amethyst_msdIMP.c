/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdIMP.c
*
* DESCRIPTION:
*       API definitions for Marvell IMP related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdIMP.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* internal IMP related function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Amethyst_IMP_ByPassMode_Get
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_BOOL *isEnable
);
static MSD_BOOL Amethyst_IMP_ByPassMode_Set
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_BOOL isEnable
);
static MSD_STATUS Amethyst_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
);

static MSD_STATUS Amethyst_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
);

static MSD_STATUS Amethyst_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Amethyst_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Amethyst_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Amethyst_IMP_Reset
(
	IN	MSD_QD_DEV	*dev
);

static MSD_STATUS Amethyst_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
);
/*******************************************************************************
* Amethyst_impRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*        addr - specified start address
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impRun
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer_MMUBypass = 0x07;
	MSD_U16 High4bitsAddr = (addr & 0xf0000) >> 16;
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x1;

	MSD_U16 pointer_AddrLo = 0x0A;
	MSD_U16 LowAddr = addr & 0xff;

	MSD_U16 pointer_AddrHi = 0x0B;
	MSD_U16 HighAddr = (addr & 0xff00) >> 8;

	MSD_BOOL isByPassMode;
	MSD_DBG_INFO(("Amethyst_impRun Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_IMP_Stop(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}
	retVal = Amethyst_IMP_Reset(dev);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_Reset returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Amethyst_IMP_ByPassMode_Get(dev, pointer_MMUBypass, &isByPassMode);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_ByPassMode_Get returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}
	retVal = Amethyst_IMP_Write(dev, pointer_MMUBypass, (1 << 7) | High4bitsAddr);
	retVal |= Amethyst_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Amethyst_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Amethyst_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_EXamine returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	retVal = Amethyst_IMP_ByPassMode_Set(dev, pointer_MMUBypass, isByPassMode);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_ByPassMode_Set returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Amethyst_IMP_Write(dev, pointer, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impRun Amethyst_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}
	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_impRun Exit.\n"));

	return retVal;		
}

/*******************************************************************************
* Amethyst_impReset
*
* DESCRIPTION:
*       This routine is to reset imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impReset
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer = 0x08;
	MSD_U16 data = 0x0f;

	MSD_DBG_INFO(("Amethyst_impReset Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_IMP_Write(dev, pointer, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impReset Amethyst_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_impReset Exit.\n"));

	return retVal;
}


/*******************************************************************************
* Amethyst_impLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to memory
*
*
* INPUTS:
*        addr - start address
*        datasize - data size
*        data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	dataSize,
	IN	MSD_U8*	data
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Amethyst_impLoadToRAM Called.\n"));
	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_IMP_Stop(dev);

	/* work around*/
	retVal |= Amethyst_IMP_Write(dev, 0x2f, 0);
	retVal |= Amethyst_IMP_Write(dev, 0x70, 0x08);		
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impLoadToRAM Amethyst_IMP_Write returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	retVal = Amethyst_IMP_Download(dev, addr, dataSize, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impLoadToRAM Amethyst_IMP_Download returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return MSD_FAIL;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_impLoadToRAM Exit.\n"));

	return retVal;
}


/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Amethyst_IMP_ByPassMode_Get
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	pointer,
OUT	MSD_BOOL *isEnable
)
{
	MSD_STATUS retVal;
	MSD_U16 data;
	retVal = Amethyst_IMP_Read(dev, pointer, &data);
	if ((data & 0x80) != 0)
		*isEnable = MSD_TRUE;
	else
		*isEnable = MSD_FALSE;
	return retVal;
}
static MSD_BOOL Amethyst_IMP_ByPassMode_Set
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U16	pointer,
IN	MSD_BOOL isEnable
)
{
	MSD_STATUS retVal;
	MSD_U16 data;
	retVal = Amethyst_IMP_Read(dev, pointer, &data);
	if (retVal != MSD_OK)
		return MSD_FAIL;
	if (MSD_TRUE == isEnable)
		data = 0x80 | data;
	else
		data = 0x7F & data;
	retVal = Amethyst_IMP_Write(dev, pointer, data);
	return retVal;
}
static MSD_STATUS Amethyst_IMP_Write
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	IN	MSD_U16	data
)
{
	MSD_STATUS retVal;

	MSD_U8 regAddr = 0x13;
	data = 0x8000 | ((0x7F & pointer) << 8) | (data & 0xFF);

	retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, regAddr, data);

	return retVal;
}

static MSD_STATUS Amethyst_IMP_Read
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U16	pointer,
	OUT	MSD_U16	*data
)
{
	MSD_STATUS retVal;
	MSD_U8 regAddr = 0x13;

	pointer = (pointer & 0x7f) << 8;
	retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, regAddr, pointer);
	retVal |= msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL2_DEV_ADDR, regAddr, data);
	*data = *data & 0xff;

	return retVal;
}

static MSD_STATUS Amethyst_IMP_De_Next
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x6;

	retVal = Amethyst_IMP_Write(dev, pointer, data);
	return retVal;
}

static MSD_STATUS Amethyst_IMP_EXamine
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x3;
	return Amethyst_IMP_Write(dev, pointer, data);
}

static MSD_STATUS Amethyst_IMP_Stop
(
	IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0x2;

	retVal = Amethyst_IMP_Write(dev, pointer, data);

	return retVal;
}

static MSD_STATUS Amethyst_IMP_Reset
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_U16 pointer = 0x8;
	MSD_U16 data = 0xf;

	retVal = Amethyst_IMP_Write(dev, pointer, data);

	return retVal;
}

static MSD_STATUS Amethyst_IMP_Download
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	datasize,
	IN	MSD_U8	*data
)
{
	MSD_STATUS retVal;
	MSD_U32 Idx;

	MSD_U16 pointer_AddrLo = 0x0A;
	MSD_U16 LowAddr = (MSD_U16)(addr & 0xff);

	MSD_U16 pointer_AddrHi = 0x0B;
	MSD_U16 HighAddr = (MSD_U16)((addr & 0xff00) >> 8);

	MSD_U16 pointer_WrData = 0x0c;
	MSD_U16 pointer_MMUBypass = 0x07;
	MSD_U16 High4bitsAddr = (addr & 0xf0000) >> 16;
	MSD_BOOL isByPassMode;
	retVal = Amethyst_IMP_ByPassMode_Get(dev, pointer_MMUBypass, &isByPassMode);

	retVal |= Amethyst_IMP_Write(dev, pointer_MMUBypass, (1 << 7) | High4bitsAddr);
	retVal |= Amethyst_IMP_Write(dev, pointer_AddrLo, LowAddr);
	retVal |= Amethyst_IMP_Write(dev, pointer_AddrHi, HighAddr);

	retVal |= Amethyst_IMP_EXamine(dev);

	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	if (datasize > 0)
	{
		for (Idx = 0; Idx < datasize; Idx++)
		{
			Amethyst_IMP_Write(dev, pointer_WrData, (MSD_U16)data[Idx]);
			Amethyst_IMP_De_Next(dev);
		}
	}
	retVal = Amethyst_IMP_ByPassMode_Set(dev, pointer_MMUBypass, isByPassMode);
	if (retVal != MSD_OK)
	{
		return MSD_FAIL;
	}

	return MSD_OK;
}



/*******************************************************************************
* impStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impStop
(
IN	MSD_QD_DEV	*dev
)
{
	MSD_STATUS retVal;

	MSD_DBG_INFO(("Amethyst_impStop Called.\n"));

	msdSemTake(dev->devNum,  dev->tblRegsSem, OS_WAIT_FOREVER);

	retVal = Amethyst_IMP_Stop(dev);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_impStop Amethyst_IMP_Stop returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum,  dev->tblRegsSem);
	MSD_DBG_INFO(("Amethyst_impStop Exit.\n"));

	return retVal;
}
