/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdTrunk.c
*
* DESCRIPTION:
*       API definitions for Marvell Trunk functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdTrunk.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdTrunkMaskTableGet
*
* DESCRIPTION:
*       This function gets Trunk Mask for the given Trunk Number.
*
* INPUTS:
*       devNum  - physical device number
*       maskNum - Mask Number.
*
* OUTPUTS:
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    OUT MSD_U32  *trunkMask
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
		if (dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable(dev, maskNum, trunkMask);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkMaskTableSet
*
* DESCRIPTION:
*       This function sets Trunk Mask for the given Mask Number.
*
* INPUTS:
*       devNum  - physical device number
*       maskNum - Mask Number (0~7).
*       trunkMask - Trunk mask bits. Bit 0 controls trunk masking for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMaskTableSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  maskNum,
    IN  MSD_U32  trunkMask
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
		if (dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable(dev, maskNum, trunkMask);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkHashTrunkGet
*
* DESCRIPTION:
*       This routine gets enable status for hash DA & SA for TrunkMask selection. 
*       Trunk load balancing is accomplished by using the frame's DA and SA fields 
*       to access one of eight Trunk Masks.
*		When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL  *en
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
		if (dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk(dev, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkHashTrunkSet
*
* DESCRIPTION:
*       This routine enable or disable hash DA & SA for TrunkMask selection. Trunk 
*       load balancing is accomplished by using the frame's DA and SA fields to 
*		access one of eight Trunk Masks.
*       When this bit is set to a one the hashed computed for address table
*       lookups is used for the TrunkMask selection. When this bit is cleared to
*       a zero the lower 3 bits of the frame's DA and SA are XOR'ed together to
*       select the TrunkMask to use.
*
* INPUTS:
*       devNum  - physical device number
*       en - MSD_TRUE to use lookup table, MSD_FALSE to use XOR.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkHashTrunkSet
(
    IN  MSD_U8  devNum,
    IN  MSD_BOOL  en
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
		if (dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk(dev, en);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkRoutingGet
*
* DESCRIPTION:
*       This function same as msdTrunkMappingGet, in order to version compatibility.
*
* INPUTS:
*       devNum  - physical device number
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkMap - Trunk map bits. Bit 0 controls trunk map for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingGet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    OUT MSD_U32  *trunkMap
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
		if (dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting(dev, trunkId, trunkMap);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkRoutingSet
*
* DESCRIPTION:
*       This function same as msdTrunkMappingSet, in order to version compatibility.
*
* INPUTS:
*       devNum  - physical device number
*       trunkId - Trunk ID.
*       trunkMap - Trunk map bits. Bit 0 controls trunk mapping for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkRoutingSet
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  trunkId,
    IN  MSD_U32  trunkMap
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
		if (dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting(dev, trunkId, trunkMap);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdTrunkMappingGet
*
* DESCRIPTION:
*       This function retrieves mapping information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical device number
*       trunkId - Trunk ID.
*
* OUTPUTS:
*       trunkMap - Trunk map bits. Bit 0 controls trunk map for port 0,
*                    bit 1 for port 1 , etc.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMappingGet
(
IN  MSD_U8  devNum,
IN  MSD_U32  trunkId,
OUT MSD_U32  *trunkMap
)
{
	MSD_STATUS retVal;
	retVal = msdTrunkRoutingGet(devNum, trunkId, trunkMap);

	return retVal;
}

/*******************************************************************************
* msdTrunkMappingSet
*
* DESCRIPTION:
*       This function sets mapping information for the given Trunk ID.
*
* INPUTS:
*       devNum  - physical device number
*       trunkId - Trunk ID.
*       trunkMap - Trunk map bits. Bit 0 controls trunk mapping for port 0,
*                    bit 1 for port 1 , etc.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdTrunkMappingSet
(
IN  MSD_U8  devNum,
IN  MSD_U32  trunkId,
IN  MSD_U32  trunkMap
)
{
	MSD_STATUS retVal;
	retVal = msdTrunkRoutingSet(devNum, trunkId, trunkMap);

	return retVal;
}

/*******************************************************************************
* msdPortLAGPortGet
*
* DESCRIPTION:
*       This function returns trunk state of the port.
*       When trunk is disabled, trunkId field won't have valid value.
*
* INPUTS:
*       portNum - the logical port number.
*
* OUTPUTS:
*       en - MSD_TRUE, if the port is a member of a LAG,
*             MSD_FALSE, otherwise.
*       trunkId - 0 ~ 31, valid only if en is MSD_TRUE
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS msdPortLAGPortGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    OUT MSD_BOOL  *en,
    OUT MSD_U32  *LAGId
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
		if (dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort(dev, portNum, en, LAGId);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}

/*******************************************************************************
* msdPortLAGPortSet
*
* DESCRIPTION:
*        This function enables/disables trunk bit and sets the LAG ID.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
*       en - MSD_TRUE to make the port be a member of a LAG with the given LAG ID.
*             MSD_FALSE, otherwise.
*       LAGId - valid ID is 0 ~ 31
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		 None.
*
*******************************************************************************/
MSD_STATUS msdPortLAGPortSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  en,
    IN  MSD_U32  LAGId
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
		if (dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort != NULL)
		{
			retVal = dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort(dev, portNum, en, LAGId);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

    return retVal;    
}


