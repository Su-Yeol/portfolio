/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdDevObj.c
*
* DESCRIPTION:
*       API definitions for system configuration, and enabling.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <driver/msdSysConfig.h>
#include <driver/msdDevObj.h>
#include <driver/msdHwAccess.h>
#include <utils/msdUtils.h>
#ifdef TOPAZ_BUILD_IN
#include <topaz/include/Topaz_msdApi.h>
#endif
#ifdef PERIDOT_BUILD_IN
#include <peridot/include/Peridot_msdApi.h>
#endif
#ifdef PEARL_BUILD_IN
#include <pearl/include/Pearl_msdApi.h>
#endif
#ifdef AGATE_BUILD_IN
#include <agate/include/Agate_msdApi.h>
#endif
#ifdef AMETHYST_BUILD_IN
#include <amethyst/include/Amethyst_msdApi.h>
#endif
#ifdef OAK_BUILD_IN
#include <oak/include/Oak_msdApi.h>
#endif
#ifdef SPRUCE_BUILD_IN
#include <spruce/include/Spruce_msdApi.h>
#endif
#ifdef BONSAIZ1_BUILD_IN
#include <bonsaiz1/include/BonsaiZ1_msdApi.h>
#endif
#ifdef FIR_BUILD_IN
#include <fir/include/Fir_msdApi.h>
#endif
#ifdef BONSAI_BUILD_IN
#include <bonsai/include/Bonsai_msdApi.h>
#endif

