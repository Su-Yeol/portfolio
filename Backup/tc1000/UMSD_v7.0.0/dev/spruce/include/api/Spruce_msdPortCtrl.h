/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdPortCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PortCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdPortCtrl_h
#define Spruce_msdPortCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

/*  typedef: enum SPRUCE_MSD_8021Q_MODE */
typedef enum
{
	SPRUCE_MSD_DISABLE = 0,
	SPRUCE_MSD_FALLBACK,
	SPRUCE_MSD_CHECK,
	SPRUCE_MSD_SECURE
} SPRUCE_MSD_8021Q_MODE;

/*  typedef: enum SPRUCE_MSD_EGRESS_MODE */
typedef enum
{
	SPRUCE_MSD_EGRESS_MODE_UNMODIFIED = 0,
	SPRUCE_MSD_EGRESS_MODE_UNTAGGED,
	SPRUCE_MSD_EGRESS_MODE_TAGGED
} SPRUCE_MSD_EGRESS_MODE;

/*
 * Typedef: enum SPRUCE_MSD_MTU_SIZE
 *
 * Description: Defines Jumbo Frame Size allowed to be tx and rx
 *
 * Fields:
 *      SPRUCE_MSD_MTU_SIZE_1522 - Rx and Tx frames with max byte of 1522.
 *      SPRUCE_MSD_MTU_SIZE_2048 - Rx and Tx frames with max byte of 2048.
 *      SPRUCE_MSD_MTU_SIZE_10240 - Rx and Tx frames with max byte of 10240.
 *                 
 */
typedef enum
{
    SPRUCE_MSD_MTU_SIZE_1522 = 0,
    SPRUCE_MSD_MTU_SIZE_2048,
    SPRUCE_MSD_MTU_SIZE_10240
} SPRUCE_MSD_MTU_SIZE;

/* Definition for the Port Duplex Mode */
typedef enum
{
    SPRUCE_MSD_DUPLEX_HALF,
    SPRUCE_MSD_DUPLEX_FULL,
    SPRUCE_MSD_DUPLEX_AUTO
} SPRUCE_MSD_PORT_FORCE_DUPLEX_MODE;

/* Definition for full duplex flow control mode */
typedef enum
{
	Spruce_PORT_FC_TX_RX_ENABLED,
	Spruce_PORT_RX_ONLY,
	Spruce_PORT_TX_ONLY,
	Spruce_PORT_PFC_ENABLED
} SPRUCE_MSD_PORT_FC_MODE;

/* Definition for the forced Port Speed */
typedef enum
{
    Spruce_PORT_FORCE_SPEED_10M = 0x0,
    Spruce_PORT_FORCE_SPEED_100M,
    Spruce_PORT_FORCE_SPEED_200M,  
    Spruce_PORT_FORCE_SPEED_1000M,
	Spruce_PORT_FORCE_SPEED_2_5G,
	Spruce_PORT_FORCE_SPEED_5G,
    Spruce_PORT_FORCE_SPEED_10G,     
    Spruce_PORT_DO_NOT_FORCE_SPEED
} SPRUCE_MSD_PORT_FORCED_SPEED_MODE;

typedef enum
{
    SPRUCE_PORT_FORCE_LINK_UP,
    SPRUCE_PORT_FORCE_LINK_DOWN,
    SPRUCE_PORT_DO_NOT_FORCE_LINK
} SPRUCE_MSD_PORT_FORCE_LINK_MODE;

/*
 *  typedef: enum SPRUCE_MSD_ELIMIT_MODE
 *
 *  Description: Enumeration of the port egress rate limit counting mode.
 *
 *  Enumerations:
 *      SPRUCE_MSD_ELIMIT_FRAME -
 *                Count the number of frames
 *      SPRUCE_MSD_ELIMIT_LAYER1 -
 *                Count all Layer 1 bytes: 
 *                Preamble (8bytes) + Frame's DA to CRC + IFG (12bytes)
 *      SPRUCE_MSD_ELIMIT_LAYER2 -
 *                Count all Layer 2 bytes: Frame's DA to CRC
 *      SPRUCE_MSD_ELIMIT_LAYER3 -
 *                Count all Layer 3 bytes: 
 *                Frame's DA to CRC - 18 - 4 (if frame is tagged)
 */
