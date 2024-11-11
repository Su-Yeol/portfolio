/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Oak_msdPolicy.c
*
* DESCRIPTION:
*       API definitions for Policy
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdPolicy.h>
#include <oak/include/api/Oak_msdApiInternal.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

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
)
{
	MSD_STATUS    retVal = MSD_OK;         /* Functions return value.      */
	MSD_U8        hwPort;                  /* the physical port number     */
	MSD_U8		  phyAddr;
	MSD_U16	      tmpData;
	MSD_U16       count = (MSD_U16)0x10;
   
	MSD_DBG_INFO(("Oak_gpolicySetMgmtCtrl Called.\n"));

	if (pointer > (MSD_U8)63)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (pointer: %u). It should be within [0, 63].\n", pointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate LPORT to hardware port */
		hwPort = MSD_LPORT_2_PORT(port);
		phyAddr = OAK_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

			tmpData = (MSD_U16)1;
			while (tmpData == (MSD_U16)1)
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, OAK_POLICY_CONTROL, (MSD_U8)15, (MSD_U8)1, &tmpData);
				if (retVal != MSD_OK)
				{
					retVal = MSD_FAIL;
					break;
				}
				if (count == 0U)
				{
					tmpData = 0;
				}

				count -= 1U;
			}

			if (retVal != MSD_FAIL)
			{
				tmpData = (MSD_U16)((MSD_U16)((MSD_U16)2 << 14) | (MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x3F) << 8) | (MSD_U16)data);
				retVal = msdSetAnyReg(dev->devNum, phyAddr, OAK_POLICY_CONTROL, tmpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_INFO(("Failed to set OAK_POLICY_CONTROL.\n"));
				}
			}

			msdSemGive(dev->devNum, dev->tblRegsSem);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetMgmtCtrl Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS    retVal = MSD_OK;        /* Functions return value.      */
	MSD_U8        hwPort;                 /* the physical port number     */
	MSD_U8		  phyAddr;
	MSD_U16       count, tmpData;

	MSD_DBG_INFO(("Oak_gpolicyGetMgmtCtrl Called.\n"));

	if (pointer > (MSD_U8)63)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (pointer: %u). It should be within [0, 63].\n", pointer));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate LPORT to hardware port */
		hwPort = MSD_LPORT_2_PORT(port);
		phyAddr = OAK_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

			count = (MSD_U16)5;
			tmpData = (MSD_U16)1;
			while (tmpData == (MSD_U16)1)
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, OAK_POLICY_CONTROL, (MSD_U8)15, (MSD_U8)1, &tmpData);
				if (retVal != MSD_OK)
				{
					retVal = MSD_FAIL;
					break;
				}
				if (count == 0U)
				{
					tmpData = 0;
				}

				count -= 1U;
			}

			if (retVal != MSD_FAIL)
			{
				tmpData = (MSD_U16)((MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x3F) << 8) | (MSD_U16)0);
				retVal = msdSetAnyReg(dev->devNum, phyAddr, OAK_POLICY_CONTROL, tmpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_INFO(("Failed set OAK_POLICY_CONTROL.\n"));
				}
				else
				{
					retVal = msdGetAnyReg(dev->devNum, phyAddr, OAK_POLICY_CONTROL, &tmpData);
					if (retVal != MSD_OK)
					{
						retVal = MSD_FAIL;
					}
					else
					{
						*data = (MSD_U8)(tmpData & (MSD_U16)0xff);
					}
				}
			}

			msdSemGive(dev->devNum, dev->tblRegsSem);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetMgmtCtrl Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal = MSD_OK;     /* Functions return value.      */
	MSD_U8           hwPort;              /* the physical port number     */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetCpuDest Called.\n"));

	/* translate LPORT to hardware port */
	if ((cpuPort == (MSD_U8)0x1F) || (cpuPort == (MSD_U8)0x1E))
	{
		hwPort = (MSD_U8)cpuPort;
	}
	else
	{
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(cpuPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", cpuPort));
			retVal = MSD_BAD_PARAM;
		}
	}

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x38, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xE0) | hwPort;
			retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x38, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetCpuDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           datac;

	MSD_DBG_INFO(("Oak_gpolicyGetCpuDest Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x38, &datac);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (MSD_U16)((MSD_U16)datac & (MSD_U16)0x1F);

		if ((data == (MSD_U16)0x1F) || (data == (MSD_U16)0x1E))
		{
			*cpuPort = (MSD_LPORT)data;
		}
		else
		{
			*cpuPort = MSD_PORT_2_LPORT((MSD_U8)data);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetCpuDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
	MSD_U8           hwPort;           /* the physical port number     */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetIngressMonitorDest Called.\n"));

	/* translate LPORT to hardware port */
	if ((imd == (MSD_U32)0x1F) || (imd == (MSD_U32)0x1E))
	{
		hwPort = (MSD_U8)imd;
	}
	else
	{
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(imd);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", port));
			retVal = MSD_BAD_PARAM;
		}
	}

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x30, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xE0) | hwPort;
			retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x30, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetIngressMonitorDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           datac;

	MSD_DBG_INFO(("Oak_gpolicyGetIngressMonitorDest Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x30, &datac);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (MSD_U16)((MSD_U16)datac & (MSD_U16)0x1F);

		if ((data == (MSD_U16)0x1F) || (data == (MSD_U16)0x1E))
		{
			*imd = (MSD_LPORT)data;
		}
		else
		{
			*imd = MSD_PORT_2_LPORT((MSD_U8)data);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetIngressMonitorDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */

	MSD_DBG_INFO(("Oak_gpolicySetRsvd2CpuEnables Called.\n"));

	retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x20, (MSD_U8)(enBits & (MSD_U8)0xff));
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x21, (MSD_U8)((enBits >> 8) & (MSD_U8)0xff));
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetRsvd2CpuEnables Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U8        data;
	MSD_U16       dataw;

	MSD_DBG_INFO(("Oak_gpolicyGetIngressMonitorDest Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x20, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*enBits = data;
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x21, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			dataw = data;
			dataw <<= 8;
			*enBits |= dataw & (MSD_U16)0xff00;
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetRsvd2CpuEnables Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gpolicySetMGMTPri Called.\n"));

	if (pri > (MSD_U16)0x7)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (pri: %u). It should be within [0, 7].\n", pri));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x38, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* Set related bit */
			data = (MSD_U8)((data & (MSD_U8)0x1F) | (MSD_U8)(((MSD_U8)pri & (MSD_U8)0x7) << 5));
			retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x38, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetMGMTPri Exit.\n"));
	return retVal;

}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicyGetMGMTPri Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x38, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*pri = (MSD_U16)(((MSD_U16)data & (MSD_U16)0xE0) >> 5);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetMGMTPri Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
	MSD_U8           hwPort;           /* the physical port number     */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetRouteDest Called.\n"));

	/* translate LPORT to hardware port */
	if ((routePort == (MSD_U32)0x1F) || (routePort == (MSD_U32)0x1E))
	{
		hwPort = (MSD_U8)routePort;
	}
	else
	{
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(routePort);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", routePort));
			retVal = MSD_BAD_PARAM;
		}
	}

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x33, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xE0) | hwPort;
			retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x33, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetRouteDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           datac;

	MSD_DBG_INFO(("Oak_gpolicyGetRouteDest Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x33, &datac);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (MSD_U16)((MSD_U16)datac & (MSD_U16)0x1F);

		if ((data == (MSD_U16)0x1F) || (data == (MSD_U16)0x1E))
		{
			*routePort = (MSD_LPORT)data;
		}
		else
		{
			*routePort = MSD_PORT_2_LPORT((MSD_U8)data);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetRouteDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
	MSD_U8           hwPort;           /* the physical port number     */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetMirrorDest Called.\n"));

	/* translate LPORT to hardware port */
	if ((mirrorPort == (MSD_LPORT)0x1F) || (mirrorPort == (MSD_LPORT)0x1E))
	{
		hwPort = (MSD_U8)mirrorPort;
	}
	else
	{
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(mirrorPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", mirrorPort));
			retVal = MSD_BAD_PARAM;
		}
	}

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x32, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xE0) | hwPort;
			retVal = Oak_gpolicySetMgmtCtrl(dev, port, (MSD_U8)0x32, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetMirrorDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8			 datac;

	MSD_DBG_INFO(("Oak_gpolicyGetMirrorDest Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, port, (MSD_U8)0x32, &datac);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (MSD_U16)((MSD_U16)datac & (MSD_U16)0x1F);

		if ((data == (MSD_U16)0x1F) || (data == (MSD_U16)0x1E))
		{
			*mirrorPort = (MSD_LPORT)data;
		}
		else
		{
			*mirrorPort = MSD_PORT_2_LPORT((MSD_U8)data);
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetMirrorDest Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetIllegalIPAddr Called.\n"));
	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0x3F) | (MSD_U8)((MSD_U8)action << 6);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x2, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetIllegalIPAddr Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetIllegalIPAddr Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_ROUTE_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0xC0) >> 6);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetIllegalIPAddr Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetDipDaMismatch Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xCF) | (MSD_U8)((MSD_U8)action << 4);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x2, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetDipDaMismatch Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetDipDaMismatch Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_ROUTE_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x30) >> 4);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetDipDaMismatch Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gpolicySetZeroSIP Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xF3) | (MSD_U8)((MSD_U8)action << 2);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x2, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetZeroSIP Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetZeroSIP Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_ROUTE_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x0C) >> 2);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetZeroSIP Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gpolicySetMcTtlExceeded
