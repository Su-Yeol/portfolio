/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Fir_msdFRER.c
*
* DESCRIPTION:
*       API definitions for Frame Replication and Elimination for Reliability
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <fir/include/api/Fir_msdFRER.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/**********************************************************************************************
*
* Below is internal basic read/write functions
*
**********************************************************************************************/
typedef enum
{
	NO_OPERATION = 0x0,
	FLUSH_ALL = 0x1,
	FLUSH_ONE = 0x2,
	READ = 0x3,
	LOAD = 0x4,
	GET_NEXT = 0x5,
	GET_VIOLATION = 0X6,
	READ_CLEAR = 0X7
} FIR_MSD_FRER_OPERATION;

typedef enum
{
	SEQ_GEN = 0x0,
	INDV_RCVY = 0x1,
	SEQ_RCVY = 0x2,
	BANK_0 = 0x4,
	BANK_1 = 0X5
} FIR_MSD_FRER_BLOCK;

static MSD_STATUS Fir_frerBasicWrite
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort,
IN MSD_U8 frerRegAddr,
IN MSD_U16 writeData
);

static MSD_STATUS Fir_frerBasicRead
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort,
IN MSD_U8 frerRegAddr,
OUT MSD_U16* readData
);

static MSD_STATUS Fir_frerWaitBusyBitSelfClear
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort
);

static MSD_STATUS Fir_frerSeqGenOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenOp,
IN  MSD_U8	seqGenId
);

static MSD_STATUS Fir_frerIndvRcvyOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyOp,
IN  MSD_U8	indvRcvyId
);

static MSD_STATUS Fir_frerSeqRcvyOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyOp,
IN  MSD_U8	seqRcvyId
);

static MSD_STATUS Fir_frerBank0OpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Op,
IN  MSD_U8	bk0Id
);

static MSD_STATUS Fir_frerBank1OpPerform
(
IN	MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Op,
IN  MSD_U8	bk1Id
);

/************************************************************************************************************************************/
/****************************************** below is public function ***************************************************************/
/***********************************************************************************************************************************/

/******************************************************************************
* Fir_gfrerSeqGenFlushAll
*
* DESCRIPTION:
*        This routine flush all sequence generation instances.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqGenFlushAll
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerSeqGenFlushAll Called.\n"));

	/* Program Tuning register */
	op = FLUSH_ALL;

	retVal = Fir_frerSeqGenOpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqGenOpPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqGenFlushAll Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqGenFlushEntry
