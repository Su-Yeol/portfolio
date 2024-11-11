/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdSysCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Sys control functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdSysCtrl_h
#define msdSysCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdSysFloodBCSet
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode. VLAN rules and other switch policy still 
*       applies to these Broadcast frames.
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's
*       Egress Floods mode.
*
* INPUTS:
*       devNum - physical device number
*       en     - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysFloodBCSet
(
    IN MSD_U8   devNum,
    IN MSD_BOOL en
);

/*******************************************************************************
* msdSysFloodBCGet
*
* DESCRIPTION:
*       Flood Broadcast.This routine gets flood broadcast enable status. 
*       When Flood Broadcast is enabled, frames with the Broadcast destination
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode. VLAN rules and other switch policy still 
*       applies to these Broadcast frames.
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's
*       Egress Floods mode.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       en     - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysFloodBCGet
(
    IN MSD_U8    devNum,
    OUT MSD_BOOL *en
);

/*******************************************************************************
* msdSysMonitorMgmtCtrlSet
*
* DESCRIPTION:
*       Sets Monitor and mgmt control data.
*
* INPUTS:
*       devNum - physical device number
*       point  - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*       data   - Monitor and mgmt Control data written to the register
*                pointed to by the point above.
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
MSD_STATUS msdSysMonitorMgmtCtrlSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U8  point,
    IN  MSD_U8  data
);

/*******************************************************************************
* msdSysMonitorMgmtCtrlGet
*
* DESCRIPTION:
*       Gets Monitor and mgmt control data.
*
* INPUTS:
*       devNum - physical device number
*       point  - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2IntCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2IntCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*
* OUTPUTS:
*       data  - Monitor and mgmt Control data written to the register
*                pointed to by the point above.
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
MSD_STATUS msdSysMonitorMgmtCtrlGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U8  point,
    OUT MSD_U8  *data
);

/*******************************************************************************
* msdSysCPUDestSet
*
* DESCRIPTION:
*       This routine sets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria
*       are mapped to the CPU Destination port, overriding where the frame would
*       normally go. In the case of ARP, the frame will be mapped normally and it
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - the logical port number.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysCPUDestSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum
);

/*******************************************************************************
* msdSysCPUDestGet
*
* DESCRIPTION:
*       This routine gets CPU Destination Port. CPU Destination port indicates the
*       port number on this device where the CPU is connected (either directly or
*       indirectly through another Marvell switch device).
*
*       Many modes of frame processing need to know where the CPU is located.
*       These modes are:
*        1. When IGMP/MLD frame is received and Snooping is enabled
*        2. When the port is configured as a DSA port and it receives a To_CPU frame
*        3. When a Rsvd2CPU frame enters the port
*        4. When the port's SA Filtering mode is Drop to CPU
*        5. When any of the port's Policy Options trap the frame to the CPU
*        6. When the ingressing frame is an ARP and ARP mirroring is enabled in the
*           device
*
*       In all cases, except for ARP, the frames that meet the enabled criteria
*       are mapped to the CPU Destination port, overriding where the frame would
*       normally go. In the case of ARP, the frame will be mapped normally and it
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring
*       will occur and ingressing To_CPU frames will be discarded.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       portNum  - the logical port number.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysCPUDestGet
(
    IN  MSD_U8    devNum,
    OUT MSD_LPORT *portNum
);

/*******************************************************************************
* msdSysIngressMonitorDestSet
*
* DESCRIPTION:
*       This routine sets Ingress Monitor Destination Port. Frames that are
*       targeted toward an Ingress Monitor Destination go out the port number
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Ingress Monitor type, and frames received on a Network port that
*       is enabled to be the Ingress Monitor Source Port.
*       If the Ingress Monitor Destination Port resides in this device these bits
*       should point to the Network port where these frames are to egress. If the
*       Ingress Monitor Destination Port resides in another device these bits
*       should point to the Marvell Tag port in this device that is used to get
*       to the device that contains the Ingress Monitor Destination Port.
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - the logical port number.when port = 0x1F,IMD is disabled.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysIngressMonitorDestSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum
);

/*******************************************************************************
* msdSysIngressMonitorDestGet
*
* DESCRIPTION:
*       This routine gets Ingress Monitor Destination Port.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       portNum  - the logical port number.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysIngressMonitorDestGet
(
    IN  MSD_U8    devNum,
    OUT MSD_LPORT *portNum
);

/*******************************************************************************
* msdSysEgressMonitorDestSet
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
*       devNum   - physical device number
*       portNum  - the logical port number.If port = 0x1F, EMD is disabled
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysEgressMonitorDestSet
(
    IN MSD_U8     devNum,
    IN MSD_LPORT  portNum
);

/*******************************************************************************
* msdSysEgressMonitorDestGet
*
* DESCRIPTION:
*       This routine gets Egress Monitor Destination Port.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       portNum  - the logical port number.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysEgressMonitorDestGet
(
    IN  MSD_U8    devNum,
    OUT MSD_LPORT *portNum
);
/*******************************************************************************
* msdSysRsvd2CpuEnablesSet
*
* DESCRIPTION:
*       This routine enable or disable trap reserved DA (01:80:C2:00:00:0x) to
*       CPU port.  When the Rsvd2Cpu(gsysSetRsvd2Cpu) is set to a one, the 16
*       reserved multicast DA addresses, whose bit in this register are also
*       set to a one, are treadted as MGMT frames. All the reserved DA's take
*       the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*		If the tested bit in this register is cleared to a zero, the frame will
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       devNum - physical device number
*       enBits - bit vector of enabled Reserved Multicast.
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysRsvd2CpuEnablesSet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  enBits
);

/*******************************************************************************
* msdSysRsvd2CpuEnablesGet
*
* DESCRIPTION:
*       Gets the reserved DA enable bits. When the Rsvd2Cpu(gsysSetRsvd2Cpu) is
*       set to a one, the 16 reserved multicast DA addresses, whose bit in this
*       register are also set to a one, are treadted as MGMT frames. All the
*       reserved DA's take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of
*       this register is tested. When x = 0x2, bit 2 of this field is tested and
*		so on.
*       If the tested bit in this register is cleared to a zero, the frame will
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       enBits - bit vector of enabled Reserved Multicast.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysRsvd2CpuEnablesGet
(
    IN  MSD_U8  devNum,
    OUT MSD_U16 *enBits
);

/*******************************************************************************
* msdSysMGMTPriSet
*
* DESCRIPTION:
*       This routine sets the priority for Rsvd2CPU frames.These bits are used
*		as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       devNum - physical device number
*       pri    - PRI[2:0] bits (should be less than 8)
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
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysMGMTPriSet
(
    IN MSD_U8  devNum,
    IN MSD_U16 pri
);

/*******************************************************************************
* gsysGetMGMTPri
*
* DESCRIPTION:
*       This routine gets the priority for Rsvd2CPU frames. These bits are used
*		as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       pri    - PRI[2:0] bits (should be less than 8)
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysMGMTPriGet
(
    IN  MSD_U8  devNum,
    OUT MSD_U16 *pri
);
/*******************************************************************************
* msdPortSmiSetupSet
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
MSD_STATUS msdPortSmiSetupSet
(
IN  MSD_U8  devNum,
IN MSD_LPORT  portNum,
IN MSD_U16		  smiAddr
);

/*******************************************************************************
* msdPortSmiSetupGet
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
MSD_STATUS msdPortSmiSetupGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT		portNum,
OUT MSD_U16			*smiAddr
);
/*******************************************************************************
* msdGlobal1RegDump
*
* DESCRIPTION:
*       This routine sets read 32 global 1 registers and print them out
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdGlobal1RegDump
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
* msdGlobal2RegDump
*
* DESCRIPTION:
*       This routine sets read 32 global 2 registers and print them out
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdGlobal2RegDump
(
	IN  MSD_U8  devNum
);

/*******************************************************************************
* msdSysRMUModeSet
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
MSD_STATUS msdSysRMUModeSet
(
IN MSD_U8    devNum,
IN MSD_U16   data
);

/*******************************************************************************
* msdSysRMUModeGet
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
MSD_STATUS msdSysRMUModeGet
(
IN  MSD_U8    devNum,
OUT MSD_U16   *data
);

/*******************************************************************************
* msdQbvWrite
*
* DESCRIPTION:
*       These routine write Qbv registers.
*
* INPUTS:
*       portNum    - the logic port number(0x1F - Qbv global registers)
*       regOffset  - Qbv register offset
*       data       - Qbv data
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
MSD_STATUS msdQbvWrite
(
IN MSD_U8    devNum,
IN MSD_U8    portNum,
IN MSD_U8    regOffset,
IN MSD_U16   data
);

/*******************************************************************************
* msdQbvRead
*
* DESCRIPTION:
*       These routine gets Qbv register value.
*
* INPUTS:
*       portNum    - the logic port number(0x1F - Qbv global registers)
*       regOffset  - Qbv register offset
*
* OUTPUTS:
*       data - Qbv data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdQbvRead
(
IN MSD_U8    devNum,
IN MSD_U8    portNum,
IN MSD_U8    regOffset,
OUT MSD_U16  *data
);

/*******************************************************************************
* msdSysDevIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       devNum - device number
*       data   - interrupt mask, any combination of the following:
*                MSD_WATCH_DOG_INT			0x8000
*                MSD_JAM_LIMIT_INT			0x4000
*                MSD_DUPLEX_MISMATCH_INT	0x2000
*                MSD_WAKE_EVENT_INT			0x1000
*                MSD_PHY_INT_11 			0x800
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
MSD_STATUS msdSysDevIntEnableSet
(
IN MSD_U8    devNum,
IN MSD_U16   data
);

/*******************************************************************************
* msdSysDevIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       data    - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysDevIntEnableGet
(
IN MSD_U8    devNum,
OUT MSD_U16  *data
);

/*******************************************************************************
* msdSysDevIntStatusGet
*
* DESCRIPTION:
*       These routine gets interrupt source.
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       data     - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysDevIntStatusGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *status
);

/*******************************************************************************
* msdSysActiveIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       devNum  - device number
*       data    - interrupt data, any combination of the following:
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
MSD_STATUS msdSysActiveIntEnableSet
(
IN MSD_U8    devNum,
IN MSD_U16   data
);

/*******************************************************************************
* msdSysActiveIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       data     - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysActiveIntEnableGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *data
);

/*******************************************************************************
* msdSysActiveIntStatusGet
*
* DESCRIPTION:
*       This routine reads an hardware driven event status.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       data      - interrupt status
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSysActiveIntStatusGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *status
);


#ifdef __cplusplus
}
#endif

#endif /* __msdSysCtrl_h */
