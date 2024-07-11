/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Fir_msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/


#include <fir/include/api/Fir_msdPortRmon.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


/* Definitions for MIB Counter */
/*
#define FIR_MSD_STATS_NO_OP               0x0
#define FIR_MSD_STATS_FLUSH_ALL           0x1
#define FIR_MSD_STATS_FLUSH_PORT          0x2
#define FIR_MSD_STATS_READ_COUNTER        0x4
#define FIR_MSD_STATS_CAPTURE_PORT        0x5
*/

/*
 *    Type definition for MIB counter operation
*/
typedef enum 
{
	Fir_STATS_FLUSH_ALL = 1,            /* Flush all counters for all ports */
	Fir_STATS_FLUSH_PORT = 2,           /* Flush all counters for a port */
	Fir_STATS_READ_COUNTER = 4,         /* Read a specific counter from a port */
	Fir_STATS_READ_ALL = 5,              /* Read all counters from a port */
	Fir_STATS_READ_ALL_AND_CLEAR = 6,	 /* Read all counters from a port and clear*/
} FIR_MSD_STATS_OPERATION;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Fir_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   FIR_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
	IN   FIR_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
);

static MSD_STATUS Fir_statsReadCounter_MultiChip
(
IN   MSD_QD_DEV      *dev,
IN   MSD_U16         port,
IN   MSD_U32         counter,
OUT  MSD_U32         *statsData
);

static MSD_STATUS Fir_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
);

static MSD_STATUS Fir_statsOperationPerform_MultiChip
(
IN   MSD_QD_DEV            *dev,
IN   FIR_MSD_STATS_OPERATION   statsOp,
IN   MSD_U8                port,
IN   FIR_MSD_STATS_COUNTERS    counter,
OUT  MSD_VOID              *statsData
);


