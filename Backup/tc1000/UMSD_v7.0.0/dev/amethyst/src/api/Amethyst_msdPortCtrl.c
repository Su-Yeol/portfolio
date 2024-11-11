/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Amethyst_msdPortCtrl.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdPortCtrl.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


#define AMETHYST_MSD_GET_RATE_LIMIT_PER_FRAME(_frames, _dec)    \
        ((_frames)?(1000000000 / (16 * (_frames)) + ((1000000000 % (16 * (_frames)) > (16*(_frames)/2))?1:0)):0)

#define AMETHYST_MSD_GET_RATE_LIMIT_PER_BYTE(_kbps, _dec)    \
        ((_kbps)?((8000000*(_dec)) / (16 * (_kbps)) + ((8000000*(_dec)) % (16 * (_kbps))?1:0)):0)

static MSD_STATUS Amethyst_writeFlowCtrlReg
(
	IN MSD_QD_DEV	*dev, 
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	IN MSD_U8	data
);
static MSD_STATUS Amethyst_readFlowCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	OUT MSD_U8	*data
);

static MSD_STATUS Amethyst_writeExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	IN MSD_U16 data
);
static MSD_STATUS Amethyst_readExtendedPortCtrlReg
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT port,
	IN MSD_U8 pointer,
	OUT MSD_U16 *data
);

