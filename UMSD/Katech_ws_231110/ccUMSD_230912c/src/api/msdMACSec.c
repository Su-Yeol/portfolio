/**********************************************************************************************
* Copyright (c) 2022 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdMacsec.c
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
*	pDev - pointer to FIR_DEV
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
UMSD_FUNC MSD_STATUS msdMacsecRead
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
*	pDev - pointer to FIR_DEV
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
UMSD_FUNC MSD_STATUS msdMacsecWrite
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
* msdMacsecFlushAll
* 
* DESCRIPTION:
*		This function is used to flush all MACSec type module.
*
* INPUTS:
*		devNum  - physical devie number
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
MSD_STATUS msdMacsecFlushAll
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
*	pDev - pointer to FIR_DEV
*	portNum - port number
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
MSD_STATUS msdMacsecGetEvent
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
* msdMacsecCreateRule
* 
* DESCRIPTION:
* 	This function is used to create macsec rule.
* 
* INPUTS:
* 		devNum  - physical devie number
* 		portNum - port number
* 		rule - MSEC_RULE configuration
* 		isEgress - direction
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
MSD_STATUS msdMacsecCreateRule
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
* msdMacsecDeleteRule
* 
* DESCRIPTION:
* 		This function is used to delete macsec rule.
* 
* INPUTS:
* 		devNum  - physical devie number
* 		portNum - port number
* 		ruleIndex - MSEC_RULE index
* 		isEgress - direction
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
MSD_STATUS msdMacsecDeleteRule
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
* msdMacsecGetRule
* 
* DESCRIPTION:
* 	This function is used to get macsec rule.
* 
* INPUTS:
* 		devNum  - physical devie number
* 		portNum - port number
* 		ruleIndex - rule index
* 		isEgress - direction
* 
* OUTPUTS:
* 		rule - MSEC_RULE configuration
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
MSD_STATUS msdMacsecGetRule
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
* msdMacsecCreateSecY
* 
* DESCRIPTION:
* 		This function is used to create macsec secY.
* 
* INPUTS:
* 		devNum  - physical devie number
* 		portNum - port number
* 		secY - secY configuration
* 		isEgress - direction
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
MSD_STATUS msdMacsecCreateRxSecY
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
* msdMacsecCreateSecY
*
* DESCRIPTION:
* 		This function is used to create macsec secY.
*
* INPUTS:
* 		devNum  - physical devie number
* 		portNum - port number
* 		secY - secY configuration
* 		isEgress - direction
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
MSD_STATUS msdMacsecCreateTxSecY
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
	devNum  - physical devie number
	portNum - port number
	secYIndex - secY index
	isEgress - direction

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteTxSecY
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
	devNum  - physical devie number
	portNum - port number
	secYIndex - secY index
	isEgress - direction

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteRxSecY
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
	devNum  - physical devie number
	portNum - port number
	secYIndex - secY index
	isEgress - direction

OUTPUTS:
	secY

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxSecY
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
	devNum  - physical devie number
	portNum - port number
	secYIndex - secY index
	isEgress - direction

OUTPUTS:
	secY

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRxSecY
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
/*RXSC*/
/******************************************************************************
DESCRIPTION:
	This function is used to create ingress sc.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	rxSC - rxSC configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecCreateRxSC
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
	devNum  - physical devie number
	portNum - port number
	scIndex - sc index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteRxSC
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
	devNum  - physical devie number
	portNum - port number
	scIndex - SC index

OUTPUTS:
	rxSC

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRxSC
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
/*RXSA*/
/******************************************************************************
DESCRIPTION:
	This function is used to create ingress sa.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	rxSA - rxSA configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecCreateRxSA
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
	devNum  - physical devie number
	portNum - port number
	saIndex - sa index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteRxSA
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
	devNum  - physical devie number
	portNum - port number
	saIndex - SA index

OUTPUTS:
	rxSA

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRxSA
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
	This function is used to get ingress sa next packet number.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	saIndex - SA index

OUTPUTS:
	nextPN - next packet number

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRxSANextPN
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetRxSANextPN != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxSANextPN(dev, saIndex, nextPN);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*TXSC*/
/******************************************************************************
DESCRIPTION:
	This function is used to create egress sc.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	txSC - txSC configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecCreateTxSC
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
	devNum  - physical devie number
	portNum - port number
	scIndex - sc index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteTxSC
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
	devNum  - physical devie number
	portNum - port number
	scIndex - SC index

OUTPUTS:
	rxSC

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxSC
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
/*TXSA*/
/******************************************************************************
DESCRIPTION:
	This function is used to create egress sa.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	txSA - txSA configuration

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecCreateTxSA
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
	devNum  - physical devie number
	portNum - port number
	saIndex - sa index

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecDeleteTxSA
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
	devNum  - physical devie number
	portNum - port number
	saIndex - SA index

OUTPUTS:
	txSA

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxSA
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
	This function is used to get egress sa next packet number.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	saIndex - SA index

OUTPUTS:
	nextPN - next packet number

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxSANextPN
(
	IN  MSD_U8  devNum,
	IN MSD_U8 saIndex,
	OUT MSD_U64* nextPN
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
		if (dev->SwitchDevObj.MACSECObj.gmacsecGetTxSANextPN != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxSANextPN(dev, saIndex, nextPN);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/*States*/