/*******************************************************************************
* Fir_gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gstatsFlushAll
(
        IN MSD_QD_DEV  *dev
)
{
    MSD_STATUS           retVal;

    MSD_DBG_INFO(("Fir_gstatsFlushAll Called.\n"));

	retVal = Fir_statsOperationPerform(dev, Fir_STATS_FLUSH_ALL, 0, (FIR_MSD_STATS_COUNTERS)0, NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Fir_statsOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Fir_gstatsFlushAll Exit.\n"));
	return retVal;

}


/*******************************************************************************
* Fir_gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gstatsFlushPort
(
    IN MSD_QD_DEV  *dev,
    IN MSD_LPORT      port
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Fir_gstatsFlushPort Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_statsOperationPerform(dev, Fir_STATS_FLUSH_PORT, hwPort, (FIR_MSD_STATS_COUNTERS)0, NULL);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_statsOperationPerform FLUSH_PORT returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gstatsFlushPort Exit.\n"));
	return retVal;

}

/*******************************************************************************
* Fir_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine Fir_gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Fir_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  FIR_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Fir_gstatsGetPortCounter Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		if ((counter > (FIR_MSD_TYPE_BANK2 + 0x01)) || (counter < 0) || ((counter > 0x1f) && (counter < FIR_MSD_TYPE_BANK)))
		{
			MSD_DBG_ERROR(("Bad counter: %d.\n", (MSD_16)counter));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = Fir_statsOperationPerform(dev, Fir_STATS_READ_COUNTER, hwPort, counter, (MSD_VOID*)statsData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Fir_statsOperationPerform READ_COUNTER returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Fir_gstatsGetPortCounter Exit.\n"));
	return retVal;

}


/*******************************************************************************
* Fir_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine Fir_gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to FIR_MSD_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT FIR_MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Fir_gstatsGetPortAllCounters Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		retVal = Fir_statsOperationPerform(dev, Fir_STATS_READ_ALL, hwPort, (FIR_MSD_STATS_COUNTERS)0, (MSD_VOID*)statsCounterSet);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_statsOperationPerform READ_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gstatsGetPortAllCounters Exit.\n"));
	return retVal;

}

/*******************************************************************************
* Fir_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine Fir_gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (FIR_MSD_COUNT_RX_ONLY, FIR_MSD_COUNT_TX_ONLY, 
*                    and FIR_MSD_COUNT_RX_TX)
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
    OUT FIR_MSD_HISTOGRAM_MODE    *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Fir_gstatsGetHistogramMode Called.\n"));
	
    if(mode == NULL)
    {
		MSD_DBG_ERROR(("Mode is NULL.\n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* Get the Histogram mode bit.                */
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_GLOBAL_CONTROL2, (MSD_U8)6, (MSD_U8)2, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Read histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* Software definition starts from 0 ~ 3, while hardware supports the values from 1 to 3 */
			switch (data)
			{
			case 0x1U:
				*mode = FIR_MSD_COUNT_RX_ONLY;
				break;
			case 0x2U:
				*mode = FIR_MSD_COUNT_TX_ONLY;
				break;
			case 0x3U:
				*mode = FIR_MSD_COUNT_RX_TX;
				break;
			default:
				MSD_DBG_ERROR(("Read back Bad Mode: %u.\n", data));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}
	}

    MSD_DBG_INFO(("Fir_gstatsGetHistogramMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Fir_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (FIR_MSD_COUNT_RX_ONLY, FIR_MSD_COUNT_TX_ONLY, 
*                    and FIR_MSD_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Fir_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
    IN FIR_MSD_HISTOGRAM_MODE        mode
)
{
    MSD_STATUS       retVal = MSD_OK;         /* Functions return value.      */
    MSD_U16          tmpdata = 0;           /* The register's read data.    */

    MSD_DBG_INFO(("Fir_gstatsSetHistogramMode Called.\n"));

    switch (mode)
    {
        case FIR_MSD_COUNT_RX_ONLY:
			tmpdata = 1U;
			break;
        case FIR_MSD_COUNT_TX_ONLY:
			tmpdata = 2U;
			break;
        case FIR_MSD_COUNT_RX_TX:
			tmpdata = 3U;
            break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", mode));
			retVal = MSD_BAD_PARAM;
			break;
    }

	if (retVal != MSD_BAD_PARAM)
	{
		/* Set the Histogram mode bit.                */
		retVal = msdSetAnyRegField(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_GLOBAL_CONTROL2, (MSD_U8)6, (MSD_U8)2, tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Fir_gstatsSetHistogramMode Exit.\n"));
	return retVal;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Fir_statsOperationPerform
*
* DESCRIPTION:
*       This function is used by all stats control functions, and is responsible
*       to write the required operation into the stats registers.
*
* INPUTS:
*       statsOp       - The stats operation bits to be written into the stats
*                     operation register.
*       port        - port number
*       counter     - counter to be read if it's read operation
*
* OUTPUTS:
*       statsData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static MSD_STATUS Fir_statsOperationPerform_MultiChip
(
    IN   MSD_QD_DEV            *dev,
    IN   FIR_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   FIR_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
)
{
    MSD_STATUS   retVal;         /* Functions return value.      */
    MSD_U16      data;			/* Data to be set into the      */
    MSD_U32	     tmpCounter;
    MSD_U32	     startCounter;
    MSD_U32	     lastCounter;
    MSD_U16	     portNum;
    MSD_U8 	     bank;

    msdSemTake(dev->devNum, dev->statsRegsSem, OS_WAIT_FOREVER);

	portNum = (MSD_U16)(((MSD_U16)port + 1U) << 5);

    /* Wait until the stats in ready. */
    data = 1U;
    while (data == 1U)
    {
		retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

	retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_STATS_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;
    }
	data &= (MSD_U16)0xfff;
    /* Set the STAT Operation register */
    switch (statsOp)
    {
    case Fir_STATS_FLUSH_ALL:
		data |= (MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_FLUSH_ALL << 12);
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_STATS_OPERATION, data);

        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;

    case Fir_STATS_FLUSH_PORT:
		data &= (MSD_U16)0xc1f;
		data |= (MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_FLUSH_PORT << 12) | portNum;
		retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_STATS_OPERATION, data);

        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;

    case Fir_STATS_READ_COUNTER:
        retVal = Fir_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
        break;

    case Fir_STATS_READ_ALL:
    {
        MSD_U8 bankSize = 2U;
		for (bank = 0; bank < bankSize; bank++)
        {
            lastCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_Late : (MSD_U32)Fir_STATS_OutMGMT;
            startCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_InGoodOctetsLo : (MSD_U32)Fir_STATS_InDiscards;

			if (bank == 1U)
			{
				statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
			}
            for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
            {
                retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
                if (retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->statsRegsSem);
                    return retVal;
                }
            }
        }

		/* Bank 2*/
		bankSize = 3U;
		for (bank = 2; bank < bankSize; bank++)
		{
			lastCounter = (MSD_U32)Fir_STATS_AtsFiltered;
			startCounter = (MSD_U32)Fir_STATS_DropEvents;

			if (bank == 2U)
			{
				statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
			}
			for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
			{
				retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
				if (retVal != MSD_OK)
				{
					msdSemGive(dev->devNum, dev->statsRegsSem);
					return retVal;
				}
			}
		}

    }
    break;

    default:

        msdSemGive(dev->devNum, dev->statsRegsSem);
        return MSD_FAIL;
    }

    msdSemGive(dev->devNum, dev->statsRegsSem);
    return MSD_OK;
}
static MSD_STATUS Fir_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   FIR_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   FIR_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;			/* Data to be set into the      */
    MSD_U32	tmpCounter;
    MSD_U32	startCounter;
    MSD_U32	lastCounter;
    MSD_U16	portNum;
	MSD_U8 	bank;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Fir_statsOperationPerform_MultiChip(dev, statsOp, port, counter, statsData);
    }

    msdSemTake(dev->devNum, dev->statsRegsSem, OS_WAIT_FOREVER);

	portNum = (MSD_U16)(((MSD_U16)port + 1U) << 5);

    /* Wait until the stats in ready. */
    data = 1U;
    while(data == 1U)
    {
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

	retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, &data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->statsRegsSem);
		return retVal;
	}
	data &= (MSD_U16)0xfff;
    /* Set the STAT Operation register */
    switch (statsOp)
    {
        case Fir_STATS_FLUSH_ALL:
			data |= (MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_FLUSH_ALL << 12);
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, data);

            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

        case Fir_STATS_FLUSH_PORT:
			data &= (MSD_U16)0xc1f;
			data |= (MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_FLUSH_PORT << 12) | portNum;
			retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, data);
			
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

		case Fir_STATS_READ_COUNTER:
            retVal = Fir_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->statsRegsSem);
                return retVal;
            }
            break;

        case Fir_STATS_READ_ALL:
			{
				MSD_U8 bankSize = 2U;
				for (bank = 0; bank < bankSize; bank++)
				{
					lastCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_Late : (MSD_U32)Fir_STATS_OutMGMT;
					startCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_InGoodOctetsLo : (MSD_U32)Fir_STATS_InDiscards;

					if (bank == 1U)
					{
						statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
					}
					for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
					{
						retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
						if(retVal != MSD_OK)
						{
							msdSemGive(dev->devNum, dev->statsRegsSem);
							return retVal;
						}
					}
				}
				/* Bank 2*/
				bankSize = 3U;
				for (bank = 2; bank < bankSize; bank++)
				{
					lastCounter = (MSD_U32)Fir_STATS_AtsFiltered;
					startCounter = (MSD_U32)Fir_STATS_DropEvents;

					if (bank == 2U)
					{
						statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
					}
					for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
					{
						retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
						if (retVal != MSD_OK)
						{
							msdSemGive(dev->devNum, dev->statsRegsSem);
							return retVal;
						}
					}
				}

			}
			break;
		case Fir_STATS_READ_ALL_AND_CLEAR:
			{
				MSD_U8 bankSize = 2U;
				for (bank = 0; bank < bankSize; bank++)
				{
					lastCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_Late : (MSD_U32)Fir_STATS_OutMGMT;
					startCounter = (bank == 0U) ? (MSD_U32)Fir_STATS_InGoodOctetsLo : (MSD_U32)Fir_STATS_InDiscards;

					if (bank == 1U)
					{
						statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
					}
					for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
					{
						retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
						if(retVal != MSD_OK)
						{
							msdSemGive(dev->devNum, dev->statsRegsSem);
							return retVal;
						}
					}
				}
				/* Bank 2*/
				bankSize = 3U;
				for (bank = 2; bank < bankSize; bank++)
				{
					lastCounter = (MSD_U32)Fir_STATS_AtsFiltered;
					startCounter = (MSD_U32)Fir_STATS_DropEvents;

					if (bank == 2U)
					{
						statsData = (MSD_U32 *)statsData + Fir_STATS_Late + 1;
					}
					for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
					{
						retVal = Fir_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
						if (retVal != MSD_OK)
						{
							msdSemGive(dev->devNum, dev->statsRegsSem);
							return retVal;
						}
					}
				}

			}
			break;
        default:
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return MSD_FAIL;
    }

    msdSemGive(dev->devNum, dev->statsRegsSem);
    return MSD_OK;
}

