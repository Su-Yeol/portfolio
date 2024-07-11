/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Peridot_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef __Peridot_msdApi_h
#define __Peridot_msdApi_h
#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <peridot/include/api/Peridot_msdBrgFdb.h>
#include <peridot/include/api/Peridot_msdVct.h>
#include <peridot/include/api/Peridot_msdBrgStu.h>
#include <peridot/include/api/Peridot_msdBrgVtu.h>
#include <peridot/include/api/Peridot_msdIMP.h>
#include <peridot/include/api/Peridot_msdPhyCtrl.h>
#include <peridot/include/api/Peridot_msdPIRL.h>
#include <peridot/include/api/Peridot_msdPortCtrl.h>
#include <peridot/include/api/Peridot_msdPortPrioMap.h>
#include <peridot/include/api/Peridot_msdPortRmon.h>
#include <peridot/include/api/Peridot_msdQueueCtrl.h>
#include <peridot/include/api/Peridot_msdSysCtrl.h>
#include <peridot/include/api/Peridot_msdQav.h>
#include <peridot/include/api/Peridot_msdPTP.h>
#include <peridot/include/api/Peridot_msdTCAM.h>
#include <peridot/include/api/Peridot_msdRMU.h>
#include <peridot/include/api/Peridot_msdLed.h>
#include <peridot/include/api/Peridot_msdEEPROM.h>
#include <peridot/include/driver/Peridot_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __Peridot_msdApi_h */
