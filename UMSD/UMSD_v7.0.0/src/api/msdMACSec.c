/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdMacSec.c
*
* DESCRIPTION:
*       API definitions for Macsec 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdMACSec.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/******************************************************************************
* DESCRIPTION:
*	This function is used to read MACSec register.
*
* INPUTS:
*	devNum  - physical device number
*   regAddr - register address
*	data - register data
*
* OUTPUTS:
*	None
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS msdMacSecRead
(
	IN MSD_U8  devNum,
	IN MSD_U32 regAddr,
	OUT MSD_U32* data
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecRead != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecRead(*dev, regAddr, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* DESCRIPTION:
*	This function is used to write MACSec register.
*
* INPUTS:
*	devNum  - physical device number
*   regAddr - register address
*	data - register data
*
* OUTPUTS:
*	None
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	None
******************************************************************************/
MSD_STATUS msdMacSecWrite
(
	IN MSD_U8  devNum,
	IN MSD_U32 regAddr,
	IN MSD_U32 data
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecWrite != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecWrite(*dev, regAddr, data);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecFlushAll
* 
* DESCRIPTION:
*		This function is used to flush all MACSec type module.
*
* INPUTS:
*		devNum  - physical device number
*
* OUTPUTS:
*		None
*
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		None
******************************************************************************/
MSD_STATUS msdMacSecFlushAll
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecFlushAll != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecFlushAll(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* DESCRIPTION:
*	This function is used to get event value.
*
* INPUTS:
*	devNum  - physical device number
*
* OUTPUTS:
*	event - macsec ECC/SMStall/RegParity event
*
* RETURNS:
*	MSD_STATUS
*
* NOTES/WARNINGS:
*	Register type is RO
******************************************************************************/
MSD_STATUS msdMacSecGetEvent
(
	IN MSD_U8  devNum,
	OUT MSD_MSEC_EVENT* event
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetEvent != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetEvent(dev, event);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecCreateRule
* 
* DESCRIPTION:
* 	This function is used to create macsec rule.
* 
* INPUTS:
* 		devNum  - physical device number
* 		ruleIndex - rule index
* 		rule - MACSec rule configuration
* 
* OUTPUTS:
* 		None
* 
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		None
******************************************************************************/
MSD_STATUS msdMacSecCreateRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex,
	IN MSD_MSEC_RULE* rule
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRule != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRule(dev, ruleIndex, rule);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecDeleteRule
* 
* DESCRIPTION:
* 		This function is used to delete macsec rule.
* 
* INPUTS:
* 		devNum  - physical device number
* 		ruleIndex - rule index
* 
* OUTPUTS:
* 		None
* 
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule(dev, ruleIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecGetRule
* 
* DESCRIPTION:
* 	This function is used to get macsec rule.
* 
* INPUTS:
* 		devNum  - physical device number
* 		ruleIndex - rule index
* 
* OUTPUTS:
* 		rule - MACSec rule configuration
* 
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		None
******************************************************************************/
MSD_STATUS msdMacSecGetRule
(
	IN  MSD_U8  devNum,
	IN MSD_U8 ruleIndex,
	OUT MSD_MSEC_RULE* rule
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRule != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRule(dev, ruleIndex, rule);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecCreateSecY
* 
* DESCRIPTION:
* 		This function is used to create macsec secY.
* 
* INPUTS:
* 		devNum  - physical device number
* 		secYIndex - secy index
* 		secY - secY configuration
* 
* OUTPUTS:
* 		None
* 
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_INGRESS* secY
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY(dev, secYIndex, secY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdMacSecCreateSecY
*
* DESCRIPTION:
* 		This function is used to create macsec secY.
*
* INPUTS:
* 		devNum  - physical device number
* 		secYIndex - secy index
* 		secY - secY configuration
*
* OUTPUTS:
* 		None
*
* RETURNS:
*	    MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex,
	IN MSD_SECY_EGRESS* secY
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY(dev, secYIndex, secY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete macsec secY.

INPUTS:
	devNum  - physical device number
	secYIndex - secY index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY(dev, secYIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete macsec secY.

INPUTS:
	devNum  - physical device number
	secYIndex - secY index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY(dev, secYIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get macsec secY.

INPUTS:
	devNum  - physical device number
	secYIndex - secY index

OUTPUTS:
	secY

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_EGRESS* secY
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY(dev, secYIndex, secY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get macsec secY.

INPUTS:
	devNum  - physical device number
	secYIndex - secY index

OUTPUTS:
	secY	- secy configuration

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRxSecY
(
	IN MSD_U8 devNum,
	IN MSD_U8 secYIndex,
	OUT MSD_SECY_INGRESS* secY
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY(dev, secYIndex, secY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to create ingress sc.

INPUTS:
	devNum  - physical device number
	scIndex - sc index
	rxSC - rxSC configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_RX_SC* rxSC
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC(dev, scIndex, rxSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete ingress sc.

INPUTS:
	devNum  - physical device number
	scIndex - sc index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC(dev, scIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/******************************************************************************
DESCRIPTION:
	This function is used to get ingress sc.

INPUTS:
	devNum  - physical device number
	scIndex - SC index

OUTPUTS:
	rxSC	 sc configuration

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_RX_SC* rxSC
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC(dev, scIndex, rxSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to create ingress sa.

INPUTS:
	devNum  - physical device number
	saIndex - sa index
	rxSA - rxSA configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecCreateRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_RX_SA* rxSA
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA(dev, saIndex, rxSA);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete ingress sa.

INPUTS:
	devNum  - physical device number
	saIndex - sa index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA(dev, saIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/******************************************************************************
DESCRIPTION:
	This function is used to get ingress sa.

INPUTS:
	devNum  - physical device number
	saIndex - SA index

OUTPUTS:
	rxSA	 sa configuration

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_RX_SA* rxSA
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA(dev, saIndex, rxSA);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to create egress sc.

INPUTS:
	devNum  - physical device number
	scIndex - sc index
	txSC - txSC configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	IN MSD_MSEC_TX_SC* txSC
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC(dev, scIndex, txSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete egress sc.

INPUTS:
	devNum  - physical device number
	scIndex - sc index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC(dev, scIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get egress sc.

INPUTS:
	devNum  - physical device number
	scIndex - SC index

OUTPUTS:
	rxSC	- sc configuration

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSC
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scIndex,
	OUT MSD_MSEC_TX_SC* txSC
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC(dev, scIndex, txSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to create egress sa.

INPUTS:
	devNum  - physical device number
	saIndex - sa index
	txSA - txSA configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecCreateTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	IN MSD_MSEC_TX_SA* txSA
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA(dev, saIndex, txSA);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to delete egress sa.

INPUTS:
	devNum  - physical device number
	saIndex - sa index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecDeleteTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA(dev, saIndex);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get egress sa.

INPUTS:
	devNum  - physical device number
	saIndex - SA index

OUTPUTS:
	txSA	- sa configuration

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSA
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_MSEC_TX_SA* txSA
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA(dev, saIndex, txSA);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get ingress sc related mibs.

INPUTS:
	devNum  - physical device number
	scCounterIndex - SC counter index
	autoClear - clear or not after operation read

OUTPUTS:
	states - ingress sc mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRXSCStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SC_COUNTER* states
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates(dev, scCounterIndex, autoClear, states);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get ingress secY related mibs.

INPUTS:
	devNum  - physical device number
	secYCounterIndex - secY counter index
	autoClear - enable or disable

OUTPUTS:
	states - ingress secY mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRXSecYStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_RX_SECY_COUNTER* states
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates(dev, secYCounterIndex, autoClear, states);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/******************************************************************************
DESCRIPTION:
	This function is used to get egress sc related mibs.

INPUTS:
	devNum  - physical device number
	scCounterIndex - SC counter index
	autoClear - enable or disable

OUTPUTS:
	states - egress sc mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTXSCStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 scCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SC_COUNTER* states
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates(dev, scCounterIndex, autoClear, states);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get egress secY related mibs.

INPUTS:
	devNum  - physical device number
	secYCounterIndex - secY counter index
	autoClear - enable or disable

OUTPUTS:
	states - egress secY mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTXSecYStates
(
	IN  MSD_U8  devNum,
	IN MSD_U8 secYCounterIndex,
	IN MSD_AUTO_CLEAR autoClear,
	OUT MSD_MSEC_TX_SECY_COUNTER* states
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates(dev, secYCounterIndex, autoClear, states);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to enable or disable macsec module.

INPUTS:
	devNum  - physical device number
	portNum - port number
	isEnable - enable or disable

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecEnableEgressSet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet(dev, portNum, isEnable);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to enable or disable macsec module.

INPUTS:
	devNum  - physical device number
	portNum - port number
	isEnable - enable or disable

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecEnableIngressSet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet(dev, portNum, isEnable);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get macsec module status.

INPUTS:
	devNum  - physical device number
	portNum - port number
	isEnable - enable or disable

OUTPUTS:
	portControl - port control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecEnableIngressGet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet(dev, portNum, isEnable);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get macsec module status.

INPUTS:
	devNum  - physical device number
	portNum - port number
	isEnable - enable or disable

OUTPUTS:
	portControl - port control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecEnableEgressGet
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet(dev, portNum, isEnable);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to set Tx PN threshold.

INPUTS:
	devNum  - physical device number
	portNum - port number
	pnThr - PN threshold

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecSetTxPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U32 pnThr
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold(dev, pnThr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get Tx PN threshold.

INPUTS:
	devNum  - physical device number
	portNum - port number

OUTPUTS:
	pnThr - PN threshold

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U32* pnThr
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold != NULL)
		{
			
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold(dev, pnThr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to set Tx XPN threshold.

INPUTS:
	devNum  - physical device number
	portNum - port number
	xpnThr - XPN threshold

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecSetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U64 xpnThr
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold(dev, xpnThr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/******************************************************************************
DESCRIPTION:
	This function is used to get Tx XPN threshold.

INPUTS:
	devNum  - physical device number
	portNum - port number

OUTPUTS:
	xpnThr - XPN threshold

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U64* xpnThr
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold != NULL)
		{

			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold(dev, xpnThr);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to set default RX SCI.

INPUTS:
	devNum  - physical device number
	portNum - port number
	defaultSCI - default Rx SCI

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecSetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultRxSCI
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI(dev, portNum, defaultRxSCI);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get default RX SCI.

INPUTS:
	devNum  - physical device number
	portNum - port number

OUTPUTS:
	defaultSCI - default Rx SCI

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_U64* defaultRxSCI
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI(dev, portNum, defaultRxSCI);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to set interrupt control.

INPUTS:
	devNum  - physical device number
	index - index
	txscIntCtrl - interrupt control

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecSetTxSCIntCtrl
(
	IN MSD_U8 devNum,
	IN MSD_U8 index,
	IN MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl(dev, index, txscIntCtrl);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get interrupt control.

INPUTS:
	devNum  - physical device number
	index - index
	txscIntCtrl - interrupt type

OUTPUTS:
	int_control - control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetTxSCIntCtrl
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	OUT MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl(dev, index, txscIntCtrl);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
DESCRIPTION:
	This function is used to get interrupt status.

INPUTS:
	devNum  - physical device number
	isAutoClear - auto clear or not after get next

OUTPUTS:
	nextIndex - next index
	txscInt - tx sc interrupt

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacSecGetNextTxSCIntStatus
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	IN MSD_BOOL isAutoClear,
	OUT MSD_U8* nextIndex,
	OUT MSD_MACSEC_INT_TXSC* txscInt
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus(dev, index, isAutoClear, nextIndex, txscInt);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}