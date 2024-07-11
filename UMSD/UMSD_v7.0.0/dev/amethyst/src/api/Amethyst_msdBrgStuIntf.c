/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdBrgStuIntf.c
*
* DESCRIPTION:
*       API definitions for SID (VTU 802.1s Port State Information Database) 
*       Translation Unit.
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <amethyst/include/api/Amethyst_msdBrgStu.h>

/*******************************************************************************
* Amethyst_gstuAddEntry
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on SID.
*
* INPUTS:
*       stuEntry    - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Amethyst_gstuAddEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_STU_ENTRY    *stuEntry
)
{
	AMETHYST_MSD_STU_ENTRY entry;
	MSD_LPORT lport = 0;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY in Amethyst_gstuAddEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.sid = stuEntry->sid;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		entry.portState[lport] = stuEntry->portState[lport];
	}
	return Amethyst_gstuAddEntry(dev, &entry);
}

MSD_STATUS Amethyst_gstuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_STU_ENTRY *stuEntry
)
{
	AMETHYST_MSD_STU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == stuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Amethyst_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(AMETHYST_MSD_STU_ENTRY));
	entry.sid = stuEntry->sid;

	retVal = Amethyst_gstuGetEntryNext(dev, &entry);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;
	msdMemCpy(stuEntry->portState, entry.portState, sizeof(entry.portState));

	return retVal;
}

/*******************************************************************************
* Amethyst_gstuDelEntry
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       stuEntry - the STU entry to be deleted
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Amethyst_gstuDelEntryIntf
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_U16 sid
)
{
	AMETHYST_MSD_STU_ENTRY entry;

    msdMemSet((void*)(&entry), 0, sizeof(AMETHYST_MSD_STU_ENTRY));
	entry.sid = sid;

	return Amethyst_gstuDelEntry(dev, &entry);
}

/*******************************************************************************
* Amethyst_gstuFindSidEntry
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       stuEntry - contains the SID to searche for
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       stuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no such entry.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        Valid SID is 0 ~ 63.
*
*******************************************************************************/
MSD_STATUS Amethyst_gstuFindSidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16 sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
)
{
	MSD_STATUS retVal;
	AMETHYST_MSD_STU_ENTRY entry;
	MSD_LPORT lport = 0;

	if (NULL == stuEntry || NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_STU_ENTRY or found in Amethyst_gstuFindSidEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.sid = sid;

	retVal = Amethyst_gstuFindSidEntry(dev, &entry, found);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)stuEntry, 0, sizeof(MSD_STU_ENTRY));
	stuEntry->sid = entry.sid;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		stuEntry->portState[lport] = entry.portState[lport];
	}

	return retVal;
}

/*******************************************************************************
* Amethyst_gstuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the STU table
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       count  - number of valid entries
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Amethyst_gstuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16 *count
)
{
    MSD_STATUS retVal = MSD_OK;
    AMETHYST_MSD_STU_ENTRY entry;

    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input param count in Amethyst_gstuGetEntryCountIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    *count = 0;
    msdMemSet((void*)(&entry), 0, sizeof(AMETHYST_MSD_STU_ENTRY));
    entry.sid = 0x3f;

    while (retVal == MSD_OK)
    {
        retVal = Amethyst_gstuGetEntryNext(dev, &entry);
        if (retVal == MSD_NO_SUCH)
            break;
        (*count)++;
        if (entry.sid == 0x3f)
            break;

        if (retVal != MSD_OK)
            return retVal;
    }

    return MSD_OK;
}
