/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Fir_msdQav.c
*
* DESCRIPTION:
*       API definitions for Qav
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <fir/include/api/Fir_msdQav.h>
#include <fir/include/api/Fir_msdApiInternal.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

typedef enum
{
    MSD_AVB_BLOCK_PTP = 0x0,
    MSD_AVB_BLOCK_AVB_POLICY = 0x1,
    MSD_AVB_BLOCK_QAV = 0x2,
    MSD_AVB_BLOCK_QBV = 0x3
} MSD_AVB_BLOCK;

/*
* Typedef: enum MSD_PTP_OPERATION
*
* Description: Defines the PTP (Precise Time Protocol) Operation type
*
* Fields:
*      PTP_WRITE_DATA             - Write data to the PTP register
*      PTP_READ_DATA            - Read data from PTP register
*      PTP_READ_MULTIPLE_DATA    - Read multiple data from PTP register
*      PTP_READ_TIMESTAMP_DATA    - Read timestamp data from PTP register
*                    valid bit will be reset after read
*/
typedef enum
{
    PTP_WRITE_DATA = 0x3,
    PTP_READ_DATA = 0x4,
    PTP_READ_MULTIPLE_DATA = 0x6,
    PTP_READ_TIMESTAMP_DATA = 0x8,
} MSD_PTP_OPERATION;

/*
*  typedef: struct MSD_PTP_OP_DATA
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
    MSD_U32    ptpMultiData[FIR_MAX_PTP_CONSECUTIVE_READ];
    MSD_U32    nData;
} MSD_PTP_OP_DATA;

static MSD_STATUS ptpOperationPerform
(
IN    MSD_QD_DEV             *dev,
IN    MSD_PTP_OPERATION      ptpOp,
INOUT MSD_PTP_OP_DATA        *opData
);

static MSD_STATUS ptpOperationPerform_MultiChip
(
IN    MSD_QD_DEV             *dev,
IN    MSD_PTP_OPERATION      ptpOp,
INOUT MSD_PTP_OP_DATA        *opData
);


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
    IN    MSD_PTP_OPERATION      ptpOp,
    INOUT MSD_PTP_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32          i;
    MSD_U16          ptpOperation;
    MSD_U16          data;

    msdSemTake(dev->devNum, dev->ptpRegsSem, OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
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
    /* Set the PTP Operation register */
    switch (ptpOp)
    {
    case PTP_WRITE_DATA:
    {
        data = (MSD_U16)opData->ptpData;
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_DATA, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

		ptpOperation = (MSD_U16)PTP_WRITE_DATA;
		ptpOperation &= (MSD_U16)3;
        ptpOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
			(MSD_U16)(opData->ptpPort << 8) |
			(MSD_U16)(opData->ptpBlock << 5) |
			(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

        retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
    }
    break;

    case PTP_READ_DATA:
    {
		ptpOperation = (MSD_U16)PTP_READ_DATA;
		ptpOperation &= (MSD_U16)3;
        ptpOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
			(MSD_U16)(opData->ptpPort << 8) |
			(MSD_U16)(opData->ptpBlock << 5) |
			(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

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
        opData->ptpData = (MSD_U32)data;
    }
    break;

    case PTP_READ_MULTIPLE_DATA:
    {
		ptpOperation = (MSD_U16)PTP_READ_MULTIPLE_DATA;
		ptpOperation &= (MSD_U16)3;
        ptpOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
			(MSD_U16)(opData->ptpPort << 8) |
			(MSD_U16)(opData->ptpBlock << 5) |
			(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

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

        for (i = 0; i<opData->nData; i++)
        {
            retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_AVB_DATA, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpMultiData[i] = (MSD_U32)data;
        }
    }
    break;

    case PTP_READ_TIMESTAMP_DATA:
    {
		ptpOperation = (MSD_U16)PTP_READ_MULTIPLE_DATA;
		ptpOperation &= (MSD_U16)3;
        ptpOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
			(MSD_U16)(opData->ptpPort << 8) |
			(MSD_U16)(opData->ptpBlock << 5) |
			(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

		data = (MSD_U8)1;
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
        opData->ptpMultiData[0] = (MSD_U32)data;

		if ((data & (MSD_U16)0x1) != (MSD_U16)0x1)
        {
            /* valid bit is not set */
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

        for (i = 1U; i<opData->nData; i++)
        {
            retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_AVB_DATA, &data);
            opData->ptpMultiData[i] = (MSD_U32)data;
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->ptpRegsSem);
                return retVal;
            }
        }

		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_DATA, (MSD_U16)0);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }

		ptpOperation = (MSD_U16)PTP_WRITE_DATA;
		ptpOperation &= (MSD_U16)3;
        ptpOperation <<= 13;
		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
			(MSD_U16)(opData->ptpPort << 8) |
			(MSD_U16)(opData->ptpBlock << 5) |
			(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_AVB_COMMAND, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->ptpRegsSem);
            return retVal;
        }
    }
    break;

    default:
        msdSemGive(dev->devNum, dev->ptpRegsSem);
        return MSD_FAIL;
		break;
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
    msdSemGive(dev->devNum, dev->ptpRegsSem);
    return retVal;
}