/******************************************************************************
DESCRIPTION:
	This function is used to get ingress sc related mibs.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	scCounterIndex - SC counter index
	autoClear - Enable or disable

OUTPUTS:
	states - ingress sc mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRXSCStates
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
	devNum  - physical devie number
	portNum - port number
	secYCounterIndex - secY counter index
	autoClear - Enable or disable

OUTPUTS:
	states - ingress secY mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRXSecYStates
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
	devNum  - physical devie number
	portNum - port number
	scCounterIndex - SC counter index
	autoClear - Enable or disable

OUTPUTS:
	states - egress sc mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTXSCStates
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
	devNum  - physical devie number
	portNum - port number
	secYCounterIndex - secY counter index
	autoClear - Enable or disable

OUTPUTS:
	states - egress secY mibs

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTXSecYStates
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
/*MACSec enable*/
/******************************************************************************
DESCRIPTION:
	This function is used to enable or disable macsec module.

INPUTS:
	devNum  - physical devie number
	portNum - port number
	isEgress - Egress or Ingress
	portControl - port control

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecEnableEgressSet
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
	devNum  - physical devie number
	portNum - port number
	isEgress - Egress or Ingress
	portControl - port control

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecEnableIngressSet
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
devNum  - physical devie number
portNum - port number
isEgress - Egress or Ingress

OUTPUTS:
	portControl - port control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecEnableIngressGet
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
devNum  - physical devie number
portNum - port number
isEgress - Egress or Ingress

OUTPUTS:
	portControl - port control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecEnableEgressGet
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
devNum  - physical devie number
portNum - port number
pNThr - PN threshold

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecSetTxPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U32 pNThr
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
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold(dev, (MSD_U64)pNThr);
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
devNum  - physical devie number
portNum - port number

OUTPUTS:
	pNThr - PN threshold

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U32* pNThr
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
			
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold(dev, pNThr);
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
devNum  - physical devie number
portNum - port number
pNThr - PN threshold

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecSetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	IN MSD_U64 pNThr
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
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold(dev, (MSD_U64)pNThr);
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
devNum  - physical devie number
portNum - port number

OUTPUTS:
	pNThr - PN threshold

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxXPNThreshold
(
	IN  MSD_U8  devNum,
	OUT MSD_U64* pNThr
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

			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold(dev, pNThr);
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
devNum  - physical devie number
portNum - port number
defaultSCI - default SCI

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecSetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultSCI
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
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI(dev, portNum, defaultSCI);
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
devNum  - physical devie number
portNum - port number

OUTPUTS:
	defaultSCI - default SCI

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetRxDefaultSCI
(
	IN  MSD_U8  devNum,
	IN MSD_U16 portNum,
	OUT MSD_U64* defaultSCI
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
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI(dev, portNum, defaultSCI);
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
devNum  - physical devie number
portNum - port number
index - index
type - interrupt type
int_control - control

OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecSetTxSCIntCtrl
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
devNum  - physical devie number
portNum - port number
index - index
type - interrupt type

OUTPUTS:
	int_control - control

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetTxSCIntCtrl
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
devNum  - physical devie number
portNum - port number
type - interrupt global type
index - index
isAUTOClear - Auto clear, enable or disable

OUTPUTS:
	nextIndex - next valid index
	int_type - interrupt detailed type

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecGetNextTxSCIntStatus
(
	IN  MSD_U8  devNum,
	IN MSD_U8 index,
	IN MSD_BOOL isAUTOClear,
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
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus(dev, index, isAUTOClear, nextIndex, txscInt);
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
		This function is used to configure common macsec egress(encrypted) feature.
		Below is the common api rule:
		1. Any packets will be encrypted
		2. Key default is all 0
		3. Salt default is all 0
		4. SSCI default is 0
		4. Index means tcam rule index, secY index, SC index, SA0 index is 2*index, SA1 index is 2*index + 1
		5. Default SA0 AN is 0 and SA1 AN is 1

	INPUTS:
		devNum  - physical devie number
		portNum - port number
		index   - rule index
		cipherSuite - cipher suite
		isAutoRekey - if enable auto rekey
		txNextPNRolloEn - next PN rollover enable or not
		tci - sectag tci filed
		nextPN - Sa0 and SA1 next packets number
		thresholdPN - Sa0 and SA1 packets number threshold
		isActiveSA1 - If active SA1
		sci  - SCI
		secYCounterIndex - secY mibs index
		scCounterIndex - SC mibs index
	OUTPUTS:
		None

	RETURNS:
		MSD_STATUS

	COMMENTS:
		None
	******************************************************************************/
