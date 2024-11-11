/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdFRER.c
*
* DESCRIPTION:
*       API definitions for Frame Replication and Elimination for Reliability
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdFRER.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/******************************************************************************
* msdFrerSeqGenFlushAll
*
* DESCRIPTION:
*        This routine flush all sequence generation instances.
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
MSD_STATUS msdFrerSeqGenFlushAll
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

    if (dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll != NULL)
        return dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqGenFlushEntry
*
* DESCRIPTION:
*        This routine flush a sequence generation instance.
*
* INPUTS:
*        devNum			- physical device number
*		 seqGenId		- sequence generation instance identifier
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
MSD_STATUS msdFrerSeqGenFlushEntry
(
    IN  MSD_U8  devNum,
	IN  MSD_U8  seqGenId
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry(dev, seqGenId);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqGenRead
*
* DESCRIPTION:
*        This routine gets sequence generation instance data.
*
* INPUTS:
*		 devNum		- physical device number
*        seqGenId	- sequence generation instance identifier
*
* OUTPUTS:
*        seqGenData  - sequence generation instance data
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
MSD_STATUS msdFrerSeqGenRead
(
IN  MSD_U8   devNum,
IN  MSD_U8	seqGenId,
OUT MSD_FRER_SEQGEN	*seqGenData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqGenRead != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqGenRead(dev, seqGenId, seqGenData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqGenLoad
*
* DESCRIPTION:
*        This routine load a sequence generation instance.
*
* INPUTS:
*		 devNum          - physical device number
*        seqGenData		 - sequence generation instance identifier
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
MSD_STATUS msdFrerSeqGenLoad
(
IN  MSD_U8          devNum,
IN  MSD_FRER_SEQGEN	*seqGenData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad(dev, seqGenData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyFlushAll
*
* DESCRIPTION:
*        This routine flush all individual recovery instances.
*
* INPUTS:
*        devNum		- physical device number
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
MSD_STATUS msdFrerIndvRcvyFlushAll
(
IN  MSD_U8          devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll != NULL)
        return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyFlushEntry
*
* DESCRIPTION:
*        This routine flush an individual recovery instance.
*
* INPUTS:
*		 devNum			- physical device number
*        indvRcvyId		- individual recovery instance identifier
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
MSD_STATUS msdFrerIndvRcvyFlushEntry
(
IN  MSD_U8  devNum,
IN  MSD_U8	indvRcvyId
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry(dev, indvRcvyId);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyRead
*
* DESCRIPTION:
*        This routine gets an individual recovery instance data.
*
* INPUTS:
*		 devNum			- physical device number
*        indvRcvyId		- individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
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
MSD_STATUS msdFrerIndvRcvyRead
(
IN  MSD_U8          devNum,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead(dev, indvRcvyId, indvRcvyData, indvRcvyResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyLoad
*
* DESCRIPTION:
*        This routine load a individual recovery instance.
*
* INPUTS:
*		 devNum			- physical device number
*        indvRcvyData	- individual recovery instance identifier
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
MSD_STATUS msdFrerIndvRcvyLoad
(
IN  MSD_U8              devNum,
IN MSD_FRER_INDVRCVY	*indvRcvyData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyLoad != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyLoad(dev, indvRcvyData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyGetNext
*
* DESCRIPTION:
*        This routine gets next individual recovery instance data.
*
* INPUTS:
*		 devNum			- physical device number
*        indvRcvyId		- individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
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
MSD_STATUS msdFrerIndvRcvyGetNext
(
IN  MSD_U8	devNum,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext(dev, indvRcvyId, indvRcvyData, indvRcvyResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerIndvRcvyReadClear
*
* DESCRIPTION:
*        This routine gets an individual recovery instance and clear the instance¡¯s counters.
*
* INPUTS:
*		 devNum          - physical device number
*        indvRcvyId  - individual recovery instance identifier
*
* OUTPUTS:
*        indvRcvyData    - individual recovery instance data
*		 indvRcvyResets  - individual recovery reset time
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
MSD_STATUS msdFrerIndvRcvyReadClear
(
IN  MSD_U8	devNum,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY	*indvRcvyData,
OUT MSD_U16	*indvRcvyResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear(dev, indvRcvyId, indvRcvyData, indvRcvyResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyFlushAll
*
* DESCRIPTION:
*        This routine flush all sequence recovery instances.
*
* INPUTS:
*        devNum		- physical device number
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
MSD_STATUS msdFrerSeqRcvyFlushAll
(
IN  MSD_U8          devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll != NULL)
        return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyFlushEntry
*
* DESCRIPTION:
*        This routine flush a sequence recovery instance.
*
* INPUTS:
*        devNum		- physical device number
*        seqRcvyId	- sequence recovery instance identifier
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
MSD_STATUS msdFrerSeqRcvyFlushEntry
(
IN  MSD_U8	devNum,
IN  MSD_U8	seqRcvyId
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry(dev, seqRcvyId);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyRead
*
* DESCRIPTION:
*        This routine gets an sequence recovery instance data.
*
* INPUTS:
*		 devNum		- physical device number
*        seqRcvyId	- sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
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
MSD_STATUS msdFrerSeqRcvyRead
(
IN  MSD_U8              devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead(dev, seqRcvyId, seqRcvyData, seqRcvyResets, latErrResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyLoad
*
* DESCRIPTION:
*        This routine load a sequence recovery instance.
*
* INPUTS:
*		 devNum			- physical device number
*        seqRcvyData	- sequence recovery instance identifier
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
MSD_STATUS msdFrerSeqRcvyLoad
(
IN  MSD_U8                  devNum,
IN  MSD_FRER_SEQRCVY	*seqRcvyData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad(dev, seqRcvyData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyGetNext
*
* DESCRIPTION:
*        This routine gets next sequence recovery instance data.
*
* INPUTS:
*		 devNum			- physical device number
*        seqRcvyId		- sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
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
MSD_STATUS msdFrerSeqRcvyGetNext
(
IN  MSD_U8                  devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext(dev, seqRcvyId, seqRcvyData, seqRcvyResets, latErrResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyGetViolation
*
* DESCRIPTION:
*        This routine gets next higher sequence recovery instance whose LatErrStatus bit is set to a one.
*
* INPUTS:
*		 devNum		- physical device number
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
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
MSD_STATUS msdFrerSeqRcvyGetViolation
(
IN  MSD_U8                  devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation(dev, seqRcvyId, seqRcvyData, seqRcvyResets, latErrResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerSeqRcvyReadClear
*
* DESCRIPTION:
*        This routine gets an sequence recovery instance and clear the instance¡¯s counters.
*
* INPUTS:
*		 devNum		- physical device number
*        seqRcvyId  - sequence recovery instance identifier
*
* OUTPUTS:
*        seqRcvyData    - sequence recovery instance data
*		 seqRcvyResets  - sequence recovery reset time
*		 latErrResets   - latent error reset time
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
MSD_STATUS msdFrerSeqRcvyReadClear
(
IN  MSD_U8                  devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear(dev, seqRcvyId, seqRcvyData, seqRcvyResets, latErrResets);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrFlushAll
*
* DESCRIPTION:
*        This routine flush all bank 0 stream counter instances.
*
* INPUTS:
*        devNum		- physical device number
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
MSD_STATUS msdFrerBank0SCtrFlushAll
(
IN  MSD_U8              devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll != NULL)
        return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrFlushCtr
*
* DESCRIPTION:
*        This routine flush an bank 0 stream counter instance.
*
* INPUTS:
*		 devNum	- physical device number
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
MSD_STATUS msdFrerBank0SCtrFlushCtr
(
IN  MSD_U8	devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrRead
*
* DESCRIPTION:
*        This routine gets an bank 0 stream counter instance data.
*
* INPUTS:
*		 devNum	- physical device number
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
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
MSD_STATUS msdFrerBank0SCtrRead
(
IN  MSD_U8	devNum,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead(dev, bk0Id, bk0ConfigData, bk0SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrLoad
*
* DESCRIPTION:
*        This routine load a bank 0 stream counter instance.
*
* INPUTS:
*		 devNum			- physical device number
*        bk0ConfigData	- bank 0 stream config data
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
MSD_STATUS msdFrerBank0SCtrLoad
(
IN  MSD_U8	devNum,
IN MSD_FRER_BANK0_CONFIG	*bk0ConfigData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad(dev, bk0ConfigData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrGetNext
*
* DESCRIPTION:
*        This routine gets next bank 0 stream counter instance data.
*
* INPUTS:
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
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
MSD_STATUS msdFrerBank0SCtrGetNext
(
IN  MSD_U8                  devNum,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext(dev, bk0Id, bk0ConfigData, bk0SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank0SCtrReadClear
*
* DESCRIPTION:
*        This routine gets a bank 0 stream counter instance data and clear the instance¡¯s counters.
*
* INPUTS:
*		 devNum	- physical device number
*        bk0Id  - bank 0 stream counter instance identifier
*
* OUTPUTS:
*        bk0ConfigData    - bank 0 stream counter config data
*		 bk0SCtrData  - bank 0 stream counter counter data
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
MSD_STATUS msdFrerBank0SCtrReadClear
(
IN  MSD_U8	devNum,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear(dev, bk0Id, bk0ConfigData, bk0SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrFlushAll
*
* DESCRIPTION:
*        This routine flush all bank 1 stream counter instances.
*
* INPUTS:
*        devNum		- physical device number
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
MSD_STATUS msdFrerBank1SCtrFlushAll
(
IN  MSD_U8	devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll != NULL)
        return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrFlushCtr
*
* DESCRIPTION:
*        This routine flush an bank 1 stream counter instance.
*
* INPUTS:
*		 devNum	- physical device number
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
MSD_STATUS msdFrerBank1SCtrFlushCtr
(
IN  MSD_U8	devNum
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry(dev);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrRead
*
* DESCRIPTION:
*        This routine gets an bank 1 stream counter instance data.
*
* INPUTS:
*		 devNum	- physical device number
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
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
MSD_STATUS msdFrerBank1SCtrRead
(
IN  MSD_U8              devNum,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead(dev, bk1Id, bk1ConfigData, bk1SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrLoad
*
* DESCRIPTION:
*        This routine load a bank 1 stream counter instance.
*
* INPUTS:
*		 devNum			- physical device number
*        bk1ConfigData	- bank 1 stream config data
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
MSD_STATUS msdFrerBank1SCtrLoad
(
IN  MSD_U8              devNum,
IN MSD_FRER_BANK1_CONFIG	*bk1ConfigData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad(dev, bk1ConfigData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrGetNext
*
* DESCRIPTION:
*        This routine gets next bank 1 stream counter instance data.
*
* INPUTS:
*		 devNum	- physical device number
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
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
MSD_STATUS msdFrerBank1SCtrGetNext
(
IN  MSD_U8	devNum,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext(dev, bk1Id, bk1ConfigData, bk1SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}

/******************************************************************************
* msdFrerBank1SCtrReadClear
*
* DESCRIPTION:
*        This routine gets a bank 1 stream counter instance data and clear the instance¡¯s counters.
*
* INPUTS:
*		 devNum	- physical device number
*        bk1Id  - bank 1 stream counter instance identifier
*
* OUTPUTS:
*        bk1ConfigData    - bank 1 stream counter config data
*		 bk1SCtrData  - bank 1 stream counter counter data
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
MSD_STATUS msdFrerBank1SCtrReadClear
(
IN  MSD_U8              devNum,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
)
{
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        return MSD_FAIL;
    }

    if (dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear != NULL)
		return dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear(dev, bk1Id, bk1ConfigData, bk1SCtrData);
    else
        return MSD_NOT_SUPPORTED;
}