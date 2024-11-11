/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef msdApi_h
#define msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/
#include <driver/msdApiTypes.h>
#include <driver/msdHwAccess.h>
#include <platform/msdSem.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

/* add modules whatever you needed */
#include <api/msdArp.h>
#include <api/msdBrgFdb.h>
#include <api/msdEcid.h>
#include <api/msdAdvVct.h>
#include <api/msdBrgStu.h>
#include <api/msdBrgVtu.h>
#include <api/msdIMP.h>
#include <api/msdEEPROM.h>
#include <api/msdPhyCtrl.h>
#include <api/msdPIRL.h>
#include <api/msdPortCtrl.h>
#include <api/msdQosMap.h>
#include <api/msdPortRmon.h>
#include <api/msdPolicy.h>
#include <api/msdQueueCtrl.h>
#include <api/msdSysCtrl.h>
#include <api/msdTCAM.h>
#include <api/msdTCAMRC.h>
#include <api/msdTCAMRCComp.h>
#include <api/msdRMU.h>
#include <api/msdQav.h>
#include <api/msdPTP.h>
#include <api/msdTrunk.h>
#include <api/msdLed.h>
#include <api/msdQci.h>
#include <api/msdQcr.h>
#include <api/msdFRER.h>
#include <api/msdMACSec.h>
#ifdef __cplusplus
}
#endif

#endif /* msdApi_h */
