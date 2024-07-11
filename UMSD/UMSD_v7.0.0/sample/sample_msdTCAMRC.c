/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdTCAMRC.c
*
* DESCRIPTION:
*    the sample sample_msdTCAMRC will demonstrate how to
*    1. Flush All Entries for TCAM Range Check table.
*    2. Load a TCAM RC entry to match Ethernet frame with specified DA and vid.
*    3. Flush a single TCAM RC entry.
*    4. Load the TCAM RC entry again.
*    5. Get next higher valid TCAM RC entry starting from a specific entry number.
*    6. Read a TCAM RC entry.
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

static void displayTcamRCData(MSD_TCAM_RC_DATA *rcData);

static void displayTcamRCData(MSD_TCAM_RC_DATA *rcData)
{
	MSG_PRINT(("----- Range Check Data -----\n"));

	MSG_PRINT(("rcIndex           : %04x\n", rcData->rcIndex));
	MSG_PRINT(("rcResult          : %04x\n", rcData->rcResult));
	MSG_PRINT(("rcResultMask      : %04x\n", rcData->rcResultMask));

	MSG_PRINT(("----- Ingress Action -----\n"));

	MSG_PRINT(("continu           : %04x\n", rcData->continu));
	MSG_PRINT(("interrupt         : %04x\n", rcData->interrupt));
	MSG_PRINT(("IncTcamCtr        : %04x\n", rcData->IncTcamCtr));
	MSG_PRINT(("tcamCtr           : %04x\n", rcData->tcamCtr));
	MSG_PRINT(("vidOverride       : %04x\n", rcData->vidOverride));
	MSG_PRINT(("vidData           : %04x\n", rcData->vidData));

	MSG_PRINT(("nextId            : %04x\n", rcData->nextId));
	MSG_PRINT(("qpriOverride      : %04x\n", rcData->qpriOverride));
	MSG_PRINT(("qpriData          : %04x\n", rcData->qpriData));
	MSG_PRINT(("fpriOverride      : %04x\n", rcData->fpriOverride));
	MSG_PRINT(("fpriData          : %04x\n", rcData->fpriData));

	MSG_PRINT(("dpvSF             : %04x\n", rcData->dpvSF));
	MSG_PRINT(("dpvData           : %04x\n", (unsigned int)rcData->dpvData));
	MSG_PRINT(("dpvMode           : %04x\n", rcData->dpvMode));

	MSG_PRINT(("unKnownFilter     : %04x\n", rcData->unKnownFilter));

	MSG_PRINT(("ldBalanceOverride : %04x\n", rcData->ldBalanceOverride));
	MSG_PRINT(("ldBalanceData     : %04x\n", rcData->ldBalanceData));

	MSG_PRINT(("factionOverride   : %04x\n", rcData->factionOverride));
	MSG_PRINT(("factionData       : %04x\n", rcData->factionData));

	MSG_PRINT(("colorMode         : %04x\n", rcData->colorMode));
	MSG_PRINT(("vtuPageOverride   : %04x\n", rcData->vtuPageOverride));
	MSG_PRINT(("vtuPage           : %04x\n", rcData->vtuPage));
	MSG_PRINT(("egActPoint        : %04x\n", rcData->egActPoint));
	MSG_PRINT(("DSCPOverride      : %04x\n", rcData->DSCPOverride));
	MSG_PRINT(("DSCP              : %04x\n", rcData->DSCP));

	MSG_PRINT(("tcamTunnel        : %04x\n", rcData->tcamTunnel));
	MSG_PRINT(("ipMulticast       : %04x\n", rcData->ipMulticast));
	MSG_PRINT(("ip2me             : %04x\n", rcData->ip2me));
	MSG_PRINT(("routeEntry        : %04x\n", rcData->routeEntry));
}

