/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Fir_msdMACSecIntf.c
*
* DESCRIPTION:
*       API definitions for MACSec feature
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <fir/include/api/Fir_msdMACSec.h>
#include <fir/include/api/Fir_msdApiInternal.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

MSD_STATUS Fir_gmacsecGetEventIntf
(
	IN MSD_QD_DEV *dev,
	OUT MSD_MSEC_EVENT* event
)
{
	FIR_MSEC_EVENT msc_event;
	MSD_STATUS retVal;
	msdMemSet(&msc_event, 0, sizeof(FIR_MSEC_EVENT));
	retVal = Fir_gmacsecGetEvent(dev, &msc_event);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetEventIntf Fir_gmacsecGetEvent returned: %s.\n", msdDisplayStatus(retVal)));
	}
	event->msecCECCErrEvent = msc_event.msecCECCErrEvent;
	event->msecUCECCErrEvent = msc_event.msecUCECCErrEvent;
	event->msecCECCErrInjectMisEvent = msc_event.msecCECCErrInjectMisEvent;
	event->msecCECCOutMisEvent = msc_event.msecCECCOutMisEvent;
	event->msecRegParityErrEvent = msc_event.msecRegParityErrEvent;
	event->msecCECCErrEvent = msc_event.msecCECCErrEvent;
	return retVal;
}

MSD_STATUS Fir_gmacsecCreateRuleIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	IN MSD_MSEC_RULE* rule
)
{
	FIR_MSEC_RULE msc_rule;
	msdMemSet(&msc_rule, 0, sizeof(FIR_MSEC_RULE));
	msc_rule.action.isControlledPort = rule->action.isControlledPort;
	msc_rule.action.secYIndex = rule->action.secYIndex;
	msc_rule.action.txSCIndex = rule->action.txSCIndex;
	msc_rule.keyAdPCP = rule->keyAdPCP;
	msc_rule.keyADTag = rule->keyADTag;
	msc_rule.keyAdVID = rule->keyAdVID;
	msc_rule.keyDirection = rule->keyDirection;
	msc_rule.keyQPCP = rule->keyQPCP;
	msc_rule.keyQTag = rule->keyQTag;
	msc_rule.keyQVID = rule->keyQVID;
	msc_rule.keyRTag = rule->keyRTag;
	msc_rule.keySecTag = rule->keySecTag;
	msc_rule.keySecTagC = rule->keySecTagC;
	msc_rule.keySecTagE = rule->keySecTagE;
	msc_rule.keySkipTag = rule->keySkipTag;
	msc_rule.keySPV = rule->keySPV;
	msc_rule.keyLengthType = rule->keyLengthType;
	msc_rule.maskAdPCP = rule->maskAdPCP;
	msc_rule.maskADTag = rule->maskADTag;
	msc_rule.maskAdVID = rule->maskAdVID;
	msc_rule.maskDirection = rule->maskDirection;
	msc_rule.maskQPCP = rule->maskQPCP;
	msc_rule.maskQTag = rule->maskQTag;
	msc_rule.maskQVID = rule->maskQVID;
	msc_rule.maskRTag = rule->maskRTag;
	msc_rule.maskSecTag = rule->maskSecTag;
	msc_rule.maskSecTagC = rule->maskSecTagC;
	msc_rule.maskSecTagE = rule->maskSecTagE;
	msc_rule.maskSkipTag = rule->maskSkipTag;
	msc_rule.maskSPV = rule->maskSPV;
	msc_rule.maskLengthType = rule->maskLengthType;
	msdMemCpy(msc_rule.keyMacDA, rule->keyMacDA, 6 * sizeof(MSD_U8));
	msdMemCpy(msc_rule.keyMacSA, rule->keyMacSA, 6 * sizeof(MSD_U8));
	msdMemCpy(msc_rule.maskMacDA, rule->maskMacDA, 6 * sizeof(MSD_U8));
	msdMemCpy(msc_rule.maskMacSA, rule->maskMacSA, 6 * sizeof(MSD_U8));
	/*done*/
	return Fir_gmacsecCreateRule(dev, ruleIndex, &msc_rule);
}

