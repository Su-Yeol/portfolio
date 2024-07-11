/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdIMP.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell IMP functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Amethyst_msdIMP_h
#define __Amethyst_msdIMP_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Amethyst_impRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*        addr - specified start address
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impRun
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr
);
MSD_STATUS Amethyst_impRunIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U32	addr
);
/*******************************************************************************
* Amethyst_impReset
*
* DESCRIPTION:
*       This routine is to reset Amethyst_imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impReset
(
	IN	MSD_QD_DEV	*dev
);
MSD_STATUS Amethyst_impResetIntf
(
IN	MSD_QD_DEV	*dev
);

/*******************************************************************************
* Amethyst_impLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to memory
*
*
* INPUTS:
*        addr - start address
*        datasize - data size
*        data - data to be written
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impLoadToRAM
(
	IN	MSD_QD_DEV	*dev,
	IN	MSD_U32	addr,
	IN	MSD_U32	dataSize,
	IN	MSD_U8*	data
);
MSD_STATUS Amethyst_impLoadToRAMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U32	addr,
IN	MSD_U32	dataSize,
IN	MSD_U8*	data
);
/*******************************************************************************
* impStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_impStop
(
IN	MSD_QD_DEV	*dev
);
MSD_STATUS Amethyst_impStopIntf
(
IN	MSD_QD_DEV	*dev
);
#ifdef __cplusplus
}
#endif

#endif /* __Amethyst_msdIMP_h */