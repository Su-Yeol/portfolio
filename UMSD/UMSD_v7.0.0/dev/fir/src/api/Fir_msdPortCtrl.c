/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Fir_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <fir/include/api/Fir_msdPortCtrl.h>
#include <fir/include/api/Fir_msdApiInternal.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


#define FIR_MSD_GET_RATE_LIMIT_PER_FRAME(_frames, _dec)    \
        ((_frames)?(1000000000 / (16 * (_frames)) + ((1000000000 % (16 * (_frames)) > (16*(_frames)/2))?1:0)):0)

#define FIR_MSD_GET_RATE_LIMIT_PER_BYTE(_kbps, _dec)    \
        ((_kbps)?((8000000*(_dec)) / (16 * (_kbps)) + ((8000000*(_dec)) % (16 * (_kbps))?1:0)):0)

static MSD_STATUS Fir_writeFlowCtrlReg
(
	IN MSD_QD_DEV	*dev, 
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	IN MSD_U8	data
);
static MSD_STATUS Fir_readFlowCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	OUT MSD_U8	*data
);

static MSD_STATUS Fir_writeExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	IN MSD_U16 data
);
static MSD_STATUS Fir_readExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	OUT MSD_U16 *data
);

/*******************************************************************************
* Fir_gvlnSetPortVid
*
* DESCRIPTION:
*       This routine Set the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       vid  - the port vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gvlnSetPortVid Called.\n"));
    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}	
	else
	{
		if (vid > (MSD_U16)0xFFF)
		{
			MSD_DBG_ERROR(("Bad vid. It should be within [0, 0xFFF]\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)0, (MSD_U8)12, vid);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write FIR_PVID Register.\n"));
			}			
		}
	}

    MSD_DBG_INFO(("Fir_gvlnSetPortVid Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gvlnGetPortVid
*
* DESCRIPTION:
*       This routine Get the port default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       vid  - the port vlan id.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gvlnGetPortVid
(
	IN MSD_QD_DEV *dev,
	IN  MSD_LPORT port,
	OUT MSD_U16   *vid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gvlnGetPortVid Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)0, (MSD_U8)12, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PVID Register.\n"));
		}
		else
		{
			*vid = data;
		}
	}

    MSD_DBG_INFO(("Fir_gvlnGetPortVid Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetForceDefaultVid
*
* DESCRIPTION:
*       This routine Set the port force default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*       en   - enable force port default vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetForceDefaultVid
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;
	MSD_U16          data;

	MSD_DBG_INFO(("Fir_gprtSetForceDefaultVID Called.\n"));
	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)12, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PVID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetForceDefaultVID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetForceDefaultVid
*
* DESCRIPTION:
*       This routine Get the port force default vlan id.
*
* INPUTS:
*       port - logical port number to set.
*
* OUTPUTS:
*       en  - enable port force default vlan id.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetForceDefaultVid
(
IN MSD_QD_DEV	*dev,
IN  MSD_LPORT	port,
OUT MSD_BOOL	*en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetForceDefaultVID Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PVID, (MSD_U8)12, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PVID Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

	MSD_DBG_INFO(("Fir_gprtGetForceDefaultVID Exit.\n"));
	return retVal;
}

/********************************************************************
* Fir_gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the IEEE 802.1q mode for this port
*
* INPUTS:
*       port    - logical port number to set.
*       mode     - 802.1q mode for this port 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN FIR_MSD_8021Q_MODE    mode
)
{
    MSD_STATUS       retVal = MSD_OK;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gvlnSetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case FIR_MSD_DISABLE:
		case FIR_MSD_FALLBACK:
		case FIR_MSD_CHECK:
		case FIR_MSD_SECURE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)10, (MSD_U8)2, (MSD_U16)mode);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gvlnSetPortVlanDot1qMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Fir_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Fir_get.
*
* OUTPUTS:
*       mode     - 802.1q mode for this port 
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT FIR_MSD_8021Q_MODE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gvlnGetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (NULL == mode)
		{
			MSD_DBG_ERROR(("Failed(bad mode input).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)10, (MSD_U8)2, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL2 Register.\n"));
			}
			else
			{
				*mode = (FIR_MSD_8021Q_MODE)data;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gvlnGetPortVlanDot1qMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetDiscardTagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as 
*       Tagged will be discarded as they enter this switch port. Priority only 
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard tagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetDiscardTagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(mode, data);

		/* Set DiscardTagged. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)9, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDiscardTagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Fir_gets DiscardTagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardTagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetDiscardTagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DiscardTagged. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)9, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDiscardTagged Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetDiscardUntagged
*
* DESCRIPTION:
*       When this bit is set to a one, all non-MGMT frames that are processed as 
*       Untagged will be discarded as they enter this switch port. Priority only 
*       tagged frames (with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to discard untagged frame, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetDiscardUntagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(mode, data);

		/* Set DiscardUnTagged. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)8, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDiscardUntagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Fir_gets DiscardUntagged bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if DiscardUntagged bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetDiscardUntagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DiscardUnTagged. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)8, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDiscardUnTagged Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine set Forward Unknown mode of a switch port. 
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetUnicastFloodBlock
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetUnicastFloodBlock Called.\n"));

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    data ^= 1U;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)2, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetUnicastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Fir_gets Forward Unknown mode of a switch port.
*       When this mode is set to MSD_TRUE, normal switch operation occurs.
*       When this mode is set to MSD_FALSE, unicast frame with unknown DA addresses
*       will not egress out this port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA unicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetUnicastFloodBlock
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetUnicastFloodBlock Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)2, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register.\n"));
		}
		else
		{
			data ^= (MSD_U16)1;

			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetUnicastFloodBlock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetMulticastFloodBlock
*
* DESCRIPTION:
*       When this bit is set to a one, normal switch operation will occurs and 
*       multicast frames with unknown DA addresses are allowed to egress out this 
*       port (assuming the VLAN settings allow the frame to egress this port too).
*       When this bit is cleared to a zero, multicast frames with unknown DA 
*       addresses will not egress out this port.
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetMulticastFloodBlock
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(mode, data);

		data ^= (MSD_U16)1;

		/* Set DefaultForward. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)3, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetMulticastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Fir_gets DefaultForward bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for normal switch operation or MSD_FALSE to do not egress out the unknown DA multicast frames
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetMulticastFloodBlock
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtGetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DefaultForward. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)3, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register.\n"));
		}
		else
		{
			data ^= (MSD_U16)1;

			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetMulticastFloodBlock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetDiscardBCastMode
*
* DESCRIPTION:
*       This routine sets the Discard Broadcast mode.
*       If the mode is enabled, all the broadcast frames to the given port will 
*       be discarded.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetDiscardBCastMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, (MSD_U8)0x19, (MSD_U8)13, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDiscardBCastMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Fir_gets the Discard Broadcast Mode. If the mode is enabled,
*       all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetDiscardBCastMode
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetDiscardBCastMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, (MSD_U8)0x19, (MSD_U8)13, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDiscardBCastMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine sets the Enable/disable trapping all IPV4 IGMP packets to 
*       the CPU,based on their ingress port.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetIGMPSnoopEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)10, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_CT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetIGMPSnoopEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetIGMPSnoopEnable
*
* DESCRIPTION:
*       This routine gets the Enable/disable trapping all IPV4 IGMP packets to
*       the CPU,based on their ingress port.
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetIGMPSnoopEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetIGMPSnoopEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetHeaderMode
*
* DESCRIPTION:
*       This routine gets the ingress and egress header mode of a switch 
*       port. 
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetHeaderMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetHeaderMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetHeaderMode
*
* DESCRIPTION:
*       This routine sets the ingress and egress header mode of a switch 
*       port. To be used only for port connected to cpu mainly used for layer3
*       32bit alignment.
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetHeaderMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)11, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetHeaderMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetDropOnLock
*
* DESCRIPTION:
*       This routine gets the DropOnLock mode of specified port on specified device
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled,
*             MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDropOnLock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetDropOnLock
*
* DESCRIPTION:
*       This routine sets the Drop on Lock. When set to one, Ingress frames
*       will be discarded if their SA field is not in the ATU's address database
*       of specified port on specified device
*
* INPUTS:
*       port - logical port number
*       en - MSD_TRUE, to enable the mode,
*             MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
)
{
    MSD_U16          data;           /* Used to poll the data */
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)14, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDropOnLock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - FIR_MSD_MTU_SIZE (1522, 2048, or 10240)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  FIR_MSD_MTU_SIZE   mode
)
{
    MSD_STATUS       retVal = MSD_OK;      /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetJumboMode Called.\n"));

    if (mode > FIR_MSD_MTU_SIZE_10240)
    {
        MSD_DBG_ERROR(("Bad Parameter(MTU Mode).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate LPORT to hardware port */
		hwPort = MSD_LPORT_2_PORT(port);
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_ERROR(("Failed (Bad Port).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			switch (mode)
			{
			case FIR_MSD_MTU_SIZE_1522:
			case FIR_MSD_MTU_SIZE_2048:
			case FIR_MSD_MTU_SIZE_10240:
				break;
			default:
				MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
				retVal = MSD_BAD_PARAM;
				break;
			}
			if (retVal != MSD_BAD_PARAM)
			{
				/* Set the Jumbo Fram Size bit.               */
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, (MSD_U16)mode);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetJumboMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Fir_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - FIR_MSD_MTU_SIZE (1522, 2048, or 10240)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT FIR_MSD_MTU_SIZE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gsysGetJumboMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get Jumbo Frame Mode.            */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			*mode = (FIR_MSD_MTU_SIZE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gsysGetJumboMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetSpecifiedMTU
*
* DESCRIPTION:
*       This routine set specified MTU size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       size - specified MTU size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		This API will change jumbo mode to 0x3 and MTU register 
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetSpecifiedMTU
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
	MSD_U16          tmpMode;

	MSD_DBG_INFO(("Fir_gprtSetSpecifiedMTU Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		tmpMode = (MSD_U16)0x3;
		/* Set the Jumbo Frame mode bit. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, (MSD_U16)tmpMode);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			/* Set MTU register */
			retVal = Fir_writeExtendedPortCtrlReg(dev, (MSD_LPORT)phyAddr, (MSD_U8)0x20, size & (MSD_U16)0x7FFF);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write MTU Register.\n"));
			}
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetSpecifiedMTU Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetSpecifiedMTU
*
* DESCRIPTION:
*       This routine gets the max specified MTU allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       size - specified MTU size
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetSpecifiedMTU
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16       *size
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	MSD_U16          data;           /* to keep the read valve       */

	MSD_DBG_INFO(("Fir_gprtGetSpecifiedMTU Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get MTU register */
		retVal = Fir_readExtendedPortCtrlReg(dev, (MSD_LPORT)phyAddr, (MSD_U8)0x20, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read MTU Register.\n"));
		}
		else
		{
			*size = data & (MSD_U16)0x7fff;
		}
	}

	MSD_DBG_INFO(("Fir_gprtGetSpecifiedMTU Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetLearnEnable
*
* DESCRIPTION:
*       This routine enables/disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for enable or MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetLearnEnable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{          
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          pav;
	MSD_U16          data = 0;

    MSD_DBG_INFO(("Fir_gprtSetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Set the port's PAV to all zeros */
		if (mode != MSD_FALSE)
		{
			pav = (MSD_U16)((MSD_U16)1 << port);
		}
		else
		{
			pav = 0;
		}

		/* Read back register value */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)0, (MSD_U8)16, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PAV Register.\n"));
		}
		else
		{
			data = (data & (MSD_U16)0xF800) | pav;
		}
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)0, (MSD_U8)16, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PAV Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetLearnEnable Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Fir_gets LearnDisable setup
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE: Learning disabled on the given port ingress frames,
*                 MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetLearnEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U16          pav;

    MSD_DBG_INFO(("Fir_gprtGetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)0, (MSD_U8)10, &pav);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PAV Register.\n"));
		}
		else
		{
			if ((pav & (MSD_U16)0x07FF) != 0U)
			{
				data = (MSD_U16)1;
			}
			else
			{
				data = 0;
			}

			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetLearnEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT FIR_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtGetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)10, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE Register.\n"));
		}
		else
		{
			*mode = (FIR_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetVTUPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  FIR_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;     /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;                /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtSetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case FIR_PRI_OVERRIDE_NONE:
		case FIR_PRI_OVERRIDE_FRAME:
		case FIR_PRI_OVERRIDE_QUEUE:
		case FIR_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)10, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE Register.\n"));
			}
			
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetVTUPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT FIR_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtGetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE.\n"));
		}
		else
		{
			*mode = (FIR_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetSAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  FIR_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;              /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtSetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case FIR_PRI_OVERRIDE_NONE:
		case FIR_PRI_OVERRIDE_FRAME:
		case FIR_PRI_OVERRIDE_QUEUE:
		case FIR_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)12, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE Register.\n"));
			}
			
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetSAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT FIR_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtGetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)14, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE.\n"));
		}
		else
		{
			*mode = (FIR_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  FIR_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;             /* to keep the read valve       */

    MSD_DBG_INFO(("Fir_gprtSetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case FIR_PRI_OVERRIDE_NONE:
		case FIR_PRI_OVERRIDE_FRAME:
		case FIR_PRI_OVERRIDE_QUEUE:
		case FIR_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PRI_OVERRIDE, (MSD_U8)14, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PRI_OVERRIDE Register.\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDAPriorityOverride Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetMessagePort
*
* DESCRIPTION:
*       This routine gets message port mode for specific port on a specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL1 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetMessagePort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetMessagePort
*
* DESCRIPTION:
*       This routine Set message port for specific port on a specific device.
*       Learning and learn refresh message frames will be sent out to this port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE: enable Message Port, MSD_FALSE: otherwise
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
MSD_STATUS Fir_gprtSetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL1 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetMessagePort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetEtherType
*
* DESCRIPTION:
*       This routine gets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode
*       API) of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       eType - Ethernet type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Fir_gprtGetEtherType Called.\n"));

	pointer = (MSD_U8)0x2;

	retVal = Fir_readExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read FIR_REG_PORT_ETH_TYPE Register.\n"));
    }

    MSD_DBG_INFO(("Fir_gprtGetEtherType Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetEtherType
*
* DESCRIPTION:
*       This routine sets the port's special Ether Type. This Ether Type is used
*       for Policy (see gprtSetPolicy API) and FrameMode (see gprtSetFrameMode API)
*       of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       eType - Ethernet type
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
MSD_STATUS Fir_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Fir_gprtSetEtherType Called.\n"));

	pointer = (MSD_U8)0x2;

	retVal = Fir_writeExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Write FIR_REG_PORT_ETH_TYPE Register.\n"));
    }

    MSD_DBG_INFO(("Fir_gprtSetEtherType Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetAllowVidZero
*
* DESCRIPTION:
*       This routine Get allow VID of Zero mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: allow VID of zero, MSD_FALSE: otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetAllowVidZero
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)3, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetAllowVidZero Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetAllowVidZero
*
* DESCRIPTION:
*       This routine Set allow VID of Zero of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE: allow VID of zero, MSD_FALSE: otherwise
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
MSD_STATUS Fir_gprtSetAllowVidZero
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)3, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetAllowVidZero Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetTrunkPort
*
* DESCRIPTION:
*       This function enables/disables and sets the trunk ID.
*        
* INPUTS:
*       port - the logical port number.
*       en - MSD_TRUE to make the port be a member of a trunk with the given trunkId.
*             MSD_FALSE, otherwise.
*       trunkId - valid ID is 0 ~ 31
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      en,
    IN MSD_U32       trunkId
)
{
    MSD_U16          data;           
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */

    MSD_DBG_INFO(("Fir_gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		if (en == MSD_TRUE)
		{
			/* need to enable trunk. so check the trunkId */
			if (FIR_IS_TRUNK_ID_VALID(dev, trunkId) != 1)
			{
				MSD_DBG_ERROR(("Failed(bad trunkId).\n"));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				/* Set TrunkId. */
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)8, (MSD_U8)5, (MSD_U16)trunkId);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL1 Register.\n"));
				}
			}
		}

		if (retVal == MSD_OK)
		{
			/* Set TrunkPort bit. */
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)14, (MSD_U8)1, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write FIR_PORT_CONTROL1 Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetTrunkPort Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetTrunkPort
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*        
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a trunk,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    OUT MSD_BOOL     *en,
    OUT MSD_U32        *trunkId
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		data = 0;
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL1 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);

			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL1, (MSD_U8)8, (MSD_U8)5, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PORT_CONTROL1 Register.\n"));
			}
			else
			{
				*trunkId = (MSD_U32)data;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetTrunkPort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetFlowCtrl
*
* DESCRIPTION:
*       This routine enable/disable port flow control and set flow control mode
*          mode - PORT_FC_TX_RX_ENABLED,
*                 PORT_RX_ONLY,
*                 PORT_TX_ONLY,
*                 PORT_PFC_ENABLED
*
* INPUTS:
*       port - the logical port number.
*		en - enable/disable the flow control
*       mode - flow control mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*        
* COMMENTS:
*        Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  FIR_MSD_PORT_FC_MODE      mode
)
{
	MSD_STATUS       retVal = MSD_OK;
	MSD_U8			 tmpData;
	MSD_U8			 tmpVal;

    MSD_DBG_INFO(("Fir_gprtSetFlowCtrl Called.\n"));

    switch (mode)
    {
    case Fir_PORT_FC_TX_RX_ENABLED:
    case Fir_PORT_TX_ONLY:
    case Fir_PORT_RX_ONLY:
    case Fir_PORT_PFC_ENABLED:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
		retVal = MSD_BAD_PARAM;
		break;
    }

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Fir_readFlowCtrlReg(dev, port, (MSD_U8)0x10, &tmpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
		}
		else
		{
			/* translate BOOL to binary */
			MSD_BOOL_2_BIT(en, tmpVal);

			tmpData &= (MSD_U8)~(MSD_U8)0x07;
			tmpData = (MSD_U8)(tmpData | (MSD_U8)((MSD_U8)1 << 3) | (MSD_U8)(tmpVal << 2) | (MSD_U8)mode);

			retVal = Fir_writeFlowCtrlReg(dev, port, (MSD_U8)0x10, tmpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
			}	
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Fir_get switch port flow control enable/disable status and return 
*       flow control mode
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*		en - enable/disable the flow control
*       mode - flow control mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*        
*******************************************************************************/
MSD_STATUS Fir_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT FIR_MSD_PORT_FC_MODE      *mode
)
{
	MSD_U8			tmpData;
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Fir_gprtGetFlowCtrl Called.\n"));

	retVal = Fir_readFlowCtrlReg(dev, port, (MSD_U8)0x10, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
    }
	else
	{
		*en = (MSD_BOOL)(MSD_U8)((MSD_U8)(tmpData >> 2) & (MSD_U8)0x1);
		*mode = (FIR_MSD_PORT_FC_MODE)(MSD_U8)(tmpData & (MSD_U8)0x3);
	}

    MSD_DBG_INFO(("Fir_gprtGetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetEgressMonitorSource
*
* DESCRIPTION:
*       When this bit is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that egresses out this port will
*       also be sent to the EgressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set EgressMonitorSource, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetEgressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(mode, data);

		/* Set EgressMonitorSource. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)5, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set EgressMonitorSource bit error\n"));
		}
	}
    
    MSD_DBG_INFO(("Fir_gprtSetEgressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine Fir_gets EgressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if EgressMonitorSource bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetEgressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the EgressMonitorSource. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)5, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get EgressMonitorSource bit error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetEgressMonitorSource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetIngressMonitorSource
*
* DESCRIPTION:
*       When this be is cleared to a zero, normal network switching occurs.
*       When this bit is set to a one, any frame that ingresses in this port will
*       also be sent to the IngressMonitorDest Port
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE to set IngressMonitorSource, MSD_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetIngressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(mode, data);

		/* Set IngressMonitorSource. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)4, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set IngressMonitorSource bit error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetIngressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine Fir_gets IngressMonitorSource bit for the given port
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode  - MSD_TRUE if IngressMonitorSource bit is set, MSD_FALSE otherwise
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetIngressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the IngressMonitorSource. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)4, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get IngressMonitorSource bit error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetIngressMonitorSource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_grcSetEgressRate
*
* DESCRIPTION:
*       This routine sets the port's egress data limit.
*        
*
* INPUTS:
*       port      - logical port number.
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type). 
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the FIR_MSD_ELIMIT_MODE of 
*                                FIR_MSD_ELIMIT_LAYER1,
*                                FIR_MSD_ELIMIT_LAYER2, or 
*                                FIR_MSD_ELIMIT_LAYER3 (see Fir_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with FIR_MSD_ELIMIT_MODE of 
*                                FIR_MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_grcSetEgressRate
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN FIR_MSD_ELIMIT_MODE mode,
    IN MSD_U32	rate
)
{
    MSD_STATUS     retVal = MSD_OK;        /* Functions return value.      */
    MSD_U16        data = 0;
    MSD_U32        eDec = 0;
    MSD_U8         hwPort, phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Fir_grcSetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case FIR_MSD_ELIMIT_FRAME:
		case FIR_MSD_ELIMIT_LAYER1:
		case FIR_MSD_ELIMIT_LAYER2:
		case FIR_MSD_ELIMIT_LAYER3:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = Fir_grcSetELimitMode(dev, port, mode);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_grcSetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				if (mode == FIR_MSD_ELIMIT_FRAME)
				{
					if (rate == 0U) /* disable egress rate limit */
					{
						eDec = 0;
						data = 0;
					}
					else if ((rate < (MSD_U32)3815) || (rate > (MSD_U32)14880000))
					{
						MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
						retVal = MSD_BAD_PARAM;
					}
					else
					{
						eDec = (MSD_U32)1;
						data = (MSD_U16)FIR_MSD_GET_RATE_LIMIT_PER_FRAME(rate, eDec);
					}
				}
				else
				{
					if (rate == (MSD_U32)0)
					{
						eDec = 0;
					}
					else if (rate < (MSD_U32)1000)    /* less than 1Mbps */
					{
						/* it should be divided by 64 */
						if ((rate % (MSD_U32)64) != 0U)
						{
							MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
							retVal = MSD_BAD_PARAM;
						}
						else
						{
							eDec = rate / (MSD_U32)64;
						}
					}
					else if (rate <= (MSD_U32)100000)    /* less than or equal to 100Mbps */
					{
						/* it should be divided by 1000 */
						if ((rate % (MSD_U32)1000) != 0U)
						{
							MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
							retVal = MSD_BAD_PARAM;
						}
						else
						{
							eDec = rate / (MSD_U32)1000;
						}
					}
					else if (rate <= (MSD_U32)1000000)    /* less than or equal to 1000Mbps */
					{
						/* it should be divided by 10000 */
						if ((rate % (MSD_U32)10000) != 0U)
						{
							MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
							retVal = MSD_BAD_PARAM;
						}
						else
						{
							eDec = rate / (MSD_U32)10000;
						}
					}
					else if (rate <= (MSD_U32)10000000)    /* less than or equal to 10Gbps */
					{
						/* it should be divided by 100000 */
						if ((rate % (MSD_U32)100000) != 0U)
						{
							MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
							retVal = MSD_BAD_PARAM;
						}
						else
						{
							eDec = rate / (MSD_U32)100000;
						}
					}
					else
					{
						MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
						retVal = MSD_BAD_PARAM;
					}

					if (rate == 0U)
					{
						data = 0;
					}
					else
					{
						data = (MSD_U16)FIR_MSD_GET_RATE_LIMIT_PER_BYTE(rate, eDec);
					}
				}

				if (retVal != MSD_BAD_PARAM)
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL, (MSD_U8)0, (MSD_U8)7, (MSD_U16)eDec);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("ERROR to write FIR_EGRESS_RATE_CTRL Register.\n"));
					}
					else
					{
						retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL2, (MSD_U8)0, (MSD_U8)14, (MSD_U16)data);
						if (retVal != MSD_OK)
						{
							MSD_DBG_ERROR(("ERROR to write FIR_EGRESS_RATE_CTRL2 Register.\n"));
						}
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_grcSetEgressRate Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Fir_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the FIR_MSD_ELIMIT_MODE of
*                                FIR_MSD_ELIMIT_LAYER1,
*                                FIR_MSD_ELIMIT_LAYER2, or
*                                FIR_MSD_ELIMIT_LAYER3 (see Fir_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with FIR_MSD_ELIMIT_MODE of
*                                FIR_MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_grcGetEgressRate
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT FIR_MSD_ELIMIT_MODE *mode,
	OUT MSD_U32	*rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        eRate, eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Fir_grcGetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Fir_grcGetELimitMode(dev, port, mode);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_grcGetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL, (MSD_U8)0, (MSD_U8)7, &eDec);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_EGRESS_RATE_CTRL Register.\n"));
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL2, (MSD_U8)0, (MSD_U8)14, &eRate);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to read FIR_EGRESS_RATE_CTRL2 Register.\n"));
				}
				else
				{
					if (*mode == FIR_MSD_ELIMIT_FRAME)
					{
						*rate = (MSD_U32)FIR_MSD_GET_RATE_LIMIT_PER_FRAME(eRate, eDec);
					}
					else
					{
						/* Count Per Byte */
						*rate = (MSD_U32)FIR_MSD_GET_RATE_LIMIT_PER_BYTE(eRate, eDec);
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_grcGetEgressRate Exit.\n"));
	return retVal;
}


/*******************************************************************************
* Fir_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            FIR_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            FIR_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            FIR_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            FIR_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - FIR_MSD_ELIMIT_MODE enum type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None. 
*
*******************************************************************************/
MSD_STATUS Fir_grcSetELimitMode
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  FIR_MSD_ELIMIT_MODE      mode
)
{
	MSD_U16            data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Fir_grcSetELimitMode Called.\n"));

	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		data = (MSD_U16)mode & (MSD_U16)0x3;

		/* Set the Elimit mode.            */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL2, (MSD_U8)14, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_EGRESS_RATE_CTRL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Fir_grcSetELimitMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Fir_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            FIR_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            FIR_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            FIR_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            FIR_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - FIR_MSD_ELIMIT_MODE enum type
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT FIR_MSD_ELIMIT_MODE  *mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_grcGetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the Elimit mode.            */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EGRESS_RATE_CTRL2, (MSD_U8)14, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_EGRESS_RATE_CTRL2 Register.\n"));
		}
		else
		{
			*mode = (FIR_MSD_ELIMIT_MODE)data;
		}
	}

    MSD_DBG_INFO(("Fir_grcGetELimitMode Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT FIR_MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    MSD_U16          data = 0;
    MSD_U8           hwPort;           /* the physical port number     */
    MSD_U8			 phyAddr;
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */

    MSD_DBG_INFO(("Fir_gprtSetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		switch (mode)
		{
		case FIR_MSD_DUPLEX_HALF:
			data = (MSD_U16)0x1;
			break;
		case FIR_MSD_DUPLEX_FULL:
			data = (MSD_U16)0x3;
			break;
		case FIR_MSD_DUPLEX_AUTO:
			data = 0x0;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)2, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PHY_CONTROL Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetDuplex Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT FIR_MSD_PORT_FORCE_DUPLEX_MODE  *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)2, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PHY_CONTROL Register.\n"));
		}
		else
		{
			switch (data)
			{
			case 0x1U:
				*mode = FIR_MSD_DUPLEX_HALF;
				break;
			case 0x3U:
				*mode = FIR_MSD_DUPLEX_FULL;
				break;
			case 0:
			case 0x2U:
				*mode = FIR_MSD_DUPLEX_AUTO;
				break;
			default:
				MSD_DBG_ERROR(("Failed (Bad mode).\n"));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDuplex Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetDuplexStatus
*
* DESCRIPTION:
*       This routine retrives the port duplex mode.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for Full-duplex  or MSD_FALSE for Half-duplex
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetDuplexStatus
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
	
	MSD_DBG_INFO(("Fir_gprtGetDuplexStatus Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the force flow control bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_STATUS, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_STATUS Register.\n"));
		}
		else
		{
			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetDuplexStatus Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT FIR_MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PHY_CONTROL Register.\n"));
		}
		else
		{
			if ((data & (MSD_U16)0x1) == 0U)
			{
				*mode = FIR_PORT_DO_NOT_FORCE_LINK;
			}
			else if ((data & (MSD_U16)0x3) == 3U)
			{
				*mode = FIR_PORT_FORCE_LINK_UP;
			}
			else if ((data & (MSD_U16)0x3) == 1U)
			{
				*mode = FIR_PORT_FORCE_LINK_DOWN;
			}
			else
			{
				MSD_DBG_ERROR(("ERROR no match force link mode.\n"));
				retVal = MSD_FAIL;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetForceLink Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT FIR_MSD_PORT_FORCE_LINK_MODE  mode
)
{
    MSD_U16          data = 0;
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
	MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */

    MSD_DBG_INFO(("Fir_gprtSetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		switch (mode)
		{
		case FIR_PORT_DO_NOT_FORCE_LINK:
			data = 0;
			break;
		case FIR_PORT_FORCE_LINK_UP:
			data = (MSD_U16)3;
			break;
		case FIR_PORT_FORCE_LINK_DOWN:
			data = (MSD_U16)1;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mpde).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read FIR_PHY_CONTROL Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetForceLink Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetLinkState
*
* DESCRIPTION:
*       This routine retrives the link state.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       state - MSD_TRUE for Up  or MSD_FALSE for Down
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetLinkState
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *state
)
{
	MSD_U16          data;           /* Used to poll the SWReset bit */
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	
	MSD_DBG_INFO(("Fir_gprtGetLinkState Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the force flow control bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_STATUS, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_STATUS Register.\n"));
		}
		else
		{
			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *state);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetLinkState Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - FIR_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN FIR_MSD_PORT_FORCED_SPEED_MODE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;
    MSD_U8           hwPort;
    MSD_U8			 phyAddr;
    MSD_U16			 data1 = 0;
    MSD_U16			 data2 = 0;
    MSD_U16			 linkStatus = 0;

    MSD_DBG_INFO(("Fir_gprtSetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Set the ForceSpeed bit.  */
		if (mode == Fir_PORT_DO_NOT_FORCE_SPEED)
		{
			/*Get the force link status and then force link down*/
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &linkStatus);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Get link status Failed\n"));
			}
			else
			{
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, (MSD_U16)1);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Force Link down failed\n"));
				}
				else
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, (MSD_U16)0);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Set Force Speed bit error\n"));
					}
				}
			}
		}
		else
		{
			switch (mode)
			{
			case Fir_PORT_FORCE_SPEED_10M:
				data1 = 0;
				data2 = 0;
				break;
			case Fir_PORT_FORCE_SPEED_100M:
				data1 = 0;
				data2 = (MSD_U16)1;
				break;
			case Fir_PORT_FORCE_SPEED_200M:
				if ((port != (MSD_LPORT)5) && (port != (MSD_LPORT)7))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 5, 7 support 200M.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)1;
				break;
			case Fir_PORT_FORCE_SPEED_1000M:
				data1 = 0;
				data2 = (MSD_U16)2;
				break;
			case Fir_PORT_FORCE_SPEED_2_5G:
				if ((port != (MSD_LPORT)6) && (port != (MSD_LPORT)7) && (port != (MSD_LPORT)8) && (port != (MSD_LPORT)9))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 6, 7, 8, 9 support 2.5G.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)2;
				break;
			case Fir_PORT_FORCE_SPEED_5G:
				if ((port != (MSD_LPORT)8) && (port != (MSD_LPORT)9))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 8, 9 support 5G.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)3;
				break;
			case Fir_PORT_FORCE_SPEED_10G:
				if ((port != (MSD_LPORT)8) && (port != (MSD_LPORT)9))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 8, 9 support 10G.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = 0;
				data2 = (MSD_U16)3;
				break;
			default:
				MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
				retVal = MSD_BAD_PARAM;
				break;
			}

			/* Write foce speed value */
			if (retVal == MSD_OK)
			{
				/*Get the force link status and then force link down*/
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &linkStatus);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get link status Failed\n"));
				}
				else
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, (MSD_U16)1);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Force Link down failed\n"));
					}
					else
					{
						/* Set the ForceSpeed bit.  */
						retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, (MSD_U16)1);
						if (retVal != MSD_OK)
						{
							MSD_DBG_ERROR(("Set Force Speed bit error\n"));
						}
						else
						{
							retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)12, (MSD_U8)1, data1);
							if (retVal != MSD_OK)
							{
								MSD_DBG_ERROR(("Set Alternate Speed bit error\n"));
							}
							else
							{
								retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)0, (MSD_U8)2, data2);
								if (retVal != MSD_OK)
								{
									MSD_DBG_ERROR(("Set Speed bit error\n"));
								}
							}
						}
					}
				}
			}
		}

		if (retVal == MSD_OK)
		{
			/*Set back the force link status*/
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, linkStatus);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set back force link status error\n"));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetForceSpeed Called.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - FIR_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT FIR_MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
    MSD_U16          data1;
    MSD_U16          data2;
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Get the ForceSpeed bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, &data1);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get force Speed bit error\n"));
		}
		else
		{
			if (data1 == (MSD_U16)0)
			{
				*mode = Fir_PORT_DO_NOT_FORCE_SPEED;
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)12, (MSD_U8)1, &data1);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get Alternate Speed bit error\n"));
				}
				else
				{
					retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PHY_CONTROL, (MSD_U8)0, (MSD_U8)2, &data2);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Get Speed bit error\n"));
					}
					else
					{
						if (data1 == (MSD_U16)1)
						{
							switch (data2)
							{
							case 0:
								*mode = Fir_PORT_FORCE_SPEED_10M;
								break;
							case 1U:
								*mode = Fir_PORT_FORCE_SPEED_200M;
								break;
							case 2U:
								*mode = Fir_PORT_FORCE_SPEED_2_5G;
								break;
							case 3U:
								*mode = Fir_PORT_FORCE_SPEED_5G;
								break;
							default:
								MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
								retVal = MSD_FAIL;
								break;
							}
						}
						else
						{
							switch (data2)
							{
							case 0:
								*mode = Fir_PORT_FORCE_SPEED_10M;
								break;
							case 1U:
								*mode = Fir_PORT_FORCE_SPEED_100M;
								break;
							case 2U:
								*mode = Fir_PORT_FORCE_SPEED_1000M;
								break;
							case 3U:
								*mode = Fir_PORT_FORCE_SPEED_10G;
								break;
							default:
								MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
								retVal = MSD_FAIL;
								break;
							}
						}
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetForceSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - FIR_MSD_PORT_SPEED type.
*                (PORT_SPEED_10_MBPS,PORT_SPEED_100_MBPS, PORT_SPEED_1000_MBPS,
*                etc.)
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT FIR_MSD_PORT_SPEED   *speed
)
{
	MSD_U16          data, speedEx;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetSpeed Called.\n"));	

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_PORT_STATUS, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read FIR_PORT_STATUS Register.\n"));
		}
		else
		{
			/* Reg0_13 + Reg0[9:8]
				000 - 10M
				001 - 100M
				101 - 200M
				010 - 1000M
				011 - 10G
				110 - 2.5G
				111 - 5G
				*/
			speedEx = (MSD_U16)(((data & (MSD_U16)0x2000) >> 11) | ((data & (MSD_U16)0x300) >> 8));

			switch (speedEx)
			{
			case 0:
				*speed = Fir_PORT_SPEED_10_MBPS;
				break;
			case 1U:
				*speed = Fir_PORT_SPEED_100_MBPS;
				break;
			case 5U:
				*speed = Fir_PORT_SPEED_200_MBPS;
				break;
			case 2U:
				*speed = Fir_PORT_SPEED_1000_MBPS;
				break;
			case 6U:
				*speed = Fir_PORT_SPEED_2_5_GBPS;
				break;
			case 7U:
				*speed = Fir_PORT_SPEED_5_GBPS;
				break;
			case 3U:
				*speed = Fir_PORT_SPEED_10_GBPS;
				break;
			default:
				*speed = Fir_PORT_SPEED_UNKNOWN;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetSpeed Called.\n"));
	return retVal;
}


/*******************************************************************************
* Fir_gprtGetVlanPorts
*
* DESCRIPTION:
*       this routine Get port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       memPorts - vlan ports
*       memPortsLen - number of vlan ports
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
MSD_STATUS Fir_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          tmpData;        /* to keep the read valve       */
    MSD_U8			 phyAddr;
    MSD_U8           i;
    MSD_U8           portLen = 0;
	MSD_U16          data = 0;

    MSD_DBG_INFO(("Fir_gprtGetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)10, &tmpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_QD_REG_PORT_VLAN_MAP register error\n"));
		}
		else
		{
			data = (MSD_U16)(tmpData & (MSD_U32)0x07ff);

			for (i = 0; i < dev->numOfPorts; i++)
			{
				if ((data & (MSD_U16)((MSD_U16)1 << i)) != 0U)
				{
					memPorts[portLen] = i;
					portLen += 1U;
				}
			}

			*memPortsLen = (MSD_U8)portLen;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetVlanPorts Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetVlanPorts
*
* DESCRIPTION:
*       this routine Set port based vlan table of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       memPorts - vlan ports to set
*       memPortsLen - number of vlan ports
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
MSD_STATUS Fir_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U16          data = 0;          /* to keep the read valve       */
    MSD_U8			 phyAddr;
    MSD_U8           i;
	MSD_U16          tmpData = 0, tmpData1 = 0;


    MSD_DBG_INFO(("Fir_gprtSetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (memPortsLen > dev->numOfPorts)
		{
			MSD_DBG_ERROR(("Failed (Bad num of vlan ports).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			data = 0;
			for (i = 0; i < memPortsLen; i++)
			{
				if (memPorts[i] < dev->numOfPorts)
				{
					data |= (MSD_U16)(1 << memPorts[i]);
				}
				else
				{
					MSD_DBG_ERROR(("Failed (Bad member port).\n"));
					retVal = MSD_BAD_PARAM;
					break;
				}
			}

			if (retVal != MSD_BAD_PARAM)
			{
				/* move port 11 value to high 16 bits*/
				tmpData1 = (data & (MSD_U16)0x7ff);

				/* read back port vlan register */
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)10, &tmpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get FIR_PORT_VLAN_MAP register error\n"));
				}
				else
				{
					tmpData = (tmpData & (MSD_U16)0xF800) | tmpData1;

					retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)16, tmpData);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Set FIR_PORT_VLAN_MAP register error\n"));
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetVlanPorts Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetFrameMode
*
* DESCRIPTION:
*       this routine Get frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_FRAME_MODE
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
MSD_STATUS Fir_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT FIR_MSD_FRAME_MODE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)8, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (FIR_MSD_FRAME_MODE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetFrameMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetFrameMode
*
* DESCRIPTION:
*       this routine Set frame mode of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - MSD_FRAME_MODE
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
MSD_STATUS Fir_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  FIR_MSD_FRAME_MODE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
    MSD_U8           hwPort;           /* the physical port number     */
    MSD_U16          data;             /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case FIR_FRAME_MODE_NORMAL:
		case FIR_FRAME_MODE_DSA:
		case FIR_FRAME_MODE_PROVIDER:
		case FIR_FRAME_MODE_ETHER_TYPE_DSA:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)8, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_PORT_CONTROL register error\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetFrameMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetPortECID
*
* DESCRIPTION:
*       this routine Set port E-CID of a specified port
*
* INPUTS:
*       port - logical port number
*       ecid - port E-CID
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
MSD_STATUS Fir_gprtSetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_U16 ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Fir_gprtSetPortECID Called.\n"));

	pointer = (MSD_U8)0x13;

	/*read E-CID register value*/
	retVal = Fir_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read FIR_PORT_E-CID Register.\n"));
	}
	else
	{
		tempValue = (tempValue & (MSD_U16)0xF000) | (ecid & (MSD_U16)0xFFF);

		retVal = Fir_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write FIR_PORT_ECID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetPortECID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetPortECID
*
* DESCRIPTION:
*       This routine gets the port's E-CID.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       ecid - port E-CID
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_U16 *ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Fir_gprtGetPortECID Called.\n"));

	pointer = (MSD_U8)0x13;

	retVal = Fir_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read FIR_PORT_E-CID Register.\n"));
	}
	else
	{
		*ecid = tempValue & (MSD_U16)0xFFF;
	}

	MSD_DBG_INFO(("Fir_gprtGetPortECID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetIgnoreETag
*
* DESCRIPTION:
*       this routine Set port ignore E-CID of a specified port
*
* INPUTS:
*       port - logical port number
*       en - port ignore E-CID enable
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
MSD_STATUS Fir_gprtSetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_BOOL en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Fir_gprtSetIgnoreETag Called.\n"));

	pointer = (MSD_U8)0x13;

	/*read E-CID register value*/
	retVal = Fir_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read FIR_REG_PORT_E-CID Register.\n"));
	}
	else
	{
		tempValue = (MSD_U16)((tempValue & (MSD_U16)0xEFFF) | (MSD_U16)((MSD_U16)en << 12));

		retVal = Fir_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write FIR_REG_PORT_ECID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetIgnoreETag Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetIgnoreETag
*
* DESCRIPTION:
*       This routine gets the port's ignore E-CID.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - port ignore E-CID enable
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_BOOL *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Fir_gprtGetIgnoreETag Called.\n"));

	pointer = (MSD_U8)0x13;

	retVal = Fir_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read FIR_REG_PORT_E-CID Register.\n"));
	}
	else
	{
		*en = (MSD_BOOL)(MSD_U16)((MSD_U16)(tempValue & (MSD_U16)0x1000) >> 12);
	}

	MSD_DBG_INFO(("Fir_gprtGetIgnoreETag Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetEgressMode
*
* DESCRIPTION:
*       this routine Set egress mode of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - MSD_EGRESS_MODE
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
MSD_STATUS Fir_gprtSetEgressMode
(
	IN MSD_QD_DEV*  dev,
	IN MSD_LPORT  port,
	IN FIR_MSD_EGRESS_MODE mode
)
{
	MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
	MSD_U8           hwPort;            /* the physical port number     */
	MSD_U16          data;              /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtSetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case FIR_MSD_EGRESS_MODE_UNMODIFIED:
		case FIR_MSD_EGRESS_MODE_UNTAGGED:
		case FIR_MSD_EGRESS_MODE_TAGGED:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)12, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_PORT_CONTROL register error\n"));
			}
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetEgressMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetEgressMode
*
* DESCRIPTION:
*       this routine Get egress mode of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_EGRESS_MODE
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
MSD_STATUS Fir_gprtGetEgressMode
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT FIR_MSD_EGRESS_MODE  *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (FIR_MSD_EGRESS_MODE)data;
		}
	}

	MSD_DBG_INFO(("Fir_gprtGetEgressMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetExtendedPortCtrlReg
*
* DESCRIPTION:
*       This function writes to specified field in a switch's extended control register.
*
* INPUTS:
*       devAddr     - Device Address to write the register for.
*       portNum     - The port number.
*       index		- The Extended Port Control Addresses.
*                    0x00 to 0x0F = Define the various Ether Types decoded by the port
*                    0x10 to 0x1F = Misc control registers
*                    0x20 to 0xFF = Reserved for future use
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*       data        - Data to be written.
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
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetExtendedPortCtrlReg
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  portNum,
	IN  MSD_U8     index,
	IN  MSD_U8     fieldOffset,
	IN  MSD_U8     fieldLength,
	IN  MSD_U16    data
)
{
	MSD_U16 mask;
	MSD_U16 tmpData;
	MSD_STATUS   retVal;

	retVal = Fir_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_readExtendedPortCtrlReg register error\n"));
	}
	else
	{
		MSD_CALC_MASK(fieldOffset, fieldLength, mask);

		/* Set the desired bits to 0.                       */
		tmpData &= (MSD_U16)~mask;
		/* Set the given data into the above reset bits.    */
		tmpData |= (MSD_U16)(data << fieldOffset) & mask;

		retVal = Fir_writeExtendedPortCtrlReg(dev, portNum, index, tmpData);
	}

	return retVal;
}

/*******************************************************************************
* Fir_gprtGetExtendedPortCtrlReg
*
* DESCRIPTION:
*       This function read the specified field in a switch's extended control register.
*
* INPUTS:
*       devAddr     - Device Address to write the register for.
*       portNum     - The port number.
*       index		- The Extended Port Control Addresses.
*                    0x00 to 0x0F = Define the various Ether Types decoded by the port
*                    0x10 to 0x1F = Misc control registers
*                    0x20 to 0xFF = Reserved for future use
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data        - Data to be read.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       1.  The sum of fieldOffset & fieldLength parameters must be smaller-
*           equal to 16.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetExtendedPortCtrlReg
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  portNum,
	IN  MSD_U8     index,
	IN  MSD_U8     fieldOffset,
	IN  MSD_U8     fieldLength,
	OUT MSD_U16    *data
)
{
	MSD_U16 mask;            /* Bits mask to be read */
	MSD_U16 tmpData;
	MSD_STATUS   retVal;

	retVal = Fir_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_readExtendedPortCtrlReg register error\n"));
	}		
	else
	{
		MSD_CALC_MASK(fieldOffset, fieldLength, mask);

		tmpData = (tmpData & mask) >> fieldOffset;
		*data = tmpData;
	}

	return retVal;
}