MSD_STATUS Fir_gmacsecGetRuleIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 ruleIndex,
	OUT MSD_MSEC_RULE* rule
)
{
	FIR_MSEC_RULE msc_rule;
	MSD_STATUS retVal;
	msdMemSet(&msc_rule, 0, sizeof(FIR_MSEC_RULE));
	retVal = Fir_gmacsecGetRule(dev, ruleIndex, &msc_rule);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRuleIntf Fir_gmacsecGetRule returned: %s.\n", msdDisplayStatus(retVal)));
	}
	rule->action.isControlledPort = msc_rule.action.isControlledPort;
	rule->action.secYIndex = msc_rule.action.secYIndex;
	rule->action.txSCIndex = msc_rule.action.txSCIndex;
	rule->keyAdPCP = msc_rule.keyAdPCP;
	rule->keyADTag = msc_rule.keyADTag;
	rule->keyAdVID = msc_rule.keyAdVID;
	rule->keyDirection = msc_rule.keyDirection;
	msdMemCpy(rule->keyMacDA, msc_rule.keyMacDA, 6 * sizeof(MSD_U8));
	msdMemCpy(rule->keyMacSA, msc_rule.keyMacSA, 6 * sizeof(MSD_U8));
	rule->keyQPCP = msc_rule.keyQPCP;
	rule->keyQTag = msc_rule.keyQTag;
	rule->keyQVID = msc_rule.keyQVID;
	rule->keyRTag = msc_rule.keyRTag;
	rule->keySecTag = msc_rule.keySecTag;
	rule->keySecTagC = msc_rule.keySecTagC;
	rule->keySecTagE = msc_rule.keySecTagE;
	rule->keySkipTag = msc_rule.keySkipTag;
	rule->keySPV = msc_rule.keySPV;
	rule->keyLengthType = msc_rule.keyLengthType;
	rule->maskAdPCP = msc_rule.maskAdPCP;
	rule->maskADTag = msc_rule.maskADTag;
	rule->maskAdVID = msc_rule.maskAdVID;
	rule->maskDirection = msc_rule.maskDirection;
	msdMemCpy(rule->maskMacDA, msc_rule.maskMacDA, 6 * sizeof(MSD_U8));
	msdMemCpy(rule->maskMacSA, msc_rule.maskMacSA, 6 * sizeof(MSD_U8));
	rule->maskQPCP = msc_rule.maskQPCP;
	rule->maskQTag = msc_rule.maskQTag;
	rule->maskQVID = msc_rule.maskQVID;
	rule->maskRTag = msc_rule.maskRTag;
	rule->maskSecTag = msc_rule.maskSecTag;
	rule->maskSecTagC = msc_rule.maskSecTagC;
	rule->maskSecTagE = msc_rule.maskSecTagE;
	rule->maskSkipTag = msc_rule.maskSkipTag;
	rule->maskSPV = msc_rule.maskSPV;
	rule->maskLengthType = msc_rule.maskLengthType;
	return retVal;
}
/*done*/
MSD_STATUS Fir_gmacsecCreateTxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_EGRESS* txsecY
)
{
	FIR_SECY_EGRESS msc_txsecY;
	msdMemSet(&msc_txsecY, 0, sizeof(FIR_SECY_EGRESS));
	msc_txsecY.afterADTag = txsecY->afterADTag;
	msc_txsecY.afterQTag = txsecY->afterQTag;
	msc_txsecY.afterRTag = txsecY->afterRTag;
	msc_txsecY.afterSkipTag = txsecY->afterSkipTag;
	msc_txsecY.cipher = txsecY->cipher;
	msc_txsecY.protectFrame = txsecY->protectFrame;
	msc_txsecY.secTagTCI = txsecY->secTagTCI;
	msc_txsecY.secYCounterIndex = txsecY->secYCounterIndex;
	msc_txsecY.txConfOffset = txsecY->txConfOffset;
	msc_txsecY.txSecYEn = txsecY->txSecYEn;
	/*done*/
	return Fir_gmacsecCreateTxSecY(dev, secYIndex, &msc_txsecY);
}

MSD_STATUS Fir_gmacsecCreateRxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_INGRESS* rxsecY
)
{
	FIR_SECY_INGRESS msc_rxsecY;
	msdMemSet(&msc_rxsecY, 0, sizeof(FIR_SECY_INGRESS));
	msc_rxsecY.cipher = rxsecY->cipher;
	msc_rxsecY.replayProtect = rxsecY->replayProtect;
	msc_rxsecY.replayWindow = rxsecY->replayWindow;
	msc_rxsecY.rxConfOffset = rxsecY->rxConfOffset;
	msc_rxsecY.rxSecYEn = rxsecY->rxSecYEn;
	msc_rxsecY.secYCounterIndex = rxsecY->secYCounterIndex;
	msc_rxsecY.allowZeroRxPN = rxsecY->allowZeroRxPN;
	msc_rxsecY.validateFrame = rxsecY->validateFrame;
	/*done*/
	return Fir_gmacsecCreateRxSecY(dev, secYIndex, &msc_rxsecY);
}

