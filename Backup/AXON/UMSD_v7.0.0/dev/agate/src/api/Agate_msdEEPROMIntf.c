/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Agate_msdEEPROMIntf.c
*
* DESCRIPTION:
*       API definitions for Marvell EEPROM related operation
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <agate/include/api/Agate_msdEEPROM.h>

/*******************************************************************************
* Agate_EEPROMWrite
*
* DESCRIPTION:
*       This routine is used to write one word EEPROM data
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
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_EEPROMWriteIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
IN  MSD_U16  data
)
{
	return Agate_EEPROMWrite(dev, addr, data);
}
/*******************************************************************************
* Agate_EEPROMRead
*
* DESCRIPTION:
*       This routine is used to read one word EEPROM data
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
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Agate_EEPROMReadIntf
(
IN  MSD_QD_DEV *dev,
IN  MSD_U16  addr,
OUT  MSD_U16  *data
)
{
	return Agate_EEPROMRead(dev, addr, data);
}