/**************************************************************************************
*                                                                                     *
*              Following Sample only supported by Amethyst                            *
*                                                                                     *
**************************************************************************************/
/********************************************************************************
* sample_msdTCAMRC
*
*	This sample API will demonstrate how to load/read/getNext/flush TCAM Range Check entry:
*
*******************************************************************************/
MSD_STATUS sample_msdTCAMRC(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        rcPointer;
	MSD_TCAM_RC_DATA  rcData;

	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    Flush all entries for TCAM RC table
	*/
	if ((status = msdTcamRCAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a TCAM RC entry
	*/
	msdMemSet(&rcData, 0, sizeof(MSD_TCAM_RC_DATA)); /* Set all bit to 0 by default */
	rcData.rcIndex = 0x0;		 /* range check index ?link between frame TCAM hit and range check TCAM */
	rcData.rcResult = 0x3;       /* Comapre Vector */
	rcData.rcResultMask = 0xff;   /* Comapre Vector Mask */

	rcData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
	rcData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	rcData.dpvMode = 0x3; /*  Replace the frame's DPV with the TCAM RC Entry's DPV Data */
	rcData.dpvData = 0x006; /*B0000110, frame hit this entry will egress from port1 and port2*/

	/*
	*    Load this TCAM RC entry.
	*/
	rcPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdTcamRCEntryAdd(devNum, rcPointer, &rcData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Flush this TCAM RC entry above.
	*/
	if ((status = msdTcamRCEntryDelete(devNum, rcPointer)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryDelete returned fail.\n"));
		return status;
	}

	/*
	*    Load this TCAM RC entry again.
	*/
	rcPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdTcamRCEntryAdd(devNum, rcPointer, &rcData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Get next higher valid TCAM RC entry starting from a specific entry number.(0xff).
	*    Start to search valid TCAM RC entry from the lowest rcPointer
	*    Display this found valid TCAM RC entry
	*/
	rcPointer = 0xff; /*0xff for Peridot/Amethyst, 0x7f for Topaz*/
	if ((status = msdTcamRCEntryGetNext(devNum, &rcPointer, &rcData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryGetNext returned fail.\n"));
		return status;
	}
	MSG_PRINT(("find the next valid TCAM RC entry number is %d\n", (int)rcPointer));/* result rcPointer==1*/
	displayTcamRCData(&rcData);

	/*
	*    Read this TCAM RC entry.
	*    Display this TCAM RC
	*/
	if ((status = msdTcamRCEntryRead(devNum, rcPointer, &rcData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryRead returned fail.\n"));
		return status;
	}
	displayTcamRCData(&rcData);


	MSG_PRINT(("sample_msdTCAMRC sample code success.\n"));
	return MSD_OK;
}

/********************************************************************************
* sample_msdTCAMRC_Adv
*
*	This sample API will demonstrate how to use Range Check Entry to match E-Tagged 
*	frames with ECID between 0x1 and 0x11.
*
*******************************************************************************/
MSD_STATUS sample_msdTCAMRC_Adv(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer = 1;
	MSD_U8         rcPointer = 0x11;
	MSD_LPORT	   portNum = 0;
	MSD_TCAM_DATA  tcamData;
	MSD_TCAM_RC_DATA  rcData;
	MSD_TCAM_RC_COMP_DATA tcamRCCompData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    First, set TCAM Mode for all ports
	*    In the real operation, customize this part of code according to your hardware setup
	*    Set the TCAM Mode for the ports which want to use tcam
	*/
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	*    Flush all entries for TCAM table
	*/
	if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA)); /* Set tcamData all bit to 0 by default */
	
	tcamData.nextId = rcPointer;	/* The nextId is passed to the Range Check TCAM as a key for that search. */
	tcamData.continu = 0x6;			/* continue bit should be set to 6, Done frame matching, continue w/Range Checking immediately */

	if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}


	/*
	*    Initialize a TCAM RC entry
	*/
	msdMemSet(&rcData, 0, sizeof(MSD_TCAM_RC_DATA)); /* Set all bit to 0 by default */
	rcData.rcIndex = rcPointer;		 /* range check index, link between frame TCAM hit and range check TCAM */
	rcData.rcResult = 0x1;			 /* Range Check Compare Vector */
	rcData.rcResultMask = 0xff;		 /* Mask for Range Check Compare Vector */

	rcData.dpvMode = 0x3; /*  Replace the frame's DPV with the TCAM RC Entry's DPV Data */
	rcData.dpvData = 0x006; /*B0000110, frame hit this entry will egress from port1 and port2*/

	/*
	*    Load this TCAM RC entry.
	*/

	if ((status = msdTcamRCEntryAdd(devNum, rcPointer, &rcData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a Port Range Comparators 
	*/
	msdMemSet(&tcamRCCompData, 0, sizeof(MSD_TCAM_RC_COMP_DATA)); /* Set all bit to 0 by default */
	tcamRCCompData.Comp0Sel = 0x0;			/* Compare 0 Selections, select GRP:E-CID_Base(from E-Tagged frames)  */
	tcamRCCompData.Comp0Op = 0x0;			/* Comparator 0's Operation */
	tcamRCCompData.Comp0LoLimit = 0x1;		/* Compare Low Limit 0 */
	tcamRCCompData.Comp0HiLimit = 0x11;     /* Compare High Limit 0 */

	/*
	*    Load TCAM RCComp entry.
	*/

	if ((status = msdTcamRCCompAdd(devNum, portNum, &tcamRCCompData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCCompAdd returned fail.\n"));
		return status;
	}

	/******************************************************************************************************
	*   After this setup, if one E-Tag frame with the E-CID_Base between 0x1 and 0x11 entered to port0    *
	*   the frame will hit the TCAM range check entry and the frame will egress from port1 and port2      *
	*******************************************************************************************************/

	MSG_PRINT(("sample_msdTCAM_Adv sample code success.\n"));
	return MSD_OK;
}

	
