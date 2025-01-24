/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdQci.c
*
* DESCRIPTION:
*    This sample will demonstrate how to configure Qci.
*
* DEPENDENCIES:   None.
*
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Configure stream filter to filter frame size greater than 500 bytes.
*
*
* Note:
*		None.
*
* Comment:
*		Only support Fir, Bonsai
*
********************************************************************************/

MSD_STATUS sample_msdStreamFilter(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_U8 streamFilterId = 2;
	MSD_U16 maxSDUSize = 500;
	MSD_U8 blockEn = 0;

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
	tcamData.streamFilterEn = 0x1; /* enable stream filter */
	tcamData.streamFilterId = 0x2; /* use stream filter instance id = 2 */


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
	*    Flush all stream filter instance.
	*/
	if ((status = msdStreamFilterFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream filter instance.
	*/
	if ((status = msdStreamFilterSet(devNum, streamFilterId, maxSDUSize, blockEn)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterSet returned fail.\n"));
		return status;
	}

	/**************************************************************************************
	*   After this above configuration, the frame with the DA = 01:00:5E:00:00:00 entered *
	*   to port1 with frame size greater than 500 bytes, the frame will be filtered,      *
	*   otherwise, the frame can forward to all ports                                     *
	**************************************************************************************/

	MSG_PRINT(("sample_msdStreamFilter success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #2:
*       Configure stream gate to generate periodic window open or close.
*       Set cycle time to 4ms
*       Set 1st window open with window time 0xFFFF * 8ns
*       Set 2nd window close with window time 0xFFFF * 8ns
*       Set 3rd window open with window time 0xFFFF * 8ns
*       Set 4th window close with window time 0xFFFF * 8ns
*       Set 5th window open with window time 0xFFFF * 8ns
*       Set last window close with remain time
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/
MSD_STATUS sample_msdStreamGate(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_U8 streamFilterId = 2;
	MSD_U16 maxSDUSize = 0x3FFF;
	MSD_U8 blockEn = 0;

	MSD_U8 gateId = 0;
	MSD_U8 defGateState = 0;
	MSD_U8 defGateIPVEn = 0;
	MSD_U8 defGateIPV = 0;
	MSD_STREAM_GATE_ENTRY streamGateEntry;

	MSD_U8 gateTimeGenId = 0;
	MSD_U32  gateCycleTime = 4000000000;
	MSD_U64  gateBaseTime = 0;
	MSD_U8 trigGenTimeSel = 0;
	int i;

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
	tcamData.streamFilterEn = 0x1; /* enable stream filter */
	tcamData.streamFilterId = 0x2; /* use stream filter instance id = 2 */


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
	*    Flush all stream filter instance.
	*/
	if ((status = msdStreamFilterFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream filter instance.
	*/
	if ((status = msdStreamFilterSet(devNum, streamFilterId, maxSDUSize, blockEn)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterSet returned fail.\n"));
		return status;
	}

	/*
	*    Flush all stream gate instance.
	*/
	if ((status = msdStreamGateFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamGateFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Enable stream gate instance.
	*/
	if ((status = msdStreamFilterEnGateSet(devNum, streamFilterId, gateId)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterEnGateSet returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream gate instance, default gate state to close.
	*/
	if ((status = msdStreamGateDefStateSet(devNum, gateId, defGateState, defGateIPVEn, defGateIPV)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamGateDefStateSet returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream gate entry.
	*/
	for (i = 1; i < 6; i++)
	{
		streamGateEntry.gateState = i%2;
		streamGateEntry.gateIPVEn = 1;
		streamGateEntry.gateIPV = (MSD_U8)i;
		streamGateEntry.gateWindowTime = 0xFFFF;
		streamGateEntry.gateAllowedBytes = 0;

		if ((status = msdStreamGateEntryAdd(devNum, gateId, (MSD_U8)(i-1), &streamGateEntry)) != MSD_OK)
		{
			MSG_PRINT(("msdStreamGateEntryAdd returned fail.\n"));
			return status;
		}
	}

	/*
	*    Configure TrigGenTimeSel to use PTP Global Timer.
	*/
	if ((status = msdTaiTrigGenTimerSelSet(devNum, trigGenTimeSel)) != MSD_OK)
	{
		MSG_PRINT(("msdTaiTrigGenTimerSelSet returned fail.\n"));
		return status;
	}

	/*
	*    Flush all stream gate time geration instance.
	*/
	if ((status = msdStreamGateTimeGenFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamGateFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream gate select stream gate time geration id.
	*/
	if ((status = msdStreamGateTimeGenIdSet(devNum, gateId, gateTimeGenId)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamGateTimeGenIdSet returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream gate time geration cycle time.
	*/
	if ((status = msdStreamGateTimeGenSet(devNum, gateTimeGenId, gateCycleTime, gateBaseTime)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamGateTimeGenSet returned fail.\n"));
		return status;
	}


	/*******************************************************************************************
	*   After this above configuration, the frame with the DA = 01:00:5E:00:00:00 entered to   *
	*   port1, the frame will be filtered when stream gate window is closed, the frame can     *
	*   pass when stream gate window is open                                                   *
	*******************************************************************************************/

	MSG_PRINT(("sample_msdStreamGate success.\n"));
	return MSD_OK;
}

/********************************************************************************
* Sample #3:
*       Configure flow meter to limit stream with layer2 10Mbps.
*
*
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/
MSD_STATUS sample_msdFlowMeter(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer;
	MSD_TCAM_DATA  tcamData;

	MSD_LPORT port;
	MSD_TCAM_MODE tcamMode;
	MSD_U8 streamFilterId = 2;
	MSD_U16 maxSDUSize = 0x3FFF;
	MSD_U8 blockEn = 0;

	MSD_U8 flowMeterId = 1;
	MSD_U32 tgtRate = 10000;
	MSD_U32 bstSize = 1518;
	MSD_U8 countMode = 1;

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
	tcamData.streamFilterEn = 0x1; /* enable stream filter */
	tcamData.streamFilterId = 0x2; /* use stream filter instance id = 2 */
	tcamData.flowMeterEn = 1; /* enable flow meter */
	tcamData.flowMeterId = 0x1; /* use flow meter instance id = 1 */


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
	*    Flush all stream filter instance.
	*/
	if ((status = msdStreamFilterFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure stream filter instance.
	*/
	if ((status = msdStreamFilterSet(devNum, streamFilterId, maxSDUSize, blockEn)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterSet returned fail.\n"));
		return status;
	}

	/*
	*    Flush all flow meter instance.
	*/
	if ((status = msdFlowMeterFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdFlowMeterFlushAll returned fail.\n"));
		return status;
	}

	/*
	*    Configure flow meter instance to limit 10Mbps.
	*/
	if ((status = msdFlowMeterAdvConfig(devNum, flowMeterId, tgtRate, bstSize, countMode)) != MSD_OK)
	{
		MSG_PRINT(("msdStreamFilterSet returned fail.\n"));
		return status;
	}


	/*******************************************************************************************
	*   After this above configuration, the frame with the DA = 01:00:5E:00:00:00 entered to   *
	*   port1, the frame will be limited to 10Mbps                                             *
	*******************************************************************************************/

	MSG_PRINT(("sample_msdFlowMeter success.\n"));
	return MSD_OK;
}