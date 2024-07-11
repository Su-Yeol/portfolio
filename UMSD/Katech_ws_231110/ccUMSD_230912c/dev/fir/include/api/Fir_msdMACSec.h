/**********************************************************************************************
* Copyright (c) 2022 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Fir_msdMACSec.h
*
* DESCRIPTION:
*       API Prototypes for Switch Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef Fir_msdMACSec_h
#define Fir_msdMACSec_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define MSEC_SAK_LEN 32

typedef enum
{
	FIR_NULL = 0,
	FIR_DISABLED = 1,
	FIR_CHECK,
	FIR_STRICT
} FIR_VALIDATE_FRAME;

typedef enum
{
	FIR_DISABLE = 0,
	FIR_ENABLE = 1
}FIR_AUTO_CLEAR;

typedef struct _FIR_MSEC_EVENT {
	MSD_U32 msecCECCErrEvent;
	MSD_U32 msecUCECCErrEvent;
	MSD_U32 msecCECCErrInjectMisEvent;
	MSD_U32 msecCECCOutMisEvent;
	MSD_BOOL msecRegParityErrEvent;
	MSD_U32 msecSMStallEvent;
}FIR_MSEC_EVENT;

typedef struct _FIR_MSEC_TX_SECY_COUNTER {
	MSD_U64 outOctetsSecYControlledPort;
	MSD_U64 outOctetsSecYEncrypted;
	MSD_U64 outOctetsSecYProtected;
	MSD_U32 outPktsSecYUnicast;
	MSD_U32 outPktsSecYMulticast;
	MSD_U32 outPktSecYBroadcast;
	MSD_U32 outPktsSecYuntagged;
	MSD_U32 outPktsSecYTooLong;
	MSD_U32 outPktsSecYNoSAError;
}FIR_MSEC_TX_SECY_COUNTER;

typedef struct _FIR_MSEC_TX_SC_COUNTER {
	MSD_U32 outPktsProtected;
	MSD_U32 outPktsEncrypted;
}FIR_MSEC_TX_SC_COUNTER;

typedef struct _FIR_MSEC_RX_SECY_COUNTER {
	MSD_U64 inOctetsSecYControlledPort;
	MSD_U64 inOctetsSecYValidated;
	MSD_U64 inOctetsSecYDecrypted;
	MSD_U32 inPktsSecYUnicast;
	MSD_U32 inPktsSecYMulticast;
	MSD_U32 inPktsSecYBroadCast;
	MSD_U32 inPktsSecYUnTagged;
	MSD_U32 inPktsSecYNoTag;
	MSD_U32 inPktsSecYBadTag;
	MSD_U32 inPktsSecYNoSA;
	MSD_U32 inPktsSecYNoSAErr;
}FIR_MSEC_RX_SECY_COUNTER;

typedef struct _FIR_MSEC_RX_SC_COUNTER {
	MSD_U32 inPktsSCLate;
	MSD_U32 inPktsSCNotValid;
	MSD_U32 inPktsSCInvalid;
	MSD_U32 inPktsSCDelayed;
	MSD_U32 inPktsSCUnchecked;
	MSD_U32 inPktsSCOK;
}FIR_MSEC_RX_SC_COUNTER;

typedef struct _FIR_MSEC_TX_SA {
	MSD_BOOL txSAValid;
	MSD_U8 salt[12];
	MSD_U8 sak[MSEC_SAK_LEN];
	MSD_U32 ssci;
	MSD_U64 nextPN;
}FIR_MSEC_TX_SA;

typedef struct _FIR_MSEC_RX_SA {
	MSD_BOOL rxSAValid;
	MSD_U8 salt[12];
	MSD_U8 sak[MSEC_SAK_LEN];
	MSD_U32 ssci;
	MSD_U64 nextPN;
}FIR_MSEC_RX_SA;

typedef struct _FIR_MSEC_RX_SA_ENTRY {
	/*5 bits*/
	MSD_U8 saIndex;
	MSD_BOOL valid;
	/*2 bits*/
	MSD_U8 an;
}FIR_MSEC_RX_SA_ENTRY;