*
* DESCRIPTION:
*        This routine flush a sequence generation instance.
*
* INPUTS:
*        seqGenId  - sequence generation instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqGenFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_DBG_INFO(("Fir_gfrerSeqGenFlushEntry Called.\n"));

	/* Program Tuning register */
	op = FLUSH_ONE;
	retVal = Fir_frerSeqGenOpPerform(dev, op, seqGenId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqGenOpPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqGenFlushEntry Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerSeqGenRead
*
* DESCRIPTION:
*        This routine gets sequence generation instance data.
*
* INPUTS:
*        seqGenId  - sequence generation instance identifier
*
* OUTPUTS:
*        seqGenData  - sequence generation instance data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqGenRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenId,
OUT FIR_MSD_FRER_SEQGEN	*seqGenData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;
	MSD_DBG_INFO(("Fir_gfrerSeqGenRead Called.\n"));

	if (seqGenId > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ;
	retVal = Fir_frerSeqGenOpPerform(dev, op, seqGenId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqGenOpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, SEQ_GEN, 0, &data);
	seqGenData->seqGenId = seqGenId;
	retVal = Fir_frerBasicRead(dev, SEQ_GEN, 1, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR((" Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	seqGenData->genSeqNum = data;
	MSD_DBG_INFO(("Fir_gfrerSeqGenRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqGenLoad
*
* DESCRIPTION:
*        This routine load a sequence generation instance.
*
* INPUTS:
*        seqGenData		 - sequence generation instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqGenLoad
(
IN  MSD_QD_DEV	*dev,
IN  FIR_MSD_FRER_SEQGEN	*seqGenData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerSeqGenLoad Called.\n"));

	if (seqGenData->seqGenId > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	retVal = Fir_frerBasicWrite(dev, SEQ_GEN, 1, seqGenData->genSeqNum);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR((" Fir_frerBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	op = LOAD;
	retVal = Fir_frerSeqGenOpPerform(dev, op, seqGenData->seqGenId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqGenOpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqGenLoad Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerIndvRcvyFlushAll
*
* DESCRIPTION:
*        This routine flush all individual recovery instances.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyFlushAll
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyFlushAll Called.\n"));

	op = FLUSH_ALL;
	retVal = Fir_frerIndvRcvyOpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyFlushAll Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerIndvRcvyFlushEntry
*
* DESCRIPTION:
*        This routine flush an individual recovery instance.
*
* INPUTS:
*        indvRcvyId  - individual recovery instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyFlushEntry Called.\n"));

	if (indvRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = FLUSH_ONE;
	retVal = Fir_frerIndvRcvyOpPerform(dev, op, indvRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyFlushEntry Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerIndvRcvyRead
*
* DESCRIPTION:
*        This routine gets an individual recovery instance data.
*
* INPUTS:
*        indvRcvyId  - individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyRead Called.\n"));

	if (indvRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ;
	retVal = Fir_frerIndvRcvyOpPerform(dev, op, indvRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 0, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyId = indvRcvyId;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 1, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyEn = (data & 0x8000) >> 15;
	indvRcvyData->indvRcvyAlg = (data & 0x2000) >> 13;
	indvRcvyData->indvRcvyTakeNoSeq = (data & 0x1000) >> 12;
	indvRcvyData->indvRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyRstRate = (data & 0x3000) >> 12;
	indvRcvyData->indvRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyResets = &data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerIndvRcvyLoad
*
* DESCRIPTION:
*        This routine load a individual recovery instance.
*
* INPUTS:
*        indvRcvyData	- individual recovery instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyLoad
(
IN  MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_INDVRCVY	*indvRcvyData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16 data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyLoad Called.\n"));

	if (indvRcvyData->indvRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	data = ((indvRcvyData->indvRcvyEn & (MSD_U8)0x1) << 15) | ((indvRcvyData->indvRcvyAlg & (MSD_U8)0x1) << 13) | ((indvRcvyData->indvRcvyTakeNoSeq & (MSD_U8)0x1) << 12) | (indvRcvyData->indvRcvySeqHisLen & (MSD_U8)0x1F);
	retVal = Fir_frerBasicWrite(dev, INDV_RCVY, 1, data);

	data = ((indvRcvyData->indvRcvyRstRate & (MSD_U8)0x3) << 12) | (indvRcvyData->indvRcvyRstTime & (MSD_U16)0x1FF);
	retVal = Fir_frerBasicWrite(dev, INDV_RCVY, 2, data);

	op = LOAD;

	retVal = Fir_frerIndvRcvyOpPerform(dev, op, (indvRcvyData->indvRcvyId & 0x7F));
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform LOAD returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyLoad Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerIndvRcvyGetNext
*
* DESCRIPTION:
*        This routine gets next individual recovery instance data.
*
* INPUTS:
*        indvRcvyId  - individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyRead Called.\n"));

	if (indvRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = GET_NEXT;
	retVal = Fir_frerIndvRcvyOpPerform(dev, op, indvRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 0, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyId = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 1, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyEn = (data & 0x8000) >> 15;
	indvRcvyData->indvRcvyAlg = (data & 0x2000) >> 13;
	indvRcvyData->indvRcvyTakeNoSeq = (data & 0x1000) >> 12;
	indvRcvyData->indvRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyRstRate = (data & 0x3000) >> 12;
	indvRcvyData->indvRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyResets = &data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerIndvRcvyReadClear
*
* DESCRIPTION:
*        This routine gets an individual recovery instance and clear the instance��s counters.
*
* INPUTS:
*        indvRcvyId  - individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerIndvRcvyReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyId,
OUT FIR_MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyReadClear Called.\n"));

	if (indvRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ_CLEAR;
	retVal = Fir_frerIndvRcvyOpPerform(dev, op, indvRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerIndvRcvyOpPerform READ_CLEAR returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 0, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyId = indvRcvyId;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 1, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyEn = (data & 0x8000) >> 15;
	indvRcvyData->indvRcvyAlg = (data & 0x2000) >> 13;
	indvRcvyData->indvRcvyTakeNoSeq = (data & 0x1000) >> 12;
	indvRcvyData->indvRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 2, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyData->indvRcvyRstRate = (data & 0x3000) >> 12;
	indvRcvyData->indvRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, INDV_RCVY, 4, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	indvRcvyResets = &data;

	MSD_DBG_INFO(("Fir_gfrerIndvRcvyReadClear Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqRcvyFlushAll
*
* DESCRIPTION:
*        This routine flush all sequence recovery instances.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyFlushAll
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyFlushAll Called.\n"));

	op = FLUSH_ALL;

	retVal = Fir_frerSeqRcvyOpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyFlushAll Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerSeqRcvyFlushEntry
*
* DESCRIPTION:
*        This routine flush an sequence recovery instance.
*
* INPUTS:
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyFlushEntry
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyFlushEntry Called.\n"));

	if (seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = FLUSH_ONE;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, seqRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyFlushEntry Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerSeqRcvyRead
*
* DESCRIPTION:
*        This routine gets an sequence recovery instance data.
*
* INPUTS:
*        indvRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Called.\n"));

	if (seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, seqRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform READ returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 0, &data);
	seqRcvyData->seqRcvyId = seqRcvyId;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 1, &data);
	seqRcvyData->seqRcvyEn = (data & 0x8000) >> 15;
	seqRcvyData->seqRcvyPort = (data & 0x1F00) >> 8;
	seqRcvyData->seqRcvyIndex = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 2, &data);
	seqRcvyData->seqRcvyAlg = (data & 0x2000) >> 13;
	seqRcvyData->seqRcvyTakeNoSeq = (data & 0x1000) >> 12;
	seqRcvyData->latErrIntEn = (data & 0x200) >> 9;
	seqRcvyData->latErrStatus = (data & 0x100) >> 8;
	seqRcvyData->seqRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 3, &data);
	seqRcvyData->seqRcvyRstRate = (data & 0x3000) >> 12;
	seqRcvyData->seqRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 4, &data);
	seqRcvyData->latErrEn = (data & 0x8000) >> 15;
	seqRcvyData->rdantPaths = (data & 0x3000) >> 12;
	seqRcvyData->latErrDiff = data & 0xFF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 5, &data);
	seqRcvyData->latErrTestRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrTestTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 6, &data);
	seqRcvyData->latErrRstRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 8, &data);
	seqRcvyResets = &data;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 9, &data);
	latErrResets = &data;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqRcvyLoad
*
* DESCRIPTION:
*        This routine load a sequence recovery instance.
*
* INPUTS:
*        seqRcvyData	- sequence recovery instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyLoad
(
IN MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_SEQRCVY	*seqRcvyData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_frerSeqRcvyLoad Called.\n"));

	if (seqRcvyData->seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	data = ((seqRcvyData->seqRcvyEn & (MSD_U8)1) << 15) | ((seqRcvyData->seqRcvyPort & (MSD_U8)0x1F) << 8) | (seqRcvyData->seqRcvyIndex & (MSD_U8)0x7F);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 1, data);

	data = ((seqRcvyData->seqRcvyAlg & (MSD_U8)0x1) << 13) | ((seqRcvyData->seqRcvyTakeNoSeq & (MSD_U8)0x1) << 12) | ((seqRcvyData->latErrIntEn & (MSD_U8)0x1) << 9) | ((seqRcvyData->latErrStatus & (MSD_U8)0x1) << 8) | (seqRcvyData->seqRcvySeqHisLen & (MSD_U8)0x1F);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 2, data);

	data = ((seqRcvyData->seqRcvyRstRate & (MSD_U8)0x3) << 12) | (seqRcvyData->seqRcvyRstTime & (MSD_U16)0x1FF);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 3, data);

	data = ((seqRcvyData->latErrEn & (MSD_U8)0x1) << 15) | ((seqRcvyData->rdantPaths & (MSD_U8)0x3) << 12) | (seqRcvyData->latErrDiff & (MSD_U8)0xFF);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 4, data);

	data = ((seqRcvyData->latErrTestRate & (MSD_U8)0x3) << 12) | (seqRcvyData->latErrTestTime & (MSD_U16)0x1FF);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 5, data);

	data = ((seqRcvyData->latErrRstRate & (MSD_U8)0x3) << 12) | (seqRcvyData->latErrRstTime & (MSD_U16)0x1FF);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 6, data);

	op = LOAD;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, (seqRcvyData->seqRcvyId & (MSD_U8)0x7F));
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform LOAD returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_frerSeqRcvyLoad Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqRcvyGetNext
*
* DESCRIPTION:
*        This routine gets next sequence recovery instance data.
*
* INPUTS:
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Called.\n"));

	if (seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = GET_NEXT;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, seqRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform READ returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 0, &data);
	seqRcvyData->seqRcvyId = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 1, &data);
	seqRcvyData->seqRcvyEn = (data & 0x8000) >> 15;
	seqRcvyData->seqRcvyPort = (data & 0x1F00) >> 8;
	seqRcvyData->seqRcvyIndex = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 2, &data);
	seqRcvyData->seqRcvyAlg = (data & 0x2000) >> 13;
	seqRcvyData->seqRcvyTakeNoSeq = (data & 0x1000) >> 12;
	seqRcvyData->latErrIntEn = (data & 0x200) >> 9;
	seqRcvyData->latErrStatus = (data & 0x100) >> 8;
	seqRcvyData->seqRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 3, &data);
	seqRcvyData->seqRcvyRstRate = (data & 0x3000) >> 12;
	seqRcvyData->seqRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 4, &data);
	seqRcvyData->latErrEn = (data & 0x8000) >> 15;
	seqRcvyData->rdantPaths = (data & 0x3000) >> 12;
	seqRcvyData->latErrDiff = data & 0xFF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 5, &data);
	seqRcvyData->latErrTestRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrTestTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 6, &data);
	seqRcvyData->latErrRstRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 8, &data);
	seqRcvyResets = &data;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 9, &data);
	latErrResets = &data;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerSeqRcvyGetViolation
*
* DESCRIPTION:
*        This routine gets next higher sequence recovery instance whose LatErrStatus bit is set to a one.
*
* INPUTS:
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyGetViolation
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Called.\n"));

	if (seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = GET_VIOLATION;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, seqRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform READ returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 0, &data);
	seqRcvyData->seqRcvyId = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 1, &data);
	seqRcvyData->seqRcvyEn = (data & 0x8000) >> 15;
	seqRcvyData->seqRcvyPort = (data & 0x1F00) >> 8;
	seqRcvyData->seqRcvyIndex = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 2, &data);
	seqRcvyData->seqRcvyAlg = (data & 0x2000) >> 13;
	seqRcvyData->seqRcvyTakeNoSeq = (data & 0x1000) >> 12;
	seqRcvyData->latErrIntEn = (data & 0x200) >> 9;
	seqRcvyData->latErrStatus = (data & 0x100) >> 8;
	seqRcvyData->seqRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 3, &data);
	seqRcvyData->seqRcvyRstRate = (data & 0x3000) >> 12;
	seqRcvyData->seqRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 4, &data);
	seqRcvyData->latErrEn = (data & 0x8000) >> 15;
	seqRcvyData->rdantPaths = (data & 0x3000) >> 12;
	seqRcvyData->latErrDiff = data & 0xFF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 5, &data);
	seqRcvyData->latErrTestRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrTestTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 6, &data);
	seqRcvyData->latErrRstRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 8, &data);
	seqRcvyResets = &data;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 9, &data);
	latErrResets = &data;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerSeqRcvyReadClear
*
* DESCRIPTION:
*        This routine gets an sequence recovery instance and clear the instance��s counters.
*
* INPUTS:
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerSeqRcvyReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyId,
OUT FIR_MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Called.\n"));

	if (seqRcvyId > (MSD_U8)0x7F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ_CLEAR;
	retVal = Fir_frerSeqRcvyOpPerform(dev, op, seqRcvyId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerSeqRcvyOpPerform READ returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 0, &data);
	seqRcvyData->seqRcvyId = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 1, &data);
	seqRcvyData->seqRcvyEn = (data & 0x8000) >> 15;
	seqRcvyData->seqRcvyPort = (data & 0x1F00) >> 8;
	seqRcvyData->seqRcvyIndex = data & 0x7F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 2, &data);
	seqRcvyData->seqRcvyAlg = (data & 0x2000) >> 13;
	seqRcvyData->seqRcvyTakeNoSeq = (data & 0x1000) >> 12;
	seqRcvyData->latErrIntEn = (data & 0x200) >> 9;
	seqRcvyData->latErrStatus = (data & 0x100) >> 8;
	seqRcvyData->seqRcvySeqHisLen = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 3, &data);
	seqRcvyData->seqRcvyRstRate = (data & 0x3000) >> 12;
	seqRcvyData->seqRcvyRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 4, &data);
	seqRcvyData->latErrEn = (data & 0x8000) >> 15;
	seqRcvyData->rdantPaths = (data & 0x3000) >> 12;
	seqRcvyData->latErrDiff = data & 0xFF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 5, &data);
	seqRcvyData->latErrTestRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrTestTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 6, &data);
	seqRcvyData->latErrRstRate = (data & 0x3000) >> 12;
	seqRcvyData->latErrRstTime = data & 0x3FF;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 8, &data);
	seqRcvyResets = &data;

	retVal = Fir_frerBasicRead(dev, SEQ_RCVY, 9, &data);
	latErrResets = &data;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerSeqRcvyRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank0SCtrFlushAll
