/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Amethyst_msdIMPIntf.c
*
* DESCRIPTION:
*       API definitions for Marvell IMP related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <amethyst/include/api/Amethyst_msdIMP.h>

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
MSD_STATUS Amethyst_impRunIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U32	addr
)
{
	return Amethyst_impRun(dev, addr);
}
/*******************************************************************************
* Amethyst_impReset
*
* DESCRIPTION:
*       This routine is to reset imp
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
MSD_STATUS Amethyst_impResetIntf
(
IN	MSD_QD_DEV	*dev
)
{
	return Amethyst_impReset(dev);
}

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
MSD_STATUS Amethyst_impLoadToRAMIntf
(
IN	MSD_QD_DEV	*dev,
IN	MSD_U32	addr,
IN	MSD_U32	dataSize,
IN	MSD_U8*	data
)
{
	return Amethyst_impLoadToRAM(dev, addr, dataSize, data);
}
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
MSD_STATUS Amethyst_impStopIntf
(
IN	MSD_QD_DEV	*dev
)
{
	return Amethyst_impStop(dev);
}