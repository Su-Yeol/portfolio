/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_EEPROM.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell EEPROM functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Amethyst_EEPROM_h
#define __Amethyst_EEPROM_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Amethyst_EEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write one byte EEPROM data
*
*
* INPUTS:
*      addr - EEPROM address
*      data - data to be written
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
MSD_STATUS Amethyst_EEPROMWrite
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
MSD_STATUS Amethyst_EEPROMWriteIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
/*******************************************************************************
* Amethyst_EEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one byte EEPROM data
*
*
* INPUTS:
*      addr - EEPROM address
*
* OUTPUTS:
*      data - EEPROM Data
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_EEPROMRead
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);
MSD_STATUS Amethyst_EEPROMReadIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);

/*******************************************************************************
* Amethyst_EEPROMChipSelSet
*
* DESCRIPTION:
*       This routine is used to select one external EERPOM chip to operation
*
*
* INPUTS:
*      chipSelect - selected EEPROM chip number 
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
MSD_STATUS Amethyst_EEPROMChipSelSet
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U8  chipSelect
);
MSD_STATUS Amethyst_EEPROMChipSelSetIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U8  chipSelect
);
/*******************************************************************************
* Amethyst_EEPROMChipSelGet
*
* DESCRIPTION:
*       This routine is used to get which external EERPOM chip is actively been operated on
*
*
* INPUTS:
*      None
*
* OUTPUTS:
*      chipSelect - selected EEPROM chip number 
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Amethyst_EEPROMChipSelGet
(
	IN  MSD_QD_DEV *dev,
	OUT  MSD_U8  *chipSelect
);
MSD_STATUS Amethyst_EEPROMChipSelGetIntf
(
	IN  MSD_QD_DEV *dev,
	OUT  MSD_U8  *chipSelect
);
#ifdef __cplusplus
}
#endif

#endif /* __Amethyst_EEPROM_h */