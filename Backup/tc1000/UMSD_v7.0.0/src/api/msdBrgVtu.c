/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Marvell VTU functionality 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <api/msdBrgVtu.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdVlanEntryFind
*
* DESCRIPTION:
*       Find VTU entry for a specific VID(13bits data for Peridot and 12bits for others),
*       it will return the entry, if found, along with its associated data
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       entry  - pointer to VTU entry structure
*       found  - 1/0 for found / not found
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVlanEntryFind
(
    IN  MSD_U8  devNum,
    IN  MSD_U16 vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
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
		if (dev->SwitchDevObj.VTUObj.gvtuFindVidEntry != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuFindVidEntry(dev, vlanId, entry, found);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdVlanExists
*
* DESCRIPTION:
*       Check if the vlan entry with a specific vlan id exist or not in VTU table.
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       isExists   - 1/0 for exist / not exist
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanExists
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
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
		if (dev->SwitchDevObj.VTUObj.gvtuExistVidEntry != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuExistVidEntry(dev, vlanId, isExists);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdVlanEntryNextGet
*
* DESCRIPTION:
*       Gets next valid vlan entry from the specified vid.
*
* INPUTS:
*       vlanId  - the vlan id to start the search
*
* OUTPUTS:
*       vlanEntry - next valid vlan entry
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*       MSD_NO_SUCH - no more entries
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryNextGet
(
	IN  MSD_U8 devNum,
    IN  MSD_U16 vlanId,
	OUT MSD_VTU_ENTRY *vlanEntry
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
		vlanEntry->vid = vlanId;
		if (dev->SwitchDevObj.VTUObj.gvtuGetEntryNext != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuGetEntryNext(dev, vlanEntry);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdVlanAllDelete
*
* DESCRIPTION:
*       Removes all entries from VTU Table.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanAllDelete
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
		if (dev->SwitchDevObj.VTUObj.gvtuFlush != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuFlush(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanEntryAdd
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       devNum - physical device number
*       entry  - pointer to VTU entry structure
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryAdd
(
    IN  MSD_U8   devNum,
    IN  MSD_VTU_ENTRY *vtuEntry
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
		if (dev->SwitchDevObj.VTUObj.gvtuAddEntry != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuAddEntry(dev, vtuEntry);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanEntryDelete
*
* DESCRIPTION:
*       This routine deletes VTU entry specified by VID in vtuEntry.
*
* INPUTS:
*       devNum - physical device number
*       vlanId - vlan id.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdVlanEntryDelete
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId
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
		if (dev->SwitchDevObj.VTUObj.gvtuDelEntry != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuDelEntry(dev, vlanId);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanModeSet
*
* DESCRIPTION:
*       Sets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*       MSD_VTU_MODE - VTU Mode specified in the enumeration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_VTU_MODE  vtuMode
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
		if (dev->SwitchDevObj.VTUObj.gvtuSetMode != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuSetMode(dev, vtuMode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanModeGet
*
* DESCRIPTION:
*       Gets VTU Mode
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       MSD_VTU_MODE - VTU Mode specified in the enumeration
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVlanModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_MODE  *vtuMode
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
		if (dev->SwitchDevObj.VTUObj.gvtuGetMode != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuGetMode(dev, vtuMode);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanViolationGet
*
* DESCRIPTION:
*       Gets VTU Violation data
*
* INPUTS:
*       devNum       - physical device number
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS msdVlanViolationGet
(
    IN  MSD_U8  devNum,
    OUT MSD_VTU_INT_STATUS  *vtuIntStatus
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
		if (dev->SwitchDevObj.VTUObj.gvtuGetViolation != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuGetViolation(dev, vtuIntStatus);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanEntryCountGet
*
* DESCRIPTION:
*       Gets number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
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
*       none
*
*******************************************************************************/
MSD_STATUS msdVlanEntryCountGet
(
    IN  MSD_U8   devNum,
    OUT MSD_U16  *count
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
		if (dev->SwitchDevObj.VTUObj.gvtuGetEntryCount != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuGetEntryCount(dev, count);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanMemberTagSet
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, update the memberTag
*       to Allow/Block vlan on the port. If it not exist, return MSD_NO_SUCH
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*       memberTag - enum element of MSD_PORT_MEMBER_TAG
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanMemberTagSet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
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
		if (dev->SwitchDevObj.VTUObj.gvtuSetMemberTag != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuSetMemberTag(dev, vlanId, portNum, memberTag);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*******************************************************************************
* msdVlanMemberTagGet
*
* DESCRIPTION:
*       If the entry with this vlan id exist in the VTU table, get the memberTag
*       on the port. If it not exist, return MSD_NO_SUCH.
*
* INPUTS:
*       devNum  - physical device number
*       vlanId  - vlan id
*       portNum - port number
*
* OUTPUTS:
*       memberTag - enum element of MSD_VLAN_MEMBER_TAG
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NO_SUCH   - the entry with this valn id not exist
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS msdVlanMemberTagGet
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
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
		if (dev->SwitchDevObj.VTUObj.gvtuGetMemberTag != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuGetMemberTag(dev, vlanId, portNum, memberTag);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/*******************************************************************************
* msdVtuDump
*
* DESCRIPTION:
*       Finds all VTU Entries and print it out.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
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
MSD_STATUS msdVtuDump
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
		if (dev->SwitchDevObj.VTUObj.gvtuDump != NULL)
		{
			retVal = dev->SwitchDevObj.VTUObj.gvtuDump(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

