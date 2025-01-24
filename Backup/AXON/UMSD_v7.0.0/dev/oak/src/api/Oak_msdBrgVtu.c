/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Oak_msdBrgVtu.c
*
* DESCRIPTION:
*       API definitions for Vlan Translation Unit for 802.1Q.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <oak/include/api/Oak_msdBrgVtu.h>
#include <oak/include/api/Oak_msdApiInternal.h>
#include <oak/include/driver/Oak_msdHwAccess.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/****************************************************************************/
/* Forward function declaration.                                            */
/****************************************************************************/

static MSD_STATUS Oak_vtuOperationPerform
(
    IN        MSD_QD_DEV         *dev,
    IN      OAK_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT     OAK_MSD_VTU_ENTRY      *vtuEntry
);

static MSD_STATUS Oak_vtuOperationPerform_MultiChip
(
IN        MSD_QD_DEV         *dev,
IN      OAK_MSD_VTU_OPERATION    vtuOp,
INOUT   MSD_U8               *valid,
INOUT    OAK_MSD_VTU_ENTRY       *entry
);

MSD_STATUS Oak_gvtuGetViolation_MultiChip
(
IN  MSD_QD_DEV         *dev,
OUT OAK_MSD_VTU_INT_STATUS *vtuIntStatus
);

static MSD_STATUS Oak_gvtuSetMode_MultiChip
(
IN  MSD_QD_DEV      *dev,
IN  OAK_MSD_VTU_MODE    mode
);

static MSD_STATUS Oak_gvtuGetMode_MultiChip
(
IN  MSD_QD_DEV     *dev,
OUT OAK_MSD_VTU_MODE   *mode
);


