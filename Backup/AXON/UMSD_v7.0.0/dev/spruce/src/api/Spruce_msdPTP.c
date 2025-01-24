/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Spruce_msdPTP.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <spruce/include/api/Spruce_msdPTP.h>
#include <spruce/include/driver/Spruce_msdHwAccess.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

typedef enum
{
    SPRUCE_MSD_AVB_BLOCK_PTP = 0x0,
    SPRUCE_MSD_AVB_BLOCK_AVB_POLICY = 0x1,
    SPRUCE_MSD_AVB_BLOCK_QAV = 0x2,
    SPRUCE_MSD_AVB_BLOCK_QBV = 0x3
} SPRUCE_MSD_AVB_BLOCK;

/*
* Typedef: enum SPRUCE_MSD_PTP_OPERATION
*
* Description: Defines the PTP (Precise Time Protocol) Operation type
*
* Fields:
*      SPRUCE_PTP_WRITE_DATA             - Write data to the PTP register
*      SPRUCE_PTP_READ_DATA            - Read data from PTP register
*      SPRUCE_PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
*      SPRUCE_PTP_READ_TIMESTAMP_DATA    - Read timestamp data from PTP register
*                    valid bit will be reset after read
*/
typedef enum
{
    SPRUCE_PTP_WRITE_DATA = 0x3,
    SPRUCE_PTP_READ_DATA = 0x4,
    SPRUCE_PTP_READ_MULTIPLE_DATA = 0x6,
    SPRUCE_PTP_READ_TIMESTAMP_DATA = 0x8,
} SPRUCE_MSD_PTP_OPERATION;

/*
*  typedef: struct SPRUCE_MSD_PTP_OP_DATA
*
*  Description: data required by PTP Operation
*
*  Fields:
*      ptpPort        - physical port of the device
*      ptpBlock    - AVB Block
*      ptpAddr     - register address
*      ptpData     - data for ptp register.
*      ptpMultiData- used for multiple read operation.
*      nData         - number of data to be read on multiple read operation.
*/
typedef struct
{
    MSD_U32    ptpPort;
    MSD_U32    ptpBlock;
    MSD_U32    ptpAddr;
    MSD_U32    ptpData;
    MSD_U32    ptpMultiData[MAX_PTP_CONSECUTIVE_READ];
    MSD_U32    nData;
} SPRUCE_MSD_PTP_OP_DATA;


/********************************************************/
/*     static function declaration                      */
/********************************************************/
static MSD_STATUS ptpOperationPerform_MultiChip
(
    IN    MSD_QD_DEV             *dev,
    IN    SPRUCE_MSD_PTP_OPERATION      ptpOp,
    INOUT SPRUCE_MSD_PTP_OP_DATA        *opData
);

static MSD_STATUS ptpOperationPerform
(
    IN    MSD_QD_DEV             *dev,
    IN    SPRUCE_MSD_PTP_OPERATION      ptpOp,
    INOUT SPRUCE_MSD_PTP_OP_DATA        *opData
);

static MSD_STATUS gptpSetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U32        regOffset,
    IN  MSD_U32        data
);

static MSD_STATUS gptpGetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U32        regOffset,
    OUT MSD_U32        *data
);

static MSD_STATUS gptpGlobalPointDataSet
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 point,
    IN  MSD_U8 data
);

static MSD_STATUS gptpGlobalPointDataGet
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 point,
    OUT MSD_U8 *data
);

static MSD_STATUS gptpTimeArrayStore
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 timeArrayIndex,
    IN  SPRUCE_PTP_TOD_STORE_OPERATION storeOp,
    IN  SPRUCE_PTP_TIME_ARRAY *timeArray
);

static MSD_STATUS gptpTimeArrayCapture
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 timeArrayIndex,
    OUT SPRUCE_PTP_TIME_ARRAY       *timeArray
);

/************ PTP module read & write   ***************/
static MSD_STATUS gptpPortRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

static MSD_STATUS gptpPortRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

static MSD_STATUS gptpPortRegFieldGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    OUT  MSD_U16    *data
);

static MSD_STATUS gptpPortRegFieldSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    IN  MSD_U16     data
);

static MSD_STATUS gptpGlobalRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

static MSD_STATUS gptpGlobalRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

/*
static MSD_STATUS gptpGlobalPointFieldGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U8      point,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    OUT MSD_U8      *data
);
*/

static MSD_STATUS gptpGlobalPointFieldSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U8      point,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    IN  MSD_U8      data
);


/********************************************************/
/*     public function declaration                      */
/********************************************************/
MSD_STATUS Spruce_gptpSetIntEnable
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN SPRUCE_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Spruce_gptpSetIntEnable Called.\n"));

    retval = gptpGetReg(dev, port, (MSD_U32)0x2, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    data = data & 0xFFFFFFFCU;   /* data = data & ~(MSD_U32)3; */
    data = data | ((interruptEn->ptpArrIntEn == MSD_TRUE) ? (MSD_U32)1 : (MSD_U32)0) | (MSD_U32)(((interruptEn->ptpDepIntEn == MSD_TRUE) ? (MSD_U32)1 : (MSD_U32)0) << 1);
    retval = gptpSetReg(dev, port, (MSD_U32)0x02, data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retval;
    }

    MSD_DBG_INFO(("Spruce_gptpSetIntEnable Exit.\n"));
    return retval;
}

MSD_STATUS Spruce_gptpGetIntEnable
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT SPRUCE_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;
    MSD_U32 temp = 0;

    MSD_DBG_INFO(("Spruce_gptpGetIntEnable Called.\n"));

    retval = gptpGetReg(dev, port, (MSD_U32)0x2, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    temp = data & (MSD_U32)0x1;
    if (temp == (MSD_U32)1)
    {
        interruptEn->ptpArrIntEn = MSD_TRUE;
    }
    else
    {
        interruptEn->ptpArrIntEn = MSD_FALSE;
    }

    temp =(data & (MSD_U32)0x02) >> 1;
    if (temp == (MSD_U32)1)
    {
        interruptEn->ptpDepIntEn = MSD_TRUE;
    }
    else
    {
        interruptEn->ptpDepIntEn = MSD_FALSE;
    }

    MSD_DBG_INFO(("Spruce_gptpGetIntEnable Exit.\n"));
    return retval;
}

MSD_STATUS Spruce_gptpSetPortPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_BOOL  en
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Spruce_gptpSetProtPTPEn Called.\n"));

    retval = gptpGetReg(dev, port, 0x0, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    if (en == MSD_TRUE)
    {
        data &= 0xFFFFFFFEU;  /* data &= ~(MSD_U32)0x1; */
    }
        
    else
    {
        data |= (MSD_U32)0x1;
    }
        

    retval = gptpSetReg(dev, port, 0x0, data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Set the PTP register error\n"));
        return retval;
    }

    MSD_DBG_INFO(("Spruce_gptpSetProtPTPEn Exit.\n"));
    return retval;
}