/*******************************************************************************
* Fir_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - FIR_PORT_STP_STATE
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
MSD_STATUS Fir_gprtGetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT FIR_MSD_PORT_STP_STATE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)0, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (FIR_MSD_PORT_STP_STATE)data;
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetPortState Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - FIR_PORT_STP_STATE
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
MSD_STATUS Fir_gprtSetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  FIR_MSD_PORT_STP_STATE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;     /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
    MSD_U16          data;                /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case FIR_MSD_PORT_DISABLE:
		case FIR_MSD_PORT_BLOCKING:
		case FIR_MSD_PORT_LEARNING:
		case FIR_MSD_PORT_FORWARDING:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL, (MSD_U8)0, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_PORT_CONTROL register error\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetPortState Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetIgnoreFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetIgnoreFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)14, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetAllowBadFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetAllowBadFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Fir_gprtSetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_writeFlowCtrlReg(dev, port, (MSD_U8)0x1, (MSD_U8)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */

	MSD_DBG_INFO(("Fir_gprtGetPauseLimitOut Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Fir_readFlowCtrlReg(dev, port, (MSD_U8)0x1, (MSD_U8*)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Fir_gprtSetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_writeFlowCtrlReg(dev, port, (MSD_U8)0x0, (MSD_U8)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetPauseLimitIn Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Fir_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_readFlowCtrlReg(dev, port, (MSD_U8)0x0, (MSD_U8*)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8  queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Fir_gprtSetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_writeFlowCtrlReg(dev, port, (MSD_U8)0x17, (MSD_U8)queues);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetQueueToPause Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U8  *queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Fir_gprtGetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_readFlowCtrlReg(dev, port, (MSD_U8)0x17, (MSD_U8*)queues);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetQueueToPause Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_BOOL en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           data, data1;

    MSD_DBG_INFO(("Fir_gprtSetRxPriFlowCtrlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pfcPri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)((pfcPri / 2U) + (MSD_U8)0x18);

			retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BOOL_2_BIT(en, data1);

				if ((pfcPri % 2U) != 0U)
				{
					data &= (MSD_U8)0x7f;
					data |= (MSD_U8)(data1 << 7);
				}
				else
				{
					data &= (MSD_U8)0xf7;
					data |= (MSD_U8)(data1 << 3);
				}

				retVal = Fir_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetRxPriFlowCtrlEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtGetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_BOOL *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           bitMask;
    MSD_U8           data;

    MSD_DBG_INFO(("Fir_gprtGetRxPriFlowCtrlEnable Called.\n"));


    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pfcPri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)((pfcPri / 2U) + (MSD_U8)0x18);

			if ((pfcPri % 2U) != 0U)
			{
				bitMask = (MSD_U8)0x80;
			}
			else
			{
				bitMask = (MSD_U8)0x08;
			}

			retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
			}
			else
			{
				if ((data & bitMask) != (MSD_U8)0)
				{
					*en = MSD_TRUE;
				}
				else
				{
					*en = MSD_FALSE;
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetRxPriFlowCtrlEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtSetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_U8 queue
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           data;

    MSD_DBG_INFO(("Fir_gprtSetRxPriFlowCtrlToQueue Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pfcPri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if (queue >= FIR_MAX_NUM_OF_QUEUES)
			{
				MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				pointer = (MSD_U8)((pfcPri / 2U) + (MSD_U8)0x18);

				retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
				}
				else
				{
					if ((pfcPri % 2U) != 0U)
					{
						data &= (MSD_U8)0x8f;
						data |= (MSD_U8)(queue << 4);
					}
					else
					{
						data &= (MSD_U8)0xf8;
						data |= queue;
					}

					retVal = Fir_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetRxPriFlowCtrlToQueue Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_U8 *queue
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           data;

    MSD_DBG_INFO(("Fir_gprtGetRxPriFlowCtrlToQueue Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pfcPri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)((pfcPri / 2U) + (MSD_U8)0x18);

			retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
			}
			else
			{
				if ((pfcPri % 2U) != 0U)
				{
					*queue = (data >> 4) & (MSD_U8)0x7;
				}
				else
				{
					*queue = data & (MSD_U8)0x7;
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetRxPriFlowCtrlToQueue Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
IN MSD_BOOL en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           data, data1;

    MSD_DBG_INFO(("Fir_gprtSetTxPriFlowCtrlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pri), pri : %u.\n", pri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)((pri / 2U) + (MSD_U8)0x1c);

			retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
			}
			else
			{
				MSD_BOOL_2_BIT(en, data1);

				if ((pri % 2U) != 0U)
				{
					data &= (MSD_U8)0x7f;
					data |= (MSD_U8)(data1 << 7);
				}
				else
				{
					data &= (MSD_U8)0xf7;
					data |= (MSD_U8)(data1 << 3);
				}

				retVal = Fir_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Fir_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetTxPriFlowCtrlEnable Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
OUT MSD_BOOL *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8           pointer;
    MSD_U8           bitMask;
    MSD_U8           data;

    MSD_DBG_INFO(("Fir_gprtGetTxPriFlowCtrlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (pri > (MSD_U8)0x7)
		{
			MSD_DBG_ERROR(("Failed (Bad pri), pri : %u.\n", pri));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			pointer = (MSD_U8)((pri / 2U) + (MSD_U8)0x1c);

			if ((pri % 2U) != 0U)
			{
				bitMask = (MSD_U8)0x80;
			}
			else
			{
				bitMask = (MSD_U8)0x08;
			}

			retVal = Fir_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
			}
			else
			{
				if ((data & bitMask) != (MSD_U8)0)
				{
					*en = MSD_TRUE;
				}
				else
				{
					*en = MSD_FALSE;
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetTxPriFlowCtrlEnable Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetOutQueueSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8 queue,
OUT MSD_U16  *count
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetOutQueueSize Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_Q_COUNTER, (MSD_U8)12, (MSD_U8)4, (MSD_U16)queue);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_Q_COUNTER register error\n"));
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_Q_COUNTER, (MSD_U8)0, (MSD_U8)9, count);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get FIR_Q_COUNTER register error\n"));
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetOutQueueSize Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)7, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetMapDA Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)7, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetMapDA Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)13, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetPortLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)13, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetPortLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetHoldAt1 Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetHoldAt1 Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)14, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetInt0nAgeOut Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetInt0nAgeOut Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)11, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetRefreshLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetRefreshLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)12, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetIgnoreWrongData Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PAV, (MSD_U8)12, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_QD_REG_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetIgnoreWrongData Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtSetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Fir_gprtSetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)6, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Fir_gprtSetARPMirror Exit.\n"));
    return retVal;
}

MSD_STATUS Fir_gprtGetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Fir_gprtGetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PORT_CONTROL2, (MSD_U8)6, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get FIR_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Fir_gprtGetARPMirror Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Fir_gprtSetPreemptSize
*
* DESCRIPTION:
*       this routine Set port preemption size of a specified port
*
* INPUTS:
*       port - logical port number
*       size - port preemption size
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
MSD_STATUS Fir_gprtSetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U16     size
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtSetPreemptSize Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		data = size & (MSD_U16)0x3;

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, (MSD_U8)8, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetPreemptSize Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetPreemptSize
*
* DESCRIPTION:
*       This routine gets the port's preemption size.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       size - port preemption size
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_U16    *size
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetPreemptSize Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, (MSD_U8)8, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
		}
		else
		{
			*size = data & (MSD_U16)0x3;
		}
	}

	MSD_DBG_INFO(("Fir_gprtGetPreemptSize Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetPreemptEnable
*
* DESCRIPTION:
*       this routine Set port preemption enable of a specified port
*
* INPUTS:
*       port - logical port number
*       en   - port preemption enable
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
MSD_STATUS Fir_gprtSetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtSetPreemptEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, (MSD_U8)10, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetPreemptEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetPreemptEnable
*
* DESCRIPTION:
*       This routine gets the port's preemption enable.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - port preemption enable
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_BOOL   *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtGetPreemptEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

	MSD_DBG_INFO(("Fir_gprtGetPreemptEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtSetPreemptQueue
*
* DESCRIPTION:
*       this routine Set port preemption queue of a specified port
*
* INPUTS:
*       port - logical port number
*       queue - port preemption queue
*       en    - 1: port��s egress queues to either the port��s Preemptable MAC 
*               0: the port��s Express MAC
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
MSD_STATUS Fir_gprtSetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
IN  MSD_BOOL    en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
	MSD_U16          data = 0;

	MSD_DBG_INFO(("Fir_gprtSetPreemptQueue Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad Queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* translate BOOL to binary */
			MSD_BOOL_2_BIT(en, data);

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, queue, (MSD_U8)1, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
			}
			
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetPreemptQueue Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gprtGetPreemptQueue
*
* DESCRIPTION:
*       This routine gets the port's preemption queue.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       queue - port preemption queue
*
* OUTPUTS:
*       en - the port��s Preemptable MAC or the port��s Express MAC
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
OUT MSD_BOOL    *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Fir_gprtSetPreemptQueue Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad Queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_PREEMPTION_CONTROL, queue, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set FIR_PREEMPTION_CONTROL register error\n"));
			}
			else
			{
				MSD_BIT_2_BOOL(data, *en);
			}
		}
	}

	MSD_DBG_INFO(("Fir_gprtSetPreemptQueue Exit.\n"));
	return retVal;
}

MSD_STATUS Fir_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
)
{
	MSD_STATUS       retVal = MSD_OK; /* Functions return value.      */
	MSD_U16	 data;
	MSD_U8   hwPort;         /* the physical port number     */
	MSD_U8	 phyAddr;
	MSD_U8   i = 0;

	char desc[32][48] =
	{
		{ "Port Status Register(0x0)" },
		{ "Physical Control Register(0x1)" },
		{ "Flow Control Register(0x2)" },
		{ "Switch Identifier Register(0x3)" },
		{ "Port Control0 Register(0x4)" },
		{ "Port Control1 Register(0x5)" },
		{ "Port Based VLAN Map(0x6)" },
		{ "Default Port VLAN ID & Priority(0x7)" },
		{ "Port Control 2 Register(0x8)" },
		{ "Egress Rate Control(0x9)" },
		{ "Egress Rate Control 2(0xa)" },
		{ "Port Association Vector(0xb)" },
		{ "Port ATU Control(0xc)" },
		{ "Override Register(0xd)" },
		{ "Policy & MGMT Control Register(0xe)" },
		{ "Reserved(0xf)" },
		{ "Extended Port Control Operation Register(0x10)" },
		{ "Extended Port Control Data Register(0x11)" },
		{ "Reserved(0x12)" },
		{ "Reserved(0x13)" },
		{ "Reserved(0x14)" },
		{ "Reserved(0x15)" },
		{ "LED Control(0x16)" },
		{ "IP Priority Mapping Table(0x17)" },
		{ "Port IEEE Priority Remapping Registers(0x18)" },
		{ "Port Control 3 Register(0x19)" },
		{ "Reserved(0x1a)" },
		{ "Queue Counter Registers(0x1b)" },
		{ "Port Queue Control Register(0x1c)" },
		{ "Port Queue Control2 Register(0x1d)" },
		{ "Cut Through Register(0x1e)" },
		{ "Debug Counter(0x1f)" }
	};

	MSD_DBG_INFO(("Fir_gprtRegDump Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		MSG(("\n------------------------------------------------------\n"));
		for (i = 0; i < 32U; i++)
		{
			retVal = msdGetAnyReg(dev->devNum, phyAddr, i, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
				retVal = MSD_FAIL;
				break;
			}
			MSG(("%-48s%04x\n", desc[i], data));
		}
	}

	MSD_DBG_INFO(("Fir_gprtRegDump Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                      */
/****************************************************************************/
static MSD_STATUS Fir_writeFlowCtrlReg
(
	IN MSD_QD_DEV	*dev, 
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	IN MSD_U8	data
)
{
	MSD_STATUS    retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8        hwPort;            /* the physical port number     */
	MSD_U8		  phyAddr;
    MSD_U16       count, tmpData;

    MSD_DBG_INFO(("Fir_writeFlowCtrlReg Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_LIMIT_PAUSE_CONTROL, (MSD_U8)15, (MSD_U8)1, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			if (count == 0U)
			{
				tmpData = 0;
				retVal = MSD_FAIL;
			}

			count -= 1U;
		}

		if (retVal != MSD_FAIL)
		{
			tmpData = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x7F) << 8) | data);

			retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_LIMIT_PAUSE_CONTROL, tmpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_INFO(("Failed msdSetAnyReg.\n"));
			}
			
		}
		else
		{
			retVal = MSD_FAIL;
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

	return retVal;
}

static MSD_STATUS Fir_readFlowCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	OUT MSD_U8	*data
)
{
	MSD_STATUS       retVal = MSD_OK;    /* Functions return value.      */
    MSD_U8           hwPort;             /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          tmpData;
    MSD_U16           count = 0x10;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

		do {
			retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_LIMIT_PAUSE_CONTROL, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			if (count == 0U)
			{
				retVal = MSD_FAIL;
				tmpData = 0;
			}
			count -= 1U;

		} while ((tmpData & (MSD_U16)0x8000) != 0U);

		if (retVal != MSD_FAIL)
		{
			tmpData = (MSD_U16)((MSD_U16)(((MSD_U16)pointer & (MSD_U16)0x7F) << 8) | (MSD_U16)0);
			retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_LIMIT_PAUSE_CONTROL, tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				do {
					retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_LIMIT_PAUSE_CONTROL, &tmpData);
					if (retVal != MSD_OK)
					{
						retVal = MSD_FAIL;
						break;
					}
					if (count == 0U)
					{
						retVal = MSD_FAIL;
						tmpData = 0;
					}
					count -= 1U;

				} while ((tmpData & (MSD_U16)0x8000) != 0U);

				if (retVal != MSD_FAIL)
				{
					*data = (MSD_U8)(tmpData & (MSD_U16)0xff);
				}
				else
				{
					retVal = MSD_FAIL;
				}
			}
		}
		else
		{
			retVal = MSD_FAIL;
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

    return retVal;
}

static MSD_STATUS Fir_writeExtendedPortCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8		pointer,
	IN MSD_U16		data
)
{
	MSD_STATUS    retVal = MSD_OK;    /* Functions return value.      */
	MSD_U8        hwPort;             /* the physical port number     */
	MSD_U8		  phyAddr;
	MSD_U16       count, tmpData;

	MSD_DBG_INFO(("Fir_writeExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			if (count == 0U)
			{
				retVal = MSD_FAIL;
				tmpData = 0;
			}

			count -= 1U;
		}

		if (retVal != MSD_FAIL)
		{
			/*set data*/
			retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_DATA, data);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				/*set command*/
				tmpData = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)3 << 12) | (MSD_U16)((MSD_U16)pointer & (MSD_U16)0xFF));
				retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, tmpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_INFO(("Failed msdSetAnyReg.\n"));
				}
				else
				{
					/* wait operation complete */
					count = (MSD_U16)5;
					tmpData = (MSD_U16)1;
					while (tmpData == (MSD_U16)1)
					{
						retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
						if (retVal != MSD_OK)
						{
							retVal = MSD_FAIL;
							break;
						}
						if (count == 0U)
						{
							retVal = MSD_FAIL;
							tmpData = 0;
						}

						count -= 1U;
					}
				}
			}
		}
		else
		{
			retVal = MSD_FAIL;
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

	return retVal;
}