/*******************************************************************************
* Oak_gvtuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid VTU entry from the specified VID.
*
* INPUTS:
*       vtuEntry - the VID and VTUPage to start the search.
*
* OUTPUTS:
*       vtuEntry - match VTU  entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from the VID and VTU Page (13th vid) in the vtuEntry specified by the user.
*
*******************************************************************************/
MSD_STATUS Oak_gvtuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT OAK_MSD_VTU_ENTRY  *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port; 
    MSD_LPORT            lport; 
    OAK_MSD_VTU_ENTRY    entry;

    MSD_DBG_INFO(("Oak_gvtuGetEntryNext Called.\n"));

	if(vtuEntry->vid > (MSD_U16)0xfff)
	{
		MSD_DBG_ERROR(("Bad vid: %u.\n", vtuEntry->vid));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		entry.vid = vtuEntry->vid;
		entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;
		valid = 0;

		retVal = Oak_vtuOperationPerform(dev, OAK_GET_NEXT_ENTRY, &valid, &entry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_vtuOperationPerform OAK_GET_NEXT_ENTRY returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* retrieve the value from the operation */
			if ((entry.vid == (MSD_U16)0xFFF) && (valid == (MSD_U8)0))
			{
				MSD_DBG_INFO(("Oak_gvtuGetEntryNext Exit. MSD_NO_SUCH\n"));
				retVal = MSD_NO_SUCH;
			}
			else
			{
				vtuEntry->DBNum = entry.DBNum;
				vtuEntry->vid = entry.vid;

				vtuEntry->vidPolicy = entry.vidPolicy;
				vtuEntry->sid = entry.sid;

				vtuEntry->vidExInfo.useVIDQPri = entry.vidExInfo.useVIDQPri;
				vtuEntry->vidExInfo.vidQPri = entry.vidExInfo.vidQPri;
				vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
				vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
				vtuEntry->vidExInfo.dontLearn = entry.vidExInfo.dontLearn;
				vtuEntry->vidExInfo.filterUC = entry.vidExInfo.filterUC;
				vtuEntry->vidExInfo.filterBC = entry.vidExInfo.filterBC;
				vtuEntry->vidExInfo.filterMC = entry.vidExInfo.filterMC;
				vtuEntry->vidExInfo.routeDis = entry.vidExInfo.routeDis;
				vtuEntry->vidExInfo.mldSnoop = entry.vidExInfo.mldSnoop;
				vtuEntry->vidExInfo.igmpSnoop = entry.vidExInfo.igmpSnoop;
				vtuEntry->vidExInfo.vtuPage = entry.vidExInfo.vtuPage;

				for (lport = 0; lport < dev->numOfPorts; lport++)
				{
					port = MSD_LPORT_2_PORT(lport);
					if (port >= (MSD_U32)MSD_MAX_SWITCH_PORTS)
					{
						MSD_DBG_ERROR(("Port Error Number:%u.\n", port));
						retVal = MSD_BAD_PARAM;
						break;
					}
					else
					{
						vtuEntry->memberTagP[lport] = entry.memberTagP[port];
					}
				}
			}
		}
	}

    MSD_DBG_INFO(("Oak_gvtuGetEntryNext Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gvtuFindVidEntry
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
MSD_STATUS Oak_gvtuFindVidEntry
(
    IN  MSD_QD_DEV       *dev,
    INOUT OAK_MSD_VTU_ENTRY  *vtuEntry,
    OUT MSD_BOOL         *found
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    MSD_U8               port;
    MSD_LPORT            lport;
    OAK_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Oak_gvtuFindVidEntry Called.\n"));

	if ((NULL == found) || (NULL == vtuEntry))
	{
		MSD_DBG_ERROR(("Input pointer param MSD_BOOL found or vtuentry is NULL.\n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		*found = MSD_FALSE;

		valid = (MSD_U8)0; /* valid is not used as input in this operation */
		if (((MSD_U8)1 == vtuEntry->vidExInfo.vtuPage) && ((MSD_U16)0 == vtuEntry->vid))
		{
			entry.vid = (MSD_U16)0xfff;
			entry.vidExInfo.vtuPage = (MSD_U8)0;
		}
		else if (((MSD_U8)0 == vtuEntry->vidExInfo.vtuPage) && ((MSD_U16)0 == vtuEntry->vid))
		{
			entry.vid = (MSD_U16)0xfff;
			entry.vidExInfo.vtuPage = (MSD_U8)1;
		}
		else
		{
			/* Decrement 1 from vid    */
			entry.vid = vtuEntry->vid - (MSD_U16)1;
			entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;
		}

		retVal = Oak_vtuOperationPerform(dev, OAK_GET_NEXT_ENTRY, &valid, &entry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_vtuOperationPerform vtuFindVidEntry returned: %d).\n", msdDisplayStatus(retVal)));
		}
		else
		{
			/* retrive the value from the operation */
			if ((entry.vid != vtuEntry->vid) || (entry.vidExInfo.vtuPage != vtuEntry->vidExInfo.vtuPage) || (valid != (MSD_U8)1))
			{
				MSD_DBG_INFO(("Oak_gvtuFindVidEntry Exit.\n"));
				retVal = MSD_OK;
			}
			else
			{
				vtuEntry->DBNum = entry.DBNum;
				vtuEntry->vid = entry.vid;

				vtuEntry->vidPolicy = entry.vidPolicy;
				vtuEntry->sid = entry.sid;

				vtuEntry->vidExInfo.useVIDQPri = entry.vidExInfo.useVIDQPri;
				vtuEntry->vidExInfo.vidQPri = entry.vidExInfo.vidQPri;
				vtuEntry->vidExInfo.useVIDFPri = entry.vidExInfo.useVIDFPri;
				vtuEntry->vidExInfo.vidFPri = entry.vidExInfo.vidFPri;
				vtuEntry->vidExInfo.dontLearn = entry.vidExInfo.dontLearn;
				vtuEntry->vidExInfo.filterUC = entry.vidExInfo.filterUC;
				vtuEntry->vidExInfo.filterBC = entry.vidExInfo.filterBC;
				vtuEntry->vidExInfo.filterMC = entry.vidExInfo.filterMC;
				vtuEntry->vidExInfo.routeDis = entry.vidExInfo.routeDis;
				vtuEntry->vidExInfo.mldSnoop = entry.vidExInfo.mldSnoop;
				vtuEntry->vidExInfo.igmpSnoop = entry.vidExInfo.igmpSnoop;
				vtuEntry->vidExInfo.vtuPage = entry.vidExInfo.vtuPage;

				for (lport = 0; lport < dev->numOfPorts; lport++)
				{
					port = MSD_LPORT_2_PORT(lport);
					if (port >= (MSD_U32)MSD_MAX_SWITCH_PORTS)
					{
						MSD_DBG_ERROR(("Port Error Number:%u.\n", port));
						retVal = MSD_BAD_PARAM;
						break;
					}
					else
					{
						vtuEntry->memberTagP[lport] = entry.memberTagP[port];
					}
				}

				*found = MSD_TRUE;
			}
		}
	}

    MSD_DBG_INFO(("Oak_gvtuFindVidEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gvtuFlush
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
MSD_STATUS Oak_gvtuFlush
(
    IN  MSD_QD_DEV       *dev
)
{
    MSD_STATUS       retVal;

    MSD_DBG_INFO(("Oak_gvtuFlush Called.\n"));

    retVal = Oak_vtuOperationPerform(dev,OAK_FLUSH_ALL,NULL,NULL);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Oak_vtuOperationPerform OAK_FLUSH_ALL returned: %d.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Oak_gvtuFlush Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gvtuAddEntry
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
MSD_STATUS Oak_gvtuAddEntry
(
    IN  MSD_QD_DEV   *dev,
    IN OAK_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal = MSD_OK;
    MSD_U8               port; 
    MSD_LPORT            lport; 
    OAK_MSD_VTU_ENTRY    entry;

    MSD_DBG_INFO(("Oak_gvtuAddEntry Called.\n"));

	if ((vtuEntry->vid > (MSD_U16)0xfff) || (vtuEntry->sid > (MSD_U8)0x3f) || (vtuEntry->DBNum > (MSD_U16)0xfff))
	{
		MSD_DBG_ERROR(("Bad vid or sid or DBNum: vid %d sid %d DBNum %d.\n", vtuEntry->vid, vtuEntry->sid, vtuEntry->DBNum));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		entry.DBNum = vtuEntry->DBNum;
		entry.vid = vtuEntry->vid;
		entry.vidPolicy = vtuEntry->vidPolicy;
		entry.sid = vtuEntry->sid;
		entry.vidExInfo.useVIDQPri = vtuEntry->vidExInfo.useVIDQPri;
		entry.vidExInfo.vidQPri = vtuEntry->vidExInfo.vidQPri;
		entry.vidExInfo.useVIDFPri = vtuEntry->vidExInfo.useVIDFPri;
		entry.vidExInfo.vidFPri = vtuEntry->vidExInfo.vidFPri;
		entry.vidExInfo.dontLearn = vtuEntry->vidExInfo.dontLearn;
		entry.vidExInfo.filterUC = vtuEntry->vidExInfo.filterUC;
		entry.vidExInfo.filterBC = vtuEntry->vidExInfo.filterBC;
		entry.vidExInfo.filterMC = vtuEntry->vidExInfo.filterMC;
		entry.vidExInfo.routeDis = vtuEntry->vidExInfo.routeDis;
		entry.vidExInfo.mldSnoop = vtuEntry->vidExInfo.mldSnoop;
		entry.vidExInfo.igmpSnoop = vtuEntry->vidExInfo.igmpSnoop;
		entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;

		valid = (MSD_U8)1; /* for load operation */

		for (port = 0; port < dev->maxPorts; port++)
		{
			lport = MSD_PORT_2_LPORT(port);
			if (lport == (MSD_U32)MSD_INVALID_PORT)
			{
				entry.memberTagP[port] = Oak_NOT_A_MEMBER;
			}
			else if (lport < (MSD_U32)MSD_MAX_SWITCH_PORTS)
			{
				entry.memberTagP[port] = vtuEntry->memberTagP[lport];
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
			retVal = Oak_vtuOperationPerform(dev, OAK_LOAD_PURGE_ENTRY, &valid, &entry);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_vtuOperationPerform load entry returned: %d.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Oak_gvtuAddEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gvtuDelEntry
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
MSD_STATUS Oak_gvtuDelEntry
(
    IN  MSD_QD_DEV   *dev,
    IN OAK_MSD_VTU_ENTRY *vtuEntry
)
{
    MSD_U8               valid;
    MSD_STATUS           retVal;
    OAK_MSD_VTU_ENTRY        entry;

    MSD_DBG_INFO(("Oak_gvtuDelEntry Called.\n"));
    
	if(vtuEntry->vid > (MSD_U16)0xfff)
	{
		MSD_DBG_ERROR(("Bad vid: %u.\n", vtuEntry->vid));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		entry.vid = vtuEntry->vid;
		entry.vidExInfo.vtuPage = vtuEntry->vidExInfo.vtuPage;

		valid = 0; /* for delete operation */

		retVal = Oak_vtuOperationPerform(dev, OAK_LOAD_PURGE_ENTRY, &valid, &entry);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_vtuOperationPerform delete entry returned: %d.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Oak_gvtuDelEntry Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gvtuSetMode
*
* DESCRIPTION:
*       Set VTU Mode
*
* INPUTS:
*       OAK_MSD_VTU_MODE - VTU Mode
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
static MSD_STATUS Oak_gvtuSetMode_MultiChip
(
    IN  MSD_QD_DEV      *dev,
    IN  OAK_MSD_VTU_MODE    mode
)
{
    MSD_STATUS	retVal;
    MSD_U16		data;

    MSD_DBG_INFO(("Oak_gvtuSetMode_MultiChip Called.\n"));

    if ((mode < Oak_VTU_MODE_0) || (mode > Oak_VTU_MODE_3))
    {
        MSD_DBG_ERROR(("Bad mdoe: %d\n", mode));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);

	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            MSD_DBG_ERROR(("Read VTU Operation register busy bit returned: %s", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

    data = (MSD_U16)mode;
	retVal = Oak_msdDirectSetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)10, (MSD_U8)2, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
    msdSemGive(dev->devNum, dev->vtuRegsSem);

    MSD_DBG_INFO(("Oak_gvtuSetMode_MultiChip Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Oak_gvtuSetMode
(
    IN  MSD_QD_DEV      *dev,
    IN  OAK_MSD_VTU_MODE    mode
)
{
	MSD_STATUS	retVal;
	MSD_U16		tmpdata;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_gvtuSetMode_MultiChip(dev, mode);
    }
	
    MSD_DBG_INFO(("Oak_gvtuSetMode Called.\n"));

    if ((mode < Oak_VTU_MODE_0) || (mode > Oak_VTU_MODE_3))
    {
        MSD_DBG_ERROR(("Bad mdoe: %d\n", mode));
        return MSD_BAD_PARAM;
    }

	msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);
		
	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
			MSD_DBG_ERROR(("Read VTU Operation register busy bit returned: %s", msdDisplayStatus(retVal)));
            return retVal;
        }
    }

	tmpdata = (MSD_U16)mode;
	retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)10, (MSD_U8)2, tmpdata);
	if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	msdSemGive(dev->devNum, dev->vtuRegsSem);

	MSD_DBG_INFO(("Oak_gvtuSetMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gvtuGetMode
*
* DESCRIPTION:
*       Get VTU Mode
*
* INPUTS:
*       None
*
* OUTPUTS:
*       OAK_MSD_VTU_MODE - VTU Mode
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static MSD_STATUS Oak_gvtuGetMode_MultiChip
(
    IN  MSD_QD_DEV     *dev,
    OUT OAK_MSD_VTU_MODE   *mode
)
{
    MSD_STATUS	retVal;
	MSD_U16		tmpdata;

    MSD_DBG_INFO(("Oak_gvtuGetMode_MultiChip Called.\n"));
    msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);

	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
    {
		retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)10, (MSD_U8)2, &tmpdata);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	switch (tmpdata)
    {
    case 0:
        *mode = Oak_VTU_MODE_0;
        break;
    case 1U:
        *mode = Oak_VTU_MODE_1;
        break;
    case 2U:
        *mode = Oak_VTU_MODE_2;
        break;
    case 3U:
        *mode = Oak_VTU_MODE_3;
        break;
    default:
		MSD_DBG_ERROR(("Get VTU MODE, tmpdata: %d\n", tmpdata));
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return MSD_FAIL;
		break;
    }
    msdSemGive(dev->devNum, dev->vtuRegsSem);

    MSD_DBG_INFO(("Oak_gvtuGetMode_MultiChip Exit.\n"));
    return MSD_OK;
}
MSD_STATUS Oak_gvtuGetMode
(
    IN  MSD_QD_DEV     *dev,
    OUT OAK_MSD_VTU_MODE   *mode
)
{
	MSD_STATUS	retVal;
	MSD_U16		tmpdata;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_gvtuGetMode_MultiChip(dev, mode);
    }
	
    MSD_DBG_INFO(("Oak_gvtuGetMode Called.\n"));
	msdSemTake(dev->devNum, dev->vtuRegsSem,OS_WAIT_FOREVER);

	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &tmpdata);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)10, (MSD_U8)2, &tmpdata);
	if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Write VTU operation register returned: %d.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }
	switch (tmpdata)
	{
		case 0:
			*mode = Oak_VTU_MODE_0;
			break;
		case 1U:
			*mode = Oak_VTU_MODE_1;
			break;
		case 2U:
			*mode = Oak_VTU_MODE_2;
			break;
		case 3U:
			*mode = Oak_VTU_MODE_3;
			break;
		default:
			MSD_DBG_ERROR(("Get VTU MODE, tmpdata: %d\n", tmpdata));
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return MSD_FAIL;
			break;
	}
	msdSemGive(dev->devNum, dev->vtuRegsSem);

	MSD_DBG_INFO(("Oak_gvtuGetMode Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gvtuGetViolation
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
MSD_STATUS Oak_gvtuGetViolation_MultiChip
(
    IN  MSD_QD_DEV         *dev,
    OUT OAK_MSD_VTU_INT_STATUS *vtuIntStatus
)
{
    MSD_U8               spid;
    MSD_U16              vid;
    MSD_U8				page;
    MSD_U16              intCause;
    MSD_STATUS           retVal;
    OAK_MSD_VTU_ENTRY        entry;
    MSD_U16				tmp;

    MSD_DBG_INFO(("Oak_gvtuGetViolation_MultiChip Called.\n"));

    /* check Violation occurred */
	retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_TOP_LEVEL_INTERRUPT, (MSD_U8)5, (MSD_U8)1, &intCause);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read VTU Violation Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	if (intCause == (MSD_U16)0)
    {
        /* No Violation occurred. */
        vtuIntStatus->vtuIntCause = 0;
        MSD_DBG_INFO(("Oak_gvtuGetViolation_MultiChip Exit, No Violation occurred.\n"));
        return MSD_OK;
    }

    entry.DBNum = 0;

    retVal = Oak_vtuOperationPerform(dev, OAK_SERVICE_VIOLATIONS, NULL, &entry);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Oak_vtuOperationPerform OAK_SERVICE_VIOLATIONS returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_OPERATION, &tmp);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Read VTU OPERATION Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	intCause = (MSD_U16)((tmp >> 4) & (MSD_U16)0x7);
	spid = (MSD_U8)(tmp & (MSD_U8)0x1F);
    vid = entry.vid;
    page = entry.vidExInfo.vtuPage;

	if (spid == (MSD_U8)0x1F)
    {
		vtuIntStatus->vtuIntCause = OAK_MSD_VTU_FULL_VIOLATION;
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = 0;
    }
    else
    {
		vtuIntStatus->vtuIntCause = intCause & (OAK_MSD_VTU_MEMBER_VIOLATION | OAK_MSD_VTU_MISS_VIOLATION);
        vtuIntStatus->spid = spid;
		vtuIntStatus->vid = vid | (MSD_U16)(((MSD_U16)page & (MSD_U16)0x1) << 12);
    }

    MSD_DBG_INFO(("Oak_gvtuGetViolation_MultiChip Exit.\n"));
    return MSD_OK;
}
MSD_STATUS Oak_gvtuGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT OAK_MSD_VTU_INT_STATUS *vtuIntStatus
)
{
    MSD_U8               spid;
    MSD_U16              vid;
	MSD_U8				page;
    MSD_U16              intCause;
    MSD_STATUS           retVal;
    OAK_MSD_VTU_ENTRY        entry;
	MSD_U16				tmp;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
       return Oak_gvtuGetViolation_MultiChip(dev, vtuIntStatus);
    }

    MSD_DBG_INFO(("Oak_gvtuGetViolation Called.\n"));

    /* check Violation occurred */
	retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_STATUS, (MSD_U8)5, (MSD_U8)1, &intCause);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU Violation Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	if (intCause == (MSD_U16)0)
    {
        /* No Violation occurred. */
        vtuIntStatus->vtuIntCause = 0;
		MSD_DBG_INFO(("Oak_gvtuGetViolation Exit, No Violation occurred.\n"));
        return MSD_OK;
    }

    entry.DBNum = 0;

    retVal = Oak_vtuOperationPerform(dev, OAK_SERVICE_VIOLATIONS, NULL, &entry);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Oak_vtuOperationPerform OAK_SERVICE_VIOLATIONS returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, &tmp);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read VTU OPERATION Register returned: %d.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	intCause = (MSD_U16)((tmp >> 4) & (MSD_U16)0x7);
	spid = (MSD_U8)(tmp & (MSD_U8)0x1F);
    vid = entry.vid;
	page = entry.vidExInfo.vtuPage;

	if (spid == (MSD_U8)0x1F)
    {
		vtuIntStatus->vtuIntCause = OAK_MSD_VTU_FULL_VIOLATION;
        vtuIntStatus->spid = spid;
        vtuIntStatus->vid = 0;
    }
    else
    {
		vtuIntStatus->vtuIntCause = intCause & (OAK_MSD_VTU_MEMBER_VIOLATION | OAK_MSD_VTU_MISS_VIOLATION);
        vtuIntStatus->spid = spid;
		vtuIntStatus->vid = vid | (MSD_U16)(((MSD_U16)page & (MSD_U16)0x1) << 12);
    }
    
    MSD_DBG_INFO(("Oak_gvtuGetViolation Exit.\n"));
    return MSD_OK;
}

/****************************************************************************/
/* Internal use functions.                                                  */
/****************************************************************************/

/*******************************************************************************
* Oak_vtuOperationPerform
*
* DESCRIPTION:
*       This function is used by all VTU control functions, and is responsible
*       to write the required operation into the VTU registers.
*
* INPUTS:
*       vtuOp       - The VTU operation bits to be written into the VTU
*                     operation register.
*       DBNum       - DBNum where the given vid belongs to
*       vid         - vlan id
*       valid       - valid bit
*       vtuData     - VTU Data with memberTag information
*
* OUTPUTS:
*       DBNum       - DBNum where the given vid belongs to
*       vid         - vlan id
*       valid       - valid bit
*       vtuData     - VTU Data with memberTag information
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static MSD_STATUS Oak_vtuOperationPerform_MultiChip
(
    IN        MSD_QD_DEV         *dev,
    IN      OAK_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT    OAK_MSD_VTU_ENTRY       *entry
)
{
    MSD_STATUS       retVal;         /* Functions return value. */
    MSD_U16          data;           /* Data to be set into the register */
	MSD_U32          extendData = 0;

    msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);

	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

    /* Set the VTU data register    */
    /* There is no need to setup data reg. on flush, get next, or service violation */
    if ((vtuOp != OAK_FLUSH_ALL) && (vtuOp != OAK_GET_NEXT_ENTRY) && (vtuOp != OAK_SERVICE_VIOLATIONS))
    {
        /****************** VTU DATA 1 REG *******************/

        /* Oak_get data and write to OAK_QD_REG_VTU_DATA1_REG (ports 0 to 3) */
		extendData = (MSD_U32)(((MSD_U32)entry->memberTagP[0] & (MSD_U32)3) | (MSD_U32)(((MSD_U32)entry->memberTagP[1] & (MSD_U32)3) << 2)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[2] & (MSD_U32)3) << 4) | (MSD_U32)(((MSD_U32)entry->memberTagP[3] & (MSD_U32)3) << 6)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[4] & (MSD_U32)3) << 8) | (MSD_U32)(((MSD_U32)entry->memberTagP[5] & (MSD_U32)3) << 10)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[6] & (MSD_U32)3) << 12) | (MSD_U32)(((MSD_U32)entry->memberTagP[7] & (MSD_U32)3) << 14) | (MSD_U32)(((MSD_U32)entry->memberTagP[11] & (MSD_U32)3) << 16));

		retVal = Oak_msdDirectSetMultiChipExtendedReg(dev, OAK_VTU_DATA1_REG, extendData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /****************** VTU DATA 2 REG *******************/
		if (dev->maxPorts > (MSD_U8)8)
        {
			data = (MSD_U16)(((MSD_U16)entry->memberTagP[8] & (MSD_U16)3) | (MSD_U16)(((MSD_U16)entry->memberTagP[9] & (MSD_U16)3) << 2)
				| (MSD_U16)(((MSD_U16)entry->memberTagP[10] & (MSD_U16)3) << 4));

			if (entry->vidExInfo.useVIDQPri == MSD_TRUE)
			{
				data |= (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)(((MSD_U16)entry->vidExInfo.vidQPri & (MSD_U16)0x7) << 12));
			}

			if (entry->vidExInfo.useVIDFPri == MSD_TRUE)
			{
				data |= (MSD_U16)((MSD_U16)1 << 11) | (MSD_U16)(((MSD_U16)entry->vidExInfo.vidFPri & (MSD_U16)0x7) << 8);
			}

			retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_VTU_DATA2_REG, data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }
        }
    }

    /* Set the VID register (OAK_QD_REG_VTU_VID_REG) */
    /* There is no need to setup VID reg. on flush and service violation */
    if ((vtuOp != OAK_FLUSH_ALL) && (vtuOp != OAK_SERVICE_VIOLATIONS))
    {
        data = 0;

		data |= (MSD_U16)((MSD_U16)((MSD_U16)entry->vidExInfo.vtuPage & (MSD_U16)0x1) << 13) | ((entry->vid) & (MSD_U16)0xFFF) | (MSD_U16)((MSD_U16)*valid << 12);
        retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_VTU_VID_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

    /* Set SID, FID, VIDPolicy, if it's Load operation */
	if ((vtuOp == OAK_LOAD_PURGE_ENTRY) && (*valid == (MSD_U8)1))
    {
		data = (MSD_U16)entry->sid & (MSD_U16)0x3F;
		if (entry->vidExInfo.dontLearn == MSD_TRUE)
		{
			data |= (MSD_U16)0x8000;
		}
		if (entry->vidExInfo.filterUC == MSD_TRUE)
		{
			data |= (MSD_U16)0x4000;
		}
		if (entry->vidExInfo.filterBC == MSD_TRUE)
		{
			data |= (MSD_U16)0x2000;
		}
		if (entry->vidExInfo.filterMC == MSD_TRUE)
		{
			data |= (MSD_U16)0x1000;
		}
		if (entry->vidExInfo.routeDis == MSD_TRUE)
		{
			data |= (MSD_U16)0x0400;
		}
		if (entry->vidExInfo.mldSnoop == MSD_TRUE)
		{
			data |= (MSD_U16)0x0200;
		}
		if (entry->vidExInfo.igmpSnoop == MSD_TRUE)
		{
			data |= (MSD_U16)0x0100;
		}

        retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_STU_SID_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

		data = (MSD_U16)((MSD_U16)entry->vidPolicy << 12);

		data |= entry->DBNum & (MSD_U16)0xFFF;

        retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_VTU_FID_REG, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

    /* Start the VTU Operation by defining the DBNum, vtuOp and VTUBusy    */
    /*
    * Flush operation will skip the above two setup (for data and vid), and
    * come to here directly
    */

	retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_OPERATION, &data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }

	data &= (MSD_U16)0xC00;
	data |= (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)vtuOp << 12);

	retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_VTU_OPERATION, data);
    if (retVal != MSD_OK)
    {
        msdSemGive(dev->devNum, dev->vtuRegsSem);
        return retVal;
    }

    /* only two operations need to go through the mess below to Oak_get some data
    * after the operations -  service violation and Oak_get next entry
    */

    /* If the operation is to service violation operation wait for the response   */
    if (vtuOp == OAK_SERVICE_VIOLATIONS)
    {
        /* Wait until the VTU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }
        }

        /* Oak_get the VID that was involved in the violation */
		retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_VID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /* Get the vid - bits 0-11 */
		entry->vid = data & (MSD_U16)0xFFF;

        /* Get the page vid - bit 12 */
		entry->vidExInfo.vtuPage = (MSD_U8)((data & (MSD_U16)0x2000) >> 13);

    } /* end of service violations */

    /* If the operation is a Oak_get next operation wait for the response   */
    if (vtuOp == OAK_GET_NEXT_ENTRY)
    {
        entry->vidExInfo.useVIDQPri = MSD_FALSE;
        entry->vidExInfo.vidQPri = 0;
        entry->vidExInfo.useVIDFPri = MSD_FALSE;
        entry->vidExInfo.vidFPri = 0;
        entry->vidExInfo.vtuPage = 0;

        entry->sid = 0;
        entry->vidPolicy = MSD_FALSE;

        /* Wait until the VTU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->vtuRegsSem);
                return retVal;
            }
        }


        /****************** Oak_get the vid *******************/

		retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_VID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /* the vid is bits 0-11 */
		entry->vid = data & (MSD_U16)0xFFF;

		entry->vidExInfo.vtuPage = (MSD_U8)((data >> 13) & (MSD_U8)1);

        /* the vid valid is bits 12 */
		*valid = (MSD_U8)((data >> 12) & (MSD_U8)1);

		if (*valid == (MSD_U8)0)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return MSD_OK;
        }

        /****************** Oak_get the SID *******************/
        retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_STU_SID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
		entry->sid = (MSD_U8)(data & (MSD_U8)0x3F);

		entry->vidExInfo.dontLearn = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x8000) >> 15) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.filterUC = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x4000) >> 14) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.filterBC = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x2000) >> 13) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.filterMC = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x1000) >> 12) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.routeDis = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x0400) >> 10) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.mldSnoop = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x0200) >> 9) == 1U ? MSD_TRUE : MSD_FALSE;
		entry->vidExInfo.igmpSnoop = (MSD_U16)((MSD_U16)(data & (MSD_U16)0x0100) >> 8) == 1U ? MSD_TRUE : MSD_FALSE;

        retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_FID_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

		entry->vidPolicy = ((MSD_U16)(data >> 12) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;

		entry->DBNum = data & (MSD_U16)0xFFF;

		extendData = 0;
		retVal = Oak_msdDirectGetMultiChipExtendedReg(dev, OAK_VTU_DATA1_REG, &extendData);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }

        /* Oak_get data from data register for ports 0 to 7 */
		entry->memberTagP[0] = (OAK_MSD_MEMTAGP)(MSD_U32)(extendData & (MSD_U32)3);
        entry->memberTagP[1] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 2) & (MSD_U32)3);
        entry->memberTagP[2] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 4) & (MSD_U32)3);
        entry->memberTagP[3] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 6) & (MSD_U32)3);
        entry->memberTagP[4] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 8) & (MSD_U32)3);
        entry->memberTagP[5] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 10) & (MSD_U32)3);
        entry->memberTagP[6] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 12) & (MSD_U32)3);
        entry->memberTagP[7] =  (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 14) & (MSD_U32)3);
		entry->memberTagP[11] = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 16) & (MSD_U32)3);

        /* Oak_get data from data register for ports 8 to 10 */
		retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_VTU_DATA2_REG, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
		entry->memberTagP[8] = (OAK_MSD_MEMTAGP)(MSD_U16)(data & (MSD_U16)3);
		entry->memberTagP[9] = (OAK_MSD_MEMTAGP)(MSD_U16)((MSD_U16)(data >> 2) & (MSD_U16)3);
		entry->memberTagP[10] = (OAK_MSD_MEMTAGP)(MSD_U16)((MSD_U16)(data >> 4) & (MSD_U16)3);

		if ((data & (MSD_U16)0x8000) != (MSD_U16)0)
        {
            entry->vidExInfo.useVIDQPri = MSD_TRUE;
			entry->vidExInfo.vidQPri = (MSD_U8)((data >> 12) & (MSD_U8)0x7);
        }
        else
        {
            entry->vidExInfo.useVIDQPri = MSD_FALSE;
			entry->vidExInfo.useVIDQPri = MSD_FALSE;
        }

		if ((data & (MSD_U16)0x800) != (MSD_U16)0)
        {
            entry->vidExInfo.useVIDFPri = MSD_TRUE;
			entry->vidExInfo.vidFPri = (MSD_U8)((data >> 8) & (MSD_U8)0x7);
        }
        else
        {
            entry->vidExInfo.useVIDFPri = MSD_FALSE;
			entry->vidExInfo.useVIDFPri = MSD_FALSE;
        }
    } /* end of Oak_get next entry */

    msdSemGive(dev->devNum, dev->vtuRegsSem);
    return MSD_OK;
}

