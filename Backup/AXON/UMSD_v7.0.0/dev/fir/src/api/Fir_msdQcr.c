/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Fir_msdQcr.c
*
* DESCRIPTION:
*       API definitions for Asynchronous Traffic Shaping logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <fir/include/api/Fir_msdQcr.h>
#include <fir/include/api/Fir_msdQueueCtrl.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

#define TIME_INTERVAL 16U
#define TIME_FACTOR (8000000/(TIME_INTERVAL))
typedef enum
{
    NOP = 0x0,
    FLUSH_ALL = 0x1,
    FLUSH_ONE = 0x2,
    READ = 0x3,
    LOAD = 0x4,
    RESERVED = 0x5
} FIR_MSD_ATS_OPERATION;

typedef struct
{
    MSD_U16 atsSchedID;
    MSD_U16 atsSchedGrpID;
    MSD_U16 bucketDec;
    MSD_U16 brf;
    MSD_U32 cbsLimit;
    MSD_U16 maxTimeScale;
    MSD_U16 maxResidenceTime;

} FIR_MSD_ATS_SCHEDULER;


static MSD_STATUS atsBasicWrite
(
    IN MSD_QD_DEV* dev,
    IN MSD_U8 atsRegAddr,
    IN MSD_U16 writeData
);
static MSD_STATUS atsBasicRead
(
    IN MSD_QD_DEV* dev,
    IN MSD_U8 atsRegAddr,
    IN MSD_U16* readData
);
static MSD_STATUS atsWaitBusyBitSelfClear
(
    IN MSD_QD_DEV* dev
);
static MSD_STATUS atsOperationPerform
(
    IN    MSD_QD_DEV*           dev,
    IN    FIR_MSD_ATS_OPERATION atsOp,
    IN    MSD_U16               atsSchedId
);
static MSD_STATUS atsSetGrpId
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_U16       atsSchedGrpId
);
static MSD_STATUS atsGetGrpId
(
    IN    MSD_QD_DEV*    dev,
    OUT   MSD_U16*       atsSchedGrpId
);
static MSD_STATUS atsSetBktDec
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_U16       atsSchedBktDec
);
static MSD_STATUS atsGetBktDec
(
    IN    MSD_QD_DEV*    dev,
    OUT   MSD_U16*       atsSchedBktDec
);
static MSD_STATUS atsSetBrf
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U16       atsSchedBrf
);
static MSD_STATUS atsGetBrf
(
    IN    MSD_QD_DEV*   dev,
    OUT   MSD_U16*      atsSchedBrf
);
static MSD_STATUS atsSetCbsLimit
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U32       atsSchedCbsLimit
);
static MSD_STATUS atsGetCbsLimit
(
    IN    MSD_QD_DEV*   dev,
    OUT   MSD_U32*      atsSchedCbsLimit
);
static MSD_STATUS atsSetMaxResidenceTime
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U16       atsSchedMaxTimeScale,
    IN    MSD_U16       atsSchedMaxResidenceTime
);
static MSD_STATUS atsGetMaxResidenceTime
(
    IN    MSD_QD_DEV*   dev,
    OUT    MSD_U16*      atsSchedMaxTimeScale,
    OUT    MSD_U16*      atsSchedMaxResidenceTime
);

static MSD_STATUS atsSetEntry
(
    IN    MSD_QD_DEV*   dev,
    IN    FIR_MSD_ATS_SCHEDULER       entry
);
static MSD_STATUS atsGetEntry
(
    IN    MSD_QD_DEV*   dev,
    OUT    FIR_MSD_ATS_SCHEDULER*       entry
);


