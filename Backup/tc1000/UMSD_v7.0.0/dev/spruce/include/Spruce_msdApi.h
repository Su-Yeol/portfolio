/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Spruce_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdApi_h
#define Spruce_msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <spruce/include/api/Spruce_msdArp.h>
#include <spruce/include/api/Spruce_msdBrgFdb.h>
#include <spruce/include/api/Spruce_msdEcid.h>
#include <spruce/include/api/Spruce_msdBrgStu.h>
#include <spruce/include/api/Spruce_msdBrgVtu.h>
#include <spruce/include/api/Spruce_msdEEPROM.h>
#include <spruce/include/api/Spruce_msdPhyCtrl.h>
#include <spruce/include/api/Spruce_msdPIRL.h>
#include <spruce/include/api/Spruce_msdPolicy.h>
#include <spruce/include/api/Spruce_msdPortCtrl.h>
#include <spruce/include/api/Spruce_msdPortPrioMap.h>
#include <spruce/include/api/Spruce_msdPortRmon.h>
#include <spruce/include/api/Spruce_msdQueueCtrl.h>
#include <spruce/include/api/Spruce_msdSysCtrl.h>
#include <spruce/include/api/Spruce_msdQav.h>
#include <spruce/include/api/Spruce_msdPTP.h>
#include <spruce/include/api/Spruce_msdTCAM.h>
#include <spruce/include/api/Spruce_msdTCAMRC.h>
#include <spruce/include/api/Spruce_msdTCAMRCComp.h>
#include <spruce/include/api/Spruce_msdRMU.h>
#include <spruce/include/api/Spruce_msdLed.h>
#include <spruce/include/driver/Spruce_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __Spruce_msdApi_h */
