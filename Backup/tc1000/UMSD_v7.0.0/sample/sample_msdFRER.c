/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdFRER.c
*
* DESCRIPTION:
*    This sample will demonstrate how to configure FRER.
*
* DEPENDENCIES:   None.
*
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Configure sequence generation to insert Rtag and sequence number.
*
*
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/

MSD_STATUS sample_msdSeqGen(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_EGRESS_MODE egressMode;
	MSD_FRER_SEQGEN seqGenData;
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
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	egressMode = MSD_EGRESS_MODE_UNTAGGED;
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}

		if ((status = msdPortEgressModeSet(devNum, port, egressMode)) != MSD_OK)
		{
			MSG_PRINT(("%s: %d, %s, fail, msdPortEgressModeSet return error.\n", __FILE__, __LINE__, __FUNCTION__));
			return status;
		}

	}

	/*
	*    Flush all entries for both ingress and egress TCAM table
	*/
	if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a TCAM entry
	*/
	msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
	tcamData.frameType = 0x0;   /* Normal Port */
	tcamData.frameTypeMask = 0x3;     /* Mask Type */

	/*
	*   use frameOctet and framOctetMask to match Ethernet frame format (unit: byte), with the DA = 01:00:5E:00:00:00
	*                            DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	*                           6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*                 |                       |                       |         | vid |       |       ......     |               |
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*   frameOctet     0 1 0 0 5 E 0 0 0 0 0 0                                 
	*   frameOctetMask F F F F F F F F F F F F                                 
	*/

	/* Match DA = 01:00:5E:00:00:00 */
	tcamData.frameOctet[0] = 0x01;
	tcamData.frameOctetMask[0] = 0xff;
	tcamData.frameOctet[1] = 0x00;
	tcamData.frameOctetMask[1] = 0xff;
	tcamData.frameOctet[2] = 0x5E;
	tcamData.frameOctetMask[2] = 0xff;
	tcamData.frameOctet[3] = 0x00;
	tcamData.frameOctetMask[3] = 0xff;
	tcamData.frameOctet[4] = 0x00;
	tcamData.frameOctetMask[4] = 0xff;
	tcamData.frameOctet[5] = 0x00;
	tcamData.frameOctetMask[5] = 0xff;

	tcamData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
	tcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	tcamData.frerSeqGenEn = 0x1; /* enable sequence generation */
	tcamData.frerIndvRcvyId = 0x2; /* use sequence generation instance id = 2 */


	/*
	*    Load this TCAM entry.
	*/
	tcamPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}


	/*
	*    Flush all sequence generation instance.
	*/
	if ((status = msdFrerSeqGenFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerSeqGenFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure sequence generation instance.
	*/
	seqGenData.seqGenId = 0x2;
	seqGenData.genSeqNum = 0x5;
	if ((status = msdFrerSeqGenLoad(devNum, &seqGenData)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerSeqGenLoad returned fail.\n"));
		return status;
	}

	/**************************************************************************************
	*   After this above configuration, the frame with the DA = 01:00:5E:00:00:00 entered *
	*   to port1, the frame will be filtered and insert Rtag and sequence number.         *
	***************************************************************************************/

	MSG_PRINT(("sample_msdSeqGen success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2:
*       Configure individual recovery to filter frames according to sequence id 
*		and bank 0 stream counter will record frames.
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/
MSD_STATUS sample_msdIndvRcvy(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_FRER_INDVRCVY indvRcvyData;
	MSD_FRER_BANK0_CONFIG bk0ConfigData;

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
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	*    Flush all entries for both ingress and egress TCAM table
	*/
	if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a TCAM entry
	*/
	msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
	tcamData.frameType = 0x0;   /* Normal Port */
	tcamData.frameTypeMask = 0x3;     /* Mask Type */

	/*
	*   use frameOctet and framOctetMask to match Ethernet frame format (unit: byte), with the DA = 01:00:5E:00:00:00
	*                            DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	*                           6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*                 |                       |                       |         | vid |       |       ......     |               |
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*   frameOctet     0 1 0 0 5 E 0 0 0 0 0 0
	*   frameOctetMask F F F F F F F F F F F F
	*/

	/* Match DA = 01:00:5E:00:00:00 */
	tcamData.frameOctet[0] = 0x01;
	tcamData.frameOctetMask[0] = 0xff;
	tcamData.frameOctet[1] = 0x00;
	tcamData.frameOctetMask[1] = 0xff;
	tcamData.frameOctet[2] = 0x5E;
	tcamData.frameOctetMask[2] = 0xff;
	tcamData.frameOctet[3] = 0x00;
	tcamData.frameOctetMask[3] = 0xff;
	tcamData.frameOctet[4] = 0x00;
	tcamData.frameOctetMask[4] = 0xff;
	tcamData.frameOctet[5] = 0x00;
	tcamData.frameOctetMask[5] = 0xff;

	tcamData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
	tcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	tcamData.frerSctrInc = 0x1; /* increment stream counter. */
	tcamData.frerSctrIndex = 0x2; /* stream counter index = 2 */
	tcamData.frerSeqEncType = 0x1; /* sequence encapsulate type */
	tcamData.frerIndvRcvyEn = 0x1; /* individual recovery enable */
	tcamData.frerIndvRcvyId = 0x3; /* individual recovery instance id = 3 */

	/*
	*    Load this TCAM entry.
	*/
	tcamPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}


	/*
	*    Flush all individual recovery instance.
	*/
	if ((status = msdFrerIndvRcvyFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerIndvRcvyFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure individual recovery instance.
	*/
	indvRcvyData.indvRcvyId = 0x3; /* individual recovery instance id = 3 */
	indvRcvyData.indvRcvyEn = 0x1; /* individual recovery enable */
	indvRcvyData.indvRcvyAlg = 0x1; /*individual recovery algorithm; 0 = match recovery algorithm; 1 = vector recovery algorithm*/
	indvRcvyData.indvRcvyTakeNoSeq = 0x1; /*individual recovery take no sequence enable*/
	indvRcvyData.indvRcvySeqHisLen = 0x1F; /*individual recovery sequence history length*/
	indvRcvyData.indvRcvyRstRate = 0x0; /*individual recovery reset rate*/
	indvRcvyData.indvRcvyRstTime = 0x0; /*individual recovery reset time*/


	if ((status = msdFrerIndvRcvyLoad(devNum, &indvRcvyData)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerIndvRcvyLoad returned fail.\n"));
		return status;
	}

	/*
	*    Flush all bank 0 configurations and counters.
	*/
	if ((status = msdFrerBank0SCtrFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerBank0SCtrFlushAll returned fail.\n"));
		return status;
	}

	/*
	*     Configure bank 0 stream counters.
	*/
	bk0ConfigData.bk0SCtrId = 0x2; /* bank 0 stream counter id = 2 */
	bk0ConfigData.bk0SCtrEn = 0x1; /* enable bank 0 stream counter */
	bk0ConfigData.bk0SCtrPort = 0x1; /* counter port 1 frames */
	bk0ConfigData.bk0SCtrIndex = 0x2; /*bank 0 stream counter index = 2 */
	if ((status = msdFrerBank0SCtrLoad(devNum, &bk0ConfigData)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerBank0SCtrLoad returned fail.\n"));
		return status;
	}

	/*******************************************************************************************
	*   After this above configuration, the frame with Rtag and the DA = 01:00:5E:00:00:00     *
	*   entered to port1 will be filtered by sequence number.                    *
\	*******************************************************************************************/

	MSG_PRINT(("sample_msdIndvRcvy success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #3:
*       Configure sequence recovery to filter frames according to sequence id 
*		and bank 1 stream counter will record frames.
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/
MSD_STATUS sample_msdSeqRcvy(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;

	MSD_FRER_SEQRCVY seqRcvyData;
	MSD_FRER_BANK1_CONFIG bk1ConfigData;

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
	tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
	for (port = 0; port < dev->maxPorts; port++)
	{
		if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
		{
			MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
			return status;
		}
	}

	/*
	*    Flush all entries for both ingress and egress TCAM table
	*/
	if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a TCAM entry
	*/
	msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
	tcamData.frameType = 0x0;   /* Normal Port */
	tcamData.frameTypeMask = 0x3;     /* Mask Type */

	/*
	*   use frameOctet and framOctetMask to match Ethernet frame format (unit: byte), with the DA = 01:00:5E:00:00:00
	*                            DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	*                           6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*                 |                       |                       |         | vid |       |       ......     |               |
	*                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*   frameOctet     0 1 0 0 5 E 0 0 0 0 0 0
	*   frameOctetMask F F F F F F F F F F F F
	*/

	/* Match DA = 01:00:5E:00:00:00 */
	tcamData.frameOctet[0] = 0x01;
	tcamData.frameOctetMask[0] = 0xff;
	tcamData.frameOctet[1] = 0x00;
	tcamData.frameOctetMask[1] = 0xff;
	tcamData.frameOctet[2] = 0x5E;
	tcamData.frameOctetMask[2] = 0xff;
	tcamData.frameOctet[3] = 0x00;
	tcamData.frameOctetMask[3] = 0xff;
	tcamData.frameOctet[4] = 0x00;
	tcamData.frameOctetMask[4] = 0xff;
	tcamData.frameOctet[5] = 0x00;
	tcamData.frameOctetMask[5] = 0xff;

	tcamData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
	tcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
	tcamData.frerSctrInc = 0x1; /* increment stream counter. */
	tcamData.frerSctrIndex = 0x2; /* stream counter index = 2 */
	tcamData.frerSeqEncType = 0x1; /* sequence encapsulate type */
	tcamData.frerSeqRcvyEn = 0x1; /* sequence recovery enable */
	tcamData.frerSeqRcvyIndex = 0x3; /* sequence recovery instance id = 3 */


	/*
	*    Load this TCAM entry.
	*/
	tcamPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}


	/*
	*    Flush all sequence recovery instance.
	*/
	if ((status = msdFrerSeqRcvyFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerSeqRcvyFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure sequence recovery instance.
	*/

	seqRcvyData.seqRcvyId = 0x3;
	seqRcvyData.seqRcvyEn = 0x1;
	seqRcvyData.seqRcvyPort = 0x3;
	seqRcvyData.seqRcvyIndex = 0x3;
	seqRcvyData.seqRcvyAlg = 0x1;
	seqRcvyData.seqRcvyTakeNoSeq = 0x1;
	seqRcvyData.latErrIntEn = 0x0;
	seqRcvyData.latErrStatus = 0x0;
	seqRcvyData.seqRcvySeqHisLen = 0x1F;

	if ((status = msdFrerSeqRcvyLoad(devNum, &seqRcvyData)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerSeqRcvyLoad returned fail.\n"));
		return status;
	}

	/*
	*    Flush all bank 1 configurations and counters.
	*/
	if ((status = msdFrerBank1SCtrFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerBank1SCtrFlushAll returned fail.\n"));
		return status;
	}

	/*
	*     Configure bank 1 stream counters.
	*/
	bk1ConfigData.bk1SCtrId = 0x2; /* bank 1 stream counter id = 2 */
	bk1ConfigData.bk1SCtrEn = 0x1; /* enable bank 1 stream counter */
	bk1ConfigData.bk1SCtrPort = 0x3; /* counter port 3 frames */
	bk1ConfigData.bk1SCtrIndex = 0x2; /*bank 0 stream counter index = 2 */
	if ((status = msdFrerBank1SCtrLoad(devNum, &bk1ConfigData)) != MSD_OK)
	{
		MSG_PRINT(("msdFrerBank1SCtrLoad returned fail.\n"));
		return status;
	}

	/*******************************************************************************************
	*   After this above configuration, the frame with the DA = 01:00:5E:00:00:00 sent from 2  *
	*	ports entered to port3, the frame will be filtered by sequence number.                 *
	*******************************************************************************************/

	MSG_PRINT(("sample_msdSeqRcvy success.\n"));
	return MSD_OK;
}