/******************************************************************************
* Fir global API
******************************************************************************/
/*******************************************************************************
* Fir_gatsFlushAll
*
* DESCRIPTION:
*       This routine flush all ATS Instances.
*
* INPUTS:
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsFlushAll
(
    IN  MSD_QD_DEV* const   dev
)
{
    MSD_STATUS retVal;
    FIR_MSD_ATS_OPERATION atsOp = FLUSH_ALL;
    retVal = atsOperationPerform(dev, atsOp, 0);
	return retVal;
}
/*******************************************************************************
* Fir_gatsFlushOne
*
* DESCRIPTION:
*       This routine flush one ATS Instance.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsFlushOne
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16    atsEntryId
)
{
    MSD_STATUS retVal = MSD_OK;;
	if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        FIR_MSD_ATS_OPERATION atsOp = FLUSH_ONE;
        retVal = atsOperationPerform(dev, atsOp, atsEntryId);
    }
	return retVal;
}
/******************************************************************************
* Fir_gatsSetGrpId
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       atsEntryId      - The Entry Id.
*       atsEntryGrpId   - The Entry GroupId.
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
MSD_STATUS Fir_gatsSetGrpId
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
IN  MSD_U16             atsEntryGrpId
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);

        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        if (retVal != MSD_OK)
        {
		    msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
        else 
        {
            atsEntry.atsSchedGrpID = atsEntryGrpId;
            retVal = atsSetEntry(dev, atsEntry);
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
    }
    return retVal;
}
/*******************************************************************************
* Fir_gatsGetGrpId
*
* DESCRIPTION:
*       This routine get ATS Instance Group Id.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       atsEntryGrpId   - ATS Scheduler Group Id
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetGrpId
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
OUT  MSD_U16*   const   atsEntryGrpId
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
        else
        {
            *atsEntryGrpId = atsEntry.atsSchedGrpID;
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        } 
    }
    return retVal;
}
/*******************************************************************************
* Fir_gatsSetCbsLimit
*
* DESCRIPTION:
*       This routine set ATS Instance CBS Limit.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*       atsCbsLimit   - ATS CBS Limit
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetCbsLimit
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
IN  MSD_U32             atsCbsLimit
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
        else
        {
            atsEntry.cbsLimit = atsCbsLimit;
            retVal = atsSetEntry(dev, atsEntry);
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }    
    }
    return retVal;
}
/*******************************************************************************
* Fir_gatsGetCbsLimit
*
* DESCRIPTION:
*       This routine get ATS Instance CBS Limit.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id

* OUTPUTS:
*       atsCbsLimit   - ATS CBS Limit
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetCbsLimit
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
OUT MSD_U32*    const   atsCbsLimit
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);

        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->qcrRegsSem);  
        }
        else
        {
            *atsCbsLimit = atsEntry.cbsLimit;
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }   
    } 
    return MSD_OK;
}
/*******************************************************************************
* Fir_gatsSetRateLimit
*
* DESCRIPTION:
*       This routine set ATS Instance Rate Limit. The value can be matched to
*    a pair of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*       Due to the limitation of precision, the real rate limit value may not
*    be equal to the target rate limit value, but it must be the closest to
*    the target rate limit value. If you want to get the real rate limit, you
*    can use the function: Fir_gatsGetRateLimit.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*       atsRateLimit  - ATS Target Rate Limit
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetRateLimit
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
IN  MSD_U32             rateLimit
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U16 bucketRateFactor = 0;
    MSD_U64 d_bktdec = 0;
    MSD_U16 bktdec = 0;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        if ((rateLimit < (MSD_U32)8) || (rateLimit >(MSD_U32)10000000))
	    {
            retVal = MSD_BAD_PARAM;
	    }
        else
        {
            for (bucketRateFactor = (MSD_U16)15; ; bucketRateFactor--)
            {
                /*Formular: d_bktdec = (MSD_DOUBLE)(((MSD_U64)8000000 * (1 << bucketRateFactor)) / (rateLimit * TIME_INTERVAL)); */
                d_bktdec = (MSD_U64)(TIME_FACTOR * ((MSD_U64)1 << bucketRateFactor));
                d_bktdec = (d_bktdec * 10) / (MSD_U64)rateLimit;
                if (d_bktdec >= (MSD_U64)655350 || d_bktdec <= (MSD_U64)0)
                {
                    if (bucketRateFactor == (MSD_U16)0)
                    {
                        return MSD_BAD_PARAM;
                    }
                    continue;
                }
                if ((d_bktdec % 10) >= 5)
                {
                    bktdec = (MSD_U16)(d_bktdec / (MSD_U64)10 + (MSD_U64)1);
                }
                else
                {
                    bktdec = (MSD_U16)(d_bktdec / (MSD_U64)10);
                }
                break;
            }
            msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
            atsEntry.atsSchedID = atsEntryId;
            retVal = atsGetEntry(dev, &atsEntry);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->qcrRegsSem); 
            }
            else
            {
                atsEntry.brf = bucketRateFactor;
                atsEntry.bucketDec = bktdec;
                retVal = atsSetEntry(dev, atsEntry);
                msdSemGive(dev->devNum, dev->qcrRegsSem);
            }
        }
    }
    return retVal;
}
/*******************************************************************************
* Fir_gatsGetRateLimit
*
* DESCRIPTION:
*       This routine get ATS Instance Rate Limit. The value can be matched to a pair
*    of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id

* OUTPUTS:
*       atsRateLimit   - ATS Rate Limit
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetRateLimit
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
OUT MSD_U32*    const   rateLimit
)
{
    FIR_MSD_ATS_SCHEDULER atsEntry;
    MSD_STATUS retVal = MSD_OK;
    MSD_U16 bktdec = 0;
    MSD_U16 bucketRateFactor = 0;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
        else
        {
            bucketRateFactor = atsEntry.brf;
            bktdec = atsEntry.bucketDec;
	        if (bktdec == (MSD_U16)0)
            {
                *rateLimit = 0;
            }
            else
            {
                *rateLimit = (MSD_U32)(((MSD_U64)TIME_FACTOR * ((MSD_U64)1 << bucketRateFactor)) / (MSD_U64)bktdec);
            }
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }  
    }  
    return MSD_OK;
}
/*******************************************************************************
* Fir_gatsSetMaxResidenceTime
*
* DESCRIPTION:
*       This routine set ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*       maxTimeScale   - max Time Scale
*       maxResidenceTime - max Residence Time
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetMaxResidenceTime
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16     atsEntryId,
IN  MSD_U16     maxTimeScale,
IN  MSD_U16     maxResidenceTime
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }
        else
        {
            atsEntry.maxTimeScale = maxTimeScale;
            atsEntry.maxResidenceTime = maxResidenceTime;
            retVal = atsSetEntry(dev, atsEntry);
            msdSemGive(dev->devNum, dev->qcrRegsSem);
        }   
    }
    return retVal;

}
/*******************************************************************************
* Fir_gatsGetMaxResidenceTime
*
* DESCRIPTION:
*       This routine get ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       maxTimeScale   - max Time Scale
*       maxResidenceTime - max Residence Time
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetMaxResidenceTime
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             atsEntryId,
OUT MSD_U16*    const   maxTimeScale,
OUT MSD_U16*    const   maxResidenceTime
)
{
    MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_SCHEDULER atsEntry;
    if (atsEntryId >= (MSD_U16)16)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);

        atsEntry.atsSchedID = atsEntryId;
        retVal = atsGetEntry(dev, &atsEntry);

        msdSemGive(dev->devNum, dev->qcrRegsSem);

        *maxTimeScale = atsEntry.maxTimeScale;
        *maxResidenceTime = atsEntry.maxResidenceTime; 
    }
    return retVal;

}

/*******************************************************************************
* Fir_gatsSetQueueEnables
*
* DESCRIPTION:
*       This routine enable ATS Transmission Selection Algorithm in Queue.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*       status  - 0: disable; 1: enable.
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetQueueEnables
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             port,
IN  MSD_U16             queue,
IN  MSD_BOOL            status
)
{
    MSD_STATUS retVal = MSD_OK;
	MSD_U8 pointer = (MSD_U8)0x07;
    MSD_U8 data = ((MSD_U8)status) << queue;
    if (port >= (MSD_U16)10 || queue > (MSD_U16)7)
    {
        retVal = MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        retVal = Fir_gprtSetQueueCtrl(dev, port, pointer, data);
        msdSemGive(dev->devNum, dev->qcrRegsSem);
    }
    return retVal;
}

/*******************************************************************************
* Fir_gatsGetQueueEnables
*
* DESCRIPTION:
*       This routine check if ATS Transmission Selection Algorithm in Queue is enabled.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*
* OUTPUTS:
*       status  - True: enabled; False: disabled
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetQueueEnables
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             port,
IN  MSD_U16             queue,
OUT MSD_BOOL*           status
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U8 pointer = 0x07;
    MSD_U8 data = 0;
    if (port >= (MSD_U16)10 || queue > (MSD_U16)7)
    {
        return MSD_BAD_PARAM;
    }
    else
    {
        msdSemTake(dev->devNum, dev->qcrRegsSem, OS_WAIT_FOREVER);
        retVal = Fir_gprtGetQueueCtrl(dev, port, pointer, &data);
        *status = (MSD_BOOL)((data >> queue)&0x1);
        msdSemGive(dev->devNum, dev->qcrRegsSem);
    }
    return retVal;
}
/******************************************************************************
* Fir static API
******************************************************************************/
static MSD_STATUS atsBasicWrite
(
    IN MSD_QD_DEV* dev,
    IN MSD_U8 atsRegAddr,
    IN MSD_U16 writeData
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U16 busyBit = 1;
    MSD_U16 avbOp = 3;
    MSD_U16 avbPort = 4;
    MSD_U16 avbBlock = 4;

    msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        /*wait busyBit Clear*/
        while (busyBit != 0)
        {
            retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &busyBit);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }
        /*set TSN data*/
        retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_DATA, writeData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
        /*set TSN command*/
        retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (atsRegAddr & 0x1F)));
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

        /*Set AVB writeData*/
        retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, writeData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

        /*Set AVB Command*/
	    retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (atsRegAddr & 0x1F)));
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
    }
    
    msdSemGive(dev->devNum, dev->ptpRegsSem);
	return retVal;
}

