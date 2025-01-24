/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdTCAMRCComp.c
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

static void displayTcamRCCompData(MSD_TCAM_RC_COMP_DATA *tcamRCCompData);

static void displayTcamRCCompData(MSD_TCAM_RC_COMP_DATA *tcamRCCompData)
{
	MSG_PRINT(("----- Port Range Comparator -----\n"));

	MSG_PRINT(("Comp0Sel          : %04x\n", tcamRCCompData->Comp0Sel));
	MSG_PRINT(("Comp0Op           : %04x\n", tcamRCCompData->Comp0Op));
	MSG_PRINT(("Comp0LoLimit      : %04x\n", tcamRCCompData->Comp0LoLimit));
	MSG_PRINT(("Comp0HiLimit      : %04x\n", tcamRCCompData->Comp0HiLimit));

	MSG_PRINT(("Comp1Sel          : %04x\n", tcamRCCompData->Comp1Sel));
	MSG_PRINT(("Comp1Op           : %04x\n", tcamRCCompData->Comp1Op));
	MSG_PRINT(("Comp1LoLimit      : %04x\n", tcamRCCompData->Comp1LoLimit));
	MSG_PRINT(("Comp1HiLimit      : %04x\n", tcamRCCompData->Comp1HiLimit));

	MSG_PRINT(("Comp2Sel          : %04x\n", tcamRCCompData->Comp2Sel));
	MSG_PRINT(("Comp2Op           : %04x\n", tcamRCCompData->Comp2Op));
	MSG_PRINT(("Comp2LoLimit      : %04x\n", tcamRCCompData->Comp2LoLimit));
	MSG_PRINT(("Comp2HiLimit      : %04x\n", tcamRCCompData->Comp2HiLimit));

	MSG_PRINT(("Comp3Sel          : %04x\n", tcamRCCompData->Comp3Sel));
	MSG_PRINT(("Comp3Op           : %04x\n", tcamRCCompData->Comp3Op));
	MSG_PRINT(("Comp3LoLimit      : %04x\n", tcamRCCompData->Comp3LoLimit));
	MSG_PRINT(("Comp3HiLimit      : %04x\n", tcamRCCompData->Comp3HiLimit));

	MSG_PRINT(("Comp4Sel          : %04x\n", tcamRCCompData->Comp4Sel));
	MSG_PRINT(("Comp4Op           : %04x\n", tcamRCCompData->Comp4Op));
	MSG_PRINT(("Comp4LoLimit      : %04x\n", tcamRCCompData->Comp4LoLimit));
	MSG_PRINT(("Comp4HiLimit      : %04x\n", tcamRCCompData->Comp4HiLimit));

	MSG_PRINT(("Comp5Sel          : %04x\n", tcamRCCompData->Comp5Sel));
	MSG_PRINT(("Comp5Op           : %04x\n", tcamRCCompData->Comp5Op));
	MSG_PRINT(("Comp5LoLimit      : %04x\n", tcamRCCompData->Comp5LoLimit));
	MSG_PRINT(("Comp5HiLimit      : %04x\n", tcamRCCompData->Comp5HiLimit));

	MSG_PRINT(("Comp6Sel          : %04x\n", tcamRCCompData->Comp6Sel));
	MSG_PRINT(("Comp6Op           : %04x\n", tcamRCCompData->Comp6Op));
	MSG_PRINT(("Comp6LoLimit      : %04x\n", tcamRCCompData->Comp6LoLimit));
	MSG_PRINT(("Comp6HiLimit      : %04x\n", tcamRCCompData->Comp6HiLimit));

	MSG_PRINT(("Comp7Sel          : %04x\n", tcamRCCompData->Comp7Sel));
	MSG_PRINT(("Comp7Op           : %04x\n", tcamRCCompData->Comp7Op));
	MSG_PRINT(("Comp7LoLimit      : %04x\n", tcamRCCompData->Comp7LoLimit));
	MSG_PRINT(("Comp7HiLimit      : %04x\n", tcamRCCompData->Comp7HiLimit));
}

