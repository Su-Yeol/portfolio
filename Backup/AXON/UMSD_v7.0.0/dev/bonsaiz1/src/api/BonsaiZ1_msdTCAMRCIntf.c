/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* BonsaiZ1_msdTCAMRCIntf.c
*
* DESCRIPTION:
*       API definitions for TCAM range check
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsaiz1/include/api/BonsaiZ1_msdTCAMRC.h>

/*******************************************************************************
* BonsaiZ1_gtcamRCLoadEntryIntf
*
* DESCRIPTION:
*       This routine loads a TCAM Range Check entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       2 blocks of data. All 2 blocks need to loaded in a particular order for the TCAM RC
*       to operate correctly while frames are flowing through the switch.
*       If the entry is currently valid, it must first be flushed. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set
*       accordingly.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*        rcData    - Tcam entry Data
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
MSD_STATUS BonsaiZ1_gtcamRCLoadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			rcEntryNum,
IN  MSD_TCAM_RC_DATA *rcData
)
{
	BONSAIZ1_MSD_TCAM_RC_DATA tcamrc;

	if (NULL == rcData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_DATA in BonsaiZ1_gtcamRCLoadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcamrc.rcIndex = (MSD_U8)rcData->rcIndex;
	tcamrc.rcResult = rcData->rcResult;
	tcamrc.rcResultMask = rcData->rcResultMask;

	tcamrc.continu = rcData->continu;
	tcamrc.interrupt = rcData->interrupt;
	tcamrc.IncTcamCtr = rcData->IncTcamCtr;
	tcamrc.vidOverride = rcData->vidOverride;
	tcamrc.vidData = rcData->vidData;
	tcamrc.nextId = (MSD_U8)rcData->nextId;
	tcamrc.qpriOverride = rcData->qpriOverride;
	tcamrc.qpriData = rcData->qpriData;
	tcamrc.fpriOverride = rcData->fpriOverride;
	tcamrc.fpriData = rcData->fpriData;
	tcamrc.dpvSF = rcData->dpvSF;
	tcamrc.dpvData = rcData->dpvData;
	tcamrc.dpvMode = rcData->dpvMode;
	tcamrc.colorMode = rcData->colorMode;
	tcamrc.vtuPageOverride = rcData->vtuPageOverride;
	tcamrc.vtuPage = rcData->vtuPage;
	tcamrc.unKnownFilter = rcData->unKnownFilter;
	tcamrc.egActPoint = rcData->egActPoint;
	tcamrc.ldBalanceOverride = rcData->ldBalanceOverride;
	tcamrc.ldBalanceData = rcData->ldBalanceData;
	tcamrc.DSCPOverride = rcData->DSCPOverride;
	tcamrc.DSCP = rcData->DSCP;
	tcamrc.factionOverride = rcData->factionOverride;
	tcamrc.factionData = rcData->factionData;
	tcamrc.tcamTunnel = MSD_FALSE; /* reserved */
	tcamrc.ipMulticast = rcData->ipMulticast;
	tcamrc.ip2me = rcData->ip2me;
	tcamrc.routeEntry = rcData->routeEntry;
	tcamrc.tcamCtr = rcData->tcamCtr;

	return BonsaiZ1_gtcamRCLoadEntry(dev, rcEntryNum, &tcamrc);
}