*
* DESCRIPTION:
*        This routine flush all bank 0 stream counter instances.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrFlushAll
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS           retVal;
	MSD_U8			     op;

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrFlushAll Called.\n"));

	op = FLUSH_ALL;
	retVal = Fir_frerBank0OpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrFlushAll Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank0SCtrFlushCtr
*
* DESCRIPTION:
*        This routine flush all bank 0 stream counters.
*
* INPUTS:
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrFlushCtr
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS	retVal;
	MSD_U8		op;

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrFlushCtr Called.\n"));

	op = FLUSH_ONE;
	retVal = Fir_frerBank0OpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrFlushCtr Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerBank0SCtrRead
*
* DESCRIPTION:
*        This routine gets an bank 0 stream counter instance data.
*
* INPUTS:
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8		op;
	MSD_U16		data;
	MSD_U16		data1;
	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Called.\n"));

	if (bk0Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ;
	retVal = Fir_frerBank0OpPerform(dev, op, bk0Id);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_0, 0, &data);
	bk0ConfigData->bk0SCtrId = bk0Id;

	retVal = Fir_frerBasicRead(dev, BANK_0, 1, &data);
	bk0ConfigData->bk0SCtrEn = (data & 0x8000) >> 15;
	bk0ConfigData->bk0SCtrPort = (data & 0x1F00) >> 8;
	bk0ConfigData->bk0SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_0, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 5, &data1);
	bk0SCtrData->indvRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 7, &data1);
	bk0SCtrData->indvRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 9, &data1);
	bk0SCtrData->indvRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 11, &data1);
	bk0SCtrData->indvRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 13, &data1);
	bk0SCtrData->indvRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 15, &data1);
	bk0SCtrData->indvRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 17, &data1);
	bk0SCtrData->sidInput = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 18, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 19, &data1);
	bk0SCtrData->seqEncErrored = (data << 16) | data1;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank0SCtrLoad
