/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Topaz_msdBrgFdb.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell STU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Topaz_msdBrgStu_h
#define __Topaz_msdBrgStu_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported STU Types			 			                                   */
/****************************************************************************/
/*
 * typedef: enum TOPAZ_MSD_PORT_STP_STATE
 *
 * Description: Enumeration of the port spanning tree state.
 *
 * Enumerations:
 *   TOPAZ_MSD_PORT_DISABLE    - port is disabled.
 *   TOPAZ_MSD_PORT_BLOCKING   - port is in blocking/listening state.
 *   TOPAZ_MSD_PORT_LEARNING   - port is in learning state.
 *   TOPAZ_MSD_PORT_FORWARDING - port is in forwarding state.
 */
#ifndef TOPAZ_STP_STATE
typedef enum
{
    TOPAZ_MSD_PORT_DISABLE = 0,
    TOPAZ_MSD_PORT_BLOCKING,
    TOPAZ_MSD_PORT_LEARNING,
    TOPAZ_MSD_PORT_FORWARDING
} TOPAZ_MSD_PORT_STP_STATE;
#define TOPAZ_STP_STATE
#endif

/*
 *  typedef: struct TOPAZ_MSD_STU_ENTRY
 *
 *  Description: 802.1s Port State Information Database (STU) Entry
 *
 *  Fields:
 *      sid       - STU ID 
 *      portState - Per VLAN Port States for each port.
 */
typedef struct
{
    MSD_U16                sid;
    TOPAZ_MSD_PORT_STP_STATE    portState[MSD_MAX_SWITCH_PORTS];
} TOPAZ_MSD_STU_ENTRY;

/****************************************************************************/
/* Exported STU Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Topaz_gstuGetEntryNext
*
* DESCRIPTION:
*       Gets next valid STU entry from the specified SID.
*
* INPUTS:
*       stuEntry - the SID to start the search.
*
* OUTPUTS:
*       stuEntry - next STU entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gstuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT TOPAZ_MSD_STU_ENTRY  *stuEntry
);
MSD_STATUS Topaz_gstuGetEntryNextIntf
(
    IN  MSD_QD_DEV       *dev,
    INOUT MSD_STU_ENTRY  *stuEntry
);
/*******************************************************************************
* Topaz_gstuAddEntry
*
* DESCRIPTION:
*       Creates or update the entry in STU table based on SID.
*
* INPUTS:
*       stuEntry    - stu entry to insert to the STU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_gstuAddEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  TOPAZ_MSD_STU_ENTRY    *stuEntry
);
MSD_STATUS Topaz_gstuAddEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_STU_ENTRY    *stuEntry
);
/*******************************************************************************
* Topaz_gstuDelEntry
*
* DESCRIPTION:
*       Deletes STU entry specified by SID.
*
* INPUTS:
*       stuEntry - the STU entry to be deleted 
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_gstuDelEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  TOPAZ_MSD_STU_ENTRY    *stuEntry
);
MSD_STATUS Topaz_gstuDelEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_U16 sid
);

/*******************************************************************************
* Topaz_gstuFindSidEntry
*
* DESCRIPTION:
*       Find STU entry for a specific SID, it will return the entry, if found,
*       along with its associated data
*
* INPUTS:
*       stuEntry - contains the SID to searche for
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       stuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no such entry.
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        Valid SID is 0 ~ 63.
*
*******************************************************************************/
MSD_STATUS Topaz_gstuFindSidEntry
(
    IN  MSD_QD_DEV       *dev,
    INOUT TOPAZ_MSD_STU_ENTRY  *stuEntry,
    OUT MSD_BOOL         *found
);
MSD_STATUS Topaz_gstuFindSidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16    sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
);

/*******************************************************************************
* Topaz_gstuGetEntryCountIntf
*
* DESCRIPTION:
*       Get number of valid entries in the STU table
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       count  - number of valid entries
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS Topaz_gstuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16 *count
);
MSD_STATUS Topaz_gstuDump
(
IN MSD_QD_DEV    *dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Topaz_msdBrgStu_h */
