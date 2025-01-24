/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Amethyst_msdEcid.c
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
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>

static MSD_STATUS Amethyst_ecidOperationPerform
(
    IN    MSD_QD_DEV *dev,
    IN    AMETHYST_MSD_ATU_OPERATION atuOp,        
    INOUT AMETHYST_MSD_ECID_ENTRY *entry
);

/*******************************************************************************
* Amethyst_gBpeEnableSet
*
* DESCRIPTION:
*       This routine is used to configure the device for Port Extender mode or not. After enable this mode,
*       users can operate ECID table, such as add/delete/move entries, etc.
*
* INPUTS:
*       en    - MSD_TRUE: enable the feature for CPU operation, MSD_FALSE: otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       calling this API will cause the E-CID portion of the ATU to be flused, so it's best to call
*       this API during dirver initialization.
*
*******************************************************************************/
MSD_STATUS Amethyst_gBpeEnableSet
(
    IN MSD_QD_DEV *dev,
    IN  MSD_BOOL en
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_DBG_INFO(("Amethyst_gBpeEnableSet Called.\n"));
    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);

    /* Wait until the ATU in ready. */
    data = 1;
    while (data == 1)
    {
        if (IS_SMI_MULTICHIP_SUPPORTED(dev))
            retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);
        else
            retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);

        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

	/* translate BOOL to binary */
	MSD_BOOL_2_BIT(en, data);

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
        retVal = Amethyst_msdDirectSetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 15, 1, data);
    else
        retVal = msdSetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 15, 1, data);

    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        MSD_DBG_ERROR(("msdSetAnyRegField for AMETHYST_QD_REG_ATU_FID_REG returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    MSD_DBG_INFO(("Amethyst_gBpeEnableSet Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gBpeEnableGet
*
* DESCRIPTION:
*       This routine get the status if the device is configured Port Extender mode or not
*
* INPUTS:
*       None
*
* OUTPUTS:
*       en    - MSD_TRUE: enable the feature for CPU operation, MSD_FALSE: otherwise.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gBpeEnableGet
(
    IN MSD_QD_DEV *dev,
    OUT MSD_BOOL *en
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_DBG_INFO(("Amethyst_gBpeEnableGet Called.\n"));

    data = 0;
    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 15, 1, &data);
    else
        retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 15, 1, &data);

    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for AMETHYST_QD_REG_ATU_FID_REG returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_BIT_2_BOOL(data, *en);

    MSD_DBG_INFO(("Amethyst_gBpeEnableGet Exit.\n"));
    return retVal;
}

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
MSD_STATUS Amethyst_gecidAddEntry
(
    IN  MSD_QD_DEV *dev,
    IN  AMETHYST_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Amethyst_gecidAddEntry Called.\n"));
    if (ecidEntry->ecid > 0xfff || ecidEntry->group > 3)
    {
        MSD_DBG_ERROR(("Bad ecid: %d or group: %d.\n", (MSD_U16)ecidEntry->ecid, (MSD_U16)ecidEntry->group));
        return MSD_BAD_PARAM;
    }
    /* If this is trunk entry, we need to check trunkId range, it should be within [0, 0xF]*/
    if (MSD_TRUE == ecidEntry->lag && 0 == AMETHYST_IS_TRUNK_ID_VALID(dev, ecidEntry->lagId))
    {
        MSD_DBG_ERROR(("Bad TrunkId: %d. It should be within [0, 31].\n", (MSD_U16)ecidEntry->lagId));
        return MSD_BAD_PARAM;
    }

    if (ecidEntry->entryState == 0)
    {
        MSD_DBG_ERROR(("Bad entry state, Entry State should not be ZERO\n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_LOAD_PURGE_ENTRY, ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_LOAD_PURGE_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gecidAddEntry Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gecidFlushAll
*
* DESCRIPTION:
*       This routine delete all entries in the ECID table
*
* INPUTS:
*
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
MSD_STATUS Amethyst_gecidFlushAll
(
    IN MSD_QD_DEV *dev
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Amethyst_gecidFlushAll Called.\n"));
    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));
    ecidEntry.entryState = 0;

    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_FLUSH_ALL, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gecidFlushAll Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gecidMove
*
* DESCRIPTION:
*       This routine moves entries from a port to another
*
* INPUTS:
*       moveFrom - port where moving frome
*       moveTo   - port where moving to
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
MSD_STATUS Amethyst_gecidMove
(
    IN MSD_QD_DEV *dev,
    IN  MSD_LPORT moveFrom,
    IN  MSD_LPORT moveTo
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;
    MSD_U32 tempPort1;
    MSD_U32 tempPort2;

    MSD_DBG_INFO(("Amethyst_gecidMove Called.\n"));

    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));
    if (moveTo == 0x1F)
        tempPort1 = moveTo;
    else
        tempPort1 = (MSD_U32)MSD_LPORT_2_PORT(moveTo);
    tempPort2 = (MSD_U32)MSD_LPORT_2_PORT(moveFrom);

    if ((tempPort1 == MSD_INVALID_PORT) || (tempPort2 == MSD_INVALID_PORT))
    {
        MSD_DBG_ERROR(("Bad Port: moveto %u, moveFrom %u .\n", (unsigned int)(moveTo), (unsigned int)(moveFrom)));
        return MSD_BAD_PARAM;
    }


    ecidEntry.entryState = 0xf;
    ecidEntry.portVec = tempPort1 << 5 | tempPort2;

    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_FLUSH_ALL, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gecidMove Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gecidFlushEntry
*
* DESCRIPTION:
*       This routine deletes one entry of the specified group in the ECID table
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
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
MSD_STATUS Amethyst_gecidFlushEntry
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Amethyst_gecidFlushEntry Called.\n"));
    if (ecid > 0xfff || group > 3)
    {
        MSD_DBG_ERROR(("Bad ecid: %d or group: %d.\n", (MSD_U16)ecid, (MSD_U16)group));
        return MSD_BAD_PARAM;
    }

    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));

    ecidEntry.entryState = 0;
    ecidEntry.group = group;
    ecidEntry.ecid = ecid;

    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_LOAD_PURGE_ENTRY, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gecidFlushEntry Exit.\n"));
    return retVal;
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
MSD_STATUS Amethyst_gecidGetEcidEntryNext
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT AMETHYST_MSD_ECID_ENTRY* ecidEntry
)
{
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Amethyst_gecidGetEcidEntryNext Called.\n"));
    if (ecid > 0xfff || group > 3)
    {
        MSD_DBG_ERROR(("Bad ecid: %d or group: %d.\n", (MSD_U16)ecid, (MSD_U16)group));
        return MSD_BAD_PARAM;
    }

    msdMemSet(ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));

    ecidEntry->group = group;
    ecidEntry->ecid = ecid;

    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_GET_NEXT_ENTRY, ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    if (ecidEntry->entryState == 0)
    {
        MSD_DBG_INFO(("No more valid Entry found!.\n"));
        MSD_DBG_INFO(("Amethyst_gecidGetEcidEntryNext Exit.\n"));
        return MSD_NO_SUCH;
    }

    MSD_DBG_INFO(("Amethyst_gecidGetEcidEntryNext Exit.\n"));
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
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_DBG_INFO(("Amethyst_gecidFindEcidEntry Called.\n"));
    if (ecid > 0xfff || group > 3)
    {
        MSD_DBG_ERROR(("Bad ecid: %d or group: %d.\n", (MSD_U16)ecid, (MSD_U16)group));
        return MSD_BAD_PARAM;
    }

    *found = MSD_FALSE;
    data = group << 12 | ecid;
    if (data == 0)
        data = 0x3fff;
    else
        data = data - 1;

    ecidEntry->ecid = data & 0xfff;
    ecidEntry->group = (data >> 12) & 0x03;

	retVal = Amethyst_gecidGetEcidEntryNext(dev, ecidEntry->group, ecidEntry->ecid, ecidEntry);
	if (MSD_NO_SUCH == retVal)
	{
		MSD_DBG_INFO(("Amethyst_gecidFindEcidEntry Exit, Not found valid entry.\n"));
		return MSD_OK;
	}
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Amethyst_gecidFindEcidEntry returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    if (ecidEntry->group != group || ecidEntry->ecid != ecid)
    {
		MSD_DBG_INFO(("Amethyst_gecidFindEcidEntry Exit, Not found this entry.\n"));
		return MSD_OK;
    }

    *found = MSD_TRUE;
    MSD_DBG_INFO(("Amethyst_gecidFindEcidEntry Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gecidGetEntryCount
*
* DESCRIPTION:
*       This routine counts all entries in the ECID table
*
* INPUTS:
*       None
*
* OUTPUTS:
*       count - The number of current valid entries
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
static MSD_STATUS Amethyst_gecidGetEntryCount_MultiChip
(
    IN MSD_QD_DEV *dev,
    OUT MSD_U32 *count
)
{
    MSD_STATUS retVal;
    MSD_U16 data;
    MSD_U16 bin;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Amethyst_gecidGetEntryCount_MultiChip Called.\n"));
    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));

    /*check if the device in Split ATU mode*/
    data = 0;
    retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 15, 1, &data);
    if (retVal != MSD_OK)
        return retVal;
    if (data == 0)
        return MSD_FEATURE_NOT_ENABLE;

    /*Set the ecid mode as 1*/
    retVal = Amethyst_msdDirectSetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 14, 1, 1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    *count = 0;
    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_GET_NEXT_ENTRY, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    for (bin = 4; bin < 8; bin++)
    {
        data = bin << 11;
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_ATU_STATS, data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
        retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_ATU_STATS, 0, 11, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
        *count += data;
    }

    MSD_DBG_INFO(("Amethyst_gecidGetEntryCount_MultiChip Exit.\n"));
    return retVal;
}
MSD_STATUS Amethyst_gecidGetEntryCount
(
    IN MSD_QD_DEV *dev,
    OUT MSD_U32 *count
)
{
    MSD_STATUS retVal;
    MSD_U16 data;
    MSD_U16 bin;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return Amethyst_gecidGetEntryCount_MultiChip(dev, count);
    }

    MSD_DBG_INFO(("Amethyst_gecidGetEntryCount Called.\n"));
    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));

    /*check if the device in Split ATU mode*/
    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 15, 1, &data);
    if (retVal != MSD_OK)
        return retVal;
    if (data == 0)
        return MSD_FEATURE_NOT_ENABLE;

    /*Set the ecid mode as 1*/
    retVal = msdSetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 14, 1, 1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Set the count mode as 0 for ecid count, and then issue the getNext operation*/
    data = 0;
    retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_ATU_STATS, data);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    *count = 0;    
    retVal = Amethyst_ecidOperationPerform(dev, AMETHYST_GET_NEXT_ENTRY, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Amethyst_ecidOperationPerform AMETHYST_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    for (bin = 4; bin < 8; bin++)
    {
        data = bin << 11;
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_ATU_STATS, data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
        retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL2_DEV_ADDR, AMETHYST_QD_REG_ATU_STATS, 0, 11, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
        *count += data;
    }    

    MSD_DBG_INFO(("Amethyst_gecidGetEntryCount Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Amethyst_gecidDump
*
* DESCRIPTION:
*       This routine finds all valid ECID entries and print it out
*
* INPUTS:
*
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
MSD_STATUS Amethyst_gecidDump
(
    IN MSD_QD_DEV *dev
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_ECID_ENTRY ecidEntry;
    int temp;

    MSD_DBG_INFO(("Amethyst_gecidDump Called.\n"));

    msdMemSet(&ecidEntry, 0, sizeof(AMETHYST_MSD_ECID_ENTRY));
    ecidEntry.group = 3;
    ecidEntry.ecid = 0xfff;

    /* Print out ECID entry field */
    MSG(("\n-------------------------------------------------------------------\n"));
    MSG(("Group  ECID  PortVec  State  FPri  QPri  RemoveETag  Lag  LagId\n"));
    MSG(("-------------------------------------------------------------------\n"));
    temp = 1;
    while (1 == temp)
    {
        retVal = Amethyst_gecidGetEcidEntryNext(dev, ecidEntry.group, ecidEntry.ecid, &ecidEntry);
        if (MSD_NO_SUCH == retVal)
            break;

        if (MSD_OK != retVal)
        {
            return retVal;
        }

        MSG(("%-5d  %-03x   %-03x       %-5d  %-4d  %-4d  %-10d  %-3d  %-5d\n", ecidEntry.group, ecidEntry.ecid, ecidEntry.portVec, ecidEntry.entryState,
            ecidEntry.macFPri, ecidEntry.macQPri, ecidEntry.removeEtag, ecidEntry.lag, ecidEntry.lagId));

        if (ecidEntry.group == 3 && ecidEntry.ecid == 0xfff)
            break;
    }


    MSD_DBG_INFO(("Amethyst_gecidDump Exit.\n"));
    return MSD_OK;
}

static MSD_STATUS Amethyst_ecidOperationPerform_MultiChip
(
    IN    MSD_QD_DEV *dev,
    IN    AMETHYST_MSD_ATU_OPERATION ecidOp,
    INOUT AMETHYST_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_U16 opcodeData;
    MSD_U16 portMask;

    /*check if the device in Split ATU mode*/
    data = 0;
    retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 15, 1, &data);
    if (retVal != MSD_OK)
        return retVal;
    if (data == 0)
        return MSD_FEATURE_NOT_ENABLE;

    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);
    portMask = (MSD_U16)((1 << dev->maxPorts) - 1);

    /* Wait until the ATU in ready. */
    data = 1;
    while (data == 1)
    {
        retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

    /*Set the ecid mode as 1*/
    retVal = Amethyst_msdDirectSetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, 14, 1, 1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    opcodeData = 0;
    switch (ecidOp)
    {
    case AMETHYST_FLUSH_ALL:
        /*Update the portVec/MoveFrom&MoveTo and entryState register, flush or move acction based on the entryState*/
        data = (MSD_U16)((ecidEntry->lag << 15) | (ecidEntry->portVec << 4) | (ecidEntry->entryState));
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    case AMETHYST_LOAD_PURGE_ENTRY:
        /*Based on the Spec, some field must be 0 when do load operation, load or purge based on the entryState*/
        if (ecidEntry->lag != 0)
            data = ecidEntry->lagId | 0x0100;
        else
            data = 0x0100;
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = 0;
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE + 1, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = (ecidEntry->group << 12) | (ecidEntry->ecid & 0xfff);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE + 2, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        data = ((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & portMask) << 4) | (ecidEntry->entryState & 0xf);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = (data & 0xf7ff) | ((ecidEntry->removeEtag & 0x1) << 11);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        opcodeData |= ((ecidEntry->macQPri & 0x7) << 8) | (ecidEntry->macFPri & 0x7);
        break;
    case AMETHYST_GET_NEXT_ENTRY:
        data = (ecidEntry->group << 12) | (ecidEntry->ecid & 0xfff);
        retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE + 2, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    default:
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return MSD_FAIL;
    }

    /* Set the ATU Operation register  */
    retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }
    data &= 0x0fff;
    if (ecidOp == AMETHYST_LOAD_PURGE_ENTRY)
    {
        data &= 0x0f8;
    }
    opcodeData |= ((1 << 15) | (ecidOp << 12) | data);
    retVal = Amethyst_msdDirectSetMultiChipReg(dev, AMETHYST_QD_REG_ATU_OPERATION, opcodeData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Read back the ecid entry for getNext operation*/
    if (ecidOp == AMETHYST_GET_NEXT_ENTRY)
    {
        /* Wait until the ATU in ready. */
        data = 1;
        while (data == 1)
        {
            retVal = Amethyst_msdDirectGetMultiChipRegField(dev, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_OPERATION, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->macFPri = data & 0x07;
        ecidEntry->macQPri = (data >> 8) & 0x07;

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_DATA_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->lag = (data >> 15) & 0x01;
        ecidEntry->portVec = (data >> 4) & portMask;
        ecidEntry->entryState = data & 0xf;

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->lagId = data & 0x1F;

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_MAC_BASE + 2, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->ecid = data & 0xfff;
        ecidEntry->group = (data >> 12) & 0x03;

        retVal = Amethyst_msdDirectGetMultiChipReg(dev, AMETHYST_QD_REG_ATU_FID_REG_MULTICHIP, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->removeEtag = (data >> 11) & 0x01;
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    return MSD_OK;
}

static MSD_STATUS Amethyst_ecidOperationPerform
(
    IN    MSD_QD_DEV *dev,
    IN    AMETHYST_MSD_ATU_OPERATION ecidOp,
    INOUT AMETHYST_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_U16 opcodeData;
    MSD_U16 portMask;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev))
    {
        return Amethyst_ecidOperationPerform_MultiChip(dev, ecidOp, ecidEntry);
    }

    /*check if the device in Split ATU mode*/
    data = 0;
    retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 15, 1, &data);
    if (retVal != MSD_OK)
        return retVal;
    if (data == 0)
        return MSD_FEATURE_NOT_ENABLE;

    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);
    portMask = (MSD_U16)((1 << dev->maxPorts) - 1);

    /* Wait until the ATU in ready. */
    data = 1;
    while (data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

    /*Set the ecid mode as 1*/
    retVal = msdSetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, 14, 1, 1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    opcodeData = 0;
    switch (ecidOp)
    {
    case AMETHYST_FLUSH_ALL:
        /*Update the portVec/MoveFrom&MoveTo and entryState register, flush or move acction based on the entryState*/
        data = (MSD_U16)((ecidEntry->lag << 15) | (ecidEntry->portVec << 4) | (ecidEntry->entryState));
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    case AMETHYST_LOAD_PURGE_ENTRY:
        /*Based on the Spec, some field must be 0 when do load operation, load or purge based on the entryState*/
        if (ecidEntry->lag != 0)
            data = ecidEntry->lagId | 0x0100;
        else
            data = 0x0100;
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = 0;
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE + 1, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = (ecidEntry->group << 12) | (ecidEntry->ecid & 0xfff);
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE + 2, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        data = ((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & portMask)<< 4) | (ecidEntry->entryState & 0xf);
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = (data & 0xf7ff) | ((ecidEntry->removeEtag & 0x1) << 11);
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        opcodeData |= ((ecidEntry->macQPri & 0x7) << 8) | (ecidEntry->macFPri & 0x7);
        break;
    case AMETHYST_GET_NEXT_ENTRY:
        data = (ecidEntry->group << 12) | (ecidEntry->ecid & 0xfff);
        retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE + 2, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    default:
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return MSD_FAIL;
    }

    /* Set the ATU Operation register  */
    retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }
    data &= 0x0fff;
    if (ecidOp == AMETHYST_LOAD_PURGE_ENTRY)
    {
        data &= 0x0f8;
    }
    opcodeData |= ((1 << 15) | (ecidOp << 12) | data);
    retVal = msdSetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, opcodeData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Read back the ecid entry for getNext operation*/
    if (ecidOp == AMETHYST_GET_NEXT_ENTRY)
    {
        /* Wait until the ATU in ready. */
        data = 1;
        while (data == 1)
        {
            retVal = msdGetAnyRegField(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, 15, 1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_OPERATION, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->macFPri = data & 0x07;
        ecidEntry->macQPri = (data >> 8) & 0x07;

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_DATA_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->lag = (data >> 15) & 0x01;
        ecidEntry->portVec = (data >> 4) & portMask;
        ecidEntry->entryState = data & 0xf;

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->lagId = data & 0x1F;

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_MAC_BASE + 2, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->ecid = data & 0xfff;
        ecidEntry->group = (data >> 12) & 0x03;

        retVal = msdGetAnyReg(dev->devNum, AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_ATU_FID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        ecidEntry->removeEtag = (data >> 11) & 0x01;
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    return MSD_OK;
}