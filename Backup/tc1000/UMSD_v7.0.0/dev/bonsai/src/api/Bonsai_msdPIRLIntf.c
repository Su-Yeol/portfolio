/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Bonsai_msdPIRLIntf.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsai/include/api/Bonsai_msdPIRL.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/*******************************************************************************
* Bonsai_gpirlInitialize
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
MSD_STATUS Bonsai_gpirlInitializeIntf
(
IN  MSD_QD_DEV              *dev
)
{
	return Bonsai_gpirlInitialize(dev);
}

/*******************************************************************************
* Bonsai_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- tarBonsai_get port
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
MSD_STATUS Bonsai_gpirlInitResourceIntf
(
IN    MSD_QD_DEV            *dev,
IN    MSD_LPORT             irlPort,
IN    MSD_U32               irlRes
)
{
	return Bonsai_gpirlInitResource(dev, irlPort, irlRes);
}

/*******************************************************************************
* Bonsai_gpirlCustomSetup_sr2c
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user 
*       specific tarBonsai_get rate, tarBonsai_get burst size and countMode.
*
* INPUTS:
*       tBonsai_gtRate	- tarBonsai_get rate(units is fps when countMode = BONSAI_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is kbps)
*       tBonsai_gtBstSize	- tarBonsai_get burst size(units is Byte)
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
*			CBS >= BktIncrement * tBonsai_gtBstSize
*
*******************************************************************************/
MSD_STATUS Bonsai_gpirlCustomSetup_sr2cIntf
(
IN  MSD_U32	   	tgtRate,
IN  MSD_U32	   	tgtBstSize,
IN  MSD_PIRL_COUNT_MODE countMode,
OUT MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
	BONSAI_MSD_PIRL_CUSTOM_RATE_LIMIT setup;
	MSD_STATUS       retVal;

	if (NULL == customSetup)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_CUSTOM_RATE_LIMIT in Bonsai_gpirlCustomSetup_sr2cIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	setup.isValid = customSetup->isValid;
	setup.ebsLimit = customSetup->ebsLimit;
	setup.cbsLimit = customSetup->cbsLimit;
	setup.bktIncrement = customSetup->bktIncrement;
	setup.bktRateFactorGrn = customSetup->bktRateFactorGrn;
	setup.bktRateFactorYel = customSetup->bktRateFactorYel;
	setup.countMode = countMode;

	retVal = Bonsai_gpirlCustomSetup_sr2c( tgtRate, tgtBstSize, &setup);
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
* Bonsai_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - tarBonsai_get logical port
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
MSD_STATUS Bonsai_gpirlReadResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
OUT MSD_PIRL_DATA    *pirlData
)
{
	BONSAI_MSD_PIRL_DATA pirl;
	MSD_STATUS       retVal;

	retVal= Bonsai_gpirlReadResource(dev, irlPort, irlRes, &pirl);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Bonsai_gpirlReadResourceIntf is NULL. \n"));
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
* Bonsai_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- tarBonsai_get port
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
MSD_STATUS Bonsai_gpirlWriteResourceIntf
(
IN  MSD_QD_DEV       *dev,
IN  MSD_LPORT    	irlPort,
IN  MSD_U32        	irlRes,
IN  MSD_PIRL_DATA    *pirlData
)
{
	BONSAI_MSD_PIRL_DATA pirl;

	if (NULL == pirlData)
	{
		MSD_DBG_ERROR(("Input param MSD_PIRL_DATA in Bonsai_gpirlWriteResourceIntf is NULL. \n"));
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

	return Bonsai_gpirlWriteResource(dev, irlPort, irlRes, &pirl);

}

/*******************************************************************************
* Bonsai_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarBonsai_get resource 
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
MSD_STATUS Bonsai_gpirlGetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 		 regOffset,
OUT MSD_U16		 *data
)
{
	return Bonsai_gpirlGetResReg(dev, irlPort, irlRes, regOffset, data);
}

/*******************************************************************************
* Bonsai_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarBonsai_get resource 
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
MSD_STATUS Bonsai_gpirlSetResRegIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT	 irlPort,
IN  MSD_U32		 irlRes,
IN  MSD_U32 	 regOffset,
IN  MSD_U16		 data
)
{
	return Bonsai_gpirlSetResReg(dev, irlPort, irlRes, regOffset, data);
}

/*******************************************************************************
* Bonsai_gpirlAdvConfigIntf
*
* DESCRIPTION:
*       This routine config ingress rate limit based on packet type or priority
*       the given resource of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarBonsai_get resource
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
MSD_STATUS Bonsai_gpirlAdvConfigIntf
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
    BONSAI_MSD_PIRL_CUSTOM_RATE_LIMIT customSetup;
    customSetup.countMode = countMode;

	status = Bonsai_gpirlCustomSetup_sr2c(tgtRate, tgtBstSize, &customSetup);
    if (status != MSD_OK)
    {
        return status;
    }

    BONSAI_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(MSD_PIRL_DATA));

    pirl.bktTypeMask = pktTypeOrPri->bktTypeMask;
    pirl.tcamFlows = pktTypeOrPri->tcamFlows;
    pirl.priAndPt = pktTypeOrPri->priAndPt;
    pirl.useFPri = pktTypeOrPri->useFPri;
    pirl.priSelect = pktTypeOrPri->priSelect;
    pirl.accountQConf = MSD_TRUE;
    pirl.actionMode = Bonsai_PIRL_ACTION_DROP;

    pirl.customSetup.isValid = customSetup.isValid;
    pirl.customSetup.ebsLimit = customSetup.ebsLimit;
    pirl.customSetup.cbsLimit = customSetup.cbsLimit;
    pirl.customSetup.bktIncrement = customSetup.bktIncrement;
    pirl.customSetup.bktRateFactorGrn = customSetup.bktRateFactorGrn;
    pirl.customSetup.bktRateFactorYel = customSetup.bktRateFactorYel;

    pirl.customSetup.countMode = customSetup.countMode;

    return Bonsai_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}

/*******************************************************************************
* Bonsai_gpirlAdvConfigDisableIntf
*
* DESCRIPTION:
*       This routine clear config for ingress rate limit to the given resource
*       of the port
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarBonsai_get resource
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
MSD_STATUS Bonsai_gpirlAdvConfigDisableIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT  irlPort,
IN  MSD_U32  irlRes
)
{
    BONSAI_MSD_PIRL_DATA pirl;
    msdMemSet(&pirl, 0, sizeof(BONSAI_MSD_PIRL_DATA));

    return Bonsai_gpirlWriteResource(dev, irlPort, irlRes, &pirl);
}