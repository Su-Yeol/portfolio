/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Oak_msdPIRL.c
*
* DESCRIPTION:
*       API definitions for PIRL Resources
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdPIRL.h>
#include <oak/include/driver/Oak_msdHwAccess.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

/*
 * Typedef: enum OAK_MSD_PIRL_OPERATION
 *
 * Description: Defines the PIRL (Port Ingress Rate Limit) Operation type
 *
 * Fields:
 *      Oak_PIRL_INIT_ALL_RESOURCE - Initialize all resources to the inital state
 *      Oak_PIRL_INIT_RESOURCE     - Initialize selected resources to the inital state
 *      Oak_PIRL_WRITE_RESOURCE    - Write to the selected resource/register
 *      Oak_PIRL_READ_RESOURCE     - Read from the selected resource/register
 */
typedef enum
{
	Oak_PIRL_READ_RESOURCE          = 0x0,
    Oak_PIRL_INIT_ALL_RESOURCE      = 0x1,
    Oak_PIRL_INIT_RESOURCE          = 0x2,
    Oak_PIRL_WRITE_RESOURCE         = 0x3
} OAK_MSD_PIRL_OPERATION;


/*
 *  typedef: struct OAK_MSD_PIRL_OP_DATA
 *
 *  Description: data required by PIRL Operation
 *
 *  Fields:
 *      irlUnit   - Ingress Rate Limit Unit that defines one of IRL resources.
 *      irlReg    - Ingress Rate Limit Register.
 *      irlData   - Ingress Rate Limit Data.
 */
typedef struct
{
    MSD_U32    irlUnit;
    MSD_U32    irlReg;
    MSD_U32    irlData;
} OAK_MSD_PIRL_OP_DATA;

/****************************************************************************/
/* STATS operation function declaration.                                    */
/****************************************************************************/
static MSD_STATUS Oak_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    OAK_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT OAK_MSD_PIRL_OP_DATA     *opData
);

static MSD_STATUS Oak_pirlOperationPerform_MultiChip
(
IN    MSD_QD_DEV           *dev,
IN    OAK_MSD_PIRL_OPERATION   pirlOp,
IN    MSD_U32              irlPort,
IN    MSD_U32              irlRes,
IN    MSD_U32              irlReg,
INOUT OAK_MSD_PIRL_OP_DATA     *opData
);


