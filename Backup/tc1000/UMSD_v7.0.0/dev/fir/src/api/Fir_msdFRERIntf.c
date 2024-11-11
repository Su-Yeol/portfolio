/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Fir_msdCBIntf.c
*
* DESCRIPTION:
*       API definitions for FRER
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <fir/include/api/Fir_msdFRER.h>

MSD_STATUS Fir_gfrerSeqGenLoadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_FRER_SEQGEN	*seqGenData
)
{
	FIR_MSD_FRER_SEQGEN    seqGen;
	if (NULL == seqGenData)
	{
		MSD_DBG_ERROR(("Input param MSD_FRER_SEQGEN in Fir_gfrerSeqGenLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&seqGen, 0, sizeof(FIR_MSD_FRER_SEQGEN));
	seqGen.seqGenId = seqGenData->seqGenId;
	seqGen.genSeqNum = seqGenData->genSeqNum;

	return Fir_gfrerSeqGenLoad(dev, &seqGen);
}

MSD_STATUS Fir_gfrerSeqGenReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId,
OUT MSD_FRER_SEQGEN	*seqGenData
)
{
	FIR_MSD_FRER_SEQGEN    seqGen;
	MSD_STATUS           retVal;

	retVal = Fir_gfrerSeqGenRead(dev, seqGenId, &seqGen);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)seqGenData, 0, sizeof(MSD_FRER_SEQGEN));
	seqGenData->seqGenId = seqGen.seqGenId;
	seqGenData->genSeqNum = seqGen.genSeqNum;

	return retVal;
}

MSD_STATUS Fir_gfrerIndvRcvyLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_INDVRCVY	*indvRcvyData
)
{
	FIR_MSD_FRER_INDVRCVY    indvRcvy;
	if (NULL == indvRcvyData)
	{
		MSD_DBG_ERROR(("Input param MSD_FRER_INDVRCVY in Fir_gfrerIndvRcvyLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&indvRcvy, 0, sizeof(FIR_MSD_FRER_INDVRCVY));
	indvRcvy.indvRcvyId = indvRcvyData->indvRcvyId;
	indvRcvy.indvRcvyEn = indvRcvyData->indvRcvyEn;
	indvRcvy.indvRcvyAlg = indvRcvyData->indvRcvyAlg;
	indvRcvy.indvRcvyTakeNoSeq = indvRcvyData->indvRcvyTakeNoSeq;
	indvRcvy.indvRcvySeqHisLen = indvRcvyData->indvRcvySeqHisLen;
	indvRcvy.indvRcvyRstRate = indvRcvyData->indvRcvyRstRate;
	indvRcvy.indvRcvyRstTime = indvRcvyData->indvRcvyRstTime;

	return Fir_gfrerIndvRcvyLoad(dev, &indvRcvy);
}

MSD_STATUS Fir_gfrerIndvRcvyReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16*	indvRcvyResets
)
{
	FIR_MSD_FRER_INDVRCVY	indvRcvy;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerIndvRcvyRead(dev, indvRcvyId, &indvRcvy, indvRcvyResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)indvRcvyData, 0, sizeof(MSD_FRER_INDVRCVY));
	indvRcvyData->indvRcvyId = indvRcvy.indvRcvyId;
	indvRcvyData->indvRcvyEn = indvRcvy.indvRcvyEn;
	indvRcvyData->indvRcvyAlg = indvRcvy.indvRcvyAlg;
	indvRcvyData->indvRcvyTakeNoSeq = indvRcvy.indvRcvyTakeNoSeq;
	indvRcvyData->indvRcvySeqHisLen = indvRcvy.indvRcvySeqHisLen;
	indvRcvyData->indvRcvyRstRate = indvRcvy.indvRcvyRstRate;
	indvRcvyData->indvRcvyRstTime = indvRcvy.indvRcvyRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerIndvRcvyGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16*	indvRcvyResets
)
{
	FIR_MSD_FRER_INDVRCVY	indvRcvy;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerIndvRcvyGetNext(dev, indvRcvyId, &indvRcvy, indvRcvyResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)indvRcvyData, 0, sizeof(MSD_FRER_INDVRCVY));
	indvRcvyData->indvRcvyId = indvRcvy.indvRcvyId;
	indvRcvyData->indvRcvyEn = indvRcvy.indvRcvyEn;
	indvRcvyData->indvRcvyAlg = indvRcvy.indvRcvyAlg;
	indvRcvyData->indvRcvyTakeNoSeq = indvRcvy.indvRcvyTakeNoSeq;
	indvRcvyData->indvRcvySeqHisLen = indvRcvy.indvRcvySeqHisLen;
	indvRcvyData->indvRcvyRstRate = indvRcvy.indvRcvyRstRate;
	indvRcvyData->indvRcvyRstTime = indvRcvy.indvRcvyRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerIndvRcvyReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16*	indvRcvyResets
)
{
	FIR_MSD_FRER_INDVRCVY	indvRcvy;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerIndvRcvyReadClear(dev, indvRcvyId, &indvRcvy, indvRcvyResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)indvRcvyData, 0, sizeof(MSD_FRER_INDVRCVY));
	indvRcvyData->indvRcvyId = indvRcvy.indvRcvyId;
	indvRcvyData->indvRcvyEn = indvRcvy.indvRcvyEn;
	indvRcvyData->indvRcvyAlg = indvRcvy.indvRcvyAlg;
	indvRcvyData->indvRcvyTakeNoSeq = indvRcvy.indvRcvyTakeNoSeq;
	indvRcvyData->indvRcvySeqHisLen = indvRcvy.indvRcvySeqHisLen;
	indvRcvyData->indvRcvyRstRate = indvRcvy.indvRcvyRstRate;
	indvRcvyData->indvRcvyRstTime = indvRcvy.indvRcvyRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerSeqRcvyLoadIntf
