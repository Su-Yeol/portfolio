/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* BonsaiZ1_msdBrgFdbIntf.c
*
* DESCRIPTION:
*       API definitions for Multiple Forwarding Databases 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsaiz1/include/api/BonsaiZ1_msdBrgFdb.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdApiInternal.h>

/*******************************************************************************
* gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
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
*       DBNum in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbAddMacEntryIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_ATU_ENTRY *macEntry
)
{
	BONSAIZ1_MSD_ATU_ENTRY entry;

	if (NULL == macEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in BonsaiZ1_gfdbAddMacEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

    entry.DBNum = macEntry->fid;
    entry.portVec = macEntry->portVec;
    entry.entryState = macEntry->entryState;
    entry.exPrio.macFPri = macEntry->exPrio.macFPri;
    entry.exPrio.macQPri = macEntry->exPrio.macQPri;
    entry.LAG = macEntry->trunkMemberOrLAG;

    msdMemCpy(entry.macAddr.arEther, macEntry->macAddr.arEther, 6);
    
    return BonsaiZ1_gfdbAddMacEntry(dev, &entry);
}

/*******************************************************************************
* gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address starting from the specified Mac Addr 
*		in a particular ATU database (DBNum or FID).
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbGetAtuEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_ATU_ENTRY *atuEntry
)
{
	BONSAIZ1_MSD_ATU_ENTRY entry;
	MSD_STATUS retVal;

	if (NULL == atuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in BonsaiZ1_gfdbGetAtuEntryNextIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	entry.DBNum = atuEntry->fid;
	entry.portVec = atuEntry->portVec;
	entry.entryState = atuEntry->entryState;
	entry.exPrio.macFPri = atuEntry->exPrio.macFPri;
	entry.exPrio.macQPri = atuEntry->exPrio.macQPri;
	entry.LAG = atuEntry->trunkMemberOrLAG;

	msdMemCpy(entry.macAddr.arEther, atuEntry->macAddr.arEther, 6);

	retVal = BonsaiZ1_gfdbGetAtuEntryNext(dev, &entry);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)atuEntry, 0, sizeof(MSD_ATU_ENTRY));
	atuEntry->fid = entry.DBNum;
	atuEntry->portVec = entry.portVec;
	atuEntry->entryState = entry.entryState;
	atuEntry->exPrio.macFPri = entry.exPrio.macFPri;
	atuEntry->exPrio.macQPri = entry.exPrio.macQPri;
	atuEntry->trunkMemberOrLAG = entry.LAG;
	msdMemCpy(atuEntry->macAddr.arEther, entry.macAddr.arEther, 6);

	return retVal;
}


/*******************************************************************************
* BonsaiZ1_gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbFlushIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_FLUSH_CMD flushCmd
)
{
	return BonsaiZ1_gfdbFlush(dev, (BONSAIZ1_MSD_FLUSH_CMD)flushCmd);
}

/*******************************************************************************
* BonsaiZ1_gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*       API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       DBNum     - ATU MAC Address Database Number.
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
*		none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbFlushInDBIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_FLUSH_CMD flushCmd,
IN MSD_U32 DBNum
)
{
	return BonsaiZ1_gfdbFlushInDB(dev, (BONSAIZ1_MSD_FLUSH_CMD)flushCmd, DBNum);
}

/*******************************************************************************
* BonsaiZ1_gfdbMove
*
* DESCRIPTION:
*        This routine moves all or all non-static addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbMoveIntf
(
IN MSD_QD_DEV    *dev,
IN MSD_MOVE_CMD  moveCmd,
IN MSD_LPORT     moveFrom,
IN MSD_LPORT     moveTo
)
{
	return BonsaiZ1_gfdbMove(dev, (BONSAIZ1_MSD_MOVE_CMD)moveCmd, moveFrom, moveTo);
}

/*******************************************************************************
* BonsaiZ1_gfdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or all non-static addresses which are in the particular
*        ATU Database (DBNum) from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        DBNum    - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbMoveInDBIntf
(
IN MSD_QD_DEV     *dev,
IN MSD_MOVE_CMD   moveCmd,
IN MSD_U32        DBNum,
IN MSD_LPORT      moveFrom,
IN MSD_LPORT      moveTo
)
{
	return BonsaiZ1_gfdbMoveInDB(dev, (BONSAIZ1_MSD_MOVE_CMD)moveCmd, DBNum, moveFrom, moveTo);
}

/*******************************************************************************
* BonsaiZ1_gfdbPortRemove
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbPortRemoveIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_LPORT portNum
)
{
    BONSAIZ1_MSD_MOVE_CMD pmoveCmd;

    switch (moveCmd)
    {
    case MSD_MOVE_ALL:
        pmoveCmd = BONSAIZ1_MSD_MOVE_ALL;
        break;
    case MSD_MOVE_ALL_NONSTATIC:
        pmoveCmd = BONSAIZ1_MSD_MOVE_ALL_NONSTATIC;
        break;
    default:
        return MSD_BAD_PARAM;
		break;
    }

    return BonsaiZ1_gfdbMove(dev, pmoveCmd, portNum, (MSD_U32)0x1F);
}

/*******************************************************************************
* BonsaiZ1_gfdbPortRemoveInDB
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port in the
*        specified ATU DBNum.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*        DBNum    - fid
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbPortRemoveInDBIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_U32  DBNum,
    IN  MSD_LPORT  portNum
)
{
    BONSAIZ1_MSD_MOVE_CMD pmoveCmd;

    switch (moveCmd)
    {
    case MSD_MOVE_ALL:
        pmoveCmd = BONSAIZ1_MSD_MOVE_ALL;
        break;
    case MSD_MOVE_ALL_NONSTATIC:
        pmoveCmd = BONSAIZ1_MSD_MOVE_ALL_NONSTATIC;
        break;
    default:
        return MSD_BAD_PARAM;
		break;
    }

    return BonsaiZ1_gfdbMoveInDB(dev, pmoveCmd, DBNum, portNum, (MSD_LPORT)0x1F);
}

/*******************************************************************************
* BonsaiZ1_gfdbGetViolation
*
* DESCRIPTION:
*       Get ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, dbNum and mac address.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbGetViolationIntf
(
IN  MSD_QD_DEV         *dev,
OUT MSD_ATU_INT_STATUS *atuIntStatus
)
{
	BONSAIZ1_MSD_ATU_INT_STATUS atuInt;
	MSD_STATUS       retVal;

	msdMemSet((void*)&atuInt, 0, sizeof(BONSAIZ1_MSD_ATU_INT_STATUS));
	retVal = BonsaiZ1_gfdbGetViolation(dev, &atuInt);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == atuIntStatus)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_INT_STATUS in BonsaiZ1_gfdbGetViolationIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)atuIntStatus, 0, sizeof(MSD_ATU_INT_STATUS));
    atuIntStatus->atuIntCause.ageOutVio = ((atuInt.atuIntCause & BONSAIZ1_MSD_ATU_AGE_OUT_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.memberVio = ((atuInt.atuIntCause & BONSAIZ1_MSD_ATU_MEMBER_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.missVio = ((atuInt.atuIntCause & BONSAIZ1_MSD_ATU_MISS_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
    atuIntStatus->atuIntCause.fullVio = ((atuInt.atuIntCause & BONSAIZ1_MSD_ATU_FULL_VIOLATION) == 0U) ? MSD_FALSE : MSD_TRUE;
	atuIntStatus->fid = atuInt.dbNum;
	atuIntStatus->spid = atuInt.spid;
	msdMemCpy(atuIntStatus->macAddr.arEther, atuInt.macAddr.arEther, 6);
	return retVal;
}

/*******************************************************************************
* BonsaiZ1_gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       macAddr - the Mac address to search.
*       fid     - DBNum used to search
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM  - on bad parameter
*
* COMMENTS:
*        DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbFindAtuMacEntryIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_ETHERADDR *macAddr,
IN  MSD_U32       fid,
OUT MSD_ATU_ENTRY *atuEntry,
OUT MSD_BOOL      *found
)
{
	BONSAIZ1_MSD_ATU_ENTRY entry;
	MSD_STATUS       retVal;

    if (NULL == macAddr)
    {
        MSD_DBG_ERROR(("Input param MSD_ETHERADDR in BonsaiZ1_gfdbFindAtuMacEntryIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

	if (NULL == atuEntry)
	{
		MSD_DBG_ERROR(("Input param MSD_ATU_ENTRY in BonsaiZ1_gfdbFindAtuMacEntryIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
    
    msdMemSet((void*)&entry, 0, sizeof(BONSAIZ1_MSD_ATU_ENTRY));
    entry.DBNum = (MSD_U16)fid;
	msdMemCpy(entry.macAddr.arEther, macAddr, 6);

	retVal = BonsaiZ1_gfdbFindAtuMacEntry(dev, &entry, found);
	if ((MSD_OK != retVal) || (MSD_FALSE == *found))
	{
		return retVal;
	}

	msdMemSet((void*)atuEntry, 0, sizeof(MSD_ATU_ENTRY));
	atuEntry->fid = entry.DBNum;
	atuEntry->portVec = entry.portVec;
	atuEntry->entryState = entry.entryState;
	atuEntry->exPrio.macFPri = entry.exPrio.macFPri;
	atuEntry->exPrio.macQPri = entry.exPrio.macQPri;
	atuEntry->trunkMemberOrLAG = entry.LAG;
	msdMemCpy(atuEntry->macAddr.arEther, entry.macAddr.arEther, 6);
	return retVal;
}

/*******************************************************************************
* gfdbDump
*
* DESCRIPTION:
*       Find all MAC address in the specified fid and print it out.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_gfdbDump
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       fid
)
{
    MSD_STATUS status;
    MSD_ATU_ENTRY entry;
    MSD_16 temp;

    msdMemSet(&entry, 0, sizeof(MSD_ATU_ENTRY));
    
    entry.macAddr.arEther[0] = (MSD_U8)0xff;
    entry.macAddr.arEther[1] = (MSD_U8)0xff;
    entry.macAddr.arEther[2] = (MSD_U8)0xff;
    entry.macAddr.arEther[3] = (MSD_U8)0xff;
    entry.macAddr.arEther[4] = (MSD_U8)0xff;
    entry.macAddr.arEther[5] = (MSD_U8)0xff;
    entry.fid = (MSD_U16)fid;

    /* Print out ATU entry field */
    MSG(("\n--------------------------------------------------------\n"));
    MSG(("FID    MAC          LAG   PORTVEC      STATE  FPRI  QPRI\n"));
    MSG(("--------------------------------------------------------\n"));

    temp = 1;
    while (1 == temp)
    {
        status = BonsaiZ1_gfdbGetAtuEntryNextIntf(dev, &entry);
		if (MSD_NO_SUCH == status)
		{
			break;
		}

        if (MSD_OK != status)
        {
            return status;
        }

        MSG(("0x%-5x%02x%02x%02x%02x%02x%02x  %-5x%x%x%x%x%x%x%x%x%x%x%x%x  %-7x%-6x%-4x\n", entry.fid, entry.macAddr.arEther[0], entry.macAddr.arEther[1], entry.macAddr.arEther[2],
                                                                    entry.macAddr.arEther[3], entry.macAddr.arEther[4], entry.macAddr.arEther[5], entry.trunkMemberOrLAG,
																	((entry.portVec & 0x800) ? 1 : 0), ((entry.portVec & 0x400) ? 1 : 0), ((entry.portVec & 0x200) ? 1 : 0), ((entry.portVec & 0x100) ? 1 : 0),
                                                                    ((entry.portVec & 0x80) ? 1 : 0), ((entry.portVec & 0x40) ? 1 : 0), ((entry.portVec & 0x20) ? 1 : 0),
                                                                    ((entry.portVec & 0x10) ? 1 : 0), ((entry.portVec & 0x8) ? 1 : 0), ((entry.portVec & 0x4) ? 1 : 0),
                                                                    ((entry.portVec & 0x2) ? 1 : 0), ((entry.portVec & 0x1) ? 1 : 0),
                                                                    entry.entryState, entry.exPrio.macFPri, entry.exPrio.macQPri));

        if (BONSAIZ1_IS_BROADCAST_MAC(entry.macAddr))
        {
            break;
        }

    }

    MSG(("\n"));
    return MSD_OK;
}