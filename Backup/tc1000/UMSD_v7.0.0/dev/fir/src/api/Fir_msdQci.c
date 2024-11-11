/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Fir_msdQci.c
*
* DESCRIPTION:
*       API definitions for Qci
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#include <fir/include/api/Fir_msdQci.h>
#include <fir/include/api/Fir_msdApiInternal.h>
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
	STREAM_FILTER = 0,
	STREAM_GATE = 1,
	STREAM_GATE_TIME_GEN = 2,
	FLOW_METER = 3,
} FIR_MSD_QCI_BLOCK;

typedef enum
{
	NO_OPERATION = 0,
	FLUSH_ALL = 1,
	FLUSH_ONE = 2,
	READ = 3,
	LOAD = 4,
	RESERVED_GET_NEXT = 5,
	GET_VIOLATION = 6,
	READ_CLEAR = 7,
} FIR_MSD_QCI_OPERATION;

/* tsn basic read/write*/
static MSD_STATUS Fir_TSNBasicRead
(
IN	MSD_QD_DEV	*dev,
INOUT FIR_MSD_TSN_OP_DATA	*opData
);

static MSD_STATUS Fir_TSNBasicWrite
(
IN	MSD_QD_DEV	*dev,
INOUT FIR_MSD_TSN_OP_DATA	*opData
);

static MSD_STATUS Fir_taiBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
);

static MSD_STATUS Fir_waitQciBusyBitClear
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U8        qciBlock
);

/* stream filter basic read/write*/
static MSD_STATUS Fir_streamFilterBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
);

static MSD_STATUS Fir_streamFilterBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
);

static MSD_STATUS Fir_streamFilterOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        streamFilterId
);

/* stream gate basic read/write*/
static MSD_STATUS Fir_streamGateBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
);

static MSD_STATUS Fir_streamGateBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
);

static MSD_STATUS Fir_streamGateOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        streamGateId
);

static MSD_STATUS Fir_streamGateEntryOpWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        pointer
);

/* stream gate time generation related*/
static MSD_STATUS Fir_streamGateTimeGenBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
);

static MSD_STATUS Fir_streamGateTimeGenOpWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        streamGateTimeGenId
);

/* flow meter related */
static MSD_STATUS Fir_flowMeterBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
);

static MSD_STATUS Fir_flowMeterBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
);

static MSD_STATUS Fir_flowMeterOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        flowMeterId
);

/************************************************************************************************************************************/
/****************************************** below is public function ***************************************************************/
/***********************************************************************************************************************************/

/******************************************************************************
* Fir_gqciStreamFilterFlushAll
*
* DESCRIPTION:
*        This routine flush all stream filter instances.
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
MSD_STATUS Fir_gqciStreamFiltFlushAll
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS retVal;
	MSD_U8 op;

	MSD_DBG_INFO(("Fir_gqciStreamFilterFlushAll Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)FLUSH_ALL;
	retVal = Fir_streamFilterOperation(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterFlushAll Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterSet
*
* DESCRIPTION:
*        This routine enable stream filter instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        maxSDUSize      - maximum frame size
*        blockEn         - oversize blocking enable
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
MSD_STATUS Fir_gqciStreamFiltSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U16     maxSDUSize,
IN  MSD_U8		blockEn
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFilterSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 1 */
	regAdr = 1U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	tempData = (tempData & (MSD_U16)0xFFFE) | ((MSD_U16)blockEn & (MSD_U16)0x1);
	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 3 */
	regAdr = 3U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	tempData = (tempData & (MSD_U16)0xC000) | (maxSDUSize & (MSD_U16)0x3FFF);
	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterBlockStatusSet
*
* DESCRIPTION:
*        This routine enable oversize blocking mechanism active or deactive.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        blockStatus     - oversize blocking status
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
MSD_STATUS Fir_gqciStreamFiltBlockStatSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
IN  MSD_U8	blockStatus
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFiltBlockStatSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 1 */
	regAdr = 1U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write desired field */
	tempData = (tempData & (MSD_U16)0xFFFD) | (MSD_U16)(((MSD_U16)blockStatus & (MSD_U16)0x1) << 1);

	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 0, issue load operation */
	op = (MSD_U8)LOAD;

	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterBlockStatusSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterBlockStatusGet
*
* DESCRIPTION:
*        This routine gets oversize blocking mechanism active or deactive status.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        blockStatus     - oversize blocking status.
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
MSD_STATUS Fir_gqciStreamFiltBlockStatGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U8	*blockStatus
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFiltBlockStatGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* access stream filter offset 1 */
	regAdr = 1U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	*blockStatus = (MSD_U8)((tempData & (MSD_U16)0x2) >> 1);

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFiltBlockStatGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterEnAts
*
* DESCRIPTION:
*        This routine enable ATS(Qcr) scheduler instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        atsId		     - ATS scheduler instance identifier
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
MSD_STATUS Fir_gqciStreamFiltEnAts
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U8      atsId
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFilterEnAts Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 2 */
	regAdr = 2U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	tempData = (tempData & (MSD_U16)0x70FF) | (MSD_U16)0x8000 | (MSD_U16)((atsId & (MSD_U16)0xF) << 8);
	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterEnAts Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterEnGate
*
* DESCRIPTION:
*        This routine enable stream gate instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        gateId		     - stream gate instance identifier
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
MSD_STATUS Fir_gqciStreamFiltEnGate
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U8      gateId
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFilterEnGate Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 2 */
	regAdr = 2U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	tempData = (tempData & (MSD_U16)0xFF70) | (MSD_U16)0x80 | (MSD_U16)(gateId & (MSD_U16)0xF);
	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterEnGate Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterMatchCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter matching frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream filter matching frames counter.
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
MSD_STATUS Fir_gqciStreamFiltMatchCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFilterMatchCtrGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* access stream filter offset 4 */
	regAdr = 4U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	*counter = tempData;

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterMatchCtrGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamFilterFiltCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream filter filtered frames counter.
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
MSD_STATUS Fir_gqciStreamFiltFilterCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamFilterFilteredCtrGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* access stream filter offset 5 */
	regAdr = 5U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	*counter = tempData;

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamFilterFilteredCtrGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateFiltCtrGet
*
* DESCRIPTION:
*        This routine gets stream gate filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream gate filtered frames counter.
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
MSD_STATUS Fir_gqciStreamGateFiltCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamGateFilteredCtrGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* access stream filter offset 6 */
	regAdr = 6U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	*counter = tempData;

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateFilteredCtrGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterFiltCtrGet
*
* DESCRIPTION:
*        This routine gets flow meter filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - flow meter filtered frames counter.
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
MSD_STATUS Fir_gqciFlowMeterFiltCtrGet
(
	IN  MSD_QD_DEV* dev,
	IN  MSD_U8  streamFilterId,
	OUT MSD_U16* counter
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterFiltCtrGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream filter offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamFilterOperation(dev, op, streamFilterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* access stream filter offset 7 */
	regAdr = 7U;
	retVal = Fir_streamFilterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	*counter = tempData;

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterFiltCtrGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateFlushAll
*
* DESCRIPTION:
*        This routine flush all stream gate instances.
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
MSD_STATUS Fir_gqciStreamGateFlushAll
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS retVal;
	MSD_U8 op;

	MSD_DBG_INFO(("Fir_gqciStreamGateFlushAll Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)FLUSH_ALL;
	retVal = Fir_streamGateOperation(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateFlushAll Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateDefStateSet
*
* DESCRIPTION:
*        This routine enable default gate state and default gate IPV.
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        defGateState    - default gate state
*        defGateIPVEn    - default gate IPV enable
*        defGateIPV      - default gate IPV
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
MSD_STATUS Fir_gqciStreamGateDefStateSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	defGateState,
IN  MSD_U8	defGateIPVEn,
IN  MSD_U8	defGateIPV
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 1 */
	regAdr = 1U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write desired field */
	tempData = (tempData & (MSD_U16)0xE0FF) | (MSD_U16)(((MSD_U16)defGateState & (MSD_U16)0x1) << 12) | 
		(MSD_U16)(((MSD_U16)defGateIPVEn & (MSD_U16)0x1) << 11) | (MSD_U16)(((MSD_U16)defGateIPV & (MSD_U16)0x7) << 8);

	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateEntryScaleSet
*
* DESCRIPTION:
*        This routine set stream gate time scale and byte scale
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        timeScale       - time scale
*        byteScale       - byte scale
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
MSD_STATUS Fir_gqciStreamGateScaleSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	timeScale,
IN  MSD_U8	byteScale
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 2 */
	regAdr = 2U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write desired field */
	tempData = (tempData & (MSD_U16)0xF0FF) | (MSD_U16)(((MSD_U16)timeScale & (MSD_U16)0x3) << 10) |
		(MSD_U16)(((MSD_U16)byteScale & (MSD_U16)0x3) << 8);

	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateTimeGenIdSet
*
* DESCRIPTION:
*        This routine set stream gate time generation identifier
*
* INPUTS:
*        streamGateId			 - stream gate instance identifier
*        streamGateTimeGenId	 - stream gate time generation instance identifier
*
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
MSD_STATUS Fir_gqciStreamGateTimeGenIdSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	streamGateTimeGenId
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 2 */
	regAdr = 2U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write streamGateTimeGenId field */
	tempData = (tempData & (MSD_U16)0xFFFC) | (MSD_U16)((MSD_U16)streamGateTimeGenId & (MSD_U16)0x3);

	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateDefStateSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateEntryAdd
*
* DESCRIPTION:
*        This routine load stream gate entry
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        pointer         - pointer to the desired Gate Control Entry
*        streamGateEntry - stream gate entry
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
MSD_STATUS Fir_gqciStreamGateEntryAdd
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8  pointer,
IN  FIR_MSD_STREAM_GATE_ENTRY *streamGateEntry
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciStreamGateEntryAdd Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	regAdr = 0U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write desired field */
	tempData = (tempData & (MSD_U16)0xFFF0) | (MSD_U16)((MSD_U16)streamGateId & (MSD_U16)0xF);
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 5 */
	regAdr = 5U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write desired field */
	tempData = (tempData & (MSD_U16)0xFFE0) | (MSD_U16)(((MSD_U16)streamGateEntry->gateState & (MSD_U16)0x1) << 4) |
		(MSD_U16)(((MSD_U16)streamGateEntry->gateIPVEn & (MSD_U16)0x1) << 3) | (MSD_U16)((MSD_U16)streamGateEntry->gateIPV & (MSD_U16)0x7);
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 6 */
	regAdr = 6U;
	tempData = streamGateEntry->gateWindowTime;
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 7 */
	regAdr = 7U;
	tempData = streamGateEntry->gateAllowedBytes;
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 4, write */
	retVal = Fir_streamGateEntryOpWrite(dev, pointer);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateEntryOpWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream gate offset 2 */
	regAdr = 2U;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write Gate Control List Execution Enable */
	tempData = (tempData & (MSD_U16)0x7FFF) | (MSD_U16)0x8000;
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access stream filter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_streamGateOperation(dev, op, streamGateId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateEntryAdd Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateTimeGenFlush
*
* DESCRIPTION:
*        This routine flush stream gate time generation instances.
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
MSD_STATUS Fir_gqciStreamGateTimeGenFlush
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U8 i, j;

	MSD_DBG_INFO(("Fir_gqciStreamGateTimeGenFlush Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate time gen offset 0 */
	for (i = 0; i < 4U; i++)
	{
		for (j = 1U; j < 7U; j++)
		{
			retVal = Fir_streamGateTimeGenBasicWrite(dev, j, 0);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
				msdSemGive(dev->devNum, dev->qciRegsSem);
				return retVal;
			}
		}

		retVal = Fir_streamGateTimeGenOpWrite(dev, i);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_streamGateTimeGenOpWrite returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->qciRegsSem);
			return retVal;
		}
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateTimeGenFlush Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciStreamGateTimeGenSet
*
* DESCRIPTION:
*        This routine sets stream gate time generation cycle time and base time.
*
* INPUTS:
*        streamGateGenId - stream gate time generation instance identifier
*        gateCycleTime   - gate cycle time, unit ns
*        gateBaseTime    - gate base time, PTP Global Timer, PTP ToD, PTP 1722 Nano
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
MSD_STATUS Fir_gqciStreamGateTimeGenSet
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U8  streamGateGenId,
IN  MSD_U32 gateCycleTime,
IN  MSD_U64 gateBaseTime
)
{
	MSD_STATUS retVal;
	MSD_U16 dataVal;
	MSD_U8 tmpRegAddr;
	MSD_U8 trigSel;
	MSD_U32 cycleTime;

	MSD_DBG_INFO(("Fir_gqciStreamGateTimeGenSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* get TrigGenTimeSel (see TAI offset 0x12) */
	tmpRegAddr = (MSD_U8)0x12;
	retVal = Fir_taiBasicRead(dev, tmpRegAddr, &dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_taiBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* check trigSel using PTP Global Timer, PTP ToD, PTP 1722 Nano */
	trigSel = (MSD_U8)((dataVal & (MSD_U16)0x60) >> 5);
	if (trigSel == 0U)
	{
		/* the unit of this interval is as same as PTP Global Timer */
		cycleTime = gateCycleTime / 4U;
	}
	else if ((trigSel == 1U) || (trigSel == 2U))
	{
		/* the unit of this interval is nanoseconds */
		cycleTime = gateCycleTime;
	}
	else
	{
		MSD_DBG_ERROR(("TrigGenTimeSel is reseved, please check.\n"));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return MSD_BAD_PARAM;
	}

	/* write gate cycle time low 16 bits*/
	tmpRegAddr = 2U;
	dataVal = (MSD_U16)(cycleTime & (MSD_U32)0xFFFF);
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate cycle time high 8 bits*/
	tmpRegAddr = 3U;
	dataVal = (MSD_U16)((cycleTime >> 16) & (MSD_U32)0xFF);
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate base time low 16 bits*/
	tmpRegAddr = 4U;
	dataVal = (MSD_U16)(gateBaseTime & (MSD_U32)0xFFFF);
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate base time middle 16 bits*/
	tmpRegAddr = 5U;
	dataVal = (MSD_U16)((gateBaseTime >> 16) & (MSD_U32)0xFFFF);
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate base time high 8 bits*/
	tmpRegAddr = 6U;
	dataVal = (MSD_U16)((gateBaseTime >> 32) & (MSD_U32)0xFF);
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate time generation enable */
	tmpRegAddr = 1U;
	dataVal = 1U;
	retVal = Fir_streamGateTimeGenBasicWrite(dev, tmpRegAddr, dataVal);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write gate time generation instance to corresponding Id */
	retVal = Fir_streamGateTimeGenOpWrite(dev, streamGateGenId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenOpWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciStreamGateTimeGenSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterFlushAll
*
* DESCRIPTION:
*        This routine flush all flow meter instances.
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
MSD_STATUS Fir_gqciFlowMeterFlushAll
(
IN  MSD_QD_DEV     *dev
)
{
	MSD_STATUS retVal;
	MSD_U8 op;

	MSD_DBG_INFO(("Fir_gqciFlowMeterFlushAll Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access flow meter offset 0 */
	op = (MSD_U8)FLUSH_ALL;
	retVal = Fir_flowMeterOperation(dev, op, 0);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterFlushAll Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterCoupleFlagSet
*
* DESCRIPTION:
*        This routine set flow meter coupling flag
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        flag      	   - coupling flag
*
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
MSD_STATUS Fir_gqciFlowMeterCoupleFlagSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	flag
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterCouplingFlagSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write coupling flag field */
	tempData = (tempData & (MSD_U16)0xFDFF) | (MSD_U16)(((MSD_U16)flag & (MSD_U16)0x1) << 9);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterCouplingFlagSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterCoupleFlagGet
*
* DESCRIPTION:
*        This routine get flow meter coupling flag
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
*
* OUTPUTS:
*        flag      	   - coupling flag
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
MSD_STATUS Fir_gqciFlowMeterCoupleFlagGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
OUT MSD_U8	*flag
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterCoupleFlagGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* split coupling flag field */
	*flag = (MSD_U8)((tempData & (MSD_U16)0x200) >> 9);

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterCoupleFlagGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterDropOnYelSet
*
* DESCRIPTION:
*        This routine set flow meter drop on yellow
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        enable    	   - enable drop on yellow
*
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
MSD_STATUS Fir_gqciFlowMeterDropOnYelSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	enable
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterDropOnYelSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write drop on yellow field */
	tempData = (tempData & (MSD_U16)0xFBFF) | (MSD_U16)(((MSD_U16)enable & (MSD_U16)0x1) << 10);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterDropOnYelSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterDropOnYelGet
*
* DESCRIPTION:
*        This routine get flow meter drop on yellow
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        
*
*
* OUTPUTS:
*        enable    	   - enable drop on yellow.
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
MSD_STATUS Fir_gqciFlowMeterDropOnYelGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	*enable
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterDropOnYelGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* split drop on yellow field */
	*enable = (MSD_U8)((tempData & (MSD_U16)0x400) >> 10);

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterDropOnYelGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterRedBlockEnSet
*
* DESCRIPTION:
*        This routine enable flow meter red block
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        enable    	   - enable red block
*
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
MSD_STATUS Fir_gqciFlowMeterRedBlockEnSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	enable
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlockEnSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write red block en field */
	tempData = (tempData & (MSD_U16)0xFFFE) | (MSD_U16)((MSD_U16)enable & (MSD_U16)0x1);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlockEnSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterRedBlockEnGet
*
* DESCRIPTION:
*        This routine get flow meter red block enable bit
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        
*
*
* OUTPUTS:
*        enable    	   - enable red block.
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
MSD_STATUS Fir_gqciFlowMeterRedBlockEnGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	*enable
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlockEnGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* split red block en field */
	*enable = (MSD_U8)(tempData & (MSD_U16)0x1);

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlockEnGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterRedBlkStatSet
*
* DESCRIPTION:
*        This routine sets flow meter red block status
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        status    	   - red block status
*
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
MSD_STATUS Fir_gqciFlowMeterRedBlkStatSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	status
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlkStatSet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write red block status field */
	tempData = (tempData & (MSD_U16)0xFFFD) | (MSD_U16)(((MSD_U16)status & (MSD_U16)0x1) << 1);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlkStatSet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterRedBlkStatGet
*
* DESCRIPTION:
*        This routine gets flow meter red block status
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
* OUTPUTS:
*        status    	   - red block status
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
MSD_STATUS Fir_gqciFlowMeterRedBlkStatGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
OUT MSD_U8	*status
)
{
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlkStatGet Called.\n"));

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* split red block staus field */
	*status = (MSD_U8)((tempData & (MSD_U16)0x2) >> 1);

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterRedBlkStatGet Exit.\n"));
	return retVal;
}


/******************************************************************************
* Fir_gqciFlowMeterAdvConfig
*
* DESCRIPTION:
*        This routine calculates flow meter bktDec, BTRF, BBRF value based on
*        target rate and target burst size
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        tgtRate       - target rate, units is bps
*        bstSize       - target burst size (units is byte)
*        countMode     - count mode
*
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
MSD_STATUS Fir_gqciFlowMeterAdvConfig
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U32 tgtRate,
IN  MSD_U32 bstSize,
IN  MSD_U8  countMode
)
{
#if defined(__KERNEL__) && defined(LINUX)
	return MSD_NOT_SUPPORTED;
#else
	MSD_STATUS retVal;
	MSD_U8 op;
	MSD_U8 regAdr;
	MSD_U16 tempData;

	MSD_U32 flowMeter_constant = (MSD_U32)500000000;
	MSD_U16 bktDecrement = (MSD_U16)0x2000;
	MSD_U16 bktDec = 0U;
	MSD_U16 bktTokenFactorGrn = 0U;
	MSD_U16 bktByteFactorGrn = 0U;
	MSD_U16 tmpBktTokenFactorGrn = 0U;
	MSD_U16 tmpBktByteFactorGrn = 0U;
	MSD_U32 cbsLimit = (MSD_U32)(2U * bstSize);

	MSD_DOUBLE Rate = 0.0;
	MSD_DOUBLE delta = 1.0, deltaMin = 1.0, mode = 1.0;
	MSD_DOUBLE tgRate = (MSD_DOUBLE)(tgtRate * 1000.0);
	MSD_U32 j;

	MSD_DBG_INFO(("Fir_gqciFlowMeterAdvConfig Called.\n"));

	if (tgtRate > 0U && tgtRate < 61)
	{
		MSD_DBG_ERROR(("Bad tgtRate, tgtRate: %u.\n", tgtRate));
		return MSD_BAD_PARAM;
	}

	if (countMode > 2U)
	{
		MSD_DBG_ERROR(("Bad Count Mode, countMode: %u.\n", countMode));
		return MSD_BAD_PARAM;
	}

	if (tgtRate != 0)
	{
		for (j = 1U; j < bktDecrement; j++)
		{
			tmpBktByteFactorGrn = (MSD_U16)(tgRate / flowMeter_constant);

			if ((MSD_U32)((tgRate / flowMeter_constant) * j * mode + 0.5) < (MSD_U32)0xFFFF)
			{
				tmpBktTokenFactorGrn = (MSD_U16)(((tgRate / flowMeter_constant) - tmpBktByteFactorGrn) * j * mode + 0.5);
			}
			else
			{
				continue;
			}

			Rate = (MSD_DOUBLE)(((MSD_DOUBLE)flowMeter_constant / mode) * ((MSD_DOUBLE)tmpBktByteFactorGrn + (MSD_DOUBLE)(tmpBktTokenFactorGrn / (j * mode))));
			delta = (MSD_DOUBLE)(((tgRate * mode) - Rate) / tgRate);
			if (delta < (MSD_DOUBLE)0)
			{
				delta = (MSD_DOUBLE)0 - delta;
			}

			if (deltaMin > delta)
			{
				deltaMin = delta;
				bktDec = (MSD_U16)j;
				bktTokenFactorGrn = tmpBktTokenFactorGrn;
				bktByteFactorGrn = tmpBktByteFactorGrn;

				if (bktTokenFactorGrn >= bktDec)
				{
					continue;
				}

				if (delta == (MSD_DOUBLE)0)
				{
					break;
				}
			}
		}

		if (deltaMin == (MSD_DOUBLE)1)
		{
			MSD_DBG_ERROR(("Failed (Fir_gqciFlowMeterAdvConfig tune failed).\n"));
			return MSD_FAIL;
		}

		MSD_DBG_INFO(("delta((tgtRate - rate)/tgtRate) = %f", deltaMin));
	}

	msdSemTake(dev->devNum, dev->qciRegsSem, OS_WAIT_FOREVER);

	/* access stream gate offset 0 */
	op = (MSD_U8)READ;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 1 */
	regAdr = 1U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write count mode field */
	tempData = (tempData & (MSD_U16)0xCFFF) | (MSD_U16)(((MSD_U16)countMode & (MSD_U16)0x3) << 12);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 2 */
	regAdr = 2U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write bktDec field */
	tempData = (tempData & (MSD_U16)0xE000) | (MSD_U16)((MSD_U16)bktDec & (MSD_U16)0x1FFF);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 3 */
	regAdr = 3U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write bktTokenRateFactorGrn field */
	tempData = (tempData & (MSD_U16)0xE000) | (MSD_U16)((MSD_U16)bktTokenFactorGrn & (MSD_U16)0x1FFF);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 5 */
	regAdr = 5U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write bktByteRateFactorGrn field */
	tempData = (tempData & (MSD_U16)0xFFC0) | (MSD_U16)((MSD_U16)bktByteFactorGrn & (MSD_U16)0x3F);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 6 */
	regAdr = 6U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write CBSLimit low 16 bits field */
	tempData = (MSD_U16)((MSD_U16)cbsLimit & (MSD_U16)0xFFFF);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 7 */
	regAdr = 7U;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* write CBSLimit high 7 bits field */
	tempData = (MSD_U16)((MSD_U16)(cbsLimit >> 16)& (MSD_U16)0xFF);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	/* access flow meter offset 0, issue load operation */
	op = (MSD_U8)LOAD;
	retVal = Fir_flowMeterOperation(dev, op, flowMeterId);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterOperation returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->qciRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->qciRegsSem);
	MSD_DBG_INFO(("Fir_gqciFlowMeterAdvConfig Exit.\n"));
	return retVal;

#endif
}



/* TSN basic read/write */
static MSD_STATUS Fir_TSNBasicRead
(
IN	MSD_QD_DEV	*dev,
INOUT FIR_MSD_TSN_OP_DATA	*opData
)
{
	MSD_STATUS	retVal;
	MSD_U16		data;
	MSD_U16		tsnOperation;

	msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		/* Wait until the tsn in ready. */
		data = 1U;
		while (data == 1U)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		tsnOperation = 0;
		tsnOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (tsnOperation) |
			(MSD_U16)(opData->tsnPort << 8) |
			(MSD_U16)(opData->tsnBlock << 5) |
			(MSD_U16)(opData->tsnAddr & (MSD_U16)0x1F));

		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
		{
			retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_AVB_DATA, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
		opData->tsnData = (MSD_U32)data;

	}
	else
	{
		/* Wait until the tsn in ready. */
		data = 1U;
		while (data == 1U)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		/* read data */
		tsnOperation = 0;
		tsnOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (tsnOperation) |
			(MSD_U16)(opData->tsnPort << 8) |
			(MSD_U16)(opData->tsnBlock << 5) |
			(MSD_U16)(opData->tsnAddr & (MSD_U16)0x1F));

		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		data = 1U;
		while (data == 1U)
		{
			retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
			if (retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->ptpRegsSem);
				return retVal;
			}
		}

		retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, &data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
		opData->tsnData = data;

	}

	msdSemGive(dev->devNum, dev->ptpRegsSem);

	return retVal;
}

static MSD_STATUS Fir_taiBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x0;
	opData.tsnPort = 0x1E;
	opData.tsnAddr = regAddr;
	opData.tsnData = 0;

	retVal = Fir_TSNBasicRead(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*dataVal = (MSD_U16)opData.tsnData;

	return retVal;
}

static MSD_STATUS Fir_TSNBasicWrite
(
IN	MSD_QD_DEV	*dev,
INOUT FIR_MSD_TSN_OP_DATA	*opData
)
{
	MSD_STATUS	retVal;
	MSD_U16		data;
	MSD_U16		tsnOperation;

	msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		data = (MSD_U16)opData->tsnData;
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_DATA, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		tsnOperation = 3U;
		tsnOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (tsnOperation) |
			(MSD_U16)(opData->tsnPort << 8) |
			(MSD_U16)(opData->tsnBlock << 5) |
			(MSD_U16)(opData->tsnAddr & (MSD_U16)0x1F));

		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}
	else
	{
		data = (MSD_U16)opData->tsnData;
		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}

		tsnOperation = 3U;
		tsnOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (tsnOperation) |
			(MSD_U16)(opData->tsnPort << 8) |
			(MSD_U16)(opData->tsnBlock << 5) |
			(MSD_U16)(opData->tsnAddr & (MSD_U16)0x1F));

		retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
		if (retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->ptpRegsSem);
			return retVal;
		}
	}

	msdSemGive(dev->devNum, dev->ptpRegsSem);

	return retVal;
}

static MSD_STATUS Fir_waitQciBusyBitClear
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U8        qciBlock
)
{
	MSD_STATUS retVal;
	MSD_U16	   tempData;
	MSD_U16    count = 10U;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = qciBlock;
	opData.tsnAddr = regAddr;
	opData.tsnData = 0;

	/* Wait until the qci in ready. */
	do
	{
		retVal = Fir_TSNBasicRead(dev, &opData);
		if (retVal != MSD_OK)
		{
			break;
		}

		if (count-- == 0U)
		{
			return MSD_FAIL;
		}

		tempData = (MSD_U16)(opData.tsnData & (MSD_U32)0x8000);

	} while (tempData == (MSD_U16)0x8000);

	return retVal;
}

/* stream filter related */
static MSD_STATUS Fir_streamFilterBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0;
	opData.tsnAddr = regAddr;
	opData.tsnData = dataVal;

	retVal = Fir_TSNBasicWrite(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

static MSD_STATUS Fir_streamFilterBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0;
	opData.tsnAddr = regAddr;
	opData.tsnData = 0;

	retVal = Fir_TSNBasicRead(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*dataVal = (MSD_U16)opData.tsnData;

	return retVal;
}

static MSD_STATUS Fir_streamFilterOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        streamFilterId
)
{
	MSD_STATUS retVal;
	MSD_U8 block;
	MSD_U8 regAdr;
	MSD_U16 tempData = 0;

	regAdr = 0;
	block = (MSD_U8)STREAM_FILTER;

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	tempData = (MSD_U16)0x8000 | (MSD_U16)((op & (MSD_U16)0x7) << 12) | ((MSD_U16)streamFilterId & (MSD_U16)0xF);

	retVal = Fir_streamFilterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamFilterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

/* stream gate related*/
static MSD_STATUS Fir_streamGateBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0x1;
	opData.tsnAddr = regAddr;
	opData.tsnData = dataVal;

	retVal = Fir_TSNBasicWrite(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

static MSD_STATUS Fir_streamGateBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0x1;
	opData.tsnAddr = regAddr;
	opData.tsnData = 0;

	retVal = Fir_TSNBasicRead(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*dataVal = (MSD_U16)opData.tsnData;

	return retVal;
}

static MSD_STATUS Fir_streamGateOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        streamGateId
)
{
	MSD_STATUS retVal;
	MSD_U8 block;
	MSD_U8 regAdr;
	MSD_U16 tempData = 0;

	regAdr = 0;
	block = (MSD_U8)STREAM_GATE;

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* stream gate enable */
	regAdr = 1;
	retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	tempData = tempData | 0x8000;
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* operation */
	regAdr = 0;
	tempData = (MSD_U16)0x8000 | (MSD_U16)((op & (MSD_U16)0x7) << 12) | ((MSD_U16)streamGateId & (MSD_U16)0xF);

	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

static MSD_STATUS Fir_streamGateEntryOpWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        pointer
)
{
	MSD_STATUS retVal;
	MSD_U8 count = (MSD_U8)0x10;
	MSD_U8 regAdr;
	MSD_U16 tempData = 0;

	regAdr = 0x4U;
	do
	{
		retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
		if (retVal != MSD_OK)
		{
			break;
		}

		if (count-- == 0U)
		{
			return MSD_FAIL;
		}

	} while ((tempData & (MSD_U16)0x8000) == (MSD_U16)0x8000);

	tempData = (MSD_U16)0x8000 | (MSD_U16)(pointer & (MSD_U8)0xF);
	retVal = Fir_streamGateBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	do
	{
		retVal = Fir_streamGateBasicRead(dev, regAdr, &tempData);
		if (retVal != MSD_OK)
		{
			break;
		}

		if (count-- == 0U)
		{
			return MSD_FAIL;
		}

	} while ((tempData & (MSD_U16)0x8000) == (MSD_U16)0x8000);

	return retVal;
}

/* stream gate time generation related*/
static MSD_STATUS Fir_streamGateTimeGenBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0x2;
	opData.tsnAddr = regAddr;
	opData.tsnData = dataVal;

	retVal = Fir_TSNBasicWrite(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

static MSD_STATUS Fir_streamGateTimeGenOpWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        streamGateTimeGenId
)
{
	MSD_STATUS retVal;
	MSD_U8 block;
	MSD_U8 regAdr;
	MSD_U16 tempData = 0;

	regAdr = 0;
	block = (MSD_U8)STREAM_GATE_TIME_GEN;

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	tempData = (MSD_U16)0x8000 | ((MSD_U16)streamGateTimeGenId & (MSD_U16)0x3);

	retVal = Fir_streamGateTimeGenBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_streamGateTimeGenBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

/* flow meter related */
static MSD_STATUS Fir_flowMeterBasicWrite
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
IN  MSD_U16       dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0x3;
	opData.tsnAddr = regAddr;
	opData.tsnData = dataVal;

	retVal = Fir_TSNBasicWrite(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}

static MSD_STATUS Fir_flowMeterBasicRead
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        regAddr,
OUT MSD_U16       *dataVal
)
{
	MSD_STATUS retVal;
	FIR_MSD_TSN_OP_DATA	opData;

	opData.tsnBlock = 0x4;
	opData.tsnPort = 0x3;
	opData.tsnAddr = regAddr;
	opData.tsnData = 0;

	retVal = Fir_TSNBasicRead(dev, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_TSNBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	*dataVal = (MSD_U16)opData.tsnData;

	return retVal;
}

static MSD_STATUS Fir_flowMeterOperation
(
IN  MSD_QD_DEV    *dev,
IN  MSD_U8        op,
IN  MSD_U8        flowMeterId
)
{
	MSD_STATUS retVal;
	MSD_U8 block;
	MSD_U8 regAdr;
	MSD_U16 tempData = 0;

	regAdr = 0;
	block = (MSD_U8)FLOW_METER;

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* flow meter enable */
	regAdr = 1;
	retVal = Fir_flowMeterBasicRead(dev, regAdr, &tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	tempData = tempData | 0x8000;
	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* operation */
	regAdr = 0;
	tempData = (MSD_U16)0x8000 | (MSD_U16)((op & (MSD_U16)0x7) << 12) | ((MSD_U16)flowMeterId & (MSD_U16)0x7F);

	retVal = Fir_flowMeterBasicWrite(dev, regAdr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_flowMeterBasicWrite returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	retVal = Fir_waitQciBusyBitClear(dev, regAdr, block);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_waitQciBusyBitClear returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	return retVal;
}