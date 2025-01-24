/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Oak_msdSysCtrl.c
*
* DESCRIPTION:
*       API definitions for system global control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdSysCtrl.h>
#include <oak/include/api/Oak_msdApiInternal.h>
#include <oak/include/driver/Oak_msdHwAccess.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

static MSD_STATUS Oak_gsysGetSMISetup_MultiChip
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		portNum,
OUT MSD_U16			*data
);

static MSD_STATUS Oak_gsysSetSMISetup_MultiChip
(
IN MSD_QD_DEV    *dev,
IN MSD_LPORT  portNum,
IN MSD_U16		  data
);


/*******************************************************************************
* Oak_gsysSetTrunkMaskTable
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       maskNum - Mask Number (0~7).
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                   bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetTrunkMaskTable
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       maskNum,
    IN MSD_U32       trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32          mask;
	MSD_U32          tmpData = 0;
    MSD_U16          count = 0x10;

    MSD_DBG_INFO(("Oak_gsysSetTrunkMaskTable Called.\n"));

	if (maskNum >= OAK_MAX_LAG_MASK_NUM)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %u]\n", maskNum, OAK_MAX_LAG_MASK_NUM - 1U));
        return MSD_BAD_PARAM;
    }

	mask = (MSD_U32)(((MSD_U32)1 << dev->numOfPorts) - 1U);
    if(trunkMask > mask)
    {
        MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkMask: 0x%x). It should be within [0, 0x%x]\n", trunkMask, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == 0U)
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((data & (MSD_U16)0x8000) != 0U);

	/* Set related register */
	data &= (MSD_U16)0x0800; /* maintain existing HasHash bit.*/
	mask = MSD_LPORTVEC_2_PORTVEC(trunkMask);
	tmpData = (MSD_U32)((MSD_U32)0x8000 | (MSD_U32)data | (maskNum << 12) | (MSD_U32)(mask & (MSD_U32)0x7ff) | (MSD_U32)(((mask & (MSD_U32)0x0800) >> 11) << 16));
	retVal = msdSetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, tmpData);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Oak_gsysSetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Oak_gsysGetTrunkMaskTable
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                   bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if maskNum > 0x7)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetTrunkMaskTable
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       maskNum,
    OUT MSD_U32       *trunkMask
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32          mask;
	MSD_U32          tmpData = 0;
    MSD_U16          count = 0x10;

    MSD_DBG_INFO(("Oak_gsysGetTrunkMaskTable Called.\n"));

	if (maskNum >= OAK_MAX_LAG_MASK_NUM)
	{
		MSD_DBG_ERROR(("MSD_BAD_PARAM (maskNum: %u). It should be within [0, %d]\n", maskNum, OAK_MAX_LAG_MASK_NUM - 1));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mask tbl register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == 0U)
		{
			MSD_DBG_ERROR(("Wait G2 trunk mask tbl register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((data & (MSD_U16)0x8000) != 0U);

	/* Set related register */
	data &= (MSD_U16)0x0800;
	data = (MSD_U16)(data | (MSD_U16)((MSD_U16)maskNum << 12));
	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* read-back value */
	retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, &tmpData);
    msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
    }

	mask = (MSD_U32)(((MSD_U32)1 << dev->maxPorts) - 1U);
	data = (MSD_U16)(((MSD_U32)tmpData & (MSD_U32)0x7ff) | ((((MSD_U32)tmpData & (MSD_U32)0x10000) >> 16) << 11));

    *trunkMask = MSD_PORTVEC_2_LPORTVEC(data & mask);

    MSD_DBG_INFO(("Oak_gsysGetTrunkMaskTable Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Oak_gsysSetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toOak_gether to 
*       select the TrunkMask to use.
*
* INPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetHashTrunk
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16            data;

    MSD_DBG_INFO(("Oak_gsysSetHashTrunk Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, (MSD_U8)11, (MSD_U8)1, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Oak_gsysSetHashTrunk Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysGetHashTrunk
*
* DESCRIPTION:
*       Hash DA & SA for TrunkMask selection. Trunk load balancing is accomplished 
*       by using the frame's DA and SA fields to access one of eight Trunk Masks. 
*       When this bit is set to a one the hashed computed for address table 
*       lookups is used for the TrunkMask selection. When this bit is cleared to 
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed toOak_gether to 
*       select the TrunkMask to use.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetHashTrunk
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Oak_gsysGetHashTrunk Called.\n"));

    /* Get related bit */
	retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_MASK_TBL, (MSD_U8)11, (MSD_U8)1, &data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 trunk mask tbl returned: %s.\n", msdDisplayStatus(retVal)));
    }
	else
	{
		MSD_BIT_2_BOOL(data, *en);
	}

    MSD_DBG_INFO(("Oak_gsysGetHashTrunk Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysSetTrunkRouting
*
* DESCRIPTION:
*       This function sets routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetTrunkRouting
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32       trunkId,
    IN MSD_U32       trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32          mask;
	MSD_U32          tmpData = 0;
	MSD_U16          count = (MSD_U16)0x10;

    MSD_DBG_INFO(("Oak_gsysSetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0x1f] */
	if (OAK_IS_TRUNK_ID_VALID(dev, trunkId) != 1)
    {
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", trunkId));
        return MSD_BAD_PARAM;
    }

	/* Check if trunkRoute is our of range [0,0xfff] */
	mask = (MSD_U32)(((MSD_U32)1 << dev->numOfPorts) - 1U);
    if(trunkRoute > mask)
    {
		MSD_DBG_ERROR(("MSD_BAD_PARAM (trunkRoute: 0x%x). It should be within [0, 0x%x]\n", trunkRoute, mask));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem,OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == 0U)
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((data & (MSD_U16)0x8000) != 0U);

	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)(((MSD_U16)trunkId & (MSD_U16)0x10) >> 4);
	retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MISC, (MSD_U8)13, (MSD_U8)1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	mask = MSD_LPORTVEC_2_PORTVEC(trunkRoute);
	tmpData = (MSD_U32)((MSD_U32)0x8000 | (((MSD_U32)trunkId & (MSD_U32)0xf) << 11) | (mask & (MSD_U32)0x7ff) | (((mask & (MSD_U32)0x0800) >> 11) << 16));
	retVal = msdSetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_ROUTING, tmpData);

	msdSemGive(dev->devNum, dev->tblRegsSem);

	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    MSD_DBG_INFO(("Oak_gsysSetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Oak_gsysGetTrunkRouting
*
* DESCRIPTION:
*       This function retrieves routing information for the given Trunk ID.
*
* INPUTS:
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkRoute - Trunk route bits. Bit 0 controls trunk routing for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (if trunkId > 0x1F)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetTrunkRouting
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U32       trunkId,
    OUT MSD_U32       *trunkRoute
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
    MSD_U32          mask;
	MSD_U32          tmpData = 0;
    MSD_U16           count = 0x10;

    MSD_DBG_INFO(("Oak_gsysGetTrunkRouting Called.\n"));

	/* Check if trunkId is out of range [0,0x1f] */
	if (OAK_IS_TRUNK_ID_VALID(dev, trunkId) != 1)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (trunkId: %u)\n", trunkId));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    /* Check if the register can be accessed. */
    do
    {
		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_ROUTING, &data);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read G2 trunk mapping register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == 0U)
		{
			MSD_DBG_ERROR(("Wait G2 trunk mapping register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((data & (MSD_U16)0x8000) != 0U);

	/* Write TrunkId[4] in Global 2, Misc Register 0x1D, bit 13 */
	data = (MSD_U16)(((MSD_U16)trunkId & (MSD_U16)0x10) >> 4);
	retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MISC, (MSD_U8)13, (MSD_U8)1, data);
	if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 misc register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
	}

	/* Set related register */
	data = (MSD_U16)(((MSD_U16)trunkId & (MSD_U16)0xf) << 11);
	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_ROUTING, data);
    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdSetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return retVal;
    }

	/* Read-back value */
	retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_TRUNK_ROUTING, &tmpData);

	msdSemGive(dev->devNum, dev->tblRegsSem);

    if(retVal != MSD_OK)
    {
	    MSD_DBG_ERROR(("msdGetAnyReg for G2 trunk mapping register returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

	mask = (MSD_U32)(((MSD_U32)1 << dev->maxPorts) - 1U);

	data = (MSD_U16)((tmpData & (MSD_U32)0x7ff) | (((tmpData & (MSD_U32)0x10000) >> 16) << 11));

	*trunkRoute = MSD_PORTVEC_2_LPORTVEC(data & mask);

    MSD_DBG_INFO(("Oak_gsysGetTrunkRouting Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Oak_gsysSetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       en - MSD_TRUE to enable Flood Broadcast, MSD_FALSE otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetFloodBC
(
    IN MSD_QD_DEV    *dev,
    IN MSD_BOOL        en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;

    MSD_DBG_INFO(("Oak_gsysSetFloodBC Called.\n"));

	MSD_BOOL_2_BIT(en, data);

    /* Set related bit */
	retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MANAGEMENT, (MSD_U8)12, (MSD_U8)1, data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdSetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Oak_gsysSetFloodBC Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysGetFloodBC
*
* DESCRIPTION:
*       Flood Broadcast.
*       When Flood Broadcast is enabled, frames with the Broadcast destination 
*       address will flood out all the ports regardless of the setting of the
*       port's Egress Floods mode (see gprtSetEgressFlood API). VLAN rules and 
*       other switch policy still applies to these Broadcast frames. 
*       When this feature is disabled, frames with the Broadcast destination
*       address are considered Multicast frames and will be affected by port's 
*       Egress Floods mode.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       en - MSD_TRUE if Flood Broadcast is enabled, MSD_FALSE otherwise.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetFloodBC
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_BOOL      *en
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */

    MSD_DBG_INFO(("Oak_gsysGetFloodBC Called.\n"));

    /* Get related bit */
	retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MANAGEMENT, (MSD_U8)12, (MSD_U8)1, &data);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("msdGetAnyRegField for G2 management register returned: %s.\n", msdDisplayStatus(retVal)));
    }
	else
	{
		MSD_BIT_2_BOOL(data, *en);
	}

    MSD_DBG_INFO(("Oak_gsysGetFloodBC Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysSetEgressMonitorDest
*
* DESCRIPTION:
*       This routine sets Egress Monitor Destination Port. Frames that are 
*       tarOak_geted toward an Egress Monitor Destination go out the port number 
*       indicated in these bits. This includes frames received on a Marvell Tag port
*       with the Egress Monitor type, and frames transmitted on a Network port that 
*       is enabled to be the Egress Monitor Source Port.
*       If the Egress Monitor Destination Port resides in this device these bits 
*       should point to the Network port where these frames are to egress. If the 
*       Egress Monitor Destination Port resides in another device these bits 
*       should point to the Marvell Tag port in this device that is used to Oak_get 
*       to the device that contains the Egress Monitor Destination Port.
*
* INPUTS:
*       port  - the logical port number.If port = 0x1F, EMD is disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetEgressMonitorDest
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT        port
)
{
    MSD_STATUS       retVal = MSD_OK;  /* Functions return value.      */
    MSD_U8           hwPort;           /* the physical port number     */

    MSD_DBG_INFO(("Oak_gsysSetEgressMonitorDest Called.\n"));

    /* translate LPORT to hardware port */
	if ((port == (MSD_LPORT)0x1F) || (port == (MSD_LPORT)0x1E))
	{
		hwPort = (MSD_U8)port;
	}
	else
	{
		hwPort = (MSD_U8)MSD_LPORT_2_PORT(port);
		if (hwPort == MSD_INVALID_PORT)
		{
			MSD_DBG_INFO(("Bad port %u.\n", port));
			retVal = MSD_BAD_PARAM;
		}
	}

	if (retVal != MSD_BAD_PARAM)
	{
		retVal = msdSetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MANAGEMENT, (MSD_U8)0, (MSD_U8)5, (MSD_U16)hwPort);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Oak_gsysSetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

    MSD_DBG_INFO(("Oak_gsysSetEgressMonitorDest Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Oak_gsysGetEgressMonitorDest
*
* DESCRIPTION:
*       This routine Oak_gets Egress Monitor Destination Port.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       port  - the logical port number.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetEgressMonitorDest
(
    IN  MSD_QD_DEV    *dev,
    OUT MSD_LPORT      *port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16          data;           /* The register's read data.    */
	MSD_U16          datac;

    MSD_DBG_INFO(("Oak_gsysGetEgressMonitorDest Called.\n"));
		
	retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_MANAGEMENT, (MSD_U8)0, (MSD_U8)5, &datac);
    if(retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gsysGetMonitorMgmtCtrl returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		data = datac;

		if ((data == (MSD_U16)0x1F) || (data == (MSD_U16)0x1E))
		{
			*port = (MSD_LPORT)data;
		}
		else
		{
			*port = MSD_PORT_2_LPORT((MSD_U8)data);
		}
	}

    MSD_DBG_INFO(("Oak_gsysGetEgressMonitorDest Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysSetSMISetup
*
* DESCRIPTION:
*       This routine is to set SMI PHY Setup
*
* INPUTS:
*       pointer - [9:0] bits (should be less than 0x400)
*		data	- SMI PHY Setup.
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Oak_gsysSetSMISetup_MultiChip
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT  portNum,
    IN MSD_U16		  data
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16			 tmpdata;
    MSD_U32    		 timeOut;
	timeOut = (MSD_U32)MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    MSD_DBG_INFO(("Oak_gsysSetSMISetup_MultiChip Called.\n"));

	if (portNum > (MSD_LPORT)11)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM (portNum: %u). It should be within [0,11].\n", portNum));
        return MSD_BAD_PARAM;
    }
	if (data > (MSD_U16)0x1f)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM (regaddress: %u). It should be within [0,31].\n", data));
        return MSD_BAD_PARAM;
    }

	retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_SMI_PHY_DATA, data);

    do
    {
		if (Oak_msdDirectGetMultiChipReg(dev, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
        {
            MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		if (timeOut < (MSD_U32)1)
        {
            MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

	} while ((tmpdata & (MSD_U16)MSD_SMI_BUSY) != 0U);

	tmpdata = (MSD_U16)((MSD_U16)0xC000 | (MSD_U16)0x0400 | portNum);

	if (Oak_msdDirectSetMultiChipReg(dev, OAK_SMI_PHY_CMD, tmpdata) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write OAK_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    do
    {
		if (Oak_msdDirectGetMultiChipReg(dev, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
        {
            MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		if (timeOut < (MSD_U32)1)
        {
            MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

	} while ((tmpdata & (MSD_U16)MSD_SMI_BUSY) != 0U);


    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Oak_gsysSetSMISetup returned: %s.\n", msdDisplayStatus(retVal)));
        return retVal;
    }

    MSD_DBG_INFO(("Oak_gsysSetSMISetup_MultiChip Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Oak_gsysSetSMISetup
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT  portNum,
    IN MSD_U16		  data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			 tmpdata;
	MSD_U32    		 timeOut;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_gsysSetSMISetup_MultiChip(dev, portNum, data);
    }

	timeOut = (MSD_U32)MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	MSD_DBG_INFO(("Oak_gsysSetSMISetup Called.\n"));

	if (portNum > (MSD_LPORT)11)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (portNum: %u). It should be within [0,10].\n", portNum));
		return MSD_BAD_PARAM;
	}
	if (data > (MSD_U16)0x1f)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (regaddress: %u). It should be within [0,31].\n", data));
		return MSD_BAD_PARAM;
	}

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_DATA, data);

	do
	{
		if (msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
		{
			MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < (MSD_U32)1)
		{
			MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);

	tmpdata = (MSD_U16)((MSD_U16)0xc000 | (MSD_U16)0x0400 | portNum);

	if (msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, tmpdata) != MSD_OK)
	{
		MSD_DBG_ERROR(("Write OAK_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	do
	{
		if (msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
		{
			MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < (MSD_U32)1)
		{
			MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);


	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_gsysSetSMISetup returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Oak_gsysSetSMISetup Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gsysGetSMISetup
*
* DESCRIPTION:
*       This routine is to get SMI PHY Setup
*
* INPUTS:
*       portNum - [9:0] bits (should be less than 0x400)
*
* OUTPUTS:
*       data	- SMI PHY Setup.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Oak_gsysGetSMISetup_MultiChip
(
IN  MSD_QD_DEV		*dev,
IN  MSD_LPORT		portNum,
OUT MSD_U16			*data
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U16			 tmpdata;
    MSD_U32    		 timeOut;
	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    MSD_DBG_INFO(("Oak_gsysGetSMISetup_MultiChip Called.\n"));

	if (portNum > (MSD_LPORT)11)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM (pointer: %u). It should be within [0,11].\n", portNum));
        return MSD_BAD_PARAM;
    }

    do
    {
		if (Oak_msdDirectGetMultiChipReg(dev, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
        {
            MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		if (timeOut < (MSD_U32)1)
        {
            MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);

	tmpdata = (MSD_U16)((MSD_U16)0xc000 | (MSD_U16)0x0800 | portNum);

	if (Oak_msdDirectSetMultiChipReg(dev, OAK_SMI_PHY_CMD, tmpdata) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write OAK_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    do
    {
		if (Oak_msdDirectGetMultiChipReg(dev, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
        {
            MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		if (timeOut < (MSD_U32)1)
        {
            MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);

	retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_SMI_PHY_DATA, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Oak_msdDirectGetMultiChipReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

    MSD_DBG_INFO(("Oak_gsysGetSMISetup_MultiChip Exit.\n"));
    return MSD_OK;
}

MSD_STATUS Oak_gsysGetSMISetup
(
    IN  MSD_QD_DEV		*dev,
    IN  MSD_LPORT		portNum,
    OUT MSD_U16			*data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			 tmpdata;
	MSD_U32    		 timeOut;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_gsysGetSMISetup_MultiChip(dev, portNum, data);
    }

	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	MSD_DBG_INFO(("Oak_gsysGetSMISetup Called.\n"));

	if (portNum > (MSD_U32)11)
	{
		MSD_DBG_INFO(("MSD_BAD_PARAM (pointer: %u). It should be within [0,11].\n", portNum));
		return MSD_BAD_PARAM;
	}

	do
	{
		if (msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
		{
			MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < (MSD_U32)1)
		{
			MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);

	tmpdata = (MSD_U16)((MSD_U16)0xc000 | (MSD_U16)0x0800 | portNum);

	if (msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, tmpdata) != MSD_OK)
	{
		MSD_DBG_ERROR(("Write OAK_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	do
	{
		if (msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_CMD, &tmpdata) != MSD_OK)
		{
			MSD_DBG_ERROR(("read OAK_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < (MSD_U32)1)
		{
			MSD_DBG_ERROR(("read SMI Phy OAK_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((tmpdata & MSD_SMI_BUSY) != 0U);

	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_SMI_PHY_DATA, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	
	MSD_DBG_INFO(("Oak_gsysGetSMISetup Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gsysGlobal1RegDump
*
* DESCRIPTION:
*       These routine dump global 1 register.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGlobal1RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal = MSD_OK; /* Functions return value.      */
	MSD_U32	         tmpdata;
	MSD_U16          i = 0;

	char desc[32][48] = 
	{
		{ "Switch Global Status(0x0)" },
		{ "ATU FID Register(0x1)" },
		{ "VTU FID Register(0x2)" },
		{ "VTU SID Register(0x3)" },
		{ "Switch Global Control Register(0x4)" },
		{ "VTU Operation Register(0x5)" },
		{ "VTU VID Register(0x6)" },
		{ "VTU/STU Data Register Ports 0 to 7(0x7)" },
		{ "VTU/STU Data Register Ports 8 to 10(0x8)" },
		{ "ATU AVB FPri Disable Register" },
		{ "ATU Control Register(0xa)" },
		{ "ATU Operation Register(0xb)" },
		{ "ATU Data Register(0xc)" },
		{ "Switch MAC Address Register Bytes 0 & 1(0xd)" },
		{ "Switch MAC Address Register Bytes 2 & 3(0xe)" },
		{ "Switch MAC Address Register Bytes 4 & 5(0xf)" },
		{ "Extended Global Control Command" },
		{ "Extended Global Control Data" },
		{ "Reserved(0x12)" },
		{ "Reserved(0x13)" },
		{ "Reserved(0x14)" },
		{ "Semaphore Register" },
		{ "MGMT Extension Register" },
		{ "Reserved(0x17)" },
		{ "Reliable SMI CRC Register" },
		{ "Reserved(0x19)" },
		{ "Reserved(0x1a)" },
		{ "Total Free Counter(0x1b)" },
		{ "Global Control 2(0x1c)" },
		{ "Stats Operation Register(0x1d)" },
		{ "Stats Counter Register Byte 3 & 2(0x1e)" },
		{ "Stats Counter Register Byte 1 & 0(0x1f)" }
	};

	MSD_DBG_INFO(("Oak_gsysGlobal1RegDump Called.\n"));

	MSG(("\n------------------------------------------------------\n"));

	for (i = 0; i < 32U; i++)
	{
		retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, (MSD_U8)i, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			break;
		}
		MSG(("%-48s%04x\n", desc[i], tmpdata));
	}

	MSD_DBG_INFO(("Oak_gsysGlobal1RegDump Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysGlobal2RegDump
*
* DESCRIPTION:
*       These routine dump global 2 register.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGlobal2RegDump
(
IN  MSD_QD_DEV    *dev
)
{
	MSD_STATUS       retVal = MSD_OK; /* Functions return value.      */
	MSD_U32	         tmpdata;
	MSD_U16           i = 0;

	char desc[32][48] =
	{
		{ "Interrupt Source Register(0x0)" },
		{ "Interupt Mask Register(0x1)" },
		{ "Interrupt2 Source Register(0x2)" },
		{ "Interrupt2 Mask Register(0x3)" },
		{ "Flow Control Delay Register(0x4)" },
		{ "Switch Management Register(0x5)" },
		{ "Device Mapping Table Register(0x6)" },
		{ "LAG Mask Table Register(0x7)" },
		{ "LAG Mapping Register(0x8)" },
		{ "Ingress Rate Command Register(0x9)" },
		{ "Ingress Rate Data Register(0xa)" },
		{ "Cross Chip Port VLAN Addr Register(0xb)" },
		{ "Cross Chip Port VLAN Data Register(0xc)" },
		{ "Switch MAC/WoL/WoF Register(0xd)" },
		{ "ATU Stats(0xe)" },
		{ "Priority Override Table(0xf)" },
		{ "I2C Block Register(0x10)" },
		{ "Reserved(0x11)" },
		{ "Energy Management Register(0x12)" },
		{ "IMP Communication/Debug Register(0x13)" },
		{ "EEPROM Command(0x14)" },
		{ "EEPROM Addr(0x15)" },
		{ "AVB/TSN Command Register(0x16)" },
		{ "AVB/TSN Data Register(0x17)" },
		{ "SMI PHY Command for SMI Access(0x18)" },
		{ "SMI PHY Data Register(0x19)" },
		{ "Scratch and Misc(0x1a)" },
		{ "Watch Dog Control Register(0x1b)" },
		{ "QoS Weights Register(0x1c)" },
		{ "Misc Register(0x1d)" },
		{ "Reserved(0x1e)" },
		{ "Reserved(0x1f)" }
	};

	MSD_DBG_INFO(("Oak_gsysGlobal2RegDump Called.\n"));

	MSG(("\n------------------------------------------------\n"));
	for (i = 0; i < 32U; i++)
	{
		retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, (MSD_U8)i, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("msdGetAnyReg returned: %s.\n", msdDisplayStatus(retVal)));
			break;
		}
		MSG(("%-42s%04x\n", desc[i], tmpdata));
	}

	MSD_DBG_INFO(("Oak_gsysGlobal2RegDump Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysSetRMUMode
*
* DESCRIPTION:
*       These routine sets Remote Management Unit Mode.
*
* INPUTS:
*       data - RMU mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given (If pri is not less than 8)
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysSetRMUMode
(
IN MSD_QD_DEV    *dev,
IN MSD_U16        data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysSetRMUMode Called.\n"));

	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL2, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		/* Set related bit */
		tmpdata = (MSD_U16)((tmpdata & (MSD_U16)0xF8FF) | (MSD_U16)((MSD_U16)(data & (MSD_U16)0x7) << 8));

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL2, tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gsysSetRMUMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysGetRMUMode
*
* DESCRIPTION:
*       These routine gets Remote Management Unit Mode..
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       data - RMU mode
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysGetRMUMode
(
IN  MSD_QD_DEV    *dev,
OUT MSD_U16      *data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysGetRMUMode Called.\n"));

	/* Get related bit */
	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL2, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 global control2 register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		/* Set related bit */
		*data = (MSD_U16)((tmpdata & (MSD_U16)0x700) >> 8);
	}

	MSD_DBG_INFO(("Oak_gsysGetRMUMode Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysQbvWrite
*
* DESCRIPTION:
*       These routine write QBV registers.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*       data       - QBV data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysQbvWrite
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
IN MSD_U16       data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysQbvWrite Called.\n"));

	/*set data to G2 0x17 register*/
	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_DATA, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G2 AVB Data register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/*check busy bit until its zero*/
	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
	{
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Read G2 AVB command register returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}

	/*set command to G2 0x16 register*/
	tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)0x3 << 13) | (MSD_U16)(((MSD_U16)qbvPort & (MSD_U16)0x1F) << 8) | (MSD_U16)((MSD_U16)0x3 << 5) | (MSD_U16)((MSD_U16)qbvRegAddr & (MSD_U16)0x1F));

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_COMMAND, tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G2 AVB command register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Oak_gsysQbvWrite Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gsysQbvRead
*
* DESCRIPTION:
*       These routine gets QBV register value.
*
* INPUTS:
*       qbvPort    - QBV port (0x1F - Qbv global registers)
*       qbvRegAddr - QBV register address
*
* OUTPUTS:
*       data - QBV data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysQbvRead
(
IN MSD_QD_DEV    *dev,
IN MSD_U8        qbvPort,
IN MSD_U8        qbvRegAddr,
OUT MSD_U16      *data
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysQbvRead Called.\n"));

	/*check busy bit until its zero*/
	tmpdata = (MSD_U16)1;
	while (tmpdata == (MSD_U16)1)
	{
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_COMMAND, (MSD_U8)15, (MSD_U8)1, &tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Read G2 AVB command register returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}

	/*set command to G2 0x16 register*/
	tmpdata = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)(((MSD_U16)qbvPort & (MSD_U16)0x1F) << 8) | (MSD_U16)((MSD_U16)0x3 << 5) | (MSD_U16)((MSD_U16)qbvRegAddr & (MSD_U16)0x1F));

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_COMMAND, tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G2 AVB command register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* Get AVB data G2 0x17 register */
	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_AVB_DATA, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G2 AVB data register returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	MSD_DBG_INFO(("Oak_gsysQbvRead Exit.\n"));
	return MSD_OK;
}

/*******************************************************************************
* Oak_gsysDevIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable Interrupt Mask to drive the 
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt mask, any combination of the folowing:
*                MSD_WATCH_DOG_INT			0x8000
*                MSD_JAM_LIMIT_INT			0x4000
*                MSD_DUPLEX_MISMATCH_INT	0x2000
*                MSD_WAKE_EVENT_INT			0x1000
*                MSD_PHY_INT_11 			0x800
*                MSD_PHY_INT_10 			0x400
*                MSD_PHY_INT_9 				0x200
*                MSD_PHY_INT_8 				0x100
*                MSD_PHY_INT_7 				0x80
*                MSD_PHY_INT_6 				0x40
*                MSD_PHY_INT_5 				0x20
*                MSD_PHY_INT_4 				0x10
*                MSD_PHY_INT_3 				0x8
*                MSD_PHY_INT_2 				0x4
*                MSD_PHY_INT_1 				0x2
*                MSD_PHY_INT_0 				0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntEnableSet
(
	IN MSD_QD_DEV    *dev,
	IN MSD_U16       en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U32          data = 0;

	MSD_DBG_INFO(("Oak_gsysDevIntEnableSet Called.\n"));

	data = (MSD_U32)((MSD_U32)en & (MSD_U32)0xF7FF) | (MSD_U32)((((MSD_U32)en & (MSD_U32)0x0800) >> 11) << 16);

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdSetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_INT_MASK, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Write G2 interrupt mask register returned: %s.\n", msdDisplayStatus(retVal)));
	}

	MSD_DBG_INFO(("Oak_gsysDevIntEnableSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysDevIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable Interrupt Mask to drive the
*       DeviceInt bit in the Switch Global Status register.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt mask
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			 tmpdata = 0;
	MSD_U32          data = 0;

	MSD_DBG_INFO(("Oak_gsysDevIntEnableGet Called.\n"));

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_INT_MASK, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G2 interrupt mask register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		tmpdata = (MSD_U16)((((data & (MSD_U32)0x10000) >> 16) << 11) | (data & (MSD_U32)0xffff));

		*en = (MSD_U16)tmpdata;
	}

	MSD_DBG_INFO(("Oak_gsysDevIntEnableGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysDevIntStatusGet
*
* DESCRIPTION:
*       These routine gets interrupt source, global 2 register offset 0x2.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt source
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysDevIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *status
)
{
	MSD_STATUS      retVal;         /* Functions return value.      */
	MSD_U16			tmpdata = 0;
	MSD_U32         data = 0;

	MSD_DBG_INFO(("Oak_gsysDevIntStatusGet Called.\n"));

	/*set G2 0x01 register, interrupt mask*/
	retVal = msdGetAnyExtendedReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_INT_SOURCE, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G2 interrupt source register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		tmpdata = (MSD_U16)((((data & (MSD_U32)0x10000) >> 16) << 11) | (data & (MSD_U32)0xffff));

		*status = tmpdata;
	}

	MSD_DBG_INFO(("Oak_gsysDevIntStatusGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysActiveIntEnableSet
*
* DESCRIPTION:
*       These routine sets interrupt enable or disable(global 1 offset 0x4) to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*       en     - interrupt data, any combination of the folowing:
*                MSD_DEVICE2_INT	0x200
*                MSD_AVB_INT 		0x100
*                MSD_DEVICE_INT		0x80
*                MSD_STATS_DONE		0x40 
*                MSD_VTU_PROB		0x20 
*                MSD_VTU_DONE		0x10
*                MSD_ATU_PROB		0x8
*                MSD_ATU_DONE 		0x4 
*                MSD_TCAM_INT		0x2
*                MSD_EE_INT			0x1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntEnableSet
(
IN MSD_QD_DEV    *dev,
IN MSD_U16       en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysActiveIntEnableSet Called.\n"));

	/*get G1 0x04 register, interrupt to drive the device¡¯s INTn low pin.*/
	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		tmpdata = (tmpdata & (MSD_U16)0xFC00) | en;

		/*set G1 0x04 register, interrupt to drive the device¡¯s INTn low pin.*/
		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL, tmpdata);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Write G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
		}
	}

	MSD_DBG_INFO(("Oak_gsysActiveIntEnableSet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysActiveIntEnableGet
*
* DESCRIPTION:
*       These routine gets interrupt enable or disable to drive the device¡¯s INTn pin low.
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntEnableGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16       *en
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			tmpdata;

	MSD_DBG_INFO(("Oak_gsysActiveIntEnableGet Called.\n"));

	/*get G1 0x04 register, interrupt to drive the device¡¯s INTn low pin.*/
	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_CONTROL, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*en = tmpdata & (MSD_U16)0x3FF;
	}

	MSD_DBG_INFO(("Oak_gsysActiveIntEnableGet Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gsysActiveIntStatusGet
*
* DESCRIPTION:
*       This routine reads an hardware driven event status, global 1 offset 0x0
*
* INPUTS:
*       dev    - device structure
*
* OUTPUTS:
*       en     - interrupt status
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gsysActiveIntStatusGet
(
IN MSD_QD_DEV    *dev,
OUT MSD_U16      *status
)
{
	MSD_STATUS       retVal;         /* Functions return value.      */
	MSD_U16			 tmpdata;

	MSD_DBG_INFO(("Oak_gsysActiveIntStatusGet Called.\n"));

	/*set G1 0x00 register, Switch Global Control Register*/
	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL1_DEV_ADDR, OAK_GLOBAL_STATUS, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Read G1 Global Control Register returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		*status = tmpdata & (MSD_U16)0x3FF;
	}

	MSD_DBG_INFO(("Oak_gsysActiveIntStatusGet Exit.\n"));
	return retVal;
}