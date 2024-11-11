/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* BonsaiZ1_msdPortCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PortCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef BonsaiZ1_msdPortCtrl_h
#define BonsaiZ1_msdPortCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

/*  typedef: enum BONSAIZ1_MSD_8021Q_MODE */
typedef enum
{
	BONSAIZ1_MSD_DISABLE = 0,
	BONSAIZ1_MSD_FALLBACK,
	BONSAIZ1_MSD_CHECK,
	BONSAIZ1_MSD_SECURE
} BONSAIZ1_MSD_8021Q_MODE;

/*  typedef: enum BONSAIZ1_MSD_EGRESS_MODE */
typedef enum
{
	BONSAIZ1_MSD_EGRESS_MODE_UNMODIFIED = 0,
	BONSAIZ1_MSD_EGRESS_MODE_UNTAGGED,
	BONSAIZ1_MSD_EGRESS_MODE_TAGGED
} BONSAIZ1_MSD_EGRESS_MODE;

/*
 * Typedef: enum BONSAIZ1_MSD_MTU_SIZE
 *
 * Description: Defines Jumbo Frame Size allowed to be tx and rx
 *
 * Fields:
 *      BONSAIZ1_MSD_MTU_SIZE_1522 - Rx and Tx frames with max byte of 1522.
 *      BONSAIZ1_MSD_MTU_SIZE_2048 - Rx and Tx frames with max byte of 2048.
 *      BONSAIZ1_MSD_MTU_SIZE_10240 - Rx and Tx frames with max byte of 10240.
 *                 
 */
typedef enum
{
    BONSAIZ1_MSD_MTU_SIZE_1522 = 0,
    BONSAIZ1_MSD_MTU_SIZE_2048,
    BONSAIZ1_MSD_MTU_SIZE_10240
} BONSAIZ1_MSD_MTU_SIZE;

/* Definition for the Port Duplex Mode */
typedef enum
{
    BONSAIZ1_MSD_DUPLEX_HALF,
    BONSAIZ1_MSD_DUPLEX_FULL,
    BONSAIZ1_MSD_DUPLEX_AUTO
} BONSAIZ1_MSD_PORT_FORCE_DUPLEX_MODE;

/* Definition for full duplex flow control mode */
typedef enum
{
	BonsaiZ1_PORT_FC_TX_RX_ENABLED,
	BonsaiZ1_PORT_RX_ONLY,
	BonsaiZ1_PORT_TX_ONLY,
	BonsaiZ1_PORT_PFC_ENABLED
} BONSAIZ1_MSD_PORT_FC_MODE;

/* Definition for the forced Port Speed */
typedef enum
{
    BonsaiZ1_PORT_FORCE_SPEED_10M = 0x0,
    BonsaiZ1_PORT_FORCE_SPEED_100M,
    BonsaiZ1_PORT_FORCE_SPEED_200M,  
    BonsaiZ1_PORT_FORCE_SPEED_1000M,
	BonsaiZ1_PORT_FORCE_SPEED_2_5G,
	BonsaiZ1_PORT_FORCE_SPEED_5G,
    BonsaiZ1_PORT_FORCE_SPEED_10G,     
    BonsaiZ1_PORT_DO_NOT_FORCE_SPEED
} BONSAIZ1_MSD_PORT_FORCED_SPEED_MODE;

typedef enum
{
    BONSAIZ1_PORT_FORCE_LINK_UP,
    BONSAIZ1_PORT_FORCE_LINK_DOWN,
    BONSAIZ1_PORT_DO_NOT_FORCE_LINK
} BONSAIZ1_MSD_PORT_FORCE_LINK_MODE;

/*
 *  typedef: enum BONSAIZ1_MSD_ELIMIT_MODE
 *
 *  Description: Enumeration of the port egress rate limit counting mode.
 *
 *  Enumerations:
 *      BONSAIZ1_MSD_ELIMIT_FRAME -
 *                Count the number of frames
 *      BONSAIZ1_MSD_ELIMIT_LAYER1 -
 *                Count all Layer 1 bytes: 
 *                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
 *      BONSAIZ1_MSD_ELIMIT_LAYER2 -
 *                Count all Layer 2 bytes: Frame's DA to CRC
 *      BONSAIZ1_MSD_ELIMIT_LAYER3 -
 *                Count all Layer 3 bytes: 
 *                Frame's DA to CRC - 18 - 4 (if frame is tagged)
 */
