/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Spruce_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdPortCtrl.h>
#include <spruce/include/api/Spruce_msdApiInternal.h>
#include <spruce/include/driver/Spruce_msdHwAccess.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


#define SPRUCE_MSD_GET_RATE_LIMIT_PER_FRAME(_frames, _dec)    \
        ((_frames)?(1000000000 / (16 * (_frames)) + ((1000000000 % (16 * (_frames)) > (16*(_frames)/2))?1:0)):0)

#define SPRUCE_MSD_GET_RATE_LIMIT_PER_BYTE(_kbps, _dec)    \
        ((_kbps)?((8000000*(_dec)) / (16 * (_kbps)) + ((8000000*(_dec)) % (16 * (_kbps))?1:0)):0)

static MSD_STATUS Spruce_writeFlowCtrlReg
(
	IN MSD_QD_DEV	*dev, 
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	IN MSD_U8	data
);
static MSD_STATUS Spruce_readFlowCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	OUT MSD_U8	*data
);

static MSD_STATUS Spruce_writeExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	IN MSD_U16 data
);
static MSD_STATUS Spruce_readExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	OUT MSD_U16 *data
);

/*******************************************************************************
* Spruce_gvlnSetPortVid
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
MSD_STATUS Spruce_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gvlnSetPortVid Called.\n"));
    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
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
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PVID, (MSD_U8)0, (MSD_U8)12, vid);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write SPRUCE_PVID Register.\n"));
			}			
		}
	}

    MSD_DBG_INFO(("Spruce_gvlnSetPortVid Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gvlnGetPortVid
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
MSD_STATUS Spruce_gvlnGetPortVid
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

    MSD_DBG_INFO(("Spruce_gvlnGetPortVid Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PVID, (MSD_U8)0, (MSD_U8)12, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PVID Register.\n"));
		}
		else
		{
			*vid = data;
		}
	}

    MSD_DBG_INFO(("Spruce_gvlnGetPortVid Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetForceDefaultVid
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
MSD_STATUS Spruce_gprtSetForceDefaultVid
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

	MSD_DBG_INFO(("Spruce_gprtSetForceDefaultVID Called.\n"));
	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PVID, (MSD_U8)12, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PVID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetForceDefaultVID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetForceDefaultVid
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
MSD_STATUS Spruce_gprtGetForceDefaultVid
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

	MSD_DBG_INFO(("Spruce_gprtGetForceDefaultVID Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PVID, (MSD_U8)12, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PVID Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

	MSD_DBG_INFO(("Spruce_gprtGetForceDefaultVID Exit.\n"));
	return retVal;
}

/********************************************************************
* Spruce_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Spruce_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN SPRUCE_MSD_8021Q_MODE    mode
)
{
    MSD_STATUS       retVal = MSD_OK;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gvlnSetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case SPRUCE_MSD_DISABLE:
		case SPRUCE_MSD_FALLBACK:
		case SPRUCE_MSD_CHECK:
		case SPRUCE_MSD_SECURE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)10, (MSD_U8)2, (MSD_U16)mode);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gvlnSetPortVlanDot1qMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Spruce_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Spruce_get.
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
MSD_STATUS Spruce_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT SPRUCE_MSD_8021Q_MODE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gvlnGetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)10, (MSD_U8)2, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL2 Register.\n"));
			}
			else
			{
				*mode = (SPRUCE_MSD_8021Q_MODE)data;
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gvlnGetPortVlanDot1qMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetDiscardTagged
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
MSD_STATUS Spruce_gprtSetDiscardTagged
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

    MSD_DBG_INFO(("Spruce_gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)9, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetDiscardTagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Spruce_gets DiscardTagged bit for the given port
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
MSD_STATUS Spruce_gprtGetDiscardTagged
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

    MSD_DBG_INFO(("Spruce_gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DiscardTagged. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)9, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDiscardTagged Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetDiscardUntagged
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
MSD_STATUS Spruce_gprtSetDiscardUntagged
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

    MSD_DBG_INFO(("Spruce_gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)8, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetDiscardUntagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Spruce_gets DiscardUntagged bit for the given port
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
MSD_STATUS Spruce_gprtGetDiscardUntagged
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

    MSD_DBG_INFO(("Spruce_gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DiscardUnTagged. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)8, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDiscardUnTagged Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetUnicastFloodBlock
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
MSD_STATUS Spruce_gprtSetUnicastFloodBlock
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

    MSD_DBG_INFO(("Spruce_gprtSetUnicastFloodBlock Called.\n"));

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    data ^= 1U;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)2, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetUnicastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Spruce_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Spruce_gprtGetUnicastFloodBlock
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

	MSD_DBG_INFO(("Spruce_gprtGetUnicastFloodBlock Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)2, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL Register.\n"));
		}
		else
		{
			data ^= (MSD_U16)1;

			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetUnicastFloodBlock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetMulticastFloodBlock
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
MSD_STATUS Spruce_gprtSetMulticastFloodBlock
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

    MSD_DBG_INFO(("Spruce_gprtSetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)3, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetMulticastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Spruce_gets DefaultForward bit for the given port
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
MSD_STATUS Spruce_gprtGetMulticastFloodBlock
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

    MSD_DBG_INFO(("Spruce_gprtGetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the DefaultForward. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)3, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL Register.\n"));
		}
		else
		{
			data ^= (MSD_U16)1;

			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetMulticastFloodBlock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetDiscardBCastMode
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
MSD_STATUS Spruce_gprtSetDiscardBCastMode
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

    MSD_DBG_INFO(("Spruce_gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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

    MSD_DBG_INFO(("Spruce_gprtSetDiscardBCastMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Spruce_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Spruce_gprtGetDiscardBCastMode
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

	MSD_DBG_INFO(("Spruce_gprtGetDiscardBCastMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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

    MSD_DBG_INFO(("Spruce_gprtGetDiscardBCastMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetIGMPSnoopEnable
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
MSD_STATUS Spruce_gprtSetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Spruce_gprtSetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)10, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_CT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetIGMPSnoopEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetIGMPSnoopEnable
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
MSD_STATUS Spruce_gprtGetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Spruce_gprtGetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetIGMPSnoopEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetHeaderMode
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
MSD_STATUS Spruce_gprtGetHeaderMode
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

    MSD_DBG_INFO(("Spruce_gprtGetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetHeaderMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetHeaderMode
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
MSD_STATUS Spruce_gprtSetHeaderMode
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

    MSD_DBG_INFO(("Spruce_gprtSetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)11, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetHeaderMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetDropOnLock
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
MSD_STATUS Spruce_gprtGetDropOnLock
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

    MSD_DBG_INFO(("Spruce_gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		data = 0;

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDropOnLock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetDropOnLock
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
MSD_STATUS Spruce_gprtSetDropOnLock
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

    MSD_DBG_INFO(("Spruce_gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)14, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetDropOnLock Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - SPRUCE_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Spruce_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  SPRUCE_MSD_MTU_SIZE   mode
)
{
    MSD_STATUS       retVal = MSD_OK;      /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
	MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gprtSetJumboMode Called.\n"));

    if (mode > SPRUCE_MSD_MTU_SIZE_10240)
    {
        MSD_DBG_ERROR(("Bad Parameter(MTU Mode).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate LPORT to hardware port */
		hwPort = MSD_LPORT_2_PORT(port);
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_ERROR(("Failed (Bad Port).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			switch (mode)
			{
			case SPRUCE_MSD_MTU_SIZE_1522:
			case SPRUCE_MSD_MTU_SIZE_2048:
			case SPRUCE_MSD_MTU_SIZE_10240:
				break;
			default:
				MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
				retVal = MSD_BAD_PARAM;
				break;
			}
			if (retVal != MSD_BAD_PARAM)
			{
				/* Set the Jumbo Fram Size bit.               */
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, (MSD_U16)mode);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetJumboMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Spruce_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - SPRUCE_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Spruce_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT SPRUCE_MSD_MTU_SIZE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gsysGetJumboMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get Jumbo Frame Mode.            */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_MTU_SIZE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gsysGetJumboMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetSpecifiedMTU
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
MSD_STATUS Spruce_gprtSetSpecifiedMTU
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

	MSD_DBG_INFO(("Spruce_gprtSetSpecifiedMTU Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		tmpMode = (MSD_U16)0x3;
		/* Set the Jumbo Frame mode bit. */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)12, (MSD_U8)2, (MSD_U16)tmpMode);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			/* Set MTU register */
			retVal = Spruce_writeExtendedPortCtrlReg(dev, (MSD_LPORT)phyAddr, (MSD_U8)0x20, size & (MSD_U16)0x7FFF);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write MTU Register.\n"));
			}
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetSpecifiedMTU Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetSpecifiedMTU
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
MSD_STATUS Spruce_gprtGetSpecifiedMTU
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

	MSD_DBG_INFO(("Spruce_gprtGetSpecifiedMTU Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get MTU register */
		retVal = Spruce_readExtendedPortCtrlReg(dev, (MSD_LPORT)phyAddr, (MSD_U8)0x20, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read MTU Register.\n"));
		}
		else
		{
			*size = data & (MSD_U16)0x7fff;
		}
	}

	MSD_DBG_INFO(("Spruce_gprtGetSpecifiedMTU Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetLearnEnable
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
MSD_STATUS Spruce_gprtSetLearnEnable
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
	MSD_U32          data = 0;

    MSD_DBG_INFO(("Spruce_gprtSetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdGetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)0, (MSD_U8)17, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PAV Register.\n"));
		}
		else
		{
			/* Check port 11, 16 bit*/
			if ((pav & (MSD_U16)0x0800) != 0U)
			{
				data = (data & (MSD_U32)0xF800) | (MSD_U32)(((pav & (MSD_U32)0x0800) >> 11) << 16);
			}
			else
			{
				data = (data & (MSD_U16)0xF800) | pav;
			}

			retVal = msdSetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)0, (MSD_U8)17, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write SPRUCE_PAV Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetLearnEnable Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Spruce_gets LearnDisable setup
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
MSD_STATUS Spruce_gprtGetLearnEnable
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
    MSD_U32          pav;

    MSD_DBG_INFO(("Spruce_gprtGetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)0, (MSD_U8)17, &pav);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PAV Register.\n"));
		}
		else
		{
			if ((pav & (MSD_U32)0x107FF) != 0U)
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

    MSD_DBG_INFO(("Spruce_gprtGetLearnEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtGetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)10, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE Register.\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetVTUPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  SPRUCE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;     /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;                /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtSetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case SPRUCE_PRI_OVERRIDE_NONE:
		case SPRUCE_PRI_OVERRIDE_FRAME:
		case SPRUCE_PRI_OVERRIDE_QUEUE:
		case SPRUCE_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)10, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE Register.\n"));
			}
			
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetVTUPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtGetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE.\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetSAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  SPRUCE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;              /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtSetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case SPRUCE_PRI_OVERRIDE_NONE:
		case SPRUCE_PRI_OVERRIDE_FRAME:
		case SPRUCE_PRI_OVERRIDE_QUEUE:
		case SPRUCE_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)12, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE Register.\n"));
			}
			
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetSAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtGetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)14, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE.\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_PRI_OVERRIDE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDAPriorityOverride Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  SPRUCE_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
    MSD_U16          data;             /* to keep the read valve       */

    MSD_DBG_INFO(("Spruce_gprtSetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case SPRUCE_PRI_OVERRIDE_NONE:
		case SPRUCE_PRI_OVERRIDE_FRAME:
		case SPRUCE_PRI_OVERRIDE_QUEUE:
		case SPRUCE_PRI_OVERRIDE_FRAME_QUEUE:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PRI_OVERRIDE, (MSD_U8)14, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PRI_OVERRIDE Register.\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetDAPriorityOverride Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetMessagePort
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
MSD_STATUS Spruce_gprtGetMessagePort
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

    MSD_DBG_INFO(("Spruce_gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL1 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetMessagePort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetMessagePort
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
MSD_STATUS Spruce_gprtSetMessagePort
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

    MSD_DBG_INFO(("Spruce_gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL1 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetMessagePort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetEtherType
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
MSD_STATUS Spruce_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Spruce_gprtGetEtherType Called.\n"));

	pointer = (MSD_U8)0x2;

	retVal = Spruce_readExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read SPRUCE_REG_PORT_ETH_TYPE Register.\n"));
    }

    MSD_DBG_INFO(("Spruce_gprtGetEtherType Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetEtherType
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
MSD_STATUS Spruce_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Spruce_gprtSetEtherType Called.\n"));

	pointer = (MSD_U8)0x2;

	retVal = Spruce_writeExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Write SPRUCE_REG_PORT_ETH_TYPE Register.\n"));
    }

    MSD_DBG_INFO(("Spruce_gprtSetEtherType Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetAllowVidZero
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
MSD_STATUS Spruce_gprtGetAllowVidZero
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

    MSD_DBG_INFO(("Spruce_gprtGetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)3, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL2 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetAllowVidZero Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetAllowVidZero
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
MSD_STATUS Spruce_gprtSetAllowVidZero
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

    MSD_DBG_INFO(("Spruce_gprtSetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)3, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetAllowVidZero Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetTrunkPort
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
MSD_STATUS Spruce_gprtSetTrunkPort
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

    MSD_DBG_INFO(("Spruce_gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			if (SPRUCE_IS_TRUNK_ID_VALID(dev, trunkId) != 1)
			{
				MSD_DBG_ERROR(("Failed(bad trunkId).\n"));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				/* Set TrunkId. */
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)8, (MSD_U8)5, (MSD_U16)trunkId);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL1 Register.\n"));
				}
			}
		}

		if (retVal == MSD_OK)
		{
			/* Set TrunkPort bit. */
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)14, (MSD_U8)1, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to write SPRUCE_PORT_CONTROL1 Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetTrunkPort Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetTrunkPort
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
MSD_STATUS Spruce_gprtGetTrunkPort
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

    MSD_DBG_INFO(("Spruce_gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		data = 0;
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL1 Register.\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);

			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL1, (MSD_U8)8, (MSD_U8)5, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_CONTROL1 Register.\n"));
			}
			else
			{
				*trunkId = (MSD_U32)data;
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetTrunkPort Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetFlowCtrl
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
MSD_STATUS Spruce_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  SPRUCE_MSD_PORT_FC_MODE      mode
)
{
	MSD_STATUS       retVal = MSD_OK;
	MSD_U8			 tmpData;
	MSD_U8			 tmpVal;

    MSD_DBG_INFO(("Spruce_gprtSetFlowCtrl Called.\n"));

    switch (mode)
    {
    case Spruce_PORT_FC_TX_RX_ENABLED:
    case Spruce_PORT_TX_ONLY:
    case Spruce_PORT_RX_ONLY:
    case Spruce_PORT_PFC_ENABLED:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
		retVal = MSD_BAD_PARAM;
		break;
    }

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = Spruce_readFlowCtrlReg(dev, port, (MSD_U8)0x10, &tmpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
		}
		else
		{
			/* translate BOOL to binary */
			MSD_BOOL_2_BIT(en, tmpVal);

			tmpData &= (MSD_U8)~(MSD_U8)0x07;
			tmpData = (MSD_U8)(tmpData | (MSD_U8)((MSD_U8)1 << 3) | (MSD_U8)(tmpVal << 2) | (MSD_U8)mode);

			retVal = Spruce_writeFlowCtrlReg(dev, port, (MSD_U8)0x10, tmpData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
			}	
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Spruce_get switch port flow control enable/disable status and return 
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
MSD_STATUS Spruce_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT SPRUCE_MSD_PORT_FC_MODE      *mode
)
{
	MSD_U8			tmpData;
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Spruce_gprtGetFlowCtrl Called.\n"));

	retVal = Spruce_readFlowCtrlReg(dev, port, (MSD_U8)0x10, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
    }
	else
	{
		*en = (MSD_BOOL)(MSD_U8)((MSD_U8)(tmpData >> 2) & (MSD_U8)0x1);
		*mode = (SPRUCE_MSD_PORT_FC_MODE)(MSD_U8)(tmpData & (MSD_U8)0x3);
	}

    MSD_DBG_INFO(("Spruce_gprtGetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetEgressMonitorSource
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
MSD_STATUS Spruce_gprtSetEgressMonitorSource
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

    MSD_DBG_INFO(("Spruce_gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)5, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set EgressMonitorSource bit error\n"));
		}
	}
    
    MSD_DBG_INFO(("Spruce_gprtSetEgressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine Spruce_gets EgressMonitorSource bit for the given port
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
MSD_STATUS Spruce_gprtGetEgressMonitorSource
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

    MSD_DBG_INFO(("Spruce_gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the EgressMonitorSource. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)5, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get EgressMonitorSource bit error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetEgressMonitorSource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetIngressMonitorSource
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
MSD_STATUS Spruce_gprtSetIngressMonitorSource
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

    MSD_DBG_INFO(("Spruce_gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)4, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set IngressMonitorSource bit error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetIngressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine Spruce_gets IngressMonitorSource bit for the given port
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
MSD_STATUS Spruce_gprtGetIngressMonitorSource
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

    MSD_DBG_INFO(("Spruce_gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* Get the IngressMonitorSource. */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)4, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get IngressMonitorSource bit error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetIngressMonitorSource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the SPRUCE_MSD_ELIMIT_MODE of 
*                                SPRUCE_MSD_ELIMIT_LAYER1,
*                                SPRUCE_MSD_ELIMIT_LAYER2, or 
*                                SPRUCE_MSD_ELIMIT_LAYER3 (see Spruce_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with SPRUCE_MSD_ELIMIT_MODE of 
*                                SPRUCE_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Spruce_grcSetEgressRate
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN SPRUCE_MSD_ELIMIT_MODE mode,
    IN MSD_U32	rate
)
{
    MSD_STATUS     retVal = MSD_OK;        /* Functions return value.      */
    MSD_U16        data = 0;
    MSD_U32        eDec = 0;
    MSD_U8         hwPort, phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Spruce_grcSetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case SPRUCE_MSD_ELIMIT_FRAME:
		case SPRUCE_MSD_ELIMIT_LAYER1:
		case SPRUCE_MSD_ELIMIT_LAYER2:
		case SPRUCE_MSD_ELIMIT_LAYER3:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = Spruce_grcSetELimitMode(dev, port, mode);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_grcSetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				if (mode == SPRUCE_MSD_ELIMIT_FRAME)
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
						data = (MSD_U16)SPRUCE_MSD_GET_RATE_LIMIT_PER_FRAME(rate, eDec);
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
						data = (MSD_U16)SPRUCE_MSD_GET_RATE_LIMIT_PER_BYTE(rate, eDec);
					}
				}

				if (retVal != MSD_BAD_PARAM)
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL, (MSD_U8)0, (MSD_U8)7, (MSD_U16)eDec);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("ERROR to write SPRUCE_EGRESS_RATE_CTRL Register.\n"));
					}
					else
					{
						retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL2, (MSD_U8)0, (MSD_U8)14, (MSD_U16)data);
						if (retVal != MSD_OK)
						{
							MSD_DBG_ERROR(("ERROR to write SPRUCE_EGRESS_RATE_CTRL2 Register.\n"));
						}
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_grcSetEgressRate Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Spruce_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the SPRUCE_MSD_ELIMIT_MODE of
*                                SPRUCE_MSD_ELIMIT_LAYER1,
*                                SPRUCE_MSD_ELIMIT_LAYER2, or
*                                SPRUCE_MSD_ELIMIT_LAYER3 (see Spruce_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with SPRUCE_MSD_ELIMIT_MODE of
*                                SPRUCE_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Spruce_grcGetEgressRate
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT SPRUCE_MSD_ELIMIT_MODE *mode,
	OUT MSD_U32	*rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        eRate, eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Spruce_grcGetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Spruce_grcGetELimitMode(dev, port, mode);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_grcGetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL, (MSD_U8)0, (MSD_U8)7, &eDec);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_EGRESS_RATE_CTRL Register.\n"));
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL2, (MSD_U8)0, (MSD_U8)14, &eRate);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR to read SPRUCE_EGRESS_RATE_CTRL2 Register.\n"));
				}
				else
				{
					if (*mode == SPRUCE_MSD_ELIMIT_FRAME)
					{
						*rate = (MSD_U32)SPRUCE_MSD_GET_RATE_LIMIT_PER_FRAME(eRate, eDec);
					}
					else
					{
						/* Count Per Byte */
						*rate = (MSD_U32)SPRUCE_MSD_GET_RATE_LIMIT_PER_BYTE(eRate, eDec);
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_grcGetEgressRate Exit.\n"));
	return retVal;
}


/*******************************************************************************
* Spruce_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            SPRUCE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            SPRUCE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            SPRUCE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            SPRUCE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - SPRUCE_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Spruce_grcSetELimitMode
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  SPRUCE_MSD_ELIMIT_MODE      mode
)
{
	MSD_U16            data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Spruce_grcSetELimitMode Called.\n"));

	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
		data = (MSD_U16)mode & (MSD_U16)0x3;

		/* Set the Elimit mode.            */
		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL2, (MSD_U8)14, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_EGRESS_RATE_CTRL2 Register.\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_grcSetELimitMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Spruce_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            SPRUCE_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            SPRUCE_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            SPRUCE_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            SPRUCE_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - SPRUCE_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Spruce_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT SPRUCE_MSD_ELIMIT_MODE  *mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Spruce_grcGetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the Elimit mode.            */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EGRESS_RATE_CTRL2, (MSD_U8)14, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_EGRESS_RATE_CTRL2 Register.\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_ELIMIT_MODE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_grcGetELimitMode Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    MSD_U16          data = 0;
    MSD_U8           hwPort;           /* the physical port number     */
    MSD_U8			 phyAddr;
	MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */

    MSD_DBG_INFO(("Spruce_gprtSetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		switch (mode)
		{
		case SPRUCE_MSD_DUPLEX_HALF:
			data = (MSD_U16)0x1;
			break;
		case SPRUCE_MSD_DUPLEX_FULL:
			data = (MSD_U16)0x3;
			break;
		case SPRUCE_MSD_DUPLEX_AUTO:
			data = 0x0;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)2, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PHY_CONTROL Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetDuplex Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_FORCE_DUPLEX_MODE  *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Spruce_gprtGetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)2, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PHY_CONTROL Register.\n"));
		}
		else
		{
			switch (data)
			{
			case 0x1U:
				*mode = SPRUCE_MSD_DUPLEX_HALF;
				break;
			case 0x3U:
				*mode = SPRUCE_MSD_DUPLEX_FULL;
				break;
			case 0:
			case 0x2U:
				*mode = SPRUCE_MSD_DUPLEX_AUTO;
				break;
			default:
				MSD_DBG_ERROR(("Failed (Bad mode).\n"));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDuplex Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetDuplexStatus
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
MSD_STATUS Spruce_gprtGetDuplexStatus
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
	
	MSD_DBG_INFO(("Spruce_gprtGetDuplexStatus Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the force flow control bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_STATUS, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_STATUS Register.\n"));
		}
		else
		{
			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *mode);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetDuplexStatus Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gprtGetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PHY_CONTROL Register.\n"));
		}
		else
		{
			if ((data & (MSD_U16)0x1) == 0U)
			{
				*mode = SPRUCE_PORT_DO_NOT_FORCE_LINK;
			}
			else if ((data & (MSD_U16)0x3) == 3U)
			{
				*mode = SPRUCE_PORT_FORCE_LINK_UP;
			}
			else if ((data & (MSD_U16)0x3) == 1U)
			{
				*mode = SPRUCE_PORT_FORCE_LINK_DOWN;
			}
			else
			{
				MSD_DBG_ERROR(("ERROR no match force link mode.\n"));
				retVal = MSD_FAIL;
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetForceLink Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT SPRUCE_MSD_PORT_FORCE_LINK_MODE  mode
)
{
    MSD_U16          data = 0;
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U8			 phyAddr;
	MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */

    MSD_DBG_INFO(("Spruce_gprtSetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		switch (mode)
		{
		case SPRUCE_PORT_DO_NOT_FORCE_LINK:
			data = 0;
			break;
		case SPRUCE_PORT_FORCE_LINK_UP:
			data = (MSD_U16)3;
			break;
		case SPRUCE_PORT_FORCE_LINK_DOWN:
			data = (MSD_U16)1;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mpde).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}
		if (retVal != MSD_BAD_PARAM)
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("ERROR to read SPRUCE_PHY_CONTROL Register.\n"));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetForceLink Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetLinkState
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
MSD_STATUS Spruce_gprtGetLinkState
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
	
	MSD_DBG_INFO(("Spruce_gprtGetLinkState Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		/* Get the force flow control bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_STATUS, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_STATUS Register.\n"));
		}
		else
		{
			/* translate binary to BOOL  */
			MSD_BIT_2_BOOL(data, *state);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetLinkState Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - SPRUCE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Spruce_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN SPRUCE_MSD_PORT_FORCED_SPEED_MODE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;
    MSD_U8           hwPort;
    MSD_U8			 phyAddr;
    MSD_U16			 data1 = 0;
    MSD_U16			 data2 = 0;
    MSD_U16			 linkStatus = 0;

    MSD_DBG_INFO(("Spruce_gprtSetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Set the ForceSpeed bit.  */
		if (mode == Spruce_PORT_DO_NOT_FORCE_SPEED)
		{
			/*Get the force link status and then force link down*/
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &linkStatus);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Get link status Failed\n"));
			}
			else
			{
				retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, (MSD_U16)1);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Force Link down failed\n"));
				}
				else
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, (MSD_U16)0);
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
			case Spruce_PORT_FORCE_SPEED_10M:
				data1 = 0;
				data2 = 0;
				break;
			case Spruce_PORT_FORCE_SPEED_100M:
				data1 = 0;
				data2 = (MSD_U16)1;
				break;
			case Spruce_PORT_FORCE_SPEED_200M:
				if ((port != (MSD_LPORT)7) || (port != (MSD_LPORT)8))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 7, 8 support 200M.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)1;
				break;
			case Spruce_PORT_FORCE_SPEED_1000M:
				data1 = 0;
				data2 = (MSD_U16)2;
				break;
			case Spruce_PORT_FORCE_SPEED_2_5G:
				if ((port != (MSD_LPORT)9) && (port != (MSD_LPORT)10) && (port != (MSD_LPORT)11))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 9, 10, 11 support 2.5G.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)2;
				break;
			case Spruce_PORT_FORCE_SPEED_5G:
				if ((port != (MSD_LPORT)9) && (port != (MSD_LPORT)10) && (port != (MSD_LPORT)11))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 9, 10, 11 support 5G.\n"));
					retVal = MSD_NOT_SUPPORTED;
				}
				data1 = (MSD_U16)1;
				data2 = (MSD_U16)3;
				break;
			case Spruce_PORT_FORCE_SPEED_10G:
				if ((port != (MSD_LPORT)9) && (port != (MSD_LPORT)10) && (port != (MSD_LPORT)11))
				{
					MSD_DBG_ERROR(("Failed (Bad Port), Only port 9, 10, 11 support 10G.\n"));
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

			if (retVal == MSD_OK)
			{
				/*Get the force link status and then force link down*/
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, &linkStatus);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get link status Failed\n"));
				}
				else
				{
					retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, (MSD_U16)1);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Force Link down failed\n"));
					}
					else
					{
						/* Set the ForceSpeed bit.  */
						retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, (MSD_U16)1);
						if (retVal != MSD_OK)
						{
							MSD_DBG_ERROR(("Set Force Speed bit error\n"));
						}
						else
						{
							retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)12, (MSD_U8)1, data1);
							if (retVal != MSD_OK)
							{
								MSD_DBG_ERROR(("Set Alternate Speed bit error\n"));
							}
							else
							{
								retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)0, (MSD_U8)2, data2);
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
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)4, (MSD_U8)2, linkStatus);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set back force link status error\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetForceSpeed Called.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - SPRUCE_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Spruce_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
    MSD_U16          data1;
    MSD_U16          data2;
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Spruce_gprtGetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Get the ForceSpeed bit.  */
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)13, (MSD_U8)1, &data1);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get force Speed bit error\n"));
		}
		else
		{
			if (data1 == (MSD_U16)0)
			{
				*mode = Spruce_PORT_DO_NOT_FORCE_SPEED;
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)12, (MSD_U8)1, &data1);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get Alternate Speed bit error\n"));
				}
				else
				{
					retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PHY_CONTROL, (MSD_U8)0, (MSD_U8)2, &data2);
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
								*mode = Spruce_PORT_FORCE_SPEED_10M;
								break;
							case 1U:
								*mode = Spruce_PORT_FORCE_SPEED_200M;
								break;
							case 2U:
								*mode = Spruce_PORT_FORCE_SPEED_2_5G;
								break;
							case 3U:
								*mode = Spruce_PORT_FORCE_SPEED_5G;
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
								*mode = Spruce_PORT_FORCE_SPEED_10M;
								break;
							case 1U:
								*mode = Spruce_PORT_FORCE_SPEED_100M;
								break;
							case 2U:
								*mode = Spruce_PORT_FORCE_SPEED_1000M;
								break;
							case 3U:
								*mode = Spruce_PORT_FORCE_SPEED_10G;
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

    MSD_DBG_INFO(("Spruce_gprtGetForceSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - SPRUCE_MSD_PORT_SPEED type.
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
MSD_STATUS Spruce_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT SPRUCE_MSD_PORT_SPEED   *speed
)
{
	MSD_U16          data, speedEx;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Spruce_gprtGetSpeed Called.\n"));	

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

		retVal = msdGetAnyReg(dev->devNum, phyAddr, SPRUCE_PORT_STATUS, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR to read SPRUCE_PORT_STATUS Register.\n"));
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
				*speed = Spruce_PORT_SPEED_10_MBPS;
				break;
			case 1U:
				*speed = Spruce_PORT_SPEED_100_MBPS;
				break;
			case 5U:
				*speed = Spruce_PORT_SPEED_200_MBPS;
				break;
			case 2U:
				*speed = Spruce_PORT_SPEED_1000_MBPS;
				break;
			case 6U:
				*speed = Spruce_PORT_SPEED_2_5_GBPS;
				break;
			case 7U:
				*speed = Spruce_PORT_SPEED_5_GBPS;
				break;
			case 3U:
				*speed = Spruce_PORT_SPEED_10_GBPS;
				break;
			default:
				*speed = Spruce_PORT_SPEED_UNKNOWN;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetSpeed Called.\n"));
	return retVal;
}


/*******************************************************************************
* Spruce_gprtGetVlanPorts
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
MSD_STATUS Spruce_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U32          tmpData;        /* to keep the read valve       */
    MSD_U8			 phyAddr;
    MSD_U8           i;
    MSD_U8           portLen = 0;
	MSD_U16          data = 0;

    MSD_DBG_INFO(("Spruce_gprtGetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)17, &tmpData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_QD_REG_PORT_VLAN_MAP register error\n"));
		}
		else
		{
			data = (MSD_U16)(tmpData & (MSD_U32)0x07ff) | (MSD_U16)(((tmpData & (MSD_U32)0x10000) >> 16) << 11);

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

    MSD_DBG_INFO(("Spruce_gprtGetVlanPorts Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetVlanPorts
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
MSD_STATUS Spruce_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
)
{
    MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
    MSD_U8           hwPort;            /* the physical port number     */
    MSD_U32          data = 0;          /* to keep the read valve       */
    MSD_U8			 phyAddr;
    MSD_U8           i;
	MSD_U32          tmpData = 0, tmpData1 = 0;


    MSD_DBG_INFO(("Spruce_gprtSetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
					data |= (MSD_U32)(1 << memPorts[i]);
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
				tmpData1 = (data & (MSD_U32)0x7ff) | (((data & (MSD_U32)0x0800) >> 11) << 16);

				/* read back port vlan register */
				retVal = msdGetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)17, &tmpData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get SPRUCE_PORT_VLAN_MAP register error\n"));
				}
				else
				{
					tmpData = (tmpData & (MSD_U16)0xF800) | tmpData1;

					retVal = msdSetAnyExtendedRegField(dev->devNum, phyAddr, SPRUCE_PORT_VLAN_MAP, (MSD_U8)0, (MSD_U8)17, tmpData);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Set SPRUCE_PORT_VLAN_MAP register error\n"));
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetVlanPorts Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetFrameMode
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
MSD_STATUS Spruce_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT SPRUCE_MSD_FRAME_MODE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Spruce_gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)8, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_FRAME_MODE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetFrameMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetFrameMode
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
MSD_STATUS Spruce_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  SPRUCE_MSD_FRAME_MODE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
    MSD_U8           hwPort;           /* the physical port number     */
    MSD_U16          data;             /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case SPRUCE_FRAME_MODE_NORMAL:
		case SPRUCE_FRAME_MODE_DSA:
		case SPRUCE_FRAME_MODE_PROVIDER:
		case SPRUCE_FRAME_MODE_ETHER_TYPE_DSA:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)8, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL register error\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetFrameMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetPortECID
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
MSD_STATUS Spruce_gprtSetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_U16 ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Spruce_gprtSetPortECID Called.\n"));

	pointer = (MSD_U8)0x13;

	/*read E-CID register value*/
	retVal = Spruce_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read SPRUCE_PORT_E-CID Register.\n"));
	}
	else
	{
		tempValue = (tempValue & (MSD_U16)0xF000) | (ecid & (MSD_U16)0xFFF);

		retVal = Spruce_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write SPRUCE_PORT_ECID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetPortECID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetPortECID
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
MSD_STATUS Spruce_gprtGetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_U16 *ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Spruce_gprtGetPortECID Called.\n"));

	pointer = (MSD_U8)0x13;

	retVal = Spruce_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read SPRUCE_PORT_E-CID Register.\n"));
	}
	else
	{
		*ecid = tempValue & (MSD_U16)0xFFF;
	}

	MSD_DBG_INFO(("Spruce_gprtGetPortECID Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetIgnoreETag
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
MSD_STATUS Spruce_gprtSetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_BOOL en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Spruce_gprtSetIgnoreETag Called.\n"));

	pointer = (MSD_U8)0x13;

	/*read E-CID register value*/
	retVal = Spruce_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read SPRUCE_REG_PORT_E-CID Register.\n"));
	}
	else
	{
		tempValue = (MSD_U16)((tempValue & (MSD_U16)0xEFFF) | (MSD_U16)((MSD_U16)en << 12));

		retVal = Spruce_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write SPRUCE_REG_PORT_ECID Register.\n"));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetIgnoreETag Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetIgnoreETag
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
MSD_STATUS Spruce_gprtGetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_BOOL *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Spruce_gprtGetIgnoreETag Called.\n"));

	pointer = (MSD_U8)0x13;

	retVal = Spruce_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read SPRUCE_REG_PORT_E-CID Register.\n"));
	}
	else
	{
		*en = (MSD_BOOL)(MSD_U16)((MSD_U16)(tempValue & (MSD_U16)0x1000) >> 12);
	}

	MSD_DBG_INFO(("Spruce_gprtGetIgnoreETag Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetEgressMode
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
MSD_STATUS Spruce_gprtSetEgressMode
(
	IN MSD_QD_DEV*  dev,
	IN MSD_LPORT  port,
	IN SPRUCE_MSD_EGRESS_MODE mode
)
{
	MSD_STATUS       retVal = MSD_OK;   /* Functions return value.      */
	MSD_U8           hwPort;            /* the physical port number     */
	MSD_U16          data;              /* to keep the read valve       */
	MSD_U8			 phyAddr;

	MSD_DBG_INFO(("Spruce_gprtSetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case SPRUCE_MSD_EGRESS_MODE_UNMODIFIED:
		case SPRUCE_MSD_EGRESS_MODE_UNTAGGED:
		case SPRUCE_MSD_EGRESS_MODE_TAGGED:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)12, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL register error\n"));
			}
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetEgressMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetEgressMode
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
MSD_STATUS Spruce_gprtGetEgressMode
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT SPRUCE_MSD_EGRESS_MODE  *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Spruce_gprtGetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)12, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_EGRESS_MODE)data;
		}
	}

	MSD_DBG_INFO(("Spruce_gprtGetEgressMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetExtendedPortCtrlReg
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
MSD_STATUS Spruce_gprtSetExtendedPortCtrlReg
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

	retVal = Spruce_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_readExtendedPortCtrlReg register error\n"));
	}
	else
	{
		MSD_CALC_MASK(fieldOffset, fieldLength, mask);

		/* Set the desired bits to 0.                       */
		tmpData &= (MSD_U16)~mask;
		/* Set the given data into the above reset bits.    */
		tmpData |= (MSD_U16)(data << fieldOffset) & mask;

		retVal = Spruce_writeExtendedPortCtrlReg(dev, portNum, index, tmpData);
	}

	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetExtendedPortCtrlReg
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
MSD_STATUS Spruce_gprtGetExtendedPortCtrlReg
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

	retVal = Spruce_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Spruce_readExtendedPortCtrlReg register error\n"));
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
* Spruce_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - SPRUCE_PORT_STP_STATE
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
MSD_STATUS Spruce_gprtGetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_STP_STATE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Spruce_gprtGetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)0, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PORT_CONTROL register error\n"));
		}
		else
		{
			*mode = (SPRUCE_MSD_PORT_STP_STATE)data;
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetPortState Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - SPRUCE_PORT_STP_STATE
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
MSD_STATUS Spruce_gprtSetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  SPRUCE_MSD_PORT_STP_STATE  mode
)
{
    MSD_STATUS       retVal = MSD_OK;     /* Functions return value.      */
    MSD_U8           hwPort;              /* the physical port number     */
    MSD_U16          data;                /* to keep the read valve       */
    MSD_U8			 phyAddr;

    MSD_DBG_INFO(("Spruce_gprtSetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		switch (mode)
		{
		case SPRUCE_MSD_PORT_DISABLE:
		case SPRUCE_MSD_PORT_BLOCKING:
		case SPRUCE_MSD_PORT_LEARNING:
		case SPRUCE_MSD_PORT_FORWARDING:
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad mode).\n"));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			data = (MSD_U16)mode;

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL, (MSD_U8)0, (MSD_U8)2, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL register error\n"));
			}	
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetPortState Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetIgnoreFcs
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

    MSD_DBG_INFO(("Spruce_gprtSetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetIgnoreFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetIgnoreFcs
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

    MSD_DBG_INFO(("Spruce_gprtGetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetIgnoreFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetAllowBadFcs
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

    MSD_DBG_INFO(("Spruce_gprtSetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)14, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetAllowBadFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetAllowBadFcs
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

    MSD_DBG_INFO(("Spruce_gprtGetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetAllowBadFcs Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Spruce_gprtSetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Spruce_writeFlowCtrlReg(dev, port, (MSD_U8)0x1, (MSD_U8)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */

	MSD_DBG_INFO(("Spruce_gprtGetPauseLimitOut Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Spruce_readFlowCtrlReg(dev, port, (MSD_U8)0x1, (MSD_U8*)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Spruce_gprtSetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Spruce_writeFlowCtrlReg(dev, port, (MSD_U8)0x0, (MSD_U8)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetPauseLimitIn Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Spruce_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Spruce_readFlowCtrlReg(dev, port, (MSD_U8)0x0, (MSD_U8*)limit);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetPauseLimitOut Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8  queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Spruce_gprtSetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Spruce_writeFlowCtrlReg(dev, port, (MSD_U8)0x17, (MSD_U8)queues);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetQueueToPause Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U8  *queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Spruce_gprtGetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Spruce_readFlowCtrlReg(dev, port, (MSD_U8)0x17, (MSD_U8*)queues);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetQueueToPause Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetRxPriFlowCtrlEnable
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

    MSD_DBG_INFO(("Spruce_gprtSetRxPriFlowCtrlEnable Called.\n"));

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

			retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

				retVal = Spruce_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetRxPriFlowCtrlEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtGetRxPriFlowCtrlEnable
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

    MSD_DBG_INFO(("Spruce_gprtGetRxPriFlowCtrlEnable Called.\n"));


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

			retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

    MSD_DBG_INFO(("Spruce_gprtGetRxPriFlowCtrlEnable Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtSetRxPriFlowCtrlToQueue
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

    MSD_DBG_INFO(("Spruce_gprtSetRxPriFlowCtrlToQueue Called.\n"));

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
			if (queue >= SPRUCE_MAX_NUM_OF_QUEUES)
			{
				MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				pointer = (MSD_U8)((pfcPri / 2U) + (MSD_U8)0x18);

				retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

					retVal = Spruce_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetRxPriFlowCtrlToQueue Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetRxPriFlowCtrlToQueue
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

    MSD_DBG_INFO(("Spruce_gprtGetRxPriFlowCtrlToQueue Called.\n"));

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

			retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

    MSD_DBG_INFO(("Spruce_gprtGetRxPriFlowCtrlToQueue Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetTxPriFlowCtrlEnable
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

    MSD_DBG_INFO(("Spruce_gprtSetTxPriFlowCtrlEnable Called.\n"));

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

			retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

				retVal = Spruce_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Spruce_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetTxPriFlowCtrlEnable Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetTxPriFlowCtrlEnable
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

    MSD_DBG_INFO(("Spruce_gprtGetTxPriFlowCtrlEnable Called.\n"));

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

			retVal = Spruce_readFlowCtrlReg(dev, port, pointer, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
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

    MSD_DBG_INFO(("Spruce_gprtGetTxPriFlowCtrlEnable Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetOutQueueSize
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

    MSD_DBG_INFO(("Spruce_gprtGetOutQueueSize Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= SPRUCE_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_Q_COUNTER, (MSD_U8)12, (MSD_U8)4, (MSD_U16)queue);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_Q_COUNTER register error\n"));
			}
			else
			{
				retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_Q_COUNTER, (MSD_U8)0, (MSD_U8)9, count);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Get SPRUCE_Q_COUNTER register error\n"));
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetOutQueueSize Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetMapDA
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

    MSD_DBG_INFO(("Spruce_gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)7, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetMapDA Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetMapDA
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

    MSD_DBG_INFO(("Spruce_gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)7, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetMapDA Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetPortLocked
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

    MSD_DBG_INFO(("Spruce_gprtSetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)13, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetPortLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetPortLocked
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

    MSD_DBG_INFO(("Spruce_gprtGetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)13, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetPortLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetHoldAt1
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

    MSD_DBG_INFO(("Spruce_gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)15, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetHoldAt1 Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetHoldAt1
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

    MSD_DBG_INFO(("Spruce_gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetHoldAt1 Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetInt0nAgeOut
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

    MSD_DBG_INFO(("Spruce_gprtSetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)14, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetInt0nAgeOut Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetInt0nAgeOut
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

    MSD_DBG_INFO(("Spruce_gprtGetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)14, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetInt0nAgeOut Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetRefreshLocked
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

    MSD_DBG_INFO(("Spruce_gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)11, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetRefreshLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetRefreshLocked
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

    MSD_DBG_INFO(("Spruce_gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)11, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetRefreshLocked Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetIgnoreWrongData
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

    MSD_DBG_INFO(("Spruce_gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)12, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PAV register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetIgnoreWrongData Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetIgnoreWrongData
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

    MSD_DBG_INFO(("Spruce_gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PAV, (MSD_U8)12, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_QD_REG_PAV register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetIgnoreWrongData Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtSetARPMirror
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

    MSD_DBG_INFO(("Spruce_gprtSetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)6, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PORT_CONTROL2 register error\n"));
		}
	}

    MSD_DBG_INFO(("Spruce_gprtSetARPMirror Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gprtGetARPMirror
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

    MSD_DBG_INFO(("Spruce_gprtGetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PORT_CONTROL2, (MSD_U8)6, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Get SPRUCE_PORT_CONTROL2 register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

    MSD_DBG_INFO(("Spruce_gprtGetARPMirror Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Spruce_gprtSetPreemptSize
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
MSD_STATUS Spruce_gprtSetPreemptSize
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

	MSD_DBG_INFO(("Spruce_gprtSetPreemptSize Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		data = size & (MSD_U16)0x3;

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, (MSD_U8)8, (MSD_U8)2, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetPreemptSize Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetPreemptSize
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
MSD_STATUS Spruce_gprtGetPreemptSize
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

	MSD_DBG_INFO(("Spruce_gprtGetPreemptSize Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, (MSD_U8)8, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
		}
		else
		{
			*size = data & (MSD_U16)0x3;
		}
	}

	MSD_DBG_INFO(("Spruce_gprtGetPreemptSize Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetPreemptEnable
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
MSD_STATUS Spruce_gprtSetPreemptEnable
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

	MSD_DBG_INFO(("Spruce_gprtSetPreemptEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, data);

		retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, (MSD_U8)10, (MSD_U8)1, data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetPreemptEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetPreemptEnable
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
MSD_STATUS Spruce_gprtGetPreemptEnable
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

	MSD_DBG_INFO(("Spruce_gprtGetPreemptEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, (MSD_U8)10, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
		}
		else
		{
			MSD_BIT_2_BOOL(data, *en);
		}
	}

	MSD_DBG_INFO(("Spruce_gprtGetPreemptEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtSetPreemptQueue
*
* DESCRIPTION:
*       this routine Set port preemption queue of a specified port
*
* INPUTS:
*       port - logical port number
*       queue - port preemption queue
*       en    - 1: port¡¯s egress queues to either the port¡¯s Preemptable MAC 
*               0: the port¡¯s Express MAC
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
MSD_STATUS Spruce_gprtSetPreemptQueue
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

	MSD_DBG_INFO(("Spruce_gprtSetPreemptQueue Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (queue >= SPRUCE_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad Queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* translate BOOL to binary */
			MSD_BOOL_2_BIT(en, data);

			retVal = msdSetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, queue, (MSD_U8)1, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
			}
			
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetPreemptQueue Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gprtGetPreemptQueue
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
*       en - the port¡¯s Preemptable MAC or the port¡¯s Express MAC
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
MSD_STATUS Spruce_gprtGetPreemptQueue
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

	MSD_DBG_INFO(("Spruce_gprtSetPreemptQueue Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (queue >= SPRUCE_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad Queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_PREEMPTION_CONTROL, queue, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Set SPRUCE_PREEMPTION_CONTROL register error\n"));
			}
			else
			{
				MSD_BIT_2_BOOL(data, *en);
			}
		}
	}

	MSD_DBG_INFO(("Spruce_gprtSetPreemptQueue Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
)
{
	MSD_STATUS retVal = MSD_OK; /* Functions return value.      */
	MSD_U32	 data;
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

	MSD_DBG_INFO(("Spruce_gprtRegDump Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyExtendedReg(dev->devNum, phyAddr, i, &data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
				retVal = MSD_FAIL;
				break;
			}
			MSG(("%-48s%04x\n", desc[i], data));
		}
	}

	MSD_DBG_INFO(("Spruce_gprtRegDump Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal functions.                                                      */
/****************************************************************************/
static MSD_STATUS Spruce_writeFlowCtrlReg
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

    MSD_DBG_INFO(("Spruce_writeFlowCtrlReg Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_LIMIT_PAUSE_CONTROL, (MSD_U8)15, (MSD_U8)1, &tmpData);
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

			retVal = msdSetAnyReg(dev->devNum, phyAddr, SPRUCE_LIMIT_PAUSE_CONTROL, tmpData);
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

static MSD_STATUS Spruce_readFlowCtrlReg
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
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

		do {
			retVal = msdGetAnyReg(dev->devNum, phyAddr, SPRUCE_LIMIT_PAUSE_CONTROL, &tmpData);
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
			retVal = msdSetAnyReg(dev->devNum, phyAddr, SPRUCE_LIMIT_PAUSE_CONTROL, tmpData);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				do {
					retVal = msdGetAnyReg(dev->devNum, phyAddr, SPRUCE_LIMIT_PAUSE_CONTROL, &tmpData);
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

static MSD_STATUS Spruce_writeExtendedPortCtrlReg
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

	MSD_DBG_INFO(("Spruce_writeExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
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
			retVal = msdSetAnyReg(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_DATA, data);
			if (retVal != MSD_OK)
			{
				retVal = MSD_FAIL;
			}
			else
			{
				/*set command*/
				tmpData = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)3 << 12) | (MSD_U16)((MSD_U16)pointer & (MSD_U16)0xFF));
				retVal = msdSetAnyReg(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, tmpData);
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
						retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
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

static MSD_STATUS Spruce_readExtendedPortCtrlReg
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

	MSD_DBG_INFO(("Spruce_readExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = SPRUCE_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
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
			retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
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
			retVal = msdSetAnyReg(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, tmpData);
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
					retVal = msdGetAnyRegField(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_CMD, (MSD_U8)15, (MSD_U8)1, &tmpData);
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
					retVal = msdGetAnyReg(dev->devNum, phyAddr, SPRUCE_EXT_PORT_CTRL_DATA, &tmpData);
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