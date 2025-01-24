/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdIMP.h
*
* DESCRIPTION:
*       API/Structure definitions for IMP.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdIMP_h
#define msdIMP_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
*   msdImpRun
*
* DESCRIPTION:
*       This routine is to run IMP with specified start address
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - specified start address
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS   msdImpRun
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  addr
);

/*******************************************************************************
*   msdImpReset
*
* DESCRIPTION:
*       This routine is to reset imp
*
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS   msdImpReset
(
    IN  MSD_U8  devNum
);

/*******************************************************************************
*   msdImpWriteEEPROM
*
* DESCRIPTION:
*       This routine is to write data to EEPROM
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - EEPROM address
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS   msdImpWriteEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  addr,
    IN  MSD_U8  data
);

/*******************************************************************************
*   msdImpLoadToEEPROM
*
* DESCRIPTION:
*       This routine is to load data to EEPROM devices
*
*
* INPUTS:
*       devNum  - physical device number
*       datasize - data size
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS   msdImpLoadToEEPROM
(
    IN  MSD_U8  devNum,
    IN  MSD_U16  dataSize,
    IN  MSD_U8  *data
);

/*******************************************************************************
*   msdImpLoadToRAM
*
* DESCRIPTION:
*       This routine is to load data to RAM.
*
*
* INPUTS:
*       devNum  - physical device number
*       addr - start address
*       datasize - data size
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS   msdImpLoadToRAM
(
    IN  MSD_U8  devNum,
    IN  MSD_U32  addr,
    IN  MSD_U32  dataSize,
    IN  MSD_U8  *data
);

/*******************************************************************************
* msdImpStop
*
* DESCRIPTION:
*       This routine is to stop imp
*
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdImpStop
(
    IN  MSD_U8  devNum
);

#ifdef __cplusplus
}
#endif

#endif /* __msdIMP_h */