MSD_STATUS Fir_gmacsecGetTxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_EGRESS* secY
)
{
	FIR_SECY_EGRESS msc_secY;
	MSD_STATUS retVal;
	msdMemSet(&msc_secY, 0, sizeof(FIR_SECY_EGRESS));
	retVal = Fir_gmacsecGetTxSecY(dev, secYIndex, &msc_secY);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTxSecYIntf Fir_gmacsecGetTxSecY returned: %s.\n", msdDisplayStatus(retVal)));
	}
	secY->afterADTag = msc_secY.afterADTag;
	secY->afterQTag = msc_secY.afterQTag;
	secY->afterRTag = msc_secY.afterRTag;
	secY->afterSkipTag = msc_secY.afterSkipTag;
	secY->cipher = msc_secY.cipher;
	secY->protectFrame = msc_secY.protectFrame;
	secY->secTagTCI = msc_secY.secTagTCI;
	secY->secYCounterIndex = msc_secY.secYCounterIndex;
	secY->txConfOffset = msc_secY.txConfOffset;
	secY->txSecYEn = msc_secY.txSecYEn;

	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecGetRxSecYIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_INGRESS* secY
)
{
	FIR_SECY_INGRESS msc_secY;
	MSD_STATUS retVal;
	msdMemSet(&msc_secY, 0, sizeof(FIR_SECY_INGRESS));
	retVal = Fir_gmacsecGetRxSecY(dev, secYIndex, &msc_secY);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRxSecYIntf Fir_gmacsecGetRxSecY returned: %s.\n", msdDisplayStatus(retVal)));
	}
	secY->cipher = msc_secY.cipher;
	secY->replayProtect = msc_secY.replayProtect;
	secY->replayWindow = msc_secY.replayWindow;
	secY->rxConfOffset = msc_secY.rxConfOffset;
	secY->rxSecYEn = msc_secY.rxSecYEn;
	secY->secYCounterIndex = msc_secY.secYCounterIndex;
	secY->allowZeroRxPN = msc_secY.allowZeroRxPN;
	secY->validateFrame = msc_secY.validateFrame;

	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecCreateRxSCIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 rxSCYIndex,
	IN MSD_MSEC_RX_SC* rxSC
)
{
	FIR_MSEC_RX_SC msc_rxSC;
	msdMemSet(&msc_rxSC, 0, sizeof(FIR_MSEC_RX_SC));
	msc_rxSC.scCounterIndex = rxSC->scCounterIndex;
	msc_rxSC.sci = rxSC->sci;
	msc_rxSC.secYIndex = rxSC->secYIndex;

	msc_rxSC.sa0An = rxSC->sa0An;
	msc_rxSC.sa0Index = rxSC->sa0Index;
	msc_rxSC.sa0Valid = rxSC->sa0Valid;

	msc_rxSC.sa1An = rxSC->sa1An;
	msc_rxSC.sa1Index = rxSC->sa1Index;
	msc_rxSC.sa1Valid = rxSC->sa1Valid;

	/*done*/
	return Fir_gmacsecCreateRxSC(dev, rxSCYIndex, &msc_rxSC);
}

MSD_STATUS Fir_gmacsecGetRxSCIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_RX_SC* rxSC
)
{
	FIR_MSEC_RX_SC msc_rxSC;
	MSD_STATUS retVal;
	msdMemSet(&msc_rxSC, 0, sizeof(FIR_MSEC_RX_SC));
	retVal = Fir_gmacsecGetRxSC(dev, scIndex, &msc_rxSC);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRxSCIntf Fir_gmacsecGetRxSC returned: %s.\n", msdDisplayStatus(retVal)));
	}
	rxSC->scCounterIndex = msc_rxSC.scCounterIndex;
	rxSC->sci = msc_rxSC.sci;
	rxSC->secYIndex = msc_rxSC.secYIndex;

	rxSC->sa0An = msc_rxSC.sa0An;
	rxSC->sa0Index = msc_rxSC.sa0Index;
	rxSC->sa0Valid = msc_rxSC.sa0Valid;

	rxSC->sa1An = msc_rxSC.sa1An;
	rxSC->sa1Index = msc_rxSC.sa1Index;
	rxSC->sa1Valid = msc_rxSC.sa1Valid;

	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecCreateRxSAIntf
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_RX_SA* rxSA
)
{
	FIR_MSEC_RX_SA msc_rxSA;
	msdMemSet(&msc_rxSA, 0, sizeof(FIR_MSEC_RX_SA));
	msc_rxSA.nextPN = rxSA->nextPN;
	msc_rxSA.rxSAValid = rxSA->rxSAValid;
	msdMemCpy(msc_rxSA.salt, rxSA->salt, 12 * sizeof(MSD_U8));
	msdMemCpy(msc_rxSA.sak, rxSA->sak, MSEC_SAK_LEN * sizeof(MSD_U8));
	msc_rxSA.ssci = rxSA->ssci;

	/*done*/
	return Fir_gmacsecCreateRxSA(dev, saIndex, &msc_rxSA);
}