/*******************************************************************************
* Oak_gpirlInitialize
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
MSD_STATUS Oak_gpirlInitialize
(
    IN  MSD_QD_DEV              *dev
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    OAK_MSD_PIRL_OPERATION    op;

    MSD_DBG_INFO(("Oak_gpirlInitialize Called.\n"));

    op = Oak_PIRL_INIT_ALL_RESOURCE;

    retVal = Oak_pirlOperationPerform(dev, op, 0, 0, 0, NULL);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_INIT_ALL_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
    }

    MSD_DBG_INFO(("Oak_gpirlInitialize Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Oak_gpirlInitResource
*
* DESCRIPTION:
*       This routine initializes the selected PIRL Resource for a particular port.
*
* INPUTS:
*       irlPort	- tarOak_get port
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
MSD_STATUS Oak_gpirlInitResource
(
    IN    MSD_QD_DEV            *dev,
	IN    MSD_LPORT             irlPort,
    IN    MSD_U32               irlRes
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    OAK_MSD_PIRL_OPERATION    op;
    OAK_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Oak_gpirlInitResource Called.\n"));

    op = Oak_PIRL_INIT_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", irlPort));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (irlRes > (MSD_U32)7)
		{
			MSD_DBG_ERROR(("Bad irlRes: %u.\n", irlRes));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			retVal = Oak_pirlOperationPerform(dev, op, (MSD_U32)port, irlRes, (MSD_U32)0, &opData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_INIT_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Oak_gpirlInitResource Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Oak_gpirlCustomSetup_sr2c
*
* DESCRIPTION:
*       This function Calculate CBS/EBS/BI/BRFGrn/BRFYel/Delta according to user 
*       specific target rate, target burst size and countMode.
*
* INPUTS:
*       tgtRate	- target rate(units is fps when countMode = OAK_MSD_PIRL_COUNT_FRAME,
*				  otherwise units is bps)
*       tgtBstSize	- target burst size(units is Byte)
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
*			CBS >= BktIncrement * tOak_gtBstSize
*
*******************************************************************************/
MSD_STATUS Oak_gpirlCustomSetup_sr2c
(
    IN  MSD_U32	   	tgtRate,
    IN  MSD_U32	   	tgtBstSize,
	INOUT OAK_MSD_PIRL_CUSTOM_RATE_LIMIT  *customSetup
)
{
#if defined(__KERNEL__) && defined(LINUX)
    return MSD_NOT_SUPPORTED;
#else
	MSD_U32 IRL_constant = (MSD_U32)500000000;
	MSD_U16 bktIncrement = (MSD_U16)0x1FFF;
    MSD_U16 bktRateFactorGrn = 0U;
    MSD_U32 cbsLimit = (MSD_U32)0xFFFFFF;

    OAK_MSD_PIRL_CUSTOM_RATE_LIMIT Setup;
    MSD_DOUBLE	   	Rate = 0.0;
    MSD_DOUBLE delta = 1.0, deltaMin = 1.0, mode = 1.0;
    MSD_U32 j;
    MSD_DOUBLE tgRate = (MSD_DOUBLE)(tgtRate * 1000.0);

    MSD_DBG_INFO(("Oak_gpirlCustomSetup Called.\n"));

	if ((tgtBstSize < (MSD_U32)1600) || (tgtRate == (MSD_U32)0))
    {
        MSD_DBG_ERROR(("Bad tgtBstSize Or tgtRate, tgtBstSize: %u, tgtRate: %u.\n", tgtBstSize, tgtRate));
        return MSD_BAD_PARAM;
    }

    msdMemSet(&Setup, 0, sizeof(OAK_MSD_PIRL_CUSTOM_RATE_LIMIT));

	if (customSetup->countMode == OAK_MSD_PIRL_COUNT_FRAME)
	{
		IRL_constant /= 8U;
	}

    for (j = 1U; j < bktIncrement; j++)
    {
		if ((MSD_U32)((MSD_DOUBLE)(cbsLimit / j) + 0.5) < (tgtBstSize + 1U))
		{
			continue;
		}
		if ((MSD_U32)((tgRate / IRL_constant) * j * mode + 0.5) < 0xFFFF)
		{
			bktRateFactorGrn = (MSD_U16)((tgRate / IRL_constant) * j * mode + 0.5);
		}
		else
		{
			continue;
		}

        Rate = (MSD_DOUBLE)((IRL_constant / mode) * ((MSD_DOUBLE)bktRateFactorGrn / j));
        delta = (MSD_DOUBLE)(tgRate - Rate) / tgRate;
		if (delta < (MSD_DOUBLE)0)
		{
			delta = (MSD_DOUBLE)0 - delta;
		}

        if (deltaMin > delta)
        {
            deltaMin = delta;
            Setup.bktIncrement = j;
            Setup.bktRateFactorGrn = bktRateFactorGrn;
            Setup.cbsLimit = j * tgtBstSize;

			if (delta == (MSD_DOUBLE)0)
            {
                break;
            }
        }
    }

	if (deltaMin == (MSD_DOUBLE)1)
    {
        MSD_DBG_ERROR(("Failed (Oak_gpirlCustomSetup tune failed).\n"));
        return MSD_FAIL;
    }

    MSD_DBG_INFO(("delta((tgRate - rate)/tgRate) = %f", deltaMin));

    customSetup->isValid = MSD_TRUE;
    customSetup->cbsLimit = Setup.cbsLimit;
    customSetup->bktIncrement = Setup.bktIncrement;
    customSetup->bktRateFactorGrn = Setup.bktRateFactorGrn;

    customSetup->bktRateFactorYel = 0;
    customSetup->ebsLimit = 0;

    customSetup->Delta = deltaMin;

    MSD_DBG_INFO(("Oak_gpirlCustomSetup Exit.\n"));
    return MSD_OK;
#endif
}

