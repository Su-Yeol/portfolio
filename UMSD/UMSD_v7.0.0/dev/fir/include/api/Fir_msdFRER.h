/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdCB_h
*
* DESCRIPTION:
*       API/Structure definitions for FRER (Frame Replication and Elimination for Reliability).
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#ifndef Fir_msdCB_h
#define Fir_msdCB_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	MSD_U8 seqGenId;
	MSD_U16 genSeqNum;

} FIR_MSD_FRER_SEQGEN;

typedef struct
{
	MSD_U8 indvRcvyId;
	MSD_U8 indvRcvyEn;
	MSD_U8 indvRcvyAlg;
	MSD_U8 indvRcvyTakeNoSeq;
	MSD_U8 indvRcvySeqHisLen;
	MSD_U8 indvRcvyRstRate;
	MSD_U16 indvRcvyRstTime;

} FIR_MSD_FRER_INDVRCVY;

typedef struct
{
	MSD_U8 seqRcvyId;
	MSD_U8 seqRcvyEn;
	MSD_U8 seqRcvyPort;
	MSD_U8 seqRcvyIndex;
	MSD_U8 seqRcvyAlg;
	MSD_U8 seqRcvyTakeNoSeq;
	MSD_U8 latErrIntEn;
	MSD_U8 latErrStatus;
	MSD_U8 seqRcvySeqHisLen;
	MSD_U8 seqRcvyRstRate;
	MSD_U16 seqRcvyRstTime;
	MSD_U8 latErrEn;
	MSD_U8 rdantPaths;
	MSD_U8 latErrDiff;
	MSD_U8 latErrTestRate;
	MSD_U16 latErrTestTime;
	MSD_U8 latErrRstRate;
	MSD_U16 latErrRstTime;

} FIR_MSD_FRER_SEQRCVY;

typedef struct
{
	MSD_U8 bk0SCtrId;
	MSD_U8 bk0SCtrEn;
	MSD_U8 bk0SCtrPort;
	MSD_U8 bk0SCtrIndex;

} FIR_MSD_FRER_BANK0_CONFIG;

typedef struct
{
	MSD_U32 indvRcvyTagless;
	MSD_U32 indvRcvyPassed;
	MSD_U32 indvRcvyDiscarded;
	MSD_U32 indvRcvyOutofOrder;
	MSD_U32 indvRcvyRogue;
	MSD_U32 indvRcvyLost;
	MSD_U32 sidInput;
	MSD_U32 seqEncErrored;

} FIR_MSD_FRER_BANK0_COUNTER;

typedef struct
{
	MSD_U8 bk1SCtrId;
	MSD_U8 bk1SCtrEn;
	MSD_U8 bk1SCtrPort;
	MSD_U8 bk1SCtrIndex;

} FIR_MSD_FRER_BANK1_CONFIG;

typedef struct
{
	MSD_U32 seqRcvyTagless;
	MSD_U32 seqRcvyPassed;
	MSD_U32 seqRcvyDiscarded;
	MSD_U32 seqRcvyOutofOrder;
	MSD_U32 seqRcvyRogue;
	MSD_U32 seqRcvyLost;
	MSD_U32 sidOutput;

} FIR_MSD_FRER_BANK1_COUNTER;

MSD_STATUS Fir_gfrerSeqGenFlushAll
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerSeqGenFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId
);

MSD_STATUS Fir_gfrerSeqGenRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId,
OUT FIR_MSD_FRER_SEQGEN	*seqGenData
);

MSD_STATUS Fir_gfrerSeqGenReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId,
OUT MSD_FRER_SEQGEN	*seqGenData
);

MSD_STATUS Fir_gfrerSeqGenLoad
(
IN  MSD_QD_DEV	*dev,
IN  FIR_MSD_FRER_SEQGEN	*seqGenData
);

MSD_STATUS Fir_gfrerSeqGenLoadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_FRER_SEQGEN	*seqGenData
);

MSD_STATUS Fir_gfrerIndvRcvyFlushAll
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerIndvRcvyFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId
);

MSD_STATUS Fir_gfrerIndvRcvyRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerIndvRcvyReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerIndvRcvyLoad
(
IN MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_INDVRCVY	*indvRcvyData
);

MSD_STATUS Fir_gfrerIndvRcvyLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_INDVRCVY	*indvRcvyData
);

MSD_STATUS Fir_gfrerIndvRcvyGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerIndvRcvyGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerIndvRcvyReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerIndvRcvyReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

MSD_STATUS Fir_gfrerSeqRcvyFlushAll
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerSeqRcvyFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId
);

MSD_STATUS Fir_gfrerSeqRcvyRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyLoad
(
IN MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_SEQRCVY	*SeqRcvyData
);

MSD_STATUS Fir_gfrerSeqRcvyLoadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_FRER_SEQRCVY	*seqRcvyData
);

MSD_STATUS Fir_gfrerSeqRcvyGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*SeqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyGetViolation
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*SeqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyGetViolationIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*SeqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerSeqRcvyReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

MSD_STATUS Fir_gfrerBank0SCtrFlushAll
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerBank0SCtrFlushCtr
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerBank0SCtrRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank0SCtrReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank0SCtrLoad
(
IN MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData
);

MSD_STATUS Fir_gfrerBank0SCtrLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_BANK0_CONFIG	*bk0ConfigData
);

MSD_STATUS Fir_gfrerBank0SCtrGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank0SCtrGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank0SCtrReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank0SCtrReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrFlushAll
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerBank1SCtrFlushCtr
(
IN  MSD_QD_DEV	*dev
);

MSD_STATUS Fir_gfrerBank1SCtrRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrReadIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrLoad
(
IN  MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData
);

MSD_STATUS Fir_gfrerBank1SCtrLoadIntf
(
IN MSD_QD_DEV	*dev,
IN MSD_FRER_BANK1_CONFIG	*bk1ConfigData
);

MSD_STATUS Fir_gfrerBank1SCtrGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrGetNextIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

MSD_STATUS Fir_gfrerBank1SCtrReadClearIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);



#ifdef __cplusplus
}
#endif

#endif /* Fir_msdCB_h */