/*******************************************************************************
* Fir_statsReadCounter
*
* DESCRIPTION:
*       This function is used to read a captured counter.
*
* INPUTS:
*       port        - port number
*       counter     - counter to be read if it's read operation
*
* OUTPUTS:
*       statsData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*
* COMMENTS:
*        If Semaphore is used, Semaphore should be acquired before this function call.
*******************************************************************************/
static MSD_STATUS Fir_statsReadCounter_MultiChip
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
)
{
    MSD_STATUS   retVal;         /* Functions return value.            */
    MSD_U16      data;/* Data to be set into the  register. */
    MSD_U16    counter3_2;     /* Counter Register Bytes 3 & 2       */
    MSD_U16    counter1_0;     /* Counter Register Bytes 1 & 0       */

    data = 1U;
    while (data == 1U)
    {
		retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
    }

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_READ_COUNTER << 12) | port | (MSD_U16)((MSD_U16)counter & (MSD_U16)0x1f));
	if ((counter & (MSD_U32)FIR_MSD_TYPE_BANK) != 0U)
    {
		data |= (MSD_U16)((MSD_U16)1 << 10);
    }

	if ((counter & (MSD_U32)FIR_MSD_TYPE_BANK2) != 0U)
	{
		data |= (MSD_U16)((MSD_U16)1 << 11);
	}

	retVal = Fir_msdDirectSetMultiChipReg(dev, FIR_STATS_OPERATION, data);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1U;
    while (data == 1U)
    {
		retVal = Fir_msdDirectGetMultiChipRegField(dev, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
    }

	retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_STATS_COUNTER3_2, &counter3_2);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

	retVal = Fir_msdDirectGetMultiChipReg(dev, FIR_STATS_COUNTER1_0, &counter1_0);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

	*statsData = (MSD_U32)(((MSD_U32)counter3_2 << 16) | counter1_0);

    return MSD_OK;

}