typedef enum
{
    SPRUCE_MSD_ELIMIT_FRAME = 0,
    SPRUCE_MSD_ELIMIT_LAYER1,
    SPRUCE_MSD_ELIMIT_LAYER2,
    SPRUCE_MSD_ELIMIT_LAYER3
} SPRUCE_MSD_ELIMIT_MODE;

#ifndef SPRUCE_STP_STATE
typedef enum
 {
	SPRUCE_MSD_PORT_DISABLE = 0,
	SPRUCE_MSD_PORT_BLOCKING,
	SPRUCE_MSD_PORT_LEARNING,
	SPRUCE_MSD_PORT_FORWARDING
 } SPRUCE_MSD_PORT_STP_STATE;
#define SPRUCE_STP_STATE
#endif

/* Definition for the Port Speed */
typedef enum
{
	Spruce_PORT_SPEED_10_MBPS = 0,
	Spruce_PORT_SPEED_100_MBPS = 1,
	Spruce_PORT_SPEED_200_MBPS = 2,
	Spruce_PORT_SPEED_1000_MBPS = 3,	
    Spruce_PORT_SPEED_2_5_GBPS = 4, 
	Spruce_PORT_SPEED_5_GBPS = 5,
    Spruce_PORT_SPEED_10_GBPS = 6,
	Spruce_PORT_SPEED_UNKNOWN = 7
} SPRUCE_MSD_PORT_SPEED;

typedef enum
{
    SPRUCE_PRI_OVERRIDE_NONE = 0,
    SPRUCE_PRI_OVERRIDE_FRAME,
    SPRUCE_PRI_OVERRIDE_QUEUE,
    SPRUCE_PRI_OVERRIDE_FRAME_QUEUE,
} SPRUCE_MSD_PRI_OVERRIDE;

typedef enum
{
    SPRUCE_FRAME_MODE_NORMAL,
    SPRUCE_FRAME_MODE_DSA,
    SPRUCE_FRAME_MODE_PROVIDER,
    SPRUCE_FRAME_MODE_ETHER_TYPE_DSA
}  SPRUCE_MSD_FRAME_MODE;