*
* DESCRIPTION:
*        This routine load a bank 0 stream counter instance.
*
* INPUTS:
*        bk0ConfigData	- bank 0 stream config data
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrLoad
(
IN  MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;
	MSD_DBG_INFO(("Fir_gfrerBank0SCtrLoad Called.\n"));

	if (bk0ConfigData->bk0SCtrId > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	data = ((bk0ConfigData->bk0SCtrEn & (MSD_U8)0x1) << 15) | ((bk0ConfigData->bk0SCtrPort & (MSD_U8)0x1F) << 8) | (bk0ConfigData->bk0SCtrIndex & (MSD_U8)0x1F);
	retVal = Fir_frerBasicWrite(dev, BANK_0, 1, data);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	op = LOAD;
	retVal = Fir_frerBank0OpPerform(dev, op, bk0ConfigData->bk0SCtrId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform LOAD returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_frerB0SCtrLoad Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank0SCtrGetNext
*
* DESCRIPTION:
*        This routine gets next bank 0 stream counter instance data.
*
* INPUTS:
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8		op;
	MSD_U16		data;
	MSD_U16		data1;
	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Called.\n"));

	if (bk0Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = GET_NEXT;
	retVal = Fir_frerBank0OpPerform(dev, op, bk0Id);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_0, 0, &data);
	bk0ConfigData->bk0SCtrId = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_0, 1, &data);
	bk0ConfigData->bk0SCtrEn = (data & 0x8000) >> 15;
	bk0ConfigData->bk0SCtrPort = (data & 0x1F00) >> 8;
	bk0ConfigData->bk0SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_0, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 5, &data1);
	bk0SCtrData->indvRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 7, &data1);
	bk0SCtrData->indvRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 9, &data1);
	bk0SCtrData->indvRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 11, &data1);
	bk0SCtrData->indvRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 13, &data1);
	bk0SCtrData->indvRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 15, &data1);
	bk0SCtrData->indvRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 17, &data1);
	bk0SCtrData->sidInput = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 18, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 19, &data1);
	bk0SCtrData->seqEncErrored = (data << 16) | data1;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank0SCtrReadClear