static MSD_STATUS Fir_readExtendedPortCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8		pointer,
	OUT MSD_U16		*data
)
{
	MSD_STATUS    retVal = MSD_OK;   /* Functions return value.      */
	MSD_U8        hwPort;            /* the physical port number     */
	MSD_U8		  phyAddr;
	MSD_U16       count, tmpData;

	MSD_DBG_INFO(("Fir_readExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = FIR_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
				break;
			}
			if (count == (MSD_U16)0)
			{
				retVal = MSD_FAIL;
				tmpData = 0;
			}

			count -= 1U;
		}

		if (retVal != MSD_FAIL)
		{
			/*set command*/
			tmpData = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)4 << 12) | (MSD_U16)((MSD_U16)pointer & (MSD_U16)0xFF));
			retVal = msdSetAnyReg(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, tmpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_INFO(("Failed msdSetAnyReg.\n"));
			}
			else
			{
				/* wait operation complete */
				count = (MSD_U16)5;
				tmpData = (MSD_U16)1;
				while (tmpData == (MSD_U16)1)
				{
					retVal = msdGetAnyRegField(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
					if (retVal != MSD_OK)
					{
						retVal = MSD_FAIL;
						break;
					}
					if (count == (MSD_U16)0)
					{
						retVal = MSD_FAIL;
						tmpData = 0;
					}
					count -= 1U;
				}

				if (retVal != MSD_FAIL)
				{
					/*read data*/
					tmpData = 0;
					retVal = msdGetAnyReg(dev->devNum, phyAddr, FIR_EXT_PORT_CTRL_DATA, &tmpData);
					if (retVal != MSD_OK)
					{
						MSD_DBG_INFO(("Failed msdGetAnyReg.\n"));
					}
					else
					{
						*data = (MSD_U16)tmpData;
					}
				}
			}
		}

		msdSemGive(dev->devNum, dev->tblRegsSem);
	}

	return retVal;
}