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
MSD_STATUS Fir_gqciStreamGateEntryAddIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8  pointer,
IN  MSD_STREAM_GATE_ENTRY *streamGateEntry
)
{
	FIR_MSD_STREAM_GATE_ENTRY tempGateEntry;

	if (NULL == streamGateEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_STREAM_GATE_ENTRY in Fir_gqciStreamGateEntryAddIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&tempGateEntry, 0, sizeof(FIR_MSD_STREAM_GATE_ENTRY));

	tempGateEntry.gateState = streamGateEntry->gateState;
	tempGateEntry.gateIPVEn = streamGateEntry->gateIPVEn;
	tempGateEntry.gateIPV = streamGateEntry->gateIPV;
	tempGateEntry.gateWindowTime = streamGateEntry->gateWindowTime;
	tempGateEntry.gateAllowedBytes = streamGateEntry->gateAllowedBytes;

	return Fir_gqciStreamGateEntryAdd(dev, streamGateId, pointer, &tempGateEntry);
}
