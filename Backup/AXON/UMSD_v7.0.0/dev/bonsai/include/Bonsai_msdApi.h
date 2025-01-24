/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Bonsai_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Bonsai_msdApi_h
#define Bonsai_msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <bonsai/include/api/Bonsai_msdArp.h>
#include <bonsai/include/api/Bonsai_msdBrgFdb.h>
#include <bonsai/include/api/Bonsai_msdEcid.h>
#include <bonsai/include/api/Bonsai_msdBrgStu.h>
#include <bonsai/include/api/Bonsai_msdBrgVtu.h>
#include <bonsai/include/api/Bonsai_msdEEPROM.h>
#include <bonsai/include/api/Bonsai_msdPhyCtrl.h>
#include <bonsai/include/api/Bonsai_msdPIRL.h>
#include <bonsai/include/api/Bonsai_msdPolicy.h>
#include <bonsai/include/api/Bonsai_msdPortCtrl.h>
#include <bonsai/include/api/Bonsai_msdPortPrioMap.h>
#include <bonsai/include/api/Bonsai_msdPortRmon.h>
#include <bonsai/include/api/Bonsai_msdQueueCtrl.h>
#include <bonsai/include/api/Bonsai_msdSysCtrl.h>
#include <bonsai/include/api/Bonsai_msdQav.h>
#include <bonsai/include/api/Bonsai_msdQci.h>
#include <bonsai/include/api/Bonsai_msdPTP.h>
#include <bonsai/include/api/Bonsai_msdTCAM.h>
#include <bonsai/include/api/Bonsai_msdTCAMRC.h>
#include <bonsai/include/api/Bonsai_msdTCAMRCComp.h>
#include <bonsai/include/api/Bonsai_msdRMU.h>
#include <bonsai/include/api/Bonsai_msdLed.h>
#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __Bonsai_msdApi_h */
