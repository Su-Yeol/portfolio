/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdEcid.c
*
* DESCRIPTION:
*
*    The sample sample_msdBpeEnable will demonstrate how to
*    1. Enable or disable the port extender mode
*
*    Note: 
*         The port extender mode must be enabled before any ecid operation.
*         Change the mode will cause the E-CID portion of the ATU to be flused, 
*         so it's best to set the mode during dirver initialization.
*
*
*    The sample sample_msdEcid will demonstrate how to
*    1. Flush all ecid entries from the ecid table.
*    2. Add a ecid entry into the ecid Data Base.
*    3. Move all ecid entries from a port to another
*    4. Find one ecid entry exist or not.
*    5. Delete a ecid entry from the ecid table.
*
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"


/********************************************************************************
* Sample #1:
*       Enable the device port extender mode, The port extender mode must be enabled
*       before any ecid operation.  Change the mode will cause the E-CID portion of 
*       the ATU to be flused, so it's best to set the mode during dirver initialization.
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdBpeEnable(MSD_U8 devNum)
{
    MSD_STATUS status;

    MSG_PRINT(("sample_msdBpeEnable Enter.\n"));

    /*
    * Enable the port extender mode
    */
    if ((status = msdBpeEnableSet(devNum, MSD_TRUE)) != MSD_OK)
    {
        MSG_PRINT(("msdBpeEnableSet returned fail.\n"));
        return status;
    }

    MSG_PRINT(("sample_msdBpeEnable success.\n"));
    return MSD_OK;
}


/********************************************************************************
* Sample #2:
*       Basic ECID operation.
*
* Description:
*
* Comment:
*
********************************************************************************/
MSD_STATUS sample_msdEcid(MSD_U8 devNum)
{
	MSD_STATUS status;
    MSD_ECID_ENTRY ecidEntry;
    MSD_ECID_ENTRY tempEntry;
	MSD_BOOL found;
    MSD_U8 group = 0;
    MSD_U16 ecid = 0x123;

	MSG_PRINT(("sample_msdEcid Enter.\n"));

	/*
	*    Flush all ecid entries from ECID table
	*/
    if ((status = msdEcidEntryAllDelete(devNum)) != MSD_OK)
	{
		MSG_PRINT(("msdEcidEntryAllDelete returned fail.\n"));
		return status;
	}

	/*
	*    Initialize one ecid entry
	*    Assume the group is 0, ecid is 0x123
	*    
	*/
    ecidEntry.group = group;
    ecidEntry.ecid = ecid;
    ecidEntry.entryState = 0x1;
    ecidEntry.lag = 0;  /* No trunk. */
    ecidEntry.macFPri = 0;
    ecidEntry.macQPri = 0;
    ecidEntry.portVec = 1 << 2; /* Set the portVector matching with Port 2, the frame match the ecid will be forwarded to port 2. */

	/*
	*    Add this ecid entry.
	*/
    if ((status = msdEcidEntryAdd(devNum, &ecidEntry)) != MSD_OK)
	{
		MSG_PRINT(("msdEcidEntryAdd returned fail.\n"));
		return status;
	}

	/*
	*    Move the MAC Address from original port(port 2) to another(port 3).
	*/
    if ((status = msdEcidEntryMove(devNum, 2, 3)) != MSD_OK)
	{
		MSG_PRINT(("msdEcidEntryMove returned fail.\n"));
		return status;
	}

	/*
	*    find the ecid entry
	*/
	msdMemSet(&tempEntry, 0, sizeof(MSD_ECID_ENTRY));
    if ((status = msdEcidEntryFind(devNum, group, ecid, &tempEntry, &found)) != MSD_OK)
	{
		MSG_PRINT(("msdEcidEntryFind returned fail.\n"));
		return status;
	}
	MSG_PRINT(("ECID entry find result:%d\n", found));

	/*
	*    delete this ecid entry.
	*/
    if ((status = msdEcidEntryDelete(devNum, group, ecid)) != MSD_OK)
	{
		MSG_PRINT(("msdEcidEntryDelete returned fail.\n"));
		return status;
	}


	MSG_PRINT(("sample_msdEcid success.\n"));
	return MSD_OK;
}
