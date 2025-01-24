/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdQcr_h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell QCR.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Fir_msdQcr_h
#define Fir_msdQcr_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Fir_gatsFlushOne
*
* DESCRIPTION:
*       This routine flush one ATS Instance.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
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
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsFlushOne
(
    IN  MSD_QD_DEV* const dev,
    IN  MSD_U16    atsEntryId
);
/*******************************************************************************
* Fir_gatsFlushAll
*
* DESCRIPTION:
*       This routine flush all ATS Instances.
*
* INPUTS:
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
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsFlushAll
(
    IN  MSD_QD_DEV* const dev
);
/******************************************************************************
* Fir_gatsSetGrpId
*
* DESCRIPTION:
*       This function accesses PTP Command Register and Data Register.
*
* INPUTS:
*       atsEntryId      - The Entry Id.
*       atsEntryGrpId   - The Entry GroupId.
*
* OUTPUTS:
*
* RETURNS:
*       MSD_OK on success,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
******************************************************************************/
MSD_STATUS Fir_gatsSetGrpId
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    IN  MSD_U16             atsEntryGrpId
);
/*******************************************************************************
* Fir_gatsGetGrpId
*
* DESCRIPTION:
*       This routine get ATS Instance Group Id.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       atsEntryGrpId   - ATS Scheduler Group Id
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetGrpId
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    OUT  MSD_U16*   const   atsEntryGrpId
);
/*******************************************************************************
* Fir_gatsSetCbsLimit
*
* DESCRIPTION:
*       This routine set ATS Instance CBS Limit.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*       atsCbsLimit   - ATS CBS Limit
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetCbsLimit
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    IN  MSD_U32             atsCbsLimit
);
/*******************************************************************************
* Fir_gatsGetCbsLimit
*
* DESCRIPTION:
*       This routine get ATS Instance CBS Limit.
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id

* OUTPUTS:
*       atsCbsLimit   - ATS CBS Limit
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetCbsLimit
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U32*    const   atsCbsLimit
);
/*******************************************************************************
* Fir_gatsSetRateLimit
*
* DESCRIPTION:
*       This routine set ATS Instance Rate Limit. The value can be matched to
*    a pair of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*       Due to the limitation of precision, the real rate limit value may not
*    be equal to the target rate limit value, but it must be the closest to
*    the target rate limit value. If you want to get the real rate limit, you
*    can use the function: Fir_gatsGetRateLimit.
*
* INPUTS:
*       atsEntryId      - ATS Scheduler Instance Id
*       atsRateLimit    - ATS Target Rate Limit
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetRateLimit
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    IN  MSD_U32             rateLimit
);
/*******************************************************************************
* Fir_gatsGetRateLimit
*
* DESCRIPTION:
*       This routine get ATS Instance Rate Limit. The value can be matched to
*     a pair of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id

* OUTPUTS:
*       atsRateLimit   - ATS Rate Limit
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetRateLimit
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U32*    const   rateLimit
);
/*******************************************************************************
* Fir_gatsSetMaxResidenceTime
*
* DESCRIPTION:
*       This routine set ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*       maxTimeScale   - max Time Scale
*       maxResidenceTime - max Residence Time
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetMaxResidenceTime
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16     atsEntryId,
    IN  MSD_U16     maxTimeScale,
    IN  MSD_U16     maxResidenceTime
);
/*******************************************************************************
* Fir_gatsGetMaxResidenceTime
*
* DESCRIPTION:
*       This routine get ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       maxTimeScale   - max Time Scale
*       maxResidenceTime - max Residence Time
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetMaxResidenceTime
(
    IN  MSD_QD_DEV* const   dev,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U16*    const   maxTimeScale,
    OUT MSD_U16*    const   maxResidenceTime
);

/*******************************************************************************
* Fir_gatsSetQueueEnables
*
* DESCRIPTION:
*       This routine enable ATS Transmission Selection Algorithm in Queue.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*       status  - 0: disable; 1: enable. 
* OUTPUTS:
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsSetQueueEnables
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             port,
IN  MSD_U16             queue,
IN  MSD_BOOL            status
);

/*******************************************************************************
* Fir_gatsGetQueueEnables
*
* DESCRIPTION:
*       This routine check if ATS Transmission Selection Algorithm in Queue
*	is enabled.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*
* OUTPUTS:
*       status  - True: enabled; False: disabled
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Fir_gatsGetQueueEnables
(
IN  MSD_QD_DEV* const   dev,
IN  MSD_U16             port,
IN  MSD_U16             queue,
OUT MSD_BOOL*           status
);
#ifdef __cplusplus
}
#endif

#endif /* Fir_msdQcr_h */