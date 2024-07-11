/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Spruce_msdArp.c
*
* DESCRIPTION:
*       API definitions for ARP table 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdArp.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>

/****************************************************************************/
/* Internal ARP structure declaration.                                    */
/****************************************************************************/
/*
*  typedef: struct SPRUCE_MSD_ARP_DATA_HW
*
*  Description: data required by ARP Page - TCAM entry page 3 Block 0x08
*
*  Fields:
*
*/
typedef struct
{
	MSD_U16  frame[3];  /* RC action part */
} SPRUCE_MSD_ARP_DATA_HW;

typedef struct
{
	MSD_U32    arpEntry;
	SPRUCE_MSD_ARP_DATA_HW     arpDataP;
} SPRUCE_MSD_ARP_OP_DATA;

typedef enum
{
	Spruce_ARP_FLUSH_ALL = 0x1,
	Spruce_ARP_FLUSH_ENTRY = 0x2,
	Spruce_ARP_LOAD_ENTRY = 0x3,
	Spruce_ARP_PURGE_ENTRY = 0x3,
	Spruce_ARP_GET_NEXT_ENTRY = 0x4,
	Spruce_ARP_READ_ENTRY = 0x5
} SPRUCE_MSD_ARP_OPERATION;

static MSD_STATUS Spruce_setArpHWMcData
(
IN	const SPRUCE_MSD_ARP_MC_DATA	*iData,
OUT	SPRUCE_MSD_ARP_DATA_HW	*oData
);

static MSD_STATUS Spruce_setArpHWUcData
(
IN	const SPRUCE_MSD_ARP_UC_DATA	*iData,
OUT	SPRUCE_MSD_ARP_DATA_HW	*oData
);

static MSD_STATUS Spruce_getArpHWData
(
IN	const SPRUCE_MSD_ARP_DATA_HW	*iData,
OUT	SPRUCE_MSD_ARP_DATA	*oData
);

static MSD_STATUS Spruce_arpSetPage3Data(const MSD_QD_DEV *dev, const SPRUCE_MSD_ARP_DATA_HW *arpDataPtr);
static MSD_STATUS Spruce_arpGetPage3Data(const MSD_QD_DEV *dev, SPRUCE_MSD_ARP_DATA_HW *arpDataPtr);
static MSD_STATUS Spruce_waitArpReady(const MSD_QD_DEV *dev);
static MSD_STATUS Spruce_arpOperationPerform
(
IN    const MSD_QD_DEV        *dev,
IN    SPRUCE_MSD_ARP_OPERATION   arpOp,
INOUT SPRUCE_MSD_ARP_OP_DATA     *opData
);
static MSD_STATUS Spruce_setArpExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port);

