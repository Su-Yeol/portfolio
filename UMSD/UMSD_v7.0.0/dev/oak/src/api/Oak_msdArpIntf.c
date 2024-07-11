/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Oak_msdArpIntf.c
*
* DESCRIPTION:
*       API definitions for ARP table 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdArp.h>
#include <oak/include/api/Oak_msdApiInternal.h>

/*******************************************************************************
* Oak_garpLoadMcEntryIntf
*
* DESCRIPTION:
*       This routine loads an ARP entry for multicast.
*
* INPUTS:
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_garpLoadMcEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			arpEntryNum,
IN  MSD_ARP_MC_DATA  *arpMcData
)
{
	OAK_MSD_ARP_MC_DATA arp;
	MSD_U16 i;

	if (NULL == arpMcData)
	{
		MSD_DBG_ERROR(("Input param OAK_MSD_ARP_MC_DATA in Oak_garpLoadMcEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	for (i = 0; i < MSD_MAX_SWITCH_PORTS; i++)
	{
		arp.dupNum[i] = arpMcData->dupNum[i];
	}

	return Oak_garpLoadMcEntry(dev, arpEntryNum, &arp);
}

/*******************************************************************************
* Oak_garpLoadUcEntryIntf
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_garpLoadUcEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			arpEntryNum,
IN  MSD_ARP_UC_DATA  *arpUcData
)
{
	OAK_MSD_ARP_UC_DATA arp;
	MSD_U16 i;

	if (NULL == arpUcData)
	{
		MSD_DBG_ERROR(("Input param OAK_MSD_ARP_UC_DATA in Oak_garpLoadUcEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	for (i = 0; i < MSD_ETHERNET_HEADER_SIZE; i++)
	{
		arp.routeDA.arEther[i] = arpUcData->routeDA.arEther[i];
	}

	return Oak_garpLoadUcEntry(dev, arpEntryNum, &arp);
}

/*******************************************************************************
* Oak_garpReadEntryIntf
*
* DESCRIPTION:
*       This routine reads the ARP entry.
*
* INPUTS:
*       devNum			- physical device number
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
MSD_STATUS Oak_garpReadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
OUT MSD_ARP_DATA   *arpData
)
{
	MSD_STATUS           retVal;
	OAK_MSD_ARP_DATA arp;
	MSD_U16 i;

	if (NULL == arpData)
	{
		MSD_DBG_ERROR(("Input param MSD_ARP_DATA in Oak_garpReadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)arpData, 0, sizeof(MSD_ARP_DATA));

	retVal = Oak_garpReadEntry(dev, arpEntryNum, &arp);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	for (i = 0; i < MSD_ETHERNET_HEADER_SIZE; i++)
	{
		arpData->arpData[i] = arp.arpData[i];
	}

	return retVal;
}

/*******************************************************************************
* Oak_garpGetNextEntryIntf
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
MSD_STATUS Oak_garpGetNextEntryIntf
(
IN  MSD_QD_DEV     *dev,
INOUT  MSD_U32	    *arpEntryNum,
OUT  MSD_ARP_DATA   *arpData
)
{
	MSD_STATUS           retVal;
	OAK_MSD_ARP_DATA arp;
	MSD_U16 i;

	if (NULL == arpData)
	{
		MSD_DBG_ERROR(("Input param MSD_ARP_DATA in Oak_garpReadEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)arpData, 0, sizeof(MSD_ARP_DATA));

	retVal = Oak_garpGetNextEntry(dev, arpEntryNum, &arp);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	for (i = 0; i < MSD_ETHERNET_HEADER_SIZE; i++)
	{
		arpData->arpData[i] = arp.arpData[i];
	}

	return retVal;
}