/*******************************************************************************
* BonsaiZ1_gtcamRCReadEntryIntf
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
MSD_STATUS BonsaiZ1_gtcamRCReadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        rcEntryNum,
OUT MSD_TCAM_RC_DATA   *rcData
)
{
	MSD_STATUS           retVal;
	BONSAIZ1_MSD_TCAM_RC_DATA tcamrc;

	retVal = BonsaiZ1_gtcamRCReadEntry(dev, rcEntryNum, &tcamrc);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == rcData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_DATA in BonsaiZ1_gtcamRCReadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)rcData, 0, sizeof(MSD_TCAM_RC_DATA));

	rcData->rcIndex = tcamrc.rcIndex;
	rcData->rcResult = tcamrc.rcResult;
	rcData->rcResultMask = tcamrc.rcResultMask;

	rcData->continu = tcamrc.continu;
	rcData->interrupt = tcamrc.interrupt;
	rcData->IncTcamCtr = tcamrc.IncTcamCtr;
	rcData->vidOverride = tcamrc.vidOverride;
	rcData->vidData = tcamrc.vidData;
	rcData->nextId = tcamrc.nextId;
	rcData->qpriOverride = tcamrc.qpriOverride;
	rcData->qpriData = tcamrc.qpriData;
	rcData->fpriOverride = tcamrc.fpriOverride;
	rcData->fpriData = tcamrc.fpriData;
	rcData->dpvSF = tcamrc.dpvSF;
	rcData->dpvData = tcamrc.dpvData;
	rcData->dpvMode = tcamrc.dpvMode;
	rcData->colorMode = tcamrc.colorMode;
	rcData->vtuPageOverride = tcamrc.vtuPageOverride;
	rcData->vtuPage = tcamrc.vtuPage;
	rcData->unKnownFilter = tcamrc.unKnownFilter;
	rcData->egActPoint = tcamrc.egActPoint;
	rcData->ldBalanceOverride = tcamrc.ldBalanceOverride;
	rcData->ldBalanceData = tcamrc.ldBalanceData;
	rcData->DSCPOverride = tcamrc.DSCPOverride;
	rcData->DSCP = tcamrc.DSCP;
	rcData->factionOverride = tcamrc.factionOverride;
	rcData->factionData = tcamrc.factionData;
	rcData->tcamTunnel = MSD_FALSE; /* reserved */
	rcData->ipMulticast = tcamrc.ipMulticast;
	rcData->ip2me = tcamrc.ip2me;
	rcData->routeEntry = tcamrc.routeEntry;
	rcData->tcamCtr = tcamrc.tcamCtr;

	return retVal;
}
/*******************************************************************************
* BonsaiZ1_gtcamRCGetNextEntryIntf
*
* DESCRIPTION:
*       This routine finds the next higher TCAM RC Entry number that is valid (i.e.,
*       any entry whose Page 7 offset 0x02 is not equal to 0x00FF). The TCAM Entry
*       register (bits 6:0) is used as the TCAM entry to start from. To find
*       the lowest number TCAM Entry that is valid, start the Get Next operation
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        rcEntryNum - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        rcEntryNum - next pointer entry of TCAM (0 ~ 255)
*        rcData    - Tcam entry Data
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
MSD_STATUS BonsaiZ1_gtcamRCGetNextEntryIntf
(
IN     MSD_QD_DEV     *dev,
INOUT  MSD_U32        *rcEntryNum,
OUT    MSD_TCAM_RC_DATA  *rcData
)
{
	MSD_STATUS           retVal;
	BONSAIZ1_MSD_TCAM_RC_DATA tcamrc;

	retVal = BonsaiZ1_gtcamRCGetNextEntry(dev, rcEntryNum, &tcamrc);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == rcData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_DATA in BonsaiZ1_gtcamRCGetNextEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)rcData, 0, sizeof(MSD_TCAM_RC_DATA));

	rcData->rcIndex = tcamrc.rcIndex;
	rcData->rcResult = tcamrc.rcResult;
	rcData->rcResultMask = tcamrc.rcResultMask;

	rcData->continu = tcamrc.continu;
	rcData->interrupt = tcamrc.interrupt;
	rcData->IncTcamCtr = tcamrc.IncTcamCtr;
	rcData->vidOverride = tcamrc.vidOverride;
	rcData->vidData = tcamrc.vidData;
	rcData->nextId = tcamrc.nextId;
	rcData->qpriOverride = tcamrc.qpriOverride;
	rcData->qpriData = tcamrc.qpriData;
	rcData->fpriOverride = tcamrc.fpriOverride;
	rcData->fpriData = tcamrc.fpriData;
	rcData->dpvSF = tcamrc.dpvSF;
	rcData->dpvData = tcamrc.dpvData;
	rcData->dpvMode = tcamrc.dpvMode;
	rcData->colorMode = tcamrc.colorMode;
	rcData->vtuPageOverride = tcamrc.vtuPageOverride;
	rcData->vtuPage = tcamrc.vtuPage;
	rcData->unKnownFilter = tcamrc.unKnownFilter;
	rcData->egActPoint = tcamrc.egActPoint;
	rcData->ldBalanceOverride = tcamrc.ldBalanceOverride;
	rcData->ldBalanceData = tcamrc.ldBalanceData;
	rcData->DSCPOverride = tcamrc.DSCPOverride;
	rcData->DSCP = tcamrc.DSCP;
	rcData->factionOverride = tcamrc.factionOverride;
	rcData->factionData = tcamrc.factionData;
	rcData->tcamTunnel = MSD_FALSE; /* reserved */
	rcData->ipMulticast = tcamrc.ipMulticast;
	rcData->ip2me = tcamrc.ip2me;
	rcData->routeEntry = tcamrc.routeEntry;
	rcData->tcamCtr = tcamrc.tcamCtr;

	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gtcamRCFindEntryIntf