typedef enum
{
    BONSAIZ1_MSD_ELIMIT_FRAME = 0,
    BONSAIZ1_MSD_ELIMIT_LAYER1,
    BONSAIZ1_MSD_ELIMIT_LAYER2,
    BONSAIZ1_MSD_ELIMIT_LAYER3
} BONSAIZ1_MSD_ELIMIT_MODE;

#ifndef BONSAIZ1_STP_STATE
typedef enum
 {
	BONSAIZ1_MSD_PORT_DISABLE = 0,
	BONSAIZ1_MSD_PORT_BLOCKING,
	BONSAIZ1_MSD_PORT_LEARNING,
	BONSAIZ1_MSD_PORT_FORWARDING
 } BONSAIZ1_MSD_PORT_STP_STATE;
#define BONSAIZ1_STP_STATE
#endif

/* Definition for the Port Speed */
typedef enum
{
	BonsaiZ1_PORT_SPEED_10_MBPS = 0,
	BonsaiZ1_PORT_SPEED_100_MBPS = 1,
	BonsaiZ1_PORT_SPEED_200_MBPS = 2,
	BonsaiZ1_PORT_SPEED_1000_MBPS = 3,	
    BonsaiZ1_PORT_SPEED_2_5_GBPS = 4, 
	BonsaiZ1_PORT_SPEED_5_GBPS = 5,
    BonsaiZ1_PORT_SPEED_10_GBPS = 6,
	BonsaiZ1_PORT_SPEED_UNKNOWN = 7
} BONSAIZ1_MSD_PORT_SPEED;

typedef enum
{
    BONSAIZ1_PRI_OVERRIDE_NONE = 0,
    BONSAIZ1_PRI_OVERRIDE_FRAME,
    BONSAIZ1_PRI_OVERRIDE_QUEUE,
    BONSAIZ1_PRI_OVERRIDE_FRAME_QUEUE,
} BONSAIZ1_MSD_PRI_OVERRIDE;

typedef enum
{
    BONSAIZ1_FRAME_MODE_NORMAL,
    BONSAIZ1_FRAME_MODE_DSA,
    BONSAIZ1_FRAME_MODE_PROVIDER,
    BONSAIZ1_FRAME_MODE_ETHER_TYPE_DSA
}  BONSAIZ1_MSD_FRAME_MODE;


/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* BonsaiZ1_gvlnSetPortVid
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
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
);