*
* DESCRIPTION:
*        This routine gets a bank 0 stream counter instance data and clear the instance��s counters.
*
* INPUTS:
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank0SCtrReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Id,
OUT FIR_MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT FIR_MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8		op;
	MSD_U16		data;
	MSD_U16		data1;
	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Called.\n"));

	if (bk0Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ_CLEAR;
	retVal = Fir_frerBank0OpPerform(dev, op, bk0Id);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank0OpPerform READ_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_0, 0, &data);
	bk0ConfigData->bk0SCtrId = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_0, 1, &data);
	bk0ConfigData->bk0SCtrEn = (data & 0x8000) >> 15;
	bk0ConfigData->bk0SCtrPort = (data & 0x1F00) >> 8;
	bk0ConfigData->bk0SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_0, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 5, &data1);
	bk0SCtrData->indvRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 7, &data1);
	bk0SCtrData->indvRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 9, &data1);
	bk0SCtrData->indvRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 11, &data1);
	bk0SCtrData->indvRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 13, &data1);
	bk0SCtrData->indvRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 15, &data1);
	bk0SCtrData->indvRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 17, &data1);
	bk0SCtrData->sidInput = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_0, 18, &data);
	retVal = Fir_frerBasicRead(dev, BANK_0, 19, &data1);
	bk0SCtrData->seqEncErrored = (data << 16) | data1;

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank0SCtrRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank1SCtrFlushAll
*
* DESCRIPTION:
*        This routine flush all bank 1 stream counter instances.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrFlushAll
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS           retVal;
	MSD_U8			     op;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrFlushAll Called.\n"));

	op = FLUSH_ALL;
	retVal = Fir_frerBank1OpPerform(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank1OpPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrFlushAll Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank1SCtrFlushCtr
*
* DESCRIPTION:
*        This routine flush an bank 1 stream counter instance.
*
* INPUTS:
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrFlushCtr
(
IN  MSD_QD_DEV	*dev
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrFlushCtr Called.\n"));

	op = FLUSH_ONE;
	retVal = Fir_frerBank1OpPerform(dev, op, 0);

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank1OpPerform FLUSH_ONE returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrFlushCtr Exit.\n"));
	return retVal;
}

/******************************************************************************
* Fir_gfrerBank1SCtrRead
*
* DESCRIPTION:
*        This routine gets an bank 1 stream counter instance data.
*
* INPUTS:
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrRead
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;
	MSD_U16	data1;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Called.\n"));

	if (bk1Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ;
	retVal = Fir_frerBank1OpPerform(dev, op, bk1Id);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("frerOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_1, 0, &data);
	bk1ConfigData->bk1SCtrId = bk1Id;

	retVal = Fir_frerBasicRead(dev, BANK_1, 1, &data);
	bk1ConfigData->bk1SCtrEn = (data & 0x8000) >> 15;
	bk1ConfigData->bk1SCtrPort = (data & 0x1F00) >> 8;
	bk1ConfigData->bk1SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_1, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 5, &data1);
	bk1SCtrData->seqRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 7, &data1);
	bk1SCtrData->seqRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 9, &data1);
	bk1SCtrData->seqRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 11, &data1);
	bk1SCtrData->seqRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 13, &data1);
	bk1SCtrData->seqRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 15, &data1);
	bk1SCtrData->seqRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 17, &data1);
	bk1SCtrData->sidOutput = (data << 16) | data1;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank1SCtrLoad
*
* DESCRIPTION:
*        This routine load a bank 1 stream counter instance.
*
* INPUTS:
*        bk1ConfigData	- bank 1 stream config data
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrLoad
(
IN  MSD_QD_DEV	*dev,
IN FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData
)
{
	MSD_STATUS	retVal;
	FIR_MSD_FRER_OPERATION	op;
	MSD_U16	data;

	if (bk1ConfigData->bk1SCtrId > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrLoad Called.\n"));
	
	data = ((bk1ConfigData->bk1SCtrEn & (MSD_U8)0x1) << 15) | ((bk1ConfigData->bk1SCtrPort & (MSD_U8)0x1F) << 8) | (bk1ConfigData->bk1SCtrIndex & (MSD_U8)0x1F);
	retVal = Fir_frerBasicWrite(dev, BANK_1, 1, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	op = LOAD;
	retVal = Fir_frerBank1OpPerform(dev, op, bk1ConfigData->bk1SCtrId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_frerBank1OpPerform LOAD returned: %s.\n", msdDisplayStatus(retVal)));
	}

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrLoad Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank1SCtrGetNext
*
* DESCRIPTION:
*        This routine gets next bank 1 stream counter instance data.
*
* INPUTS:
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrGetNext
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;
	MSD_U16	data1;

	if (bk1Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Called.\n"));

	op = GET_NEXT;
	retVal = Fir_frerBank1OpPerform(dev, op, bk1Id);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("frerOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_1, 0, &data);
	bk1ConfigData->bk1SCtrId = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_1, 1, &data);
	bk1ConfigData->bk1SCtrEn = (data & 0x8000) >> 15;
	bk1ConfigData->bk1SCtrPort = (data & 0x1F00) >> 8;
	bk1ConfigData->bk1SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_1, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 5, &data1);
	bk1SCtrData->seqRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 7, &data1);
	bk1SCtrData->seqRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 9, &data1);
	bk1SCtrData->seqRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 11, &data1);
	bk1SCtrData->seqRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 13, &data1);
	bk1SCtrData->seqRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 15, &data1);
	bk1SCtrData->seqRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 17, &data1);
	bk1SCtrData->sidOutput = (data << 16) | data1;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Exit.\n"));
	return retVal;

}

/******************************************************************************
* Fir_gfrerBank1SCtrReadClear
*
* DESCRIPTION:
*        This routine gets a bank 1 stream counter instance data and clear the instance��s counters.
*
* INPUTS:
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS Fir_gfrerBank1SCtrReadClear
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Id,
OUT FIR_MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT FIR_MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
	MSD_STATUS	retVal;
	MSD_U8	op;
	MSD_U16	data;
	MSD_U16	data1;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Called.\n"));

	if (bk1Id > (MSD_U8)0x1F)
	{
		return MSD_BAD_PARAM;
	}

	op = READ_CLEAR;
	retVal = Fir_frerBank1OpPerform(dev, op, bk1Id);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("frerOperationPerform FLUSH_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_frerBasicRead(dev, BANK_1, 0, &data);
	bk1ConfigData->bk1SCtrId = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_1, 1, &data);
	bk1ConfigData->bk1SCtrEn = (data & 0x8000) >> 15;
	bk1ConfigData->bk1SCtrPort = (data & 0x1F00) >> 8;
	bk1ConfigData->bk1SCtrIndex = data & 0x1F;

	retVal = Fir_frerBasicRead(dev, BANK_1, 4, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 5, &data1);
	bk1SCtrData->seqRcvyTagless = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 6, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 7, &data1);
	bk1SCtrData->seqRcvyPassed = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 8, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 9, &data1);
	bk1SCtrData->seqRcvyDiscarded = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 10, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 11, &data1);
	bk1SCtrData->seqRcvyOutofOrder = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 12, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 13, &data1);
	bk1SCtrData->seqRcvyRogue = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 14, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 15, &data1);
	bk1SCtrData->seqRcvyLost = (data << 16) | data1;

	retVal = Fir_frerBasicRead(dev, BANK_1, 16, &data);
	retVal = Fir_frerBasicRead(dev, BANK_1, 17, &data1);
	bk1SCtrData->sidOutput = (data << 16) | data1;

	MSD_DBG_INFO(("Fir_gfrerBank1SCtrRead Exit.\n"));
	return retVal;

}

static MSD_STATUS Fir_frerBasicWrite
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort,
IN MSD_U8 frerRegAddr,
IN MSD_U16 writeData
)
{
	MSD_STATUS retVal;
	MSD_U16 busyBit = 1;
	MSD_U8 avbOp = 3;
	MSD_U8 avbBlock = 5;

	msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		/*wait busyBit Clear*/
		while (busyBit != 0)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		/*Set AVB writeData*/
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_DATA, writeData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*Set AVB Command*/
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (frerRegAddr & 0x1F)));
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*wait busyBit Clear*/
		busyBit = 1;
		while (busyBit != 0)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}
	}
	else
	{
		/*wait busyBit Clear*/
		while (busyBit != 0)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		/*Set AVB writeData*/
		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, writeData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*Set AVB Command*/
		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (frerRegAddr & 0x1F)));
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*wait busyBit Clear*/
		busyBit = 1;
		while (busyBit != 0)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}
	}

	msdSemGive(dev->devNum, dev->ptpRegsSem);
	return MSD_OK;
}