*
* DESCRIPTION:
*       This routine configures route policy action for multicast TTL/Hop-Limit Exceed.
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
)
{
	MSD_STATUS       retVal = MSD_OK;         /* Functions return value.      */
	MSD_U8			 data;
	MSD_U8			 action_tmp = 0;

	MSD_DBG_INFO(("Oak_gpolicySetMcTtlExceeded Called.\n"));
	
	if (action > 5)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 4].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (action == MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR)
		{
			action_tmp = (MSD_U8)1;
		}
		else if (action == MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC)
		{
			MSD_DBG_INFO(("MSD_NOT_SUPPORTED (action: %u).\n", action));
			retVal = MSD_NOT_SUPPORTED;
		}
		else
		{
			action_tmp = (MSD_U8)action;
		}

		if (retVal != MSD_NOT_SUPPORTED)
		{
			retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				data = (data & (MSD_U8)0x8f) | (MSD_U8)((MSD_U8)action_tmp << 4);
				retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x3, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetMcTtlExceeded Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gpolicyGetMcTtlExceeded
*
* DESCRIPTION:
*       This routine gets route policy action for multicast TTL/Hop-Limit Exceed. 
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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data, tempData;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetMcTtlExceeded Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		tempData = (data & (MSD_U8)0x70) >> 4;
		switch (tempData)
		{
		case 0:
			*action = MSD_ROUTE_POLICY_ACTION_NORMAL;
			break;
		case 1U:
			*action = MSD_ROUTE_POLICY_ACTION_BRIDGE_MIRROR;
			break;
		case 2U:
			*action = MSD_ROUTE_POLICY_ACTION_TRAP;
			break;
		case 3U:
			*action = MSD_ROUTE_POLICY_ACTION_DISCARD;
			break;
		case 4U:
			*action = MSD_ROUTE_POLICY_ACTION_BRIDGE;
			break;
		default:
			MSD_DBG_INFO(("getMcTtlExceeded returned register value : 0x%x\n", tempData));
			retVal = MSD_NOT_SUPPORTED;
			break;
		}
	}

	MSD_DBG_INFO(("Oak_gpolicyGetMcTtlExceeded Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetUcTtlExceeded Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (action == MSD_ROUTE_POLICY_ACTION_MIRROR_MC_TRAP_UC)
		{
			MSD_DBG_INFO(("MSD_NOT_SUPPORTED (action: %u).\n", action));
			retVal = MSD_NOT_SUPPORTED;
		}
		else
		{
			retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				data = (data & (MSD_U8)0xf3) | (MSD_U8)((MSD_U8)action << 2);
				retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x3, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetUcTtlExceeded Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetUcTtlExceeded Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_ROUTE_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x0C) >> 2);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetUcTtlExceeded Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gprtSetPortTtlThreshold Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (data & (MSD_U8)0x7f) | (MSD_U8)(((MSD_U8)ttlThreshold & (MSD_U8)0x1) << 7);
		retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x3, (MSD_U8)data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gprtSetPortTtlThreshold Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gprtGetPortTtlThreshold Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*ttlThreshold = (MSD_ROUTE_POLICY_TTL_THRESHOLD)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x80) >> 7);
	}

	MSD_DBG_INFO(("Oak_gprtGetPortTtlThreshold Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetIpHdrErr Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xFC) | (MSD_U8)action;
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x3, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetIpHdrErr Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetIpHdrErr Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x3, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_ROUTE_POLICY_ACTION)(MSD_U8)(data & (MSD_U8)0x03);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetIpHdrErr Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPFinUrgPsh Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0x3f) | (MSD_U8)((MSD_U8)action << 6);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x4, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPFinUrgPsh Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFinUrgPsh Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0xC0) >> 6);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFinUrgPsh Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPFlagsSeqNum Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xCF) | (MSD_U8)((MSD_U8)action << 4);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x4, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPFlagsSeqNum Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFlagsSeqNum Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x30) >> 4);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFlagsSeqNum Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPSYN Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xF3) | (MSD_U8)((MSD_U8)action << 2);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x4, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPSYN Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPSYN Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x0C) >> 2);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPSYN Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPHSize Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xFC) | (MSD_U8)action;
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x4, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPHSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPHSize Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)(data & (MSD_U8)0x03);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPHSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gprtSetPortTCPMinHdrSize Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x10, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (data & (MSD_U8)0xF0) | (minTCPHdrSize & (MSD_U8)0xF);
		retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x10, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gprtSetPortTCPMinHdrSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPHSize Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x10, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*minTCPHdrSize = data & (MSD_U8)0xF;
	}

	MSD_DBG_INFO(("Oak_gprtGetPortTCPMinHdrSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetICMPSize Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0x3F) | (MSD_U8)((MSD_U8)action << 6);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x5, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetICMPSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetICMPSize Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0xC0) >> 6);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetICMPSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gprtSetPortICMPMaxSize Called.\n"));

	data = (MSD_U8)(maxICMPSize & (MSD_U16)0xff);
	retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x12, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = (MSD_U8)((maxICMPSize & (MSD_U16)0xff00) >> 8);
		retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x13, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gprtSetPortICMPMaxSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS      retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */
	MSD_U8          data1;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gprtGetPortICMPMaxSize Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x12, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x13, &data1);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			*maxICMPSize = (MSD_U16)((MSD_U16)((MSD_U16)data1 << 8) | (MSD_U16)data);
		}
	}

	MSD_DBG_INFO(("Oak_gprtGetPortICMPMaxSize Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gpolicySetUDPPort Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xCF) | (MSD_U8)((MSD_U8)action << 4);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x5, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetUDPPort Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetUDPPort Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x30) >> 4);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetUDPPort Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPPort Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xF3) | (MSD_U8)((MSD_U8)action << 2);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x5, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPPort Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPPort Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x0C) >> 2);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPPort Exit.\n"));
	return retVal;
}

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
MSD_STATUS Oak_gpolicySetTCPSynFin
(
IN MSD_QD_DEV	*dev,
IN MSD_LPORT	portNum,
IN MSD_POLICY_ACTION action
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPSynFin Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xFC) | (MSD_U8)action;
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x5, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPSynFin Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPSynFin Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x5, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetTCPSynFin returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)(data & (MSD_U8)0x03);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPPort Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetTCPFrag Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x6, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xFC) | (MSD_U8)action;
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x6, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetTCPFrag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFrag Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x6, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetTCPSynFin returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{

		*action = (MSD_POLICY_ACTION)(MSD_U8)(data & (MSD_U8)0x03);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetTCPFrag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetNearNonTPMR Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0x3F) | (MSD_U8)((MSD_U8)action << 6);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x8, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetNearNonTPMR Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetNearNonTPMR Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetTCPSynFin returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0xC0) >> 6);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetNearNonTPMR Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			 data;

	MSD_DBG_INFO(("Oak_gpolicySetETag Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xF3) | (MSD_U8)((MSD_U8)action << 2);
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x8, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetETag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetETag Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetTCPSynFin returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)((MSD_U8)(data & (MSD_U8)0x0C) >> 2);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetETag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			data;

	MSD_DBG_INFO(("Oak_gpolicySetNonETag Called.\n"));

	if (action > 3)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (action: %u). It should be within [0, 3].\n", action));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = (data & (MSD_U8)0xFC) | (MSD_U8)action;
			retVal = Oak_gpolicySetMgmtCtrl(dev, portNum, (MSD_U8)0x8, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_gpolicySetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

	MSD_DBG_INFO(("Oak_gpolicySetNonETag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           data;           /* The register's read data.    */

	MSD_DBG_INFO(("Oak_gpolicyGetNonETag Called.\n"));

	retVal = Oak_gpolicyGetMgmtCtrl(dev, portNum, (MSD_U8)0x8, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gpolicyGetTCPSynFin returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*action = (MSD_POLICY_ACTION)(MSD_U8)(data & (MSD_U8)0x03);
	}

	MSD_DBG_INFO(("Oak_gpolicyGetNonETag Exit.\n"));
	return retVal;
}

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
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8          tmpdata, port;
	MSD_U8			pointer = 0;
	MSD_U8			data[MSD_MAX_SWITCH_PORTS] = {0};

	MSD_DBG_INFO(("Oak_gpolicyDump Called.\n"));

	/* Print out led field*/
	MSG(("\n-------------------------------------------------------------------------------------------------------------------------------------------------\n"));
	MSG(("Policy Field                Port0     Port1     Port2     Port3     Port4     Port5     Port6     Port7     Port8     Port9     Port10     Port11\n"));
	MSG(("-------------------------------------------------------------------------------------------------------------------------------------------------\n"));

	/*policy Basic Policy1 Byte 0, Index: 0x00*/
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("PPPoE Policy                 0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("VBAS Policy                  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("Opt82 Policy                 0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("UDP Policy                   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy Basic Policy1 Byte 1, Index: 0x01*/
	pointer = (MSD_U8)0x1;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("DA Policy                    0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("SA Policy                    0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("VTU Policy                   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("Etype Policy                 0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy Route Policy1 Byte 0, Index: 0x02*/
	pointer = (MSD_U8)0x2;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Illegal IPADR                0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("DipDaMisMatch                0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("ZeroSIP                      0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	/*policy Route Policy1 Byte 1, Index: 0x03*/
	pointer = (MSD_U8)0x3;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("TTL Threshold                0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x80) >> 7, (data[1] & (MSD_U8)0x80) >> 7, (data[2] & (MSD_U8)0x80) >> 7, (data[3] & (MSD_U8)0x80) >> 7, (data[4] & (MSD_U8)0x80) >> 7, \
		(data[5] & (MSD_U8)0x80) >> 7, (data[6] & (MSD_U8)0x80) >> 7, (data[7] & (MSD_U8)0x80) >> 7, (data[8] & (MSD_U8)0x80) >> 7, (data[9] & (MSD_U8)0x80) >> 7, \
		(data[10] & (MSD_U8)0x80) >> 7, (data[11] & (MSD_U8)0x80) >> 7));

	MSG(("McTtl Exceeded               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x70) >> 4, (data[1] & (MSD_U8)0x70) >> 4, (data[2] & (MSD_U8)0x70) >> 4, (data[3] & (MSD_U8)0x70) >> 4, (data[4] & (MSD_U8)0x70) >> 4, \
		(data[5] & (MSD_U8)0x70) >> 4, (data[6] & (MSD_U8)0x70) >> 4, (data[7] & (MSD_U8)0x70) >> 4, (data[8] & (MSD_U8)0x70) >> 4, (data[9] & (MSD_U8)0x70) >> 4, \
		(data[10] & (MSD_U8)0x70) >> 4, (data[11] & (MSD_U8)0x70) >> 4));

	MSG(("UcTtl Exceeded               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("IpHdrErr                     0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy DoS Attack Policy1 Byte 0, Index: 0x04*/
	pointer = (MSD_U8)0x4;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("TCP FIN Policy               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("TCP Flags Policy             0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("TCP SYN Policy               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("TCP HSize Policy             0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy DoS Attack Policy1 Byte 1, Index: 0x05*/
	pointer = (MSD_U8)0x5;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("ICMP Size Policy             0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("UDP Port Policy              0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("TCP Port Policy              0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("TCPSynFin Policy             0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy DoS Attack Policy1 Byte 2, Index: 0x06*/
	pointer = (MSD_U8)0x6;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("TCP Frag Policy              0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy Port Extender Policy1 Byte 0, Index: 0x08*/
	pointer = (MSD_U8)0x8;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("NearNon TPMR Policy          0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("ETag Policy                  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("Not ETag Policy              0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy Port Extender Policy1 Byte 1, Index: 0x09*/
	pointer = (MSD_U8)0x9;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Ecid Policy                  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC0) >> 6, (data[1] & (MSD_U8)0xC0) >> 6, (data[2] & (MSD_U8)0xC0) >> 6, (data[3] & (MSD_U8)0xC0) >> 6, (data[4] & (MSD_U8)0xC0) >> 6, \
		(data[5] & (MSD_U8)0xC0) >> 6, (data[6] & (MSD_U8)0xC0) >> 6, (data[7] & (MSD_U8)0xC0) >> 6, (data[8] & (MSD_U8)0xC0) >> 6, (data[9] & (MSD_U8)0xC0) >> 6, \
		(data[10] & (MSD_U8)0xC0) >> 6, (data[11] & (MSD_U8)0xC0) >> 6));

	MSG(("IgrEcid Policy               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x30) >> 4, (data[1] & (MSD_U8)0x30) >> 4, (data[2] & (MSD_U8)0x30) >> 4, (data[3] & (MSD_U8)0x30) >> 4, (data[4] & (MSD_U8)0x30) >> 4, \
		(data[5] & (MSD_U8)0x30) >> 4, (data[6] & (MSD_U8)0x30) >> 4, (data[7] & (MSD_U8)0x30) >> 4, (data[8] & (MSD_U8)0x30) >> 4, (data[9] & (MSD_U8)0x30) >> 4, \
		(data[10] & (MSD_U8)0x30) >> 4, (data[11] & (MSD_U8)0x30) >> 4));

	MSG(("EcidMiss Policy              0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xC) >> 2, (data[1] & (MSD_U8)0xC) >> 2, (data[2] & (MSD_U8)0xC) >> 2, (data[3] & (MSD_U8)0xC) >> 2, (data[4] & (MSD_U8)0xC) >> 2, \
		(data[5] & (MSD_U8)0xC) >> 2, (data[6] & (MSD_U8)0xC) >> 2, (data[7] & (MSD_U8)0xC) >> 2, (data[8] & (MSD_U8)0xC) >> 2, (data[9] & (MSD_U8)0xC) >> 2, \
		(data[10] & (MSD_U8)0xC) >> 2, (data[11] & (MSD_U8)0xC) >> 2));

	MSG(("IgrEcid Miss  Policy         0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x3, data[1] & (MSD_U8)0x3, data[2] & (MSD_U8)0x3, data[3] & (MSD_U8)0x3, data[4] & (MSD_U8)0x3, \
		data[5] & (MSD_U8)0x3, data[6] & (MSD_U8)0x3, data[7] & (MSD_U8)0x3, data[8] & (MSD_U8)0x3, data[9] & (MSD_U8)0x3, \
		data[10] & (MSD_U8)0x3, data[11] & (MSD_U8)0x3));

	/*policy Port Extender Policy1 Byte 2, Index: 0x0A*/
	pointer = (MSD_U8)0xA;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("EcidQPri Override            0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x80) >> 7, (data[1] & (MSD_U8)0x80) >> 7, (data[2] & (MSD_U8)0x80) >> 7, (data[3] & (MSD_U8)0x80) >> 7, (data[4] & (MSD_U8)0x80) >> 7, \
		(data[5] & (MSD_U8)0x80) >> 7, (data[6] & (MSD_U8)0x80) >> 7, (data[7] & (MSD_U8)0x80) >> 7, (data[8] & (MSD_U8)0x80) >> 7, (data[9] & (MSD_U8)0x80) >> 7, \
		(data[10] & (MSD_U8)0x80) >> 7, (data[11] & (MSD_U8)0x80) >> 7));

	MSG(("EcidFPri Override            0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x40) >> 6, (data[1] & (MSD_U8)0x40) >> 6, (data[2] & (MSD_U8)0x40) >> 6, (data[3] & (MSD_U8)0x40) >> 6, (data[4] & (MSD_U8)0x40) >> 6, \
		(data[5] & (MSD_U8)0x40) >> 6, (data[6] & (MSD_U8)0x40) >> 6, (data[7] & (MSD_U8)0x40) >> 6, (data[8] & (MSD_U8)0x40) >> 6, (data[9] & (MSD_U8)0x40) >> 6, \
		(data[10] & (MSD_U8)0x40) >> 6, (data[11] & (MSD_U8)0x40) >> 6));

	MSG(("IgrEcidQPri Override         0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x20) >> 5, (data[1] & (MSD_U8)0x20) >> 5, (data[2] & (MSD_U8)0x20) >> 5, (data[3] & (MSD_U8)0x20) >> 5, (data[4] & (MSD_U8)0x20) >> 5, \
		(data[5] & (MSD_U8)0x20) >> 5, (data[6] & (MSD_U8)0x20) >> 5, (data[7] & (MSD_U8)0x20) >> 5, (data[8] & (MSD_U8)0x20) >> 5, (data[9] & (MSD_U8)0x20) >> 5, \
		(data[10] & (MSD_U8)0x20) >> 5, (data[11] & (MSD_U8)0x20) >> 5));

	MSG(("IgrEcidFPri Override         0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x10) >> 4, (data[1] & (MSD_U8)0x10) >> 4, (data[2] & (MSD_U8)0x10) >> 4, (data[3] & (MSD_U8)0x10) >> 4, (data[4] & (MSD_U8)0x10) >> 4, \
		(data[5] & (MSD_U8)0x10) >> 4, (data[6] & (MSD_U8)0x10) >> 4, (data[7] & (MSD_U8)0x10) >> 4, (data[8] & (MSD_U8)0x10) >> 4, (data[9] & (MSD_U8)0x10) >> 4, \
		(data[10] & (MSD_U8)0x10) >> 4, (data[11] & (MSD_U8)0x10) >> 4));

	/*policy DoS Attack Control Byte 0, Index: 0x10*/
	pointer = (MSD_U8)0x10;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("FlagsOnly                    0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x80) >> 7, (data[1] & (MSD_U8)0x80) >> 7, (data[2] & (MSD_U8)0x80) >> 7, (data[3] & (MSD_U8)0x80) >> 7, (data[4] & (MSD_U8)0x80) >> 7, \
		(data[5] & (MSD_U8)0x80) >> 7, (data[6] & (MSD_U8)0x80) >> 7, (data[7] & (MSD_U8)0x80) >> 7, (data[8] & (MSD_U8)0x80) >> 7, (data[9] & (MSD_U8)0x80) >> 7, \
		(data[10] & (MSD_U8)0x80) >> 7, (data[11] & (MSD_U8)0x80) >> 7));

	MSG(("MinTCP HdrSize               0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0xF, data[1] & (MSD_U8)0xF, data[2] & (MSD_U8)0xF, data[3] & (MSD_U8)0xF, data[4] & (MSD_U8)0xF, \
		data[5] & (MSD_U8)0xF, data[6] & (MSD_U8)0xF, data[7] & (MSD_U8)0xF, data[8] & (MSD_U8)0xF, data[9] & (MSD_U8)0xF, \
		data[10] & (MSD_U8)0xF, data[11] & (MSD_U8)0xF));

	/*policy DoS Attack Control Byte 2, Index: 0x12*/
	pointer = (MSD_U8)0x12;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("MaxICMPSize[7:0]             0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy DoS Attack Control Byte 3, Index: 0x13*/
	pointer = (MSD_U8)0x13;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("MaxICMPSize[15:8]            0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2Cpu0 Byte 0, Index: 0x20*/
	pointer = (MSD_U8)0x20;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2Cpu Enables 0x Low      0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2Cpu0 Byte 1, Index: 0x21 */
	pointer = (MSD_U8)0x21;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2Cpu Enables 0x High     0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2Cpu2 Byte 2, Index: 0x24 */
	pointer = (MSD_U8)0x24;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2Cpu Enables 2x Low      0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2Cpu2 Byte 3, Index: 0x25 */
	pointer = (MSD_U8)0x25;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2Cpu Enables 2x High     0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2AltCpu0 Byte 0, Index: 0x28 */
	pointer = (MSD_U8)0x28;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2AltCpu Enables 0x Low   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2AltCpu0 Byte 1, Index: 0x29  */
	pointer = (MSD_U8)0x29;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2AltCpu Enables 0x High  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2AltCpu2 Byte 2, Index: 0x2C */
	pointer = (MSD_U8)0x2C;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2AltCpu Enables 2x Low   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Rsvd2AltCpu2 Byte 3, Index: 0x2D */
	pointer = (MSD_U8)0x2D;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Rsvd2AltCpu Enables 2x High  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0], data[1], data[2], data[3], data[4], \
		data[5], data[6], data[7], data[8], data[9], \
		data[10], data[11]));

	/*policy Ingress Monitor Dest, Index: 0x30  */
	pointer = (MSD_U8)0x30;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Ingress Monitor Dest         0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));

	/*policy Mirror Dest, Index: 0x32 */
	pointer = (MSD_U8)0x32;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("QinQ Mirrors                 0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0x80) >> 7, (data[1] & (MSD_U8)0x80) >> 7, (data[2] & (MSD_U8)0x80) >> 7, (data[3] & (MSD_U8)0x80) >> 7, (data[4] & (MSD_U8)0x80) >> 7, \
		(data[5] & (MSD_U8)0x80) >> 7, (data[6] & (MSD_U8)0x80) >> 7, (data[7] & (MSD_U8)0x80) >> 7, (data[8] & (MSD_U8)0x80) >> 7, (data[9] & (MSD_U8)0x80) >> 7, \
		(data[10] & (MSD_U8)0x80) >> 7, (data[11] & (MSD_U8)0x80) >> 7));

	MSG(("Mirror Dest                  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));

	/*policy Route Dest, Index: 0x33  */
	pointer = (MSD_U8)0x33;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("Route Dest                   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));

	/*policy CPU Dest, Index: 0x38 */
	pointer = (MSD_U8)0x38;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("MgmtPri                      0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xE0) >> 5, (data[1] & (MSD_U8)0xE0) >> 5, (data[2] & (MSD_U8)0xE0) >> 5, (data[3] & (MSD_U8)0xE0) >> 5, (data[4] & (MSD_U8)0xE0) >> 5, \
		(data[5] & (MSD_U8)0xE0) >> 5, (data[6] & (MSD_U8)0xE0) >> 5, (data[7] & (MSD_U8)0xE0) >> 5, (data[8] & (MSD_U8)0xE0) >> 5, (data[9] & (MSD_U8)0xE0) >> 5, \
		(data[10] & (MSD_U8)0xE0) >> 5, (data[11] & (MSD_U8)0xE0) >> 5));

	MSG(("CPU Dest                     0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));

	/*policy Alternate CPU Dest, Index: 0x39 */
	pointer = (MSD_U8)0x39;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("AltPri                       0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xE0) >> 5, (data[1] & (MSD_U8)0xE0) >> 5, (data[2] & (MSD_U8)0xE0) >> 5, (data[3] & (MSD_U8)0xE0) >> 5, (data[4] & (MSD_U8)0xE0) >> 5, \
		(data[5] & (MSD_U8)0xE0) >> 5, (data[6] & (MSD_U8)0xE0) >> 5, (data[7] & (MSD_U8)0xE0) >> 5, (data[8] & (MSD_U8)0xE0) >> 5, (data[9] & (MSD_U8)0xE0) >> 5, \
		(data[10] & (MSD_U8)0xE0) >> 5, (data[11] & (MSD_U8)0xE0) >> 5));

	MSG(("AltCPU Dest                  0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));

	/*policy PTP Dest, Index: 0x3F */
	pointer = (MSD_U8)0x3F;
	for (port = 0; port < dev->numOfPorts; port++)
	{
		retVal = Oak_gpolicyGetMgmtCtrl(dev, (MSD_LPORT)port, pointer, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gpolicyGetMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		data[port] = tmpdata;
	}

	MSG(("PtpPri                       0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		(data[0] & (MSD_U8)0xE0) >> 5, (data[1] & (MSD_U8)0xE0) >> 5, (data[2] & (MSD_U8)0xE0) >> 5, (data[3] & (MSD_U8)0xE0) >> 5, (data[4] & (MSD_U8)0xE0) >> 5, \
		(data[5] & (MSD_U8)0xE0) >> 5, (data[6] & (MSD_U8)0xE0) >> 5, (data[7] & (MSD_U8)0xE0) >> 5, (data[8] & (MSD_U8)0xE0) >> 5, (data[9] & (MSD_U8)0xE0) >> 5, \
		(data[10] & (MSD_U8)0xE0) >> 5, (data[11] & (MSD_U8)0xE0) >> 5));

	MSG(("PTP Dest                     0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x   0x%-5x \n", \
		data[0] & (MSD_U8)0x1F, data[1] & (MSD_U8)0x1F, data[2] & (MSD_U8)0x1F, data[3] & (MSD_U8)0x1F, data[4] & (MSD_U8)0x1F, \
		data[5] & (MSD_U8)0x1F, data[6] & (MSD_U8)0x1F, data[7] & (MSD_U8)0x1F, data[8] & (MSD_U8)0x1F, data[9] & (MSD_U8)0x1F, \
		data[10] & (MSD_U8)0x1F, data[11] & (MSD_U8)0x1F));


	MSD_DBG_INFO(("Oak_gpolicyDump Exit.\n"));
	return MSD_OK;
}
