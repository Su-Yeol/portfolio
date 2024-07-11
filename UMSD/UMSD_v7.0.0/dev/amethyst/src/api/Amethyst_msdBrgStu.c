/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdBrgStu.c
*
* DESCRIPTION:
*       API definitions for SID (VTU 802.1s Port State Information Database) 
*       Translation Unit.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdBrgStu.h>
#include <amethyst/include/api/Amethyst_msdApiInternal.h>
#include <amethyst/include/driver/Amethyst_msdHwAccess.h>
#include <amethyst/include/driver/Amethyst_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

typedef enum
{
    Amethyst_LOAD_PURGE_STU_ENTRY = 5,
    Amethyst_GET_NEXT_STU_ENTRY =6 
} AMETHYST_MSD_STU_OPERATION;

static MSD_STATUS Amethyst_stuOperationPerform
(
    IN      MSD_QD_DEV           *dev,
    IN      AMETHYST_MSD_STU_OPERATION    stuOp,
    INOUT   MSD_U8               *valid,
	INOUT   AMETHYST_MSD_STU_ENTRY        *stuEntry
);

/*******************************************************************************
* Amethyst_gstuGetEntryNext
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
MSD_STATUS Amethyst_gstuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT AMETHYST_MSD_STU_ENTRY  *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port; 
    MSD_LPORT            lport; 
    AMETHYST_MSD_STU_ENTRY        entry;

    MSD_DBG_INFO(("Amethyst_gstuGetEntryNext Called.\n"));
	
    if(stuEntry->sid > 0x3F)
    {		
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
        return MSD_BAD_PARAM;
    }

    entry.sid = stuEntry->sid;
    valid = 0;

    retVal = Amethyst_stuOperationPerform(dev,Amethyst_GET_NEXT_STU_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_stuOperationPerform Amethyst_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrieve the value from the operation */ 

	if ((entry.sid == 0x3F) && (valid == 0))
	{
		MSD_DBG_INFO(("Amethyst_gstuGetEntryNext Exit, MSD_NO_SUCH.\n"));
		return MSD_NO_SUCH;
	}

    stuEntry->sid = entry.sid;

    for(lport=0; lport<dev->numOfPorts; lport++)
    {
        port = MSD_LPORT_2_PORT(lport);
        if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
            return MSD_BAD_PARAM;
        }
        stuEntry->portState[lport]=entry.portState[port];
    }

    MSD_DBG_INFO(("Amethyst_gstuGetEntryNext Exit.\n"));
    return MSD_OK;
}

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
MSD_STATUS Amethyst_gstuAddEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  AMETHYST_MSD_STU_ENTRY    *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8           port; 
    MSD_LPORT           lport; 
    AMETHYST_MSD_STU_ENTRY        entry;

    MSD_DBG_INFO(("Amethyst_gstuAddEntry Called.\n"));

    if(stuEntry->sid > 0x3F)
    {
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
        return MSD_BAD_PARAM;
    }

    entry.sid = stuEntry->sid;

    valid = 1; /* for load operation */

    for(port=0; port<dev->maxPorts; port++)
    {
        lport = MSD_PORT_2_LPORT(port);
        if (lport == MSD_INVALID_PORT)
            entry.portState[port] = 0;
        else if(lport < MSD_MAX_SWITCH_PORTS)
            entry.portState[port] = stuEntry->portState[lport];
        else
        {
            MSD_DBG_ERROR(("Lport number error: %d.\n", lport));
			return MSD_BAD_PARAM;  
        }
    }

    retVal = Amethyst_stuOperationPerform(dev,Amethyst_LOAD_PURGE_STU_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_stuOperationPerform add entry returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Amethyst_gstuAddEntry Exit.\n"));
    return MSD_OK;
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
MSD_STATUS Amethyst_gstuDelEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  AMETHYST_MSD_STU_ENTRY    *stuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    AMETHYST_MSD_STU_ENTRY        entry;

    MSD_DBG_INFO(("Amethyst_gstuDelEntry Called.\n"));
    
    if(stuEntry->sid > 0x3F)
    {
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
        return MSD_BAD_PARAM;
    }

    entry.sid = stuEntry->sid;
    valid = 0; /* for delete operation */

    retVal = Amethyst_stuOperationPerform(dev,Amethyst_LOAD_PURGE_STU_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_stuOperationPerform Delete entry returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }
    MSD_DBG_INFO(("Amethyst_gstuDelEntry Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

static MSD_STATUS Amethyst_stuSetSTUData
(
    IN    MSD_QD_DEV           *dev,
    IN    AMETHYST_MSD_STU_ENTRY        *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data1,data2;           /* Data to be set into the STU data register     */

    data1 = data2 = 0;

	data1 |= ((entry->portState[7] & 3) << 14) | ((entry->portState[6] & 3) << 12) 
		| ((entry->portState[5] & 3) << 10) | ((entry->portState[4] & 3) << 8)
		| ((entry->portState[3] & 3) << 6) | ((entry->portState[2] & 3) << 4)
		| ((entry->portState[1] & 3) << 2) | (entry->portState[0] & 3);

		
	data2 |= ((entry->portState[10] & 3) << 4) | ((entry->portState[9] & 3) << 2)
		| (entry->portState[8] & 3);


	retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR, AMETHYST_QD_REG_VTU_DATA1_REG, data1);
	if(retVal != MSD_OK)
	{
		/*MSD_DBG_ERROR(("Write STU DATA1 Register returned : %s.\n", msdDisplayStatus(retVal)));*/
		return retVal;
	}

	retVal = msdSetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_DATA2_REG, 0, 6, data2);
	if(retVal != MSD_OK)
	{
		/*MSD_DBG_ERROR(("Write STU DATA2 Register returned: %s.\n", msdDisplayStatus(retVal)));*/
		return retVal;
	}

    return retVal;        
}

static MSD_STATUS Amethyst_stuGetSTUData
(
    IN    MSD_QD_DEV           *dev,
    OUT    AMETHYST_MSD_STU_ENTRY       *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data1,data2;           /* Data to be set into the      */

    data1 = data2 = 0;

    msdMemSet((void*)entry->portState,0,sizeof(entry->portState));

    retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_DATA1_REG,&data1);
    if(retVal != MSD_OK)
    {
		/*MSD_DBG_ERROR(("Read STU DATA1 Register returned : %s.\n", msdDisplayStatus(retVal)));*/
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_DATA2_REG,&data2);
    if(retVal != MSD_OK)
    {
		/*MSD_DBG_ERROR(("Read STU DATA2 Register returned: %s.\n", msdDisplayStatus(retVal)));*/
        return retVal;
    }

	entry->portState[10] = (data2 >> 4) & 0x3;
	entry->portState[9] = (data2 >> 2) & 0x3;
	entry->portState[8] = data2 & 0x3;
	
	entry->portState[7] = (data1 >> 14) & 0x3;
	entry->portState[6] = (data1 >> 12) & 0x3;
	entry->portState[5] = (data1 >> 10) & 0x3;
	entry->portState[4] = (data1 >> 8) & 0x3;
	entry->portState[3] = (data1 >> 6) & 0x3;
	entry->portState[2] = (data1 >> 4) & 0x3;
	entry->portState[1] = (data1 >> 2) & 0x3;
	entry->portState[0] = data1 & 0x3;

    return MSD_OK;
}

