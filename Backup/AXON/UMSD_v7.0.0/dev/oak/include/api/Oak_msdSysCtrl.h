/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Oak_msdSysCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell SysCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Oak_msdSysCtrl_h
#define Oak_msdSysCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported SysCtrl Types			 			                                   */
/****************************************************************************/
#define OAK_MAX_LAG_MASK_NUM    8U

/****************************************************************************/
/* Exported SysCtrl Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* Oak_gsysSetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       en - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetFloodBC
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
);

/*******************************************************************************
* Oak_gsysGetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetFloodBC
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
);

/*******************************************************************************
* Oak_gsysSetTrunkMaskTable
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       maskNum - Mask Number.
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetTrunkMaskTable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       maskNum,
    IN MSD_U32       trunkMask
);

/*******************************************************************************
* Oak_gsysGetTrunkMaskTable
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if maskNum > 0x7)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetTrunkMaskTable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       maskNum,
    OUT MSD_U32       *trunkMask
);


/*******************************************************************************
* Oak_gsysSetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*       select the TrunkMask to use.
*
* INPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetHashTrunk
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
);


/*******************************************************************************
* Oak_gsysGetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to 
*       select the TrunkMask to use.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetHashTrunk
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
);

/*******************************************************************************
* Oak_gsysSetTrunkRouting
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetTrunkRouting
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       trunkId,
    IN MSD_U32       trunkRoute
);

/*******************************************************************************
* Oak_gsysGetTrunkRouting
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetTrunkRouting
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       trunkId,
    OUT MSD_U32       *trunkRoute
);

/*******************************************************************************
* Oak_gsysSetEgressMonitorDest
*
* DESCRIPTION:
*       This routine sets Egress Monitor Destination Port. Frames that are 
*       targeted toward an Egress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Egress Monitor type, and frames transmitted on a Network port that 
*       is enabled to be the Egress Monitor Source Port.
*       If the Egress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Egress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to get 
*       to the device that contains the Egress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.If port = 0x1F, EMD is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetEgressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
);

/*******************************************************************************
* Oak_gsysGetEgressMonitorDest
*
* DESCRIPTION:
*       This routine gets Egress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetEgressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
);

/*******************************************************************************
* Oak_gsysSetSMISetup
*
* DESCRIPTION:
*       This routine is to set SMI PHY Setup
*
* INPUTS:
*       pointer - [9:0] bits (should be less than 0x400)
*		data	- SMI PHY Setup.
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetSMISetup
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT	portNum,
IN MSD_U16		  data
);

/*******************************************************************************
* Oak_gsysGetSMISetup
*
* DESCRIPTION:
*       This routine is to get SMI PHY Setup
*
* INPUTS:
*       portNum - [9:0] bits (should be less than 0x400)
*
* OUTPUTS:
*       data	- SMI PHY Setup.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetSMISetup
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		portNum,
OUT MSD_U16			*data
);

MSD_STATUS Oak_gsysGlobal1RegDump
(
IN  MSD_QD_DEV    *dev
);

MSD_STATUS Oak_gsysGlobal2RegDump
(
IN  MSD_QD_DEV    *dev
);

/*******************************************************************************
* Oak_gsysSetRMUMode
*
* DESCRIPTION:
*       These routine sets Remote Management Unit Mode.
*
* INPUTS:
*       data - RMU mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetRMUMode
(
IN MSD_QD_DEV    *dev,
IN MSD_U16        data
);

/*******************************************************************************
* Oak_gsysGetRMUMode
*
* DESCRIPTION:
*       These routine gets Remote Management Unit Mode..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       data - RMU mode
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetRMUMode
(
IN  MSD_QD_DEV    *dev,
OUT MSD_U16      *data
);

/*******************************************************************************
* Oak_gsysQbvWrite
*
* DESCRIPTION:
*       These routine write QBV registers.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*       data       - QBV data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysQbvWrite
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
IN MSD_U16       data
);

/*******************************************************************************
* Oak_gsysQbvRead
*
* DESCRIPTION:
*       These routine gets QBV register value.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*
* OUTPUTS:
*       data - QBV data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysQbvRead
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
OUT MSD_U16      *data
);

/*******************************************************************************
* Oak_gsysDevIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt mask, any combination of the folowing:
*                MSD_WATCH_DOG_INT			0x8000
*                MSD_JAM_LIMIT_INT			0x4000
*                MSD_DUPLEX_MISMATCH_INT	0x2000
*                MSD_WAKE_EVENT_INT			0x1000
*                MSD_PHY_INT_10 			0x400
*                MSD_PHY_INT_9 				0x200
*                MSD_PHY_INT_8 				0x100
*                MSD_PHY_INT_7 				0x80
*                MSD_PHY_INT_6 				0x40
*                MSD_PHY_INT_5 				0x20
*                MSD_PHY_INT_4 				0x10
*                MSD_PHY_INT_3 				0x8
*                MSD_PHY_INT_2 				0x4
*                MSD_PHY_INT_1 				0x2
*                MSD_PHY_INT_0 				0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
);

/*******************************************************************************
* Oak_gsysDevIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt mask
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
);

/*******************************************************************************
* Oak_gsysDevIntStatusGet
*
* DESCRIPTION:
*       These routine gets interrupt source.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt source
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *status
);

/*******************************************************************************
* Oak_gsysActiveIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt data, any combination of the folowing:
*                MSD_DEVICE2_INT	0x200
*                MSD_AVB_INT 		0x100
*                MSD_DEVICE_INT		0x80
*                MSD_STATS_DONE		0x40
*                MSD_VTU_PROB		0x20
*                MSD_VTU_DONE		0x10
*                MSD_ATU_PROB		0x8
*                MSD_ATU_DONE 		0x4
*                MSD_TCAM_INT		0x2
*                MSD_EE_INT			0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
);

/*******************************************************************************
* Oak_gsysActiveIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
);

/*******************************************************************************
* Oak_gsysActiveIntStatusGet
*
* DESCRIPTION:
*       This routine reads an hardware driven event status.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt status
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16      *status
);

#ifdef __cplusplus
}
#endif

#endif /* __Oak_msdSysCtrl_h */