(
IN  MSD_QD_DEV				*dev,
IN  MSD_FRER_SEQRCVY       *seqRcvyData
)
{
	FIR_MSD_FRER_SEQRCVY    seqRcvy;
	if (NULL == seqRcvyData)
	{
		MSD_DBG_ERROR(("Input param MSD_FRER_SEQRCVY in Fir_gfrerSeqRcvyLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&seqRcvy, 0, sizeof(FIR_MSD_FRER_SEQRCVY));
	seqRcvy.seqRcvyId= seqRcvyData->seqRcvyId;
	seqRcvy.seqRcvyEn = seqRcvyData->seqRcvyEn;
	seqRcvy.seqRcvyPort = seqRcvyData->seqRcvyPort;
	seqRcvy.seqRcvyIndex = seqRcvyData->seqRcvyIndex;
	seqRcvy.seqRcvyAlg = seqRcvyData->seqRcvyAlg;
	seqRcvy.seqRcvyTakeNoSeq = seqRcvyData->seqRcvyTakeNoSeq;
	seqRcvy.latErrIntEn = seqRcvyData->latErrIntEn;
	seqRcvy.latErrStatus = seqRcvyData->latErrStatus;
	seqRcvy.seqRcvySeqHisLen = seqRcvyData->seqRcvySeqHisLen;
	seqRcvy.seqRcvyRstRate = seqRcvyData->seqRcvyRstRate;
	seqRcvy.seqRcvyRstTime = seqRcvyData->seqRcvyRstTime;
	seqRcvy.latErrEn = seqRcvyData->latErrEn;
	seqRcvy.rdantPaths = seqRcvyData->rdantPaths;
	seqRcvy.latErrDiff = seqRcvyData->latErrDiff;
	seqRcvy.latErrTestRate = seqRcvyData->latErrTestRate;
	seqRcvy.latErrTestTime = seqRcvyData->latErrTestTime;
	seqRcvy.latErrRstRate = seqRcvyData->latErrRstRate;
	seqRcvy.latErrRstTime = seqRcvyData->latErrRstTime;

	return Fir_gfrerSeqRcvyLoad(dev, &seqRcvy);
}

MSD_STATUS Fir_gfrerSeqRcvyReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16*	seqRcvyResets,
OUT MSD_U16*	latErrResets
)
{
	FIR_MSD_FRER_SEQRCVY     seqRcvy;
	MSD_STATUS           retVal;

	retVal = Fir_gfrerSeqRcvyRead(dev, seqRcvyId, &seqRcvy, seqRcvyResets, latErrResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)seqRcvyData, 0, sizeof(MSD_FRER_SEQRCVY));
	seqRcvyData->seqRcvyId = seqRcvy.seqRcvyId;
	seqRcvyData->seqRcvyEn = seqRcvy.seqRcvyEn;
	seqRcvyData->seqRcvyPort = seqRcvy.seqRcvyPort;
	seqRcvyData->seqRcvyIndex = seqRcvy.seqRcvyIndex;
	seqRcvyData->seqRcvyAlg = seqRcvy.seqRcvyAlg;
	seqRcvyData->seqRcvyTakeNoSeq = seqRcvy.seqRcvyTakeNoSeq;
	seqRcvyData->latErrIntEn = seqRcvy.latErrIntEn;
	seqRcvyData->latErrStatus = seqRcvy.latErrStatus;
	seqRcvyData->seqRcvySeqHisLen = seqRcvy.seqRcvySeqHisLen;
	seqRcvyData->seqRcvyRstRate = seqRcvy.seqRcvyRstRate;
	seqRcvyData->seqRcvyRstTime = seqRcvy.seqRcvyRstTime;
	seqRcvyData->latErrEn = seqRcvy.latErrEn;
	seqRcvyData->rdantPaths = seqRcvy.rdantPaths;
	seqRcvyData->latErrDiff = seqRcvy.latErrDiff;
	seqRcvyData->latErrTestRate = seqRcvy.latErrTestRate;
	seqRcvyData->latErrTestTime = seqRcvy.latErrTestTime;
	seqRcvyData->latErrRstRate = seqRcvy.latErrRstRate;
	seqRcvyData->latErrRstTime = seqRcvy.latErrRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerSeqRcvyGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16*	seqRcvyResets,
OUT MSD_U16*	latErrResets
)
{
	FIR_MSD_FRER_SEQRCVY     seqRcvy;
	MSD_STATUS           retVal;

	retVal = Fir_gfrerSeqRcvyGetNext(dev, seqRcvyId, &seqRcvy, seqRcvyResets, latErrResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)seqRcvyData, 0, sizeof(MSD_FRER_SEQRCVY));
	seqRcvyData->seqRcvyId = seqRcvy.seqRcvyId;
	seqRcvyData->seqRcvyEn = seqRcvy.seqRcvyEn;
	seqRcvyData->seqRcvyPort = seqRcvy.seqRcvyPort;
	seqRcvyData->seqRcvyIndex = seqRcvy.seqRcvyIndex;
	seqRcvyData->seqRcvyAlg = seqRcvy.seqRcvyAlg;
	seqRcvyData->seqRcvyTakeNoSeq = seqRcvy.seqRcvyTakeNoSeq;
	seqRcvyData->latErrIntEn = seqRcvy.latErrIntEn;
	seqRcvyData->latErrStatus = seqRcvy.latErrStatus;
	seqRcvyData->seqRcvySeqHisLen = seqRcvy.seqRcvySeqHisLen;
	seqRcvyData->seqRcvyRstRate = seqRcvy.seqRcvyRstRate;
	seqRcvyData->seqRcvyRstTime = seqRcvy.seqRcvyRstTime;
	seqRcvyData->latErrEn = seqRcvy.latErrEn;
	seqRcvyData->rdantPaths = seqRcvy.rdantPaths;
	seqRcvyData->latErrDiff = seqRcvy.latErrDiff;
	seqRcvyData->latErrTestRate = seqRcvy.latErrTestRate;
	seqRcvyData->latErrTestTime = seqRcvy.latErrTestTime;
	seqRcvyData->latErrRstRate = seqRcvy.latErrRstRate;
	seqRcvyData->latErrRstTime = seqRcvy.latErrRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerSeqRcvyGetViolationIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16*	seqRcvyResets,
OUT MSD_U16*	latErrResets
)
{
	FIR_MSD_FRER_SEQRCVY     seqRcvy;
	MSD_STATUS           retVal;

	retVal = Fir_gfrerSeqRcvyGetViolation(dev, seqRcvyId, &seqRcvy, seqRcvyResets, latErrResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)seqRcvyData, 0, sizeof(MSD_FRER_SEQRCVY));
	seqRcvyData->seqRcvyId = seqRcvy.seqRcvyId;
	seqRcvyData->seqRcvyEn = seqRcvy.seqRcvyEn;
	seqRcvyData->seqRcvyPort = seqRcvy.seqRcvyPort;
	seqRcvyData->seqRcvyIndex = seqRcvy.seqRcvyIndex;
	seqRcvyData->seqRcvyAlg = seqRcvy.seqRcvyAlg;
	seqRcvyData->seqRcvyTakeNoSeq = seqRcvy.seqRcvyTakeNoSeq;
	seqRcvyData->latErrIntEn = seqRcvy.latErrIntEn;
	seqRcvyData->latErrStatus = seqRcvy.latErrStatus;
	seqRcvyData->seqRcvySeqHisLen = seqRcvy.seqRcvySeqHisLen;
	seqRcvyData->seqRcvyRstRate = seqRcvy.seqRcvyRstRate;
	seqRcvyData->seqRcvyRstTime = seqRcvy.seqRcvyRstTime;
	seqRcvyData->latErrEn = seqRcvy.latErrEn;
	seqRcvyData->rdantPaths = seqRcvy.rdantPaths;
	seqRcvyData->latErrDiff = seqRcvy.latErrDiff;
	seqRcvyData->latErrTestRate = seqRcvy.latErrTestRate;
	seqRcvyData->latErrTestTime = seqRcvy.latErrTestTime;
	seqRcvyData->latErrRstRate = seqRcvy.latErrRstRate;
	seqRcvyData->latErrRstTime = seqRcvy.latErrRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerSeqRcvyReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16*	seqRcvyResets,
