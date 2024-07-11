/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* BonsaiZ1_msdPortRmonIntf.c
*
* DESCRIPTION:
*       API definitions for RMON counters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/


#include <bonsaiz1/include/api/BonsaiZ1_msdPortRmon.h>

/*******************************************************************************
* BonsaiZ1_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine BonsaiZ1_gets a specific counter of the given port
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
MSD_STATUS BonsaiZ1_gstatsGetPortCounterIntf
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT        port,
    IN  MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
)
{
	return BonsaiZ1_gstatsGetPortCounter(dev, port, (BONSAIZ1_MSD_STATS_COUNTERS)counter, statsData);
}


/*******************************************************************************
* BonsaiZ1_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine BonsaiZ1_gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to BONSAIZ1_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS BonsaiZ1_gstatsGetPortAllCntersIntf
(
    IN  MSD_QD_DEV               *dev,
    IN  MSD_LPORT        port,
    OUT MSD_STATS_COUNTER_SET    *statsCounterSet
)
{
	BONSAIZ1_MSD_STATS_COUNTER_SET  statsCounter;
	MSD_STATUS    retVal;

	if (NULL == statsCounterSet)
	{
		MSD_DBG_ERROR(("Input param MSD_STATS_COUNTER_SET in BonsaiZ1_gstatsGetPortAllCountersIntf is NULL. \n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = BonsaiZ1_gstatsGetPortAllCounters(dev, port, &statsCounter);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("BonsaiZ1_gstatsGetPortAllCounters return fail. \n"));
		}
		else
		{
			msdMemSet((void*)statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));

			statsCounterSet->InGoodOctetsLo = statsCounter.InGoodOctetsLo;
			statsCounterSet->InGoodOctetsHi = statsCounter.InGoodOctetsHi;
			statsCounterSet->InBadOctets = statsCounter.InBadOctets;
			statsCounterSet->OutFCSErr = statsCounter.OutFCSErr;
			statsCounterSet->InUnicasts = statsCounter.InUnicasts;
			statsCounterSet->Deferred = statsCounter.Deferred;
			statsCounterSet->InBroadcasts = statsCounter.InBroadcasts;
			statsCounterSet->InMulticasts = statsCounter.InMulticasts;

			statsCounterSet->Octets64 = statsCounter.Octets64;
			statsCounterSet->Octets127 = statsCounter.Octets127;
			statsCounterSet->Octets255 = statsCounter.Octets255;
			statsCounterSet->Octets511 = statsCounter.Octets511;
			statsCounterSet->Octets1023 = statsCounter.Octets1023;
			statsCounterSet->OctetsMax = statsCounter.OctetsMax;
			statsCounterSet->OutOctetsLo = statsCounter.OutOctetsLo;
			statsCounterSet->OutOctetsHi = statsCounter.OutOctetsHi;
			statsCounterSet->OutUnicasts = statsCounter.OutUnicasts;
			statsCounterSet->Excessive = statsCounter.Excessive;
			statsCounterSet->OutMulticasts = statsCounter.OutMulticasts;
			statsCounterSet->OutBroadcasts = statsCounter.OutBroadcasts;
			statsCounterSet->Single = statsCounter.Single;

			statsCounterSet->OutPause = statsCounter.OutPause;
			statsCounterSet->InPause = statsCounter.InPause;
			statsCounterSet->Multiple = statsCounter.Multiple;
			statsCounterSet->InUndersize = statsCounter.InUndersize;
			statsCounterSet->InFragments = statsCounter.InFragments;
			statsCounterSet->InOversize = statsCounter.InOversize;
			statsCounterSet->InJabber = statsCounter.InJabber;
			statsCounterSet->InRxErr = statsCounter.InRxErr;
			statsCounterSet->InFCSErr = statsCounter.InFCSErr;
			statsCounterSet->Collisions = statsCounter.Collisions;
			statsCounterSet->Late = statsCounter.Late;

			statsCounterSet->InDiscards = statsCounter.InDiscards;
			statsCounterSet->InFiltered = statsCounter.InFiltered;
			statsCounterSet->InAccepted = statsCounter.InAccepted;
			statsCounterSet->InBadAccepted = statsCounter.InBadAccepted;
			statsCounterSet->InGoodAvbClassA = statsCounter.InGoodAvbClassA;
			statsCounterSet->InGoodAvbClassB = statsCounter.InGoodAvbClassB;
			statsCounterSet->InBadAvbClassA = statsCounter.InBadAvbClassA;
			statsCounterSet->InBadAvbClassB = statsCounter.InBadAvbClassB;
			statsCounterSet->TCAMCounter0 = statsCounter.TCAMCounter0;
			statsCounterSet->TCAMCounter1 = statsCounter.TCAMCounter1;
			statsCounterSet->TCAMCounter2 = statsCounter.TCAMCounter2;
			statsCounterSet->TCAMCounter3 = statsCounter.TCAMCounter3;
			statsCounterSet->InDroppedAvbA = statsCounter.InDroppedAvbA;
			statsCounterSet->InDroppedAvbB = statsCounter.InDroppedAvbB;
			statsCounterSet->InDaUnknown = statsCounter.InDaUnknown;
			statsCounterSet->InMGMT = statsCounter.InMGMT;
			statsCounterSet->OutQueue0 = statsCounter.OutQueue0;
			statsCounterSet->OutQueue1 = statsCounter.OutQueue1;
			statsCounterSet->OutQueue2 = statsCounter.OutQueue2;
			statsCounterSet->OutQueue3 = statsCounter.OutQueue3;
			statsCounterSet->OutQueue4 = statsCounter.OutQueue4;
			statsCounterSet->OutQueue5 = statsCounter.OutQueue5;
			statsCounterSet->OutQueue6 = statsCounter.OutQueue6;
			statsCounterSet->OutQueue7 = statsCounter.OutQueue7;
			statsCounterSet->OutCutThrough = statsCounter.OutCutThrough;
			statsCounterSet->InBadQbv = statsCounter.InBadQbv;
			statsCounterSet->OutOctetsA = statsCounter.OutOctetsA;
			statsCounterSet->OutOctetsB = statsCounter.OutOctetsB;
			statsCounterSet->OutYel = statsCounter.OutYel;
			statsCounterSet->OutDroppedYel = statsCounter.OutDroppedYel;
			statsCounterSet->OutDiscards = statsCounter.OutDiscards;
			statsCounterSet->OutMGMT = statsCounter.OutMGMT;

			statsCounterSet->DropEvents = statsCounter.DropEvents;
		}
	}

	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gstatsGetHistogramMode

*
* DESCRIPTION:
*        This routine BonsaiZ1_gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (BONSAIZ1_MSD_COUNT_RX_ONLY, BONSAIZ1_MSD_COUNT_TX_ONLY, 
*                    and BONSAIZ1_MSD_COUNT_RX_TX)
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
MSD_STATUS BonsaiZ1_gstatsGetHistogramModeIntf
(
    IN  MSD_QD_DEV   *dev,
    OUT MSD_HISTOGRAM_MODE    *mode
)
{
	return BonsaiZ1_gstatsGetHistogramMode(dev, (BONSAIZ1_MSD_HISTOGRAM_MODE*)mode);
}

/*******************************************************************************
* BonsaiZ1_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (BONSAIZ1_MSD_COUNT_RX_ONLY, BONSAIZ1_MSD_COUNT_TX_ONLY, 
*                    and BONSAIZ1_MSD_COUNT_RX_TX)
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
MSD_STATUS BonsaiZ1_gstatsSetHistogramModeIntf
(
    IN MSD_QD_DEV   *dev,
    IN MSD_HISTOGRAM_MODE        mode
)
{
	return BonsaiZ1_gstatsSetHistogramMode(dev, (BONSAIZ1_MSD_HISTOGRAM_MODE)mode);
}
