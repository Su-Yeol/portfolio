/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdSysCtrl.c
*
* DESCRIPTION:
*       API definitions for System Control
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdSysCtrl.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

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
*       en - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC(dev, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC(dev, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdSysMonitorMgmtCtrlSet
*
* DESCRIPTION:
*       Sets Monitor and mgmt control data.
*
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
*           The Pointer bits are used to access the Index registers as follows:
*              0x00 to 0x01: Rsvd2Cpu Enables for 01:C2:80:00:00:0x
*              0x02 to 0x03: Rsvd2Cpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x05: Rsvd2AltCpu Enables for 01:C2:80:00:00:0x
*              0x06 to 0x07: Rsvd2AltCpu Enables for 01:C2:80:00:00:2x
*              0x04 to 0x1F: Reserved for future use.
*              0x20 to 0x2F: Mirror Destination Port Settings
*              0x30 to 0x3F: Trap Destination Port Settings
*       data  - Monitor and mgmt Control data written to the register
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl(dev, point, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdSysMonitorMgmtCtrlGet
*
* DESCRIPTION:
*       Gets Monitor and mgmt control data.
*
* INPUTS:
*       point - Pointer to the Monitor and mgmt control register.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl(dev, point, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       port  - the logical port number.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
*       port  - the logical port number.when port = 0x1F,IMD is disabled.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdSysIngressMonitorDestGet
*
* DESCRIPTION:
*       This routine gets Ingress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
*       port  - the logical port number.If port = 0x1F, EMD is disabled
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdSysEgressMonitorDestGet
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables(dev, enBits);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
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
*       None.
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables(dev, enBits);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* msdSysMGMTPriSet
*
* DESCRIPTION:
*       This routine sets the priority for Rsvd2CPU frames.These bits are used 
*		as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri(dev, pri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}
/*******************************************************************************
* gsysGetMGMTPri
*
* DESCRIPTION:
*       This routine gets the priority for Rsvd2CPU frames. These bits are used 
*		as the PRI[2:0] bits on Rsvd2CPU frames.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       pri - PRI[2:0] bits (should be less than 8)
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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri(dev, pri);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
IN  MSD_U8    devNum, 
IN MSD_LPORT  portNum,
IN MSD_U16	  smiAddr
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup(dev, portNum, smiAddr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup(dev, portNum, smiAddr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdQbvWrite
*
* DESCRIPTION:
*       These routine write QBV registers.
*
* INPUTS:
*       portNum    - port number(0x1F - Qbv global registers)
*       regOffset  - QBV register offset
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
MSD_STATUS msdQbvWrite
(
    IN MSD_U8    devNum,
    IN MSD_U8    portNum,
    IN MSD_U8    regOffset,
    IN MSD_U16   data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite(dev, portNum, regOffset, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdQbvRead
*
* DESCRIPTION:
*       These routine gets QBV register value.
*
* INPUTS:
*       portNum    - port number (0x1F - Qbv global registers)
*       regOffset  - QBV register offset
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
MSD_STATUS msdQbvRead
(
    IN MSD_U8    devNum,
    IN MSD_U8    portNum,
    IN MSD_U8    regOffset,
    OUT MSD_U16  *data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead != NULL)
		{
			retVal = dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead(dev, portNum, regOffset, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API configure G2 offset 0x1 Interrupt Mask Register.
*
*******************************************************************************/
MSD_STATUS msdSysDevIntEnableSet
(
IN MSD_U8    devNum,
IN MSD_U16   data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gdevIntEnableSet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gdevIntEnableSet(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API gets G2 offset 0x1 Interrupt Mask Register.
*
*******************************************************************************/
MSD_STATUS msdSysDevIntEnableGet
(
IN MSD_U8    devNum,
OUT MSD_U16  *data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gdevIntEnableGet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gdevIntEnableGet(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API gets G2 offset 0x0 Interrupt Source Register.
*
*******************************************************************************/
MSD_STATUS msdSysDevIntStatusGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *status
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gdevIntStatusGet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gdevIntStatusGet(dev, status);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API configure G1 offset 0x4 Switch Global Control Register.
*
*******************************************************************************/
MSD_STATUS msdSysActiveIntEnableSet
(
IN MSD_U8    devNum,
IN MSD_U16   data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gactiveIntEnableSet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gactiveIntEnableSet(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API gets G1 offset 0x4 Switch Global Control Register.
*
*******************************************************************************/
MSD_STATUS msdSysActiveIntEnableGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *data
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gactiveIntEnableGet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gactiveIntEnableGet(dev, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

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
*       This API gets G1 offset 0x0 Switch Global Status Register.
*
*******************************************************************************/
MSD_STATUS msdSysActiveIntStatusGet
(
IN MSD_U8     devNum,
OUT MSD_U16   *status
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.INTObj.gactiveIntStatusGet != NULL)
		{
			retVal = dev->SwitchDevObj.INTObj.gactiveIntStatusGet(dev, status);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}