OUT MSD_U16*	latErrResets
)
{
	FIR_MSD_FRER_SEQRCVY     seqRcvy;
	MSD_STATUS           retVal;

	retVal = Fir_gfrerSeqRcvyReadClear(dev, seqRcvyId, &seqRcvy, seqRcvyResets, latErrResets);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)seqRcvyData, 0, sizeof(MSD_FRER_SEQRCVY));
	seqRcvyData->seqRcvyId = seqRcvy.seqRcvyId;
	seqRcvyData->seqRcvyEn = seqRcvy.seqRcvyEn;
	seqRcvyData->seqRcvyPort = seqRcvy.seqRcvyPort;
	seqRcvyData->seqRcvyIndex = seqRcvy.seqRcvyIndex;
	seqRcvyData->seqRcvyAlg = seqRcvy.seqRcvyAlg;
	seqRcvyData->seqRcvyTakeNoSeq = seqRcvy.seqRcvyTakeNoSeq;
	seqRcvyData->latErrIntEn = seqRcvy.latErrIntEn;
	seqRcvyData->latErrStatus = seqRcvy.latErrStatus;
	seqRcvyData->seqRcvySeqHisLen = seqRcvy.seqRcvySeqHisLen;
	seqRcvyData->seqRcvyRstRate = seqRcvy.seqRcvyRstRate;
	seqRcvyData->seqRcvyRstTime = seqRcvy.seqRcvyRstTime;
	seqRcvyData->latErrEn = seqRcvy.latErrEn;
	seqRcvyData->rdantPaths = seqRcvy.rdantPaths;
	seqRcvyData->latErrDiff = seqRcvy.latErrDiff;
	seqRcvyData->latErrTestRate = seqRcvy.latErrTestRate;
	seqRcvyData->latErrTestTime = seqRcvy.latErrTestTime;
	seqRcvyData->latErrRstRate = seqRcvy.latErrRstRate;
	seqRcvyData->latErrRstTime = seqRcvy.latErrRstTime;

	return retVal;

}