MSD_STATUS Fir_gmacsecGetRxSAIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_RX_SA* rxSA
)
{
	FIR_MSEC_RX_SA msc_rxSA;
	MSD_STATUS retVal;
	msdMemSet(&msc_rxSA, 0, sizeof(FIR_MSEC_RX_SA));
	retVal = Fir_gmacsecGetRxSA(dev, saIndex, &msc_rxSA);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRxSAIntf Fir_gmacsecGetRxSA returned: %s.\n", msdDisplayStatus(retVal)));
	}
	rxSA->nextPN = msc_rxSA.nextPN;
	rxSA->rxSAValid = msc_rxSA.rxSAValid;
	msdMemCpy(rxSA->salt, msc_rxSA.salt, 12 * sizeof(MSD_U8));
	rxSA->ssci = msc_rxSA.ssci;
	msdMemCpy(rxSA->sak, msc_rxSA.sak, MSEC_SAK_LEN * sizeof(MSD_U8));
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecCreateTxSCIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_TX_SC* txSC
)
{
	FIR_MSEC_TX_SC msc_txSC;
	msdMemSet(&msc_txSC, 0, sizeof(FIR_MSEC_TX_SC));
	msc_txSC.txNextPNRolloEn = txSC->txNextPNRolloEn;
	msc_txSC.enableAutoRekey = txSC->enableAutoRekey;
	msc_txSC.isActiveSA1 = txSC->isActiveSA1;
	msc_txSC.sa0An = txSC->sa0An;
	msc_txSC.sa1An = txSC->sa1An;
	msc_txSC.sa0Index = txSC->sa0Index;
	msc_txSC.sa0Valid = txSC->sa0Valid;
	msc_txSC.sa1Index = txSC->sa1Index;
	msc_txSC.sa1Valid = txSC->sa1Valid;
	msc_txSC.scCounterIndex = txSC->scCounterIndex;
	msc_txSC.sci = txSC->sci;
	/*done*/
	return Fir_gmacsecCreateTxSC(dev, scIndex, &msc_txSC);
}

MSD_STATUS Fir_gmacsecGetTxSCIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_TX_SC* txSC
)
{
	FIR_MSEC_TX_SC msc_txSC;
	MSD_STATUS retVal;
	msdMemSet(&msc_txSC, 0, sizeof(FIR_MSEC_TX_SC));
	retVal = Fir_gmacsecGetTxSC(dev, scIndex, &msc_txSC);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTxSCIntf Fir_gmacsecGetTxSC returned: %s.\n", msdDisplayStatus(retVal)));
	}
	txSC->enableAutoRekey = msc_txSC.enableAutoRekey;
	txSC->txNextPNRolloEn = msc_txSC.txNextPNRolloEn;
	txSC->isActiveSA1 = msc_txSC.isActiveSA1;
	txSC->sa0An = msc_txSC.sa0An;
	txSC->sa1An = msc_txSC.sa1An;
	txSC->sa0Index = msc_txSC.sa0Index;
	txSC->sa0Valid = msc_txSC.sa0Valid;
	txSC->sa1Index = msc_txSC.sa1Index;
	txSC->sa1Valid = msc_txSC.sa1Valid;
	txSC->scCounterIndex = msc_txSC.scCounterIndex;
	txSC->sci = msc_txSC.sci;
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecCreateTxSAIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_TX_SA* txSA
)
{
	FIR_MSEC_TX_SA msc_txSA;
	msdMemSet(&msc_txSA, 0, sizeof(FIR_MSEC_TX_SA));
	msc_txSA.nextPN = txSA->nextPN;
	msc_txSA.txSAValid = txSA->txSAValid;
	msdMemCpy(msc_txSA.salt, txSA->salt, 12 * sizeof(MSD_U8));
	msdMemCpy(msc_txSA.sak, txSA->sak, MSEC_SAK_LEN * sizeof(MSD_U8));
	msc_txSA.ssci = txSA->ssci;
	/*done*/
	return Fir_gmacsecCreateTxSA(dev, saIndex, &msc_txSA);
}

