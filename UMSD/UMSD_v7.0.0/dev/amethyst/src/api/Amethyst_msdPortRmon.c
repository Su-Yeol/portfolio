/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/


#include <amethyst/include/api/Amethyst_msdPortRmon.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


/* Definitions for MIB Counter */
/*
#define AMETHYST_MSD_STATS_NO_OP               0x0
#define AMETHYST_MSD_STATS_FLUSH_ALL           0x1
#define AMETHYST_MSD_STATS_FLUSH_PORT          0x2
#define AMETHYST_MSD_STATS_READ_COUNTER        0x4
#define AMETHYST_MSD_STATS_CAPTURE_PORT        0x5
*/

/*
 *    Type definition for MIB counter operation
*/
typedef enum 
{
	Amethyst_STATS_FLUSH_ALL = 1,            /* Flush all counters for all ports */
	Amethyst_STATS_FLUSH_PORT = 2,           /* Flush all counters for a port */
	Amethyst_STATS_READ_COUNTER = 4,         /* Read a specific counter from a port */
	Amethyst_STATS_READ_ALL = 5,              /* Read all counters from a port */
} AMETHYST_MSD_STATS_OPERATION;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Amethyst_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   AMETHYST_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
	IN   AMETHYST_MSD_STATS_COUNTERS    counter,
    OUT  MSD_VOID              *statsData
);

static MSD_STATUS Amethyst_statsReadCounter
(
    IN   MSD_QD_DEV      *dev,
    IN   MSD_U16         port,
    IN   MSD_U32         counter,
    OUT  MSD_U32         *statsData
);

/*******************************************************************************
* Amethyst_gstatsFlushAll
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
MSD_STATUS Amethyst_gstatsFlushAll
(
        IN MSD_QD_DEV  *dev
)
{
    MSD_STATUS           retVal;

    MSD_DBG_INFO(("Amethyst_gstatsFlushAll Called.\n"));

    retVal = Amethyst_statsOperationPerform(dev,Amethyst_STATS_FLUSH_ALL,0,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_statsOperationPerform FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstatsFlushAll Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Amethyst_gstatsFlushPort
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
MSD_STATUS Amethyst_gstatsFlushPort
(
    IN MSD_QD_DEV  *dev,
    IN MSD_LPORT      port
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Amethyst_gstatsFlushPort Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_statsOperationPerform(dev,Amethyst_STATS_FLUSH_PORT,hwPort,0,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_statsOperationPerform FLUSH_PORT returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstatsFlushPort Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Amethyst_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine Amethyst_gets a specific counter of the given port
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
MSD_STATUS Amethyst_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  AMETHYST_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Amethyst_gstatsGetPortCounter Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    if (counter > AMETHYST_MSD_TYPE_BANK + 0x1F || counter < 0 || (counter > 0x1f && counter < AMETHYST_MSD_TYPE_BANK))
    {
        MSD_DBG_ERROR(("Bad counter: %d.\n", (int)counter));
        return MSD_BAD_PARAM;
    }

	retVal = Amethyst_statsOperationPerform(dev, Amethyst_STATS_READ_COUNTER, hwPort, counter, (MSD_VOID*)statsData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_statsOperationPerform READ_COUNTER returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstatsGetPortCounter Exit.\n"));
    return MSD_OK;

}


/*******************************************************************************
* Amethyst_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine Amethyst_gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to AMETHYST_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Amethyst_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT AMETHYST_MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
    MSD_STATUS    retVal;
    MSD_U8        hwPort;         /* physical port number         */

    MSD_DBG_INFO(("Amethyst_gstatsGetPortAllCounters Called.\n"));

    /* translate logical port to physical port */
    hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
    {
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_statsOperationPerform(dev,Amethyst_STATS_READ_ALL,hwPort,0,(MSD_VOID*)statsCounterSet);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_statsOperationPerform READ_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstatsGetPortAllCounters Exit.\n"));
    return MSD_OK;

}

