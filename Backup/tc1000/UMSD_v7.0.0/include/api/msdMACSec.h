/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdMacSec.h
*
* DESCRIPTION:
*       API/Structure definitions for MACSec.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdMACSec_h
#define msdMACSec_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
* DESCRIPTION:
*	This function is used to read MACSec register.
*
* INPUTS:
*	devNum  - physical device number
*   regAddr - register address
*	data - register data
*
* OUTPUTS:
*	None
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS msdMacSecRead
(
	IN  MSD_U8  devNum,
	IN MSD_U32 regAddr,
	OUT MSD_U32* data
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to write MACSec register.
*
* INPUTS:
*	devNum  - physical device number
*   regAddr - register address
*	data - register data
*
* OUTPUTS:
*	None
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS msdMacSecWrite
(
	IN MSD_U8  devNum,
	IN MSD_U32 regAddr,
	IN MSD_U32 data
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to flush all MACSec type module.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*      None
******************************************************************************/
MSD_STATUS msdMacSecFlushAll
(
	IN  MSD_U8  devNum
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get event value.
*
* INPUTS:
*	devNum  - physical device number
*	portNum - port number
*
* OUTPUTS:
*	event - macsec ECC/SMStall/RegParity event
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	Register type is RO
******************************************************************************/
MSD_STATUS msdMacSecGetEvent
(
	IN MSD_U8  devNum,
	OUT MSD_MSEC_EVENT* event
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to create macsec rule.
*
* INPUTS:
*       devNum  - physical device number
*       ruleIndex - rule index
*       rule - MACSec rule configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex,
	IN MSD_MSEC_RULE* rule
);
/******************************************************************************
* DESCRIPTION:
*		This function is used to delete macsec rule.
*
* INPUTS:
*		devNum  - physical device number
*		ruleIndex - rule index
*
* OUTPUTS:
*		None
*
* RETURNS:
*		MSD_OK      - on success
*		MSD_FAIL    - on error
*		MSD_BAD_PARAM - if input parameters are beyond range.
*		MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex
);
/******************************************************************************
* DESCRIPTION:
*		This function is used to get macsec rule.
*
* INPUTS:
*		devNum  - physical device number
*		ruleIndex - rule index
*
* OUTPUTS:
*		rule - MACSec rule configuration
*
* RETURNS:
*		MSD_OK      - on success
*		MSD_FAIL    - on error
*		MSD_BAD_PARAM - if input parameters are beyond range.
*		MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*		None
******************************************************************************/
MSD_STATUS msdMacSecGetRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex,
	OUT MSD_MSEC_RULE* rule
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       secY - secY configuration
*
* OUTPUTS:
*      None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_INGRESS* secY
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       secY - secY configuration
*
* OUTPUTS:
*      None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_EGRESS* secY
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       secYIndex - secY index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       secYIndex - secY index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       secYIndex - secY index
*
* OUTPUTS:
*       secY	- secy configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*      None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_EGRESS* secY
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get macsec secY.
*
* INPUTS:
*       devNum  - physical device number
*       secYIndex - secY index
*
* OUTPUTS:
*       secY	- secy configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*      None
******************************************************************************/
MSD_STATUS msdMacSecGetRxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_INGRESS* secY
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to create ingress sc.
*
* INPUTS:
*       devNum  - physical device number
*       scIndex - sc index
*       rxSC - rxSC configuration
*
* OUTPUTS:
*      None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_RX_SC* rxSC
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete ingress sc.
*
* INPUTS:
*       devNum  - physical device number
*       scIndex - sc index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sc.
*
* INPUTS:
*       devNum  - physical device number
*       scIndex - SC index
*
* OUTPUTS:
*       rxSC	- sc configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*****************************************************************************/
MSD_STATUS msdMacSecGetRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_RX_SC* rxSC
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create ingress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - sa index
*       rxSA - rxSA configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_RX_SA* rxSA
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete ingress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - sa index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - SA index
*
* OUTPUTS:
*       rxSA	- sa configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_RX_SA* rxSA
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create egress sc.
*
* INPUTS:
*       devNum  - physical device number
*       scIndex - sc index
*       txSC - txSC configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_TX_SC* txSC
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete egress sc.
*
* INPUTS:
*      devNum  - physical device number
*      scIndex - sc index
*
* OUTPUTS:
*      None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sc.
*
* INPUTS:
*       devNum  - physical device number
*       scIndex - SC index
*
* OUTPUTS:
*       rxSC	- sc configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_TX_SC* txSC
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create egress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - sa index
*       txSA - txSA configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_TX_SA* txSA
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete egress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - sa index
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sa.
*
* INPUTS:
*       devNum  - physical device number
*       saIndex - SA index
*
* OUTPUTS:
*       txSA	- sa configuration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_TX_SA* txSA
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sc related mibs.
*
* INPUTS:
*       devNum  - physical device number
*       scCounterIndex - SC counter index
*       autoClear - Enable or disable
*
* OUTPUTS:
*       states - ingress sc mibs
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetRXSCStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SC_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress secY related mibs.
*
* INPUTS:
*       devNum  - physical device number
*       secYCounterIndex - secY counter index
*       autoClear - Enable or disable
*
* OUTPUTS:
*       states - ingress secY mibs
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetRXSecYStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SECY_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sc related mibs.
*
* INPUTS:
*       devNum  - physical device number
*       scCounterIndex - SC counter index
*       autoClear - Enable or disable
*
* OUTPUTS:
*       states - egress sc mibs
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTXSCStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SC_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress secY related mibs.
*
* INPUTS:
*       devNum  - physical device number
*       secYCounterIndex - secY counter index
*       autoClear - Enable or disable
*
* OUTPUTS:
*       states - egress secY mibs
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTXSecYStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SECY_COUNTER* states
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to enable or disable macsec module.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       isEnable - enable or disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecEnableEgressSet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to enable or disable macsec module.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       isEnable - enable or disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecEnableIngressSet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get macsec module status.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       isEnable - enable or disable
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecEnableEgressGet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get macsec module status.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       isEnable - enable or disable
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecEnableIngressGet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to set Tx PN threshold.
*
* INPUTS:
*       devNum  - physical device number
*       pnThr - PN threshold
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecSetTxPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U32 pnThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get Tx PN threshold.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*      pnThr - PN threshold
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTxPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U32* pnThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to set Tx XPN threshold.
*
* INPUTS:
*       devNum  - physical device number
*       xpnThr - XPN threshold
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecSetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U64 xpnThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get Tx XPN threshold.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*      xpnThr - XPN threshold
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U64* xpnThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to set default RX SCI.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*       defaultRxSCI - default Rx SCI
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecSetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultRxSCI
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get default RX SCI.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       defaultRxSCI - default Rx SCI
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_U64* defaultRxSCI
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to set tcsc interrupt control.
*
* INPUTS:
*	dev  - physical device
*	index - index
*	txscIntCtrl - interrupt enable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecSetTxSCIntCtrl
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	IN MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get interrupt control.
*
* INPUTS:
*	dev  - physical device
*	index - index
*
* OUTPUTS:
*   txscIntCtrl - interrupt control
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSCIntCtrl
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	OUT MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get next Txsc interrupt status.
*
* INPUTS:
*	dev  - physical device
*	index - index
*	isAutoClear - auto clear, enable or disable
*
* OUTPUTS:
*	nextIndex - next valid index
*	int_type - interrupt detailed type
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if input parameters are beyond range.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
******************************************************************************/
MSD_STATUS msdMacSecGetNextTxSCIntStatus
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	IN MSD_BOOL isAutoClear,
	OUT MSD_U8* nextIndex,
	OUT MSD_MACSEC_INT_TXSC* txscInt
);


#ifdef __cplusplus
}
#endif

#endif /* msdMacSec_h */
