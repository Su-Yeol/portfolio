/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdQav.c
*
* DESCRIPTION:
*       API definitions for Qav 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdQav.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/******************************************************************************
* msdQavPortQpriXRateSet
*
* DESCRIPTION:
*        This routine sets queue rate for specified priority queue on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum - the logical port number
*        queue - queue priority
*        rate  - priority queue rate
*                value of 0 will disable this queue's Qav rate shaper
*                rate in kbps and increments of 64kbps, a maximum supported rate is 1000Mbps
*                therefore, the valid value are:
*                64, 128, 192, 256, 320, 384,..., 1000000
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXRateSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    IN  MSD_U32     rate
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
		if (dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate != NULL)
		{
			retVal = dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate(dev, portNum, queue, rate);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}
/******************************************************************************
* msdQavPortQpriXRateGet
*
* DESCRIPTION:
*        This routine gets priority queue rate on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum - the logical port number
*        queue   - queue priority
*
* OUTPUTS:
*        rate  - priority queue rate
*                value of 0 will disable this queue's Qav rate shaper
*                rate in kbps and increments of 64kbps, a maximum supported rate is 1000Mbps
*                therefore, the valid value are:
*                64, 128, 192, 256, 320, 384,..., 1000000
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXRateGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    OUT MSD_U32     *rate
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
		if (dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate != NULL)
		{
			retVal = dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate(dev, portNum, queue, rate);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdQavPortQpriXHiLimitSet
*
* DESCRIPTION:
*        This routine sets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXHiLimitSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    IN  MSD_U16     hiLimit
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
		if (dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit != NULL)
		{
			retVal = dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit(dev, portNum, queue, hiLimit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}

/******************************************************************************
* msdQavPortQpriXHiLimitGet
*
* DESCRIPTION:
*        This routine gets priority queue HiLimit on a port.
*
* INPUTS:
*        devNum  - physical device number
*        portNum    - the logical port number
*        queue   - 2 - 3
*        hiLimit - number of credits in bytes .
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK      - on success
*        MSD_FAIL    - on error
*        MSD_BAD_PARAM - if input parameters are beyond range.
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None
*
******************************************************************************/
MSD_STATUS msdQavPortQpriXHiLimitGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      queue,
    OUT MSD_U16     *hiLimit
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
		if (dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit != NULL)
		{
			retVal = dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit(dev, portNum, queue, hiLimit);
		}
		else
		{
			retVal = MSD_NOT_SUPPORTED;
		}
	}

	return retVal;
}