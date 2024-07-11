/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdPortRmon.c
*
* DESCRIPTION:
*       API definitions for MIBs
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdPortRmon.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdStatsFlushAll
*
* DESCRIPTION:
*       Flushs all RMON counters for all ports.
*
* INPUTS:
*       devNum  - physical device number
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
MSD_STATUS msdStatsFlushAll
(
    IN  MSD_U8  devNum
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}

/*******************************************************************************
* msdStatsFlushPort
*
* DESCRIPTION:
*       Flush all RMON counters for a given port.
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
MSD_STATUS msdStatsFlushPort
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}
    
	return retVal;
}

/*******************************************************************************
* msdStatsPortCounterGet
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       counter - the counter which will be read
*
* OUTPUTS:
*       statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdStatsPortCounterGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_STATS_COUNTERS  counter,
    OUT MSD_U32  *statsData
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter(dev, portNum, counter, statsData);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStatsPortAllCountersGet
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS msdStatsPortAllCountersGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_STATS_COUNTER_SET  *statsCounterSet
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters(dev, portNum, statsCounterSet);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStatsHistogramModeGet
*
* DESCRIPTION:
*       This routine gets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical device number
*       None.
*
* OUTPUTS:
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
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
MSD_STATUS msdStatsHistogramModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_HISTOGRAM_MODE  *mode
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode(dev, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdStatsHistogramModeSet
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*       devNum  - physical device number
*       mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY,
*                    and MSD_COUNT_RX_TX)
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
*        None.
*
*******************************************************************************/
MSD_STATUS msdStatsHistogramModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_HISTOGRAM_MODE  mode
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode(dev, mode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}
/*******************************************************************************
* msdPortMibDump
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
MSD_STATUS msdPortMibDump
(
	IN  MSD_U8  devNum,
	IN  MSD_LPORT  portNum
)
{
	MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
    }
	else
	{
		if (dev->SwitchDevObj.PORTRMONObj.gmibDump != NULL)
		{
			retVal = dev->SwitchDevObj.PORTRMONObj.gmibDump(dev, portNum);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;
}