MSD_STATUS Spruce_gptpGetPortPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_BOOL  *en
)
{
    MSD_STATUS  retval;
    MSD_U32 data = 0;

    MSD_DBG_INFO(("Spruce_gptpGetProtPTPEn Called.\n"));

    retval = gptpGetReg(dev, port, 0x0, &data);
    if (retval != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retval;
    }

    *en = ((data & (MSD_U32)0x1) == (MSD_U32)1) ? MSD_FALSE : MSD_TRUE;

    MSD_DBG_INFO(("Spruce_gptpGetProtPTPEn Exit.\n"));
    return retval;
}


MSD_STATUS Spruce_gptpGetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  SPRUCE_PTP_TIME    timeToRead,
    OUT SPRUCE_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    MSD_U8 hwPort;
    SPRUCE_MSD_PTP_OPERATION op;
    SPRUCE_MSD_PTP_OP_DATA opData;
    MSD_U32 baseReg;
    MSD_U32 temp;

    MSD_DBG_INFO(("Spruce_gptpGetTimeStamp Called.\n"));
    if (port == 0U)
    {
        MSD_DBG_ERROR(("Failed, Port 0 do not support PTP.\n"));
        return MSD_BAD_PARAM;
    }
    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
        return MSD_BAD_PARAM;
    }

    baseReg = (MSD_U32)8;

    switch (timeToRead)
    {
        case SPRUCE_PTP_ARR0_TIME:
            opData.ptpAddr = baseReg + (MSD_U32)0;
            break;
        case SPRUCE_PTP_ARR1_TIME:
            opData.ptpAddr = baseReg + (MSD_U32)4;
            break;
        case SPRUCE_PTP_DEP_TIME:
            opData.ptpAddr = baseReg + (MSD_U32)8;
            break;
        default:
            MSD_DBG_ERROR(("Invalid time to be read\n"));
            return MSD_BAD_PARAM;
    }

    opData.ptpPort = hwPort;
    opData.ptpBlock = (MSD_U32)SPRUCE_MSD_AVB_BLOCK_PTP;


    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed ptpOperation.\n"));
        return retVal;
    }

    ptpStatus->isValid = ((opData.ptpMultiData[0] & (MSD_U32)0x1) == (MSD_U32)1) ? MSD_TRUE : MSD_FALSE;
    temp = (opData.ptpMultiData[0] >> 1) & (MSD_U32)0x3;
    if (temp == (MSD_U32)0)
    {
        ptpStatus->status = MSD_PTP_INT_NORMAL;
    }
    else if (temp == (MSD_U32)1)
    {
        ptpStatus->status = MSD_PTP_INT_OVERWRITE;
    }
    else if (temp == (MSD_U32)2)
    {
        ptpStatus->status = MSD_PTP_INT_DROP;
    }
    else
    {
        MSD_DBG_ERROR(("Invalid .MSD_PTP_INT_STATUS\n"));
        return MSD_FAIL;
    }
    ptpStatus->timeStamped = SPRUCEMSD_PTP_BUILD_TIME(opData.ptpMultiData[2], opData.ptpMultiData[1]);
    ptpStatus->ptpSeqId = opData.ptpMultiData[3];

    MSD_DBG_INFO(("Spruce_gptpGetTimeStamp Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetTSValidSt
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
)
{
    MSD_STATUS  retVal;
    MSD_U32 data = 0;
    MSD_U32 baseReg;
    MSD_DBG_INFO(("Spruce_gptpGetTSValidSt Called.\n"));

    baseReg = (MSD_U32)8;

    switch (timeToRead)
    {
        case MSD_PTP_ARR0_TIME:
            baseReg = baseReg + (MSD_U32)0;
            break;
        case MSD_PTP_ARR1_TIME:
            baseReg = baseReg + (MSD_U32)4;
            break;
        case MSD_PTP_DEP_TIME:
            baseReg = baseReg + (MSD_U32)8;
            break;
        default:
            MSD_DBG_ERROR(("Invalid time to get valid\n"));
            return MSD_BAD_PARAM;
    }

    retVal = gptpGetReg(dev, port, baseReg, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP register error\n"));
        return retVal;
    }

    *isValid = ((data & (MSD_U32)0x1) == (MSD_U32)0x1) ? MSD_TRUE : MSD_FALSE;
    MSD_DBG_INFO(("Spruce_gptpGetTSValidSt Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpResetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToReset
)
{
    MSD_STATUS  retVal;
    MSD_U32 baseReg;
    MSD_U32 index;
    MSD_DBG_INFO(("Spruce_gptpResetTimeStamp Called.\n"));

    switch (timeToReset)
    {
        case MSD_PTP_ARR0_TIME:
            baseReg = (MSD_U32)8;
            break;
        case MSD_PTP_ARR1_TIME:
            baseReg = (MSD_U32)12;
            break;
        case MSD_PTP_DEP_TIME:
            baseReg = (MSD_U32)16;
            break;
        default:
            MSD_DBG_ERROR(("Invalid time to reset\n"));
            return MSD_BAD_PARAM;
    }

    for (index = 0; index < (MSD_U32)4; index++)
    {
        MSD_U32 tempAddr = baseReg + index;
        retVal = gptpSetReg(dev, port, tempAddr, 0);
        if (retVal != MSD_OK)
        {
            MSD_DBG_ERROR(("Set the PTP register error\n"));
            return retVal;
        }
    }

    MSD_DBG_INFO(("Spruce_gptpResetTimeStamp Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetIntStatus
(
    IN  MSD_QD_DEV* dev,
    OUT SPRUCE_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    SPRUCE_MSD_PTP_OPERATION op;
    SPRUCE_MSD_PTP_OP_DATA opData;

    MSD_DBG_INFO(("Spruce_gptpGetIntStatus Called.\n"));

    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_DATA;
    opData.ptpAddr = 8;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading PTP status.\n"));
        return retVal;
    }

    ptpIntSt->ptpIntStPortVect = opData.ptpData & (((MSD_U32)1 << 12) - (MSD_U32)1);
    ptpIntSt->ptpTrigGenInt = (MSD_U8)(((MSD_U32)opData.ptpData >> 15) & (MSD_U32)1);
    ptpIntSt->ptpEventInt = (MSD_U8)(((MSD_U32)opData.ptpData >> 14) & (MSD_U32)1);

    /* read port 11 */
    opData.ptpAddr = 9;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading PTP status.\n"));
        return retVal;
    }

    ptpIntSt->ptpIntStPortVect = ptpIntSt->ptpIntStPortVect | ((opData.ptpData & (MSD_U32)0x1) << 11);

    MSD_DBG_INFO(("Spruce_gptpGetIntStatus Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpSetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    delay
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpSetMeanPathDelay Called.\n"));

    retVal = gptpSetReg(dev, portNum, (MSD_U32)0x1c, (MSD_U32)delay);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Writing mean path delay error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Spruce_gptpSetMeanPathDelay Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *delay
)
{
    MSD_STATUS retVal;
    MSD_U32 data;
    MSD_DBG_INFO(("Spruce_gptpGetMeanPathDelay Called.\n"));

    retVal = gptpGetReg(dev, portNum, (MSD_U32)0x1c, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Reading mean path delay error\n"));
        return retVal;
    }

    *delay = (MSD_U16)data;

    MSD_DBG_INFO(("Spruce_gptpGetMeanPathDelay Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpSetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpSetIgrPathDelayAsym Called.\n"));

    retVal = gptpSetReg(dev, portNum, (MSD_U32)0x1d, (MSD_U32)asym);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Writing ingress path delay asym error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Spruce_gptpSetIgrPathDelayAsym Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
)
{
    MSD_STATUS retVal;
    MSD_U32 data;
    MSD_DBG_INFO(("Spruce_gptpGetIgrPathDelayAsym Called.\n"));

    retVal = gptpGetReg(dev, portNum, (MSD_U32)0x1d, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Reading ingress path delay asym error\n"));
        return retVal;
    }

    *asym = (MSD_U16)data;

    MSD_DBG_INFO(("Spruce_gptpGetIgrPathDelayAsym Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpSetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpSetEgrPathDelayAsym Called.\n"));

    retVal = gptpSetReg(dev, portNum, (MSD_U32)0x1e, (MSD_U32)asym);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Writing egress path delay asym error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Spruce_gptpSetEgrPathDelayAsym Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
)
{
    MSD_STATUS retVal;
    MSD_U32 data;
    MSD_DBG_INFO(("Spruce_gptpGetEgrPathDelayAsym Called.\n"));

    retVal = gptpGetReg(dev, portNum, (MSD_U32)0x1e, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Reading egress path delay asym error\n"));
        return retVal;
    }

    *asym = (MSD_U16)data;

    MSD_DBG_INFO(("Spruce_gptpGetEgrPathDelayAsym Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetTODBusyBitStatus
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_BOOL    *busyStatus
)
{
    MSD_U32 tmpData;
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Spruce_gptpGetTODBusyBitStatus Called.\n"));

    if (busyStatus == NULL)
    {
        MSD_DBG_ERROR(("Input param busyStatus in Spruce_gptpGetTODBusyBitStatus is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = gptpGetReg(dev, (MSD_U32)0x1f, (MSD_U32)0x12, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get the PTP Tod Busy bit Failed.\n"));
        return retVal;
    }

    *busyStatus = ((tmpData & 0x8000U) == 0x8000U) ? MSD_TRUE : MSD_FALSE;

    MSD_DBG_INFO(("Spruce_gptpGetTODBusyBitStatus Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpTODStoreCompensation
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  SPRUCE_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_BOOL todBusy;

    MSD_DBG_INFO(("Spruce_gptpTODStoreCompensation Called.\n"));

    /*check the timeArrayIndex*/
    if (!(timeArrayIndex < (MSD_U8)2))
    {
        MSD_DBG_ERROR(("Time array index is over range.\n"));
        return MSD_BAD_PARAM;
    }

    /* wait TOD busy bit cleared */
    retVal = Spruce_gptpGetTODBusyBitStatus(dev, &todBusy);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    if (todBusy != MSD_FALSE)
    {
        return MSD_BUSY;
    }

    msdSemTake(dev->devNum, dev->ptpTodSem, OS_WAIT_FOREVER);

    retVal = gptpTimeArrayStore(dev, timeArrayIndex, SPRUCE_PTP_TOD_STORE_COMP, timeArray);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->ptpTodSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->ptpTodSem);
    MSD_DBG_INFO(("Spruce_gptpTODStoreCompensation Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpTODStoreAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  SPRUCE_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_BOOL todBusy;

    MSD_DBG_INFO(("Spruce_gptpTODStoreAll Called.\n"));

    /*check the timeArrayIndex*/
    if (!(timeArrayIndex < (MSD_U8)2))
    {
        MSD_DBG_ERROR(("Time array index is over range.\n"));
        return MSD_BAD_PARAM;
    }

    /* wait TOD busy bit cleared */
    retVal = Spruce_gptpGetTODBusyBitStatus(dev, &todBusy);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    if (todBusy != MSD_FALSE)
    {
        return MSD_BUSY;
    }

    msdSemTake(dev->devNum, dev->ptpTodSem, OS_WAIT_FOREVER);

    retVal = gptpTimeArrayStore(dev, timeArrayIndex, SPRUCE_PTP_TOD_STORE_ALL, timeArray);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->ptpTodSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->ptpTodSem);
    MSD_DBG_INFO(("Spruce_gptpTODStoreAll Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpTODCaptureAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT SPRUCE_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_BOOL todBusy;

    MSD_DBG_INFO(("Spruce_gptpTODCaptureAll Called.\n"));
    /*check the timeArrayIndex*/
    if (!(timeArrayIndex < (MSD_U8)2))
    {
        MSD_DBG_ERROR(("Time array index is over range.\n"));
        return MSD_BAD_PARAM;
    }

    /* wait TOD busy bit cleared */
    retVal = Spruce_gptpGetTODBusyBitStatus(dev, &todBusy);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    if (todBusy != MSD_FALSE)
    {
        return MSD_BUSY;
    }

    msdSemTake(dev->devNum, dev->ptpTodSem, OS_WAIT_FOREVER);

    retVal = gptpTimeArrayCapture(dev, timeArrayIndex, timeArray);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->ptpTodSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->ptpTodSem);
    MSD_DBG_INFO(("Spruce_gptpTODCaptureAll Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpSetPulse
(
    IN  MSD_QD_DEV* dev,
    IN  SPRUCE_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    MSD_U32 data;
    MSD_DBG_INFO(("Spruce_gptpSetPulse Called.\n"));

    data = (((MSD_U32)pulsePara->ptpPulseWidth & (MSD_U32)0xf) << 12) |
        (((MSD_U32)pulsePara->ptpAltTCAMTimeSel & (MSD_U32)0x1) << 11) |
        (((MSD_U32)pulsePara->ptp1ppsWidthRange & (MSD_U32)7) << 8) |
        (((MSD_U32)pulsePara->ptpTCAMTimeSel & (MSD_U32)0xf) << 4) |
        (((MSD_U32)pulsePara->ptp1ppsPhase & (MSD_U32)1) << 3) |
        ((MSD_U32)pulsePara->ptp1ppsSelect & (MSD_U32)0x3);
    retVal = gptpSetReg(dev, (MSD_U32)0x1f, (MSD_U32)0x1e, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Set 1PPS error\n"));
        return retVal;
    }

    MSD_DBG_INFO(("Spruce_gptpSetPulse Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGetPulse
(
    IN  MSD_QD_DEV* dev,
    OUT SPRUCE_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    MSD_U32 data;
    MSD_DBG_INFO(("Spruce_gptpGetPulse Called.\n"));

    retVal = gptpGetReg(dev, 0x1fU, 0x1eU, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Get 1PPS error\n"));
        return retVal;
    }

    pulsePara->ptpPulseWidth = (MSD_U8)((data >> 12) & (MSD_U32)0xF);
    pulsePara->ptpAltTCAMTimeSel = (MSD_U8)((data >> 11) & (MSD_U32)1);
    pulsePara->ptp1ppsWidthRange = (MSD_U8)((data >> 8) & (MSD_U32)7);
    pulsePara->ptpTCAMTimeSel = (MSD_U8)((data >> 4) & (MSD_U32)0xF);
    pulsePara->ptp1ppsPhase = (MSD_U8)((data >> 3) & (MSD_U32)1);
    pulsePara->ptp1ppsSelect = (MSD_U8)(data & (MSD_U32)0x3);

    MSD_DBG_INFO(("Spruce_gptpGetPulse Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gtaiGetPTPGlobalTime
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U32 *ptpTime
)
{
    MSD_STATUS retVal;
    SPRUCE_MSD_PTP_OP_DATA   opData;
    SPRUCE_MSD_PTP_OPERATION op;
    MSD_DBG_INFO(("Spruce_gtaiGetPTPGlobalTime Called.\n"));

    opData.ptpBlock = 0x0;    /* PTP register space */
    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.ptpPort = 0x1E; /* TAI Global */
    opData.ptpAddr = 14;
    opData.nData = 2;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed reading PTP Global Time.\n"));
        return MSD_FAIL;
    }

    *ptpTime = (opData.ptpMultiData[1] << 16) | opData.ptpMultiData[0];

    MSD_DBG_INFO(("Spruce_gtaiGetPTPGlobalTime Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpPortRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpPortRegGet Called.\n"));
    retVal = gptpPortRegGet(dev, portNum, regOffset, data);
    MSD_DBG_INFO(("Spruce_gptpPortRegGet Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpPortRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpPortRegSet Called.\n"));
    retVal = gptpPortRegSet(dev, portNum, regOffset, data);
    MSD_DBG_INFO(("Spruce_gptpPortRegSet Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGlobalRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpGlobalRegGet Called.\n"));
    retVal = gptpGlobalRegGet(dev, regOffset, data);
    MSD_DBG_INFO(("Spruce_gptpGlobalRegGet Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptpGlobalRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_DBG_INFO(("Spruce_gptpGlobalRegSet Called.\n"));
    retVal = gptpGlobalRegSet(dev, regOffset, data);
    MSD_DBG_INFO(("Spruce_gptpGlobalRegSet Exit.\n"));
    return retVal;
}

MSD_STATUS Spruce_gptp1588v2ConfigSet
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  SPRUCE_PTP_MODE  ptpMode,
    IN  SPRUCE_PTP_STEP  ptpStep
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0;
    MSD_U8 index = 0;
    MSD_U16 data = 0;
    MSD_U16 stepVal = 0;
    MSD_U8 highBit = 0;
    MSD_U8 lowBit = 0;

    MSD_DBG_INFO(("Spruce_gptp1588v2ConfigSet Called.\n"));

    /*  Set <DisPTP> to 0, enable PTP logic */
    regOffset = 0;
    data = 0;
    highBit = 0;
    lowBit = 0;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Set <TransSpec> to 0 */
    regOffset = 0;
    data = 0;
    highBit = (MSD_U8)15;
    lowBit = (MSD_U8)12;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* OneStep config */
    regOffset = 0;
    if(SPRUCE_PTP_ONE_STEP == ptpStep)
    {
        stepVal = (MSD_U16)3;
    }
    else if(SPRUCE_PTP_TWO_STEP == ptpStep)
    {
        stepVal  = 0;
    }
    else
    {
        return MSD_FAIL;
    }
    highBit = (MSD_U8)7;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, stepVal));

    /* PTP mode config */
    /* check time array index */
    if (!(timeArrayIndex <= (MSD_U8)1))
    {
        MSD_DBG_ERROR(("Invalid timeArrayIndex\n"));
        return MSD_BAD_PARAM;
    }
    if((MSD_U8)0 == timeArrayIndex)
    {
        index = 0x0;
        highBit = (MSD_U8)1;
        lowBit =(MSD_U8) 0;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointFieldSet(dev, index, highBit, lowBit, (MSD_U8)ptpMode));
    }
    else if((MSD_U8)1 == timeArrayIndex)
    {
        index = (MSD_U8)0x3;
        highBit = (MSD_U8)1;
        lowBit = (MSD_U8)0;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointFieldSet(dev, index, highBit, lowBit, (MSD_U8)ptpMode));
    }
    else
    {
        return MSD_FAIL;
    }

    /* PTP Hardware Acceleration config */
    regOffset =(MSD_U32) 0x2;
    data = (MSD_U16)1;
    highBit = (MSD_U8)6;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    MSD_DBG_INFO(("Spruce_gptp1588v2ConfigSet Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Spruce_gptp8021asConfigSet
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  SPRUCE_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0;
    MSD_U8 index = 0;
    MSD_U8 temp = 0;
    MSD_U16 data = 0;
    MSD_U8 highBit = 0;
    MSD_U8 lowBit = 0;

    MSD_DBG_INFO(("Spruce_gptp8021asConfigSet Called.\n"));

    /*  Set <DisPTP> to 0, enable PTP logic */
    regOffset = 0;
    data = 0;
    highBit = 0;
    lowBit = 0;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Set <TransSpec> to 1 */
    regOffset = 0;
    data = (MSD_U16)1;
    highBit = (MSD_U8)15;
    lowBit = (MSD_U8)12;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

   /* OneStep config to 0, two-step*/
    regOffset = 0;
    data = 0;
    highBit = (MSD_U8)7;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Set <OneStepSync> to 0 */
    regOffset = 0x0;
    data = 0;
    highBit = (MSD_U8)8;
    lowBit = (MSD_U8)8;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Relay mode config */
    /* check time array index */
    if (!(timeArrayIndex <= (MSD_U8)1))
    {
        MSD_DBG_ERROR(("Invalid timeArrayIndex\n"));
        return MSD_BAD_PARAM;
    }
    if((MSD_U8)0 == timeArrayIndex)
    {
        /* set domain 0 PTPMode to BC and relayMode to xxx */
        index = 0x0;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataGet(dev, index, &temp));
        /* ptpMode field to 0*/
        MSD_BF_SET(temp, (MSD_U8)SPRUCE_PTP_BC, (MSD_U8)0, (MSD_U8)2);
        /* relayMode field */
        MSD_BF_SET(temp, (MSD_U8)relayMode, (MSD_U8)3, (MSD_U8)1);
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataSet(dev, index, temp));
    }
    else if((MSD_U8)1 == timeArrayIndex)
    {
        /* set domain 1 PTPMode to BC and relayMode to xxx */
        index = (MSD_U8)0x3;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataGet(dev, index, &temp));
        /* ptpMode field to 0*/
        MSD_BF_SET(temp, (MSD_U8)SPRUCE_PTP_BC, (MSD_U8)0, (MSD_U8)2);
        /* relayMode field */
        MSD_BF_SET(temp, (MSD_U8)relayMode, (MSD_U8)2, (MSD_U8)1);
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataSet(dev, index, temp));
    }
    else
    {
        return MSD_FAIL;
    }

    /* PTP Hardware Acceleration config */
    regOffset = (MSD_U32)0x2;
    data = (MSD_U16)1;
    highBit = (MSD_U8)6;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    MSD_DBG_INFO(("Spruce_gptp8021asConfigSet Exit.\n"));   
    return MSD_OK;
}

MSD_STATUS Spruce_gptp8021asRevConfigSet
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  SPRUCE_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0;
    MSD_U8 index = 0;
    MSD_U8 temp = 0;
    MSD_U16 data = 0;
    MSD_U8 highBit = 0;
    MSD_U8 lowBit = 0;

    MSD_DBG_INFO(("Spruce_gptp8021asRevConfigSet Called.\n"));

    /*  Set <DisPTP> to 0, enable PTP logic */
    regOffset = 0;
    data = 0;
    highBit = 0;
    lowBit = 0;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Set <TransSpec> to 1 */
    regOffset = 0;
    data = (MSD_U16)1;
    highBit = (MSD_U8)15;
    lowBit = (MSD_U8)12;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

   /* OneStep config to 3, one-step*/
    regOffset = 0;
    data = (MSD_U16)3;
    highBit = (MSD_U8)7;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Set <OneStepSync> to 1 */
    regOffset = 0x0;
    data = (MSD_U16)1;
    highBit = (MSD_U8)8;
    lowBit = (MSD_U8)8;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    /* Relay mode config */
    /* check time array index */
    if (!(timeArrayIndex <= (MSD_U8)1))
    {
        MSD_DBG_ERROR(("Invalid timeArrayIndex\n"));
        return MSD_BAD_PARAM;
    }
    if((MSD_U8)0 == timeArrayIndex)
    {
        /* set domain 0 PTPMode to BC and relayMode to xxx */
        index = (MSD_U8)0x0;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataGet(dev, index, &temp));
        /* ptpMode field to 0*/
        MSD_BF_SET(temp, (MSD_U8)SPRUCE_PTP_BC, (MSD_U8)0, (MSD_U8)2);
        /* relayMode field */
        MSD_BF_SET(temp, (MSD_U8)relayMode, (MSD_U8)3, (MSD_U8)1);
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataSet(dev, index, temp));
    }
    else if((MSD_U8)1 == timeArrayIndex)
    {
        /* set domain 1 PTPMode to BC and relayMode to xxx */
        index = (MSD_U8)0x3;
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataGet(dev, index, &temp));
        /* ptpMode field to 0*/
        MSD_BF_SET(temp, (MSD_U8)SPRUCE_PTP_BC, (MSD_U8)0, (MSD_U8)2);
        /* relayMode field */
        MSD_BF_SET(temp, (MSD_U8)relayMode, (MSD_U8)2, (MSD_U8)1);
        SPRUCE_PTP_ATTEMPT(retVal, gptpGlobalPointDataSet(dev, index, temp));
    }
    else
    {
        return MSD_FAIL;
    }

    /* PTP Hardware Acceleration config */
    regOffset = (MSD_U32)0x2;
    data = (MSD_U16)1;
    highBit = (MSD_U8)6;
    lowBit = (MSD_U8)6;
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, data));

    MSD_DBG_INFO(("Spruce_gptp8021asRevConfigSet Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Spruce_gptpPreemptTsSelSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_BOOL    tsSelect
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0x0;
    MSD_U8 highBit = (MSD_U8)9;
    MSD_U8 lowBit = (MSD_U8)9;
    MSD_DBG_INFO(("Spruce_gptpPreemptTsSelSet Called.\n"));
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, (MSD_U16)tsSelect));
    MSD_DBG_INFO(("Spruce_gptpPreemptTsSelSet Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Spruce_gptpPreemptTsSelGet
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT   portNum,
OUT  MSD_BOOL   *tsSelect
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0x0;
    MSD_U16 data = 0;
    MSD_U8 highBit = (MSD_U8)9;
    MSD_U8 lowBit = (MSD_U8)9;
    MSD_DBG_INFO(("Spruce_gptpPreemptTsSelGet Called.\n"));
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldGet(dev, portNum, regOffset, highBit, lowBit, &data));
    *tsSelect = (MSD_BOOL)data;
    MSD_DBG_INFO(("Spruce_gptpPreemptTsSelGet Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Spruce_gptpPreemptCtrlSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      preemptCtrl
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0x0;
    MSD_U8 highBit = (MSD_U8)3;
    MSD_U8 lowBit = (MSD_U8)2;
    MSD_DBG_INFO(("Spruce_gptpPreemptCtrlSet Called.\n"));
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldSet(dev, portNum, regOffset, highBit, lowBit, (MSD_U16)preemptCtrl));
    MSD_DBG_INFO(("Spruce_gptpPreemptCtrlSet Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Spruce_gptpPreemptCtrlGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    OUT  MSD_U8     *preemptCtrl
)
{
    MSD_STATUS retVal;
    MSD_U32 regOffset = 0x0;
    MSD_U16 data = 0;
    MSD_U8 highBit = (MSD_U8)3;
    MSD_U8 lowBit = (MSD_U8)2;
    MSD_DBG_INFO(("Spruce_gptpPreemptCtrlGet Called.\n"));
    SPRUCE_PTP_ATTEMPT(retVal, gptpPortRegFieldGet(dev, portNum, regOffset, highBit, lowBit, &data));
    *preemptCtrl = (MSD_U8)data;
    MSD_DBG_INFO(("Spruce_gptpPreemptCtrlGet Exit.\n"));
    return MSD_OK;
}

/********************************************************/
/*     static function definition                       */
/********************************************************/
/******************************************************************************
* ptpOperationPerform
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       ptpOp      - The stats operation bits to be written into the stats
*                    operation register.
*
* OUTPUTS:
*       ptpData    - points to the data storage that the operation requires.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
static MSD_STATUS ptpOperationPerform_MultiChip
(
    IN    MSD_QD_DEV             *dev,
    IN    SPRUCE_MSD_PTP_OPERATION      ptpOp,
    INOUT SPRUCE_MSD_PTP_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32             i;
    MSD_U16          ptpOperation;
    MSD_U16 data;

    msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
    data = 1U;
    while (data == 1U)
    {
        retVal = Spruce_msdDirectGetMultiChipRegField(dev, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
    }
    /* Set the PTP Operation register */
    switch (ptpOp)
    {
        case SPRUCE_PTP_WRITE_DATA:
        {
            data = (MSD_U16)opData->ptpData;
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_DATA, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            ptpOperation = (MSD_U16)SPRUCE_PTP_WRITE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation |
                (MSD_U16)(opData->ptpPort << 8) |
                (MSD_U16)(opData->ptpBlock << 5) |
                (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_COMMAND, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }
        break;

        case SPRUCE_PTP_READ_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation |
                (MSD_U16)(opData->ptpPort << 8) |
                (MSD_U16)(opData->ptpBlock << 5) |
                (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_COMMAND, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while (data == 1U)
            {
                retVal = Spruce_msdDirectGetMultiChipRegField(dev, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = Spruce_msdDirectGetMultiChipReg(dev, SPRUCE_AVB_DATA, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpData = (MSD_U32)data;
        }
        break;

        case SPRUCE_PTP_READ_MULTIPLE_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation |
                (MSD_U16)(opData->ptpPort << 8) |
                (MSD_U16)(opData->ptpBlock << 5) |
                (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_COMMAND, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while (data == 1U)
            {
                retVal = Spruce_msdDirectGetMultiChipRegField(dev, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->ptpRegsSem);
                    return retVal;
                }
            }

            for (i = 0; i < opData->nData; i++)
            {
                retVal = Spruce_msdDirectGetMultiChipReg(dev, SPRUCE_AVB_DATA, &data);
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->ptpRegsSem);
                    return retVal;
                }
                opData->ptpMultiData[i] = (MSD_U32)data;
            }
        }
        break;

        case SPRUCE_PTP_READ_TIMESTAMP_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation |
                (MSD_U16)(opData->ptpPort << 8) |
                (MSD_U16)(opData->ptpBlock << 5) |
                (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_COMMAND, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while (data == 1U)
            {
                retVal = Spruce_msdDirectGetMultiChipRegField(dev, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = Spruce_msdDirectGetMultiChipReg(dev, SPRUCE_AVB_DATA, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpMultiData[0] = (MSD_U32)data;

            if ((data & 0x1U) != 1U)
            {
                /* valid bit is not set */
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            for (i = 1U; i < opData->nData; i++)
            {
                retVal = Spruce_msdDirectGetMultiChipReg(dev, SPRUCE_AVB_DATA, &data);
                opData->ptpMultiData[i] = (MSD_U32)data;
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_DATA, 0);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }

            ptpOperation = (MSD_U16)SPRUCE_PTP_WRITE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation |
                (MSD_U16)(opData->ptpPort << 8) |
                (MSD_U16)(opData->ptpBlock << 5) |
                (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = Spruce_msdDirectSetMultiChipReg(dev, SPRUCE_AVB_COMMAND, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }
        break;

        default:
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return MSD_FAIL;
        }
    }

    data = 1U;
    while (data == 1U)
    {
        retVal = Spruce_msdDirectGetMultiChipRegField(dev, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
    }
    msdSemGive(dev->devNum, dev->ptpRegsSem);
    return retVal;
}

static MSD_STATUS ptpOperationPerform
(
    IN    MSD_QD_DEV             *dev,
    IN    SPRUCE_MSD_PTP_OPERATION      ptpOp,
    INOUT SPRUCE_MSD_PTP_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32             i;
    MSD_U16          ptpOperation;
    MSD_U16 data;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return ptpOperationPerform_MultiChip(dev, ptpOp, opData);
    }

    msdSemTake(dev->devNum,dev->ptpRegsSem,OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
    data = 1U;
    while(data == 1U)
    {
        retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    /* Set the PTP Operation register */
    switch (ptpOp)
    {
        case SPRUCE_PTP_WRITE_DATA:
        {
            data = (MSD_U16)opData->ptpData;
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            ptpOperation = (MSD_U16)SPRUCE_PTP_WRITE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<=13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation | 
                    (MSD_U16)(opData->ptpPort << 8)    |
                    (MSD_U16)(opData->ptpBlock << 5)    |
                    (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
        }
        break;

        case SPRUCE_PTP_READ_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_DATA;
            ptpOperation &= 3U;
            ptpOperation <<= 13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation | 
                    (MSD_U16)(opData->ptpPort << 8)    |
                    (MSD_U16)(opData->ptpBlock << 5)    |
                    (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
                retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, &data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpData = (MSD_U32)data;
        }
        break;

        case SPRUCE_PTP_READ_MULTIPLE_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<=13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation | 
                    (MSD_U16)(opData->ptpPort << 8)    |
                    (MSD_U16)(opData->ptpBlock << 5)    |
                    (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
                retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            for(i=0; i<opData->nData; i++)
            {
                retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
                opData->ptpMultiData[i] = (MSD_U32)data;
            }
        }
        break;

        case SPRUCE_PTP_READ_TIMESTAMP_DATA:
        {
            ptpOperation = (MSD_U16)SPRUCE_PTP_READ_MULTIPLE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<=13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation | 
                    (MSD_U16)(opData->ptpPort << 8)    |
                    (MSD_U16)(opData->ptpBlock << 5)    |
                    (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
                retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, &data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpMultiData[0] = (MSD_U32)data;

            if ((data & 0x1U) != 1U)
            {
                /* valid bit is not set */
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            for(i=1U; i<opData->nData; i++)
            {
                retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, &data);
                opData->ptpMultiData[i] = (MSD_U32)data;
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_DATA, 0);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            ptpOperation = (MSD_U16)SPRUCE_PTP_WRITE_DATA;
            ptpOperation &= 3U;
            ptpOperation <<=13;
            data = (MSD_U16)((MSD_U16)1 << 15) | ptpOperation | 
                    (MSD_U16)(opData->ptpPort << 8)    |
                    (MSD_U16)(opData->ptpBlock << 5)    |
                    (MSD_U16)(opData->ptpAddr & 0x1FU);
            retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
        }
        break;

        default:
        {    
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return MSD_FAIL;
        }
    }

    data = 1U;
    while(data == 1U)
    {
        retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL2_DEV_ADDR, SPRUCE_AVB_COMMAND, 15U, 1U, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    msdSemGive(dev->devNum,dev->ptpRegsSem);
    return retVal;
}

/*******************************************************************************
* gptpSetReg
*
* DESCRIPTION:
*       This routine writes data to PTP register.
*
* INPUTS:
*       port         - logical port number
*       regOffset    - register to be written
*        data        - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM  - bad parameter
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static MSD_STATUS gptpSetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U32        regOffset,
    IN  MSD_U32        data
)
{
    MSD_STATUS           retVal;
    MSD_U32                hwPort;
    SPRUCE_MSD_PTP_OPERATION    op;
    SPRUCE_MSD_PTP_OP_DATA        opData;

    MSD_DBG_INFO(("gptpSetReg Called.\n"));

    /* translate logical port to physical port */
    if (port == 0x1FU)
    {
        hwPort = port;
    }
    else if (port == 0x0U)
    {
        MSD_DBG_ERROR(("Failed, Port 0 do not support PTP.\n"));
        return MSD_BAD_PARAM;
    }
    else
    {
        hwPort = MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
        {
            MSD_DBG_ERROR(("Failed (Bad Port).\n"));
            return MSD_BAD_PARAM;
        }
    }

    if ((regOffset > 0x1FU) || (data > 0xFFFFU))
    {
        MSD_DBG_ERROR(("Failed (Invalid reg offset/data).\n"));
        return MSD_BAD_PARAM;
    }

    op = SPRUCE_PTP_WRITE_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpData = data;
    opData.ptpBlock = (MSD_U32)SPRUCE_MSD_AVB_BLOCK_PTP;    /* PTP register space */

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed (ptpOperationPerform).\n"));
        return MSD_FAIL;
    }

    MSD_DBG_INFO(("OK.\n"));
    return MSD_OK;
}

/*******************************************************************************
* gptpGetReg
*
* DESCRIPTION:
*       This routine reads PTP register.
*
* INPUTS:
*       port         - logical port number.
*       regOffset    - register to read
*
* OUTPUTS:
*        data        - register data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM  - bad parameter
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static MSD_STATUS gptpGetReg
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT      port,
    IN  MSD_U32        regOffset,
    OUT MSD_U32        *data
)
{
    MSD_STATUS           retVal;
    MSD_U32              hwPort;
    SPRUCE_MSD_PTP_OPERATION    op;
    SPRUCE_MSD_PTP_OP_DATA      opData;

    /* translate logical port to physical port */
    if (port == 0x1FU)
    {
        hwPort = port;
    }
    else if (port == 0x0U)
    {
        MSD_DBG_ERROR(("Failed, Port 0 do not support PTP.\n"));
        return MSD_BAD_PARAM;
    }
    else
    {
        hwPort = MSD_LPORT_2_PORT(port);
        if (hwPort == MSD_INVALID_PORT)
        {
            MSD_DBG_ERROR(("Failed (Bad Port).\n"));
            return MSD_BAD_PARAM;
        }
    }

    if (regOffset > 0x1FU)
    {
        MSD_DBG_ERROR(("Failed (Invalid reg offset).\n"));
        return MSD_BAD_PARAM;
    }

    op = SPRUCE_PTP_READ_DATA;
    opData.ptpPort = hwPort;
    opData.ptpAddr = regOffset;
    opData.ptpBlock = (MSD_U32)SPRUCE_MSD_AVB_BLOCK_PTP;    /* PTP register space */

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed (ptpOperationPerform).\n"));
        return MSD_FAIL;
    }

    *data = opData.ptpData;

    return MSD_OK;
}

/*******************************************************************************
* gptpGlobalPointDataSet
*
* DESCRIPTION:
*        Set indirect register pointer control data from PTP Global registers.
*
* INPUTS:
*        port - logical port number.
*        point - Pointer to following index registers:
*            PTP_GLOBAL_CONFIG_PTP_MODE_REG
*            PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM
*        data -  PTP Global configuration data write to the indirect register
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*
* COMMENTS:
*        None
*
*******************************************************************************/
static MSD_STATUS gptpGlobalPointDataSet
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 point,
    IN  MSD_U8 data
)
{
    MSD_STATUS retVal;
    MSD_U32 count, tmpData;

    msdSemTake(dev->devNum, dev->ptpModeSem, OS_WAIT_FOREVER);

    count = 5U;
    tmpData = 0x8000U;
    while ((tmpData & 0x8000U) == 0x8000U)
    {
        retVal = gptpGetReg(dev, 0x1FU, 7U, &tmpData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpModeSem);
            return retVal;
        }
        if (--count == 0U)
        {
            msdSemGive(dev->devNum, dev->ptpModeSem);
            return MSD_FAIL;
        }
    }

    tmpData = ((MSD_U32)1 << 15) | ((MSD_U32)point << 8) | (MSD_U32)data;

    retVal = gptpSetReg(dev, 0x1fU, 7U, tmpData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->ptpModeSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->ptpModeSem);
    return retVal;
}

/*******************************************************************************
* gptpGlobalPointDataGet
*
* DESCRIPTION:
*        Get indirect register pointer control data from PTP Global registers.
*
* INPUTS:
*        point - Pointer to following index registers:
*            PTP_GLOBAL_CONFIG_PTP_MODE_REG
*            PTP_GLOBAL_CONFIG_PTP_EXTERNAL_TIME_FORM
* OUTPUTS:
*        data -  PTP Global configuration data read from the indirect register
*                pointed to by the point above.
*
* RETURNS:
*
* COMMENTS:
*        None
*
*******************************************************************************/
static MSD_STATUS gptpGlobalPointDataGet
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 point,
    OUT MSD_U8 *data
)
{
    MSD_STATUS retVal;
    MSD_U32 count, tmpData;

    msdSemTake(dev->devNum, dev->ptpModeSem, OS_WAIT_FOREVER);
    count = 5U;

    tmpData = ((MSD_U32)point << 8) | 0U;
    retVal = gptpSetReg(dev, 0x1fU, 7U, tmpData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->ptpModeSem);
        return retVal;
    }

    do {
        retVal = gptpGetReg(dev, 0x1fU, 7U, &tmpData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpModeSem);
            return retVal;
        }
        if (count-- == 0U)
        {
            msdSemGive(dev->devNum, dev->ptpModeSem);
            return MSD_FAIL;
        }

    } while ((tmpData & 0x8000U) == 0x8000U);

    *data = (MSD_U8)tmpData;
    msdSemGive(dev->devNum, dev->ptpModeSem);

    return retVal;
}

/*******************************************************************************
* gptpTimeArraySet
*
* DESCRIPTION:
*       This function Store PTP time array Data.
*
* INPUTS:
*       storeOp   - Store compensation and store all
*       timeArrayIndex - Time array index
*       timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS gptpTimeArrayStore
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 timeArrayIndex,
    IN  SPRUCE_PTP_TOD_STORE_OPERATION storeOp,
    IN  SPRUCE_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;  
    SPRUCE_MSD_PTP_OP_DATA     opData;
    SPRUCE_MSD_PTP_OPERATION   op;


    /*Load point Set*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;
    opData.ptpAddr = 0x10;
    opData.ptpData = timeArray->todLoadPoint & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing TOD Load Point.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x11;
    opData.ptpData = (MSD_U32)(timeArray->todLoadPoint >> 16) & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading TOD Load Point.\n"));
        return retVal;
    }

    /*Time array index, Clock Valid bit and domain number will be set when do the operation*/

    /*ToD Set*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;

    opData.ptpAddr = 0x13;
    opData.ptpData = timeArray->todNanoseconds & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x14;
    opData.ptpData = (MSD_U32)(timeArray->todNanoseconds >> 16) & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x15;
    opData.ptpData = timeArray->todSecondsLow & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x16;
    opData.ptpData = (MSD_U32)(timeArray->todSecondsLow >> 16) & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x17;
    opData.ptpData = (MSD_U32)timeArray->todSecondsHigh & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD.\n"));
        return retVal;
    }

    /*1722 time set*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;

    opData.ptpAddr = 0x18;
    opData.ptpData = timeArray->Nanoseconds1722Low & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing 1722 ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x19;
    opData.ptpData = (MSD_U32)(timeArray->Nanoseconds1722Low >> 16) & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing 1722 ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x1a;
    opData.ptpData = timeArray->Nanoseconds1722High & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing 1722 ToD.\n"));
        return retVal;
    }
    opData.ptpAddr = 0x1b;
    opData.ptpData = (MSD_U32)(timeArray->Nanoseconds1722High >> 16) & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing 1722 ToD.\n"));
        return retVal;
    }

    /*Tod compensation set*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;
    opData.ptpAddr = 0x1C;
    opData.ptpData = timeArray->todCompensation & 0xffffU;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD compensation.\n"));
        return retVal;
    }

    opData.ptpAddr = 0x1D;
    opData.ptpData = (MSD_U32)(timeArray->todCompensation >> 16) & 0xffffU;
    (retVal = ptpOperationPerform(dev, op, &opData));
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD compensation.\n"));
        return retVal;
    }

    /*Store operation*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;
    opData.ptpAddr = 0x12;
    opData.ptpData = ((MSD_U32)1 << 15) |
        ((MSD_U32)storeOp << 12) |
        (((MSD_U32)timeArrayIndex & 1U) << 9) |
        (((MSD_U32)timeArray->clkValid & 1U) << 8) |
        ((MSD_U32)timeArray->domainNumber & 0xffU);

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD store operation control.\n"));
    }

    return retVal;
}

/*******************************************************************************
* gptpTimeArrayCapture
*
* DESCRIPTION:
*       This function Capture PTP time array Data.
*
* INPUTS:
*       timeArrayIndex - Time array index
*       timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static MSD_STATUS gptpTimeArrayCapture
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8 timeArrayIndex,
    OUT SPRUCE_PTP_TIME_ARRAY       *timeArray
)
{
    MSD_STATUS retVal; 
    SPRUCE_MSD_PTP_OP_DATA   opData;
    SPRUCE_MSD_PTP_OPERATION op;
    MSD_U32 tmpData;

    /*Get the origin operation register value*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_DATA;
    opData.ptpAddr = 0x12;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading ToD operation control.\n"));
        return retVal;
    }
    tmpData = opData.ptpData;

    /*Capture operation*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_WRITE_DATA;
    opData.ptpAddr = 0x12;
    tmpData &= 0x89ffU;
    opData.ptpData = ((MSD_U32)1 << 15) |
        ((MSD_U32)4 << 12) |
        (((MSD_U32)timeArrayIndex & 1U) << 9) |
        tmpData;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed writing ToD capture operation control.\n"));
        return retVal;
    }

    /*Get the Load Point*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    opData.ptpAddr = 0x10;
    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading TOD Load Point.\n"));
        return retVal;
    }

    timeArray->todLoadPoint = opData.ptpMultiData[1];
    timeArray->todLoadPoint <<= 16;
    timeArray->todLoadPoint |= opData.ptpMultiData[0];

    /*Get clock valid bit and domain number*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_DATA;
    opData.ptpAddr = 0x12;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading ToD operation control.\n"));
        return retVal;
    }
    timeArray->clkValid = ((((MSD_U32)opData.ptpData >> 8) & 1U) == 1U) ? MSD_TRUE : MSD_FALSE;
    timeArray->domainNumber = (MSD_U8)opData.ptpData & 0xffU;

    /*Get the Tod time*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x13;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading ToD.\n"));
        return retVal;
    }

    timeArray->todNanoseconds = opData.ptpMultiData[1] & 0xffffU;
    timeArray->todNanoseconds <<= 16;
    timeArray->todNanoseconds |= opData.ptpMultiData[0] & 0xffffU;
    timeArray->todSecondsLow = opData.ptpMultiData[3] & 0xffffU;
    timeArray->todSecondsLow <<= 16;
    timeArray->todSecondsLow |= opData.ptpMultiData[2] & 0xffffU;

    op = SPRUCE_PTP_READ_DATA;
    opData.ptpAddr = 0x17;
    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading ToD.\n"));
        return retVal;
    }

    timeArray->todSecondsHigh = (MSD_U16)(opData.ptpData & 0xffffU);

    /*Get 1722 time*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.nData = 4;
    opData.ptpAddr = 0x18;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading 1722 ToD.\n"));
        return retVal;
    }

    timeArray->Nanoseconds1722High = (opData.ptpMultiData[3] << 16) | opData.ptpMultiData[2];
    timeArray->Nanoseconds1722Low = (opData.ptpMultiData[1] << 16) | opData.ptpMultiData[0];

    /*Get compensation*/
    opData.ptpBlock = 0;
    opData.ptpPort = 0x1F;    /* Global register */
    op = SPRUCE_PTP_READ_MULTIPLE_DATA;
    opData.nData = 2;
    opData.ptpAddr = 0x1C;

    retVal = ptpOperationPerform(dev, op, &opData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Failed reading ToD compensation.\n"));
        return retVal;
    }

    timeArray->todCompensation = (opData.ptpMultiData[1] & 0xffffU) << 16;
    timeArray->todCompensation |= opData.ptpMultiData[0];

    return retVal;
}

/************ PTP module read & write   ***************/
static MSD_STATUS gptpPortRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_U32 temp;
    retVal = gptpGetReg(dev, portNum, regOffset, &temp);
    *data = (MSD_U16)temp;
    return retVal;
}

static MSD_STATUS gptpPortRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    return gptpSetReg(dev, portNum, regOffset, (MSD_U32)data);
}

static MSD_STATUS gptpPortRegFieldGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_U16 valRead;
    retVal = gptpPortRegGet(dev, portNum, regOffset, &valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *data = MSD_BF_GET(valRead, low, high - low + 1U);
    return MSD_OK;
}

static MSD_STATUS gptpPortRegFieldSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_U16 valRead;
    retVal = gptpPortRegGet(dev, portNum, regOffset, &valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    MSD_BF_SET(valRead, data, low, high - low + 1U);
    retVal = gptpPortRegSet(dev, portNum, regOffset, valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    return MSD_OK;
}

static MSD_STATUS gptpGlobalRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_U32 temp;
    MSD_LPORT portNum = 0x1FU;
    retVal = gptpGetReg(dev, portNum, regOffset, &temp);
    *data = (MSD_U16)temp;
    return retVal;
}

static MSD_STATUS gptpGlobalRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_LPORT portNum = 0x1FU;
    return gptpSetReg(dev, portNum, regOffset, (MSD_U32)data);
}

/*
static MSD_STATUS gptpGlobalPointFieldGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U8      point,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    OUT MSD_U8      *data
)
{
    MSD_STATUS retVal;
    MSD_U8 valRead;
    retVal = gptpGlobalPointDataGet(dev, point, &valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    *data = MSD_BF_GET(valRead, low, high - low + 1U);
    return MSD_OK;
}
*/

static MSD_STATUS gptpGlobalPointFieldSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U8      point,
    IN  MSD_U8      high,
    IN  MSD_U8      low,
    IN  MSD_U8      data
)
{
    MSD_STATUS retVal;
    MSD_U8 valRead;
    retVal = gptpGlobalPointDataGet(dev, point, &valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    MSD_BF_SET(valRead, data, low, high - low + 1U);
    retVal = gptpGlobalPointDataSet(dev, point, valRead);
    if (retVal != MSD_OK)
    {
        return retVal;
    }
    return MSD_OK;
}