static MSD_STATUS Fir_frerBasicRead
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort,
IN MSD_U8 frerRegAddr,
OUT MSD_U16* readData
)
{
	MSD_STATUS retVal;
	MSD_U16 busyBit = 1;
	MSD_U16 avbOp = 0;
	MSD_U16 avbBlock = 5;
	MSD_U16 data;

	msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);
	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		/*wait busyBit Clear*/
		while (busyBit != 0)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)(avbOp << 13) | (MSD_U16)(avbPort << 8) | (MSD_U16)(avbBlock << 5) | (MSD_U16)(frerRegAddr & 0x1F));
		/*Set AVB Command*/
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*wait busyBit Clear*/
		busyBit = 1;
		while (busyBit != 0)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		/*Get AVB Data*/
		retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_AVB_DATA, readData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}
	else
	{
		/*wait busyBit Clear*/
		while (busyBit != 0)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)(avbOp << 13) | (MSD_U16)(avbPort << 8) | (MSD_U16)(avbBlock << 5) | (MSD_U16)(frerRegAddr & 0x1F));
		/*Set AVB Command*/
		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		/*wait busyBit Clear*/
		busyBit = 1;
		while (busyBit != 0)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, 15, 1, &busyBit);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		/*Get AVB Data*/
		retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, readData);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}
	msdSemGive(dev->devNum, dev->ptpRegsSem);
	return MSD_OK;
}