static MSD_STATUS ptpOperationPerform
(
    IN    MSD_QD_DEV             *dev,
    IN    MSD_PTP_OPERATION      ptpOp,
    INOUT MSD_PTP_OP_DATA        *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U32          i;
	MSD_U16          ptpOperation;
	MSD_U16          data;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return ptpOperationPerform_MultiChip(dev, ptpOp, opData);
    }

    msdSemTake(dev->devNum,dev->ptpRegsSem, OS_WAIT_FOREVER);

    /* Wait until the ptp in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    /* Set the PTP Operation register */
    switch (ptpOp)
    {
        case PTP_WRITE_DATA:
		{
            data = (MSD_U16)opData->ptpData;
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

			ptpOperation = (MSD_U16)PTP_WRITE_DATA;
			ptpOperation &= (MSD_U16)3;
            ptpOperation <<=13;
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
				(MSD_U16)(opData->ptpPort << 8) |
				(MSD_U16)(opData->ptpBlock << 5) |
				(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
		}
		break;

        case PTP_READ_DATA:
		{
			ptpOperation = (MSD_U16)PTP_READ_DATA;
			ptpOperation &= (MSD_U16)3;
            ptpOperation <<=13;
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
				(MSD_U16)(opData->ptpPort << 8) |
				(MSD_U16)(opData->ptpBlock << 5) |
				(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));

			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
				retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

			retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, &data);
			if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpData = (MSD_U32)data;
		}
        break;

        case PTP_READ_MULTIPLE_DATA:
		{
			ptpOperation = (MSD_U16)PTP_READ_MULTIPLE_DATA;
			ptpOperation &= (MSD_U16)3;
            ptpOperation <<=13;
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
				(MSD_U16)(opData->ptpPort << 8) |
				(MSD_U16)(opData->ptpBlock << 5) |
				(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
				retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

            for(i=0; i<opData->nData; i++)
            {
				retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
				opData->ptpMultiData[i] = (MSD_U32)data;
            }
		}
        break;

        case PTP_READ_TIMESTAMP_DATA:
		{
			ptpOperation = (MSD_U16)PTP_READ_MULTIPLE_DATA;
			ptpOperation &= (MSD_U16)3;
            ptpOperation <<=13;
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
				(MSD_U16)(opData->ptpPort << 8) |
				(MSD_U16)(opData->ptpBlock << 5) |
				(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            data = 1U;
            while(data == 1U)
            {
				retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

			retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, &data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }
            opData->ptpMultiData[0] = (MSD_U32)data;

            if (!(data & 0x1))
            {
                /* valid bit is not set */
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

            for(i=1; i<opData->nData; i++)
            {
				retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, &data);
                opData->ptpMultiData[i] = (MSD_U32)data;
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum,dev->ptpRegsSem);
                    return retVal;
                }
            }

			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_DATA, (MSD_U16)0);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum,dev->ptpRegsSem);
                return retVal;
            }

			ptpOperation = (MSD_U16)PTP_WRITE_DATA;
			ptpOperation &= (MSD_U16)3;
            ptpOperation <<=13;
			data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (ptpOperation) |
				(MSD_U16)(opData->ptpPort << 8) |
				(MSD_U16)(opData->ptpBlock << 5) |
				(MSD_U16)(opData->ptpAddr & (MSD_U16)0x1F));
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, data);
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
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL2_DEV_ADDR, FIR_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum,dev->ptpRegsSem);
            return retVal;
        }
    }
    msdSemGive(dev->devNum,dev->ptpRegsSem);
    return retVal;
}

