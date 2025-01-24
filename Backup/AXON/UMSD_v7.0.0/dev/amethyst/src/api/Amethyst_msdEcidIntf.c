/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Amethyst_msdEcidIntf.c
*
* DESCRIPTION:
*       API definitions for ECID Databases 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdEcid.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>

/*******************************************************************************
* Amethyst_gecidAddEntry
*
* DESCRIPTION:
*       This routine creates a new entry in the ECID table
*
* INPUTS:
*       ecidEntry - ECID entry to be added to ECID table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidAddEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_ECID_ENTRY *ecidEntry
)
{
    AMETHYST_MSD_ECID_ENTRY entry;

    if (NULL == ecidEntry)
    {
        MSD_DBG_ERROR(("Input param MSD_ECID_ENTRY in Amethyst_gecidAddEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    entry.ecid = ecidEntry->ecid;
    entry.entryState = ecidEntry->entryState;
    entry.group = ecidEntry->group;
    entry.lag = ecidEntry->lag;
    entry.lagId = ecidEntry->lagId;
    entry.macFPri = ecidEntry->macFPri;
    entry.macQPri = ecidEntry->macQPri;
    entry.portVec = ecidEntry->portVec;
    entry.removeEtag = ecidEntry->removeEtag;

    return Amethyst_gecidAddEntry(dev, &entry);
}

/*******************************************************************************
* Amethyst_gecidGetEcidEntryNext
*
* DESCRIPTION:
*       Gets the next valid ECID entry starting from the specified group and ecid value
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
*
* OUTPUTS:
*       ecidEntry - valid ECID entry
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*       MSD_NO_SUCH - No valid entry exist
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidGetEcidEntryNextIntf
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT MSD_ECID_ENTRY* ecidEntry
)
{
    AMETHYST_MSD_ECID_ENTRY entry;
    MSD_STATUS retVal;

    if (NULL == ecidEntry)
    {
        MSD_DBG_ERROR(("Input param MSD_ECID_ENTRY in Amethyst_gecidGetEcidEntryNextIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gecidGetEcidEntryNext(dev, group, ecid, &entry);
    if (MSD_OK != retVal)
        return retVal;

    msdMemSet((void*)ecidEntry, 0, sizeof(MSD_ECID_ENTRY));
    ecidEntry->ecid = entry.ecid;
    ecidEntry->entryState = entry.entryState;
    ecidEntry->group = entry.group;
    ecidEntry->lag = entry.lag;
    ecidEntry->lagId = entry.lagId;
    ecidEntry->macFPri = entry.macFPri;
    ecidEntry->macQPri = entry.macQPri;
    ecidEntry->portVec = entry.portVec;
    ecidEntry->removeEtag = entry.removeEtag;

    return retVal;
}

/*******************************************************************************
* Amethyst_gecidFindEcidEntry
*
* DESCRIPTION:
*       This routine check if the specified ecid of one group exist in ECID table, if exists, return the valid ECID entry
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
*
* OUTPUTS:
*       ecidEntry - valid ECID entry
*       found     - MSD_TRUE, if the appropriate entry exists
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidFindEcidEntry
(
IN MSD_QD_DEV *dev,
IN  MSD_U8  group,
IN  MSD_U16 ecid,
OUT AMETHYST_MSD_ECID_ENTRY* ecidEntry,
OUT MSD_BOOL* found
);
MSD_STATUS Amethyst_gecidFindEcidEntryIntf
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT MSD_ECID_ENTRY* ecidEntry,
    OUT MSD_BOOL* found
)
{
    AMETHYST_MSD_ECID_ENTRY entry;
    MSD_STATUS retVal;

    if (NULL == ecidEntry || NULL == found)
    {
        MSD_DBG_ERROR(("Input param MSD_ECID_ENTRY or found in Amethyst_gecidFindEcidEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gecidFindEcidEntry(dev, group, ecid, &entry, found);
    if (MSD_OK != retVal || MSD_FALSE == *found)
        return retVal;

    msdMemSet((void*)ecidEntry, 0, sizeof(MSD_ECID_ENTRY));
    ecidEntry->ecid = entry.ecid;
    ecidEntry->entryState = entry.entryState;
    ecidEntry->group = entry.group;
    ecidEntry->lag = entry.lag;
    ecidEntry->lagId = entry.lagId;
    ecidEntry->macFPri = entry.macFPri;
    ecidEntry->macQPri = entry.macQPri;
    ecidEntry->portVec = entry.portVec;
    ecidEntry->removeEtag = entry.removeEtag;

    return retVal;
}