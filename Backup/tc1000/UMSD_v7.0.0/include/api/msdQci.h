/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdQci.h
*
* DESCRIPTION:
*       API definitions for Qci.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdQci_h
#define msdQci_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
* msdStreamFilterFlushAll
*
* DESCRIPTION:
*        This routine flush all stream filter instances.
*
* INPUTS:
*        devNum	   - physical device number.
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
);

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
);

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
);

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
*        blockStatus     - oversize blocking status.
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/******************************************************************************
* msdStreamGateDefStateSet
*
* DESCRIPTION:
*        This routine sets stream gate default gate state.
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
IN  MSD_U8   streamGateId,
IN  MSD_U8	 defGateState,
IN  MSD_U8	 defGateIPVEn,
IN  MSD_U8	 defGateIPV
);

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
);

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
);

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
);

/******************************************************************************
* msdStreamGateTimeGenFlushAll
*
* DESCRIPTION:
*        This routine flush all stream gate time generation instances.
*
* INPUTS:
*        devNum           - physical device number
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
);

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
);

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
);

/******************************************************************************
* msdFlowMeterCouplingFlagSet
*
* DESCRIPTION:
*        This routine sets flow meter coupling flag.
*
* INPUTS:
*        devNum           - physical device number
*        flowMeterId   - flow meter instance identifier
*        flag      	   - coupling flag
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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

#ifdef __cplusplus
}
#endif

#endif /* msdQci_h */