*
* DESCRIPTION:
*       Find the specified valid tcam Range Check entry in ingress TCAM Range Check Table.
*
* INPUTS:
*       rcEntryNum - the tcam range check entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       rcData - the entry parameters.
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
MSD_STATUS BonsaiZ1_gtcamRCFindEntryIntf
(
IN	MSD_QD_DEV     *dev,
IN	MSD_U32        rcEntryNum,
OUT	MSD_TCAM_RC_DATA  *rcData,
OUT	MSD_BOOL		  *found
)
{
	MSD_STATUS           retVal;
	BONSAIZ1_MSD_TCAM_RC_DATA tcamrc;

	msdMemSet((void*)&tcamrc, 0, sizeof(BONSAIZ1_MSD_TCAM_RC_DATA));

	retVal = BonsaiZ1_gtcamRCFindEntry(dev, rcEntryNum, &tcamrc, found);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == rcData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_DATA in BonsaiZ1_gtcamRCFindEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)rcData, 0, sizeof(MSD_TCAM_RC_DATA));

	if (*found == MSD_FALSE)
	{
		return retVal;
	}

	rcData->rcIndex = tcamrc.rcIndex;
	rcData->rcResult = tcamrc.rcResult;
	rcData->rcResultMask = tcamrc.rcResultMask;

	rcData->continu = tcamrc.continu;
	rcData->interrupt = tcamrc.interrupt;
	rcData->IncTcamCtr = tcamrc.IncTcamCtr;
	rcData->vidOverride = tcamrc.vidOverride;
	rcData->vidData = tcamrc.vidData;
	rcData->nextId = tcamrc.nextId;
	rcData->qpriOverride = tcamrc.qpriOverride;
	rcData->qpriData = tcamrc.qpriData;
	rcData->fpriOverride = tcamrc.fpriOverride;
	rcData->fpriData = tcamrc.fpriData;
	rcData->dpvSF = tcamrc.dpvSF;
	rcData->dpvData = tcamrc.dpvData;
	rcData->dpvMode = tcamrc.dpvMode;
	rcData->colorMode = tcamrc.colorMode;
	rcData->vtuPageOverride = tcamrc.vtuPageOverride;
	rcData->vtuPage = tcamrc.vtuPage;
	rcData->unKnownFilter = tcamrc.unKnownFilter;
	rcData->egActPoint = tcamrc.egActPoint;
	rcData->ldBalanceOverride = tcamrc.ldBalanceOverride;
	rcData->ldBalanceData = tcamrc.ldBalanceData;
	rcData->DSCPOverride = tcamrc.DSCPOverride;
	rcData->DSCP = tcamrc.DSCP;
	rcData->factionOverride = tcamrc.factionOverride;
	rcData->factionData = tcamrc.factionData;
	rcData->tcamTunnel = MSD_FALSE; /* reserved */
	rcData->ipMulticast = tcamrc.ipMulticast;
	rcData->ip2me = tcamrc.ip2me;
	rcData->routeEntry = tcamrc.routeEntry;
	rcData->tcamCtr = tcamrc.tcamCtr;

	return retVal;
}