/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdQci.c
*
* DESCRIPTION:
*       API definitions for Qci (PSFP (Per-Stream Filtering and Policing))
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdQci.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/******************************************************************************
* msdStreamFilterFlushAll
*
* DESCRIPTION:
*        This routine flush all stream filter instances.
*
* INPUTS:
*        devNum			- physical device number
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
MSD_STATUS msdStreamFilterFlushAll
(
IN MSD_U8 devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll(dev);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterSet
*
* DESCRIPTION:
*        This routine enable stream filter instance.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*        maxSDUSize      - maximum SDU size
*        blockEn         - oversize blocking enable
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
MSD_STATUS msdStreamFilterSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
IN  MSD_U16  maxSDUSize,
IN  MSD_U8   blockEn
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltSet(dev, streamFilterId, maxSDUSize, blockEn);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterBlockStatusSet
*
* DESCRIPTION:
*        This routine sets stream filter block status.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*        blockStatus     - oversize blocking status
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
MSD_STATUS msdStreamFilterBlockStatusSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
IN  MSD_U8	 blockStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet(dev, streamFilterId, blockStatus);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterBlockStatusGet
*
* DESCRIPTION:
*        This routine gets stream filter block status.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*
*
* OUTPUTS:
*        blockStatus     - oversize blocking status
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
MSD_STATUS msdStreamFilterBlockStatusGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
OUT MSD_U8	 *blockStatus
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet(dev, streamFilterId, blockStatus);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterEnAtsSet
*
* DESCRIPTION:
*        This routine sets stream filter enable ATS(Qcr) instance id.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*        atsId           - ATS instance identifier
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
MSD_STATUS msdStreamFilterEnAtsSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
IN  MSD_U8	 atsId
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts(dev, streamFilterId, atsId);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterEnGateSet
*
* DESCRIPTION:
*        This routine sets stream filter enable stream gate instance id.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*        gateId          - stream gate instance identifier
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
MSD_STATUS msdStreamFilterEnGateSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
IN  MSD_U8	 gateId
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate(dev, streamFilterId, gateId);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterMatchCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter matched frames counter.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*
*
* OUTPUTS:
*        counter        - stream filter matched frames counter
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
MSD_STATUS msdStreamFilterMatchCounterGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
OUT MSD_U16	 *counter
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet(dev, streamFilterId, counter);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamFilterFiltedCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter filtered frames counter.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*
*
* OUTPUTS:
*        counter        - stream filter filtered frames counter
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
MSD_STATUS msdStreamFilterFiltedCounterGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
OUT MSD_U16	 *counter
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet(dev, streamFilterId, counter);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}

/******************************************************************************
* msdStreamGateFiltedCounterGet
*
* DESCRIPTION:
*        This routine gets stream gate filtered frames counter.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*
*
* OUTPUTS:
*        counter        - stream gate filtered frames counter
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
MSD_STATUS msdStreamGateFiltedCounterGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamFilterId,
OUT MSD_U16	 *counter
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet(dev, streamFilterId, counter);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}

/******************************************************************************
* msdFlowMeterFiltedCounterGet
*
* DESCRIPTION:
*        This routine gets flow meter filtered frames counter.
*
* INPUTS:
*        devNum          - physical device number
*        streamFilterId  - stream filter instance identifier
*
*
* OUTPUTS:
*        counter        - flow meter filtered frames counter
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
MSD_STATUS msdFlowMeterFiltedCounterGet
(
	IN  MSD_U8   devNum,
	IN  MSD_U8   streamFilterId,
	OUT MSD_U16* counter
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet(dev, streamFilterId, counter);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}

/******************************************************************************
* msdStreamGateFlushAll
*
* DESCRIPTION:
*        This routine flush all stream gate instances.
*
* INPUTS:
*        devNum			- physical device number
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
MSD_STATUS msdStreamGateFlushAll
(
IN MSD_U8 devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll(dev);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateDefStateSet
*
* DESCRIPTION:
*        This routine sets stream gate default gate state and gate IPV.
*
* INPUTS:
*        devNum          - physical device number
*        streamGateId    - stream gate instance identifier
*        defGateState    - default gate state
*        defGateIPVEn    - default gate IPV enable
*        defGateIPV      - default gate IPV
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
MSD_STATUS msdStreamGateDefStateSet
(
IN  MSD_U8   devNum,
IN  MSD_U8  streamGateId,
IN  MSD_U8	defGateState,
IN  MSD_U8	defGateIPVEn,
IN  MSD_U8	defGateIPV
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet(dev, streamGateId, defGateState, defGateIPVEn, defGateIPV);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateScaleSet
*
* DESCRIPTION:
*        This routine sets stream gate time scale, byte scale.
*
* INPUTS:
*        devNum          - physical device number
*        streamGateId    - stream gate instance identifier
*        timeScale       - time scale, 8ns, 16ns, 32ns, 64ns steps
*        byteScale       - byte scale, 1 byte, 4 byte, 16 byte, 64 byte steps
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
MSD_STATUS msdStreamGateScaleSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamGateId,
IN  MSD_U8	 timeScale,
IN  MSD_U8	 byteScale
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet(dev, streamGateId, timeScale, byteScale);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateTimeGenIdSet
*
* DESCRIPTION:
*        This routine sets stream gate time generation instance identifier.
*
* INPUTS:
*        devNum                 - physical device number
*        streamGateId           - stream gate instance identifier
*        streamGateTimeGenId	- stream gate time generation instance identifier
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
MSD_STATUS msdStreamGateTimeGenIdSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamGateId,
IN  MSD_U8	 streamGateTimeGenId
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet(dev, streamGateId, streamGateTimeGenId);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateEntryAdd
*
* DESCRIPTION:
*        This routine sets stream gate control entry.
*
* INPUTS:
*        devNum           - physical device number
*        streamGateId     - stream gate instance identifier
*        pointer          - pointer to the desired gate control entry
*        streamGateEntry  - gate control entry
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
MSD_STATUS msdStreamGateEntryAdd
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamGateId,
IN  MSD_U8   pointer,
IN  MSD_STREAM_GATE_ENTRY *streamGateEntry
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd(dev, streamGateId, pointer, streamGateEntry);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateTimeGenFlushAll
*
* DESCRIPTION:
*        This routine flush all stream gate time generation instances.
*
* INPUTS:
*        devNum      - physical device number
* 
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
MSD_STATUS msdStreamGateTimeGenFlushAll
(
IN  MSD_U8   devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush(dev);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdStreamGateTimeGenSet
*
* DESCRIPTION:
*        This routine sets stream gate time generation cycle time and base time.
*
* INPUTS:
*        devNum           - physical device number
*        streamGateGenId  - stream gate instance identifier
*        gateCycleTime    - gate cycle time, unit ns
*        gateBaseTime     - gate base time, PTP Global Timer, PTP ToD, PTP 1722 Nano
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
MSD_STATUS msdStreamGateTimeGenSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   streamGateGenId,
IN  MSD_U32  gateCycleTime,
IN  MSD_U64  gateBaseTime
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet(dev, streamGateGenId, gateCycleTime, gateBaseTime);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterFlushAll
*
* DESCRIPTION:
*        This routine flush all flow meter instances.
*
* INPUTS:
*        devNum      - physical device number
*
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
MSD_STATUS msdFlowMeterFlushAll
(
IN  MSD_U8   devNum
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll(dev);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterCouplingFlagSet
*
* DESCRIPTION:
*        This routine sets flow meter coupling flag.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*        couplingFlag     - coupling flag
*
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
MSD_STATUS msdFlowMeterCouplingFlagSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
IN  MSD_U8   couplingFlag
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet(dev, flowMeterId, couplingFlag);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterCouplingFlagGet
*
* DESCRIPTION:
*        This routine gets flow meter coupling flag.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier 
*
*
* OUTPUTS:
*        couplingFlag     - coupling flag.
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
MSD_STATUS msdFlowMeterCouplingFlagGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
OUT MSD_U8   *couplingFlag
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet(dev, flowMeterId, couplingFlag);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterDropOnYelSet
*
* DESCRIPTION:
*        This routine sets flow meter drop on yellow.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*        dropOnYel        - drop on yellow
*
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
MSD_STATUS msdFlowMeterDropOnYelSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
IN  MSD_U8   dropOnYel
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet(dev, flowMeterId, dropOnYel);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterDropOnYelGet
*
* DESCRIPTION:
*        This routine gets flow meter drop on yellow.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*
*
* OUTPUTS:
*        dropOnYel        - drop on yellow.
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
MSD_STATUS msdFlowMeterDropOnYelGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
OUT MSD_U8   *dropOnYel
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet(dev, flowMeterId, dropOnYel);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterRedBlockEnSet
*
* DESCRIPTION:
*        This routine sets flow meter red block enable.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*        enable           - red block enable
*
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
MSD_STATUS msdFlowMeterRedBlockEnSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
IN  MSD_U8   enable
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet(dev, flowMeterId, enable);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterRedBlockEnGet
*
* DESCRIPTION:
*        This routine gets flow meter red block enable.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*
*
* OUTPUTS:
*        enable          - red block enable.
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
MSD_STATUS msdFlowMeterRedBlockEnGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
OUT MSD_U8   *enable
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet(dev, flowMeterId, enable);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterRedBlockStatusSet
*
* DESCRIPTION:
*        This routine sets flow meter red block status.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*        status           - red block status
*
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
MSD_STATUS msdFlowMeterRedBlockStatusSet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
IN  MSD_U8   status
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet(dev, flowMeterId, status);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterRedBlockStatusGet
*
* DESCRIPTION:
*        This routine gets flow meter red block status.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*
*
* OUTPUTS:
*        status          - red block status.
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
MSD_STATUS msdFlowMeterRedBlockStatusGet
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
OUT MSD_U8   *status
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet(dev, flowMeterId, status);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}


/******************************************************************************
* msdFlowMeterAdvConfig
*
* DESCRIPTION:
*        This routine sets flow meter instance based on given rate and count mode.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId      - flow meter instance identifier
*        tgtRate          - target rate, units is bps
*        bstSize          - target burst size (units is byte)
*        countMode        - count mode
*
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
MSD_STATUS msdFlowMeterAdvConfig
(
IN  MSD_U8   devNum,
IN  MSD_U8   flowMeterId,
IN  MSD_U32  tgtRate,
IN  MSD_U32  bstSize,
IN  MSD_U8   countMode
)
{
	MSD_QD_DEV* dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}
	else
	{
		if (dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig != NULL)
		{
			return dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig(dev, flowMeterId, tgtRate, bstSize, countMode);
		}
		else
		{
			return MSD_NOT_SUPPORTED;
		}
	}
}