MSD_STATUS Fir_gfrerBank0SCtrReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG*	bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER*	bk0SCtrData
)
{
	FIR_MSD_FRER_BANK0_CONFIG	bk0Config;
	FIR_MSD_FRER_BANK0_COUNTER	bk0Ctr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank0SCtrRead(dev, bk0Id, &bk0Config, &bk0Ctr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)bk0ConfigData, 0, sizeof(MSD_FRER_BANK0_CONFIG));
	bk0ConfigData->bk0SCtrId = bk0Config.bk0SCtrId;
	bk0ConfigData->bk0SCtrEn = bk0Config.bk0SCtrEn;
	bk0ConfigData->bk0SCtrPort = bk0Config.bk0SCtrPort;
	bk0ConfigData->bk0SCtrIndex = bk0Config.bk0SCtrIndex;

	msdMemSet((void*)bk0SCtrData, 0, sizeof(MSD_FRER_BANK0_COUNTER));
	bk0SCtrData->indvRcvyTagless = bk0Ctr.indvRcvyTagless;
	bk0SCtrData->indvRcvyPassed = bk0Ctr.indvRcvyPassed;
	bk0SCtrData->indvRcvyDiscarded = bk0Ctr.indvRcvyDiscarded;
	bk0SCtrData->indvRcvyOutofOrder = bk0Ctr.indvRcvyOutofOrder;
	bk0SCtrData->indvRcvyRogue = bk0Ctr.indvRcvyRogue;
	bk0SCtrData->indvRcvyLost = bk0Ctr.indvRcvyLost;
	bk0SCtrData->sidInput = bk0Ctr.sidInput;
	bk0SCtrData->seqEncErrored = bk0Ctr.seqEncErrored;

	return retVal;

}