static MSD_STATUS Fir_frerWaitBusyBitSelfClear
(
IN MSD_QD_DEV* dev,
IN MSD_U8 avbPort
)
{
	MSD_STATUS retVal;
	MSD_U8 frerRegAddr = 0;
	MSD_U16 frerData = 0;
	MSD_32 count = 0;
	while (count < 100)
	{
		count++;
		retVal = Fir_frerBasicRead(dev, avbPort, frerRegAddr, &frerData);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
		if ((frerData & (MSD_U16)0x8000) == 0)
		{
			retVal = MSD_OK;
			break;
		}
		if (count >= 10)
		{
			retVal = MSD_FAIL;
			return retVal;
		}
	}
	return MSD_OK;
}

static MSD_STATUS Fir_frerSeqGenOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqGenOp,
IN  MSD_U8	seqGenId
)
{
	MSD_STATUS	retVal;    /* Functions return value */
	MSD_U16	data;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->frerRegsSem, OS_WAIT_FOREVER);

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, SEQ_GEN);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)seqGenOp << 12) | (MSD_U16)seqGenId);
	retVal = Fir_frerBasicWrite(dev, SEQ_GEN, 0, data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}
	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, SEQ_GEN);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->frerRegsSem);
	return retVal;
}

static MSD_STATUS Fir_frerIndvRcvyOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	indvRcvyOp,
IN  MSD_U8	indvRcvyId
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data = 0;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->frerRegsSem, OS_WAIT_FOREVER);

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, INDV_RCVY);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)indvRcvyOp << 12) | (MSD_U16)indvRcvyId);
	retVal = Fir_frerBasicWrite(dev, INDV_RCVY, 0, data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}
	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, INDV_RCVY);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->frerRegsSem);
	return retVal;
}

static MSD_STATUS Fir_frerSeqRcvyOpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	seqRcvyOp,
IN  MSD_U8	seqRcvyId
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U16          data = 0;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->frerRegsSem, OS_WAIT_FOREVER);

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, SEQ_RCVY);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)seqRcvyOp << 12) | (MSD_U16)seqRcvyId);
	retVal = Fir_frerBasicWrite(dev, SEQ_RCVY, 0, data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}
	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, SEQ_RCVY);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->frerRegsSem);
	return retVal;
}

static MSD_STATUS Fir_frerBank0OpPerform
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8	bk0Op,
IN  MSD_U8	bk0Id
)
{
	MSD_STATUS	retVal;    /* Functions return value */
	MSD_U16	data = 0;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->frerRegsSem, OS_WAIT_FOREVER);

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, BANK_0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)bk0Op << 12) | (MSD_U16)bk0Id);
	retVal = Fir_frerBasicWrite(dev, BANK_0, 0, data);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, BANK_0);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->frerRegsSem);
	return retVal;
}

static MSD_STATUS Fir_frerBank1OpPerform
(
IN	MSD_QD_DEV	*dev,
IN  MSD_U8	bk1Op,
IN  MSD_U8	bk1Id
)
{
	MSD_STATUS	retVal;    /* Functions return value */
	MSD_U16	data = 0;     /* temporary Data storage */

	msdSemTake(dev->devNum, dev->frerRegsSem, OS_WAIT_FOREVER);

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, BANK_1);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)bk1Op << 12) | (MSD_U16)bk1Id);
	retVal = Fir_frerBasicWrite(dev, BANK_1, 0, data);

	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	/*wait busyBit Clear*/
	retVal = Fir_frerWaitBusyBitSelfClear(dev, BANK_1);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->frerRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->frerRegsSem);
	return retVal;
}

