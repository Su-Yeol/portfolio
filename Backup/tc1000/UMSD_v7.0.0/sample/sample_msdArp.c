/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdARP.c
*
* DESCRIPTION:
*    The sample sample_msdARP_Unicast will demonstrate how to
*    1. Flush All Entries for ARP table.
*    2. Load a ARP entry for unicast frame to replace a new destination address.
*    3. Flush a single ARP entry.
*    4. Load the ARP entry for unicast frame again.
*    5. Get next higher valid ARP entry starting from a specific entry number.
*    6. Read a ARP entry.
*
*    The sample sample_msdARP_Multicast will demonstrate how to
*    1. Flush All Entries for ARP table.
*    2. Load a ARP entry for multicast frame to duplicate the frame with customized number per port.
*    3. Flush a single ARP entry.
*    4. Load the ARP entry for multicast frame again.
*    5. Get next higher valid ARP entry starting from a specific entry number.
*    6. Read a ARP entry.
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

static void displayARPData(MSD_ARP_DATA *arpData);

static void displayARPData(MSD_ARP_DATA *arpData)
{
	MSG_PRINT(("----- Route DA -----\n"));

	MSG_PRINT(("Route DA[15:0]     : %04x\n", (arpData->arpData[1] << 8 | arpData->arpData[0])));
	MSG_PRINT(("Route DA[31:16]    : %04x\n", (arpData->arpData[3] << 8 | arpData->arpData[2])));
	MSG_PRINT(("Route DA[47:32]    : %04x\n", (arpData->arpData[5] << 8 | arpData->arpData[4])));

}