MSD_STATUS msdMacsecConfigureEgress
(
	IN  MSD_U8  devNum,
	IN  MSD_U8  ruleIndex,
	IN  MSD_U8  scIndex,
	IN  MSD_U8  secYIndex,
	IN  MSD_CIPHER_SUITE cipherSuite,
	IN  MSD_BOOL isAutoRekey,
	IN  MSD_U8   tci,
	IN  MSD_U64  nextPN,
	IN  MSD_U64  thresholdPN,
	IN  MSD_BOOL isActiveSA1,
	IN  MSD_U64  sci,
	IN  MSD_U8   secYCounterIndex,
	IN  MSD_U8   scCounterIndex
)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_MSEC_RULE rule;
	MSD_SECY_EGRESS txsecY;
	MSD_MSEC_TX_SC txSC;
	MSD_MSEC_TX_SA sa0, sa1;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));
	msdMemSet(&txsecY, 0, sizeof(MSD_SECY_EGRESS));
	msdMemSet(&txSC, 0, sizeof(MSD_MSEC_TX_SC));
	msdMemSet(&sa0, 0, sizeof(MSD_MSEC_TX_SA));
	msdMemSet(&sa1, 0, sizeof(MSD_MSEC_TX_SA));
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		/*Step 1, MACSec init*/
		if (dev->SwitchDevObj.MACSECObj.gmacsecFlushAll != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecFlushAll(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 2, Configure macsec rule, all the key and mask is 0, it means hit any packets*/
		rule.action.isControlledPort = MSD_TRUE;
		rule.action.SecYIndex = secYIndex;
		rule.action.TxSCIndex = scIndex;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRule != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRule(dev, ruleIndex, &rule);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 3, Configure macsec secY*/
		txsecY.cipher = cipherSuite;
		txsecY.protectFrame = MSD_TRUE;
		txsecY.secTagTCI = tci;
		txsecY.secYCounterIndex = secYCounterIndex;
		txsecY.txConfOffset = 12;/*Sectag after DA and SA*/
		txsecY.txSecYEn = MSD_TRUE;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY(dev, secYIndex, &txsecY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 4, Configure macsec sc*/
		txSC.enable_auto_rekey = isAutoRekey;
		txSC.isActiveSA1 = isActiveSA1;
		txSC.sa0Index = 2 * scIndex;
		txSC.sA0Valid = MSD_TRUE;
		txSC.sa1Index = 2 * scIndex + 1;
		txSC.sA1Valid = MSD_TRUE;
		txSC.scCounterIndex = scCounterIndex;
		txSC.sci = sci;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC(dev, scIndex, &txSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 5, Configure macsec sa*/
		sa0.nextPN = nextPN;
		sa0.txSAValid = MSD_TRUE;
		sa1.nextPN = nextPN;
		sa1.txSAValid = MSD_TRUE;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA(dev, 2 * scIndex, &sa0);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA(dev, 2 * scIndex + 1, &sa1);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 6, configure PN threshold*/
		thresholdPN = 0;
		if ((cipherSuite == 0)|(cipherSuite == 1))
		{
			if (dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold != NULL)
			{
				retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold(dev, (MSD_U32)thresholdPN);
			}
			else
			{
				retVal = MSD_NOT_SUPPORTED;
			}
		}
		else
		{
			if (dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold != NULL)
			{
				retVal = dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold(dev, thresholdPN);
			}
			else
			{
				retVal = MSD_NOT_SUPPORTED;
			}
		}
	}
	
	return retVal;
}
/******************************************************************************
DESCRIPTION:
	This function is used to configure common macsec ingress(decrypted) feature.
	Below is the common api rule:
	1. Any packets will be decrypted
	2. Key default is all 0
	3. Salt default is all 0
	4. SSCI default is 0
	4. Index means tcam rule index, secY index, SC index, SA0 index is 2*index, SA1 index is 2*index + 1
	5. Default SA0 AN is 0, SA1 AN is 1

INPUTS:
	devNum  - physical devie number
	portNum - port number
	index   - rule index
	cipherSuite - cipher suite
	validateFrame - valid frame mode
	nextPN - Sa0 and SA1 next packets number
	sci  - SCI
	secYCounterIndex - secY mibs index
	scCounterIndex - SC mibs index
	replayProtect - replay protect enable or not
	replayWindow - replay window value
OUTPUTS:
	None

RETURNS:
	MSD_STATUS

COMMENTS:
	None
******************************************************************************/
MSD_STATUS msdMacsecConfigureIngress
(
	IN  MSD_U8  devNum,
	IN  MSD_U8  ruleIndex,
	IN  MSD_U8  scIndex,
	IN  MSD_U8  secYIndex,
	IN  MSD_CIPHER_SUITE cipherSuite,
	IN  MSD_VALIDATE_FRAME validateFrame,
	IN  MSD_U64  nextPN,
	IN  MSD_U64  sci,
	IN  MSD_U8   secYCounterIndex,
	IN  MSD_U8   scCounterIndex, 
	IN  MSD_BOOL allowZeroRxPN,
	IN  MSD_BOOL replayProtect,
	IN  MSD_32  replayWindow
)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_MSEC_RULE rule;
	MSD_SECY_INGRESS rxsecY;
	MSD_MSEC_RX_SC rxSC;
	MSD_MSEC_RX_SA sa0, sa1;
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));
	msdMemSet(&rxsecY, 0, sizeof(MSD_SECY_INGRESS));
	msdMemSet(&rxSC, 0, sizeof(MSD_MSEC_RX_SC));
	msdMemSet(&sa0, 0, sizeof(MSD_MSEC_RX_SA));
	msdMemSet(&sa1, 0, sizeof(MSD_MSEC_RX_SA));
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		retVal = MSD_FAIL;
	}
	else
	{
		/*Step 1, MACSec init*/
		if (dev->SwitchDevObj.MACSECObj.gmacsecFlushAll != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecFlushAll(dev);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 2, Configure macsec rule, all the key and mask is 0, it means hit any packets*/
		rule.action.isControlledPort = MSD_TRUE;
		rule.action.SecYIndex = secYIndex;
		rule.action.TxSCIndex = scIndex;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRule != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRule(dev, ruleIndex, &rule);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 3, Configure macsec secY*/
		rxsecY.cipher = cipherSuite;
		rxsecY.replayProtect = replayProtect;
		rxsecY.replayWindow = replayWindow;
		rxsecY.rxConfOffset = 0; /*Default after sectag, all bytes are encrypted*/
		rxsecY.rxSecYEn = MSD_TRUE;
		rxsecY.secYCounterIndex = secYCounterIndex;
		rxsecY.allowZeroRxPN = allowZeroRxPN;
		rxsecY.validateFrame = validateFrame;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY(dev, secYIndex, &rxsecY);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 4, Configure macsec sc*/
		rxSC.scCounterIndex = scCounterIndex;
		rxSC.sci = sci;
		rxSC.secYIndex = secYIndex;
		rxSC.rxSA[0].an = 0;
		rxSC.rxSA[0].saIndex = 2 * scIndex;
		rxSC.rxSA[0].valid = MSD_TRUE;
		rxSC.rxSA[1].an = 1;
		rxSC.rxSA[1].saIndex = 2 * scIndex + 1;
		rxSC.rxSA[1].valid = MSD_TRUE;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC(dev, scIndex, &rxSC);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		/*Step 5, Configure macsec sa*/
		sa0.nextPN = nextPN;
		sa0.rxSAValid = MSD_TRUE;
		sa1.nextPN = nextPN;
		sa1.rxSAValid = MSD_TRUE;
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA(dev, 2 * scIndex, &sa0);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
		if (dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA != NULL)
		{
			retVal = dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA(dev, 2*scIndex + 1, &sa1);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}
	return retVal;
}