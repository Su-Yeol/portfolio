/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Spruce_msdTCAMIntf.c
*
* DESCRIPTION:
*       API definitions for control of Ternary Content Addressable Memory
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdTCAM.h>

/*******************************************************************************
* Spruce_gtcamLoadEntry
*
* DESCRIPTION:
*       This routine loads a TCAM entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       3 pages of data. All 3 pages need to loaded in a particular order for the TCAM 
*       to operate correctly while frames are flowing through the switch. 
*       If the entry is currently valid, it must first be flushed. Then page 2 needs 
*       to be loaded first, followed by page 1 and then finally page 0. 
*       Each page load requires its own write TCAMOp with these TCAM page bits set 
*       accordingly. 
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
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
MSD_STATUS Spruce_gtcamLoadEntryIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    IN  MSD_TCAM_DATA        *tcamData
)
{
	SPRUCE_MSD_TCAM_DATA tcam;
	MSD_U8 i = 0;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Spruce_gtcamLoadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcam.frameType = tcamData->frameType;
	tcam.frameTypeMask = tcamData->frameTypeMask;
	tcam.timeKey = tcamData->timeKey;
	tcam.timeKeyMask = tcamData->timeKeyMask;
	tcam.spv = tcamData->spv;
	tcam.spvMask = tcamData->spvMask;
	tcam.ppri = tcamData->ppri;
	tcam.ppriMask = tcamData->ppriMask;
	tcam.pvid = tcamData->pvid;
	tcam.pvidMask = tcamData->pvidMask;
	tcam.continu = tcamData->continu;
	tcam.interrupt = tcamData->interrupt;
	tcam.IncTcamCtr = tcamData->IncTcamCtr;
	tcam.vidOverride = tcamData->vidOverride;
	tcam.vidData = tcamData->vidData;
	tcam.nextId = (MSD_U8)tcamData->nextId;
	tcam.qpriOverride = tcamData->qpriOverride;
	tcam.qpriData = tcamData->qpriData;
	tcam.fpriOverride = tcamData->fpriOverride;
	tcam.fpriData = tcamData->fpriData;
	tcam.dpvSF = tcamData->dpvSF;
	tcam.dpvData = tcamData->dpvData;
	tcam.dpvMode = tcamData->dpvMode;
	tcam.colorMode = tcamData->colorMode;
	tcam.vtuPageOverride = tcamData->vtuPageOverride;
	tcam.vtuPage = tcamData->vtuPage;
	tcam.unKnownFilter = tcamData->unKnownFilter;
	tcam.egActPoint = tcamData->egActPoint;
	tcam.ldBalanceOverride = tcamData->ldBalanceOverride;
	tcam.ldBalanceData = tcamData->ldBalanceData;
	tcam.DSCPOverride = tcamData->DSCPOverride;
	tcam.DSCP = tcamData->DSCP;
	tcam.factionOverride = tcamData->factionOverride;
	tcam.factionData = tcamData->factionData;
	tcam.tcamTunnel = MSD_FALSE; /* Reserved*/
	tcam.ipMulticast = tcamData->ipMulticast;
	tcam.ip2me = tcamData->ip2me;
	tcam.routeEntry = tcamData->routeEntry;
	tcam.tcamCtr = tcamData->tcamCtr;


	for (i = 0; i < 48U; i++)
	{
		tcam.frameOctet[i] = tcamData->frameOctet[i];
		tcam.frameOctetMask[i] = tcamData->frameOctetMask[i];
	}
	return Spruce_gtcamLoadEntry(dev, tcamPointer, &tcam);
}
/*******************************************************************************
* Spruce_gtcamReadTCAMData
*
* DESCRIPTION:
*       This routine reads the global 3 offsets 0x02 to 0x1B registers with 
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
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
MSD_STATUS Spruce_gtcamReadTCAMDataIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer,
    OUT MSD_TCAM_DATA        *tcamData
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_TCAM_DATA tcam;
	MSD_U8 i = 0;

	retVal=Spruce_gtcamReadTCAMData(dev, tcamPointer, &tcam);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Spruce_gtcamReadTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)tcamData, 0, sizeof(MSD_TCAM_DATA));

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->timeKey = tcam.timeKey;
	tcamData->timeKeyMask = tcam.timeKeyMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;
	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->colorMode = tcam.colorMode;
	tcamData->vtuPageOverride = tcam.vtuPageOverride;
	tcamData->vtuPage = tcam.vtuPage;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->egActPoint = tcam.egActPoint;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->DSCPOverride = tcam.DSCPOverride;
	tcamData->DSCP = tcam.DSCP;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;
	tcamData->tcamTunnel = MSD_FALSE; /* Reserved */
	tcamData->ipMulticast = tcam.ipMulticast;
	tcamData->ip2me = tcam.ip2me;
	tcamData->routeEntry = tcam.routeEntry;
	tcamData->tcamCtr = tcam.tcamCtr;
	for (i = 0; i < 48U; i++)
	{
		tcamData->frameOctet[i] = tcam.frameOctet[i];
		tcamData->frameOctetMask[i] = tcam.frameOctetMask[i];
	}
	return retVal;
}
/*******************************************************************************
* Spruce_gtcamGetNextTCAMDataRCIntf
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
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamGetNextTCAMDataIntf
(
    IN     MSD_QD_DEV     *dev,
    INOUT  MSD_U32        *tcamPointer,
    OUT    MSD_TCAM_DATA  *tcamData
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_TCAM_DATA tcam;
	MSD_U8 i = 0;

	retVal = Spruce_gtcamGetNextTCAMData(dev, tcamPointer, &tcam);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Spruce_gtcamGetNextTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)tcamData, 0, sizeof(MSD_TCAM_DATA));

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->timeKey = tcam.timeKey;
	tcamData->timeKeyMask = tcam.timeKeyMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;
	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->colorMode = tcam.colorMode;
	tcamData->vtuPageOverride = tcam.vtuPageOverride;
	tcamData->vtuPage = tcam.vtuPage;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->egActPoint = tcam.egActPoint;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->DSCPOverride = tcam.DSCPOverride;
	tcamData->DSCP = tcam.DSCP;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;
	tcamData->tcamTunnel = MSD_FALSE; /* Reserved */
	tcamData->ipMulticast = tcam.ipMulticast;
	tcamData->ip2me = tcam.ip2me;
	tcamData->routeEntry = tcam.routeEntry;
	tcamData->tcamCtr = tcam.tcamCtr;
	for (i = 0; i < 48U; i++)
	{
		tcamData->frameOctet[i] = tcam.frameOctet[i];
		tcamData->frameOctetMask[i] = tcam.frameOctetMask[i];
	}
	return retVal;	
}
/*******************************************************************************
* Spruce_gtcamFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       tcamPointer - the tcam entry index to search.
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
MSD_STATUS Spruce_gtcamFindEntryIntf
(
    IN	MSD_QD_DEV     *dev,
    IN	MSD_U32        tcamPointer,
    OUT	MSD_TCAM_DATA  *tcamData,
	OUT	MSD_BOOL		  *found
)
{
	MSD_STATUS           retVal;
	SPRUCE_MSD_TCAM_DATA tcam;
	MSD_U8 i = 0;

    msdMemSet((void*)&tcam, 0, sizeof(SPRUCE_MSD_TCAM_DATA));

	retVal = Spruce_gtcamFindEntry(dev, tcamPointer, &tcam, found);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_DATA in Spruce_gtcamFindEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)tcamData, 0, sizeof(MSD_TCAM_DATA));

	if (*found == MSD_FALSE)
	{
		return retVal;
	}

	tcamData->frameType = tcam.frameType;
	tcamData->frameTypeMask = tcam.frameTypeMask;
	tcamData->timeKey = tcam.timeKey;
	tcamData->timeKeyMask = tcam.timeKeyMask;
	tcamData->spv = tcam.spv;
	tcamData->spvMask = tcam.spvMask;
	tcamData->ppri = tcam.ppri;
	tcamData->ppriMask = tcam.ppriMask;
	tcamData->pvid = tcam.pvid;
	tcamData->pvidMask = tcam.pvidMask;
	tcamData->continu = tcam.continu;
	tcamData->interrupt = tcam.interrupt;
	tcamData->IncTcamCtr = tcam.IncTcamCtr;
	tcamData->vidOverride = tcam.vidOverride;
	tcamData->vidData = tcam.vidData;
	tcamData->nextId = tcam.nextId;
	tcamData->qpriOverride = tcam.qpriOverride;
	tcamData->qpriData = tcam.qpriData;
	tcamData->fpriOverride = tcam.fpriOverride;
	tcamData->fpriData = tcam.fpriData;
	tcamData->dpvSF = tcam.dpvSF;
	tcamData->dpvData = tcam.dpvData;
	tcamData->dpvMode = tcam.dpvMode;
	tcamData->colorMode = tcam.colorMode;
	tcamData->vtuPageOverride = tcam.vtuPageOverride;
	tcamData->vtuPage = tcam.vtuPage;
	tcamData->unKnownFilter = tcam.unKnownFilter;
	tcamData->egActPoint = tcam.egActPoint;
	tcamData->ldBalanceOverride = tcam.ldBalanceOverride;
	tcamData->ldBalanceData = tcam.ldBalanceData;
	tcamData->DSCPOverride = tcam.DSCPOverride;
	tcamData->DSCP = tcam.DSCP;
	tcamData->factionOverride = tcam.factionOverride;
	tcamData->factionData = tcam.factionData;
	tcamData->tcamTunnel = MSD_FALSE; /* reserved */
	tcamData->ipMulticast = tcam.ipMulticast;
	tcamData->ip2me = tcam.ip2me;
	tcamData->routeEntry = tcam.routeEntry;
	tcamData->tcamCtr = tcam.tcamCtr;
	for (i = 0; i < 48U; i++)
	{
		tcamData->frameOctet[i] = tcam.frameOctet[i];
		tcamData->frameOctetMask[i] = tcam.frameOctetMask[i];
	}
	return retVal;	
}
/*******************************************************************************
* Spruce_gtcamSetModeIntf
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*       Note: Do not chage the value while frames are flowing into this port,
*             You must first put the port in the Disable Port State, then change
*             TCAM mode, and then re-enable the port's Port State again.
*
* INPUTS:
*       portNum  - the logical port number
*       mode     - tcam mode to be set
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Spruce_gtcamSetModeIntf
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN MSD_TCAM_MODE mode
)
{
    return Spruce_gtcamSetMode(dev, portNum, (SPRUCE_MSD_TCAM_MODE)mode);
}
/*******************************************************************************
* Spruce_gtcamGetModeIntf
*
* DESCRIPTION:
*       Set the TCAM Mode, The following mode is supported:
*               TCAM_MODE_DISABLE, TCAM_MODE_ENABLE_48, TCAM_MODE_ENABLE_96
*
* INPUTS:
*       portNum  - the logical port number
*
* OUTPUTS:
*       mode     - get back tcam mode
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Spruce_gtcamGetModeIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  portNum,
    OUT MSD_TCAM_MODE *mode
)
{
    MSD_STATUS retVal;
    SPRUCE_MSD_TCAM_MODE temp;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("Input param MSD_TCAM_MODE in Spruce_gtcamGetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Spruce_gtcamGetMode(dev, portNum, &temp);

    *mode = (MSD_TCAM_MODE)temp;

    return retVal;
}


/*******************************************************************************
* Spruce_gtcamEgrFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for a particular port. A
*       Flush a single Egress TCAM entry command will write the same values to a 
*       Egress TCAM entry as a Flush All command, but it is done to the selected 
*		single egress TCAM entry of the selected single port only.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
MSD_STATUS Spruce_gtcamEgrFlushEntryIntf
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer
)
{
	return Spruce_gtcamEgrFlushEntry(dev, port, tcamPointer);
}
/*******************************************************************************
* Spruce_gtcamEgrFlushEntryAllPorts
*
* DESCRIPTION:
*       This routine is to flush a single egress entry for all switch ports. 
*
* INPUTS:
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
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
MSD_STATUS Spruce_gtcamEgrFlushAllPortsIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U32        tcamPointer
)
{
	return Spruce_gtcamEgrFlushEntryAllPorts(dev, tcamPointer);
}
/*******************************************************************************
* Spruce_gtcamEgrLoadEntry
*
* DESCRIPTION:
*       This routine loads a single egress TCAM entry for a specific port.
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*       tcamData    - Tcam entry Data
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
MSD_STATUS Spruce_gtcamEgrLoadEntryIntf
(
    IN  MSD_QD_DEV     *dev,
	IN  MSD_LPORT      port,
    IN  MSD_U32        tcamPointer,
    IN  MSD_TCAM_EGR_DATA  *tcamData
)
{
	SPRUCE_MSD_TCAM_EGR_DATA  tcam;

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_EGR_DATA in Spruce_gtcamEgrLoadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcam.frameModeOverride = tcamData->frameModeOverride;
	tcam.frameMode = tcamData->frameMode;
	tcam.noTtlDec = tcamData->noTtlDec;
	tcam.tagModeOverride = tcamData->tagModeOverride;
	tcam.tagMode = tcamData->tagMode;
	tcam.daMode = tcamData->daMode;
	tcam.saMode = tcamData->saMode;

	tcam.egVidModeOverride = tcamData->egVidModeOverride;
	tcam.egVidMode = tcamData->egVidMode;
	tcam.egVidData = tcamData->egVidData;

	tcam.egDSCPMode = tcamData->egDSCPMode;
	tcam.egDSCP = tcamData->egDSCP;
	tcam.egfpriModeOverride = tcamData->egfpriModeOverride;
	tcam.egfpriMode = tcamData->egfpriMode;
	tcam.egEC = tcamData->egEC;
	tcam.egFPRI = tcamData->egFPRI;

	tcam.egSIDOverride = tcamData->egSIDOverride;
	tcam.egSID = tcamData->egSID;

	return Spruce_gtcamEgrLoadEntry(dev, port, tcamPointer, &tcam);
}
/*******************************************************************************
* Spruce_gtcamEgrGetNextTCAMData
*
* DESCRIPTION:
*       This routine finds the next higher Egress TCAM Entry number that is valid 
*       (i.e.,any entry that is non-zero). The TCAM Entry register (bits 5:0) is
*       used as the Egress TCAM entry to start from. To find the lowest number
*       Egress TCAM Entry that is valid, start the Get Next operation with Egress
*       TCAM Entry set to 0x3F.
*
* INPUTS:
*		 port		 - switch port
*        tcamPointer - pointer to start search TCAM 
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamEgrGetNextTCAMDataIntf
(
    IN     MSD_QD_DEV     *dev,
	IN	   MSD_LPORT		 port,
    INOUT  MSD_U32        *tcamPointer,
    OUT    MSD_TCAM_EGR_DATA  *tcamData
)
{
	SPRUCE_MSD_TCAM_EGR_DATA  tcam;
	MSD_STATUS retVal;

	retVal = Spruce_gtcamEgrGetNextTCAMData(dev, port, tcamPointer, &tcam);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_EGR_DATA in Spruce_gtcamEgrGetNextTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));

	tcamData->frameModeOverride = tcam.frameModeOverride;
	tcamData->frameMode = tcam.frameMode;
	tcamData->noTtlDec = tcam.noTtlDec;
	tcamData->tagModeOverride = tcam.tagModeOverride;
	tcamData->tagMode = tcam.tagMode;
	tcamData->daMode = tcam.daMode;
	tcamData->saMode = tcam.saMode;

	tcamData->egVidModeOverride = tcam.egVidModeOverride;
	tcamData->egVidMode = tcam.egVidMode;
	tcamData->egVidData = tcam.egVidData;

	tcamData->egDSCPMode = tcam.egDSCPMode;
	tcamData->egDSCP = tcam.egDSCP;
	tcamData->egfpriModeOverride = tcam.egfpriModeOverride;
	tcamData->egfpriMode = tcam.egfpriMode;
	tcamData->egEC = tcam.egEC;
	tcamData->egFPRI = tcam.egFPRI;

	tcamData->egSIDOverride = tcam.egSIDOverride;
	tcamData->egSID = tcam.egSID;
	return retVal;
}
/*******************************************************************************
* Spruce_gtcamEgrReadTCAMData
*
* DESCRIPTION:
*       This routine reads a single Egress TCAM entry for a specific port.
*
*
* INPUTS:
*		port		- switch port
*       tcamPointer - pointer to the desired entry of TCAM (1 ~ 63)
*
* OUTPUTS:
*       tcamData    - Tcam entry Data
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
MSD_STATUS Spruce_gtcamEgrReadTCAMDataIntf
(
    IN  MSD_QD_DEV     *dev,
	IN	MSD_LPORT	  port,
    IN  MSD_U32        tcamPointer,
    OUT MSD_TCAM_EGR_DATA  *tcamData
)
{
	SPRUCE_MSD_TCAM_EGR_DATA  tcam;
	MSD_STATUS retVal;

	retVal = Spruce_gtcamEgrReadTCAMData(dev, port, tcamPointer, &tcam);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_EGR_DATA in Spruce_gtcamEgrReadTCAMDataIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));

	tcamData->frameModeOverride = tcam.frameModeOverride;
	tcamData->frameMode = tcam.frameMode;
	tcamData->noTtlDec = tcam.noTtlDec;
	tcamData->tagModeOverride = tcam.tagModeOverride;
	tcamData->tagMode = tcam.tagMode;
	tcamData->daMode = tcam.daMode;
	tcamData->saMode = tcam.saMode;

	tcamData->egVidModeOverride = tcam.egVidModeOverride;
	tcamData->egVidMode = tcam.egVidMode;
	tcamData->egVidData = tcam.egVidData;

	tcamData->egDSCPMode = tcam.egDSCPMode;
	tcamData->egDSCP = tcam.egDSCP;
	tcamData->egfpriModeOverride = tcam.egfpriModeOverride;
	tcamData->egfpriMode = tcam.egfpriMode;
	tcamData->egEC = tcam.egEC;
	tcamData->egFPRI = tcam.egFPRI;

	tcamData->egSIDOverride = tcam.egSIDOverride;
	tcamData->egSID = tcam.egSID;
	return retVal;
}

/*******************************************************************************
* Spruce_gtcamAdvConfig
*
* DESCRIPTION:
*       This routine sets tcam entry to match standard IPv4/IPv6, TCP/UDP packets
*       with the given header data and mask.
*
* INPUTS:
*       pktType - Enumeration of TCAM mapping packet type
*       tcamPointer  - tcam entry number
*       tcamPointer2 - tcam entry2 number for cascade
*       keyMaskPtr   - mask pointer for TCAM entry key
*       keyPtr       - data pointer for TCAM entry key
*       maskPtr      - mask pointer for packet unit
*       patternPtr   - data pointer for packet unit
*       actionPtr    - action pionter
*
* OUTPUTS:
*       entry2Used - MSD_TRUE: two tcam entries been used, MSD_FALSE: only tcamPointer been used, tcamPointer2 not used.
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
MSD_STATUS Spruce_gtcamAdvConfigIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_TCAM_PKT_TYPE pktType,
    IN  MSD_U32 tcamPointer,
    IN  MSD_U32 tcamPointer2,
    IN  MSD_TCAM_ENT_KEY *keyMaskPtr,
    IN  MSD_TCAM_ENT_KEY *keyPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *maskPtr,
    IN  MSD_TCAM_ENT_PKT_UNT *patternPtr,
    IN  MSD_TCAM_ENT_ACT *actionPtr,
    OUT MSD_BOOL *entry2Used
)
{
    SPRUCE_MSD_TCAM_ENT_KEY tmpKeyMask;
    SPRUCE_MSD_TCAM_ENT_KEY tmpKey;
    SPRUCE_MSD_TCAM_ENT_ACT tmpAction;

    if ((keyMaskPtr == NULL) || (keyPtr == NULL) || (maskPtr == NULL) || (patternPtr == NULL) || (actionPtr == NULL) || (entry2Used == NULL))
    {
        MSD_DBG_ERROR(("Input pointer parameter is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpKeyMask.frameType = keyMaskPtr->frameType;
	tmpKeyMask.timeKey = keyMaskPtr->timeKey;
    tmpKeyMask.ppri = keyMaskPtr->ppri;
    tmpKeyMask.pvid = keyMaskPtr->pvid;
    tmpKeyMask.spv = keyMaskPtr->spv;

    tmpKey.frameType = keyPtr->frameType;
	tmpKey.timeKey = keyPtr->timeKey;
    tmpKey.ppri = keyPtr->ppri;
    tmpKey.pvid = keyPtr->pvid;
    tmpKey.spv = keyPtr->spv;

    tmpAction.colorMode = actionPtr->colorMode;
    tmpAction.dpvData = actionPtr->dpvData;
    tmpAction.dpvMode = actionPtr->dpvMode;
    tmpAction.dpvSF = actionPtr->dpvSF;
    tmpAction.DSCP = actionPtr->DSCP;
    tmpAction.DSCPOverride = actionPtr->DSCPOverride;
    tmpAction.egActPoint = actionPtr->egActPoint;
    tmpAction.factionData = actionPtr->factionData;
    tmpAction.factionOverride = actionPtr->factionOverride;
    tmpAction.fpriData = actionPtr->fpriData;
    tmpAction.fpriOverride = actionPtr->fpriOverride;
    tmpAction.IncTcamCtr = actionPtr->IncTcamCtr;
    tmpAction.interrupt = actionPtr->interrupt;
    tmpAction.ldBalanceData = actionPtr->ldBalanceData;
    tmpAction.ldBalanceOverride = actionPtr->ldBalanceOverride;
    tmpAction.qpriData = actionPtr->qpriData;
    tmpAction.qpriOverride = actionPtr->qpriOverride;
    tmpAction.unKnownFilter = actionPtr->unKnownFilter;
    tmpAction.vidData = actionPtr->vidData;
    tmpAction.vidOverride = actionPtr->vidOverride;
    tmpAction.vtuPage = actionPtr->vtuPage;
    tmpAction.vtuPageOverride = actionPtr->vtuPageOverride;
	tmpAction.tcamTunnel = MSD_FALSE; /* reserved */
	tmpAction.ipMulticast = actionPtr->ipMulticast;
	tmpAction.ip2me = actionPtr->ip2me;
	tmpAction.routeEntry = actionPtr->routeEntry;
	tmpAction.tcamCtr = actionPtr->tcamCtr;

    return Spruce_gtcamAdvConfig(dev, pktType, tcamPointer, tcamPointer2, &tmpKeyMask, &tmpKey, maskPtr, patternPtr, &tmpAction, entry2Used);
}