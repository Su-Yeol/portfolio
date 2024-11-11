/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdMACSec.c
*
* DESCRIPTION:
*    This sample will demonstrate how to configure MACSec.
*
* DEPENDENCIES:   None.
*
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample:
*       Configure MACSec egress encryption or ingress decryption on port 1.
*
*
* Note:
*		None.
*
* Support device:
*		Fir
*
********************************************************************************/
MSD_STATUS sample_msdMACSecCommonEgress(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_U8 txPort = 1;
	MSD_BOOL isEnable = 0;
	MSD_U8  ruleIndex = 0;
	MSD_U8  scIndex = 0;
	MSD_U8  secYIndex = 0;
	MSD_CIPHER_SUITE cipherSuit = MSD_AES_128_PN;

	MSD_AUTO_CLEAR autoClear = MSD_DISABLE;
	MSD_MSEC_TX_SC_COUNTER txScStates;
	MSD_MSEC_TX_SECY_COUNTER txSecYStates;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacSecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecFlushAll fail.\n"));
		return status;
	}

	/*Step 1, MACSec Enable on port*/
	isEnable = 1;
	if ((status = msdMacSecEnableEgressSet(devNum, txPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecEnableSet fail.\n"));
		return status;
	}
	
	/*Step 2, Configure MACSec rule*/
	ruleIndex = 1;
	secYIndex = 1;
	scIndex = 1;

	MSD_MSEC_RULE rule;
	msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));

	/*
	*    Initialize MACSec DA address is
	*    AA-BB-CC-DD-EE-00.
	*/
	rule.keyMacDA[0] = 0xAA;
	rule.keyMacDA[1] = 0xBB;
	rule.keyMacDA[2] = 0xCC;
	rule.keyMacDA[3] = 0xDD;
	rule.keyMacDA[4] = 0xEE;
	rule.keyMacDA[5] = 0x00;

	rule.maskMacDA[0] = 0xFF;
	rule.maskMacDA[1] = 0xFF;
	rule.maskMacDA[2] = 0xFF;
	rule.maskMacDA[3] = 0xFF;
	rule.maskMacDA[4] = 0xFF;
	rule.maskMacDA[5] = 0xFF;

	rule.action.isControlledPort = 1;
	rule.action.secYIndex = secYIndex;
	rule.action.txSCIndex = scIndex;

	status = msdMacSecCreateRule(devNum, ruleIndex, &rule);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRule fail.\n"));
		return status;
	}

	/*Step 3, Configure MACSec secY*/
	MSD_SECY_EGRESS txSecY;
	msdMemSet(&txSecY, 0, sizeof(MSD_SECY_EGRESS));

	txSecY.cipher = cipherSuit;
	txSecY.protectFrame = 0x1;
	txSecY.secTagTCI = 0xB;
	txSecY.secYCounterIndex = secYIndex;
	txSecY.txSecYEn = 0x1;

	status = msdMacSecCreateTxSecY(devNum, secYIndex, &txSecY);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateTxSecY fail.\n"));
		return status;
	}

	/*Step 4, Configure MACSec SC*/
	MSD_MSEC_TX_SC txSC;
	msdMemSet(&txSC, 0, sizeof(MSD_MSEC_TX_SC));

	txSC.sa0Valid = 1;
	txSC.sa0Index = 0;
	txSC.sa0An = 0;
	txSC.sa1Valid = 1;
	txSC.sa1Index = 1;
	txSC.sa1An = 1;
	txSC.scCounterIndex = scIndex;
	txSC.sci = 0xFFFFFFFFFFFFFFFF;
	txSC.isActiveSA1 = 0; /* using SA0 */

	status = msdMacSecCreateTxSC(devNum, scIndex, &txSC);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateTxSC fail.\n"));
		return status;
	}

	/*Step 5, Configure MACSec SA*/
	MSD_MSEC_TX_SA txSA0;
	msdMemSet(&txSA0, 0, sizeof(MSD_MSEC_TX_SA));

	txSA0.txSAValid = 1;
	txSA0.nextPN = 1;

	int i;
	for (i = 0; i < MSEC_SAK_LENTH; i++)
	{
		txSA0.sak[i] = i;
	}
	
	status = msdMacSecCreateTxSA(devNum, txSC.sa0Index, &txSA0);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateTxSA fail.\n"));
		return status;
	}

	MSD_MSEC_TX_SA txSA1;
	msdMemSet(&txSA1, 0, sizeof(MSD_MSEC_TX_SA));

	txSA1.txSAValid = 1;
	txSA1.nextPN = 1;

	for (i = 0; i < MSEC_SAK_LENTH; i++)
	{
		txSA1.sak[i] = 2 * i;
	}

	status = msdMacSecCreateTxSA(devNum, txSC.sa1Index, &txSA1);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateTxSA fail.\n"));
		return status;
	}

	/*Step 6, Send traffic with DA address AA-BB-CC-DD-EE-00.*/


	/*Step 7, Check MACSec Status*/

	msdMemSet(&txSecYStates, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
	if ((status = msdMacSecGetTXSecYStates(devNum, secYIndex, autoClear, &txSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecGetTxSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txSecYStates.outOctetsSecYControlledPort:\t%llu\n", txSecYStates.outOctetsSecYControlledPort));
		MSG_PRINT(("txSecYStates.outOctetsSecYEncrypted:     \t%llu\n", txSecYStates.outOctetsSecYEncrypted));
		MSG_PRINT(("txSecYStates.outOctetsSecYProtected:     \t%llu\n", txSecYStates.outOctetsSecYProtected));
		MSG_PRINT(("txSecYStates.outPktSecYBroadcast:	     \t%d\n", txSecYStates.outPktSecYBroadcast));
		MSG_PRINT(("txSecYStates.outPktsSecYMulticast:       \t%d\n", txSecYStates.outPktsSecYMulticast));
		MSG_PRINT(("txSecYStates.outPktsSecYTooLong):        \t%d\n", txSecYStates.outPktsSecYTooLong));
		MSG_PRINT(("txSecYStates.outPktsSecYUnicast:         \t%d\n", txSecYStates.outPktsSecYUnicast));
		MSG_PRINT(("txSecYStates.outPktsSecYuntagged:        \t%d\n", txSecYStates.outPktsSecYuntagged));
	}

	msdMemSet(&txScStates, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
	if ((status = msdMacSecGetTXSCStates(devNum, scIndex, autoClear, &txScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecGetTXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txScStates.outPktsEncrypted:             \t%d\n", txScStates.outPktsEncrypted));
		MSG_PRINT(("txScStates.outPktsProtected:             \t%d\n", txScStates.outPktsProtected));
	}

	return status;
}

MSD_STATUS sample_msdMACSecCommonIngress(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_U8 rxPort = 1;
	MSD_BOOL isEnable = 0;
	MSD_U8  ruleIndex = 0;
	MSD_U8  scIndex = 0;
	MSD_U8  secYIndex = 0;
	MSD_CIPHER_SUITE cipherSuite = MSD_AES_128_PN;
	MSD_VALIDATE_FRAME validateFrame = MSD_STRICT;
	MSD_U64  nextPN = 0;
	MSD_U64  sci = 0;

	MSD_AUTO_CLEAR autoClear = MSD_ENABLE;
	MSD_MSEC_RX_SC_COUNTER rxScStates;
	MSD_MSEC_RX_SECY_COUNTER rxSecYStates;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacSecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecFlushAll fail.\n"));
		return status;
	}
	
	/*Step 1, Macsec Enable*/
	isEnable = 1;
	if ((status = msdMacSecEnableIngressSet(devNum, rxPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecIngressEnableSet fail.\n"));
		return status;
	}

	/*Step 2, Configure MACSec rule*/
	ruleIndex = 1;
	secYIndex = 1;
	scIndex = 1;

	MSD_MSEC_RULE rule;
	msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));

	/*
	*    Initialize MACSec DA address is
	*    AA-BB-CC-DD-EE-00.
	*/
	rule.keyMacDA[0] = 0xAA;
	rule.keyMacDA[1] = 0xBB;
	rule.keyMacDA[2] = 0xCC;
	rule.keyMacDA[3] = 0xDD;
	rule.keyMacDA[4] = 0xEE;
	rule.keyMacDA[5] = 0x00;

	rule.maskMacDA[0] = 0xFF;
	rule.maskMacDA[1] = 0xFF;
	rule.maskMacDA[2] = 0xFF;
	rule.maskMacDA[3] = 0xFF;
	rule.maskMacDA[4] = 0xFF;
	rule.maskMacDA[5] = 0xFF;

	rule.action.isControlledPort = 1;
	rule.action.secYIndex = secYIndex;

	status = msdMacSecCreateRule(devNum, ruleIndex, &rule);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRule fail.\n"));
		return status;
	}

	/*Step 3, Configure MACSec secY*/
	MSD_SECY_INGRESS rxSecY;
	msdMemSet(&rxSecY, 0, sizeof(MSD_SECY_INGRESS));

	rxSecY.cipher = cipherSuite;
	rxSecY.validateFrame = validateFrame;
	rxSecY.secYCounterIndex = secYIndex;
	rxSecY.rxSecYEn = 1;

	status = msdMacSecCreateRxSecY(devNum, secYIndex, &rxSecY);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRxSecY fail.\n"));
		return status;
	}

	/*Step 4, Configure MACSec SC*/
	MSD_MSEC_RX_SC rxSC;
	msdMemSet(&rxSC, 0, sizeof(MSD_MSEC_RX_SC));

	rxSC.scCounterIndex = scIndex;
	rxSC.sci = 0xFFFFFFFFFFFFFFFF;
	rxSC.secYIndex = secYIndex;
	rxSC.sa0Valid = 1;
	rxSC.sa0Index = 0;
	rxSC.sa0An = 0;

	rxSC.sa1Valid = 1;
	rxSC.sa1Index = 1;
	rxSC.sa1An = 1;

	status = msdMacSecCreateRxSC(devNum, scIndex, &rxSC);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRxSC fail.\n"));
		return status;
	}

	/*Step 5, Configure MACSec SA*/
	MSD_MSEC_RX_SA rxSA0;
	msdMemSet(&rxSA0, 0, sizeof(MSD_MSEC_RX_SA));

	rxSA0.rxSAValid = 1;
	rxSA0.nextPN = 1;

	int i;
	for (i = 0; i < MSEC_SAK_LENTH; i++)
	{
		rxSA0.sak[i] = i;
	}

	status = msdMacSecCreateRxSA(devNum, rxSC.sa0Index, &rxSA0);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRxSA fail.\n"));
		return status;
	}

	MSD_MSEC_RX_SA rxSA1;
	msdMemSet(&rxSA1, 0, sizeof(MSD_MSEC_RX_SA));

	rxSA1.rxSAValid = 1;
	rxSA1.nextPN = 1;

	for (i = 0; i < MSEC_SAK_LENTH; i++)
	{
		rxSA1.sak[i] = 2 * i;
	}

	status = msdMacSecCreateRxSA(devNum, rxSC.sa1Index, &rxSA1);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdMacSecCreateRxSA fail.\n"));
		return status;
	}

	/*Step 6, Send encrypted traffic with DA address AA-BB-CC-DD-EE-00.*/


	/*Step 7, Check Status*/

	msdMemSet(&rxSecYStates, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
	if ((status = msdMacSecGetRXSecYStates(devNum, secYIndex, autoClear, &rxSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecGetRXSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxSecYStates.inOctetsSecYControlledPort:\t%llu\n", rxSecYStates.inOctetsSecYControlledPort));
		MSG_PRINT(("rxSecYStates.inOctetsSecYDecrypted:     \t%llu\n", rxSecYStates.inOctetsSecYDecrypted));
		MSG_PRINT(("rxSecYStates.inOctetsSecYValidated:     \t%llu\n", rxSecYStates.inOctetsSecYValidated));
		MSG_PRINT(("rxSecYStates.inPktsSecYBadTag:          \t%d\n", rxSecYStates.inPktsSecYBadTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYBroadCast:       \t%d\n", rxSecYStates.inPktsSecYBroadCast));
		MSG_PRINT(("rxSecYStates.inPktsSecYMulticast        \t%d\n", rxSecYStates.inPktsSecYMulticast));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSA:            \t%d\n", rxSecYStates.inPktsSecYNoSA));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSAErr:         \t%d\n", rxSecYStates.inPktsSecYNoSAErr));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoTag:           \t%d\n", rxSecYStates.inPktsSecYNoTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnicast:         \t%d\n", rxSecYStates.inPktsSecYUnicast));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnTagged:        \t%d\n", rxSecYStates.inPktsSecYUnTagged));
	}

	msdMemSet(&rxScStates, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
	if ((status = msdMacSecGetRXSCStates(devNum, scIndex, autoClear, &rxScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacSecGetRXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxScStates.inPktsSCLate:                \t%d\n", rxScStates.inPktsSCLate));
		MSG_PRINT(("rxScStates.inPktsSCNotValid:            \t%d\n", rxScStates.inPktsSCNotValid));
		MSG_PRINT(("rxScStates.inPktsSCInvalid:             \t%d\n", rxScStates.inPktsSCInvalid));
		MSG_PRINT(("rxScStates.inPktsSCDelayed:             \t%d\n", rxScStates.inPktsSCDelayed));
		MSG_PRINT(("rxScStates.inPktsSCUnchecked:           \t%d\n", rxScStates.inPktsSCUnchecked));
		MSG_PRINT(("rxScStates.inPktsSCOK:                  \t%d\n", rxScStates.inPktsSCOK));
	}

	return status;
}