/*******************************************************************************
* BonsaiZ1_gvlnGetPortVid
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
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gvlnGetPortVid
(
	IN MSD_QD_DEV *dev,
	IN  MSD_LPORT port,
	OUT MSD_U16   *vid
);

/*******************************************************************************
* BonsaiZ1_gprtSetForceDefaultVid
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
MSD_STATUS BonsaiZ1_gprtSetForceDefaultVid
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      en
);

/*******************************************************************************
* BonsaiZ1_gprtGetForceDefaultVid
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
MSD_STATUS BonsaiZ1_gprtGetForceDefaultVid
(
IN MSD_QD_DEV	*dev,
IN  MSD_LPORT	port,
OUT MSD_BOOL	*en
);

/********************************************************************
* BonsaiZ1_gvlnSetPortVlanDot1qMode
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
MSD_STATUS BonsaiZ1_gvlnSetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN MSD_LPORT     port,
    IN BONSAIZ1_MSD_8021Q_MODE    mode
);
MSD_STATUS BonsaiZ1_gvlnSetPortVlanQModeIntf
(
IN MSD_QD_DEV        *dev,
IN MSD_LPORT     port,
IN MSD_8021Q_MODE    mode
);
/*******************************************************************************
* BonsaiZ1_gvlnGetPortVlanDot1qMode
*
* DESCRIPTION:
*       This routine gets the IEEE 802.1q mode for this port.
*
* INPUTS:
*       port     - logical port number to get.
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
MSD_STATUS BonsaiZ1_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT BONSAIZ1_MSD_8021Q_MODE   *mode
);
MSD_STATUS BonsaiZ1_gvlnGetPortVlanQModeIntf
(
IN MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
OUT MSD_8021Q_MODE   *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetDiscardTagged
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
MSD_STATUS BonsaiZ1_gprtSetDiscardTagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
);
MSD_STATUS BonsaiZ1_gprtSetDiscardTaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetDiscardTagged
*
* DESCRIPTION:
*       This routine gets DiscardTagged bit for the given port
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
MSD_STATUS BonsaiZ1_gprtGetDiscardTagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS BonsaiZ1_gprtGetDiscardTaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetDiscardUntagged
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
MSD_STATUS BonsaiZ1_gprtSetDiscardUntagged
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
);
MSD_STATUS BonsaiZ1_gprtSetDiscardUntaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);

/*******************************************************************************
* BonsaiZ1_gprtGetDiscardUntagged
*
* DESCRIPTION:
*       This routine gets DiscardUntagged bit for the given port
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
MSD_STATUS BonsaiZ1_gprtGetDiscardUntagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS BonsaiZ1_gprtGetDiscardUntaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetUnicastFloodBlock
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
MSD_STATUS BonsaiZ1_gprtSetUnicastFloodBlock
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT    port,
    IN MSD_BOOL        mode
);
MSD_STATUS BonsaiZ1_gprtSetUnicFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetUnicastFloodBlock
*
* DESCRIPTION:
*       This routine gets Forward Unknown mode of a switch port.
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
MSD_STATUS BonsaiZ1_gprtGetUnicastFloodBlock
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
);
MSD_STATUS BonsaiZ1_gprtGetUnicFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetMulticastFloodBlock
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
MSD_STATUS BonsaiZ1_gprtSetMulticastFloodBlock
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
);
MSD_STATUS BonsaiZ1_gprtSetMultiFloodBlockIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetMulticastFloodBlock
*
* DESCRIPTION:
*       This routine gets DefaultForward bit for the given port
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
MSD_STATUS BonsaiZ1_gprtGetMulticastFloodBlock
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS BonsaiZ1_gprtGetMultiFloodBlockIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetDiscardBCastMode
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
*       None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetDiscardBCastMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
);
MSD_STATUS BonsaiZ1_gprtSetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);
/*******************************************************************************
* BonsaiZ1_gprtGetDiscardBCastMode
*
* DESCRIPTION:
*       This routine gets the Discard Broadcast Mode. If the mode is enabled,
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
*       None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetDiscardBCastMode
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
);
MSD_STATUS BonsaiZ1_gprtGetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetIGMPSnoopEnable
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
MSD_STATUS BonsaiZ1_gprtSetIGMPSnoopEnable
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* BonsaiZ1_gprtGetIGMPSnoopEnable
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
MSD_STATUS BonsaiZ1_gprtGetIGMPSnoopEnable
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* BonsaiZ1_gprtGetHeaderMode
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
MSD_STATUS BonsaiZ1_gprtGetHeaderMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetHeaderMode
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
MSD_STATUS BonsaiZ1_gprtSetHeaderMode
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* BonsaiZ1_gprtGetDropOnLock
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
MSD_STATUS BonsaiZ1_gprtGetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetDropOnLock
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
MSD_STATUS BonsaiZ1_gprtSetDropOnLock
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);

/*******************************************************************************
* BonsaiZ1_gprtSetJumboMode
*
* DESCRIPTION:
*       This routine Set the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port - the logical port number
*       mode - BONSAIZ1_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS BonsaiZ1_gprtSetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  BONSAIZ1_MSD_MTU_SIZE   mode
);
MSD_STATUS BonsaiZ1_gprtSetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_MTU_SIZE   mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
*       from or to a given port.
*
* INPUTS:
*       port  - the logical port number.
*
* OUTPUTS:
*       mode - BONSAIZ1_MSD_MTU_SIZE (1522, 2048, or 10240)
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
MSD_STATUS BonsaiZ1_gprtGetJumboMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT BONSAIZ1_MSD_MTU_SIZE   *mode
);
MSD_STATUS BonsaiZ1_gprtGetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_MTU_SIZE   *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetSpecifiedMTU
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
MSD_STATUS BonsaiZ1_gprtSetSpecifiedMTU
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
);

/*******************************************************************************
* BonsaiZ1_gprtGetSpecifiedMTU
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
MSD_STATUS BonsaiZ1_gprtGetSpecifiedMTU
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16       *size
);

/*******************************************************************************
* BonsaiZ1_gprtSetSpecifiedMTUIntf
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
MSD_STATUS BonsaiZ1_gprtSetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
);

/*******************************************************************************
* BonsaiZ1_gprtGetSpecifiedMTUIntf
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
MSD_STATUS BonsaiZ1_gprtGetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_U16      *size
);

/*******************************************************************************
* BonsaiZ1_gprtSetLearnEnable
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
MSD_STATUS BonsaiZ1_gprtSetLearnEnable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
);
MSD_STATUS BonsaiZ1_gprtSetLearnEnableIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetLearnEnable
*
* DESCRIPTION:
*       This routine gets LearnDisable setup
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
*		None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetLearnEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
);
MSD_STATUS BonsaiZ1_gprtGetLearnEnableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *mode
);

MSD_STATUS BonsaiZ1_gprtGetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtSetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS BonsaiZ1_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS BonsaiZ1_gprtGetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtSetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS BonsaiZ1_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS BonsaiZ1_gprtGetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS BonsaiZ1_gprtSetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS BonsaiZ1_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT BONSAIZ1_MSD_PRI_OVERRIDE  mode
);


/*******************************************************************************
* BonsaiZ1_gprtGetMessagePort
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
MSD_STATUS BonsaiZ1_gprtGetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetMessagePort
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
MSD_STATUS BonsaiZ1_gprtSetMessagePort
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
);

/*******************************************************************************
* BonsaiZ1_gprtGetEtherType
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
MSD_STATUS BonsaiZ1_gprtGetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_U16  *eType
);

/*******************************************************************************
* BonsaiZ1_gprtSetEtherType
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
MSD_STATUS BonsaiZ1_gprtSetEtherType
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_U16  eType
);

/*******************************************************************************
* BonsaiZ1_gprtGetAllowVidZero
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
MSD_STATUS BonsaiZ1_gprtGetAllowVidZero
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL  *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetAllowVidZero
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
MSD_STATUS BonsaiZ1_gprtSetAllowVidZero
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL  en
);

/*******************************************************************************
* BonsaiZ1_gprtSetTrunkPort
*
* DESCRIPTION:
*        This function enables/disables and sets the trunk ID.
*        
* INPUTS:
*        port - the logical port number.
*        en - MSD_TRUE to make the port be a member of a trunk with the given trunkId.
*             MSD_FALSE, otherwise.
*        trunkId - valid ID is 0 ~ 31
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if port is invalid nor INVALID_TRUNK_ID
*
* COMMENTS: 
*		 None.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      en,
    IN MSD_U32       trunkId
);

/*******************************************************************************
* BonsaiZ1_gprtGetTrunkPort
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
MSD_STATUS BonsaiZ1_gprtGetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    OUT MSD_BOOL     *en,
    OUT MSD_U32      *trunkId
);

/*******************************************************************************
* BonsaiZ1_gprtSetFlowCtrl
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
*       Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  BONSAIZ1_MSD_PORT_FC_MODE      mode
);
MSD_STATUS BonsaiZ1_gprtSetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_BOOL     en,
IN  MSD_PORT_FC_MODE      mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetFlowCtrl
*
* DESCRIPTION:
*       This routine get switch port flow control enable/disable status and return 
*       flow control mode
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       en - enable/disable the flow control
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
MSD_STATUS BonsaiZ1_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT BONSAIZ1_MSD_PORT_FC_MODE      *mode
);
MSD_STATUS BonsaiZ1_gprtGetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *en,
OUT MSD_PORT_FC_MODE      *mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetForceSpeed
*
* DESCRIPTION:
*       This routine forces switch MAC speed. 
*
* INPUTS:
*       port - the logical port number.
*       mode - BONSAIZ1_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS BonsaiZ1_gprtSetForceSpeed
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN BONSAIZ1_MSD_PORT_FORCED_SPEED_MODE  mode
);
MSD_STATUS BonsaiZ1_gprtSetForceSpeedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_PORT_FORCED_SPEED_MODE  mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetForceSpeed
*
* DESCRIPTION:
*       This routine retrieves switch MAC Force Speed value
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       mode - BONSAIZ1_MSD_PORT_FORCED_SPEED_MODE (10, 100, 200, 1000, 2.5g, 10g,or No Speed Force)
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
MSD_STATUS BonsaiZ1_gprtGetForceSpeed
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT BONSAIZ1_MSD_PORT_FORCED_SPEED_MODE   *mode
);
MSD_STATUS BonsaiZ1_gprtGetForceSpeedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_FORCED_SPEED_MODE   *mode
);
/*******************************************************************************
* BonsaiZ1_gprtSetEgressMonitorSource
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
MSD_STATUS BonsaiZ1_gprtSetEgressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
);

/*******************************************************************************
* BonsaiZ1_gprtGetEgressMonitorSource
*
* DESCRIPTION:
*       This routine gets EgressMonitorSource bit for the given port
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
MSD_STATUS BonsaiZ1_gprtGetEgressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetIngressMonitorSource
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
MSD_STATUS BonsaiZ1_gprtSetIngressMonitorSource
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL        mode
);

/*******************************************************************************
* BonsaiZ1_gprtGetIngressMonitorSource
*
* DESCRIPTION:
*       This routine gets IngressMonitorSource bit for the given port
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
MSD_STATUS BonsaiZ1_gprtGetIngressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);

/*******************************************************************************
* BonsaiZ1_grcSetEgressRate
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
*                        kbRate - rate in kbps that should used with the BONSAIZ1_MSD_ELIMIT_MODE of
*                                BONSAIZ1_MSD_ELIMIT_LAYER1,
*                                BONSAIZ1_MSD_ELIMIT_LAYER2, or
*                                BONSAIZ1_MSD_ELIMIT_LAYER3 (see BonsaiZ1_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with BONSAIZ1_MSD_ELIMIT_MODE of
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
*		None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_grcSetEgressRate
(
	IN MSD_QD_DEV       *dev,
	IN MSD_LPORT        port,
    IN BONSAIZ1_MSD_ELIMIT_MODE   mode,
	IN MSD_U32   rate
);
MSD_STATUS BonsaiZ1_grcSetEgressRateIntf
(
IN MSD_QD_DEV       *dev,
IN MSD_LPORT        port,
IN MSD_ELIMIT_MODE   mode,
IN MSD_U32   rate
);
/*******************************************************************************
* BonsaiZ1_grcGetEgressRate
*
* DESCRIPTION:
*       This routine gets the port's egress data limit.
*
* INPUTS:
*       port    - logical port number.
*
* OUTPUTS:
*       rateType  - egress data rate limit (MSD_ERATE_TYPE union type).
*                    union type is used to support multiple devices with the
*                    different formats of egress rate.
*                    MSD_ERATE_TYPE has the following fields:
*                        kbRate - rate in kbps that should used with the BONSAIZ1_MSD_ELIMIT_MODE of
*                                BONSAIZ1_MSD_ELIMIT_LAYER1,
*                                BONSAIZ1_MSD_ELIMIT_LAYER2, or
*                                BONSAIZ1_MSD_ELIMIT_LAYER3 (see BonsaiZ1_grcSetELimitMode)
*                            64kbps ~ 1Mbps    : increments of 64kbps,
*                            1Mbps ~ 100Mbps   : increments of 1Mbps, and
*                            100Mbps ~ 1000Mbps: increments of 10Mbps
*                            1Gbps ~ 5Gbps: increments of 100Mbps
*                            Therefore, the valid values are:
*                                64, 128, 192, 256, 320, 384,..., 960,
*                                1000, 2000, 3000, 4000, ..., 100000,
*                                110000, 120000, 130000, ..., 1000000
*                                1100000, 1200000, 1300000, ..., 5000000.
*                        fRate - frame per second that should used with BONSAIZ1_MSD_ELIMIT_MODE of
*                                MSD_PIRL_ELIMIT_FRAME
*                            Valid values are between 3815 and 14880000
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_grcGetEgressRate
(
	IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT BONSAIZ1_MSD_ELIMIT_MODE   *mode,
    OUT MSD_U32   *rate
);
MSD_STATUS BonsaiZ1_grcGetEgressRateIntf
(
IN MSD_QD_DEV		*dev,
IN MSD_LPORT		port,
OUT MSD_ELIMIT_MODE   *mode,
OUT MSD_U32   *rate
);

/*******************************************************************************
* BonsaiZ1_grcSetELimitMode
*
* DESCRIPTION:
*       This routine sets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            BONSAIZ1_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            BONSAIZ1_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            BONSAIZ1_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            BONSAIZ1_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*       mode - BONSAIZ1_MSD_ELIMIT_MODE enum type
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
MSD_STATUS BonsaiZ1_grcSetELimitMode
(
    IN  MSD_QD_DEV			*dev,
    IN  MSD_LPORT			port,
    IN  BONSAIZ1_MSD_ELIMIT_MODE      mode
);
MSD_STATUS BonsaiZ1_grcSetELimitModeIntf
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  MSD_ELIMIT_MODE      mode
);
/*******************************************************************************
* BonsaiZ1_grcGetELimitMode
*
* DESCRIPTION:
*       This routine gets Egress Rate Limit counting mode.
*       The supported modes are as follows:
*            BONSAIZ1_MSD_ELIMIT_FRAME -
*                Count the number of frames
*            BONSAIZ1_MSD_ELIMIT_LAYER1 -
*                Count all Layer 1 bytes: 
*                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
*            BONSAIZ1_MSD_ELIMIT_LAYER2 -
*                Count all Layer 2 bytes: Frame's DA to CRC
*            BONSAIZ1_MSD_ELIMIT_LAYER3 -
*                Count all Layer 1 bytes: 
*                Frame's DA to CRC - 18 - 4 (if frame is tagged)
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - BONSAIZ1_MSD_ELIMIT_MODE enum type
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
MSD_STATUS BonsaiZ1_grcGetELimitMode
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT BONSAIZ1_MSD_ELIMIT_MODE  *mode
);
MSD_STATUS BonsaiZ1_grcGetELimitModeIntf
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		port,
OUT MSD_ELIMIT_MODE  *mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetDuplexStatus
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
MSD_STATUS BonsaiZ1_gprtSetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN BONSAIZ1_MSD_PORT_FORCE_DUPLEX_MODE   mode
);
MSD_STATUS BonsaiZ1_gprtSetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN MSD_PORT_FORCE_DUPLEX_MODE   mode
); 
MSD_STATUS BonsaiZ1_gprtGetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT BONSAIZ1_MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS BonsaiZ1_gprtGetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS BonsaiZ1_gprtGetDuplexStatus
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
);
MSD_STATUS BonsaiZ1_gprtGetDuplexStatusIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
/*******************************************************************************
* BonsaiZ1_gprtGetLinkState
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
*		None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT BONSAIZ1_MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS BonsaiZ1_gprtGetForceLinkIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS BonsaiZ1_gprtSetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT BONSAIZ1_MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS BonsaiZ1_gprtSetForceLinkIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS BonsaiZ1_gprtGetLinkState
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *state
);

/*******************************************************************************
* BonsaiZ1_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port MAC speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       speed - BONSAIZ1_MSD_PORT_SPEED type.
*                (BonsaiZ1_PORT_SPEED_10_MBPS,BonsaiZ1_PORT_SPEED_100_MBPS, BonsaiZ1_PORT_SPEED_1000_MBPS,
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
MSD_STATUS BonsaiZ1_gprtGetSpeed
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT BONSAIZ1_MSD_PORT_SPEED   *speed
);
MSD_STATUS BonsaiZ1_gprtGetSpeedIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_SPEED   *speed
);

/*******************************************************************************
* BonsaiZ1_gprtGetVlanPorts
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
MSD_STATUS BonsaiZ1_gprtGetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_LPORT  *memPorts,
OUT MSD_U8  *memPortsLen
);

/*******************************************************************************
* BonsaiZ1_gprtSetVlanPorts
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
MSD_STATUS BonsaiZ1_gprtSetVlanPorts
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_LPORT  *memPorts,
IN  MSD_U8  memPortsLen
);

/*******************************************************************************
* BonsaiZ1_gprtGetFrameMode
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
MSD_STATUS BonsaiZ1_gprtGetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT BONSAIZ1_MSD_FRAME_MODE  *mode
);
MSD_STATUS BonsaiZ1_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetFrameMode
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
MSD_STATUS BonsaiZ1_gprtSetFrameMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  BONSAIZ1_MSD_FRAME_MODE  mode
);

MSD_STATUS BonsaiZ1_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetPortECID
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
MSD_STATUS BonsaiZ1_gprtSetPortECID
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16 ecid
);

/*******************************************************************************
* BonsaiZ1_gprtGetPortECID
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
MSD_STATUS BonsaiZ1_gprtGetPortECID
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT  MSD_U16 *ecid
);

/*******************************************************************************
* BonsaiZ1_gprtSetIgnoreETag
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
MSD_STATUS BonsaiZ1_gprtSetIgnoreETag
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL en
);

/*******************************************************************************
* BonsaiZ1_gprtGetIgnoreETag
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
MSD_STATUS BonsaiZ1_gprtGetIgnoreETag
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT  MSD_BOOL *en
);

/*******************************************************************************
* BonsaiZ1_gprtSetEgressMode
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
MSD_STATUS BonsaiZ1_gprtSetEgressMode
(
IN MSD_QD_DEV*  dev,
IN MSD_LPORT  port,
IN BONSAIZ1_MSD_EGRESS_MODE mode
);

MSD_STATUS BonsaiZ1_gprtSetEgressModeIntf
(
IN MSD_QD_DEV*  dev,
IN MSD_LPORT  port,
IN MSD_EGRESS_MODE mode
);

/*******************************************************************************
* BonsaiZ1_gprtGetEgressMode
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
MSD_STATUS BonsaiZ1_gprtGetEgressMode
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT BONSAIZ1_MSD_EGRESS_MODE  *mode
);

MSD_STATUS BonsaiZ1_gprtGetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_EGRESS_MODE  *mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetExtendedPortCtrlReg
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
MSD_STATUS BonsaiZ1_gprtSetExtendedPortCtrlReg
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U8     index,
IN  MSD_U8     fieldOffset,
IN  MSD_U8     fieldLength,
IN  MSD_U16    data
);

/*******************************************************************************
* BonsaiZ1_gprtGetExtendedPortCtrlReg
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
MSD_STATUS BonsaiZ1_gprtGetExtendedPortCtrlReg
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  portNum,
IN  MSD_U8     index,
IN  MSD_U8     fieldOffset,
IN  MSD_U8     fieldLength,
OUT MSD_U16    *data
);

/*******************************************************************************
* BonsaiZ1_gprtGetPortState
*
* DESCRIPTION:
*       this routine Get port state of a specified port
*
* INPUTS:
*       port - logical port number
*
* OUTPUTS:
*       mode - BONSAIZ1_PORT_STP_STATE
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
MSD_STATUS BonsaiZ1_gprtGetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT BONSAIZ1_MSD_PORT_STP_STATE  *mode
);
MSD_STATUS BonsaiZ1_gprtGetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_STP_STATE  *mode
);

/*******************************************************************************
* BonsaiZ1_gprtSetPortState
*
* DESCRIPTION:
*       this routine Set port state of a specified port
*
* INPUTS:
*       port - logical port number
*       mode - BONSAIZ1_PORT_STP_STATE
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
MSD_STATUS BonsaiZ1_gprtSetPortState
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  BONSAIZ1_MSD_PORT_STP_STATE  mode
);
MSD_STATUS BonsaiZ1_gprtSetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_PORT_STP_STATE  mode
);

MSD_STATUS BonsaiZ1_gprtSetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS BonsaiZ1_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS BonsaiZ1_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS BonsaiZ1_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS BonsaiZ1_gprtSetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8  queues
);
MSD_STATUS BonsaiZ1_gprtGetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U8  *queues
);

MSD_STATUS BonsaiZ1_gprtSetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_BOOL en
);

MSD_STATUS BonsaiZ1_gprtGetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_BOOL *en
);

MSD_STATUS BonsaiZ1_gprtSetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_U8 queue
);

MSD_STATUS BonsaiZ1_gprtGetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_U8 *queue
);

MSD_STATUS BonsaiZ1_gprtSetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
IN MSD_BOOL en
);

MSD_STATUS BonsaiZ1_gprtGetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
OUT MSD_BOOL *en
);

MSD_STATUS BonsaiZ1_gprtGetOutQueueSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8 queue,
OUT MSD_U16  *count
);

MSD_STATUS BonsaiZ1_gprtSetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS BonsaiZ1_gprtGetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS BonsaiZ1_gprtSetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U16     size
);

MSD_STATUS BonsaiZ1_gprtGetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_U16    *size
);

MSD_STATUS BonsaiZ1_gprtSetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    en
);

MSD_STATUS BonsaiZ1_gprtGetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_BOOL   *en
);

MSD_STATUS BonsaiZ1_gprtSetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
IN  MSD_BOOL    en
);

MSD_STATUS BonsaiZ1_gprtGetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
OUT MSD_BOOL    *en
);

MSD_STATUS BonsaiZ1_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
);
#ifdef __cplusplus
}
#endif

#endif /* __BonsaiZ1_msdPortCtrl_h */