/******************************************************************************
* Fir_gqavSetPortQpriXRate
*
* DESCRIPTION:
*        This routine set Priority Queue rate on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue   - queue priority
*        rate    - number of credits in bytes .
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
MSD_STATUS Fir_gqavSetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT     port,
    IN  MSD_U8        queue,
    IN  MSD_U32        rate
)
{
    MSD_STATUS           retVal;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA        opData;
    MSD_U32            hwPort;

    MSD_DBG_INFO(("Fir_gqavSetPortQpriXRate Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == (MSD_U32)MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if ((rate % (MSD_U32)32) != 0U)
			{
				MSD_DBG_ERROR(("Failed (Bad rate).\n"));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				/* Write rate low bits */
				opData.ptpBlock = (MSD_U32)MSD_AVB_BLOCK_QAV;    /* QAV register space */
				opData.ptpAddr = (MSD_U32)((MSD_U32)queue * 2U);

				opData.ptpPort = hwPort;

				op = PTP_WRITE_DATA;
				opData.ptpData = (rate / (MSD_U32)32) & (MSD_U32)0xffff;

				retVal = ptpOperationPerform(dev, op, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("OperationPerform Fir_gqavSetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					/* Read rate high bits value, modify this register */
					opData.ptpBlock = (MSD_U32)MSD_AVB_BLOCK_QAV;    /* QAV register space */
					opData.ptpAddr = (MSD_U32)(((MSD_U32)queue * 2U) + 1U);

					opData.ptpPort = hwPort;

					op = PTP_READ_DATA;
					opData.ptpData = 0;

					retVal = ptpOperationPerform(dev, op, &opData);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("OperationPerform Fir_gqavSetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
					}
					else
					{
						/* Write back */
						op = PTP_WRITE_DATA;
						opData.ptpData = (opData.ptpData & (MSD_U32)0x7FFF) | ((((rate / (MSD_U32)32) & (MSD_U32)0x10000) >> 16) << 15);

						retVal = ptpOperationPerform(dev, op, &opData);
						if (retVal != MSD_OK)
						{
							MSD_DBG_ERROR(("OperationPerform Fir_gqavSetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
						}
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gqavSetPortQpriXRate Exit.\n"));
	return retVal;
}
/******************************************************************************
* Fir_gqavGetPortQpriXRate
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 rate Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        rate - number of credits in bytes .
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
MSD_STATUS Fir_gqavGetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN    MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U32        *rate
)
{
    MSD_STATUS           retVal;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA        opData;
    MSD_U32            hwPort;
	MSD_U32            tmpData = 0;

    MSD_DBG_INFO(("Fir_gqavGetPortQpriXRate Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
	if (hwPort == (MSD_U32)MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			opData.ptpBlock = (MSD_U32)MSD_AVB_BLOCK_QAV;    /* QAV register space */

			opData.ptpAddr = (MSD_U32)((MSD_U32)queue * 2U);
			opData.ptpPort = hwPort;

			op = PTP_READ_DATA;

			retVal = ptpOperationPerform(dev, op, &opData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("OperationPerform Fir_gqavGetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				/* Store low bit value */
				tmpData = opData.ptpData & (MSD_U32)0xffff;

				/* Read high bit value */
				opData.ptpAddr = (MSD_U32)(((MSD_U32)queue * 2U) + 1U);

				retVal = ptpOperationPerform(dev, op, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("OperationPerform Fir_gqavGetPortQpriXRate returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					*rate = ((((opData.ptpData & (MSD_U32)0x8000) >> 15) << 16) | tmpData) * (MSD_U32)32;
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gqavGetPortQpriXRate Exit.\n"));
	return retVal;
}
/******************************************************************************
* Fir_gqavSetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine set Priority Queue 2-3 HiLimit on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue - 2 - 3
*        hiLimit - number of credits in bytes .
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
MSD_STATUS Fir_gqavSetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN    MSD_LPORT    port,
    IN  MSD_U8        queue,
    IN  MSD_U16        hiLimit
)
{
    MSD_STATUS           retVal;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA      opData;
    MSD_U32              hwPort;
	MSD_U16              data = 0;

    MSD_DBG_INFO(("Fir_gqavSetPortQpriXHiLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == (MSD_U32)MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if (hiLimit > (MSD_U16)0x3fff)
			{
				MSD_DBG_ERROR(("Failed (Bad hiLimit).\n"));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				opData.ptpBlock = (MSD_U32)MSD_AVB_BLOCK_QAV;    /* QAV register space */
				opData.ptpAddr = (MSD_U32)(((MSD_U32)queue * 2U) + 1U);

				opData.ptpPort = hwPort;

				op = PTP_READ_DATA;

				retVal = ptpOperationPerform(dev, op, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("OperationPerform Fir_gqavGetPortQpriXHiLimit returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					data = (MSD_U16)(opData.ptpData & (MSD_U16)0x8000);

					op = PTP_WRITE_DATA;
					opData.ptpData = (MSD_U32)data | (hiLimit & (MSD_U32)0x3fff);

					retVal = ptpOperationPerform(dev, op, &opData);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("OperationPerform Fir_gqavSetPortQpriXHiLimit returned: %s.\n", msdDisplayStatus(retVal)));
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Fir_gqavSetPortQpriXHiLimit Exit.\n"));
	return retVal;
}
/******************************************************************************
* Fir_gqavGetPortQpriXHiLimit
*
* DESCRIPTION:
*        This routine get Priority Queue 2-3 HiLimit Bytes on a port.
*
* INPUTS:
*        port    - the logical port number
*        queue    - 2 - 3
*
* OUTPUTS:
*        hiLimit - number of credits in bytes .
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
MSD_STATUS Fir_gqavGetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U16        *hiLimit
)
{
    MSD_STATUS           retVal;
    MSD_PTP_OPERATION    op;
    MSD_PTP_OP_DATA        opData;
    MSD_U32            hwPort;

    MSD_DBG_INFO(("Fir_gqavGetPortQpriXHiLimit Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = (MSD_U32)MSD_LPORT_2_PORT(port);
    if (hwPort == (MSD_U32)MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Failed (Bad Port).\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if (queue >= FIR_MAX_NUM_OF_QUEUES)
		{
			MSD_DBG_ERROR(("Failed (Bad queue).\n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			opData.ptpBlock = (MSD_U32)MSD_AVB_BLOCK_QAV;    /* QAV register space */

			opData.ptpAddr = (MSD_U32)(((MSD_U32)queue * 2U) + 1U);
			opData.ptpPort = hwPort;

			op = PTP_READ_DATA;

			retVal = ptpOperationPerform(dev, op, &opData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("OperationPerform Fir_gqavGetPortQpriXHiLimit returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				*hiLimit = (MSD_U16)opData.ptpData & (MSD_U16)0x3fff;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gqavGetPortQpriXHiLimit Exit.\n"));
	return retVal;
}