/*******************************************************************************
* Spruce_garpLoadMcEntry
*
* DESCRIPTION:
*       This routine loads an ARP entry for multicast.
*
* INPUTS:
*       devNum			- physical device number
*       arpEntryNum		- Pointer to the desired entry of ARP
*       arpMcData		- For multicast, this data becomes the value for the number
*						  of frames to duplicate for each egress port as follows:
*						  arpMcData[0] = P0_DUP
*						  arpMcData[1] = P1_DUP
*						  arpMcData[2] = P2_DUP
*						  arpMcData[3] = P3_DUP
*						  arpMcData[4] = P4_DUP
*						  arpMcData[5] = P5_DUP
*						  arpMcData[6] = P6_DUP
*						  arpMcData[7] = P7_DUP
*						  arpMcData[8] = P8_DUP
*						  arpMcData[9] = P9_DUP
*						  arpMcData[10] = P10_DUP
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
MSD_STATUS Spruce_garpLoadMcEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
IN  SPRUCE_MSD_ARP_MC_DATA   *arpMcData
)
{
	MSD_STATUS  retVal;         /* Functions return value.*/
	SPRUCE_MSD_ARP_OPERATION   op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	MSD_DBG_INFO(("Spruce_garpLoadMcEntry Called.\n"));

	/* check if the given pointer is valid */
	if ((arpEntryNum >= SPRUCE_MAX_ARP_ENTRY) || (NULL == arpMcData))
	{
		MSD_DBG_ERROR(("Bad arpEntryNum %u out of range OR arpMcData is NULL.\n", arpEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_ARP_LOAD_ENTRY;
		arpOpData.arpEntry = arpEntryNum;
		retVal = Spruce_setArpHWMcData(arpMcData, &arpOpData.arpDataP);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_setArpHWMcData returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_arpOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Spruce_garpLoadMcEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_garpLoadUcEntry
*
* DESCRIPTION:
*       This routine loads an ARP entry for unicast.
*
* INPUTS:
*       devNum			- physical device number
*       arpEntryNum		- Pointer to the desired entry of ARP
*       arpUcData		- For unicast, this field is the ARP DA MAC address which is
*						  use to replace DA field of the Unicast routing frame.
*						  arpUcData[0]:
*										RouteDA[47:40]
*						  arpUcData[1]:
*										RouteDA[39:32]
*						  arpUcData[2]:
*										RouteDA[31:24]
*						  arpUcData[3]:
*										RouteDA[23:16]
*						  arpUcData[4]:
*										RouteDA[15:8]
*						  arpUcData[5]:
*										RouteDA[7:0]
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
MSD_STATUS Spruce_garpLoadUcEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
IN  SPRUCE_MSD_ARP_UC_DATA   *arpUcData
)
{
	MSD_STATUS  retVal;         /* Functions return value.*/
	SPRUCE_MSD_ARP_OPERATION   op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	/* check if the given pointer is valid */
	if ((arpEntryNum >= SPRUCE_MAX_ARP_ENTRY) || (NULL == arpUcData))
	{
		MSD_DBG_ERROR(("Bad arpEntryNum %u out of range OR arpUcData is NULL.\n", arpEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_ARP_LOAD_ENTRY;
		arpOpData.arpEntry = arpEntryNum;
		retVal = Spruce_setArpHWUcData(arpUcData, &arpOpData.arpDataP);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_setArpHWUcData returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_arpOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Spruce_garpLoadUcEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_garpFlushEntry
*
* DESCRIPTION:
*       This routine delete an ARP entry.
*
*
* INPUTS:
*       arpEntryNum		- Pointer to the desired entry of ARP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_garpFlushEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        arpEntryNum
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_ARP_OPERATION    op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	MSD_DBG_INFO(("Spruce_garpFlushEntry Called.\n"));

	/* check if the given pointer is valid */
	if (arpEntryNum >= SPRUCE_MAX_ARP_ENTRY)
	{
		MSD_DBG_ERROR(("Bad arpEntryNum %u. It should be within [0, %u].\n", arpEntryNum, SPRUCE_MAX_ARP_ENTRY - 1U));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_ARP_FLUSH_ENTRY;
		arpOpData.arpEntry = arpEntryNum;
		retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_arpOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Spruce_garpFlushEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_garpFlushAll
*
* DESCRIPTION:
*       This routine is to flush all entries. 
*
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_garpFlushAll
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_ARP_OPERATION    op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	MSD_DBG_INFO(("Spruce_garpFlushAll Called.\n"));

	/* Program Tuning register */
	op = Spruce_ARP_FLUSH_ALL;
	retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_arpOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
	}

	MSD_DBG_INFO(("Spruce_garpFlushAll Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Spruce_garpReadEntry
*
* DESCRIPTION:
*       This routine reads the ARP entry.
*
*
* INPUTS:
*       arpEntryNum		- Pointer to the desired entry of ARP
*       arpData			- ARP entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_garpReadEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        arpEntryNum,
OUT SPRUCE_MSD_ARP_DATA        *arpData
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_ARP_OPERATION    op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	MSD_DBG_INFO(("Spruce_garpReadEntry Called.\n"));


	/* check if the given pointer is valid */
	if ((arpEntryNum >= SPRUCE_MAX_ARP_ENTRY) || (NULL == arpData))
	{
		MSD_DBG_ERROR(("Bad arpEntryNum %u out of range OR arpData is NULL.\n", arpEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Spruce_ARP_READ_ENTRY;
		arpOpData.arpEntry = arpEntryNum;
		/*tcamOpData.tcamDataP = tcamData;*/
		retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_arpOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Spruce_getArpHWData(&arpOpData.arpDataP, arpData);
		}
	}

	MSD_DBG_INFO(("Spruce_garpReadEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_garpGetNextEntry
*
* DESCRIPTION:
*       This routine finds the next higher ARP Entry number that is valid starting
*		from the specified ARP entry.
*
* INPUTS:
*       devNum			- physical device number
*       arpEntryNum		- Pointer to the desired entry of ARP
*
* OUTPUTS:
*       arpEntryNum		- Pointer to the desired entry of ARP
*       arpData			- ARP entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_garpGetNextEntry
(
IN     MSD_QD_DEV     *dev,
INOUT  MSD_U32        *arpEntryNum,
OUT    SPRUCE_MSD_ARP_DATA  *arpData
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_ARP_OPERATION    op;
	SPRUCE_MSD_ARP_OP_DATA     arpOpData;

	MSD_DBG_INFO(("Spruce_garpGetNextEntry Called.\n"));

	/* check if the given pointer is valid */
	if (arpEntryNum == NULL )
	{
		MSD_DBG_ERROR(("arpEntryNum is NULL.\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if ((*arpEntryNum >= SPRUCE_MAX_ARP_ENTRY) || (arpData == NULL))
		{
			MSD_DBG_ERROR(("Bad arpEntryNum value %u out of range OR arpData is NULL.\n", *arpEntryNum));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* Program Tuning register */
			op = Spruce_ARP_GET_NEXT_ENTRY;
			arpOpData.arpEntry = *arpEntryNum;
			/*tcamOpData.tcamDataP = tcamData;*/
			retVal = Spruce_arpOperationPerform(dev, op, &arpOpData);
			if (retVal == MSD_NO_SUCH)
			{
				MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
				retVal = MSD_NO_SUCH;
			}
			else if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_arpOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				*arpEntryNum = arpOpData.arpEntry;
				retVal = Spruce_getArpHWData(&arpOpData.arpDataP, arpData);
			}
		}
	}

	MSD_DBG_INFO(("Spruce_garpGetNextEntry Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/

static MSD_STATUS Spruce_setArpHWMcData
(
IN	const SPRUCE_MSD_ARP_MC_DATA	*iData,
OUT	SPRUCE_MSD_ARP_DATA_HW	*oData
)
{
	msdMemSet(oData, 0, sizeof(SPRUCE_MSD_ARP_DATA_HW));

	MSD_BF_SET(oData->frame[0], iData->dupNum[0], 0, 4);
	MSD_BF_SET(oData->frame[0], iData->dupNum[1], 4, 4);
	MSD_BF_SET(oData->frame[0], iData->dupNum[2], 8, 4);
	MSD_BF_SET(oData->frame[0], iData->dupNum[3], 12, 4);
	MSD_BF_SET(oData->frame[1], iData->dupNum[4], 0, 4);
	MSD_BF_SET(oData->frame[1], iData->dupNum[5], 4, 4);
	MSD_BF_SET(oData->frame[1], iData->dupNum[6], 8, 4);
	MSD_BF_SET(oData->frame[1], iData->dupNum[7], 12, 4);
	MSD_BF_SET(oData->frame[2], iData->dupNum[8], 0, 4);
	MSD_BF_SET(oData->frame[2], iData->dupNum[9], 4, 4);
	MSD_BF_SET(oData->frame[2], iData->dupNum[10], 8, 4);

	return MSD_OK;
}

static MSD_STATUS Spruce_setArpHWUcData
(
IN	const SPRUCE_MSD_ARP_UC_DATA	*iData,
OUT	SPRUCE_MSD_ARP_DATA_HW	*oData
)
{
	msdMemSet(oData, 0, sizeof(SPRUCE_MSD_ARP_DATA_HW));

	MSD_BF_SET(oData->frame[0], iData->routeDA.arEther[5], 0, 8);
	MSD_BF_SET(oData->frame[0], iData->routeDA.arEther[4], 8, 8);
	MSD_BF_SET(oData->frame[1], iData->routeDA.arEther[3], 0, 8);
	MSD_BF_SET(oData->frame[1], iData->routeDA.arEther[2], 8, 8);
	MSD_BF_SET(oData->frame[2], iData->routeDA.arEther[1], 0, 8);
	MSD_BF_SET(oData->frame[2], iData->routeDA.arEther[0], 8, 8);

	return MSD_OK;
}

static MSD_STATUS Spruce_getArpHWData
(
IN	const SPRUCE_MSD_ARP_DATA_HW	*iData,
OUT	SPRUCE_MSD_ARP_DATA	*oData
)
{

	oData->arpData[0] = (MSD_U8)MSD_BF_GET(iData->frame[0], 0, 8);
	oData->arpData[1] = (MSD_U8)MSD_BF_GET(iData->frame[0], 8, 8);
	oData->arpData[2] = (MSD_U8)MSD_BF_GET(iData->frame[1], 0, 8);
	oData->arpData[3] = (MSD_U8)MSD_BF_GET(iData->frame[1], 8, 8);
	oData->arpData[4] = (MSD_U8)MSD_BF_GET(iData->frame[2], 0, 8);
	oData->arpData[5] = (MSD_U8)MSD_BF_GET(iData->frame[2], 8, 8);

	return MSD_OK;
}

static MSD_STATUS Spruce_arpSetPage3Data(const MSD_QD_DEV *dev, const SPRUCE_MSD_ARP_DATA_HW *arpDataPtr)
{
	MSD_STATUS  retVal = MSD_OK;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i<5; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)i, arpDataPtr->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Spruce_arpGetPage3Data(const MSD_QD_DEV *dev, SPRUCE_MSD_ARP_DATA_HW *arpDataPtr)
{
	MSD_STATUS  retVal = MSD_OK;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i<5; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)i, &arpDataPtr->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			break;
		}
	}

	return retVal;
}

static MSD_STATUS Spruce_waitArpReady(const MSD_QD_DEV *dev)
{
	MSD_STATUS       retVal = MSD_OK;    /* Functions return value */

	MSD_U16          tmpdata;     /* temporary Data storage */
	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
	{
		retVal = msdGetAnyRegField(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		if (retVal != MSD_OK)
		{
			break;
		}
	}
	return retVal;
}

/*******************************************************************************
* Spruce_tcamOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       tcamOp   - The tcam operation
*       tcamData - address and data to be written into TCAM
*
* OUTPUTS:
*       tcamData - data read from TCAM pointed by address
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS Spruce_arpOperationPerform
(
IN    const MSD_QD_DEV           *dev,
IN    SPRUCE_MSD_ARP_OPERATION   arpOp,
INOUT SPRUCE_MSD_ARP_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          tmpdata = 0;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Spruce_waitArpReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/*Change Block to 8, indicate ARP Table Page */
	retVal = Spruce_setArpExtensionReg(dev, (MSD_U8)8, 0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set the ARP Operation register */
	switch (arpOp)
	{
	case Spruce_ARP_FLUSH_ALL:
	{
		/* Wait until the ARP in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		tmpdata = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)arpOp << 12) | (MSD_U16)((MSD_U16)3 << 10));
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Spruce_ARP_FLUSH_ENTRY:
	{
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)arpOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->arpEntry);
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Spruce_ARP_LOAD_ENTRY:
		/*    case Spruce_TCAM_PURGE_ENTRY: */
	{

		/* load Page 3 Block 8 */
		/* Wait until the arp in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_setArpExtensionReg(dev, (MSD_U8)8, 0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_arpSetPage3Data(dev, &opData->arpDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* load Page 2 Block 1*/
		tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Spruce_ARP_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->arpEntry);
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

	}
		break;

	case Spruce_ARP_GET_NEXT_ENTRY:
	{
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)arpOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->arpEntry);
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, &tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		if ((tmpdata & (MSD_U16)0xff) == (MSD_U16)0xff)
		{
			MSD_U16 data1;
			MSD_U16 data2;
			MSD_U16 data3;
			retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_P0_KEYS_1, &data1);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_P0_KEYS_2, &data2);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			retVal = msdGetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_P0_KEYS_3, &data3);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			if ((data1 == (MSD_U16)0x0) && (data2 == (MSD_U16)0x0) && (data3 == (MSD_U16)0x0))
			{
				/* No higher valid TCAM entry */
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return MSD_NO_SUCH;
			}
			else
			{
				/* The highest valid TCAM entry found*/
			}
		}

		/* Get next entry and read the entry */
		opData->arpEntry = (MSD_U32)tmpdata & (MSD_U32)0xff;

	}
	case Spruce_ARP_READ_ENTRY:
	{
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		arpOp = Spruce_ARP_READ_ENTRY;
		/* Read page 2 */
		tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)arpOp << 12) | (MSD_U16)((MSD_U16)3 << 10) | (MSD_U16)opData->arpEntry);
		retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, SPRUCE_TCAM_OPERATION, tmpdata);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/* Wait until the arp in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_setArpExtensionReg(dev, (MSD_U8)8, 0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Spruce_arpGetPage3Data(dev, &opData->arpDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the arp in ready. */
		retVal = Spruce_waitArpReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

	}
		break;

	default:
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return MSD_FAIL;
		break;
	}

	/*Change Block to 0, in case other misoperation*/
	retVal = Spruce_setArpExtensionReg(dev, 0, 0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Wait until the tcam in ready. */
	retVal = Spruce_waitArpReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}

static MSD_STATUS Spruce_setArpExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          tmpData = 0;

	tmpData = (MSD_U16)((MSD_U16)(((MSD_U16)Block & (MSD_U16)0xF) << 12) | (Port & (MSD_U16)0x1F));

	retVal = msdSetAnyReg(dev->devNum, SPRUCE_TCAM_DEV_ADDR, (MSD_U8)1, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_setArpExtensionReg returned: %s.\n", msdDisplayStatus(retVal)));
	}

	return retVal;
}