/*******************************************************************************
* Amethyst_gvlnSetPortVid
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
MSD_STATUS Amethyst_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gvlnSetPortVid Called.\n"));
    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}	

    if (vid > 0xFFF)
    {
        MSD_DBG_ERROR(("Bad vid. It should be within [0, 0xFFF]\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,AMETHYST_QD_REG_PVID,0,12, vid);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PVID Register.\n"));
        return retVal;
    }
    MSD_DBG_INFO(("Amethyst_gvlnSetPortVid Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gvlnGetPortVid
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
MSD_STATUS Amethyst_gvlnGetPortVid
(
	IN MSD_QD_DEV *dev,
	IN  MSD_LPORT port,
	OUT MSD_U16   *vid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16          data;           /* The register's read data.    */
	MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gvlnGetPortVid Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_PVID, 0, 12, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PVID Register.\n"));
        return retVal;
    }

    *vid = data;

    MSD_DBG_INFO(("Amethyst_gvlnGetPortVid Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetForceDefaultVid
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
MSD_STATUS Amethyst_gprtSetForceDefaultVid
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

	MSD_DBG_INFO(("Amethyst_gprtSetForceDefaultVID Called.\n"));
	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	
	MSD_BOOL_2_BIT(en, data);

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PVID, 12, 1, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PVID Register.\n"));
		return retVal;
	}
	MSD_DBG_INFO(("Amethyst_gprtSetForceDefaultVID Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetForceDefaultVid
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
MSD_STATUS Amethyst_gprtGetForceDefaultVid
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

	MSD_DBG_INFO(("Amethyst_gprtGetForceDefaultVID Called.\n"));

	phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PVID, 12, 1, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PVID Register.\n"));
		return retVal;
	}

	MSD_BIT_2_BOOL(data, *en);

	MSD_DBG_INFO(("Amethyst_gprtGetForceDefaultVID Exit.\n"));
	return MSD_OK;
}

/********************************************************************
* Amethyst_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Amethyst_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN AMETHYST_MSD_8021Q_MODE    mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gvlnSetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    
    switch (mode)
    {
    case AMETHYST_MSD_DISABLE:
    case AMETHYST_MSD_FALLBACK:
    case AMETHYST_MSD_CHECK:
    case AMETHYST_MSD_SECURE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr,AMETHYST_QD_REG_PORT_CONTROL2,10,2,(MSD_U16)mode );
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gvlnSetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Amethyst_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Amethyst_get.
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
MSD_STATUS Amethyst_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT AMETHYST_MSD_8021Q_MODE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U8           phyPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gvlnGetPortVlanDot1qMode Called.\n"));

    phyPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, phyPort);
	if (phyPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    if(mode == NULL)
    {
        MSD_DBG_ERROR(("Failed(bad mode input).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AMETHYST_QD_REG_PORT_CONTROL2,10,2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = data;
    MSD_DBG_INFO(("Amethyst_gvlnGetPortVlanDot1qMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetDiscardTagged
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
MSD_STATUS Amethyst_gprtSetDiscardTagged
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

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 9, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardTagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Amethyst_gets DiscardTagged bit for the given port
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
MSD_STATUS Amethyst_gprtGetDiscardTagged
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

    MSD_DBG_INFO(("Amethyst_gprtGetDiscardTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 9, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetDiscardTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetDiscardUntagged
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
MSD_STATUS Amethyst_gprtSetDiscardUntagged
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

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardUntagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set DiscardUnTagged. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 8, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardUntagged Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Amethyst_gets DiscardUntagged bit for the given port
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
MSD_STATUS Amethyst_gprtGetDiscardUntagged
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

    MSD_DBG_INFO(("Amethyst_gprtGetDiscardUnTagged Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DiscardUnTagged. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 8, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetDiscardUnTagged Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetUnicastFloodBlock
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
MSD_STATUS Amethyst_gprtSetUnicastFloodBlock
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

    MSD_DBG_INFO(("Amethyst_gprtSetUnicastFloodBlock Called.\n"));

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);
    data ^= 1;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 2, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetUnicastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Amethyst_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Amethyst_gprtGetUnicastFloodBlock
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

	MSD_DBG_INFO(("Amethyst_gprtGetUnicastFloodBlock Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 2, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }
    data ^= 1;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetUnicastFloodBlock Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetMulticastFloodBlock
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
MSD_STATUS Amethyst_gprtSetMulticastFloodBlock
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

    MSD_DBG_INFO(("Amethyst_gprtSetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    data ^= 1;

    /* Set DefaultForward. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 3, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetMulticastFloodBlock Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Amethyst_gets DefaultForward bit for the given port
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
MSD_STATUS Amethyst_gprtGetMulticastFloodBlock
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtGetMulticastFloodBlock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the DefaultForward. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 3, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    data ^= 1;

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetMulticastFloodBlock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetDiscardBCastMode
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
MSD_STATUS Amethyst_gprtSetDiscardBCastMode
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

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardBCastMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, 0x19, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDiscardBCastMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Amethyst_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Amethyst_gprtGetDiscardBCastMode
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

	MSD_DBG_INFO(("Amethyst_gprtGetDiscardBCastMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, 0x19, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read Register, phyAddr: %x, regAddr:%x.\n", phyAddr, 0x19));
        return retVal;
    }

	MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetDiscardBCastMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetIGMPSnoopEnable
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
MSD_STATUS Amethyst_gprtSetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Amethyst_gprtSetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 10, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_CT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetIGMPSnoopEnable
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
MSD_STATUS Amethyst_gprtGetIGMPSnoopEnable
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

    MSD_DBG_INFO(("Amethyst_gprtGetIGMPSnoopEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetIGMPSnoopEnable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetHeaderMode
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
MSD_STATUS Amethyst_gprtGetHeaderMode
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

    MSD_DBG_INFO(("Amethyst_gprtGetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetHeaderMode
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
MSD_STATUS Amethyst_gprtSetHeaderMode
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

    MSD_DBG_INFO(("Amethyst_gprtSetHeaderMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetHeaderMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetDropOnLock
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
MSD_STATUS Amethyst_gprtGetDropOnLock
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

    MSD_DBG_INFO(("Amethyst_gprtGetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetDropOnLock
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
MSD_STATUS Amethyst_gprtSetDropOnLock
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

    MSD_DBG_INFO(("Amethyst_gprtSetDropOnLock Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDropOnLock Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - AMETHYST_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Amethyst_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  AMETHYST_MSD_MTU_SIZE   mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetJumboMode Called.\n"));

    if (mode > AMETHYST_MSD_MTU_SIZE_10240)
    {
        MSD_DBG_ERROR(("Bad Parameter\n"));
        return MSD_BAD_PARAM;
    }

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_MSD_MTU_SIZE_1522:
    case AMETHYST_MSD_MTU_SIZE_2048:
    case AMETHYST_MSD_MTU_SIZE_10240:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    /* Set the Jumbo Fram Size bit.               */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 12, 2, (MSD_U16)mode);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Amethyst_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - AMETHYST_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Amethyst_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT AMETHYST_MSD_MTU_SIZE   *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gsysGetJumboMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get Jumbo Frame Mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_MTU_SIZE)data;

    MSD_DBG_INFO(("Amethyst_gsysGetJumboMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetLearnEnable
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
MSD_STATUS Amethyst_gprtSetLearnEnable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{          
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          pav;

    MSD_DBG_INFO(("Amethyst_gprtSetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	/* Set the port's PAV to all zeros */
    if (mode)
        pav = (MSD_U16)(1 << port);
    else
        pav = 0;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 0, 11, pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PAV Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetLearnEnable Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Amethyst_gets LearnDisable setup
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
MSD_STATUS Amethyst_gprtGetLearnEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U16          pav;

    MSD_DBG_INFO(("Amethyst_gprtGetLearnEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 0, 11, &pav);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PAV Register.\n"));
        return retVal;
    }
    if (pav)
		data = 1;
	else
		data = 0;

    /* translate binary to BOOL  */
    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetLearnEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AMETHYST_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtGetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 10, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Amethyst_gprtGetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AMETHYST_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtSetVTUPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_PRI_OVERRIDE_NONE:
    case AMETHYST_PRI_OVERRIDE_FRAME:
    case AMETHYST_PRI_OVERRIDE_QUEUE:
    case AMETHYST_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 10, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetVTUPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AMETHYST_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtGetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 12, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Amethyst_gprtGetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AMETHYST_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtSetSAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_PRI_OVERRIDE_NONE:
    case AMETHYST_PRI_OVERRIDE_FRAME:
    case AMETHYST_PRI_OVERRIDE_QUEUE:
    case AMETHYST_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 12, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetSAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AMETHYST_MSD_PRI_OVERRIDE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtGetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE.\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_PRI_OVERRIDE)data;

    MSD_DBG_INFO(("Amethyst_gprtGetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  AMETHYST_MSD_PRI_OVERRIDE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;
    MSD_U16          data;           /* to keep the read valve       */

    MSD_DBG_INFO(("Amethyst_gprtSetDAPriorityOverride Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_PRI_OVERRIDE_NONE:
    case AMETHYST_PRI_OVERRIDE_FRAME:
    case AMETHYST_PRI_OVERRIDE_QUEUE:
    case AMETHYST_PRI_OVERRIDE_FRAME_QUEUE:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PRI_OVERRIDE, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PRI_OVERRIDE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDAPriorityOverride Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetMessagePort
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
MSD_STATUS Amethyst_gprtGetMessagePort
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

    MSD_DBG_INFO(("Amethyst_gprtGetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetMessagePort
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
MSD_STATUS Amethyst_gprtSetMessagePort
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

    MSD_DBG_INFO(("Amethyst_gprtSetMessagePort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetMessagePort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetEtherType
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
MSD_STATUS Amethyst_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Amethyst_gprtGetEtherType Called.\n"));

	pointer = 0x2;

	retVal = Amethyst_readExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read AMETHYST_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetEtherType
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
MSD_STATUS Amethyst_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           pointer;

    MSD_DBG_INFO(("Amethyst_gprtSetEtherType Called.\n"));

	pointer = 0x2;

	retVal = Amethyst_writeExtendedPortCtrlReg(dev, port, pointer, eType);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Write AMETHYST_REG_PORT_ETH_TYPE Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetEtherType Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetAllowVidZero
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
MSD_STATUS Amethyst_gprtGetAllowVidZero
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

    MSD_DBG_INFO(("Amethyst_gprtGetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 3, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetAllowVidZero Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetAllowVidZero
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
MSD_STATUS Amethyst_gprtSetAllowVidZero
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

    MSD_DBG_INFO(("Amethyst_gprtSetAllowVidZero Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 3, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetAllowVidZero Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetTrunkPort
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
MSD_STATUS Amethyst_gprtSetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      en,
    IN MSD_U32       trunkId
)
{
    MSD_U16          data;           
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    if(en == MSD_TRUE)
    {
        /* need to enable trunk. so check the trunkId */
        if (!AMETHYST_IS_TRUNK_ID_VALID(dev, trunkId))
        {
            MSD_DBG_ERROR(("Failed(bad trunkId).\n"));
            return MSD_BAD_PARAM;
        }

        /* Set TrunkId. */
        retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 8, 5, (MSD_U16)trunkId);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
            return retVal;
        }
    }

    /* Set TrunkPort bit. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetTrunkPort Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetTrunkPort
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
MSD_STATUS Amethyst_gprtGetTrunkPort
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

    MSD_DBG_INFO(("Amethyst_gprtGetTrunkPort Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL1, 8, 5, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_CONTROL1 Register.\n"));
        return retVal;
    }

    *trunkId = (MSD_U32)data;

    MSD_DBG_INFO(("Amethyst_gprtGetTrunkPort Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetFlowCtrl
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
MSD_STATUS Amethyst_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  AMETHYST_MSD_PORT_FC_MODE      mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8			tmpData;

    MSD_DBG_INFO(("Amethyst_gprtSetFlowCtrl Called.\n"));

    switch (mode)
    {
    case Amethyst_PORT_FC_TX_RX_ENABLED:
    case Amethyst_PORT_TX_ONLY:
    case Amethyst_PORT_RX_ONLY:
    case Amethyst_PORT_PFC_ENABLED:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

	retVal = Amethyst_readFlowCtrlReg(dev, port, 0x10, &tmpData);
	if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
        return retVal;
	}

	tmpData &= ~0x07;
	tmpData = (MSD_U8)(tmpData | (1 << 3 ) |((en == MSD_TRUE?1:0) << 2) | mode);

	retVal = Amethyst_writeFlowCtrlReg(dev, port, 0x10, tmpData);
	if(retVal != MSD_OK)
	{
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
        return retVal;
	}

    MSD_DBG_INFO(("Amethyst_gprtSetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Amethyst_get switch port flow control enable/disable status and return 
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
MSD_STATUS Amethyst_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT AMETHYST_MSD_PORT_FC_MODE      *mode
)
{
	MSD_U8			tmpData;
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Amethyst_gprtGetFlowCtrl Called.\n"));

	retVal = Amethyst_readFlowCtrlReg(dev, port, 0x10, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x10, msdDisplayStatus(retVal)));
        return retVal;
    }

	*en = (tmpData >> 2) & 0x1;
	*mode = (AMETHYST_MSD_PORT_FC_MODE)(tmpData & 0x3); 

    MSD_DBG_INFO(("Amethyst_gprtGetFlowCtrl Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Amethyst_gprtSetEgressMonitorSource
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
MSD_STATUS Amethyst_gprtSetEgressMonitorSource
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

    MSD_DBG_INFO(("Amethyst_gprtSetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set EgressMonitorSource. */
	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 5, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set EgressMonitorSource bit error\n"));
        return retVal;
    }
    
    MSD_DBG_INFO(("Amethyst_gprtSetEgressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine Amethyst_gets EgressMonitorSource bit for the given port
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
MSD_STATUS Amethyst_gprtGetEgressMonitorSource
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

    MSD_DBG_INFO(("Amethyst_gprtGetEgressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the EgressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 5, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get EgressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetEgressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetIngressMonitorSource
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
MSD_STATUS Amethyst_gprtSetIngressMonitorSource
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

    MSD_DBG_INFO(("Amethyst_gprtSetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(mode, data);

    /* Set IngressMonitorSource. */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 4, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetIngressMonitorSource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine Amethyst_gets IngressMonitorSource bit for the given port
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
MSD_STATUS Amethyst_gprtGetIngressMonitorSource
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

    MSD_DBG_INFO(("Amethyst_gprtGetIngressMonitorSource Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    /* Get the IngressMonitorSource. */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 4, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get IngressMonitorSource bit error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetIngressMonitorSource Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the AMETHYST_MSD_ELIMIT_MODE of 
*                                AMETHYST_MSD_ELIMIT_LAYER1,
*                                AMETHYST_MSD_ELIMIT_LAYER2, or 
*                                AMETHYST_MSD_ELIMIT_LAYER3 (see Amethyst_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AMETHYST_MSD_ELIMIT_MODE of 
*                                AMETHYST_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Amethyst_grcSetEgressRate
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN AMETHYST_MSD_ELIMIT_MODE mode,
    IN MSD_U32	rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data;
    MSD_U32        eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Amethyst_grcSetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    switch (mode)
    {
    case AMETHYST_MSD_ELIMIT_FRAME:
    case AMETHYST_MSD_ELIMIT_LAYER1:
    case AMETHYST_MSD_ELIMIT_LAYER2:
    case AMETHYST_MSD_ELIMIT_LAYER3:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
        return MSD_BAD_PARAM;
    }

    if((retVal = Amethyst_grcSetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_grcSetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (mode == AMETHYST_MSD_ELIMIT_FRAME)    
    {
        if (rate == 0) /* disable egress rate limit */
        {
            eDec = 0;
            data = 0;
        }
        else if((rate < 3815)  || (rate > 14880000))
        {
            MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
            return MSD_BAD_PARAM;
        }
        else
        {
            eDec = 1;
            data = (MSD_U16)AMETHYST_MSD_GET_RATE_LIMIT_PER_FRAME(rate,eDec);
        }
    }
    else
    {
        if(rate == 0)
        {
            eDec = 0;
        }
        else if(rate < 1000)    /* less than 1Mbps */
        {
            /* it should be divided by 64 */
            if (rate % 64)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/64;
        }
        else if(rate <= 100000)    /* less than or equal to 100Mbps */
        {
            /* it should be divided by 1000 */
            if (rate % 1000)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/1000;
        }
        else if(rate <= 1000000)    /* less than or equal to 1000Mbps */
        {
            /* it should be divided by 10000 */
            if (rate % 10000)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/10000;
        }
        else if(rate <= 10000000)    /* less than or equal to 10Gbps */
        {
            /* it should be divided by 100000 */
            if (rate % 100000)
            {
                MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
                return MSD_BAD_PARAM;
            }
            eDec = rate/100000;
        }
        else
        {
            MSD_DBG_ERROR(("Failed (Bad Rate).\n"));
            return MSD_BAD_PARAM;
        }

        if(rate == 0)
        {
            data = 0;
        }
        else
        {
            data = (MSD_U16)AMETHYST_MSD_GET_RATE_LIMIT_PER_BYTE(rate,eDec);
        }
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_EGRESS_RATE_CTRL, 0, 7, (MSD_U16)eDec);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

	retVal = msdSetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_EGRESS_RATE_CTRL2, 0, 14, (MSD_U16)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to write AMETHYST_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_grcSetEgressRate Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Amethyst_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the AMETHYST_MSD_ELIMIT_MODE of
*                                AMETHYST_MSD_ELIMIT_LAYER1,
*                                AMETHYST_MSD_ELIMIT_LAYER2, or
*                                AMETHYST_MSD_ELIMIT_LAYER3 (see Amethyst_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with AMETHYST_MSD_ELIMIT_MODE of
*                                AMETHYST_MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Amethyst_grcGetEgressRate
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT AMETHYST_MSD_ELIMIT_MODE *mode,
	OUT MSD_U32	*rate
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        eRate, eDec;
    MSD_U8        hwPort,phyAddr;        /* Physical port.               */

    MSD_DBG_INFO(("Amethyst_grcGetEgressRate Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

    if((retVal = Amethyst_grcGetELimitMode(dev,port,mode)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_grcGetELimitMode returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AMETHYST_QD_REG_EGRESS_RATE_CTRL,0,7,&eDec);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_EGRESS_RATE_CTRL Register.\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr,AMETHYST_QD_REG_EGRESS_RATE_CTRL2,0,14,&eRate );
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    if (*mode == AMETHYST_MSD_ELIMIT_FRAME)    
    {
        *rate = AMETHYST_MSD_GET_RATE_LIMIT_PER_FRAME(eRate, eDec);
    }
    else
    {
        /* Count Per Byte */
        *rate = AMETHYST_MSD_GET_RATE_LIMIT_PER_BYTE(eRate, eDec);
    }

    MSD_DBG_INFO(("Amethyst_grcGetEgressRate Exit.\n"));
    return MSD_OK;
}


/*******************************************************************************
* Amethyst_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AMETHYST_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AMETHYST_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AMETHYST_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AMETHYST_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - AMETHYST_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Amethyst_grcSetELimitMode
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
    IN  AMETHYST_MSD_ELIMIT_MODE      mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_grcSetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    data = (MSD_U16)mode & 0x3;

    /* Set the Elimit mode.            */
    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EGRESS_RATE_CTRL2, 14, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_grcSetELimitMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Amethyst_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            AMETHYST_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            AMETHYST_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            AMETHYST_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            AMETHYST_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AMETHYST_MSD_ELIMIT_MODE enum type
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
MSD_STATUS Amethyst_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT AMETHYST_MSD_ELIMIT_MODE  *mode
)
{
    MSD_U16            data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;        /* Physical port.               */
	MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_grcGetELimitMode Called.\n"));

    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    /* Get the Elimit mode.            */
    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EGRESS_RATE_CTRL2, 14, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_EGRESS_RATE_CTRL2 Register.\n"));
        return retVal;
    }

    *mode = data;

    MSD_DBG_INFO(("Amethyst_grcGetELimitMode Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AMETHYST_MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case AMETHYST_MSD_DUPLEX_HALF:
        data = 0x1;
        break;
    case AMETHYST_MSD_DUPLEX_FULL:
        data = 0x3;
        break;
    case AMETHYST_MSD_DUPLEX_AUTO:
        data = 0x0;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 2, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetDuplex Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetDuplex
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AMETHYST_MSD_PORT_FORCE_DUPLEX_MODE  *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetDuplex Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 2, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    switch (data)
    {
    case 0x1:
        *mode = AMETHYST_MSD_DUPLEX_HALF;
        break;
    case 0x3:
        *mode = AMETHYST_MSD_DUPLEX_FULL;
        break;
    case 0:
    case 0x2:
        *mode = AMETHYST_MSD_DUPLEX_AUTO;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
        break;
    }


    MSD_DBG_INFO(("Amethyst_gprtGetDuplex Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetDuplexStatus
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
MSD_STATUS Amethyst_gprtGetDuplexStatus
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	MSD_U16          data;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
	
	MSD_DBG_INFO(("Amethyst_gprtGetDuplexStatus Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_PORT_STATUS, 10, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *mode);

    MSD_DBG_INFO(("Amethyst_gprtGetDuplexStatus Exit.\n"));
	return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT AMETHYST_MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    if ((data & 0x1) == 0)
        *mode = AMETHYST_PORT_DO_NOT_FORCE_LINK;
    else if ((data & 0x3) == 3)
        *mode = AMETHYST_PORT_FORCE_LINK_UP;
    else if ((data & 0x3) == 1)
        *mode = AMETHYST_PORT_FORCE_LINK_DOWN;

    MSD_DBG_INFO(("Amethyst_gprtGetForceLink Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetForceLink
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT AMETHYST_MSD_PORT_FORCE_LINK_MODE  mode
)
{
    MSD_U16          data;
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetForceLink Called.\n"));
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

    switch (mode)
    {
    case AMETHYST_PORT_DO_NOT_FORCE_LINK:
        data = 0;
        break;
    case AMETHYST_PORT_FORCE_LINK_UP:
        data = 3;
        break;
    case AMETHYST_PORT_FORCE_LINK_DOWN:
        data = 1;
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mpde).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetForceLink Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetLinkState
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
MSD_STATUS Amethyst_gprtGetLinkState
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
	
	MSD_DBG_INFO(("Amethyst_gprtGetLinkState Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	/* Get the force flow control bit.  */
	retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_PORT_STATUS, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }

	/* translate binary to BOOL  */
	MSD_BIT_2_BOOL(data, *state);

    MSD_DBG_INFO(("Amethyst_gprtGetLinkState Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed.
*
* INPUTS:
*       port - the logical port number.
*       mode - AMETHYST_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Amethyst_gprtSetForceSpeed
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN AMETHYST_MSD_PORT_FORCED_SPEED_MODE  mode
)
{
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;
    MSD_U16			data1;
    MSD_U16			data2;
    MSD_U16			linkStatus;

    MSD_DBG_INFO(("Amethyst_gprtSetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* Set the ForceSpeed bit.  */
    if (mode == Amethyst_PORT_DO_NOT_FORCE_SPEED)
    {
		/*Get the force link status and then force link down*/
		if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, &linkStatus)) != MSD_OK)
		{
			MSD_DBG_ERROR(("Get link status Failed\n"));
			return retVal;
		}
		if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, 1)) != MSD_OK)
		{
			MSD_DBG_ERROR(("Force Link down failed\n"));
			return retVal;
		}

        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 13, 1, 0)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Force Speed bit error\n"));
            return retVal;
        }
    }
    else
    {
        switch (mode)
        {
        case Amethyst_PORT_FORCE_SPEED_10M:
            data1 = 0;
            data2 = 0;
            break;
        case Amethyst_PORT_FORCE_SPEED_100M:
            data1 = 0;
            data2 = 1;
            break;
        case Amethyst_PORT_FORCE_SPEED_200M:
            if (port != 0)
            {
                MSD_DBG_ERROR(("Failed (Bad Port), Only port 0 support 200M.\n"));
				return MSD_NOT_SUPPORTED;
            }
            data1 = 1;
            data2 = 1;
            break;
        case Amethyst_PORT_FORCE_SPEED_1000M:
            data1 = 0;
            data2 = 2;
            break;
        case Amethyst_PORT_FORCE_SPEED_2_5G:
			if ((port != 0) && (port != 9) && (port != 10))
            {
                MSD_DBG_ERROR(("Failed (Bad Port), Only port 0, 9, 10 support 2.5G.\n"));
				return MSD_NOT_SUPPORTED;
            }
            data1 = 1;
            data2 = 2;
            break;
		case Amethyst_PORT_FORCE_SPEED_5G:
			if ((port != 0) && (port != 9) && (port != 10))
			{
				MSD_DBG_ERROR(("Failed (Bad Port), Only port 0, 9, 10 support 5G.\n"));
				return MSD_NOT_SUPPORTED;
			}
			data1 = 1;
			data2 = 3;
			break;
        case Amethyst_PORT_FORCE_SPEED_10G:
			if ((port != 0) && (port != 9) && (port != 10))
            {
                MSD_DBG_ERROR(("Failed (Bad Port), Only port 0, 9, 10 support 10G.\n"));
				return MSD_NOT_SUPPORTED;
            }
            data1 = 0;
            data2 = 3;
            break;
        default:
            MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
            return MSD_BAD_PARAM;
        }

		/*Get the force link status and then force link down*/
		if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, &linkStatus)) != MSD_OK)
		{
			MSD_DBG_ERROR(("Get link status Failed\n"));
			return retVal;
		}
		if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, 1)) != MSD_OK)
		{
			MSD_DBG_ERROR(("Force Link down failed\n"));
			return retVal;
		}

		/* Set the ForceSpeed bit.  */
		if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 13, 1, 1)) != MSD_OK)
		{
			MSD_DBG_ERROR(("Set Force Speed bit error\n"));
			return retVal;
		}

        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 12, 1, data1)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Alternate Speed bit error\n"));
            return retVal;
        }
        if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 0, 2, data2)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Set Speed bit error\n"));
            return retVal;
        }
    }

    /*Set back the force link status*/
    if ((retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 4, 2, linkStatus)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Set back force link status error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetForceSpeed Called.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AMETHYST_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Amethyst_gprtGetForceSpeed
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT AMETHYST_MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
    MSD_U16          data1;
    MSD_U16          data2;
    MSD_STATUS       retVal;
    MSD_U8           hwPort;
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetForceSpeed Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the ForceSpeed bit.  */
    if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 13, 1, &data1)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Get force Speed bit error\n"));
        return retVal;
    }

    if (data1 == 0)
    {
        *mode = Amethyst_PORT_DO_NOT_FORCE_SPEED;
    }
    else
    {
        if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 12, 1, &data1)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Get Alternate Speed bit error\n"));
            return retVal;
        }
        if ((retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PHY_CONTROL, 0, 2, &data2)) != MSD_OK)
        {
            MSD_DBG_ERROR(("Get Speed bit error\n"));
            return retVal;
        }
        if (data1 == 1)
        {
            switch (data2)
            {
            case 0:
                *mode = Amethyst_PORT_FORCE_SPEED_10M;
                break;
            case 1:
                *mode = Amethyst_PORT_FORCE_SPEED_200M;
                break;
            case 2:
                *mode = Amethyst_PORT_FORCE_SPEED_2_5G;
                break;
            case 3:
                *mode = Amethyst_PORT_FORCE_SPEED_5G;
                break;
            default:
                MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
                return MSD_FAIL;
            }
        }
        else
        {
            switch (data2)
            {
            case 0:
                *mode = Amethyst_PORT_FORCE_SPEED_10M;
                break;
            case 1:
                *mode = Amethyst_PORT_FORCE_SPEED_100M;
                break;
            case 2:
                *mode = Amethyst_PORT_FORCE_SPEED_1000M;
                break;
            case 3:
                *mode = Amethyst_PORT_FORCE_SPEED_10G;
                break;
            default:
                MSD_DBG_ERROR(("Get invalid speed from hardware\n."));
                return MSD_FAIL;
            }
        }
    }

    MSD_DBG_INFO(("Amethyst_gprtGetForceSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - AMETHYST_MSD_PORT_SPEED type.
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
MSD_STATUS Amethyst_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT AMETHYST_MSD_PORT_SPEED   *speed
)
{
	MSD_U16          data, speedEx;
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Amethyst_gprtGetSpeed Called.\n"));	

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if (hwPort == MSD_INVALID_PORT)
	{
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);

	retVal = msdGetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_STATUS, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to read AMETHYST_QD_REG_PORT_STATUS Register.\n"));
        return retVal;
    }
	/* Reg0_13 + Reg0[9:8]
		000 - 10M
		001 - 100M
		101 - 200M
		010 - 1000M
		011 - 10G
		110 - 2.5G
		111 - 5G
	*/
	speedEx = (MSD_U16)(((data & 0x2000) >> 11) | ((data & 0x300) >> 8));

	switch (speedEx)
	{
	case 0:
		*speed = Amethyst_PORT_SPEED_10_MBPS;
		break;
	case 1:
		*speed = Amethyst_PORT_SPEED_100_MBPS;
		break;
	case 5:
		*speed = Amethyst_PORT_SPEED_200_MBPS;
		break;
	case 2:
		*speed = Amethyst_PORT_SPEED_1000_MBPS;
		break;
	case 6:
		*speed = Amethyst_PORT_SPEED_2_5_GBPS;
		break;
	case 7:
		*speed = Amethyst_PORT_SPEED_5_GBPS;
		break;
	case 3:
		*speed = Amethyst_PORT_SPEED_10_GBPS;
		break;
	default:
		*speed = Amethyst_PORT_SPEED_UNKNOWN;
		break;
	}

    MSD_DBG_INFO(("Amethyst_gprtGetSpeed Called.\n"));
	return MSD_OK;
}


/*******************************************************************************
* Amethyst_gprtGetVlanPorts
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
MSD_STATUS Amethyst_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;
    MSD_U8           i;
    MSD_32           portLen = 0;

    MSD_DBG_INFO(("Amethyst_gprtGetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_VLAN_MAP, 0, 11, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PORT_VLAN_MAP register error\n"));
        return retVal;
    }
    for (i = 0; i < dev->numOfPorts; i++)
    {
        if (data & (1 << i))
        {
            memPorts[portLen++] = i;
        }
    }

    *memPortsLen = (MSD_U8)portLen;

    MSD_DBG_INFO(("Amethyst_gprtGetVlanPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetVlanPorts
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
MSD_STATUS Amethyst_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;
    MSD_U8           i;

    MSD_DBG_INFO(("Amethyst_gprtSetVlanPorts Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (memPortsLen > dev->numOfPorts)
    {
        MSD_DBG_ERROR(("Failed (Bad num of vlan ports).\n"));
        return MSD_BAD_PARAM;
    }

    data = 0;
    for (i = 0; i < memPortsLen; i++)
    {
        if (memPorts[i] < dev->numOfPorts)
            data |= (1 << memPorts[i]);
        else
        {
            MSD_DBG_ERROR(("Failed (Bad member port).\n"));
            return MSD_BAD_PARAM;
        }
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_VLAN_MAP, 0, 11, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_VLAN_MAP register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetVlanPorts Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetFrameMode
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
MSD_STATUS Amethyst_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT AMETHYST_MSD_FRAME_MODE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 8, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_FRAME_MODE)data;

    MSD_DBG_INFO(("Amethyst_gprtGetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetFrameMode
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
MSD_STATUS Amethyst_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AMETHYST_MSD_FRAME_MODE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetFrameMode Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_FRAME_MODE_NORMAL:
    case AMETHYST_FRAME_MODE_DSA:
    case AMETHYST_FRAME_MODE_PROVIDER:
    case AMETHYST_FRAME_MODE_ETHER_TYPE_DSA:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 8, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Amethyst_gprtSetFrameMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetPortECID
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
MSD_STATUS Amethyst_gprtSetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_U16 ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Amethyst_gprtSetPortECID Called.\n"));

	pointer = 0x13;

	/*read E-CID register value*/
	retVal = Amethyst_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read AMETHYST_REG_PORT_E-CID Register.\n"));
		return retVal;
	}

	tempValue = (tempValue & 0xF000) | (ecid & 0xFFF);

	retVal = Amethyst_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write AMETHYST_REG_PORT_ECID Register.\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Amethyst_gprtSetPortECID Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetPortECID
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
MSD_STATUS Amethyst_gprtGetPortECID
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_U16 *ecid
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Amethyst_gprtGetPortECID Called.\n"));

	pointer = 0x13;

	retVal = Amethyst_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read AMETHYST_REG_PORT_E-CID Register.\n"));
		return retVal;
	}

	*ecid = tempValue & 0xFFF;

	MSD_DBG_INFO(("Amethyst_gprtGetPortECID Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetIgnoreETag
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
MSD_STATUS Amethyst_gprtSetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	IN  MSD_BOOL en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Amethyst_gprtSetIgnoreETag Called.\n"));

	pointer = 0x13;

	/*read E-CID register value*/
	retVal = Amethyst_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read AMETHYST_REG_PORT_E-CID Register.\n"));
		return retVal;
	}

	tempValue = (MSD_U16)((tempValue & 0xEFFF) | (en << 12));

	retVal = Amethyst_writeExtendedPortCtrlReg(dev, port, pointer, tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write AMETHYST_REG_PORT_ECID Register.\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Amethyst_gprtSetIgnoreETag Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetIgnoreETag
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
MSD_STATUS Amethyst_gprtGetIgnoreETag
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT  MSD_BOOL *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           pointer;
	MSD_U16			 tempValue;

	MSD_DBG_INFO(("Amethyst_gprtGetIgnoreETag Called.\n"));

	pointer = 0x13;

	retVal = Amethyst_readExtendedPortCtrlReg(dev, port, pointer, &tempValue);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read AMETHYST_REG_PORT_E-CID Register.\n"));
		return retVal;
	}

	*en = (tempValue & 0x1000) >> 12;

	MSD_DBG_INFO(("Amethyst_gprtGetIgnoreETag Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetEgressMode
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
MSD_STATUS Amethyst_gprtSetEgressMode
(
	IN MSD_QD_DEV*  dev,
	IN MSD_LPORT  port,
	IN AMETHYST_MSD_EGRESS_MODE mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Amethyst_gprtSetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	switch (mode)
	{
	case AMETHYST_MSD_EGRESS_MODE_UNMODIFIED:
	case AMETHYST_MSD_EGRESS_MODE_UNTAGGED:
	case AMETHYST_MSD_EGRESS_MODE_TAGGED:
		break;
	default:
		MSD_DBG_ERROR(("Failed (Bad mode).\n"));
		return MSD_BAD_PARAM;
	}

	data = (MSD_U16)mode;

	retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 12, 2, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Set AMETHYST_REG_PORT_CONTROL register error\n"));
		return retVal;
	}

	MSD_DBG_INFO(("Amethyst_gprtSetEgressMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetEgressMode
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
MSD_STATUS Amethyst_gprtGetEgressMode
(
	IN  MSD_QD_DEV  *dev,
	IN  MSD_LPORT  port,
	OUT AMETHYST_MSD_EGRESS_MODE  *mode
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16          data;           /* to keep the read valve       */
	MSD_U8			phyAddr;

	MSD_DBG_INFO(("Amethyst_gprtGetEgressMode Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 12, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Get AMETHYST_REG_PORT_CONTROL register error\n"));
		return retVal;
	}

	*mode = (AMETHYST_MSD_EGRESS_MODE)data;

	MSD_DBG_INFO(("Amethyst_gprtGetEgressMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetExtendedPortCtrlReg
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
MSD_STATUS Amethyst_gprtSetExtendedPortCtrlReg
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

	retVal = Amethyst_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	MSD_CALC_MASK(fieldOffset, fieldLength, mask);

	/* Set the desired bits to 0.                       */
	tmpData &= ~mask;
	/* Set the given data into the above reset bits.    */
	tmpData |= ((data << fieldOffset) & mask);

	retVal = Amethyst_writeExtendedPortCtrlReg(dev, portNum, index, tmpData);

	return retVal;
}

/*******************************************************************************
* Amethyst_gprtGetExtendedPortCtrlReg
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
MSD_STATUS Amethyst_gprtGetExtendedPortCtrlReg
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

	retVal = Amethyst_readExtendedPortCtrlReg(dev, portNum, index, &tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}		

	MSD_CALC_MASK(fieldOffset, fieldLength, mask);

	tmpData = (tmpData & mask) >> fieldOffset;
	*data = tmpData;

	return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - AMETHYST_PORT_STP_STATE
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
MSD_STATUS Amethyst_gprtGetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT AMETHYST_MSD_PORT_STP_STATE  *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 0, 2, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }

    *mode = (AMETHYST_MSD_PORT_STP_STATE)data;

    MSD_DBG_INFO(("Amethyst_gprtGetPortState Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - AMETHYST_PORT_STP_STATE
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
MSD_STATUS Amethyst_gprtSetPortState
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  AMETHYST_MSD_PORT_STP_STATE  mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetPortState Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    switch (mode)
    {
    case AMETHYST_MSD_PORT_DISABLE:
    case AMETHYST_MSD_PORT_BLOCKING:
    case AMETHYST_MSD_PORT_LEARNING:
    case AMETHYST_MSD_PORT_FORWARDING:
        break;
    default:
        MSD_DBG_ERROR(("Failed (Bad mode).\n"));
        return MSD_BAD_PARAM;
    }

    data = (MSD_U16)mode;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL, 0, 2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL register error\n"));
        return retVal;
    }


    MSD_DBG_INFO(("Amethyst_gprtSetPortState Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetCutThrouthEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  AMETHYST_CUT_THROUGH  *cutThru
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtSetCutThrouthEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    data = ((cutThru->enable & 0x1) << 8) | cutThru->cutThruQueue;

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_CT_CONTROL, 0, 9, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_CT_CONTROL register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetCutThrouthEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetCutThrouthEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT  AMETHYST_CUT_THROUGH  *cutThru
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;           /* to keep the read valve       */
    MSD_U8			phyAddr;

    MSD_DBG_INFO(("Amethyst_gprtGetCutThrouthEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_CT_CONTROL, 0, 9, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_CT_CONTROL register error\n"));
        return retVal;
    }

    cutThru->enable = (data >> 8) & 0x1;
    cutThru->cutThruQueue = data & 0xff;

    MSD_DBG_INFO(("Amethyst_gprtGetCutThrouthEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetIgnoreFcs
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

    MSD_DBG_INFO(("Amethyst_gprtSetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetIgnoreFcs
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

    MSD_DBG_INFO(("Amethyst_gprtGetIgnoreFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetIgnoreFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetAllowBadFcs
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

    MSD_DBG_INFO(("Amethyst_gprtSetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetAllowBadFcs
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

    MSD_DBG_INFO(("Amethyst_gprtGetAllowBadFcs Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetAllowBadFcs Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtSetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, 0x1, (MSD_U8)limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_readFlowCtrlReg(dev, port, 0x1, (MSD_U8*)limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x1, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtSetPauseLimitIn Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, 0x0, (MSD_U8)limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetPauseLimitIn Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtGetPauseLimitOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_readFlowCtrlReg(dev, port, 0x0, (MSD_U8*)limit);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x0, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetPauseLimitOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8  queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtSetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, 0x17, (MSD_U8)queues);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetQueueToPause Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U8  *queues
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */

    MSD_DBG_INFO(("Amethyst_gprtGetQueueToPause Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_readFlowCtrlReg(dev, port, 0x17, (MSD_U8*)queues);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, 0x17, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetQueueToPause Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetRxPriorityFlowControlEnable
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

    MSD_DBG_INFO(("Amethyst_gprtSetRxPriorityFlowControlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pfcPri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pfcPri / 2 + 0x18);

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BOOL_2_BIT(en, data1);
	if (pfcPri % 2)
    {
        data &= 0x7f;
        data |= (data1 << 7);
    }
    else
    {
        data &= 0xf7;
        data |= (data1 << 3);
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetRxPriorityFlowControlEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetRxPriorityFlowControlEnable
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

    MSD_DBG_INFO(("Amethyst_gprtGetRxPriorityFlowControlEnable Called.\n"));


    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pfcPri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pfcPri / 2 + 0x18);

	if (pfcPri % 2)
        bitMask = 0x80;
    else
        bitMask = 0x08;

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    if ((data & bitMask) != 0)
        *en = MSD_TRUE;
    else
        *en = MSD_FALSE;

    MSD_DBG_INFO(("Amethyst_gprtGetRxPriorityFlowControlEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetRxPriorityFlowControlToQueue
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

    MSD_DBG_INFO(("Amethyst_gprtSetRxPriorityFlowControlToQueue Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pfcPri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
        return MSD_BAD_PARAM;
    }

	if (queue >= AMETHYST_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pfcPri / 2 + 0x18);

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }
	if (pfcPri % 2)
    {
        data &= 0x8f;
        data |= (queue << 4);
    }
    else
    {
        data &= 0xf8;
        data |= queue;
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetRxPriorityFlowControlToQueue Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetRxPriorityFlowControlToQueue
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

    MSD_DBG_INFO(("Amethyst_gprtGetRxPriorityFlowControlToQueue Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pfcPri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pfcPri), pfcPri : %u.\n", pfcPri));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pfcPri / 2 + 0x18);

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }
	if (pfcPri % 2)
    {
        *queue = (data >> 4) & 0x7;
    }
    else
    {
        *queue = data & 0x7;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetRxPriorityFlowControlToQueue Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetTxPriorityFlowControlEnable
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

    MSD_DBG_INFO(("Amethyst_gprtSetTxPriorityFlowControlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pri), pri : %u.\n", pri));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pri / 2 + 0x1c);

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BOOL_2_BIT(en, data1);
	if (pri % 2)
    {
        data &= 0x7f;
        data |= (data1 << 7);
    }
    else
    {
        data &= 0xf7;
        data |= (data1 << 3);
    }

    retVal = Amethyst_writeFlowCtrlReg(dev, port, pointer, (MSD_U8)data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_writeFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetTxPriorityFlowControlEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetTxPriorityFlowControlEnable
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

    MSD_DBG_INFO(("Amethyst_gprtGetTxPriorityFlowControlEnable Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    if (pri > 0x7)
    {
        MSD_DBG_ERROR(("Failed (Bad pri), pri : %u.\n", pri));
        return MSD_BAD_PARAM;
    }

    pointer = (MSD_U8)(pri / 2 + 0x1c);

	if (pri % 2)
        bitMask = 0x80;
    else
        bitMask = 0x08;

    retVal = Amethyst_readFlowCtrlReg(dev, port, pointer, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_readFlowCtrlReg port:%x, reg:%x, returned: %s.\n", port, pointer, msdDisplayStatus(retVal)));
        return retVal;
    }

    if ((data & bitMask) != 0)
        *en = MSD_TRUE;
    else
        *en = MSD_FALSE;

    MSD_DBG_INFO(("Amethyst_gprtGetTxPriorityFlowControlEnable Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetOutQueueSize
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

    MSD_DBG_INFO(("Amethyst_gprtGetOutQueueSize Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

	if (queue >= AMETHYST_MAX_NUM_OF_QUEUES)
    {
        MSD_DBG_ERROR(("Failed (Bad queue), queue : %u.\n", queue));
        return MSD_BAD_PARAM;
    }

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_Q_COUNTER, 12, 4, queue);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_Q_COUNTER, 0, 9, count);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_Q_COUNTER register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtGetOutQueueSize Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetMapDA
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

    MSD_DBG_INFO(("Amethyst_gprtSetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 7, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetMapDA
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

    MSD_DBG_INFO(("Amethyst_gprtGetMapDA Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 7, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetMapDA Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetPortLocked
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

    MSD_DBG_INFO(("Amethyst_gprtSetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 13, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetPortLocked
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

    MSD_DBG_INFO(("Amethyst_gprtGetPortLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 13, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetPortLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetHoldAt1
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

    MSD_DBG_INFO(("Amethyst_gprtSetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 15, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetHoldAt1
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

    MSD_DBG_INFO(("Amethyst_gprtGetHoldAt1 Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 15, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetHoldAt1 Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetInt0nAgeOut
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

    MSD_DBG_INFO(("Amethyst_gprtSetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 14, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetInt0nAgeOut
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

    MSD_DBG_INFO(("Amethyst_gprtGetInt0nAgeOut Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 14, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetInt0nAgeOut Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetRefreshLocked
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

    MSD_DBG_INFO(("Amethyst_gprtSetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 11, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetRefreshLocked
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

    MSD_DBG_INFO(("Amethyst_gprtGetRefreshLocked Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 11, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetRefreshLocked Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetIgnoreWrongData
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

    MSD_DBG_INFO(("Amethyst_gprtSetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 12, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetIgnoreWrongData
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

    MSD_DBG_INFO(("Amethyst_gprtGetIgnoreWrongData Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PAV, 12, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PAV register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetIgnoreWrongData Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtSetARPMirror
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

    MSD_DBG_INFO(("Amethyst_gprtSetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    /* translate BOOL to binary */
    MSD_BOOL_2_BIT(en, data);

    retVal = msdSetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 6, 1, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gprtSetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtGetARPMirror
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

    MSD_DBG_INFO(("Amethyst_gprtGetARPMirror Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
    phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_PORT_CONTROL2, 6, 1, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get AMETHYST_QD_REG_PORT_CONTROL2 register error\n"));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gprtGetARPMirror Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Amethyst_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16	data;
	MSD_U8   hwPort;         /* the physical port number     */
	MSD_U8	phyAddr;
	MSD_U8 i = 0;
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

	MSD_DBG_INFO(("Amethyst_gprtRegDump Called.\n"));
	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		return MSD_BAD_PARAM;
	}

	MSG(("\n------------------------------------------------------\n"));
	for (i = 0; i < 32; i++)
	{
		retVal = msdGetAnyReg(dev->devNum, phyAddr, i, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		MSG(("%-48s%04x\n", desc[i], data));
	}

	MSD_DBG_INFO(("Amethyst_gprtRegDump Exit.\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal functions.                                                      */
/****************************************************************************/
static MSD_STATUS Amethyst_writeFlowCtrlReg
(
	IN MSD_QD_DEV	*dev, 
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	IN MSD_U8	data
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8        hwPort;         /* the physical port number     */
	MSD_U8		 phyAddr;
    MSD_U16   count, tmpData;
    MSD_DBG_INFO(("Amethyst_writeFlowCtrlReg Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    count = 5;
    tmpData = 1;
    while(tmpData ==1)
    {
		retVal = msdGetAnyRegField(dev->devNum,  phyAddr, AMETHYST_QD_REG_LIMIT_PAUSE_CONTROL, 15, 1, &tmpData);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if ((count--) == 0)
		{
			msdSemGive(dev->devNum,  dev->tblRegsSem);
			return MSD_FAIL;
		}
    }

	tmpData =  (MSD_U16)((1 << 15) | ((pointer&0x7F) << 8) | (MSD_U16)data);

	retVal = msdSetAnyReg(dev->devNum,  phyAddr, AMETHYST_QD_REG_LIMIT_PAUSE_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;

}

static MSD_STATUS Amethyst_readFlowCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8	pointer,
	OUT MSD_U8	*data
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16            tmpData;
    int count=0x10;

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    do {
        retVal = msdGetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_LIMIT_PAUSE_CONTROL, &tmpData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
        if ((count--) == 0)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return MSD_FAIL;
        }
    } while (tmpData & 0x8000);

	tmpData =  (MSD_U16)(((pointer&0x7F) << 8) | 0);
	retVal = msdSetAnyReg(dev->devNum,  phyAddr, AMETHYST_QD_REG_LIMIT_PAUSE_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
	   msdSemGive(dev->devNum, dev->tblRegsSem);
	   return retVal;
	}

	do {
		retVal = msdGetAnyReg(dev->devNum,  phyAddr, AMETHYST_QD_REG_LIMIT_PAUSE_CONTROL, &tmpData);
        if(retVal != MSD_OK)
        {
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
        }
		if((count--)==0) 
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
    } while (tmpData&0x8000);

	*data = (MSD_U8)(tmpData & 0xff);
    msdSemGive(dev->devNum, dev->tblRegsSem);

    return retVal;
}

static MSD_STATUS Amethyst_writeExtendedPortCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8		pointer,
	IN MSD_U16		data
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U8        hwPort;         /* the physical port number     */
	MSD_U8		 phyAddr;
	MSD_U16   count, tmpData;
	MSD_DBG_INFO(("Amethyst_writeExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	count = 5;
	tmpData = 1;
	while (tmpData == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, 15, 1, &tmpData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if ((count--) == 0)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
	}

	/*set data*/
	retVal = msdSetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_DATA, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}
	/*set command*/
	tmpData = (MSD_U16)((1 << 15) | (3 << 12) | (pointer & 0xFF));
	retVal = msdSetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}
	/* wait operation complete */
	count = 5;
	tmpData = 1;
	while (tmpData == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, 15, 1, &tmpData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if ((count--) == 0)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;

}
static MSD_STATUS Amethyst_readExtendedPortCtrlReg
(
	IN MSD_QD_DEV	*dev,
	IN MSD_LPORT	port,
	IN MSD_U8		pointer,
	OUT MSD_U16		*data
)
{
	MSD_STATUS    retVal;         /* Functions return value.      */
	MSD_U8        hwPort;         /* the physical port number     */
	MSD_U8		 phyAddr;
	MSD_U16   count, tmpData;
	MSD_DBG_INFO(("Amethyst_readExtendedPortCtrlReg Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = AMETHYST_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if (hwPort == MSD_INVALID_PORT)
	{
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

	count = 5;
	tmpData = 1;
	while (tmpData == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, 15, 1, &tmpData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if ((count--) == 0)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
	}
	/*set command*/
	tmpData = (MSD_U16)((1 << 15) | (4 << 12) | (pointer & 0xFF));
	retVal = msdSetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}
	/* wait operation complete */
	count = 5;
	tmpData = 1;
	while (tmpData == 1)
	{
		retVal = msdGetAnyRegField(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_CMD, 15, 1, &tmpData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return retVal;
		}
		if ((count--) == 0)
		{
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
	}

	/*read data*/
	tmpData = 0;
	retVal = msdGetAnyReg(dev->devNum, phyAddr, AMETHYST_QD_REG_EXTENDED_PORT_CTRL_DATA, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_INFO(("Failed.\n"));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	*data = (MSD_U16)tmpData;

	msdSemGive(dev->devNum, dev->tblRegsSem);
	return retVal;
}