static MSD_STATUS Fir_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
)
{
    MSD_STATUS   retVal;         /* Functions return value.            */
    MSD_U16      data;/* Data to be set into the  register. */ 
    MSD_U16    counter3_2;     /* Counter Register Bytes 3 & 2       */
    MSD_U16    counter1_0;     /* Counter Register Bytes 1 & 0       */

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Fir_statsReadCounter_MultiChip(dev, port, counter, statsData);
    }
	
    data = 1U;
    while(data == 1U)
    {
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}
	
	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)Fir_STATS_READ_COUNTER << 12) | port | (MSD_U16)((MSD_U16)counter & (MSD_U16)0x1f));
	if ((counter & (MSD_U32)FIR_MSD_TYPE_BANK) != 0U)
	{
		data |= (MSD_U16)((MSD_U16)1 << 10);
	}

	if ((counter & (MSD_U32)FIR_MSD_TYPE_BANK2) != 0U)
	{
		data |= (MSD_U16)((MSD_U16)1 << 11);
	}

	retVal = msdSetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, data);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1U;
    while(data == 1U)
    {
		retVal = msdGetAnyRegField(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}

	retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_COUNTER3_2, &counter3_2);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum, FIR_GLOBAL1_DEV_ADDR, FIR_STATS_COUNTER1_0, &counter1_0);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

	*statsData = (MSD_U32)((MSD_U32)counter3_2 << 16) | counter1_0;

    return MSD_OK;

}