/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

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
*
*******************************************************************************/
MSD_STATUS Spruce_gvlnSetPortVid
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_U16       vid
);

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
*
*******************************************************************************/
MSD_STATUS Spruce_gvlnGetPortVid
(
	IN MSD_QD_DEV *dev,
	IN  MSD_LPORT port,
	OUT MSD_U16   *vid
);

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
);

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
);

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
);
MSD_STATUS Spruce_gvlnSetPortVlanQModeIntf
(
IN MSD_QD_DEV        *dev,
IN MSD_LPORT     port,
IN MSD_8021Q_MODE    mode
);
/*******************************************************************************
* Spruce_gvlnGetPortVlanDot1qMode
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
MSD_STATUS Spruce_gvlnGetPortVlanDot1qMode
(
    IN MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    OUT SPRUCE_MSD_8021Q_MODE   *mode
);
MSD_STATUS Spruce_gvlnGetPortVlanQModeIntf
(
IN MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
OUT MSD_8021Q_MODE   *mode
);
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
);
MSD_STATUS Spruce_gprtSetDiscardTaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* Spruce_gprtGetDiscardTagged
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
MSD_STATUS Spruce_gprtGetDiscardTagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS Spruce_gprtGetDiscardTaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
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
);
MSD_STATUS Spruce_gprtSetDiscardUntaggedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);

/*******************************************************************************
* Spruce_gprtGetDiscardUntagged
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
MSD_STATUS Spruce_gprtGetDiscardUntagged
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS Spruce_gprtGetDiscardUntaggedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
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
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT    port,
    IN MSD_BOOL        mode
);
MSD_STATUS Spruce_gprtSetUnicFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* Spruce_gprtGetUnicastFloodBlock
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
MSD_STATUS Spruce_gprtGetUnicastFloodBlock
(
	IN  MSD_QD_DEV   *dev,
	IN  MSD_LPORT    port,
	OUT MSD_BOOL     *mode
);
MSD_STATUS Spruce_gprtGetUnicFloodBlockIntf
(
IN  MSD_QD_DEV   *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
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
);
MSD_STATUS Spruce_gprtSetMultiFloodBlockIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL        mode
);
/*******************************************************************************
* Spruce_gprtGetMulticastFloodBlock
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
MSD_STATUS Spruce_gprtGetMulticastFloodBlock
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);
MSD_STATUS Spruce_gprtGetMultiFloodBlockIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *mode
);
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
*       None.
*
*******************************************************************************/
MSD_STATUS Spruce_gprtSetDiscardBCastMode
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  MSD_BOOL      en
);
MSD_STATUS Spruce_gprtSetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_BOOL      en
);
/*******************************************************************************
* Spruce_gprtGetDiscardBCastMode
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
MSD_STATUS Spruce_gprtGetDiscardBCastMode
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_LPORT     port,
	OUT MSD_BOOL      *en
);
MSD_STATUS Spruce_gprtGetDiscardBCastModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *en
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);
MSD_STATUS Spruce_gprtSetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_MTU_SIZE   mode
);
/*******************************************************************************
* Spruce_gprtGetJumboMode
*
* DESCRIPTION:
*       This routine gets the max frame size allowed to be received and transmitted
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
);
MSD_STATUS Spruce_gprtGetJumboModeIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_MTU_SIZE   *mode
);
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
);

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
);

/*******************************************************************************
* Spruce_gprtSetSpecifiedMTUIntf
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
MSD_STATUS Spruce_gprtSetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_U16      size
);

/*******************************************************************************
* Spruce_gprtGetSpecifiedMTUIntf
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
MSD_STATUS Spruce_gprtGetSpecifiedMTUIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_U16      *size
);

/*******************************************************************************
* Spruce_gprtSetLearnEnable
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
MSD_STATUS Spruce_gprtSetLearnEnable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      mode
);
MSD_STATUS Spruce_gprtSetLearnEnableIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_BOOL      mode
);
/*******************************************************************************
* Spruce_gprtGetLearnEnable
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
MSD_STATUS Spruce_gprtGetLearnEnable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT MSD_BOOL      *mode
);
MSD_STATUS Spruce_gprtGetLearnEnableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_BOOL      *mode
);

MSD_STATUS Spruce_gprtGetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtGetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtSetVTUPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Spruce_gprtSetVTUPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS Spruce_gprtGetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtGetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtSetSAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Spruce_gprtSetSAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  mode
);

MSD_STATUS Spruce_gprtGetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtGetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  *mode
);
MSD_STATUS Spruce_gprtSetDAPriOverrideIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PRI_OVERRIDE  mode
);
MSD_STATUS Spruce_gprtSetDAPriorityOverride
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT SPRUCE_MSD_PRI_OVERRIDE  mode
);


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
);

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
);

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
);

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
);

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
);

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
);

/*******************************************************************************
* Spruce_gprtSetTrunkPort
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
MSD_STATUS Spruce_gprtSetTrunkPort
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port,
    IN MSD_BOOL      en,
    IN MSD_U32       trunkId
);

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
    OUT MSD_U32      *trunkId
);

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
*       Set ForcedFC=1, FCValue = enable/disable, FCMode
*
*******************************************************************************/
MSD_STATUS Spruce_gprtSetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    IN  MSD_BOOL     en,
	IN  SPRUCE_MSD_PORT_FC_MODE      mode
);
MSD_STATUS Spruce_gprtSetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
IN  MSD_BOOL     en,
IN  MSD_PORT_FC_MODE      mode
);
/*******************************************************************************
* Spruce_gprtGetFlowCtrl
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
MSD_STATUS Spruce_gprtGetFlowCtrl
(
	IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *en,
	OUT SPRUCE_MSD_PORT_FC_MODE      *mode
);
MSD_STATUS Spruce_gprtGetFlowCtrlIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT    port,
OUT MSD_BOOL     *en,
OUT MSD_PORT_FC_MODE      *mode
);

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
);
MSD_STATUS Spruce_gprtSetForceSpeedIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT     port,
IN MSD_PORT_FORCED_SPEED_MODE  mode
);
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
);
MSD_STATUS Spruce_gprtGetForceSpeedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_FORCED_SPEED_MODE   *mode
);
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
);

/*******************************************************************************
* Spruce_gprtGetEgressMonitorSource
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
MSD_STATUS Spruce_gprtGetEgressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);

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
);

/*******************************************************************************
* Spruce_gprtGetIngressMonitorSource
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
MSD_STATUS Spruce_gprtGetIngressMonitorSource
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT    port,
    OUT MSD_BOOL     *mode
);

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
MSD_STATUS Spruce_grcSetEgressRate
(
	IN MSD_QD_DEV       *dev,
	IN MSD_LPORT        port,
    IN SPRUCE_MSD_ELIMIT_MODE   mode,
	IN MSD_U32   rate
);
MSD_STATUS Spruce_grcSetEgressRateIntf
(
IN MSD_QD_DEV       *dev,
IN MSD_LPORT        port,
IN MSD_ELIMIT_MODE   mode,
IN MSD_U32   rate
);
/*******************************************************************************
* Spruce_grcGetEgressRate
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
MSD_STATUS Spruce_grcGetEgressRate
(
	IN MSD_QD_DEV		*dev,
    IN  MSD_LPORT		port,
    OUT SPRUCE_MSD_ELIMIT_MODE   *mode,
    OUT MSD_U32   *rate
);
MSD_STATUS Spruce_grcGetEgressRateIntf
(
IN MSD_QD_DEV		*dev,
IN MSD_LPORT		port,
OUT MSD_ELIMIT_MODE   *mode,
OUT MSD_U32   *rate
);

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
);
MSD_STATUS Spruce_grcSetELimitModeIntf
(
IN  MSD_QD_DEV			*dev,
IN  MSD_LPORT			port,
IN  MSD_ELIMIT_MODE      mode
);
/*******************************************************************************
* Spruce_grcGetELimitMode
*
* DESCRIPTION:
*       This routine gets Egress Rate Limit counting mode.
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
);
MSD_STATUS Spruce_grcGetELimitModeIntf
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		port,
OUT MSD_ELIMIT_MODE  *mode
);
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
MSD_STATUS Spruce_gprtSetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN SPRUCE_MSD_PORT_FORCE_DUPLEX_MODE   mode
);
MSD_STATUS Spruce_gprtSetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
IN MSD_PORT_FORCE_DUPLEX_MODE   mode
); 
MSD_STATUS Spruce_gprtGetDuplex
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT SPRUCE_MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS Spruce_gprtGetDuplexIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_FORCE_DUPLEX_MODE   *mode
);
MSD_STATUS Spruce_gprtGetDuplexStatus
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *mode
);
MSD_STATUS Spruce_gprtGetDuplexStatusIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL   *mode
);
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
*		None
*
*******************************************************************************/
MSD_STATUS Spruce_gprtGetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS Spruce_gprtGetForceLinkIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_LINK_MODE   *mode
);
MSD_STATUS Spruce_gprtSetForceLink
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT SPRUCE_MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS Spruce_gprtSetForceLinkIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_LPORT  port,
    OUT MSD_PORT_FORCE_LINK_MODE  mode
);
MSD_STATUS Spruce_gprtGetLinkState
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_LPORT  port,
	OUT MSD_BOOL   *state
);

