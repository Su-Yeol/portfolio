/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Oak_msdBrgFdb.h
*
* DESCRIPTION:
*       API/Structure definitions for Multiple Forwarding Databases
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Oak_msdBrgFdb_h
#define Oak_msdBrgFdb_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported ATU Types			 			                                   */
/****************************************************************************/
/*
 * typedef: enum MSD_FLUSH_CMD
 *
 * Description: Enumeration of the address translation unit flush operation.
 *
 * Enumerations:
 *   MSD_FLUSH_ALL       - flush all entries.
 *   MSD_FLUSH_ALL_NONSTATIC - flush all non-static entries.
 */
typedef enum
{
    OAK_MSD_FLUSH_ALL       = 1,
    OAK_MSD_FLUSH_ALL_NONSTATIC = 2,
} OAK_MSD_FLUSH_CMD;

/*
 * typedef: enum MSD_MOVE_CMD
 *
 * Description: Enumeration of the address translation unit move or remove operation.
 *     When destination port is set to 0x1F, Remove operation occurs.
 *
 * Enumerations:
 *   MSD_MOVE_ALL       - move all entries.
 *   MSD_MOVE_ALL_NONSTATIC - move all non-static entries.
 */
typedef enum
{
    OAK_MSD_MOVE_ALL       = 1,
    OAK_MSD_MOVE_ALL_NONSTATIC = 2,
} OAK_MSD_MOVE_CMD;

/*
 *  typedef: struct MSD_ATU_EXT_PRI
 *
 *  Description: 
 *        Extended priority information for the address tarnslaton unit entry.

 *  Fields:
 *      macFPri    - MAC frame priority data (0 ~ 7).
 *      macQPri    - MAC Queue priority data (0 ~ 7).
 *
 */
typedef struct
{
    MSD_U8             macFPri;
    MSD_U8             macQPri;
} OAK_MSD_ATU_EXT_PRI;


/*
 *  typedef: struct MSD_ATU_ENTRY
 *
 *  Description: address translation unit Entry
 *
 *  Fields:
 *      macAddr    - mac address
 *      LAG        - Link Aggregation Mapped Address
 *      portVec    - port Vector. 
 *                     If trunkMember field is MSD_TRUE, this value represents trunk ID.
 *      entryState - the entry state.
 *      DBNum      - ATU MAC Address Database number. If multiple address 
 *                     databases are not being used, DBNum should be zero.
 *                     If multiple address databases are being used, this value
 *                     should be set to the desired address database number.
 *      exPrio     - extended priority information. 
 *
 *  Comment:
 *      The entryState union Type is determine according to the Mac Type.
 */
typedef struct
{
    MSD_ETHERADDR      macAddr;
    MSD_BOOL           LAG;
    MSD_U32            portVec;
    MSD_U8             reserved;
    MSD_U16            DBNum;
    MSD_U8             entryState;
    OAK_MSD_ATU_EXT_PRI    exPrio;
} OAK_MSD_ATU_ENTRY;

/*
 *  typedef: struct MSD_ATU_INT_STATUS
 *
 *  Description: VLAN tarnslaton unit interrupt status
 *
 *  Fields:
 *      intCause  - ATU Interrupt Cause
 *                    MSD_AGE_OUT_VIOLATION,MSD_MEMEBER_VIOLATION,
 *                    MSD_MISS_VIOLATION, MSD_FULL_VIOLATION
 *                    
 *      SPID      - source port number
 *                     if intCause is MSD_FULL_VIOLATION, it means nothing
 *      DBNum     - DB Num (or FID)
 *                     if intCause is MSD_FULL_VIOLATION, it means nothing
 *      macAddr      - MAC Address
 */
typedef struct
{
    MSD_U16   atuIntCause;
    MSD_U8    spid;
    MSD_U16   dbNum;
    MSD_ETHERADDR  macAddr;
} OAK_MSD_ATU_INT_STATUS;

/*
 * Bit Vector Definition for ATU Violation
 */
#define OAK_MSD_ATU_AGE_OUT_VIOLATION		0x8U
#define OAK_MSD_ATU_MEMBER_VIOLATION        0x4U
#define OAK_MSD_ATU_MISS_VIOLATION          0x2U
#define OAK_MSD_ATU_FULL_VIOLATION       	0x1U

/*
 * Maximum LearnLimit Counter
 */
