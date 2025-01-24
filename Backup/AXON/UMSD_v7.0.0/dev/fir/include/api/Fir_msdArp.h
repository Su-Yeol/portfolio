/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdArp.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell ARP table functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Fir_msdArp_h
#define Fir_msdArp_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************/
/* Exported TCAM Types			 			                                   */
/****************************************************************************/

#define FIR_MAX_ARP_ENTRY 256U

/*
*  typedef: union MSD_ARP_DATA
*
*  Description: union of arp table
*
*  Fields:
*      arpData	   - The data of ARP Table.
*/
typedef struct
{
	MSD_U8		arpData[6];
} FIR_MSD_ARP_DATA;

/*
*  typedef: union MSD_ARP_MC_DATA
*
*  Description: union of arp table for Multicast entry
*
*  Fields:
*      dupNum	   - number of frames to duplicate for each egress port.
*/
typedef struct
{
	MSD_U8		dupNum[MSD_MAX_SWITCH_PORTS];
} FIR_MSD_ARP_MC_DATA;

/*
*  typedef: union MSD_ARP_DATA
*
*  Description: union of arp table for Unicast entry
*
*  Fields:
*      routeDA	   - ARP DA MAC address used to replace DA field for the unicast routing frames.
*/
typedef struct
{
	MSD_ETHERADDR		routeDA;
} FIR_MSD_ARP_UC_DATA;

/*******************************************************************************
* Fir_garpLoadMcEntryIntf
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
MSD_STATUS Fir_garpLoadMcEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			arpEntryNum,
IN  MSD_ARP_MC_DATA  *arpMcData
);
/*******************************************************************************
* Fir_garpLoadMcEntry
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
MSD_STATUS Fir_garpLoadMcEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
IN  FIR_MSD_ARP_MC_DATA  *arpMcData
);
/*******************************************************************************
* Fir_garpLoadUcEntryIntf
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
MSD_STATUS Fir_garpLoadUcEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			arpEntryNum,
IN  MSD_ARP_UC_DATA  *arpUcData
);
/*******************************************************************************
* Fir_garpLoadUcEntry
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
*       arpUcDataLen	- number of unicast arp data
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
MSD_STATUS Fir_garpLoadUcEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
IN  FIR_MSD_ARP_UC_DATA	*arpUcData
);
/*******************************************************************************
* Fir_garpFlushEntry
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
MSD_STATUS Fir_garpFlushEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        arpEntryNum
);
/*******************************************************************************
* Fir_garpFlushAll
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
MSD_STATUS Fir_garpFlushAll
(
IN  MSD_QD_DEV     *dev
);
/*******************************************************************************
* Fir_garpReadEntryIntf
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
MSD_STATUS Fir_garpReadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		   arpEntryNum,
IN  MSD_ARP_DATA   *arpData
);
/*******************************************************************************
* Fir_garpReadEntry
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
MSD_STATUS Fir_garpReadEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        arpEntryNum,
OUT FIR_MSD_ARP_DATA        *arpData
);
/*******************************************************************************
* Fir_garpGetNextEntryIntf
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
MSD_STATUS Fir_garpGetNextEntryIntf
(
IN  MSD_QD_DEV     *dev,
INOUT  MSD_U32		*arpEntryNum,
OUT  MSD_ARP_DATA   *arpData
);
/*******************************************************************************
* Fir_garpGetNextEntry
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
MSD_STATUS Fir_garpGetNextEntry
(
IN     MSD_QD_DEV     *dev,
INOUT  MSD_U32        *arpEntryNum,
OUT    FIR_MSD_ARP_DATA  *arpData
);

#ifdef __cplusplus
}
#endif

#endif /* Fir_msdArp_h */