/*******************************************************************************
* Spruce_gprtGetSpeed
*
* DESCRIPTION:
*       This routine retrives the port MAC speed.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       speed - SPRUCE_MSD_PORT_SPEED type.
*                (Spruce_PORT_SPEED_10_MBPS,Spruce_PORT_SPEED_100_MBPS, Spruce_PORT_SPEED_1000_MBPS,
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
);
MSD_STATUS Spruce_gprtGetSpeedIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_SPEED   *speed
);

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
);

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
);

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
);
MSD_STATUS Spruce_gprtGetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_FRAME_MODE  *mode
);

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
);

MSD_STATUS Spruce_gprtSetFrameModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_FRAME_MODE  mode
);

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
);

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
);

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
);

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
);

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
);

MSD_STATUS Spruce_gprtSetEgressModeIntf
(
IN MSD_QD_DEV*  dev,
IN MSD_LPORT  port,
IN MSD_EGRESS_MODE mode
);

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
);

MSD_STATUS Spruce_gprtGetEgressModeIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_EGRESS_MODE  *mode
);

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
);

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
);

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
);
MSD_STATUS Spruce_gprtGetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_PORT_STP_STATE  *mode
);

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
);
MSD_STATUS Spruce_gprtSetPortStateIntf
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_PORT_STP_STATE  mode
);

MSD_STATUS Spruce_gprtSetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetIgnoreFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetAllowBadFcs
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS Spruce_gprtGetPauseLimitOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS Spruce_gprtSetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U16  limit
);

MSD_STATUS Spruce_gprtGetPauseLimitIn
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U16  *limit
);

MSD_STATUS Spruce_gprtSetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8  queues
);
MSD_STATUS Spruce_gprtGetQueueToPause
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_U8  *queues
);

MSD_STATUS Spruce_gprtSetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_BOOL en
);

MSD_STATUS Spruce_gprtGetRxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_BOOL *en
);

MSD_STATUS Spruce_gprtSetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
IN MSD_U8 queue
);

MSD_STATUS Spruce_gprtGetRxPriFlowCtrlToQueue
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pfcPri,
OUT MSD_U8 *queue
);

MSD_STATUS Spruce_gprtSetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
IN MSD_BOOL en
);

MSD_STATUS Spruce_gprtGetTxPriFlowCtrlEnable
(
IN  MSD_QD_DEV  *dev,
IN MSD_LPORT port,
IN MSD_U8 pri,
OUT MSD_BOOL *en
);

MSD_STATUS Spruce_gprtGetOutQueueSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_U8 queue,
OUT MSD_U16  *count
);

MSD_STATUS Spruce_gprtSetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetMapDA
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetPortLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetHoldAt1
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetInt0nAgeOut
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetRefreshLocked
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetIgnoreWrongData
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
IN  MSD_BOOL  en
);

MSD_STATUS Spruce_gprtGetARPMirror
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT  port,
OUT MSD_BOOL  *en
);

MSD_STATUS Spruce_gprtSetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U16     size
);

MSD_STATUS Spruce_gprtGetPreemptSize
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_U16    *size
);

MSD_STATUS Spruce_gprtSetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_BOOL    en
);

MSD_STATUS Spruce_gprtGetPreemptEnable
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
OUT  MSD_BOOL   *en
);

MSD_STATUS Spruce_gprtSetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
IN  MSD_BOOL    en
);

MSD_STATUS Spruce_gprtGetPreemptQueue
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   port,
IN  MSD_U8      queue,
OUT MSD_BOOL    *en
);

MSD_STATUS Spruce_gprtRegDump
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  port
);
#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdPortCtrl_h */
