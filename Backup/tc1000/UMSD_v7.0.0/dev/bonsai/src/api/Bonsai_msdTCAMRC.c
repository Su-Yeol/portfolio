/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Bonsai_msdTCAMRC.c
*
* DESCRIPTION:
*       API definitions for TCAM range check
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsai/include/api/Bonsai_msdTCAMRC.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* Internal TCAM structure declaration.                                    */
/****************************************************************************/

/*
*  typedef: struct BONSAI_MSD_TCAM_RC_DATA_HW
*
*  Description: data required by Range Check TCAM RC Page - TCAM entry page 2 Block 0x01
*
*  Fields:
*
*/
typedef struct
{
	MSD_U16  frame[26];  /* RC action part */
} BONSAI_MSD_TCAM_RC_DATA_HW;

typedef struct
{
	MSD_U32    tcamEntry;
	BONSAI_MSD_TCAM_RC_DATA_HW    tcamDataP;
} BONSAI_MSD_TCAM_RC_OP_DATA;

typedef enum
{
	Bonsai_TCAMRC_FLUSH_ALL = 0x1,
	Bonsai_TCAMRC_FLUSH_ENTRY = 0x2,
	Bonsai_TCAMRC_LOAD_ENTRY = 0x3,
	Bonsai_TCAMRC_PURGE_ENTRY = 0x3,
	Bonsai_TCAMRC_GET_NEXT_ENTRY = 0x4,
	Bonsai_TCAMRC_READ_ENTRY = 0x5
} BONSAI_MSD_TCAM_RC_OPERATION;

/****************************************************************************/
/* TCAM operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Bonsai_setTcamRCHWData
(
IN	const BONSAI_MSD_TCAM_RC_DATA	*iData,
OUT	BONSAI_MSD_TCAM_RC_DATA_HW	*oData
);
static MSD_STATUS Bonsai_getTcamRCHWData
(
IN	const BONSAI_MSD_TCAM_RC_DATA_HW	*iData,
OUT	BONSAI_MSD_TCAM_RC_DATA	*oData
);
static MSD_STATUS Bonsai_tcamSetPage2RCData(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamSetPage2RCActionData(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage2RCActionData(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_tcamGetPage2RCData(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr);
static MSD_STATUS Bonsai_waitTcamReady(const MSD_QD_DEV *dev);
static MSD_STATUS Bonsai_tcamRCOperationPerform
(
IN    MSD_QD_DEV           *dev,
IN    BONSAI_MSD_TCAM_RC_OPERATION   tcamrcOp,
INOUT BONSAI_MSD_TCAM_RC_OP_DATA     *opData
);
static void displayTcamRCData(BONSAI_MSD_TCAM_RC_DATA *tcamData);
static MSD_STATUS Bonsai_setTcamExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port);

/*******************************************************************************
* Bonsai_gtcamRCFlushAllEntry
*
* DESCRIPTION:
*       This routine is to flush all TCAM Range Check entries.Based on the function
*		of Entry Delete, set Page 2 Block 7 Reg 1 to 0x1F.
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
MSD_STATUS Bonsai_gtcamRCFlushAllEntry
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS           retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCFlushAllEntry Called.\n"));

	/* Program Tuning register */
	op = Bonsai_TCAMRC_FLUSH_ALL;
	retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Bonsai_tcamRCOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCFlushAllEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamRCFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry. 