static MSD_STATUS atsBasicRead
(
    IN MSD_QD_DEV* dev,
    IN MSD_U8 atsRegAddr,
    IN MSD_U16* readData
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U16 busyBit = 1;
    MSD_U16 avbOp = 0;
    MSD_U16 avbPort = 4;
    MSD_U16 avbBlock = 4;

    msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        /*wait busyBit Clear*/
        while (busyBit != 0)
        {
            retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &busyBit);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }
        /*Set TSN Command*/
        retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (atsRegAddr & 0x1F)));
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

        /*wait busyBit Clear*/
        busyBit = 1;
        while (busyBit != 0)
        {
            retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &busyBit);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }
        /*Get TSN Data*/
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

        /*Set AVB Command*/
        retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (0x8000 | (avbOp << 13) | (avbPort << 8) | (avbBlock << 5) | (atsRegAddr & 0x1F)));
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
	return retVal;
}

static MSD_STATUS atsWaitBusyBitSelfClear
(
    IN MSD_QD_DEV* dev
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = 0;
    MSD_32 count = 0;
    MSD_U16 atsData = 0;
    while (count < 100)
    {
        count++;
        retVal = atsBasicRead(dev, atsRegAddr, &atsData);
        if (retVal != MSD_OK)
        {
            break;
        }
		else if ((atsData & (MSD_U16)0x8000) == (MSD_U16)0)
        {
            retVal = MSD_OK;
            break;
        }
        else if (count >= 10)
        {
			retVal = MSD_BUSY;
            break;
        }
    }
	return retVal;
}