MSD_STATUS sample_msdARP_Unicast(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        arpPointer;
	MSD_ARP_UC_DATA   arpUcData;
	MSD_ARP_DATA   arpData;

	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    Flush all entries for ARP table
	*/
	if ((status = msdArpAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdArpAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a ARP entry for unicast frame
	*/
	msdMemSet(&arpUcData, 0, sizeof(MSD_ARP_UC_DATA)); /* Set all bit to 0 by default */
	msdMemSet(&arpData, 0, sizeof(MSD_ARP_DATA)); /* Set all bit to 0 by default */

	/* Replace DA = 01:00:5E:00:00:00 */
	arpUcData.routeDA.arEther[5] = 0x01;
	arpUcData.routeDA.arEther[4] = 0x00;
	arpUcData.routeDA.arEther[3] = 0x5E;
	arpUcData.routeDA.arEther[2] = 0x00;
	arpUcData.routeDA.arEther[1] = 0x00;
	arpUcData.routeDA.arEther[0] = 0x00;

	/*
	*    Load this ARP entry.
	*/
	arpPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdArpUcEntryAdd(devNum, arpPointer, &arpUcData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpUcEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Flush this ARP entry above.
	*/
	if ((status = msdArpEntryDelete(devNum, arpPointer)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryDelete returned fail.\n"));
		return status;
	}

	/*
	*    Load this ARP entry again.
	*/
	arpPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdArpUcEntryAdd(devNum, arpPointer, &arpUcData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpUcEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Get next higher valid ARP entry starting from a specific entry number.(0xff).
	*    Start to search valid ARP entry from the lowest arpPointer
	*    Display this found valid ARP entry
	*/
	arpPointer = 0xff; 
	if ((status = msdArpEntryGetNext(devNum, &arpPointer, &arpData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryGetNext returned fail.\n"));
		return status;
	}
	MSG_PRINT(("find the next valid ARP entry number is %d\n", (int)arpPointer));/* result arpPointer==1*/

	/*
	*    Read this ARP entry.
	*/
	if ((status = msdArpEntryRead(devNum, arpPointer, &arpData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryRead returned fail.\n"));
		return status;
	}
	displayARPData(&arpData);
	MSG_PRINT(("sample_msdARP_Unicast sample code success.\n"));
	return MSD_OK;
}

MSD_STATUS sample_msdARP_Multicast(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        arpPointer;
	MSD_ARP_MC_DATA   arpMcData;
	MSD_ARP_DATA   arpData;

	MSD_QD_DEV *dev = sohoDevGet(devNum);

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}

	/*
	*    Flush all entries for ARP table
	*/
	if ((status = msdArpAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdArpAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize a ARP entry for multicast frame
	*/
	msdMemSet(&arpMcData, 0, sizeof(MSD_ARP_MC_DATA)); /* Set all bit to 0 by default */
	msdMemSet(&arpData, 0, sizeof(MSD_ARP_DATA)); /* Set all bit to 0 by default */

	/* If the entry is a Multicast routine entry, then each nibble is the egress port’s
	 * frame duplication number. 
	 * dupNum[0] = P0_DUP
	 * dupNum[1] = P1_DUP
	 * dupNum[2] = P2_DUP
	 * dupNum[3] = P3_DUP
	 * dupNum[4] = P4_DUP
	 * dupNum[5] = P5_DUP
	 * dupNum[6] = P6_DUP
	 * dupNum[7] = P7_DUP
	 * dupNum[8] = P8_DUP
	 * dupNum[9] = P9_DUP
	 * dupNum[10] = P10_DUP
	 */
	arpMcData.dupNum[0] = 0x01;
	arpMcData.dupNum[1] = 0x02;
	arpMcData.dupNum[2] = 0x03;
	arpMcData.dupNum[3] = 0x04;
	arpMcData.dupNum[4] = 0x05;
	arpMcData.dupNum[5] = 0x06;
	arpMcData.dupNum[6] = 0x07;
	arpMcData.dupNum[7] = 0x08;
	arpMcData.dupNum[8] = 0x09;
	arpMcData.dupNum[9] = 0x0a;
	arpMcData.dupNum[10] = 0x0b;

	/*
	*    Load this ARP entry.
	*/
	arpPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdArpMcEntryAdd(devNum, arpPointer, &arpMcData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpMcEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Flush this ARP entry above.
	*/
	if ((status = msdArpEntryDelete(devNum, arpPointer)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryDelete returned fail.\n"));
		return status;
	}

	/*
	*    Load this ARP entry again.
	*/
	arpPointer = 1;  /* 0 ~ 0xff */
	if ((status = msdArpMcEntryAdd(devNum, arpPointer, &arpMcData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpMcEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Get next higher valid ARP entry starting from a specific entry number.(0xff).
	*    Start to search valid ARP entry from the lowest arpPointer
	*    Display this found valid ARP entry
	*/
	arpPointer = 0xff;
	if ((status = msdArpEntryGetNext(devNum, &arpPointer, &arpData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryGetNext returned fail.\n"));
		return status;
	}
	MSG_PRINT(("find the next valid ARP entry number is %d\n", (int)arpPointer));/* result arpPointer==1*/

	/*
	*    Read this ARP entry.
	*/
	if ((status = msdArpEntryRead(devNum, arpPointer, &arpData)) != MSD_OK)
	{
		MSG_PRINT(("msdArpEntryRead returned fail.\n"));
		return status;
	}
	displayARPData(&arpData);
	MSG_PRINT(("sample_msdARP_Multicast sample code success.\n"));
	return MSD_OK;
}

MSD_STATUS sample_msdARP_static_routing(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_U32        tcamPointer1 = 1;
	MSD_TCAM_DATA  tcamData1;

	MSD_U32        arpPointer1 = 1;
	MSD_ARP_UC_DATA   arpData1;
	MSD_LPORT      port9 = 9;

	MSD_U32        egrTcamPointer = 0xf;
	MSD_TCAM_EGR_DATA egrTcamData;


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
	*     Step 1
	*     Initialize and load the ingress TCAM entry
	*
	*
	*     Use frameOctet and framOctetMask to match:
	*
	*     1. Ethernet frame format as below (unit: byte), with specified DA = 01:00:5E:00:00:00, Type = 0x0800 (ipv4)
	*                              DA                      SA                   Tag     Length/Type   Data and Pad         FCS
	*                             6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
	*                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*                   |                       |                       |               |       |       ......     |               |
	*                   +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
	*     frameOctet                                                                     0 8 0 0
	*     frameOctetMask                                                                 F F F F
	*
	*     2. IPv4 packet header format is in the "Data and Pad" filed of Ethernet frame (unit: bit) as below, with specified ipv4 Destination Address = 10.0.1.111/24, so network adderss is 10.0.1.0
	*       0           3                              11              15                              23                              31
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |Version-4 bits | Header Length |       Type of Service         |                Total Length                                   |  tcamData1.frameOctet[18] - tcamData1.frameOctet[21]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |                   Identifier                                  |   Flag    |           Fragment Offset                         |  tcamData1.frameOctet[22] - tcamData1.frameOctet[25]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |    Time to Live               |         Protocal              |               Header Checksum                                 |  tcamData1.frameOctet[26] - tcamData1.frameOctet[29]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |                                            Source IP Address - 32 bits                                                        |  tcamData1.frameOctet[30] - tcamData1.frameOctet[33]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |                                       Destination IP Address - 32 bits                                                        |  tcamData1.frameOctet[34] - tcamData1.frameOctet[37]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*     |                                                Options and Padding - 0-32 bits                                                |  tcamData1.frameOctet[38] - tcamData1.frameOctet[41]
	*     +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
	*
	*
	*     Ethernet Frame Type: tcamData1.frameOctet[16], tcamData1.frameOctet[17]
	*     IPv4 Destination Address:  tcamData1.frameOctet[34] - tcamData1.frameOctet[37]
	*
	*/

	msdMemSet(&tcamData1, 0, sizeof(MSD_TCAM_DATA)); /* Set tcamData1 all bit to 0 by default */

	tcamData1.frameType = 0x0;   /* Normal Port */
	tcamData1.frameTypeMask = 0x3;     /* Mask Type */


	/* Match Ethernet Frame Type = 0x0800(ipv4) */
	tcamData1.frameOctet[16] = 0x08;
	tcamData1.frameOctetMask[16] = 0xff;
	tcamData1.frameOctet[17] = 0x00;
	tcamData1.frameOctetMask[17] = 0xff;

	/* Match IPv4 Destination Network Address = 10.0.1.0/24(A.0.1.0/24), the last byte is host address, do not care */
	tcamData1.frameOctet[34] = 0xA; //match A
	tcamData1.frameOctetMask[34] = 0xff;
	tcamData1.frameOctet[35] = 0x0;  // match 0
	tcamData1.frameOctetMask[35] = 0xff;
	tcamData1.frameOctet[36] = 0x1; // match 1
	tcamData1.frameOctetMask[36] = 0xff;
	tcamData1.frameOctet[37] = 0x00; // do not care
	tcamData1.frameOctetMask[37] = 0x00;

	tcamData1.continu = 0x0;  /*  continue code should be set to 0, no next entry */
	tcamData1.routeEntry = 0x1; /*  this entry is a route entry */
	tcamData1.nextId = 0x1;   /*  nextId/flowId means use APR table entry No.1's content to replace the MAC DA of the hit frame */
	tcamData1.dpvMode = 0x3; /*  Replace the frame¡¯s RPV with the TCAM Entry¡¯s RPV Data */
	tcamData1.dpvData = 0x200; /* B01000000000, frame hit this entry will egress from port9, so all the hit frame (with ipv4 DA network address = 10.0.1.x/24) will be routed to port1*/
	tcamData1.egActPoint = (MSD_U8)egrTcamPointer;  /* in order to cascade ingress and egress tcam entry, EgrTcamPointer should NOT be 0*/

	if ((status = msdTcamEntryAdd(devNum, tcamPointer1, &tcamData1)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
		return status;
	}

	msdMemSet(&arpData1, 0, sizeof(MSD_ARP_UC_DATA)); /* Set arpData1 all bit to 0 by default */

	/*
	*    Step 2
	*    Initialize and load the arp entry
	*    The hit ipv4 frame's MAC DA will be replaced by the specified MAC Address, like the following address 01-02-03-04-05-06
	*/
	arpData1.routeDA.arEther[0] = 0x1;
	arpData1.routeDA.arEther[1] = 0x2;
	arpData1.routeDA.arEther[2] = 0x3;
	arpData1.routeDA.arEther[3] = 0x4;
	arpData1.routeDA.arEther[4] = 0x5;
	arpData1.routeDA.arEther[5] = 0x6;

	if ((status = msdArpUcEntryAdd(devNum, arpPointer1, &arpData1)) != MSD_OK)
	{
		MSG_PRINT(("msdArpUcEntryAdd returned fail.\n"));
		return status;
	}


	/*
	*    Step 3
	*    Initialize and load the egress TCAM entry
	*    The hit ipv4 frame's MAC SA will be replaced by the pre-defined switch MAC Address, e.g. modify the frame's SA to MAC1(pre-defined in Global 2 offset 0x0D)
	*/
	msdMemSet(&egrTcamData, 0, sizeof(MSD_TCAM_EGR_DATA)); /* Set all bit to 0 by default */
	egrTcamData.saMode = 0x1; /* modify the frame's SA to MAC1 (pre-defined in Global 2 offset 0x0D) */
	if ((status = msdEgrTcamEntryAdd(devNum, port9, egrTcamPointer, &egrTcamData)) != MSD_OK)
	{
		MSG_PRINT(("msdEgrTcamEntryAdd returned fail.\n"));
		return status;
	}


	MSG_PRINT(("sample_msd_static_routing sample code success.\n"));
	return MSD_OK;
}