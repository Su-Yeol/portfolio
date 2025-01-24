/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* BonsaiZ1_msdApi.h
*
* DESCRIPTION:
*       API definitions for QuarterDeck Device
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef BonsaiZ1_msdApi_h
#define BonsaiZ1_msdApi_h

#ifdef __cplusplus
extern "C" {
#endif

/* minimum need header files*/

/* add modules whatever you needed */
#include <bonsaiz1/include/api/BonsaiZ1_msdArp.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdBrgFdb.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdEcid.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdBrgStu.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdBrgVtu.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdEEPROM.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPhyCtrl.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPIRL.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPolicy.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPortCtrl.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPortPrioMap.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPortRmon.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdQueueCtrl.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdSysCtrl.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdQav.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdPTP.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdTCAM.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdTCAMRC.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdTCAMRCComp.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdRMU.h>
#include <bonsaiz1/include/api/BonsaiZ1_msdLed.h>
#include <bonsaiz1/include/driver/BonsaiZ1_msdHwAccess.h>
#ifdef __cplusplus
}
#endif

#endif /* __BonsaiZ1_msdApi_h */
