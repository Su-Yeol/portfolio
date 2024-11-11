/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/******************************************************************************
* Bonsai_msdQci.h
*
* DESCRIPTION:
*       API definitions for Qci
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef Bonsai_msdQci_h
#define Bonsai_msdQci_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	MSD_U32    tsnPort;
	MSD_U32    tsnBlock;
	MSD_U32    tsnAddr;
	MSD_U32    tsnData;
} BONSAI_MSD_TSN_OP_DATA;


typedef struct
{
	MSD_U8  gateState;
	MSD_U8  gateIPVEn;
	MSD_U8  gateIPV;
	MSD_U16 gateWindowTime;
	MSD_U16 gateAllowedBytes;
} BONSAI_MSD_STREAM_GATE_ENTRY;


/******************************************************************************
* Bonsai_gqciStreamFilterFlushAll
*
* DESCRIPTION:
*        This routine flush all stream filter instances.
*
* INPUTS:
*        None.
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
MSD_STATUS Bonsai_gqciStreamFiltFlushAll
(
IN  MSD_QD_DEV     *dev
);


/******************************************************************************
* Bonsai_gqciStreamFilterSet
*
* DESCRIPTION:
*        This routine enable stream filter instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        maxSDUSize      - maximum frame size
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
MSD_STATUS Bonsai_gqciStreamFiltSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U16     maxSDUSize,
IN  MSD_U8		blockEn
);


/******************************************************************************
* Bonsai_gqciStreamFilterBlockStatusSet
*
* DESCRIPTION:
*        This routine enable oversize blocking mechanism active or deactive.
*
* INPUTS:
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
MSD_STATUS Bonsai_gqciStreamFiltBlockStatSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
IN  MSD_U8	blockStatus
);

/******************************************************************************
* Bonsai_gqciStreamFilterBlockStatusGet
*
* DESCRIPTION:
*        This routine gets oversize blocking mechanism active or deactive status.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
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
MSD_STATUS Bonsai_gqciStreamFiltBlockStatGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U8	*blockStatus
);


/******************************************************************************
* Bonsai_gqciStreamFiltEnAts
*
* DESCRIPTION:
*        This routine enable ATS(Qcr) scheduler instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        atsId		     - ATS scheduler instance identifier
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
MSD_STATUS Bonsai_gqciStreamFiltEnAts
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U8      atsId
);


/******************************************************************************
* Bonsai_gqciStreamFiltEnGate
*
* DESCRIPTION:
*        This routine enable stream gate instance.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*        gateId		     - stream gate instance identifier
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
MSD_STATUS Bonsai_gqciStreamFiltEnGate
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8		streamFilterId,
IN  MSD_U8      gateId
);


/******************************************************************************
* Bonsai_gqciStreamFilterMatchCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter matching frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream filter matching frames counter.
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
MSD_STATUS Bonsai_gqciStreamFiltMatchCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
);


/******************************************************************************
* Bonsai_gqciStreamFiltFilterCounterGet
*
* DESCRIPTION:
*        This routine gets stream filter filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream filter filtered frames counter.
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
MSD_STATUS Bonsai_gqciStreamFiltFilterCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
);


/******************************************************************************
* Bonsai_gqciStreamGateFiltCtrGet
*
* DESCRIPTION:
*        This routine gets stream gate filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - stream gate filtered frames counter.
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
MSD_STATUS Bonsai_gqciStreamGateFiltCtrGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamFilterId,
OUT MSD_U16	*counter
);

/******************************************************************************
* Bonsai_gqciFlowMeterFiltCtrGet
*
* DESCRIPTION:
*        This routine gets flow meter filtered frames counter.
*
* INPUTS:
*        streamFilterId  - stream filter instance identifier
*
* OUTPUTS:
*        counter		 - flow meter filtered frames counter.
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
MSD_STATUS Bonsai_gqciFlowMeterFiltCtrGet
(
	IN  MSD_QD_DEV* dev,
	IN  MSD_U8  streamFilterId,
	OUT MSD_U16* counter
);

/******************************************************************************
* Bonsai_gqciStreamGateFlushAll
*
* DESCRIPTION:
*        This routine flush all stream gate instances.
*
* INPUTS:
*        None.
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
MSD_STATUS Bonsai_gqciStreamGateFlushAll
(
IN  MSD_QD_DEV     *dev
);

/******************************************************************************
* Bonsai_gqciStreamFilterBlockStatusSet
*
* DESCRIPTION:
*        This routine enable default gate state and default gate IPV.
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
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
MSD_STATUS Bonsai_gqciStreamGateDefStateSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	defGateState,
IN  MSD_U8	defGateIPVEn,
IN  MSD_U8	defGateIPV
);

/******************************************************************************
* Bonsai_gqciStreamGateEntryScaleSet
*
* DESCRIPTION:
*        This routine set stream gate time scale and byte scale
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        timeScale       - time scale
*        byteScale       - byte scale
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
MSD_STATUS Bonsai_gqciStreamGateScaleSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	timeScale,
IN  MSD_U8	byteScale
);

/******************************************************************************
* Bonsai_gqciStreamGateTimeGenIdSet
*
* DESCRIPTION:
*        This routine set stream gate time generation identifier
*
* INPUTS:
*        streamGateId			 - stream gate instance identifier
*        streamGateTimeGenId	 - stream gate time generation instance identifier
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
MSD_STATUS Bonsai_gqciStreamGateTimeGenIdSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8	streamGateTimeGenId
);

/******************************************************************************
* Bonsai_gqciStreamGateEntryAdd
*
* DESCRIPTION:
*        This routine load stream gate entry
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        pointer         - Pointer to the desired Gate Control Entry
*        streamGateEntry - stream gate entry
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
MSD_STATUS Bonsai_gqciStreamGateEntryAdd
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8  pointer,
IN  BONSAI_MSD_STREAM_GATE_ENTRY *streamGateEntry
);

/******************************************************************************
* Bonsai_gqciStreamGateEntryAdd
*
* DESCRIPTION:
*        This routine load stream gate entry
*
* INPUTS:
*        streamGateId	 - stream gate instance identifier
*        pointer         - pointer to the desired Gate Control Entry
*        streamGateEntry - stream gate entry
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
MSD_STATUS Bonsai_gqciStreamGateEntryAddIntf
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  streamGateId,
IN  MSD_U8  pointer,
IN  MSD_STREAM_GATE_ENTRY *streamGateEntry
);

/******************************************************************************
* Bonsai_gqciStreamGateTimeGenFlush
*
* DESCRIPTION:
*        This routine flush stream gate time generation instances.
*
* INPUTS:
*        None.
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
MSD_STATUS Bonsai_gqciStreamGateTimeGenFlush
(
IN  MSD_QD_DEV     *dev
);

/******************************************************************************
* Bonsai_gqciStreamGateTimeGenSet
*
* DESCRIPTION:
*        This routine sets stream gate time generation cycle time and base time.
*
* INPUTS:
*        streamGateId	 - stream gate time generation instance identifier
*        gateBaseTime    - gate cycle time, unit ns
*        gateCycleTime   -
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
MSD_STATUS Bonsai_gqciStreamGateTimeGenSet
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U8  streamGateGenId,
IN  MSD_U32 gateCycleTime,
IN  MSD_U64 gateBaseTime
);

/******************************************************************************
* Bonsai_gqciFlowMeterFlushAll
*
* DESCRIPTION:
*        This routine flush all flow meter instances.
*
* INPUTS:
*        None.
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
MSD_STATUS Bonsai_gqciFlowMeterFlushAll
(
IN  MSD_QD_DEV     *dev
);

/******************************************************************************
* Bonsai_gqciFlowMeterCoupleFlagSet
*
* DESCRIPTION:
*        This routine set flow meter coupling flag
*
* INPUTS:
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
MSD_STATUS Bonsai_gqciFlowMeterCoupleFlagSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	flag
);

/******************************************************************************
* Bonsai_gqciFlowMeterCoupleFlagGet
*
* DESCRIPTION:
*        This routine get flow meter coupling flag
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
*
* OUTPUTS:
*        flag      	   - coupling flag
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
MSD_STATUS Bonsai_gqciFlowMeterCoupleFlagGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
OUT MSD_U8	*flag
);

/******************************************************************************
* Bonsai_gqciFlowMeterDropOnYelSet
*
* DESCRIPTION:
*        This routine set flow meter drop on yellow
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        enable    	   - enable drop on yellow
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
MSD_STATUS Bonsai_gqciFlowMeterDropOnYelSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	enable
);


/******************************************************************************
* Bonsai_gqciFlowMeterDropOnYelGet
*
* DESCRIPTION:
*        This routine get flow meter drop on yellow
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
*
* OUTPUTS:
*        enable    	   - enable drop on yellow.
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
MSD_STATUS Bonsai_gqciFlowMeterDropOnYelGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	*enable
);

/******************************************************************************
* Bonsai_gqciFlowMeterRedBlockEnSet
*
* DESCRIPTION:
*        This routine enable flow meter red block
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        enable    	   - enable red block
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
MSD_STATUS Bonsai_gqciFlowMeterRedBlockEnSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	enable
);


/******************************************************************************
* Bonsai_gqciFlowMeterRedBlockEnGet
*
* DESCRIPTION:
*        This routine get flow meter red block enable bit
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
*
* OUTPUTS:
*        enable    	   - enable red block.
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
MSD_STATUS Bonsai_gqciFlowMeterRedBlockEnGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	*enable
);

/******************************************************************************
* Bonsai_gqciFlowMeterRedBlkStatSet
*
* DESCRIPTION:
*        This routine sets flow meter red block status
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        status    	   - red block status
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
MSD_STATUS Bonsai_gqciFlowMeterRedBlkStatSet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U8	status
);

/******************************************************************************
* Bonsai_gqciFlowMeterRedBlkStatGet
*
* DESCRIPTION:
*        This routine gets flow meter red block status
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*
*
* OUTPUTS:
*        status    	   - red block status
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
MSD_STATUS Bonsai_gqciFlowMeterRedBlkStatGet
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
OUT MSD_U8	*status
);

/******************************************************************************
* Bonsai_gqciFlowMeterAdvConfig
*
* DESCRIPTION:
*        This routine calculates flow meter bktDec, BTRF, BBRF value based on
*        target rate and target burst size
*
* INPUTS:
*        flowMeterId   - flow meter instance identifier
*        tgtRate       - target rate, units is bps
*        bstSize       - target burst size (units is byte)
*        countMode     - count mode
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
MSD_STATUS Bonsai_gqciFlowMeterAdvConfig
(
IN  MSD_QD_DEV	*dev,
IN  MSD_U8  flowMeterId,
IN  MSD_U32 tgtRate,
IN  MSD_U32 bstSize,
IN  MSD_U8  countMode
);


#ifdef __cplusplus
}
#endif

#endif /* __Bonsai_msdQci_h */