MSD_STATUS Fir_gmacsecGetTxSAIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_TX_SA* txSA
)
{
	FIR_MSEC_TX_SA msc_txSA;
	MSD_STATUS retVal;
	msdMemSet(&msc_txSA, 0, sizeof(FIR_MSEC_TX_SA));
	retVal = Fir_gmacsecGetTxSA(dev, saIndex, &msc_txSA);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTxSAIntf Fir_gmacsecGetTxSA returned: %s.\n", msdDisplayStatus(retVal)));
	}
	txSA->nextPN = msc_txSA.nextPN;
	msdMemCpy(txSA->salt, msc_txSA.salt, 12 * sizeof(MSD_U8));
	msdMemCpy(txSA->sak, msc_txSA.sak, MSEC_SAK_LEN * sizeof(MSD_U8));
	txSA->ssci = msc_txSA.ssci;
	txSA->txSAValid = msc_txSA.txSAValid;
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecGetRXSCStatesIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SC_COUNTER* states
)
{
	FIR_MSEC_RX_SC_COUNTER msc_states;
	MSD_STATUS retVal;
	msdMemSet(&msc_states, 0, sizeof(FIR_MSEC_RX_SC_COUNTER));
	retVal = Fir_gmacsecGetRXSCStates(dev, scCounterIndex, autoClear, &msc_states);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRXSCStatesIntf Fir_gmacsecGetRXSCStates returned: %s.\n", msdDisplayStatus(retVal)));
	}
	states->inPktsSCDelayed = msc_states.inPktsSCDelayed;
	states->inPktsSCInvalid = msc_states.inPktsSCInvalid;
	states->inPktsSCLate = msc_states.inPktsSCLate;
	states->inPktsSCNotValid = msc_states.inPktsSCNotValid;
	states->inPktsSCOK = msc_states.inPktsSCOK;
	states->inPktsSCUnchecked = msc_states.inPktsSCUnchecked;
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecGetRXSecYStatesIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SECY_COUNTER* states
)
{
	FIR_MSEC_RX_SECY_COUNTER msc_states;
	MSD_STATUS retVal;
	msdMemSet(&msc_states, 0, sizeof(FIR_MSEC_RX_SECY_COUNTER));
	retVal = Fir_gmacsecGetRXSecYStates(dev, secYCounterIndex, autoClear, &msc_states);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetRXSecYStatesIntf Fir_gmacsecGetRXSecYStates returned: %s.\n", msdDisplayStatus(retVal)));
	}
	states->inOctetsSecYControlledPort = msc_states.inOctetsSecYControlledPort;
	states->inOctetsSecYDecrypted = msc_states.inOctetsSecYDecrypted;
	states->inOctetsSecYValidated = msc_states.inOctetsSecYValidated;
	states->inPktsSecYBadTag = msc_states.inPktsSecYBadTag;
	states->inPktsSecYBroadCast = msc_states.inPktsSecYBroadCast;
	states->inPktsSecYMulticast = msc_states.inPktsSecYMulticast;
	states->inPktsSecYNoSA = msc_states.inPktsSecYNoSA;
	states->inPktsSecYNoSAErr = msc_states.inPktsSecYNoSAErr;
	states->inPktsSecYNoTag = msc_states.inPktsSecYNoTag;
	states->inPktsSecYUnicast = msc_states.inPktsSecYUnicast;
	states->inPktsSecYUnTagged = msc_states.inPktsSecYUnTagged;
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecGetTXSCStatesIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SC_COUNTER* states
)
{
	FIR_MSEC_TX_SC_COUNTER msc_states;
	MSD_STATUS retVal;
	msdMemSet(&msc_states, 0, sizeof(FIR_MSEC_TX_SC_COUNTER));
	retVal = Fir_gmacsecGetTXSCStates(dev, scCounterIndex, autoClear, &msc_states);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTXSCStatesIntf Fir_gmacsecGetTXSCStates returned: %s.\n", msdDisplayStatus(retVal)));
	}
	states->outPktsEncrypted = msc_states.outPktsEncrypted;
	states->outPktsProtected = msc_states.outPktsProtected;
	return retVal;
}