static MSD_STATUS atsOperationPerform
(
    IN    MSD_QD_DEV*           dev,
    IN    FIR_MSD_ATS_OPERATION atsOp,
    IN    MSD_U16               atsSchedId
)
{
    MSD_STATUS retVal;
    MSD_U8 atsRegAddr = 0;
    MSD_U16 opData = 0;
    retVal = atsWaitBusyBitSelfClear(dev);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    
	/* ats enable */
	atsRegAddr = 1;
	retVal = atsBasicRead(dev, atsRegAddr, &opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_atsBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	opData = opData | 0x8000;
	retVal = atsBasicWrite(dev, atsRegAddr, opData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_atsBasicRead returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* ats operation */
	atsRegAddr = 0;
    opData = (MSD_U16)0x8000 | (((MSD_U16)atsOp & (MSD_U16)0x7) << 12) | (atsSchedId & (MSD_U16)0xF);
    retVal = atsBasicWrite(dev, atsRegAddr, opData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsWaitBusyBitSelfClear(dev);
	return retVal;
}

static MSD_STATUS atsSetGrpId
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_U16       atsSchedGrpId
)
{
    MSD_STATUS retVal;
	MSD_U8 atsRegAddr = (MSD_U8)1;
    MSD_U16 atsData;
    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    atsData = (atsData & 0xFFF0) | (atsSchedGrpId & 0xF);
    retVal = atsBasicWrite(dev, atsRegAddr, atsData);
	return retVal;
}

static MSD_STATUS atsGetGrpId
(
    IN    MSD_QD_DEV*    dev,
    OUT   MSD_U16*       atsSchedGrpId
)
{
    MSD_STATUS retVal;
	MSD_U8 atsRegAddr = (MSD_U8)1;

    retVal = atsBasicRead(dev, atsRegAddr, atsSchedGrpId);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *atsSchedGrpId = *atsSchedGrpId & (MSD_U16)0xF;
	return retVal;
}

static MSD_STATUS atsSetBktDec
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_U16       atsSchedBktDec
)
{
    MSD_STATUS retVal;
	MSD_U8 atsRegAddr = (MSD_U8)2;

    retVal = atsBasicWrite(dev, atsRegAddr, atsSchedBktDec);
    return retVal;

}

static MSD_STATUS atsGetBktDec
(
    IN    MSD_QD_DEV*    dev,
    OUT   MSD_U16*       atsSchedBktDec
)
{
    MSD_STATUS retVal;
	MSD_U8 atsRegAddr = (MSD_U8)2;

    retVal = atsBasicRead(dev, atsRegAddr, atsSchedBktDec);
    return retVal;
}

static MSD_STATUS atsSetBrf
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U16       atsSchedBrf
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = (MSD_U8)3;
    MSD_U16 atsData;
    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    atsData = (atsData & (MSD_U16)0xFFF0) | (atsSchedBrf & (MSD_U16)0xF);
    retVal = atsBasicWrite(dev, atsRegAddr, atsData);

    return retVal;

}