#define OAK_MAX_ATU_PORT_LEARNLIMIT  0x3FF


/****************************************************************************/
/* Exported ATU Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* gfdbAddMacEntry
*
* DESCRIPTION:
*       Creates the new entry in MAC address table.
*
* INPUTS:
*       macEntry    - mac address entry to insert to the ATU.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       DBNum in macEntry -
*            ATU MAC Address Database number. If multiple address 
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbAddMacEntry
(
    IN MSD_QD_DEV    *dev,
    IN OAK_MSD_ATU_ENTRY *macEntry
);

MSD_STATUS Oak_gfdbAddMacEntryIntf
(
    IN MSD_QD_DEV    *dev,
    IN MSD_ATU_ENTRY *macEntry
);
/*******************************************************************************
* Oak_gfdbGetAtuEntryNext
*
* DESCRIPTION:
*       Gets next lexicographic MAC address starting from the specified Mac Addr
*		in a particular ATU database (DBNum or FID).
*
* INPUTS:
*       atuEntry - the Mac Address to start the search.
*
* OUTPUTS:
*       atuEntry - match Address translate unit entry.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       Search starts from atu.macAddr[xx:xx:xx:xx:xx:xx] specified by the
*       user.
*
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetAtuEntryNext
(
	IN MSD_QD_DEV    *dev,
	INOUT OAK_MSD_ATU_ENTRY *atuEntry
);
MSD_STATUS Oak_gfdbGetAtuEntryNextIntf
(
	IN MSD_QD_DEV    *dev,
	INOUT MSD_ATU_ENTRY *atuEntry
);
/*******************************************************************************
* Oak_gfdbFlush
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the MAC Address
*       Table.
*
* INPUTS:
*       flushCmd - the flush operation type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbFlush
(
	IN MSD_QD_DEV    *dev,
	IN OAK_MSD_FLUSH_CMD flushCmd
);

MSD_STATUS Oak_gfdbFlushIntf
(
	IN MSD_QD_DEV    *dev,
	IN MSD_FLUSH_CMD flushCmd
);

/*******************************************************************************
* Oak_gfdbFlushInDB
*
* DESCRIPTION:
*       This routine flush all or all non-static addresses from the particular
*       ATU Database (DBNum). If multiple address databases are being used, this
*       API can be used to flush entries in a particular DBNum database.
*
* INPUTS:
*       flushCmd  - the flush operation type.
*       DBNum     - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL- on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbFlushInDB
(
	IN MSD_QD_DEV    *dev,
	IN OAK_MSD_FLUSH_CMD flushCmd,
	IN MSD_U32 DBNum
);

MSD_STATUS Oak_gfdbFlushInDBIntf
(
	IN MSD_QD_DEV    *dev,
	IN MSD_FLUSH_CMD flushCmd,
	IN MSD_U32 DBNum
);
/*******************************************************************************
* Oak_gfdbMove
*
* DESCRIPTION:
*        This routine moves all or all non-static addresses from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbMove
(
	IN MSD_QD_DEV     *dev,
	IN OAK_MSD_MOVE_CMD   moveCmd,
	IN MSD_U32        moveFrom,
	IN MSD_U32        moveTo
);

MSD_STATUS Oak_gfdbMoveIntf
(
	IN MSD_QD_DEV     *dev,
	IN MSD_MOVE_CMD   moveCmd,
	IN MSD_U32        moveFrom,
	IN MSD_U32        moveTo
);

/*******************************************************************************
* Oak_gfdbPortRemove
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbPortRemoveIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_LPORT portNum
);

/*******************************************************************************
* Oak_gfdbMoveInDB
*
* DESCRIPTION:
*        This routine move all or all non-static addresses which are in the particular
*        ATU Database (DBNum) from a port to another.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        DBNum    - ATU MAC Address Database Number.
*        moveFrom - port where moving from
*        moveTo   - port where moving to
*
* OUTPUTS:
*     None
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*		 none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbMoveInDB
(
	IN MSD_QD_DEV     *dev,
	IN OAK_MSD_MOVE_CMD   moveCmd,
	IN MSD_U32        DBNum,
	IN MSD_LPORT      moveFrom,
	IN MSD_LPORT      moveTo
);
MSD_STATUS Oak_gfdbMoveInDBIntf
(
	IN MSD_QD_DEV     *dev,
	IN MSD_MOVE_CMD   moveCmd,
	IN MSD_U32        DBNum,
	IN MSD_LPORT      moveFrom,
	IN MSD_LPORT      moveTo
);

/*******************************************************************************
* Oak_gfdbPortRemoveInDB
*
* DESCRIPTION:
*        This routine remove all or all non-static addresses from a port in the 
*        specified ATU DBNum.
*
* INPUTS:
*        moveCmd  - the move operation type.
*        portNum  - logical port number
*        DBNum    - fid
*
* OUTPUTS:
*        None
*
* RETURNS:
*        MSD_OK  - on success
*        MSD_FAIL- on error
*        MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbPortRemoveInDBIntf
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_MOVE_CMD  moveCmd,
    IN  MSD_U32  DBNum,
    IN  MSD_LPORT  portNum
);

/*******************************************************************************
* Oak_gfdbDelAtuEntry
*
* DESCRIPTION:
*       Deletes ATU entry.
*
* INPUTS:
*       macEntry - the ATU entry to be deleted.
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
*       DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbDelAtuEntry
(
    IN MSD_QD_DEV     *dev,
    IN MSD_ETHERADDR  *macAddr,
    IN MSD_U32   fid
);

/*******************************************************************************
* Oak_gfdbGetViolation
*
* DESCRIPTION:
*       Get ATU Violation data
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       atuIntStatus - interrupt cause, source portID, dbNum and mac address.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       none
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetViolation
(
    IN  MSD_QD_DEV         *dev,
    OUT OAK_MSD_ATU_INT_STATUS *atuIntStatus
);
MSD_STATUS Oak_gfdbGetViolationIntf
(
    IN  MSD_QD_DEV         *dev,
    OUT MSD_ATU_INT_STATUS *atuIntStatus
);

/*******************************************************************************
* Oak_gfdbFindAtuMacEntry
*
* DESCRIPTION:
*       Find FDB entry for specific MAC address from the ATU.
*
* INPUTS:
*       atuEntry - the Mac address to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       atuEntry - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM  - on bad parameter
*
* COMMENTS:
*        DBNum in atuEntry -
*            ATU MAC Address Database number. If multiple address
*            databases are not being used, DBNum should be zero.
*            If multiple address databases are being used, this value
*            should be set to the desired address database number.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbFindAtuMacEntry
(
    IN MSD_QD_DEV    *dev,
    INOUT OAK_MSD_ATU_ENTRY  *atuEntry,
    OUT MSD_BOOL         *found
);
MSD_STATUS Oak_gfdbFindAtuMacEntryIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_ETHERADDR *macAddr,
    IN  MSD_U32       fid,
    OUT MSD_ATU_ENTRY *atuEntry,
    OUT MSD_BOOL      *found
);

/*******************************************************************************
* Oak_gfdbSetAgingTimeout
*
* DESCRIPTION:
*        Sets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The standard recommends 300 sec.
*        For amethyst, the time-base is 3750 milliseconds, so it can be set to 3750,
*        3750*2, 3750*3,...3750*0xff (almost 16 minites)
*        Since in this API, parameter 'timeout' is milliseconds based,we will set the value
*        rounded to the nearest supported value smaller than the given timeout.
*        If the given timeout is less than 3750, minimum timeout value
*        3750 will be used instead. E.g.) 3800 becomes 3750 and 3000 becomes 3750.
*
* INPUTS:
*       timeout - aging time in miliseconds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbSetAgingTimeout
(
    IN MSD_QD_DEV    *dev,
    IN MSD_U32 timeout
);

/*******************************************************************************
* Oak_gfdbGetAgingTimeout
*
* DESCRIPTION:
*        Gets the timeout period in milliseconds for aging out dynamically learned
*        forwarding information. The returned value may not be the same as the value
*        programmed with <gfdbSetAgingTimeout>. Please refer to the description of
*        <gfdbSetAgingTimeout>.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       timeout - aging time in milliseconds.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetAgingTimeout
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32  *timeout
);

/*******************************************************************************
* Oak_gfdbGetLearn2All
*
* DESCRIPTION:
*        When more than one Marvell device is used to form a single 'switch', it
*        may be desirable for all devices in the 'switch' to learn any address this
*        device learns. When this bit is set to a one all other devices in the
*        'switch' learn the same addresses this device learns. When this bit is
*        cleared to a zero, only the devices that actually receive frames will learn
*        from those frames. This mode typically supports more active MAC addresses
*        at one time as each device in the switch does not need to learn addresses
*        it may nerver use.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode  - MSD_TRUE if Learn2All is enabled, MSD_FALSE otherwise
*
* RETURNS:
*        MSD_OK           - on success
*        MSD_FAIL         - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*        None.
*
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetLearn2All
(
    IN  MSD_QD_DEV  *dev,
    OUT MSD_BOOL  *mode
);

/*******************************************************************************
* Oak_gfdbSetLearn2All
*
* DESCRIPTION:
*        Enable or disable Learn2All mode.
*
* INPUTS:
*        mode - MSD_TRUE to set Learn2All, MSD_FALSE otherwise
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Oak_gfdbSetLearn2All
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_BOOL  mode
);

/*******************************************************************************
* Oak_gfdbSetPortLearnLimit
*
* DESCRIPTION:
*        Set auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port or if
*        the port's PAV has more then 1 bit set to a '1'.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbSetPortLearnLimit
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U32  limit
);

/*******************************************************************************
* Oak_gfdbGetPortLearnLimit
*
* DESCRIPTION:
*        Get auto learning limit for specified port of a specified device.
*
*        When the limit is non-zero value, the number of unicast MAC addresses that
*        can be learned on this port are limited to the specified value. When the
*        learn limit has been reached any frame that ingresses this port with a
*        source MAC address not already in the address database that is associated
*        with this port will be discarded. Normal auto-learning will resume on the
*        port as soon as the number of active unicast MAC addresses associated to this
*        port is less than the learn limit.
*
*        When the limit is non-zero value, normal address learning and frame policy occurs.
*
*        This feature will not work when this port is configured as a LAG port or if
*        the port's PAV has more then 1 bit set to a '1'.
*
*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports*        The following care is needed when enabling this feature:
*            1) dsable learning on the ports
*            2) flush all non-static addresses in the ATU
*            3) define the desired limit for the ports
*            4) re-enable learing on the ports
*
* INPUTS:
*        port - the logical port number
*        limit - auto learning limit ( 0 ~ 255 )
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK   - on success
*        MSD_FAIL - on error
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetPortLearnLimit
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT  portNum,
    OUT MSD_U32  *limit
);

/*******************************************************************************
* Oak_gfdbGetPortLearnCount
*
* DESCRIPTION:
*        Read the current number of active unicast MAC addresses associated with
*        the given port. This counter (LearnCnt) is held at zero if learn limit
*        (gfdbSetPortAtuLearnLimit API) is set to zero.
*
* INPUTS:
*       port  - logical port number
*
* OUTPUTS:
*       count - current auto learning count
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None.
*
* GalTis:
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetPortLearnCount
(
    IN  MSD_QD_DEV   *dev,
    IN  MSD_LPORT  portNum,
    IN  MSD_U32  *count
);

/*******************************************************************************
* Oak_gfdbGetEntryCount
*
* DESCRIPTION:
*       Counts all entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetEntryCount
(
    IN  MSD_QD_DEV  *dev,
    OUT MSD_U32  *count
);

/*******************************************************************************
* Oak_gfdbGetEntryCountPerFid
*
* DESCRIPTION:
*       Counts all entries in the specified fid in Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetEntryCountPerFid
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32  fid,
    OUT MSD_U32  *count
);

/*******************************************************************************
* Oak_gfdbGetNonStaticEntryCount
*
* DESCRIPTION:
*       Counts all non-static entries in the Address Translation Unit.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetNonStaticEntryCount
(
    IN  MSD_QD_DEV  *dev,
    OUT MSD_U32  *count
);

/*******************************************************************************
* Oak_gfdbGetNStaticEntryCountFid
*
* DESCRIPTION:
*       Counts all non-static entries in the specified fid in the Address Translation Unit.
*
* INPUTS:
*       fid - DBNum
*
* OUTPUTS:
*       count - number of valid entries.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gfdbGetNStaticEntryCountFid
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32  fid,
    OUT MSD_U32  *count
);

/*******************************************************************************
* gfdbDump
*
* DESCRIPTION:
*       Find all MAC address in the specified fid and print it out.
*
* INPUTS:
*       fid    - ATU MAC Address Database Number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gfdbDump
(
IN MSD_QD_DEV    *dev,
IN MSD_U32       fid
);
#ifdef __cplusplus
}
#endif

#endif /* __msdBrgFdb_h */
