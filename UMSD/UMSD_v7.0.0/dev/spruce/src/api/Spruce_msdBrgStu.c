/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Spruce_msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID (VTU 802.1s Port State Information Database) 
*       Translation Unit.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdBrgStu.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

typedef enum
{
    Spruce_LOAD_PURGE_STU_ENTRY = 5,
    Spruce_GET_NEXT_STU_ENTRY =6 
} SPRUCE_MSD_STU_OPERATION;

static MSD_STATUS Spruce_stuOperationPerform
(
    IN      MSD_QD_DEV           *dev,
    IN      SPRUCE_MSD_STU_OPERATION    stuOp,
    INOUT   MSD_U8               *valid,
	INOUT   SPRUCE_MSD_STU_ENTRY        *stuEntry
);

static MSD_STATUS Spruce_stuSetSTUData
(
IN    MSD_QD_DEV           *dev,
IN    SPRUCE_MSD_STU_ENTRY        *entry
);

static MSD_STATUS Spruce_stuGetSTUData
(
IN    MSD_QD_DEV           *dev,
OUT    SPRUCE_MSD_STU_ENTRY       *entry
);


/*******************************************************************************
* Spruce_gstuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid STU entry from the specified SID.
*
* INPUTS:
*       stuEntry - the SID to start the search.
*
* OUTPUTS:
*       stuEntry - next STU entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None
*
*******************************************************************************/
MSD_STATUS Spruce_gstuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT SPRUCE_MSD_STU_ENTRY  *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port; 
    MSD_LPORT            lport; 
    SPRUCE_MSD_STU_ENTRY        entry;

    MSD_DBG_INFO(("Spruce_gstuGetEntryNext Called.\n"));
	
	if (stuEntry->sid > (MSD_U16)0x3F)
    {		
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		entry.sid = stuEntry->sid;
		valid = (MSD_U8)0;

		retVal = Spruce_stuOperationPerform(dev, Spruce_GET_NEXT_STU_ENTRY, &valid, &entry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_stuOperationPerform Spruce_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* retrieve the value from the operation */
			if ((entry.sid == (MSD_U16)0x3F) && (valid == (MSD_U8)0))
			{
				MSD_DBG_INFO(("Spruce_gstuGetEntryNext Exit, MSD_NO_SUCH.\n"));
				retVal = MSD_NO_SUCH;
			}
			else
			{
				stuEntry->sid = entry.sid;

				for (lport = 0; lport < dev->numOfPorts; lport++)
				{
					port = MSD_LPORT_2_PORT(lport);
					if (port >= (MSD_U8)MSD_MAX_SWITCH_PORTS)
					{
						MSD_DBG_ERROR(("Port Error Number:%d.\n", port));
						retVal = MSD_BAD_PARAM;
						break;
					}
					else
					{
						stuEntry->portState[lport] = entry.portState[port];
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gstuGetEntryNext Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gstuAddEntry
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
MSD_STATUS Spruce_gstuAddEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  SPRUCE_MSD_STU_ENTRY    *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal = MSD_OK;
    MSD_U8               port; 
    MSD_LPORT            lport; 
    SPRUCE_MSD_STU_ENTRY    entry;

    MSD_DBG_INFO(("Spruce_gstuAddEntry Called.\n"));

    if(stuEntry->sid > (MSD_U16)0x3F)
    {
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		entry.sid = stuEntry->sid;

		valid = (MSD_U8)1; /* for load operation */

		for (port = 0; port < dev->maxPorts; port++)
		{
			lport = MSD_PORT_2_LPORT(port);
			if (lport == (MSD_LPORT)MSD_INVALID_PORT)
			{
				entry.portState[port] = (SPRUCE_MSD_PORT_STP_STATE)0;
			}
			else if (lport < (MSD_LPORT)MSD_MAX_SWITCH_PORTS)
			{
				entry.portState[port] = stuEntry->portState[lport];
			}
			else
			{
				MSD_DBG_ERROR(("Lport number error: %d.\n", lport));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = Spruce_stuOperationPerform(dev, Spruce_LOAD_PURGE_STU_ENTRY, &valid, &entry);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_stuOperationPerform add entry returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Spruce_gstuAddEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gstuDelEntry
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
MSD_STATUS Spruce_gstuDelEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  SPRUCE_MSD_STU_ENTRY    *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    SPRUCE_MSD_STU_ENTRY    entry;

    MSD_DBG_INFO(("Spruce_gstuDelEntry Called.\n"));
    
	if (stuEntry->sid > (MSD_U16)0x3F)
    {
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		entry.sid = stuEntry->sid;
		valid = (MSD_U8)0; /* for delete operation */

		retVal = Spruce_stuOperationPerform(dev, Spruce_LOAD_PURGE_STU_ENTRY, &valid, &entry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Spruce_stuOperationPerform Delete entry returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Spruce_gstuDelEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Spruce_gstuFindSidEntry
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
MSD_STATUS Spruce_gstuFindSidEntry
(
IN  MSD_QD_DEV       *dev,
INOUT SPRUCE_MSD_STU_ENTRY  *stuEntry,
OUT MSD_BOOL         *found
)
{
	MSD_U8               valid;
	MSD_STATUS           retVal = MSD_OK;
	MSD_U8               port;
	MSD_LPORT            lport;
	SPRUCE_MSD_STU_ENTRY    entry;

	MSD_DBG_INFO(("Spruce_gstuFindSidEntry Called.\n"));

	if (stuEntry->sid > (MSD_U16)0x3F)
	{
		MSD_DBG_ERROR(("Bad Sid : %u.\n", stuEntry->sid));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (NULL == found)
		{
			MSD_DBG_ERROR(("Input param MSD_BOOL found is NULL. \n"));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			*found = MSD_FALSE;

			/* Decrement 1 from sid */
			if (stuEntry->sid == (MSD_U16)0)
			{
				entry.sid = (MSD_U16)0x3F;
			}
			else
			{
				entry.sid = stuEntry->sid - (MSD_U16)1;
			}

			valid = 0; /* valid is not used as input in this operation */

			retVal = Spruce_stuOperationPerform(dev, Spruce_GET_NEXT_STU_ENTRY, &valid, &entry);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Spruce_stuOperationPerform Spruce_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
			}
			else
			{
				/* retrive the value from the operation */
				if ((entry.sid != stuEntry->sid) || (valid == (MSD_U8)0))
				{
					MSD_DBG_INFO(("Spruce_gstuFindSidEntry Exit, MSD_NO_SUCH\n"));
					retVal = MSD_OK;
				}
				else
				{
					for (lport = 0; lport < dev->numOfPorts; lport++)
					{
						port = MSD_LPORT_2_PORT(lport);
						if (port >= (MSD_U8)MSD_MAX_SWITCH_PORTS)
						{
							MSD_DBG_ERROR(("Port Error Number:%d.\n", port));
							retVal = MSD_BAD_PARAM;
							break;
						}
						else
						{
							stuEntry->portState[lport] = entry.portState[port];
						}
					}

					*found = MSD_TRUE;
				}
			}
		}
	}

	MSD_DBG_INFO(("Spruce_gstuFindSidEntry Exit.\n"));
	return retVal;
}

MSD_STATUS Spruce_gstuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	SPRUCE_MSD_STU_ENTRY entry;
	MSD_U16 temp;

	msdMemSet(&entry, 0, sizeof(SPRUCE_MSD_STU_ENTRY));
	entry.sid = 0x3f;

	/* Print out ATU entry field */
	MSG(("\n----------------------------\n"));
	MSG(("SID        PORTSTATE         \n"));
	MSG(("----------------------------\n"));

	temp = (MSD_U16)1;
	while ((MSD_U16)1 == temp)
	{
		status = Spruce_gstuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
		{
			break;
		}

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("%-5x%x %x %x %x %x %x %x %x %x %x %x   \n", entry.sid,
			entry.portState[0], entry.portState[1], entry.portState[2], entry.portState[3], entry.portState[4], entry.portState[5],
			entry.portState[6], entry.portState[7], entry.portState[8], entry.portState[9], entry.portState[10], entry.portState[11]));

		if (entry.sid == (MSD_U16)0x3f)
		{
			break;
		}

	}

	MSG(("\n"));
	return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

static MSD_STATUS Spruce_stuSetSTUData
(
    IN    MSD_QD_DEV           *dev,
    IN    SPRUCE_MSD_STU_ENTRY        *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data2 = 0;      /* Data to be set into the STU data register     */
	MSD_U32          data1 = 0;

	data1 |= (MSD_U32)(((MSD_U32)entry->portState[11] & (MSD_U32)3) << 16) | (MSD_U32)(((MSD_U32)entry->portState[7] & (MSD_U32)3) << 14) | (MSD_U32)(((MSD_U32)entry->portState[6] & (MSD_U32)3) << 12)
		| (MSD_U32)(((MSD_U32)entry->portState[5] & (MSD_U32)3) << 10) | (MSD_U32)(((MSD_U32)entry->portState[4] & (MSD_U32)3) << 8)
		| (MSD_U32)(((MSD_U32)entry->portState[3] & (MSD_U32)3) << 6) | (MSD_U32)(((MSD_U32)entry->portState[2] & (MSD_U32)3) << 4)
		| (MSD_U32)(((MSD_U32)entry->portState[1] & (MSD_U32)3) << 2) | (MSD_U32)((MSD_U32)entry->portState[0] & (MSD_U32)3);

		
	data2 |= (MSD_U16)(((MSD_U16)entry->portState[10] & (MSD_U16)3) << 4) | (MSD_U16)(((MSD_U16)entry->portState[9] & (MSD_U16)3) << 2)
		| (MSD_U16)((MSD_U16)entry->portState[8] & (MSD_U16)3);


	retVal = msdSetAnyExtendedReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_DATA1_REG, data1);
	if(retVal != MSD_OK)
	{
		/*MSD_DBG_ERROR(("Write STU DATA1 Register returned : %s.\n", msdDisplayStatus(retVal)));*/
		return retVal;
	}

	retVal = msdSetAnyRegField(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_DATA2_REG, (MSD_U8)0, (MSD_U8)6, data2);
	if(retVal != MSD_OK)
	{
		/*MSD_DBG_ERROR(("Write STU DATA2 Register returned: %s.\n", msdDisplayStatus(retVal)));*/
		return retVal;
	}

    return retVal;        
}

static MSD_STATUS Spruce_stuGetSTUData
(
    IN    MSD_QD_DEV           *dev,
    OUT    SPRUCE_MSD_STU_ENTRY       *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data2 = 0;      /* Data to be set into the      */
	MSD_U32          data1 = 0;

    msdMemSet((void*)entry->portState, 0 ,sizeof(entry->portState));

	retVal = msdGetAnyExtendedReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_DATA1_REG, &data1);
    if(retVal != MSD_OK)
    {
		/*MSD_DBG_ERROR(("Read STU DATA1 Register returned : %s.\n", msdDisplayStatus(retVal)));*/
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_DATA2_REG, &data2);
    if(retVal != MSD_OK)
    {
		/*MSD_DBG_ERROR(("Read STU DATA2 Register returned: %s.\n", msdDisplayStatus(retVal)));*/
        return retVal;
    }

	entry->portState[10] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U16)((MSD_U16)(data2 >> 4) & (MSD_U16)0x3);
	entry->portState[9] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U16)((MSD_U16)(data2 >> 2) & (MSD_U16)0x3);
	entry->portState[8] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U16)(data2 & (MSD_U16)0x3);

	entry->portState[11] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 16) & (MSD_U32)0x3);
	entry->portState[7] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 14) & (MSD_U32)0x3);
	entry->portState[6] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 12) & (MSD_U32)0x3);
	entry->portState[5] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 10) & (MSD_U32)0x3);
	entry->portState[4] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 8) & (MSD_U32)0x3);
	entry->portState[3] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 6) & (MSD_U32)0x3);
	entry->portState[2] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 4) & (MSD_U32)0x3);
	entry->portState[1] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)((MSD_U32)(data1 >> 2) & (MSD_U32)0x3);
	entry->portState[0] = (SPRUCE_MSD_PORT_STP_STATE)(MSD_U32)(data1 & (MSD_U32)0x3);

    return MSD_OK;
}

