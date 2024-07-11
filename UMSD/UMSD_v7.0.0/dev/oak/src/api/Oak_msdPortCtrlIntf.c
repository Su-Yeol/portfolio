/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Oak_msdPortCtrlIntf.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdPortCtrl.h>
#include <oak/include/api/Oak_msdApiInternal.h>
#include <oak/include/driver/Oak_msdHwAccess.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/********************************************************************
* Oak_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Oak_gvlnSetPortVlanQModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
	IN MSD_8021Q_MODE    mode
)
{
	return Oak_gvlnSetPortVlanDot1qMode(dev, port, (OAK_MSD_8021Q_MODE)mode);
}
/*******************************************************************************
* Oak_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Oak_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Oak_get.
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
MSD_STATUS Oak_gvlnGetPortVlanQModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
	OUT MSD_8021Q_MODE   *mode
)
{
	return Oak_gvlnGetPortVlanDot1qMode(dev, port, (OAK_MSD_8021Q_MODE*)mode);
}
/*******************************************************************************
* Oak_gprtSetDiscardTagged
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
MSD_STATUS Oak_gprtSetDiscardTaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Oak_gprtSetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Oak_gets DiscardTagged bit for the given port
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
MSD_STATUS Oak_gprtGetDiscardTaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Oak_gprtGetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtSetDiscardUntagged
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
MSD_STATUS Oak_gprtSetDiscardUntaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Oak_gprtSetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Oak_gets DiscardUntagged bit for the given port
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
MSD_STATUS Oak_gprtGetDiscardUntaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Oak_gprtGetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtSetUnicastFloodBlock
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
MSD_STATUS Oak_gprtSetUnicFloodBlockIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Oak_gprtSetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Oak_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Oak_gprtGetUnicFloodBlockIntf
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
)
{
	return Oak_gprtGetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Oak_gprtSetMulticastFloodBlock
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
MSD_STATUS Oak_gprtSetMultiFloodBlockIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Oak_gprtSetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Oak_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Oak_gets DefaultForward bit for the given port
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
MSD_STATUS Oak_gprtGetMultiFloodBlockIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Oak_gprtGetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Oak_gprtSetDiscardBCastMode
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
MSD_STATUS Oak_gprtSetDiscardBCastModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
	return Oak_gprtSetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Oak_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Oak_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Oak_gprtGetDiscardBCastModeIntf
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
)
{
	return Oak_gprtGetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Oak_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Oak_gprtSetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	IN  MSD_MTU_SIZE   mode
)
{
	return Oak_gprtSetJumboMode(dev, port, (OAK_MSD_MTU_SIZE)mode);
}

/*******************************************************************************
* Oak_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Oak_gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS Oak_gprtGetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	OUT MSD_MTU_SIZE   *mode
)
{
	return Oak_gprtGetJumboMode(dev, port, (OAK_MSD_MTU_SIZE*)mode);
}

/*******************************************************************************
* Oak_gprtSetSpecifiedMTUIntf
*
* DESCRIPTION:
*       This routine set specified MTU size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       size - MTU size
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
MSD_STATUS Oak_gprtSetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
)
{
	return Oak_gprtSetSpecifiedMTU(dev, port, size);
}

/*******************************************************************************
* Oak_gprtGetSpecifiedMTUIntf
*
* DESCRIPTION:
*       This routine gets specified MTU size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       size  - MTU size
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
MSD_STATUS Oak_gprtGetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_U16      *size
)
{
	return Oak_gprtGetSpecifiedMTU(dev, port, size);
}

/*******************************************************************************
* Oak_gprtSetLearnEnable
*
* DESCRIPTION:
*       This routine enables/disables automatic learning of new source MAC
*       addresses on the given port ingress
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_TRUE for disable or MSD_FALSE otherwise
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
MSD_STATUS Oak_gprtSetLearnEnableIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Oak_gprtSetLearnEnable(dev, port, mode);
}

/*******************************************************************************
* Oak_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Oak_gets LearnDisable setup
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
MSD_STATUS Oak_gprtGetLearnEnableIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
	return Oak_gprtGetLearnEnable(dev, port, mode);
}

MSD_STATUS Oak_gprtGetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Oak_gprtGetVTUPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Oak_gprtSetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Oak_gprtSetVTUPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Oak_gprtGetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Oak_gprtGetSAPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Oak_gprtSetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Oak_gprtSetSAPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Oak_gprtGetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Oak_gprtGetDAPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Oak_gprtSetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Oak_gprtSetDAPriorityOverride(dev, port, (OAK_MSD_PRI_OVERRIDE)mode);
}


/*******************************************************************************
* Oak_gprtSetFlowCtrl
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
MSD_STATUS Oak_gprtSetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  MSD_PORT_FC_MODE      mode
)
{
	return Oak_gprtSetFlowCtrl(dev, port, en, (OAK_MSD_PORT_FC_MODE)mode);
}
/*******************************************************************************
* Oak_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Oak_get switch port flow control enable/disable status and return 
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
MSD_STATUS Oak_gprtGetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT MSD_PORT_FC_MODE      *mode
)
{
	return Oak_gprtGetFlowCtrl(dev, port, en, (OAK_MSD_PORT_FC_MODE*)mode);
}

/*******************************************************************************
* Oak_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed. 
*
* INPUTS:
*       port - the logical port number.
*       mode - MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Oak_gprtSetForceSpeedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
	IN MSD_PORT_FORCED_SPEED_MODE  mode
)
{
	OAK_MSD_PORT_FORCED_SPEED_MODE oakMode;
	switch (mode)
	{
	case MSD_PORT_FORCE_SPEED_10M:
		oakMode = Oak_PORT_FORCE_SPEED_10M;
		break;
	case MSD_PORT_FORCE_SPEED_100M:
		oakMode = Oak_PORT_FORCE_SPEED_100M;
		break;
	case MSD_PORT_FORCE_SPEED_200M:
		oakMode = Oak_PORT_FORCE_SPEED_200M;
		break;
	case MSD_PORT_FORCE_SPEED_1000M:
		oakMode = Oak_PORT_FORCE_SPEED_1000M;
		break;
	case MSD_PORT_FORCE_SPEED_2_5G:
		oakMode = Oak_PORT_FORCE_SPEED_2_5G;
		break;
	case MSD_PORT_FORCE_SPEED_5G:
		oakMode = Oak_PORT_FORCE_SPEED_5G;
		break;
	case MSD_PORT_FORCE_SPEED_10G:
		oakMode = Oak_PORT_FORCE_SPEED_10G;
		break;
	case MSD_PORT_DO_NOT_FORCE_SPEED:
		oakMode = Oak_PORT_DO_NOT_FORCE_SPEED;
		break;
	default:
		MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
		return MSD_BAD_PARAM;
		break;
	}

	return Oak_gprtSetForceSpeed(dev, port, oakMode);
}

/*******************************************************************************
* Oak_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS Oak_gprtGetForceSpeedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
	OUT MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
	return Oak_gprtGetForceSpeed(dev, port, (OAK_MSD_PORT_FORCED_SPEED_MODE*)mode);
}

/*******************************************************************************
* Oak_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the MSD_ELIMIT_MODE of 
*                                MSD_ELIMIT_LAYER1,
*                                MSD_ELIMIT_LAYER2, or 
*                                MSD_ELIMIT_LAYER3 (see Oak_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with MSD_ELIMIT_MODE of 
*                                MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Oak_grcSetEgressRateIntf
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN MSD_ELIMIT_MODE   mode,
    IN MSD_U32   rate
)
{
    return Oak_grcSetEgressRate(dev, port, (OAK_MSD_ELIMIT_MODE)mode, rate);
}

/*******************************************************************************
* Oak_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Oak_gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the MSD_ELIMIT_MODE of
*                                MSD_ELIMIT_LAYER1,
*                                MSD_ELIMIT_LAYER2, or
*                                MSD_ELIMIT_LAYER3 (see Oak_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
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
MSD_STATUS Oak_grcGetEgressRateIntf
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_ELIMIT_MODE   *mode,
    OUT MSD_U32   *rate
)
{
    return Oak_grcGetEgressRate(dev, port, (OAK_MSD_ELIMIT_MODE*)mode, rate);
}


/*******************************************************************************
* Oak_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            MSD_ELIMIT_FRAME -
*                Count the number of frames
*            MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - MSD_ELIMIT_MODE enum type
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
MSD_STATUS Oak_grcSetELimitModeIntf
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
	IN  MSD_ELIMIT_MODE      mode
)
{
	return Oak_grcSetELimitMode(dev, port, (OAK_MSD_ELIMIT_MODE)mode);
}


/*******************************************************************************
* Oak_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Oak_gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            MSD_ELIMIT_FRAME -
*                Count the number of frames
*            MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - MSD_ELIMIT_MODE enum type
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
MSD_STATUS Oak_grcGetELimitModeIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
	OUT MSD_ELIMIT_MODE  *mode
)
{
	return Oak_grcGetELimitMode(dev, port, (OAK_MSD_ELIMIT_MODE*)mode);
}

MSD_STATUS Oak_gprtSetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    return Oak_gprtSetDuplex(dev, port, (OAK_MSD_PORT_FORCE_DUPLEX_MODE)mode);
}

MSD_STATUS Oak_gprtGetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
)
{
    return Oak_gprtGetDuplex(dev, port, (OAK_MSD_PORT_FORCE_DUPLEX_MODE*)mode);
}

/*******************************************************************************
* Oak_gprtGetDuplexStatus
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
MSD_STATUS Oak_gprtGetDuplexStatusIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	return Oak_gprtGetDuplexStatus(dev, port, mode);
}

MSD_STATUS Oak_gprtGetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    return Oak_gprtGetForceLink(dev, port, (OAK_MSD_PORT_FORCE_LINK_MODE*)mode);
}

MSD_STATUS Oak_gprtSetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE  mode
)
{
    return Oak_gprtSetForceLink(dev, port, (OAK_MSD_PORT_FORCE_LINK_MODE)mode);
}

/*******************************************************************************
* Oak_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - MSD_PORT_SPEED type.
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
MSD_STATUS Oak_gprtGetSpeedIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_PORT_SPEED   *speed
)
{
	return Oak_gprtGetSpeed(dev, port, (OAK_MSD_PORT_SPEED*)speed);
}


/*******************************************************************************
* Oak_gprtGetFrameMode
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
MSD_STATUS Oak_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
)
{
    return Oak_gprtGetFrameMode(dev, port, (OAK_MSD_FRAME_MODE*)mode);
}

/*******************************************************************************
* Oak_gprtSetFrameMode
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
MSD_STATUS Oak_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
)
{
    return Oak_gprtSetFrameMode(dev, port, (OAK_MSD_FRAME_MODE)mode);
}

/*******************************************************************************
* Oak_gprtSetEgressModeIntf
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
MSD_STATUS Oak_gprtSetEgressModeIntf
(
IN MSD_QD_DEV*  dev,
IN MSD_LPORT  port,
IN MSD_EGRESS_MODE mode
)
{
	OAK_MSD_EGRESS_MODE OakMode;
	switch (mode)
	{
		case MSD_EGRESS_MODE_UNMODIFIED:
			OakMode = OAK_MSD_EGRESS_MODE_UNMODIFIED;
			break;
		case MSD_EGRESS_MODE_UNTAGGED:
			OakMode = OAK_MSD_EGRESS_MODE_UNTAGGED;
			break;
		case MSD_EGRESS_MODE_TAGGED:
			OakMode = OAK_MSD_EGRESS_MODE_TAGGED;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			return MSD_BAD_PARAM;
			break;
	}

	return Oak_gprtSetEgressMode(dev, port, OakMode);
}

/*******************************************************************************
* Oak_gprtGetEgressModeIntf
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
MSD_STATUS Oak_gprtGetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_EGRESS_MODE  *mode
)
{
	return Oak_gprtGetEgressMode(dev, port, (OAK_MSD_EGRESS_MODE *)mode);
}

MSD_STATUS Oak_gprtGetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_STP_STATE  *mode
)
{
    return Oak_gprtGetPortState(dev, port, (OAK_MSD_PORT_STP_STATE*)mode);
}

MSD_STATUS Oak_gprtSetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  MSD_PORT_STP_STATE  mode
)
{
    return Oak_gprtSetPortState(dev, port, (OAK_MSD_PORT_STP_STATE)mode);
}
