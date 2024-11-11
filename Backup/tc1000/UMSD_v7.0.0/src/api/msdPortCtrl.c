/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdPortCtrl.c
*
* DESCRIPTION:
*       API definitions for Switch Port Functions and Registers 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdPortCtrl.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdPortForceDefaultVlanIdGet
*
* DESCRIPTION:
*       This routine gets the port ForceDefaultVID.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en  - the port ForceDefaultVID enable.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPortForceDefaultVlanIdGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT portNum,
OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortForceDefaultVlanIdSet
*
* DESCRIPTION:
*       This routine sets the port ForceDefaultVID.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en      - the port ForceDefaultVID enable.
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
*       None
*
*******************************************************************************/
MSD_STATUS msdPortForceDefaultVlanIdSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDefaultVlanIdGet
*
* DESCRIPTION:
*       This routine gets the port default vlan id.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       vlanId  - the port vlan id.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPortDefaultVlanIdGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16  *vlanId
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid(dev, portNum, vlanId);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDefaultVlanIdSet 
*
* DESCRIPTION:
*       This routine sets the port default vlan id.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       vlanId  - the port vlan id.
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
*       None
*
*******************************************************************************/
MSD_STATUS msdPortDefaultVlanIdSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  vlanId
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid(dev, portNum, vlanId);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPort8021qModeGet
*
* DESCRIPTION:
*       This routine gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode  - 802.1q mode for this port
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
MSD_STATUS msdPort8021qModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_8021Q_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/********************************************************************
* gvlnSetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine sets the IEEE 802.1q mode for this port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode  - 802.1q mode for this port
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
MSD_STATUS msdPort8021qModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_8021Q_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardTaggedGet
*
* DESCRIPTION:
*       This routine gets DiscardTagged bit for the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if DiscardTagged bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardTaggedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardTaggedSet
*
* DESCRIPTION:
*       This routine sets DiscardTagged bit for the given port. When this bit is 
*       set to a one, all non-MGMT frames that are processed as Tagged will be 
*       discarded as they enter this switch port. Priority only tagged frames 
*		(with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to discard tagged frame, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardTaggedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardUntaggedGet
*
* DESCRIPTION:
*       This routine gets DiscardUntagged bit for the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*
* OUTPUTS:
*       en  - MSD_TRUE if DiscardUntagged bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardUntaggedGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardUntaggedSet
*
* DESCRIPTION:
*       This routine sets DiscardUntagged bit for the given port. When this bit 
*       is set to a one, all non-MGMT frames that are processed as Untagged will 
*       be discarded as they enter this switch port. Priority only tagged frames 
*		(with a VID of 0x000) are considered tagged.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       en - MSD_TRUE to discard untagged frame, MSD_FALSE otherwise
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
MSD_STATUS msdPortDiscardUntaggedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortUnicastFloodBlockGet
*
* DESCRIPTION:
*       This routine gets egress flooding mode for unitcast frame of a switch 
*       port.
*		When this mode is MSD_TRUE, unicast frame with unknown DA addresses
*       will not egress out this port.
*       When this mode is MSD_FALSE, normal switch operation occurs.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE to do not egress out the unknown DA unicast frames
*            MSD_FALSE for normal operation
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
MSD_STATUS msdPortUnicastFloodBlockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortUnicastFloodBlockSet
*
* DESCRIPTION:
*       This routine sets egress flooding mode for unknown unitcast frame of a switch 
*       port. 
*		When this mode is set to MSD_TRUE, unicast frame with unknown DA addresses
*       will not egress out this port.
*       When this mode is set to MSD_FALSE, normal switch operation occurs.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to do not egress out the unknown DA unicast frames
*            MSD_FALSE for normal operation
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
MSD_STATUS msdPortUnicastFloodBlockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortMulticastFloodBlockGet
*
* DESCRIPTION:
*       This routine gets egress flooding mode of unknown multicast of a switch port.
*       When this mode is set to MSD_TRUE, multicast frames with unknown DA
*       addresses will not egress out this port.
*       when this mode is set to MSD_FALSE, normal switch operation will occurs and
*       multicast frames with unknown DA addresses are allowed to egress out this
*       port (assuming the VLAN settings allow the frame to egress this port too).
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_TRUE  to do not egress out the unknown DA multicast frames and
*               MSD_FALSE for normal switch operation
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
MSD_STATUS msdPortMulticastFloodBlockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortMulticastFloodBlockSet
*
* DESCRIPTION:
*       This routine sets egress flooding mode of unknown multicast of a switch port.
*       When this mode is set to MSD_TRUE, multicast frames with unknown DA
*       addresses will not egress out this port.
*       when this mode is set to MSD_FALSE, normal switch operation will occurs and
*       multicast frames with unknown DA addresses are allowed to egress out this
*       port (assuming the VLAN settings allow the frame to egress this port too).
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_TRUE  to do not egress out the unknown DA multicast frames and 
*               MSD_FALSE for normal switch operation
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
MSD_STATUS msdPortMulticastFloodBlockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardBcastModeGet
*
* DESCRIPTION:
*       This routine gets the discard broadcast mode. If the mode is enabled,
*       all the broadcast frames to the given port will be discarded.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortDiscardBcastModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDiscardBcastModeSet
*
* DESCRIPTION:
*       This routine sets the discard broadcast mode.
*       If the mode is enabled, all the broadcast frames to the given port will
*       be discarded.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortDiscardBcastModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortIGMPSnoopEnableGet
*
* DESCRIPTION:
*       This routine gets the enable status for IPV4 IGMP packets snooping.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortIGMPSnoopEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortIGMPSnoopEnableSet
*
* DESCRIPTION:
*       This routine enable or disable IPV4 IGMP packets snooping.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortIGMPSnoopEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortHeaderModeGet
*
* DESCRIPTION:
*       This routine gets the ingress and egress header mode of a switch port.       
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortHeaderModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode != NULL)
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode(dev, portNum, en);
		else
			retVal = MSD_NOT_SUPPORTED;
	}

	return retVal;
}

/*******************************************************************************
* msdPortHeaderModeSet
*
* DESCRIPTION:
*       This routine sets the ingress and egress header mode of a switch 
*       port. To be used only for port connected to cpu mainly used for layer3
*       32bit alignment.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortHeaderModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDropOnLockGet
*
* DESCRIPTION:
*       This routine gets the DropOnLock mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE, if enabled, MSD_FALSE, otherwise.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortDropOnLockGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDropOnLockSet
*
* DESCRIPTION:
*       This routine sets the Drop on Lock mode of specified port on specified device. 
*       When set to one, Ingress frames will be discarded if their SA field is not in 
*       the ATU's address database of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE, to enable the mode, MSD_FALSE, otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdPortDropOnLockSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortMtuGet
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       devNum  - physical device number
*       port  - the logical port number.
*
* OUTPUTS:
*       size - MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS msdPortMtuGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_MTU_SIZE  *size
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode != NULL)
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode(dev, portNum, size);
		else
			retVal = MSD_NOT_SUPPORTED;
	}

	return retVal;
}

/*******************************************************************************
* msdPortMtuSet
*
* DESCRIPTION:
*       This routine sets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       mode - MTU Size (1522, 2048, or 10240)
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
MSD_STATUS msdPortMtuSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_MTU_SIZE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortSpecifiedMtuGet
*
* DESCRIPTION:
*       This routine gets specified MTU size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       devNum  - physical device number
*       port    - the logical port number.
*
* OUTPUTS:
*       size - specified MTU size
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
MSD_STATUS msdPortSpecifiedMtuGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
OUT MSD_U16   *size
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU(dev, portNum, size);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortSpecifiedMtuSet
*
* DESCRIPTION:
*       This routine sets specified MTU size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number
*       size    - specified MTU size
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
*      This API will change Jumbo mode to 0x3.
*
*******************************************************************************/
MSD_STATUS msdPortSpecifiedMtuSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_U16    size
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU(dev, portNum, size);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortLearnEnableGet
*
* DESCRIPTION:
*       This routine gets learn enable status of specified port on specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE: Learning enabled on the given port ingress frames,
*                 MSD_FALSE otherwise
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
MSD_STATUS msdPortLearnEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortLearnEnableSet
*
* DESCRIPTION:
*       This routine enable or disable auto learn mode of specified port on 
*       specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE: Learning enabled on the given port ingress frames,
*               MSD_FALSE otherwise
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
MSD_STATUS msdPortLearnEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortVTUPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets VTU priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortVTUPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortVTUPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets VTU priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortVTUPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortSAPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets SA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortSAPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortSAPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets SA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortSAPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDAPriorityOverrideGet
*
* DESCRIPTION:
*       This routine gets DA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortDAPriorityOverrideGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PRI_OVERRIDE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortDAPriorityOverrideSet
*
* DESCRIPTION:
*       This routine sets DA priority override of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_PRI_OVERRIDE
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
MSD_STATUS msdPortDAPriorityOverrideSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PRI_OVERRIDE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortMessagePortGet
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
MSD_STATUS msdPortMessagePortGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortMessagePortSet
*
* DESCRIPTION:
*       This routine enable or disable message port for specific port on a specific device. 
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
MSD_STATUS msdPortMessagePortSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEtherTypeGet
*
* DESCRIPTION:
*       This routine gets the port's special Ether Type. This Ether Type is used 
*       for Policy and FrameMode of specified port on specified device
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
MSD_STATUS msdPortEtherTypeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *eType
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType(dev, portNum, eType);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEtherTypeSet
*
* DESCRIPTION:
*       This routine sets the port's special Ether Type. This Ether Type is used 
*       for Policy and FrameMode of specified port on specified device
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
MSD_STATUS msdPortEtherTypeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  eType
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType(dev, portNum, eType);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortAllowVidZeroGet
*
* DESCRIPTION:
*       This routine gets allow VID of Zero mode of specified port on specified device
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
MSD_STATUS msdPortAllowVidZeroGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortAllowVidZeroSet
*
* DESCRIPTION:
*       This routine enable or disable allow VID of Zero of specified port on specified device
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
MSD_STATUS msdPortAllowVidZeroSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortFlowControlEnableGet
*
* DESCRIPTION:
*       This routine gets switch port flow control enable/disable status and return
*       flow control mode
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - enable/disable the flow control
*       mode - flow control mode
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
MSD_STATUS msdPortFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en,
    OUT MSD_PORT_FC_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl(dev, portNum, en, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortFlowControlEnableSet
*
* DESCRIPTION:
*       This routine enable/disable port flow control and set flow control mode
*          mode - PORT_FC_TX_RX_ENABLED,
*                 PORT_RX_ONLY,
*                 PORT_TX_ONLY,
*                 PORT_PFC_ENABLED
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
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
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS msdPortFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en,
    IN  MSD_PORT_FC_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl(dev, portNum, en, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEgressMonitorSourceEnableGet
*
* DESCRIPTION:
*       This routine gets EgressMonitorSource bit for the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE if EgressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortEgressMonitorSourceEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEgressMonitorSourceEnableSet
*
* DESCRIPTION:
*       This routine sets EgressMonitorSource bit for the given port. When this bit 
*       is cleared to a zero, normal network switching occurs. When this bit is set 
*       to a one, any frame that egresses out this port will also be sent to the 
*		EgressMonitorDest Port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*       en - MSD_TRUE to set EgressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS msdPortEgressMonitorSourceEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortIngressMonitorSourceEnableGet
*
* DESCRIPTION:
*       This routine gets IngressMonitorSource bit for the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       en  - MSD_TRUE if IngressMonitorSource bit is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortIngressMonitorSourceEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortIngressMonitorSourceEnableSet
*
* DESCRIPTION:
*       This routine Sets IngressMonitorSource bit for the given port. When this 
*       be is cleared to a zero, normal network switching occurs. When this bit 
*       is set to a one, any frame that ingresses in this port will also be sent 
*		to the IngressMonitorDest Port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*       en - MSD_TRUE to set IngressMonitorSource, MSD_FALSE otherwise
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
MSD_STATUS msdPortIngressMonitorSourceEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEgressRateGet
*
* DESCRIPTION:
*       This routine gets the port's egress rate and count mode.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*
* OUTPUTS:
*       mode - MSD_ELIMIT_MODE
*       rate  - egress rate
*                    rate in kbps that should used with the MSD_ELIMIT_MODE of
*                     MSD_ELIMIT_LAYER1, MSD_ELIMIT_LAYER2, or MSD_ELIMIT_LAYER3
*                     64kbps ~ 1Mbps    : increments of 64kbps,
*                     1Mbps ~ 100Mbps   : increments of 1Mbps, 
*                     100Mbps ~ 1000Mbps: increments of 10Mbps
*                     1Gbps ~ 10Gbps: increments of 100Mbps
*                     Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 10000000.
*                    rate in frame per second that should used with MSD_ELIMIT_MODE
*                     of MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
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
MSD_STATUS msdPortEgressRateGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_ELIMIT_MODE  *mode,
    OUT MSD_U32  *rate
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
		if (dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate(dev, portNum, mode, rate);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortEgressRateSet
*
* DESCRIPTION:
*       This routine sets the port's egress rate.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       mode - MSD_ELIMIT_MODE
*       rate  - egress rate
*                    rate in kbps that should used with the MSD_ELIMIT_MODE of
*                     MSD_ELIMIT_LAYER1, MSD_ELIMIT_LAYER2, or MSD_ELIMIT_LAYER3
*                     64kbps ~ 1Mbps    : increments of 64kbps,
*                     1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                     100Mbps ~ 1000Mbps: increments of 10Mbps
*                     1Gbps ~ 10Gbps: increments of 100Mbps
*                     Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 10000000.
*                    rate in frame per second that should used with MSD_ELIMIT_MODE
*                     of MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
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
*		None
*
*******************************************************************************/
MSD_STATUS msdPortEgressRateSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_ELIMIT_MODE  mode,
    IN  MSD_U32  rate
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
		if (dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate(dev, portNum, mode, rate);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortForceDuplexModeGet
*
* DESCRIPTION:
*       This routine retrives the port force duplex mode.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCE_DUPLEX_MODE
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
MSD_STATUS msdPortForceDuplexModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCE_DUPLEX_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdPortForceDuplexModeSet
*
* DESCRIPTION:
*       This routine sets the port force duplex mode.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*       mode - MSD_PORT_FORCE_DUPLEX_MODE
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortForceDuplexModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCE_DUPLEX_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortDuplexStatusGet
*
* DESCRIPTION:
*       This routine retrives the port duplex status.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_TRUE for Full-duplex  or MSD_FALSE for Half-duplex
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
MSD_STATUS msdPortDuplexStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortForceLinkModeGet
*
* DESCRIPTION:
*       This routine retrives the force link state.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCE_LINK_MODE
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
MSD_STATUS msdPortForceLinkModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCE_LINK_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortForceLinkModeSet
*
* DESCRIPTION:
*       This routine sets the force link state.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*       mode - MSD_PORT_FORCE_LINK_MODE
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortForceLinkModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCE_LINK_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortLinkStatusGet
*
* DESCRIPTION:
*       This routine retrives the force link state.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - the logical port number.
*
* OUTPUTS:
*       state - MSD_TRUE for Up  or MSD_FALSE for Down
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
MSD_STATUS msdPortLinkStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *state
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState(dev, portNum, state);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortForceSpeedModeGet
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       mode - PERIDOT_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS msdPortForceSpeedModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_FORCED_SPEED_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
};

/*******************************************************************************
* msdPortForceSpeedModeSet
*
* DESCRIPTION:
*       This routine sets switch MAC force speed.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - PERIDOT_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
*
* OUTPUTS:
*       None
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
MSD_STATUS msdPortForceSpeedModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_FORCED_SPEED_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortSpeedStatusGet
*
* DESCRIPTION:
*       This routine retrives the port MAC speed.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       speed - MSD_PORT_SPEED type.
*                (PORT_SPEED_10M,PORT_SPEED_100M, PORT_SPEED_1000M, PORT_SPEED_10G
*                etc.)
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
MSD_STATUS msdPortSpeedStatusGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_SPEED  *speed
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

		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed(dev, portNum, speed);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortCutThroughEnableGet
*
* DESCRIPTION:
*       This routine gets port Cut Through configuration of specified port on
*       specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       cutThru - MSD_CUT_THROUGH
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
MSD_STATUS msdPortCutThroughEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_CUT_THROUGH  *cutThru
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable(dev, portNum, cutThru);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortCutThroughEnableSet
*
* DESCRIPTION:
*       This routine sets port Cut Through configuration of specified port on
*       specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       cutThru - MSD_CUT_THROUGH
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
MSD_STATUS msdPortCutThroughEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_CUT_THROUGH  *cutThru
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable(dev, portNum, cutThru);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortIgnoreFcsGet
*
* DESCRIPTION:
*       This routine gets force good FCS enable status. When force good FCS
*       bit is set to a one, the last four bytes of frames received on this port
*       are overwritten with a good CRC and the frames will be accepted by switch.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to set force good fcs, MSD_FALSE otherwise
*
* OUTPUTS:
*       en - MSD_TRUE if force good fcs is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortIgnoreFcsGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortIgnoreFcsSet
*
* DESCRIPTION:
*       This routine enable or disable force good FCS. When force good FCS
*       bit is set to a one, the last four bytes of frames received on this port
*       are overwritten with a good CRC and the frames will be accepted by switch.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to set force good fcs, MSD_FALSE otherwise
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
MSD_STATUS msdPortIgnoreFcsSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortAllowBadFcsGet
*
* DESCRIPTION:
*       This routine gets the configuration for Allow receiving frames on this 
*       port with a bad FCS of specified port on specified deviceand the frames
*       will be accepted by switch.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if allow bad fcs is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortAllowBadFcsGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortAllowBadFcsSet
*
* DESCRIPTION:
*       This routine sets allow bad fcs to Allow receiving frames on this
*       port with a bad FCS of specified port on specified deviceand the frames
*       will be accepted by switch.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to set allow bad fcs, MSD_FALSE otherwise
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
MSD_STATUS msdPortAllowBadFcsSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPauseLimitOutGet
*
* DESCRIPTION:
*       This routine gets pause refresh frames transmit limit for specific port
*       on a specific device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitOutGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *limit
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut(dev, portNum, limit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPauseLimitOutSet
*
* DESCRIPTION:
*       This routine sets pause refresh frames transmit limit for specific port 
*       on a specific device.
*       Set 0 for transmitting continuous pause frame refreshes.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitOutSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  limit
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut(dev, portNum, limit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPauseLimitInGet
*
* DESCRIPTION:
*       This routine gets pause refresh frames receive limit for specific 
*       port on a specific device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitInGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U16  *limit
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn(dev, portNum, limit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPauseLimitInSet
*
* DESCRIPTION:
*       This routine sets pause refresh frames receive limit for specific port 
*       on a specific device. 
*       Set 0 forreceiving continuous pause frame refreshes
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       limit - the max number of Pause refresh frames for each congestion
*               situation( 0 ~ 0xFF)
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
MSD_STATUS msdPortPauseLimitInSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U16  limit
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn(dev, portNum, limit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortQueueToPauseGet
*
* DESCRIPTION:
*       This routine gets QueueToPause value that is used to determine which 
*       queues are paused when receiving a pause frame.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       queues - output queues that are paused off
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
MSD_STATUS msdPortQueueToPauseGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_U8  *queues
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause(dev, portNum, queues);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortQueueToPauseSet
*
* DESCRIPTION:
*       This routine sets QueueToPause value that is used to determine which queues 
*       are paused when receiving a pause frame. When IEEE 802.3x flow control is 
*       enabled on this port and a properly formed Pause frame is received on this 
*		port, this QtoPause value is used to determine which output queues on this 
*		port are Paused off or not.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       queues - output queues that are paused off
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
MSD_STATUS msdPortQueueToPauseSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  queues
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause(dev, portNum, queues);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortRxPriorityFlowControlEnableGet
*
* DESCRIPTION:
*       This routine gets the status if swich port pay attention for the 
*		specified PFC priority information or not when receiving a PFC pause frame.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       en - MSD_TRUE when enable, MSD_FALSE when disable
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
MSD_STATUS msdPortRxPriorityFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable(dev, portNum, pfcPri, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortRxPriorityFlowControlEnableSet
*
* DESCRIPTION:
*       This routine decides if swich port pay attention for the specified PFC 
*		priority information when receiving a PFC pause frame.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       en - MSD_TRUE to enable, MSD_FALSE to disable
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
MSD_STATUS msdPortRxPriorityFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable(dev, portNum, pfcPri, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortRxPriorityFlowControlToQueueGet
*
* DESCRIPTION:
*       This routine Get receive priority flow control frame's Priority X(0~7)
*       to Egress Queue mapping of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       queue - priority queue
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
MSD_STATUS msdPortRxPriorityFlowControlToQueueGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_U8  *queue
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue(dev, portNum, pfcPri, queue);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortRxPriorityFlowControlToQueueSet
*
* DESCRIPTION:
*       This routine Set receive priority flow control frame's Priority X(0~7)
*       to Egress Queue mapping of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       queue - priority queue
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
MSD_STATUS msdPortRxPriorityFlowControlToQueueSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_U8  queue
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue(dev, portNum, pfcPri, queue);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortTxPriorityFlowControlEnableGet
*
* DESCRIPTION:
*       This routine Get transmit priority flow control enable of specified
*       port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*
* OUTPUTS:
*       en - MSD_TRUE when enable, MSD_FALSE when disable
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
MSD_STATUS msdPortTxPriorityFlowControlEnableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable(dev, portNum, pfcPri, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortTxPriorityFlowControlEnableSet
*
* DESCRIPTION:
*       This routine Set transmit priority flow control enable of specified 
*       port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       pfcPri - priority flow control frame's priority
*       en - MSD_TRUE to enable, MSD_FALSE to disable
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
MSD_STATUS msdPortTxPriorityFlowControlEnableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  pfcPri,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable(dev, portNum, pfcPri, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortOutQueueSizeGet
*
* DESCRIPTION:
*       This routine gets egress queue size counter value.This counter reflects
*       the current number of Egress buffers switched to this port and each 
*       queue. The que=0xf is the total number of buffers across all four
*       priority queues.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       queueNum - the queue number
*
* OUTPUTS:
*       count - queue size counter value
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
MSD_STATUS msdPortOutQueueSizeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8  queueNum,
    OUT MSD_U16  *count
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize(dev, portNum, queueNum, count);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortMapDAGet
*
* DESCRIPTION:
*       This routine gets Map DA mode of specified port on specified device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if Map DA is set, MSD_FALSE otherwise
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
MSD_STATUS msdPortMapDAGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortMapDASet
*
* DESCRIPTION:
*       This routine sets map DA mode of specified port on specified device. When 
*       set to zero, the frame will be sent out the ports defined by Egress Floods 
*       subsequently modified by any enabled filetering. Otherwise, normal switch 
*       operation will occur
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to set Map DA bit, MSD_FALSE otherwise
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
MSD_STATUS msdPortMapDASet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortARPMirrorGet
*
* DESCRIPTION:
*       this routine gets Arp Mirror Enable bit of specified port on specified
*       device
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - MSD_TRUE if enable, MSD_FALSE otherwise
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
MSD_STATUS msdPortARPMirrorGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortARPMirrorSet
*
* DESCRIPTION:
*       this routine sets Arp Mirror Enable bit of specified port on specified 
*       device. when set to one non-filtered Tagged or Untagged Frames that 
*       ingress this port that have the Broadcast Destination Address an Ethertype 
*       of 0x0806 are mirrored to the CPUDest port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en - MSD_TRUE to enable, MSD_FALSE otherwise
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
MSD_STATUS msdPortARPMirrorSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortBasedVlanMapGet
*
* DESCRIPTION:
*       this routine gets port based vlan table of a specified port
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
MSD_STATUS msdPortBasedVlanMapGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_LPORT  *memPorts,
    OUT MSD_U8  *memPortsLen
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts(dev, portNum, memPorts, memPortsLen);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortBasedVlanMapSet
*
* DESCRIPTION:
*       this routine sets port based vlan table of a specified port
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
MSD_STATUS msdPortBasedVlanMapSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_LPORT  *memPorts,
    IN  MSD_U8  memPortsLen
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts(dev, portNum, memPorts, memPortsLen);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortFrameModeGet
*
* DESCRIPTION:
*       this routine gets frame mode of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
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
MSD_STATUS msdPortFrameModeGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_FRAME_MODE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortFrameModeSet
*
* DESCRIPTION:
*       This routine sets frame mode of a specified port.Frmae Mode is used to 
*       define the expected Ingress and the generated Egress tagging frame 
*		format for this port as follows:
*            MSD_FRAME_MODE_NORMAL -
*                Normal Network mode uses industry standard IEEE 802.3ac Tagged or 
*                Untagged frames. Tagged frames use an Ether Type of 0x8100.
*            MSD_FRAME_MODE_DSA -
*                DSA mode uses a Marvell defined tagged frame format for 
*                Chip-to-Chip and Chip-to-CPU connections.
*            MSD_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port 
*                (see gprtSetPortEType/gprtGetPortEType API).
*            MSD_FRAME_MODE_ETHER_TYPE_DSA -
*                Ether Type DSA mode uses standard Marvell DSA Tagged frame info 
*                flowing a user definable Ether Type. This mode allows the mixture
*                of Normal Network frames with DSA Tagged frames and is useful to 
*                be used on ports that connect to a CPU.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
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
MSD_STATUS msdPortFrameModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_FRAME_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortEgressModeSet
*
* DESCRIPTION:
*       This routine sets egress mode of a specified port.These bits determine
*       how frames look when they egress this port.Egress mode format for this 
*       port as follows:
*            MSD_EGRESS_MODE_UNMODIFIED -
*                Default to Unmodified mode ¨C frames are transmitted unmodified
*            MSD_EGRESS_MODE_UNTAGGED -
*                Default to Transmit all frames Untagged ¨C remove the tag from any
*                tagged frame.
*            MSD_FRAME_MODE_PROVIDER -
*                Provider mode uses user definable Ether Types per port
*                (see gprtSetPortEType/gprtGetPortEType API).
*            MSD_EGRESS_MODE_TAGGED -
*                Default to Transmit all frames Tagged ¨C add a tag to any untagged
*                frame (the IEEE standard Ether Type of 0x8100 is used).
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
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
MSD_STATUS msdPortEgressModeSet
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_EGRESS_MODE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortEgressModeGet
*
* DESCRIPTION:
*       this routine gets egress mode of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
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
MSD_STATUS msdPortEgressModeGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
OUT MSD_EGRESS_MODE *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortECIDSet
*
* DESCRIPTION:
*       this routine set port E-CID of a specified port
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortECIDSet
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_U16 ecid
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID(dev, portNum, ecid);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortECIDGet
*
* DESCRIPTION:
*       this routine gets port E-CID of a specified port
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
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortECIDGet
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	OUT MSD_U16 *ecid
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID(dev, portNum, ecid);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortIgnoreETagSet
*
* DESCRIPTION:
*       this routine set port ignore E-Tag of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en      - enable or disable port ignore E-Tag
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
MSD_STATUS msdPortIgnoreETagSet
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_BOOL en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortIgnoreETagGet
*
* DESCRIPTION:
*       this routine gets port ignore E-Tag of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - enable or disable port ignore E-Tag
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
MSD_STATUS msdPortIgnoreETagGet
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum,
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortStateGet
*
* DESCRIPTION:
*       This routine gets the port state.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_PORT_STP_STATE
*               MSD_PORT_DISABLE    - port is disabled.
*               MSD_PORT_BLOCKING   - port is in blocking/listening state.
*               MSD_PORT_LEARNING   - port is in learning state.
*               MSD_PORT_FORWARDING - port is in forwarding state.
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
MSD_STATUS msdPortStateGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_STP_STATE  *mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortStateSet
*
* DESCRIPTION:
*       This routine sets the port state.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       mode - MSD_PORT_STP_STATE
*               MSD_PORT_DISABLE    - port is disabled.
*               MSD_PORT_BLOCKING   - port is in blocking/listening state.
*               MSD_PORT_LEARNING   - port is in learning state.
*               MSD_PORT_FORWARDING - port is in forwarding state.
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
MSD_STATUS msdPortStateSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_STP_STATE  mode
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState(dev, portNum, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortRegDump
*
* DESCRIPTION:
*       This routine sets read 32 port registers for a specified port and print
*		them out
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
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
MSD_STATUS msdPortRegDump
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtRegDump != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtRegDump(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortExtendedPortCtrlRegSet
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
MSD_STATUS msdPortExtendedPortCtrlRegSet
(
	IN  MSD_U8     devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_U8     index,
	IN  MSD_U8     fieldOffset,
	IN  MSD_U8     fieldLength,
	IN  MSD_U16    data
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg(dev, portNum, index, fieldOffset, fieldLength, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortExtendedPortCtrlRegGet
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
MSD_STATUS msdPortExtendedPortCtrlRegGet
(
	IN  MSD_U8     devNum,
	IN  MSD_LPORT  portNum,
	IN  MSD_U8     index,
	IN  MSD_U8     fieldOffset,
	IN  MSD_U8     fieldLength,
	OUT  MSD_U16   *data
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg(dev, portNum, index, fieldOffset, fieldLength, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptEnableSet
*
* DESCRIPTION:
*       this routine set port preempt enable of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       en      - enable or disable preempt enable
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
MSD_STATUS msdPortPreemptEnableSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_BOOL en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptEnableGet
*
* DESCRIPTION:
*       this routine gets port preempt enable of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       en - enable or disable port preempt enable
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
MSD_STATUS msdPortPreemptEnableGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable(dev, portNum, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptSizeSet
*
* DESCRIPTION:
*       this routine set port preempt size of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       size    - preempt size
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
MSD_STATUS msdPortPreemptSizeSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_U16    size
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize(dev, portNum, size);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptSizeGet
*
* DESCRIPTION:
*       this routine gets port preempt size of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*
* OUTPUTS:
*       size    - port preempt size
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
MSD_STATUS msdPortPreemptSizeGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
OUT MSD_U16   *size
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize(dev, portNum, size);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptQueueSet
*
* DESCRIPTION:
*       this routine set egress queues connect to preemptable MAC of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       queue   - priority queue
*       en      - enable or disable preemptable MAC
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
MSD_STATUS msdPortPreemptQueueSet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_U8     queue,
IN  MSD_BOOL   en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue(dev, portNum, queue, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdPortPreemptQueueGet
*
* DESCRIPTION:
*       this routine gets port preempt queue enable preemption of a specified port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number
*       queue   - priority queue
*
* OUTPUTS:
*       en      - egress queues connect to connect to preemptable MAC
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
MSD_STATUS msdPortPreemptQueueGet
(
IN  MSD_U8  devNum,
IN  MSD_LPORT  portNum,
IN  MSD_U8     queue,
OUT MSD_BOOL   *en
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
		if (dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue != NULL)
		{
			retVal = dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue(dev, portNum, queue, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}