/*******************************************************************************
* Spruce_stuOperationPerform
*
* DESCRIPTION:
*       This function is used by all STU control functions, and is responsible
*       to write the required operation into the STU registers.
*
* INPUTS:
*       stuOp       - The STU operation bits to be written into the STU
*                     operation register.
*       sid         - sid
*       valid       - valid bit
*       stuData     - STU Data with port state information
*
* OUTPUTS:
*       sid         - sid
*       valid       - valid bit
*       stuData     - STU Data with port state information
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static MSD_STATUS Spruce_stuOperationPerform
(
    IN        MSD_QD_DEV           *dev,
    IN      SPRUCE_MSD_STU_OPERATION    stuOp,
    INOUT   MSD_U8               *valid,
    INOUT    SPRUCE_MSD_STU_ENTRY        *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* Data to be set into the      */
                                /* register.                    */

    msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);

    /* Wait until the VTU in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }
    /* Set the VTU data register if Load operation is required. */
    if (stuOp == Spruce_LOAD_PURGE_STU_ENTRY)
    {
		if (*valid == (MSD_U8)1)
        {
            /* set the Port State for all the ports */
            retVal = Spruce_stuSetSTUData(dev,entry);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }

            /* Set the valid bit (SPRUCE_QD_REG_VTU_VID_REG) */
			data = (MSD_U16)((MSD_U16)*valid << 12);
			retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_VID_REG, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }        
        }
        else
        {
            /* Clear the valid bit (SPRUCE_QD_REG_VTU_VID_REG) */
			data = 0;
			retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_VID_REG, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }        
        }
    }

    /* Set the SID register (SPRUCE_QD_REG_STU_SID_REG) */
	data = (entry->sid) & (MSD_U16)0x3F;
	retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_STU_SID_REG, data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	
	data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)stuOp << 12));

	retVal = msdSetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_OPERATION, data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }

    /* If the operation is a Spruce_get next operation wait for the response   */
    if(stuOp == Spruce_GET_NEXT_STU_ENTRY)
    {
        /* Wait until the STU in ready. */
        /* Wait until the VTU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }
        }

        /****************** Spruce_get the valid bit *******************/
		retVal = msdGetAnyRegField(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_VTU_VID_REG, (MSD_U8)12, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        *valid = (MSD_U8)data;

        /****************** Spruce_get the sid *******************/

		retVal = msdGetAnyReg(dev->devNum, SPRUCE_GLOBAL1_DEV_ADDR, SPRUCE_STU_SID_REG, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /* the sid is bits 0-5 */
		entry->sid = data & (MSD_U16)0x3F;

		if (*valid != (MSD_U8)0)
        {
            /* Spruce_get the Port State for all the ports */
            retVal = Spruce_stuGetSTUData(dev,entry);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }

        } /* entry is valid */

    } /* end of Spruce_get next entry */

    msdSemGive(dev->devNum, dev->vtuRegsSem);
    return MSD_OK;
}