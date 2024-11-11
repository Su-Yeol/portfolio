/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdEcid.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell ECID functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Amethyst_msdEcid_h
#define __Amethyst_msdEcid_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/*
*  typedef: struct AMETHYST_MSD_ECID_ENTRY
*
*  Description: address translation unit Entry
*
*  Fields:
*      group	        - ECID group (0 ~ 3).
*      ecid				- ECID value (0 ~ 0xFFF).
*      entryState       - the entry state.
*      portVec			- port vector.
*      lag              - MSD_TRUE if entry belongs to a LAG.
*      lagId			- LAG ID if lag is MSD_TRUE.
*      macFPri          - MAC frame priority data (0 ~ 7).
*      macQPri          - MAC Queue priority data (0 ~ 7).
*      removeEtag       - This flag needs to be set to a MSD_TRUE only if this ECID entry matches the E-CID
*						assigned to a Cascade Port and the DPV bits of this ECID entry point to that Cascade Port.
*						Then it configures egress to remove the frame¡¯s E-Tag as it is transmitted out the Cascade Port.
*
*  Comment:
*      None.
*/

typedef struct
{
    MSD_U8  	group;
    MSD_U16 	ecid;
    MSD_U8  	entryState;
    MSD_U32  	portVec;
    MSD_BOOL	lag;
    MSD_U8		lagId;
    MSD_U8		macFPri;
    MSD_U8		macQPri;
    MSD_BOOL	removeEtag;
} AMETHYST_MSD_ECID_ENTRY;


/*******************************************************************************
* Amethyst_gBpeEnableSet
*
* DESCRIPTION:
*       This routine is used to configure the device for Port Extender mode or not. After enable this mode, 
*       users can operate ECID table, such as add/delete/move entries, etc.
*
* INPUTS:
*       en    - MSD_TRUE: enable the feature for CPU operation, MSD_FALSE: otherwise.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       calling this API will cause the E-CID portion of the ATU to be flused, so it's best to call
*       this API during dirver initialization.
*
*******************************************************************************/
MSD_STATUS Amethyst_gBpeEnableSet
(
    IN MSD_QD_DEV *dev,
    IN  MSD_BOOL en
);

/*******************************************************************************
* Amethyst_gBpeEnableGet
*
* DESCRIPTION:
*       This routine get the status if the device is configured Port Extender mode or not
*
* INPUTS:
*       None
*
* OUTPUTS:
*       en    - MSD_TRUE: enable the feature for CPU operation, MSD_FALSE: otherwise.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gBpeEnableGet
(
    IN MSD_QD_DEV *dev,
    OUT MSD_BOOL *en
);

/*******************************************************************************
* Amethyst_gecidAddEntry
*
* DESCRIPTION:
*       This routine creates a new entry in the ECID table
*
* INPUTS:
*       ecidEntry - ECID entry to be added to ECID table
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidAddEntry
(
    IN MSD_QD_DEV *dev,
    IN  AMETHYST_MSD_ECID_ENTRY *ecidEntry
);
MSD_STATUS Amethyst_gecidAddEntryIntf
(
    IN MSD_QD_DEV *dev,
    IN  MSD_ECID_ENTRY *ecidEntry
);

/*******************************************************************************
* Amethyst_gecidFlushAll
*
* DESCRIPTION:
*       This routine delete all entries in the ECID table
*
* INPUTS:
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidFlushAll
(
    IN MSD_QD_DEV *dev
);

/*******************************************************************************
* Amethyst_gecidMove
*
* DESCRIPTION:
*       This routine moves entries from a port to another
*
* INPUTS:
*       moveFrom - port where moving frome
*       moveTo   - port where moving to
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidMove
(
    IN MSD_QD_DEV *dev,
    IN  MSD_LPORT moveFrom,
    IN  MSD_LPORT moveTo
);

/*******************************************************************************
* Amethyst_gecidFlushEntry
*
* DESCRIPTION:
*       This routine deletes one entry of the specified group in the ECID table
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidFlushEntry
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid
);

/*******************************************************************************
* Amethyst_gecidGetEcidEntryNext
*
* DESCRIPTION:
*       Gets the next valid ECID entry starting from the specified group and ecid value
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
*
* OUTPUTS:
*       ecidEntry - valid ECID entry
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*       MSD_NO_SUCH - No valid entry exist
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidGetEcidEntryNext
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT AMETHYST_MSD_ECID_ENTRY* ecidEntry
);
MSD_STATUS Amethyst_gecidGetEcidEntryNextIntf
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT MSD_ECID_ENTRY* ecidEntry
);

/*******************************************************************************
* Amethyst_gecidFindEcidEntry
*
* DESCRIPTION:
*       This routine check if the specified ecid of one group exist in ECID table, if exists, return the valid ECID entry
*
* INPUTS:
*       group  - ecid group
*       ecid   - ecid base value
*
* OUTPUTS:
*       ecidEntry - valid ECID entry
*       found     - MSD_TRUE, if the appropriate entry exists
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidFindEcidEntry
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT AMETHYST_MSD_ECID_ENTRY* ecidEntry,
    OUT MSD_BOOL* found
);
MSD_STATUS Amethyst_gecidFindEcidEntryIntf
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8  group,
    IN  MSD_U16 ecid,
    OUT MSD_ECID_ENTRY* ecidEntry,
    OUT MSD_BOOL* found
);

/*******************************************************************************
* Amethyst_gecidGetEntryCount
*
* DESCRIPTION:
*       This routine counts all entries in the ECID table
*
* INPUTS:
*       None
*
* OUTPUTS:
*       count - The number of current valid entries
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidGetEntryCount
(
    IN MSD_QD_DEV *dev,
    OUT MSD_U32 *count
);

/*******************************************************************************
* Amethyst_gecidDump
*
* DESCRIPTION:
*       This routine finds all valid ECID entries and print it out
*
* INPUTS:
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*       MSD_FEATURE_NOT_ENABLE - The Bpe feature not been enable
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Amethyst_gecidDump
(
    IN MSD_QD_DEV *dev
);

#ifdef __cplusplus
}
#endif

#endif /* __msdEcid_h */
