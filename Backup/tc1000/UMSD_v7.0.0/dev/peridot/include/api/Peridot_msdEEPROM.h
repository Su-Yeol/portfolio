/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Peridot_EEPROM.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell EEPROM functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Peridot_EEPROM_h
#define __Peridot_EEPROM_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* Peridot_EEPROMWrite
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
MSD_STATUS Peridot_EEPROMWrite
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
MSD_STATUS Peridot_EEPROMWriteIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	IN  MSD_U16  data
);
/*******************************************************************************
* Peridot_EEPROMRead
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
MSD_STATUS Peridot_EEPROMRead
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);
MSD_STATUS Peridot_EEPROMReadIntf
(
	IN  MSD_QD_DEV *dev,
	IN  MSD_U16  addr,
	OUT MSD_U16  *data
);

#ifdef __cplusplus
}
#endif

#endif /* __Peridot_EEPROM_h */