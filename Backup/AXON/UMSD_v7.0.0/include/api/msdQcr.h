/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdQcr.h
*
* DESCRIPTION:
*       API definitions for QCR feature.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#ifndef msdQcr_h
#define msdQcr_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************************************************
* msdQcrFlushOne
*
* DESCRIPTION:
*       This routine flush one ATS Instance.
*
* INPUTS:
*       devNum          - physical device number
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
MSD_STATUS msdQcrFlushOne
(
    IN  MSD_U8   devNum,
    IN  MSD_U16    atsEntryId
);
/*******************************************************************************
* msdQcrFlushAll
*
* DESCRIPTION:
*       This routine flush all ATS Instances.
*
* INPUTS:
*       devNum          - physical device number
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
MSD_STATUS msdQcrFlushAll
(
    IN  MSD_U8   devNum
);
/*******************************************************************************
* msdQcrSetGrpId
*
* DESCRIPTION:
*       This routine set ATS Instance Group Id.
*
* INPUTS:
*       devNum        - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*       atsEntryGrpId - ATS Scheduler Group Id
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
MSD_STATUS msdQcrSetGrpId
(
    IN  MSD_U8   devNum,
    IN  MSD_U16  atsEntryId,
    IN  MSD_U16  atsEntryGrpId
);

/*******************************************************************************
* msdQcrGetGrpId
*
* DESCRIPTION:
*       This routine get ATS Instance Group Id.
*
* INPUTS:
*       devNum          - physical device number
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
MSD_STATUS msdQcrGetGrpId
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    OUT  MSD_U16* const   atsEntryGrpId
);
/*******************************************************************************
* msdQcrSetCbsLimit
*
* DESCRIPTION:
*       This routine set ATS Instance CBS Limit.
*
* INPUTS:
*       devNum          - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*       atsCbsLimit   - ATS CBS Limit
*
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
MSD_STATUS msdQcrSetCbsLimit
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    IN  MSD_U32             atsCbsLimit
);
/*******************************************************************************
* msdQcrGetCbsLimit
*
* DESCRIPTION:
*       This routine get ATS Instance CBS Limit.
*
* INPUTS:
*       devNum          - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*
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
MSD_STATUS msdQcrGetCbsLimit
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U32* const   atsCbsLimit
);
/*******************************************************************************
* msdQcrSetRateLimit
*
* DESCRIPTION:
*       This routine set ATS Instance Rate Limit. The value can be matched to
*    a pair of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*       Due to the limitation of precision, the real rate limit value may not
*    be equal to the target rate limit value, but it must be the closest to
*    the target rate limit value. If you want to get the real rate limit, you
*    can use the function: msdQcrGetRateLimit.
*
* INPUTS:
*       devNum        - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*       rateLimit     - ATS Target Rate Limit
*
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
MSD_STATUS msdQcrSetRateLimit
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    IN  MSD_U32             rateLimit
);
/*******************************************************************************
* msdQcrGetRateLimit
*
* DESCRIPTION:
*       This routine get ATS Instance Rate Limit. The value can be matched to a pair
*       of parameters [Brf, BktDec] by the formula:
*       Rate in bits/sec = (500,000,000 * 2 ^ Brf)/BktDec
*
* INPUTS:
*       devNum        - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*
* OUTPUTS:
*       rateLimit   - ATS Rate Limit
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
MSD_STATUS msdQcrGetRateLimit
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U32* const   rateLimit
);
/*******************************************************************************
* msdQcrSetMaxResidenceTime
*
* DESCRIPTION:
*       This routine set ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       devNum        - physical device number
*       atsEntryId    - ATS Scheduler Instance Id
*       maxTimeScale   - max Time Scale
*       maxResidenceTime - max Residence Time
*
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
MSD_STATUS msdQcrSetMaxResidenceTime
(
    IN  MSD_U8      devNum,
    IN  MSD_U16     atsEntryId,
    IN  MSD_U16     maxTimeScale,
    IN  MSD_U16     maxResidenceTime
);
/*******************************************************************************
* msdQcrGetMaxResidenceTime
*
* DESCRIPTION:
*       This routine get ATS Instance Max Residence Time and max Time Scale.
*           when maxTimeScale = 0, the unit of maxResidenceTime is nanosecond,
*           when maxTimeScale = 1, the unit of maxResidenceTime is microsecond
*
* INPUTS:
*       devNum        - physical device number
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
MSD_STATUS msdQcrGetMaxResidenceTime
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             atsEntryId,
    OUT MSD_U16* const   maxTimeScale,
    OUT MSD_U16* const   maxResidenceTime
);

/*******************************************************************************
* msdQcrSetQueueEnables
*
* DESCRIPTION:
*       This routine enable ATS Transmission Selection Algorithm in Queue.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       devNum        - physical device number
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*       status  - 0: disable; 1: enable.
*
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
MSD_STATUS msdQcrSetQueueEnables
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             port,
    IN  MSD_U16             queue,
    IN  MSD_BOOL            status
);

/*******************************************************************************
* msdQcrGetQueueEnables
*
* DESCRIPTION:
*       This routine check if ATS Transmission Selection Algorithm in Queue is enabled.
*           0 = ATS Transmission Selection Algorithm is not used
*           1 = ATS Transmission Selection Algorithm is used
*
* INPUTS:
*       devNum        - physical device number
*       port    - The egress port
*       queue   - The queue in egress port which ats is enable.
*
* OUTPUTS:
*       status  - True: enabled; False: disabled
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
MSD_STATUS msdQcrGetQueueEnables
(
    IN  MSD_U8              devNum,
    IN  MSD_U16             port,
    IN  MSD_U16             queue,
    OUT MSD_BOOL* status
);



#ifdef __cplusplus
}
#endif

#endif /* msdQcr_h */
