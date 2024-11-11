/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Bonsai_EEPROM.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell EEPROM functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Bonsai_EEPROM_h
#define Bonsai_EEPROM_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Bonsai_EEPROMWrite
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
MSD_STATUS Bonsai_EEPROMWrite
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
MSD_STATUS Bonsai_EEPROMWriteIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
/*******************************************************************************
* Bonsai_EEPROMRead
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
MSD_STATUS Bonsai_EEPROMRead
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);
MSD_STATUS Bonsai_EEPROMReadIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);

/*******************************************************************************
* Bonsai_EEPROMChipSelSet
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
MSD_STATUS Bonsai_EEPROMChipSelSet
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U8  chipSelect
);
MSD_STATUS Bonsai_EEPROMChipSelSetIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U8  chipSelect
);
/*******************************************************************************
* Bonsai_EEPROMChipSelGet
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
MSD_STATUS Bonsai_EEPROMChipSelGet
(
	IN  MSD_QD_DEV *dev,
	OUT  MSD_U8  *chipSelect
);
MSD_STATUS Bonsai_EEPROMChipSelGetIntf
(
	IN  MSD_QD_DEV *dev,
	OUT  MSD_U8  *chipSelect
);
#ifdef __cplusplus
}
#endif

#endif /* __Bonsai_EEPROM_h */