static MSD_STATUS atsGetBrf
(
    IN    MSD_QD_DEV*   dev,
    OUT   MSD_U16*      atsSchedBrf
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = (MSD_U8)3;
    MSD_U16 atsData;
    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *atsSchedBrf = (MSD_U8)(atsData & (MSD_U16)0xF);
	return retVal;
}

static MSD_STATUS atsSetCbsLimit
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U32       atsSchedCbsLimit
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = (MSD_U8)4;
    MSD_U16 atsData = (MSD_U16)(atsSchedCbsLimit & (MSD_U32)0xFFFF);

    retVal = atsBasicWrite(dev, atsRegAddr, atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    atsRegAddr = (MSD_U8)5;
    atsData = (MSD_U16)((atsSchedCbsLimit & (MSD_U32)0xFF0000) >> 16);
    retVal = atsBasicWrite(dev, atsRegAddr, atsData);

    return retVal;

}

static MSD_STATUS atsGetCbsLimit
(
    IN    MSD_QD_DEV*   dev,
    OUT   MSD_U32*      atsSchedCbsLimit
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = (MSD_U8)4;
    MSD_U16 atsData = 0;
    *atsSchedCbsLimit = 0;
    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *atsSchedCbsLimit |= (MSD_U32)atsData & (MSD_U32)0xFFFF;

    atsRegAddr = (MSD_U8)5;
    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *atsSchedCbsLimit |= ((MSD_U32)atsData & (MSD_U32)0xFF) << 16;
	return retVal;
}

