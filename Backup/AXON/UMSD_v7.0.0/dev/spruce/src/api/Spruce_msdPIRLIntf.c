/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Spruce_msdPIRLIntf.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <spruce/include/api/Spruce_msdPIRL.h>
#include <spruce/include/api/Spruce_msdApiInternal.h>
#include <spruce/include/driver/Spruce_msdHwAccess.h>
#include <spruce/include/driver/Spruce_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/*******************************************************************************
* Spruce_gpirlInitialize
*
* DESCRIPTION:
*       This routine initializes all PIRL Resources for all ports.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlInitializeIntf
(
IN  MSD_QD_DEV              *dev
)
{
	return Spruce_gpirlInitialize(dev);
}

/*******************************************************************************
* Spruce_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- tarSpruce_get port
*		irlRes - resource unit to be accessed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlInitResourceIntf
(
IN    MSD_QD_DEV            *dev,
IN    MSD_LPORT             irlPort,
IN    MSD_U32               irlRes
)
{
	return Spruce_gpirlInitResource(dev, irlPort, irlRes);
}

/*******************************************************************************
* Spruce_gpirlCustomSetup_sr2c
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user 
*       specific tarSpruce_get rate, tarSpruce_get burst size and countMode.
*
* INPUTS:
*       tSpruce_gtRate	- tarSpruce_get rate(units is fps when countMode = SPRUCE_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is kbps)
*       tSpruce_gtBstSize	- tarSpruce_get burst size(units is Byte)
*       customSetup - Constant and countMode in customSetup as input parameter
*
* OUTPUTS:
*       customSetup - customer setup including CBS/EBS/BI/BRFGrn/BRFYel/Delta/countMode
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		We have the following contrain in calculate the final parameters:
*			CBS >= BktIncrement * tSpruce_gtBstSize
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlCustomSetup_sr2cIntf
(
IN  MSD_U32	   	tgtRate,
IN  MSD_U32	   	tgtBstSize,
IN  MSD_PIRL_COUNT_MODE countMode,
OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
	SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT setup;
	MSD_STATUS       retVal;

	if (NULL == customSetup)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_CUSTOM_RATE_LIMIT in Spruce_gpirlCustomSetup_sr2cIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	setup.isValid = customSetup->isValid;
	setup.ebsLimit = customSetup->ebsLimit;
	setup.cbsLimit = customSetup->cbsLimit;
	setup.bktIncrement = customSetup->bktIncrement;
	setup.bktRateFactorGrn = customSetup->bktRateFactorGrn;
	setup.bktRateFactorYel = customSetup->bktRateFactorYel;
	setup.countMode = countMode;

	retVal = Spruce_gpirlCustomSetup_sr2c( tgtRate, tgtBstSize, &setup);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	msdMemSet((void*)customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT));

	customSetup->isValid = setup.isValid;
	customSetup->ebsLimit = setup.ebsLimit;
	customSetup->cbsLimit = setup.cbsLimit;
	customSetup->bktIncrement = setup.bktIncrement;
	customSetup->bktRateFactorGrn = setup.bktRateFactorGrn;
	customSetup->countMode = setup.countMode;
	customSetup->bktRateFactorYel = setup.bktRateFactorYel;

	return retVal;
}

/*******************************************************************************
* Spruce_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - tarSpruce_get logical port
*		irlRes  - resource unit to be accessed
*
* OUTPUTS:
*       pirlData - IRL Resource data
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlReadResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
OUT MSD_PIRL_DATA    *pirlData
)
{
	SPRUCE_MSD_PIRL_DATA pirl;
	MSD_STATUS       retVal;

	retVal= Spruce_gpirlReadResource(dev, irlPort, irlRes, &pirl);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Spruce_gpirlReadResourceIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}
	msdMemSet((void*)pirlData, 0, sizeof(MSD_PIRL_DATA));

	pirlData->bktTypeMask = pirl.bktTypeMask;
	pirlData->tcamFlows = pirl.tcamFlows;
	pirlData->priAndPt = pirl.priAndPt;
	pirlData->useFPri = pirl.useFPri;
	pirlData->priSelect = pirl.priSelect;
	pirlData->colorAware = pirl.colorAware;
	pirlData->accountGrnOverflow = pirl.accountGrnOverflow;
	pirlData->accountQConf = pirl.accountQConf;
	pirlData->accountFiltered = pirl.accountFiltered;
	pirlData->samplingMode = pirl.samplingMode;
	pirlData->actionMode = pirl.actionMode;
	pirlData->fcMode = pirl.fcMode;
	pirlData->fcPriority = pirl.fcPriority;
	pirlData->customSetup.isValid = pirl.customSetup.isValid;
	pirlData->customSetup.ebsLimit = pirl.customSetup.ebsLimit;
	pirlData->customSetup.cbsLimit = pirl.customSetup.cbsLimit;
	pirlData->customSetup.bktIncrement = pirl.customSetup.bktIncrement;
	pirlData->customSetup.bktRateFactorGrn = pirl.customSetup.bktRateFactorGrn;
	pirlData->customSetup.bktRateFactorYel = pirl.customSetup.bktRateFactorYel;

	pirlData->customSetup.countMode = pirl.customSetup.countMode;
	return retVal;
}

/*******************************************************************************
* Spruce_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- tarSpruce_get port
*		irlRes  - resource unit to be accessed
*       pirlData - IRL Resource data
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
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlWriteResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
IN  MSD_PIRL_DATA    *pirlData
)
{
	SPRUCE_MSD_PIRL_DATA pirl;

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Spruce_gpirlWriteResourceIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	pirl.bktTypeMask = pirlData->bktTypeMask;
	pirl.tcamFlows = pirlData->tcamFlows;
	pirl.priAndPt = pirlData->priAndPt;
	pirl.useFPri = pirlData->useFPri;
	pirl.priSelect = pirlData->priSelect;
	pirl.colorAware = pirlData->colorAware;
	pirl.accountGrnOverflow = pirlData->accountGrnOverflow;
	pirl.accountQConf = pirlData->accountQConf;
	pirl.accountFiltered = pirlData->accountFiltered;
	pirl.samplingMode = pirlData->samplingMode;
	pirl.actionMode = pirlData->actionMode;
	pirl.fcMode = pirlData->fcMode;
	pirl.fcPriority = pirlData->fcPriority;
	pirl.customSetup.isValid = pirlData->customSetup.isValid;
	pirl.customSetup.ebsLimit = pirlData->customSetup.ebsLimit;
	pirl.customSetup.cbsLimit = pirlData->customSetup.cbsLimit;
	pirl.customSetup.bktIncrement = pirlData->customSetup.bktIncrement;
	pirl.customSetup.bktRateFactorGrn = pirlData->customSetup.bktRateFactorGrn;
	pirl.customSetup.bktRateFactorYel = pirlData->customSetup.bktRateFactorYel;

	pirl.customSetup.countMode = pirlData->customSetup.countMode;

	return Spruce_gpirlWriteResource(dev, irlPort, irlRes, &pirl);

}

/*******************************************************************************
* Spruce_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource 
*       regOffset - register address
*
* OUTPUTS:
*        data	- register value
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlGetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
OUT MSD_U16		 *data
)
{
	return Spruce_gpirlGetResReg(dev, irlPort, irlRes, regOffset, data);
}

/*******************************************************************************
* Spruce_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource 
*       regOffset - register address
*       data	- register value
*
* OUTPUTS:
*        
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlSetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 	 regOffset,
IN  MSD_U16		 data
)
{
	return Spruce_gpirlSetResReg(dev, irlPort, irlRes, regOffset, data);
}

/*******************************************************************************
* Spruce_gpirlAdvConfigIntf
*
* DESCRIPTION:
*       This routine config ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlAdvConfigIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes,
IN  MSD_U32  tgtRate,
IN  MSD_U32  tgtBstSize,
IN  MSD_PIRL_COUNT_MODE  countMode,
IN  MSD_PIRL_PKTTYPE_OR_PRI  *pktTypeOrPri
)
{
    MSD_STATUS status;
    SPRUCE_MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    customSetup.countMode = countMode;

	status = Spruce_gpirlCustomSetup_sr2c(tgtRate, tgtBstSize, &customSetup);
    if (status != MSD_OK)
    {
        return status;
    }

    SPRUCE_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(MSD_PIRL_DATA));

    pirl.bktTypeMask = pktTypeOrPri->bktTypeMask;
    pirl.tcamFlows = pktTypeOrPri->tcamFlows;
    pirl.priAndPt = pktTypeOrPri->priAndPt;
    pirl.useFPri = pktTypeOrPri->useFPri;
    pirl.priSelect = pktTypeOrPri->priSelect;
    pirl.accountQConf = MSD_TRUE;
    pirl.actionMode = Spruce_PIRL_ACTION_DROP;

    pirl.customSetup.isValid = customSetup.isValid;
    pirl.customSetup.ebsLimit = customSetup.ebsLimit;
    pirl.customSetup.cbsLimit = customSetup.cbsLimit;
    pirl.customSetup.bktIncrement = customSetup.bktIncrement;
    pirl.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
    pirl.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;

    pirl.customSetup.countMode = customSetup.countMode;

    return Spruce_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}

/*******************************************************************************
* Spruce_gpirlAdvConfigDisableIntf
*
* DESCRIPTION:
*       This routine clear config for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarSpruce_get resource
*       regOffset - register address
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gpirlAdvConfigDisableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes
)
{
    SPRUCE_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(SPRUCE_MSD_PIRL_DATA));

    return Spruce_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}