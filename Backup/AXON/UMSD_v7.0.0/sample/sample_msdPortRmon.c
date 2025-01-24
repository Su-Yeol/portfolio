/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdPortRmon.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Get all counters of the given port.
*    2. Flush all counters for a port.
*    3. Get a specific counter of the given port.
*    4. Flush all counters for all ports.
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

void displayCounter(MSD_STATS_COUNTER_SET *statsCounter);

MSD_STATUS sample_msdPortRmon(MSD_U8 devNum)
{
	MSD_STATUS status;

	MSD_LPORT port;
	MSD_STATS_COUNTER_SET statsCounterSet;

	MSD_STATS_COUNTERS counter;
	MSD_U32 statsData;

	/*
	 *    Get all counters of the given port(port0).
	 */
	port = 0;
	if ((status = msdStatsPortAllCountersGet(devNum, port, &statsCounterSet)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsPortAllCountersGet returned fail.\n"));
		return status;
	}
	/* display the counter */
	displayCounter(&statsCounterSet);

	/*
	 *    Flush all counters of this port(port0).
	 */
	if ((status = msdStatsFlushPort(devNum, port)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsFlushPort returned fail.\n"));
		return status;
	}

	/*
	 *    Get a specific counter(e.g. STATS_MaxOctets) of this given port(port0)
	 *    statsDate should be 0 after msdStatsFlushPort above
	 */
    counter = MSD_STATS_MaxOctets;
	if ((status = msdStatsPortCounterGet(devNum, port, counter, &statsData)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsPortCounterGet returned fail.\n"));
		return status;
	}

	/*
	 *    Flush all counters for all ports.
	 */
	if ((status = msdStatsFlushAll(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdStatsFlushAll returned fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdPortRmon sample code success.\n"));
	return MSD_OK;
}

void displayCounter(MSD_STATS_COUNTER_SET *statsCounter)
{
	/* Bank 0 */
	MSG_PRINT(("InGoodOctetsLo  %08u   \n", statsCounter->InGoodOctetsLo));
	MSG_PRINT(("InGoodOctetsHi  %08u   \n", statsCounter->InGoodOctetsHi));
	MSG_PRINT(("InBadOctets     %08u   \n", statsCounter->InBadOctets));
	MSG_PRINT(("OutFCSErr       %08u   \n", statsCounter->OutFCSErr));
	MSG_PRINT(("InUnicasts      %08u   \n", statsCounter->InUnicasts));
	MSG_PRINT(("Deferred        %08u   \n", statsCounter->Deferred));
	MSG_PRINT(("InBroadcasts    %08u   \n", statsCounter->InBroadcasts));
	MSG_PRINT(("InMulticasts    %08u   \n", statsCounter->InMulticasts));
    /*
        Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
        (refer to Histogram Mode)
    */
	MSG_PRINT(("Octets64        %08u   \n", statsCounter->Octets64));
	MSG_PRINT(("Octets127       %08u   \n", statsCounter->Octets127));
	MSG_PRINT(("Octets255       %08u   \n", statsCounter->Octets255));
	MSG_PRINT(("Octets511       %08u   \n", statsCounter->Octets511));
	MSG_PRINT(("Octets1023      %08u   \n", statsCounter->Octets1023));
	MSG_PRINT(("OctetsMax       %08u   \n", statsCounter->OctetsMax));
	MSG_PRINT(("OutOctetsLo     %08u   \n", statsCounter->OutOctetsLo));
	MSG_PRINT(("OutOctetsHi     %08u   \n", statsCounter->OutOctetsHi));
	MSG_PRINT(("OutUnicasts     %08u   \n", statsCounter->OutUnicasts));
	MSG_PRINT(("Excessive       %08u   \n", statsCounter->Excessive));
	MSG_PRINT(("OutMulticasts   %08u   \n", statsCounter->OutMulticasts));
	MSG_PRINT(("OutBroadcasts   %08u   \n", statsCounter->OutBroadcasts));
	MSG_PRINT(("Single          %08u   \n", statsCounter->Single));

	MSG_PRINT(("OutPause        %08u   \n", statsCounter->OutPause));
	MSG_PRINT(("InPause         %08u   \n", statsCounter->InPause));
	MSG_PRINT(("Multiple        %08u   \n", statsCounter->Multiple));
	MSG_PRINT(("InUndersize     %08u   \n", statsCounter->InUndersize));
	MSG_PRINT(("InFragments     %08u   \n", statsCounter->InFragments));
	MSG_PRINT(("InOversize      %08u   \n", statsCounter->InOversize));
	MSG_PRINT(("InJabber        %08u   \n", statsCounter->InJabber));
	MSG_PRINT(("InRxErr         %08u   \n", statsCounter->InRxErr));
	MSG_PRINT(("InFCSErr        %08u   \n", statsCounter->InFCSErr));
	MSG_PRINT(("Collisions      %08u   \n", statsCounter->Collisions));
	MSG_PRINT(("Late            %08u   \n", statsCounter->Late));

	/* Bank 1 */
	MSG_PRINT(("InDiscards      %08u   \n", statsCounter->InDiscards));
	MSG_PRINT(("InFiltered      %08u   \n", statsCounter->InFiltered));
	MSG_PRINT(("InAccepted      %08u   \n", statsCounter->InAccepted));
	MSG_PRINT(("InBadAccepted   %08u   \n", statsCounter->InBadAccepted));
	MSG_PRINT(("InGoodAvbClassA %08u   \n", statsCounter->InGoodAvbClassA));
	MSG_PRINT(("InGoodAvbClassB %08u   \n", statsCounter->InGoodAvbClassB));
	MSG_PRINT(("InBadAvbClassA  %08u   \n", statsCounter->InBadAvbClassA));
	MSG_PRINT(("InBadAvbClassB  %08u   \n", statsCounter->InBadAvbClassB));
	MSG_PRINT(("TCAMCounter0    %08u   \n", statsCounter->TCAMCounter0));
	MSG_PRINT(("TCAMCounter1    %08u   \n", statsCounter->TCAMCounter1));
	MSG_PRINT(("TCAMCounter2    %08u   \n", statsCounter->TCAMCounter2));
	MSG_PRINT(("TCAMCounter3    %08u   \n", statsCounter->TCAMCounter3));
	MSG_PRINT(("InDroppedAvbA   %08u   \n", statsCounter->InDroppedAvbA));
	MSG_PRINT(("InDroppedAvbB   %08u   \n", statsCounter->InDroppedAvbB));
	MSG_PRINT(("InDaUnknown     %08u   \n", statsCounter->InDaUnknown));
	MSG_PRINT(("InMGMT          %08u   \n", statsCounter->InMGMT));
	MSG_PRINT(("OutQueue0       %08u   \n", statsCounter->OutQueue0));
	MSG_PRINT(("OutQueue1       %08u   \n", statsCounter->OutQueue1));
	MSG_PRINT(("OutQueue2       %08u   \n", statsCounter->OutQueue2));
	MSG_PRINT(("OutQueue3       %08u   \n", statsCounter->OutQueue3));
	MSG_PRINT(("OutQueue4       %08u   \n", statsCounter->OutQueue4));
	MSG_PRINT(("OutQueue5       %08u   \n", statsCounter->OutQueue5));
	MSG_PRINT(("OutQueue6       %08u   \n", statsCounter->OutQueue6));
	MSG_PRINT(("OutQueue7       %08u   \n", statsCounter->OutQueue7));
	MSG_PRINT(("OutCutThrough   %08u   \n", statsCounter->OutCutThrough));
	MSG_PRINT(("InBadQbv        %08u   \n", statsCounter->InBadQbv));
	MSG_PRINT(("OutOctetsA      %08u   \n", statsCounter->OutOctetsA));
	MSG_PRINT(("OutOctetsB      %08u   \n", statsCounter->OutOctetsB));
	MSG_PRINT(("OutYellow       %08u   \n", statsCounter->OutYel));
	MSG_PRINT(("OutDroppedYel   %08u   \n", statsCounter->OutDroppedYel));
	MSG_PRINT(("OutDiscards     %08u   \n", statsCounter->OutDiscards));
	MSG_PRINT(("OutMGMT         %08u   \n", statsCounter->OutMGMT));
	MSG_PRINT(("DropEvents      %08u   \n", statsCounter->DropEvents));

}