MSD_STATUS sample_msdTCAMRCComp(MSD_U8 devNum)
{
	MSD_STATUS     status;
	MSD_LPORT	   portNum = 0;
	MSD_TCAM_RC_COMP_DATA   tcamRCCompData;

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
	msdMemSet(&tcamRCCompData, 0, sizeof(MSD_TCAM_RC_COMP_DATA)); /* Set all bit to 0 by default */

	/* Set  */
	tcamRCCompData.Comp0Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp0Sel = 0x0;		/* Select GRP:E-CID_Base (from E-Tagged frames) in the frame that is tested bt this comparator */
	tcamRCCompData.Comp0LoLimit = 0x01;  /* Define E-CID_Base's low limit */
	tcamRCCompData.Comp0HiLimit = 0x10; /* Define E-CID_Base's high limit */

	tcamRCCompData.Comp1Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp1Sel = 0x1;		/* Select Ingress_E-CID_Base (from E-Tagged frames) in the frame that is tested bt this comparator */
	tcamRCCompData.Comp1LoLimit = 0x02;  /* Define Ingress_E-CID_Base's low limit */
	tcamRCCompData.Comp1HiLimit = 0x20; /* Define Ingress_E-CID_Base's high limit */

	tcamRCCompData.Comp2Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp2Sel = 0x2;		/* Select CVLAN-ID (the VID from Q-Tagged frames) in the frame that is tested bt this comparator */
	tcamRCCompData.Comp2LoLimit = 0x03;  /* Define CVLAN-ID's low limit */
	tcamRCCompData.Comp2HiLimit = 0x30; /* Define CVLAN-ID's high limit */

	tcamRCCompData.Comp3Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp3Sel = 0x3;		/* Select SVLAN-ID (the VID from S-Tagged frames) in the frame that is tested bt this comparator */
	tcamRCCompData.Comp3LoLimit = 0x04; /* Define SVLAN-ID's low limit */
	tcamRCCompData.Comp3HiLimit = 0x40; /* Define SVLAN-ID's high limit */

	tcamRCCompData.Comp4Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp4Sel = 0x4;		/* Select TCP Source Port in the frame that is tested bt this comparator */
	tcamRCCompData.Comp4LoLimit = 0x05; /* Define TCP Source Port's low limit */
	tcamRCCompData.Comp4HiLimit = 0x50; /* Define TCP Source Port's high limit */

	tcamRCCompData.Comp5Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp5Sel = 0x5;		/* Select TCP Destination Port in the frame that is tested bt this comparator */
	tcamRCCompData.Comp5LoLimit = 0x06; /* Define TCP Destination Port's low limit */
	tcamRCCompData.Comp5HiLimit = 0x60; /* Define TCP Destination Port's high limit */

	tcamRCCompData.Comp6Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp6Sel = 0x6;		/* Select UDP Source Port in the frame that is tested bt this comparator */
	tcamRCCompData.Comp6LoLimit = 0x07; /* Define UDP Source Port's low limit */
	tcamRCCompData.Comp6HiLimit = 0x70; /* Define UDP Source Port's high limit */

	tcamRCCompData.Comp7Op = 0;			/* Match if field is inside this comparator's values */
	tcamRCCompData.Comp7Sel = 0x7;		/* Select UDP Destination Port in the frame that is tested bt this comparator */
	tcamRCCompData.Comp7LoLimit = 0x08; /* Define UDP Destination Port's low limit */
	tcamRCCompData.Comp7HiLimit = 0x80; /* Define UDP Destination Port's high limit */
	/*
	*    Load this Range Comparator at port 0.
	*/
	if ((status = msdTcamRCCompAdd(devNum, portNum, &tcamRCCompData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCCompAdd returned fail.\n"));
		return status;
	}

	/*
	*    Flush this Range Comparator above.
	*/
	if ((status = msdTcamRCCompFlushPort(devNum, portNum)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCCompFlushPort returned fail.\n"));
		return status;
	}

	/*
	*    Load this Range Comparator at port 0 again.
	*/
	portNum = 0;  
	if ((status = msdTcamRCCompAdd(devNum, portNum, &tcamRCCompData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCCompAdd returned fail.\n"));
		return status;
	}

	/*
	*    Read this Range Comparator.
	*/
	if ((status = msdTcamRCCompRead(devNum, portNum, &tcamRCCompData)) != MSD_OK)
	{
		MSG_PRINT(("msdTcamRCCompRead returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdARP_Unicast sample code success.\n"));
	displayTcamRCCompData(&tcamRCCompData);

	return MSD_OK;
}