/*******************************************************************************
* Fir_gmibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
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
MSD_STATUS Fir_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
)
{
	MSD_STATUS status;
	FIR_MSD_STATS_COUNTER_SET statsCounter;

	msdMemSet(&statsCounter, 0, sizeof(FIR_MSD_STATS_COUNTER_SET));

	status = Fir_gstatsGetPortAllCounters(dev, portNum, &statsCounter);
	if (status != MSD_OK)
	{
		MSD_DBG_ERROR(("Fir_gstatsGetPortAllCounters returned: %s.\n", msdDisplayStatus(status)));
	}
	else
	{
		/* Bank 0 */
		MSG(("InGoodOctetsLo          %08lu   \n", statsCounter.InGoodOctetsLo));
		MSG(("InGoodOctetsHi          %08lu   \n", statsCounter.InGoodOctetsHi));
		MSG(("InBadOctets             %08lu   \n", statsCounter.InBadOctets));
		MSG(("OutFCSErr               %08lu   \n", statsCounter.OutFCSErr));
		MSG(("InUnicasts              %08lu   \n", statsCounter.InUnicasts));
		MSG(("Deferred/OutPFrames     %08lu   \n", statsCounter.Deferred));
		MSG(("InBroadcasts            %08lu   \n", statsCounter.InBroadcasts));
		MSG(("InMulticasts            %08lu   \n", statsCounter.InMulticasts));
		/*
		Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
		(refer to Histogram Mode)
		*/
		MSG(("Octets64                %08lu   \n", statsCounter.Octets64));
		MSG(("Octets127               %08lu   \n", statsCounter.Octets127));
		MSG(("Octets255               %08lu   \n", statsCounter.Octets255));
		MSG(("Octets511               %08lu   \n", statsCounter.Octets511));
		MSG(("Octets1023              %08lu   \n", statsCounter.Octets1023));
		MSG(("OctetsMax               %08lu   \n", statsCounter.OctetsMax));
		MSG(("OutOctetsLo             %08lu   \n", statsCounter.OutOctetsLo));
		MSG(("OutOctetsHi             %08lu   \n", statsCounter.OutOctetsHi));
		MSG(("OutUnicasts             %08lu   \n", statsCounter.OutUnicasts));
		MSG(("Excessive/InPFrames     %08lu   \n", statsCounter.Excessive));
		MSG(("OutMulticasts           %08lu   \n", statsCounter.OutMulticasts));
		MSG(("OutBroadcasts           %08lu   \n", statsCounter.OutBroadcasts));
		MSG(("Single/InBadPFrags      %08lu   \n", statsCounter.Single));

		MSG(("OutPause                %08lu   \n", statsCounter.OutPause));
		MSG(("InPause                 %08lu   \n", statsCounter.InPause));
		MSG(("Multiple/InPFrags       %08lu   \n", statsCounter.Multiple));
		MSG(("InUndersize             %08lu   \n", statsCounter.InUndersize));
		MSG(("InFragments             %08lu   \n", statsCounter.InFragments));
		MSG(("InOversize              %08lu   \n", statsCounter.InOversize));
		MSG(("InJabber                %08lu   \n", statsCounter.InJabber));
		MSG(("InRxErr                 %08lu   \n", statsCounter.InRxErr));
		MSG(("InFCSErr                %08lu   \n", statsCounter.InFCSErr));
		MSG(("Collisions/OutPFrags    %08lu   \n", statsCounter.Collisions));
		MSG(("Late/InPAssemblyErr     %08lu   \n", statsCounter.Late));

		/* Bank 1 */
		MSG(("InDiscards              %08lu   \n", statsCounter.InDiscards));
		MSG(("InFiltered              %08lu   \n", statsCounter.InFiltered));
		MSG(("InAccepted              %08lu   \n", statsCounter.InAccepted));
		MSG(("InBadAccepted           %08lu   \n", statsCounter.InBadAccepted));
		MSG(("InGoodAvbClassA         %08lu   \n", statsCounter.InGoodAvbClassA));
		MSG(("InGoodAvbClassB         %08lu   \n", statsCounter.InGoodAvbClassB));
		MSG(("InBadAvbClassA          %08lu   \n", statsCounter.InBadAvbClassA));
		MSG(("InBadAvbClassB          %08lu   \n", statsCounter.InBadAvbClassB));
		MSG(("TCAMCounter0            %08lu   \n", statsCounter.TCAMCounter0));
		MSG(("TCAMCounter1            %08lu   \n", statsCounter.TCAMCounter1));
		MSG(("TCAMCounter2            %08lu   \n", statsCounter.TCAMCounter2));
		MSG(("TCAMCounter3            %08lu   \n", statsCounter.TCAMCounter3));
		MSG(("InDroppedAvbA           %08lu   \n", statsCounter.InDroppedAvbA));
		MSG(("InDroppedAvbB           %08lu   \n", statsCounter.InDroppedAvbB));
		MSG(("InDaUnknown             %08lu   \n", statsCounter.InDaUnknown));
		MSG(("InMGMT                  %08lu   \n", statsCounter.InMGMT));
		MSG(("OutQueue0               %08lu   \n", statsCounter.OutQueue0));
		MSG(("OutQueue1               %08lu   \n", statsCounter.OutQueue1));
		MSG(("OutQueue2               %08lu   \n", statsCounter.OutQueue2));
		MSG(("OutQueue3               %08lu   \n", statsCounter.OutQueue3));
		MSG(("OutQueue4               %08lu   \n", statsCounter.OutQueue4));
		MSG(("OutQueue5               %08lu   \n", statsCounter.OutQueue5));
		MSG(("OutQueue6               %08lu   \n", statsCounter.OutQueue6));
		MSG(("OutQueue7               %08lu   \n", statsCounter.OutQueue7));
		MSG(("OutCutThrough           %08lu   \n", statsCounter.OutCutThrough));
		MSG(("InBadQbv                %08lu   \n", statsCounter.InBadQbv));
		MSG(("OutOctetsA              %08lu   \n", statsCounter.OutOctetsA));
		MSG(("OutOctetsB              %08lu   \n", statsCounter.OutOctetsB));
		MSG(("OutYellow               %08lu   \n", statsCounter.OutYel));
		MSG(("OutDroppedYel           %08lu   \n", statsCounter.OutDroppedYel));
		MSG(("OutDiscards             %08lu   \n", statsCounter.OutDiscards));
		MSG(("OutMGMT                 %08lu   \n", statsCounter.OutMGMT));

		/* Bank 2 */
		MSG(("DropEvents              %08lu   \n", statsCounter.DropEvents));
		MSG(("AtsFiltered             %08lu   \n", statsCounter.AtsFiltered));
		MSG(("\n"));
	}

	return status;
}

