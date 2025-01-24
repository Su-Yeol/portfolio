/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Oak_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Oak_msdApi_h
#define Oak_msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <oak/include/api/Oak_msdArp.h>
#include <oak/include/api/Oak_msdBrgFdb.h>
#include <oak/include/api/Oak_msdEcid.h>
#include <oak/include/api/Oak_msdBrgStu.h>
#include <oak/include/api/Oak_msdBrgVtu.h>
#include <oak/include/api/Oak_msdEEPROM.h>
#include <oak/include/api/Oak_msdPhyCtrl.h>
#include <oak/include/api/Oak_msdPIRL.h>
#include <oak/include/api/Oak_msdPolicy.h>
#include <oak/include/api/Oak_msdPortCtrl.h>
#include <oak/include/api/Oak_msdPortPrioMap.h>
#include <oak/include/api/Oak_msdPortRmon.h>
#include <oak/include/api/Oak_msdQueueCtrl.h>
#include <oak/include/api/Oak_msdSysCtrl.h>
#include <oak/include/api/Oak_msdQav.h>
#include <oak/include/api/Oak_msdPTP.h>
#include <oak/include/api/Oak_msdTCAM.h>
#include <oak/include/api/Oak_msdTCAMRC.h>
#include <oak/include/api/Oak_msdTCAMRCComp.h>
#include <oak/include/api/Oak_msdRMU.h>
#include <oak/include/api/Oak_msdLed.h>
#include <oak/include/driver/Oak_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __Oak_msdApi_h */