/*******************************************************************************
* Amethyst_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine Amethyst_gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (AMETHYST_MSD_COUNT_RX_ONLY, AMETHYST_MSD_COUNT_TX_ONLY, 
*                    and AMETHYST_MSD_COUNT_RX_TX)
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
MSD_STATUS Amethyst_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
    OUT AMETHYST_MSD_HISTOGRAM_MODE    *mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Amethyst_gstatsGetHistogramMode Called.\n"));
	
    if(mode == NULL)
    {
		MSD_DBG_ERROR(("Mode is NULL.\n"));
        return MSD_BAD_PARAM;
    }

    /* Get the Histogram mode bit.                */
	retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_GLOBAL_CONTROL2,6,2,&data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* Software definition starts from 0 ~ 3, while hardware supports the values from 1 to 3 */
	switch(data)
	{
	    case 0x1:
			*mode = AMETHYST_MSD_COUNT_RX_ONLY;
			break;
		case 0x2:
			*mode = AMETHYST_MSD_COUNT_TX_ONLY;
			break;
		case 0x3:
			*mode = AMETHYST_MSD_COUNT_RX_TX;
			break;
        default:
			MSD_DBG_ERROR(("Read back Bad Mode: %u.\n", (unsigned int)data));
            return MSD_BAD_PARAM;
	}

    MSD_DBG_INFO(("Amethyst_gstatsGetHistogramMode Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Amethyst_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (AMETHYST_MSD_COUNT_RX_ONLY, AMETHYST_MSD_COUNT_TX_ONLY, 
*                    and AMETHYST_MSD_COUNT_RX_TX)
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
MSD_STATUS Amethyst_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
    IN AMETHYST_MSD_HISTOGRAM_MODE        mode
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Amethyst_gstatsSetHistogramMode Called.\n"));

    switch (mode)
    {
        case AMETHYST_MSD_COUNT_RX_ONLY:
			data = 1;
			break;
        case AMETHYST_MSD_COUNT_TX_ONLY:
			data = 2;
			break;
        case AMETHYST_MSD_COUNT_RX_TX:
			data = 3;
            break;
        default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", (unsigned int)mode));
            return MSD_BAD_PARAM;
    }

    /*data = (MSD_U16)mode;*/

    /* Set the Histogram mode bit.                */
	retVal = msdSetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_GLOBAL_CONTROL2,6,2,data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write histogram mode bit returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstatsSetHistogramMode Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Amethyst_statsOperationPerform
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
static MSD_STATUS Amethyst_statsOperationPerform_MultiChip
(
    IN   MSD_QD_DEV            *dev,
    IN   AMETHYST_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   AMETHYST_MSD_STATS_COUNTERS    counter,
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

    msdSemTake(dev->devNum, dev->statsRegsSem, OS_WAIT_FOREVER);

    portNum = (port + 1) << 5;

    /* Wait until the stats in ready. */
    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_STATS_OPERATION, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

    retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_STATS_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;
    }
    data &= 0xfff;
    /* Set the STAT Operation register */
    switch (statsOp)
    {
    case Amethyst_STATS_FLUSH_ALL:
        data |= (1 << 15) | (Amethyst_STATS_FLUSH_ALL << 12);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_STATS_OPERATION, data);

        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;

    case Amethyst_STATS_FLUSH_PORT:
        data &= 0xc1f;
        data |= (1 << 15) | (Amethyst_STATS_FLUSH_PORT << 12) | portNum;
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_STATS_OPERATION, data);

        msdSemGive(dev->devNum, dev->statsRegsSem);
        return retVal;

    case Amethyst_STATS_READ_COUNTER:
        retVal = Amethyst_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
        break;

    case Amethyst_STATS_READ_ALL:
    {
        int bankSize = 2;
        for (bank = 0; bank<bankSize; bank++)
        {
            lastCounter = (bank == 0) ? (MSD_U32)Amethyst_STATS_Late : (MSD_U32)Amethyst_STATS_OutMGMT;
            startCounter = (bank == 0) ? (MSD_U32)Amethyst_STATS_InGoodOctetsLo : (MSD_U32)Amethyst_STATS_InDiscards;

            if (bank == 1)
                statsData = (MSD_U32 *)statsData + Amethyst_STATS_Late + 1;
            for (tmpCounter = startCounter; tmpCounter <= lastCounter; tmpCounter++)
            {
                retVal = Amethyst_statsReadCounter(dev, portNum, tmpCounter, ((MSD_U32*)statsData + tmpCounter - startCounter));
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
static MSD_STATUS Amethyst_statsOperationPerform
(
    IN   MSD_QD_DEV            *dev,
    IN   AMETHYST_MSD_STATS_OPERATION   statsOp,
    IN   MSD_U8                port,
    IN   AMETHYST_MSD_STATS_COUNTERS    counter,
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

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return Amethyst_statsOperationPerform_MultiChip(dev, statsOp, port, counter, statsData);
    }

    msdSemTake(dev->devNum, dev->statsRegsSem,OS_WAIT_FOREVER);

	portNum = (port + 1) << 5;

    /* Wait until the stats in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;
        }
    }

	retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,&data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->statsRegsSem);
		return retVal;
	}
	data &= 0xfff;
    /* Set the STAT Operation register */
    switch (statsOp)
    {
        case Amethyst_STATS_FLUSH_ALL:
            data |= (1 << 15) | (Amethyst_STATS_FLUSH_ALL << 12);
            retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,data);

            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

        case Amethyst_STATS_FLUSH_PORT:
			data &= 0xc1f;
            data |= (1 << 15) | (Amethyst_STATS_FLUSH_PORT << 12) | portNum ;
            retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,data);
			
            msdSemGive(dev->devNum, dev->statsRegsSem);
            return retVal;

		case Amethyst_STATS_READ_COUNTER:
            retVal = Amethyst_statsReadCounter(dev, portNum, (MSD_U32)counter, (MSD_U32*)statsData);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->statsRegsSem);
                return retVal;
            }
            break;

        case Amethyst_STATS_READ_ALL:
			{
				int bankSize = 2;
				for(bank=0; bank<bankSize; bank++)
				{
					lastCounter = (bank == 0) ? (MSD_U32)Amethyst_STATS_Late : (MSD_U32)Amethyst_STATS_OutMGMT;
					startCounter = (bank==0)?(MSD_U32)Amethyst_STATS_InGoodOctetsLo : (MSD_U32)Amethyst_STATS_InDiscards;

					if(bank==1)
						statsData = (MSD_U32 *)statsData + Amethyst_STATS_Late +1;
					for(tmpCounter=startCounter; tmpCounter<=lastCounter; tmpCounter++)
					{
						retVal = Amethyst_statsReadCounter(dev, portNum, tmpCounter,((MSD_U32*)statsData+tmpCounter-startCounter));
						if(retVal != MSD_OK)
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
* Amethyst_statsReadCounter
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
static MSD_STATUS Amethyst_statsReadCounter_MultiChip
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

    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_STATS_OPERATION, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
    }

    data = (MSD_U16)((1 << 15) | (Amethyst_STATS_READ_COUNTER << 12) | port | (counter & 0x1f));
    if (counter & AMETHYST_MSD_TYPE_BANK)
    {
        data |= (1 << 10);
    }

    retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_STATS_OPERATION, data);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_STATS_OPERATION, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
    }

    retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_STATS_COUNTER3_2, &counter3_2);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_STATS_COUNTER1_0, &counter1_0);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    *statsData = (counter3_2 << 16) | counter1_0;

    return MSD_OK;

}
static MSD_STATUS Amethyst_statsReadCounter
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

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return Amethyst_statsReadCounter_MultiChip(dev, port, counter, statsData);
    }
	
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}
	
    data = (MSD_U16)((1 << 15) | (Amethyst_STATS_READ_COUNTER << 12) | port | (counter&0x1f) );
	if (counter & AMETHYST_MSD_TYPE_BANK)
	{
		data |= (1<<10);
	}

    retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,data);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STATS_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            return retVal;
        }
	}

	retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_STATS_COUNTER3_2, &counter3_2);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_STATS_COUNTER1_0, &counter1_0);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    *statsData = (counter3_2 << 16) | counter1_0;

    return MSD_OK;

}
/*******************************************************************************
* Amethyst_gmibDump
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
MSD_STATUS Amethyst_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
)
{
	MSD_STATUS status;
	AMETHYST_MSD_STATS_COUNTER_SET statsCounter; 
	msdMemSet(&statsCounter, 0, sizeof(AMETHYST_MSD_STATS_COUNTER_SET));
	if ((status = Amethyst_gstatsGetPortAllCounters(dev, portNum, &statsCounter)) != MSD_OK)
	{
		return status;
	}
	/* Bank 0 */
	MSG(("InGoodOctetsLo  %08lu   \n", statsCounter.InGoodOctetsLo));
	MSG(("InGoodOctetsHi  %08lu   \n", statsCounter.InGoodOctetsHi));
	MSG(("InBadOctets     %08lu   \n", statsCounter.InBadOctets));
	MSG(("OutFCSErr       %08lu   \n", statsCounter.OutFCSErr));
	MSG(("InUnicasts      %08lu   \n", statsCounter.InUnicasts));
	MSG(("Deferred        %08lu   \n", statsCounter.Deferred));
	MSG(("InBroadcasts    %08lu   \n", statsCounter.InBroadcasts));
	MSG(("InMulticasts    %08lu   \n", statsCounter.InMulticasts));
	/*
	Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
	(refer to Histogram Mode)
	*/
	MSG(("Octets64        %08lu   \n", statsCounter.Octets64));
	MSG(("Octets127       %08lu   \n", statsCounter.Octets127));
	MSG(("Octets255       %08lu   \n", statsCounter.Octets255));
	MSG(("Octets511       %08lu   \n", statsCounter.Octets511));
	MSG(("Octets1023      %08lu   \n", statsCounter.Octets1023));
	MSG(("OctetsMax       %08lu   \n", statsCounter.OctetsMax));
	MSG(("OutOctetsLo     %08lu   \n", statsCounter.OutOctetsLo));
	MSG(("OutOctetsHi     %08lu   \n", statsCounter.OutOctetsHi));
	MSG(("OutUnicasts     %08lu   \n", statsCounter.OutUnicasts));
	MSG(("Excessive       %08lu   \n", statsCounter.Excessive));
	MSG(("OutMulticasts   %08lu   \n", statsCounter.OutMulticasts));
	MSG(("OutBroadcasts   %08lu   \n", statsCounter.OutBroadcasts));
	MSG(("Single          %08lu   \n", statsCounter.Single));

	MSG(("OutPause        %08lu   \n", statsCounter.OutPause));
	MSG(("InPause         %08lu   \n", statsCounter.InPause));
	MSG(("Multiple        %08lu   \n", statsCounter.Multiple));
	MSG(("InUndersize     %08lu   \n", statsCounter.InUndersize));
	MSG(("InFragments     %08lu   \n", statsCounter.InFragments));
	MSG(("InOversize      %08lu   \n", statsCounter.InOversize));
	MSG(("InJabber        %08lu   \n", statsCounter.InJabber));
	MSG(("InRxErr         %08lu   \n", statsCounter.InRxErr));
	MSG(("InFCSErr        %08lu   \n", statsCounter.InFCSErr));
	MSG(("Collisions      %08lu   \n", statsCounter.Collisions));
	MSG(("Late            %08lu   \n", statsCounter.Late));

	/* Bank 1 */
	MSG(("InDiscards      %08lu   \n", statsCounter.InDiscards));
	MSG(("InFiltered      %08lu   \n", statsCounter.InFiltered));
	MSG(("InAccepted      %08lu   \n", statsCounter.InAccepted));
	MSG(("InBadAccepted   %08lu   \n", statsCounter.InBadAccepted));
	MSG(("InGoodAvbClassA %08lu   \n", statsCounter.InGoodAvbClassA));
	MSG(("InGoodAvbClassB %08lu   \n", statsCounter.InGoodAvbClassB));
	MSG(("InBadAvbClassA  %08lu   \n", statsCounter.InBadAvbClassA));
	MSG(("InBadAvbClassB  %08lu   \n", statsCounter.InBadAvbClassB));
	MSG(("TCAMCounter0    %08lu   \n", statsCounter.TCAMCounter0));
	MSG(("TCAMCounter1    %08lu   \n", statsCounter.TCAMCounter1));
	MSG(("TCAMCounter2    %08lu   \n", statsCounter.TCAMCounter2));
	MSG(("TCAMCounter3    %08lu   \n", statsCounter.TCAMCounter3));
	MSG(("InDroppedAvbA   %08lu   \n", statsCounter.InDroppedAvbA));
	MSG(("InDroppedAvbB   %08lu   \n", statsCounter.InDroppedAvbB));
	MSG(("InDaUnknown     %08lu   \n", statsCounter.InDaUnknown));
	MSG(("InMGMT          %08lu   \n", statsCounter.InMGMT));
	MSG(("OutQueue0       %08lu   \n", statsCounter.OutQueue0));
	MSG(("OutQueue1       %08lu   \n", statsCounter.OutQueue1));
	MSG(("OutQueue2       %08lu   \n", statsCounter.OutQueue2));
	MSG(("OutQueue3       %08lu   \n", statsCounter.OutQueue3));
	MSG(("OutQueue4       %08lu   \n", statsCounter.OutQueue4));
	MSG(("OutQueue5       %08lu   \n", statsCounter.OutQueue5));
	MSG(("OutQueue6       %08lu   \n", statsCounter.OutQueue6));
	MSG(("OutQueue7       %08lu   \n", statsCounter.OutQueue7));
	MSG(("OutCutThrough   %08lu   \n", statsCounter.OutCutThrough));
	MSG(("InBadQbv        %08lu   \n", statsCounter.InBadQbv));
	MSG(("OutOctetsA      %08lu   \n", statsCounter.OutOctetsA));
	MSG(("OutOctetsB      %08lu   \n", statsCounter.OutOctetsB));
	MSG(("OutYellow       %08lu   \n", statsCounter.OutYel));
	MSG(("OutDroppedYel   %08lu   \n", statsCounter.OutDroppedYel));
	MSG(("OutDiscards     %08lu   \n", statsCounter.OutDiscards));
	MSG(("OutMGMT         %08lu   \n", statsCounter.OutMGMT));

	MSG(("\n"));
	return MSD_OK;

}

