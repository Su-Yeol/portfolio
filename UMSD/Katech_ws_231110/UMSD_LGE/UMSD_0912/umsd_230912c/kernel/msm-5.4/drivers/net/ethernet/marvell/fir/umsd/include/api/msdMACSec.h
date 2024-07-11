/**********************************************************************************************
* Copyright (c) 2022 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdMacsec.h
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
*       This function is used to configure common macsec egress(encrypted) feature.
*       Below is the common api rule:
*       1. Any packets will be encrypted
*       2. Key default is all 0
*       3. Salt default is all 0
*       4. SSCI default is 0
*       5. Index means tcam rule index, secY index, SC index, SA0 index is 2*index, SA1 index is 2*index + 1
*       6. Default SA0 AN is 0 and SA1 AN is 1
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       index   - rule index
*       cipherSuite - cipher suite
*       isAutoRekey - if enable auto rekey
*       tci - sectag tci filed
*       nextPN - Sa0 and SA1 next packets number
*       thresholdPN - Sa0 and SA1 packets number threshold
*       isActiveSA1 - If active SA1
*       sci  - SCI
*       secYCounterIndex - secY mibs index
*       scCounterIndex - SC mibs index
*
* OUTPUTS:
*	   None
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
UMSD_FUNC MSD_STATUS msdMacsecConfigureEgress
(
	IN  MSD_U8  devNum,
	IN  MSD_U8  ruleIndex,
	IN  MSD_U8  scIndex,
	IN  MSD_U8  secYIndex,
	IN  MSD_CIPHER_SUITE cipherSuite,
	IN  MSD_BOOL isAutoRekey,
	IN  MSD_U8   tci,
	IN  MSD_U64  nextPN,
	IN  MSD_U64  thresholdPN,
	IN  MSD_BOOL isActiveSA1,
	IN  MSD_U64  sci,
	IN  MSD_U8   secYCounterIndex,
	IN  MSD_U8   scCounterIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to configure common macsec ingress(decrypted) feature.
*       Below is the common api rule:
*       1. Any packets will be decrypted
*       2. Key default is all 0
*       3. Salt default is all 0
*       4. SSCI default is 0
*       5. Index means tcam rule index, secY index, SC index, SA0 index is 2*index, SA1 index is 2*index + 1
*       6. Default SA0 AN is 0, SA1 AN is 1
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       index   - rule index
*       cipherSuite - cipher suite
*       validateFrame - valid frame mode
*       nextPN - Sa0 and SA1 next packets number
*       sci  - SCI
*       secYCounterIndex - secY mibs index
*       scCounterIndex - SC mibs index
*		allowZeroRxPN - allow PN 0 enable or not
*       replayProtect - replay protect enable or not
*       replayWindow - replay window value
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
UMSD_FUNC MSD_STATUS msdMacsecConfigureIngress
(
	IN  MSD_U8  devNum,
	IN  MSD_U8  ruleIndex,
	IN  MSD_U8  scIndex,
	IN  MSD_U8  secYIndex,
	IN  MSD_CIPHER_SUITE cipherSuite,
	IN  MSD_VALIDATE_FRAME validateFrame,
	IN  MSD_U64  nextPN,
	IN  MSD_U64  sci,
	IN  MSD_U8   secYCounterIndex,
	IN  MSD_U8   scCounterIndex,
	IN  MSD_BOOL allowZeroRxPN,
	IN  MSD_BOOL replayProtect,
	IN  MSD_32  replayWindow
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to read MACSec register.
*
* INPUTS:
*	pDev - pointer to FIR_DEV
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
UMSD_FUNC MSD_STATUS msdMacsecRead
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
*	pDev - pointer to FIR_DEV
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
UMSD_FUNC MSD_STATUS msdMacsecWrite
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
*       devNum  - physical devie number
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
UMSD_FUNC MSD_STATUS msdMacsecFlushAll
(
	IN  MSD_U8  devNum
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get event value.
*
* INPUTS:
*	pDev - pointer to FIR_DEV
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
UMSD_FUNC MSD_STATUS msdMacsecGetEvent
(
	IN MSD_U8  devNum,
	OUT MSD_MSEC_EVENT* event
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to create macsec rule.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       rule - MSEC_RULE configuration
*       isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecCreateRule
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
*		devNum  - physical devie number
*		portNum - port number
*		ruleIndex - MSEC_RULE index
*		isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex
);
/******************************************************************************
* DESCRIPTION:
*		This function is used to get macsec rule.
*
* INPUTS:
*		devNum  - physical devie number
*		portNum - port number
*		ruleIndex - rule index
*		isEgress - direction
*
* OUTPUTS:
*		rule - MSEC_RULE configuration
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
UMSD_FUNC MSD_STATUS msdMacsecGetRule
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
*       devNum  - physical devie number
*       portNum - port number
*       secY - secY configuration
*       isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecCreateRxSecY
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
*       devNum  - physical devie number
*       portNum - port number
*       secY - secY configuration
*       isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecCreateTxSecY
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
*       devNum  - physical devie number
*       portNum - port number
*       secYIndex - secY index
*       isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteRxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to delete macsec secY.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       secYIndex - secY index
*       isEgress - direction
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteTxSecY
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get macsec secY.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       secYIndex - secY index
*       isEgress - direction
*
* OUTPUTS:
*       secY
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxSecY
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
*       devNum  - physical devie number
*       portNum - port number
*       secYIndex - secY index
*       isEgress - direction
*
* OUTPUTS:
*       secY
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
UMSD_FUNC MSD_STATUS msdMacsecGetRxSecY
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecCreateRxSC
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sc.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       scIndex - SC index
*
* OUTPUTS:
*       rxSC
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
UMSD_FUNC MSD_STATUS msdMacsecGetRxSC
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecCreateRxSA
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sa.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       saIndex - SA index
*
* OUTPUTS:
*       rxSA
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
UMSD_FUNC MSD_STATUS msdMacsecGetRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_RX_SA* rxSA
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sa next packet number.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       saIndex - SA index
*
* OUTPUTS:
*       nextPN - next packet number
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
UMSD_FUNC MSD_STATUS msdMacsecGetRxSANextPN
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to create egress sc.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecCreateTxSC
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
*      devNum  - physical devie number
*      portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sc.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       scIndex - SC index
*
* OUTPUTS:
*       rxSC
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxSC
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecCreateTxSA
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecDeleteTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sa.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       saIndex - SA index
*
* OUTPUTS:
*       txSA
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_TX_SA* txSA
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get egress sa next packet number.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       saIndex - SA index
*
* OUTPUTS:
*       nextPN - next packet number
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxSANextPN
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
);

/******************************************************************************
* DESCRIPTION:
*       This function is used to get ingress sc related mibs.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecGetRXSCStates
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecGetRXSecYStates
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecGetTXSCStates
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
*       devNum  - physical devie number
*       portNum - port number
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
UMSD_FUNC MSD_STATUS msdMacsecGetTXSecYStates
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
*       devNum  - physical devie number
*       portNum - port number
*       isEgress - Egress or Ingress
*       portControl - port control
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
UMSD_FUNC MSD_STATUS msdMacsecEnableEgressSet
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
*       devNum  - physical devie number
*       portNum - port number
*       isEgress - Egress or Ingress
*       portControl - port control
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
UMSD_FUNC MSD_STATUS msdMacsecEnableIngressSet
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
*       devNum  - physical devie number
*       portNum - port number
*       isEgress - Egress or Ingress
*
* OUTPUTS:
*       portControl - port control
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
UMSD_FUNC MSD_STATUS msdMacsecEnableEgressGet
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
*       devNum  - physical devie number
*       portNum - port number
*       isEgress - Egress or Ingress
*
* OUTPUTS:
*       portControl - port control
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
UMSD_FUNC MSD_STATUS msdMacsecEnableIngressGet
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
*       devNum  - physical devie number
*       portNum - port number
*       pNThr - PN threshold
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
UMSD_FUNC MSD_STATUS msdMacsecSetTxPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U32 pNThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get Tx PN threshold.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*
* OUTPUTS:
*      pNThr - PN threshold
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U32* pNThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to set Tx XPN threshold.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       pNThr - PN threshold
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
UMSD_FUNC MSD_STATUS msdMacsecSetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U64 pNThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get Tx XPN threshold.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*
* OUTPUTS:
*      pNThr - PN threshold
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U64* pNThr
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to set default RX SCI.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*       defaultSCI - default SCI
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
UMSD_FUNC MSD_STATUS msdMacsecSetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultSCI
);
/******************************************************************************
* DESCRIPTION:
*       This function is used to get default RX SCI.
*
* INPUTS:
*       devNum  - physical devie number
*       portNum - port number
*
* OUTPUTS:
*       defaultSCI - default SCI
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
UMSD_FUNC MSD_STATUS msdMacsecGetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_U64* defaultSCI
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to set tcsc interrupt control.
*
* INPUTS:
* dev  - physical devie
* index - index
* int_control_txsc - interrupt enable
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
UMSD_FUNC MSD_STATUS msdMacsecSetTxSCIntCtrl
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
* dev  - physical devie
* index - index
*
* OUTPUTS:
*       int_control - control
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
UMSD_FUNC MSD_STATUS msdMacsecGetTxSCIntCtrl
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
* dev  - physical devie
* index - index
* isAUTOClear - Auto clear, enable or disable
*
* OUTPUTS:
*       nextIndex - next valid index
*       int_type - interrupt detailed type
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
UMSD_FUNC MSD_STATUS msdMacsecGetNextTxSCIntStatus
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	IN MSD_BOOL isAUTOClear,
	OUT MSD_U8* nextIndex,
	OUT MSD_MACSEC_INT_TXSC* txscInt
);

#ifdef __cplusplus
}
#endif

#endif /* msdMacsec_h */