typedef struct _FIR_MSEC_RX_SC {
	/*4 bits*/
	MSD_U8 scCounterIndex;
	/*6 bits*/
	MSD_U8 secYIndex;
	MSD_U64 sci;
	FIR_MSEC_RX_SA_ENTRY rxSA[2];
}FIR_MSEC_RX_SC;

typedef struct _FIR_MSEC_TX_SC {
	MSD_BOOL txNextPNRolloEn;
	/*4 bits*/
	MSD_U8 scCounterIndex;
	MSD_U64 sci;
	MSD_BOOL enable_auto_rekey;
	MSD_BOOL sA0Valid;
	MSD_BOOL sA1Valid;
	/*2 bits*/
	MSD_U8 sa0An;
	/*2 bits*/
	MSD_U8 sa1An;
	/*5 bits*/
	MSD_U8 sa0Index;
	/*5 bits*/
	MSD_U8 sa1Index;
	/*else sa0*/
	MSD_BOOL isActiveSA1;
}FIR_MSEC_TX_SC;

typedef struct _FIR_SECY_EGRESS {
	/*4 bits*/
	MSD_U8 secYCounterIndex;
	/*6 bits*/
	MSD_U8 txConfOffset;
	MSD_CIPHER_SUITE cipher;
	MSD_BOOL protectFrame;
	MSD_BOOL txSecYEn;
	MSD_BOOL afterSkipTag;
	MSD_BOOL afterRTag;
	MSD_BOOL afterQTag;
	MSD_BOOL afterADTag;
	/*6 bits*/
	MSD_U8 secTagTCI;
}FIR_SECY_EGRESS;

typedef struct _FIR_SECY_INGRESS {
	MSD_BOOL allowZeroRxPN;
	/*4 bits*/
	MSD_U8 secYCounterIndex;
	/*6 bits*/
	MSD_U8 rxConfOffset;
	MSD_BOOL replayProtect;
	MSD_CIPHER_SUITE cipher;
	FIR_VALIDATE_FRAME validateFrame;
	MSD_BOOL rxSecYEn;
	MSD_U32 replayWindow;
}FIR_SECY_INGRESS;
	
typedef struct _FIR_MACSEC_INT_CONTROL_TXSC {
	MSD_BOOL txAutoRekeyIntEn;
	MSD_BOOL txSAInvalidIntEn;
	MSD_BOOL txPNThrIntEn;
}FIR_MACSEC_INT_CONTROL_TXSC;	

typedef struct _FIR_MACSEC_INT_TXSC {
	MSD_BOOL txAutoRekeyInt;
	MSD_BOOL txSAInvalidInt;
	MSD_BOOL txPNThrInt;
}FIR_MACSEC_INT_TXSC;

typedef struct _FIR_MSEC_TCAM_ACTION {
	/*6bits*/
	MSD_U8 txSCIndex;
	MSD_BOOL isControlledPort;
	/*6bits*/
	MSD_U8 secYIndex;
} FIR_MSEC_TCAM_ACTION;