/*******************************************************************************
* Oak_gpirlReadResource
*
* DESCRIPTION:
*       This function read Resource bucket parameter from the given resource of port
*
* INPUTS:
*       irlPort - tarOak_get logical port
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
MSD_STATUS Oak_gpirlReadResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    OUT OAK_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal = MSD_OK;     /* Functions return value */
    MSD_U16          data[9];		      /* temporary Data storage */
    OAK_MSD_PIRL_OPERATION    op;
    OAK_MSD_PIRL_OP_DATA      opData;
	MSD_U8			port;
    MSD_32          i;

    MSD_DBG_INFO(("Oak_gpirlReadResource Called.\n"));

    op = Oak_PIRL_READ_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", irlPort));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (irlRes > (MSD_U32)7)
		{
			MSD_DBG_ERROR(("Bad irlRes: %u.\n", irlRes));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			msdMemSet((void*)pirlData, 0, sizeof(OAK_MSD_PIRL_DATA));

			for (i = 0; i < 9; i++)
			{
				opData.irlData = 0;

				retVal = Oak_pirlOperationPerform(dev, op, (MSD_U32)port, irlRes, (MSD_U32)i, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
					retVal = MSD_FAIL;
					break;
				}
				else
				{
					data[i] = (MSD_U16)opData.irlData;
				}
			}

			if (retVal != MSD_FAIL)
			{
				pirlData->bktTypeMask = (data[0]) & (MSD_U16)0xFFFF;
				pirlData->tcamFlows = ((MSD_U16)(data[2] >> 13) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;
				pirlData->priAndPt = ((MSD_U16)(data[1] >> 9) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;
				pirlData->useFPri = ((MSD_U16)(data[1] >> 8) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;
				pirlData->priSelect = (MSD_U8)(data[1] & (MSD_U16)0xff);

				pirlData->colorAware = ((MSD_U16)(data[1] >> 13) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;

				pirlData->accountGrnOverflow = ((MSD_U16)(data[1] >> 12) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;
				pirlData->accountQConf = ((MSD_U16)(data[1] >> 11) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;
				pirlData->accountFiltered = ((MSD_U16)(data[1] >> 10) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;

				pirlData->samplingMode = ((MSD_U16)(data[1] >> 14) & (MSD_U16)0x1) == 1U ? MSD_TRUE : MSD_FALSE;

				pirlData->actionMode = ((MSD_U16)(data[8] >> 11) & (MSD_U16)0x1) == 1U ? Oak_PIRL_ACTION_FC : Oak_PIRL_ACTION_DROP;
				pirlData->fcMode = ((MSD_U16)(data[8] >> 12) & (MSD_U16)0x1) == 1U ? OAK_PIRL_FC_DEASSERT_EBS_LIMIT : OAK_PIRL_FC_DEASSERT_EMPTY;
				pirlData->fcPriority = (MSD_U8)((data[8] >> 13) & (MSD_U16)0x7);

				pirlData->customSetup.ebsLimit = ((MSD_U32)((MSD_U32)data[8] & (MSD_U32)0xFF) << 16) | ((MSD_U32)data[7] & (MSD_U32)0xFFFF);
				pirlData->customSetup.cbsLimit = ((MSD_U32)((MSD_U32)data[5] & (MSD_U32)0xFF) << 16) | ((MSD_U32)data[4] & (MSD_U32)0xFFFF);
				pirlData->customSetup.bktIncrement = (MSD_U32)data[2] & (MSD_U32)0x1FFF;
				pirlData->customSetup.bktRateFactorGrn = (MSD_U32)data[3] & (MSD_U32)0xFFFF;
				pirlData->customSetup.bktRateFactorYel = (MSD_U32)data[6] & (MSD_U32)0xFFFF;
				pirlData->customSetup.countMode = (OAK_MSD_PIRL_COUNT_MODE)(MSD_U16)((MSD_U16)(data[2] >> 14) & (MSD_U16)3);
				pirlData->customSetup.isValid = MSD_TRUE;
			}
		}
	}

    MSD_DBG_INFO(("Oak_gpirlReadResource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gpirlWriteResource
*
* DESCRIPTION:
*       This function writes Resource bucket parameter to the given resource of port
*
* INPUTS:
*       irlPort	- tarOak_get port
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
MSD_STATUS Oak_gpirlWriteResource
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_LPORT    	irlPort,
	IN  MSD_U32        	irlRes,
    IN  OAK_MSD_PIRL_DATA    *pirlData
)
{
    MSD_STATUS       retVal = MSD_OK; /* Functions return value */
    MSD_U16          data[9];     /* temporary Data storage */
    OAK_MSD_PIRL_OPERATION    op;
    OAK_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;
    MSD_16          i;

    MSD_DBG_INFO(("Oak_gpirlWriteResource Called.\n"));

    op = Oak_PIRL_WRITE_RESOURCE;
	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", irlPort));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (irlRes > (MSD_U32)7)
		{
			MSD_DBG_ERROR(("Bad irlRes: %u.\n", irlRes));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			/* reg0 data */
			data[0] = (MSD_U16)(pirlData->bktTypeMask & (MSD_U32)0xFFFF);        /* Bit[15:0] : Traffic Type   */

			/* reg1 data */
			data[1] = (MSD_U16)((MSD_U16)((MSD_U16)((pirlData->samplingMode == MSD_TRUE) ? 1U : 0U) << 14) |	   /* Bit[14] : sampling Mode */
				(MSD_U16)((MSD_U16)((pirlData->colorAware == MSD_TRUE) ? 1U : 0U) << 13) |			             /* Bit[13] : Color Aware */
				(MSD_U16)((MSD_U16)((pirlData->accountGrnOverflow == MSD_TRUE) ? 1U : 0U) << 12) |             /* Bit[12] : AcctForGrnOvflow */
				(MSD_U16)((MSD_U16)((pirlData->accountQConf == MSD_TRUE) ? 1U : 0U) << 11) |		           /* Bit[11] : AcctForQConf */
				(MSD_U16)((MSD_U16)((pirlData->accountFiltered == MSD_TRUE) ? 1U : 0U) << 10) |              /* Bit[10] : AcctForFiltered */
				(MSD_U16)((MSD_U16)((pirlData->priAndPt == MSD_TRUE) ? 1U : 0U) << 9) |			         /* Bit[9] : Priority And Packet Type */
				(MSD_U16)((MSD_U16)((pirlData->useFPri == MSD_TRUE) ? 1U : 0U) << 8) |			             /* Bit[8] : Frame Priority vs Queue Priority */
				(MSD_U16)(pirlData->priSelect & (MSD_U16)0xFF)); 					            /* Bit[7:0] : Priority Select */

			/* reg2 data */
			data[2] = (MSD_U16)((MSD_U16)((MSD_U16)pirlData->customSetup.countMode << 14) |
				(MSD_U16)((MSD_U16)((pirlData->tcamFlows == MSD_TRUE) ? 1U : 0U) << 13) |
				(MSD_U16)((pirlData->customSetup.bktIncrement) & (MSD_U16)0x1FFF));

			/* reg3 data */
			data[3] = (MSD_U16)(pirlData->customSetup.bktRateFactorGrn & (MSD_U16)0xFFFF);    /* Bit[15:0] : Bucket Rate Factor Green */

			/* reg4 data */
			data[4] = (MSD_U16)(pirlData->customSetup.cbsLimit & (MSD_U16)0xFFFF);   /* Bit[15:0] : CBS Limit[15:0] */

			/* reg5 data */
			data[5] = (MSD_U16)((MSD_U16)(pirlData->customSetup.cbsLimit >> 16) & (MSD_U16)0xFF);   /* Bit[7:0] : CBS Limit[23:16] */

			/* reg6 data */
			data[6] = (MSD_U16)(pirlData->customSetup.bktRateFactorYel & (MSD_U16)0xFFFF);		/* Bit[15:0] : Bucket Rate Factor Yellow */

			/* reg7 data */
			data[7] = (MSD_U16)(pirlData->customSetup.ebsLimit & (MSD_U16)0xFFFF);	/* Bit[15:0] : EBS Limit[15:0] */

			/* reg8 data */
			data[8] = (MSD_U16)((MSD_U16)(((MSD_U16)pirlData->fcPriority & (MSD_U16)0x7) << 13) |
				(MSD_U16)((MSD_U16)pirlData->fcMode << 12) |
				(MSD_U16)((MSD_U16)pirlData->actionMode << 11) |
				(MSD_U16)((MSD_U32)(pirlData->customSetup.ebsLimit >> 16) & (MSD_U32)0xFF));

			for (i = 0; i < 9; i++)
			{
				opData.irlData = data[i];

				retVal = Oak_pirlOperationPerform(dev, op, (MSD_U32)port, irlRes, (MSD_U32)i, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
					break;
				}
			}
		}
	}

    MSD_DBG_INFO(("Oak_gpirlWriteResource Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gpirlGetResReg
*
* DESCRIPTION:
*       This routine read general register value from the given resource of the port.
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarOak_get resource 
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
MSD_STATUS Oak_gpirlGetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32 	regOffset,
    OUT MSD_U16		 *data
)
{
	MSD_STATUS       retVal;            /* Functions return value */
    OAK_MSD_PIRL_OPERATION    op;
    OAK_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Oak_gpirlGetResReg Called.\n"));

	if (NULL == data)
	{
		MSD_DBG_ERROR(("Input param MSD_U16 data is NULL. \n"));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		op = Oak_PIRL_READ_RESOURCE;

		port = MSD_LPORT_2_PORT(irlPort);
		if (port == MSD_INVALID_PORT)
		{
			MSD_DBG_ERROR(("Bad Port: %u.\n", irlPort));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			if (irlRes > (MSD_U32)7)
			{
				MSD_DBG_ERROR(("Bad irlRes: %u.\n", irlRes));
				retVal = MSD_BAD_PARAM;
			}
			else
			{
				opData.irlData = 0;

				retVal = Oak_pirlOperationPerform(dev, op, (MSD_U32)port, irlRes, regOffset, &opData);
				if (retVal != MSD_OK)
				{
					MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_READ_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
				}
				else
				{
					*data = (MSD_U16)opData.irlData;
				}
			}
		}
	}

    MSD_DBG_INFO(("Oak_gpirlGetResReg Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Oak_gpirlSetResReg
*
* DESCRIPTION:
*       This routine set general register value to the given resource of the port..
*
* INPUTS:
*       irlPort   - logical port number
*       irlRes - tarOak_get resource 
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
MSD_STATUS Oak_gpirlSetResReg
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT	 irlPort,
	IN  MSD_U32		 irlRes,
    IN  MSD_U32      regOffset,
    IN  MSD_U16		 data
)
{
	MSD_STATUS       retVal;            /* Functions return value */
    OAK_MSD_PIRL_OPERATION    op;
    OAK_MSD_PIRL_OP_DATA        opData;
	MSD_U8			port;

    MSD_DBG_INFO(("Oak_gpirlSetResReg Called.\n"));

    op = Oak_PIRL_WRITE_RESOURCE;

	port = MSD_LPORT_2_PORT(irlPort);
	if(port == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", irlPort));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		if (irlRes > (MSD_U32)7)
		{
			MSD_DBG_ERROR(("Bad irlRes: %u.\n", irlRes));
			retVal = MSD_BAD_PARAM;
		}
		else
		{
			opData.irlData = data;

			retVal = Oak_pirlOperationPerform(dev, op, (MSD_U32)port, irlRes, regOffset, &opData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Oak_pirlOperationPerform Oak_PIRL_WRITE_RESOURCE returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Oak_gpirlSetResReg Exit.\n"));
	return retVal;
}

/*******************************************************************************
* statsOperationPerform
*
* DESCRIPTION:
*       This function accesses Ingress Rate Command Register and Data Register.
*
* INPUTS:
*       pirlOp       - The stats operation bits to be written into the stats
*                     operation register.
*       irlPort      - port number
*       irlRes       - resource unit to be accessed
*       irlReg       - IRL register address to be accessed
*       counter      - counter to be read if it's read operation
*
* OUTPUTS:
*       pirlData   - points to the data storage where the MIB counter will be saved.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static MSD_STATUS Oak_pirlOperationPerform_MultiChip
(
    IN    MSD_QD_DEV           *dev,
    IN    OAK_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT OAK_MSD_PIRL_OP_DATA     *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U16          data;     /* temporary Data storage */

    msdSemTake(dev->devNum, dev->pirlRegsSem, OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
    case Oak_PIRL_INIT_ALL_RESOURCE:
		data = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_INIT_ALL_RESOURCE << 13);
		retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;
    case Oak_PIRL_INIT_RESOURCE:
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_INIT_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
			(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5));
		retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;

    case Oak_PIRL_WRITE_RESOURCE:
        data = (MSD_U16)opData->irlData;
		retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_IGR_RATE_DATA_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }

		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_WRITE_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
			(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5) | (MSD_U16)((MSD_U16)irlReg & (MSD_U16)0xF));
		retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        break;

    case Oak_PIRL_READ_RESOURCE:
		data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_READ_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
			(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5) | (MSD_U16)((MSD_U16)irlReg & (MSD_U16)0xF));
		retVal = Oak_msdDirectSetMultiChipReg(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }

		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
        }

		retVal = Oak_msdDirectGetMultiChipReg(dev, OAK_IGR_RATE_DATA_MULTICHIP, &data);
        opData->irlData = (MSD_U32)data;
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
        msdSemGive(dev->devNum, dev->pirlRegsSem);
        return retVal;

    default:
        msdSemGive(dev->devNum, dev->pirlRegsSem);
        return MSD_FAIL;
		break;
    }

    /* Wait until the pirl in ready. */

	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = Oak_msdDirectGetMultiChipRegField(dev, OAK_IGR_RATE_COMMAND_MULTICHIP, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    msdSemGive(dev->devNum, dev->pirlRegsSem);
    return retVal;
}

static MSD_STATUS Oak_pirlOperationPerform
(
    IN    MSD_QD_DEV           *dev,
    IN    OAK_MSD_PIRL_OPERATION   pirlOp,
    IN    MSD_U32              irlPort,
    IN    MSD_U32              irlRes,
    IN    MSD_U32              irlReg,
    INOUT OAK_MSD_PIRL_OP_DATA     *opData
)
{
    MSD_STATUS       retVal;    /* Functions return value */
    MSD_U16          data;     /* temporary Data storage */

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Oak_pirlOperationPerform_MultiChip(dev, pirlOp, irlPort, irlRes, irlReg, opData);
    }

    msdSemTake(dev->devNum, dev->pirlRegsSem,OS_WAIT_FOREVER);

    /* Wait until the pirl in ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    /* Set the PIRL Operation register */
    switch (pirlOp)
    {
        case Oak_PIRL_INIT_ALL_RESOURCE:
			data = (MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_INIT_ALL_RESOURCE << 13);
			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;
        case Oak_PIRL_INIT_RESOURCE:
			data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_INIT_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
				(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5));
			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

        case Oak_PIRL_WRITE_RESOURCE:
            data = (MSD_U16)opData->irlData;
			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_DATA, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

			data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_WRITE_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
				(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5) | (MSD_U16)((MSD_U16)irlReg & (MSD_U16)0xF));
			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            break;

        case Oak_PIRL_READ_RESOURCE:
			data = (MSD_U16)((MSD_U16)0x8000 | (MSD_U16)((MSD_U16)Oak_PIRL_READ_RESOURCE << 13) | (MSD_U16)(((MSD_U16)irlPort & (MSD_U16)0x1f) << 8) |
				(MSD_U16)(((MSD_U16)irlRes & (MSD_U16)0x7) << 5) | (MSD_U16)((MSD_U16)irlReg & (MSD_U16)0xF));
			retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, data);
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }

			data = (MSD_U16)1;
			while (data == (MSD_U16)1)
            {
				retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
                if(retVal != MSD_OK)
                {
                    msdSemGive(dev->devNum, dev->pirlRegsSem);
                    return retVal;
                }
            }

			retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_DATA, &data);
            opData->irlData = (MSD_U32)data;
            if(retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->pirlRegsSem);
                return retVal;
            }
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;

        default:
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return MSD_FAIL;
			break;
    }

    /* Wait until the pirl in ready. */

	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_IGR_RATE_COMMAND, (MSD_U8)15, (MSD_U8)1, &data);
        if(retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->pirlRegsSem);
            return retVal;
        }
    }

    msdSemGive(dev->devNum, dev->pirlRegsSem);
    return retVal;
}