/* static function declare*/
static MSD_STATUS InitObj(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Peridot(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Topaz(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Pearl(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Agate(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Amethyst(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Oak(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Spruce(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_BonsaiZ1(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Fir(MSD_QD_DEV *dev);
static MSD_STATUS InitDevObj_Bonsai(MSD_QD_DEV *dev);
static MSD_STATUS msdReserveFreeQueueSize(MSD_QD_DEV *qdev);


static MSD_STATUS InitDevObj_Peridot(MSD_QD_DEV *dev)
{
#ifdef PERIDOT_BUILD_IN
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Peridot_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Peridot_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Peridot_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Peridot_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Peridot_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Peridot_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Peridot_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Peridot_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Peridot_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Peridot_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Peridot_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Peridot_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Peridot_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Peridot_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Peridot_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Peridot_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Peridot_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Peridot_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Peridot_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Peridot_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Peridot_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Peridot_gfdbGetNonStaticEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = &Peridot_gfdbDump;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = &Peridot_gprtSetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = &Peridot_gprtGetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Peridot_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Peridot_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Peridot_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Peridot_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Peridot_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Peridot_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Peridot_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Peridot_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Peridot_gprtSetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Peridot_gprtGetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Peridot_gprtSetRxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Peridot_gprtGetRxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Peridot_gprtSetRxPriorityFlowControlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Peridot_gprtGetRxPriorityFlowControlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Peridot_gprtSetTxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Peridot_gprtGetTxPriorityFlowControlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Peridot_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Peridot_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Peridot_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Peridot_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Peridot_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Peridot_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Peridot_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Peridot_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Peridot_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Peridot_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Peridot_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Peridot_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Peridot_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Peridot_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Peridot_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = &Peridot_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = &Peridot_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Peridot_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Peridot_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Peridot_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Peridot_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Peridot_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Peridot_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Peridot_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Peridot_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Peridot_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Peridot_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Peridot_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Peridot_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &Peridot_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &Peridot_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Peridot_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Peridot_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Peridot_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Peridot_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Peridot_gvtuDump;
	
    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Peridot_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Peridot_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Peridot_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Peridot_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Peridot_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Peridot_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Peridot_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Peridot_gsysGetTrunkRouting;
	
	dev->SwitchDevObj.IMPObj.impRun = &Peridot_impRunIntf;
	dev->SwitchDevObj.IMPObj.impReset = &Peridot_impResetIntf;
	dev->SwitchDevObj.IMPObj.impWriteEEPROM = &Peridot_impWriteEEPROMIntf;
	dev->SwitchDevObj.IMPObj.impLoadToEEPROM = &Peridot_impLoadToEEPROMIntf;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = &Peridot_impLoadToRAMIntf;
	dev->SwitchDevObj.IMPObj.impStop = &Peridot_impStopIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = &Peridot_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Peridot_serdesReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = &Peridot_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = &Peridot_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = &Peridot_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = &Peridot_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = &Peridot_gphySetPortAutoModeIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = &Peridot_gphySetEEE;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = &Peridot_gphySetFlowControlEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = &Peridot_gphySetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = &Peridot_gphyGetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = &Peridot_gphyGetIntStatus;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Peridot_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Peridot_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Peridot_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Peridot_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Peridot_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Peridot_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Peridot_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Peridot_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Peridot_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Peridot_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Peridot_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Peridot_gpirlGetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Peridot_gpirlSetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Peridot_gpirlAdvConfigIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Peridot_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Peridot_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Peridot_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Peridot_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Peridot_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Peridot_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Peridot_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Peridot_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Peridot_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Peridot_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Peridot_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Peridot_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Peridot_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Peridot_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Peridot_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Peridot_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Peridot_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Peridot_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Peridot_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Peridot_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Peridot_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Peridot_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Peridot_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Peridot_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Peridot_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Peridot_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Peridot_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Peridot_gprtSetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Peridot_gprtGetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Peridot_gprtSetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Peridot_gprtGetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Peridot_gprtSetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Peridot_gprtGetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Peridot_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Peridot_gprtGetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Peridot_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Peridot_gprtGetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Peridot_gprtSetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Peridot_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Peridot_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Peridot_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Peridot_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Peridot_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Peridot_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Peridot_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Peridot_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Peridot_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Peridot_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Peridot_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Peridot_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Peridot_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Peridot_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Peridot_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Peridot_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Peridot_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Peridot_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Peridot_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Peridot_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Peridot_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Peridot_gprtGetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Peridot_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Peridot_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Peridot_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Peridot_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Peridot_gprtRegDump;

    dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Peridot_gprtGetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Peridot_gprtSetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Peridot_gqosGetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Peridot_gprtGetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Peridot_gprtSetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Peridot_gprtGetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Peridot_gprtSetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Peridot_gprtGetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Peridot_gprtSetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Peridot_gqosSetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Peridot_gprtRewriteDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Peridot_gprtSetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Peridot_gprtGetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Peridot_gprtSetCosToDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Peridot_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Peridot_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Peridot_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Peridot_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Peridot_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Peridot_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Peridot_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Peridot_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Peridot_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Peridot_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Peridot_gprtSetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Peridot_gprtGetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Peridot_gsysSetQoSWeightIntf;
    dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Peridot_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Peridot_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Peridot_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = &Peridot_gsysSetMonitorMgmtCtrl;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = &Peridot_gsysGetMonitorMgmtCtrl;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = &Peridot_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = &Peridot_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = &Peridot_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = &Peridot_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Peridot_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Peridot_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = &Peridot_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = &Peridot_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = &Peridot_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = &Peridot_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Peridot_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Peridot_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Peridot_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Peridot_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Peridot_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Peridot_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Peridot_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Peridot_gsysQbvRead;

    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Peridot_gqavGetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Peridot_gqavSetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Peridot_gqavGetPortQpriXRate;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Peridot_gqavSetPortQpriXRate;

    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Peridot_gptpSetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Peridot_gptpGetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Peridot_gptpSetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Peridot_gptpGetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpSetPortConfig = &Peridot_gptpSetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPortConfig = &Peridot_gptpGetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Peridot_gptpGetTimeStampIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Peridot_gptpGetTSValidSt;
    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Peridot_gptpResetTimeStamp;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = &Peridot_gptpSetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = &Peridot_gptpGetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Peridot_gptpGetIntStatusIntf;
    dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Peridot_gptpSetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Peridot_gptpGetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Peridot_gptpSetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Peridot_gptpGetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Peridot_gptpSetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Peridot_gptpGetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpSetMode = &Peridot_gptpSetModeIntf;
    dev->SwitchDevObj.PTPObj.gptpGetMode = &Peridot_gptpGetModeIntf;
    dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = &Peridot_gptpSetMasterSlaveIntf;
    dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = &Peridot_gptpGetMasterSlaveIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Peridot_gptpGetTODBusyBitStatus;
    dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Peridot_gptpTODStoreCompensationIntf;
    dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Peridot_gptpTODStoreAllIntf;
    dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Peridot_gptpTODCaptureAllIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPulse = &Peridot_gptpSetPulseIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPulse = &Peridot_gptpGetPulseIntf;
    dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Peridot_gtaiGetPTPGlobalTime;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Peridot_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Peridot_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Peridot_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Peridot_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Peridot_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Peridot_gtcamFindEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Peridot_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Peridot_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Peridot_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Peridot_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Peridot_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Peridot_gtcamEgrFlushEntryAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Peridot_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Peridot_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Peridot_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &Peridot_gtcamDump;
    dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Peridot_gtcamAdvConfigIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &Peridot_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Peridot_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Peridot_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Peridot_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Peridot_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Peridot_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Peridot_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Peridot_msdRmuRegDump;

    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Peridot_msdGetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Peridot_msdSetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Peridot_msdGetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Peridot_msdSetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Peridot_msdSetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Peridot_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Peridot_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Peridot_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Peridot_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Peridot_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Peridot_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Peridot_EEPROMReadIntf;

    dev->SwitchDevObj.LEDObj.gledForceOn = &Peridot_gledForceOnIntf;
    dev->SwitchDevObj.LEDObj.gledForceOff = &Peridot_gledForceOffIntf;
    dev->SwitchDevObj.LEDObj.gledLinkAct = &Peridot_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Peridot_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Peridot_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Peridot_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Peridot_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Peridot_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Peridot_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Peridot_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Peridot_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Peridot_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Peridot_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Peridot_gledDump;

#endif
    return MSD_OK;
}

static MSD_STATUS InitDevObj_Topaz(MSD_QD_DEV *dev)
{
#ifdef TOPAZ_BUILD_IN
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Topaz_gfdbAddMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Topaz_gfdbGetAtuEntryNextIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlush = &Topaz_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Topaz_gfdbFlushInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbMove = &Topaz_gfdbMoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Topaz_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Topaz_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Topaz_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Topaz_gfdbDelAtuEntry;
    dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Topaz_gfdbGetViolationIntf;
    dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Topaz_gfdbFindAtuMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Topaz_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Topaz_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Topaz_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Topaz_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Topaz_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Topaz_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Topaz_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Topaz_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Topaz_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Topaz_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Topaz_gfdbGetNonStaticEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = &Topaz_gfdbDump;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = &Topaz_gprtSetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = &Topaz_gprtGetCutThrouthEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Topaz_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Topaz_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Topaz_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Topaz_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Topaz_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Topaz_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Topaz_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Topaz_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Topaz_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Topaz_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Topaz_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Topaz_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Topaz_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Topaz_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Topaz_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Topaz_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Topaz_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Topaz_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Topaz_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Topaz_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Topaz_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Topaz_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Topaz_gprtGetARPMirror;

    dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = &Topaz_gvctGetAdvCableDiagIntf;
    dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = &Topaz_gvctGetAdvExtendedStatusIntf;

    dev->SwitchDevObj.STUObj.gstuAddEntry = &Topaz_gstuAddEntryIntf;
    dev->SwitchDevObj.STUObj.gstuDelEntry = &Topaz_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Topaz_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Topaz_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Topaz_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Topaz_gstuDump;

    dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Topaz_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Topaz_gvtuExistVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuFlush = &Topaz_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Topaz_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Topaz_gvtuGetEntryNextIntf;
    dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Topaz_gvtuDelEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Topaz_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Topaz_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Topaz_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Topaz_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Topaz_gvtuDump;

    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Topaz_gprtGetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Topaz_gprtGetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Topaz_gprtSetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Topaz_gprtSetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Topaz_gprtGetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Topaz_gqosGetPrioMapRule;
    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Topaz_gprtSetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Topaz_gqosSetPrioMapRuleIntf;

    dev->SwitchDevObj.IMPObj.impRun = &Topaz_impRunIntf;
    dev->SwitchDevObj.IMPObj.impReset = &Topaz_impResetIntf;
    dev->SwitchDevObj.IMPObj.impWriteEEPROM = &Topaz_impWriteEEPROMIntf;
    dev->SwitchDevObj.IMPObj.impLoadToEEPROM = &Topaz_impLoadToEEPROMIntf;
    dev->SwitchDevObj.IMPObj.impLoadToRAM = &Topaz_impLoadToRAMIntf;
    dev->SwitchDevObj.IMPObj.impStop = &Topaz_impStopIntf;

    dev->SwitchDevObj.PHYCTRLObj.gphyReset = &Topaz_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Topaz_serdesReset;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = &Topaz_gphySetPortLoopback;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = &Topaz_gphySetPortSpeedIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = &Topaz_gphyPortPowerDown;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = &Topaz_gphySetPortDuplexMode;
    dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = &Topaz_gphySetPortAutoModeIntf;
    dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = &Topaz_gphySetEEE;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = &Topaz_gphySetFlowControlEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = &Topaz_gphySetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = &Topaz_gphyGetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = &Topaz_gphyGetIntStatus;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Topaz_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Topaz_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Topaz_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Topaz_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Topaz_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Topaz_gsysActiveIntStatusGet;

    dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Topaz_gpirlInitializeIntf;
    dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Topaz_gpirlInitResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Topaz_gpirlCustomSetupIntf;
    dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Topaz_gpirlReadResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Topaz_gpirlWriteResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Topaz_gpirlGetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Topaz_gpirlSetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Topaz_gpirlAdvConfigIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Topaz_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Topaz_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Topaz_gprtGetForceDefaultVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Topaz_gvlnSetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Topaz_gvlnGetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Topaz_gvlnSetPortVlanDot1qModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Topaz_gvlnGetPortVlanDot1qModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Topaz_gprtSetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Topaz_gprtGetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Topaz_gprtSetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Topaz_gprtGetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Topaz_gprtSetUnicastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Topaz_gprtGetUnicastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Topaz_gprtSetMulticastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Topaz_gprtGetMulticastFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Topaz_gprtSetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Topaz_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Topaz_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Topaz_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Topaz_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Topaz_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Topaz_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Topaz_gprtGetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Topaz_gprtSetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Topaz_gprtGetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Topaz_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Topaz_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Topaz_gprtSetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Topaz_gprtGetVTUPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Topaz_gprtSetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Topaz_gprtGetSAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Topaz_gprtSetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Topaz_gprtGetDAPriorityOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Topaz_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Topaz_gprtGetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Topaz_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Topaz_gprtGetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Topaz_gprtSetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Topaz_gprtGetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Topaz_gprtSetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Topaz_gprtGetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Topaz_gprtSetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Topaz_gprtGetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Topaz_grcSetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Topaz_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Topaz_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Topaz_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Topaz_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Topaz_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Topaz_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Topaz_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Topaz_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Topaz_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Topaz_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Topaz_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Topaz_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Topaz_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Topaz_gprtGetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Topaz_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Topaz_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Topaz_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Topaz_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Topaz_gprtRegDump;

    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Topaz_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Topaz_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Topaz_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Topaz_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Topaz_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Topaz_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Topaz_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Topaz_gsysGetTrunkRouting;

    dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Topaz_gstatsFlushAll;
    dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Topaz_gstatsFlushPort;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Topaz_gstatsGetPortCounterIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Topaz_gstatsGetPortAllCountersIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Topaz_gstatsGetHistogramModeIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Topaz_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Topaz_gmibDump;

    dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Topaz_gprtSetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Topaz_gprtGetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Topaz_gprtSetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Topaz_gprtGetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Topaz_gsysSetQoSWeightIntf;
    dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Topaz_gsysGetQoSWeightIntf;

    dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Topaz_gsysSetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Topaz_gsysGetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = &Topaz_gsysSetMonitorMgmtCtrl;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = &Topaz_gsysGetMonitorMgmtCtrl;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = &Topaz_gsysSetCPUDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = &Topaz_gsysGetCPUDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = &Topaz_gsysSetIngressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = &Topaz_gsysGetIngressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Topaz_gsysSetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Topaz_gsysGetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = &Topaz_gsysSetRsvd2CpuEnables;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = &Topaz_gsysGetRsvd2CpuEnables;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = &Topaz_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = &Topaz_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Topaz_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Topaz_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Topaz_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Topaz_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Topaz_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Topaz_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Topaz_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Topaz_gsysQbvRead;

    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Topaz_gqavGetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Topaz_gqavSetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Topaz_gqavGetPortQpriXRate;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Topaz_gqavSetPortQpriXRate;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Topaz_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Topaz_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Topaz_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Topaz_gtcamRemoveEntryPort;

    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Topaz_gptpSetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Topaz_gptpGetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Topaz_gptpSetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Topaz_gptpGetProtPTPEn;
    dev->SwitchDevObj.PTPObj.gptpSetPortConfig = &Topaz_gptpSetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPortConfig = &Topaz_gptpGetPortConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Topaz_gptpGetTimeStampIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Topaz_gptpGetTSValidSt;
    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Topaz_gptpResetTimeStamp;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = &Topaz_gptpSetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = &Topaz_gptpGetGlobalConfigIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Topaz_gptpGetIntStatusIntf;

	if (dev->deviceId != MSD_88E6141)
    {
        dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Topaz_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Topaz_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Topaz_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Topaz_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Topaz_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Topaz_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = &Topaz_gtcamDump;
        dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Topaz_gtcamAdvConfigIntf;
	}
	/* Topaz deviceId MSD_88E6141 doesn't support TCAM and AVB */

	dev->SwitchDevObj.RMUObj.grmuGetID = &Topaz_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Topaz_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Topaz_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Topaz_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Topaz_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Topaz_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Topaz_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Topaz_msdRmuRegDump;

    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Topaz_msdGetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Topaz_msdSetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Topaz_msdGetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Topaz_msdSetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Topaz_msdSetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Topaz_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Topaz_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Topaz_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Topaz_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Topaz_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Topaz_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Topaz_EEPROMReadIntf;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Topaz_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Topaz_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &Topaz_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Topaz_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Topaz_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Topaz_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Topaz_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Topaz_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Topaz_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Topaz_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Topaz_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Topaz_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Topaz_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Topaz_gledDump;

#endif
    return MSD_OK;
}

static MSD_STATUS InitDevObj_Pearl(MSD_QD_DEV *dev)
{
#ifdef PEARL_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Pearl_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Pearl_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Pearl_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Pearl_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Pearl_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Pearl_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Pearl_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Pearl_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Pearl_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Pearl_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Pearl_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Pearl_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Pearl_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Pearl_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Pearl_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Pearl_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Pearl_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Pearl_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Pearl_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Pearl_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Pearl_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Pearl_gfdbGetNonStaticEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Pearl_gfdbDump;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Pearl_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Pearl_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Pearl_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Pearl_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Pearl_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Pearl_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Pearl_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Pearl_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Pearl_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Pearl_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Pearl_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Pearl_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Pearl_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Pearl_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Pearl_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Pearl_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Pearl_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Pearl_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Pearl_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Pearl_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Pearl_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Pearl_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Pearl_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = &Pearl_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = &Pearl_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Pearl_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Pearl_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Pearl_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Pearl_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Pearl_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Pearl_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Pearl_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Pearl_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Pearl_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Pearl_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Pearl_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Pearl_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Pearl_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Pearl_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Pearl_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Pearl_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Pearl_gvtuDump;

	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Pearl_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Pearl_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Pearl_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Pearl_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Pearl_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Pearl_gqosGetPrioMapRule;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Pearl_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Pearl_gqosSetPrioMapRuleIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = &Pearl_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Pearl_serdesReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = &Pearl_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = &Pearl_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = &Pearl_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = &Pearl_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = &Pearl_gphySetPortAutoModeIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = &Pearl_gphySetEEE;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = &Pearl_gphySetFlowControlEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = &Pearl_gphySetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = &Pearl_gphyGetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = &Pearl_gphyGetIntStatus;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Pearl_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Pearl_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Pearl_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Pearl_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Pearl_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Pearl_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Pearl_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Pearl_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Pearl_gpirlCustomSetupIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Pearl_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Pearl_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Pearl_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Pearl_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Pearl_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Pearl_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Pearl_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Pearl_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Pearl_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Pearl_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Pearl_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Pearl_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Pearl_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Pearl_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Pearl_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Pearl_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Pearl_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Pearl_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Pearl_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Pearl_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Pearl_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Pearl_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Pearl_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Pearl_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Pearl_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Pearl_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Pearl_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Pearl_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Pearl_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Pearl_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Pearl_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Pearl_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Pearl_gprtSetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Pearl_gprtGetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Pearl_gprtSetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Pearl_gprtGetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Pearl_gprtSetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Pearl_gprtGetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Pearl_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Pearl_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Pearl_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Pearl_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Pearl_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Pearl_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Pearl_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Pearl_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Pearl_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Pearl_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Pearl_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Pearl_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Pearl_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Pearl_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Pearl_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Pearl_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Pearl_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Pearl_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Pearl_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Pearl_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Pearl_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Pearl_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Pearl_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Pearl_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Pearl_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Pearl_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Pearl_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Pearl_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Pearl_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Pearl_gprtRegDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Pearl_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Pearl_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Pearl_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Pearl_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Pearl_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Pearl_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Pearl_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Pearl_gsysGetTrunkRouting;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Pearl_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Pearl_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Pearl_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Pearl_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Pearl_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Pearl_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Pearl_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Pearl_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Pearl_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Pearl_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Pearl_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Pearl_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Pearl_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = &Pearl_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = &Pearl_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = &Pearl_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = &Pearl_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Pearl_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Pearl_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = &Pearl_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = &Pearl_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = &Pearl_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = &Pearl_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Pearl_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Pearl_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Pearl_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Pearl_gsysGetRMUMode;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Pearl_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Pearl_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Pearl_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Pearl_gqavSetPortQpriXRate;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Pearl_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Pearl_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Pearl_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Pearl_gtcamRemoveEntryPort;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Pearl_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Pearl_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Pearl_gptpSetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Pearl_gptpGetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = &Pearl_gptpSetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = &Pearl_gptpGetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Pearl_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Pearl_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Pearl_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = &Pearl_gptpSetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = &Pearl_gptpGetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Pearl_gptpGetIntStatusIntf;

	if (dev->deviceId != MSD_88E6320)
	{
		dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Pearl_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Pearl_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Pearl_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Pearl_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Pearl_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Pearl_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = &Pearl_gtcamDump;
		dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Pearl_gtcamAdvConfigIntf;
		dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = &Pearl_gprtSetCutThrouthEnableIntf;
		dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = &Pearl_gprtGetCutThrouthEnableIntf;
	}
	/* Pearl deviceId MSD_88E6320 doesn't support TCAM and cut thru */

	dev->SwitchDevObj.RMUObj.grmuGetID = &Pearl_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Pearl_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Pearl_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Pearl_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Pearl_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Pearl_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Pearl_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Pearl_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Pearl_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Pearl_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Pearl_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Pearl_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Pearl_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Pearl_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Pearl_msdGetSMIPhyXMDIOReg;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Pearl_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Pearl_EEPROMReadIntf;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Pearl_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Pearl_gledForceOffIntf;	
	dev->SwitchDevObj.LEDObj.gledModeSet = &Pearl_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Pearl_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Pearl_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Pearl_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Pearl_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Pearl_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Pearl_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Pearl_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Pearl_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Pearl_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Pearl_gledDump;

#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_Agate(MSD_QD_DEV *dev)
{
#ifdef AGATE_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Agate_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Agate_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Agate_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Agate_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Agate_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Agate_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Agate_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Agate_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Agate_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Agate_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Agate_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Agate_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Agate_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Agate_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Agate_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Agate_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Agate_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Agate_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Agate_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Agate_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Agate_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Agate_gfdbGetNonStaticEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Agate_gfdbDump;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Agate_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Agate_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Agate_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Agate_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Agate_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Agate_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Agate_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Agate_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Agate_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Agate_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Agate_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Agate_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Agate_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Agate_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Agate_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Agate_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Agate_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Agate_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Agate_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Agate_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Agate_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Agate_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Agate_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = &Agate_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = &Agate_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Agate_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Agate_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Agate_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Agate_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Agate_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Agate_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Agate_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Agate_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Agate_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Agate_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Agate_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Agate_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Agate_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Agate_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Agate_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Agate_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Agate_gvtuDump;

	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Agate_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Agate_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Agate_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Agate_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Agate_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Agate_gqosGetPrioMapRule;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Agate_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Agate_gqosSetPrioMapRuleIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = &Agate_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Agate_serdesReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = &Agate_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = &Agate_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = &Agate_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = &Agate_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = &Agate_gphySetPortAutoModeIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = &Agate_gphySetEEE;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = &Agate_gphySetFlowControlEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = &Agate_gphySetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = &Agate_gphyGetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = &Agate_gphyGetIntStatus;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Agate_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Agate_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Agate_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Agate_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Agate_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Agate_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Agate_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Agate_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Agate_gpirlCustomSetupIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Agate_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Agate_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Agate_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Agate_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Agate_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Agate_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Agate_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Agate_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Agate_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Agate_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Agate_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Agate_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Agate_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Agate_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Agate_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Agate_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Agate_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Agate_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Agate_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Agate_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Agate_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Agate_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Agate_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Agate_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Agate_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Agate_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Agate_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Agate_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Agate_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Agate_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Agate_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Agate_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Agate_gprtSetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Agate_gprtGetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Agate_gprtSetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Agate_gprtGetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Agate_gprtSetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Agate_gprtGetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Agate_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Agate_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Agate_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Agate_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Agate_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Agate_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Agate_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Agate_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Agate_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Agate_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Agate_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Agate_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Agate_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Agate_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Agate_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Agate_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Agate_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Agate_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Agate_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Agate_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Agate_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Agate_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Agate_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Agate_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Agate_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Agate_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Agate_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Agate_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Agate_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Agate_gprtRegDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Agate_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Agate_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Agate_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Agate_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Agate_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Agate_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Agate_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Agate_gsysGetTrunkRouting;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Agate_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Agate_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Agate_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Agate_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Agate_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Agate_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Agate_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Agate_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Agate_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Agate_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Agate_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Agate_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Agate_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = &Agate_gsysSetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = &Agate_gsysGetCPUDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = &Agate_gsysSetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = &Agate_gsysGetIngressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Agate_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Agate_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = &Agate_gsysSetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = &Agate_gsysGetRsvd2CpuEnables;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = &Agate_gsysSetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = &Agate_gsysGetMGMTPri;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Agate_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Agate_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Agate_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Agate_gsysGetRMUMode;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Agate_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Agate_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Agate_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Agate_gqavSetPortQpriXRate;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Agate_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Agate_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Agate_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Agate_gtcamRemoveEntryPort;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Agate_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Agate_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Agate_gptpSetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Agate_gptpGetProtPTPEn;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = &Agate_gptpSetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = &Agate_gptpGetPortConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Agate_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Agate_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Agate_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = &Agate_gptpSetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = &Agate_gptpGetGlobalConfigIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Agate_gptpGetIntStatusIntf;

	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Agate_gtcamFindEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Agate_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Agate_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Agate_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Agate_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Agate_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &Agate_gtcamDump;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Agate_gtcamAdvConfigIntf;
	
	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Agate_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Agate_EEPROMReadIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &Agate_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Agate_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMibDump = &Agate_msdRmuMibDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Agate_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Agate_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Agate_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Agate_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Agate_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Agate_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Agate_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Agate_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Agate_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Agate_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Agate_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Agate_msdGetSMIPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Agate_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Agate_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Agate_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Agate_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Agate_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Agate_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Agate_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Agate_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Agate_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Agate_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Agate_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Agate_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Agate_gledDump;

#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_Amethyst(MSD_QD_DEV *dev)
{
	MSD_STATUS retVal = MSD_OK;

#ifdef AMETHYST_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Amethyst_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Amethyst_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Amethyst_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Amethyst_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Amethyst_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Amethyst_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Amethyst_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Amethyst_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Amethyst_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Amethyst_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Amethyst_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Amethyst_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Amethyst_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Amethyst_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Amethyst_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Amethyst_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Amethyst_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Amethyst_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Amethyst_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Amethyst_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Amethyst_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Amethyst_gfdbGetNonStaticEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Amethyst_gfdbDump;

	/* Amethyst deviceId MSD_88E6193X and MSD_88E6191X doesn't support Route, TSN, CutThrough */
	if ((dev->deviceId != MSD_88E6191X) && (dev->deviceId != MSD_88E6193X))
	{
		dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Amethyst_garpLoadMcEntryIntf;
		dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Amethyst_garpLoadUcEntryIntf;
		dev->SwitchDevObj.ARPObj.garpFlushEntry = &Amethyst_garpFlushEntry;
		dev->SwitchDevObj.ARPObj.garpFlushAll = &Amethyst_garpFlushAll;
		dev->SwitchDevObj.ARPObj.garpReadEntry = &Amethyst_garpReadEntryIntf;
		dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Amethyst_garpGetNextEntryIntf;

		dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Amethyst_gqavGetPortQpriXHiLimit;
		dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Amethyst_gqavSetPortQpriXHiLimit;
		dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Amethyst_gqavGetPortQpriXRate;
		dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Amethyst_gqavSetPortQpriXRate;

		dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Amethyst_gptpSetIntEnableIntf;
		dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Amethyst_gptpGetIntEnableIntf;
		dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Amethyst_gptpSetProtPTPEn;
		dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Amethyst_gptpGetProtPTPEn;
		dev->SwitchDevObj.PTPObj.gptpSetPortConfig = &Amethyst_gptpSetPortConfigIntf;
		dev->SwitchDevObj.PTPObj.gptpGetPortConfig = &Amethyst_gptpGetPortConfigIntf;
		dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Amethyst_gptpGetTimeStampIntf;
		dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Amethyst_gptpGetTSValidSt;
		dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Amethyst_gptpResetTimeStamp;
		dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = &Amethyst_gptpSetGlobalConfigIntf;
		dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = &Amethyst_gptpGetGlobalConfigIntf;
		dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Amethyst_gptpGetIntStatusIntf;
		dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Amethyst_gptpSetMeanPathDelay;
		dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Amethyst_gptpGetMeanPathDelay;
		dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Amethyst_gptpSetIgrPathDelayAsym;
		dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Amethyst_gptpGetIgrPathDelayAsym;
		dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Amethyst_gptpSetEgrPathDelayAsym;
		dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Amethyst_gptpGetEgrPathDelayAsym;
		dev->SwitchDevObj.PTPObj.gptpSetMode = &Amethyst_gptpSetModeIntf;
		dev->SwitchDevObj.PTPObj.gptpGetMode = &Amethyst_gptpGetModeIntf;
		dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = &Amethyst_gptpSetMasterSlaveIntf;
		dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = &Amethyst_gptpGetMasterSlaveIntf;
		dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Amethyst_gptpGetTODBusyBitStatus;
		dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Amethyst_gptpTODStoreCompensationIntf;
		dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Amethyst_gptpTODStoreAllIntf;
		dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Amethyst_gptpTODCaptureAllIntf;
		dev->SwitchDevObj.PTPObj.gptpSetPulse = &Amethyst_gptpSetPulseIntf;
		dev->SwitchDevObj.PTPObj.gptpGetPulse = &Amethyst_gptpGetPulseIntf;
		dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Amethyst_gtaiGetPTPGlobalTime;

		dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = &Amethyst_gprtSetCutThrouthEnableIntf;
		dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = &Amethyst_gprtGetCutThrouthEnableIntf;
	}
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Amethyst_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Amethyst_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Amethyst_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Amethyst_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Amethyst_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Amethyst_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Amethyst_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Amethyst_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Amethyst_gprtSetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Amethyst_gprtGetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Amethyst_gprtSetRxPriorityFlowControlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Amethyst_gprtGetRxPriorityFlowControlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Amethyst_gprtSetRxPriorityFlowControlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Amethyst_gprtGetRxPriorityFlowControlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Amethyst_gprtSetTxPriorityFlowControlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Amethyst_gprtGetTxPriorityFlowControlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Amethyst_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Amethyst_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Amethyst_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Amethyst_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Amethyst_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Amethyst_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Amethyst_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Amethyst_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Amethyst_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Amethyst_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Amethyst_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Amethyst_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Amethyst_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Amethyst_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Amethyst_gprtGetARPMirror;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = &Amethyst_gvctGetAdvCableDiagIntf;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = &Amethyst_gvctGetAdvExtendedStatusIntf;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Amethyst_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Amethyst_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Amethyst_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Amethyst_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Amethyst_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Amethyst_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Amethyst_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Amethyst_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Amethyst_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Amethyst_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Amethyst_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Amethyst_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &Amethyst_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &Amethyst_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Amethyst_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Amethyst_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Amethyst_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Amethyst_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Amethyst_gvtuDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Amethyst_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Amethyst_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Amethyst_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Amethyst_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Amethyst_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Amethyst_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Amethyst_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Amethyst_gsysGetTrunkRouting;

	dev->SwitchDevObj.IMPObj.impRun = &Amethyst_impRunIntf;
	dev->SwitchDevObj.IMPObj.impReset = &Amethyst_impResetIntf;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = &Amethyst_impLoadToRAMIntf;
	dev->SwitchDevObj.IMPObj.impStop = &Amethyst_impStopIntf;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Amethyst_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Amethyst_EEPROMReadIntf;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Amethyst_EEPROMChipSelSetIntf;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Amethyst_EEPROMChipSelGetIntf;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = &Amethyst_gphyReset;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Amethyst_serdesReset;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = &Amethyst_gphySetPortLoopback;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = &Amethyst_gphySetPortSpeedIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = &Amethyst_gphyPortPowerDown;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = &Amethyst_gphySetPortDuplexMode;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = &Amethyst_gphySetPortAutoModeIntf;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = &Amethyst_gphySetEEE;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = &Amethyst_gphySetFlowControlEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = &Amethyst_gphySetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = &Amethyst_gphyGetIntEnable;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = &Amethyst_gphyGetIntStatus;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Amethyst_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Amethyst_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Amethyst_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Amethyst_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Amethyst_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Amethyst_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Amethyst_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Amethyst_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Amethyst_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Amethyst_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Amethyst_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Amethyst_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Amethyst_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Amethyst_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Amethyst_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Amethyst_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Amethyst_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Amethyst_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Amethyst_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Amethyst_gvlnSetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Amethyst_gvlnGetPortVlanDot1qModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Amethyst_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Amethyst_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Amethyst_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Amethyst_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Amethyst_gprtSetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Amethyst_gprtGetUnicastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Amethyst_gprtSetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Amethyst_gprtGetMulticastFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Amethyst_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Amethyst_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Amethyst_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Amethyst_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Amethyst_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Amethyst_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Amethyst_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Amethyst_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Amethyst_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Amethyst_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Amethyst_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Amethyst_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Amethyst_gprtSetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Amethyst_gprtGetVTUPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Amethyst_gprtSetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Amethyst_gprtGetSAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Amethyst_gprtSetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Amethyst_gprtGetDAPriorityOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Amethyst_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Amethyst_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Amethyst_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Amethyst_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Amethyst_gprtSetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Amethyst_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Amethyst_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Amethyst_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Amethyst_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Amethyst_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Amethyst_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Amethyst_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Amethyst_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Amethyst_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Amethyst_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Amethyst_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Amethyst_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Amethyst_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Amethyst_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Amethyst_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Amethyst_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Amethyst_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Amethyst_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Amethyst_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Amethyst_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Amethyst_gprtSetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Amethyst_gprtGetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Amethyst_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Amethyst_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Amethyst_gprtSetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Amethyst_gprtGetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Amethyst_gprtSetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Amethyst_gprtGetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Amethyst_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Amethyst_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Amethyst_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Amethyst_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Amethyst_gprtRegDump;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Amethyst_gprtGetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Amethyst_gprtSetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Amethyst_gqosGetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Amethyst_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Amethyst_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Amethyst_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Amethyst_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Amethyst_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Amethyst_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Amethyst_gqosSetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Amethyst_gprtRewriteDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Amethyst_gprtSetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Amethyst_gprtGetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Amethyst_gprtSetCosToDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Amethyst_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Amethyst_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Amethyst_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Amethyst_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Amethyst_gstatsGetPortAllCountersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Amethyst_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Amethyst_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Amethyst_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Amethyst_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Amethyst_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Amethyst_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Amethyst_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Amethyst_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Amethyst_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Amethyst_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Amethyst_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Amethyst_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Amethyst_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Amethyst_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Amethyst_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Amethyst_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Amethyst_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Amethyst_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Amethyst_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Amethyst_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Amethyst_gsysQbvRead;

	/* Amethyst deviceId MSD_88E6191X doesn't support ECID, TCAM */
	if (dev->deviceId != MSD_88E6191X)
	{
		dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Amethyst_gBpeEnableSet;
		dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Amethyst_gBpeEnableGet;
		dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Amethyst_gecidAddEntryIntf;
		dev->SwitchDevObj.ECIDObj.gecidDump = &Amethyst_gecidDump;
		dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Amethyst_gecidFindEcidEntryIntf;
		dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Amethyst_gecidFlushAll;
		dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Amethyst_gecidFlushEntry;
		dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Amethyst_gecidGetEcidEntryNextIntf;
		dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Amethyst_gecidGetEntryCount;
		dev->SwitchDevObj.ECIDObj.gecidMove = &Amethyst_gecidMove;

		dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Amethyst_gtcamFlushAll;
		dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Amethyst_gtcamFlushEntry;
		dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Amethyst_gtcamLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Amethyst_gtcamReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Amethyst_gtcamGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Amethyst_gtcamFindEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Amethyst_gtcamSetModeIntf;
		dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Amethyst_gtcamGetModeIntf;
		dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Amethyst_gtcamAddEntryPort;
		dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Amethyst_gtcamRemoveEntryPort;
		dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Amethyst_gtcamEgrFlushEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Amethyst_gtcamEgrFlushEntryAllPortsIntf;
		dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Amethyst_gtcamEgrLoadEntryIntf;
		dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Amethyst_gtcamEgrGetNextTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Amethyst_gtcamEgrReadTCAMDataIntf;
		dev->SwitchDevObj.TCAMObj.gtcamDump = &Amethyst_gtcamDump;
		dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Amethyst_gtcamAdvConfigIntf;

		dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Amethyst_gtcamRCLoadEntryIntf;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Amethyst_gtcamRCFlushAllEntry;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Amethyst_gtcamRCFlushEntry;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Amethyst_gtcamRCReadEntryIntf;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Amethyst_gtcamRCGetNextEntryIntf;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Amethyst_gtcamRCFindEntryIntf;
		dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Amethyst_gtcamRCEntryDump;

		dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Amethyst_gtcamRCCompLoadIntf;
		dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Amethyst_gtcamRCCompFlushAll;
		dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Amethyst_gtcamRCCompFlushPort;
		dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Amethyst_gtcamRCCompReadIntf;

		dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Amethyst_msdRmuTCAMDumpIntf;
		dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Amethyst_msdRmuTCAMReadIntf;
		dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Amethyst_msdRmuTCAMWriteIntf;
		dev->SwitchDevObj.RMUObj.grmuEcidDump = &Amethyst_msdRmuEcidDumpIntf;
	}
	dev->SwitchDevObj.RMUObj.grmuGetID = &Amethyst_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Amethyst_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Amethyst_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Amethyst_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Amethyst_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Amethyst_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Amethyst_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Amethyst_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Amethyst_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Amethyst_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Amethyst_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Amethyst_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Amethyst_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Amethyst_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Amethyst_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Amethyst_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Amethyst_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &Amethyst_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Amethyst_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Amethyst_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Amethyst_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Amethyst_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Amethyst_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Amethyst_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Amethyst_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Amethyst_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Amethyst_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Amethyst_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Amethyst_gledDump;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Amethyst_gpolicySetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Amethyst_gpolicyGetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Amethyst_gpolicySetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Amethyst_gpolicyGetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Amethyst_gpolicySetIngressMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Amethyst_gpolicyGetIngressMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Amethyst_gpolicySetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Amethyst_gpolicyGetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Amethyst_gpolicySetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Amethyst_gpolicyGetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Amethyst_gpolicySetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Amethyst_gpolicyGetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Amethyst_gpolicySetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Amethyst_gpolicyGetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Amethyst_gpolicySetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Amethyst_gpolicyGetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Amethyst_gpolicySetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Amethyst_gpolicyGetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Amethyst_gpolicySetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Amethyst_gpolicyGetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Amethyst_gpolicySetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Amethyst_gpolicyGetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Amethyst_gpolicySetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Amethyst_gpolicyGetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Amethyst_gprtSetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Amethyst_gprtGetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Amethyst_gpolicySetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Amethyst_gpolicyGetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Amethyst_gpolicySetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Amethyst_gpolicyGetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Amethyst_gpolicySetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Amethyst_gpolicyGetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Amethyst_gpolicySetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Amethyst_gpolicyGetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Amethyst_gpolicySetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Amethyst_gpolicyGetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Amethyst_gprtSetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Amethyst_gprtGetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Amethyst_gpolicySetICMPSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Amethyst_gpolicyGetICMPSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Amethyst_gprtSetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Amethyst_gprtGetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Amethyst_gpolicySetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Amethyst_gpolicyGetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Amethyst_gpolicySetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Amethyst_gpolicyGetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Amethyst_gpolicySetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Amethyst_gpolicyGetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Amethyst_gpolicySetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Amethyst_gpolicyGetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Amethyst_gpolicySetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Amethyst_gpolicyGetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Amethyst_gpolicySetETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Amethyst_gpolicyGetETag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Amethyst_gpolicySetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Amethyst_gpolicyGetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = &Amethyst_gpolicyDump;

	/*For Amethyst, reserve 16 free queue size*/
	retVal = msdReserveFreeQueueSize(dev);

#endif
	return retVal;
}

static MSD_STATUS InitDevObj_Oak(MSD_QD_DEV *dev)
{
#ifdef OAK_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Oak_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Oak_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Oak_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Oak_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Oak_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Oak_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Oak_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Oak_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Oak_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Oak_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Oak_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Oak_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Oak_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Oak_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Oak_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Oak_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Oak_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Oak_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Oak_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Oak_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Oak_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Oak_gfdbGetNStaticEntryCountFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Oak_gfdbDump;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Oak_garpLoadMcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Oak_garpLoadUcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = &Oak_garpFlushEntry;
	dev->SwitchDevObj.ARPObj.garpFlushAll = &Oak_garpFlushAll;
	dev->SwitchDevObj.ARPObj.garpReadEntry = &Oak_garpReadEntryIntf;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Oak_garpGetNextEntryIntf;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Oak_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Oak_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Oak_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Oak_gqavSetPortQpriXRate;

	dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Oak_serdesReset;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Oak_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Oak_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Oak_gptpSetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Oak_gptpGetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Oak_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Oak_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Oak_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Oak_gptpGetIntStatusIntf;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Oak_gptpSetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Oak_gptpGetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Oak_gptpSetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Oak_gptpGetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Oak_gptpSetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Oak_gptpGetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Oak_gptpGetTODBusyBitStatus;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Oak_gptpTODStoreCompenIntf;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Oak_gptpTODStoreAllIntf;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Oak_gptpTODCaptureAllIntf;
	dev->SwitchDevObj.PTPObj.gptpSetPulse = &Oak_gptpSetPulseIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = &Oak_gptpGetPulseIntf;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Oak_gtaiGetPTPGlobalTime;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = &Oak_gptpPortRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = &Oak_gptpPortRegSet;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &Oak_gptpGlobalRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &Oak_gptpGlobalRegSet;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &Oak_gptp1588v2ConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &Oak_gptp8021asConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &Oak_gptp8021asRevConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &Oak_gptpPreemptTimestampSelSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &Oak_gptpPreemptTimestampSelGet;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &Oak_gptpPreemptCtrlSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &Oak_gptpPreemptCtrlGet;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Oak_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Oak_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Oak_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Oak_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Oak_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Oak_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Oak_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Oak_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Oak_gprtSetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Oak_gprtGetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Oak_gprtSetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Oak_gprtGetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Oak_gprtSetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Oak_gprtGetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Oak_gprtSetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Oak_gprtGetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Oak_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Oak_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Oak_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Oak_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Oak_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Oak_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Oak_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Oak_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Oak_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Oak_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Oak_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Oak_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Oak_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Oak_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Oak_gprtGetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Oak_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Oak_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Oak_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Oak_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Oak_gvlnSetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Oak_gvlnGetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Oak_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Oak_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Oak_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Oak_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Oak_gprtSetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Oak_gprtGetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Oak_gprtSetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Oak_gprtGetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Oak_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Oak_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Oak_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Oak_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Oak_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Oak_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Oak_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Oak_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Oak_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Oak_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &Oak_gprtSetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &Oak_gprtGetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Oak_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Oak_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Oak_gprtSetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Oak_gprtGetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Oak_gprtSetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Oak_gprtGetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Oak_gprtSetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Oak_gprtGetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Oak_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Oak_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Oak_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Oak_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Oak_gprtSetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Oak_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Oak_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Oak_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Oak_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Oak_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Oak_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Oak_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Oak_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Oak_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Oak_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Oak_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Oak_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Oak_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Oak_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Oak_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Oak_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Oak_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Oak_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Oak_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Oak_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Oak_gprtSetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Oak_gprtGetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Oak_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Oak_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Oak_gprtSetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Oak_gprtGetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Oak_gprtSetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Oak_gprtGetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Oak_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Oak_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Oak_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Oak_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &Oak_gprtSetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &Oak_gprtGetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &Oak_gprtSetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &Oak_gprtGetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &Oak_gprtSetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &Oak_gprtGetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Oak_gprtRegDump;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Oak_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Oak_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Oak_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Oak_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Oak_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Oak_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Oak_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Oak_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Oak_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Oak_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Oak_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Oak_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &Oak_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &Oak_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Oak_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Oak_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Oak_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Oak_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Oak_gvtuDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Oak_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Oak_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Oak_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Oak_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Oak_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Oak_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Oak_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Oak_gsysGetTrunkRouting;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Oak_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Oak_EEPROMReadIntf;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Oak_EEPROMChipSelSetIntf;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Oak_EEPROMChipSelGetIntf;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Oak_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Oak_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Oak_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Oak_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Oak_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Oak_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Oak_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Oak_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Oak_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Oak_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Oak_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Oak_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Oak_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Oak_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Oak_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Oak_gprtGetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Oak_gprtSetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Oak_gqosGetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Oak_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Oak_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Oak_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Oak_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Oak_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Oak_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Oak_gqosSetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Oak_gprtRewriteDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Oak_gprtSetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Oak_gprtGetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Oak_gprtSetCosToDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Oak_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Oak_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Oak_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Oak_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Oak_gstatsGetPortAllCntersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Oak_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Oak_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Oak_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Oak_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Oak_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Oak_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Oak_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Oak_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Oak_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Oak_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Oak_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Oak_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Oak_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Oak_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Oak_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Oak_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Oak_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Oak_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Oak_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Oak_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Oak_gsysQbvRead;

	dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Oak_gBpeEnableSet;
	dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Oak_gBpeEnableGet;
	dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Oak_gecidAddEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidDump = &Oak_gecidDump;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Oak_gecidFindEcidEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Oak_gecidFlushAll;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Oak_gecidFlushEntry;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Oak_gecidGetEcidEntryNextIntf;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Oak_gecidGetEntryCount;
	dev->SwitchDevObj.ECIDObj.gecidMove = &Oak_gecidMove;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Oak_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Oak_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Oak_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Oak_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Oak_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Oak_gtcamFindEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Oak_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Oak_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Oak_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Oak_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Oak_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Oak_gtcamEgrFlushAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Oak_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Oak_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Oak_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &Oak_gtcamDump;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Oak_gtcamAdvConfigIntf;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Oak_gtcamRCLoadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Oak_gtcamRCFlushAllEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Oak_gtcamRCFlushEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Oak_gtcamRCReadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Oak_gtcamRCGetNextEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Oak_gtcamRCFindEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Oak_gtcamRCEntryDump;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Oak_gtcamRCCompLoadIntf;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Oak_gtcamRCCompFlushAll;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Oak_gtcamRCCompFlushPort;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Oak_gtcamRCCompReadIntf;

	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Oak_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Oak_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Oak_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = &Oak_msdRmuEcidDumpIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &Oak_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Oak_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Oak_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Oak_msdRmuMultiRegAccessIntf;
	dev->SwitchDevObj.RMUObj.grmuRegDump = &Oak_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Oak_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Oak_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Oak_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Oak_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Oak_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Oak_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Oak_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Oak_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Oak_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Oak_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Oak_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Oak_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &Oak_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Oak_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Oak_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Oak_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Oak_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Oak_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Oak_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Oak_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Oak_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Oak_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Oak_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Oak_gledDump;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Oak_gpolicySetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Oak_gpolicyGetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Oak_gpolicySetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Oak_gpolicyGetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Oak_gpolicySetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Oak_gpolicyGetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Oak_gpolicySetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Oak_gpolicyGetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Oak_gpolicySetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Oak_gpolicyGetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Oak_gpolicySetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Oak_gpolicyGetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Oak_gpolicySetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Oak_gpolicyGetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Oak_gpolicySetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Oak_gpolicyGetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Oak_gpolicySetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Oak_gpolicyGetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Oak_gpolicySetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Oak_gpolicyGetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Oak_gpolicySetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Oak_gpolicyGetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Oak_gpolicySetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Oak_gpolicyGetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Oak_gprtSetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Oak_gprtGetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Oak_gpolicySetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Oak_gpolicyGetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Oak_gpolicySetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Oak_gpolicyGetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Oak_gpolicySetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Oak_gpolicyGetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Oak_gpolicySetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Oak_gpolicyGetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Oak_gpolicySetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Oak_gpolicyGetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Oak_gprtSetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Oak_gprtGetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Oak_gpolicySetICMPSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Oak_gpolicyGetICMPSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Oak_gprtSetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Oak_gprtGetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Oak_gpolicySetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Oak_gpolicyGetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Oak_gpolicySetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Oak_gpolicyGetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Oak_gpolicySetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Oak_gpolicyGetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Oak_gpolicySetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Oak_gpolicyGetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Oak_gpolicySetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Oak_gpolicyGetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Oak_gpolicySetETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Oak_gpolicyGetETag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Oak_gpolicySetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Oak_gpolicyGetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = &Oak_gpolicyDump;

#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_Spruce(MSD_QD_DEV *dev)
{
#ifdef SPRUCE_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Spruce_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Spruce_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Spruce_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Spruce_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Spruce_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Spruce_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Spruce_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Spruce_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Spruce_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Spruce_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Spruce_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Spruce_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Spruce_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Spruce_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Spruce_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Spruce_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Spruce_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Spruce_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Spruce_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Spruce_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Spruce_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Spruce_gfdbGetNStaticEntryCountFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Spruce_gfdbDump;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Spruce_garpLoadMcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Spruce_garpLoadUcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = &Spruce_garpFlushEntry;
	dev->SwitchDevObj.ARPObj.garpFlushAll = &Spruce_garpFlushAll;
	dev->SwitchDevObj.ARPObj.garpReadEntry = &Spruce_garpReadEntryIntf;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Spruce_garpGetNextEntryIntf;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Spruce_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Spruce_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Spruce_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Spruce_gqavSetPortQpriXRate;

	dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Spruce_serdesReset;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Spruce_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Spruce_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Spruce_gptpSetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Spruce_gptpGetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Spruce_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Spruce_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Spruce_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Spruce_gptpGetIntStatusIntf;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Spruce_gptpSetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Spruce_gptpGetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Spruce_gptpSetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Spruce_gptpGetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Spruce_gptpSetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Spruce_gptpGetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Spruce_gptpGetTODBusyBitStatus;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Spruce_gptpTODStoreCompenIntf;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Spruce_gptpTODStoreAllIntf;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Spruce_gptpTODCaptureAllIntf;
	dev->SwitchDevObj.PTPObj.gptpSetPulse = &Spruce_gptpSetPulseIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = &Spruce_gptpGetPulseIntf;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Spruce_gtaiGetPTPGlobalTime;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = &Spruce_gptpPortRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = &Spruce_gptpPortRegSet;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &Spruce_gptpGlobalRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &Spruce_gptpGlobalRegSet;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &Spruce_gptp1588v2ConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &Spruce_gptp8021asConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &Spruce_gptp8021asRevCfgSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &Spruce_gptpPreemptTsSelSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &Spruce_gptpPreemptTsSelGet;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &Spruce_gptpPreemptCtrlSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &Spruce_gptpPreemptCtrlGet;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Spruce_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Spruce_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Spruce_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Spruce_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Spruce_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Spruce_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Spruce_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Spruce_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Spruce_gprtSetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Spruce_gprtGetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Spruce_gprtSetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Spruce_gprtGetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Spruce_gprtSetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Spruce_gprtGetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Spruce_gprtSetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Spruce_gprtGetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Spruce_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Spruce_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Spruce_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Spruce_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Spruce_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Spruce_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Spruce_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Spruce_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Spruce_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Spruce_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Spruce_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Spruce_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Spruce_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Spruce_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Spruce_gprtGetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Spruce_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Spruce_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Spruce_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Spruce_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Spruce_gvlnSetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Spruce_gvlnGetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Spruce_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Spruce_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Spruce_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Spruce_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Spruce_gprtSetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Spruce_gprtGetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Spruce_gprtSetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Spruce_gprtGetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Spruce_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Spruce_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Spruce_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Spruce_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Spruce_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Spruce_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Spruce_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Spruce_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Spruce_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Spruce_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &Spruce_gprtSetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &Spruce_gprtGetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Spruce_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Spruce_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Spruce_gprtSetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Spruce_gprtGetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Spruce_gprtSetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Spruce_gprtGetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Spruce_gprtSetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Spruce_gprtGetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Spruce_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Spruce_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Spruce_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Spruce_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Spruce_gprtSetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Spruce_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Spruce_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Spruce_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Spruce_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Spruce_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Spruce_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Spruce_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Spruce_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Spruce_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Spruce_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Spruce_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Spruce_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Spruce_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Spruce_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Spruce_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Spruce_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Spruce_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Spruce_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Spruce_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Spruce_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Spruce_gprtSetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Spruce_gprtGetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Spruce_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Spruce_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Spruce_gprtSetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Spruce_gprtGetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Spruce_gprtSetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Spruce_gprtGetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Spruce_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Spruce_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Spruce_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Spruce_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &Spruce_gprtSetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &Spruce_gprtGetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &Spruce_gprtSetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &Spruce_gprtGetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &Spruce_gprtSetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &Spruce_gprtGetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Spruce_gprtRegDump;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Spruce_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Spruce_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Spruce_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Spruce_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Spruce_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Spruce_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Spruce_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Spruce_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Spruce_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Spruce_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Spruce_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Spruce_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &Spruce_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &Spruce_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Spruce_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Spruce_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Spruce_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Spruce_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Spruce_gvtuDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Spruce_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Spruce_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Spruce_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Spruce_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Spruce_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Spruce_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Spruce_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Spruce_gsysGetTrunkRouting;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Spruce_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Spruce_EEPROMReadIntf;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Spruce_EEPROMChipSelSetIntf;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Spruce_EEPROMChipSelGetIntf;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Spruce_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Spruce_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Spruce_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Spruce_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Spruce_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Spruce_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Spruce_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Spruce_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Spruce_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Spruce_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Spruce_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Spruce_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Spruce_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Spruce_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Spruce_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Spruce_gprtGetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Spruce_gprtSetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Spruce_gqosGetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Spruce_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Spruce_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Spruce_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Spruce_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Spruce_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Spruce_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Spruce_gqosSetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Spruce_gprtRewriteDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Spruce_gprtSetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Spruce_gprtGetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Spruce_gprtSetCosToDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Spruce_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Spruce_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Spruce_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Spruce_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Spruce_gstatsGetPortAllCntersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Spruce_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Spruce_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Spruce_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Spruce_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Spruce_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Spruce_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Spruce_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &Spruce_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &Spruce_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Spruce_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Spruce_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Spruce_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Spruce_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Spruce_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Spruce_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Spruce_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Spruce_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Spruce_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Spruce_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Spruce_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Spruce_gsysQbvRead;

	dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Spruce_gBpeEnableSet;
	dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Spruce_gBpeEnableGet;
	dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Spruce_gecidAddEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidDump = &Spruce_gecidDump;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Spruce_gecidFindEcidEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Spruce_gecidFlushAll;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Spruce_gecidFlushEntry;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Spruce_gecidGetEcidEntryNextIntf;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Spruce_gecidGetEntryCount;
	dev->SwitchDevObj.ECIDObj.gecidMove = &Spruce_gecidMove;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Spruce_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Spruce_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Spruce_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Spruce_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Spruce_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Spruce_gtcamFindEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Spruce_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Spruce_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Spruce_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Spruce_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Spruce_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Spruce_gtcamEgrFlushAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Spruce_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Spruce_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Spruce_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &Spruce_gtcamDump;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Spruce_gtcamAdvConfigIntf;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Spruce_gtcamRCLoadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Spruce_gtcamRCFlushAllEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Spruce_gtcamRCFlushEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Spruce_gtcamRCReadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Spruce_gtcamRCGetNextEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Spruce_gtcamRCFindEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Spruce_gtcamRCEntryDump;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Spruce_gtcamRCCompLoadIntf;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Spruce_gtcamRCCompFlushAll;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Spruce_gtcamRCCompFlushPort;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Spruce_gtcamRCCompReadIntf;

	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Spruce_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Spruce_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Spruce_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = &Spruce_msdRmuEcidDumpIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &Spruce_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Spruce_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Spruce_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Spruce_msdRmuMultiRegAccessIntf;
	dev->SwitchDevObj.RMUObj.grmuRegDump = &Spruce_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Spruce_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Spruce_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Spruce_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Spruce_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Spruce_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Spruce_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Spruce_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Spruce_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Spruce_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Spruce_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Spruce_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Spruce_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &Spruce_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Spruce_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Spruce_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Spruce_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Spruce_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Spruce_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Spruce_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Spruce_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Spruce_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Spruce_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Spruce_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Spruce_gledDump;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Spruce_gpolicySetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Spruce_gpolicyGetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Spruce_gpolicySetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Spruce_gpolicyGetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Spruce_gpolicySetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Spruce_gpolicyGetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Spruce_gpolicySetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Spruce_gpolicyGetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Spruce_gpolicySetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Spruce_gpolicyGetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Spruce_gpolicySetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Spruce_gpolicyGetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Spruce_gpolicySetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Spruce_gpolicyGetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Spruce_gpolicySetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Spruce_gpolicyGetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Spruce_gpolicySetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Spruce_gpolicyGetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Spruce_gpolicySetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Spruce_gpolicyGetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Spruce_gpolicySetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Spruce_gpolicyGetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Spruce_gpolicySetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Spruce_gpolicyGetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Spruce_gprtSetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Spruce_gprtGetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Spruce_gpolicySetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Spruce_gpolicyGetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Spruce_gpolicySetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Spruce_gpolicyGetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Spruce_gpolicySetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Spruce_gpolicyGetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Spruce_gpolicySetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Spruce_gpolicyGetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Spruce_gpolicySetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Spruce_gpolicyGetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Spruce_gprtSetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Spruce_gprtGetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Spruce_gpolicySetICMPSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Spruce_gpolicyGetICMPSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Spruce_gprtSetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Spruce_gprtGetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Spruce_gpolicySetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Spruce_gpolicyGetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Spruce_gpolicySetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Spruce_gpolicyGetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Spruce_gpolicySetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Spruce_gpolicyGetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Spruce_gpolicySetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Spruce_gpolicyGetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Spruce_gpolicySetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Spruce_gpolicyGetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Spruce_gpolicySetETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Spruce_gpolicyGetETag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Spruce_gpolicySetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Spruce_gpolicyGetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = &Spruce_gpolicyDump;

#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_BonsaiZ1(MSD_QD_DEV *dev)
{
#ifdef BONSAIZ1_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &BonsaiZ1_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &BonsaiZ1_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &BonsaiZ1_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &BonsaiZ1_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &BonsaiZ1_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &BonsaiZ1_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &BonsaiZ1_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &BonsaiZ1_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &BonsaiZ1_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &BonsaiZ1_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &BonsaiZ1_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &BonsaiZ1_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &BonsaiZ1_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &BonsaiZ1_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &BonsaiZ1_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &BonsaiZ1_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &BonsaiZ1_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &BonsaiZ1_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &BonsaiZ1_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &BonsaiZ1_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &BonsaiZ1_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &BonsaiZ1_gfdbGetNStaticEntryCountFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &BonsaiZ1_gfdbDump;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &BonsaiZ1_garpLoadMcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &BonsaiZ1_garpLoadUcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = &BonsaiZ1_garpFlushEntry;
	dev->SwitchDevObj.ARPObj.garpFlushAll = &BonsaiZ1_garpFlushAll;
	dev->SwitchDevObj.ARPObj.garpReadEntry = &BonsaiZ1_garpReadEntryIntf;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = &BonsaiZ1_garpGetNextEntryIntf;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &BonsaiZ1_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &BonsaiZ1_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &BonsaiZ1_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &BonsaiZ1_gqavSetPortQpriXRate;

	dev->SwitchDevObj.PHYCTRLObj.serdesReset = &BonsaiZ1_serdesReset;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &BonsaiZ1_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &BonsaiZ1_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &BonsaiZ1_gptpSetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &BonsaiZ1_gptpGetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &BonsaiZ1_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &BonsaiZ1_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &BonsaiZ1_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &BonsaiZ1_gptpGetIntStatusIntf;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &BonsaiZ1_gptpSetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &BonsaiZ1_gptpGetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &BonsaiZ1_gptpSetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &BonsaiZ1_gptpGetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &BonsaiZ1_gptpSetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &BonsaiZ1_gptpGetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &BonsaiZ1_gptpGetTODBusyBitStatus;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &BonsaiZ1_gptpTODStoreCompenIntf;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &BonsaiZ1_gptpTODStoreAllIntf;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &BonsaiZ1_gptpTODCaptureAllIntf;
	dev->SwitchDevObj.PTPObj.gptpSetPulse = &BonsaiZ1_gptpSetPulseIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = &BonsaiZ1_gptpGetPulseIntf;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &BonsaiZ1_gtaiGetPTPGlobalTime;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = &BonsaiZ1_gptpPortRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = &BonsaiZ1_gptpPortRegSet;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &BonsaiZ1_gptpGlobalRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &BonsaiZ1_gptpGlobalRegSet;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &BonsaiZ1_gptp1588v2ConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &BonsaiZ1_gptp8021asConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &BonsaiZ1_gptp8021asRevCfgSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &BonsaiZ1_gptpPreemptTsSelSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &BonsaiZ1_gptpPreemptTsSelGet;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &BonsaiZ1_gptpPreemptCtrlSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &BonsaiZ1_gptpPreemptCtrlGet;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &BonsaiZ1_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &BonsaiZ1_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &BonsaiZ1_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &BonsaiZ1_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &BonsaiZ1_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &BonsaiZ1_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &BonsaiZ1_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &BonsaiZ1_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &BonsaiZ1_gprtSetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &BonsaiZ1_gprtGetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &BonsaiZ1_gprtSetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &BonsaiZ1_gprtGetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &BonsaiZ1_gprtSetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &BonsaiZ1_gprtGetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &BonsaiZ1_gprtSetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &BonsaiZ1_gprtGetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &BonsaiZ1_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &BonsaiZ1_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &BonsaiZ1_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &BonsaiZ1_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &BonsaiZ1_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &BonsaiZ1_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &BonsaiZ1_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &BonsaiZ1_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &BonsaiZ1_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &BonsaiZ1_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &BonsaiZ1_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &BonsaiZ1_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &BonsaiZ1_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &BonsaiZ1_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &BonsaiZ1_gprtGetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &BonsaiZ1_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &BonsaiZ1_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &BonsaiZ1_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &BonsaiZ1_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &BonsaiZ1_gvlnSetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &BonsaiZ1_gvlnGetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &BonsaiZ1_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &BonsaiZ1_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &BonsaiZ1_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &BonsaiZ1_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &BonsaiZ1_gprtSetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &BonsaiZ1_gprtGetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &BonsaiZ1_gprtSetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &BonsaiZ1_gprtGetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &BonsaiZ1_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &BonsaiZ1_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &BonsaiZ1_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &BonsaiZ1_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &BonsaiZ1_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &BonsaiZ1_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &BonsaiZ1_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &BonsaiZ1_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &BonsaiZ1_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &BonsaiZ1_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &BonsaiZ1_gprtSetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &BonsaiZ1_gprtGetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &BonsaiZ1_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &BonsaiZ1_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &BonsaiZ1_gprtSetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &BonsaiZ1_gprtGetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &BonsaiZ1_gprtSetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &BonsaiZ1_gprtGetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &BonsaiZ1_gprtSetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &BonsaiZ1_gprtGetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &BonsaiZ1_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &BonsaiZ1_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &BonsaiZ1_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &BonsaiZ1_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &BonsaiZ1_gprtSetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &BonsaiZ1_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &BonsaiZ1_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &BonsaiZ1_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &BonsaiZ1_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &BonsaiZ1_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &BonsaiZ1_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &BonsaiZ1_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &BonsaiZ1_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &BonsaiZ1_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &BonsaiZ1_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &BonsaiZ1_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &BonsaiZ1_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &BonsaiZ1_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &BonsaiZ1_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &BonsaiZ1_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &BonsaiZ1_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &BonsaiZ1_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &BonsaiZ1_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &BonsaiZ1_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &BonsaiZ1_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &BonsaiZ1_gprtSetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &BonsaiZ1_gprtGetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &BonsaiZ1_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &BonsaiZ1_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &BonsaiZ1_gprtSetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &BonsaiZ1_gprtGetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &BonsaiZ1_gprtSetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &BonsaiZ1_gprtGetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &BonsaiZ1_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &BonsaiZ1_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &BonsaiZ1_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &BonsaiZ1_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &BonsaiZ1_gprtSetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &BonsaiZ1_gprtGetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &BonsaiZ1_gprtSetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &BonsaiZ1_gprtGetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &BonsaiZ1_gprtSetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &BonsaiZ1_gprtGetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &BonsaiZ1_gprtRegDump;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &BonsaiZ1_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &BonsaiZ1_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &BonsaiZ1_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &BonsaiZ1_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &BonsaiZ1_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &BonsaiZ1_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &BonsaiZ1_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &BonsaiZ1_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &BonsaiZ1_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &BonsaiZ1_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &BonsaiZ1_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &BonsaiZ1_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &BonsaiZ1_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &BonsaiZ1_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &BonsaiZ1_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &BonsaiZ1_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &BonsaiZ1_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &BonsaiZ1_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &BonsaiZ1_gvtuDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &BonsaiZ1_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &BonsaiZ1_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &BonsaiZ1_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &BonsaiZ1_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &BonsaiZ1_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &BonsaiZ1_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &BonsaiZ1_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &BonsaiZ1_gsysGetTrunkRouting;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &BonsaiZ1_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &BonsaiZ1_EEPROMReadIntf;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &BonsaiZ1_EEPROMChipSelSetIntf;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &BonsaiZ1_EEPROMChipSelGetIntf;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &BonsaiZ1_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &BonsaiZ1_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &BonsaiZ1_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &BonsaiZ1_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &BonsaiZ1_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &BonsaiZ1_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &BonsaiZ1_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &BonsaiZ1_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &BonsaiZ1_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &BonsaiZ1_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &BonsaiZ1_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &BonsaiZ1_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &BonsaiZ1_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &BonsaiZ1_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &BonsaiZ1_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &BonsaiZ1_gprtGetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &BonsaiZ1_gprtSetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &BonsaiZ1_gqosGetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &BonsaiZ1_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &BonsaiZ1_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &BonsaiZ1_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &BonsaiZ1_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &BonsaiZ1_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &BonsaiZ1_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &BonsaiZ1_gqosSetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &BonsaiZ1_gprtRewriteDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &BonsaiZ1_gprtSetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &BonsaiZ1_gprtGetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &BonsaiZ1_gprtSetCosToDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &BonsaiZ1_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &BonsaiZ1_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &BonsaiZ1_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &BonsaiZ1_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &BonsaiZ1_gstatsGetPortAllCntersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &BonsaiZ1_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &BonsaiZ1_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &BonsaiZ1_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &BonsaiZ1_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &BonsaiZ1_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &BonsaiZ1_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &BonsaiZ1_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = &BonsaiZ1_gsysSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = &BonsaiZ1_gsysGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &BonsaiZ1_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &BonsaiZ1_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &BonsaiZ1_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &BonsaiZ1_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &BonsaiZ1_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &BonsaiZ1_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &BonsaiZ1_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &BonsaiZ1_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &BonsaiZ1_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &BonsaiZ1_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &BonsaiZ1_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &BonsaiZ1_gsysQbvRead;

	dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &BonsaiZ1_gBpeEnableSet;
	dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &BonsaiZ1_gBpeEnableGet;
	dev->SwitchDevObj.ECIDObj.gecidAddEntry = &BonsaiZ1_gecidAddEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidDump = &BonsaiZ1_gecidDump;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &BonsaiZ1_gecidFindEcidEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = &BonsaiZ1_gecidFlushAll;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &BonsaiZ1_gecidFlushEntry;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &BonsaiZ1_gecidGetEcidEntryNextIntf;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &BonsaiZ1_gecidGetEntryCount;
	dev->SwitchDevObj.ECIDObj.gecidMove = &BonsaiZ1_gecidMove;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &BonsaiZ1_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &BonsaiZ1_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &BonsaiZ1_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &BonsaiZ1_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &BonsaiZ1_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &BonsaiZ1_gtcamFindEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &BonsaiZ1_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &BonsaiZ1_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &BonsaiZ1_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &BonsaiZ1_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &BonsaiZ1_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &BonsaiZ1_gtcamEgrFlushAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &BonsaiZ1_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &BonsaiZ1_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &BonsaiZ1_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &BonsaiZ1_gtcamDump;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &BonsaiZ1_gtcamAdvConfigIntf;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &BonsaiZ1_gtcamRCLoadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &BonsaiZ1_gtcamRCFlushAllEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &BonsaiZ1_gtcamRCFlushEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &BonsaiZ1_gtcamRCReadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &BonsaiZ1_gtcamRCGetNextEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &BonsaiZ1_gtcamRCFindEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &BonsaiZ1_gtcamRCEntryDump;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &BonsaiZ1_gtcamRCCompLoadIntf;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &BonsaiZ1_gtcamRCCompFlushAll;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &BonsaiZ1_gtcamRCCompFlushPort;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &BonsaiZ1_gtcamRCCompReadIntf;

	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &BonsaiZ1_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &BonsaiZ1_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &BonsaiZ1_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = &BonsaiZ1_msdRmuEcidDumpIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &BonsaiZ1_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &BonsaiZ1_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &BonsaiZ1_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &BonsaiZ1_msdRmuMultiRegAccessIntf;
	dev->SwitchDevObj.RMUObj.grmuRegDump = &BonsaiZ1_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &BonsaiZ1_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &BonsaiZ1_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &BonsaiZ1_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &BonsaiZ1_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &BonsaiZ1_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &BonsaiZ1_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &BonsaiZ1_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &BonsaiZ1_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &BonsaiZ1_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &BonsaiZ1_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &BonsaiZ1_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &BonsaiZ1_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &BonsaiZ1_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &BonsaiZ1_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &BonsaiZ1_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &BonsaiZ1_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &BonsaiZ1_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &BonsaiZ1_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &BonsaiZ1_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &BonsaiZ1_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &BonsaiZ1_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &BonsaiZ1_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &BonsaiZ1_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &BonsaiZ1_gledDump;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &BonsaiZ1_gpolicySetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &BonsaiZ1_gpolicyGetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &BonsaiZ1_gpolicySetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &BonsaiZ1_gpolicyGetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &BonsaiZ1_gpolicySetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &BonsaiZ1_gpolicyGetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &BonsaiZ1_gpolicySetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &BonsaiZ1_gpolicyGetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &BonsaiZ1_gpolicySetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &BonsaiZ1_gpolicyGetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &BonsaiZ1_gpolicySetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &BonsaiZ1_gpolicyGetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &BonsaiZ1_gpolicySetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &BonsaiZ1_gpolicyGetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &BonsaiZ1_gpolicySetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &BonsaiZ1_gpolicyGetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &BonsaiZ1_gpolicySetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &BonsaiZ1_gpolicyGetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &BonsaiZ1_gpolicySetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &BonsaiZ1_gpolicyGetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &BonsaiZ1_gpolicySetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &BonsaiZ1_gpolicyGetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &BonsaiZ1_gpolicySetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &BonsaiZ1_gpolicyGetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &BonsaiZ1_gprtSetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &BonsaiZ1_gprtGetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &BonsaiZ1_gpolicySetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &BonsaiZ1_gpolicyGetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &BonsaiZ1_gpolicySetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &BonsaiZ1_gpolicyGetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &BonsaiZ1_gpolicySetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &BonsaiZ1_gpolicyGetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &BonsaiZ1_gpolicySetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &BonsaiZ1_gpolicyGetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &BonsaiZ1_gpolicySetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &BonsaiZ1_gpolicyGetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &BonsaiZ1_gprtSetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &BonsaiZ1_gprtGetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &BonsaiZ1_gpolicySetICMPSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &BonsaiZ1_gpolicyGetICMPSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &BonsaiZ1_gprtSetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &BonsaiZ1_gprtGetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &BonsaiZ1_gpolicySetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &BonsaiZ1_gpolicyGetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &BonsaiZ1_gpolicySetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &BonsaiZ1_gpolicyGetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &BonsaiZ1_gpolicySetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &BonsaiZ1_gpolicyGetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &BonsaiZ1_gpolicySetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &BonsaiZ1_gpolicyGetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &BonsaiZ1_gpolicySetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &BonsaiZ1_gpolicyGetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = &BonsaiZ1_gpolicySetETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &BonsaiZ1_gpolicyGetETag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &BonsaiZ1_gpolicySetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &BonsaiZ1_gpolicyGetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = &BonsaiZ1_gpolicyDump;

#endif
	return MSD_OK;
}

static MSD_STATUS InitDevObj_Fir(MSD_QD_DEV *dev)
{
#ifdef FIR_BUILD_IN
    dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Fir_gfdbAddMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Fir_gfdbGetAtuEntryNextIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlush = &Fir_gfdbFlushIntf;
    dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Fir_gfdbFlushInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbMove = &Fir_gfdbMoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Fir_gfdbMoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Fir_gfdbPortRemoveIntf;
    dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Fir_gfdbPortRemoveInDBIntf;
    dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Fir_gfdbDelAtuEntry;
    dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Fir_gfdbGetViolationIntf;
    dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Fir_gfdbFindAtuMacEntryIntf;
    dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Fir_gfdbSetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Fir_gfdbGetAgingTimeout;
    dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Fir_gfdbSetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Fir_gfdbGetLearn2All;
    dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Fir_gfdbSetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Fir_gfdbGetPortLearnLimit;
    dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Fir_gfdbGetPortLearnCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Fir_gfdbGetEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Fir_gfdbGetEntryCountPerFid;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Fir_gfdbGetNonStaticEntryCount;
    dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Fir_gfdbGetNStaticEntryCountFid;
    dev->SwitchDevObj.ATUObj.gfdbDump = &Fir_gfdbDump;

    dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Fir_garpLoadMcEntryIntf;
    dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Fir_garpLoadUcEntryIntf;
    dev->SwitchDevObj.ARPObj.garpFlushEntry = &Fir_garpFlushEntry;
    dev->SwitchDevObj.ARPObj.garpFlushAll = &Fir_garpFlushAll;
    dev->SwitchDevObj.ARPObj.garpReadEntry = &Fir_garpReadEntryIntf;
    dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Fir_garpGetNextEntryIntf;

    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Fir_gqavGetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Fir_gqavSetPortQpriXHiLimit;
    dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Fir_gqavGetPortQpriXRate;
    dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Fir_gqavSetPortQpriXRate;

    dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Fir_serdesReset;

    dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Fir_gptpSetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Fir_gptpGetIntEnableIntf;
    dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Fir_gptpSetPortPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Fir_gptpGetPortPTPEn;
    dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Fir_gptpGetTimeStampIntf;
    dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Fir_gptpGetTSValidSt;
    dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Fir_gptpResetTimeStamp;
    dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Fir_gptpGetIntStatusIntf;
    dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Fir_gptpSetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Fir_gptpGetMeanPathDelay;
    dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Fir_gptpSetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Fir_gptpGetIgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Fir_gptpSetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Fir_gptpGetEgrPathDelayAsym;
    dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Fir_gptpGetTODBusyBitStatus;
    dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Fir_gptpTODStoreCompenIntf;
    dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Fir_gptpTODStoreAllIntf;
    dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Fir_gptpTODCaptureAllIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPulse = &Fir_gptpSetPulseIntf;
    dev->SwitchDevObj.PTPObj.gptpGetPulse = &Fir_gptpGetPulseIntf;
    dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Fir_gtaiGetPTPGlobalTime;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = &Fir_gptpPortRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = &Fir_gptpPortRegSet;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &Fir_gptpGlobalRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &Fir_gptpGlobalRegSet;
    dev->SwitchDevObj.PTPObj.gptpGetTaiReg = &Fir_gTaiRegGet;
    dev->SwitchDevObj.PTPObj.gptpSetTaiReg = &Fir_gTaiRegSet;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &Fir_gptp1588v2ConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &Fir_gptp8021asConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &Fir_gptp8021asRevConfigSetIntf;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &Fir_gptpPreemptTimestampSelSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &Fir_gptpPreemptTimestampSelGet;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &Fir_gptpPreemptCtrlSet;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &Fir_gptpPreemptCtrlGet;
    dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel = &Fir_gtaiTrigGenTimerSelSet;
    dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel = &Fir_gtaiTrigGenTimerSelGet;

    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Fir_gprtSetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Fir_gprtGetIgnoreFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Fir_gprtSetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Fir_gprtGetAllowBadFcs;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Fir_gprtSetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Fir_gprtGetPauseLimitOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Fir_gprtSetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Fir_gprtGetPauseLimitIn;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Fir_gprtSetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Fir_gprtGetQueueToPause;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Fir_gprtSetRxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Fir_gprtGetRxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Fir_gprtSetRxPriFlowCtrlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Fir_gprtGetRxPriFlowCtrlToQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Fir_gprtSetTxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Fir_gprtGetTxPriFlowCtrlEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Fir_gprtGetOutQueueSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Fir_gprtSetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Fir_gprtGetMapDA;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Fir_gprtSetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Fir_gprtGetPortLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Fir_gprtSetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Fir_gprtGetHoldAt1;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Fir_gprtSetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Fir_gprtGetInt0nAgeOut;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Fir_gprtSetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Fir_gprtGetRefreshLocked;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Fir_gprtSetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Fir_gprtGetIgnoreWrongData;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Fir_gprtSetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Fir_gprtGetARPMirror;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Fir_gprtSetForceDefaultVid;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Fir_gprtGetForceDefaultVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Fir_gvlnSetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Fir_gvlnGetPortVid;
    dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Fir_gvlnSetPortVlanQModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Fir_gvlnGetPortVlanQModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Fir_gprtSetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Fir_gprtGetDiscardTaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Fir_gprtSetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Fir_gprtGetDiscardUntaggedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Fir_gprtSetUnicFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Fir_gprtGetUnicFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Fir_gprtSetMultiFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Fir_gprtGetMultiFloodBlockIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Fir_gprtSetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Fir_gprtGetDiscardBCastModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Fir_gprtSetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Fir_gprtGetIGMPSnoopEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Fir_gprtSetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Fir_gprtGetHeaderMode;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Fir_gprtSetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Fir_gprtGetDropOnLock;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Fir_gprtSetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Fir_gprtGetJumboModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &Fir_gprtSetSpecifiedMTUIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &Fir_gprtGetSpecifiedMTUIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Fir_gprtSetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Fir_gprtGetLearnEnableIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Fir_gprtSetVTUPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Fir_gprtGetVTUPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Fir_gprtSetSAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Fir_gprtGetSAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Fir_gprtSetDAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Fir_gprtGetDAPriOverrideIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Fir_gprtSetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Fir_gprtGetMessagePort;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Fir_gprtSetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Fir_gprtGetEtherType;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Fir_gprtSetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Fir_gprtGetAllowVidZero;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Fir_gprtSetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Fir_gprtGetFlowCtrlIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Fir_gprtSetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Fir_gprtGetEgressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Fir_gprtSetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Fir_gprtGetIngressMonitorSource;
    dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Fir_grcSetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Fir_grcGetEgressRateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Fir_gprtGetDuplexStatusIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Fir_gprtSetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Fir_gprtGetDuplexIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Fir_gprtGetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Fir_gprtSetForceLinkIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Fir_gprtGetLinkState;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Fir_gprtSetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Fir_gprtGetForceSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Fir_gprtGetSpeedIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Fir_gprtSetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Fir_gprtGetFrameModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Fir_gprtSetPortECID;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Fir_gprtGetPortECID;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Fir_gprtSetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Fir_gprtGetEgressModeIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Fir_gprtSetIgnoreETag;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Fir_gprtGetIgnoreETag;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Fir_gprtSetExtendedPortCtrlReg;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Fir_gprtGetExtendedPortCtrlReg;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Fir_gprtSetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Fir_gprtGetVlanPorts;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Fir_gprtSetPortStateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Fir_gprtGetPortStateIntf;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &Fir_gprtSetPreemptEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &Fir_gprtGetPreemptEnable;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &Fir_gprtSetPreemptSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &Fir_gprtGetPreemptSize;
    dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &Fir_gprtSetPreemptQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &Fir_gprtGetPreemptQueue;
    dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Fir_gprtRegDump;

    dev->SwitchDevObj.STUObj.gstuAddEntry = &Fir_gstuAddEntryIntf;
    dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Fir_gstuGetEntryNextIntf;
    dev->SwitchDevObj.STUObj.gstuDelEntry = &Fir_gstuDelEntryIntf;
    dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Fir_gstuFindSidEntryIntf;
    dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Fir_gstuGetEntryCountIntf;
    dev->SwitchDevObj.STUObj.gstuDump = &Fir_gstuDump;

    dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Fir_gvtuFindVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Fir_gvtuExistVidEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuFlush = &Fir_gvtuFlushIntf;
    dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Fir_gvtuAddEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Fir_gvtuGetEntryNextIntf;
    dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Fir_gvtuDelEntryIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMode = &Fir_gvtuSetModeIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetMode = &Fir_gvtuGetModeIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Fir_gvtuGetViolationIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Fir_gvtuGetEntryCountIntf;
    dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Fir_gvtuSetMemberTagIntf;
    dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Fir_gvtuGetMemberTagIntf;
    dev->SwitchDevObj.VTUObj.gvtuDump = &Fir_gvtuDump;

    dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Fir_gprtSetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Fir_gprtGetTrunkPort;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Fir_gsysSetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Fir_gsysGetTrunkMaskTable;
    dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Fir_gsysSetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Fir_gsysGetHashTrunk;
    dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Fir_gsysSetTrunkRouting;
    dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Fir_gsysGetTrunkRouting;

    dev->SwitchDevObj.EEPROMObj.geepromWrite = &Fir_EEPROMWriteIntf;
    dev->SwitchDevObj.EEPROMObj.geepromRead = &Fir_EEPROMReadIntf;
    dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Fir_EEPROMChipSelSetIntf;
    dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Fir_EEPROMChipSelGetIntf;

    dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Fir_gsysDevIntEnableSet;
    dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Fir_gsysDevIntEnableGet;
    dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Fir_gsysDevIntStatusGet;
    dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Fir_gsysActiveIntEnableSet;
    dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Fir_gsysActiveIntEnableGet;
    dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Fir_gsysActiveIntStatusGet;

    dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Fir_gpirlInitializeIntf;
    dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Fir_gpirlInitResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Fir_gpirlCustomSetup_sr2cIntf;
    dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Fir_gpirlReadResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Fir_gpirlWriteResourceIntf;
    dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Fir_gpirlGetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Fir_gpirlSetResRegIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Fir_gpirlAdvConfigIntf;
    dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Fir_gpirlAdvConfigDisableIntf;

    dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Fir_gprtGetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Fir_gprtSetIEEEPrioMapIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Fir_gqosGetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Fir_gprtGetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Fir_gprtSetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Fir_gprtGetDefQpriIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Fir_gprtSetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Fir_gprtGetDefFpri;
    dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Fir_gprtSetInitialPriIntf;
    dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Fir_gqosSetPrioMapRuleIntf;
    dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Fir_gprtRewriteDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Fir_gprtSetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Fir_gprtGetCosToQueue;
    dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Fir_gprtSetCosToDscp;
    dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Fir_gprtGetCosToDscp;

    dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Fir_gstatsFlushAll;
    dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Fir_gstatsFlushPort;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Fir_gstatsGetPortCounterIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Fir_gstatsGetPortAllCntersIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Fir_gstatsGetHistogramModeIntf;
    dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Fir_gstatsSetHistogramModeIntf;
    dev->SwitchDevObj.PORTRMONObj.gmibDump = &Fir_gmibDump;

    dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Fir_gprtSetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Fir_gprtGetQueueCtrlIntf;
    dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Fir_gprtSetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Fir_gprtGetPortSchedIntf;
    dev->SwitchDevObj.QUEUEObj.gprtSetQoSWeight = &Fir_gprtSetQoSWeightIntf;
    dev->SwitchDevObj.QUEUEObj.gprtGetQoSWeight = &Fir_gprtGetQoSWeightIntf;

    dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Fir_gsysSetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Fir_gsysGetFloodBC;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Fir_gsysSetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Fir_gsysGetEgressMonitorDest;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Fir_gsysSetSMISetup;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Fir_gsysGetSMISetup;
    dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Fir_gsysGlobal1RegDump;
    dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Fir_gsysGlobal2RegDump;
    dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Fir_gsysSetRMUMode;
    dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Fir_gsysGetRMUMode;
    dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Fir_gsysQbvWrite;
    dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Fir_gsysQbvRead;

    dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Fir_gBpeEnableSet;
    dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Fir_gBpeEnableGet;
    dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Fir_gecidAddEntryIntf;
    dev->SwitchDevObj.ECIDObj.gecidDump = &Fir_gecidDump;
    dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Fir_gecidFindEcidEntryIntf;
    dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Fir_gecidFlushAll;
    dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Fir_gecidFlushEntry;
    dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Fir_gecidGetEcidEntryNextIntf;
    dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Fir_gecidGetEntryCount;
    dev->SwitchDevObj.ECIDObj.gecidMove = &Fir_gecidMove;

    dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Fir_gtcamFlushAll;
    dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Fir_gtcamFlushEntry;
    dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Fir_gtcamLoadEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Fir_gtcamReadTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Fir_gtcamGetNextTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Fir_gtcamFindEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Fir_gtcamSetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Fir_gtcamGetModeIntf;
    dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Fir_gtcamAddEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Fir_gtcamRemoveEntryPort;
    dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Fir_gtcamEgrFlushEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Fir_gtcamEgrFlushAllPortsIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Fir_gtcamEgrLoadEntryIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Fir_gtcamEgrGetNextTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Fir_gtcamEgrReadTCAMDataIntf;
    dev->SwitchDevObj.TCAMObj.gtcamDump = &Fir_gtcamDump;
    dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Fir_gtcamAdvConfigIntf;

    dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Fir_gtcamRCLoadEntryIntf;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Fir_gtcamRCFlushAllEntry;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Fir_gtcamRCFlushEntry;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Fir_gtcamRCReadEntryIntf;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Fir_gtcamRCGetNextEntryIntf;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Fir_gtcamRCFindEntryIntf;
    dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Fir_gtcamRCEntryDump;

    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Fir_gtcamRCCompLoadIntf;
    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Fir_gtcamRCCompFlushAll;
    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Fir_gtcamRCCompFlushPort;
    dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Fir_gtcamRCCompReadIntf;

    dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Fir_msdRmuTCAMDumpIntf;
    dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Fir_msdRmuTCAMReadIntf;
    dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Fir_msdRmuTCAMWriteIntf;
    dev->SwitchDevObj.RMUObj.grmuEcidDump = &Fir_msdRmuEcidDumpIntf;

    dev->SwitchDevObj.RMUObj.grmuGetID = &Fir_msdRmuGetIDIntf;
    dev->SwitchDevObj.RMUObj.grmuAtuDump = &Fir_msdRmuAtuDumpIntf;
    dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Fir_msdRmuMib2DumpIntf;
    dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Fir_msdRmuMultiRegAccessIntf;
    dev->SwitchDevObj.RMUObj.grmuRegDump = &Fir_msdRmuRegDump;

    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Fir_msdGetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Fir_msdSetSMIC45PhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Fir_msdGetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Fir_msdSetSMIPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Fir_msdSetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Fir_msdGetSMIPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Fir_msdGetSMIExtPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Fir_msdSetSMIExtPhyReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Fir_msdSetSMIExtPhyXMDIOReg;
    dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Fir_msdGetSMIExtPhyXMDIOReg;

    dev->SwitchDevObj.LEDObj.gledForceOn = &Fir_gledForceOnIntf;
    dev->SwitchDevObj.LEDObj.gledForceOff = &Fir_gledForceOffIntf;
    dev->SwitchDevObj.LEDObj.gledLinkAct = &Fir_gledLinkActIntf;
    dev->SwitchDevObj.LEDObj.gledModeSet = &Fir_gledModeSetIntf;
    dev->SwitchDevObj.LEDObj.gledModeGet = &Fir_gledModeGetIntf;
    dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Fir_gledBlinkRateSetIntf;
    dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Fir_gledBlinkRateGetIntf;
    dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Fir_gledPulseStretchSetIntf;
    dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Fir_gledPulseStretchGetIntf;
    dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Fir_gledSpecialModeSetIntf;
    dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Fir_gledSpecialModeGetIntf;
    dev->SwitchDevObj.LEDObj.gledCtrlSet = &Fir_gledCtrlSetIntf;
    dev->SwitchDevObj.LEDObj.gledCtrlGet = &Fir_gledCtrlGetIntf;
    dev->SwitchDevObj.LEDObj.gledDump = &Fir_gledDump;

    dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Fir_gpolicySetMgmtCtrl;
    dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Fir_gpolicyGetMgmtCtrl;
    dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Fir_gpolicySetCpuDest;
    dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Fir_gpolicyGetCpuDest;
    dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Fir_gpolicySetIgrMonitorDest;
    dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Fir_gpolicyGetIgrMonitorDest;
    dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Fir_gpolicySetRsvd2CpuEnables;
    dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Fir_gpolicyGetRsvd2CpuEnables;
    dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Fir_gpolicySetMGMTPri;
    dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Fir_gpolicyGetMGMTPri;
    dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Fir_gpolicySetRouteDest;
    dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Fir_gpolicyGetRouteDest;
    dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Fir_gpolicySetMirrorDest;
    dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Fir_gpolicyGetMirrorDest;
    dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Fir_gpolicySetIllegalIPAddr;
    dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Fir_gpolicyGetIllegalIPAddr;
    dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Fir_gpolicySetDipDaMismatch;
    dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Fir_gpolicyGetDipDaMismatch;
    dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Fir_gpolicySetZeroSIP;
    dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Fir_gpolicyGetZeroSIP;
    dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Fir_gpolicySetMcTtlExceeded;
    dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Fir_gpolicyGetMcTtlExceeded;
    dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Fir_gpolicySetUcTtlExceeded;
    dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Fir_gpolicyGetUcTtlExceeded;
    dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Fir_gprtSetPortTtlThreshold;
    dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Fir_gprtGetPortTtlThreshold;
    dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Fir_gpolicySetIpHdrErr;
    dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Fir_gpolicyGetIpHdrErr;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Fir_gpolicySetTCPFinUrgPsh;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Fir_gpolicyGetTCPFinUrgPsh;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Fir_gpolicySetTCPFlagsSeqNum;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Fir_gpolicyGetTCPFlagsSeqNum;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Fir_gpolicySetTCPSYN;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Fir_gpolicyGetTCPSYN;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Fir_gpolicySetTCPHSize;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Fir_gpolicyGetTCPHSize;
    dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Fir_gprtSetPortTCPMinHdrSize;
    dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Fir_gprtGetPortTCPMinHdrSize;
    dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Fir_gpolicySetICMPSize;
    dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Fir_gpolicyGetICMPSize;
    dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Fir_gprtSetPortICMPMaxSize;
    dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Fir_gprtGetPortICMPMaxSize;
    dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Fir_gpolicySetUDPPort;
    dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Fir_gpolicyGetUDPPort;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Fir_gpolicySetTCPPort;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Fir_gpolicyGetTCPPort;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Fir_gpolicySetTCPSynFin;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Fir_gpolicyGetTCPSynFin;
    dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Fir_gpolicySetTCPFrag;
    dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Fir_gpolicyGetTCPFrag;
    dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Fir_gpolicySetNearNonTPMR;
    dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Fir_gpolicyGetNearNonTPMR;
    dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Fir_gpolicySetETag;
    dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Fir_gpolicyGetETag;
    dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Fir_gpolicySetNonETag;
    dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Fir_gpolicyGetNonETag;
    dev->SwitchDevObj.POLICYObj.gpolicyDump = &Fir_gpolicyDump;

    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll = &Fir_gfrerSeqGenFlushAll;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry = &Fir_gfrerSeqGenFlushEntry;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenRead = &Fir_gfrerSeqGenReadIntf;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad = &Fir_gfrerSeqGenLoadIntf;

    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll = &Fir_gfrerIndvRcvyFlushAll;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry = &Fir_gfrerIndvRcvyFlushEntry;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead = &Fir_gfrerIndvRcvyReadIntf;
	dev->SwitchDevObj.FRERObj.gfrerIndvRcvyLoad = &Fir_gfrerIndvRcvyLoadIntf;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext = &Fir_gfrerIndvRcvyGetNextIntf;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear = &Fir_gfrerIndvRcvyReadClearIntf;

    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll = &Fir_gfrerSeqRcvyFlushAll;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry = &Fir_gfrerSeqRcvyFlushEntry;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead = &Fir_gfrerSeqRcvyReadIntf;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad = &Fir_gfrerSeqRcvyLoadIntf;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext = &Fir_gfrerSeqRcvyGetNextIntf;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation = &Fir_gfrerSeqRcvyGetViolationIntf;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear = &Fir_gfrerSeqRcvyReadClearIntf;

    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll = &Fir_gfrerBank0SCtrFlushAll;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry = &Fir_gfrerBank0SCtrFlushCtr;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead = &Fir_gfrerBank0SCtrReadIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad = &Fir_gfrerBank0SCtrLoadIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext = &Fir_gfrerBank0SCtrGetNextIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear = &Fir_gfrerBank0SCtrReadClearIntf;

    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll = &Fir_gfrerBank1SCtrFlushAll;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry = &Fir_gfrerBank1SCtrFlushCtr;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead = &Fir_gfrerBank1SCtrReadIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad = &Fir_gfrerBank1SCtrLoadIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext = &Fir_gfrerBank1SCtrGetNextIntf;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear = &Fir_gfrerBank1SCtrReadClearIntf;

    dev->SwitchDevObj.QCRObj.gqcrFlushAll = &Fir_gatsFlushAll;
    dev->SwitchDevObj.QCRObj.gqcrFlushOne = &Fir_gatsFlushOne;
    dev->SwitchDevObj.QCRObj.gqcrSetGrpId = &Fir_gatsSetGrpId;
    dev->SwitchDevObj.QCRObj.gqcrGetGrpId = &Fir_gatsGetGrpId;
    dev->SwitchDevObj.QCRObj.gqcrSetCbsLimit = &Fir_gatsSetCbsLimit;
    dev->SwitchDevObj.QCRObj.gqcrGetCbsLimit = &Fir_gatsGetCbsLimit;
    dev->SwitchDevObj.QCRObj.gqcrSetMaxResidenceTime = &Fir_gatsSetMaxResidenceTime;
    dev->SwitchDevObj.QCRObj.gqcrGetMaxResidenceTime = &Fir_gatsGetMaxResidenceTime;
    dev->SwitchDevObj.QCRObj.gqcrSetRateLimit = &Fir_gatsSetRateLimit;
    dev->SwitchDevObj.QCRObj.gqcrGetRateLimit = &Fir_gatsGetRateLimit;
    dev->SwitchDevObj.QCRObj.gqcrSetQueueEnables = &Fir_gatsSetQueueEnables;
    dev->SwitchDevObj.QCRObj.gqcrGetQueueEnables = &Fir_gatsGetQueueEnables;

	dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll = &Fir_gqciStreamFiltFlushAll;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltSet = &Fir_gqciStreamFiltSet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet = &Fir_gqciStreamFiltBlockStatSet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet = &Fir_gqciStreamFiltBlockStatGet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts = &Fir_gqciStreamFiltEnAts;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate = &Fir_gqciStreamFiltEnGate;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet = &Fir_gqciStreamFiltMatchCtrGet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet = &Fir_gqciStreamFiltFilterCtrGet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet = &Fir_gqciStreamGateFiltCtrGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet = &Fir_gqciFlowMeterFiltCtrGet;

	dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll = &Fir_gqciStreamGateFlushAll;
	dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet = &Fir_gqciStreamGateDefStateSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet = &Fir_gqciStreamGateScaleSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet = &Fir_gqciStreamGateTimeGenIdSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd = &Fir_gqciStreamGateEntryAddIntf;

	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush = &Fir_gqciStreamGateTimeGenFlush;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet = &Fir_gqciStreamGateTimeGenSet;

	dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll = &Fir_gqciFlowMeterFlushAll;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet = &Fir_gqciFlowMeterCoupleFlagSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet = &Fir_gqciFlowMeterCoupleFlagGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet = &Fir_gqciFlowMeterDropOnYelSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet = &Fir_gqciFlowMeterDropOnYelGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet = &Fir_gqciFlowMeterRedBlockEnSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet = &Fir_gqciFlowMeterRedBlockEnGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet = &Fir_gqciFlowMeterRedBlkStatSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet = &Fir_gqciFlowMeterRedBlkStatGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig = &Fir_gqciFlowMeterAdvConfig;

	dev->SwitchDevObj.MACSECObj.gmacsecFlushAll = &Fir_gmacsecFlushAll;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRule = &Fir_gmacsecCreateRuleIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule = &Fir_gmacsecDeleteRule;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRule = &Fir_gmacsecGetRuleIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY = &Fir_gmacsecCreateTxSecYIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY = &Fir_gmacsecDeleteTxSecY;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY = &Fir_gmacsecGetTxSecYIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY = &Fir_gmacsecCreateRxSecYIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY = &Fir_gmacsecDeleteRxSecY;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY = &Fir_gmacsecGetRxSecYIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC = &Fir_gmacsecCreateRxSCIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC = &Fir_gmacsecDeleteRxSC;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC = &Fir_gmacsecGetRxSCIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA = &Fir_gmacsecCreateRxSAIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA = &Fir_gmacsecDeleteRxSA;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA = &Fir_gmacsecGetRxSAIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC = &Fir_gmacsecCreateTxSCIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC = &Fir_gmacsecDeleteTxSC;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC = &Fir_gmacsecGetTxSCIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA = &Fir_gmacsecCreateTxSAIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA = &Fir_gmacsecDeleteTxSA;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA = &Fir_gmacsecGetTxSAIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates = &Fir_gmacsecGetRXSCStatesIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates = &Fir_gmacsecGetRXSecYStatesIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates = &Fir_gmacsecGetTXSCStatesIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates = &Fir_gmacsecGetTXSecYStatesIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet = &Fir_gmacsecEnableEgressSet;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet = &Fir_gmacsecEnableEgressGet;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet = &Fir_gmacsecEnableIngressSet;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet = &Fir_gmacsecEnableIngressGet;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold = &Fir_gmacsecSetTxPNThreshold;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold = &Fir_gmacsecGetTxPNThreshold;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold = &Fir_gmacsecSetTxXPNThreshold;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold = &Fir_gmacsecGetTxXPNThreshold;
	dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI = &Fir_gmacsecSetRxDefaultSCI;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI = &Fir_gmacsecGetRxDefaultSCI;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl = &Fir_gmacsecSetTxSCIntCtrlIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl = &Fir_gmacsecGetTxSCIntCtrlIntf;
	dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus = &Fir_gmacsecGetNextTxSCIntStatusIntf;

#endif
    return MSD_OK;
}

static MSD_STATUS InitDevObj_Bonsai(MSD_QD_DEV *dev)
{
#ifdef BONSAI_BUILD_IN
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = &Bonsai_gfdbAddMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = &Bonsai_gfdbGetAtuEntryNextIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlush = &Bonsai_gfdbFlushIntf;
	dev->SwitchDevObj.ATUObj.gfdbFlushInDB = &Bonsai_gfdbFlushInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbMove = &Bonsai_gfdbMoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = &Bonsai_gfdbMoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = &Bonsai_gfdbPortRemoveIntf;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = &Bonsai_gfdbPortRemoveInDBIntf;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = &Bonsai_gfdbDelAtuEntry;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = &Bonsai_gfdbGetViolationIntf;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = &Bonsai_gfdbFindAtuMacEntryIntf;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = &Bonsai_gfdbSetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = &Bonsai_gfdbGetAgingTimeout;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = &Bonsai_gfdbSetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = &Bonsai_gfdbGetLearn2All;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = &Bonsai_gfdbSetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = &Bonsai_gfdbGetPortLearnLimit;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = &Bonsai_gfdbGetPortLearnCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = &Bonsai_gfdbGetEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = &Bonsai_gfdbGetEntryCountPerFid;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = &Bonsai_gfdbGetNonStaticEntryCount;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = &Bonsai_gfdbGetNStaticEntryCountFid;
	dev->SwitchDevObj.ATUObj.gfdbDump = &Bonsai_gfdbDump;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = &Bonsai_garpLoadMcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = &Bonsai_garpLoadUcEntryIntf;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = &Bonsai_garpFlushEntry;
	dev->SwitchDevObj.ARPObj.garpFlushAll = &Bonsai_garpFlushAll;
	dev->SwitchDevObj.ARPObj.garpReadEntry = &Bonsai_garpReadEntryIntf;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = &Bonsai_garpGetNextEntryIntf;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = &Bonsai_gqavGetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = &Bonsai_gqavSetPortQpriXHiLimit;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = &Bonsai_gqavGetPortQpriXRate;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = &Bonsai_gqavSetPortQpriXRate;

	dev->SwitchDevObj.PHYCTRLObj.serdesReset = &Bonsai_serdesReset;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = &Bonsai_gptpSetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = &Bonsai_gptpGetIntEnableIntf;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = &Bonsai_gptpSetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = &Bonsai_gptpGetPortPTPEn;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = &Bonsai_gptpGetTimeStampIntf;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = &Bonsai_gptpGetTSValidSt;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = &Bonsai_gptpResetTimeStamp;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = &Bonsai_gptpGetIntStatusIntf;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = &Bonsai_gptpSetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = &Bonsai_gptpGetMeanPathDelay;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = &Bonsai_gptpSetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = &Bonsai_gptpGetIgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = &Bonsai_gptpSetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = &Bonsai_gptpGetEgrPathDelayAsym;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = &Bonsai_gptpGetTODBusyBitStatus;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = &Bonsai_gptpTODStoreCompenIntf;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = &Bonsai_gptpTODStoreAllIntf;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = &Bonsai_gptpTODCaptureAllIntf;
	dev->SwitchDevObj.PTPObj.gptpSetPulse = &Bonsai_gptpSetPulseIntf;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = &Bonsai_gptpGetPulseIntf;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = &Bonsai_gtaiGetPTPGlobalTime;
	dev->SwitchDevObj.PTPObj.gptpGetPortReg = &Bonsai_gptpPortRegGet;
	dev->SwitchDevObj.PTPObj.gptpSetPortReg = &Bonsai_gptpPortRegSet;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = &Bonsai_gptpGlobalRegGet;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = &Bonsai_gptpGlobalRegSet;
	dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = &Bonsai_gptp1588v2ConfigSetIntf;
	dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = &Bonsai_gptp8021asConfigSetIntf;
	dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = &Bonsai_gptp8021asRevCfgSetIntf;
	dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = &Bonsai_gptpPreemptTsSelSet;
	dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = &Bonsai_gptpPreemptTsSelGet;
	dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = &Bonsai_gptpPreemptCtrlSet;
	dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = &Bonsai_gptpPreemptCtrlGet;
	dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel = &Bonsai_gtaiTrigGenTimerSelSet;
	dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel = &Bonsai_gtaiTrigGenTimerSelGet;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = &Bonsai_gprtSetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = &Bonsai_gprtGetIgnoreFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = &Bonsai_gprtSetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = &Bonsai_gprtGetAllowBadFcs;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = &Bonsai_gprtSetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = &Bonsai_gprtGetPauseLimitOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = &Bonsai_gprtSetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = &Bonsai_gprtGetPauseLimitIn;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = &Bonsai_gprtSetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = &Bonsai_gprtGetQueueToPause;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = &Bonsai_gprtSetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = &Bonsai_gprtGetRxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = &Bonsai_gprtSetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = &Bonsai_gprtGetRxPriFlowCtrlToQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = &Bonsai_gprtSetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = &Bonsai_gprtGetTxPriFlowCtrlEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = &Bonsai_gprtGetOutQueueSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = &Bonsai_gprtSetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = &Bonsai_gprtGetMapDA;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = &Bonsai_gprtSetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = &Bonsai_gprtGetPortLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = &Bonsai_gprtSetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = &Bonsai_gprtGetHoldAt1;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = &Bonsai_gprtSetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = &Bonsai_gprtGetInt0nAgeOut;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = &Bonsai_gprtSetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = &Bonsai_gprtGetRefreshLocked;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = &Bonsai_gprtSetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = &Bonsai_gprtGetIgnoreWrongData;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = &Bonsai_gprtSetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = &Bonsai_gprtGetARPMirror;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = &Bonsai_gprtSetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = &Bonsai_gprtGetForceDefaultVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = &Bonsai_gvlnSetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = &Bonsai_gvlnGetPortVid;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = &Bonsai_gvlnSetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = &Bonsai_gvlnGetPortVlanQModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = &Bonsai_gprtSetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = &Bonsai_gprtGetDiscardTaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = &Bonsai_gprtSetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = &Bonsai_gprtGetDiscardUntaggedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = &Bonsai_gprtSetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = &Bonsai_gprtGetUnicFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = &Bonsai_gprtSetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = &Bonsai_gprtGetMultiFloodBlockIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = &Bonsai_gprtSetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = &Bonsai_gprtGetDiscardBCastModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = &Bonsai_gprtSetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = &Bonsai_gprtGetIGMPSnoopEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = &Bonsai_gprtSetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = &Bonsai_gprtGetHeaderMode;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = &Bonsai_gprtSetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = &Bonsai_gprtGetDropOnLock;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = &Bonsai_gprtSetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = &Bonsai_gprtGetJumboModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = &Bonsai_gprtSetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = &Bonsai_gprtGetSpecifiedMTUIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = &Bonsai_gprtSetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = &Bonsai_gprtGetLearnEnableIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = &Bonsai_gprtSetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = &Bonsai_gprtGetVTUPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = &Bonsai_gprtSetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = &Bonsai_gprtGetSAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = &Bonsai_gprtSetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = &Bonsai_gprtGetDAPriOverrideIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = &Bonsai_gprtSetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = &Bonsai_gprtGetMessagePort;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = &Bonsai_gprtSetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = &Bonsai_gprtGetEtherType;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = &Bonsai_gprtSetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = &Bonsai_gprtGetAllowVidZero;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = &Bonsai_gprtSetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = &Bonsai_gprtGetFlowCtrlIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = &Bonsai_gprtSetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = &Bonsai_gprtGetEgressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = &Bonsai_gprtSetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = &Bonsai_gprtGetIngressMonitorSource;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = &Bonsai_grcSetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = &Bonsai_grcGetEgressRateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplexStatus = &Bonsai_gprtGetDuplexStatusIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = &Bonsai_gprtSetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = &Bonsai_gprtGetDuplexIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = &Bonsai_gprtGetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceLink = &Bonsai_gprtSetForceLinkIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = &Bonsai_gprtGetLinkState;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = &Bonsai_gprtSetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = &Bonsai_gprtGetForceSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = &Bonsai_gprtGetSpeedIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = &Bonsai_gprtSetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = &Bonsai_gprtGetFrameModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = &Bonsai_gprtSetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = &Bonsai_gprtGetPortECID;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = &Bonsai_gprtSetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = &Bonsai_gprtGetEgressModeIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = &Bonsai_gprtSetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreETag = &Bonsai_gprtGetIgnoreETag;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = &Bonsai_gprtSetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = &Bonsai_gprtGetExtendedPortCtrlReg;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = &Bonsai_gprtSetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = &Bonsai_gprtGetVlanPorts;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = &Bonsai_gprtSetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = &Bonsai_gprtGetPortStateIntf;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = &Bonsai_gprtSetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = &Bonsai_gprtGetPreemptEnable;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = &Bonsai_gprtSetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = &Bonsai_gprtGetPreemptSize;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = &Bonsai_gprtSetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = &Bonsai_gprtGetPreemptQueue;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = &Bonsai_gprtRegDump;

	dev->SwitchDevObj.STUObj.gstuAddEntry = &Bonsai_gstuAddEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = &Bonsai_gstuGetEntryNextIntf;
	dev->SwitchDevObj.STUObj.gstuDelEntry = &Bonsai_gstuDelEntryIntf;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = &Bonsai_gstuFindSidEntryIntf;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = &Bonsai_gstuGetEntryCountIntf;
	dev->SwitchDevObj.STUObj.gstuDump = &Bonsai_gstuDump;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = &Bonsai_gvtuFindVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = &Bonsai_gvtuExistVidEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuFlush = &Bonsai_gvtuFlushIntf;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = &Bonsai_gvtuAddEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = &Bonsai_gvtuGetEntryNextIntf;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = &Bonsai_gvtuDelEntryIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = &Bonsai_gvtuSetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = &Bonsai_gvtuGetModeIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = &Bonsai_gvtuGetViolationIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = &Bonsai_gvtuGetEntryCountIntf;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = &Bonsai_gvtuSetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = &Bonsai_gvtuGetMemberTagIntf;
	dev->SwitchDevObj.VTUObj.gvtuDump = &Bonsai_gvtuDump;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = &Bonsai_gprtSetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = &Bonsai_gprtGetTrunkPort;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = &Bonsai_gsysSetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = &Bonsai_gsysGetTrunkMaskTable;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = &Bonsai_gsysSetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = &Bonsai_gsysGetHashTrunk;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = &Bonsai_gsysSetTrunkRouting;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = &Bonsai_gsysGetTrunkRouting;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = &Bonsai_EEPROMWriteIntf;
	dev->SwitchDevObj.EEPROMObj.geepromRead = &Bonsai_EEPROMReadIntf;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = &Bonsai_EEPROMChipSelSetIntf;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = &Bonsai_EEPROMChipSelGetIntf;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = &Bonsai_gsysDevIntEnableSet;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = &Bonsai_gsysDevIntEnableGet;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = &Bonsai_gsysDevIntStatusGet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = &Bonsai_gsysActiveIntEnableSet;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = &Bonsai_gsysActiveIntEnableGet;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = &Bonsai_gsysActiveIntStatusGet;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = &Bonsai_gpirlInitializeIntf;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = &Bonsai_gpirlInitResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = &Bonsai_gpirlCustomSetup_sr2cIntf;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = &Bonsai_gpirlReadResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = &Bonsai_gpirlWriteResourceIntf;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = &Bonsai_gpirlGetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = &Bonsai_gpirlSetResRegIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = &Bonsai_gpirlAdvConfigIntf;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = &Bonsai_gpirlAdvConfigDisableIntf;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = &Bonsai_gprtGetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = &Bonsai_gprtSetIEEEPrioMapIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = &Bonsai_gqosGetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = &Bonsai_gprtGetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = &Bonsai_gprtSetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = &Bonsai_gprtGetDefQpriIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefFpri = &Bonsai_gprtSetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefFpri = &Bonsai_gprtGetDefFpri;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = &Bonsai_gprtSetInitialPriIntf;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = &Bonsai_gqosSetPrioMapRuleIntf;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = &Bonsai_gprtRewriteDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = &Bonsai_gprtSetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = &Bonsai_gprtGetCosToQueue;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = &Bonsai_gprtSetCosToDscp;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = &Bonsai_gprtGetCosToDscp;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = &Bonsai_gstatsFlushAll;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = &Bonsai_gstatsFlushPort;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = &Bonsai_gstatsGetPortCounterIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = &Bonsai_gstatsGetPortAllCntersIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = &Bonsai_gstatsGetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = &Bonsai_gstatsSetHistogramModeIntf;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = &Bonsai_gmibDump;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = &Bonsai_gprtSetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = &Bonsai_gprtGetQueueCtrlIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = &Bonsai_gprtSetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = &Bonsai_gprtGetPortSchedIntf;
	dev->SwitchDevObj.QUEUEObj.gprtSetQoSWeight = &Bonsai_gprtSetQoSWeightIntf;
	dev->SwitchDevObj.QUEUEObj.gprtGetQoSWeight = &Bonsai_gprtGetQoSWeightIntf;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = &Bonsai_gsysSetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = &Bonsai_gsysGetFloodBC;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = &Bonsai_gsysSetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = &Bonsai_gsysGetEgressMonitorDest;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = &Bonsai_gsysSetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = &Bonsai_gsysGetSMISetup;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = &Bonsai_gsysGlobal1RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = &Bonsai_gsysGlobal2RegDump;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = &Bonsai_gsysSetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = &Bonsai_gsysGetRMUMode;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = &Bonsai_gsysQbvWrite;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = &Bonsai_gsysQbvRead;

	dev->SwitchDevObj.ECIDObj.gBpeEnableSet = &Bonsai_gBpeEnableSet;
	dev->SwitchDevObj.ECIDObj.gBpeEnableGet = &Bonsai_gBpeEnableGet;
	dev->SwitchDevObj.ECIDObj.gecidAddEntry = &Bonsai_gecidAddEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidDump = &Bonsai_gecidDump;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = &Bonsai_gecidFindEcidEntryIntf;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = &Bonsai_gecidFlushAll;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = &Bonsai_gecidFlushEntry;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = &Bonsai_gecidGetEcidEntryNextIntf;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = &Bonsai_gecidGetEntryCount;
	dev->SwitchDevObj.ECIDObj.gecidMove = &Bonsai_gecidMove;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = &Bonsai_gtcamFlushAll;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = &Bonsai_gtcamFlushEntry;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = &Bonsai_gtcamLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = &Bonsai_gtcamReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = &Bonsai_gtcamGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = &Bonsai_gtcamFindEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = &Bonsai_gtcamSetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = &Bonsai_gtcamGetModeIntf;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = &Bonsai_gtcamAddEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = &Bonsai_gtcamRemoveEntryPort;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = &Bonsai_gtcamEgrFlushEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = &Bonsai_gtcamEgrFlushAllPortsIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = &Bonsai_gtcamEgrLoadEntryIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = &Bonsai_gtcamEgrGetNextTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = &Bonsai_gtcamEgrReadTCAMDataIntf;
	dev->SwitchDevObj.TCAMObj.gtcamDump = &Bonsai_gtcamDump;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = &Bonsai_gtcamAdvConfigIntf;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = &Bonsai_gtcamRCLoadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushAllEntry = &Bonsai_gtcamRCFlushAllEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = &Bonsai_gtcamRCFlushEntry;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = &Bonsai_gtcamRCReadEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = &Bonsai_gtcamRCGetNextEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = &Bonsai_gtcamRCFindEntryIntf;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = &Bonsai_gtcamRCEntryDump;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = &Bonsai_gtcamRCCompLoadIntf;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = &Bonsai_gtcamRCCompFlushAll;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = &Bonsai_gtcamRCCompFlushPort;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = &Bonsai_gtcamRCCompReadIntf;

	dev->SwitchDevObj.RMUObj.grmuTCAMDump = &Bonsai_msdRmuTCAMDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = &Bonsai_msdRmuTCAMReadIntf;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = &Bonsai_msdRmuTCAMWriteIntf;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = &Bonsai_msdRmuEcidDumpIntf;

	dev->SwitchDevObj.RMUObj.grmuGetID = &Bonsai_msdRmuGetIDIntf;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = &Bonsai_msdRmuAtuDumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = &Bonsai_msdRmuMib2DumpIntf;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = &Bonsai_msdRmuMultiRegAccessIntf;
	dev->SwitchDevObj.RMUObj.grmuRegDump = &Bonsai_msdRmuRegDump;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = &Bonsai_msdGetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = &Bonsai_msdSetSMIC45PhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = &Bonsai_msdGetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = &Bonsai_msdSetSMIPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = &Bonsai_msdSetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = &Bonsai_msdGetSMIPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = &Bonsai_msdGetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = &Bonsai_msdSetSMIExtPhyReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = &Bonsai_msdSetSMIExtPhyXMDIOReg;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = &Bonsai_msdGetSMIExtPhyXMDIOReg;

	dev->SwitchDevObj.LEDObj.gledForceOn = &Bonsai_gledForceOnIntf;
	dev->SwitchDevObj.LEDObj.gledForceOff = &Bonsai_gledForceOffIntf;
	dev->SwitchDevObj.LEDObj.gledLinkAct = &Bonsai_gledLinkActIntf;
	dev->SwitchDevObj.LEDObj.gledModeSet = &Bonsai_gledModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledModeGet = &Bonsai_gledModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = &Bonsai_gledBlinkRateSetIntf;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = &Bonsai_gledBlinkRateGetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = &Bonsai_gledPulseStretchSetIntf;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = &Bonsai_gledPulseStretchGetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = &Bonsai_gledSpecialModeSetIntf;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = &Bonsai_gledSpecialModeGetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = &Bonsai_gledCtrlSetIntf;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = &Bonsai_gledCtrlGetIntf;
	dev->SwitchDevObj.LEDObj.gledDump = &Bonsai_gledDump;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = &Bonsai_gpolicySetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = &Bonsai_gpolicyGetMgmtCtrl;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = &Bonsai_gpolicySetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = &Bonsai_gpolicyGetCpuDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = &Bonsai_gpolicySetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = &Bonsai_gpolicyGetIgrMonitorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = &Bonsai_gpolicySetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = &Bonsai_gpolicyGetRsvd2CpuEnables;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = &Bonsai_gpolicySetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = &Bonsai_gpolicyGetMGMTPri;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = &Bonsai_gpolicySetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = &Bonsai_gpolicyGetRouteDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = &Bonsai_gpolicySetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = &Bonsai_gpolicyGetMirrorDest;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = &Bonsai_gpolicySetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = &Bonsai_gpolicyGetIllegalIPAddr;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = &Bonsai_gpolicySetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = &Bonsai_gpolicyGetDipDaMismatch;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = &Bonsai_gpolicySetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = &Bonsai_gpolicyGetZeroSIP;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = &Bonsai_gpolicySetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = &Bonsai_gpolicyGetMcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = &Bonsai_gpolicySetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = &Bonsai_gpolicyGetUcTtlExceeded;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = &Bonsai_gprtSetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = &Bonsai_gprtGetPortTtlThreshold;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = &Bonsai_gpolicySetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = &Bonsai_gpolicyGetIpHdrErr;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = &Bonsai_gpolicySetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = &Bonsai_gpolicyGetTCPFinUrgPsh;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = &Bonsai_gpolicySetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = &Bonsai_gpolicyGetTCPFlagsSeqNum;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = &Bonsai_gpolicySetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = &Bonsai_gpolicyGetTCPSYN;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = &Bonsai_gpolicySetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = &Bonsai_gpolicyGetTCPHSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = &Bonsai_gprtSetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = &Bonsai_gprtGetPortTCPMinHdrSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = &Bonsai_gpolicySetICMPSize;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = &Bonsai_gpolicyGetICMPSize;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = &Bonsai_gprtSetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = &Bonsai_gprtGetPortICMPMaxSize;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = &Bonsai_gpolicySetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = &Bonsai_gpolicyGetUDPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = &Bonsai_gpolicySetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = &Bonsai_gpolicyGetTCPPort;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = &Bonsai_gpolicySetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = &Bonsai_gpolicyGetTCPSynFin;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = &Bonsai_gpolicySetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = &Bonsai_gpolicyGetTCPFrag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = &Bonsai_gpolicySetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = &Bonsai_gpolicyGetNearNonTPMR;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = &Bonsai_gpolicySetETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = &Bonsai_gpolicyGetETag;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = &Bonsai_gpolicySetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = &Bonsai_gpolicyGetNonETag;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = &Bonsai_gpolicyDump;

	dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll = &Bonsai_gqciStreamFiltFlushAll;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltSet = &Bonsai_gqciStreamFiltSet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet = &Bonsai_gqciStreamFiltBlockStatSet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet = &Bonsai_gqciStreamFiltBlockStatGet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts = &Bonsai_gqciStreamFiltEnAts;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate = &Bonsai_gqciStreamFiltEnGate;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet = &Bonsai_gqciStreamFiltMatchCtrGet;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet = &Bonsai_gqciStreamFiltFilterCtrGet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet = &Bonsai_gqciStreamGateFiltCtrGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet = &Bonsai_gqciFlowMeterFiltCtrGet;

	dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll = &Bonsai_gqciStreamGateFlushAll;
	dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet = &Bonsai_gqciStreamGateDefStateSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet = &Bonsai_gqciStreamGateScaleSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet = &Bonsai_gqciStreamGateTimeGenIdSet;
	dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd = &Bonsai_gqciStreamGateEntryAddIntf;

	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush = &Bonsai_gqciStreamGateTimeGenFlush;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet = &Bonsai_gqciStreamGateTimeGenSet;

	dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll = &Bonsai_gqciFlowMeterFlushAll;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet = &Bonsai_gqciFlowMeterCoupleFlagSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet = &Bonsai_gqciFlowMeterCoupleFlagGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet = &Bonsai_gqciFlowMeterDropOnYelSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet = &Bonsai_gqciFlowMeterDropOnYelGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet = &Bonsai_gqciFlowMeterRedBlockEnSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet = &Bonsai_gqciFlowMeterRedBlockEnGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet = &Bonsai_gqciFlowMeterRedBlkStatSet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet = &Bonsai_gqciFlowMeterRedBlkStatGet;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig = &Bonsai_gqciFlowMeterAdvConfig;

#endif
	return MSD_OK;
}

static MSD_STATUS InitObj(MSD_QD_DEV *dev)
{
	dev->SwitchDevObj.ATUObj.gfdbAddMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAtuEntryNext = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFlush = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbMoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemove = NULL;
	dev->SwitchDevObj.ATUObj.gfdbPortRemoveInDB = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDelAtuEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetViolation = NULL;
	dev->SwitchDevObj.ATUObj.gfdbFindAtuMacEntry = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetAgingTimeout = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetLearn2All = NULL;
	dev->SwitchDevObj.ATUObj.gfdbSetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnLimit = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetPortLearnCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCount = NULL;
	dev->SwitchDevObj.ATUObj.gfdbGetNonStaticEntryCountPerFid = NULL;
	dev->SwitchDevObj.ATUObj.gfdbDump = NULL;

	dev->SwitchDevObj.ECIDObj.gecidAddEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFlushAll = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFlushEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidMove = NULL;
	dev->SwitchDevObj.ECIDObj.gecidGetEcidEntryNext = NULL;
	dev->SwitchDevObj.ECIDObj.gecidFindEcidEntry = NULL;
	dev->SwitchDevObj.ECIDObj.gecidGetEntryCount = NULL;
	dev->SwitchDevObj.ECIDObj.gecidDump = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gprtSetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetCutThrouthEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowBadFcs = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPauseLimitIn = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetQueueToPause = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRxPriorityFlowControlToQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetTxPriorityFlowControlEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetOutQueueSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMapDA = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHoldAt1 = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetInt0nAgeOut = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetRefreshLocked = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIgnoreWrongData = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetARPMirror = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetARPMirror = NULL;

	dev->SwitchDevObj.POLICYObj.gpolicySetMgmtCtrl = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMgmtCtrl = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetCpuDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetCpuDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIngressMonitorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIngressMonitorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMGMTPri = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMGMTPri = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetRouteDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetRouteDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMirrorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMirrorDest = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIllegalIPAddr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIllegalIPAddr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetDipDaMismatch = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetDipDaMismatch = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetZeroSIP = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetZeroSIP = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetMcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetMcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetUcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUcTtlExceeded = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTtlThreshold = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTtlThreshold = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetIpHdrErr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetIpHdrErr = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFinUrgPsh = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFinUrgPsh = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFlagsSeqNum = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFlagsSeqNum = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSYN = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSYN = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPHSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPHSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortTCPMinHdrSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortTCPMinHdrSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetICMPSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetICMPSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtSetPortICMPMaxSize = NULL;
	dev->SwitchDevObj.POLICYObj.gprtGetPortICMPMaxSize = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetUDPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetUDPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPPort = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPSynFin = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPSynFin = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetTCPFrag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetTCPFrag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetNearNonTPMR = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNearNonTPMR = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicySetNonETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyGetNonETag = NULL;
	dev->SwitchDevObj.POLICYObj.gpolicyDump = NULL;

	dev->SwitchDevObj.VCTObj.gvctGetAdvCableDiag = NULL;
	dev->SwitchDevObj.VCTObj.gvctGetAdvExtendedStatus = NULL;

	dev->SwitchDevObj.STUObj.gstuAddEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryNext = NULL;
	dev->SwitchDevObj.STUObj.gstuFindSidEntry = NULL;
	dev->SwitchDevObj.STUObj.gstuGetEntryCount = NULL;
	dev->SwitchDevObj.STUObj.gstuDump = NULL;

	dev->SwitchDevObj.VTUObj.gvtuFindVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuExistVidEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuFlush = NULL;
	dev->SwitchDevObj.VTUObj.gvtuAddEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryNext = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDelEntry = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMode = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetViolation = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetEntryCount = NULL;
	dev->SwitchDevObj.VTUObj.gvtuSetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuGetMemberTag = NULL;
	dev->SwitchDevObj.VTUObj.gvtuDump = NULL;

	dev->SwitchDevObj.IMPObj.impRun = NULL;
	dev->SwitchDevObj.IMPObj.impReset = NULL;
	dev->SwitchDevObj.IMPObj.impWriteEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToEEPROM = NULL;
	dev->SwitchDevObj.IMPObj.impLoadToRAM = NULL;
	dev->SwitchDevObj.IMPObj.impStop = NULL;

	dev->SwitchDevObj.EEPROMObj.geepromWrite = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromRead = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromSetMChipSel = NULL;
	dev->SwitchDevObj.EEPROMObj.geepromGetMChipSel = NULL;

	dev->SwitchDevObj.PHYCTRLObj.gphyReset = NULL;
    dev->SwitchDevObj.PHYCTRLObj.serdesReset = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortLoopback = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortSpeed = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyPortPowerDown = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortDuplexMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetPortAutoMode = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetEEE = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetFlowControlEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphySetIntEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntEnable = NULL;
	dev->SwitchDevObj.PHYCTRLObj.gphyGetIntStatus = NULL;

	dev->SwitchDevObj.PIRLObj.gpirlInitialize = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlInitResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlCustomSetup = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlReadResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlWriteResource = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlGetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlSetResReg = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfig = NULL;
	dev->SwitchDevObj.PIRLObj.gpirlAdvConfigDisable = NULL;

	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnSetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gvlnGetPortVlanDot1qMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardTagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardUntagged = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetUnicastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMulticastFloodBlock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDiscardBCastMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIGMPSnoopEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetHeaderMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDropOnLock = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetJumboMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSpecifiedMTU = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpecifiedMTU = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLearnEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVTUPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDAPriorityOverride = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetMessagePort = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetMessagePort = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEtherType = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEtherType = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetAllowVidZero = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFlowCtrl = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetIngressMonitorSource = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcSetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.grcGetEgressRate = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetDuplex = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceLink = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetLinkState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetSpeed = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetFrameMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortECID = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortECID = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetEgressMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetEgressMode = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetIgnoreETag = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetExtendedPortCtrlReg = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetExtendedPortCtrlReg = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetVlanPorts = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetForceDefaultVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetForceDefaultVid = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPortState = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptEnable = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptSize = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtSetPreemptQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtGetPreemptQueue = NULL;
	dev->SwitchDevObj.PORTCTRLObj.gprtRegDump = NULL;

	dev->SwitchDevObj.TrunkObj.gprtSetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gprtGetTrunkPort = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkMaskTable = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetHashTrunk = NULL;
	dev->SwitchDevObj.TrunkObj.gsysSetTrunkRouting = NULL;
	dev->SwitchDevObj.TrunkObj.gsysGetTrunkRouting = NULL;

	dev->SwitchDevObj.PORTRMONObj.gstatsFlushAll = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsFlushPort = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortCounter = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetPortAllCounters = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsGetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gstatsSetHistogramMode = NULL;
	dev->SwitchDevObj.PORTRMONObj.gmibDump = NULL;

	dev->SwitchDevObj.QUEUEObj.gprtSetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetQueueCtrl = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtSetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetPortSched = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysSetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gsysGetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtSetQoSWeight = NULL;
	dev->SwitchDevObj.QUEUEObj.gprtGetQoSWeight = NULL;

	dev->SwitchDevObj.SYSCTRLObj.gsysSetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetFloodBC = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMonitorMgmtCtrl = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetCPUDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetIngressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetEgressMonitorDest = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRsvd2CpuEnables = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetMGMTPri = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetSMISetup = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetSMISetup = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal1RegDump = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGlobal2RegDump = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysSetRMUMode = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysGetRMUMode = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvWrite = NULL;
	dev->SwitchDevObj.SYSCTRLObj.gsysQbvRead = NULL;

	dev->SwitchDevObj.QOSMAPObj.gprtGetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetIEEEPrioMap = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosGetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetDefQpri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetInitialPri = NULL;
	dev->SwitchDevObj.QOSMAPObj.gqosSetPrioMapRule = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtRewriteDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToQueue = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtSetCosToDscp = NULL;
	dev->SwitchDevObj.QOSMAPObj.gprtGetCosToDscp = NULL;

	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXHiLimit = NULL;
	dev->SwitchDevObj.QAVObj.gqavGetPortQpriXRate = NULL;
	dev->SwitchDevObj.QAVObj.gqavSetPortQpriXRate = NULL;

	dev->SwitchDevObj.PTPObj.gptpSetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntEnable = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPortConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTSValidSt = NULL;
	dev->SwitchDevObj.PTPObj.gptpResetTimeStamp = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIntStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMode = NULL;
	dev->SwitchDevObj.PTPObj.gptpSetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetMasterSlave = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODStoreAll = NULL;
	dev->SwitchDevObj.PTPObj.gptpTODCaptureAll = NULL;

	dev->SwitchDevObj.PTPObj.gptpSetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gptpGetPulse = NULL;
	dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPortReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPortReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetGlobalReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetGlobalReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetTaiReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetTaiReg = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet1588v2Config = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet8021asConfig = NULL;
    dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel = NULL;
    dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl = NULL;
    dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl = NULL;
	dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel = NULL;
	dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel = NULL;

	dev->SwitchDevObj.TCAMObj.gtcamFlushAll = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamFindEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamSetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamGetMode = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAddEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamRemoveEntryPort = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrFlushEntryAllPorts = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrLoadEntry = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrGetNextTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamEgrReadTCAMData = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamDump = NULL;
	dev->SwitchDevObj.TCAMObj.gtcamAdvConfig = NULL;

	dev->SwitchDevObj.TCAMRCObj.gtcamRCLoadEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFlushEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCReadEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCGetNextEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCFindEntry = NULL;
	dev->SwitchDevObj.TCAMRCObj.gtcamRCEntryDump = NULL;

	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompLoad = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushAll = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompFlushPort = NULL;
	dev->SwitchDevObj.TCAMRCCompObj.gtcamRCCompRead = NULL;

	dev->SwitchDevObj.ARPObj.garpLoadMcEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpLoadUcEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpFlushEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpFlushAll = NULL;
	dev->SwitchDevObj.ARPObj.garpReadEntry = NULL;
	dev->SwitchDevObj.ARPObj.garpGetNextEntry = NULL;

	dev->SwitchDevObj.RMUObj.grmuGetID = NULL;
	dev->SwitchDevObj.RMUObj.grmuAtuDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuEcidDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMibDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuMib2Dump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMDump = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMRead = NULL;
	dev->SwitchDevObj.RMUObj.grmuTCAMWrite = NULL;
	dev->SwitchDevObj.RMUObj.grmuMultiRegAccess = NULL;
    dev->SwitchDevObj.RMUObj.grmuRegDump = NULL;

	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIC45PhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdSetSMIExtPhyXMDIOReg = NULL;
	dev->SwitchDevObj.SmiPhyRegAccessObj.msdGetSMIExtPhyXMDIOReg = NULL;

	dev->SwitchDevObj.LEDObj.gledForceOn = NULL;
	dev->SwitchDevObj.LEDObj.gledForceOff = NULL;
	dev->SwitchDevObj.LEDObj.gledLinkAct = NULL;
	dev->SwitchDevObj.LEDObj.gledModeSet = NULL;
	dev->SwitchDevObj.LEDObj.gledModeGet = NULL;
	dev->SwitchDevObj.LEDObj.gledBlinkRateSet = NULL;
	dev->SwitchDevObj.LEDObj.gledBlinkRateGet = NULL;
	dev->SwitchDevObj.LEDObj.gledPulseStretchSet = NULL;
	dev->SwitchDevObj.LEDObj.gledPulseStretchGet = NULL;
	dev->SwitchDevObj.LEDObj.gledSpecialModeSet = NULL;
	dev->SwitchDevObj.LEDObj.gledSpecialModeGet = NULL;
	dev->SwitchDevObj.LEDObj.gledCtrlSet = NULL;
	dev->SwitchDevObj.LEDObj.gledCtrlGet = NULL;
	dev->SwitchDevObj.LEDObj.gledDump = NULL;

	dev->SwitchDevObj.INTObj.gdevIntEnableSet = NULL;
	dev->SwitchDevObj.INTObj.gdevIntEnableGet = NULL;
	dev->SwitchDevObj.INTObj.gdevIntStatusGet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntEnableSet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntEnableGet = NULL;
	dev->SwitchDevObj.INTObj.gactiveIntStatusGet = NULL;

    dev->SwitchDevObj.QCRObj.gqcrFlushAll = NULL;
    dev->SwitchDevObj.QCRObj.gqcrFlushOne = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetGrpId = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetGrpId = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetCbsLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetCbsLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetMaxResidenceTime = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetMaxResidenceTime = NULL;
    dev->SwitchDevObj.QCRObj.gqcrSetRateLimit = NULL;
    dev->SwitchDevObj.QCRObj.gqcrGetRateLimit = NULL;

    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqGenLoad = NULL;

    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerIndvRcvyReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyGetViolation = NULL;
    dev->SwitchDevObj.FRERObj.gfrerSeqRcvyReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank0SCtrReadClear = NULL;

    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushAll = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrFlushEntry = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrRead = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrLoad = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrGetNext = NULL;
    dev->SwitchDevObj.FRERObj.gfrerBank1SCtrReadClear = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamFiltFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltBlockStatGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnAts = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltEnGate = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltMatchCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamFiltFilterCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateFiltCtrGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterFiltCtrGet = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamGateFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateDefStateSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateScaleSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenIdSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateEntryAdd = NULL;

	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenFlush = NULL;
	dev->SwitchDevObj.QCIObj.gqciStreamGateTimeGenSet = NULL;

	dev->SwitchDevObj.QCIObj.gqciFlowMeterFlushAll = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterCoupleFlagGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterDropOnYelGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlockEnGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatSet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterRedBlkStatGet = NULL;
	dev->SwitchDevObj.QCIObj.gqciFlowMeterAdvConfig = NULL;

	dev->SwitchDevObj.MACSECObj.gmacsecFlushAll = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRule = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSecY = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSC = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecCreateTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecDeleteTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSA = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSCStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRXSecYStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSCStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTXSecYStates = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressSet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableEgressGet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressSet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecEnableIngressGet = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxXPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxXPNThreshold = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetRxDefaultSCI = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetRxDefaultSCI = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecSetTxSCIntCtrl = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetTxSCIntCtrl = NULL;
	dev->SwitchDevObj.MACSECObj.gmacsecGetNextTxSCIntStatus = NULL;

	return MSD_OK;
}

MSD_STATUS InitDevObj(MSD_QD_DEV *dev)
{
	MSD_STATUS retVal;
	retVal = InitObj(dev);
	if (dev->devName == MSD_TOPAZ)
	{
		retVal = InitDevObj_Topaz(dev);
	}
	else if (dev->devName == MSD_PERIDOT)
	{
		retVal = InitDevObj_Peridot(dev);
	}
	else if (dev->devName == MSD_PEARL)
	{
		retVal = InitDevObj_Pearl(dev);
	}
	else if (dev->devName == MSD_AGATE)
	{
		retVal = InitDevObj_Agate(dev);
	}
	else if (dev->devName == MSD_AMETHYST)
	{
		retVal = InitDevObj_Amethyst(dev);
	}
	else if (dev->devName == MSD_OAK)
	{
		retVal = InitDevObj_Oak(dev);
	}
    else if (dev->devName == MSD_SPRUCE)
    {
        retVal = InitDevObj_Spruce(dev);
    }
    else if (dev->devName == MSD_BONSAIZ1)
    {
        retVal = InitDevObj_BonsaiZ1(dev);
    }
	else if (dev->devName == MSD_FIR)
    {
        retVal = InitDevObj_Fir(dev);
    }
	else if (dev->devName == MSD_BONSAI)
	{
		retVal = InitDevObj_Bonsai(dev);
	}
	else
	{
		retVal = MSD_FAIL;
	}

	return retVal;
}

/*******************************************************************************
* msdReserveFreeQueueSize
*
* DESCRIPTION:
*       workaround for QC WatchDog trigger issue: reserve 16 free queue size, total 1024. Set threshold 
*       for the free pool FIFO to 0x10. This means the recycle pointer will not be reused immediately, 
*       16 pointers will be given to per-port module before this pointer can be used.
*
* INPUTS:
*        dev - pointer to the structure for device.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK, if input is valid. MSD_FAIL, otherwise.
*
*******************************************************************************/
static MSD_STATUS msdReserveFreeQueueSize(MSD_QD_DEV *qdev)
{
	MSD_U16 reg27, tempData;
	MSD_STATUS retVal;

	/*read back G1 offset 0x1b register value*/
	retVal = msdGetAnyReg(qdev->devNum, (MSD_U8)0x1B, (MSD_U8)0x1B, &reg27);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdGetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*Change the hidden block to QC Hidden registers, Hidden Block: 0x2*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1B, 0x2000);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*wait QCHiddenCtrl register busy bit self clear*/
	do
	{
		retVal = msdGetAnyReg(qdev->devNum, 0x1B, 0x1B, &tempData);
		if (retVal != 0)
		{
			MSD_DBG_ERROR(("msdGetAnyReg return failed, retval = %d.\n", retVal));
			return MSD_FAIL;
		}
	} while ((tempData & 0x8000) == 0x8000);

	/*write data to QC Hidden Data, Device Offset 0x1B, Register Offset: 0x1A, reserve 16 free queue size*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1A, 0x8400);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}
	/*write cmd register, QC Hidden Control, Device Offset 0x1B, Register Offset: 0x19*/
	tempData = (0xE000 | (0x1F << 8) | 0xC);

	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x19, tempData);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}

	/*restore the Hidden block*/
	retVal = msdSetAnyReg(qdev->devNum, 0x1B, 0x1B, reg27);
	if (retVal != 0)
	{
		MSD_DBG_ERROR(("msdSetAnyReg return failed, retval = %d.\n", retVal));
		return MSD_FAIL;
	}

	return MSD_OK;
}