/*Rule mask 0 care, 1 don't care*/
typedef struct _FIR_MSEC_RULE {
	/*7bits*/
	MSD_U8 keySPV;
	MSD_U8 maskSPV;
	/*1bits*/
	MSD_U8 keySecTagC;
	MSD_U8 maskSecTagC;
	/*1bits*/
	MSD_U8 keySecTagE;
	MSD_U8 maskSecTagE;
	/*1bits*/
	MSD_U8 keySecTag;
	MSD_U8 maskSecTag;
	/*1bits*/
	MSD_U8 keySkipSecTag;
	MSD_U8 maskSkipSecTag;
	/*1bits*/
	MSD_U8 keyRTag;
	MSD_U8 maskRTag;
	/*1bits*/
	MSD_U8 keyQTag;
	MSD_U8 maskQTag;
	/*1bits*/
	MSD_U8 keyADTag;
	MSD_U8 maskADTag;
	/*1bits*/
	MSD_U8 keyDirection;
	MSD_U8 maskDirection;

	MSD_U8 keyMACDA[6];
	MSD_U8 maskMACDA[6];
	MSD_U8 keyMACSA[6];
	MSD_U8 maskMACSA[6];

	/*4bits*/
	MSD_U8 keyAdPCP;
	MSD_U8 maskAdPCP;
	/*12bits*/
	MSD_U16 keyAdVID;
	MSD_U16 maskAdVID;
	/*4bits*/
	MSD_U8 keyQPCP;
	MSD_U8 maskQPCP;
	/*12bits*/
	MSD_U16 keyQVID;
	MSD_U16 maskQVID;

	/*16bits*/
	MSD_U16 keyLengthType;
	MSD_U16 maskLengthType;

	FIR_MSEC_TCAM_ACTION action;
}FIR_MSEC_RULE;