/*******************************************************************************
* Amethyst_stuOperationPerform
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
static MSD_STATUS Amethyst_stuOperationPerform
(
    IN        MSD_QD_DEV           *dev,
    IN      AMETHYST_MSD_STU_OPERATION    stuOp,
    INOUT   MSD_U8               *valid,
    INOUT    AMETHYST_MSD_STU_ENTRY        *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* Data to be set into the      */
                                /* register.                    */

    msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);

    /* Wait until the VTU in ready. */
    data = 1;
    while(data == 1)
    {
        retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_OPERATION,15,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }
    /* Set the VTU data register if Load operation is required. */
    if (stuOp == Amethyst_LOAD_PURGE_STU_ENTRY)
    {
        if (*valid == 1)
        {
            /* set the Port State for all the ports */
            retVal = Amethyst_stuSetSTUData(dev,entry);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }

            /* Set the valid bit (AMETHYST_QD_REG_VTU_VID_REG) */
            data = (*valid) << 12 ;
            retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,(MSD_U8)(AMETHYST_QD_REG_VTU_VID_REG),data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }        
        }
        else
        {
            /* Clear the valid bit (AMETHYST_QD_REG_VTU_VID_REG) */
			data = 0;
            retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,(MSD_U8)(AMETHYST_QD_REG_VTU_VID_REG),data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }        
        }
    }

    /* Set the SID register (AMETHYST_QD_REG_STU_SID_REG) */
    data= (entry->sid) & 0x3F;
    retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,(MSD_U8)(AMETHYST_QD_REG_STU_SID_REG),data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	
    data = (MSD_U16)((1 << 15) | (stuOp << 12));

    retVal = msdSetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_OPERATION,data);
    if(retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }

    /* If the operation is a Amethyst_get next operation wait for the response   */
    if(stuOp == Amethyst_GET_NEXT_STU_ENTRY)
    {
        /* Wait until the STU in ready. */
        /* Wait until the VTU in ready. */
        data = 1;
        while(data == 1)
        {
            retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_OPERATION,15,1,&data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }
        }

        /****************** Amethyst_get the valid bit *******************/
        retVal = msdGetAnyRegField(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_VTU_VID_REG,12,1,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        *valid = (MSD_U8)data;

        /****************** Amethyst_get the sid *******************/

        retVal = msdGetAnyReg(dev->devNum,  AMETHYST_GLOBAL1_DEV_ADDR,AMETHYST_QD_REG_STU_SID_REG,&data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /* the sid is bits 0-5 */
        entry->sid   = data & 0x3F;

        if (*valid)
        {
            /* Amethyst_get the Port State for all the ports */
            retVal = Amethyst_stuGetSTUData(dev,entry);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }

        } /* entry is valid */

    } /* end of Amethyst_get next entry */

    msdSemGive(dev->devNum, dev->vtuRegsSem);
    return MSD_OK;
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
MSD_STATUS Amethyst_gstuFindSidEntry
(
    IN  MSD_QD_DEV       *dev,
    INOUT AMETHYST_MSD_STU_ENTRY  *stuEntry,
    OUT MSD_BOOL         *found
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port;
    MSD_LPORT            lport;
    AMETHYST_MSD_STU_ENTRY        entry;

    MSD_DBG_INFO(("Amethyst_gstuFindSidEntry Called.\n"));
	
    if(stuEntry->sid > 0x3F)
    {
		MSD_DBG_ERROR(("Bad Sid : %d.\n", stuEntry->sid));
        return MSD_BAD_PARAM;
    }

	if (NULL == found)
	{
		MSD_DBG_ERROR(("Input param MSD_BOOL found is NULL. \n"));
		return MSD_BAD_PARAM;
	}

    *found = MSD_FALSE;

    /* Decrement 1 from sid */
	if (stuEntry->sid == 0)
	{
		entry.sid = 0x3F;
	}
	else 
	{
		entry.sid   = stuEntry->sid-1;
	}

    valid = 0; /* valid is not used as input in this operation */

    retVal = Amethyst_stuOperationPerform(dev,Amethyst_GET_NEXT_STU_ENTRY,&valid, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Amethyst_stuOperationPerform Amethyst_GET_NEXT_STU_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    /* retrive the value from the operation */ 
	if ((entry.sid != stuEntry->sid) || (valid == 0))
	{
		MSD_DBG_INFO(("Amethyst_gstuFindSidEntry Exit, MSD_NO_SUCH\n"));
		return MSD_OK;
	}

    for(lport=0; lport<dev->numOfPorts; lport++)
    {
        port = MSD_LPORT_2_PORT(lport);
        if(port>=MSD_MAX_SWITCH_PORTS )
        {
            MSD_DBG_ERROR(("Port Error Number:%d.\n",port));
            return MSD_BAD_PARAM;
        }
        stuEntry->portState[lport]=entry.portState[port];
    }

    *found = MSD_TRUE;

    MSD_DBG_INFO(("Amethyst_gstuFindSidEntry Exit.\n"));
    return MSD_OK;
}
MSD_STATUS Amethyst_gstuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	AMETHYST_MSD_STU_ENTRY entry;
	int temp;

	msdMemSet(&entry, 0, sizeof(AMETHYST_MSD_STU_ENTRY));
	entry.sid = 0x3f;

	/* Print out ATU entry field */
	MSG(("\n-------------------------\n"));
	MSG(("SID     PORTSTATE         \n"));
	MSG(("-------------------------\n"));

	temp = 1;
	while (1 == temp)
	{
		status = Amethyst_gstuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
			break;

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("%-5x%x %x %x %x %x %x %x %x %x %x %x   \n", entry.sid,
			entry.portState[0], entry.portState[1], entry.portState[2], entry.portState[3], entry.portState[4], entry.portState[5],
			entry.portState[6], entry.portState[7], entry.portState[8], entry.portState[9], entry.portState[10]));

		if (entry.sid == 0x3f)
			break;

	}

	MSG(("\n"));
	return MSD_OK;
}