static MSD_STATUS atsSetMaxResidenceTime
(
    IN    MSD_QD_DEV*   dev,
    IN    MSD_U16       atsSchedMaxTimeScale,
    IN    MSD_U16       atsSchedMaxResidenceTime
)
{
    MSD_STATUS retVal;
    MSD_U8 atsRegAddr = (MSD_U8)6;
    MSD_U16 atsData = ((MSD_U16)atsSchedMaxResidenceTime & (MSD_U16)0xFFF) | (((MSD_U16)atsSchedMaxTimeScale & (MSD_U16)0x3) << 12);

    retVal = atsBasicWrite(dev, atsRegAddr, atsData);
    return retVal;

}

static MSD_STATUS atsGetMaxResidenceTime
(
    IN    MSD_QD_DEV*   dev,
    OUT    MSD_U16*      atsSchedMaxTimeScale,
    OUT    MSD_U16*      atsSchedMaxResidenceTime
)
{
	MSD_STATUS retVal = MSD_OK;
    MSD_U8 atsRegAddr = (MSD_U8)6;
    MSD_U16 atsData = 0;

    retVal = atsBasicRead(dev, atsRegAddr, &atsData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *atsSchedMaxTimeScale = (atsData & (MSD_U16)0x3000) >> 12;
    *atsSchedMaxResidenceTime = atsData & (MSD_U16)0xFFF;
	return retVal;
}

static MSD_STATUS atsSetEntry
(
    IN    MSD_QD_DEV*   dev,
    IN    FIR_MSD_ATS_SCHEDULER       entry
)
{
    MSD_STATUS retVal;
    FIR_MSD_ATS_OPERATION atsOp = LOAD;
    if (entry.atsSchedID >= (MSD_U16)16)
    {
        return MSD_BAD_PARAM;
    }
    retVal = atsSetGrpId(dev, entry.atsSchedGrpID);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsSetBktDec(dev, entry.bucketDec);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsSetBrf(dev, entry.brf);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsSetCbsLimit(dev, entry.cbsLimit);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsSetMaxResidenceTime(dev, entry.maxTimeScale, entry.maxResidenceTime);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    
    retVal = atsOperationPerform(dev, atsOp, entry.atsSchedID);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    atsOp = NOP;
    retVal = atsOperationPerform(dev, atsOp, entry.atsSchedID);

    return retVal;

}


static MSD_STATUS atsGetEntry
(
    IN    MSD_QD_DEV*   dev,
    OUT   FIR_MSD_ATS_SCHEDULER*       entry
)
{
	MSD_STATUS retVal = MSD_OK;
    FIR_MSD_ATS_OPERATION atsOperation = READ;
    if (entry->atsSchedID >= (MSD_U16)16)
    {
        return MSD_BAD_PARAM;
    }
    retVal = atsOperationPerform(dev, atsOperation, entry->atsSchedID);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsGetGrpId(dev, &(entry->atsSchedGrpID));
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsGetBktDec(dev, &(entry->bucketDec));
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsGetBrf(dev, &(entry->brf));
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsGetCbsLimit(dev, &(entry->cbsLimit));
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    retVal = atsGetMaxResidenceTime(dev, &(entry->maxTimeScale), &(entry->maxResidenceTime));
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    atsOperation = NOP;
    retVal = atsOperationPerform(dev, atsOperation, entry->atsSchedID);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

	return retVal;
}