*
* INPUTS:
*       rcEntryNum  - pointer to the desired entry of RC table
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
MSD_STATUS Bonsai_gtcamRCFlushEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        rcEntryNum
)
{
	MSD_STATUS           retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCFlushEntry Called.\n"));

	/* check if the given pointer is valid */
	if (rcEntryNum >= BONSAI_MAX_IGR_TCAM_ENTRY)
	{
		MSD_DBG_ERROR(("Bad rcEntryNum %u. It should be within [0, %u].\n", rcEntryNum, BONSAI_MAX_IGR_TCAM_ENTRY - 1U));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAMRC_FLUSH_ENTRY;
		tcamrcOpData.tcamEntry = rcEntryNum;
		retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_tcamRCOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCFlushEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamRCLoadEntry
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
MSD_STATUS Bonsai_gtcamRCLoadEntry
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32		rcEntryNum,
IN  BONSAI_MSD_TCAM_RC_DATA *rcData
)
{
	MSD_STATUS           retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCLoadEntry Called.\n"));

	/* check if the given pointer is valid */
	if ((rcEntryNum >= BONSAI_MAX_IGR_TCAM_ENTRY) || (rcData == NULL))
	{
		MSD_DBG_ERROR(("Bad rcEntryNum %u out of range OR rcData is NULL.\n", rcEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAMRC_LOAD_ENTRY;
		tcamrcOpData.tcamEntry = rcEntryNum;
		/*tcamrcOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_setTcamRCHWData(rcData, &tcamrcOpData.tcamDataP);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_setTcamRCHWData returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("tcamrcOperationPerform LOAD_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCLoadEntry Exit.\n"));
	return retVal;
}


/*******************************************************************************
* Bonsai_gtcamRCReadEntry
*
* DESCRIPTION:
*       This routine reads the global 2 Block 1&7 offsets 0x02 to 0x1B registers with
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       rcEntryNum - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       rcData     - Tcam Range Check entry Data
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
MSD_STATUS Bonsai_gtcamRCReadEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		rcEntryNum,
IN  BONSAI_MSD_TCAM_RC_DATA *rcData
)
{
	MSD_STATUS           retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCReadEntry Called.\n"));


	/* check if the given pointer is valid */
	if ((rcEntryNum >= BONSAI_MAX_IGR_TCAM_ENTRY) || (rcData == NULL))
	{
		MSD_DBG_ERROR(("Bad rcEntryNum %u out of range OR tcamData is NULL.\n", rcEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAMRC_READ_ENTRY;
		tcamrcOpData.tcamEntry = rcEntryNum;
		/*tcamrcOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("tcamrcOperationPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = Bonsai_getTcamRCHWData(&tcamrcOpData.tcamDataP, rcData);
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCReadEntry Exit.\n"));
	return retVal;

}

/*******************************************************************************
* Bonsai_gtcamRCGetNextEntry
*
* DESCRIPTION:
*       This routine finds the next higher TCAM RC Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry
*       register (bits 6:0) is used as the TCAM entry to start from. To find
*       the lowest number TCAM Entry that is valid, start the Get Next operation
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        rcEntryNum - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        rcEntryNum - next pointer entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
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
MSD_STATUS Bonsai_gtcamRCGetNextEntry
(
IN		MSD_QD_DEV     *dev,
INOUT   MSD_U32		*rcEntryNum,
IN		BONSAI_MSD_TCAM_RC_DATA *rcData
)
{
	MSD_STATUS           retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCGetNextEntry Called.\n"));

	/* check if the given pointer is valid */
	if (rcEntryNum == NULL)
	{
		MSD_DBG_ERROR(("rcEntryNum is NULL.\n"));
		retVal = MSD_BAD_PARAM;
	}
	else if((*rcEntryNum >= BONSAI_MAX_IGR_TCAM_ENTRY) || (rcData == NULL))
	{
		MSD_DBG_ERROR(("Bad rcEntryNum value %u out of range OR tcamData is NULL.\n", *rcEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Program Tuning register */
		op = Bonsai_TCAMRC_GET_NEXT_ENTRY;
		tcamrcOpData.tcamEntry = *rcEntryNum;
		/*tcamrcOpData.tcamDataP = tcamData;*/
		retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
		if (retVal == MSD_NO_SUCH)
		{
			MSD_DBG_INFO(("No higher valid entry founded return MSD_NO_SUCH"));
		}
		else if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_tcamRCOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
			retVal = MSD_FAIL;
		}
		else
		{
			*rcEntryNum = tcamrcOpData.tcamEntry;
			retVal = Bonsai_getTcamRCHWData(&tcamrcOpData.tcamDataP, rcData);
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCGetNextEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamRCFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       rcEntryNum - the tcam entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       tcamData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Bonsai_gtcamRCFindEntry
(
IN	MSD_QD_DEV     *dev,
IN  MSD_U32		rcEntryNum,
IN  BONSAI_MSD_TCAM_RC_DATA *rcData,
OUT	MSD_BOOL		  *found
)
{
	MSD_STATUS       retVal;
	BONSAI_MSD_TCAM_RC_OPERATION    op;
	BONSAI_MSD_TCAM_RC_OP_DATA     tcamrcOpData;

	MSD_DBG_INFO(("Bonsai_gtcamRCFindEntry Called.\n"));

	/* check if the given pointer is valid */
	if ((rcEntryNum >= BONSAI_MAX_IGR_TCAM_ENTRY) || (rcData == NULL) || (found == NULL))
	{
		MSD_DBG_ERROR(("Bad rcEntryNum %u out of range OR rcData is NULL or found is NULL.\n", rcEntryNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		*found = MSD_FALSE;

		/* Program Tuning register */
		op = Bonsai_TCAMRC_GET_NEXT_ENTRY;
		tcamrcOpData.tcamEntry = rcEntryNum == (MSD_U32)0 ? (MSD_U32)0xff : (rcEntryNum - (MSD_U32)1);
		retVal = Bonsai_tcamRCOperationPerform(dev, op, &tcamrcOpData);
		if (retVal == MSD_NO_SUCH)
		{
			MSD_DBG_INFO(("Not found this entry, Bonsai_gtcamFindRCEntry Exit.\n"));
			retVal = MSD_OK;
		}
		else
		{
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_tcamRCOperationPerform GET_NEXT returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				if (tcamrcOpData.tcamEntry != rcEntryNum)
				{
					MSD_DBG_INFO(("Not found this entry, Bonsai_gtcamFindRCEntry Exit.\n"));
					retVal = MSD_OK;
				}
				else
				{
					*found = MSD_TRUE;
					retVal = Bonsai_getTcamRCHWData(&tcamrcOpData.tcamDataP, rcData);
				}
			}
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamRCFindEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Bonsai_gtcamRCEntryDump
*
* DESCRIPTION:
*       Finds all valid TCAM entries and print it out.
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
MSD_STATUS Bonsai_gtcamRCEntryDump
(
IN  MSD_QD_DEV *dev
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32    tcamPointer;
	BONSAI_MSD_TCAM_RC_DATA  tcamData;

	MSD_DBG_INFO(("Bonsai_gtcamDump Called.\n"));
	tcamPointer = (MSD_U32)0xff;

	while (status == MSD_OK)
	{
		status = Bonsai_gtcamRCGetNextEntry(dev, &tcamPointer, &tcamData);
		MSG(("find the next valid TCAM entry number is %u\n", tcamPointer));
		displayTcamRCData(&tcamData);
		if ((tcamPointer & (MSD_U32)0x00ff) == (MSD_U32)0xff)
		{
			break;
		}
	}

	MSD_DBG_INFO(("Bonsai_gtcamDump Exit.\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal functions.                                                  */
/****************************************************************************/
static MSD_STATUS Bonsai_setTcamRCHWData
(
IN	const BONSAI_MSD_TCAM_RC_DATA	*iData,
OUT	BONSAI_MSD_TCAM_RC_DATA_HW	*oData
)
{
	msdMemSet(oData, 0, sizeof(BONSAI_MSD_TCAM_RC_DATA_HW));

	/*key -> RC compare vector, rcIndex*/
	MSD_BF_SET(oData->frame[17], iData->rcIndex, 0, 8);
	MSD_BF_SET(oData->frame[16], iData->rcResult, 0, 8);
	MSD_BF_SET(oData->frame[16], iData->rcResultMask, 8, 8);

	/*Action*/
	MSD_BF_SET(oData->frame[0], iData->continu, 13, 3);
	MSD_BF_SET(oData->frame[0], (MSD_U16)(iData->vidOverride), 12, 1);
	MSD_BF_SET(oData->frame[0], iData->vidData, 0, 12);

	MSD_BF_SET(oData->frame[1], iData->nextId, 8, 8);
	MSD_BF_SET(oData->frame[1], (MSD_U16)(iData->qpriOverride), 7, 1);
	MSD_BF_SET(oData->frame[1], iData->qpriData, 4, 3);
	MSD_BF_SET(oData->frame[1], (MSD_U16)(iData->fpriOverride), 3, 1);
	MSD_BF_SET(oData->frame[1], iData->fpriData, 0, 3);

	MSD_BF_SET(oData->frame[2], iData->dpvSF, 11, 1);
	MSD_BF_SET(oData->frame[2], (MSD_U16)(iData->dpvData), 0, 7);

	MSD_BF_SET(oData->frame[4], iData->dpvMode, 14, 2);
	MSD_BF_SET(oData->frame[4], iData->colorMode, 12, 2);
	MSD_BF_SET(oData->frame[4], (MSD_U16)(iData->vtuPageOverride), 11, 1);
	MSD_BF_SET(oData->frame[4], iData->vtuPage, 10, 1);
	MSD_BF_SET(oData->frame[4], iData->unKnownFilter, 8, 2);
	MSD_BF_SET(oData->frame[4], iData->egActPoint, 0, 6);

	MSD_BF_SET(oData->frame[5], (MSD_U16)(iData->ldBalanceOverride), 15, 1);
	MSD_BF_SET(oData->frame[5], iData->ldBalanceData, 12, 3);
	MSD_BF_SET(oData->frame[5], iData->tcamTunnel, 10, 1);
	MSD_BF_SET(oData->frame[5], iData->ipMulticast, 9, 1);
	MSD_BF_SET(oData->frame[5], iData->ip2me, 8, 1);
	MSD_BF_SET(oData->frame[5], iData->routeEntry, 7, 1);
	MSD_BF_SET(oData->frame[5], (MSD_U16)(iData->DSCPOverride), 6, 1);
	MSD_BF_SET(oData->frame[5], iData->DSCP, 0, 6);

	MSD_BF_SET(oData->frame[6], (MSD_U16)(iData->factionOverride), 15, 1);
	MSD_BF_SET(oData->frame[6], iData->factionData, 0, 15);

	MSD_BF_SET(oData->frame[10], iData->flowMeterEn, 7, 1);
	MSD_BF_SET(oData->frame[10], iData->flowMeterId, 0, 4);
	MSD_BF_SET(oData->frame[11], iData->streamFilterEn, 7, 1);
	MSD_BF_SET(oData->frame[11], iData->streamFilterId, 0, 4);

	MSD_BF_SET(oData->frame[25], iData->interrupt, 15, 1);
	MSD_BF_SET(oData->frame[25], iData->IncTcamCtr, 14, 1);
	MSD_BF_SET(oData->frame[25], iData->tcamCtr, 12, 2);

	return MSD_OK;
}

static MSD_STATUS Bonsai_getTcamRCHWData
(
IN	const BONSAI_MSD_TCAM_RC_DATA_HW	*iData,
OUT	BONSAI_MSD_TCAM_RC_DATA	*oData
)
{
	/*key*/
	oData->rcIndex = (MSD_U8)MSD_BF_GET(iData->frame[17], 0, 8);
	oData->rcResult = (MSD_U8)MSD_BF_GET(iData->frame[16], 0, 8);
	oData->rcResultMask = (MSD_U8)MSD_BF_GET(iData->frame[16], 8, 8);

	/*Action*/
	oData->continu = (MSD_U8)(MSD_BF_GET(iData->frame[0], 13, 3));
	oData->vidOverride = MSD_BF_GET(iData->frame[0], 12, 1);
	oData->vidData = MSD_BF_GET(iData->frame[0], 0, 12);

	oData->nextId = (MSD_U8)(MSD_BF_GET(iData->frame[1], 8, 8));
	oData->qpriOverride = MSD_BF_GET(iData->frame[1], 7, 1);
	oData->qpriData = (MSD_U8)(MSD_BF_GET(iData->frame[1], 4, 3));
	oData->fpriOverride = MSD_BF_GET(iData->frame[1], 3, 1);
	oData->fpriData = (MSD_U8)(MSD_BF_GET(iData->frame[1], 0, 3));

	oData->dpvSF = (MSD_U8)(MSD_BF_GET(iData->frame[2], 11, 1));
	oData->dpvData = MSD_BF_GET(iData->frame[2], 0, 7);

	oData->dpvMode = (MSD_U8)(MSD_BF_GET(iData->frame[4], 14, 2));
	oData->colorMode = (MSD_U8)(MSD_BF_GET(iData->frame[4], 12, 2));
	oData->vtuPageOverride = MSD_BF_GET(iData->frame[4], 11, 1);
	oData->vtuPage = (MSD_U8)(MSD_BF_GET(iData->frame[4], 10, 1));
	oData->unKnownFilter = (MSD_U8)(MSD_BF_GET(iData->frame[4], 8, 2));
	oData->egActPoint = (MSD_U8)(MSD_BF_GET(iData->frame[4], 0, 6));

	oData->ldBalanceOverride = MSD_BF_GET(iData->frame[5], 15, 1);
	oData->ldBalanceData = (MSD_U8)(MSD_BF_GET(iData->frame[5], 12, 3));
	oData->tcamTunnel = MSD_BF_GET(iData->frame[5], 10, 1);
	oData->ipMulticast = MSD_BF_GET(iData->frame[5], 9, 1);
	oData->ip2me = MSD_BF_GET(iData->frame[5], 8, 1);
	oData->routeEntry = MSD_BF_GET(iData->frame[5], 7, 1);
	oData->DSCPOverride = MSD_BF_GET(iData->frame[5], 6, 1);
	oData->DSCP = (MSD_U8)(MSD_BF_GET(iData->frame[5], 0, 6));

	oData->factionOverride = MSD_BF_GET(iData->frame[6], 15, 1);
	oData->factionData = MSD_BF_GET(iData->frame[6], 0, 15);

	oData->flowMeterEn = MSD_BF_GET(iData->frame[10], 7, 1);
	oData->flowMeterId = MSD_BF_GET(iData->frame[10], 0, 4);
	oData->streamFilterEn = MSD_BF_GET(iData->frame[11], 7, 1);
	oData->streamFilterId = MSD_BF_GET(iData->frame[11], 0, 4);

	oData->interrupt = (MSD_U8)(MSD_BF_GET(iData->frame[25], 15, 1));
	oData->IncTcamCtr = (MSD_U8)(MSD_BF_GET(iData->frame[25], 14, 1));
	oData->tcamCtr = (MSD_U8)(MSD_BF_GET(iData->frame[25], 12, 2));

	return MSD_OK;
}


static MSD_STATUS Bonsai_tcamSetPage2RCData(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 4; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->frame[i + 14]);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamSetPage2RCActionData(const MSD_QD_DEV *dev, const BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 14; i++)
	{
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, tcamDataPtr->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}

	retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)0x1B, tcamDataPtr->frame[25]);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	return MSD_OK;
}

static MSD_STATUS Bonsai_tcamGetPage2RCData(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 4; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->frame[i + 14]);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_tcamGetPage2RCActionData(const MSD_QD_DEV *dev, BONSAI_MSD_TCAM_RC_DATA_HW *tcamDataPtr)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_16 i;

	for (i = 2; i < 14; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)i, &tcamDataPtr->frame[i - 2]);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}
	retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)0x1B, &tcamDataPtr->frame[25]);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	return MSD_OK;
}
static MSD_STATUS Bonsai_waitTcamReady(const MSD_QD_DEV *dev)
{
	MSD_STATUS       retVal;    /* Functions return value */

	MSD_U16          tmpdata;     /* temporary Data storage */

	tmpdata = 1U;
	while (tmpdata == 1U)
	{
		retVal = msdGetAnyRegField(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}
	return MSD_OK;
}

/*******************************************************************************
* Bonsai_tcamrcOperationPerform
*
* DESCRIPTION:
*       This function accesses TCAM Table
*
* INPUTS:
*       tcamrcOp   - The tcam operation
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
static MSD_STATUS Bonsai_tcamRCOperationPerform
(
IN    MSD_QD_DEV           *dev,
IN    BONSAI_MSD_TCAM_RC_OPERATION   tcamrcOp,
INOUT BONSAI_MSD_TCAM_RC_OP_DATA     *opData
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	/* Wait until the tcam in ready. */
	retVal = Bonsai_waitTcamReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/*Change Block to 7, indicate Range Check Page */
	retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)7, (MSD_U8)0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamrcOp)
	{
	case Bonsai_TCAMRC_FLUSH_ALL:
	{
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/*Set Port to 0x1F, Flush all Range Check entries */
		retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)7, (MSD_U8)0x1f);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		tcamrcOp = Bonsai_TCAMRC_FLUSH_ENTRY;
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamrcOp << 12) | (MSD_U16)((MSD_U16)2 << 10));
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Bonsai_TCAMRC_FLUSH_ENTRY:
	{
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamrcOp << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
	}
		break;

	case Bonsai_TCAMRC_LOAD_ENTRY:
		/*    case Bonsai_TCAM_PURGE_ENTRY: */
	{

		/* load Page 2 Block 7 */
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/*Access Range Check from TCAM Range Frame */
		retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)7, (MSD_U8)0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Bonsai_tcamSetPage2RCData(dev, &opData->tcamDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* load Page 2 Block 1*/
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Bonsai_TCAMRC_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/*Access Range Check Ingress Actions from TCAM Range Frame matches */
		retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)1, (MSD_U8)0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Bonsai_tcamSetPage2RCActionData(dev, &opData->tcamDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Bonsai_TCAMRC_LOAD_ENTRY << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

	}
		break;

	case Bonsai_TCAMRC_GET_NEXT_ENTRY:
	{
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamrcOp << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		if ((data & (MSD_U16)0xff) == (MSD_U16)0xff)
		{
			MSD_U16 data1;
			retVal = msdGetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_P0_KEYS_1, &data1);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->tblRegsSem);
				return retVal;
			}
			if (data1 == (MSD_U16)0x00ff)
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
		opData->tcamEntry = (MSD_U32)data & (MSD_U32)0xff;

	}
	case Bonsai_TCAMRC_READ_ENTRY:
	{
		tcamrcOp = Bonsai_TCAMRC_READ_ENTRY;

		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Read page 2 */
		/*Access Range Check Ingress Actions from TCAM Range Frame matches */
		retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)7, (MSD_U8)0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamrcOp << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		

		retVal = Bonsai_tcamGetPage2RCData(dev, &opData->tcamDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/*Access Range Check Ingress Actions from TCAM Range Frame matches */
		retVal = Bonsai_setTcamExtensionReg(dev, (MSD_U8)1, (MSD_U8)0);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)tcamrcOp << 12) | (MSD_U16)((MSD_U16)2 << 10) | (MSD_U16)opData->tcamEntry);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, BONSAI_TCAM_OPERATION, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		retVal = Bonsai_tcamGetPage2RCActionData(dev, &opData->tcamDataP);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Bonsai_waitTcamReady(dev);
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
	retVal = Bonsai_setTcamExtensionReg(dev, 0, 0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Wait until the tcam in ready. */
	retVal = Bonsai_waitTcamReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}

static void displayTcamRCData(BONSAI_MSD_TCAM_RC_DATA *tcamData)
{

	MSG(("----- Key & Mask -----\n"));

	MSG(("rcIndex           : %04x\n", tcamData->rcIndex));
	MSG(("rcResult          : %04x\n", tcamData->rcResult));
	MSG(("rcResultMask      : %04x\n", tcamData->rcResultMask));

	MSG(("----- Ingress Action -----\n"));

	MSG(("continu           : %04x\n", tcamData->continu));
	MSG(("interrupt         : %04x\n", tcamData->interrupt));
	MSG(("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr));
	MSG(("vidOverride       : %04x\n", tcamData->vidOverride));
	MSG(("vidData           : %04x\n", tcamData->vidData));

	MSG(("nextId            : %04x\n", tcamData->nextId));
	MSG(("qpriOverride      : %04x\n", tcamData->qpriOverride));
	MSG(("qpriData          : %04x\n", tcamData->qpriData));
	MSG(("fpriOverride      : %04x\n", tcamData->fpriOverride));
	MSG(("fpriData          : %04x\n", tcamData->fpriData));

	MSG(("dpvSF             : %04x\n", tcamData->dpvSF));
	MSG(("dpvData           : %04x\n", (MSD_U32)tcamData->dpvData));
	MSG(("dpvMode           : %04x\n", tcamData->dpvMode));

	MSG(("unKnownFilter     : %04x\n", tcamData->unKnownFilter));

	MSG(("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride));
	MSG(("ldBalanceData     : %04x\n", tcamData->ldBalanceData));

	MSG(("factionOverride   : %04x\n", tcamData->factionOverride));
	MSG(("factionData       : %04x\n", tcamData->factionData));
	MSG(("colorMode         : %04x\n", tcamData->colorMode));
	MSG(("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride));
	MSG(("vtuPage           : %04x\n", tcamData->vtuPage));
	MSG(("egActPoint        : %04x\n", tcamData->egActPoint));
	MSG(("DSCPOverride      : %04x\n", tcamData->DSCPOverride));
	MSG(("DSCP              : %04x\n", tcamData->DSCP));
	MSG(("ipMulticast       : %04x\n", tcamData->ipMulticast));
	MSG(("ip2me             : %04x\n", tcamData->ip2me));
	MSG(("routeEntry        : %04x\n", tcamData->routeEntry));
	MSG(("FlowMeterEn       : %04x\n", tcamData->flowMeterEn));
	MSG(("FlowMeterId       : %04x\n", tcamData->flowMeterId));
	MSG(("streamFilterEn    : %04x\n", tcamData->streamFilterEn));
	MSG(("streamFilterId    : %04x\n", tcamData->streamFilterId));
	MSG(("tcamCtr           : %04x\n", tcamData->tcamCtr));
}


static MSD_STATUS Bonsai_setTcamExtensionReg(const MSD_QD_DEV *dev, MSD_U8 Block, MSD_U8 Port)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          tmpData = 0;

	tmpData = (MSD_U16)((MSD_U16)((MSD_U16)Block & (MSD_U16)0xF) << 12) | ((MSD_U16)Port & (MSD_U16)0x1F);

	retVal = msdSetAnyReg(dev->devNum, BONSAI_TCAM_DEV_ADDR, (MSD_U8)1, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Bonsai_setTcamExtensionReg returned: %s.\n", msdDisplayStatus(retVal)));
	}

	return retVal;
}