MSD_STATUS Fir_gmacsecGetTXSecYStatesIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SECY_COUNTER* states
)
{
	FIR_MSEC_TX_SECY_COUNTER msc_states;
	MSD_STATUS retVal;
	msdMemSet(&msc_states, 0, sizeof(FIR_MSEC_TX_SECY_COUNTER));
	retVal = Fir_gmacsecGetTXSecYStates(dev, secYCounterIndex, autoClear, &msc_states);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTXSecYStatesIntf Fir_gmacsecGetTXSecYStates returned: %s.\n", msdDisplayStatus(retVal)));
	}
	states->outOctetsSecYControlledPort = msc_states.outOctetsSecYControlledPort;
	states->outOctetsSecYEncrypted = msc_states.outOctetsSecYEncrypted;
	states->outOctetsSecYProtected = msc_states.outOctetsSecYProtected;
	states->outPktSecYBroadcast = msc_states.outPktSecYBroadcast;
	states->outPktsSecYMulticast = msc_states.outPktsSecYMulticast;
	states->outPktsSecYTooLong = msc_states.outPktsSecYTooLong;
	states->outPktsSecYUnicast = msc_states.outPktsSecYUnicast;
	states->outPktsSecYuntagged = msc_states.outPktsSecYuntagged;
	states->outPktsSecYNoSAError = msc_states.outPktsSecYNoSAError;
	/*done*/
	return retVal;
}

MSD_STATUS Fir_gmacsecSetTxSCIntCtrlIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	IN MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
)
{
	FIR_MACSEC_INT_CONTROL_TXSC msc_txscIntCtrl;
	msdMemSet(&msc_txscIntCtrl, 0, sizeof(FIR_MACSEC_INT_CONTROL_TXSC));
	msc_txscIntCtrl.txAutoRekeyIntEn = txscIntCtrl->txAutoRekeyIntEn;
	msc_txscIntCtrl.txSAInvalidIntEn = txscIntCtrl->txSAInvalidIntEn;
	msc_txscIntCtrl.txPNThrIntEn = txscIntCtrl->txPNThrIntEn;
	/*done*/
	return Fir_gmacsecSetTxSCIntCtrl(dev, index, &msc_txscIntCtrl);
}

MSD_STATUS Fir_gmacsecGetTxSCIntCtrlIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	OUT MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
)
{
	FIR_MACSEC_INT_CONTROL_TXSC msc_txscIntCtrl;
	MSD_STATUS retVal;
	msdMemSet(&msc_txscIntCtrl, 0, sizeof(FIR_MACSEC_INT_CONTROL_TXSC));
	retVal = Fir_gmacsecGetTxSCIntCtrl(dev, index, &msc_txscIntCtrl);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gmacsecGetTxSCIntCtrlIntf returned: %s.\n", msdDisplayStatus(retVal)));
	}
	txscIntCtrl->txAutoRekeyIntEn = msc_txscIntCtrl.txAutoRekeyIntEn;
	txscIntCtrl->txSAInvalidIntEn = msc_txscIntCtrl.txSAInvalidIntEn;
	txscIntCtrl->txPNThrIntEn = msc_txscIntCtrl.txPNThrIntEn;
	/*done*/
	return retVal;
}


MSD_STATUS Fir_gmacsecGetNextTxSCIntStatusIntf
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 index,
	IN MSD_BOOL isAutoClear,
	OUT MSD_U8* nextIndex,
	OUT MSD_MACSEC_INT_TXSC* txscInt
)
{
	FIR_MACSEC_INT_TXSC msc_txscInt;
	MSD_STATUS retVal;
	msdMemSet(&msc_txscInt, 0, sizeof(FIR_MACSEC_INT_TXSC));
	retVal = Fir_gmacsecGetNextTxSCIntStatus(dev, index, isAutoClear, nextIndex, &msc_txscInt);
	if (MSD_OK != retVal)
	{
		return retVal;
	}
	txscInt->txAutoRekeyInt = msc_txscInt.txAutoRekeyInt;
	txscInt->txSAInvalidInt = msc_txscInt.txSAInvalidInt;
	txscInt->txPNThrInt = msc_txscInt.txPNThrInt;
	/*done*/
	return retVal;
}