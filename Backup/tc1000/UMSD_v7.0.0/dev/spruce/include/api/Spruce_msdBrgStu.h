/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdBrgStu.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell STU functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdBrgStu_h
#define Spruce_msdBrgStu_h

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
 * typedef: enum SPRUCE_PORT_STP_STATE
 *
 * Description: Enumeration of the port spanning tree state.
 *
 * Enumerations:
 *   SPRUCE_MSD_PORT_DISABLE    - port is disabled.
 *   SPRUCE_MSD_PORT_BLOCKING   - port is in blocking/listening state.
 *   SPRUCE_MSD_PORT_LEARNING   - port is in learning state.
 *   SPRUCE_MSD_PORT_FORWARDING - port is in forwarding state.
 */
#ifndef SPRUCE_STP_STATE
typedef enum
{
    SPRUCE_MSD_PORT_DISABLE = 0,
    SPRUCE_MSD_PORT_BLOCKING,
    SPRUCE_MSD_PORT_LEARNING,
    SPRUCE_MSD_PORT_FORWARDING
} SPRUCE_MSD_PORT_STP_STATE;

#define SPRUCE_STP_STATE

#endif
/*
 *  typedef: struct SPRUCE_MSD_STU_ENTRY
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
    SPRUCE_MSD_PORT_STP_STATE    portState[MSD_MAX_SWITCH_PORTS];
} SPRUCE_MSD_STU_ENTRY;

/****************************************************************************/
/* Exported STU Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Spruce_gstuGetEntryNext
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
MSD_STATUS Spruce_gstuGetEntryNext
(
    IN  MSD_QD_DEV       *dev,
    INOUT SPRUCE_MSD_STU_ENTRY  *stuEntry
);
MSD_STATUS Spruce_gstuGetEntryNextIntf
(
IN MSD_QD_DEV    *dev,
INOUT MSD_STU_ENTRY *stuEntry
);

/*******************************************************************************
* Spruce_gstuAddEntry
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
MSD_STATUS Spruce_gstuAddEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  SPRUCE_MSD_STU_ENTRY    *stuEntry
);
MSD_STATUS Spruce_gstuAddEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_STU_ENTRY    *stuEntry
);
/*******************************************************************************
* Spruce_gstuDelEntry
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
MSD_STATUS Spruce_gstuDelEntry
(
    IN  MSD_QD_DEV       *dev,
    IN  SPRUCE_MSD_STU_ENTRY    *stuEntry
);
MSD_STATUS Spruce_gstuDelEntryIntf
(
    IN  MSD_QD_DEV       *dev,
    IN  MSD_U16 sid
);
/*******************************************************************************
* Spruce_gstuFindSidEntry
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
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        Valid SID is 0 ~ 63.
*
*******************************************************************************/
MSD_STATUS Spruce_gstuFindSidEntry
(
    IN  MSD_QD_DEV       *dev,
    INOUT SPRUCE_MSD_STU_ENTRY  *stuEntry,
    OUT MSD_BOOL         *found
);
MSD_STATUS Spruce_gstuFindSidEntryIntf
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U16 sid,
    OUT MSD_STU_ENTRY *stuEntry,
    OUT MSD_BOOL *found
);

/*******************************************************************************
* Spruce_gstuGetEntryCountIntf
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
MSD_STATUS Spruce_gstuGetEntryCountIntf
(
    IN  MSD_QD_DEV *dev,
    OUT MSD_U16 *count
);

MSD_STATUS Spruce_gstuDump
(
IN MSD_QD_DEV    *dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdBrgStu_h */