MSD_STATUS Fir_gfrerBank0SCtrLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_BANK0_CONFIG	*bk0ConfigData
)
{
	FIR_MSD_FRER_BANK0_CONFIG    bk0Config;
	if (NULL == bk0ConfigData)
	{
		MSD_DBG_ERROR(("Input param MSD_FRER_BANK0 in Fir_gfrerBank0SCtrLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&bk0Config, 0, sizeof(FIR_MSD_FRER_BANK0_CONFIG));
	bk0Config.bk0SCtrId = bk0ConfigData->bk0SCtrId;
	bk0Config.bk0SCtrEn = bk0ConfigData->bk0SCtrEn;
	bk0Config.bk0SCtrPort = bk0ConfigData->bk0SCtrPort;
	bk0Config.bk0SCtrIndex = bk0ConfigData->bk0SCtrIndex;

	return Fir_gfrerBank0SCtrLoad(dev, &bk0Config);
}

MSD_STATUS Fir_gfrerBank0SCtrGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG*	bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER*	bk0SCtrData
)
{
	FIR_MSD_FRER_BANK0_CONFIG	bk0Config;
	FIR_MSD_FRER_BANK0_COUNTER	bk0Ctr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank0SCtrGetNext(dev, bk0Id, &bk0Config, &bk0Ctr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)bk0ConfigData, 0, sizeof(MSD_FRER_BANK0_CONFIG));
	bk0ConfigData->bk0SCtrId = bk0Config.bk0SCtrId;
	bk0ConfigData->bk0SCtrEn = bk0Config.bk0SCtrEn;
	bk0ConfigData->bk0SCtrPort = bk0Config.bk0SCtrPort;
	bk0ConfigData->bk0SCtrIndex = bk0Config.bk0SCtrIndex;

	msdMemSet((void*)bk0SCtrData, 0, sizeof(MSD_FRER_BANK0_COUNTER));
	bk0SCtrData->indvRcvyTagless = bk0Ctr.indvRcvyTagless;
	bk0SCtrData->indvRcvyPassed = bk0Ctr.indvRcvyPassed;
	bk0SCtrData->indvRcvyDiscarded = bk0Ctr.indvRcvyDiscarded;
	bk0SCtrData->indvRcvyOutofOrder = bk0Ctr.indvRcvyOutofOrder;
	bk0SCtrData->indvRcvyRogue = bk0Ctr.indvRcvyRogue;
	bk0SCtrData->indvRcvyLost = bk0Ctr.indvRcvyLost;
	bk0SCtrData->sidInput = bk0Ctr.sidInput;
	bk0SCtrData->seqEncErrored = bk0Ctr.seqEncErrored;

	return retVal;

}

MSD_STATUS Fir_gfrerBank0SCtrReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
	FIR_MSD_FRER_BANK0_CONFIG	bk0Config;
	FIR_MSD_FRER_BANK0_COUNTER	bk0Ctr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank0SCtrReadClear(dev, bk0Id, &bk0Config, &bk0Ctr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)bk0ConfigData, 0, sizeof(MSD_FRER_BANK0_CONFIG));
	bk0ConfigData->bk0SCtrId = bk0Config.bk0SCtrId;
	bk0ConfigData->bk0SCtrEn = bk0Config.bk0SCtrEn;
	bk0ConfigData->bk0SCtrPort = bk0Config.bk0SCtrPort;
	bk0ConfigData->bk0SCtrIndex = bk0Config.bk0SCtrIndex;

	msdMemSet((void*)bk0SCtrData, 0, sizeof(MSD_FRER_BANK0_COUNTER));
	bk0SCtrData->indvRcvyTagless = bk0Ctr.indvRcvyTagless;
	bk0SCtrData->indvRcvyPassed = bk0Ctr.indvRcvyPassed;
	bk0SCtrData->indvRcvyDiscarded = bk0Ctr.indvRcvyDiscarded;
	bk0SCtrData->indvRcvyOutofOrder = bk0Ctr.indvRcvyOutofOrder;
	bk0SCtrData->indvRcvyRogue = bk0Ctr.indvRcvyRogue;
	bk0SCtrData->indvRcvyLost = bk0Ctr.indvRcvyLost;
	bk0SCtrData->sidInput = bk0Ctr.sidInput;
	bk0SCtrData->seqEncErrored = bk0Ctr.seqEncErrored;

	return retVal;

}

