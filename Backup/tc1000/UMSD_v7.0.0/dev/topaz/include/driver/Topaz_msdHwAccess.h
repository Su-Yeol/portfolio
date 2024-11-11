/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Topaz_msdHwAccess.h
*
* DESCRIPTION:
*       Functions declarations for Hw accessing switch register, smi phy registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __Topaz_gtHwAccessh
#define __Topaz_gtHwAccessh

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <driver/msdHwAccess.h>

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                         */
/****************************************************************************/

/*****************************************************************************
* Topaz_msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Topaz_msdSetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* Topaz_msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       value    - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_msdGetSMIPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT MSD_U16  *data
);
/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Topaz_msdSetSMIC45PhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
);

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address to read.
*
* OUTPUTS:
*       data    - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_msdGetSMIC45PhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
);

/*****************************************************************************
* TOPAZ_msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/

MSD_STATUS TOPAZ_msdSetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN MSD_U16   data
);

/*****************************************************************************
* TOPAZ_msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS TOPAZ_msdGetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT  MSD_U16   *data
);
/*****************************************************************************
* msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_msdSetSMIPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    devAddr,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN MSD_U16   data
);
/*****************************************************************************
* msdGetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_msdGetSMIPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    devAddr,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	OUT  MSD_U16   *data
);

MSD_STATUS Topaz_msdWritePagedPhyReg
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U16   data
);

MSD_STATUS Topaz_msdReadPagedPhyReg
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	OUT MSD_U16   *data
);

MSD_STATUS Topaz_msdSetPagedPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN  MSD_U16   data
);

MSD_STATUS Topaz_msdGetPagedPhyRegField
(
	IN MSD_QD_DEV *dev,
	IN  MSD_U8    portNum,
	IN  MSD_U8    pageNum,
	IN  MSD_U8    regAddr,
	IN  MSD_U8    fieldOffset,
	IN  MSD_U8    fieldLength,
	IN  MSD_U16   *data
);

MSD_STATUS Topaz_msdSetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16  regAddr,
    IN MSD_U16 data
);

MSD_STATUS Topaz_msdGetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16  regAddr,
    OUT MSD_U16 *data
);

MSD_STATUS Topaz_msdPhyReset
(
	IN  MSD_QD_DEV    *dev,
	IN  MSD_U8        portNum,
	IN  MSD_U8		 pageNum,
	IN  MSD_U16       u16Data
);

/*****************************************************************************
* Topaz_msdSetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly write external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
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
MSD_STATUS Topaz_msdSetSMIExtPhyReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8    devAddr,
IN MSD_U8    regAddr,
IN MSD_U16   data
);

/*****************************************************************************
* Topaz_msdGetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly read external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       value    - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Topaz_msdGetSMIExtPhyReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
OUT MSD_U16  *data
);

MSD_STATUS Topaz_msdWritePagedPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
IN  MSD_U16   data
);

MSD_STATUS Topaz_msdReadPagedPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
OUT MSD_U16   *data
);

MSD_STATUS Topaz_msdSetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16  regAddr,
IN MSD_U16 data
);

MSD_STATUS Topaz_msdGetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16  regAddr,
OUT MSD_U16 *data
);

#ifdef __cplusplus
}
#endif
#endif /* __Topaz_gtHwAccessh */