/*Macsec register R/W*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to read MACSec register.
*
* INPUTS:
*	dev  - physical devie
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
MSD_STATUS Fir_gmacsecRead
(
	IN MSD_QD_DEV dev,
	IN MSD_U32 regAddr,
	OUT MSD_U32* data
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to write MACSec register.
*
* INPUTS:
*	dev  - physical devie
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
MSD_STATUS Fir_gmacsecWrite
(
	IN MSD_QD_DEV dev,
	IN MSD_U32 regAddr,
	IN MSD_U32 data
);

/*init*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to flush all MACSec type module.
*
* INPUTS:
*	dev  - physical devie
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
MSD_STATUS Fir_gmacsecFlushAll
(
	IN MSD_QD_DEV *dev
);
	
/*Event*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to get event value.
*
* INPUTS:
*	dev  - physical devie
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
MSD_STATUS Fir_gmacsecGetEvent
(
	IN MSD_QD_DEV *dev,
	OUT FIR_MSEC_EVENT* event
);
MSD_STATUS Fir_gmacsecGetEventIntf
(
	IN MSD_QD_DEV *dev,
	OUT MSD_MSEC_EVENT * event
);
/*Rule*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create MACSec rule.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	rule - MSEC_RULE configuration
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
MSD_STATUS Fir_gmacsecCreateRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	IN FIR_MSEC_RULE* rule
);
MSD_STATUS Fir_gmacsecCreateRuleIntf
(
	IN MSD_QD_DEV *dev, 
	IN MSD_U8 ruleIndex,
	IN MSD_MSEC_RULE* rule
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete MACSec rule.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	ruleIndex - MSEC_RULE index
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
MSD_STATUS Fir_gmacsecDeleteRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex
);
	
/******************************************************************************
* DESCRIPTION:
*	This function is used to get MACSec rule.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	ruleIndex - rule index
*
* OUTPUTS:
*	rule - MSEC_RULE configuration
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	OUT FIR_MSEC_RULE* rule
);
MSD_STATUS Fir_gmacsecGetRuleIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	OUT MSD_MSEC_RULE* rule
);
/*SecY*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create MACSec Tx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secY - secY configuration
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
MSD_STATUS Fir_gmacsecCreateTxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	IN FIR_SECY_EGRESS* txsecY
);
MSD_STATUS Fir_gmacsecCreateTxSecYIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_EGRESS* txsecY
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to create MACSec Rx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secY - secY configuration
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
MSD_STATUS Fir_gmacsecCreateRxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	IN FIR_SECY_INGRESS* rxsecY
);
MSD_STATUS Fir_gmacsecCreateRxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_INGRESS* rxsecY
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete MACSec Tx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYIndex - secY index
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
MSD_STATUS Fir_gmacsecDeleteTxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete MACSec Rx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYIndex - secY index
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
MSD_STATUS Fir_gmacsecDeleteRxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get MACSec Tx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYIndex - secY index
*
* OUTPUTS:
*	secY
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	OUT FIR_SECY_EGRESS* secY
);
MSD_STATUS Fir_gmacsecGetTxSecYIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_EGRESS* secY
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get MACSec Rx secY.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYIndex - secY index
*
* OUTPUTS:
*	secY
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	OUT FIR_SECY_INGRESS* secY
);
MSD_STATUS Fir_gmacsecGetRxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_INGRESS* secY
);
/*RXSC*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create ingress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	rxSC - rxSC configuration
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
MSD_STATUS Fir_gmacsecCreateRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 rxSCYIndex,
	IN FIR_MSEC_RX_SC* rxSC
);
MSD_STATUS Fir_gmacsecCreateRxSCIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 rxSCYIndex,
	IN MSD_MSEC_RX_SC* rxSC
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete ingress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scIndex - sc index
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
MSD_STATUS Fir_gmacsecDeleteRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get ingress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scIndex - SC index
*
* OUTPUTS:
*	rxSC
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT FIR_MSEC_RX_SC* rxSC
);
MSD_STATUS Fir_gmacsecGetRxSCIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_RX_SC* rxSC
);
/*RXSA*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create ingress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	rxSA - rxSA configuration
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
MSD_STATUS Fir_gmacsecCreateRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN FIR_MSEC_RX_SA* rxSA
);
MSD_STATUS Fir_gmacsecCreateRxSAIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_RX_SA* rxSA
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete ingress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - sa index
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
MSD_STATUS Fir_gmacsecDeleteRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get ingress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - SA index
*
* OUTPUTS:
*	rxSA
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT FIR_MSEC_RX_SA* rxSA
);
MSD_STATUS Fir_gmacsecGetRxSAIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_RX_SA* rxSA
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get ingress sa next packet number.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - SA index
*
* OUTPUTS:
*	nextPN - next packet number
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRxSANextPN
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
);

/*TXSC*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create egress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	txSC - txSC configuration
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
MSD_STATUS Fir_gmacsecCreateTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	IN FIR_MSEC_TX_SC* txSC
);
MSD_STATUS Fir_gmacsecCreateTxSCIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_TX_SC* txSC
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete egress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scIndex - sc index
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
MSD_STATUS Fir_gmacsecDeleteTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get egress sc.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scIndex - SC index
*
* OUTPUTS:
*	rxSC
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT FIR_MSEC_TX_SC* txSC
);
MSD_STATUS Fir_gmacsecGetTxSCIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_TX_SC* txSC
);
/*TXSA*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to create egress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	txSA - txSA configuration
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
MSD_STATUS Fir_gmacsecCreateTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN FIR_MSEC_TX_SA* txSA
);
MSD_STATUS Fir_gmacsecCreateTxSAIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_TX_SA* txSA
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to delete egress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - sa index
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
MSD_STATUS Fir_gmacsecDeleteTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get egress sa.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - SA index
*
* OUTPUTS:
*	txSA
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT FIR_MSEC_TX_SA* txSA
);
MSD_STATUS Fir_gmacsecGetTxSAIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_TX_SA* txSA
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get egress sa next packet number.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	saIndex - SA index
*
* OUTPUTS:
*	nextPN - next packet number
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxSANextPN
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
);

/*States*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to get ingress sc related mibs.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scCounterIndex - SC counter index
*	autoClear - Enable or disable
*
* OUTPUTS:
*	states - ingress sc mibs
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRXSCStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_RX_SC_COUNTER* states
);
MSD_STATUS Fir_gmacsecGetRXSCStatesIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SC_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get ingress secY related mibs.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYCounterIndex - secY counter index
*	autoClear - Enable or disable 
*
* OUTPUTS:
*	states - ingress secY mibs
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRXSecYStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_RX_SECY_COUNTER* states
);
MSD_STATUS Fir_gmacsecGetRXSecYStatesIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SECY_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get egress sc related mibs.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	scCounterIndex - SC counter index
*	autoClear - Enable or disable 
*
* OUTPUTS:
*	states - egress sc mibs
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTXSCStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_TX_SC_COUNTER* states
);
MSD_STATUS Fir_gmacsecGetTXSCStatesIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SC_COUNTER* states
);
/******************************************************************************
* DESCRIPTION:
*	This function is used to get egress secY related mibs.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	secYCounterIndex - secY counter index
*	autoClear - Enable or disable
*
* OUTPUTS:
*	states - egress secY mibs
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTXSecYStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_TX_SECY_COUNTER* states
);
MSD_STATUS Fir_gmacsecGetTXSecYStatesIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SECY_COUNTER* states
);
/*MACSec enable*/
/******************************************************************************
* DESCRIPTION:
*	This function is used to enable or disable MACSec module and set ingress.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	portControlIns - port control ingress
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
MSD_STATUS Fir_gmacsecEnableIngressSet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to enable or disable MACSec module and set egress.
*
* INPUTS:
*	dev  - physical devie
*	portNum - port number
*	portControlEgs - port control Egress
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
MSD_STATUS Fir_gmacsecEnableEgressSet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
);

MSD_STATUS Fir_gmacsecEnableIngressGet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
);

MSD_STATUS Fir_gmacsecEnableEgressGet
(
	IN MSD_QD_DEV* dev,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to set Tx PN threshold.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
* pNThr - PN threshold
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
MSD_STATUS Fir_gmacsecSetTxPNThreshold
(
	IN MSD_QD_DEV *dev,
	IN MSD_U32 pNThr
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get Tx PN threshold.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
*
* OUTPUTS:
*	pNThr - PN threshold
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxPNThreshold
(
	IN MSD_QD_DEV *dev,
	OUT MSD_U32* pNThr
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to set Tx XPN threshold.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
* pNThr - PN threshold
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
MSD_STATUS Fir_gmacsecSetTxXPNThreshold
(
	IN MSD_QD_DEV *dev,
	IN MSD_U64 pNThr
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get Tx XPN threshold.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
*
* OUTPUTS:
*	pNThr - PN threshold
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxXPNThreshold
(
	IN MSD_QD_DEV *dev,
	OUT MSD_U64* pNThr
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to set default RX SCI.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
* defaultSCI - default SCI
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
MSD_STATUS Fir_gmacsecSetRxDefaultSCI
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultSCI
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get default RX SCI.
*
* INPUTS:
* dev  - physical devie
* portNum - port number
*
* OUTPUTS:
*	defaultSCI - default SCI
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetRxDefaultSCI
(
	IN MSD_QD_DEV *dev,
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
*	None
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecSetTxSCIntCtrl
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	IN FIR_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get interrupt control.
*
* INPUTS:
* dev  - physical devie
* index - index
*
* OUTPUTS:
*	 int_control_txsc - interrupt enable
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetTxSCIntCtrl
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	OUT FIR_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);

/******************************************************************************
* DESCRIPTION:
*	This function is used to get next Txsc interrupt status.
*
* INPUTS:
* dev  - physical devie
* index - index
* isAUTOClear - Auto clear, enable or disable
*
* OUTPUTS:
*	nextIndex - next valid index
*	txscint - txsc interrupt status
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS Fir_gmacsecGetNextTxSCIntStatus
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	IN MSD_BOOL isAUTOClear,
	OUT MSD_U8 *nextIndex,
	OUT FIR_MACSEC_INT_TXSC* txscInt
);

MSD_STATUS Fir_gmacsecSetTxSCIntCtrlIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	IN MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);

MSD_STATUS Fir_gmacsecGetTxSCIntCtrlIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	OUT MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
);

MSD_STATUS Fir_gmacsecGetNextTxSCIntStatusIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	IN MSD_BOOL isAUTOClear,
	OUT MSD_U8* nextIndex,
	OUT MSD_MACSEC_INT_TXSC* txscInt
);

#ifdef __cplusplus
}
#endif
#endif
