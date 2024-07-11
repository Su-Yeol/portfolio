/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdFRER.h
*
* DESCRIPTION:
*       API definitions for FRER feature.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#ifndef msdFRER_h
#define msdFRER_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif


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
);

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
    IN  MSD_U8   devNum,
	IN  MSD_U8	seqGenId
);

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
    IN  MSD_U8	devNum,
	IN  MSD_U8	seqGenId,
	OUT MSD_FRER_SEQGEN	*seqGenData
	);

/******************************************************************************
* msdFrerSeqGenLoad
*
* DESCRIPTION:
*        This routine load a sequence generation instance.
*
* INPUTS:
*		 devNum          - physical device number
*        seqGenData		 - sequence generation instance data
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
    IN  MSD_U8	devNum,
	IN  MSD_FRER_SEQGEN	*seqGenData
	);

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
IN  MSD_U8	devNum
);

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
IN  MSD_U8	devNum,
IN  MSD_U8	indvRcvyId
);

/******************************************************************************
* msdFrerIndvRcvyRead
*
* DESCRIPTION:
*        This routine gets the individual recovery instance data.
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
IN  MSD_U8	devNum,
IN  MSD_U8	indvRcvyId,
OUT MSD_FRER_INDVRCVY *indvRcvyData,
OUT MSD_U16	*indvRcvyResets
);

/******************************************************************************
* msdFrerIndvRcvyLoad
*
* DESCRIPTION:
*        This routine load an individual recovery instance.
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
IN MSD_U8	devNum,
IN MSD_FRER_INDVRCVY	*indvRcvyData
);

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
);

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
);

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
IN  MSD_U8	devNum
);

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
);

/******************************************************************************
* msdFrerSeqRcvyRead
*
* DESCRIPTION:
*        This routine gets the sequence recovery instance data.
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
IN  MSD_U8	devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

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
IN  MSD_U8              devNum,
IN  MSD_FRER_SEQRCVY	*seqRcvyData
);

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
IN  MSD_U8	devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

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
IN  MSD_U8	devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

/******************************************************************************
* msdFrerSeqRcvyReadClear
*
* DESCRIPTION:
*        This routine gets next sequence recovery instance and clear the instance¡¯s counters.
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
IN  MSD_U8	devNum,
IN  MSD_U8	seqRcvyId,
OUT MSD_FRER_SEQRCVY	*seqRcvyData,
OUT MSD_U16	*seqRcvyResets,
OUT MSD_U16	*latErrResets
);

/******************************************************************************
* msdFrerBank0SCtrFlushAll
*
* DESCRIPTION:
*        This routine is to flush all bank 0 stream counter instances.
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
IN  MSD_U8	devNum
);

/******************************************************************************
* msdFrerBank0SCtrFlushCtr
*
* DESCRIPTION:
*        This routine is to flush all bank0 stream counter instances' counters.
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
);

/******************************************************************************
* msdFrerBank0SCtrRead
*
* DESCRIPTION:
*        This routine gets the bank 0 stream counter instance data.
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
);

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
);

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
IN  MSD_U8	devNum,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

/******************************************************************************
* msdFrerBank0SCtrReadClear
*
* DESCRIPTION:
*        This routine gets the bank 0 stream counter instance data and clear the instance¡¯s counters.
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
IN  MSD_U8              devNum,
IN  MSD_U8	bk0Id,
OUT MSD_FRER_BANK0_CONFIG	*bk0ConfigData,
OUT MSD_FRER_BANK0_COUNTER	*bk0SCtrData
);

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
IN  MSD_U8              devNum
);

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
);

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
IN  MSD_U8	devNum,
IN  MSD_U8	bk1Id,
OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
);

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
IN  MSD_U8	devNum,
IN MSD_FRER_BANK1_CONFIG	*bk1ConfigData
);

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
);

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
    IN  MSD_U8	devNum,
	IN  MSD_U8	bk1Id,
	OUT MSD_FRER_BANK1_CONFIG	*bk1ConfigData,
	OUT MSD_FRER_BANK1_COUNTER	*bk1SCtrData
	);




#ifdef __cplusplus
}
#endif

#endif /* msdQcr_h */
