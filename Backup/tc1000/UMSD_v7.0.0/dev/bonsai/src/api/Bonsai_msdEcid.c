/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Bonsai_msdEcid.c
*
* DESCRIPTION:
*       API definitions for ECID Databases 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsai/include/api/Bonsai_msdEcid.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>

static MSD_STATUS Bonsai_ecidOperationPerform
(
    IN    MSD_QD_DEV *dev,
    IN    BONSAI_MSD_ATU_OPERATION atuOp,        
    INOUT BONSAI_MSD_ECID_ENTRY *entry
);

static MSD_STATUS Bonsai_ecidOperationPerform_MultiChip
(
IN    MSD_QD_DEV *dev,
IN    BONSAI_MSD_ATU_OPERATION ecidOp,
INOUT BONSAI_MSD_ECID_ENTRY *ecidEntry
);

static MSD_STATUS Bonsai_gecidGetEntryCount_MultiChip
(
IN MSD_QD_DEV *dev,
OUT MSD_U32 *count
);


/*******************************************************************************
* Bonsai_gBpeEnableSet
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
MSD_STATUS Bonsai_gBpeEnableSet
(
    IN MSD_QD_DEV *dev,
    IN  MSD_BOOL en
)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_U16 tmpdata;

    MSD_DBG_INFO(("Bonsai_gBpeEnableSet Called.\n"));
    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);

    /* Wait until the ATU in ready. */
	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
    {
		if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
		{
			retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		}
		else
		{
			retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		}

        if (retVal != MSD_OK)
        {
			break;
        }
    }

	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("BONSAI_ATU_OPERATION returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		/* translate BOOL to binary */
		MSD_BOOL_2_BIT(en, tmpdata);

		if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
		{
			retVal = Bonsai_msdDirectSetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)15, (MSD_U8)1, tmpdata);
		}
		else
		{
			retVal = msdSetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)15, (MSD_U8)1, tmpdata);
		}

		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdSetAnyRegField for BONSAI_QD_REG_ATU_FID_REG returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    msdSemGive(dev->devNum, dev->atuRegsSem);
    MSD_DBG_INFO(("Bonsai_gBpeEnableSet Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gBpeEnableGet
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
MSD_STATUS Bonsai_gBpeEnableGet
(
    IN MSD_QD_DEV *dev,
    OUT MSD_BOOL *en
)
{
    MSD_STATUS retVal;
	MSD_U16 tmpdata;

    MSD_DBG_INFO(("Bonsai_gBpeEnableGet Called.\n"));

	tmpdata = 0;
	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &tmpdata);
	}
	else
	{
		retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)15, (MSD_U8)1, &tmpdata);
	}

    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("msdGetAnyRegField for BONSAI_ATU_FID_REG returned: %s.\n", msdDisplayStatus(retVal)));
    }
	else
	{
		MSD_BIT_2_BOOL(tmpdata, *en);
	}

    MSD_DBG_INFO(("Bonsai_gBpeEnableGet Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidAddEntry
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
MSD_STATUS Bonsai_gecidAddEntry
(
    IN  MSD_QD_DEV *dev,
    IN  BONSAI_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Bonsai_gecidAddEntry Called.\n"));
    if ((ecidEntry->ecid > (MSD_U16)0xfff) || (ecidEntry->group > (MSD_U8)3))
    {
        MSD_DBG_ERROR(("Bad ecid: %u or group: %u.\n", ecidEntry->ecid, ecidEntry->group));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		/* If this is trunk entry, we need to check trunkId range, it should be within [0, 0xF]*/
		if ((MSD_TRUE == ecidEntry->lag) && (0 == BONSAI_IS_TRUNK_ID_VALID(dev, ecidEntry->lagId)))
		{
			MSD_DBG_ERROR(("Bad TrunkId: %u. It should be within [0, 31].\n", ecidEntry->lagId));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if (ecidEntry->entryState == (MSD_U8)0)
			{
				MSD_DBG_ERROR(("Bad entry state, Entry State should not be ZERO\n"));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				retVal = Bonsai_ecidOperationPerform(dev, BONSAI_LOAD_PURGE_ENTRY, ecidEntry);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_LOAD_PURGE_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidAddEntry Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidFlushAll
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
MSD_STATUS Bonsai_gecidFlushAll
(
    IN MSD_QD_DEV *dev
)
{
    MSD_STATUS retVal;
    BONSAI_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Bonsai_gecidFlushAll Called.\n"));

    msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));
    ecidEntry.entryState = 0;

    retVal = Bonsai_ecidOperationPerform(dev, BONSAI_FLUSH_ALL, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Bonsai_gecidFlushAll Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidMove
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
MSD_STATUS Bonsai_gecidMove
(
    IN MSD_QD_DEV *dev,
    IN  MSD_LPORT moveFrom,
    IN  MSD_LPORT moveTo
)
{
    MSD_STATUS retVal;
    BONSAI_MSD_ECID_ENTRY ecidEntry;
    MSD_U32 tempPort1;
    MSD_U32 tempPort2;

    MSD_DBG_INFO(("Bonsai_gecidMove Called.\n"));

    msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));

	if (moveTo == (MSD_U32)0x1F)
	{
		tempPort1 = moveTo;
	}
	else
	{
		tempPort1 = (MSD_U32)MSD_LPORT_2_PORT(moveTo);
	}

    tempPort2 = (MSD_U32)MSD_LPORT_2_PORT(moveFrom);

    if ((tempPort1 == (MSD_U32)MSD_INVALID_PORT) || (tempPort2 == (MSD_U32)MSD_INVALID_PORT))
    {
        MSD_DBG_ERROR(("Bad Port: moveto %u, moveFrom %u .\n", moveTo, moveFrom));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		ecidEntry.entryState = (MSD_U8)0xf;
		ecidEntry.portVec = (tempPort1 << 5) | tempPort2;

		retVal = Bonsai_ecidOperationPerform(dev, BONSAI_FLUSH_ALL, &ecidEntry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidMove Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidFlushEntry
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
MSD_STATUS Bonsai_gecidFlushEntry
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid
)
{
    MSD_STATUS retVal;
    BONSAI_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Bonsai_gecidFlushEntry Called.\n"));
    if ((ecid > (MSD_U16)0xfff) || (group > (MSD_U8)3))
    {
        MSD_DBG_ERROR(("Bad ecid: %u or group: %u.\n", ecid, group));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));

		ecidEntry.entryState = 0;
		ecidEntry.group = group;
		ecidEntry.ecid = ecid;

		retVal = Bonsai_ecidOperationPerform(dev, BONSAI_LOAD_PURGE_ENTRY, &ecidEntry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_FLUSH_ALL returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidFlushEntry Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidGetEcidEntryNext
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
MSD_STATUS Bonsai_gecidGetEcidEntryNext
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT BONSAI_MSD_ECID_ENTRY* ecidEntry
)
{
    MSD_STATUS retVal;

    MSD_DBG_INFO(("Bonsai_gecidGetEcidEntryNext Called.\n"));
    if ((ecid > (MSD_U16)0xfff) || (group > (MSD_U8)3))
    {
        MSD_DBG_ERROR(("Bad ecid: %u or group: %u.\n", ecid, group));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdMemSet(ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));

		ecidEntry->group = group;
		ecidEntry->ecid = ecid;

		retVal = Bonsai_ecidOperationPerform(dev, BONSAI_GET_NEXT_ENTRY, ecidEntry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			if (ecidEntry->entryState == (MSD_U8)0)
			{
				MSD_DBG_INFO(("No more valid Entry found!.\n"));
				retVal = MSD_NO_SUCH;
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidGetEcidEntryNext Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidFindEcidEntry
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
MSD_STATUS Bonsai_gecidFindEcidEntry
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT BONSAI_MSD_ECID_ENTRY* ecidEntry,
    OUT MSD_BOOL* found
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_DBG_INFO(("Bonsai_gecidFindEcidEntry Called.\n"));
    if ((ecid > (MSD_U16)0xfff) || (group > (MSD_U8)3))
    {
        MSD_DBG_ERROR(("Bad ecid: %u or group: %u.\n", ecid, group));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		*found = MSD_FALSE;
		data = (MSD_U16)((MSD_U16)group << 12) | ecid;
		if (data == (MSD_U16)0)
		{
			data = (MSD_U16)0x3fff;
		}
		else
		{
			data = data - (MSD_U16)1;
		}

		ecidEntry->ecid = data & (MSD_U16)0xfff;
		ecidEntry->group = (MSD_U8)((data >> 12) & (MSD_U16)0x03);

		retVal = Bonsai_gecidGetEcidEntryNext(dev, ecidEntry->group, ecidEntry->ecid, ecidEntry);
		if (MSD_NO_SUCH == retVal)
		{
			MSD_DBG_INFO(("Not found valid entry.\n"));
			retVal = MSD_OK;
		}
		else
		{
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Bonsai_gecidFindEcidEntry returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				if ((ecidEntry->group != group) || (ecidEntry->ecid != ecid))
				{
					MSD_DBG_INFO(("Not found this entry.\n"));
					retVal = MSD_OK;
				}
				else
				{
					*found = MSD_TRUE;
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidFindEcidEntry Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidGetEntryCount
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
static MSD_STATUS Bonsai_gecidGetEntryCount_MultiChip
(
    IN MSD_QD_DEV *dev,
    OUT MSD_U32 *count
)
{
    MSD_STATUS retVal;
    MSD_U16  data;
    MSD_16  bin;
    BONSAI_MSD_ECID_ENTRY ecidEntry;

    MSD_DBG_INFO(("Bonsai_gecidGetEntryCount_MultiChip Called.\n"));

    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count is NULL. \n"));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));

		/*check if the device in Split ATU mode*/
		data = 0;
		retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("ERROR Bonsai_msdDirectGetMultiChipRegField.\n"));
		}
		else
		{
			if (data == (MSD_U16)0)
			{
				retVal = MSD_FEATURE_NOT_ENABLE;
			}
			else
			{
				/*Set the ecid mode as 1*/
				retVal = Bonsai_msdDirectSetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)14, (MSD_U8)1, (MSD_U16)1);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("ERROR Bonsai_msdDirectSetMultiChipRegField.\n"));
				}
				else
				{
					*count = 0;

					retVal = Bonsai_ecidOperationPerform(dev, BONSAI_GET_NEXT_ENTRY, &ecidEntry);
					if (retVal != MSD_OK)
					{
						MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
					}
					else
					{
						for (bin = 4; bin < 8; bin++)
						{
							data = (MSD_U16)((MSD_U16)bin << 11);
							retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_ATU_STATS, data);
							if (retVal != MSD_OK)
							{
								break;
							}
							else
							{
								retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_ATU_STATS, (MSD_U8)0, (MSD_U8)11, &data);
								if (retVal != MSD_OK)
								{
									break;
								}
								else
								{
									*count += (MSD_U32)data;
								}
							}
						}
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Bonsai_gecidGetEntryCount_MultiChip Exit.\n"));
    return retVal;
}

MSD_STATUS Bonsai_gecidGetEntryCount
(
    IN MSD_QD_DEV *dev,
    OUT MSD_U32 *count
)
{
    MSD_STATUS retVal;
    MSD_U16 data;
    MSD_16 bin;
    BONSAI_MSD_ECID_ENTRY ecidEntry;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_gecidGetEntryCount_MultiChip(dev, count);
    }

    MSD_DBG_INFO(("Bonsai_gecidGetEntryCount Called.\n"));
    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));

    /*check if the device in Split ATU mode*/
    data = 0;
	retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)15, (MSD_U8)1, &data);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	if (data == (MSD_U16)0)
	{
		return MSD_FEATURE_NOT_ENABLE;
	}

    /*Set the ecid mode as 1*/
	retVal = msdSetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)14, (MSD_U8)1, (MSD_U16)1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Set the count mode as 0 for ecid count, and then issue the getNext operation*/
    data = 0;
	retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_ATU_STATS, data);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    *count = 0;    
    retVal = Bonsai_ecidOperationPerform(dev, BONSAI_GET_NEXT_ENTRY, &ecidEntry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Bonsai_ecidOperationPerform BONSAI_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    for (bin = 4; bin < 8; bin++)
    {
		data = (MSD_U16)((MSD_U16)bin << 11);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_ATU_STATS, data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
		retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_ATU_STATS, (MSD_U8)0, (MSD_U8)11, &data);
        if (retVal != MSD_OK)
        {
            return retVal;
        }
        *count += data;
    }    

    MSD_DBG_INFO(("Bonsai_gecidGetEntryCount Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Bonsai_gecidDump
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
MSD_STATUS Bonsai_gecidDump
(
    IN MSD_QD_DEV *dev
)
{
    MSD_STATUS retVal;
    BONSAI_MSD_ECID_ENTRY ecidEntry;
    MSD_U16 temp;

    MSD_DBG_INFO(("Bonsai_gecidDump Called.\n"));

    msdMemSet(&ecidEntry, 0, sizeof(BONSAI_MSD_ECID_ENTRY));
	ecidEntry.group = (MSD_U8)3;
	ecidEntry.ecid = (MSD_U16)0xfff;

    /* Print out ECID entry field */
    MSG(("\n-------------------------------------------------------------------\n"));
    MSG(("Group  ECID  PortVec  State  FPri  QPri  RemoveETag  Lag  LagId\n"));
    MSG(("-------------------------------------------------------------------\n"));
	temp = 1U;
	while (1U == temp)
    {
        retVal = Bonsai_gecidGetEcidEntryNext(dev, ecidEntry.group, ecidEntry.ecid, &ecidEntry);
		if (MSD_NO_SUCH == retVal)
		{
			break;
		}

        if (MSD_OK != retVal)
        {
            return retVal;
        }

        MSG(("%-5d  %-03x   %-03x       %-5d  %-4d  %-4d  %-10d  %-3d  %-5d\n", ecidEntry.group, ecidEntry.ecid, ecidEntry.portVec, ecidEntry.entryState,
            ecidEntry.macFPri, ecidEntry.macQPri, ecidEntry.removeEtag, ecidEntry.lag, ecidEntry.lagId));

		if ((ecidEntry.group == (MSD_U8)3) && (ecidEntry.ecid == (MSD_U16)0xfff))
		{
			break;
		}
    }

    MSD_DBG_INFO(("Bonsai_gecidDump Exit.\n"));
    return MSD_OK;
}

static MSD_STATUS Bonsai_ecidOperationPerform_MultiChip
(
    IN    MSD_QD_DEV *dev,
    IN    BONSAI_MSD_ATU_OPERATION ecidOp,
    INOUT BONSAI_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_U16 opcodeData;
    MSD_U32 portMask;

    /*check if the device in Split ATU mode*/
    data = 0;
	retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &data);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	if (data == (MSD_U16)0)
	{
		return MSD_FEATURE_NOT_ENABLE;
	}

    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);
	portMask = (MSD_U32)(((MSD_U32)1 << dev->maxPorts) - (MSD_U32)1);

    /* Wait until the ATU in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

    /*Set the ecid mode as 1*/
	retVal = Bonsai_msdDirectSetMultiChipRegField(dev, BONSAI_ATU_FID_REG_MULTICHIP, (MSD_U8)14, (MSD_U8)1, (MSD_U16)1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    opcodeData = 0;
    switch (ecidOp)
    {
    case BONSAI_FLUSH_ALL:
        /*Update the portVec/MoveFrom&MoveTo and entryState register, flush or move acction based on the entryState*/
		data = (MSD_U16)(((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & (MSD_U16)0x3ff) << 4) | (MSD_U16)ecidEntry->entryState);
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    case BONSAI_LOAD_PURGE_ENTRY:
        /*Based on the Spec, some field must be 0 when do load operation, load or purge based on the entryState*/
		if (ecidEntry->lag == MSD_TRUE)
		{
			data = ecidEntry->lagId | (MSD_U16)0x0100;
		}
		else
		{
			data = (MSD_U16)0x0100;
		}
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_MAC_BASE, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = 0;
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_MAC_BASE + 1U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		data = (MSD_U16)((MSD_U16)ecidEntry->group << 12) | (ecidEntry->ecid & (MSD_U16)0xfff);
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_MAC_BASE + 2U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

		data = (MSD_U16)(((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & (MSD_U16)0x3ff) << 4) | (ecidEntry->entryState & (MSD_U16)0xf));

		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_FID_REG_MULTICHIP, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		data = (data & (MSD_U16)0xf7ff) | (MSD_U16)(((MSD_U16)ecidEntry->removeEtag & (MSD_U16)0x1) << 11);
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_FID_REG_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		opcodeData |= (MSD_U16)((MSD_U16)(ecidEntry->macQPri & (MSD_U16)0x7) << 8) | (ecidEntry->macFPri & (MSD_U16)0x7);
        break;
    case BONSAI_GET_NEXT_ENTRY:
		data = (MSD_U16)((MSD_U16)ecidEntry->group << 12) | (ecidEntry->ecid & (MSD_U16)0xfff);
		retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_MAC_BASE + 2U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    default:
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return MSD_FAIL;
		break;
    }

    /* Set the ATU Operation register  */
	retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }
	data &= (MSD_U16)0x0fff;
    if (ecidOp == BONSAI_LOAD_PURGE_ENTRY)
    {
		data &= (MSD_U16)0x0f8;
    }
	opcodeData |= (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)ecidOp << 12) | data;
	retVal = Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_ATU_OPERATION, opcodeData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Read back the ecid entry for getNext operation*/
    if (ecidOp == BONSAI_GET_NEXT_ENTRY)
    {
        /* Wait until the ATU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = Bonsai_msdDirectGetMultiChipRegField(dev, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_OPERATION, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->macFPri = (MSD_U8)(data & (MSD_U16)0x07);
		ecidEntry->macQPri = (MSD_U8)((data >> 8) & (MSD_U16)0x07);

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_DATA_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->lag = (MSD_BOOL)(MSD_U16)((MSD_U16)(data >> 15) & (MSD_U16)0x1);
		ecidEntry->portVec = ((data & (MSD_U16)0x3ff0) >> 4) & portMask;
		ecidEntry->entryState = (MSD_U8)(data & (MSD_U16)0xf);

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_MAC_BASE, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->lagId = (MSD_U8)(data & (MSD_U16)0x1F);

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_MAC_BASE + 2U, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->ecid = data & (MSD_U16)0xfff;
		ecidEntry->group = (MSD_U8)((data >> 12) & (MSD_U8)0x03);

		retVal = Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_ATU_FID_REG_MULTICHIP, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->removeEtag = (MSD_BOOL)(MSD_U16)((MSD_U16)(data >> 11) & (MSD_U16)0x01);
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    return MSD_OK;
}

static MSD_STATUS Bonsai_ecidOperationPerform
(
    IN    MSD_QD_DEV *dev,
    IN    BONSAI_MSD_ATU_OPERATION ecidOp,
    INOUT BONSAI_MSD_ECID_ENTRY *ecidEntry
)
{
    MSD_STATUS retVal;
    MSD_U16 data;

    MSD_U16 opcodeData;
    MSD_U16 portMask = 0;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_ecidOperationPerform_MultiChip(dev, ecidOp, ecidEntry);
    }

    /*check if the device in Split ATU mode*/
    data = 0;
	retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)15, (MSD_U8)1, &data);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	if (data == (MSD_U16)0)
	{
		return MSD_FEATURE_NOT_ENABLE;
	}

    msdSemTake(dev->devNum, dev->atuRegsSem, OS_WAIT_FOREVER);
	portMask = (MSD_U32)(((MSD_U32)1 << dev->maxPorts) - (MSD_U32)1);

    /* Wait until the ATU in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
    }

    /*Set the ecid mode as 1*/
	retVal = msdSetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, (MSD_U8)14, (MSD_U8)1, (MSD_U16)1);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    opcodeData = 0;
    switch (ecidOp)
    {
    case BONSAI_FLUSH_ALL:
        /*Update the portVec/MoveFrom&MoveTo and entryState register, flush or move acction based on the entryState*/
		data = (MSD_U16)(((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & (MSD_U16)0x3FF) << 4) | (ecidEntry->entryState));
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    case BONSAI_LOAD_PURGE_ENTRY:
        /*Based on the Spec, some field must be 0 when do load operation, load or purge based on the entryState*/
		if (ecidEntry->lag == MSD_TRUE)
		{
			data = ecidEntry->lagId | (MSD_U16)0x0100;
		}
		else
		{
			data = (MSD_U16)0x0100;
		}
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        data = 0;
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE + 1U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		data = (MSD_U16)((MSD_U16)ecidEntry->group << 12) | (ecidEntry->ecid & (MSD_U16)0xfff);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE + 2U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

		data = (MSD_U16)((MSD_U16)((MSD_U16)ecidEntry->lag << 15) | ((ecidEntry->portVec & (MSD_U16)0x3FF) << 4) | (ecidEntry->entryState & (MSD_U16)0xf));

		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_DATA_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		data = (data & (MSD_U16)0xf7ff) | (MSD_U16)(((MSD_U16)ecidEntry->removeEtag & (MSD_U16)0x1) << 11);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		opcodeData |= (MSD_U16)(((MSD_U16)ecidEntry->macQPri & (MSD_U16)0x7) << 8) | (ecidEntry->macFPri & (MSD_U16)0x7);
        break;
    case BONSAI_GET_NEXT_ENTRY:
		data = (MSD_U16)((MSD_U16)ecidEntry->group << 12) | (ecidEntry->ecid & (MSD_U16)0xfff);
		retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE + 2U, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
        break;
    default:
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return MSD_FAIL;
		break;
    }

    /* Set the ATU Operation register  */
	retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }
	data &= (MSD_U16)0x0fff;
    if (ecidOp == BONSAI_LOAD_PURGE_ENTRY)
    {
		data &= (MSD_U16)0x0f8;
    }
	opcodeData |= (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)ecidOp << 12) | data;
	retVal = msdSetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, opcodeData);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->atuRegsSem);
        return retVal;
    }

    /*Read back the ecid entry for getNext operation*/
    if (ecidOp == BONSAI_GET_NEXT_ENTRY)
    {
        /* Wait until the ATU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = msdGetAnyRegField(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->atuRegsSem);
                return retVal;
            }
        }

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_OPERATION, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->macFPri = (MSD_U8)(data & (MSD_U16)0x07);
		ecidEntry->macQPri = (MSD_U8)((data >> 8) & (MSD_U16)0x07);

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_DATA_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->lag = (MSD_BOOL)(MSD_U16)((MSD_U16)(data >> 15) & (MSD_U32)0x1);
		ecidEntry->portVec = ((data & (MSD_U32)0x3ff0) >> 4) & portMask;
		ecidEntry->entryState = (MSD_U8)(data & (MSD_U8)0xf);

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->lagId = (MSD_U8)(data & (MSD_U16)0x1F);

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_MAC_BASE + 2U, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->ecid = data & (MSD_U16)0xfff;
		ecidEntry->group = (MSD_U8)((data >> 12) & (MSD_U16)0x03);

		retVal = msdGetAnyReg(dev->devNum, BONSAI_GLOBAL1_DEV_ADDR, BONSAI_ATU_FID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->atuRegsSem);
            return retVal;
        }
		ecidEntry->removeEtag = (MSD_BOOL)(MSD_U16)((MSD_U16)(data >> 11) & (MSD_U16)0x01);
    }

    msdSemGive(dev->devNum, dev->atuRegsSem);
    return MSD_OK;
}