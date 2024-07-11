/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Bonsai_msdPortCtrlIntf.c
*
* DESCRIPTION:
*       API implementation for switch port common control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsai/include/api/Bonsai_msdPortCtrl.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/********************************************************************
* Bonsai_gvlnSetPortVlanDot1qMode
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
MSD_STATUS Bonsai_gvlnSetPortVlanQModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
	IN MSD_8021Q_MODE    mode
)
{
	return Bonsai_gvlnSetPortVlanDot1qMode(dev, port, (BONSAI_MSD_8021Q_MODE)mode);
}
/*******************************************************************************
* Bonsai_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine Bonsai_gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to Bonsai_get.
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
MSD_STATUS Bonsai_gvlnGetPortVlanQModeIntf
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
	OUT MSD_8021Q_MODE   *mode
)
{
	return Bonsai_gvlnGetPortVlanDot1qMode(dev, port, (BONSAI_MSD_8021Q_MODE*)mode);
}
/*******************************************************************************
* Bonsai_gprtSetDiscardTagged
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
MSD_STATUS Bonsai_gprtSetDiscardTaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Bonsai_gprtSetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine Bonsai_gets DiscardTagged bit for the given port
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
MSD_STATUS Bonsai_gprtGetDiscardTaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Bonsai_gprtGetDiscardTagged(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtSetDiscardUntagged
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
MSD_STATUS Bonsai_gprtSetDiscardUntaggedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Bonsai_gprtSetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine Bonsai_gets DiscardUntagged bit for the given port
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
MSD_STATUS Bonsai_gprtGetDiscardUntaggedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Bonsai_gprtGetDiscardUntagged(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtSetUnicastFloodBlock
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
MSD_STATUS Bonsai_gprtSetUnicFloodBlockIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Bonsai_gprtSetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine Bonsai_gets Forward Unknown mode of a switch port.
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
MSD_STATUS Bonsai_gprtGetUnicFloodBlockIntf
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
)
{
	return Bonsai_gprtGetUnicastFloodBlock(dev, port, mode);
}
/*******************************************************************************
* Bonsai_gprtSetMulticastFloodBlock
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
MSD_STATUS Bonsai_gprtSetMultiFloodBlockIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
)
{
	return Bonsai_gprtSetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Bonsai_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine Bonsai_gets DefaultForward bit for the given port
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
MSD_STATUS Bonsai_gprtGetMultiFloodBlockIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
)
{
	return Bonsai_gprtGetMulticastFloodBlock(dev, port, mode);
}

/*******************************************************************************
* Bonsai_gprtSetDiscardBCastMode
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
MSD_STATUS Bonsai_gprtSetDiscardBCastModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
)
{
	return Bonsai_gprtSetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Bonsai_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine Bonsai_gets the Discard Broadcast Mode. If the mode is enabled,
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
MSD_STATUS Bonsai_gprtGetDiscardBCastModeIntf
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
)
{
	return Bonsai_gprtGetDiscardBCastMode(dev, port, en);
}

/*******************************************************************************
* Bonsai_gprtSetJumboMode
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
MSD_STATUS Bonsai_gprtSetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	IN  MSD_MTU_SIZE   mode
)
{
	return Bonsai_gprtSetJumboMode(dev, port, (BONSAI_MSD_MTU_SIZE)mode);
}

/*******************************************************************************
* Bonsai_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine Bonsai_gets the max frame size allowed to be received and transmitted
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
MSD_STATUS Bonsai_gprtGetJumboModeIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
	OUT MSD_MTU_SIZE   *mode
)
{
	return Bonsai_gprtGetJumboMode(dev, port, (BONSAI_MSD_MTU_SIZE*)mode);
}

/*******************************************************************************
* Bonsai_gprtSetSpecifiedMTUIntf
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
MSD_STATUS Bonsai_gprtSetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
)
{
	return Bonsai_gprtSetSpecifiedMTU(dev, port, size);
}

/*******************************************************************************
* Bonsai_gprtGetSpecifiedMTUIntf
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
MSD_STATUS Bonsai_gprtGetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_U16      *size
)
{
	return Bonsai_gprtGetSpecifiedMTU(dev, port, size);
}

/*******************************************************************************
* Bonsai_gprtSetLearnEnable
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
MSD_STATUS Bonsai_gprtSetLearnEnableIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
)
{
	return Bonsai_gprtSetLearnEnable(dev, port, mode);
}

/*******************************************************************************
* Bonsai_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine Bonsai_gets LearnDisable setup
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
MSD_STATUS Bonsai_gprtGetLearnEnableIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
)
{
	return Bonsai_gprtGetLearnEnable(dev, port, mode);
}

MSD_STATUS Bonsai_gprtGetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Bonsai_gprtGetVTUPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Bonsai_gprtSetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Bonsai_gprtSetVTUPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Bonsai_gprtGetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Bonsai_gprtGetSAPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Bonsai_gprtSetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Bonsai_gprtSetSAPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE)mode);
}

MSD_STATUS Bonsai_gprtGetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
)
{
    return Bonsai_gprtGetDAPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE*)mode);
}
MSD_STATUS Bonsai_gprtSetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
)
{
    return Bonsai_gprtSetDAPriorityOverride(dev, port, (BONSAI_MSD_PRI_OVERRIDE)mode);
}


/*******************************************************************************
* Bonsai_gprtSetFlowCtrl
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
MSD_STATUS Bonsai_gprtSetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  MSD_PORT_FC_MODE      mode
)
{
	return Bonsai_gprtSetFlowCtrl(dev, port, en, (BONSAI_MSD_PORT_FC_MODE)mode);
}
/*******************************************************************************
* Bonsai_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine Bonsai_get switch port flow control enable/disable status and return 
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
MSD_STATUS Bonsai_gprtGetFlowCtrlIntf
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT MSD_PORT_FC_MODE      *mode
)
{
	return Bonsai_gprtGetFlowCtrl(dev, port, en, (BONSAI_MSD_PORT_FC_MODE*)mode);
}

/*******************************************************************************
* Bonsai_gprtSetForceSpeed
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
MSD_STATUS Bonsai_gprtSetForceSpeedIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
	IN MSD_PORT_FORCED_SPEED_MODE  mode
)
{
	BONSAI_MSD_PORT_FORCED_SPEED_MODE bonsaiMode;
	switch (mode)
	{
	case MSD_PORT_FORCE_SPEED_10M:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_10M;
		break;
	case MSD_PORT_FORCE_SPEED_100M:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_100M;
		break;
	case MSD_PORT_FORCE_SPEED_200M:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_200M;
		break;
	case MSD_PORT_FORCE_SPEED_1000M:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_1000M;
		break;
	case MSD_PORT_FORCE_SPEED_2_5G:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_2_5G;
		break;
	case MSD_PORT_FORCE_SPEED_5G:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_5G;
		break;
	case MSD_PORT_FORCE_SPEED_10G:
		bonsaiMode = Bonsai_PORT_FORCE_SPEED_10G;
		break;
	case MSD_PORT_DO_NOT_FORCE_SPEED:
		bonsaiMode = Bonsai_PORT_DO_NOT_FORCE_SPEED;
		break;
	default:
		MSD_DBG_ERROR(("Failed (Bad Speed).\n"));
		return MSD_BAD_PARAM;
		break;
	}

	return Bonsai_gprtSetForceSpeed(dev, port, bonsaiMode);
}

/*******************************************************************************
* Bonsai_gprtGetForceSpeed
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
MSD_STATUS Bonsai_gprtGetForceSpeedIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
	OUT MSD_PORT_FORCED_SPEED_MODE   *mode
)
{
	return Bonsai_gprtGetForceSpeed(dev, port, (BONSAI_MSD_PORT_FORCED_SPEED_MODE*)mode);
}

/*******************************************************************************
* Bonsai_grcSetEgressRate
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
*                                MSD_ELIMIT_LAYER3 (see Bonsai_grcSetELimitMode)
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
MSD_STATUS Bonsai_grcSetEgressRateIntf
(
    IN MSD_QD_DEV       *dev,
    IN MSD_LPORT        port,
    IN MSD_ELIMIT_MODE   mode,
    IN MSD_U32   rate
)
{
    return Bonsai_grcSetEgressRate(dev, port, (BONSAI_MSD_ELIMIT_MODE)mode, rate);
}

/*******************************************************************************
* Bonsai_grcGetEgressRate
*
* DESCRIPTION:
*       This routine Bonsai_gets the port's egress data limit.
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
*                                MSD_ELIMIT_LAYER3 (see Bonsai_grcSetELimitMode)
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
MSD_STATUS Bonsai_grcGetEgressRateIntf
(
    IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT MSD_ELIMIT_MODE   *mode,
    OUT MSD_U32   *rate
)
{
    return Bonsai_grcGetEgressRate(dev, port, (BONSAI_MSD_ELIMIT_MODE*)mode, rate);
}


/*******************************************************************************
* Bonsai_grcSetELimitMode
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
MSD_STATUS Bonsai_grcSetELimitModeIntf
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
	IN  MSD_ELIMIT_MODE      mode
)
{
	return Bonsai_grcSetELimitMode(dev, port, (BONSAI_MSD_ELIMIT_MODE)mode);
}


/*******************************************************************************
* Bonsai_grcGetELimitMode
*
* DESCRIPTION:
*       This routine Bonsai_gets Egress Rate Limit counting mode.
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
MSD_STATUS Bonsai_grcGetELimitModeIntf
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
	OUT MSD_ELIMIT_MODE  *mode
)
{
	return Bonsai_grcGetELimitMode(dev, port, (BONSAI_MSD_ELIMIT_MODE*)mode);
}

MSD_STATUS Bonsai_gprtSetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   mode
)
{
    return Bonsai_gprtSetDuplex(dev, port, (BONSAI_MSD_PORT_FORCE_DUPLEX_MODE)mode);
}

MSD_STATUS Bonsai_gprtGetDuplexIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
)
{
    return Bonsai_gprtGetDuplex(dev, port, (BONSAI_MSD_PORT_FORCE_DUPLEX_MODE*)mode);
}

/*******************************************************************************
* Bonsai_gprtGetDuplexStatus
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
MSD_STATUS Bonsai_gprtGetDuplexStatusIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
)
{
	return Bonsai_gprtGetDuplexStatus(dev, port, mode);
}

MSD_STATUS Bonsai_gprtGetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE   *mode
)
{
    return Bonsai_gprtGetForceLink(dev, port, (BONSAI_MSD_PORT_FORCE_LINK_MODE*)mode);
}

MSD_STATUS Bonsai_gprtSetForceLinkIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_LINK_MODE  mode
)
{
    return Bonsai_gprtSetForceLink(dev, port, (BONSAI_MSD_PORT_FORCE_LINK_MODE)mode);
}

/*******************************************************************************
* Bonsai_gprtGetSpeed
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
MSD_STATUS Bonsai_gprtGetSpeedIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_PORT_SPEED   *speed
)
{
	return Bonsai_gprtGetSpeed(dev, port, (BONSAI_MSD_PORT_SPEED*)speed);
}


/*******************************************************************************
* Bonsai_gprtGetFrameMode
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
MSD_STATUS Bonsai_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
)
{
    return Bonsai_gprtGetFrameMode(dev, port, (BONSAI_MSD_FRAME_MODE*)mode);
}

/*******************************************************************************
* Bonsai_gprtSetFrameMode
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
MSD_STATUS Bonsai_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
)
{
    return Bonsai_gprtSetFrameMode(dev, port, (BONSAI_MSD_FRAME_MODE)mode);
}

/*******************************************************************************
* Bonsai_gprtSetEgressModeIntf
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
MSD_STATUS Bonsai_gprtSetEgressModeIntf
(
IN MSD_QD_DEV*  dev,
IN MSD_LPORT  port,
IN MSD_EGRESS_MODE mode
)
{
	BONSAI_MSD_EGRESS_MODE BonsaiMode;
	switch (mode)
	{
		case MSD_EGRESS_MODE_UNMODIFIED:
			BonsaiMode = BONSAI_MSD_EGRESS_MODE_UNMODIFIED;
			break;
		case MSD_EGRESS_MODE_UNTAGGED:
			BonsaiMode = BONSAI_MSD_EGRESS_MODE_UNTAGGED;
			break;
		case MSD_EGRESS_MODE_TAGGED:
			BonsaiMode = BONSAI_MSD_EGRESS_MODE_TAGGED;
			break;
		default:
			MSD_DBG_ERROR(("Failed (Bad Mode).\n"));
			return MSD_BAD_PARAM;
			break;
	}

	return Bonsai_gprtSetEgressMode(dev, port, BonsaiMode);
}

/*******************************************************************************
* Bonsai_gprtGetEgressModeIntf
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
MSD_STATUS Bonsai_gprtGetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_EGRESS_MODE  *mode
)
{
	return Bonsai_gprtGetEgressMode(dev, port, (BONSAI_MSD_EGRESS_MODE *)mode);
}

MSD_STATUS Bonsai_gprtGetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_STP_STATE  *mode
)
{
    return Bonsai_gprtGetPortState(dev, port, (BONSAI_MSD_PORT_STP_STATE*)mode);
}

MSD_STATUS Bonsai_gprtSetPortStateIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  port,
    IN  MSD_PORT_STP_STATE  mode
)
{
    return Bonsai_gprtSetPortState(dev, port, (BONSAI_MSD_PORT_STP_STATE)mode);
}
