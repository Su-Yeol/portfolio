/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdAdvVct.c
*
* DESCRIPTION:
*       API for Marvell Virtual Cable Tester.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <amethyst/include/api/Amethyst_msdBrgVtu.h>

/*******************************************************************************
* Amethyst_gvtuFindVidEntry
*
* DESCRIPTION:
*       Find VTU entry for a specific VID and VTU Page, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       vtuEntry - contains the VID and VTU Page to search for.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       vtuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuFindVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16    vlanId,
    OUT MSD_VTU_ENTRY *entry,
    OUT MSD_BOOL *found
)
{
	MSD_U8               port;
	MSD_LPORT               lport;
	AMETHYST_MSD_VTU_ENTRY tempEntry;
	MSD_STATUS           retVal;

    if (NULL == entry || NULL == found)
	{
		MSD_DBG_ERROR(("Input pointer param MSD_VTU_ENTRY or found in Amethyst_gvtuFindVidEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
    if (vlanId > 0x1fff)
    {
        MSD_DBG_ERROR(("Input bad vlanId. \n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId & 0xfff;
    tempEntry.vidExInfo.vtuPage = (vlanId >> 12) & 0x1;
	

    retVal = Amethyst_gvtuFindVidEntry(dev, &tempEntry, found);

    msdMemSet((void*)entry, 0, sizeof(MSD_VTU_ENTRY));

	if (*found == MSD_FALSE)
		return retVal;
	
	entry->fid = tempEntry.DBNum;
	entry->vid = tempEntry.vid | (tempEntry.vidExInfo.vtuPage << 12);
	entry->vidPolicy = tempEntry.vidPolicy;
	entry->sid = tempEntry.sid;
	entry->vidExInfo.useVIDFPri = tempEntry.vidExInfo.useVIDFPri;
	entry->vidExInfo.vidFPri = tempEntry.vidExInfo.vidFPri;
	entry->vidExInfo.dontLearn = tempEntry.vidExInfo.dontLearn;
	entry->vidExInfo.filterUC = tempEntry.vidExInfo.filterUC;
	entry->vidExInfo.filterBC = tempEntry.vidExInfo.filterBC;
	entry->vidExInfo.filterMC = tempEntry.vidExInfo.filterMC;
	entry->vidExInfo.routeDis = tempEntry.vidExInfo.routeDis;
	entry->vidExInfo.mldSnoop = tempEntry.vidExInfo.mldSnoop;
	entry->vidExInfo.igmpSnoop = tempEntry.vidExInfo.igmpSnoop;
	entry->vidExInfo.useVIDQPri = tempEntry.vidExInfo.useVIDQPri;
	entry->vidExInfo.vidQPri = tempEntry.vidExInfo.vidQPri;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		port = MSD_LPORT_2_PORT(lport);
        if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
            return MSD_BAD_PARAM;
        }

		entry->memberTagP[lport] = tempEntry.memberTagP[port];
	}
	return retVal;
}
/*******************************************************************************
* Amethyst_gvtuExistVidEntryIntf
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
MSD_STATUS Amethyst_gvtuExistVidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    OUT MSD_BOOL *isExists
)
{
    AMETHYST_MSD_VTU_ENTRY tempEntry;
    MSD_STATUS           retVal;

    if (NULL == isExists)
    {
        MSD_DBG_ERROR(("Input pointer param isExists in Amethyst_gvtuExistVidEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }
    if (vlanId > 0x1fff)
    {
        MSD_DBG_ERROR(("Input bad vlanId. \n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId & 0xfff;
    tempEntry.vidExInfo.vtuPage = (vlanId >> 12) & 0x1;

    retVal = Amethyst_gvtuFindVidEntry(dev, &tempEntry, isExists);

    return retVal;
}
/*******************************************************************************
* Amethyst_gvtuFlush
*
* DESCRIPTION:
*       This routine removes all entries from VTU Table.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuFlushIntf
(
    IN  MSD_QD_DEV   *dev
)
{
	return Amethyst_gvtuFlush(dev);
}
/*******************************************************************************
* Amethyst_gvtuAddEntry
*
* DESCRIPTION:
*       Creates the new entry in VTU table based on user input.
*
* INPUTS:
*       vtuEntry - vtu entry to insert to the VTU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuAddEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN MSD_VTU_ENTRY *vtuEntry
)
{
	MSD_U8               port;
	MSD_LPORT               lport;
	AMETHYST_MSD_VTU_ENTRY entry;

	if (NULL == vtuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Amethyst_gvtuAddEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
    if (vtuEntry->vid > 0x1fff)
    {
        MSD_DBG_ERROR(("Bad vid: %d. \n", vtuEntry->vid));
        return MSD_BAD_PARAM;
    }

	entry.DBNum = vtuEntry->fid;
	entry.vid = vtuEntry->vid & 0xfff;
	entry.vidPolicy = vtuEntry->vidPolicy;
	entry.sid = vtuEntry->sid;
	entry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
	entry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;
	entry.vidExInfo.dontLearn = vtuEntry->vidExInfo.dontLearn;
	entry.vidExInfo.filterUC = vtuEntry->vidExInfo.filterUC;
	entry.vidExInfo.filterBC = vtuEntry->vidExInfo.filterBC;
	entry.vidExInfo.filterMC = vtuEntry->vidExInfo.filterMC;
	entry.vidExInfo.routeDis = vtuEntry->vidExInfo.routeDis;
	entry.vidExInfo.mldSnoop = vtuEntry->vidExInfo.mldSnoop;
	entry.vidExInfo.igmpSnoop = vtuEntry->vidExInfo.igmpSnoop;
	entry.vidExInfo.useVIDQPri = vtuEntry->vidExInfo.useVIDQPri;
	entry.vidExInfo.vidQPri = vtuEntry->vidExInfo.vidQPri;
    entry.vidExInfo.vtuPage = (vtuEntry->vid >> 12) & 0x1;

	for (lport = 0; lport<dev->numOfPorts; lport++)
	{
		port = MSD_LPORT_2_PORT(lport);
        if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
            return MSD_BAD_PARAM;
        }

		entry.memberTagP[port] = vtuEntry->memberTagP[lport];
	}

	return Amethyst_gvtuAddEntry(dev, &entry);
}
/*******************************************************************************
* Amethyst_gvtuDelEntry
*
* DESCRIPTION:
*       Deletes VTU entry specified by user.
*
* INPUTS:
*       vtuEntry - the VTU entry to be deleted
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuDelEntryIntf
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_U16 vlanId
)
{
	AMETHYST_MSD_VTU_ENTRY entry;

    if (vlanId > 0x1fff)
    {
        MSD_DBG_ERROR(("Input bad vlanId. \n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&entry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    entry.vid = vlanId & 0xfff;
    entry.vidExInfo.vtuPage = (vlanId >> 12) & 0x1;

	return Amethyst_gvtuDelEntry(dev, &entry);
}
/*******************************************************************************
* Amethyst_gvtuSetMode
*
* DESCRIPTION:
*       Set VTU Mode
*
* INPUTS:
*       AMETHYST_MSD_VTU_MODE - VTU Mode
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuSetModeIntf
(
    IN  MSD_QD_DEV      *dev,
    IN  MSD_VTU_MODE    mode
)
{
	return Amethyst_gvtuSetMode(dev, (AMETHYST_MSD_VTU_MODE)mode);
}
/*******************************************************************************
* Amethyst_gvtuGetMode
*
* DESCRIPTION:
*       Get VTU Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       AMETHYST_MSD_VTU_MODE - VTU Mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuGetModeIntf
(
    IN  MSD_QD_DEV     *dev,
    OUT MSD_VTU_MODE   *mode
)
{
	return Amethyst_gvtuGetMode(dev, (AMETHYST_MSD_VTU_MODE*)mode);
}
/*******************************************************************************
* Amethyst_gvtuGetViolation
*
* DESCRIPTION:
*       Get VTU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       vtuIntStatus - interrupt cause, source portID, and vid.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gvtuGetViolationIntf
(
    IN  MSD_QD_DEV         *dev,
    OUT MSD_VTU_INT_STATUS *vtuIntStatus
)
{
	AMETHYST_MSD_VTU_INT_STATUS vtuint;
	MSD_STATUS           retVal;

    if (NULL == vtuIntStatus)
    {
        MSD_DBG_ERROR(("Input param MSD_VTU_INT_STATUS in Amethyst_gvtuGetViolationIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

	msdMemSet((void*)(&vtuint), 0, sizeof(AMETHYST_MSD_VTU_INT_STATUS));
    msdMemSet((void*)vtuIntStatus, 0, sizeof(MSD_VTU_INT_STATUS));

	retVal = Amethyst_gvtuGetViolation(dev, &vtuint);

	if (MSD_OK != retVal)
		return retVal;

    vtuIntStatus->vtuIntCause.fullVio = (vtuint.vtuIntCause & MSD_VTU_FULL_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
    vtuIntStatus->vtuIntCause.memberVio = (vtuint.vtuIntCause & MSD_VTU_MEMBER_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
    vtuIntStatus->vtuIntCause.missVio = (vtuint.vtuIntCause & MSD_VTU_MISS_VIOLATION) != 0 ? MSD_TRUE : MSD_FALSE;
	vtuIntStatus->spid = vtuint.spid;
	vtuIntStatus->vid = vtuint.vid;

	return retVal;
}
/*******************************************************************************
* Amethyst_gvtuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the VTU table
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numEntries  - number of valid entries
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
MSD_STATUS Amethyst_gvtuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16    *count
)
{
    MSD_STATUS           retVal = MSD_OK;
    AMETHYST_MSD_VTU_ENTRY tempEntry;
    
    if (NULL == count)
    {
        MSD_DBG_ERROR(("Input pointer param count in Amethyst_gvtuGetEntryCountIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    *count = 0;
    msdMemSet((void*)(&tempEntry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    tempEntry.vidExInfo.vtuPage = 1;
    tempEntry.vid = 0xfff;

    while (retVal == MSD_OK)
    {
        retVal = Amethyst_gvtuGetEntryNext(dev, &tempEntry);
        if (retVal == MSD_NO_SUCH)
            break;
        (*count)++;
        if (tempEntry.vid == 0xfff && tempEntry.vidExInfo.vtuPage == 1)
            break;

        if (retVal != MSD_OK)
            return retVal;
    }

    return MSD_OK;
}
/*******************************************************************************
* Amethyst_gvtuSetMemberTagIntf
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
MSD_STATUS Amethyst_gvtuSetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    IN  MSD_PORT_MEMBER_TAG memberTag
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_VTU_ENTRY tempEntry;
    MSD_U8 phyPort; /* Physical port.*/
    MSD_BOOL found;

    phyPort = MSD_LPORT_2_PORT(portNum);
    if (phyPort == MSD_INVALID_PORT || phyPort >= MSD_MAX_SWITCH_PORTS || vlanId > 0x1fff)
    {
        MSD_DBG_ERROR(("Failed (Bad Port or Bad vlanId) in Amethyst_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }
    if (memberTag < MSD_MEMBER_EGRESS_UNMODIFIED || memberTag > MSD_NOT_A_MEMBER)
    {
        MSD_DBG_ERROR(("Failed (Bad memberTag) in Topaz_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId & 0xfff;
    tempEntry.vidExInfo.vtuPage = (vlanId >> 12) & 0x1;

    retVal = Amethyst_gvtuFindVidEntry(dev, &tempEntry, &found);
    if (MSD_OK != retVal)
        return retVal;

    if (found == MSD_FALSE)
        return MSD_NO_SUCH;

    /*Update the memberTag*/
    tempEntry.memberTagP[phyPort] = memberTag;
    retVal = Amethyst_gvtuAddEntry(dev, &tempEntry);

    return retVal;
}
/*******************************************************************************
* Amethyst_gvtuGetMemberTagIntf
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
MSD_STATUS Amethyst_gvtuGetMemberTagIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16  vlanId,
    IN  MSD_LPORT  portNum,
    OUT MSD_PORT_MEMBER_TAG *memberTag
)
{
    MSD_STATUS retVal;
    AMETHYST_MSD_VTU_ENTRY tempEntry;
    MSD_U8 phyPort; /* Physical port.*/
    MSD_BOOL found;

    phyPort = MSD_LPORT_2_PORT(portNum);
    if (phyPort == MSD_INVALID_PORT || phyPort >= MSD_MAX_SWITCH_PORTS || vlanId > 0x1fff)
    {
        MSD_DBG_ERROR(("Failed (Bad Port or Bad vlanId) in Amethyst_gvtuSetMemberTagIntf.\n"));
        return MSD_BAD_PARAM;
    }

    msdMemSet((void*)(&tempEntry), 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
    tempEntry.vid = vlanId & 0xfff;
    tempEntry.vidExInfo.vtuPage = (vlanId >> 12) & 0x1;

    retVal = Amethyst_gvtuFindVidEntry(dev, &tempEntry, &found);
    if (MSD_OK != retVal)
        return retVal;

    if (found == MSD_FALSE)
        return MSD_NO_SUCH;

    /*Get the memberTag*/
    *memberTag = tempEntry.memberTagP[phyPort];

    return retVal;
}

MSD_STATUS Amethyst_gvtuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_VTU_ENTRY *vtuEntry
)
{
	AMETHYST_MSD_VTU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == vtuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_VTU_ENTRY in Amethyst_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	msdMemSet((void*)&entry, 0, sizeof(AMETHYST_MSD_VTU_ENTRY));
	entry.vid = vtuEntry->vid & 0xfff;
	entry.vidExInfo.vtuPage = (vtuEntry->vid >> 12) & 0x1;

	retVal = Amethyst_gvtuGetEntryNext(dev, &entry);
	if (MSD_OK != retVal)
		return retVal;

	msdMemSet((void*)vtuEntry, 0, sizeof(MSD_VTU_ENTRY));
	vtuEntry->vid = entry.vid;
	vtuEntry->fid = entry.DBNum;
	vtuEntry->sid = entry.sid;
	vtuEntry->vidPolicy = entry.vidPolicy;
	vtuEntry->vidExInfo.dontLearn = entry.vidExInfo.dontLearn;
	vtuEntry->vidExInfo.filterBC = entry.vidExInfo.filterBC;
	vtuEntry->vidExInfo.filterMC = entry.vidExInfo.filterMC;
	vtuEntry->vidExInfo.filterUC = entry.vidExInfo.filterUC;
	vtuEntry->vidExInfo.routeDis = entry.vidExInfo.routeDis;
	vtuEntry->vidExInfo.mldSnoop = entry.vidExInfo.mldSnoop;
	vtuEntry->vidExInfo.igmpSnoop = entry.vidExInfo.igmpSnoop;
	vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
	vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
	vtuEntry->vidExInfo.useVIDQPri = entry.vidExInfo.useVIDQPri;
	vtuEntry->vidExInfo.vidQPri = entry.vidExInfo.vidQPri;
	msdMemCpy(vtuEntry->memberTagP, entry.memberTagP, sizeof(entry.memberTagP));

	return retVal;
}

