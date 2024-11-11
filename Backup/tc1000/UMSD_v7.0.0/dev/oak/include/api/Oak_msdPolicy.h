/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Oak_msdPolicy.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell policy functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Oak_msdPolicy_h
#define Oak_msdPolicy_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Oak_gpolicySetMgmtCtrl
*
* DESCRIPTION:
*       Sets policy and mgmt control data.
*
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x0F: Polcy Trap/Mirror/Discard Policy Selections
*              0x10 to 0x1F: Policy Configurations
*              0x20 to 0x2F: Reserved DA's to CPU & Alt CPU Enables
*              0x30 to 0x3F: Trap/Mirror/CPU Port Settings
*       data  - Octet Data of the Policy Control Register referenced by the
*               by the point above.
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
MSD_STATUS Oak_gpolicySetMgmtCtrl
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8		pointer,
	IN MSD_U8		data
);

/*******************************************************************************
* Oak_gpolicyGetMgmtCtrl
*
* DESCRIPTION:
*       Gets policy and mgmt control data.
*
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x0F: Polcy Trap/Mirror/Discard Policy Selections
*              0x10 to 0x1F: Policy Configurations
*              0x20 to 0x2F: Reserved DA's to CPU & Alt CPU Enables
*              0x30 to 0x3F: Trap/Mirror/CPU Port Settings
*
* OUTPUTS:
*       data  - Octet Data of the Policy Control Register referenced by the
*               by the point above.
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
MSD_STATUS Oak_gpolicyGetMgmtCtrl
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_U8		pointer,
OUT MSD_U8		*data
);
/*******************************************************************************
* Oak_gpolicySetCpuDest
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
*		 7. When any of the port¡¯s Miss Traps occur that are enabled
*
*       In all cases, except for ARP, the frames that meet the enabled criteria
*       are mapped to the CPU Destination port, overriding where the frame would
*       normally go. In the case of ARP, the frame will be mapped normally and it
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring
*       will occur and ingressing To_CPU frames will be discarded.Use a value of 0x1E
*		to map the frames to the internal CPU.
*
* INPUTS:
*       port	- the logical port number.
*		cpuPort - The CPU destination port.
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
MSD_STATUS Oak_gpolicySetCpuDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_LPORT    cpuPort
);
/*******************************************************************************
* Oak_gpolicyGetCpuDest
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
*		 7. When any of the port¡¯s Miss Traps occur that are enabled
*
*       In all cases, except for ARP, the frames that meet the enabled criteria
*       are mapped to the CPU Destination port, overriding where the frame would
*       normally go. In the case of ARP, the frame will be mapped normally and it
*       will also get copied to this port.
*       Frames that filtered or discarded will not be mapped to the CPU Destination
*       port with the exception of the Rsvd2CPU and DSA Tag cases.
*
*       If CPUDest = 0x1F, the remapped frames will be discarded, no ARP mirroring
*       will occur and ingressing To_CPU frames will be discarded.Use a value of 0x1E
*		to map the frames to the internal CPU.
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*		cpuPort - The CPU destination port.
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
MSD_STATUS Oak_gpolicyGetCpuDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
OUT MSD_LPORT   *cpuPort
);
/*******************************************************************************
* Oak_gpolicySetIgrMonitorDest
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
*       port	- the logical port number.
*		imd		- The ingress monitor destination port.
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
MSD_STATUS Oak_gpolicySetIgrMonitorDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_LPORT    imd
);
/*******************************************************************************
* Oak_gpolicyGetIgrMonitorDest
*
* DESCRIPTION:
*       This routine Oak_gets Ingress Monitor Destination Port.
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*		imd		- The ingress monitor destination port.
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
MSD_STATUS Oak_gpolicyGetIgrMonitorDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
OUT MSD_LPORT   *imd
);
/*******************************************************************************
* Oak_gpolicySetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Peridot_gsysSetRsvd2Cpu) is set to a one,
*       the 16 reserved multicast DA addresses, whose bit in this register are
*       also set to a one, are treadted as MGMT frames. All the reserved DA's
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       port	- the logical port number.
*       enBits  - bit vector of enabled Reserved Multicast.
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
MSD_STATUS Oak_gpolicySetRsvd2CpuEnables
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_U16		enBits
);
/*******************************************************************************
* Oak_gpolicyGetRsvd2CpuEnables
*
* DESCRIPTION:
*       Reserved DA Enables. When the Rsvd2Cpu(Peridot_gsysSetRsvd2Cpu) is set to a one,
*       the 16 reserved multicast DA addresses, whose bit in this register are
*       also set to a one, are treadted as MGMT frames. All the reserved DA's
*       take the form 01:80:C2:00:00:0x. When x = 0x0, bit 0 of this register is
*       tested. When x = 0x2, bit 2 of this field is tested and so on.
*       If the tested bit in this register is cleared to a zero, the frame will
*       be treated as a normal (non-MGMT) frame.
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*       enBits  - bit vector of enabled Reserved Multicast.
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
MSD_STATUS Oak_gpolicyGetRsvd2CpuEnables
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	port,
OUT MSD_U16     *enBits
);
/*******************************************************************************
* Oak_gpolicySetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       port	- the logical port number.
*       pri		- PRI[2:0] bits (should be less than 8)
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
MSD_STATUS Oak_gpolicySetMGMTPri
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_U16		pri
);
/*******************************************************************************
* Oak_gpolicyGetMGMTPri
*
* DESCRIPTION:
*       These bits are used as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*       pri		- PRI[2:0] bits (should be less than 8)
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
MSD_STATUS Oak_gpolicyGetMGMTPri
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	port,
OUT MSD_U16     *pri
);
/*******************************************************************************
* Oak_gpolicySetRouteDest
*
* DESCRIPTION:
*       This routine sets Route Destination Port.
*		Frames that ingress a port that trigger a Route policy trap or mirror are
*		mapped (copied if action is a mirror) to this port as long as the frame is not
*		filtered or discarded. This function is disabled when this register is set to
*		0x1F. The RouteDest should point to the port that directs these frames to the
*		CPU that will process these frames. This target port should be a DSA Tag
*		port so the frames will egress with a To_CPU DSA Tag with a CPU Code of
*		Route (0x6).
*
* INPUTS:
*       port	- the logical port number.
*		routePort - The Route destination port.
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
MSD_STATUS Oak_gpolicySetRouteDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_LPORT    routePort
);
/*******************************************************************************
* Oak_gpolicyGetRouteDest
*
* DESCRIPTION:
*       This routine gets Route Destination Port.
*		Frames that ingress a port that trigger a Route policy trap or mirror are
*		mapped (copied if action is a mirror) to this port as long as the frame is not
*		filtered or discarded. This function is disabled when this register is set to
*		0x1F. The RouteDest should point to the port that directs these frames to the
*		CPU that will process these frames. This target port should be a DSA Tag
*		port so the frames will egress with a To_CPU DSA Tag with a CPU Code of
*		Route (0x6).
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*		routePort - The Route destination port.
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
MSD_STATUS Oak_gpolicyGetRouteDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
OUT MSD_LPORT   *routePort
);
/*******************************************************************************
* Oak_gpolicySetMirrorDest
*
* DESCRIPTION:
*       This routine sets Mirror Destination Port.
*       Frames that ingress a port that trigger a policy mirror are mapped (copied) to
*       this port as long as the frame is not filtered or discarded. This function is
*       disabled when this register is set to 0x1F. The MirrorDest should point to the
*       port that directs these frames to the CPU that will process these frames. This
*       target port should be a DSA Tag port so the frames will egress with a To_CPU
*       DSA Tag with a CPU Code of Policy Mirror.
*
* INPUTS:
*       port	- the logical port number.
*		mirrorPort - The mirror destination port.
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
MSD_STATUS Oak_gpolicySetMirrorDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
IN MSD_LPORT    mirrorPort
);
/*******************************************************************************
* Oak_gpolicyGetMirrorDest
*
* DESCRIPTION:
*       This routine gets Mirror Destination Port.
*       Frames that ingress a port that trigger a policy mirror are mapped (copied) to
*       this port as long as the frame is not filtered or discarded. This function is
*       disabled when this register is set to 0x1F. The MirrorDest should point to the
*       port that directs these frames to the CPU that will process these frames. This
*       target port should be a DSA Tag port so the frames will egress with a To_CPU
*       DSA Tag with a CPU Code of Policy Mirror.
*
* INPUTS:
*       port	- the logical port number.
*
* OUTPUTS:
*		mirrorPort - The mirror destination port.
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
MSD_STATUS Oak_gpolicyGetMirrorDest
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	port,
OUT MSD_LPORT   *mirrorPort
);
/*******************************************************************************
* Oak_gpolicySetIllegalIPAddr
*
* DESCRIPTION:
*       This routine configures route policy action for Illegal IP Address.
*       Illegal IP Addresses are:
*       For IPv4 SIP:
*       127.0.0.0 to 127.255.255.255 (Loopback addr or 127.0.0.0/8)
*       224.0.0.0 to 239.255.255.255 (Multicast ClassD or 224.0.0.0/4)
*       255.255.255.255 (Limited Broadcast addr)
*       For IPv4 DIP:
*       0.0.0.0 (Unspecified addr)
*       127.0.0.0 to 127.255.255.255 (Loopback addr or 127.0.0.0/8)
*       For IPv6 SIP:
*       0.0.0.0.0.0.0.1 (Loopback addr or ::1/128)
*       FF00.0.0.0.0.0.0.0 to all ones (Multicast addr or FF::/8)
*       For IPv6 DIP:
*       0.0.0.0.0.0.0.0 (Unspecified addr or ::/128)
*       0.0.0.0.0.0.0.1 (Loopback addr or ::1/128)
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetIllegalIPAddr
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetIllegalIPAddr
*
* DESCRIPTION:
*       This routine gets route policy action for Illegal IP Address.
*       Illegal IP Addresses are:
*       For IPv4 SIP:
*       127.0.0.0 to 127.255.255.255 (Loopback addr or 127.0.0.0/8)
*       224.0.0.0 to 239.255.255.255 (Multicast ClassD or 224.0.0.0/4)
*       255.255.255.255 (Limited Broadcast addr)
*       For IPv4 DIP:
*       0.0.0.0 (Unspecified addr)
*       127.0.0.0 to 127.255.255.255 (Loopback addr or 127.0.0.0/8)
*       For IPv6 SIP:
*       0.0.0.0.0.0.0.1 (Loopback addr or ::1/128)
*       FF00.0.0.0.0.0.0.0 to all ones (Multicast addr or FF::/8)
*       For IPv6 DIP:
*       0.0.0.0.0.0.0.0 (Unspecified addr or ::/128)
*       0.0.0.0.0.0.0.1 (Loopback addr or ::1/128)
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetIllegalIPAddr
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetDipDaMismatch
*
* DESCRIPTION:
*       This routine configures route policy action for DIP/DA mismatch. DIP/DA mismatch triggers are:
*       For IPv4:
*       If DA[40] = 0 and DIP[31:28] = 0xE
*       If DA[40] = 1 and DA <> Broadcast and:
*       DA[47:24] <> 01:00:5E or
*       DA[23] <> 0 or
*       DA[22:0] <> DIP[22:0] or
*       DIP[31:28] <> 0xE
*       For IPv6:
*       If DA[40] = 0 and DIP[127:120] = 0xFF
*       If DA[40] = 1 and DA <> Broadcast and:
*       DA[47:32] <> 33:33 or
*       DA[31:0] <> DIP[31:0] or
*       DIP[127:120] <> 0xFF
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetDipDaMismatch
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetDipDaMismatch
*
* DESCRIPTION:
*       This routine gets route policy action for DIP/DA mismatch. DIP/DA mismatch triggers are:
*       For IPv4:
*       If DA[40] = 0 and DIP[31:28] = 0xE
*       If DA[40] = 1 and DA <> Broadcast and:
*       DA[47:24] <> 01:00:5E or
*       DA[23] <> 0 or
*       DA[22:0] <> DIP[22:0] or
*       DIP[31:28] <> 0xE
*       For IPv6:
*       If DA[40] = 0 and DIP[127:120] = 0xFF
*       If DA[40] = 1 and DA <> Broadcast and:
*       DA[47:32] <> 33:33 or
*       DA[31:0] <> DIP[31:0] or
*       DIP[127:120] <> 0xFF
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetDipDaMismatch
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetZeroSIP
*
* DESCRIPTION:
*       This routine configures route policy action for Zero SIP.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetZeroSIP
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetZeroSIP
*
* DESCRIPTION:
*       This routine gets route policy action for ZeroSIP.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetZeroSIP
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetMcTtlExceeded
*
* DESCRIPTION:
*       This routine configures route policy action for Zero SIP.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetMcTtlExceeded
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetMcTtlExceeded
*
* DESCRIPTION:
*       This routine gets route policy action for ZeroSIP.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetMcTtlExceeded
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetUcTtlExceeded
*
* DESCRIPTION:
*       This routine configures route policy action for unicast TTL/Hop-Limit Exceed.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetUcTtlExceeded
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetUcTtlExceeded
*
* DESCRIPTION:
*       This routine gets route policy action for unicast TTL/Hop-Limit Exceed.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetUcTtlExceeded
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gprtSetPortTtlThreshold
*
* DESCRIPTION:
*       This routine sets TTL/Hop-Limit Threshold.
*
* INPUTS:
*       portNum	- the logical port number.
*		ttlThreshold  - Enumeration of the route policy TTL Threshold.
*			MSD_POLICY_TTL_THRESHOLD_1		- Check Packet¡¯s IPv4 TTL or IPv6 Hop-Limit ¡Ü 1.
*			MSD_POLICY_TTL_THRESHOLD_0		- Check Packet¡¯s IPv4 TTL or IPv6 Hop-Limit = 0.
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
MSD_STATUS Oak_gprtSetPortTtlThreshold
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_TTL_THRESHOLD ttlThreshold
);
/*******************************************************************************
* Oak_gprtGetPortTtlThreshold
*
* DESCRIPTION:
*       This routine gets TTL/Hop-Limit Threshold.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		ttlThreshold  - Enumeration of the route policy TTL Threshold.
*			MSD_POLICY_TTL_THRESHOLD_1		- Check Packet¡¯s IPv4 TTL or IPv6 Hop-Limit ¡Ü 1.
*			MSD_POLICY_TTL_THRESHOLD_0		- Check Packet¡¯s IPv4 TTL or IPv6 Hop-Limit = 0.
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
MSD_STATUS Oak_gprtGetPortTtlThreshold
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_TTL_THRESHOLD *ttlThreshold
);
/*******************************************************************************
* Oak_gpolicySetIpHdrErr
*
* DESCRIPTION:
*       This routine configures route policy action for IP Header Error. An IP Header Error occurs
*       when an IPv4 frame¡¯s IP Header or IPv6 frame¡¯s IP Header fails to pass any the following checks:
*       IPv4 Header Checksum
*       IPv4 Header Version = 4
*       IPv4 Header Length is ¡Ý5
*       IPv4 Header Length is ¡ÜTotal Length/4
*       IPv4 Header SIP ¡ÙDIP
*       IPv6 Header Version = 6
*       IPv6 Header SIP ¡ÙDIP
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicySetIpHdrErr
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_ROUTE_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetIpHdrErr
*
* DESCRIPTION:
*       This routine gets route policy action for IP Header Error. An IP Header Error occurs
*       when an IPv4 frame¡¯s IP Header or IPv6 frame¡¯s IP Header fails to pass any the following checks:
*       IPv4 Header Checksum
*       IPv4 Header Version = 4
*       IPv4 Header Length is ¡Ý5
*       IPv4 Header Length is ¡ÜTotal Length/4
*       IPv4 Header SIP ¡ÙDIP
*       IPv6 Header Version = 6
*       IPv6 Header SIP ¡ÙDIP
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the route policy action.
*			MSD_ROUTE_POLICY_ACTION_NORMAL				- Normal frame switching, do nothing.
*			MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC	- For IP Unicast frame (DA MAC is unicast), trap to RouteDest;
*														  for IP multicast frames (DA MAC is multicast), mirror to RouteDest.
*			MSD_ROUTE_POLICY_ACTION_TRAP				- Trap frame to the RouteDest.
*			MSD_ROUTE_POLICY_ACTION_DISCARD				- Discard the frame.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE				- Bridging only, no routing.
*			MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR		- Bridge and Mirror to RouteDest.
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
MSD_STATUS Oak_gpolicyGetIpHdrErr
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_ROUTE_POLICY_ACTION   *action
); 
/*******************************************************************************
* Oak_gpolicySetTCPFinUrgPsh
*
* DESCRIPTION:
*       This routine configures policy action for the port when receiving one TCP frame and
*       frame¡¯s TCP FIN, URG and PSH flags are all equal to one and the TCP sequence number is zero.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPFinUrgPsh
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPFinUrgPsh
*
* DESCRIPTION:
*       This routine gets policy action for the port when receiving the TCP frame and the frame¡¯s
*       TCP FIN, URG and PSH flags are all equal to one and the TCP sequence number is zero.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPFinUrgPsh
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetTCPFlagsSeqNum
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame and its
*       TCP flags are all zero and TCP sequence number is also zero.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPFlagsSeqNum
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPFlagsSeqNum
*
* DESCRIPTION:
*       This routine gets policy action for the port when receiving the TCP frame and the frame¡¯s
*       TCP FIN, URG and PSH flags are all equal to one and the TCP sequence number is zero.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPFlagsSeqNum
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetTCPSYN
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame and its
*       TCP SYN flag is set to a one and TCP Source Port is less than 1024.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPSYN
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPSYN
*
* DESCRIPTION:
*       This routine gets policy action when receiving TCP frame and its
*       TCP SYN flag is set to a one and TCP Source Port is less than 1024.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPSYN
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gpolicySetTCPHSize
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame and its
*       data offset field is less than the configured MinTCPHdrSize.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPHSize
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPHSize
*
* DESCRIPTION:
*       This routine gets policy action when receiving TCP frame and its
*       TCP SYN flag is set to a one and TCP Source Port is less than 1024.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPHSize
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION   *action
);
/*******************************************************************************
* Oak_gprtSetPortTCPMinHdrSize
*
* DESCRIPTION:
*       This routine configures MinTCPHdrSize.
*
* INPUTS:
*       portNum	- the logical port number.
*		minTCPHdrSize  - Minimum TCP Header Size.
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
MSD_STATUS Oak_gprtSetPortTCPMinHdrSize
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_U8		minTCPHdrSize
);
/*******************************************************************************
* Oak_gprtGetPortTCPMinHdrSize
*
* DESCRIPTION:
*       This routine gets MinTCPHdrSize.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		minTCPHdrSize  - Minimum TCP Header Size.
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
MSD_STATUS Oak_gprtGetPortTCPMinHdrSize
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_U8		*minTCPHdrSize
);

/*******************************************************************************
* Oak_gpolicySetICMPSize
*
* DESCRIPTION:
*       This routine configures  policy action when receiving ICMP ping packet size exceeds
*		the configured MaxICMPSize.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetICMPSize
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetICMPSize
*
* DESCRIPTION:
*       This routine gets policy action when receiving ICMP ping packet size exceeds
*		the configured MaxICMPSize.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetICMPSize
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gprtSetPortICMPMaxSize
*
* DESCRIPTION:
*       This routine configures MaxICMPSize.
*
* INPUTS:
*       portNum	- the logical port number.
*		maxICMPSize  - Maximum ICMP Ping Packet Payload Size.
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
MSD_STATUS Oak_gprtSetPortICMPMaxSize
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_U16		maxICMPSize
);
/*******************************************************************************
* Oak_gprtGetPortICMPMaxSize
*
* DESCRIPTION:
*       This routine gets Max ICMP Size.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		maxICMPSize  - Maximum ICMP Ping Packet Payload Size.
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
MSD_STATUS Oak_gprtGetPortICMPMaxSize
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_U16		*maxICMPSize
);
/*******************************************************************************
* Oak_gpolicySetUDPPort
*
* DESCRIPTION:
*       This routine configures policy action when receiving UDP frame with its
*		Source Port equal to its Destination Port.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetUDPPort
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetUDPPort
*
* DESCRIPTION:
*       This routine gets policy action when receiving UDP frame with its
*		Source Port equal to its Destination Port.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetUDPPort
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetTCPPort
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame with its
*		Source Port equal to its Destination Port.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPPort
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPPort
*
* DESCRIPTION:
*       This routine gets policy action when receiving TCP frame with its
*		Source Port equal to its Destination Port.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPPort
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetTCPSynFin
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame and its
*		TCP SYN and FIN flag is set to a one.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame..
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
MSD_STATUS Oak_gpolicySetTCPSynFin
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPSynFin
*
* DESCRIPTION:
*       This routine get policy action when receiving TCP frame and its
*		TCP SYN and FIN flag is set to a one.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPSynFin
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetTCPFrag
*
* DESCRIPTION:
*       This routine configures policy action when receiving TCP frame with
*		an IP fragment offset equal to 1.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetTCPFrag
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetTCPFrag
*
* DESCRIPTION:
*       This routine get policy action when receiving TCP frame and its
*		TCP SYN and FIN flag is set to a one.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the Dos policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetTCPFrag
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetNearNonTPMR
*
* DESCRIPTION:
*       This routine configures policy action when the ingressing frame is
*		not E-Tagged, but contains a DA of 01:80:C2:00:00:03.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetNearNonTPMR
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetNearNonTPMR
*
* DESCRIPTION:
*       This routine gets policy action when the ingressing frame is
*		not E-Tagged, but contains a DA of 01:80:C2:00:00:03.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetNearNonTPMR
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetETag
*
* DESCRIPTION:
*       This routine configures policy action when the ingressing frame contains an E-Tagged.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetETag
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetETag
*
* DESCRIPTION:
*       This routine gets policy action when the ingressing frame is E-Tagged.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetETag
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);
/*******************************************************************************
* Oak_gpolicySetNonETag
*
* DESCRIPTION:
*       This routine configures policy action when the ingressing frame does NOT contain E-Tagged.
*
* INPUTS:
*       portNum	- the logical port number.
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicySetNonETag
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
);
/*******************************************************************************
* Oak_gpolicyGetNonETag
*
* DESCRIPTION:
*       This routine gets policy action when the ingressing frame is E-Tagged.
*
* INPUTS:
*       portNum	- the logical port number.
*
* OUTPUTS:
*		action  - Enumeration of the PE policy action.
*			MSD_POLICY_ACTION_NORMAL    - Normal frame switching, do nothing.
*			MSD_POLICY_ACTION_MIRROR	- Mirror frame to MirrorDest.
*			MSD_POLICY_ACTION_TRAP		- Trap frame to the CPUDest.
*			MSD_POLICY_ACTION_DISCARD	- Discard the frame.
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
MSD_STATUS Oak_gpolicyGetNonETag
(
IN  MSD_QD_DEV	*dev,
IN  MSD_LPORT	portNum,
OUT MSD_POLICY_ACTION *action
);

/*******************************************************************************
* Oak_gpolicyDump
*
* DESCRIPTION:
*       This routine gets policy registers.
*
* INPUTS:
*       dev	- the pointer of device structure.
*
* OUTPUTS:
*		None.
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
MSD_STATUS Oak_gpolicyDump
(
IN  MSD_QD_DEV	*dev
);

#ifdef __cplusplus
}
#endif

#endif /* __Oak_msdPolicy_h */