static MSD_STATUS Oak_vtuOperationPerform
(
    IN        MSD_QD_DEV         *dev,
    IN      OAK_MSD_VTU_OPERATION    vtuOp,
    INOUT   MSD_U8               *valid,
    INOUT    OAK_MSD_VTU_ENTRY       *entry
)
{
	MSD_STATUS       retVal;         /* Functions return value.*/
	MSD_U16          data;           /* Data to be set into the register */
	MSD_U32          extendData = 0;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_vtuOperationPerform_MultiChip(dev, vtuOp, valid, entry);
    }

	msdSemTake(dev->devNum, dev->vtuRegsSem, OS_WAIT_FOREVER);
  
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->vtuRegsSem);
            return retVal;
        }
    }

	/* Set the VTU data register    */
	/* There is no need to setup data reg. on flush, Oak_get next, or service violation */
	if((vtuOp != OAK_FLUSH_ALL) && (vtuOp != OAK_GET_NEXT_ENTRY) && (vtuOp != OAK_SERVICE_VIOLATIONS))
	{
		/****************** VTU DATA 1 REG *******************/

		/* Oak_get data and write to OAK_QD_REG_VTU_DATA1_REG (ports 0 to 3) */
		extendData = (MSD_U32)(((MSD_U32)entry->memberTagP[0] & (MSD_U32)3) | (MSD_U32)(((MSD_U32)entry->memberTagP[1] & (MSD_U32)3) << 2)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[2] & (MSD_U32)3) << 4) | (MSD_U32)(((MSD_U32)entry->memberTagP[3] & (MSD_U32)3) << 6)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[4] & (MSD_U32)3) << 8) | (MSD_U32)(((MSD_U32)entry->memberTagP[5] & (MSD_U32)3) << 10)
			| (MSD_U32)(((MSD_U32)entry->memberTagP[6] & (MSD_U32)3) << 12) | (MSD_U32)(((MSD_U32)entry->memberTagP[7] & (MSD_U32)3) << 14) | (MSD_U32)(((MSD_U32)entry->memberTagP[11] & (MSD_U32)3) << 16));

		retVal = msdSetAnyExtendedReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_DATA1_REG, extendData);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

	    /****************** VTU DATA 2 REG *******************/
		if(dev->maxPorts > (MSD_U8)8)
		{
			
			data = (MSD_U16)((MSD_U16)((MSD_U16)entry->memberTagP[8] & (MSD_U16)3) | (MSD_U16)(((MSD_U16)entry->memberTagP[9] & (MSD_U16)3) << 2)
				| (MSD_U16)(((MSD_U16)entry->memberTagP[10] & (MSD_U16)3) << 4));

			if (entry->vidExInfo.useVIDQPri == MSD_TRUE)
			{
				data |= (MSD_U16)0x8000 | (MSD_U16)(((MSD_U16)entry->vidExInfo.vidQPri & (MSD_U16)0x7) << 12);
			}

			if (entry->vidExInfo.useVIDFPri == MSD_TRUE)
			{
				data |= (MSD_U16)((MSD_U16)1 << 11) | (MSD_U16)(((MSD_U16)entry->vidExInfo.vidFPri & (MSD_U16)0x7) << 8);
			}

			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_DATA2_REG, data);
			if(retVal != MSD_OK)
			{
			  msdSemGive(dev->devNum, dev->vtuRegsSem);
			  return retVal;
			}
		}
	}

	/* Set the VID register (OAK_QD_REG_VTU_VID_REG) */
	/* There is no need to setup VID reg. on flush and service violation */
	if((vtuOp != OAK_FLUSH_ALL) && (vtuOp != OAK_SERVICE_VIOLATIONS) )
	{
		data = 0;

		data |= (MSD_U16)(((MSD_U16)entry->vidExInfo.vtuPage & (MSD_U16)0x1) << 13) | ((entry->vid) & (MSD_U16)0xFFF) | (MSD_U16)((MSD_U16)*valid << 12);
		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_VID_REG, data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
	}

	/* Set SID, FID, VIDPolicy, if it's Load operation */
	if ((vtuOp == OAK_LOAD_PURGE_ENTRY) && (*valid == (MSD_U8)1))
	{
		data = (MSD_U16)((MSD_U16)entry->sid & (MSD_U16)0x3F);
		if (entry->vidExInfo.dontLearn == MSD_TRUE)
		{
			data |= (MSD_U16)0x8000;
		}
		if (entry->vidExInfo.filterUC == MSD_TRUE)
		{
			data |= (MSD_U16)0x4000;
		}
		if (entry->vidExInfo.filterBC == MSD_TRUE)
		{
			data |= (MSD_U16)0x2000;
		}
		if (entry->vidExInfo.filterMC == MSD_TRUE)
		{
			data |= (MSD_U16)0x1000;
		}
		if (entry->vidExInfo.routeDis == MSD_TRUE)
		{
			data |= (MSD_U16)0x0400;
		}
		if (entry->vidExInfo.mldSnoop == MSD_TRUE)
		{
			data |= (MSD_U16)0x0200;
		}
		if (entry->vidExInfo.igmpSnoop == MSD_TRUE)
		{
			data |= (MSD_U16)0x0100;
		}

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_STU_SID_REG, data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        

		data = (MSD_U16)((MSD_U16)entry->vidPolicy << 12);

		data |= entry->DBNum & (MSD_U16)0xFFF;

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_FID_REG, data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
	}

	/* Start the VTU Operation by defining the DBNum, vtuOp and VTUBusy    */
	/* 
	 * Flush operation will skip the above two setup (for data and vid), and 
	 * come to here directly
	*/

	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, &data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	data &= (MSD_U16)0xC00;
	data |= (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)vtuOp << 12);

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, data);
	if(retVal != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->vtuRegsSem);
		return retVal;
	}

	/* only two operations need to go through the mess below to Oak_get some data 
	* after the operations -  service violation and Oak_get next entry
	*/

	/* If the operation is to service violation operation wait for the response   */
	if(vtuOp == OAK_SERVICE_VIOLATIONS)
	{
		/* Wait until the VTU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
		{
			retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}

		/* Oak_get the VID that was involved in the violation */
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_VID_REG, &data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* Get the vid - bits 0-11 */
		entry->vid = data & (MSD_U16)0xFFF;

		/* Get the page vid - bit 12 */
		entry->vidExInfo.vtuPage = (MSD_U8)((data & (MSD_U16)0x2000) >> 13);

	} /* end of service violations */

	/* If the operation is a Oak_get next operation wait for the response   */
	if(vtuOp == OAK_GET_NEXT_ENTRY)
	{
		entry->vidExInfo.useVIDQPri = MSD_FALSE;
		entry->vidExInfo.vidQPri = 0;
		entry->vidExInfo.useVIDFPri = MSD_FALSE;
		entry->vidExInfo.vidFPri = 0;
		entry->vidExInfo.vtuPage = 0;

		entry->sid = 0;
		entry->vidPolicy = MSD_FALSE;

		/* Wait until the VTU in ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
		{
			retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_OPERATION, (MSD_U8)15, (MSD_U8)1, &data);
			if(retVal != MSD_OK)
			{
				msdSemGive(dev->devNum, dev->vtuRegsSem);
				return retVal;
			}
		}


		/****************** Oak_get the vid *******************/

		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_VID_REG, &data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* the vid is bits 0-11 */
		entry->vid = data & (MSD_U16)0xFFF;

		entry->vidExInfo.vtuPage = (MSD_U8)((data >> 13) & (MSD_U8)0x1);

		/* the vid valid is bits 12 */
		*valid = (MSD_U8)((data >> 12) & (MSD_U8)0x1);

		if (*valid == (MSD_U8)0)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return MSD_OK;
		}

		/****************** Oak_get the SID *******************/
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_STU_SID_REG, &data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}        
		entry->sid = (MSD_U8)(data & (MSD_U8)0x3F);
		
		entry->vidExInfo.dontLearn = (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x8000) >> 15);
		entry->vidExInfo.filterUC =  (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x4000)>>14);
		entry->vidExInfo.filterBC =  (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x2000)>>13);
		entry->vidExInfo.filterMC =  (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x1000)>>12);
		entry->vidExInfo.routeDis =  (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x0400)>>10);
		entry->vidExInfo.mldSnoop =  (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x0200)>>9);
		entry->vidExInfo.igmpSnoop = (MSD_BOOL)(MSD_U16)((MSD_U16)(data & (MSD_U16)0x0100)>>8);

		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_FID_REG, &data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}   
		
		entry->vidPolicy = (MSD_BOOL)(MSD_U16)((MSD_U16)(data >> 12) & (MSD_U16)0x1);
		entry->DBNum = data & (MSD_U16)0xFFF;

		retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_DATA1_REG, &extendData);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}

		/* Oak_get data from data register for ports 0 to 7 */
		entry->memberTagP[0] = (OAK_MSD_MEMTAGP)(MSD_U32)(extendData & (MSD_U32)3);
		entry->memberTagP[1]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 2) & (MSD_U32)3);
		entry->memberTagP[2]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 4) & (MSD_U32)3);
		entry->memberTagP[3]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 6) & (MSD_U32)3);
		entry->memberTagP[4]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 8) & (MSD_U32)3);
		entry->memberTagP[5]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 10) & (MSD_U32)3);
		entry->memberTagP[6]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 12) & (MSD_U32)3);
		entry->memberTagP[7]  = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 14) & (MSD_U32)3);
		entry->memberTagP[11] = (OAK_MSD_MEMTAGP)(MSD_U32)((MSD_U32)(extendData >> 16) & (MSD_U32)3);

		/* Oak_get data from data register for ports 8 to 10 */
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_VTU_DATA2_REG, &data);
		if(retVal != MSD_OK)
		{
			msdSemGive(dev->devNum, dev->vtuRegsSem);
			return retVal;
		}
		entry->memberTagP[8] = (OAK_MSD_MEMTAGP)(MSD_U16)(data & (MSD_U16)3);
		entry->memberTagP[9] = (OAK_MSD_MEMTAGP)(MSD_U16)((MSD_U16)(data >> 2) & (MSD_U16)3);
		entry->memberTagP[10] = (OAK_MSD_MEMTAGP)(MSD_U16)((MSD_U16)(data >> 4) & (MSD_U16)3);

		if ((data & (MSD_U16)0x8000) != (MSD_U16)0)
		{
			entry->vidExInfo.useVIDQPri = MSD_TRUE;
			entry->vidExInfo.vidQPri = (MSD_U8)((data >> 12) & (MSD_U8)0x7);
		}
		else
		{
			entry->vidExInfo.useVIDQPri = MSD_FALSE;
			entry->vidExInfo.vidQPri = (MSD_U8)0;
		}
		
		if ((data & (MSD_U16)0x800) != (MSD_U16)0)
		{
			entry->vidExInfo.useVIDFPri = MSD_TRUE;
			entry->vidExInfo.vidFPri = (MSD_U8)((data >> 8) & (MSD_U8)0x7);
		}
		else
		{
			entry->vidExInfo.useVIDFPri = MSD_FALSE;
			entry->vidExInfo.vidFPri = (MSD_U8)0;
		}
	} /* end of Oak_get next entry */

	msdSemGive(dev->devNum, dev->vtuRegsSem);
	return MSD_OK;
}
MSD_STATUS Oak_gvtuDump
(
IN MSD_QD_DEV    *dev
)
{
	MSD_STATUS status;
	OAK_MSD_VTU_ENTRY entry;
	MSD_U16 temp;

	msdMemSet(&entry, 0, sizeof(OAK_MSD_VTU_ENTRY));
	entry.vidExInfo.vtuPage = (MSD_U8)1;
	entry.vid = (MSD_U16)0xfff;

	/* Print out VTU entry field */
	MSG(("\n-------------------------------------------------------------------------------------------------------------------------------------------\n"));
	MSG(("FID    VID    SID      MEMBERTAGP              VP USEFPRI FPRI USEQPRI QPRI DONTLEARN FILTERUC FILTERBC FILTERMC routeDis MLDSnoop IGMPSnoop \n"));
	MSG(("-------------------------------------------------------------------------------------------------------------------------------------------\n"));

	temp = (MSD_U16)1;
	while ((MSD_U16)1 == temp)
	{
		status = Oak_gvtuGetEntryNext(dev, &entry);
		if (MSD_NO_SUCH == status)
		{
			break;
		}

		if (MSD_OK != status)
		{
			return status;
		}

		MSG(("0x%-5x0x%-5x0x%-5x%x %x %x %x %x %x %x %x %x %x %x %x   %-3x%--8x%-5x%-8x%-5x%-10x%-9x%-9x%-9x%-9x%-9x%-9x\n", entry.DBNum, (entry.vid | (entry.vidExInfo.vtuPage << 12)), entry.sid,
			entry.memberTagP[0], entry.memberTagP[1], entry.memberTagP[2], entry.memberTagP[3], entry.memberTagP[4], entry.memberTagP[5],
			entry.memberTagP[6], entry.memberTagP[7], entry.memberTagP[8], entry.memberTagP[9], entry.memberTagP[10], entry.memberTagP[11], entry.vidPolicy,
			entry.vidExInfo.useVIDFPri, entry.vidExInfo.vidFPri, entry.vidExInfo.useVIDQPri, entry.vidExInfo.vidQPri,
			entry.vidExInfo.dontLearn, entry.vidExInfo.filterUC, entry.vidExInfo.filterBC, entry.vidExInfo.filterMC,entry.vidExInfo.routeDis,entry.vidExInfo.mldSnoop,entry.vidExInfo.igmpSnoop));

		if ((entry.vid == (MSD_U16)0xfff) && (entry.vidExInfo.vtuPage == (MSD_U8)1))
		{
			break;
		}

	}

	MSG(("\n"));
	return MSD_OK;
}