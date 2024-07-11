/**********************************************************************************************
* Copyright (c) 2022 Marvell.
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
* Sample #1:
*       Configure basic egress on port 1.
*
*
* Note:
*		None.
*
* Comment:
*		Only support Fir
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
	MSD_CIPHER_SUITE cipherSuite = MSD_AES_CTR_128;
	MSD_BOOL isAutoRekey = 0;
	MSD_U8   tci = 0;
	MSD_U64  nextPN = 0;
	MSD_U64  thresholdPN = 0;
	MSD_BOOL isActiveSA1 = 0;
	MSD_U64  sci = 0;
	MSD_U8   secYCounterIndex = 0;
	MSD_U8   scCounterIndex = 0;

	MSD_AUTO_CLEAR autoClear = MSD_ENABLE;
	MSD_MSEC_RX_SC_COUNTER rxScStates;
	MSD_MSEC_RX_SECY_COUNTER rxSecYStates;
	MSD_MSEC_TX_SC_COUNTER txScStates;
	MSD_MSEC_TX_SECY_COUNTER txSecYStates;

	MSD_QD_DEV *dev = sohoDevGet(devNum);

	MSG_PRINT(("msdMacsecCommonEgress debugging.\n"));
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacsecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecFlushAll fail.\n"));
		return status;
	}

	/*Step 1, Macsec Enable*/
	isEnable = 1;
	if ((status = msdMacsecEnableEgressSet(devNum, txPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecEnableSet fail.\n"));
		return status;
	}
	
	/*Step 2, Configure Egress*/
	ruleIndex = 1;
	scIndex = 1;
	secYIndex = 1;
	tci = 0xb;
	nextPN = 1;
	isActiveSA1 = 1;
	sci = 0xFFFFFFFFFFFF;
	if ((status = msdMacsecConfigureEgress(devNum, ruleIndex, scIndex, secYIndex, cipherSuite, isAutoRekey, tci, nextPN, thresholdPN, isActiveSA1, sci, secYCounterIndex, scCounterIndex)) != MSD_OK)
	{
		MSG_PRINT(("msdConfigureEgress fail.\n"));
		return status;
	}

	/*Step 3, Send traffic mibs*/
	/*Transmit pkts here, using Pktgen.*/
	/*Step 4, Stop traffic*/

	/*Step 5, Check States*/
	
	msdMemSet(&rxScStates, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
	if ((status = msdMacsecGetRXSCStates(devNum, scCounterIndex, autoClear, &rxScStates)) != MSD_OK)
	{ 
		MSG_PRINT(("msdMacsecGetRXSCStates fail.\n")); 
		return status;
	}
	else 
	{
		MSG_PRINT(("rxScStates.inPktsSCLate:        \t%d\n", rxScStates.inPktsSCLate));
		MSG_PRINT(("rxScStates.inPktsSCNotValid:    \t%d\n", rxScStates.inPktsSCNotValid));
		MSG_PRINT(("rxScStates.inPktsSCInvalid:     \t%d\n", rxScStates.inPktsSCInvalid));
		MSG_PRINT(("rxScStates.inPktsSCDelayed:     \t%d\n", rxScStates.inPktsSCDelayed));
		MSG_PRINT(("rxScStates.inPktsSCUnchecked:   \t%d\n", rxScStates.inPktsSCUnchecked));
		MSG_PRINT(("rxScStates.inPktsSCOK:          \t%d\n", rxScStates.inPktsSCOK));
	}
	msdMemSet(&rxSecYStates, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
	if ((status = msdMacsecGetRXSecYStates(devNum, secYCounterIndex, autoClear, &rxSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxSecYStates.inOctetsSecYControlledPort:\t%d\n", rxSecYStates.inOctetsSecYControlledPort));
		MSG_PRINT(("rxSecYStates.inOctetsSecYDecrypted:     \t%d\n", rxSecYStates.inOctetsSecYDecrypted));
		MSG_PRINT(("rxSecYStates.inOctetsSecYValidated:     \t%d\n", rxSecYStates.inOctetsSecYValidated));
		MSG_PRINT(("rxSecYStates.inPktsSecYBadTag:          \t%d\n", rxSecYStates.inPktsSecYBadTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYBroadCast:       \t%d\n", rxSecYStates.inPktsSecYBroadCast));
		MSG_PRINT(("rxSecYStates.inPktsSecYMulticast        \t%d\n", rxSecYStates.inPktsSecYMulticast));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSA:            \t%d\n", rxSecYStates.inPktsSecYNoSA));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSAErr:         \t%d\n", rxSecYStates.inPktsSecYNoSAErr));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoTag:           \t%d\n", rxSecYStates.inPktsSecYNoTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnicast:         \t%d\n", rxSecYStates.inPktsSecYUnicast));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnTagged:        \t%d\n", rxSecYStates.inPktsSecYUnTagged));
	}
	msdMemSet(&txScStates, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
	if ((status = msdMacsecGetTXSCStates(devNum, scCounterIndex, autoClear, &txScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txScStates.outPktsEncrypted:    \t%d\n", txScStates.outPktsEncrypted));
		MSG_PRINT(("txScStates.outPktsProtected:    \t%d\n", txScStates.outPktsProtected));
	}
	msdMemSet(&txSecYStates, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
	if ((status = msdMacsecGetTXSecYStates(devNum, secYCounterIndex, autoClear, &txSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTxSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txSecYStates.outOctetsSecYControlledPort:\t%d\n", txSecYStates.outOctetsSecYControlledPort));
		MSG_PRINT(("txSecYStates.outOctetsSecYEncrypted:     \t%d\n", txSecYStates.outOctetsSecYEncrypted));
		MSG_PRINT(("txSecYStates.outOctetsSecYProtected:     \t%d\n", txSecYStates.outOctetsSecYProtected));
		MSG_PRINT(("txSecYStates.outPktSecYBroadcast:	     \t%d\n", txSecYStates.outPktSecYBroadcast));
		MSG_PRINT(("txSecYStates.outPktsSecYMulticast:       \t%d\n", txSecYStates.outPktsSecYMulticast));
		MSG_PRINT(("txSecYStates.outPktsSecYTooLong):        \t%d\n", txSecYStates.outPktsSecYTooLong));
		MSG_PRINT(("txSecYStates.outPktsSecYUnicast:         \t%d\n", txSecYStates.outPktsSecYUnicast));
		MSG_PRINT(("txSecYStates.outPktsSecYuntagged:        \t%d\n", txSecYStates.outPktsSecYuntagged));
	}
	return status;
}
MSD_STATUS sample_msdMACSecCommonIngress(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_U8 txPort = 1;
	MSD_BOOL isEnable = 0;
	MSD_U8  ruleIndex = 0;
	MSD_U8  scIndex = 0;
	MSD_U8  secYIndex = 0;
	MSD_CIPHER_SUITE cipherSuite = MSD_AES_CTR_128;
	MSD_U64  nextPN = 0;
	MSD_U64  sci = 0;
	MSD_U8   secYCounterIndex = 0;
	MSD_U8   scCounterIndex = 0;
	MSD_BOOL allowZeroRxPN = 0;
	MSD_BOOL replayProtect = 0;
	MSD_32  replayWindow = 0;
	MSD_VALIDATE_FRAME validateFrame = MSD_NULL;
	MSD_VALIDATE_FRAME validFrame = MSD_NULL;

	MSD_AUTO_CLEAR autoClear = MSD_ENABLE;
	MSD_MSEC_RX_SC_COUNTER rxScStates;
	MSD_MSEC_RX_SECY_COUNTER rxSecYStates;
	MSD_MSEC_TX_SC_COUNTER txScStates;
	MSD_MSEC_TX_SECY_COUNTER txSecYStates;

	MSD_QD_DEV *dev = sohoDevGet(devNum);

	MSG_PRINT(("msdMacsecCommonIngress debugging.\n"));
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacsecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecFlushAll fail.\n"));
		return status;
	}
	
	/*Step 1, Macsec Enable*/
	isEnable = 1;
	if ((status = msdMacsecEnableIngressSet(devNum, txPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecIngressEnableSet fail.\n"));
		return status;
	}


	/*Step 2, Configure Ingress*/
	ruleIndex = 1;
	scIndex = 1;
	secYIndex = 1;
	validateFrame = MSD_STRICT;
	nextPN = 1;
	sci = 0xFFFFFFFFFFFF;
	replayWindow = 0xFFFFFFFE;
	if ((status = msdMacsecConfigureIngress(devNum, ruleIndex, scIndex, secYIndex, cipherSuite, validateFrame, nextPN, sci, secYCounterIndex, scCounterIndex, allowZeroRxPN, replayProtect, replayWindow)) != MSD_OK)
	{
		MSG_PRINT(("msdConfigureIngress fail.\n"));
		return status;
	}

	/*Step 3, Send traffic mibs*/
	/*Transmit pkts here, using Pktgen.*/
	/*Step 4, Stop traffic*/

	/*Step 5, Check States*/

	msdMemSet(&rxScStates, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
	if ((status = msdMacsecGetRXSCStates(devNum, scCounterIndex, autoClear, &rxScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxScStates.inPktsSCLate:        \t%d\n", rxScStates.inPktsSCLate));
		MSG_PRINT(("rxScStates.inPktsSCNotValid:    \t%d\n", rxScStates.inPktsSCNotValid));
		MSG_PRINT(("rxScStates.inPktsSCInvalid:     \t%d\n", rxScStates.inPktsSCInvalid));
		MSG_PRINT(("rxScStates.inPktsSCDelayed:     \t%d\n", rxScStates.inPktsSCDelayed));
		MSG_PRINT(("rxScStates.inPktsSCUnchecked:   \t%d\n", rxScStates.inPktsSCUnchecked));
		MSG_PRINT(("rxScStates.inPktsSCOK:          \t%d\n", rxScStates.inPktsSCOK));
	}
	msdMemSet(&rxSecYStates, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
	if ((status = msdMacsecGetRXSecYStates(devNum, secYCounterIndex, autoClear, &rxSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxSecYStates.inOctetsSecYControlledPort:\t%d\n", rxSecYStates.inOctetsSecYControlledPort));
		MSG_PRINT(("rxSecYStates.inOctetsSecYDecrypted:     \t%d\n", rxSecYStates.inOctetsSecYDecrypted));
		MSG_PRINT(("rxSecYStates.inOctetsSecYValidated:     \t%d\n", rxSecYStates.inOctetsSecYValidated));
		MSG_PRINT(("rxSecYStates.inPktsSecYBadTag:          \t%d\n", rxSecYStates.inPktsSecYBadTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYBroadCast:       \t%d\n", rxSecYStates.inPktsSecYBroadCast));
		MSG_PRINT(("rxSecYStates.inPktsSecYMulticast        \t%d\n", rxSecYStates.inPktsSecYMulticast));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSA:            \t%d\n", rxSecYStates.inPktsSecYNoSA));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSAErr:         \t%d\n", rxSecYStates.inPktsSecYNoSAErr));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoTag:           \t%d\n", rxSecYStates.inPktsSecYNoTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnicast:         \t%d\n", rxSecYStates.inPktsSecYUnicast));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnTagged:        \t%d\n", rxSecYStates.inPktsSecYUnTagged));
	}

	msdMemSet(&txScStates, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
	if ((status = msdMacsecGetTXSCStates(devNum, scCounterIndex, autoClear, &txScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txScStates.outPktsEncrypted:    \t%d\n", txScStates.outPktsEncrypted));
		MSG_PRINT(("txScStates.outPktsProtected:    \t%d\n", txScStates.outPktsProtected));
	}

	msdMemSet(&txSecYStates, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
	if ((status = msdMacsecGetTXSecYStates(devNum, secYCounterIndex, autoClear, &txSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTxSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txSecYStates.outOctetsSecYControlledPort:\t%d\n", txSecYStates.outOctetsSecYControlledPort));
		MSG_PRINT(("txSecYStates.outOctetsSecYEncrypted:     \t%d\n", txSecYStates.outOctetsSecYEncrypted));
		MSG_PRINT(("txSecYStates.outOctetsSecYProtected:     \t%d\n", txSecYStates.outOctetsSecYProtected));
		MSG_PRINT(("txSecYStates.outPktSecYBroadcast:	     \t%d\n", txSecYStates.outPktSecYBroadcast));
		MSG_PRINT(("txSecYStates.outPktsSecYMulticast:       \t%d\n", txSecYStates.outPktsSecYMulticast));
		MSG_PRINT(("txSecYStates.outPktsSecYTooLong):        \t%d\n", txSecYStates.outPktsSecYTooLong));
		MSG_PRINT(("txSecYStates.outPktsSecYUnicast:         \t%d\n", txSecYStates.outPktsSecYUnicast));
		MSG_PRINT(("txSecYStates.outPktsSecYuntagged:        \t%d\n", txSecYStates.outPktsSecYuntagged));
	}
	return status;
}
MSD_STATUS sample_msdMACSecInterruptStatus(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_U8 txPort = 1;
	MSD_BOOL isEnable = 0;
	MSD_MACSEC_INT_CONTROL_TXSC txscIntCtrl;
	MSD_U8  ruleIndex = 0;
	MSD_U8  scIndex = 0;
	MSD_U8  secYIndex = 0;
	MSD_CIPHER_SUITE cipherSuite = MSD_AES_CTR_128;
	MSD_U64  nextPN = 0;
	MSD_U64  sci = 0;
	MSD_U8   secYCounterIndex = 0;
	MSD_U8   scCounterIndex = 0;
	MSD_BOOL allowZeroRxPN = 0;
	MSD_BOOL replayProtect = 0;
	MSD_32  replayWindow = 0;
	MSD_VALIDATE_FRAME validateFrame = MSD_NULL;
	MSD_VALIDATE_FRAME validFrame = MSD_NULL;

	MSD_AUTO_CLEAR autoClear = MSD_ENABLE;
	MSD_U8 nextIndex = 0;
	MSD_MACSEC_INT_TXSC txscInt;
	MSD_MSEC_RX_SC_COUNTER rxScStates;
	MSD_MSEC_RX_SECY_COUNTER rxSecYStates;
	MSD_MSEC_TX_SC_COUNTER txScStates;
	MSD_MSEC_TX_SECY_COUNTER txSecYStates;

	MSD_QD_DEV* dev = sohoDevGet(devNum);

	msdMemSet(&rxScStates, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
	msdMemSet(&rxSecYStates, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
	msdMemSet(&txScStates, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
	msdMemSet(&txSecYStates, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
	msdMemSet(&txscIntCtrl, 0, sizeof(MSD_MACSEC_INT_CONTROL_TXSC));
	msdMemSet(&txscInt, 0, sizeof(MSD_MACSEC_INT_TXSC));

	MSG_PRINT(("msdMacsecInterruptStatus debugging.\n"));
	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacsecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecFlushAll fail.\n"));
		return status;
	}

	/*Step 1, Macsec Enable, set interrupt*/
	isEnable = 1;
	if ((status = msdMacsecEnableIngressSet(devNum, txPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecEnableSet fail.\n"));
		return status;
	}
	scIndex = 1;
	txscIntCtrl.txAutoRekeyIntEn = 1;
	txscIntCtrl.txPNThrIntEn = 1;
	txscIntCtrl.txSAInvalidIntEn = 1;
	if ((status = msdMacsecSetTxSCIntCtrl(devNum, scIndex, &txscIntCtrl)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsec SetInterrupt fail.\n"));
		return status;
	}

	/*Step 2, Configure Ingress*/
	ruleIndex = 1;
	secYIndex = 1;
	validateFrame = MSD_STRICT;
	nextPN = 1;
	sci = 0xFFFFFFFFFFFF;
	replayWindow = 0xFFFFFFFE;
	if ((status = msdMacsecConfigureIngress(devNum, ruleIndex, scIndex, secYIndex, cipherSuite, validateFrame, nextPN, sci, secYCounterIndex, scCounterIndex, allowZeroRxPN, replayProtect, replayWindow)) != MSD_OK)
	{
		MSG_PRINT(("msdConfigureIngress fail.\n"));
		return status;
	}

	/*Step 3, Send traffic mibs*/
	/*Transmit pkts here, using Pktgen.*/
	/*Step 4, Stop traffic*/

	/*Step 5, Check States*/

	if ((status = msdMacsecGetNextTxSCIntStatus(devNum, scIndex, autoClear, &(nextIndex), &(txscInt))) != MSD_OK)
	{
		MSG_PRINT(("msdGetNextTxSCIntStatus fail.\n"));
		return status;
	}

	if ((status = msdMacsecGetRXSCStates(devNum,  scCounterIndex, autoClear, &rxScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxScStates.inPktsSCLate:        \t%d\n", rxScStates.inPktsSCLate));
		MSG_PRINT(("rxScStates.inPktsSCNotValid:    \t%d\n", rxScStates.inPktsSCNotValid));
		MSG_PRINT(("rxScStates.inPktsSCInvalid:     \t%d\n", rxScStates.inPktsSCInvalid));
		MSG_PRINT(("rxScStates.inPktsSCDelayed:     \t%d\n", rxScStates.inPktsSCDelayed));
		MSG_PRINT(("rxScStates.inPktsSCUnchecked:   \t%d\n", rxScStates.inPktsSCUnchecked));
		MSG_PRINT(("rxScStates.inPktsSCOK:          \t%d\n", rxScStates.inPktsSCOK));
	}
	if ((status = msdMacsecGetRXSecYStates(devNum, secYCounterIndex, autoClear, &rxSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxSecYStates.inOctetsSecYControlledPort:\t%d\n", rxSecYStates.inOctetsSecYControlledPort));
		MSG_PRINT(("rxSecYStates.inOctetsSecYDecrypted:     \t%d\n", rxSecYStates.inOctetsSecYDecrypted));
		MSG_PRINT(("rxSecYStates.inOctetsSecYValidated:     \t%d\n", rxSecYStates.inOctetsSecYValidated));
		MSG_PRINT(("rxSecYStates.inPktsSecYBadTag:          \t%d\n", rxSecYStates.inPktsSecYBadTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYBroadCast:       \t%d\n", rxSecYStates.inPktsSecYBroadCast));
		MSG_PRINT(("rxSecYStates.inPktsSecYMulticast        \t%d\n", rxSecYStates.inPktsSecYMulticast));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSA:            \t%d\n", rxSecYStates.inPktsSecYNoSA));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSAErr:         \t%d\n", rxSecYStates.inPktsSecYNoSAErr));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoTag:           \t%d\n", rxSecYStates.inPktsSecYNoTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnicast:         \t%d\n", rxSecYStates.inPktsSecYUnicast));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnTagged:        \t%d\n", rxSecYStates.inPktsSecYUnTagged));
	}

	if ((status = msdMacsecGetTXSCStates(devNum, scCounterIndex, autoClear, &txScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txScStates.outPktsEncrypted:    \t%d\n", txScStates.outPktsEncrypted));
		MSG_PRINT(("txScStates.outPktsProtected:    \t%d\n", txScStates.outPktsProtected));
	}

	if ((status = msdMacsecGetTXSecYStates(devNum, secYCounterIndex, autoClear, &txSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTxSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txSecYStates.outOctetsSecYControlledPort:\t%d\n", txSecYStates.outOctetsSecYControlledPort));
		MSG_PRINT(("txSecYStates.outOctetsSecYEncrypted:     \t%d\n", txSecYStates.outOctetsSecYEncrypted));
		MSG_PRINT(("txSecYStates.outOctetsSecYProtected:     \t%d\n", txSecYStates.outOctetsSecYProtected));
		MSG_PRINT(("txSecYStates.outPktSecYBroadcast:	     \t%d\n", txSecYStates.outPktSecYBroadcast));
		MSG_PRINT(("txSecYStates.outPktsSecYMulticast:       \t%d\n", txSecYStates.outPktsSecYMulticast));
		MSG_PRINT(("txSecYStates.outPktsSecYTooLong):        \t%d\n", txSecYStates.outPktsSecYTooLong));
		MSG_PRINT(("txSecYStates.outPktsSecYUnicast:         \t%d\n", txSecYStates.outPktsSecYUnicast));
		MSG_PRINT(("txSecYStates.outPktsSecYuntagged:        \t%d\n", txSecYStates.outPktsSecYuntagged));
	}

	return status;
}
MSD_STATUS sample_msdMACSecAutoRekey(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_FAIL;
	MSD_U8 txPort = 1;
	MSD_BOOL isEnable = 0;
	MSD_U8  ruleIndex = 0;
	MSD_U8  scIndex = 0;
	MSD_U8  secYIndex = 0;
	MSD_CIPHER_SUITE cipherSuite = MSD_AES_CTR_128;
	MSD_BOOL isAutoRekey = 0;
	MSD_U8   tci = 0;
	MSD_U64  nextPN = 0;
	MSD_U64  thresholdPN = 0;
	MSD_BOOL isActiveSA1 = 0;
	MSD_U64  sci = 0;
	MSD_U8   secYCounterIndex = 0;
	MSD_U8   scCounterIndex = 0;

	MSD_AUTO_CLEAR autoClear = MSD_ENABLE;
	MSD_MSEC_RX_SC_COUNTER rxScStates;
	MSD_MSEC_RX_SECY_COUNTER rxSecYStates;
	MSD_MSEC_TX_SC_COUNTER txScStates;
	MSD_MSEC_TX_SECY_COUNTER txSecYStates;

	MSD_QD_DEV* dev = sohoDevGet(devNum);

	msdMemSet(&rxScStates, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
	msdMemSet(&rxSecYStates, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
	msdMemSet(&txScStates, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
	msdMemSet(&txSecYStates, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));

	MSG_PRINT(("msdMacsecAutoRekey debugging.\n"));

	if (dev == NULL)
	{
		MSG_PRINT(("Failed. Dev is Null.\n"));
		return MSD_FAIL;
	}
	if ((status = msdMacsecFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecFlushAll fail.\n"));
		return status;
	}

	/*Step 1, Macsec Enable*/
	isEnable = 1;
	if ((status = msdMacsecEnableIngressSet(devNum, txPort, isEnable)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecEnableSet fail.\n"));
		return status;
	}


	/*Step 2, Configure Egress*/
	ruleIndex = 1;
	scIndex = 1;
	secYIndex = 1;
	isAutoRekey = 1;
	tci = 0xb;
	nextPN = 1;
	isActiveSA1 = 1;
	sci = 0xFFFFFFFFFFFF;
	thresholdPN = 0x100;
	if ((status = msdMacsecConfigureEgress(devNum, ruleIndex, scIndex, secYIndex, cipherSuite, isAutoRekey, tci, nextPN, thresholdPN, isActiveSA1, sci, secYCounterIndex, scCounterIndex)) != MSD_OK)
	{
		MSG_PRINT(("msdConfigureEngress fail.\n"));
		return status;
	}

	/*Step 3, Send traffic mibs*/
	/*Transmit pkts here, using Pktgen.*/
	/*Step 4, Stop traffic*/

	/*Step 5, Check States*/

	if ((status = msdMacsecGetRXSCStates(devNum, scCounterIndex, autoClear, &rxScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxScStates.inPktsSCLate:        \t%d\n", rxScStates.inPktsSCLate));
		MSG_PRINT(("rxScStates.inPktsSCNotValid:    \t%d\n", rxScStates.inPktsSCNotValid));
		MSG_PRINT(("rxScStates.inPktsSCInvalid:     \t%d\n", rxScStates.inPktsSCInvalid));
		MSG_PRINT(("rxScStates.inPktsSCDelayed:     \t%d\n", rxScStates.inPktsSCDelayed));
		MSG_PRINT(("rxScStates.inPktsSCUnchecked:   \t%d\n", rxScStates.inPktsSCUnchecked));
		MSG_PRINT(("rxScStates.inPktsSCOK:          \t%d\n", rxScStates.inPktsSCOK));
	}
	if ((status = msdMacsecGetRXSecYStates(devNum, secYCounterIndex, autoClear, &rxSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetRXSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("rxSecYStates.inOctetsSecYControlledPort:\t%d\n", rxSecYStates.inOctetsSecYControlledPort));
		MSG_PRINT(("rxSecYStates.inOctetsSecYDecrypted:     \t%d\n", rxSecYStates.inOctetsSecYDecrypted));
		MSG_PRINT(("rxSecYStates.inOctetsSecYValidated:     \t%d\n", rxSecYStates.inOctetsSecYValidated));
		MSG_PRINT(("rxSecYStates.inPktsSecYBadTag:          \t%d\n", rxSecYStates.inPktsSecYBadTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYBroadCast:       \t%d\n", rxSecYStates.inPktsSecYBroadCast));
		MSG_PRINT(("rxSecYStates.inPktsSecYMulticast        \t%d\n", rxSecYStates.inPktsSecYMulticast));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSA:            \t%d\n", rxSecYStates.inPktsSecYNoSA));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoSAErr:         \t%d\n", rxSecYStates.inPktsSecYNoSAErr));
		MSG_PRINT(("rxSecYStates.inPktsSecYNoTag:           \t%d\n", rxSecYStates.inPktsSecYNoTag));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnicast:         \t%d\n", rxSecYStates.inPktsSecYUnicast));
		MSG_PRINT(("rxSecYStates.inPktsSecYUnTagged:        \t%d\n", rxSecYStates.inPktsSecYUnTagged));
	}

	if ((status = msdMacsecGetTXSCStates(devNum, scCounterIndex, autoClear, &txScStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTXSCStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txScStates.outPktsEncrypted:    \t%d\n", txScStates.outPktsEncrypted));
		MSG_PRINT(("txScStates.outPktsProtected:    \t%d\n", txScStates.outPktsProtected));
	}

	if ((status = msdMacsecGetTXSecYStates(devNum, secYCounterIndex, autoClear, &txSecYStates)) != MSD_OK)
	{
		MSG_PRINT(("msdMacsecGetTxSecYStates fail.\n"));
		return status;
	}
	else
	{
		MSG_PRINT(("txSecYStates.outOctetsSecYControlledPort:\t%d\n", txSecYStates.outOctetsSecYControlledPort));
		MSG_PRINT(("txSecYStates.outOctetsSecYEncrypted:     \t%d\n", txSecYStates.outOctetsSecYEncrypted));
		MSG_PRINT(("txSecYStates.outOctetsSecYProtected:     \t%d\n", txSecYStates.outOctetsSecYProtected));
		MSG_PRINT(("txSecYStates.outPktSecYBroadcast:	     \t%d\n", txSecYStates.outPktSecYBroadcast));
		MSG_PRINT(("txSecYStates.outPktsSecYMulticast:       \t%d\n", txSecYStates.outPktsSecYMulticast));
		MSG_PRINT(("txSecYStates.outPktsSecYTooLong):        \t%d\n", txSecYStates.outPktsSecYTooLong));
		MSG_PRINT(("txSecYStates.outPktsSecYUnicast:         \t%d\n", txSecYStates.outPktsSecYUnicast));
		MSG_PRINT(("txSecYStates.outPktsSecYuntagged:        \t%d\n", txSecYStates.outPktsSecYuntagged));
	}

	return status;
}


