/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdAdvVct.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell VCT functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdAdvVct_h
#define msdAdvVct_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdVctGetAdvCableDiag
*
* DESCRIPTION:
*       This routine perform the advanced virtual cable test for the requested
*       port and returns the the status per MDI pair.
*
* INPUTS:
*       portNum - logical port number.
*       mode - advance VCT mode (either First Peak or Maximum Peak)
*
* OUTPUTS:
*       cableStatus - the port copper cable status.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdVctGetAdvCableDiag
(
	IN  MSD_U8			devNum,
	IN  MSD_LPORT        portNum,
	IN  MSD_ADV_VCT_MODE mode,
	OUT MSD_ADV_CABLE_STATUS *cableStatus
);
/*******************************************************************************
*  msdVctGetAdvExtendStatus
*
* DESCRIPTION:
*        This routine retrieves extended cable status, such as Pair Poloarity,
*        Pair Swap, and Pair Skew. Note that this routine will be success only
*        if 1000Base-T Link is up.
*        DSP based cable length is also provided.
*
* INPUTS:
*       dev  - pointer to GT driver structure returned from mdLoadDriver
*       portNum - logical port number.
*
* OUTPUTS:
*       extendedStatus - the extended cable status.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS  msdVctGetAdvExtendStatus
(
    IN  MSD_U8	 devNum,
	IN  MSD_LPORT portNum,
    OUT MSD_ADV_EXTENDED_STATUS *extendedStatus
);
#ifdef __cplusplus
}
#endif

#endif /* __msdAdvVct_h */