MSD_STATUS Fir_gfrerBank1SCtrLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_BANK1_CONFIG	*bk1ConfigData
)
{
	FIR_MSD_FRER_BANK1_CONFIG    bk1Config;
	if (NULL == bk1ConfigData)
	{
		MSD_DBG_ERROR(("Input param MSD_FRER_BANK1 in Fir_gfrerBank1SCtrLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&bk1Config, 0, sizeof(FIR_MSD_FRER_BANK1_CONFIG));
	bk1Config.bk1SCtrId = bk1ConfigData->bk1SCtrId;
	bk1Config.bk1SCtrEn = bk1ConfigData->bk1SCtrEn;
	bk1Config.bk1SCtrPort = bk1ConfigData->bk1SCtrPort;
	bk1Config.bk1SCtrIndex = bk1ConfigData->bk1SCtrIndex;

	return Fir_gfrerBank1SCtrLoad(dev, &bk1Config);
}

MSD_STATUS Fir_gfrerBank1SCtrReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	FIR_MSD_FRER_BANK1_CONFIG	bk1Config;
	FIR_MSD_FRER_BANK1_COUNTER	bk1SCtr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank1SCtrRead(dev, bk1Id, &bk1Config, &bk1SCtr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}
	msdMemSet((void*)bk1ConfigData, 0, sizeof(MSD_FRER_BANK1_CONFIG));
	bk1ConfigData->bk1SCtrId = bk1Config.bk1SCtrId;
	bk1ConfigData->bk1SCtrEn = bk1Config.bk1SCtrEn;
	bk1ConfigData->bk1SCtrPort = bk1Config.bk1SCtrPort;
	bk1ConfigData->bk1SCtrIndex = bk1Config.bk1SCtrIndex;

	msdMemSet((void*)bk1SCtrData, 0, sizeof(MSD_FRER_BANK1_COUNTER));
	bk1SCtrData->seqRcvyTagless = bk1SCtr.seqRcvyTagless;
	bk1SCtrData->seqRcvyPassed = bk1SCtr.seqRcvyPassed;
	bk1SCtrData->seqRcvyDiscarded = bk1SCtr.seqRcvyDiscarded;
	bk1SCtrData->seqRcvyOutofOrder = bk1SCtr.seqRcvyOutofOrder;
	bk1SCtrData->seqRcvyRogue = bk1SCtr.seqRcvyRogue;
	bk1SCtrData->seqRcvyLost = bk1SCtr.seqRcvyLost;
	bk1SCtrData->sidOutput = bk1SCtr.sidOutput;

	return retVal;

}

MSD_STATUS Fir_gfrerBank1SCtrGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	FIR_MSD_FRER_BANK1_CONFIG	bk1Config;
	FIR_MSD_FRER_BANK1_COUNTER	bk1SCtr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank1SCtrGetNext(dev, bk1Id, &bk1Config, &bk1SCtr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}
	msdMemSet((void*)bk1ConfigData, 0, sizeof(MSD_FRER_BANK1_CONFIG));
	bk1ConfigData->bk1SCtrId = bk1Config.bk1SCtrId;
	bk1ConfigData->bk1SCtrEn = bk1Config.bk1SCtrEn;
	bk1ConfigData->bk1SCtrPort = bk1Config.bk1SCtrPort;
	bk1ConfigData->bk1SCtrIndex = bk1Config.bk1SCtrIndex;

	msdMemSet((void*)bk1SCtrData, 0, sizeof(MSD_FRER_BANK1_COUNTER));
	bk1SCtrData->seqRcvyTagless = bk1SCtr.seqRcvyTagless;
	bk1SCtrData->seqRcvyPassed = bk1SCtr.seqRcvyPassed;
	bk1SCtrData->seqRcvyDiscarded = bk1SCtr.seqRcvyDiscarded;
	bk1SCtrData->seqRcvyOutofOrder = bk1SCtr.seqRcvyOutofOrder;
	bk1SCtrData->seqRcvyRogue = bk1SCtr.seqRcvyRogue;
	bk1SCtrData->seqRcvyLost = bk1SCtr.seqRcvyLost;
	bk1SCtrData->sidOutput = bk1SCtr.sidOutput;

	return retVal;

}

MSD_STATUS Fir_gfrerBank1SCtrReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	FIR_MSD_FRER_BANK1_CONFIG	bk1Config;
	FIR_MSD_FRER_BANK1_COUNTER	bk1SCtr;
	MSD_STATUS	retVal;

	retVal = Fir_gfrerBank1SCtrReadClear(dev, bk1Id, &bk1Config, &bk1SCtr);

	if (MSD_OK != retVal)
	{
		return retVal;
	}
	msdMemSet((void*)bk1ConfigData, 0, sizeof(MSD_FRER_BANK1_CONFIG));
	bk1ConfigData->bk1SCtrId = bk1Config.bk1SCtrId;
	bk1ConfigData->bk1SCtrEn = bk1Config.bk1SCtrEn;
	bk1ConfigData->bk1SCtrPort = bk1Config.bk1SCtrPort;
	bk1ConfigData->bk1SCtrIndex = bk1Config.bk1SCtrIndex;

	msdMemSet((void*)bk1SCtrData, 0, sizeof(MSD_FRER_BANK1_COUNTER));
	bk1SCtrData->seqRcvyTagless = bk1SCtr.seqRcvyTagless;
	bk1SCtrData->seqRcvyPassed = bk1SCtr.seqRcvyPassed;
	bk1SCtrData->seqRcvyDiscarded = bk1SCtr.seqRcvyDiscarded;
	bk1SCtrData->seqRcvyOutofOrder = bk1SCtr.seqRcvyOutofOrder;
	bk1SCtrData->seqRcvyRogue = bk1SCtr.seqRcvyRogue;
	bk1SCtrData->seqRcvyLost = bk1SCtr.seqRcvyLost;
	bk1SCtrData->sidOutput = bk1SCtr.sidOutput;

	return retVal;

}
