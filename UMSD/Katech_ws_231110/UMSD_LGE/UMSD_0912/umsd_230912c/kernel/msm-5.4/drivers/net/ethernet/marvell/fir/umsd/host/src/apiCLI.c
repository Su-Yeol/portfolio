/*******************************************************************************
*
* apiCLI.c
*
* DESCRIPTION:
*       Command Line Interface for API debug
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "msdApi.h"
#include "apiCLI.h"
#include "version.h"

#define MAX_ARGS        200
#define MAX_ARGS_LENGTH 200
#define MAX_CMD_LENGTH  2000
#define PROMPT_STRING  "UMSD_MCLI> "
#define MAXMIBINDEX 16U
static int logOnOffFlag = 0;
static char* g_filename = NULL;
static PRINT_CALLBACK printCallback = NULL;

#if 1 // jay.choi.230901.get.API
MSD_U32 linkState = 0;
#endif

CLI_PAIR_API_STRUCT pairCmdList[] =
{
    /*Qos methods*/
    { "qossetCosQueue", (void*)&msdQosPortCosToQueueSet,						"qosgetCosQueue", (void*)&msdQosPortCosToQueueGet },
    { "qossetCosDscp", (void*)&msdQosPortCosToDscpMappingSet,					"qosgetCosDscp", (void*)&msdQosPortCosToDscpMappingGet },
    { "qossetPriMapRule", (void*)&msdPortPrioRuleSet,							"qosgetPriMapRule", (void*)&msdPortPrioRuleGet },
    { "qossetInitPri", (void*)&msdPortInitialPriSet,							"qosgetInitPri", (void*)&msdPortInitialPriGet },
    { "qossetDefQPri", (void*)&msdPortDefQPriSet,								"qosgetDefQPri", (void*)&msdPortDefQPriGet },
    { "qossetDefFPri", (void*)&msdPortDefFPriSet,								"qosgetDefFPri", (void*)&msdPortDefFPriGet },
    { "qosenRewriteDscp", (void*)&msdQosPortRewriteDscp,						"", NULL },

    /*Sys(global) methods*/
    { "globalsetFloodBC", (void*)&msdSysFloodBCSet,								"globalgetFloodBC", (void*)&msdSysFloodBCGet },
    { "globalsetCPUDest", (void*)&msdSysCPUDestSet,								"globalgetCPUDest", (void*)&msdSysCPUDestGet },
    { "globalsetIMD", (void*)&msdSysIngressMonitorDestSet,						"globalgetIMD", (void*)&msdSysIngressMonitorDestGet },
    { "globalsetEMD", (void*)&msdSysEgressMonitorDestSet,						"globalgetEMD", (void*)&msdSysEgressMonitorDestGet },
    { "globalsetMMC", (void*)&msdSysMonitorMgmtCtrlSet,							"globalgetMMC", (void*)&msdSysMonitorMgmtCtrlGet },
    { "globalsetRsvd2CpuEn", (void*)&msdSysRsvd2CpuEnablesSet,					"globalgetRsvd2CpuEn", (void*)&msdSysRsvd2CpuEnablesGet },
    { "globalsetMGMTPri", (void*)&msdSysMGMTPriSet,								"globalgetMGMTPri", (void*)&msdSysMGMTPriGet },
    { "globalsetSMISetup", (void*)&msdPortSmiSetupSet,							"globalgetSMISetup", (void*)&msdPortSmiSetupGet },
    { "globaldumpGlobal1", (void*)&msdGlobal1RegDump,							"", NULL },
    { "globaldumpGlobal2", (void*)&msdGlobal2RegDump,							"", NULL },
	{ "globalsetRMUMode", (void*)&msdSysRMUModeSet,                             "globalgetRMUMode", (void*)&msdSysRMUModeGet },

	/*Qbv methods*/
	{ "qbvwrite", (void*)&msdQbvWrite,                                          "qbvread", (void*)&msdQbvRead },

    /*Port methods*/
	{ "portsetForceDefVid", (void*)&msdPortForceDefaultVlanIdSet,				"portgetForceDefVid", (void*)&msdPortForceDefaultVlanIdGet },
    { "portsetDefVid", (void*)&msdPortDefaultVlanIdSet,							"portgetDefVid", (void*)&msdPortDefaultVlanIdGet },
    { "portsetQMode", (void*)&msdPort8021qModeSet,								"portgetQMode", (void*)&msdPort8021qModeGet },
    { "portsetDiscardTagged", (void*)&msdPortDiscardTaggedSet,					"portgetDiscardTagged", (void*)&msdPortDiscardTaggedGet },
    { "portsetDiscardUTagged", (void*)&msdPortDiscardUntaggedSet,				"portgetDiscardUTagged", (void*)&msdPortDiscardUntaggedGet },
    { "portsetUCFloodBlock", (void*)&msdPortUnicastFloodBlockSet,				"portgetUCFloodBlock", (void*)&msdPortUnicastFloodBlockGet },
    { "portsetMCFloodBlock", (void*)&msdPortMulticastFloodBlockSet,				"portgetMCFloodBlock", (void*)&msdPortMulticastFloodBlockGet },
    { "portsetBCMode", (void*)&msdPortDiscardBcastModeSet,						"portgetBCMode", (void*)&msdPortDiscardBcastModeGet },
    { "portsetIGMPSnoopEnable", (void*)&msdPortIGMPSnoopEnableSet,				"portgetIGMPSnoopEnable", (void*)&msdPortIGMPSnoopEnableGet },
    { "portsetHeaderMode", (void*)&msdPortHeaderModeSet,						"portgetHeaderMode", (void*)&msdPortHeaderModeGet },
    { "portsetDropOnLock", (void*)&msdPortDropOnLockSet,						"portgetDropOnLock", (void*)&msdPortDropOnLockGet },
    { "portsetMtu", (void*)&msdPortMtuSet,										"portgetMtu", (void*)&msdPortMtuGet },
	{ "portsetSpecifiedMtu", (void*)&msdPortSpecifiedMtuSet,                    "portgetSpecifiedMtu", (void*)&msdPortSpecifiedMtuGet },
    { "portsetLearnEnable", (void*)&msdPortLearnEnableSet,						"portgetLearnEnable", (void*)&msdPortLearnEnableGet },
    { "portsetVTUPriOverride", (void*)&msdPortVTUPriorityOverrideSet,			"portgetVTUPriOverride", (void*)&msdPortVTUPriorityOverrideGet },
    { "portsetSAPriOverride", (void*)&msdPortSAPriorityOverrideSet,				"portgetSAPriOverride", (void*)&msdPortSAPriorityOverrideGet },
    { "portsetDAPriOverride", (void*)&msdPortDAPriorityOverrideSet,				"portgetDAPriOverride", (void*)&msdPortDAPriorityOverrideGet },
    { "portsetMsgPort", (void*)&msdPortMessagePortSet,							"portgetMsgPort", (void*)&msdPortMessagePortGet },
    { "portsetEtherType", (void*)&msdPortEtherTypeSet,							"portgetEtherType", (void*)&msdPortEtherTypeGet },
    { "portsetAllowVidZero", (void*)&msdPortAllowVidZeroSet,					"portgetAllowVidZero", (void*)&msdPortAllowVidZeroGet },
    { "portsetEgrRate", (void*)&msdPortEgressRateSet,							"portgetEgrRate", (void*)&msdPortEgressRateGet },
    { "portsetFlowCtrl", (void*)&msdPortFlowControlEnableSet,					"portgetFlowCtrl", (void*)&msdPortFlowControlEnableGet },
    { "portsetEMS", (void*)&msdPortEgressMonitorSourceEnableSet,				"portgetEMS", (void*)&msdPortEgressMonitorSourceEnableGet },
    { "portsetIMS", (void*)&msdPortIngressMonitorSourceEnableSet,				"portgetIMS", (void*)&msdPortIngressMonitorSourceEnableGet },
    { "portsetForceDuplexMode", (void*)&msdPortForceDuplexModeSet,				"portgetForceDuplexMode", (void*)&msdPortForceDuplexModeGet },
    { "portsetForceLinkMode", (void*)&msdPortForceLinkModeSet,					"portgetForceLinkMode", (void*)&msdPortForceLinkModeGet },
    { "portsetForceSpeed", (void*)&msdPortForceSpeedModeSet,					"portgetForceSpeed", (void*)&msdPortForceSpeedModeGet },
    { "portsetIgnoreFcs", (void*)&msdPortIgnoreFcsSet,							"portgetIgnoreFcs", (void*)&msdPortIgnoreFcsGet },
    { "portsetAllowBadFcs", (void*)&msdPortAllowBadFcsSet,						"portgetAllowBadFcs", (void*)&msdPortAllowBadFcsGet },
    { "portsetPauseLimitOut", (void*)&msdPortPauseLimitOutSet,					"portgetPauseLimitOut", (void*)&msdPortPauseLimitOutGet },
    { "portsetPauseLimitIn", (void*)&msdPortPauseLimitInSet,					"portgetPauseLimitIn", (void*)&msdPortPauseLimitInGet },
    { "portsetQueueToPause", (void*)&msdPortQueueToPauseSet,					"portgetQueueToPause", (void*)&msdPortQueueToPauseGet },
    { "portsetRxPriFCEnable", (void*)&msdPortRxPriorityFlowControlEnableSet,	"portgetRxPriFCEnable", (void*)&msdPortRxPriorityFlowControlEnableGet },
    { "portsetRxPriFCToQueue", (void*)&msdPortRxPriorityFlowControlToQueueSet,	"portgetRxPriFCToQueue", (void*)&msdPortRxPriorityFlowControlToQueueGet },
    { "portsetTxPriFCEnable", (void*)&msdPortTxPriorityFlowControlEnableSet,	"portgetTxPriFCEnable", (void*)&msdPortTxPriorityFlowControlEnableGet },
    { "portsetMapDA", (void*)&msdPortMapDASet,									"portgetMapDA", (void*)&msdPortMapDAGet },
    { "portsetARPMirror", (void*)&msdPortARPMirrorSet,							"portgetARPMirror", (void*)&msdPortARPMirrorGet },
    { "portsetLocked", (void*)&msdPortLockedSet,								"portgetLocked", (void*)&msdPortLockedGet },
    { "portsetHoldAt1", (void*)&msdPortHoldAt1Set,								"portgetHoldAt1", (void*)&msdPortHoldAt1Get },
    { "portsetIntOnAgeOut", (void*)&msdPortIntOnAgeOutSet,						"portgetIntOnAgeOut", (void*)&msdPortIntOnAgeOutGet },
    { "portsetRefreshLocked", (void*)&msdPortRefreshLockedSet,					"portgetRefreshLocked", (void*)&msdPortRefreshLockedGet },
    { "portsetIgnoreWrongData", (void*)&msdPortIgnoreWrongDataSet,				"portgetIgnoreWrongData", (void*)&msdPortIgnoreWrongDataGet },

    { "", NULL,																	"portgetOutQueueSize", (void*)&msdPortOutQueueSizeGet },
    { "", NULL,																	"portgetLinkState", (void*)&msdPortLinkStatusGet },
    { "", NULL,																	"portgetSpeed", (void*)&msdPortSpeedStatusGet },
    { "", NULL,																	"portgetDuplex", (void*)&msdPortDuplexStatusGet },
    { "portsetFrameMode", (void*)&msdPortFrameModeSet,							"portgetFrameMode", (void*)&msdPortFrameModeGet },
	{ "portsetEgressMode", (void*)&msdPortEgressModeSet,                        "portgetEgressMode", (void*)&msdPortEgressModeGet },
	{ "portsetECID", (void*)&msdPortECIDSet,                                    "portgetECID", (void*)&msdPortECIDGet },
	{ "portsetIgnoreETag", (void*)&msdPortIgnoreETagSet,                        "portgetIgnoreETag", (void*)&msdPortIgnoreETagGet },
	{ "portsetExtendedPortCtrlReg", (void*)&msdPortExtendedPortCtrlRegSet,      "portgetExtendedPortCtrlReg", (void*)&msdPortExtendedPortCtrlRegGet },
    { "portsetPortState", (void*)&msdPortStateSet,								"portgetPortState", (void*)&msdPortStateGet },
	{ "portsetPreemptEnable", (void*)&msdPortPreemptEnableSet,                  "portgetPreemptEnable", (void*)&msdPortPreemptEnableGet },
	{ "portsetPreemptSize", (void*)&msdPortPreemptSizeSet,                      "portgetPreemptSize", (void*)&msdPortPreemptSizeGet },
	{ "portsetPreemptQueue", (void*)&msdPortPreemptQueueSet,                    "portgetPreemptQueue", (void*)&msdPortPreemptQueueGet },

    { "portdump", (void*)&msdPortRegDump,										"", NULL },

    /*QC*/
    { "qcsetQc", (void*)&msdPortQueueCtrlSet,									"qcgetQc", (void*)&msdPortQueueCtrlGet },
    { "qcsetPortSched", (void*)&msdPortSchedSet,								"qcgetPortSched", (void*)&msdPortSchedGet },

    /*Trunk*/
    { "trunksetTrunkMask", (void*)&msdTrunkMaskTableSet,						"trunkgetTrunkMask", (void*)&msdTrunkMaskTableGet },
    { "trunksetTrunkHash", (void*)&msdTrunkHashTrunkSet,						"trunkgetTrunkHash", (void*)&msdTrunkHashTrunkGet },
	{ "trunksetTrunkRoute", (void*)&msdTrunkRoutingSet,                         "trunkgetTrunkRoute", (void*)&msdTrunkRoutingGet },
	{ "trunksetTrunkMap", (void*)&msdTrunkMappingSet,                           "trunkgetTrunkMap", (void*)&msdTrunkMappingGet },
    { "trunksetLAGPort", (void*)&msdPortLAGPortSet,								"trunkgetLAGPort", (void*)&msdPortLAGPortGet },

    /*ATU methods*/
    { "atusetAging", (void*)&msdFdbAgingTimeoutSet,								"atugetAging", (void*)&msdFdbAgingTimeoutGet },
    { "atusetLearn2All", (void*)&msdFdbLearnOnAllDevicesSet,					"atugetLearn2All", (void*)&msdFdbLearnOnAllDevicesGet },
    { "atusetPortLearnLimit", (void*)&msdFdbPortLearnLimitSet,					"atugetPortLearnLimit", (void*)&msdFdbPortLearnLimitGet },
    { "", NULL,																	"atugetLearnCount", (void*)&msdFdbPortLearnCountGet },
    { "", NULL,																	"atugetCount", (void*)&msdFdbEntryCountGet },
    { "", NULL,																	"atugetCountPerFid", (void*)&msdFdbEntryCountPerFidGet },
    { "", NULL,																	"atugetNonStaticCount", (void*)&msdFdbEntryCountNonStaticGet },
    { "", NULL,																	"atugetNonStaticCountPerFid", (void*)&msdFdbEntryCountNonStaticPerFidGet },
    { "atudump", (void*)&msdFdbDump,											"", NULL },

    /*ECID methods*/
    { "ecidsetBpeEnable", (void*)&msdBpeEnableSet,								"ecidgetBpeEnable", (void*)&msdBpeEnableGet },
    { "ecidmove", (void*)&msdEcidEntryMove,										"", NULL },
    { "eciddelEntry", (void*)&msdEcidEntryDelete,								"", NULL },
    { "eciddump", (void*)&msdEcidDump,											"", NULL },

    /*vtu dump*/
    { "vlandump", (void*)&msdVtuDump,											"", NULL },

    /*stu dump*/
    { "studump", (void*)&msdStuDump,											"", NULL },

    /*tcam dump*/
    { "tcamdump", (void*)&msdTcamDump,											"", NULL },

    /*tcamrc dump*/
    { "tcamrcdump", (void*)&msdTcamRCDump,										"", NULL },

    /*Phy methods*/
    { "physetLoopback", (void*)&msdPhyPortLoopbackSet,							"", NULL },
    { "physetSpeed", (void*)&msdPhyPortSpeedSet,								"", NULL },
    { "phypowerDown", (void*)&msdPhyPortPowerDown,								"", NULL },
    { "physetDuplex", (void*)&msdPhyPortDuplexModeSet,							"", NULL },
    { "physetAutoMode", (void*)&msdPhyPortAutoModeSet,							"", NULL },
    { "physetEEE", (void*)&msdPhyEEESet,										"", NULL },
	{ "physetFlowControl", (void*)&msdPhyFlowControlEnableSet,                  "", NULL },
	{ "phyrw", (void*)&msdSetSMIPhyReg,											"phyrr", (void*)&msdGetSMIPhyReg },
	{ "phyxrw", (void*)&msdSetSMIPhyXMDIOReg,									"phyxrr", (void*)&msdGetSMIPhyXMDIOReg },
	{ "phyextrw", (void*)&msdSetSMIExtPhyReg,									"phyextrr", (void*)&msdGetSMIExtPhyReg },
	{ "phyextxrw", (void*)&msdSetSMIExtPhyXMDIOReg,								"phyextxrr", (void*)&msdGetSMIExtPhyXMDIOReg },
	
	/*interrupt methods*/
	{ "interruptsetphyIntEnable", (void*)&msdPhyInterruptEnableSet,              "interruptgetphyIntEnable", (void*)&msdPhyInterruptEnableGet },
	{ "interruptsetDevIntEnable", (void*)&msdSysDevIntEnableSet,                 "interruptgetDevIntEnable", (void*)&msdSysDevIntEnableGet },
	{ "", NULL,                                                                  "interruptgetDevIntStatus", (void*)&msdSysDevIntStatusGet },
	{ "interruptsetActiveIntEnable", (void*)&msdSysActiveIntEnableSet,           "interruptgetActiveIntEnable", (void*)&msdSysActiveIntEnableGet },
	{ "", NULL,                                                                  "interruptgetActiveIntStatus", (void*)&msdSysActiveIntStatusGet },

    /*Serdes methods*/
    { "serdesreset", (void*)&msdSerdesReset,                                     "", NULL},
    { "serdesxrw", (void*)&msdSetSMIC45PhyReg,									"serdesxrr", (void*)&msdGetSMIC45PhyReg },

    /*MIBs*/
    { "mibsetHistogramMode", (void*)&msdStatsHistogramModeSet,					"mibgetHistogramMode", (void*)&msdStatsHistogramModeGet },

    /*Qav*/
    { "qavsetXRate", (void*)&msdQavPortQpriXRateSet,							"qavgetXRate", (void*)&msdQavPortQpriXRateGet },
    { "qavsetXHiLimit", (void*)&msdQavPortQpriXHiLimitSet,						"qavgetXHiLimit", (void*)&msdQavPortQpriXHiLimitGet },

    /*PTP*/
    { "ptpsetPortPTPEn", (void*)&msdPtpPortPTPEnSet,							"ptpgetPortPTPEn", (void*)&msdPtpPortPTPEnGet },
    { "ptpsetMeanPathDelay", (void*)&msdPtpMeanPathDelaySet,					"ptpgetMeanPathDelay", (void*)&msdPtpMeanPathDelayGet },
    { "ptpsetIgrPathDelayAsym", (void*)&msdPtpIgrPathDelayAsymSet,				"ptpgetIgrPathDelayAsym", (void*)&msdPtpIgrPathDelayAsymGet },
    { "ptpsetEgrPathDelayAsym", (void*)&msdPtpEgrPathDelayAsymSet,				"ptpgetEgrPathDelayAsym", (void*)&msdPtpEgrPathDelayAsymGet },
    { "ptpsetMasterSlave", (void*)&msdPtpMasterSlaveSet,						"ptpgetMasterSlave", (void*)&msdPtpMasterSlaveGet },
    { "", NULL,																	"ptpgetTODBusyBit", (void*)&msdPtpTODBusyBitStatusGet },
    { "", NULL,																	"ptpgetPTPGlobalTime", (void*)&msdGetPTPGlobalTime },
    { "", NULL,																	"ptpgetTSValid", (void*)&msdPtpTSValidStGet },
    { "ptpresetTS", (void*)&msdPtpTimeStampReset,								"", NULL },
    { "ptpsetPortReg", (void*)&msdPtpPortRegSet,						        "ptpgetPortReg", (void*)&msdPtpPortRegGet },
    { "ptpsetGlobalReg", (void*)&msdPtpGlobalRegSet,						    "ptpgetGlobalReg", (void*)&msdPtpGlobalRegGet },
    { "ptpsetTaiReg", (void*)&msdTaiRegSet,						                "ptpgetTaiReg", (void*)&msdTaiRegGet },
    { "ptpsetPreemptTimestampSel", (void*)&msdPtpPreemptTimestampSelSet,		"ptpgetPreemptTimestampSel", (void*)&msdPtpPreemptTimestampSelGet },
    { "ptpsetPreemptCtrl", (void*)&msdPtpPreemptCtrlSet,			            "ptpgetPreemptCtrl", (void*)&msdPtpPreemptCtrlGet },
    { "ptpset1588v2Config", (void*)&msdPtp1588v2ConfigSet,			            "", NULL },
    { "ptpset8021asConfig", (void*)&msdPtp8021asConfigSet,			            "", NULL },
    { "ptpset8021asRevConfig", (void*)&msdPtp8021asRevConfigSet,			    "", NULL },
	{ "ptpsetTaiTrigGenTimerSel", (void*)&msdTaiTrigGenTimerSelSet,             "ptpgetTaiTrigGenTimerSel", (void*)&msdTaiTrigGenTimerSelGet  },

    /*LED*/
    { "ledforceOn", (void*)&msdLedForceOn,										"", NULL },
    { "ledforceOff", (void*)&msdLedForceOff,									"", NULL },
    { "ledsetLinkAct", (void*)&msdLedLinkAct,									"", NULL },
    { "ledsetMode", (void*)&msdLedModeSet,										"ledgetMode", (void*)&msdLedModeGet },
    { "ledsetBlinkRate", (void*)&msdLedBlinkRateSet,							"ledgetBlinkRate", (void*)&msdLedBlinkRateGet },
    { "ledsetPulseStretch", (void*)&msdLedPulseStretchSet,						"ledgetPulseStretch", (void*)&msdLedPulseStretchGet },
    { "ledsetSpecialMode", (void*)&msdLedSpecialModeSet,						"ledgetSpecialMode", (void*)&msdLedSpecialModeGet },
    { "ledsetCtrl", (void*)&msdLedCtrlSet,										"ledgetCtrl", (void*)&msdLedCtrlGet },
	{ "leddump", (void*)&msdLedDump,                                             "", NULL },

    /*EEPROM*/
    { "eepromwrite", (void*)&msdEEPROMWrite,									"eepromread", (void*)&msdEEPROMRead },
    { "eepromsetChipSel", (void*)&msdEEPROMChipSelSet,							"eepromgetChipSel", msdEEPROMChipSelGet },

    /*Policy*/
    { "policysetPMC", (void*)&msdPolicyMgmtCtrlSet,								"policygetPMC", (void*)&msdPolicyMgmtCtrlGet },
    { "policysetPCPUDest", (void*)&msdPolicyCPUDestSet,							"policygetPCPUDest", (void*)&msdPolicyCPUDestGet },
    { "policysetPIMD", (void*)&msdPolicyIngressMonitorDestSet,					"policygetPIMD", (void*)&msdPolicyIngressMonitorDestGet },
    { "policysetPRsvd2CpuEn", (void*)&msdPolicyRsvd2CpuEnablesSet,				"policygetPRsvd2CpuEn", (void*)&msdPolicyRsvd2CpuEnablesGet },
    { "policysetPMGMTPri", (void*)&msdPolicyMGMTPriSet,							"policygetPMGMTPri", (void*)&msdPolicyMGMTPriGet },
    { "policysetRouteDest", (void*)&msdPolicyRouteDestSet,						"policygetRouteDest", (void*)&msdPolicyRouteDestGet },
    { "policysetMirrorDest", (void*)&msdPolicyMirrorDestSet,					"policygetMirrorDest", (void*)&msdPolicyMirrorDestGet },
    { "policysetIllegalIPAddr", (void*)&msdPolicyIllegalIPAddrSet,				"policygetIllegalIPAddr", (void*)&msdPolicyIllegalIPAddrGet },
    { "policysetDipDaMismatch", (void*)&msdPolicyDipDaMismatchSet,				"policygetDipDaMismatch", (void*)&msdPolicyDipDaMismatchGet },
    { "policysetZeroSIP", (void*)&msdPolicyZeroSIPSet,							"policygetZeroSIP", (void*)&msdPolicyZeroSIPGet },
    { "policysetMcTtlExceeded", (void*)&msdPolicyMcTtlExceededSet,				"policygetMcTtlExceeded", (void*)&msdPolicyMcTtlExceededGet },
    { "policysetUcTtlExceeded", (void*)&msdPolicyUcTtlExceededSet,				"policygetUcTtlExceeded", (void*)&msdPolicyUcTtlExceededGet },
    { "policysetTtlThreshold", (void*)&msdPortTtlThresholdSet,					"policygetTtlThreshold", (void*)&msdPortTtlThresholdGet },
    { "policysetIpHdrErr", (void*)&msdPolicyIpHdrErrSet,						"policygetIpHdrErr", (void*)&msdPolicyIpHdrErrGet },
    { "policysetTCPFinUrgPsh", (void*)&msdPolicyTCPFinUrgPshSet,				"policygetTCPFinUrgPsh", (void*)&msdPolicyTCPFinUrgPshGet },
    { "policysetTCPFlagsSeqNum", (void*)&msdPolicyTCPFlagsSeqNumSet,			"policygetTCPFlagsSeqNum", (void*)&msdPolicyTCPFlagsSeqNumGet },
    { "policysetTCPSyn", (void*)&msdPolicyTCPSYNSet,							"policygetTCPSyn", (void*)&msdPolicyTCPSYNGet },
    { "policysetTCPHSize", (void*)&msdPolicyTCPHSizeSet,						"policygetTCPHSize", (void*)&msdPolicyTCPHSizeGet },
    { "policysetTCPMinHdrSize", (void*)&msdPortTCPMinHdrSizeSet,				"policygetTCPMinHdrSize", (void*)&msdPortTCPMinHdrSizeGet },
    { "policysetICMPSize", (void*)&msdPolicyICMPSizeSet,						"policygetICMPSize", (void*)&msdPolicyICMPSizeGet },
    { "policysetICMPMaxSize", (void*)&msdPortICMPMaxSizeSet,					"policygetICMPMaxSize", (void*)&msdPortICMPMaxSizeGet },
    { "policysetUDPPort", (void*)&msdPolicyUDPPortSet,							"policygetUDPPort", (void*)&msdPolicyUDPPortGet },
    { "policysetTCPPort", (void*)&msdPolicyTCPPortSet,							"policygetTCPPort", (void*)&msdPolicyTCPPortGet },
    { "policysetTCPSynFin", (void*)&msdPolicyTCPSynFinSet,						"policygetTCPSynFin", (void*)&msdPolicyTCPSynFinGet },
    { "policysetTCPFrag", (void*)&msdPolicyTCPFragSet,							"policygetTCPFrag", (void*)&msdPolicyTCPFragGet },
    { "policysetNearNonTPMR", (void*)&msdPolicyNearNonTPMRSet,					"policygetNearNonTPMR", (void*)&msdPolicyNearNonTPMRGet },
    { "policysetETag", (void*)&msdPolicyETagSet,								"policygetETag", (void*)&msdPolicyETagGet },
    { "policysetNonETag", (void*)&msdPolicyNonETagSet,							"policygetNonETag", (void*)&msdPolicyNonETagGet },
	{ "policyDump", (void*)&msdPolicyDump,                                       "", NULL },

	/*Qci methods*/
	{ "qcisetStreamFilter", (void*)&msdStreamFilterSet,                         "", NULL },
	{ "qcisetStreamFilterBlockStatus", (void*)&msdStreamFilterBlockStatusSet,   "qcigetStreamFilterBlockStatus", (void*)&msdStreamFilterBlockStatusGet },
	{ "qcisetAtsId", (void*)&msdStreamFilterEnAtsSet,                           "", NULL },
	{ "qcisetStreamGateId", (void*)&msdStreamFilterEnGateSet,                   "", NULL },
	{ "", NULL,                                                                 "qcigetStreamFilterMatchedCounter", (void*)&msdStreamFilterMatchCounterGet },
	{ "", NULL,                                                                 "qcigetStreamFilterFilteredCounter", (void*)&msdStreamFilterFiltedCounterGet },
	{ "", NULL,                                                                 "qcigetStreamGateFilteredCounter", (void*)&msdStreamGateFiltedCounterGet },
	{ "", NULL,                                                                 "qcigetFlowMeterFilteredCounter", (void*)&msdFlowMeterFiltedCounterGet },
	{ "qcisetStreamGateDefState", (void*)&msdStreamGateDefStateSet,             "", NULL },
	{ "qcisetStreamGateScale", (void*)&msdStreamGateScaleSet,                   "", NULL },
	{ "qcisetStreamGateTimeGenId", (void*)&msdStreamGateTimeGenIdSet,           "", NULL },
	{ "qcisetStreamGateTimeGen", (void*)&msdStreamGateTimeGenSet,               "", NULL },
	{ "qcisetFlowMeterCouplingFlag", (void*)&msdFlowMeterCouplingFlagSet,       "qcigetFlowMeterCouplingFlag", (void*)&msdFlowMeterCouplingFlagGet },
	{ "qcisetFlowMeterDropOnYel", (void*)&msdFlowMeterDropOnYelSet,             "qcigetFlowMeterDropOnYel", (void*)&msdFlowMeterDropOnYelGet },
	{ "qcisetFlowMeterRedBlockEn", (void*)&msdFlowMeterRedBlockEnSet,           "qcigetFlowMeterRedBlockEn", (void*)&msdFlowMeterRedBlockEnGet },
	{ "qcisetFlowMeterRedBlockStatus", (void*)&msdFlowMeterRedBlockStatusSet,   "qcigetFlowMeterRedBlockStatus", (void*)&msdFlowMeterRedBlockStatusGet },
	{ "qciflowMeterAdvConfig", (void*)&msdFlowMeterAdvConfig,                   "", NULL },

    /*Qcr*/
    { "qcrsetGrpId", (void*)&msdQcrSetGrpId,                                    "qcrgetGrpId", (void*)&msdQcrGetGrpId },
    { "qcrsetCbsLimit", (void*)&msdQcrSetCbsLimit,                              "qcrgetCbsLimit", (void*)&msdQcrGetCbsLimit },
    { "qcrsetRateLimit", (void*)&msdQcrSetRateLimit,                            "qcrgetRateLimit", (void*)&msdQcrGetRateLimit },
    { "qcrsetMaxTime", (void*)&msdQcrSetMaxResidenceTime,                       "qcrgetMaxTime", (void*)&msdQcrGetMaxResidenceTime },
    { "qcrsetAtsQueue", (void*)&msdQcrSetQueueEnables,                          "qcrgetAtsQueue", (void*)&msdQcrGetQueueEnables },

    { "", NULL, "", NULL }
};

CLI_COMMAND_STRUCT fdbCMDList[] =
{
    { "addEntry", &atuAddEntry },
    { "getNext", &atuGetNextEntry },
    { "delAll", &atuFlush },
    { "delAllInDB", &atuFlushInDB },
    { "move", &atuMove },
    { "moveInDB", &atuMoveInDB },
    { "delEntry", &atuDelEntry },
    { "getVio", &atuGetVio },
    { "findEntry", &atuFindEntry },
    { "setAging", &setU32 },
    { "getAging", &getPU32 },

    { "portRemove", &atuPortRemove },
    { "portRemoveInDB", &portRemoveINDB },
    { "getLearn2All", &getPU32 },
    { "setLearn2All", &setU32 },
    { "setPortLearnLimit", &setPortU32 },
    { "getPortLearnLimit", &getPortPU32 },
    { "getLearnCount", &getPortPU32 },
    { "getCount", &getPU32 },
    { "getCountPerFid", &getPortPU32 },
    { "getNonStaticCount", &getPU32 },
    { "getNonStaticCountPerFid", &getPortPU32 },
    { "dump", &setU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT ecidCMDList[] =
{
    { "setBpeEnable", &setU32 },
    { "getBpeEnable", &getPU32 },
    { "addEntry", &ecidAddEntry },
    { "delAll", &ecidDelAll },
    { "move", &setPortU32 },
    { "delEntry", &setPortU32 },
    { "getNext", &ecidGetNextEntry },
    { "findEntry", &ecidFindEntry },
    { "getCount", &ecidGetCount },
    { "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT stuCMDList[] =
{
    { "addEntry", &stuAddEntry },
    { "getNext", &stuGetNext },
    { "findEntry", &stuFindEntry },
    { "delEntry", &stuDelEntry },
    { "getCount", &stuGetEntryCount },
    { "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT vtuCMDList[] =
{
    { "addEntry", &vtuAddEntry },
    { "delAll", &vtuFlush },
    { "findEntry", &vtuFindEntry },
    { "isExist", &vutIsExist },
    { "getNext", &vtuGetNext },
    { "delEntry", &vtuDelEntry },
    { "setMode", &vtuSetMode },
    { "getMode", &vtuGetMode },
    { "getVio", &vtuGetVio },
    { "getCount", &vtuGetCount },
    { "setMemberTag", &vtuSetMemberTag },
    { "getMemberTag", &vtuGetMemberTag },
    { "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT impCMDList[] =
{
    { "run", &IMPRun },
    { "reset", &IMPReset },
    { "stop", &IMPStop },
    { "writeEEPROM", &IMPWriteEEPROM },
    { "loadToEEPROM", &IMPLoadToEEPROM },
    { "loadToRAM", &IMPLoadToRAM },
    { "", NULL },
};

CLI_COMMAND_STRUCT intCMDList[] =
{
	{ "setphyIntEnable", &setPortU32 },
	{ "getphyIntEnable", &getPortPU32 },
	{ "getphyIntStatus", &phyGetIntStatus },

	{ "setDevIntEnable", &setU32 },
	{ "getDevIntEnable", &getPU32 },
	{ "getDevIntStatus", &devGetIntStatus },

	{ "setActiveIntEnable", &setU32 },
	{ "getActiveIntEnable", &getPU32 },
	{ "getActiveIntStatus", &activeGetIntStatus },
	{ "", NULL },
};

CLI_COMMAND_STRUCT phyCMDList[] =
{
    { "reset", &phyReset },
    { "setLoopback", &setPortU32 },
    { "setSpeed", &setPortU32 },
    { "powerDown", &setPortU32 },
    { "setDuplex", &setPortU32 },
    { "setAutoMode", &setPortU32 },
    { "setEEE", &setPortU32 },
	{ "setFlowControl", &setPortU32 },

    { "rr", &getPortU32PU32 },
    { "rw", &setPortU32U32 },

	{ "xrr", &getPortU32U32PU32 },
	{ "xrw", &setPortU32U32U32 },

	{ "extrr", &getPortU32PU32 },
	{ "extrw", &setPortU32U32 },

	{ "extxrr", &getPortU32U32PU32 },
	{ "extxrw", &setPortU32U32U32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT serdesCMDList[] =
{
    { "reset", &setU32},

    { "xrr", &getPortU32U32PU32 },
    { "xrw", &setPortU32U32U32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT irlCMDList[] =
{
    { "reset", &irlInit },
    { "initResCount", &irlInitRes },
    { "customSetup", &irlCustomSetup },
    { "readRes", &irlReadRes },
    { "writeRes", &irlWriteRes },
    { "rr", &irlRReg },
    { "rw", &irlWReg },
    { "advConfig", &irlAdvConfig },
    { "disableAdvConfig", &irlAdvConfigDisable },
    { "", NULL },
};

CLI_COMMAND_STRUCT vctCMDList[] =
{
    { "cableDiag", &vctCableDiag },
    { "extenedStatus", &vctExtenedStatus },
    { "", NULL },
};

CLI_COMMAND_STRUCT qavCMDList[] =
{
    { "setXRate", &setPortU32U32 },
    { "getXRate", &getPortU32PU32 },
    { "setXHiLimit", &setPortU32U32 },
    { "getXHiLimit", &getPortU32PU32 },
    { "", NULL },
};

CLI_COMMAND_STRUCT ptpCMDList[] =
{
    { "setPortPTPEn", &setPortU32 },
    { "getPortPTPEn", &getPortPU32 },
    { "setIntEn", &ptpSetIntEn },
    { "getIntEn", &ptpGetIntEn },
    { "setPortConfig", &ptpSetPortConfig },
    { "getPortConfig", &ptpGetPortConfig },
    { "getTimeStamp", &ptpGetTimeStamp },
    { "getTSValid", &getPortU32PU32 },
    { "resetTS", &setPortU32 },
    { "setGlobalConfig", &ptpSetGlobalConfig },
    { "getGlobalConfig", &ptpGetGlobalConfig },
    { "getIntStatus", &ptpGetIntStatus },
    { "setMeanPathDelay", &setPortU32 },
    { "getMeanPathDelay", &getPortPU32 },
    { "setIgrPathDelayAsym", &setPortU32 },
    { "getIgrPathDelayAsym", &getPortPU32 },
    { "setEgrPathDelayAsym", &setPortU32 },
    { "getEgrPathDelayAsym", &getPortPU32 },
    { "setPtpMode", &ptpSetPtpMode },
    { "getPtpMode", &ptpGetPtpMode },
    { "setMasterSlave", &setU32 },
    { "getMasterSlave", &getPU32 },
    { "getTODBusyBit", &getPU32 },
    { "storeCompensation", &ptpStoreCompensation },
    { "storeAll", &ptpStoreAll },
    { "captureAll", &ptpCaptureAll },
    { "setPtpPulse", &ptpSetPtpPulse },
    { "getPtpPulse", &ptpGetPtpPulse },
    { "getPTPGlobalTime", &getPU32 },
    { "getPortReg", &getPortU32PU32 },
    { "setPortReg", &setPortU32U32 },
    { "getGlobalReg", &getU32PU32 },
    { "setTaiReg", &setU32U32 },
    { "getTaiReg", &getU32PU32 },
    { "setGlobalReg", &setU32U32 },
    { "setPreemptTimestampSel", &setPortU32 },
    { "getPreemptTimestampSel", &getPortPU32 },
    { "setPreemptCtrl", &setPortU32 },
    { "getPreemptCtrl", &getPortPU32 },
    { "set1588v2Config", &setPortU32U32U32 },
    { "set8021asConfig", &setPortU32U32 },
    { "set8021asRevConfig", &setPortU32U32 },
    { "setTaiTrigGenTimerSel", &setU32 },
    { "getTaiTrigGenTimerSel", &getPU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT qosCMDList[] =
{
    { "setPrioMap", &qosSetPrioMap },
    { "getPrioMap", &qosGetPrioMap },
    { "setCosQueue", &setPortU32U32 },
    { "getCosQueue", &getPortU32PU32 },
    { "setCosDscp", &setPortU32U32 },
    { "getCosDscp", &getPortU32PU32 },
    { "setPriMapRule", &setPortU32 },
    { "getPriMapRule", &getPortPU32 },
    { "setInitPri", &setPortU32 },
    { "getInitPri", &getPortPU32 },
    { "setDefQPri", &setPortU32 },
    { "getDefQPri", &getPortPU32 },
    { "setDefFPri", &setPortU32 },
    { "getDefFPri", &getPortPU32 },
    { "enRewriteDscp", &setPortU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT tcamCMDList[] =
{
    { "delAll", &tcamFlushAll },
    { "delEntry", &tcamFlushEntry },
    { "addEntry", &tcamAddEntry },
    { "modifyEntry", &tcamModifyEntry },
    { "copyEntry", &tcamCopyEntry },
    { "readEntry", &tcamReadEntry },
    { "getNextEntry", &tcamGetNextEntry },
    { "findEntry", &tcamFindEntry },
    { "setMode", &tcamSetMode },
    { "getMode", &tcamGetMode },
    { "addPort", &tcamAddPort },
    { "removePort", &tcamRemovePort },
    { "egrDelAllPorts", &tcamEgrDelAllPorts },
    { "egrDelEntry", &tcamEgrDelEntry },
    { "egrAddEntry", &tcamEgrAddEntry },
    { "egrReadEntry", &tcamEgrReadEntry },
    { "egrGetNextEntry", &tcamEgrGetNextEntry },
    { "advanceConfig", &tcamAdvConfig },
    { "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT tcamrcCMDList[] =
{
    { "delAll", &tcamrcFlushAll },
    { "delEntry", &tcamrcFlushEntry },
    { "addEntry", &tcamrcAddEntry },
    { "readEntry", &tcamrcReadEntry },
    { "getNextEntry", &tcamrcGetNextEntry },
    { "findEntry", &tcamrcFindEntry },
    { "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT tcamrccompCMDList[] =
{
    { "addRCComp", &tcamrccompCompAdd },
    { "readRCComp", &tcamrccompCompRead },
	{ "delAll", &tcamrccompFlushAll },
	{ "delEntry", &tcamrccompFlushPort },

    { "", NULL },
};

CLI_COMMAND_STRUCT arpCMDList[] =
{
    { "delAll", arpFlushAll },
    { "delEntry", arpFlushEntry },
    { "addMcEntry", arpAddMcEntry },
    { "addUcEntry", arpAddUcEntry },
    { "readEntry", arpReadEntry },
    { "getNextEntry", arpGetNextEntry },

    { "", NULL },
};

CLI_COMMAND_STRUCT rmuCMDList[] =
{
    { "getID", &rmuGetID },
    { "dumpAtu", &rmuDumpAtu },
    { "dumpEcid", &rmuDumpEcid },
    { "dumpMib2", &rmuDumpMib2 },
    { "dumpTcam", &rmuDumpTcam },
    { "dumpMib", &rmuDumpMib },
    { "tcamRead", &rmuTcamRead },
    { "tcamWrite", &rmuTcamWrite },
    { "dumpReg", &rmuDumpReg },

    { "", NULL },
};

CLI_COMMAND_STRUCT globalCtrlCMDList[] =
{
    { "setFloodBC", &setU32 },
    { "getFloodBC", &getPU32 },
    { "setCPUDest", &setU32 },
    { "getCPUDest", &getPU32 },
    { "setIMD", &setU32 },
    { "getIMD", &getPU32 },
    { "setEMD", &setU32 },
    { "getEMD", &getPU32 },
    { "setMMC", &setPortU32 },
    { "getMMC", &getPortPU32 },
    { "setRsvd2CpuEn", &setU32 },
    { "getRsvd2CpuEn", &getPU32 },
    { "setMGMTPri", &setU32 },
    { "getMGMTPri", &getPU32 },
    { "setSMISetup", &setPortU32 },
    { "getSMISetup", &getPortPU32 },
    { "dumpGlobal1", &setDev },
    { "dumpGlobal2", &setDev },
	{ "setRMUMode", &setU32 },
	{ "getRMUMode", &getPU32 },

    { "", NULL },
};

#if 1
CLI_COMMAND_STRUCT qbvCMDList[] =
{
	{ "Write", &setTBPortU32U32 },
	{ "Read", &getTBPortU32PU32 },
	{ "", NULL },
};
#else
CLI_COMMAND_STRUCT qbvCMDList[] =
{
	{ "Write", &setPortU32U32 },
	{ "Read", &getPortU32PU32 },

	{ "", NULL },
};
#endif

CLI_COMMAND_STRUCT portCtrlCMDList[] =
{
	{ "setForceDefVid", &setPortU32 },
	{ "getForceDefVid", &getPortPU32 },
    { "setDefVid", &setPortU32 },
    { "getDefVid", &getPortPU32 },
    { "setQMode", &setPortU32 },
    { "getQMode", &getPortPU32 },
    { "setDiscardTagged", &setPortU32 },
    { "getDiscardTagged", &getPortPU32 },
    { "setDiscardUTagged", &setPortU32 },
    { "getDiscardUTagged", &getPortPU32 },
    { "setUCFloodBlock", &setPortU32 },
    { "getUCFloodBlock", &getPortPU32 },
    { "setMCFloodBlock", &setPortU32 },
    { "getMCFloodBlock", &getPortPU32 },
    { "setBCMode", &setPortU32 },
    { "getBCMode", &getPortPU32 },
    { "setIGMPSnoopEnable", &setPortU32 },
    { "getIGMPSnoopEnable", &getPortPU32 },
    { "setHeaderMode", &setPortU32 },
    { "getHeaderMode", &getPortPU32 },
    { "setDropOnLock", &setPortU32 },
    { "getDropOnLock", &getPortPU32 },
    { "setMtu", &setPortU32 },
    { "getMtu", &getPortPU32 },
	{ "setSpecifiedMtu", &setPortU32 },
	{ "getSpecifiedMtu", &getPortPU32 },
    { "setLearnEnable", &setPortU32 },
    { "getLearnEnable", &getPortPU32 },
    { "setVTUPriOverride", &setPortU32 },
    { "getVTUPriOverride", &getPortPU32 },
    { "setSAPriOverride", &setPortU32 },
    { "getSAPriOverride", &getPortPU32 },
    { "setDAPriOverride", &setPortU32 },
    { "getDAPriOverride", &getPortPU32 },
    { "setMsgPort", &setPortU32 },
    { "getMsgPort", &getPortPU32 },
    { "setEtherType", &setPortU32 },
    { "getEtherType", &getPortPU32 },
    { "setAllowVidZero", &setPortU32 },
    { "getAllowVidZero", &getPortPU32 },
    { "setEgrRate", &setPortU32U32 },
    { "getEgrRate", &getPortPU32PU32 },
    { "setFlowCtrl", &setPortU32U32 },
    { "getFlowCtrl", &getPortPU32PU32 },
    { "setEMS", &setPortU32 },
    { "getEMS", &getPortPU32 },
    { "setIMS", &setPortU32 },
    { "getIMS", &getPortPU32 },
    { "setForceDuplexMode", &setPortU32 },
    { "getForceDuplexMode", &getPortPU32 },
    { "getDuplex", &getPortPU32 },
    { "setForceLinkMode", &setPortU32 },
    { "getForceLinkMode", &getPortPU32 },
    { "getLinkState", &getPortPU32 },
    { "setForceSpeed", &setPortU32 },
    { "getForceSpeed", &getPortPU32 },
    { "getSpeed", &getPortPU32 },
    { "setPortBasedVlanMap", &portSetVlanPorts },
    { "getPortBasedVlanMap", &portGetVlanPorts },
    { "setFrameMode", &setPortU32 },
    { "getFrameMode", &getPortPU32 },
	{ "setEgressMode", &setPortU32 },
	{ "getEgressMode", &getPortPU32 },
	{ "setECID", &setPortU32 },
	{ "getECID", &getPortPU32 },
	{ "setIgnoreETag", &setPortU32 },
	{ "getIgnoreETag", &getPortPU32 },
	{ "setExtendedPortCtrlReg", &setPortU32U32U32U32 },
	{ "getExtendedPortCtrlReg", &getPortU32U32U32PU32 },
    { "setPortState", &setPortU32 },
    { "getPortState", &getPortPU32 },
    { "setIgnoreFcs", &setPortU32 },
    { "getIgnoreFcs", &getPortPU32 },
    { "setAllowBadFcs", &setPortU32 },
    { "getAllowBadFcs", &getPortPU32 },
    { "setPauseLimitOut", &setPortU32 },
    { "getPauseLimitOut", &getPortPU32 },
    { "setPauseLimitIn", &setPortU32 },
    { "getPauseLimitIn", &getPortPU32 },
    { "setQueueToPause", &setPortU32 },
    { "getQueueToPause", &getPortPU32 },
    { "setRxPriFCEnable", &setPortU32U32 },
    { "getRxPriFCEnable", &getPortU32PU32 },
    { "setRxPriFCToQueue", &setPortU32U32 },
    { "getRxPriFCToQueue", &getPortU32PU32 },
    { "setTxPriFCEnable", &setPortU32U32 },
    { "getTxPriFCEnable", &getPortU32PU32 },
    { "getOutQueueSize", &getPortU32PU32 },
    { "setMapDA", &setPortU32 },
    { "getMapDA", &getPortPU32 },
    { "setARPMirror", &setPortU32 },
    { "getARPMirror", &getPortPU32 },
    { "setLocked", &setPortU32 },
    { "getLocked", &getPortPU32 },
    { "setHoldAt1", &setPortU32 },
    { "getHoldAt1", &getPortPU32 },
    { "setIntOnAgeOut", &setPortU32 },
    { "getIntOnAgeOut", &getPortPU32 },
    { "setRefreshLocked", &setPortU32 },
    { "getRefreshLocked", &getPortPU32 },
    { "setIgnoreWrongData", &setPortU32 },
    { "getIgnoreWrongData", &getPortPU32 },
    { "setCutThroughEnable", &portSetCutThrough },
    { "getCutThroughEnable", &portGetCutThrough },
    { "getPortStatus", &portGetPortStatus },
	{ "setPreemptEnable", &setPortU32 },
	{ "getPreemptEnable", &getPortPU32 },
	{ "setPreemptSize", &setPortU32 },
	{ "getPreemptSize", &getPortPU32 },
	{ "setPreemptQueue", &setPortU32U32 },
	{ "getPreemptQueue", &getPortU32PU32 },

    { "dump", &setU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT mibCMDList[] =
{
    { "flush", &mibFlushAll },
    { "flushport", &mibFlushPort },
    { "getPortCounter", &mibGetPortCounter },
    { "getPortAllCounter", &mibGetPortAllCounters },
    { "getHistogramMode", &getPU32 },
    { "setHistogramMode", &setU32 },
    { "dump", &mibDump },

    { "", NULL },
};

CLI_COMMAND_STRUCT queueCtrlCMDList[] =
{

    { "setQc", &setPortU32U32 },
    { "getQc", &getPortU32PU32 },
    { "setPortSched", &setPortU32 },
    { "getPortSched", &getPortPU32 },
    { "setQosWeight", &qcSetQosWeight },
    { "getQosWeight", &qcGetQosWeight },
    { "setPortQosWeight", &qcSetPortQosWeight },
    { "getPortQosWeight", &qcGetPortQosWeight },

    { "", NULL },
};

CLI_COMMAND_STRUCT trunkCMDList[] =
{
    { "setTrunkMask", &setPortU32 },
    { "getTrunkMask", &getPortPU32 },
    { "setTrunkHash", &setU32 },
    { "getTrunkHash", &getPU32 },
    { "setTrunkRoute", &setPortU32 },
    { "getTrunkRoute", &getPortPU32 },
	{ "setTrunkMap", &setPortU32 },
	{ "getTrunkMap", &getPortPU32 },
    { "setLAGPort", &setPortU32U32 },
    { "getLAGPort", &getPortPU32PU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT ledCMDList[] =
{
    { "forceOn", &setPortU32 },
    { "forceOff", &setPortU32 },
    { "setLinkAct", &setPortU32 },
    { "setMode", &setPortU32U32 },
    { "getMode", &getPortU32PU32 },
    { "setBlinkRate", &setPortU32 },
    { "getBlinkRate", &getPortPU32 },
    { "setPulseStretch", &setPortU32 },
    { "getPulseStretch", &getPortPU32 },
    { "setSpecialMode", &setPortU32 },
    { "getSpecialMode", &getPortPU32 },
    { "setCtrl", &setPortU32U32 },
    { "getCtrl", &getPortU32PU32 },
	{ "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT eepromCMDList[] =
{
    { "write", &setPortU32 },
    { "read", &getPortPU32 },
    { "setChipSel", &setU32 },
    { "getChipSel", &getPU32 },

    { "", NULL },
};

CLI_COMMAND_STRUCT policyCMDList[] =
{
    { "setPCPUDest", &setPortU32 },
    { "getPCPUDest", &getPortPU32 },
    { "setPIMD", &setPortU32 },
    { "getPIMD", &getPortPU32 },
    { "setPMC", &setPortU32U32 },
    { "getPMC", &getPortU32PU32 },
    { "setPRsvd2CpuEn", &setPortU32 },
    { "getPRsvd2CpuEn", &getPortPU32 },
    { "setPMGMTPri", &setPortU32 },
    { "getPMGMTPri", &getPortPU32 },
    { "setRouteDest", &setPortU32 },
    { "getRouteDest", &getPortPU32 },
    { "setMirrorDest", &setPortU32 },
    { "getMirrorDest", &getPortPU32 },
    { "setIllegalIPAddr", &setPortU32 },
    { "getIllegalIPAddr", &getPortPU32 },
    { "setDipDaMismatch", &setPortU32 },
    { "getDipDaMismatch", &getPortPU32 },
    { "setZeroSIP", &setPortU32 },
    { "getZeroSIP", &getPortPU32 },
    { "setMcTtlExceeded", &setPortU32 },
    { "getMcTtlExceeded", &getPortPU32 },
    { "setUcTtlExceeded", &setPortU32 },
    { "getUcTtlExceeded", &getPortPU32 },
    { "setTtlThreshold", &setPortU32 },
    { "getTtlThreshold", &getPortPU32 },
    { "setIpHdrErr", &setPortU32 },
    { "getIpHdrErr", &getPortPU32 },
    { "setTCPFinUrgPsh", &setPortU32 },
    { "getTCPFinUrgPsh", &getPortPU32 },
    { "setTCPFlagsSeqNum", &setPortU32 },
    { "getTCPFlagsSeqNum", &getPortPU32 },
    { "setTCPSyn", &setPortU32 },
    { "getTCPSyn", &getPortPU32 },
    { "setTCPHSize", &setPortU32 },
    { "getTCPHSize", &getPortPU32 },
    { "setTCPMinHdrSize", &setPortU32 },
    { "getTCPMinHdrSize", &getPortPU32 },
    { "setICMPSize", &setPortU32 },
    { "getICMPSize", &getPortPU32 },
    { "setICMPMaxSize", &setPortU32 },
    { "getICMPMaxSize", &getPortPU32 },
    { "setUDPPort", &setPortU32 },
    { "getUDPPort", &getPortPU32 },
    { "setTCPPort", &setPortU32 },
    { "getTCPPort", &getPortPU32 },
	{ "setTCPSynFin", &setPortU32 },
	{ "getTCPSynFin", &getPortPU32 },
	{ "setTCPFrag", &setPortU32 },
	{ "getTCPFrag", &getPortPU32 },
	{ "setNearNonTPMR", &setPortU32 },
	{ "getNearNonTPMR", &getPortPU32 },
	{ "setETag", &setPortU32 },
	{ "getETag", &getPortPU32 },
    { "setNonETag", &setPortU32 },
	{ "getNonETag", &getPortPU32 },
	{ "dump", &setDev },

    { "", NULL },
};

CLI_COMMAND_STRUCT qciCMDList[] =
{
	{ "delAllStreamFilter", &qciStreamFilterFlushAll },
	{ "setStreamFilter", &setPortU32U32 },
	{ "setStreamFilterBlockStatus", &setPortU32 },
	{ "getStreamFilterBlockStatus", &getPortPU32 },
	{ "setAtsId", &setPortU32 },
	{ "setStreamGateId", &setPortU32 },
	{ "getStreamFilterMatchedCounter", &getPortPU32 },
	{ "getStreamFilterFilteredCounter", &getPortPU32 },
	{ "getStreamGateFilteredCounter", &getPortPU32 },
    { "getFlowMeterFilteredCounter", &getPortPU32 },
	{ "delAllStreamGate", &qciStreamGateFlushAll },
	{ "setStreamGateDefState", &setPortU32U32U32 },
	{ "setStreamGateScale", &setPortU32U32 },
	{ "setStreamGateTimeGenId", &setPortU32 },
	{ "addStreamGateEntry", &qciStreamGateAddEntry },
	{ "delAllStreamGateTimeGen", &qciStreamGateTimeGenFlushAll },
	{ "setStreamGateTimeGen", &setPortU32U64 },
	{ "delAllFlowMeter", &qciFlowMeterFlushAll },
	{ "setFlowMeterCouplingFlag", &setPortU32 },
	{ "getFlowMeterCouplingFlag", &getPortPU32 },
	{ "setFlowMeterDropOnYel", &setPortU32 },
	{ "getFlowMeterDropOnYel", &getPortPU32 },
	{ "setFlowMeterRedBlockEn", &setPortU32 },
	{ "getFlowMeterRedBlockEn", &getPortPU32 },
	{ "setFlowMeterRedBlockStatus", &setPortU32 },
	{ "getFlowMeterRedBlockStatus", &getPortPU32 },
	{ "flowMeterAdvConfig", &setPortU32U32U32 },

	{ "", NULL },
};

CLI_COMMAND_STRUCT umsdApiCMDList[] =
{
    {"search", &umsdApiSearch},
    {"man", &umsdApiMan},

    {"", NULL}
};

CLI_COMMAND_STRUCT directCMDList[] =
{
    { "rr", &ReadRegister },
    { "rw", &WriteRegister },
	{ "fullChipReset", &fullChipReset },
#ifdef MSD_DEBUG_QD
	{ "debugLevelSet", &debugLvlSet },
#endif
    { "file", &fileOperation },
    { "getBus", &getBusInterface },
    { "getVersion", &getVersion },
#ifdef UNITTEST
    { "unittest", &UnitTestCases },
    { "sample", &SampleCases },
#endif
    { "customized", &CustomizedCases },
    { "enableHwSem", &enableHwSemaphore},
    { "disableHwSem", &disableHwSemaphore},

    { "", NULL },
};

CLI_COMMAND_STRUCT qcrCMDList[] =
{
    { "setGrpId", (void*)&setPortU32 },
    { "getGrpId", (void*)&getPortPU32 },
    { "setCbsLimit", (void*)&setPortU32 },
    { "getCbsLimit", (void*)&getPortPU32 },
    { "setRateLimit", (void*)&setPortU32 },
    { "getRateLimit", (void*)&getPortPU32 },
    { "setMaxTime", (void*)&setPortU32U32 },
    { "getMaxTime", (void*)&getPortPU32PU32 },
    { "setAtsQueue", (void*)&setPortU32U32 },
    { "getAtsQueue", (void*)&getPortU32PU32 },
    { "delAll", (void*)&qcrFlushAll },
    { "delEntry", (void*)&qcrFlushOne },

    { "", NULL },
};

CLI_COMMAND_STRUCT frerCMDList[] =
{  
    { "seqGenFlushAll", (void*)&frerSeqGenFlushAll },
    { "seqGenFlushOne", (void*)&frerSeqGenFlushOne },
    { "seqGenAddEntry", (void*)&frerSeqGenLoad },
    { "seqGenReadEntry", (void*)&frerSeqGenRead },

    { "indvRcvyFlushAll", (void*)&frerIndvRcvyFlushAll },
    { "indvRcvyFlushOne", (void*)&frerIndvRcvyFlushOne },
    { "indvRcvyAddEntry", (void*)&frerIndvRcvyLoad },
    { "indvRcvyReadEntry", (void*)&frerIndvRcvyRead },
    { "indvRcvyGetNext", (void*)&frerIndvRcvyGetNext },
    { "indvRcvyReadClear", (void*)&frerIndvRcvyReadClear },

    { "seqRcvyFlushAll", (void*)&frerSeqRcvyFlushAll },
    { "seqRcvyFlushOne", (void*)&frerSeqRcvyFlushOne },
    { "seqRcvyAddEntry", (void*)&frerSeqRcvyLoad },
    { "seqRcvyReadEntry", (void*)&frerSeqRcvyRead },
    { "seqRcvyGetNext", (void*)&frerSeqRcvyGetNext },
    { "seqRcvyReadClear", (void*)&frerSeqRcvyReadClear },
    { "seqRcvyGetViolation", (void*)&frerSeqRcvyGetViolation },

    { "bank0SCtrFlushAll", (void*)&frerBank0SCtrFlushAll },
    { "bank0SCtrFlushCtr", (void*)&frerBank0SCtrFlushCtr },
    { "bank0SCtrLoad", (void*)&frerBank0SCtrLoad },
    { "bank0SCtrRead", (void*)&frerBank0SCtrRead },
    { "bank0SCtrGetNext", (void*)&frerBank0SCtrGetNext },
    { "bank0SCtrReadClear", (void*)&frerBank0SCtrReadClear },

    { "bank1SCtrFlushAll", (void*)&frerBank1SCtrFlushAll },
    { "bank1SCtrFlushCtr", (void*)&frerBank1SCtrFlushCtr },
    { "bank1SCtrLoad", (void*)&frerBank1SCtrLoad },
    { "bank1SCtrRead", (void*)&frerBank1SCtrRead },
    { "bank1SCtrGetNext", (void*)&frerBank1SCtrGetNext },
    { "bank1SCtrReadClear", (void*)&frerBank1SCtrReadClear },

    { "", NULL },
};

CLI_COMMAND_STRUCT macsecCMDList[] =
{
	{ "configureEgress", (void*)&macsecConfigureEgress },
	{ "configureIngress", (void*)&macsecConfigureIngress },
	{ "flushAll", (void*)&macsecFlushAll},
    { "createRule", (void*)&macsecCreateRule},
    { "deleteRule", (void*)&macsecDeleteRule},
    { "getRule", (void*)&macsecGetRule},
    { "createTxSecY", (void*)&macsecCreateTxSecY},
	{ "createRxSecY", (void*)& macsecCreateRxSecY},
	{ "deleteTxSecY", (void*)& macsecDeleteTxSecY},
    { "deleteRxSecY", (void*)&macsecDeleteRxSecY},
    { "getTxSecY", (void*)&macsecGetTxSecY},
	{ "getRxSecY", (void*)& macsecGetRxSecY },
    { "createRxSC", (void*)&macsecCreateRxSC},
    { "deleteRxSC", (void*)&macsecDeleteRxSC},
    { "getRxSC", (void*)&macsecGetRxSC},
    { "createRxSA", (void*)&macsecCreateRxSA},
    { "deleteRxSA", (void*)&macsecDeleteRxSA},
    { "getRxSA", (void*)&macsecGetRxSA},
    { "getRxSANextPN", (void*)&macsecGetRxSANextPN},
    { "createTxSC", (void*)&macsecCreateTxSC},
    { "deleteTxSC", (void*)&macsecDeleteTxSC},
    { "getTxSC", (void*)&macsecGetTxSC},
    { "createTxSA", (void*)&macsecCreateTxSA},
    { "deleteTxSA", (void*)&macsecDeleteTxSA},
    { "getTxSA", (void*)&macsecGetTxSA},
    { "getTxSANextPN", (void*)&macsecGetTxSANextPN},
    { "getRXSCStates", (void*)&macsecGetRXSCStates},
    { "getRXSecYStates", (void*)&macsecGetRXSecYStates},
    { "getTXSCStates", (void*)&macsecGetTXSCStates},
    { "getTXSecYStates", (void*)&macsecGetTXSecYStates},
	{ "getAllStates", (void*)& macsecGetAllStates},
    { "enableIngressSet", (void*)&macsecEnableIngressSet},
	{ "enableEgressSet", (void*)& macsecEnableEgressSet},
	{ "enableIngressGet", (void*)& macsecEnableIngressGet},
    { "enableEgressGet", (void*)&macsecEnableEgressGet},
	{ "setTxPNThreshold", (void*)& macsecSetTxPNThreshold},
	{ "getTxPNThreshold", (void*)& macsecGetTxPNThreshold},
    { "setTxXPNThreshold", (void*)&macsecSetTxXPNThreshold},
    { "getTxXPNThreshold", (void*)&macsecGetTxXPNThreshold},
    { "setRxDefaultSCI", (void*)&macsecSetRxDefaultSCI},
    { "getRxDefaultSCI", (void*)&macsecGetRxDefaultSCI},
    { "setTxSCIntCtrl", (void*)&macsecSetTxSCIntCtrl},
    { "getTxSCIntCtrl", (void*)&macsecGetTxSCIntCtrl},
    { "getNextTxSCIntStatus", (void*)&macsecGetNextTxSCIntStatus},
	{ "", NULL },
};

/* CLI module command */
EXTERN_CLI_STRUCT externCLICMDList[] =
{
    { "atu", fdbCMDList },
    { "stu", stuCMDList },
    { "ecid", ecidCMDList },
    { "vlan", vtuCMDList },
    { "imp", impCMDList },
	{ "interrupt", intCMDList },
    { "phy", phyCMDList },
    { "serdes", serdesCMDList },
    { "irl", irlCMDList },
    { "vct", vctCMDList },
    { "qav", qavCMDList },
	{ "qbv", qbvCMDList},
    { "ptp", ptpCMDList },
    { "qos", qosCMDList },
    { "tcam", tcamCMDList },
    { "tcamrc", tcamrcCMDList },
    { "tcamrccomp", tcamrccompCMDList },
    { "arp", arpCMDList },
    { "rmu", rmuCMDList },

    { "trunk", trunkCMDList },
    { "port", portCtrlCMDList },
    { "mib", mibCMDList },
    { "qc", queueCtrlCMDList },
    { "global", globalCtrlCMDList },
    { "led", ledCMDList },
    { "eeprom", eepromCMDList },
    { "policy", policyCMDList },
    { "qcr", qcrCMDList },
    { "qci", qciCMDList },
    { "frer", frerCMDList },
    { "macsec", macsecCMDList},
    { "rr", directCMDList }, /*ReadRegister, NULL, NULL, "rr <devAddr> <regAddr> : Read register", "rr <devAddr> <regAddr> : Read register\n" },*/
    { "rw", directCMDList }, /*WriteRegister, NULL, NULL, "rw <devAddr> <regAddr> <data> : Write register", "rw <devAddr> <regAddr> <data> : Write register\n" },*/
	{ "fullChipReset", directCMDList },
#ifdef MSD_DEBUG_QD
	{ "debugLevelSet", directCMDList },
#endif
    { "file", directCMDList }, /*fileOperation, NULL, NULL, "file <filename> : Load the command from the file", "file <filename> : Load the command from the file\n" },*/
    { "getBus", directCMDList }, /*getBusInterface, NULL, NULL, "getBus : Get the bus interface, SMI, RMU or SMI multichip", "getBus : Get the bus interface, SMI, RMU or SMI multichip\n" },*/
    { "getVersion", directCMDList },
    { "umsd-api", umsdApiCMDList },

#ifdef UNITTEST
    { "unittest", directCMDList }, /*UnitTestCases, NULL, NULL, "unittest   : Run the Unit test cases", "unittest   : Run the Unit test cases\n" },*/
    { "sample", directCMDList }, /*SampleCases, NULL, NULL, "sample     : Run the sample cases", "sample     : Run the sample cases\n" },*/
#endif	           
    { "customized", directCMDList }, /*CustomizedCases, NULL, NULL, "customized : Run customized cases", "customized : Run customized cases\n" },*/

    { "enableHwSem", directCMDList },
    { "disableHwSem", directCMDList },

	/*{ "exec",   execSystemCmd,		"exec <systemCmd> : exec the system command\n"},*/
    { NULL, NULL }
};

void setPrintCallback(PRINT_CALLBACK callback)
{
    printCallback = callback;
}

static void CLIPrint(int level, char* prompt, char* format, ...)
{
    va_list argP;
	char dbgStr[2000] = { 0 };
    FILE *fp;

    va_start(argP, format);

    /*strncpy(dbgStr, prompt, strlen(prompt));*/
	strncpy(dbgStr, prompt, sizeof(dbgStr)-1);
    vsprintf(dbgStr + strlen(prompt), format, argP);

#ifdef J_DEBUG
	level = 1;
#endif

    if (level > 0)
    {
        if (printCallback != NULL)
		{
			printCallback(dbgStr);

		}
        else
		{
            printf("%s", dbgStr);
		}
    }

    if (logOnOffFlag == 1)
    {
        fp = fopen("CLI.log", "a");
        if (fp == NULL)
        {
            return;
        }
        fprintf(fp, "%s%s", prompt, dbgStr);

        fclose(fp);
    }
}

#define CLI_ERROR(format, ...) CLIPrint(1, "(CLI_ERROR: ) ", format, ##__VA_ARGS__)
#define CLI_INFO(format, ...) CLIPrint(1, "", format, ##__VA_ARGS__)
#define CLI_CMD_TRACE(format, ...) CLIPrint(0, "(Running CMD: )", format, ##__VA_ARGS__)
#define CLI_WARNING(format, ...) CLIPrint(1, "(CLI_WARNING: ) ", format, ##__VA_ARGS__)

static char *CMD_ARGS[MAX_ARGS] = { 0 };
static char TempString[MAX_CMD_LENGTH] = { 0 };
static int nargs;
static int prompt_cnt = 0;
static int exit_flag;
static int file_flag;
int strIcmp(const char* p1, const char* p2)
{
    while (*p1 && *p2)
    {
        char c1 = *p1;
        char c2 = *p2;
        if ((c1 >= 'A') && (c1 <= 'Z'))
        {
            c1 += 'a' - 'A';
        }
        if ((c2 >= 'A') && (c2 <= 'Z'))
        {
            c2 += 'a' - 'A';
        }
        if (c1 != c2)
        {
            return c1 - c2;
        }
        p1++;
        p2++;
    }
    return *p1 - *p2;
}

extern MSD_QD_DEV  *qddev;
extern MSD_U8	sohoDevNum;

#ifdef UNITTEST
extern void runUnitTestCode(MSD_U8 devNum);
extern void runSampleCode(MSD_U8 devNum);
int UnitTestCases(void)
{
    runUnitTestCode(sohoDevNum);
    return 0;
}
int SampleCases(void)
{
    runSampleCode(sohoDevNum);
    return 0;
}
#endif
extern void runCustomizeCode(MSD_U8 devNum);
int CustomizedCases(void)
{
    runCustomizeCode(sohoDevNum);
    return 0;
}

static char* reflectMSD_STATUS(MSD_STATUS status)
{
    char * str;
    switch (status)
    {
    case MSD_OK:
        str = "Operation succeeded";
        break;
    case MSD_FAIL:
        str = "Operation failed";
        break;
    case MSD_BAD_PARAM:
        str = "Invalid parameter";
        break;
    case MSD_NO_SUCH:
        str = "Can not find such item";
        break;
    case MSD_NOT_SUPPORTED:
        str = "The request is not support";
        break;
    case MSD_ALREADY_EXIST:
        str = "Tried to create existing item";
        break;
    case MSD_BAD_CPU_PORT:
        str = "Input CPU Port is not valid physical port number";
        break;
    case MSD_FEATURE_NOT_ENABLE:
        str = "The feature is not enabled";
        break;
    default:
        str = "Unknow status";
        break;
    }
    return str;
}
/*Callback function to do full chip reset*/
static int fullChipReset(void)
{
	MSD_STATUS retVal = 0;
	MSD_U8 devAddr = 0x1c;
	MSD_U8 regAddr = 0x13;
	MSD_U16 data = 0x8802;

	/*only support imp can full chip reset*/
	if ((qddev->devName == MSD_AMETHYST) || (qddev->devName == MSD_PERIDOT) || (qddev->devName == MSD_TOPAZ))
	{
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
		if (retVal != MSD_OK)
		{
			CLI_ERROR("msdSetAnyReg return error\n");
			return retVal;
		}

		data = 0x880f;
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
		if (retVal != MSD_OK)
		{
			CLI_ERROR("msdSetAnyReg return error\n");
			return retVal;
		}
		
		data = 0xaf00;
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
		if (retVal != MSD_OK)
		{
			CLI_ERROR("msdSetAnyReg return error\n");
			return retVal;
		}

		data = 0xf1ef;
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
		if (retVal != MSD_OK)
		{
			CLI_ERROR("msdSetAnyReg return error\n");
			return retVal;
		}
	}
	else if (qddev->devName == MSD_OAK || qddev->devName == MSD_SPRUCE || qddev->devName == MSD_BONSAIZ1 || qddev->devName == MSD_FIR || qddev->devName == MSD_BONSAI)
	{
		/* Reset Control, Index: 0x05 of Comm/Debug �C G2 Offset 0x13 */
		data = 0x8540;
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
		if (retVal != MSD_OK)
		{
			CLI_ERROR("msdSetAnyReg return error\n");
			return retVal;
		}
	}
	else
	{
		return MSD_NOT_SUPPORTED;
	}

	return retVal;
}
/*Callback function to handle the set API with port and two input numeric type parameters*/
static int setPortU32U32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U32 para2;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);        
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
#if 1
static int setTBPortU32U32(void)
{
    MSD_STATUS retVal;
    int index = 0;
		MSD_U8 tsnBlock;
    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U32 para2;
    if (nargs != 6)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);        
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    tsnBlock = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);
    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));
        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_TB_PORT_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, tsnBlock, port, para1, para2);
            break;
        }
        index++;
    }
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
#endif
/*Callback function to handle the set API with port and two input numeric type parameters*/
static int setPortU32U64(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U64 para2;

    if (nargs != 5)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U64)strtoull(CMD_ARGS[4], NULL, 0);

    while (1)
    {
        char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

        /*connect module name with subCmd*/
        strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
        strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32_U64 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
/*Callback function to handle the get API with port and one input numeric type and one outputnumeric type parameters*/
static int getPortU32PU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, para1, &para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para2);
    return retVal;
}
#if 1
static int getTBPortU32PU32(void)
{
    MSD_STATUS retVal;
    int index = 0;
    MSD_U8 tsnBlock;
    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;
    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    tsnBlock = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));
        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_TB_PORT_U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, tsnBlock, port, para1, &para2);
            break;
        }
        index++;
    }
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para2);
    return retVal;
} 
#endif
/*Callback function to handle the set API with port and three input numeric type parameters*/
static int setPortU32U32U32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1;
    MSD_U32 para2;
    MSD_U32 para3;

    if (nargs != 6)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    para3 = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2, para3);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
/*Callback function to handle the get API with port and two input numeric type and one outputnumeric type parameters*/
static int getPortU32U32PU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;
    MSD_U32 para3 = 0;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_U32__U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, para1, para2, &para3);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para3);
    return retVal;
}
/*Callback function to handle the set API with port and four input numeric type parameters*/
static int setPortU32U32U32U32(void)
{
	MSD_STATUS retVal;
	int index = 0;

	MSD_LPORT port;
	MSD_U32 para1;
	MSD_U32 para2;
	MSD_U32 para3;
	MSD_U32 para4;

	if (nargs != 7)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
	para3 = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);
	para4 = (MSD_U32)strtoul(CMD_ARGS[6], NULL, 0);

	while (1)
	{
		char temp[50] = { 0 };
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

		if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
		{
			API_SET_PORT_U32_U32_U32_U32 tempFunc = pairCmdList[index].setFunc;
			retVal = tempFunc(sohoDevNum, port, para1, para2, para3, para4);
			break;
		}
		index++;
	}

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return retVal;
}
/*Callback function to handle the get API with port and three input numeric type and one outputnumeric type parameters*/
static int getPortU32U32U32PU32(void)
{
	MSD_STATUS retVal;
	int index = 0;

	MSD_LPORT port;
	MSD_U32 para1 = 0;
	MSD_U32 para2 = 0;
	MSD_U32 para3 = 0;
	MSD_U32 para4 = 0;

	if (nargs != 6)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
	para1 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
	para2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
	para3 = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);

	while (1)
	{
		char temp[50] = { 0 };
		if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
		{
			CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
			return 1;
		}

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

		if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
		{
			API_GET_PORT_U32__U32_U32_PU32 tempFunc = pairCmdList[index].getFunc;
			retVal = tempFunc(sohoDevNum, port, para1, para2, para3, &para4);
			break;
		}
		index++;
	}

	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para4);
	return retVal;
}
/*Callback function to handle the get API with port and two output numeric type parameters*/
static int getPortPU32PU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_PU32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, &para1, &para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value1: 0x%x  value2: 0x%x\n", CMD_ARGS[1], para1, para2);
    return retVal;
}
/*Callback function to handle the set API with port and one input numeric type parameters*/
static int setPortU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    para = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_PORT_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, port, para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
/*Callback function to handle the get API with port and one output numeric type parameters*/
static int getPortPU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_LPORT port;
    MSD_U32 para = 0;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_PORT_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, port, &para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

#if 1 // jay.choi.230901.get.API
		if(!strcmp(CMD_ARGS[1], "getLinkState"))
		{
			printf("getPortPU32(): getLinkState=%d\n", para);
			linkState = para;
		}
#endif

    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para);
    return retVal;
}
static int setU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_U32 para;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    para = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    while (1)
    {
        char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getPU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_U32 para = 0;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }


    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0],strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1],strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, &para);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para);
    return retVal;
}
static int setU32U32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_U32 para1;
    MSD_U32 para2;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    para1 = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    para2 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    while (1)
    {
        char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_U32_U32 tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum, para1, para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
static int getU32PU32(void)
{
    MSD_STATUS retVal;
    int index = 0;

    MSD_U32 para1 = 0;
    MSD_U32 para2 = 0;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    para1 = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    while (1)
    {
		char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }

		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].getCmd) == 0)
        {
            API_GET_U32_PU32 tempFunc = pairCmdList[index].getFunc;
            retVal = tempFunc(sohoDevNum, para1, &para2);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("%s value: 0x%x\n", CMD_ARGS[1], para2);
    return retVal;
}
/*Callback function to handle the set API that only have device number parameter*/
static int setDev(void)
{
    MSD_STATUS retVal;
    int index = 0;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    while (1)
    {
        char temp[50] = { 0 };
        if ((pairCmdList[index].setFunc == NULL) && (pairCmdList[index].getFunc == NULL))
        {
            CLI_ERROR("Can not find the command: %s\n", CMD_ARGS[1]);
            return 1;
        }
        
		/*connect module name with subCmd*/
		strncat(temp, CMD_ARGS[0], strlen(CMD_ARGS[0]));
		strncat(temp, CMD_ARGS[1], strlen(CMD_ARGS[1]));

        if (strIcmp(temp, pairCmdList[index].setCmd) == 0)
        {
            API_SET_DEV tempFunc = pairCmdList[index].setFunc;
            retVal = tempFunc(sohoDevNum);
            break;
        }
        index++;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}

static int recurDeepCount = 0;
static void recurDisplay(cJSON *json)
{
    int i, origin;
    cJSON *temp = json;
    char str[MAX_CMD_LENGTH] = { 0 };

    if (json != NULL)
    {
        for (i = 0; i < recurDeepCount; i++)
		{
            strcat(str, "    ");
		}
        strncat(str, json->string, sizeof(str)-1);
        if (strIcmp(json->string, "") != 0)
		{
			strcat(str, ":\t");
		}

        if (json->valuestring != NULL)
		{
			strncat(str, json->valuestring, sizeof(str)-1);
		}
        else
		{
            strcat(str, "\n");
		}

        CLI_INFO("%s", str);
        if (json->child != NULL)
        {
            origin = recurDeepCount;
            recurDeepCount++;
            recurDisplay(json->child);
            recurDeepCount = origin;
        }

        while (temp->next != NULL)
        {
            temp = temp->next;
            memset(str, 0, MAX_CMD_LENGTH);
            for (i = 0; i < recurDeepCount; i++)
			{
                strcat(str, "    ");
			}
			strncat(str, temp->string, sizeof(str)-1);
            if (strIcmp(temp->string, "") != 0)
			{
                strcat(str, ":\t");
			}
            if (temp->valuestring != NULL)
			{
				strncat(str, temp->valuestring, sizeof(str)-1);
			}
            else
			{
                strcat(str, "\n");
			}

            CLI_INFO("%s", str);
            if (temp->child != NULL)
            {
                origin = recurDeepCount;
                recurDeepCount++;
                recurDisplay(temp->child);
                recurDeepCount = origin;
            }
        }
    }
}
static void displayParaHelp(char* cmd, char* subcmd)
{
    cJSON *temp1;
    cJSON *temp2;

    if (rootJSON != NULL)
    {
        if ((temp1 = cJSON_GetObjectItem(rootJSON, cmd)) == NULL)
		{
            return;
		}
        if ((temp1 = cJSON_GetObjectItem(temp1, "subcmd")) == NULL)
		{
            return;
		}
        if ((temp2 = cJSON_GetObjectItem(temp1, subcmd)) == NULL)
		{
            return;
		}

        memset(TempString, 0, MAX_CMD_LENGTH);
        const char* temp = cJSON_GetObjectItem(temp2, "help")->valuestring;
        char *temp1 = strchr(temp, ':');
		if (NULL == temp1)
		{
			return;
		}
        int len = strlen(temp) - strlen(temp1);
        memcpy(TempString, temp, len);
        CLI_INFO("%s\n", TempString);

        recurDeepCount = 0;
        recurDisplay(cJSON_GetObjectItem(temp2, "paraList")->child);
        /*Display the example*/
        CLI_INFO("Example:\n");
        recurDeepCount = 1;
        recurDisplay(cJSON_GetObjectItem(temp2, "example")->child);
    }
}
static int checkValidItem(char* cmd, char* subcmd, char* item)
{
    int retVal = 0;
    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, cmd);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), subcmd);
    cJSON *paraList = cJSON_GetObjectItem(subJSON, "paraList")->child;

    while (paraList)
    {
        if (paraList->child != NULL)
        {
            cJSON *temp = paraList->child;
            while (temp)
            {
                if (strstr(temp->string, item) != NULL)
                {
                    retVal = 1;
                    break;
                }
                temp = temp->next;
            }
        }
        paraList = paraList->next;
    }
    return retVal;
}

int runCommand(FILE* fp, char* prompt);
int fileOperation(void)
{
    int retVal = 0;
    char *filename = NULL;
    FILE* fp = NULL;

    if (nargs != 2)
    {
        CLI_ERROR("Syntax Error, Using command as follows: file <filename>: load command from file\n");
        return 1;
    }

    if (strIcmp(CMD_ARGS[1], "logOn") == 0)
    {
        logOnOffFlag = 1;
    }
    else if (strIcmp(CMD_ARGS[1], "logOff") == 0)
    {
        logOnOffFlag = 0;
    }
    else
    {
        filename = CMD_ARGS[1];
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            CLI_ERROR("Error: Cannot open the cli command file: %s\n", filename);
            return 2;
        }

        retVal = runCommand(fp, "");

        fclose(fp);
    }
    return retVal;
}
int paraHelp(void)
{
    if (nargs >= 3)
    {
        displayParaHelp(CMD_ARGS[0], CMD_ARGS[2]);
    }
    return 0;
}

static void macStr2macArr(char* inp, MSD_U8* out)
{
    int i, val;
    val = 0;
    for (i = 0; i<12; i++)
    {
        if ((inp[i] >= 48) && (inp[i] <= 57))
            val = (val << 4) + inp[i] - 48;
        if ((inp[i] >= 65) && (inp[i] <= 70))
            val = (val << 4) + (inp[i] - 65 + 10);
        if ((inp[i] >= 97) && (inp[i] <= 102))
            val = (val << 4) + (inp[i] - 97 + 10);
        if (i & 0x1)
        {
            out[i / 2] = val;
            val = 0;
        }
    }
}
static int updateFdbEntry(MSD_ATU_ENTRY * entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-macAddr") == 0)
        {
            macStr2macArr(value, entry->macAddr.arEther);
        }
        else if (strIcmp(cmd, "-lag") == 0)
        {
            entry->trunkMemberOrLAG = (MSD_BOOL)(strtoul(value, NULL, 0) & 0x1);
        }
        else if (strIcmp(cmd, "-portVec") == 0)
        {
            entry->portVec = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fid") == 0)
        {
            entry->fid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-state") == 0)
        {
            entry->entryState = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-FPri") == 0)
        {
            entry->exPrio.macFPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-QPri") == 0)
        {
            entry->exPrio.macQPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutFdbEntry(MSD_ATU_ENTRY * entry)
{
    int i, ret = 0;
    char tempStr[200] = { 0 };
    char *cmd = "atu";
    CLI_INFO("ATU entry info:\n");
    if (checkValidItem(cmd, "addEntry", "-macAddr"))
    {
        ret = sprintf(tempStr, "	macAddr:	");
        //CLI_INFO("	macAddr:	");
        for (i = 0; i < 6; i++)
        {
            //CLI_INFO("%02X", entry->macAddr.arEther[i]);
            ret = sprintf(tempStr + strlen(tempStr), "%02X", entry->macAddr.arEther[i]);
        }
        //CLI_INFO("\n");
        ret = sprintf(tempStr + strlen(tempStr), "\n");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "addEntry", "-lag"))
        CLI_INFO("	LAG:		0x%X\n", entry->trunkMemberOrLAG);
    if (checkValidItem(cmd, "addEntry", "-portVec"))
        CLI_INFO("	portVec:	0x%lX\n", entry->portVec);
    if (checkValidItem(cmd, "addEntry", "-fid"))
        CLI_INFO("	fid:		0x%X\n", entry->fid);
    if (checkValidItem(cmd, "addEntry", "-state"))
        CLI_INFO("	State:		0x%X\n", entry->entryState);
    if (checkValidItem(cmd, "addEntry", "-FPri"))
        CLI_INFO("	FPri:		0x%X\n", entry->exPrio.macFPri);
    if (checkValidItem(cmd, "addEntry", "-QPri"))
        CLI_INFO("	QPri:		0x%X\n", entry->exPrio.macQPri);
}
static void printOutFdbViolation(MSD_ATU_INT_STATUS *status)
{
    int i, ret = 0;
    char tempStr[200] = { 0 };

    CLI_INFO("ATU violation info:\n");
    CLI_INFO("	atuIntCause:	\n", status->atuIntCause);
    CLI_INFO("		Age out Vio status:	%d\n", status->atuIntCause.ageOutVio);
    CLI_INFO("		Full Vio status:	%d\n", status->atuIntCause.fullVio);
    CLI_INFO("		Member Vio status:	%d\n", status->atuIntCause.memberVio);
    CLI_INFO("		Miss Vio status:	%d\n", status->atuIntCause.missVio);
    CLI_INFO("	spid:			%X\n", status->spid);
    CLI_INFO("	fid:			%X\n", status->fid);
    ret = sprintf(tempStr, "	macAddr:	");
    //CLI_INFO("	macAddr:	");
    for (i = 0; i < 6; i++)
    {
        //CLI_INFO("%02X", status->macAddr.arEther[i]);
        ret = sprintf(tempStr + strlen(tempStr), "%02X", status->macAddr.arEther[i]);
    }
    //CLI_INFO("\n");
    ret = sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
}

int atuGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_ETHERADDR macAddr;
    MSD_U32  fid;
    MSD_ATU_ENTRY entry;

    memset(&macAddr, 0, sizeof(MSD_ETHERADDR));
    memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbEntryNextGet(sohoDevNum, &macAddr, fid, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid atu entry start from macAddr: %s for fid %d:\n", CMD_ARGS[2], fid);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next mac entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutFdbEntry(&entry);

    return 0;
}
int atuAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_ATU_ENTRY entry;

    memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

    if (updateFdbEntry(&entry, 2) != 0)
	{
        return MSD_FAIL;
	}

    retVal = msdFdbMacEntryAdd(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
            CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add mac entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int atuFlush(void)
{
    MSD_STATUS retVal;
    MSD_FLUSH_CMD flushCMD;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    flushCMD = (MSD_FLUSH_CMD)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFdbAllDelete(sohoDevNum, flushCMD);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush ATU  flushCMD=%X ret[%d: %s]"
            "\n", flushCMD, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int atuFlushInDB(void)
{
    MSD_STATUS retVal;
    MSD_FLUSH_CMD flushCMD;
    MSD_U32 fid;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    flushCMD = (MSD_FLUSH_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbAllDeleteInDB(sohoDevNum, flushCMD, fid);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush ATU based on fid flushCMD=%X fid=%lX ret[%d: %s]"
            "\n", flushCMD, fid, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int atuMove(void)
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD moveCMD;
    MSD_LPORT moveFrom;
    MSD_LPORT moveTo;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    moveCMD = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    moveFrom = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    moveTo = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdFdbMove(sohoDevNum, moveCMD, moveFrom, moveTo);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error move ATU moveCMD=%X moveFrom=%d moveTo=%d ret[%d: %s]"
            "\n", moveCMD, (int)moveFrom, (int)moveTo, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int atuMoveInDB(void)
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD moveCMD;
    MSD_U32 fid;
    MSD_LPORT moveFrom;
    MSD_LPORT moveTo;

    if (nargs != 6)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    moveCMD = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    moveFrom = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);
    moveTo = (MSD_LPORT)strtoul(CMD_ARGS[5], NULL, 0);

    retVal = msdFdbMoveInDB(sohoDevNum, moveCMD, fid, moveFrom, moveTo);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error move ATU moveCMD=%X moveFrom=%d moveTo=%d ret[%d: %s]"
            "\n", moveCMD, (int)moveFrom, (int)moveTo, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int atuDelEntry(void)
{
    MSD_STATUS retVal;
    MSD_ETHERADDR macAddr;
    MSD_U32 fid;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbMacEntryDelete(sohoDevNum, &macAddr, fid);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete mac entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int atuGetVio(void)
{
    MSD_STATUS retVal;
    MSD_ATU_INT_STATUS status;

    memset(&status, 0, sizeof(MSD_ATU_INT_STATUS));

    retVal = msdFdbViolationGet(sohoDevNum, &status);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get atu violation ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutFdbViolation(&status);
    return 0;
}
int atuFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_ATU_ENTRY entry;
    MSD_ETHERADDR macAddr;
    MSD_U32 fid;
    MSD_BOOL found;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    macStr2macArr(CMD_ARGS[2], macAddr.arEther);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    memset(&entry, 0, sizeof(MSD_ATU_ENTRY));

    retVal = msdFdbMacEntryFind(sohoDevNum, &macAddr, fid, &entry, &found);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find mac entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (found == MSD_TRUE)
    {
        CLI_INFO("Find the atu entry with macAddr %s in fid %d\n", CMD_ARGS[2], (int)fid);
        printOutFdbEntry(&entry);
    }
    else
    {
        CLI_INFO("Can not find the atu entry with macAddr %s in fid %d\n", CMD_ARGS[2], (int)fid);
    }

    return 0;
}
int atuPortRemove(void)
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD  moveCmd;
    MSD_LPORT     portNum;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    moveCmd = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdFdbPortRemove(sohoDevNum, moveCmd, portNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int portRemoveINDB(void)
{
    MSD_STATUS retVal;
    MSD_MOVE_CMD  moveCmd;
    MSD_U32        fid;
    MSD_LPORT     portNum;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    moveCmd = (MSD_MOVE_CMD)strtoul(CMD_ARGS[2], NULL, 0);
    fid = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdFdbPortRemoveInDB(sohoDevNum, moveCmd, fid, portNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}

static int updateEcidEntry(MSD_ECID_ENTRY * entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-group") == 0)
        {
            entry->group = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ecid") == 0)
        {
            entry->ecid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-lag") == 0)
        {
            entry->lag = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-portVec") == 0)
        {
            entry->portVec = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-lagId") == 0)
        {
            entry->lagId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-state") == 0)
        {
            entry->entryState = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-FPri") == 0)
        {
            entry->macFPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-QPri") == 0)
        {
            entry->macQPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-removeEtag") == 0)
        {
            entry->removeEtag = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutEcidEntry(MSD_ECID_ENTRY *entry)
{
    char *cmd = "ecid";
    CLI_INFO("ECID entry info:\n");
    if (checkValidItem(cmd, "addEntry", "-group"))
        CLI_INFO("	group:		0x%X\n", entry->group);
    if (checkValidItem(cmd, "addEntry", "-ecid"))
        CLI_INFO("	ecid:		0x%X\n", entry->ecid);
    if (checkValidItem(cmd, "addEntry", "-state"))
        CLI_INFO("	state:		0x%X\n", entry->entryState);
    if (checkValidItem(cmd, "addEntry", "-portVec"))
        CLI_INFO("	portVec:	0x%lX\n", entry->portVec);
    if (checkValidItem(cmd, "addEntry", "-lag"))
        CLI_INFO("	lag:		0x%X\n", entry->lag);
    if (checkValidItem(cmd, "addEntry", "-lagId"))
        CLI_INFO("	lagId:		0x%X\n", entry->lagId);
    if (checkValidItem(cmd, "addEntry", "-FPri"))
        CLI_INFO("	FPri:		0x%X\n", entry->macFPri);
    if (checkValidItem(cmd, "addEntry", "-QPri"))
        CLI_INFO("	QPri:		0x%X\n", entry->macQPri);
    if (checkValidItem(cmd, "addEntry", "-removeEtag"))
        CLI_INFO("	removeEtag:	0x%X\n", entry->removeEtag);
}
int ecidAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_ECID_ENTRY entry;

    memset(&entry, 0, sizeof(MSD_ECID_ENTRY));

    if (updateEcidEntry(&entry, 2) != 0)
	{
        return MSD_FAIL;
	}

    retVal = msdEcidEntryAdd(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
            CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add ecid entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int ecidDelAll(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdEcidEntryAllDelete(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete all ecid entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int ecidGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_U8  group;
    MSD_U16 ecid;
    MSD_ECID_ENTRY entry;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    group = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    ecid = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    memset(&entry, 0, sizeof(MSD_ECID_ENTRY));

    retVal = msdEcidEntryNextGet(sohoDevNum, group, ecid, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid ecid entry start from group: %d ecid: %d\n", group, ecid);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next ecid entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutEcidEntry(&entry);

    return 0;
}
int ecidFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_U8  group;
    MSD_U16 ecid;
    MSD_ECID_ENTRY entry;
    MSD_BOOL found;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    group = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    ecid = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    memset(&entry, 0, sizeof(MSD_ECID_ENTRY));


    retVal = msdEcidEntryFind(sohoDevNum, group, ecid, &entry, &found);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find ecid entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (found == MSD_TRUE)
    {
        CLI_INFO("Find the ecid entry with group %d ecid %d\n", group, ecid);
        printOutEcidEntry(&entry);
    }
    else
    {
        CLI_INFO("Can not find the ecid entry with group %d ecid %d\n", group, ecid);
    }

    return 0;
}
int ecidGetCount(void)
{
    MSD_STATUS retVal;
    MSD_U32 count;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdEcidEntryCountGet(sohoDevNum, &count);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get ecid entry count ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Get back ecid entry count: 0x%x\n", count);
    return 0;
}

static int updateArray(int* index, char* value, int maxLen, unsigned int list[])
{
    int stateIndex;
    char* temp;

    if (*value != '{')
    {
        CLI_ERROR("Invalid option input: %s\n", value);
        return -1;
    }

    temp = strchr(value, '}');
    if ((temp != NULL) && (*(temp + 1) != '\0'))
    {
        CLI_ERROR("Invalid option input: %s\n", value);
        return -1;
    }

    /*{} means all the members set to 0*/
	if (strIcmp(value, "{}") == 0)
	{
		return 0;
	}

    if (strlen(value) == 1)
    {
        value = CMD_ARGS[(*index)];
        *index = *index + 1;
    }
	else
	{
		value = value + 1;
	}

    list[0] = strtoul(value, NULL, 0);

    for (stateIndex = 1; (*index < nargs) && (stateIndex < maxLen); stateIndex++)
    {
        if (strchr(value, '}') != NULL)
            break;

        value = CMD_ARGS[(*index)];
        *index = *index + 1;
        if (*value == '}')
        {
            if (strlen(value) != 1)
            {
                CLI_ERROR("Invalid option input: %s\n", value);
                return -1;
            }

            break;
        }

        list[stateIndex] = strtoul(value, NULL, 0);
        //if (strchr(value, '}') != NULL)
        //    break;
    }

    return 0;
}
static int updateStuEntry(MSD_STU_ENTRY *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-sid") == 0)
        {
            entry->sid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-portState") == 0)
        {
            unsigned int tempList[MSD_MAX_SWITCH_PORTS] = { 0 };
            int i;
            if (updateArray(&index, value, MSD_MAX_SWITCH_PORTS, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < MSD_MAX_SWITCH_PORTS; i++)
            {
                entry->portState[i] = (MSD_PORT_STP_STATE)tempList[i];
            }
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutStuEntry(MSD_STU_ENTRY *entry)
{
    int i, ret = 0;
    char tempStr[200] = { 0 };
    char *cmd = "stu";

    CLI_INFO("Stu Entry Info:\n");
    if (checkValidItem(cmd, "addEntry", "-sid"))
        CLI_INFO("	sid:		%X\n", entry->sid);
    //CLI_INFO("	portState:	");
    if (checkValidItem(cmd, "addEntry", "-portState"))
    {
        ret = sprintf(tempStr, "	portState:	");
        for (i = 0; i < qddev->numOfPorts; i++)
        {
            //CLI_INFO("%X ", entry->portState[i]);
            ret = sprintf(tempStr + strlen(tempStr), "%X ", entry->portState[i]);
        }
        //CLI_INFO("\n");
        ret = sprintf(tempStr + strlen(tempStr), "\n");
        CLI_INFO("%s", tempStr);
    }
}
int stuAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_STU_ENTRY entry;

    memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    if (updateStuEntry(&entry, 2) != 0)
	{
        return -1;
	}

    retVal = msdStuEntryAdd(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add stu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int stuGetNext(void)
{
    MSD_STATUS retVal;
    MSD_U16 sid;
    MSD_STU_ENTRY entry;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    retVal = msdStuEntryNextGet(sohoDevNum, sid, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid stu entry start from sid %d:\n", sid);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find stu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    printOutStuEntry(&entry);
    return retVal;
}
int stuFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_STU_ENTRY entry;
    MSD_U16 sid;
    MSD_BOOL found;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&entry, 0, sizeof(MSD_STU_ENTRY));

    retVal = msdStuEntryFind(sohoDevNum, sid, &entry, &found);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find stu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (found == MSD_FALSE)
    {
        CLI_INFO("Cannot find the stu entry with the stu Id: %d\n", sid);
    }
    else
    {
        CLI_INFO("Find the stu entry with the stu Id: %d\n", sid);
        printOutStuEntry(&entry);
    }

    return 0;
}
int stuDelEntry(void)
{
    MSD_STATUS retVal;
    MSD_U16 sid;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    sid = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdStuEntryDelete(sohoDevNum, sid);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete stu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int stuGetEntryCount(void)
{
    MSD_STATUS retVal;
    MSD_U16 count;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdStuEntryCountGet(sohoDevNum, &count);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get stu entry count ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("The valied stu entry count is:%d\n", count);

    return 0;
}

static int updateVtuEntry(MSD_VTU_ENTRY *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-fid") == 0)
        {
            entry->fid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vid") == 0)
        {
            entry->vid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sid") == 0)
        {
            entry->sid = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-TagP") == 0)
        {
            int i;
            unsigned int tempList[MSD_MAX_SWITCH_PORTS] = { 0 };
            if (updateArray(&index, value, MSD_MAX_SWITCH_PORTS, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < MSD_MAX_SWITCH_PORTS; i++)
            {
                entry->memberTagP[i] = (MSD_PORT_MEMBER_TAG)tempList[i];
            }
        }
        else if (strIcmp(cmd, "-vidPolicy") == 0)
        {
            entry->vidPolicy = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-useVIDFPri") == 0)
        {
            entry->vidExInfo.useVIDFPri = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidFPri") == 0)
        {
            entry->vidExInfo.vidFPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-useVIDQPri") == 0)
        {
            entry->vidExInfo.useVIDQPri = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidQPri") == 0)
        {
            entry->vidExInfo.vidQPri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dontLearn") == 0)
        {
            entry->vidExInfo.dontLearn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-filterUC") == 0)
        {
            entry->vidExInfo.filterUC = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-filterBC") == 0)
        {
            entry->vidExInfo.filterBC = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-filterMC") == 0)
        {
            entry->vidExInfo.filterMC = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-routeDis") == 0)
        {
            entry->vidExInfo.routeDis = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-mldSnoop") == 0)
        {
            entry->vidExInfo.mldSnoop = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-igmpSnoop") == 0)
        {
            entry->vidExInfo.igmpSnoop = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutVtuEntry(MSD_VTU_ENTRY *entry)
{
    int i, ret = 0;
	char tempStr[500] = { 0 };
    char *cmd = "vlan";

	CLI_INFO("\n----------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
	if (checkValidItem(cmd, "addEntry", "-fid"))
		ret = sprintf(tempStr, "FID   ");
	if (checkValidItem(cmd, "addEntry", "-vid"))
		ret = sprintf(tempStr + strlen(tempStr), "VID    ");
	if (checkValidItem(cmd, "addEntry", "-sid"))
		ret = sprintf(tempStr + strlen(tempStr), "SID   ");
	if (checkValidItem(cmd, "addEntry", "-TagP"))
		ret = sprintf(tempStr + strlen(tempStr), "MEMBERTAGP              ");
	if (checkValidItem(cmd, "addEntry", "-vidPolicy"))
		ret = sprintf(tempStr + strlen(tempStr), "vidPolicy ");
	if (checkValidItem(cmd, "addEntry", "-useVIDFPri"))
		ret = sprintf(tempStr + strlen(tempStr), "useVIDFPri ");
	if (checkValidItem(cmd, "addEntry", "-vidFPri"))
		ret = sprintf(tempStr + strlen(tempStr), "vidFPri ");
	if (checkValidItem(cmd, "addEntry", "-useVIDQPri"))
		ret = sprintf(tempStr + strlen(tempStr), "useVIDQPri ");
	if (checkValidItem(cmd, "addEntry", "-vidQPri"))
		ret = sprintf(tempStr + strlen(tempStr), "vidQPri ");
	if (checkValidItem(cmd, "addEntry", "-dontLearn"))
		ret = sprintf(tempStr + strlen(tempStr), "dontLearn ");
	if (checkValidItem(cmd, "addEntry", "-filterUC"))
		ret = sprintf(tempStr + strlen(tempStr), "filterUC ");
	if (checkValidItem(cmd, "addEntry", "-filterBC"))
		ret = sprintf(tempStr + strlen(tempStr), "filterBC ");
	if (checkValidItem(cmd, "addEntry", "-filterMC"))
		ret = sprintf(tempStr + strlen(tempStr), "filterMC ");
	if (checkValidItem(cmd, "addEntry", "-routeDis"))
		ret = sprintf(tempStr + strlen(tempStr), "routeDis ");
	if (checkValidItem(cmd, "addEntry", "-mldSnoop"))
		ret = sprintf(tempStr + strlen(tempStr), "mldSnoop ");
	if (checkValidItem(cmd, "addEntry", "-igmpSnoop"))
		ret = sprintf(tempStr + strlen(tempStr), "igmpSnoop");

	ret = sprintf(tempStr + strlen(tempStr), "\n");
	CLI_INFO("%s", tempStr);
	CLI_INFO("----------------------------------------------------------------------------------------------------------------------------------------------------------- \n");
	/*data display*/
	memset(tempStr, 0, sizeof(tempStr));
	if (checkValidItem(cmd, "addEntry", "-fid"))
		ret = sprintf(tempStr, "0x%-4X", entry->fid);
    if (checkValidItem(cmd, "addEntry", "-vid"))
		ret = sprintf(tempStr + strlen(tempStr), "0x%-5X", entry->vid);
    if (checkValidItem(cmd, "addEntry", "-sid"))
		ret = sprintf(tempStr + strlen(tempStr), "0x%-4X", entry->sid);
    if (checkValidItem(cmd, "addEntry", "-TagP"))
    {
        for (i = 0; i < qddev->numOfPorts; i++)
        {
            ret = sprintf(tempStr + strlen(tempStr), "%X ", entry->memberTagP[i]);
        }
    }
    if (checkValidItem(cmd, "addEntry", "-vidPolicy"))
		ret = sprintf(tempStr + strlen(tempStr), "         %-10X", entry->vidPolicy);
    if (checkValidItem(cmd, "addEntry", "-useVIDFPri"))
		ret = sprintf(tempStr + strlen(tempStr), "%-11X", entry->vidExInfo.useVIDFPri);
    if (checkValidItem(cmd, "addEntry", "-vidFPri"))
		ret = sprintf(tempStr + strlen(tempStr), "%-8X", entry->vidExInfo.vidFPri);
    if (checkValidItem(cmd, "addEntry", "-useVIDQPri"))
		ret = sprintf(tempStr + strlen(tempStr), "%-11X", entry->vidExInfo.useVIDQPri);
    if (checkValidItem(cmd, "addEntry", "-vidQPri"))
		ret = sprintf(tempStr + strlen(tempStr), "%-8X", entry->vidExInfo.vidQPri);
    if (checkValidItem(cmd, "addEntry", "-dontLearn"))
		ret = sprintf(tempStr + strlen(tempStr), "%-10X", entry->vidExInfo.dontLearn);
    if (checkValidItem(cmd, "addEntry", "-filterUC"))
		ret = sprintf(tempStr + strlen(tempStr), "%-9X", entry->vidExInfo.filterUC);
    if (checkValidItem(cmd, "addEntry", "-filterBC"))
        ret = sprintf(tempStr + strlen(tempStr), "%-9X", entry->vidExInfo.filterBC);
    if (checkValidItem(cmd, "addEntry", "-filterMC"))
        ret = sprintf(tempStr + strlen(tempStr), "%-9X", entry->vidExInfo.filterMC);
    if (checkValidItem(cmd, "addEntry", "-routeDis"))
        ret = sprintf(tempStr + strlen(tempStr), "%-9X", entry->vidExInfo.routeDis);
    if (checkValidItem(cmd, "addEntry", "-mldSnoop"))
        ret = sprintf(tempStr + strlen(tempStr), "%-10X", entry->vidExInfo.mldSnoop);
    if (checkValidItem(cmd, "addEntry", "-igmpSnoop"))
        ret = sprintf(tempStr + strlen(tempStr), "%X", entry->vidExInfo.igmpSnoop);

	ret = sprintf(tempStr + strlen(tempStr), "\n");
	CLI_INFO("%s", tempStr);
}
static void printOutVtuViolation(MSD_VTU_INT_STATUS *status)
{
    CLI_INFO("VTU violation info:\n");

    CLI_INFO("	spid:			%X\n", status->spid);
    CLI_INFO("	vid:			%X\n", status->vid);
    CLI_INFO("	vtuIntCause:	\n");
    CLI_INFO("		fullVio:        %X\n", status->vtuIntCause.fullVio);
    CLI_INFO("		memberVio:      %X\n", status->vtuIntCause.memberVio);
    CLI_INFO("		missVio:        %X\n", status->vtuIntCause.missVio);
}
static int getDefaultVID(MSD_U16 *vid)
{
	MSD_STATUS retVal;
	MSD_U16 tempData;
	MSD_U8 devAddr, regAddr;

	/*global 1 register 6 for vlan id*/
	devAddr = 0x1B;
	regAddr = 0x6;

	retVal = msdGetAnyReg(sohoDevNum, devAddr, regAddr, &tempData);
	if (retVal != MSD_OK)
	{
		CLI_ERROR("msdGetAnyReg fail, devAddr = 0x%X, regAddr = 0x%X\n",devAddr, regAddr);
		return 1;
	}

	*vid = ((tempData & 0x2000) >> 1) | (tempData & 0xFFF);

	return 0;
}
int vtuAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_VTU_ENTRY entry;

    memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    if (updateVtuEntry(&entry, 2) != 0)
	{
        return -1;
	}

    retVal = msdVlanEntryAdd(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add vlan entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int vtuFlush(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdVlanAllDelete(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush vtu entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int vtuFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_VTU_ENTRY entry;
    MSD_BOOL found;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    retVal = msdVlanEntryFind(sohoDevNum, vlanId, &entry, &found);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find vlan entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (found == MSD_FALSE)
    {
        CLI_INFO("Cannot find the vlan entry with the vlan Id: 0x%X\n", vlanId);
    }
    else
    {
        CLI_INFO("Find the vlan entry with the vlan Id: 0x%X\n", vlanId);
        printOutVtuEntry(&entry);
    }

    return 0;
}
int vtuDelEntry(void)
{
    MSD_STATUS retVal;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanEntryDelete(sohoDevNum, vlanId);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete vlan entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int vtuSetMode(void)
{
    MSD_STATUS retVal;
    MSD_VTU_MODE    mode;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    mode = (MSD_VTU_MODE)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanModeSet(sohoDevNum, mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set vtu mode ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int vtuGetMode(void)
{
    MSD_STATUS retVal;
    MSD_VTU_MODE    mode;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdVlanModeGet(sohoDevNum, &mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set vlan mode ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Vtu get back mode: %x\n", mode);
    return 0;
}
int vtuGetVio(void)
{
    MSD_STATUS retVal;
    MSD_VTU_INT_STATUS status;

    memset(&status, 0, sizeof(MSD_VTU_INT_STATUS));

    retVal = msdVlanViolationGet(sohoDevNum, &status);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get atu violation ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutVtuViolation(&status);
    return 0;
}
int vutIsExist(void)
{
    MSD_STATUS retVal;
    MSD_BOOL exist;
    MSD_U16 vlanId;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdVlanExists(sohoDevNum, vlanId, &exist);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error check vlan entry exist ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (exist == MSD_FALSE)
    {
        CLI_INFO("the vlan entry with vlanId %d is not exist\n", vlanId);
    }
    else
    {
        CLI_INFO("the vlan entry with vlanId %d is exist\n", vlanId);
    }

    return 0;
}
int vtuGetNext(void)
{
    MSD_STATUS retVal;
    MSD_VTU_ENTRY entry;
    MSD_U16 vlanId;

    if (nargs < 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	if (nargs == 2)
	{
		retVal = getDefaultVID(&vlanId);
		if (retVal != 0)
		{
			CLI_ERROR("Get Hardware VLAN ID error\n");
			return 1;
		}
	}
	else
	{
		vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	}

    memset(&entry, 0, sizeof(MSD_VTU_ENTRY));

    retVal = msdVlanEntryNextGet(sohoDevNum, vlanId, &entry);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid vlan entry start from vlan Id 0x%X:\n", vlanId);
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find vlan entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutVtuEntry(&entry);

    return 0;
}
int vtuGetCount(void)
{
    MSD_STATUS retVal;
    MSD_U16 count;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdVlanEntryCountGet(sohoDevNum, &count);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get vtu entry count ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("The valid vlan entry count is:%d\n", count);

    return 0;
}
int vtuSetMemberTag(void)
{
    MSD_STATUS retVal;
    MSD_U16 vlanId;
    MSD_LPORT  portNum;
    MSD_PORT_MEMBER_TAG memberTag;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);
    memberTag = (MSD_PORT_MEMBER_TAG)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdVlanMemberTagSet(sohoDevNum, vlanId, portNum, memberTag);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set the vlan memberTag ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int vtuGetMemberTag(void)
{
    MSD_STATUS retVal;
    MSD_U16 vlanId;
    MSD_LPORT  portNum;
    MSD_PORT_MEMBER_TAG memberTag;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    vlanId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    portNum = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdVlanMemberTagGet(sohoDevNum, vlanId, portNum, &memberTag);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get the vlan memberTag ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Read back the memberTag for port %d vlan 0x%X is %d\n", portNum, vlanId, memberTag);
    return 0;
}

typedef enum {
    S200k = 0,
    S400k = 1,
    S500k = 2,
    S600k = 3,
    S800k = 4,
    S1000k = 5,
    S1200k = 6,
    S1500k = 7
} I2C_SPEED;
static MSD_U16 ReadInt(MSD_U8 *data, MSD_U8 cnt)
{
    MSD_U8 Idx = 0;
    MSD_U16 val = 0;
    cnt = cnt * 2;
    while (cnt > 0)
    {
        if ((data[Idx] >= '0') && (data[Idx] <= '9'))
        {
            val = (val << 4) | (data[Idx] - '0');
            cnt--;
            Idx++;
        }
        else if ((data[Idx] >= 'a') && (data[Idx] <= 'f'))
        {
            val = (val << 4) | (data[Idx] - 'a' + 10);
            cnt--;
            Idx++;
        }
        else if ((data[Idx] >= 'A') && (data[Idx] <= 'F' + 10))
        {
            val = (val << 4) | (data[Idx] - 'A' + 10);
            cnt--;
            Idx++;
        }
        else
        {
            val = (val << 4) | 0xF;
            cnt--;
            Idx++;
        }

    }
    return val;
}
typedef struct NODE
{
    MSD_U32 addr;
    MSD_U32 len;
    MSD_U8* data;
    struct NODE* next;
}Node;
static void freeNode(struct NODE *head)
{
    while (head != NULL)
    {
        Node* tmp = head->next;
        free(head->data);
        free(head);
        head = tmp;
    }
}

static MSD_U8* Ihx_Parse_EEPROM(MSD_U16 *data_len, MSD_U8* filename, I2C_SPEED speed, MSD_U16 eeprom_addr_src, MSD_U16 ram_dest_addr, MSD_U16 ram_exec_addr)
{
    MSD_U8* ihx_data;
    MSD_U16 ihx_data_len;
    MSD_U16 Idx;
    MSD_U8 Dnum;
    MSD_U16 type;
    MSD_U8 chk;
    MSD_U16 code_start = 0xffff;
    MSD_U16 code_end = 0;
    MSD_U16 addr;
    MSD_U16 code_size;
    MSD_U8 data;
    MSD_U16 i;
    MSD_U8* eeprom_data;

    FILE *pfile;
    if ((pfile = fopen(filename, "rb")) == NULL)
    {
        CLI_ERROR("Can not open the file: %s\n", filename);
        return NULL;
    }

    fseek(pfile, 0, SEEK_END);
    ihx_data_len = (MSD_U16)ftell(pfile);

    ihx_data = (MSD_U8*)malloc(ihx_data_len * sizeof(MSD_U8) + 1);
	if (NULL == ihx_data)
	{
		fclose(pfile);
		return NULL;
	}
    rewind(pfile);
	if (fread(ihx_data, sizeof(MSD_U8), ihx_data_len, pfile) == 0)
	{
		printf("Real length is less than expected\n");
	}

    /* get code size */
    for (Idx = 0; Idx < ihx_data_len;)
    {
        if (ihx_data[Idx] == ':')
        {
            Idx++;
            Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
            Idx += 2;
            addr = ReadInt(ihx_data + Idx, 2);
            Idx += 4;

            type = ReadInt(ihx_data + Idx, 1);
            if (type == 0)
            {
                if (code_start > addr)
                    code_start = addr;
                if (code_end < (addr + Dnum))
                    code_end = addr + Dnum;
            }
            else if (type == 1)
                break;
        }
        else
        {
            Idx += 1;
        }
    }

    code_size = code_end - code_start;

    eeprom_data = (unsigned char*)malloc((code_size + eeprom_addr_src)*sizeof(char) + 1);
	if (NULL == eeprom_data)
	{
		free(ihx_data);
		fclose(pfile);
		return NULL;
	}

    for (Idx = 0; Idx < code_size + eeprom_addr_src; Idx++)
    {
        eeprom_data[Idx] = 0xff;
    }

    /* fast boot function */
    eeprom_data[0] = 0xed;

    /* eeprom speed configuration */
    data = (MSD_U8)speed;
    eeprom_data[1] = data;

    /* code size */
    eeprom_data[2] = (MSD_U8)(code_size & 0xff);
    eeprom_data[3] = (MSD_U8)((code_size >> 8) & 0xff);

    /* ram address */
    eeprom_data[4] = (MSD_U8)(ram_dest_addr & 0xff);
    eeprom_data[5] = (MSD_U8)((ram_dest_addr >> 8) & 0xff);

    /* eeprom src address */
    eeprom_data[6] = (MSD_U8)(eeprom_addr_src & 0xff);
    eeprom_data[7] = (MSD_U8)((eeprom_addr_src >> 8) & 0xff);

    eeprom_data[8] = 0xc3;

    /* ram executing address */
    eeprom_data[9] = (MSD_U8)(ram_exec_addr & 0xff);
    eeprom_data[0xa] = (MSD_U8)((ram_exec_addr >> 8) & 0xff);

    type = 0;
    Idx = 0;
    while (type != 1)
    {
		while ((ihx_data[Idx] != ':') && (ihx_data[Idx] > 0))
		{
			Idx++;
		}
        if (ihx_data[Idx] != ':')
        {
            /* ihx file format error */
            free(eeprom_data);
			free(ihx_data);
			fclose(pfile);
            eeprom_data = NULL;
            return eeprom_data;
        }
        Idx++;

        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        chk = Dnum;
        Idx += 2;

        addr = ReadInt(ihx_data + Idx, 2);
        chk += (addr & 0xff);
        chk += ((addr >> 8) & 0xff);
        Idx += 4;

        type = ReadInt(ihx_data + Idx, 1);
        chk += type;
        Idx += 2;

        for (i = 0; i < Dnum; i++)
        {
            chk += ReadInt(ihx_data + Idx, 1);
            if (type == 0)
                eeprom_data[addr + eeprom_addr_src + i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);

            Idx += 2;
        }

        if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
        {
			free(ihx_data);
			fclose(pfile);
            free(eeprom_data);
            eeprom_data = NULL;
            return eeprom_data;
        }
        Idx += 2;

    }
	fclose(pfile);
    free(ihx_data);
    ihx_data = NULL;
    *data_len = code_size + eeprom_addr_src;
    return eeprom_data;
}
static Node* Ihx_Parse_Memory(MSD_U8* filename)
{
    MSD_U8* ihx_data;
    MSD_U32 ihx_data_len;
    MSD_U32 Idx;
    MSD_U8 Dnum;
    MSD_U16 type;
    MSD_U8 chk;
    MSD_U32 addr;
    MSD_U16 i;
    MSD_U32 highAddr = 0;
	MSD_U8 tmpArr[2] = { 0 };
    Node *head = NULL, *cur = NULL, *ne = NULL;

    FILE *pfile;
	pfile = fopen(filename, "rb");
    if (pfile == NULL)
    {
        CLI_ERROR("Can not open the file: %s\n", filename);
        return NULL;
    }

    fseek(pfile, 0, SEEK_END);
    ihx_data_len = (MSD_U16)ftell(pfile);

    ihx_data = (unsigned char*)malloc(ihx_data_len*sizeof(char) + 1);
	if (NULL == ihx_data)
	{
		fclose(pfile);
		return NULL;
	}
    rewind(pfile);
	if (fread(ihx_data, sizeof(unsigned char), ihx_data_len, pfile) == 0)
	{
		printf("Real length is less than expected\n");
	}

    type = 0;
    Idx = 0;
    while (type != 1)
    {
		while ((ihx_data[Idx] != ':') && (ihx_data[Idx] > 0))
		{
			Idx++;
		}
        if (ihx_data[Idx] != ':')
        {
            /* ihx file format error */
			freeNode(head);
			head = NULL;
            free(ihx_data);
			fclose(pfile);
            return NULL;
        }
        Idx++;

        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        chk = Dnum;
        Idx += 2;

        addr = ReadInt(ihx_data + Idx, 2);
        chk += (addr & 0xff);
        chk += ((addr >> 8) & 0xff);
        Idx += 4;
        addr += highAddr;

        type = ReadInt(ihx_data + Idx, 1);
        chk += type;
        Idx += 2;

        if (type == 0)
        {
            ne = (Node*)malloc(sizeof(Node));
			if (NULL == ne)
			{
				freeNode(head);
				free(ihx_data);
				fclose(pfile);
				return NULL;
			}
            ne->len = Dnum;
            ne->addr = addr;
			ne->data = (MSD_U8*)malloc(Dnum * sizeof(MSD_U8));
			if (NULL == ne->data)
			{
				freeNode(head);
				free(ihx_data);
				free(ne);
				fclose(pfile);
				return NULL;
			}
            ne->next = NULL;

			if (head == NULL)
			{
				head = ne;
			}

			if (cur == NULL)
			{
				cur = ne;
			}
            else
            {
                cur->next = ne;
                cur = ne;
            }
        }

        for (i = 0; i < Dnum; i++)
        {
            chk += ReadInt(ihx_data + Idx, 1);
			if (type == 0)
			{
				cur->data[i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);
			}
			else if ((type == 2) || (type == 4))
			{
				tmpArr[i] = (MSD_U8)ReadInt(ihx_data + Idx, 1);
			}

            Idx += 2;
        }
        if (type == 2)
        {
            highAddr = (((tmpArr[0] & 0xFF) << 12) | ((tmpArr[1] & 0xFF)) << 4);
        }
        else if (type == 4)
        {
            highAddr = ((tmpArr[0] & 0xFF) << 24) | ((tmpArr[1] & 0xFF) << 16);
        }
        Dnum = (MSD_U8)ReadInt(ihx_data + Idx, 1);
        if (((chk + ReadInt(ihx_data + Idx, 1)) & 0xff) != 0)
        {
            freeNode(head);
            head = NULL;
            free(ihx_data);
			fclose(pfile);
            return NULL;
        }

        Idx += 2;
    }
	fclose(pfile);
    free(ihx_data);

    return head;
}
static int UpdateFilename(char *tempName)
{
    char *pTemp = tempName;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    else if (nargs == 3)
    {
        char *p = CMD_ARGS[2];
        while (*p != 0)
        {
            if (*p != '\"')
            {
                *pTemp = *p;
                pTemp++;
            }
            p++;
        }
        *pTemp = 0;
    }
    else
    {
        char *p = tempName;
        if (g_filename == NULL)
        {
            cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
            CLI_ERROR("Syntax Error, invlid file name: %s\n", CMD_ARGS[2]);
            return 1;
        }

        strcat(tempName, g_filename);
        while ((*p != 0) && (*p != '\n'))
        {
            if (*p != '\"')
            {
                *pTemp = *p;
                pTemp++;
            }
            p++;
        }
        *pTemp = 0;
    }

    return 0;
}
int IMPRun(void)
{
    MSD_STATUS retVal;
    MSD_U16	addr;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    addr = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdImpRun(sohoDevNum, addr);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error start run the IMP ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int IMPReset(void)
{
    MSD_STATUS retVal;
    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdImpReset(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error reset the IMP ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int IMPStop(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdImpStop(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error write EEPROM ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int IMPWriteEEPROM(void)
{
    MSD_STATUS retVal;
    MSD_U16	addr;
    MSD_U8	data;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    addr = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    data = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdImpWriteEEPROM(sohoDevNum, addr, data);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error write EEPROM ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int IMPLoadToEEPROM(void)
{
    MSD_STATUS retVal;
    MSD_U8	*data = NULL;
    MSD_U16 data_len = 0;
    I2C_SPEED speed = S200k;
    char filename[MAX_ARGS_LENGTH] = { 0 };

    UpdateFilename(filename);

    MSD_U16 ram_dest_addr = 0;
    MSD_U16 ram_exec_addr = 0;
    MSD_U16 eeprom_addr_src = 0x10;

    data = Ihx_Parse_EEPROM(&data_len, filename, speed, eeprom_addr_src, ram_dest_addr, ram_exec_addr);

    if (data == NULL)
    {
        CLI_ERROR("Parsing the %s error, make sure the file is correct\n", filename);
        return MSD_FAIL;
    }

    /* load data to eeprom */
    retVal = msdImpLoadToEEPROM(sohoDevNum, data_len, data);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            free(data);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error write EEPROM ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        free(data);
        return MSD_FAIL;
    }

    free(data);
    return 0;
}
int IMPLoadToRAM(void)
{
    MSD_STATUS retVal;
    char filename[MAX_ARGS_LENGTH] = { 0 };
    MSD_U16 addr = 0xffff;
    Node* head;
    Node* node;

    UpdateFilename(filename);

    head = Ihx_Parse_Memory(filename);
    node = head;

    if (head == NULL)
    {
        CLI_ERROR("Parsing the %s error, make sure the file is correct\n", filename);
        return MSD_FAIL;
    }

    /* load data to memory */
    while (head != NULL)
    {
        retVal = msdImpLoadToRAM(sohoDevNum, head->addr, head->len, head->data);
        if (retVal != MSD_OK) {
            if (retVal == MSD_NOT_SUPPORTED)
            {
                CLI_WARNING("The device maybe not support this feature, please double checked\n");
                freeNode(node);
                return MSD_NOT_SUPPORTED;
            }
            CLI_ERROR("Error write RAM ret[%d: %s]"
                "\n", retVal, reflectMSD_STATUS(retVal));
            freeNode(node);
            return MSD_FAIL;
        }
        if (addr > head->addr)
            addr = head->addr;
        head = head->next;
    }

    freeNode(node);
    node = NULL;

    return 0;
}

int phyReset(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPhyReset(sohoDevNum, port);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error phy reset ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int phyGetIntStatus(void)
{
	MSD_STATUS retVal;
	MSD_LPORT port;
	MSD_U16 data;

	if (nargs != 3)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdPhyInterruptStatusGet(sohoDevNum, port, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error phy interrupt status ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	CLI_INFO("PHY interrupt status data : 0x%x\n", data);
	CLI_INFO("\nInterrupt Status Field               port %d\n", port);
	CLI_INFO("--------------------------------------------\n");
	if (data & MSD_PHY_AUTO_NEG_ERROR)
	{
		CLI_INFO("Copper AutoNegotiation Error:          Y\n");
	}
	else
	{
		CLI_INFO("Copper AutoNegotiation Error:          N\n");
	}

	if (data & MSD_PHY_SPEED_CHANGED)
	{
		CLI_INFO("Copper Speed Changed:                  Y\n");
	}
	else
	{
		CLI_INFO("Copper Speed Changed:                  N\n");
	}

	if (data & MSD_PHY_DUPLEX_CHANGED)
	{
		CLI_INFO("Copper Duplex Changed:                 Y\n");
	}
	else
	{
		CLI_INFO("Copper Duplex Changed:                 N\n");
	}

	if (data & MSD_PHY_PAGE_RECEIVED)
	{
		CLI_INFO("Copper Page Received:                  Y\n");
	}
	else
	{
		CLI_INFO("Copper Page Received:                  N\n");
	}

	if (data & MSD_PHY_AUTO_NEG_COMPLETED)
	{
		CLI_INFO("Copper AutoNegotiation Completed:      Y\n");
	}
	else
	{
		CLI_INFO("Copper AutoNegotiation Completed:      N\n");
	}

	if (data & MSD_PHY_LINK_STATUS_CHANGED)
	{
		CLI_INFO("Copper Link Status Changed:            Y\n");
	}
	else
	{
		CLI_INFO("Copper Link Status Changed:            N\n");
	}

	if (data & MSD_PHY_SYMBOL_ERROR)
	{
		CLI_INFO("Copper Symbol Error:                   Y\n");
	}
	else
	{
		CLI_INFO("Copper Symbol Error:                   N\n");
	}

	if (data & MSD_PHY_FALSE_CARRIER)
	{
		CLI_INFO("Copper False Carrier:                  Y\n");
	}
	else
	{
		CLI_INFO("Copper False Carrier:                  N\n");
	}

	if (data & MSD_PHY_MDI_CROSSOVER_CHANGED)
	{
		CLI_INFO("MDI Crossover Changed:                 Y\n");
	}
	else
	{
		CLI_INFO("MDI Crossover Changed:                 N\n");
	}

	if (data & MSD_PHY_DOWNSHIFT)
	{
		CLI_INFO("Downshift Interrupt:                   Y\n");
	}
	else
	{
		CLI_INFO("Downshift Interrupt:                   N\n");
	}

	if (data & MSD_PHY_ENERGY_DETECT)
	{
		CLI_INFO("Copper Energy Detect Changed:          Y\n");
	}
	else
	{
		CLI_INFO("Copper Energy Detect Changed:          N\n");
	}

	if (data & MSD_PHY_FLP_EXCHANGE_COMPLETE)
	{
		CLI_INFO("FLP Exchange Complete but no Link:     Y\n");
	}
	else
	{
		CLI_INFO("FLP Exchange Complete but no Link:     N\n");
	}

	if (data & MSD_PHY_DTE_POWER_DETECTION)
	{
		CLI_INFO("DTE power detection status changed:    Y\n");
	}
	else
	{
		CLI_INFO("DTE power detection status changed:    N\n");
	}

	if (data & MSD_PHY_POLARITY_CHANGED)
	{
		CLI_INFO("Polarity Changed:                      Y\n");
	}
	else
	{
		CLI_INFO("Polarity Changed:                      N\n");
	}

	if (data & MSD_PHY_JABBER)
	{
		CLI_INFO("Jabber:                                Y\n");
	}
	else
	{
		CLI_INFO("Jabber:                                N\n");
	}

	return 0;
}
/*Interrupt Source Register, G2 offset 0x0*/
int devGetIntStatus(void)
{
	MSD_STATUS retVal;
	MSD_U16 data;

	if (nargs != 2)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	retVal = msdSysDevIntStatusGet(sohoDevNum, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error device interrupt status ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	CLI_INFO("Device Int Status data : 0x%x\n", data);

	CLI_INFO("-----------------------------\n");
	if (data & MSD_WATCH_DOG_INT)
	{
		CLI_INFO("WatchDog Int:          Y\n");
	}
	else
	{
		CLI_INFO("WatchDog Int:          N\n");
	}

	if (data & MSD_JAM_LIMIT_INT)
	{
		CLI_INFO("JamLimit:              Y\n");
	}
	else
	{
		CLI_INFO("JamLimit:              N\n");
	}

	if (data & MSD_DUPLEX_MISMATCH_INT)
	{
		CLI_INFO("Duplex Mismatch:       Y\n");
	}
	else
	{
		CLI_INFO("Duplex Mismatch:       N\n");
	}

	if (data & MSD_WAKE_EVENT_INT)
	{
		CLI_INFO("WakeEvent:             Y\n");
	}
	else
	{
		CLI_INFO("WakeEvent:             N\n");
	}

    /* Oak, Spruce */
	if (qddev->devName == MSD_OAK || qddev->devName == MSD_SPRUCE)
	{
		if (data & MSD_SERDES_INT_11)
		{
			CLI_INFO("PHYInt 11:             Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 11:             N\n");
		}

        if (data & MSD_PHY_INT_10)
        {
            CLI_INFO("PHYInt 10:             Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 10:             N\n");
        }

        if (data & MSD_PHY_INT_9)
        {
            CLI_INFO("PHYInt 9:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 9:              N\n");
        }

        if (data & MSD_PHY_INT_8)
        {
            CLI_INFO("PHYInt 8:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 8:              N\n");
        }

        if (data & MSD_PHY_INT_7)
        {
            CLI_INFO("PHYInt 7:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 7:              N\n");
        }

        if (data & MSD_PHY_INT_6)
        {
            CLI_INFO("PHYInt 6:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 6:              N\n");
        }

        if (data & MSD_PHY_INT_5)
        {
            CLI_INFO("PHYInt 5:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 5:              N\n");
        }

        if (data & MSD_PHY_INT_4)
        {
            CLI_INFO("PHYInt 4:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 4:              N\n");
        }

        if (data & MSD_PHY_INT_3)
        {
            CLI_INFO("PHYInt 3:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 3:              N\n");
        }

        if (data & MSD_PHY_INT_2)
        {
            CLI_INFO("PHYInt 2:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 2:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("PHYInt 1:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 1:              N\n");
        }
	}

    /* Fir */
    if (qddev->devName == MSD_FIR)
    {
        if (data & MSD_PHY_INT_9)
        {
            CLI_INFO("PHYInt 9:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 9:              N\n");
        }

        if (data & MSD_PHY_INT_8)
        {
            CLI_INFO("PHYInt 8:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 8:              N\n");
        }

        if (data & MSD_PHY_INT_7)
        {
            CLI_INFO("PHYInt 7:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 7:              N\n");
        }

        if (data & MSD_PHY_INT_6)
        {
            CLI_INFO("PHYInt 6:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 6:              N\n");
        }

        if (data & MSD_PHY_INT_5)
        {
            CLI_INFO("PHYInt 5:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 5:              N\n");
        }

        if (data & MSD_PHY_INT_4)
        {
            CLI_INFO("PHYInt 4:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 4:              N\n");
        }

        if (data & MSD_PHY_INT_3)
        {
            CLI_INFO("PHYInt 3:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 3:              N\n");
        }

        if (data & MSD_PHY_INT_2)
        {
            CLI_INFO("PHYInt 2:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 2:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("PHYInt 1:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 1:              N\n");
        }
    }

    /* Bonsai, Bonsai-Z1 */
    if (qddev->devName == MSD_BONSAI || qddev->devName == MSD_BONSAIZ1)
    {
        if (data & MSD_PHY_INT_6)
        {
            CLI_INFO("PHYInt 6:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 6:              N\n");
        }

        if (data & MSD_PHY_INT_5)
        {
            CLI_INFO("PHYInt 5:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 5:              N\n");
        }

        if (data & MSD_PHY_INT_4)
        {
            CLI_INFO("PHYInt 4:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 4:              N\n");
        }

        if (data & MSD_PHY_INT_3)
        {
            CLI_INFO("PHYInt 3:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 3:              N\n");
        }

        if (data & MSD_PHY_INT_2)
        {
            CLI_INFO("PHYInt 2:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 2:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("PHYInt 1:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 1:              N\n");
        }
    }

    /* Amethyst */
	if (qddev->devName == MSD_AMETHYST)
	{
		if (data & MSD_PHY_INT_10)
		{
			CLI_INFO("PHYInt 10:             Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 10:             N\n");
		}

		if (data & MSD_PHY_INT_9)
		{
			CLI_INFO("PHYInt 9:              Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 9:              N\n");
		}

		if (data & MSD_PHY_INT_8)
		{
			CLI_INFO("PHYInt 8:              Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 8:              N\n");
		}

		if (data & MSD_PHY_INT_7)
		{
			CLI_INFO("PHYInt 7:              Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 7:              N\n");
		}

		if (data & MSD_PHY_INT_6)
		{
			CLI_INFO("PHYInt 6:              Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 6:              N\n");
		}

        if (data & MSD_PHY_INT_5)
        {
            CLI_INFO("PHYInt 5:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 5:              N\n");
        }

        if (data & MSD_PHY_INT_4)
        {
            CLI_INFO("PHYInt 4:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 4:              N\n");
        }

        if (data & MSD_PHY_INT_3)
        {
            CLI_INFO("PHYInt 3:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 3:              N\n");
        }

        if (data & MSD_PHY_INT_2)
        {
            CLI_INFO("PHYInt 2:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 2:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("PHYInt 1:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 1:              N\n");
        }
	}

    /* Pearl */
	if (qddev->devName == MSD_PEARL)
	{
        if (data & MSD_PHY_INT_4)
        {
            CLI_INFO("PHYInt 4:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 4:              N\n");
        }

        if (data & MSD_PHY_INT_3)
        {
            CLI_INFO("PHYInt 3:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 3:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("SERDES Int 1:          Y\n");
        }
        else
        {
            CLI_INFO("SERDES Int 1:          N\n");
        }
        if (data & MSD_PHY_INT_0)
        {
            CLI_INFO("SERDES Int 0:          Y\n");
        }
        else
        {
            CLI_INFO("SERDES Int 0:          N\n");
        }
	}

    /* Agate */
	if (qddev->devName == MSD_AGATE)
	{
        if (data & MSD_SERDES_INT_11)
        {
            CLI_INFO("SERDES Int:            Y\n");
        }
        else
        {
            CLI_INFO("SERDES Int:            N\n");
        }

        if (data & MSD_PHY_INT_2)
        {
            CLI_INFO("PHYInt 2:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 2:              N\n");
        }

        if (data & MSD_PHY_INT_1)
        {
            CLI_INFO("PHYInt 1:              Y\n");
        }
        else
        {
            CLI_INFO("PHYInt 1:              N\n");
        }

		if (data & MSD_PHY_INT_0)
		{
			CLI_INFO("PHYInt 0:              Y\n");
		}
		else
		{
			CLI_INFO("PHYInt 0:              N\n");
		}
	}

	return 0;
}
/*Interrupt Global Status Register, G1 offset 0x0*/
int activeGetIntStatus(void)
{
	MSD_STATUS retVal;
	MSD_U16 data;

	if (nargs != 2)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	retVal = msdSysActiveIntStatusGet(sohoDevNum, &data);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error device interrupt status ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	CLI_INFO("Active Int Status data : 0x%x\n", data);

	CLI_INFO("-------------------------------\n");
	if (qddev->devName == MSD_AMETHYST)
	{
		if (data & MSD_DEVICE2_INT)
		{
			CLI_INFO("Device2Int:            Y\n");
		}
		else
		{
			CLI_INFO("Device2Int:            N\n");
		}
	}

	if (data & MSD_AVB_INT)
	{
		CLI_INFO("AVBInt:                Y\n");
	}
	else
	{
		CLI_INFO("AVBInt:                N\n");
	}

	if (data & MSD_DEVICE_INT)
	{
		CLI_INFO("DeviceInt:             Y\n");
	}
	else
	{
		CLI_INFO("DeviceInt:             N\n");
	}

	if (data & MSD_STATS_DONE)
	{
		CLI_INFO("StatsDone:             Y\n");
	}
	else
	{
		CLI_INFO("StatsDone:             N\n");
	}

	if (data & MSD_VTU_PROB)
	{
		CLI_INFO("VTUProb:               Y\n");
	}
	else
	{
		CLI_INFO("VTUProb:               N\n");
	}

	if (data & MSD_VTU_DONE)
	{
		CLI_INFO("VTUDone:               Y\n");
	}
	else
	{
		CLI_INFO("VTUDone:               N\n");
	}

	if (data & MSD_ATU_PROB)
	{
		CLI_INFO("ATUProb:               Y\n");
	}
	else
	{
		CLI_INFO("ATUProb:               N\n");
	}

	if (data & MSD_ATU_DONE)
	{
		CLI_INFO("ATUDone:               Y\n");
	}
	else
	{
		CLI_INFO("ATUDone:               N\n");
	}

	if (data & MSD_TCAM_INT)
	{
		CLI_INFO("TCAM Int:              Y\n");
	}
	else
	{
		CLI_INFO("TCAM Int:              N\n");
	}

	if (data & MSD_EE_INT)
	{
		CLI_INFO("EEInt:                 Y\n");
	}
	else
	{
		CLI_INFO("EEInt:                 N\n");
	}

	return 0;
}

static int displayTxSecY(MSD_SECY_EGRESS* secY)
{
	char* cmd = "macsec";

	CLI_INFO("----- Egress secY -----\n");
	if (checkValidItem(cmd, "createTxSecY", "-txSecYCounterIndex"))
		CLI_INFO("txSecYCounterIndex         : %02x\n", secY->secYCounterIndex);
	if (checkValidItem(cmd, "createTxSecY", "-txCipher"))
		CLI_INFO("txCipher         : %01x\n", secY->cipher);
	if (checkValidItem(cmd, "createTxSecY", "-txConfOffset"))
		CLI_INFO("txConfOffset         : %02x\n", secY->txConfOffset);
	if (checkValidItem(cmd, "createTxSecY", "-protectFrame"))
		CLI_INFO("protectFrame         : %01x\n", secY->protectFrame);
	if (checkValidItem(cmd, "createTxSecY", "-txSecYEn"))
		CLI_INFO("txSecYEn         : %01x\n", secY->txSecYEn);
	if (checkValidItem(cmd, "createTxSecY", "-afterSkipTag"))
		CLI_INFO("afterSkipTag         : %01x\n", secY->afterSkipTag);
	if (checkValidItem(cmd, "createTxSecY", "-afterRTag"))
		CLI_INFO("afterRTag         : %01x\n", secY->afterRTag);
	if (checkValidItem(cmd, "createTxSecY", "-afterQTag"))
		CLI_INFO("afterQTag         : %01x\n", secY->afterQTag);
	if (checkValidItem(cmd, "createTxSecY", "-afterADTag"))
		CLI_INFO("afterADTag         : %01x\n", secY->afterADTag);
	if (checkValidItem(cmd, "createTxSecY", "-secTagTCI"))
		CLI_INFO("secTagTCI         : %01x\n", secY->secTagTCI);
	
	return 0;
}
static int displayRxSecY(MSD_SECY_INGRESS* secY)
{
	char* cmd = "macsec";
	CLI_INFO("----- Ingress secY -----\n");
	if (checkValidItem(cmd, "createRxSecY", "-allowZeroRxPN"))
		CLI_INFO("allowZeroRxPN         : %02x\n", secY->allowZeroRxPN);
	if (checkValidItem(cmd, "createRxSecY", "-rxSecYCounterIndex"))
		CLI_INFO("rxSecYCounterIndex         : %02x\n", secY->secYCounterIndex);
	if (checkValidItem(cmd, "createRxSecY", "-rxCipher"))
		CLI_INFO("rxCipher         : %01x\n", secY->cipher);
	if (checkValidItem(cmd, "createRxSecY", "-rxConfOffset"))
		CLI_INFO("rxConfOffset         : %02x\n", secY->rxConfOffset);
	if (checkValidItem(cmd, "createRxSecY", "-replayProtect"))
		CLI_INFO("replayProtect         : %01x\n", secY->replayProtect);
	if (checkValidItem(cmd, "createRxSecY", "-validateFrame"))
		CLI_INFO("validateFrame         : %01x\n", secY->validateFrame);
	if (checkValidItem(cmd, "createRxSecY", "-rxSecYEn"))
		CLI_INFO("rxSecYEn         : %01x\n", secY->rxSecYEn);
	if (checkValidItem(cmd, "createRxSecY", "-replayWindow"))
		CLI_INFO("replayWindow         : %08x\n", secY->replayWindow);
	
	return 0;
}
static int displayRule(MSD_MSEC_RULE* rule)
{
    char* cmd = "macsec";
        if (checkValidItem(cmd, "createRule", "-keySPV"))
            CLI_INFO("keySPV         : %02x\n", rule->keySPV);
        if (checkValidItem(cmd, "createRule", "-maskSPV"))
            CLI_INFO("maskSPV         : %02x\n", rule->maskSPV);
        if (checkValidItem(cmd, "createRule", "-keySecTagC"))
            CLI_INFO("keySecTagC         : %02x\n", rule->keySecTagC);
        if (checkValidItem(cmd, "createRule", "-maskSecTagC"))
            CLI_INFO("maskSecTagC         : %02x\n", rule->maskSecTagC);
        if (checkValidItem(cmd, "createRule", "-keySecTagE"))
            CLI_INFO("keySecTagE         : %02x\n", rule->keySecTagE);
        if (checkValidItem(cmd, "createRule", "-maskSecTagE"))
            CLI_INFO("maskSecTagE         : %02x\n", rule->maskSecTagE);
        if (checkValidItem(cmd, "createRule", "-keySecTag"))
            CLI_INFO("keySecTag         : %02x\n", rule->keySecTag);
        if (checkValidItem(cmd, "createRule", "-maskSecTag"))
            CLI_INFO("maskSecTag         : %02x\n", rule->maskSecTag);
        if (checkValidItem(cmd, "createRule", "-keySkipSecTag"))
            CLI_INFO("keySkipSecTag         : %02x\n", rule->keySkipSecTag);
        if (checkValidItem(cmd, "createRule", "-maskSkipSecTag"))
            CLI_INFO("maskSkipSecTag         : %02x\n", rule->maskSkipSecTag);
        if (checkValidItem(cmd, "createRule", "-keyRTag"))
            CLI_INFO("keyRTag         : %02x\n", rule->keyRTag);
        if (checkValidItem(cmd, "createRule", "-maskRTag"))
            CLI_INFO("maskRTag         : %02x\n", rule->maskRTag);
        if (checkValidItem(cmd, "createRule", "-keyQTag"))
            CLI_INFO("keyQTag         : %02x\n", rule->keyQTag);
        if (checkValidItem(cmd, "createRule", "-maskQtag"))
            CLI_INFO("maskQtag         : %02x\n", rule->maskQTag);
        if (checkValidItem(cmd, "createRule", "-keyADTag"))
            CLI_INFO("keyADTag         : %02x\n", rule->keyADTag);
        if (checkValidItem(cmd, "createRule", "-maskADTag"))
            CLI_INFO("maskADTag         : %02x\n", rule->maskADTag);
        if (checkValidItem(cmd, "createRule", "-keyDirection"))
            CLI_INFO("keyDirection         : %02x\n", rule->keyDirection);
        if (checkValidItem(cmd, "createRule", "-maskDirection"))
            CLI_INFO("maskDirection         : %02x\n", rule->maskDirection);
        if (checkValidItem(cmd, "createRule", "-keyMAC_DA"))
            CLI_INFO("keyMAC_DA         : %02x%02x%02x%02x%02x%02x\n", rule->keyMAC_DA[0], rule->keyMAC_DA[1], rule->keyMAC_DA[2], rule->keyMAC_DA[3], rule->keyMAC_DA[4], rule->keyMAC_DA[5]);
        if (checkValidItem(cmd, "createRule", "-maskMAC_DA"))
            CLI_INFO("maskMAC_DA         : %02x%02x%02x%02x%02x%02x\n", rule->maskMAC_DA[0], rule->maskMAC_DA[1], rule->maskMAC_DA[2], rule->maskMAC_DA[3], rule->maskMAC_DA[4], rule->maskMAC_DA[5]);
        if (checkValidItem(cmd, "createRule", "-keyMAC_SA"))
            CLI_INFO("keyMAC_SA         : %02x%02x%02x%02x%02x%02x\n", rule->keyMAC_SA[0], rule->keyMAC_SA[1], rule->keyMAC_SA[2], rule->keyMAC_SA[3], rule->keyMAC_SA[4], rule->keyMAC_SA[5]);
        if (checkValidItem(cmd, "createRule", "-maskMAC_SA"))
            CLI_INFO("keyMAC_SA         : %02x%02x%02x%02x%02x%02x\n", rule->maskMAC_SA[0], rule->maskMAC_SA[1], rule->maskMAC_SA[2], rule->maskMAC_SA[3], rule->maskMAC_SA[4], rule->maskMAC_SA[5]);
        if (checkValidItem(cmd, "createRule", "-keyAdPCP"))
            CLI_INFO("keyAdPCP         : %02x\n", rule->keyAdPCP);
        if (checkValidItem(cmd, "createRule", "-maskAdPCP"))
            CLI_INFO("maskAdPCP         : %02x\n", rule->maskAdPCP);
        if (checkValidItem(cmd, "createRule", "-keyAdVID"))
            CLI_INFO("keyAdVID         : %02x\n", rule->keyAdVID);
        if (checkValidItem(cmd, "createRule", "-maskAdVID"))
            CLI_INFO("maskAdVID         : %02x\n", rule->maskAdVID);
        if (checkValidItem(cmd, "createRule", "-keyQPCP"))
            CLI_INFO("keyQPCP         : %02x\n", rule->keyQPCP);
        if (checkValidItem(cmd, "createRule", "-maskQPCP"))
            CLI_INFO("maskQPCP         : %02x\n", rule->maskQPCP);
        if (checkValidItem(cmd, "createRule", "-keyQVID"))
            CLI_INFO("keyQVID         : %04x\n", rule->keyQVID);
        if (checkValidItem(cmd, "createRule", "-maskQVID"))
            CLI_INFO("maskQVID         : %04x\n", rule->maskQVID);
        if (checkValidItem(cmd, "createRule", "-keyLengthType"))
            CLI_INFO("keyLengthType         : %04x\n", rule->keyLengthType);
        if (checkValidItem(cmd, "createRule", "-maskLengthType"))
            CLI_INFO("maskLengthType         : %04x\n", rule->maskLengthType);
        if (checkValidItem(cmd, "createRule", "-TxSCIndex"))
            CLI_INFO("TxSCIndex         : %02x\n", rule->action.TxSCIndex);
        if (checkValidItem(cmd, "createRule", "-isControlledPort"))
            CLI_INFO("isControlledPort         : %01x\n", rule->action.isControlledPort);
        if (checkValidItem(cmd, "createRule", "-secYIndex"))
            CLI_INFO("SecYIndex         : %02x\n", rule->action.SecYIndex);
    return 0;
}
static int displayRxSC(MSD_MSEC_RX_SC* rxSC)
{
    char* cmd = "macsec";
        if (checkValidItem(cmd, "createRxSC", "-scCounterIndex"))
            CLI_INFO("scCounterIndex         : %02x\n", rxSC->scCounterIndex);
        if (checkValidItem(cmd, "createRxSC", "-secYIndex"))
            CLI_INFO("secYIndex         : %02x\n", rxSC->secYIndex);
        if (checkValidItem(cmd, "createRxSC", "-sci"))
            CLI_INFO("sci         : %016llx\n", rxSC->sci);
        if (checkValidItem(cmd, "createRxSC", "-an"))
            CLI_INFO("rxSA         : %02x\n", rxSC->rxSA->an);
        if (checkValidItem(cmd, "createRxSC", "-saIndex"))
            CLI_INFO("rxSA         : %02x\n", rxSC->rxSA->saIndex);
        if (checkValidItem(cmd, "createRxSC", "-valid"))
            CLI_INFO("rxSA         : %02x\n", rxSC->rxSA->valid);
        
    return 0;
}
static int displayRxSA(MSD_MSEC_RX_SA* rxSA)
{
    char* cmd = "macsec";
        if (checkValidItem(cmd, "createRxSA", "-rxSAValid"))
            CLI_INFO("rxSAValid     : %01x\n", rxSA->rxSAValid);
        if (checkValidItem(cmd, "createRxSA", "-salt"))
            CLI_INFO("salt      : {%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}\n", rxSA->salt[0], rxSA->salt[1], rxSA->salt[2], rxSA->salt[3], rxSA->salt[4], rxSA->salt[5], rxSA->salt[6], rxSA->salt[7], rxSA->salt[8], rxSA->salt[9], rxSA->salt[10], rxSA->salt[11]);
		if (checkValidItem(cmd, "createRxSA", "-sak"))
			CLI_INFO("sak      : {%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}\n", rxSA->sak[0], rxSA->sak[1], rxSA->sak[2], rxSA->sak[3], rxSA->sak[4], rxSA->sak[5], rxSA->sak[6], rxSA->sak[7], rxSA->sak[8], rxSA->sak[9], rxSA->sak[10], rxSA->sak[11], rxSA->sak[12], rxSA->sak[13], rxSA->sak[14], rxSA->sak[15], rxSA->sak[16], rxSA->sak[17], rxSA->sak[18], rxSA->sak[19], rxSA->sak[20], rxSA->sak[21], rxSA->sak[22], rxSA->sak[23], rxSA->sak[24], rxSA->sak[25], rxSA->sak[26], rxSA->sak[27], rxSA->sak[28], rxSA->sak[29], rxSA->sak[30], rxSA->sak[31]);
		if (checkValidItem(cmd, "createRxSA", "-ssci"))
            CLI_INFO("ssci      : %08x\n", rxSA->ssci);
        if (checkValidItem(cmd, "createRxSA", "-nextPN"))
            CLI_INFO("nextPN       : %016llx\n", rxSA->nextPN);

    return 0;
}
static int displayRxSANextPN(MSD_U64 nextPN)
{
    char* cmd = "macsec";
            CLI_INFO("RxSANextPN     : %016llx\n", nextPN);

    return 0;
}
static int displayTxSC(MSD_MSEC_TX_SC* txSC)
{
    char* cmd = "macsec";
		if (checkValidItem(cmd, "createTxSC", "-txNextPNRolloEn"))
			CLI_INFO("txNextPNRolloEn     : %02x\n", txSC->txNextPNRolloEn);
        if (checkValidItem(cmd, "createTxSC", "-scCounterIndex"))
            CLI_INFO("scCounterIndex     : %02x\n", txSC->scCounterIndex);
        if (checkValidItem(cmd, "createTxSC", "-sci"))
            CLI_INFO("sci    : %02x\n", txSC->sci);
        if (checkValidItem(cmd, "createTxSC", "-enable_auto_rekey"))
            CLI_INFO("enable_auto_rekey     : %01x\n", txSC->enable_auto_rekey);
		if (checkValidItem(cmd, "createTxSC", "-sa0An"))
			CLI_INFO("sa0An      : %02x\n", txSC->sa0An);
		if (checkValidItem(cmd, "createTxSC", "-sa1An"))
			CLI_INFO("sa1An      : %02x\n", txSC->sa1An);
        if (checkValidItem(cmd, "createTxSC", "-sA0valid"))
            CLI_INFO("SA0valid     : %01x\n", txSC->sA0Valid);
        if (checkValidItem(cmd, "createTxSC", "-sA1Valid"))
            CLI_INFO("sA1Valid    : %01x\n", txSC->sA1Valid);
        if (checkValidItem(cmd, "createTxSC", "-sa0Index"))
            CLI_INFO("sa0Index      : %02x\n", txSC->sa0Index);
		if (checkValidItem(cmd, "createTxSC", "-sa1Index"))
			CLI_INFO("sa1Index      : %02x\n", txSC->sa1Index);
		if (checkValidItem(cmd, "createTxSC", "-isActiveSA1"))
            CLI_INFO("nextPN       : %01x\n", txSC->isActiveSA1);


    return 0;
}
static int displayTxSA(MSD_MSEC_TX_SA* txSA)
{
    char* cmd = "macsec";
        if (checkValidItem(cmd, "createTxSA", "-txSAValid"))
            CLI_INFO("txSAValid    : %01x\n", txSA->txSAValid);
		if (checkValidItem(cmd, "createTxSA", "-sak"))
			CLI_INFO("sak     : {%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}\n", txSA->sak[0], txSA->sak[1], txSA->sak[2], txSA->sak[3], txSA->sak[4], txSA->sak[5], txSA->sak[6], txSA->sak[7], txSA->sak[8], txSA->sak[9], txSA->sak[10], txSA->sak[11], txSA->sak[12], txSA->sak[13], txSA->sak[14], txSA->sak[15], txSA->sak[16], txSA->sak[17], txSA->sak[18], txSA->sak[19], txSA->sak[20], txSA->sak[21], txSA->sak[22], txSA->sak[23], txSA->sak[24], txSA->sak[25], txSA->sak[26], txSA->sak[27], txSA->sak[28], txSA->sak[29], txSA->sak[30], txSA->sak[31]);
	    if (checkValidItem(cmd, "createTxSA", "-salt"))
            CLI_INFO("salt     : {%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x}\n", txSA->salt[0], txSA->salt[1], txSA->salt[2], txSA->salt[3], txSA->salt[4], txSA->salt[5], txSA->salt[6], txSA->salt[7], txSA->salt[8], txSA->salt[9], txSA->salt[10], txSA->salt[11]);
        if (checkValidItem(cmd, "createTxSA", "-ssci"))
            CLI_INFO("ssci    : %02x\n", txSA->ssci);
		if (checkValidItem(cmd, "createTxSA", "-nextPN"))
            CLI_INFO("nextPN       : %02x\n", txSA->nextPN);
    return 0;
}
static int displayTxSANextPN(MSD_U64 nextPN)
{
    char* cmd = "macsec";
        CLI_INFO("TxSANextPN     : %016llx\n", nextPN);

    return 0;
}
static int displayRXSCStates(MSD_MSEC_RX_SC_COUNTER* states)
{
    CLI_INFO("inPktsSCLate     : %08x\n", states->inPktsSCLate);
    CLI_INFO("inPktsSCNotValid     : %08x\n", states->inPktsSCNotValid);
    CLI_INFO("inPktsSCInvalid    : %08x\n", states->inPktsSCInvalid);
    CLI_INFO("inPktsSCDelayed     : %08x\n", states->inPktsSCDelayed);
    CLI_INFO("inPktsSCUnchecked     : %08x\n", states->inPktsSCUnchecked);
    CLI_INFO("inPktsSCOK    : %08x\n", states->inPktsSCOK);

    return 0;
}
static int displayRXSecYStates(MSD_MSEC_RX_SECY_COUNTER* states)
{
    CLI_INFO("inOctetsSecYControlledPort     : %016llx\n", states->inOctetsSecYControlledPort);
    CLI_INFO("inOctetsSecYValidated     : %016llx\n", states->inOctetsSecYValidated);
    CLI_INFO("inOctetsSecYDecrypted    : %016llx\n", states->inOctetsSecYDecrypted);
    CLI_INFO("inPktsSecYUnicast     : %08x\n", states->inPktsSecYUnicast);
    CLI_INFO("inPktsSecYMulticast     : %08x\n", states->inPktsSecYMulticast);
    CLI_INFO("inPktsSecYBroadCast    : %08x\n", states->inPktsSecYBroadCast);
    CLI_INFO("inPktsSecYUnTagged    : %08x\n", states->inPktsSecYUnTagged);
    CLI_INFO("inPktsSecYNoTag    : %08x\n", states->inPktsSecYNoTag);
    CLI_INFO("inPktsSecYBadTag    : %08x\n", states->inPktsSecYBadTag);
    CLI_INFO("inPktsSecYNoSA    : %08x\n", states->inPktsSecYNoSA);
    CLI_INFO("inPktsSecYNoSAErr    : %08x\n", states->inPktsSecYNoSAErr);
    return 0;
}
static int displayTXSCStates(MSD_MSEC_TX_SC_COUNTER* states)
{
    CLI_INFO("outPktsProtected     : %08x\n", states->outPktsProtected);
    CLI_INFO("outPktsEncrypted     : %08x\n", states->outPktsEncrypted);

    return 0;
}
static int displayTXSecYStates(MSD_MSEC_TX_SECY_COUNTER* states)
{
    CLI_INFO("outOctetsSecYControlledPort     : %016llx\n", states->outOctetsSecYControlledPort);
    CLI_INFO("outOctetsSecYEncrypted     : %016llx\n", states->outOctetsSecYEncrypted);
    CLI_INFO("outOctetsSecYEncrypted     : %016llx\n", states->outOctetsSecYProtected);
    CLI_INFO("outPktsSecYUnicast     : %08x\n", states->outPktsSecYUnicast);
    CLI_INFO("outPktsSecYMulticast     : %08x\n", states->outPktsSecYMulticast);
    CLI_INFO("outPktSecYBroadcast     : %08x\n", states->outPktSecYBroadcast);
    CLI_INFO("outPktsSecYuntagged     : %08x\n", states->outPktsSecYuntagged);
    CLI_INFO("outPktsSecYTooLong     : %08x\n", states->outPktsSecYTooLong);
	CLI_INFO("outPktsSecYNoSAError     : %08x\n", states->outPktsSecYNoSAError);
    return 0;
}
static int displayAllRXSCStates(MSD_MSEC_RX_SC_COUNTER rxSCStates[])
{
	CLI_INFO("inPktsSCLate                 : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n",\
		rxSCStates[0].inPktsSCLate, rxSCStates[1].inPktsSCLate, rxSCStates[2].inPktsSCLate, rxSCStates[3].inPktsSCLate,\
		rxSCStates[4].inPktsSCLate, rxSCStates[5].inPktsSCLate, rxSCStates[6].inPktsSCLate, rxSCStates[7].inPktsSCLate,\
		rxSCStates[8].inPktsSCLate, rxSCStates[9].inPktsSCLate, rxSCStates[10].inPktsSCLate, rxSCStates[11].inPktsSCLate,\
		rxSCStates[12].inPktsSCLate, rxSCStates[13].inPktsSCLate, rxSCStates[14].inPktsSCLate, rxSCStates[15].inPktsSCLate);
	CLI_INFO("inPktsSCNotValid             : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSCStates[0].inPktsSCNotValid, rxSCStates[1].inPktsSCNotValid, rxSCStates[2].inPktsSCNotValid, rxSCStates[3].inPktsSCNotValid, \
		rxSCStates[4].inPktsSCNotValid, rxSCStates[5].inPktsSCNotValid, rxSCStates[6].inPktsSCNotValid, rxSCStates[7].inPktsSCNotValid, \
		rxSCStates[8].inPktsSCNotValid, rxSCStates[9].inPktsSCNotValid, rxSCStates[10].inPktsSCNotValid, rxSCStates[11].inPktsSCNotValid, \
		rxSCStates[12].inPktsSCNotValid, rxSCStates[13].inPktsSCNotValid, rxSCStates[14].inPktsSCNotValid, rxSCStates[15].inPktsSCNotValid);
	CLI_INFO("inPktsSCInvalid              : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSCStates[0].inPktsSCInvalid, rxSCStates[1].inPktsSCInvalid, rxSCStates[2].inPktsSCInvalid, rxSCStates[3].inPktsSCInvalid, \
		rxSCStates[4].inPktsSCInvalid, rxSCStates[5].inPktsSCInvalid, rxSCStates[6].inPktsSCInvalid, rxSCStates[7].inPktsSCInvalid, \
		rxSCStates[8].inPktsSCInvalid, rxSCStates[9].inPktsSCInvalid, rxSCStates[10].inPktsSCInvalid, rxSCStates[11].inPktsSCInvalid, \
		rxSCStates[12].inPktsSCInvalid, rxSCStates[13].inPktsSCInvalid, rxSCStates[14].inPktsSCInvalid, rxSCStates[15].inPktsSCInvalid);
	CLI_INFO("inPktsSCDelayed              : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSCStates[0].inPktsSCDelayed, rxSCStates[1].inPktsSCDelayed, rxSCStates[2].inPktsSCDelayed, rxSCStates[3].inPktsSCDelayed, \
		rxSCStates[4].inPktsSCDelayed, rxSCStates[5].inPktsSCDelayed, rxSCStates[6].inPktsSCDelayed, rxSCStates[7].inPktsSCDelayed, \
		rxSCStates[8].inPktsSCDelayed, rxSCStates[9].inPktsSCDelayed, rxSCStates[10].inPktsSCDelayed, rxSCStates[11].inPktsSCDelayed, \
		rxSCStates[12].inPktsSCDelayed, rxSCStates[13].inPktsSCDelayed, rxSCStates[14].inPktsSCDelayed, rxSCStates[15].inPktsSCDelayed);
	CLI_INFO("inPktsSCUnchecked            : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSCStates[0].inPktsSCUnchecked, rxSCStates[1].inPktsSCUnchecked, rxSCStates[2].inPktsSCUnchecked, rxSCStates[3].inPktsSCUnchecked, \
		rxSCStates[4].inPktsSCUnchecked, rxSCStates[5].inPktsSCUnchecked, rxSCStates[6].inPktsSCUnchecked, rxSCStates[7].inPktsSCUnchecked, \
		rxSCStates[8].inPktsSCUnchecked, rxSCStates[9].inPktsSCUnchecked, rxSCStates[10].inPktsSCUnchecked, rxSCStates[11].inPktsSCUnchecked, \
		rxSCStates[12].inPktsSCUnchecked, rxSCStates[13].inPktsSCUnchecked, rxSCStates[14].inPktsSCUnchecked, rxSCStates[15].inPktsSCUnchecked);
	CLI_INFO("inPktsSCOK                   : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSCStates[0].inPktsSCOK, rxSCStates[1].inPktsSCOK, rxSCStates[2].inPktsSCOK, rxSCStates[3].inPktsSCOK, \
		rxSCStates[4].inPktsSCOK, rxSCStates[5].inPktsSCOK, rxSCStates[6].inPktsSCOK, rxSCStates[7].inPktsSCOK, \
		rxSCStates[8].inPktsSCOK, rxSCStates[9].inPktsSCOK, rxSCStates[10].inPktsSCOK, rxSCStates[11].inPktsSCOK, \
		rxSCStates[12].inPktsSCOK, rxSCStates[13].inPktsSCOK, rxSCStates[14].inPktsSCOK, rxSCStates[15].inPktsSCOK);
	return 0;
}
static int displayAllRXSecYStates(MSD_MSEC_RX_SECY_COUNTER rxSecYStates[])
{
	CLI_INFO("inOctetsSecYControlledPort   : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		rxSecYStates[0].inOctetsSecYControlledPort, rxSecYStates[1].inOctetsSecYControlledPort, rxSecYStates[2].inOctetsSecYControlledPort, rxSecYStates[3].inOctetsSecYControlledPort, \
		rxSecYStates[4].inOctetsSecYControlledPort, rxSecYStates[5].inOctetsSecYControlledPort, rxSecYStates[6].inOctetsSecYControlledPort, rxSecYStates[7].inOctetsSecYControlledPort, \
		rxSecYStates[8].inOctetsSecYControlledPort, rxSecYStates[9].inOctetsSecYControlledPort, rxSecYStates[10].inOctetsSecYControlledPort, rxSecYStates[11].inOctetsSecYControlledPort, \
		rxSecYStates[12].inOctetsSecYControlledPort, rxSecYStates[13].inOctetsSecYControlledPort, rxSecYStates[14].inOctetsSecYControlledPort, rxSecYStates[15].inOctetsSecYControlledPort);
	CLI_INFO("inOctetsSecYValidated        : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		rxSecYStates[0].inOctetsSecYValidated, rxSecYStates[1].inOctetsSecYValidated, rxSecYStates[2].inOctetsSecYValidated, rxSecYStates[3].inOctetsSecYValidated, \
		rxSecYStates[4].inOctetsSecYValidated, rxSecYStates[5].inOctetsSecYValidated, rxSecYStates[6].inOctetsSecYValidated, rxSecYStates[7].inOctetsSecYValidated, \
		rxSecYStates[8].inOctetsSecYValidated, rxSecYStates[9].inOctetsSecYValidated, rxSecYStates[10].inOctetsSecYValidated, rxSecYStates[11].inOctetsSecYValidated, \
		rxSecYStates[12].inOctetsSecYValidated, rxSecYStates[13].inOctetsSecYValidated, rxSecYStates[14].inOctetsSecYValidated, rxSecYStates[15].inOctetsSecYValidated);
	CLI_INFO("inOctetsSecYDecrypted        : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		rxSecYStates[0].inOctetsSecYDecrypted, rxSecYStates[1].inOctetsSecYDecrypted, rxSecYStates[2].inOctetsSecYDecrypted, rxSecYStates[3].inOctetsSecYDecrypted, \
		rxSecYStates[4].inOctetsSecYDecrypted, rxSecYStates[5].inOctetsSecYDecrypted, rxSecYStates[6].inOctetsSecYDecrypted, rxSecYStates[7].inOctetsSecYDecrypted, \
		rxSecYStates[8].inOctetsSecYDecrypted, rxSecYStates[9].inOctetsSecYDecrypted, rxSecYStates[10].inOctetsSecYDecrypted, rxSecYStates[11].inOctetsSecYDecrypted, \
		rxSecYStates[12].inOctetsSecYDecrypted, rxSecYStates[13].inOctetsSecYDecrypted, rxSecYStates[14].inOctetsSecYDecrypted, rxSecYStates[15].inOctetsSecYDecrypted);
	CLI_INFO("inPktsSecYUnicast            : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYUnicast, rxSecYStates[1].inPktsSecYUnicast, rxSecYStates[2].inPktsSecYUnicast, rxSecYStates[3].inPktsSecYUnicast, \
		rxSecYStates[4].inPktsSecYUnicast, rxSecYStates[5].inPktsSecYUnicast, rxSecYStates[6].inPktsSecYUnicast, rxSecYStates[7].inPktsSecYUnicast, \
		rxSecYStates[8].inPktsSecYUnicast, rxSecYStates[9].inPktsSecYUnicast, rxSecYStates[10].inPktsSecYUnicast, rxSecYStates[11].inPktsSecYUnicast, \
		rxSecYStates[12].inPktsSecYUnicast, rxSecYStates[13].inPktsSecYUnicast, rxSecYStates[14].inPktsSecYUnicast, rxSecYStates[15].inPktsSecYUnicast);
	CLI_INFO("inPktsSecYMulticast          : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYMulticast, rxSecYStates[1].inPktsSecYMulticast, rxSecYStates[2].inPktsSecYMulticast, rxSecYStates[3].inPktsSecYMulticast, \
		rxSecYStates[4].inPktsSecYMulticast, rxSecYStates[5].inPktsSecYMulticast, rxSecYStates[6].inPktsSecYMulticast, rxSecYStates[7].inPktsSecYMulticast, \
		rxSecYStates[8].inPktsSecYMulticast, rxSecYStates[9].inPktsSecYMulticast, rxSecYStates[10].inPktsSecYMulticast, rxSecYStates[11].inPktsSecYMulticast, \
		rxSecYStates[12].inPktsSecYMulticast, rxSecYStates[13].inPktsSecYMulticast, rxSecYStates[14].inPktsSecYMulticast, rxSecYStates[15].inPktsSecYMulticast);
	CLI_INFO("inPktsSecYBroadCast          : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYBroadCast, rxSecYStates[1].inPktsSecYBroadCast, rxSecYStates[2].inPktsSecYBroadCast, rxSecYStates[3].inPktsSecYBroadCast, \
		rxSecYStates[4].inPktsSecYBroadCast, rxSecYStates[5].inPktsSecYBroadCast, rxSecYStates[6].inPktsSecYBroadCast, rxSecYStates[7].inPktsSecYBroadCast, \
		rxSecYStates[8].inPktsSecYBroadCast, rxSecYStates[9].inPktsSecYBroadCast, rxSecYStates[10].inPktsSecYBroadCast, rxSecYStates[11].inPktsSecYBroadCast, \
		rxSecYStates[12].inPktsSecYBroadCast, rxSecYStates[13].inPktsSecYBroadCast, rxSecYStates[14].inPktsSecYBroadCast, rxSecYStates[15].inPktsSecYBroadCast);
	CLI_INFO("inPktsSecYUnTagged           : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYUnTagged, rxSecYStates[1].inPktsSecYUnTagged, rxSecYStates[2].inPktsSecYUnTagged, rxSecYStates[3].inPktsSecYUnTagged, \
		rxSecYStates[4].inPktsSecYUnTagged, rxSecYStates[5].inPktsSecYUnTagged, rxSecYStates[6].inPktsSecYUnTagged, rxSecYStates[7].inPktsSecYUnTagged, \
		rxSecYStates[8].inPktsSecYUnTagged, rxSecYStates[9].inPktsSecYUnTagged, rxSecYStates[10].inPktsSecYUnTagged, rxSecYStates[11].inPktsSecYUnTagged, \
		rxSecYStates[12].inPktsSecYUnTagged, rxSecYStates[13].inPktsSecYUnTagged, rxSecYStates[14].inPktsSecYUnTagged, rxSecYStates[15].inPktsSecYUnTagged);
	CLI_INFO("inPktsSecYNoTag              : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYNoTag, rxSecYStates[1].inPktsSecYNoTag, rxSecYStates[2].inPktsSecYNoTag, rxSecYStates[3].inPktsSecYNoTag, \
		rxSecYStates[4].inPktsSecYNoTag, rxSecYStates[5].inPktsSecYNoTag, rxSecYStates[6].inPktsSecYNoTag, rxSecYStates[7].inPktsSecYNoTag, \
		rxSecYStates[8].inPktsSecYNoTag, rxSecYStates[9].inPktsSecYNoTag, rxSecYStates[10].inPktsSecYNoTag, rxSecYStates[11].inPktsSecYNoTag, \
		rxSecYStates[12].inPktsSecYNoTag, rxSecYStates[13].inPktsSecYNoTag, rxSecYStates[14].inPktsSecYNoTag, rxSecYStates[15].inPktsSecYNoTag);
	CLI_INFO("inPktsSecYBadTag             : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYBadTag, rxSecYStates[1].inPktsSecYBadTag, rxSecYStates[2].inPktsSecYBadTag, rxSecYStates[3].inPktsSecYBadTag, \
		rxSecYStates[4].inPktsSecYBadTag, rxSecYStates[5].inPktsSecYBadTag, rxSecYStates[6].inPktsSecYBadTag, rxSecYStates[7].inPktsSecYBadTag, \
		rxSecYStates[8].inPktsSecYBadTag, rxSecYStates[9].inPktsSecYBadTag, rxSecYStates[10].inPktsSecYBadTag, rxSecYStates[11].inPktsSecYBadTag, \
		rxSecYStates[12].inPktsSecYBadTag, rxSecYStates[13].inPktsSecYBadTag, rxSecYStates[14].inPktsSecYBadTag, rxSecYStates[15].inPktsSecYBadTag);
	CLI_INFO("inPktsSecYNoSA               : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYNoSA, rxSecYStates[1].inPktsSecYNoSA, rxSecYStates[2].inPktsSecYNoSA, rxSecYStates[3].inPktsSecYNoSA, \
		rxSecYStates[4].inPktsSecYNoSA, rxSecYStates[5].inPktsSecYNoSA, rxSecYStates[6].inPktsSecYNoSA, rxSecYStates[7].inPktsSecYNoSA, \
		rxSecYStates[8].inPktsSecYNoSA, rxSecYStates[9].inPktsSecYNoSA, rxSecYStates[10].inPktsSecYNoSA, rxSecYStates[11].inPktsSecYNoSA, \
		rxSecYStates[12].inPktsSecYNoSA, rxSecYStates[13].inPktsSecYNoSA, rxSecYStates[14].inPktsSecYNoSA, rxSecYStates[15].inPktsSecYNoSA);
	CLI_INFO("inPktsSecYNoSAErr            : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		rxSecYStates[0].inPktsSecYNoSAErr, rxSecYStates[1].inPktsSecYNoSAErr, rxSecYStates[2].inPktsSecYNoSAErr, rxSecYStates[3].inPktsSecYNoSAErr, \
		rxSecYStates[4].inPktsSecYNoSAErr, rxSecYStates[5].inPktsSecYNoSAErr, rxSecYStates[6].inPktsSecYNoSAErr, rxSecYStates[7].inPktsSecYNoSAErr, \
		rxSecYStates[8].inPktsSecYNoSAErr, rxSecYStates[9].inPktsSecYNoSAErr, rxSecYStates[10].inPktsSecYNoSAErr, rxSecYStates[11].inPktsSecYNoSAErr, \
		rxSecYStates[12].inPktsSecYNoSAErr, rxSecYStates[13].inPktsSecYNoSAErr, rxSecYStates[14].inPktsSecYNoSAErr, rxSecYStates[15].inPktsSecYNoSAErr);
	return 0;
}
static int displayAllTXSCStates(MSD_MSEC_TX_SC_COUNTER txSCStates[])
{
	CLI_INFO("outPktsProtected             : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x %08x  %08x  %08x  %08x  %08x\n", \
		txSCStates[0].outPktsProtected, txSCStates[1].outPktsProtected, txSCStates[2].outPktsProtected, txSCStates[3].outPktsProtected, \
		txSCStates[4].outPktsProtected, txSCStates[5].outPktsProtected, txSCStates[6].outPktsProtected, txSCStates[7].outPktsProtected, \
		txSCStates[8].outPktsProtected, txSCStates[9].outPktsProtected, txSCStates[10].outPktsProtected, txSCStates[11].outPktsProtected, \
		txSCStates[12].outPktsProtected, txSCStates[13].outPktsProtected, txSCStates[14].outPktsProtected, txSCStates[15].outPktsProtected);
	CLI_INFO("outPktsEncrypted             : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x %08x  %08x  %08x  %08x  %08x\n", \
		txSCStates[0].outPktsEncrypted, txSCStates[1].outPktsEncrypted, txSCStates[2].outPktsEncrypted, txSCStates[3].outPktsEncrypted, \
		txSCStates[4].outPktsEncrypted, txSCStates[5].outPktsEncrypted, txSCStates[6].outPktsEncrypted, txSCStates[7].outPktsEncrypted, \
		txSCStates[8].outPktsEncrypted, txSCStates[9].outPktsEncrypted, txSCStates[10].outPktsEncrypted, txSCStates[11].outPktsEncrypted, \
		txSCStates[12].outPktsEncrypted, txSCStates[13].outPktsEncrypted, txSCStates[14].outPktsEncrypted, txSCStates[15].outPktsEncrypted);
	return 0;
}
static int displayAllTXSecYStates(MSD_MSEC_TX_SECY_COUNTER txSecYStates[])
{
	CLI_INFO("outOctetsSecYControlledPort  : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		txSecYStates[0].outOctetsSecYControlledPort, txSecYStates[1].outOctetsSecYControlledPort, txSecYStates[2].outOctetsSecYControlledPort, txSecYStates[3].outOctetsSecYControlledPort, \
		txSecYStates[4].outOctetsSecYControlledPort, txSecYStates[5].outOctetsSecYControlledPort, txSecYStates[6].outOctetsSecYControlledPort, txSecYStates[7].outOctetsSecYControlledPort, \
		txSecYStates[8].outOctetsSecYControlledPort, txSecYStates[9].outOctetsSecYControlledPort, txSecYStates[10].outOctetsSecYControlledPort, txSecYStates[11].outOctetsSecYControlledPort, \
		txSecYStates[12].outOctetsSecYControlledPort, txSecYStates[13].outOctetsSecYControlledPort, txSecYStates[14].outOctetsSecYControlledPort, txSecYStates[15].outOctetsSecYControlledPort);
	CLI_INFO("outOctetsSecYEncrypted       : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		txSecYStates[0].outOctetsSecYEncrypted, txSecYStates[1].outOctetsSecYEncrypted, txSecYStates[2].outOctetsSecYEncrypted, txSecYStates[3].outOctetsSecYEncrypted, \
		txSecYStates[4].outOctetsSecYEncrypted, txSecYStates[5].outOctetsSecYEncrypted, txSecYStates[6].outOctetsSecYEncrypted, txSecYStates[7].outOctetsSecYEncrypted, \
		txSecYStates[8].outOctetsSecYEncrypted, txSecYStates[9].outOctetsSecYEncrypted, txSecYStates[10].outOctetsSecYEncrypted, txSecYStates[11].outOctetsSecYEncrypted, \
		txSecYStates[12].outOctetsSecYEncrypted, txSecYStates[13].outOctetsSecYEncrypted, txSecYStates[14].outOctetsSecYEncrypted, txSecYStates[15].outOctetsSecYEncrypted);
	CLI_INFO("outOctetsSecYProtected       : %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx    %016llx   %016llx    %016llx    %016llx    %016llx    %016llx\n", \
		txSecYStates[0].outOctetsSecYProtected, txSecYStates[1].outOctetsSecYProtected, txSecYStates[2].outOctetsSecYProtected, txSecYStates[3].outOctetsSecYProtected, \
		txSecYStates[4].outOctetsSecYProtected, txSecYStates[5].outOctetsSecYProtected, txSecYStates[6].outOctetsSecYProtected, txSecYStates[7].outOctetsSecYProtected, \
		txSecYStates[8].outOctetsSecYProtected, txSecYStates[9].outOctetsSecYProtected, txSecYStates[10].outOctetsSecYProtected, txSecYStates[11].outOctetsSecYProtected, \
		txSecYStates[12].outOctetsSecYProtected, txSecYStates[13].outOctetsSecYProtected, txSecYStates[14].outOctetsSecYProtected, txSecYStates[15].outOctetsSecYProtected);
	CLI_INFO("outPktsSecYUnicast           : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktsSecYUnicast, txSecYStates[1].outPktsSecYUnicast, txSecYStates[2].outPktsSecYUnicast, txSecYStates[3].outPktsSecYUnicast, \
		txSecYStates[4].outPktsSecYUnicast, txSecYStates[5].outPktsSecYUnicast, txSecYStates[6].outPktsSecYUnicast, txSecYStates[7].outPktsSecYUnicast, \
		txSecYStates[8].outPktsSecYUnicast, txSecYStates[9].outPktsSecYUnicast, txSecYStates[10].outPktsSecYUnicast, txSecYStates[11].outPktsSecYUnicast, \
		txSecYStates[12].outPktsSecYUnicast, txSecYStates[13].outPktsSecYUnicast, txSecYStates[14].outPktsSecYUnicast, txSecYStates[15].outPktsSecYUnicast);
	CLI_INFO("outPktsSecYMulticast         : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktsSecYMulticast, txSecYStates[1].outPktsSecYMulticast, txSecYStates[2].outPktsSecYMulticast, txSecYStates[3].outPktsSecYMulticast, \
		txSecYStates[4].outPktsSecYMulticast, txSecYStates[5].outPktsSecYMulticast, txSecYStates[6].outPktsSecYMulticast, txSecYStates[7].outPktsSecYMulticast, \
		txSecYStates[8].outPktsSecYMulticast, txSecYStates[9].outPktsSecYMulticast, txSecYStates[10].outPktsSecYMulticast, txSecYStates[11].outPktsSecYMulticast, \
		txSecYStates[12].outPktsSecYMulticast, txSecYStates[13].outPktsSecYMulticast, txSecYStates[14].outPktsSecYMulticast, txSecYStates[15].outPktsSecYMulticast);
	CLI_INFO("outPktSecYBroadcast          : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktSecYBroadcast, txSecYStates[1].outPktSecYBroadcast, txSecYStates[2].outPktSecYBroadcast, txSecYStates[3].outPktSecYBroadcast, \
		txSecYStates[4].outPktSecYBroadcast, txSecYStates[5].outPktSecYBroadcast, txSecYStates[6].outPktSecYBroadcast, txSecYStates[7].outPktSecYBroadcast, \
		txSecYStates[8].outPktSecYBroadcast, txSecYStates[9].outPktSecYBroadcast, txSecYStates[10].outPktSecYBroadcast, txSecYStates[11].outPktSecYBroadcast, \
		txSecYStates[12].outPktSecYBroadcast, txSecYStates[13].outPktSecYBroadcast, txSecYStates[14].outPktSecYBroadcast, txSecYStates[15].outPktSecYBroadcast);
	CLI_INFO("outPktsSecYuntagged          : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktsSecYuntagged, txSecYStates[1].outPktsSecYuntagged, txSecYStates[2].outPktsSecYuntagged, txSecYStates[3].outPktsSecYuntagged, \
		txSecYStates[4].outPktsSecYuntagged, txSecYStates[5].outPktsSecYuntagged, txSecYStates[6].outPktsSecYuntagged, txSecYStates[7].outPktsSecYuntagged, \
		txSecYStates[8].outPktsSecYuntagged, txSecYStates[9].outPktsSecYuntagged, txSecYStates[10].outPktsSecYuntagged, txSecYStates[11].outPktsSecYuntagged, \
		txSecYStates[12].outPktsSecYuntagged, txSecYStates[13].outPktsSecYuntagged, txSecYStates[14].outPktsSecYuntagged, txSecYStates[15].outPktsSecYuntagged);
	CLI_INFO("outPktsSecYTooLong           : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktsSecYTooLong, txSecYStates[1].outPktsSecYTooLong, txSecYStates[2].outPktsSecYTooLong, txSecYStates[3].outPktsSecYTooLong, \
		txSecYStates[4].outPktsSecYTooLong, txSecYStates[5].outPktsSecYTooLong, txSecYStates[6].outPktsSecYTooLong, txSecYStates[7].outPktsSecYTooLong, \
		txSecYStates[8].outPktsSecYTooLong, txSecYStates[9].outPktsSecYTooLong, txSecYStates[10].outPktsSecYTooLong, txSecYStates[11].outPktsSecYTooLong, \
		txSecYStates[12].outPktsSecYTooLong, txSecYStates[13].outPktsSecYTooLong, txSecYStates[14].outPktsSecYTooLong, txSecYStates[15].outPktsSecYTooLong);
	CLI_INFO("outPktsSecYNoSAError         : %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x  %08x\n", \
		txSecYStates[0].outPktsSecYNoSAError, txSecYStates[1].outPktsSecYNoSAError, txSecYStates[2].outPktsSecYNoSAError, txSecYStates[3].outPktsSecYNoSAError, \
		txSecYStates[4].outPktsSecYNoSAError, txSecYStates[5].outPktsSecYNoSAError, txSecYStates[6].outPktsSecYNoSAError, txSecYStates[7].outPktsSecYNoSAError, \
		txSecYStates[8].outPktsSecYNoSAError, txSecYStates[9].outPktsSecYNoSAError, txSecYStates[10].outPktsSecYNoSAError, txSecYStates[11].outPktsSecYNoSAError, \
		txSecYStates[12].outPktsSecYNoSAError, txSecYStates[13].outPktsSecYNoSAError, txSecYStates[14].outPktsSecYNoSAError, txSecYStates[15].outPktsSecYNoSAError);
	return 0;
}

static int displayTxPNThreshold(MSD_U32 pNThr)
{
	char* cmd = "macsec";
	CLI_INFO("TxPNThreshold     : %08x\n", pNThr);

	return 0;
}
static int displayTxXPNThreshold(MSD_U64 pNThr)
{
    char* cmd = "macsec";
    CLI_INFO("TxXPNThreshold     : %016llx\n", pNThr);

    return 0;
}
static int displayRxDefaultSCI(MSD_U64 defaultSCI)
{
    char* cmd = "macsec";
    CLI_INFO("defaultSCI     : %016llx\n", defaultSCI);

    return 0;
}
static int displayTxSCIntCtrl(MSD_MACSEC_INT_CONTROL_TXSC* intCtrl)
{
	char* cmd = "macsec";
	CLI_INFO("txAutoRekeyIntEn     : %01x\n", intCtrl->txAutoRekeyIntEn);
	CLI_INFO("txPNThrIntEn     : %01x\n", intCtrl->txPNThrIntEn);
	CLI_INFO("txSAInvalidIntEn     : %01x\n", intCtrl->txSAInvalidIntEn);

	return 0;
}
static int displayEnableEgressGet(MSD_BOOL isEnable)
{
    char* cmd = "macsec";
    CLI_INFO("txisEnable         : %01x\n", isEnable);
    return 0;
}
static int displayEnableIngressGet(MSD_BOOL isEnable)
{
	char* cmd = "macsec";
	CLI_INFO("rxisEnable         : %01x\n", isEnable);
	return 0;
}
static int displayNextInterruptStatus1(MSD_U8 nextIndex, MSD_MACSEC_INT_TXSC* interrupt)
{
    char* cmd = "macsec";
    CLI_INFO("nextIndex     : %02x\n", nextIndex);
	CLI_INFO("txAutoRekeyInt     : %02x\n", interrupt->txAutoRekeyInt);
	CLI_INFO("txPNThrInt     : %02x\n", interrupt->txPNThrInt);
	CLI_INFO("txSAInvalidInt     : %02x\n", interrupt->txSAInvalidInt);

    return 0;
}

static int parseTxSecY(MSD_SECY_EGRESS* secY, int startIndex)
{
	int index;
	char* cmd, * value;
    
	for (index = startIndex; index < (nargs - 1);)
	{
        cmd = CMD_ARGS[index];
        index++;
		if (*cmd != '-')
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
		else
		{
			value = CMD_ARGS[index];
			index++;
		}
		if (strIcmp(cmd, "-txSecYCounterIndex") == 0)
		{
			secY->secYCounterIndex = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-txCipher") == 0)
		{
			secY->cipher = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-txConfOffset") == 0)
		{
			secY->txConfOffset = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-protectFrame") == 0)
		{
			secY->protectFrame = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-txSecYEn") == 0)
		{
			secY->txSecYEn = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-afterSkipTag") == 0)
		{
			secY->afterSkipTag = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-afterRTag") == 0)
		{
			secY->afterRTag = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-afterQTag") == 0)
		{
			secY->afterQTag = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-afterADTag") == 0)
		{
			secY->afterADTag = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-secTagTCI") == 0)
		{
			secY->secTagTCI = (MSD_U32)strtoul(value, NULL, 0);
		}
		else
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
	}
	return 0;
}
static int parseRxSecY(MSD_SECY_INGRESS* secY, int startIndex)
{
	int index;
	char* cmd, * value;

	for (index = startIndex; index < (nargs - 1);)
	{
		cmd = CMD_ARGS[index];
		index++;
		if (*cmd != '-')
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
		else
		{
			value = CMD_ARGS[index];
			index++;
		}
		if (strIcmp(cmd, "-rxSecYCounterIndex") == 0)
		{
			secY->secYCounterIndex = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-allowZeroRxPN") == 0)
		{
			secY->allowZeroRxPN = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-rxCipher") == 0)
		{
			secY->cipher = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-rxConfOffset") == 0)
		{
			secY->rxConfOffset = (MSD_U32)strtoul(value, NULL, 0);

		}
		else if (strIcmp(cmd, "-replayProtect") == 0)
		{
			secY->replayProtect = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-validateFrame") == 0)
		{
			secY->validateFrame = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-rxSecYEn") == 0)
		{
			secY->rxSecYEn = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-replayWindow") == 0)
		{
			secY->replayWindow = (MSD_U32)strtoul(value, NULL, 0);
		}
		else
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
	}
	return 0;
}
static int parseRxSC(MSD_MSEC_RX_SC* rxSC, int startIndex)
{
    int index;
    char* cmd, * value;
    for (index = startIndex; index < (nargs - 1);)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
		if (strIcmp(cmd, "-scCounterIndex") == 0)
        {
            rxSC->scCounterIndex = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-secYIndex") == 0)
        {
            rxSC->secYIndex = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sci") == 0)
        {
            rxSC->sci = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-saIndex") == 0)
        {
            rxSC->rxSA->saIndex = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-an") == 0)
        {
            rxSC->rxSA->an = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-valid") == 0)
        {
            rxSC->rxSA->valid = (MSD_BOOL)strtoul(value, NULL, 0);
        }

        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int parseRule(MSD_MSEC_RULE* rule, int startIndex)
{
    int index;
    char* cmd, * value;
    long long tmpMACAddr = 0;
    for (index = startIndex; index < (nargs - 1);)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        if (strIcmp(cmd, "-keySPV") == 0)
        {
            rule->keySPV = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskSPV") == 0)
        {
            rule->maskSPV = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keySecTagC") == 0)
        {
            rule->keySecTagC = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskSecTagC") == 0)
        {
            rule->maskSecTagC = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keySecTagE") == 0)
        {
            rule->keySecTagE = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskSecTagE") == 0)
        {
            rule->maskSecTagE = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keySecTag") == 0)
        {
            rule->keySecTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskSecTag") == 0)
        {
            rule->maskSecTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keySkipSecTag") == 0)
        {
            rule->keySkipSecTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskSkipSecTag") == 0)
        {
            rule->maskSkipSecTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyRTag") == 0)
        {
            rule->keyRTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskRTag") == 0)
        {
            rule->maskRTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyQTag") == 0)
        {
            rule->keyQTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskQTag") == 0)
        {
            rule->maskQTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyADTag") == 0)
        {
            rule->keyADTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskADTag") == 0)
        {
            rule->maskADTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyDirection") == 0)
        {
            rule->keyDirection = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskDirection") == 0)
        {
            rule->maskDirection = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyDirection") == 0)
        {
            rule->keyRTag = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyMAC_DA[6]") == 0)
        {
            tmpMACAddr = (MSD_U64)strtoul(value, NULL, 0);
            rule->keyMAC_DA[0] = (tmpMACAddr >> 40) & 0xFF;
            rule->keyMAC_DA[1] = (tmpMACAddr >> 32) & 0xFF;
            rule->keyMAC_DA[2] = (tmpMACAddr >> 24) & 0xFF;
            rule->keyMAC_DA[3] = (tmpMACAddr >> 16) & 0xFF;
            rule->keyMAC_DA[4] = (tmpMACAddr >> 8) & 0xFF;
            rule->keyMAC_DA[5] = tmpMACAddr & 0xFF;
        }
        else if (strIcmp(cmd, "-maskMAC_DA[6]") == 0)
        {
            tmpMACAddr = (MSD_U64)strtoul(value, NULL, 0);
            rule->maskMAC_DA[0] = (tmpMACAddr >> 40) & 0xFF;
            rule->maskMAC_DA[1] = (tmpMACAddr >> 32) & 0xFF;
            rule->maskMAC_DA[2] = (tmpMACAddr >> 24) & 0xFF;
            rule->maskMAC_DA[3] = (tmpMACAddr >> 16) & 0xFF;
            rule->maskMAC_DA[4] = (tmpMACAddr >> 8) & 0xFF;
            rule->maskMAC_DA[5] = tmpMACAddr & 0xFF;
        }
        else if (strIcmp(cmd, "-keyMAC_SA[6]") == 0)
        {
            tmpMACAddr = (MSD_U64)strtoul(value, NULL, 0);
            rule->keyMAC_SA[0] = (tmpMACAddr >> 40) & 0xFF;
            rule->keyMAC_SA[1] = (tmpMACAddr >> 32) & 0xFF;
            rule->keyMAC_SA[2] = (tmpMACAddr >> 24) & 0xFF;
            rule->keyMAC_SA[3] = (tmpMACAddr >> 16) & 0xFF;
            rule->keyMAC_SA[4] = (tmpMACAddr >> 8) & 0xFF;
            rule->keyMAC_SA[5] = tmpMACAddr & 0xFF;
        }
        else if (strIcmp(cmd, "-maskMAC_SA[6]") == 0)
        {
            tmpMACAddr = (MSD_U64)strtoul(value, NULL, 0);
            rule->maskMAC_SA[0] = (tmpMACAddr >> 40) & 0xFF;
            rule->maskMAC_SA[1] = (tmpMACAddr >> 32) & 0xFF;
            rule->maskMAC_SA[2] = (tmpMACAddr >> 24) & 0xFF;
            rule->maskMAC_SA[3] = (tmpMACAddr >> 16) & 0xFF;
            rule->maskMAC_SA[4] = (tmpMACAddr >> 8) & 0xFF;
            rule->maskMAC_SA[5] = tmpMACAddr & 0xFF;
        }
        else if (strIcmp(cmd, "-keyAdPCP") == 0)
        {
            rule->keyAdPCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskAdPCP") == 0)
        {
            rule->maskAdPCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyAdVID") == 0)
        {
        rule->keyAdVID = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskAdVID") == 0)
        {
        rule->maskAdVID = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyQPCP") == 0)
        {
        rule->keyQPCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskQPCP") == 0)
        {
        rule->maskQPCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyQVID") == 0)
        {
        rule->keyQVID = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskQVID") == 0)
        {
        rule->maskQVID = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-keyLengthType") == 0)
        {
        rule->keyLengthType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-maskLengthType") == 0)
        {
        rule->maskLengthType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-TxSCIndex") == 0)
        {
        rule->action.TxSCIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-isControlledPort") == 0)
        {
        rule->action.isControlledPort = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-secYIndex") == 0)
        {
        rule->action.SecYIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int parseRxSA(MSD_MSEC_RX_SA* rxSA, int startIndex)
{
    int index;
    char* cmd, * value;
    for (index = startIndex; index < (nargs - 1);)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
		if (strIcmp(cmd, "-nextPN") == 0)
        {
            rxSA->nextPN = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-rxSAValid") == 0)
        {
            rxSA->rxSAValid = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-salt") == 0)
        {
            int i;
            unsigned int tempList[12] = { 0 };
            if (updateArray(&index, value, 12, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 12; i++)
            {
                rxSA->salt[i] = (MSD_U8)(tempList[i] & 0xFF);
            }
        }
		else if (strIcmp(cmd, "-sak") == 0)
		{
			int i;
			unsigned int tempList[32] = { 0 };
			if (updateArray(&index, value, 32, tempList) != 0)
			{
				return -1;
			}
			for (i = 0; i < 32; i++)
			{
				rxSA->sak[i] = (MSD_U8)(tempList[i] & 0xFF);
			}
		}
        else if (strIcmp(cmd, "-ssci") == 0)
        {
            rxSA->ssci = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int parseTxSC(MSD_MSEC_TX_SC* txSC, int startIndex)
{
    int index;
    char* cmd, * value;
    for (index = startIndex; index < (nargs - 1);)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
        if (strIcmp(cmd, "-enable_auto_rekey") == 0)
        {
            txSC->enable_auto_rekey = (MSD_U32)strtoul(value, NULL, 0);
        }
		else if (strIcmp(cmd, "-txNextPNRolloEn") == 0)
		{
			txSC->txNextPNRolloEn = (MSD_U32)strtoul(value, NULL, 0);
		}
        else if (strIcmp(cmd, "-isActiveSA1") == 0)
        {
            txSC->isActiveSA1 = (MSD_U32)strtoul(value, NULL, 0);
        }
		else if (strIcmp(cmd, "-sa0An") == 0)
		{
			txSC->sa0An = (MSD_U32)strtoul(value, NULL, 0);
		}
        else if (strIcmp(cmd, "-sa0Index") == 0)
        {
            txSC->sa0Index = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sA0valid") == 0)
        {
            txSC->sA0Valid = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-scCounterIndex") == 0)
        {
            txSC->scCounterIndex = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sci") == 0)
        {
            txSC->sci = (MSD_U32)strtoul(value, NULL, 0);
        }
		else if (strIcmp(cmd, "-sa1An") == 0)
		{
			txSC->sa1An = (MSD_U32)strtoul(value, NULL, 0);
		}
        else if (strIcmp(cmd, "-sa1Index") == 0)
        {
            txSC->sa1Index = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sA1valid") == 0)
        {
            txSC->sA1Valid = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int parseTxSA(MSD_MSEC_TX_SA* txSA, int startIndex)
{
    int index;
    char* cmd, * value;
    for (index = startIndex; index < (nargs - 1);)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }
		if (strIcmp(cmd, "-nextPN") == 0)
        {
            txSA->nextPN = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-salt") == 0)
        {
            int i;
            unsigned int tempList[12] = { 0 };
            if (updateArray(&index, value, 12, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 12; i++)
            {
                txSA->salt[i] = (MSD_U8)(tempList[i] & 0xFF);
            }
        }
		else if (strIcmp(cmd, "-sak") == 0)
		{
			int i;
			unsigned int tempList[32] = { 0 };
			if (updateArray(&index, value, 32, tempList) != 0)
			{
				return -1;
			}
			for (i = 0; i < 32; i++)
			{
				txSA->sak[i] = (MSD_U8)(tempList[i] & 0xFF);
			}
		}
        else if (strIcmp(cmd, "-ssci") == 0)
        {
            txSA->ssci = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-txSAValid") == 0)
        {
            txSA->txSAValid = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int parseTxSCIntCtrl(MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl, int startIndex)
{
	int index;
	char* cmd, * value;
	for (index = startIndex; index < (nargs - 1);)
	{
		cmd = CMD_ARGS[index];
		index++;
		if (*cmd != '-')
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
		else
		{
			value = CMD_ARGS[index];
			index++;
		}
		if (strIcmp(cmd, "-txAutoRekeyIntEn") == 0)
		{
			txscIntCtrl->txAutoRekeyIntEn = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-txSAInvalidIntEn") == 0)
		{
			txscIntCtrl->txSAInvalidIntEn = (MSD_U32)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-txPNThrIntEn") == 0)
		{
			txscIntCtrl->txPNThrIntEn= (MSD_U32)strtoul(value, NULL, 0);
		}
		else
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
	}
	return 0;
}

static int updateCustomStruct(MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-isValid") == 0)
        {
            customSetup->isValid = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ebsLimit") == 0)
        {
            customSetup->ebsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-cbsLimit") == 0)
        {
            customSetup->cbsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktIncrement") == 0)
        {
            customSetup->bktIncrement = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorGrn") == 0)
        {
            customSetup->bktRateFactorGrn = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorYel") == 0)
        {
            customSetup->bktRateFactorYel = (MSD_U32)strtoul(value, NULL, 0);
        }
        //else if (strIcmp(cmd, "-Delta") == 0)
        //{
        //	customSetup->Delta = (MSD_DOUBLE)strtod(value, NULL);
        //}
        else if (strIcmp(cmd, "-IRLConstant") == 0)
        {
            customSetup->IRLConstant = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-countMode") == 0)
        {
            customSetup->countMode = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int  updatePkttypeOrPri(MSD_PIRL_PKTTYPE_OR_PRI *pktTypeOrPri, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-bktTypeMask") == 0)
        {
            pktTypeOrPri->bktTypeMask = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamFlows") == 0)
        {
            pktTypeOrPri->tcamFlows = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priAndPt") == 0)
        {
            pktTypeOrPri->priAndPt = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-useFPri") == 0)
        {
            pktTypeOrPri->useFPri = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priSelect") == 0)
        {
            pktTypeOrPri->priSelect = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutCustomSetup(MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup)
{
    char *cmd = "irl";
    CLI_INFO("Custom Setup info:\n");
    if (checkValidItem(cmd, "writeRes", "-isValid"))
        CLI_INFO("		isValid:            %X\n", customSetup->isValid);
    if (checkValidItem(cmd, "writeRes", "-ebsLimit"))
        CLI_INFO("		ebsLimit:           %lX\n", customSetup->ebsLimit);
    if (checkValidItem(cmd, "writeRes", "-cbsLimit"))
        CLI_INFO("		cbsLimit:           %lX\n", customSetup->cbsLimit);
    if (checkValidItem(cmd, "writeRes", "-bktIncrement"))
        CLI_INFO("		bktIncrement:       %lX\n", customSetup->bktIncrement);
    if (checkValidItem(cmd, "writeRes", "-bktRateFactorGrn"))
        CLI_INFO("		bktRateFactorGrn:   %lX\n", customSetup->bktRateFactorGrn);
    if (checkValidItem(cmd, "writeRes", "-bktRateFactorYel"))
        CLI_INFO("		bktRateFactorYel:   %lX\n", customSetup->bktRateFactorYel);
    if (checkValidItem(cmd, "writeRes", "-IRLConstant"))
        CLI_INFO("		IRLConstant:        %lX\n", customSetup->IRLConstant);
    if (checkValidItem(cmd, "writeRes", "-countMode"))
        CLI_INFO("		countMode:          %X\n", customSetup->countMode);
}
static int updateIrlRes(MSD_PIRL_DATA *pirlData, int startIndex)
{
    int index;
    char* cmd, *value;
    MSD_PIRL_CUSTOM_RATE_LIMIT *customSetup = &(pirlData->customSetup);
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-bktTypeMask") == 0)
        {
            pirlData->bktTypeMask = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamFlows") == 0)
        {
            pirlData->tcamFlows = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priAndPt") == 0)
        {
            pirlData->priAndPt = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-useFPri") == 0)
        {
            pirlData->useFPri = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-priSelect") == 0)
        {
            pirlData->priSelect = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorAware") == 0)
        {
            pirlData->colorAware = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-accountGrnOverflow") == 0)
        {
            pirlData->accountGrnOverflow = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-accountQConf") == 0)
        {
            pirlData->accountQConf = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-accountFiltered") == 0)
        {
            pirlData->accountFiltered = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-samplingMode") == 0)
        {
            pirlData->samplingMode = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-actionMode") == 0)
        {
            pirlData->actionMode = (MSD_PIRL_ACTION)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fcMode") == 0)
        {
            pirlData->fcMode = (MSD_PIRL_FC_DEASSERT)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fcPriority") == 0)
        {
            pirlData->fcPriority = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-isValid") == 0)
        {
            customSetup->isValid = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ebsLimit") == 0)
        {
            customSetup->ebsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-cbsLimit") == 0)
        {
            customSetup->cbsLimit = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktIncrement") == 0)
        {
            customSetup->bktIncrement = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorGrn") == 0)
        {
            customSetup->bktRateFactorGrn = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bktRateFactorYel") == 0)
        {
            customSetup->bktRateFactorYel = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IRLConstant") == 0)
        {
            customSetup->IRLConstant = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-countMode") == 0)
        {
            customSetup->countMode = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutIrlRes(MSD_PIRL_DATA *pirlData)
{
    char *cmd = "irl";
    CLI_INFO("IRL Resource info:\n");
    if (checkValidItem(cmd, "writeRes", "-bktTypeMask"))
        CLI_INFO("	bktTypeMask:        %X\n", pirlData->bktTypeMask);
    if (checkValidItem(cmd, "writeRes", "-tcamFlows"))
        CLI_INFO("	tcamFlows:          %X\n", pirlData->tcamFlows);
    if (checkValidItem(cmd, "writeRes", "-priAndPt"))
        CLI_INFO("	priAndPt:           %X\n", pirlData->priAndPt);
    if (checkValidItem(cmd, "writeRes", "-useFPri"))
        CLI_INFO("	useFPri:            %X\n", pirlData->useFPri);
    if (checkValidItem(cmd, "writeRes", "-priSelect"))
        CLI_INFO("	priSelect:          %X\n", pirlData->priSelect);
    if (checkValidItem(cmd, "writeRes", "-colorAware"))
        CLI_INFO("	colorAware:         %X\n", pirlData->colorAware);
    if (checkValidItem(cmd, "writeRes", "-accountGrnOverflow"))
        CLI_INFO("	accountGrnOverflow: %X\n", pirlData->accountGrnOverflow);
    if (checkValidItem(cmd, "writeRes", "-accountQConf"))
        CLI_INFO("	accountQConf:       %X\n", pirlData->accountQConf);
    if (checkValidItem(cmd, "writeRes", "-accountFiltered"))
        CLI_INFO("	accountFiltered:    %X\n", pirlData->accountFiltered);
    if (checkValidItem(cmd, "writeRes", "-samplingMode"))
        CLI_INFO("	samplingMode:       %X\n", pirlData->samplingMode);
    if (checkValidItem(cmd, "writeRes", "-actionMode"))
        CLI_INFO("	actionMode:         %X\n", pirlData->actionMode);
    if (checkValidItem(cmd, "writeRes", "-fcMode"))
        CLI_INFO("	fcMode:             %X\n", pirlData->fcMode);
    if (checkValidItem(cmd, "writeRes", "-fcPriority"))
        CLI_INFO("	fcPriority:         %X\n", pirlData->fcPriority);
    printOutCustomSetup(&(pirlData->customSetup));
}
int irlInit(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdIRLReset(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Initialize IRL ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int irlInitRes(void)
{
    MSD_STATUS retVal;
    MSD_U32	irlPort;
    MSD_U32  irlRes;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdIRLInitResourceCounter(sohoDevNum, irlPort, irlRes);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Init IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
            "\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int irlCustomSetup(void)
{
    MSD_STATUS retVal;
    MSD_U32	tgtRate;
    MSD_U32  tgtBstSize;
    MSD_PIRL_CUSTOM_RATE_LIMIT  customSetup;
    MSD_PIRL_COUNT_MODE  countMode;

    if (nargs < 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tgtRate = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    tgtBstSize = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    countMode = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    memset(&customSetup, 0, sizeof(MSD_PIRL_CUSTOM_RATE_LIMIT));
    if (updateCustomStruct(&customSetup, 5) != 0)
        return -1;

    retVal = msdIRLCustomSetup(sohoDevNum, tgtRate, tgtBstSize, countMode, &customSetup);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error custom setup tgtRate=%d tgtBstSize=%d ret[%d: %s]"
            "\n", (int)tgtRate, (int)tgtBstSize, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutCustomSetup(&customSetup);

    return 0;

}
int irlReadRes(void)
{
    MSD_STATUS retVal;
    MSD_U32	irlPort;
    MSD_U32  irlRes;
    MSD_PIRL_DATA    pirlData;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    memset(&pirlData, 0, sizeof(MSD_PIRL_DATA));

    retVal = msdIRLResourceRead(sohoDevNum, irlPort, irlRes, &pirlData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
            "\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutIrlRes(&pirlData);

    return 0;
}
int irlWriteRes(void)
{
    MSD_STATUS retVal;
    MSD_U32	irlPort;
    MSD_U32  irlRes;
    MSD_PIRL_DATA    pirlData;

    if (nargs < 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    memset(&pirlData, 0, sizeof(MSD_PIRL_DATA));
    if (updateIrlRes(&pirlData, 4) != 0)
        return -1;

    retVal = msdIRLResourceWrite(sohoDevNum, irlPort, irlRes, &pirlData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Write IRL resource irlPort=%d irlRes=%d ret[%d: %s]"
            "\n", (int)irlPort, (int)irlRes, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int irlRReg(void)
{
    MSD_STATUS retVal;
    MSD_LPORT	irlPort;
    MSD_U32  irlRes;
    MSD_U32  regOffset;
    MSD_U16 data;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    regOffset = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdIRLResRegGet(sohoDevNum, irlPort, irlRes, regOffset, &data);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read IRL register irlPort=%d irlRes=%d offset=%d ret[%d: %s]"
            "\n", (int)irlPort, (int)irlRes, (int)regOffset, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Read IRL Reg - port: %d, res: %d, offset: %d, data = %X \n", (int)irlPort, (int)irlRes, (int)regOffset, data);
    return 0;
}
int irlWReg(void)
{
    MSD_STATUS retVal;
    MSD_LPORT	irlPort;
    MSD_U32  irlRes;
    MSD_U32  regOffset;
    MSD_U16  data;

    if (nargs != 6)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    regOffset = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    data = (MSD_U16)strtoul(CMD_ARGS[5], NULL, 0);

    retVal = msdIRLResRegSet(sohoDevNum, irlPort, irlRes, regOffset, data);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read IRL register irlPort=%d irlRes=%d offset=%d ret[%d: %s]"
            "\n", (int)irlPort, (int)irlRes, (int)regOffset, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int irlAdvConfig(void)
{
    MSD_STATUS retVal;
    MSD_LPORT  irlPort;
    MSD_U32  irlRes;
    MSD_U32  tgtRate;
    MSD_U32  tgtBstSize;
    MSD_PIRL_COUNT_MODE  countMode;
    MSD_PIRL_PKTTYPE_OR_PRI pktTypeOrPri;

    if (nargs < 7)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    tgtRate = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);
    tgtBstSize = (MSD_U32)strtoul(CMD_ARGS[5], NULL, 0);
    countMode = (MSD_U32)strtoul(CMD_ARGS[6], NULL, 0);

	memset(&pktTypeOrPri, 0, sizeof(MSD_PIRL_PKTTYPE_OR_PRI));
    if (updatePkttypeOrPri(&pktTypeOrPri, 7) != 0)
        return -1;

    retVal = msdIRLAdvConfig(sohoDevNum, irlPort, irlRes, tgtRate, tgtBstSize, countMode, &pktTypeOrPri);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int irlAdvConfigDisable(void)
{
    MSD_STATUS retVal;
    MSD_LPORT  irlPort;
    MSD_U32  irlRes;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    irlPort = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    irlRes = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdIRLAdvConfigDisable(sohoDevNum, irlPort, irlRes);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}

static int updateVctMode(MSD_ADV_VCT_MODE *mode, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-mode") == 0)
        {
            mode->mode = (MSD_ADV_VCT_MOD)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-transChanSel") == 0)
        {
            mode->transChanSel = (MSD_ADV_VCT_TRANS_CHAN_SEL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-sampleAvg") == 0)
        {
            mode->sampleAvg = (MSD_ADV_VCT_SAMPLE_AVG)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-peakDetHyst") == 0)
        {
            mode->peakDetHyst = (MSD_ADV_VCT_PEAKDET_HYST)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-samplePointDist") == 0)
        {
            mode->samplePointDist = (MSD_16)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void displayAdvVCTResult(MSD_ADV_CABLE_STATUS *cableStatus, int channel, MSD_ADV_VCT_MODE mode)
{
    int i, j = channel;

    if (mode.mode == MSD_ADV_VCT_OFFSET)
    {
        if (cableStatus->u[j].offset == 0)
            CLI_INFO("Advanced VCT offset pair %x test failed\n", j);
        else if (cableStatus->u[j].offset == 0x80)
            CLI_INFO("Advanced VCT offset pair %x is 0 \n", j);
        else
            CLI_INFO("Advanced VCT offset pair %x is %d \n", j, cableStatus->u[j].offset);

        return;
    }
    if (mode.mode == MSD_ADV_VCT_SAMPLE_POINT)
    {
        if (cableStatus->u[j].sampleAmp & 0x80)
            CLI_INFO("Advanced VCT sample point return pair %x is -%d \n", j, cableStatus->u[j].sampleAmp & 0x7f);
        else
            CLI_INFO("Advanced VCT sample point return pair %x is +%d \n", j, cableStatus->u[j].sampleAmp & 0x7f);

        return;
    }

    switch (cableStatus->cableStatus[channel])
    {
    case MSD_ADV_VCT_FAIL:
        CLI_INFO("Advanced Cable Test Failed\n");
        break;
    case MSD_ADV_VCT_NORMAL:
        CLI_INFO("No problem found.\n");
        break;
    case MSD_ADV_VCT_IMP_GREATER_THAN_115:
        CLI_INFO("Impedance is greater than 115 Ohms.\n");
        CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
        break;
    case MSD_ADV_VCT_IMP_LESS_THAN_85:
        CLI_INFO("Impedance is less than 85 Ohms.\n");
        CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
        break;
    case MSD_ADV_VCT_OPEN:
        CLI_INFO("Open Cable.\n");
        CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
        break;
    case MSD_ADV_VCT_SHORT:
        CLI_INFO("Shorted Cable.\n");
        CLI_INFO("Approximately %i meters from the tested port.\n", cableStatus->u[channel].dist2fault);
        break;
    case MSD_ADV_VCT_CROSS_PAIR_SHORT:
        /*CLI_INFO("Cable Test Passed.\n");*/
        for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
        {
            if (cableStatus->u[channel].crossShort.channel[i] == MSD_TRUE)
            {
                CLI_INFO("\tCross pair short in channel %i.\n", i);
                CLI_INFO("\tApproximately %i meters from the tested port.\n",
                    cableStatus->u[channel].crossShort.dist2fault[i]);
            }
        }
        break;
    default:
        CLI_INFO("Unknown Test Result.\n");
        break;
    }
}
int vctCableDiag(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_ADV_VCT_MODE mode;
    MSD_ADV_CABLE_STATUS cableStatus;
	int i;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&mode, 0, sizeof(MSD_ADV_VCT_MODE));
    memset(&cableStatus, 0, sizeof(MSD_ADV_CABLE_STATUS));

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateVctMode(&mode, 3) != 0)
        return -1;

    retVal = msdVctGetAdvCableDiag(sohoDevNum, port, mode, &cableStatus);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Cable Diag port=%d ret[%d: %s]"
            "\n", (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

	if (mode.transChanSel == MSD_ADV_VCT_TCS_NO_CROSSPAIR)
	{
		for (i = 0; i< MSD_MDI_PAIR_NUM; i++)
		{
			CLI_INFO("MDI PAIR %i:\n", i);
			displayAdvVCTResult(&cableStatus, i, mode);
		}
	}
    else if (mode.transChanSel < MSD_ADV_VCT_TCS_CROSSPAIR_0 || mode.transChanSel > MSD_ADV_VCT_TCS_CROSSPAIR_3)
    {
        CLI_ERROR("Invalid transChanSel: %d\n", mode.transChanSel);
    }
    else
    {
        CLI_INFO("MDI PAIR %i:\n", mode.transChanSel - 4);
        displayAdvVCTResult(&cableStatus, mode.transChanSel - 4, mode);
    }

    return 0;
}
int vctExtenedStatus(void)
{
    MSD_STATUS status;
    MSD_LPORT port;
    MSD_ADV_EXTENDED_STATUS extendedStatus;
    int i;
    char ch;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&extendedStatus, 0, sizeof(MSD_ADV_EXTENDED_STATUS));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    /*
    *     Start getting Extended Information.
    */
    if ((status = msdVctGetAdvExtendStatus(sohoDevNum, port, &extendedStatus)) != MSD_OK)
    {
        CLI_INFO("msdVctGetAdvExtendStatus return Failed\n");
        return status;
    }

    if (!extendedStatus.isValid)
    {
        CLI_INFO("Not able to get Extended Status.\n");
        CLI_INFO("Please check if 1000B-T Link is established on Port %i.\n", (int)port);
        return status;
    }

    /* Pair Polarity */
    CLI_INFO("Pair Polarity:\n");
    for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
    {
        CLI_INFO("MDI PAIR %i: %s\n", i,
            (extendedStatus.pairPolarity[i] == MSD_POSITIVE) ? "Positive" : "Negative");
    }

    /* Pair Swap */
    CLI_INFO("Pair Swap:\n");
    for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
    {
        switch (extendedStatus.pairSwap[i])
        {
        case MSD_CHANNEL_A:
            ch = 'A';
            break;
        case MSD_CHANNEL_B:
            ch = 'B';
            break;
        case MSD_CHANNEL_C:
            ch = 'C';
            break;
        case MSD_CHANNEL_D:
            ch = 'D';
            break;
        default:
            CLI_ERROR("Error: reported unknown Pair Swap %i\n", extendedStatus.pairSwap[i]);
            ch = 'U';
            break;
        }

        CLI_INFO("MDI PAIR %i: Channel %c\n", i, ch);
    }

    /* Pair Skew */
    CLI_INFO("Pair Skew:\n");
    for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
    {
        CLI_INFO("MDI PAIR %i: %ins\n", i, (int)extendedStatus.pairSkew[i]);
    }

    /* Cable Len */
    CLI_INFO("Cable Len:\n");
    for (i = 0; i<MSD_MDI_PAIR_NUM; i++)
    {
        CLI_INFO("MDI PAIR %i: approximately %im\n", i, (int)extendedStatus.cableLen[i]);
    }

    return 0;
}

static int updateIntEnable(MSD_PTP_PORT_INTERRUPT_EN *interruptEn, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpArrIntEn") == 0)
        {
            interruptEn->ptpArrIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpDepIntEn") == 0)
        {
            interruptEn->ptpDepIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updatePortConfig(MSD_PTP_PORT_CONFIG* ptpData, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-transSpec") == 0)
        {
            ptpData->transSpec = (MSD_PTP_SPEC)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disTSpec") == 0)
        {
            ptpData->disTSpec = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etJump") == 0)
        {
            ptpData->etJump = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ipJump") == 0)
        {
            ptpData->ipJump = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpArrIntEn") == 0)
        {
            ptpData->ptpArrIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptpDepIntEn") == 0)
        {
            ptpData->ptpDepIntEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disTSOverwrite") == 0)
        {
            ptpData->disTSOverwrite = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-filterAct") == 0)
        {
            ptpData->filterAct = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-hwAccPtp") == 0)
        {
            ptpData->hwAccPtp = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-kpFramwSa") == 0)
        {
            ptpData->kpFramwSa = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ExthwAcc") == 0)
        {
            ptpData->ExthwAcc = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrTSMode") == 0)
        {
            ptpData->arrTSMode = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrLedCtrl") == 0)
        {
            ptpData->arrLedCtrl = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-depLedCtrl") == 0)
        {
            ptpData->depLedCtrl = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpPortConfig(MSD_PTP_PORT_CONFIG *ptpData)
{
    char *cmd = "ptp";
    CLI_INFO("PTP Port Configuration:\n");

    if (checkValidItem(cmd, "setPortConfig", "-transSpec"))
        CLI_INFO("  transSpec:      %d\n", ptpData->transSpec);
    if (checkValidItem(cmd, "setPortConfig", "-disTSpec"))
        CLI_INFO("  disTSpec:       %d\n", ptpData->disTSpec);
    if (checkValidItem(cmd, "setPortConfig", "-etJump"))
        CLI_INFO("  etJump:         0x%x\n", ptpData->etJump);
    if (checkValidItem(cmd, "setPortConfig", "-ipJump"))
        CLI_INFO("  ipJump:         0x%x\n", ptpData->ipJump);
    if (checkValidItem(cmd, "setPortConfig", "-ptpArrIntEn"))
        CLI_INFO("  ptpArrIntEn:    %d\n", ptpData->ptpArrIntEn);
    if (checkValidItem(cmd, "setPortConfig", "-ptpDepIntEn"))
        CLI_INFO("  ptpDepIntEn:    %d\n", ptpData->ptpDepIntEn);
    if (checkValidItem(cmd, "setPortConfig", "-disTSOverwrite"))
        CLI_INFO("  disTSOverwrite: %d\n", ptpData->disTSOverwrite);
    if (checkValidItem(cmd, "setPortConfig", "-filterAct"))
        CLI_INFO("  filterAct:      %d\n", ptpData->filterAct);
    if (checkValidItem(cmd, "setPortConfig", "-hwAccPtp"))
        CLI_INFO("  hwAccPtp:       %d\n", ptpData->hwAccPtp);
    if (checkValidItem(cmd, "setPortConfig", "-kpFramwSa"))
        CLI_INFO("  kpFramwSa:      %d\n", ptpData->kpFramwSa);
    if (checkValidItem(cmd, "setPortConfig", "-ExthwAcc"))
        CLI_INFO("  ExthwAcc:       %d\n", ptpData->ExthwAcc);
    if (checkValidItem(cmd, "setPortConfig", "-arrTSMode"))
        CLI_INFO("  arrTSMode:      0x%x\n", ptpData->arrTSMode);
    if (checkValidItem(cmd, "setPortConfig", "-arrLedCtrl"))
        CLI_INFO("  arrLedCtrl:     0x%x\n", ptpData->arrLedCtrl);
    if (checkValidItem(cmd, "setPortConfig", "-depLedCtrl"))
        CLI_INFO("  depLedCtrl:     0x%x\n", ptpData->depLedCtrl);
}
static void printOutTimeStamp(MSD_PTP_TS_STATUS *ptpStatus)
{
    CLI_INFO("PTP Time Stamp status:\n");
    CLI_INFO("  isValid:        %d\n", ptpStatus->isValid);
    CLI_INFO("  timeStamped:    0x%x\n", ptpStatus->timeStamped);
    CLI_INFO("  ptpSeqId:       0x%x\n", ptpStatus->ptpSeqId);
    CLI_INFO("  status:         %d\n", ptpStatus->status);
}
static int updatePtpGlobalConfig(MSD_PTP_GLOBAL_CONFIG *ptpData, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpEType") == 0)
        {
            ptpData->ptpEType = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-msgIdTSEn") == 0)
        {
            ptpData->msgIdTSEn = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsArrPtr") == 0)
        {
            ptpData->tsArrPtr = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpGlobalConfig(MSD_PTP_GLOBAL_CONFIG *ptpData)
{
    char *cmd = "ptp";
    CLI_INFO("PTP Global Configuration:\n");
    if (checkValidItem(cmd, "setGlobalConfig", "-ptpEType"))
        CLI_INFO("  ptpEType:       0x%x\n", ptpData->ptpEType);
    if (checkValidItem(cmd, "setGlobalConfig", "-msgIdTSEn"))
        CLI_INFO("  msgIdTSEn:      0x%x\n", ptpData->msgIdTSEn);
    if (checkValidItem(cmd, "setGlobalConfig", "-tsArrPtr"))
        CLI_INFO("  tsArrPtr:       0x%x\n", ptpData->tsArrPtr);
}
static void printOutPtpIntStatus(MSD_PTP_INTERRUPT_STATUS *ptpIntSt)
{
    CLI_INFO("PTP Interrupt status:\n");
    CLI_INFO("  ptpTrigGenInt:      0x%x\n", ptpIntSt->ptpTrigGenInt);
    CLI_INFO("  ptpEventInt:        0x%x\n", ptpIntSt->ptpEventInt);
    CLI_INFO("  ptpIntStPortVect:   0x%x\n", ptpIntSt->ptpIntStPortVect);
}
static int updatePtpMode(MSD_PTP_MODE_CONFIG *mode, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-altScheme") == 0)
        {
            mode->altScheme = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-grandMstr") == 0)
        {
            mode->grandMstr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-oneStep") == 0)
        {
            mode->oneStep = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-PTPMode") == 0)
        {
            mode->PTPMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPtpMode(MSD_PTP_MODE_CONFIG *mode)
{
    char *cmd = "ptp";
    CLI_INFO("PTP Mode Configuration:\n");
    if (checkValidItem(cmd, "setPtpMode", "-altScheme"))
        CLI_INFO("  altScheme:       0x%x\n", mode->altScheme);
    if (checkValidItem(cmd, "setPtpMode", "-grandMstr"))
        CLI_INFO("  grandMstr:       0x%x\n", mode->grandMstr);
    if (checkValidItem(cmd, "setPtpMode", "-oneStep"))
        CLI_INFO("  oneStep:         0x%x\n", mode->oneStep);
    if (checkValidItem(cmd, "setPtpMode", "-PTPMode"))
        CLI_INFO("  PTPMode:         0x%x\n", mode->PTPMode);
}
static int updateTimeArray(MSD_PTP_TIME_ARRAY *timeArray, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-todLoadPoint") == 0)
        {
            timeArray->todLoadPoint = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-clkValid") == 0)
        {
            timeArray->clkValid = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-domainNumber") == 0)
        {
            timeArray->domainNumber = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todNanoseconds") == 0)
        {
            timeArray->todNanoseconds = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todSecondsLow") == 0)
        {
            timeArray->todSecondsLow = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todSecondsHigh") == 0)
        {
            timeArray->todSecondsHigh = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Nanoseconds1722Low") == 0)
        {
            timeArray->Nanoseconds1722Low = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Nanoseconds1722High") == 0)
        {
            timeArray->Nanoseconds1722High = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-todCompensation") == 0)
        {
            timeArray->todCompensation = (MSD_U32)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutTimeArray(MSD_U8 timeArrayIndex, MSD_PTP_TIME_ARRAY *timeArray)
{
    char *cmd = "ptp";
    CLI_INFO("PTP timeArray %d Configuration:\n", timeArrayIndex);
    if (checkValidItem(cmd, "storeAll", "-todLoadPoint"))
        CLI_INFO("  todLoadPoint:       0x%x\n", timeArray->todLoadPoint);
    if (checkValidItem(cmd, "storeAll", "-clkValid"))
        CLI_INFO("  clkValid:           0x%x\n", timeArray->clkValid);
    if (checkValidItem(cmd, "storeAll", "-domainNumber"))
        CLI_INFO("  domainNumber:       0x%x\n", timeArray->domainNumber);
    if (checkValidItem(cmd, "storeAll", "-todNanoseconds"))
        CLI_INFO("  todNanoseconds:     0x%x\n", timeArray->todNanoseconds);
    if (checkValidItem(cmd, "storeAll", "-todSecondsLow"))
        CLI_INFO("  todSecondsLow:      0x%x\n", timeArray->todSecondsLow);
    if (checkValidItem(cmd, "storeAll", "-todSecondsHigh"))
        CLI_INFO("  todSecondsHigh:     0x%x\n", timeArray->todSecondsHigh);
    if (checkValidItem(cmd, "storeAll", "-Nanoseconds1722Low"))
        CLI_INFO("  Nanoseconds1722Low: 0x%x\n", timeArray->Nanoseconds1722Low);
    if (checkValidItem(cmd, "storeAll", "-Nanoseconds1722High"))
        CLI_INFO("  Nanoseconds1722High:0x%x\n", timeArray->Nanoseconds1722High);
    if (checkValidItem(cmd, "storeAll", "-todCompensation"))
        CLI_INFO("  todCompensation:    0x%x\n", timeArray->todCompensation);
}
static int updatePtpPulse(MSD_PTP_PULSE_STRUCT *pulsePara, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ptpPulseWidth") == 0)
        {
            pulsePara->ptpPulseWidth = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsWidthRange") == 0)
        {
            pulsePara->ptp1ppsWidthRange = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsPhase") == 0)
        {
            pulsePara->ptp1ppsPhase = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ptp1ppsSelect") == 0)
        {
            pulsePara->ptp1ppsSelect = (MSD_U8)strtoul(value, NULL, 0);
        }
		else if (strIcmp(cmd, "-ptpAltTCAMTimeSel") == 0)
		{
			pulsePara->ptpAltTCAMTimeSel = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ptpTCAMTimeSel") == 0)
		{
			pulsePara->ptpTCAMTimeSel = (MSD_U8)strtoul(value, NULL, 0);
		}
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutPulse(MSD_PTP_PULSE_STRUCT *pulsePara)
{
    char *cmd = "ptp";
    CLI_INFO("PTP Pulse Configuration:\n");
    if (checkValidItem(cmd, "setPtpPulse", "-ptpPulseWidth"))
        CLI_INFO("  ptpPulseWidth:          0x%x\n", pulsePara->ptpPulseWidth);
	if (checkValidItem(cmd, "setPtpPulse", "-ptpAltTCAMTimeSel"))
		CLI_INFO("  ptpAltTCAMTimeSel:      0x%x\n", pulsePara->ptpAltTCAMTimeSel);
    if (checkValidItem(cmd, "setPtpPulse", "-ptp1ppsWidthRange"))
        CLI_INFO("  ptp1ppsWidthRange:      0x%x\n", pulsePara->ptp1ppsWidthRange);
	if (checkValidItem(cmd, "setPtpPulse", "-ptpTCAMTimeSel"))
		CLI_INFO("  ptpTCAMTimeSel:         0x%x\n", pulsePara->ptpTCAMTimeSel);
    if (checkValidItem(cmd, "setPtpPulse", "-ptp1ppsPhase"))
        CLI_INFO("  ptp1ppsPhase:           0x%x\n", pulsePara->ptp1ppsPhase);
    if (checkValidItem(cmd, "setPtpPulse", "-ptp1ppsSelect"))
        CLI_INFO("  ptp1ppsSelect:          0x%x\n", pulsePara->ptp1ppsSelect);
}
int ptpSetIntEn(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_INTERRUPT_EN interruptEn;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&interruptEn, 0, sizeof(MSD_PTP_PORT_INTERRUPT_EN));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateIntEnable(&interruptEn, 3) != 0)
        return -1;

    retVal = msdPtpIntEnableSet(sohoDevNum, port, &interruptEn);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetIntEn(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_INTERRUPT_EN interruptEn;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&interruptEn, 0, sizeof(MSD_PTP_PORT_INTERRUPT_EN));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPtpIntEnableGet(sohoDevNum, port, &interruptEn);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("PTP interrupt info:\n");
    CLI_INFO("  Arrival interrupt enable status: %d\n", interruptEn.ptpArrIntEn);
    CLI_INFO("  Departure interrupt enable status: %d\n", interruptEn.ptpDepIntEn);

    return retVal;
}
int ptpSetPortConfig(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_CONFIG ptpData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&ptpData, 0, sizeof(MSD_PTP_PORT_CONFIG));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    if (updatePortConfig(&ptpData, 3) != 0)
        return -1;

    retVal = msdPtpPortConfigSet(sohoDevNum, port, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPortConfig(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_PORT_CONFIG ptpData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&ptpData, 0, sizeof(MSD_PTP_PORT_CONFIG));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPtpPortConfigGet(sohoDevNum, port, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpPortConfig(&ptpData);
    return retVal;
}
int ptpGetTimeStamp(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_PTP_TIME timeToRead;
    MSD_PTP_TS_STATUS ptpStatus;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&ptpStatus, 0, sizeof(MSD_PTP_TS_STATUS));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    timeToRead = (MSD_PTP_TIME)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdPtpTimeStampGet(sohoDevNum, port, timeToRead, &ptpStatus);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutTimeStamp(&ptpStatus);
    return retVal;
}
int ptpSetGlobalConfig(void)
{
    MSD_STATUS retVal;
    MSD_PTP_GLOBAL_CONFIG  ptpData;

    memset(&ptpData, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));
    if (updatePtpGlobalConfig(&ptpData, 2) != 0)
	{
        return -1;
	}

    retVal = msdPtpGlobalConfigSet(sohoDevNum, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetGlobalConfig(void)
{
    MSD_STATUS retVal;
    MSD_PTP_GLOBAL_CONFIG  ptpData;

    memset(&ptpData, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));

    retVal = msdPtpGlobalConfigGet(sohoDevNum, &ptpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpGlobalConfig(&ptpData);
    return retVal;
}
int ptpGetIntStatus(void)
{
    MSD_STATUS retVal;
    MSD_PTP_INTERRUPT_STATUS  ptpIntSt;

    memset(&ptpIntSt, 0, sizeof(MSD_PTP_INTERRUPT_STATUS));

    retVal = msdPtpIntStatusGet(sohoDevNum, &ptpIntSt);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpIntStatus(&ptpIntSt);
    return retVal;
}
int ptpSetPtpMode(void)
{
    MSD_STATUS retVal;
    MSD_PTP_MODE_CONFIG mode;

    memset(&mode, 0, sizeof(MSD_PTP_MODE_CONFIG));
    if (updatePtpMode(&mode, 2) != 0)
        return -1;

    retVal = msdPtpModeSet(sohoDevNum, &mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPtpMode(void)
{
    MSD_STATUS retVal;
    MSD_PTP_MODE_CONFIG mode;

    memset(&mode, 0, sizeof(MSD_PTP_MODE_CONFIG));
    retVal = msdPtpModeGet(sohoDevNum, &mode);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPtpMode(&mode);
    return retVal;
}
int ptpStoreCompensation(void)
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));
	if (updateTimeArray(&timeArray, 3) != 0)
	{
		return -1;
	}

    retVal = msdPtpTODStoreCompensation(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
		if (retVal == MSD_BUSY)
		{
			CLI_WARNING("TOD busy bit not cleared, please wait... \n");
			return MSD_BUSY;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpStoreAll(void)
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));
	if (updateTimeArray(&timeArray, 3) != 0)
	{
		return -1;
	}

    retVal = msdPtpTODStoreAll(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
		if (retVal == MSD_BUSY)
		{
			CLI_WARNING("TOD busy bit not cleared, please wait... \n");
			return MSD_BUSY;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpCaptureAll(void)
{
    MSD_STATUS retVal;
    MSD_U8 timeArrayIndex;
    MSD_PTP_TIME_ARRAY timeArray;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    timeArrayIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));

    retVal = msdPtpTODCaptureAll(sohoDevNum, timeArrayIndex, &timeArray);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
		if (retVal == MSD_BUSY)
		{
			CLI_WARNING("TOD busy bit not cleared, please wait... \n");
			return MSD_BUSY;
		}
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutTimeArray(timeArrayIndex, &timeArray);
    return retVal;
}
int ptpSetPtpPulse(void)
{
    MSD_STATUS retVal;
    MSD_PTP_PULSE_STRUCT pulsePara;

    memset(&pulsePara, 0, sizeof(MSD_PTP_PULSE_STRUCT));
    if (updatePtpPulse(&pulsePara, 2) != 0)
	{
        return -1;
	}

    retVal = msdPtpPulseSet(sohoDevNum, &pulsePara);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int ptpGetPtpPulse(void)
{
    MSD_STATUS retVal;
    MSD_PTP_PULSE_STRUCT pulsePara;

    memset(&pulsePara, 0, sizeof(MSD_PTP_PULSE_STRUCT));

    retVal = msdPtpPulseGet(sohoDevNum, &pulsePara);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPulse(&pulsePara);
    return retVal;
}

static int updateIEEEPriorityTableStruct(MSD_IEEE_PRIORITY_TABLE_STRUCT *ieeePriorityTableStruct, int startIndex)
{
    int index;
    char *cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-ieeeYellow") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeYellow = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disIeeeQpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.disIeeeQpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ieeeQpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeQpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disIeeeFpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.disIeeeFpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ieeeFpri") == 0)
        {
            ieeePriorityTableStruct->ingTbl.ieeeFpri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disVIDMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.disVIDMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-disPCPMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.disPCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-framePCPAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTblPcp.framePCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingGreen") == 0)
        {
            ieeePriorityTableStruct->egGreeTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPGreen") == 0)
        {
			ieeePriorityTableStruct->egGreeTblDscp.frameDSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingYel") == 0)
        {
            ieeePriorityTableStruct->egYelTblDscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPYel") == 0)
        {
			ieeePriorityTableStruct->egYelTblDscp.frameDSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-enDSCPMarkingAVB") == 0)
        {
            ieeePriorityTableStruct->egAvbTbldscp.enDSCPMarking = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameDSCPAVB") == 0)
        {
			ieeePriorityTableStruct->egAvbTbldscp.frameDSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutIEEEPriorityTableStruct(MSD_IEEE_PRIORITY_TABLE_SET table, MSD_IEEE_PRIORITY_TABLE_STRUCT *ieeePriorityTableStruct)
{
    char *cmd = "qos";
    CLI_INFO("IEEE Priority table struct for ");
    if (table == MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI)
    {
        CLI_INFO("IingTbl: \n");
        if (checkValidItem(cmd, "setPrioMap", "-ieeeYellow"))
            CLI_INFO("                  IeeeYellow:     %x\n", ieeePriorityTableStruct->ingTbl.ieeeYellow);
        if (checkValidItem(cmd, "setPrioMap", "-disIeeeQpri"))
            CLI_INFO("                  DisIeeeQpri:    %x\n", ieeePriorityTableStruct->ingTbl.disIeeeQpri);
        if (checkValidItem(cmd, "setPrioMap", "-ieeeQpri"))
            CLI_INFO("                  IeeeQpri:       %x\n", ieeePriorityTableStruct->ingTbl.ieeeQpri);
        if (checkValidItem(cmd, "setPrioMap", "-disIeeeFpri"))
            CLI_INFO("                  DisIeeeFpri:    %x\n", ieeePriorityTableStruct->ingTbl.disIeeeFpri);
        if (checkValidItem(cmd, "setPrioMap", "-ieeeFpri"))
            CLI_INFO("                  IeeeFpri:       %x\n", ieeePriorityTableStruct->ingTbl.ieeeFpri);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_PCP)
    {
        CLI_INFO("EgGreeTblPcp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-disVIDMarking"))
            CLI_INFO("                  DisVIDMarking:  %x\n", ieeePriorityTableStruct->egGreeTblPcp.disVIDMarking);
        if (checkValidItem(cmd, "setPrioMap", "-disPCPMarking"))
            CLI_INFO("                  DisPCPMarking:  %x\n", ieeePriorityTableStruct->egGreeTblPcp.disPCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-framePCP"))
            CLI_INFO("                  FramePCP:       %x\n", ieeePriorityTableStruct->egGreeTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_PCP)
    {

        CLI_INFO("EgYelTblPcp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-disVIDMarking"))
            CLI_INFO("                  DisVIDMarking:  %x\n", ieeePriorityTableStruct->egYelTblPcp.disVIDMarking);
        if (checkValidItem(cmd, "setPrioMap", "-disPCPMarking"))
            CLI_INFO("                  DisPCPMarking:  %x\n", ieeePriorityTableStruct->egYelTblPcp.disPCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-framePCP"))
            CLI_INFO("                  FramePCP:       %x\n", ieeePriorityTableStruct->egYelTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_PCP)
    {
        CLI_INFO("EgAvbTblPcp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-disVIDMarking"))
            CLI_INFO("                  DisVIDMarking:  %x\n", ieeePriorityTableStruct->egAvbTblPcp.disVIDMarking);
        if (checkValidItem(cmd, "setPrioMap", "-disPCPMarking"))
            CLI_INFO("                  DisPCPMarking:  %x\n", ieeePriorityTableStruct->egAvbTblPcp.disPCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-framePCP"))
            CLI_INFO("                  FramePCP:       %x\n", ieeePriorityTableStruct->egAvbTblPcp.framePCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_ING_PCP_2_F_Q_PRI_DEI_1)
    {
        CLI_INFO("ingTblDei1: \n");
        if (checkValidItem(cmd, "setPrioMap", "-ieeeYellow"))
            CLI_INFO("                  IeeeYellow:     %x\n", ieeePriorityTableStruct->ingTblDei1.ieeeYellow);
        if (checkValidItem(cmd, "setPrioMap", "-disIeeeQpri"))
            CLI_INFO("                  DisIeeeQpri:    %x\n", ieeePriorityTableStruct->ingTblDei1.disIeeeQpri);
        if (checkValidItem(cmd, "setPrioMap", "-ieeeQpri"))
            CLI_INFO("                  IeeeQpri:       %x\n", ieeePriorityTableStruct->ingTblDei1.ieeeQpri);
        if (checkValidItem(cmd, "setPrioMap", "-disIeeeFpri"))
            CLI_INFO("                  DisIeeeFpri:    %x\n", ieeePriorityTableStruct->ingTblDei1.disIeeeFpri);
        if (checkValidItem(cmd, "setPrioMap", "-ieeeFpri"))
            CLI_INFO("                  IeeeFpri:       %x\n", ieeePriorityTableStruct->ingTblDei1.ieeeFpri);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_GREEN_FPRI_2_DSCP)
    {
        CLI_INFO("EgGreeTblDscp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-enDSCPMarking"))
            CLI_INFO("                  EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egGreeTblDscp.enDSCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-frameDSCP"))
            CLI_INFO("                  FrameDSCP:  %x\n", ieeePriorityTableStruct->egGreeTblDscp.frameDSCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_YEL_FPRI_2_DSCP)
    {
        CLI_INFO("EgYelTblDscp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-enDSCPMarking"))
            CLI_INFO("                  EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egYelTblDscp.enDSCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-frameDSCP"))
            CLI_INFO("                  FrameDSCP:  %x\n", ieeePriorityTableStruct->egYelTblDscp.frameDSCP);
    }
    else if (table == MSD_IEEE_PRIORITY_TABLE_EG_AVB_FPRI_2_DSCP)
    {
        CLI_INFO("EgAvbTbldscp: \n");
        if (checkValidItem(cmd, "setPrioMap", "-enDSCPMarking"))
            CLI_INFO("                  EnDSCPMarking:  %x\n", ieeePriorityTableStruct->egAvbTbldscp.enDSCPMarking);
        if (checkValidItem(cmd, "setPrioMap", "-frameDSCP"))
            CLI_INFO("                  FrameDSCP:  %x\n", ieeePriorityTableStruct->egAvbTbldscp.frameDSCP);
    }
    else
    {
        CLI_INFO("Invalid IEEE table input\n");
    }
}
int qosSetPrioMap(void)
{
    MSD_LPORT port;
    MSD_IEEE_PRIORITY_TABLE_SET table;
    MSD_U8 entryPtr;
    MSD_STATUS retVal;
    int index = 0;
    MSD_IEEE_PRIORITY_TABLE_STRUCT ieeePriorityTableStruct;

    if (nargs < 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&ieeePriorityTableStruct, 0, sizeof(MSD_IEEE_PRIORITY_TABLE_STRUCT));
    if (updateIEEEPriorityTableStruct(&ieeePriorityTableStruct, 5) != 0)
        return -1;
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    table = (MSD_IEEE_PRIORITY_TABLE_SET)strtoul(CMD_ARGS[3], NULL, 0);
    entryPtr = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdQosPortPrioMapSet(sohoDevNum, port, table, entryPtr, &ieeePriorityTableStruct);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set Priority Map ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int qosGetPrioMap(void)
{
    MSD_LPORT port;
    MSD_IEEE_PRIORITY_TABLE_SET table;
    MSD_U8 entryPtr;
    MSD_STATUS retVal;
    int index = 0;
    MSD_IEEE_PRIORITY_TABLE_STRUCT ieeePriorityTableStruct;

    if (nargs != 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&ieeePriorityTableStruct, 0, sizeof(ieeePriorityTableStruct));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    table = (MSD_IEEE_PRIORITY_TABLE_SET)strtoul(CMD_ARGS[3], NULL, 0);
    entryPtr = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);

    retVal = msdQosPortPrioMapGet(sohoDevNum, port, table, entryPtr, &ieeePriorityTableStruct);

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get Priority map ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutIEEEPriorityTableStruct(table, &ieeePriorityTableStruct);

    return retVal;
}

static int updateTcamData(MSD_TCAM_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameType") == 0)
        {
            entry->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameTypeMask") == 0)
        {
            entry->frameTypeMask = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-timeKey") == 0)
        {
            entry->timeKey = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-timeKeyMask") == 0)
        {
            entry->timeKeyMask = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spv") == 0)
        {
            entry->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spvMask") == 0)
        {
            entry->spvMask = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppri") == 0)
        {
            entry->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppriMask") == 0)
        {
            entry->ppriMask = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvid") == 0)
        {
            entry->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvidMask") == 0)
        {
            entry->pvidMask = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-continu") == 0)
        {
            entry->continu = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidOverride") == 0)
        {
            entry->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidData") == 0)
        {
            entry->vidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-nextId") == 0)
        {
            entry->nextId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriOverride") == 0)
        {
            entry->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriData") == 0)
        {
            entry->qpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriOverride") == 0)
        {
            entry->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriData") == 0)
        {
            entry->fpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvSF") == 0)
        {
            entry->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvData") == 0)
        {
            entry->dpvData = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvMode") == 0)
        {
            entry->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorMode") == 0)
        {
            entry->colorMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPageOverride") == 0)
        {
            entry->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPage") == 0)
        {
            entry->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-unKnownFilter") == 0)
        {
            entry->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egActPoint") == 0)
        {
            entry->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
        {
            entry->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceData") == 0)
        {
            entry->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCPOverride") == 0)
        {
            entry->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCP") == 0)
        {
            entry->DSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionOverride") == 0)
        {
            entry->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionData") == 0)
        {
            entry->factionData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamTunnel") == 0)
        {
            entry->tcamTunnel = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ipMulticast") == 0)
        {
            entry->ipMulticast = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ip2me") == 0)
        {
            entry->ip2me = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-routeEntry") == 0)
        {
            entry->routeEntry = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSctrInc") == 0)
        {
            entry->frerSctrInc = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSctrIndex") == 0)
        {
            entry->frerSctrIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerStreamSplitEn") == 0)
        {
            entry->frerStreamSplitEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqGenEn") == 0)
        {
            entry->frerSeqGenEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqEncType") == 0)
        {
            entry->frerSeqEncType = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqRcvyEn") == 0)
        {
            entry->frerSeqRcvyEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqRcvyIndex") == 0)
        {
            entry->frerSeqRcvyIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerIndvRcvyEn") == 0)
        {
            entry->frerIndvRcvyEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerIndvRcvyId") == 0)
        {
            entry->frerIndvRcvyId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-flowMeterEn") == 0)
        {
            entry->flowMeterEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-flowMeterId") == 0)
        {
            entry->flowMeterId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsDomain") == 0)
        {
            entry->tsDomain = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsFormat") == 0)
        {
            entry->tsFormat = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-depTsEn") == 0)
        {
            entry->depTsEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrTsEn") == 0)
        {
            entry->arrTsEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-streamFilterEn") == 0)
        {
            entry->streamFilterEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-streamFilterId") == 0)
        {
            entry->streamFilterId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-interrupt") == 0)
        {
            entry->interrupt = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IncTcamCtr") == 0)
        {
            entry->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamCtr") == 0)
        {
            entry->tcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Octet") == 0)
        {
            unsigned int octIndex;
            unsigned int octData;
            unsigned int octMask;
            
            octIndex = strtoul(value, NULL, 0);
            if ((octIndex < 1) || (octIndex > 48))
            {
                CLI_ERROR("Invalid Octet number, please start from 1\n");
                return -1;
            }
            octData = strtoul(CMD_ARGS[index], NULL, 0);
            index++;
            octMask = strtoul(CMD_ARGS[index], NULL, 0);
            index++; 
            entry->frameOctet[octIndex - 1] = octData;
            entry->frameOctetMask[octIndex - 1] = octMask;
        }
        else if (strIcmp(cmd, "-frameOctet") == 0)
        {
            int i;
            unsigned int tempList[48] = { 0 };
            if (updateArray(&index, value, 48, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 48; i++)
            {
                entry->frameOctet[i] = tempList[i];
            }
        }
        else if (strIcmp(cmd, "-frameOctetMask") == 0)
        {
            int i;
            unsigned int tempList[48] = { 0 };
            if (updateArray(&index, value, 48, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 48; i++)
            {
                entry->frameOctetMask[i] = tempList[i];
            }
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updateEgrTcamData(MSD_TCAM_EGR_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameModeOverride") == 0)
        {
            entry->frameModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameMode") == 0)
        {
            entry->frameMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-noTtlDec") == 0)
        {
            entry->noTtlDec = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tagModeOverride") == 0)
        {
            entry->tagModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tagMode") == 0)
        {
            entry->tagMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-daMode") == 0)
        {
            entry->daMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-saMode") == 0)
        {
            entry->saMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egVidModeOverride") == 0)
        {
            entry->egVidModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egVidMode") == 0)
        {
            entry->egVidMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egVidData") == 0)
        {
            entry->egVidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egDSCPMode") == 0)
        {
            entry->egDSCPMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egDSCP") == 0)
        {
            entry->egDSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egfpriModeOverride") == 0)
        {
            entry->egfpriModeOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egfpriMode") == 0)
        {
            entry->egfpriMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egEC") == 0)
        {
            entry->egEC = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egFPRI") == 0)
        {
            entry->egFPRI = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egSIDOverride") == 0)
        {
            entry->egSIDOverride = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egSID") == 0)
        {
            entry->egSID = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updateTcamRCData(MSD_TCAM_RC_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-rcIndex") == 0)
        {
            entry->rcIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-rcResult") == 0)
        {
            entry->rcResult = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-rcResultMask") == 0)
        {
            entry->rcResultMask = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-continu") == 0)
        {
            entry->continu = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidOverride") == 0)
        {
            entry->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidData") == 0)
        {
            entry->vidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-nextId") == 0)
        {
            entry->nextId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriOverride") == 0)
        {
            entry->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriData") == 0)
        {
            entry->qpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriOverride") == 0)
        {
            entry->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriData") == 0)
        {
            entry->fpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvSF") == 0)
        {
            entry->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvData") == 0)
        {
            entry->dpvData = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvMode") == 0)
        {
            entry->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorMode") == 0)
        {
            entry->colorMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPageOverride") == 0)
        {
            entry->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPage") == 0)
        {
            entry->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-unKnownFilter") == 0)
        {
            entry->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egActPoint") == 0)
        {
            entry->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
        {
            entry->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceData") == 0)
        {
            entry->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCPOverride") == 0)
        {
            entry->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCP") == 0)
        {
            entry->DSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionOverride") == 0)
        {
            entry->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionData") == 0)
        {
            entry->factionData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamTunnel") == 0)
        {
            entry->tcamTunnel = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ipMulticast") == 0)
        {
            entry->ipMulticast = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ip2me") == 0)
        {
            entry->ip2me = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-routeEntry") == 0)
        {
            entry->routeEntry = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSctrInc") == 0)
        {
            entry->frerSctrInc = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSctrIndex") == 0)
        {
            entry->frerSctrIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerStreamSplitEn") == 0)
        {
            entry->frerStreamSplitEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqGenEn") == 0)
        {
            entry->frerSeqGenEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqEncType") == 0)
        {
            entry->frerSeqEncType = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqRcvyEn") == 0)
        {
            entry->frerSeqRcvyEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerSeqRcvyIndex") == 0)
        {
            entry->frerSeqRcvyIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerIndvRcvyEn") == 0)
        {
            entry->frerIndvRcvyEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frerIndvRcvyId") == 0)
        {
            entry->frerIndvRcvyId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-flowMeterEn") == 0)
        {
            entry->flowMeterEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-flowMeterId") == 0)
        {
            entry->flowMeterId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsDomain") == 0)
        {
            entry->tsDomain = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tsFormat") == 0)
        {
            entry->tsFormat = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-depTsEn") == 0)
        {
            entry->depTsEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-arrTsEn") == 0)
        {
            entry->arrTsEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-streamFilterEn") == 0)
        {
            entry->streamFilterEn = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-streamFilterId") == 0)
        {
            entry->streamFilterId = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-interrupt") == 0)
        {
            entry->interrupt = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IncTcamCtr") == 0)
        {
            entry->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-tcamCtr") == 0)
        {
            entry->tcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }

        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updateTcamRCCompData(MSD_TCAM_RC_COMP_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-Comp0Sel") == 0)
        {
            entry->Comp0Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp0Op") == 0)
        {
            entry->Comp0Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp0LoLimit") == 0)
        {
            entry->Comp0LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp0HiLimit") == 0)
        {
            entry->Comp0HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp1Sel") == 0)
        {
            entry->Comp1Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp1Op") == 0)
        {
            entry->Comp1Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp1LoLimit") == 0)
        {
            entry->Comp1LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp1HiLimit") == 0)
        {
            entry->Comp1HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp2Sel") == 0)
        {
            entry->Comp2Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp2Op") == 0)
        {
            entry->Comp2Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp2LoLimit") == 0)
        {
            entry->Comp2LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp2HiLimit") == 0)
        {
            entry->Comp2HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp3Sel") == 0)
        {
            entry->Comp3Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp3Op") == 0)
        {
            entry->Comp3Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp3LoLimit") == 0)
        {
            entry->Comp3LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp3HiLimit") == 0)
        {
            entry->Comp3HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp4Sel") == 0)
        {
            entry->Comp4Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp4Op") == 0)
        {
            entry->Comp4Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp4LoLimit") == 0)
        {
            entry->Comp4LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp4HiLimit") == 0)
        {
            entry->Comp4HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp5Sel") == 0)
        {
            entry->Comp5Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp5Op") == 0)
        {
            entry->Comp5Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp5LoLimit") == 0)
        {
            entry->Comp5LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp5HiLimit") == 0)
        {
            entry->Comp5HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp6Sel") == 0)
        {
            entry->Comp6Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp6Op") == 0)
        {
            entry->Comp6Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp6LoLimit") == 0)
        {
            entry->Comp6LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp6HiLimit") == 0)
        {
            entry->Comp6HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp7Sel") == 0)
        {
            entry->Comp7Sel = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp7Op") == 0)
        {
            entry->Comp7Op = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp7LoLimit") == 0)
        {
            entry->Comp7LoLimit = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-Comp7HiLimit") == 0)
        {
            entry->Comp7HiLimit = (MSD_U16)strtoul(value, NULL, 0);
        }

        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updateArpMcData(MSD_ARP_MC_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-dupNum") == 0)
        {
            int i;
            unsigned int tempList[11] = { 0 };
            if (updateArray(&index, value, 11, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < 11; i++)
            {
                entry->dupNum[i] = tempList[i];
            }
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static int updateArpUcData(MSD_ARP_UC_DATA *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-routeDA") == 0)
        {
            macStr2macArr(value, entry->routeDA.arEther);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}

static void v4ipStr2ipAddr(char* str, MSD_U8* addr)
{
    char* token = NULL;
    token = strtok(str, ".");
	if (NULL == token)
	{
		return;
	}
    addr[0] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[1] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[2] = (MSD_U8)strtoul(token, NULL, 0);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[3] = (MSD_U8)strtoul(token, NULL, 0);
}
static void v4ipStr2ipAddrMask(char* str, MSD_U8* addr)
{
    char* token = NULL;
    token = strtok(str, ".");
	if (NULL == token)
	{
		return;
	}
    addr[0] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[1] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[2] = (MSD_U8)strtoul(token, NULL, 16);
    token = strtok(NULL, ".");
	if (NULL == token)
	{
		return;
	}
    addr[3] = (MSD_U8)strtoul(token, NULL, 16);
}
static int updateIPV4Header(MSD_TCAM_ENT_IPV4_UNT *ipv4, MSD_TCAM_ENT_IPV4_UNT *ipv4Mask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-version") == 0)
    {
        ipv4->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ihl") == 0)
    {
        ipv4->ihl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tos") == 0)
    {
        ipv4->tos = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ipLength") == 0)
    {
        ipv4->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-id") == 0)
    {
        ipv4->id = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flag") == 0)
    {
        ipv4->flag = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-fragOffset") == 0)
    {
        ipv4->fragOffset = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ttl") == 0)
    {
        ipv4->ttl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-protocol") == 0)
    {
        ipv4->protocol = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hdrChkSum") == 0)
    {
        ipv4->hdrChkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV4Ip") == 0)
    {
        v4ipStr2ipAddr(value, ipv4->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV4Ip") == 0)
    {
        v4ipStr2ipAddr(value, ipv4->dip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-versionMask") == 0)
    {
        ipv4Mask->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ihlMask") == 0)
    {
        ipv4Mask->ihl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tosMask") == 0)
    {
        ipv4Mask->tos = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ipLengthMask") == 0)
    {
        ipv4Mask->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-idMask") == 0)
    {
        ipv4Mask->id = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flagMask") == 0)
    {
        ipv4Mask->flag = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-fragOffsetMask") == 0)
    {
        ipv4Mask->fragOffset = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ttlMask") == 0)
    {
        ipv4Mask->ttl = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-protocolMask") == 0)
    {
        ipv4Mask->protocol = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hdrChkSumMask") == 0)
    {
        ipv4Mask->hdrChkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV4IpMask") == 0)
    {
        v4ipStr2ipAddrMask(value, ipv4Mask->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV4IpMask") == 0)
    {
        v4ipStr2ipAddrMask(value, ipv4Mask->dip);
        ret = 1;
    }

    return ret;
}
static void v6ipStr2ipAddr(char* str, MSD_U16* addr)
{
    char* token = NULL;
    token = strtok(str, ":");
	if (NULL == token)
	{
		return;
	}
    addr[0] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[1] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[2] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[3] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[4] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[5] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[6] = (MSD_U16)strtoul(token, NULL, 16);
    token = strtok(NULL, ":");
	if (NULL == token)
	{
		return;
	}
    addr[7] = (MSD_U16)strtoul(token, NULL, 16);
}
static int updateIPV6Header(MSD_TCAM_ENT_IPV6_UNT *ipv6, MSD_TCAM_ENT_IPV6_UNT *ipv6Mask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-version") == 0)
    {
        ipv6->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tc") == 0)
    {
        ipv6->tc = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flowLbl") == 0)
    {
        ipv6->flowLbl = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-payloadLen") == 0)
    {
        ipv6->payloadLen = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-nextHdr") == 0)
    {
        ipv6->nextHdr = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hopLimit") == 0)
    {
        ipv6->hopLimit = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV6Ip") == 0)
    {
        v6ipStr2ipAddr(value, ipv6->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV6Ip") == 0)
    {
        v6ipStr2ipAddr(value, ipv6->dip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-versionMask") == 0)
    {
        ipv6Mask->version = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcMask") == 0)
    {
        ipv6Mask->tc = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-flowLblMask") == 0)
    {
        ipv6Mask->flowLbl = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-payloadLenMask") == 0)
    {
        ipv6Mask->payloadLen = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-nextHdrMask") == 0)
    {
        ipv6Mask->nextHdr = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-hopLimitMask") == 0)
    {
        ipv6Mask->hopLimit = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-sV6IpMask") == 0)
    {
        v6ipStr2ipAddr(value, ipv6Mask->sip);
        ret = 1;
    }
    else if (strIcmp(cmd, "-dV6IpMask") == 0)
    {
        v6ipStr2ipAddr(value, ipv6Mask->dip);
        ret = 1;
    }

    return ret;
}
static int updateTCPHeader(MSD_TCAM_ENT_TCP_UNT *tcp, MSD_TCAM_ENT_TCP_UNT *tcpMask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-srcPort") == 0)
    {
        tcp->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPort") == 0)
    {
        tcp->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-seqNum") == 0)
    {
        tcp->seqNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ackNum") == 0)
    {
        tcp->ackNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpOffset") == 0)
    {
        tcp->offset = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpFlags") == 0)
    {
        tcp->flags = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-windowSize") == 0)
    {
        tcp->windowSize = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSum") == 0)
    {
        tcp->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-urgPtr") == 0)
    {
        tcp->urgPtr = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-srcPortMask") == 0)
    {
        tcpMask->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPortMask") == 0)
    {
        tcpMask->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-seqNumMask") == 0)
    {
        tcpMask->seqNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-ackNumMask") == 0)
    {
        tcpMask->ackNum = (MSD_U32)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpOffsetMask") == 0)
    {
        tcpMask->offset = (MSD_U8)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-tcpFlagsMask") == 0)
    {
        tcpMask->flags = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-windowSizeMask") == 0)
    {
        tcpMask->windowSize = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSumMask") == 0)
    {
        tcpMask->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-urgPtrMask") == 0)
    {
        tcpMask->urgPtr = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }

    return ret;
}
static int updateUDPHeader(MSD_TCAM_ENT_UDP_UNT *udp, MSD_TCAM_ENT_UDP_UNT *udpMask, char* cmd, char* value)
{
    int ret = 0;

    if (strIcmp(cmd, "-srcPort") == 0)
    {
        udp->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPort") == 0)
    {
        udp->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-udpLength") == 0)
    {
        udp->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSum") == 0)
    {
        udp->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-srcPortMask") == 0)
    {
        udpMask->srcPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-destPortMask") == 0)
    {
        udpMask->destPort = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-udpLengthMask") == 0)
    {
        udpMask->length = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }
    else if (strIcmp(cmd, "-chkSumMask") == 0)
    {
        udpMask->chkSum = (MSD_U16)strtoul(value, NULL, 0);
        ret = 1;
    }

    return ret;
}
static int updateAdvConfigParameter(MSD_TCAM_ENT_KEY *keyMaskPtr, MSD_TCAM_ENT_KEY *keyPtr, MSD_TCAM_ENT_ACT *action, MSD_TCAM_PKT_TYPE pktType, MSD_TCAM_ENT_PKT_UNT *patternPtr, MSD_TCAM_ENT_PKT_UNT *maskPtr, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-frameType") == 0)
        {
            keyPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-frameTypeMask") == 0)
        {
            keyMaskPtr->frameType = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-timeKey") == 0)
        {
            keyPtr->timeKey = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-timeKeyMask") == 0)
        {
            keyMaskPtr->timeKey = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spv") == 0)
        {
            keyPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-spvMask") == 0)
        {
            keyMaskPtr->spv = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppri") == 0)
        {
            keyPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ppriMask") == 0)
        {
            keyMaskPtr->ppri = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvid") == 0)
        {
            keyPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-pvidMask") == 0)
        {
            keyMaskPtr->pvid = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-interrupt") == 0)
        {
            action->interrupt = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-IncTcamCtr") == 0)
        {
            action->IncTcamCtr = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidOverride") == 0)
        {
            action->vidOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vidData") == 0)
        {
            action->vidData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriOverride") == 0)
        {
            action->qpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-qpriData") == 0)
        {
            action->qpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriOverride") == 0)
        {
            action->fpriOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-fpriData") == 0)
        {
            action->fpriData = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvSF") == 0)
        {
            action->dpvSF = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvData") == 0)
        {
            action->dpvData = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-dpvMode") == 0)
        {
            action->dpvMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-colorMode") == 0)
        {
            action->colorMode = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPageOverride") == 0)
        {
            action->vtuPageOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-vtuPage") == 0)
        {
            action->vtuPage = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-unKnownFilter") == 0)
        {
            action->unKnownFilter = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-egActPoint") == 0)
        {
            action->egActPoint = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceOverride") == 0)
        {
            action->ldBalanceOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-ldBalanceData") == 0)
        {
            action->ldBalanceData = (MSD_U8)strtoul(value, NULL, 0);
        }
		else if (strIcmp(cmd, "-tcamCtr") == 0)
		{
			action->tcamCtr = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-tcamTunnel") == 0)
		{
			action->tcamTunnel = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ipMulticast") == 0)
		{
			action->ipMulticast = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-ip2me") == 0)
		{
			action->ip2me = (MSD_BOOL)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-routeEntry") == 0)
		{
			action->routeEntry = (MSD_BOOL)strtoul(value, NULL, 0);
		}
        else if (strIcmp(cmd, "-DSCPOverride") == 0)
        {
            action->DSCPOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-DSCP") == 0)
        {
            action->DSCP = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionOverride") == 0)
        {
            action->factionOverride = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-factionData") == 0)
        {
            action->factionData = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-da") == 0)
        {
            macStr2macArr(value, patternPtr->da.arEther);
        }
        else if (strIcmp(cmd, "-sa") == 0)
        {
            macStr2macArr(value, patternPtr->sa.arEther);
        }
        else if (strIcmp(cmd, "-vlanTag") == 0)
        {
            patternPtr->vlanTag = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etherType") == 0)
        {
            patternPtr->etherType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-daMask") == 0)
        {
            macStr2macArr(value, maskPtr->da.arEther);
        }
        else if (strIcmp(cmd, "-saMask") == 0)
        {
            macStr2macArr(value, maskPtr->sa.arEther);
        }
        else if (strIcmp(cmd, "-vlanTagMask") == 0)
        {
            maskPtr->vlanTag = (MSD_U32)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-etherTypeMask") == 0)
        {
            maskPtr->etherType = (MSD_U16)strtoul(value, NULL, 0);
        }
        else
        {
            int ret = 0;
            switch (pktType)
            {
            case MSD_TCAM_TYPE_IPV4_TCP:
                ret |= updateIPV4Header(&(patternPtr->pktUnit.ipv4Tcp.ipv4), &(maskPtr->pktUnit.ipv4Tcp.ipv4), cmd, value);
                ret |= updateTCPHeader(&(patternPtr->pktUnit.ipv4Tcp.tcp), &(maskPtr->pktUnit.ipv4Tcp.tcp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV4_UDP:
                ret |= updateIPV4Header(&(patternPtr->pktUnit.ipv4Udp.ipv4), &(maskPtr->pktUnit.ipv4Udp.ipv4), cmd, value);
                ret |= updateUDPHeader(&(patternPtr->pktUnit.ipv4Udp.udp), &(maskPtr->pktUnit.ipv4Udp.udp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV6_TCP:
                ret |= updateIPV6Header(&(patternPtr->pktUnit.ipv6Tcp.ipv6), &(maskPtr->pktUnit.ipv6Tcp.ipv6), cmd, value);
                ret |= updateTCPHeader(&(patternPtr->pktUnit.ipv6Tcp.tcp), &(maskPtr->pktUnit.ipv6Tcp.tcp), cmd, value);
                break;
            case MSD_TCAM_TYPE_IPV6_UDP:
                ret |= updateIPV6Header(&(patternPtr->pktUnit.ipv6Udp.ipv6), &(maskPtr->pktUnit.ipv6Udp.ipv6), cmd, value);
                ret |= updateUDPHeader(&(patternPtr->pktUnit.ipv6Udp.udp), &(maskPtr->pktUnit.ipv6Udp.udp), cmd, value);
                break;
            default:
                break;
            }
            if (ret == 0)
            {
                CLI_ERROR("Invalid option input: %s\n", cmd);
                return -1;
            }
        }
    }
    return 0;
}
static void displayTcamData(MSD_TCAM_DATA *tcamData)
{
    int i;
    char *cmd = "tcam";

    CLI_INFO("----- Key & Mask -----\n");

    if (checkValidItem(cmd, "addEntry", "-frameType"))
        CLI_INFO("frameType         : %04x\n", tcamData->frameType);
    if (checkValidItem(cmd, "addEntry", "-frameTypeMask"))
        CLI_INFO("frameTypeMask     : %04x\n", tcamData->frameTypeMask);
    if (checkValidItem(cmd, "addEntry", "-timeKey"))
        CLI_INFO("timeKey           : %04x\n", tcamData->timeKey);
    if (checkValidItem(cmd, "addEntry", "-timeKeyMask"))
        CLI_INFO("timeKeyMask       : %04x\n", tcamData->timeKeyMask);
    if (checkValidItem(cmd, "addEntry", "-spv"))
        CLI_INFO("spv               : %04x\n", tcamData->spv);
    if (checkValidItem(cmd, "addEntry", "-spvMask"))
        CLI_INFO("spvMask           : %04x\n", tcamData->spvMask);
    if (checkValidItem(cmd, "addEntry", "-ppri"))
        CLI_INFO("ppri              : %04x\n", tcamData->ppri);
    if (checkValidItem(cmd, "addEntry", "-ppriMask"))
        CLI_INFO("ppriMask          : %04x\n", tcamData->ppriMask);
    if (checkValidItem(cmd, "addEntry", "-pvid"))
        CLI_INFO("pvid              : %04x\n", tcamData->pvid);
    if (checkValidItem(cmd, "addEntry", "-pvidMask"))
        CLI_INFO("pvidMask          : %04x\n", tcamData->pvidMask);

    CLI_INFO("----- Ethernet Frame Content -----\n");

    for (i = 0; i<48; i++)
    {
        CLI_INFO("frame data Octet %2d         : %4x\n", i + 1, tcamData->frameOctet[i]);
        CLI_INFO("frame data OctetMask %2d     : %4x\n", i + 1, tcamData->frameOctetMask[i]);
    }

    CLI_INFO("----- Ingress Action -----\n");

    if (checkValidItem(cmd, "addEntry", "-continu"))
        CLI_INFO("continu           : %04x\n", tcamData->continu);
    if (checkValidItem(cmd, "addEntry", "-vidOverride"))
        CLI_INFO("vidOverride       : %04x\n", tcamData->vidOverride);
    if (checkValidItem(cmd, "addEntry", "-vidData"))
        CLI_INFO("vidData           : %04x\n", tcamData->vidData);

    if (checkValidItem(cmd, "addEntry", "-nextId"))
        CLI_INFO("nextId            : %04x\n", tcamData->nextId);
    if (checkValidItem(cmd, "addEntry", "-qpriOverride"))
        CLI_INFO("qpriOverride      : %04x\n", tcamData->qpriOverride);
    if (checkValidItem(cmd, "addEntry", "-qpriData"))
        CLI_INFO("qpriData          : %04x\n", tcamData->qpriData);
    if (checkValidItem(cmd, "addEntry", "-fpriOverride"))
        CLI_INFO("fpriOverride      : %04x\n", tcamData->fpriOverride);
    if (checkValidItem(cmd, "addEntry", "-fpriData"))
        CLI_INFO("fpriData          : %04x\n", tcamData->fpriData);

    if (checkValidItem(cmd, "addEntry", "-dpvSF"))
        CLI_INFO("dpvSF             : %04x\n", tcamData->dpvSF);
    if (checkValidItem(cmd, "addEntry", "-dpvData"))
        CLI_INFO("dpvData           : %04x\n", (unsigned int)tcamData->dpvData);
    if (checkValidItem(cmd, "addEntry", "-dpvMode"))
        CLI_INFO("dpvMode           : %04x\n", tcamData->dpvMode);

    if (checkValidItem(cmd, "addEntry", "-unKnownFilter"))
        CLI_INFO("unKnownFilter     : %04x\n", tcamData->unKnownFilter);

    if (checkValidItem(cmd, "addEntry", "-ldBalanceOverride"))
        CLI_INFO("ldBalanceOverride : %04x\n", tcamData->ldBalanceOverride);
    if (checkValidItem(cmd, "addEntry", "-ldBalanceData"))
        CLI_INFO("ldBalanceData     : %04x\n", tcamData->ldBalanceData);
    if (checkValidItem(cmd, "addEntry", "-ipMulticast"))
        CLI_INFO("ipMulticast       : %04x\n", tcamData->ipMulticast);
    if (checkValidItem(cmd, "addEntry", "-ip2me"))
        CLI_INFO("ip2me             : %04x\n", tcamData->ip2me);
    if (checkValidItem(cmd, "addEntry", "-tcamTunnel"))
        CLI_INFO("tcamTunnel        : %04x\n", tcamData->tcamTunnel);
    if (checkValidItem(cmd, "addEntry", "-routeEntry"))
        CLI_INFO("routeEntry        : %04x\n", tcamData->routeEntry);

    if (checkValidItem(cmd, "addEntry", "-factionOverride"))
        CLI_INFO("factionOverride   : %04x\n", tcamData->factionOverride);
    if (checkValidItem(cmd, "addEntry", "-factionData"))
        CLI_INFO("factionData       : %04x\n", tcamData->factionData);
    if (checkValidItem(cmd, "addEntry", "-colorMode"))
        CLI_INFO("colorMode         : %04x\n", tcamData->colorMode);
    if (checkValidItem(cmd, "addEntry", "-vtuPageOverride"))
        CLI_INFO("vtuPageOverride   : %04x\n", tcamData->vtuPageOverride);
    if (checkValidItem(cmd, "addEntry", "-vtuPage"))
        CLI_INFO("vtuPage           : %04x\n", tcamData->vtuPage);
    if (checkValidItem(cmd, "addEntry", "-egActPoint"))
        CLI_INFO("egActPoint        : %04x\n", tcamData->egActPoint);
    if (checkValidItem(cmd, "addEntry", "-DSCPOverride"))
        CLI_INFO("DSCPOverride      : %04x\n", tcamData->DSCPOverride);
    if (checkValidItem(cmd, "addEntry", "-DSCP"))
        CLI_INFO("DSCP              : %04x\n", tcamData->DSCP);

    if (checkValidItem(cmd, "addEntry", "-frerSctrInc"))
        CLI_INFO("frerSctrInc       : %04x\n", tcamData->frerSctrInc);
    if (checkValidItem(cmd, "addEntry", "-frerSctrIndex"))
        CLI_INFO("frerSctrIndex     : %04x\n", tcamData->frerSctrIndex);
    if (checkValidItem(cmd, "addEntry", "-frerStreamSplitEn"))
        CLI_INFO("frerStreamSplitEn : %04x\n", tcamData->frerStreamSplitEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqGenEn"))
        CLI_INFO("frerSeqGenEn      : %04x\n", tcamData->frerSeqGenEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqEncType"))
        CLI_INFO("frerSeqEncType    : %04x\n", tcamData->frerSeqEncType);
    if (checkValidItem(cmd, "addEntry", "-frerSeqRcvyEn"))
        CLI_INFO("frerSeqRcvyEn     : %04x\n", tcamData->frerSeqRcvyEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqRcvyIndex"))
        CLI_INFO("frerSeqRcvyIndex  : %04x\n", tcamData->frerSeqRcvyIndex);
    if (checkValidItem(cmd, "addEntry", "-frerIndvRcvyEn"))
        CLI_INFO("frerIndvRcvyEn    : %04x\n", tcamData->frerIndvRcvyEn);
    if (checkValidItem(cmd, "addEntry", "-frerIndvRcvyId"))
        CLI_INFO("frerIndvRcvyId    : %04x\n", tcamData->frerIndvRcvyId);
    if (checkValidItem(cmd, "addEntry", "-flowMeterEn"))
        CLI_INFO("flowMeterEn       : %04x\n", tcamData->flowMeterEn);
    if (checkValidItem(cmd, "addEntry", "-flowMeterId"))
        CLI_INFO("flowMeterId       : %04x\n", tcamData->flowMeterId);
    if (checkValidItem(cmd, "addEntry", "-tsDomain"))
        CLI_INFO("tsDomain          : %04x\n", tcamData->tsDomain);
    if (checkValidItem(cmd, "addEntry", "-tsFormat"))
        CLI_INFO("tsFormat          : %04x\n", tcamData->tsFormat);
    if (checkValidItem(cmd, "addEntry", "-depTsEn"))
        CLI_INFO("depTsEn           : %04x\n", tcamData->depTsEn);
    if (checkValidItem(cmd, "addEntry", "-arrTsEn"))
        CLI_INFO("arrTsEn           : %04x\n", tcamData->arrTsEn);
    if (checkValidItem(cmd, "addEntry", "-streamFilterEn"))
        CLI_INFO("streamFilterEn    : %04x\n", tcamData->streamFilterEn);
    if (checkValidItem(cmd, "addEntry", "-streamFilterId"))
        CLI_INFO("streamFilterId    : %04x\n", tcamData->streamFilterId);

    if (checkValidItem(cmd, "addEntry", "-interrupt"))
        CLI_INFO("interrupt         : %04x\n", tcamData->interrupt);
    if (checkValidItem(cmd, "addEntry", "-IncTcamCtr"))
        CLI_INFO("IncTcamCtr        : %04x\n", tcamData->IncTcamCtr);
    if (checkValidItem(cmd, "addEntry", "-tcamCtr"))
        CLI_INFO("tcamCtr           : %04x\n", tcamData->tcamCtr);
}
static void displayTcamEgrData(MSD_TCAM_EGR_DATA *tcamEgrData)
{
    char *cmd = "tcam";
    CLI_INFO("----- MSD_TCAM_EGR_DATA -----\n");

    if (checkValidItem(cmd, "egrAddEntry", "-frameModeOverride"))
        CLI_INFO("frameModeOverride : %04x\n", tcamEgrData->frameModeOverride);
    if (checkValidItem(cmd, "egrAddEntry", "-frameMode"))
        CLI_INFO("frameMode         : %04x\n", tcamEgrData->frameMode);
    if (checkValidItem(cmd, "egrAddEntry", "-noTTLDec"))
        CLI_INFO("noTtlDec          : %04x\n", tcamEgrData->noTtlDec);
    if (checkValidItem(cmd, "egrAddEntry", "-tagModeOverride"))
        CLI_INFO("tagModeOverride   : %04x\n", tcamEgrData->tagModeOverride);
    if (checkValidItem(cmd, "egrAddEntry", "-tagMode"))
        CLI_INFO("tagMode           : %04x\n", tcamEgrData->tagMode);
    if (checkValidItem(cmd, "egrAddEntry", "-daMode"))
        CLI_INFO("daMode            : %04x\n", tcamEgrData->daMode);
    if (checkValidItem(cmd, "egrAddEntry", "-saMode"))
        CLI_INFO("saMode            : %04x\n", tcamEgrData->saMode);

    if (checkValidItem(cmd, "egrAddEntry", "-egVidModeOverride"))
        CLI_INFO("egVidModeOverride : %04x\n", tcamEgrData->egVidModeOverride);
    if (checkValidItem(cmd, "egrAddEntry", "-egVidMode"))
        CLI_INFO("egVidMode         : %04x\n", tcamEgrData->egVidMode);
    if (checkValidItem(cmd, "egrAddEntry", "-egVidData"))
        CLI_INFO("egVidData         : %04x\n", tcamEgrData->egVidData);

    if (checkValidItem(cmd, "egrAddEntry", "-egDSCPMode"))
        CLI_INFO("egDSCPMode        : %04x\n", tcamEgrData->egDSCPMode);
    if (checkValidItem(cmd, "egrAddEntry", "-egDSCP"))
        CLI_INFO("egDSCP            : %04x\n", tcamEgrData->egDSCP);
    if (checkValidItem(cmd, "egrAddEntry", "-egfpriModeOverride"))
        CLI_INFO("egfpriModeOverride: %04x\n", tcamEgrData->egfpriModeOverride);
    if (checkValidItem(cmd, "egrAddEntry", "-egfpriMode"))
        CLI_INFO("egfpriMode        : %04x\n", tcamEgrData->egfpriMode);
    if (checkValidItem(cmd, "egrAddEntry", "-egEC"))
        CLI_INFO("egCFI/DE          : %04x\n", tcamEgrData->egEC);
    if (checkValidItem(cmd, "egrAddEntry", "-egFPRI"))
        CLI_INFO("egFPRI            : %04x\n", tcamEgrData->egFPRI);
    if (checkValidItem(cmd, "egrAddEntry", "-egSIDOverride"))
        CLI_INFO("egSIDOverride     : %04x\n", tcamEgrData->egSIDOverride);
    if (checkValidItem(cmd, "egrAddEntry", "-egSID"))
        CLI_INFO("egSID             : %04x\n", tcamEgrData->egSID);

}
static void displayTcamRCData(MSD_TCAM_RC_DATA *tcamrcData)
{
    char *cmd = "tcamrc";
    CLI_INFO("----- Index & Result -----\n");

    if (checkValidItem(cmd, "addEntry", "-rcIndex"))
        CLI_INFO("rcIndex           : %04x\n", tcamrcData->rcIndex);
    if (checkValidItem(cmd, "addEntry", "-rcResult"))
        CLI_INFO("rcResult          : %04x\n", tcamrcData->rcResult);
    if (checkValidItem(cmd, "addEntry", "-rcResultMask"))
        CLI_INFO("rcResultMask      : %04x\n", tcamrcData->rcResultMask);

    CLI_INFO("----- Ingress Action -----\n");

    if (checkValidItem(cmd, "addEntry", "-continu"))
        CLI_INFO("continu           : %04x\n", tcamrcData->continu);
    if (checkValidItem(cmd, "addEntry", "-vidOverride"))
        CLI_INFO("vidOverride       : %04x\n", tcamrcData->vidOverride);
    if (checkValidItem(cmd, "addEntry", "-vidData"))
        CLI_INFO("vidData           : %04x\n", tcamrcData->vidData);

    if (checkValidItem(cmd, "addEntry", "-nextId"))
        CLI_INFO("nextId            : %04x\n", tcamrcData->nextId);
    if (checkValidItem(cmd, "addEntry", "-qpriOverride"))
        CLI_INFO("qpriOverride      : %04x\n", tcamrcData->qpriOverride);
    if (checkValidItem(cmd, "addEntry", "-qpriData"))
        CLI_INFO("qpriData          : %04x\n", tcamrcData->qpriData);
    if (checkValidItem(cmd, "addEntry", "-fpriOverride"))
        CLI_INFO("fpriOverride      : %04x\n", tcamrcData->fpriOverride);
    if (checkValidItem(cmd, "addEntry", "-fpriData"))
        CLI_INFO("fpriData          : %04x\n", tcamrcData->fpriData);

    if (checkValidItem(cmd, "addEntry", "-dpvSF"))
        CLI_INFO("dpvSF             : %04x\n", tcamrcData->dpvSF);
    if (checkValidItem(cmd, "addEntry", "-dpvData"))
        CLI_INFO("dpvData           : %04x\n", (unsigned int)tcamrcData->dpvData);
    if (checkValidItem(cmd, "addEntry", "-dpvMode"))
        CLI_INFO("dpvMode           : %04x\n", tcamrcData->dpvMode);

    if (checkValidItem(cmd, "addEntry", "-unKnownFilter"))
        CLI_INFO("unKnownFilter     : %04x\n", tcamrcData->unKnownFilter);

    if (checkValidItem(cmd, "addEntry", "-ldBalanceOverride"))
        CLI_INFO("ldBalanceOverride : %04x\n", tcamrcData->ldBalanceOverride);
    if (checkValidItem(cmd, "addEntry", "-ldBalanceData"))
        CLI_INFO("ldBalanceData     : %04x\n", tcamrcData->ldBalanceData);
    if (checkValidItem(cmd, "addEntry", "-ipMulticast"))
        CLI_INFO("ipMulticast       : %04x\n", tcamrcData->ipMulticast);
    if (checkValidItem(cmd, "addEntry", "-ip2me"))
        CLI_INFO("ip2me             : %04x\n", tcamrcData->ip2me);
    if (checkValidItem(cmd, "addEntry", "-tcamTunnel"))
        CLI_INFO("tcamTunnel        : %04x\n", tcamrcData->tcamTunnel);
    if (checkValidItem(cmd, "addEntry", "-routeEntry"))
        CLI_INFO("routeEntry        : %04x\n", tcamrcData->routeEntry);

    if (checkValidItem(cmd, "addEntry", "-factionOverride"))
        CLI_INFO("factionOverride   : %04x\n", tcamrcData->factionOverride);
    if (checkValidItem(cmd, "addEntry", "-factionData"))
        CLI_INFO("factionData       : %04x\n", tcamrcData->factionData);
    if (checkValidItem(cmd, "addEntry", "-colorMode"))
        CLI_INFO("colorMode         : %04x\n", tcamrcData->colorMode);
    if (checkValidItem(cmd, "addEntry", "-vtuPageOverride"))
        CLI_INFO("vtuPageOverride   : %04x\n", tcamrcData->vtuPageOverride);
    if (checkValidItem(cmd, "addEntry", "-vtuPage"))
        CLI_INFO("vtuPage           : %04x\n", tcamrcData->vtuPage);
    if (checkValidItem(cmd, "addEntry", "-egActPoint"))
        CLI_INFO("egActPoint        : %04x\n", tcamrcData->egActPoint);
    if (checkValidItem(cmd, "addEntry", "-DSCPOverride"))
        CLI_INFO("DSCPOverride      : %04x\n", tcamrcData->DSCPOverride);
    if (checkValidItem(cmd, "addEntry", "-DSCP"))
        CLI_INFO("DSCP              : %04x\n", tcamrcData->DSCP);

    if (checkValidItem(cmd, "addEntry", "-frerSctrInc"))
        CLI_INFO("frerSctrInc       : %04x\n", tcamrcData->frerSctrInc);
    if (checkValidItem(cmd, "addEntry", "-frerSctrIndex"))
        CLI_INFO("frerSctrIndex     : %04x\n", tcamrcData->frerSctrIndex);
    if (checkValidItem(cmd, "addEntry", "-frerStreamSplitEn"))
        CLI_INFO("frerStreamSplitEn : %04x\n", tcamrcData->frerStreamSplitEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqGenEn"))
        CLI_INFO("frerSeqGenEn      : %04x\n", tcamrcData->frerSeqGenEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqEncType"))
        CLI_INFO("frerSeqEncType    : %04x\n", tcamrcData->frerSeqEncType);
    if (checkValidItem(cmd, "addEntry", "-frerSeqRcvyEn"))
        CLI_INFO("frerSeqRcvyEn     : %04x\n", tcamrcData->frerSeqRcvyEn);
    if (checkValidItem(cmd, "addEntry", "-frerSeqRcvyIndex"))
        CLI_INFO("frerSeqRcvyIndex  : %04x\n", tcamrcData->frerSeqRcvyIndex);
    if (checkValidItem(cmd, "addEntry", "-frerIndvRcvyEn"))
        CLI_INFO("frerIndvRcvyEn    : %04x\n", tcamrcData->frerIndvRcvyEn);
    if (checkValidItem(cmd, "addEntry", "-frerIndvRcvyId"))
        CLI_INFO("frerIndvRcvyId    : %04x\n", tcamrcData->frerIndvRcvyId);
    if (checkValidItem(cmd, "addEntry", "-flowMeterEn"))
        CLI_INFO("flowMeterEn       : %04x\n", tcamrcData->flowMeterEn);
    if (checkValidItem(cmd, "addEntry", "-flowMeterId"))
        CLI_INFO("flowMeterId       : %04x\n", tcamrcData->flowMeterId);
    if (checkValidItem(cmd, "addEntry", "-tsDomain"))
        CLI_INFO("tsDomain          : %04x\n", tcamrcData->tsDomain);
    if (checkValidItem(cmd, "addEntry", "-tsFormat"))
        CLI_INFO("tsFormat          : %04x\n", tcamrcData->tsFormat);
    if (checkValidItem(cmd, "addEntry", "-depTsEn"))
        CLI_INFO("depTsEn           : %04x\n", tcamrcData->depTsEn);
    if (checkValidItem(cmd, "addEntry", "-arrTsEn"))
        CLI_INFO("arrTsEn           : %04x\n", tcamrcData->arrTsEn);
    if (checkValidItem(cmd, "addEntry", "-streamFilterEn"))
        CLI_INFO("streamFilterEn    : %04x\n", tcamrcData->streamFilterEn);
    if (checkValidItem(cmd, "addEntry", "-streamFilterId"))
        CLI_INFO("streamFilterId    : %04x\n", tcamrcData->streamFilterId);

    if (checkValidItem(cmd, "addEntry", "-interrupt"))
        CLI_INFO("interrupt         : %04x\n", tcamrcData->interrupt);
    if (checkValidItem(cmd, "addEntry", "-IncTcamCtr"))
        CLI_INFO("IncTcamCtr        : %04x\n", tcamrcData->IncTcamCtr);
    if (checkValidItem(cmd, "addEntry", "-tcamCtr"))
        CLI_INFO("tcamCtr           : %04x\n", tcamrcData->tcamCtr);
}
static void displayTcamRCCompData(MSD_TCAM_RC_COMP_DATA *tcamrccompData)
{
    char *cmd = "tcamrccomp";
    CLI_INFO("----- Range Comparator 0 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp0Sel"))
        CLI_INFO("Comp0Sel          : %04x\n", tcamrccompData->Comp0Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp0Op"))
        CLI_INFO("Comp0Op           : %04x\n", tcamrccompData->Comp0Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp0LoLimit"))
        CLI_INFO("Comp0LoLimit      : %04x\n", tcamrccompData->Comp0LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp0HiLimit"))
        CLI_INFO("Comp0HiLimit      : %04x\n", tcamrccompData->Comp0HiLimit);

    CLI_INFO("----- Range Comparator 1 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp1Sel"))
        CLI_INFO("Comp1Sel          : %04x\n", tcamrccompData->Comp1Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp1Op"))
        CLI_INFO("Comp1Op           : %04x\n", tcamrccompData->Comp1Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp1LoLimit"))
        CLI_INFO("Comp1LoLimit      : %04x\n", tcamrccompData->Comp1LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp1HiLimit"))
        CLI_INFO("Comp1HiLimit      : %04x\n", tcamrccompData->Comp1HiLimit);

    CLI_INFO("----- Range Comparator 2 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp2Sel"))
        CLI_INFO("Comp2Sel          : %04x\n", tcamrccompData->Comp2Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp2Op"))
        CLI_INFO("Comp2Op           : %04x\n", tcamrccompData->Comp2Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp2LoLimit"))
        CLI_INFO("Comp2LoLimit      : %04x\n", tcamrccompData->Comp2LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp2HiLimit"))
        CLI_INFO("Comp2HiLimit      : %04x\n", tcamrccompData->Comp2HiLimit);

    CLI_INFO("----- Range Comparator 3 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp3Sel"))
        CLI_INFO("Comp3Sel          : %04x\n", tcamrccompData->Comp3Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp3Op"))
        CLI_INFO("Comp3Op           : %04x\n", tcamrccompData->Comp3Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp3LoLimit"))
        CLI_INFO("Comp3LoLimit      : %04x\n", tcamrccompData->Comp3LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp3HiLimit"))
        CLI_INFO("Comp3HiLimit      : %04x\n", tcamrccompData->Comp3HiLimit);

    CLI_INFO("----- Range Comparator 4 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp4Sel"))
        CLI_INFO("Comp4Sel          : %04x\n", tcamrccompData->Comp4Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp4Op"))
        CLI_INFO("Comp4Op           : %04x\n", tcamrccompData->Comp4Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp4LoLimit"))
        CLI_INFO("Comp4LoLimit      : %04x\n", tcamrccompData->Comp4LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp4HiLimit"))
        CLI_INFO("Comp4HiLimit      : %04x\n", tcamrccompData->Comp4HiLimit);

    CLI_INFO("----- Range Comparator 5 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp5Sel"))
        CLI_INFO("Comp5Sel          : %04x\n", tcamrccompData->Comp5Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp5Op"))
        CLI_INFO("Comp5Op           : %04x\n", tcamrccompData->Comp5Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp5LoLimit"))
        CLI_INFO("Comp5LoLimit      : %04x\n", tcamrccompData->Comp5LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp5HiLimit"))
        CLI_INFO("Comp5HiLimit      : %04x\n", tcamrccompData->Comp5HiLimit);

    CLI_INFO("----- Range Comparator 6 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp6Sel"))
        CLI_INFO("Comp6Sel          : %04x\n", tcamrccompData->Comp6Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp6Op"))
        CLI_INFO("Comp6Op           : %04x\n", tcamrccompData->Comp6Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp6LoLimit"))
        CLI_INFO("Comp6LoLimit      : %04x\n", tcamrccompData->Comp6LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp6HiLimit"))
        CLI_INFO("Comp6HiLimit      : %04x\n", tcamrccompData->Comp6HiLimit);

    CLI_INFO("----- Range Comparator 7 -----\n");

    if (checkValidItem(cmd, "addRCComp", "-Comp7Sel"))
        CLI_INFO("Comp7Sel          : %04x\n", tcamrccompData->Comp7Sel);
    if (checkValidItem(cmd, "addRCComp", "-Comp7Op"))
        CLI_INFO("Comp7Op           : %04x\n", tcamrccompData->Comp7Op);
    if (checkValidItem(cmd, "addRCComp", "-Comp7LoLimit"))
        CLI_INFO("Comp7LoLimit      : %04x\n", tcamrccompData->Comp7LoLimit);
    if (checkValidItem(cmd, "addRCComp", "-Comp7HiLimit"))
        CLI_INFO("Comp7HiLimit      : %04x\n", tcamrccompData->Comp7HiLimit);

}
static void displayArpData(MSD_ARP_DATA *arpData)
{
    CLI_INFO("The ARP Table Data:\nReg4:0x%4x\nReg3:0x%4x\nReg2:0x%4x\n", (arpData->arpData[5] << 8) | arpData->arpData[4], (arpData->arpData[3] << 8) | arpData->arpData[2], \
        (arpData->arpData[1] << 8) | arpData->arpData[0]);

}

int tcamFlushAll(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdTcamAllDelete(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush tcam all entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int tcamFlushEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryDelete(sohoDevNum, tcamPointer);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateTcamData(&tcamData, 3) != 0)
        return -1;

    retVal = msdTcamEntryAdd(sohoDevNum, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamModifyEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;
    MSD_BOOL	found;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryFind(sohoDevNum, tcamPointer, &tcamData, &found);

    if (found == MSD_FALSE)
    {
        CLI_INFO("Can not find the tcam entry entryNum=%d, please load it firstly\n", (int)tcamPointer);
        return 1;
    }

    if (updateTcamData(&tcamData, 3) != 0)
        return -1;

    retVal = msdTcamEntryAdd(sohoDevNum, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error modify single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamCopyEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer1;
    MSD_U32 tcamPointer2;
    MSD_TCAM_DATA tcamData;
    MSD_BOOL	found;

    if (nargs < 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer1 = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer2 = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdTcamEntryFind(sohoDevNum, tcamPointer1, &tcamData, &found);
	if (retVal != MSD_OK)
	{
		CLI_INFO("msdTcamEntryFind return fail.\n");
		return retVal;
	}

    if (found == MSD_FALSE)
    {
        CLI_INFO("Can not find the source tcam entry entryNum=%d, please load it firstly\n", (int)tcamPointer1);
        return 1;
    }

    retVal = msdTcamEntryAdd(sohoDevNum, tcamPointer2, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error copy to target entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer2, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamReadEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryRead(sohoDevNum, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayTcamData(&tcamData);
    return 0;
}
int tcamGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryGetNext(sohoDevNum, &tcamPointer, &tcamData);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid tcam entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[2], NULL, 0));
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next single entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("Get back next entry: %d\n", (int)tcamPointer);
    displayTcamData(&tcamData);
    return 0;
}
int tcamFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;
    MSD_BOOL	found;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamEntryFind(sohoDevNum, tcamPointer, &tcamData, &found);

    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    if (found == MSD_FALSE)
    {
        CLI_INFO("Can not find the tcam entry entryNum=%d\n", (int)tcamPointer);
    }
    else
    {
        CLI_INFO("Find the entry entryNum=%d and detail info:\n", (int)tcamPointer);
        displayTcamData(&tcamData);
    }

    return 0;
}
int tcamSetMode(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_TCAM_MODE mode;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    mode = (MSD_TCAM_MODE)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdPortTcamModeSet(sohoDevNum, port, mode);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set tcam mode port=%d mode=%d ret[%d: %s]"
            "\n", (int)port, mode, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamGetMode(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_TCAM_MODE mode;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdPortTcamModeGet(sohoDevNum, port, &mode);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get tcam mode port=%d  ret[%d: %s]"
            "\n", (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    switch (mode)
    {
    case MSD_TCAM_MODE_DISABLE:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_DISABLE\n", port);
        break;
    case MSD_TCAM_MODE_ENABLE_48:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_ENABLE_48\n", port);
        break;
    case MSD_TCAM_MODE_ENABLE_96:
        CLI_INFO("Tcam mode for Port %d is MSD_TCAM_MODE_ENABLE_96\n", port);
        break;
    default:
        CLI_INFO("Unknown Tcam mode for Port %d \n");
        break;
    }

    return retVal;
}
int tcamAddPort(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_LPORT port;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdTcamEntryPortAdd(sohoDevNum, tcamPointer, port);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add tcam port entryNum=%d port=%d  ret[%d: %s]"
            "\n", (int)tcamPointer, (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int tcamRemovePort(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_LPORT port;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdTcamEntryPortRemove(sohoDevNum, tcamPointer, port);
    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error add tcam port entryNum=%d port=%d  ret[%d: %s]"
            "\n", (int)tcamPointer, (int)port, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int tcamEgrDelAllPorts(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdEgrTcamEntryAllPortsDelete(sohoDevNum, tcamPointer);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush egr entry for all ports entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamEgrDelEntry(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_U32 tcamPointer;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryPerPortDelete(sohoDevNum, port, tcamPointer);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush egr entry per port port=%d entryNum=%d ret[%d: %s]"
            "\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamEgrAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_U32 tcamPointer;
    MSD_TCAM_EGR_DATA tcamData;

    if (nargs < 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    if (updateEgrTcamData(&tcamData, 4) != 0)
        return -1;

    retVal = msdEgrTcamEntryAdd(sohoDevNum, port, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single egress entry port=%d entryNum=%d ret[%d: %s]"
            "\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamEgrReadEntry(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_U32 tcamPointer;
    MSD_TCAM_EGR_DATA tcamData;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryRead(sohoDevNum, port, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single egress entry port=%d entryNum=%d ret[%d: %s]"
            "\n", (int)port, (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayTcamEgrData(&tcamData);
    return 0;
}
int tcamEgrGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_U32 tcamPointer;
    MSD_TCAM_EGR_DATA tcamData;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_EGR_DATA));
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdEgrTcamEntryGetNext(sohoDevNum, port, &tcamPointer, &tcamData);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid tcam egress entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[3], NULL, 0));
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next egress single entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("Get back next entry: %d\n", (int)tcamPointer);
    displayTcamEgrData(&tcamData);
    return 0;
}
int tcamAdvConfig(void)
{
    MSD_STATUS retVal;
    MSD_TCAM_PKT_TYPE pktType;
    MSD_U32 tcamPointer;
    MSD_U32 tcamPointer2;
    MSD_BOOL entry2Used;

    MSD_TCAM_ENT_KEY keyMaskPtr;
    MSD_TCAM_ENT_KEY keyPtr;
    MSD_TCAM_ENT_PKT_UNT maskPtr;
    MSD_TCAM_ENT_PKT_UNT patternPtr;
    MSD_TCAM_ENT_ACT actionPtr;


    if (nargs <= 5)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&keyMaskPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    memset(&keyPtr, 0, sizeof(MSD_TCAM_ENT_KEY));
    memset(&maskPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    memset(&patternPtr, 0, sizeof(MSD_TCAM_ENT_PKT_UNT));
    memset(&actionPtr, 0, sizeof(MSD_TCAM_ENT_ACT));
    pktType = (MSD_TCAM_PKT_TYPE)strtoul(CMD_ARGS[2], NULL, 0);
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[3], NULL, 0);
    tcamPointer2 = (MSD_U32)strtoul(CMD_ARGS[4], NULL, 0);

    if (updateAdvConfigParameter(&keyMaskPtr, &keyPtr, &actionPtr, pktType, &patternPtr, &maskPtr, 5) != 0)
        return -1;


    retVal = msdTcamAdvConfig(sohoDevNum, pktType, tcamPointer, tcamPointer2, &keyMaskPtr, &keyPtr, &maskPtr, &patternPtr, &actionPtr, &entry2Used);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (entry2Used == MSD_TRUE)
    {
        CLI_INFO("Tcam advance configuration success, and the second entry(entryNum=%d) has been used\n", tcamPointer2);
    }
    else
    {
        CLI_INFO("Tcam advance configuration success, and the second entry(entryNum=%d) not been used\n", tcamPointer2);
    }

    return 0;
}

int tcamrcFlushAll(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdTcamRCAllDelete(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush tcam all entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int tcamrcFlushEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrcPointer;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    tcamrcPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamRCEntryDelete(sohoDevNum, tcamrcPointer);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamrcPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamrcAddEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrcPointer;
    MSD_TCAM_RC_DATA tcamrcData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrcData, 0, sizeof(MSD_TCAM_RC_DATA));
    tcamrcPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateTcamRCData(&tcamrcData, 3) != 0)
        return -1;

    retVal = msdTcamRCEntryAdd(sohoDevNum, tcamrcPointer, &tcamrcData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamrcPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamrcReadEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrcPointer;
    MSD_TCAM_RC_DATA tcamrcData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrcData, 0, sizeof(MSD_TCAM_RC_DATA));
    tcamrcPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamRCEntryRead(sohoDevNum, tcamrcPointer, &tcamrcData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamrcPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayTcamRCData(&tcamrcData);
    return 0;
}
int tcamrcGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrcPointer;
    MSD_TCAM_RC_DATA tcamrcData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrcData, 0, sizeof(MSD_TCAM_RC_DATA));
    tcamrcPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamRCEntryGetNext(sohoDevNum, &tcamrcPointer, &tcamrcData);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid tcam rc entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[2], NULL, 0));
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next single entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("Get back next entry: %d\n", (int)tcamrcPointer);
    displayTcamRCData(&tcamrcData);
    return 0;
}
int tcamrcFindEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrcPointer;
    MSD_TCAM_RC_DATA tcamrcData;
    MSD_BOOL	found;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrcData, 0, sizeof(MSD_TCAM_RC_DATA));
    tcamrcPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamRCEntryFind(sohoDevNum, tcamrcPointer, &tcamrcData, &found);

    if ((retVal != MSD_OK) && (retVal != MSD_NO_SUCH)) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error find single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamrcPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    if (found == MSD_FALSE)
    {
        CLI_INFO("Can not find the tcam entry entryNum=%d\n", (int)tcamrcPointer);
    }
    else
    {
        CLI_INFO("Find the entry entryNum=%d and detail info:\n", (int)tcamrcPointer);
        displayTcamRCData(&tcamrcData);
    }

    return 0;
}

int tcamrccompCompAdd(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrccompPointer;
    MSD_TCAM_RC_COMP_DATA tcamrccompData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrccompData, 0, sizeof(MSD_TCAM_RC_COMP_DATA));
    tcamrccompPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateTcamRCCompData(&tcamrccompData, 3) != 0)
        return -1;

    retVal = msdTcamRCCompAdd(sohoDevNum, tcamrccompPointer, &tcamrccompData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single comparator portNum=%d ret[%d: %s]"
            "\n", (int)tcamrccompPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int tcamrccompCompRead(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamrccompPointer;
    MSD_TCAM_RC_COMP_DATA tcamrccompData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&tcamrccompData, 0, sizeof(MSD_TCAM_RC_COMP_DATA));
    tcamrccompPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdTcamRCCompRead(sohoDevNum, tcamrccompPointer, &tcamrccompData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single comparator portNum=%d ret[%d: %s]"
            "\n", (int)tcamrccompPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayTcamRCCompData(&tcamrccompData);
    return 0;
}
int tcamrccompFlushAll(void)
{
	MSD_STATUS retVal;

	if (nargs != 2)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	retVal = msdTcamRCCompAllDelete(sohoDevNum);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush tcam all entry ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return 0;
}
int tcamrccompFlushPort(void)
{
	MSD_STATUS retVal;
	MSD_U32 tcamrccompPort;

	if (nargs != 3)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	tcamrccompPort = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

	retVal = msdTcamRCCompFlushPort(sohoDevNum, tcamrccompPort);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
			"\n", (int)tcamrccompPort, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}

int arpFlushAll(void)
{
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    retVal = msdArpAllDelete(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush arp all entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int arpFlushEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 arpPointer;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    arpPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdArpEntryDelete(sohoDevNum, arpPointer);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
            "\n", (int)arpPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int arpAddMcEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 arpPointer;
    MSD_ARP_MC_DATA arpMcData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&arpMcData, 0, sizeof(MSD_ARP_MC_DATA));
    arpPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateArpMcData(&arpMcData, 3) != 0)
        return -1;

    retVal = msdArpMcEntryAdd(sohoDevNum, arpPointer, &arpMcData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)arpPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int arpAddUcEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 arpPointer;
    MSD_ARP_UC_DATA arpUcData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&arpUcData, 0, sizeof(MSD_ARP_DATA));
    arpPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateArpUcData(&arpUcData, 3) != 0)
        return -1;

    retVal = msdArpUcEntryAdd(sohoDevNum, arpPointer, &arpUcData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)arpPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}
int arpReadEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 arpPointer;
    MSD_ARP_DATA arpData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&arpData, 0, sizeof(MSD_ARP_DATA));
    arpPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdArpEntryRead(sohoDevNum, arpPointer, &arpData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)arpPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayArpData(&arpData);
    return 0;
}
int arpGetNextEntry(void)
{
    MSD_STATUS retVal;
    MSD_U32 arpPointer;
    MSD_ARP_DATA arpData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&arpData, 0, sizeof(MSD_ARP_DATA));
    arpPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdArpEntryGetNext(sohoDevNum, &arpPointer, &arpData);

    if (retVal == MSD_NO_SUCH)
    {
        CLI_INFO("Cannot find any valid tcam entry start from entryNum: %d\n", (int)strtoul(CMD_ARGS[2], NULL, 0));
        return retVal;
    }

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next single entry ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("Get back next entry: %d\n", (int)arpPointer);
    displayArpData(&arpData);
    return 0;
}

int ReadRegister(void)
{
    MSD_STATUS retVal;
    MSD_U8 devAddr, regAddr;
    MSD_U32 data = 0;
	MSD_U16 temp16 = 0;
	MSD_U32 temp32 = 0;

    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: rr  <devAddr> <regAddr> : Read register\n");
        return 1;
    }

    devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
    regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

	/* Oak/Spruce support extended(bit 16) */
	if ((qddev->devName == MSD_OAK) || (qddev->devName == MSD_SPRUCE))
	{
		retVal = msdGetAnyExtendedReg(sohoDevNum, devAddr, regAddr, &temp32);
		data = temp32;
	}
	else
	{
		retVal = msdGetAnyReg(sohoDevNum, devAddr, regAddr, &temp16);
		data = (MSD_U32)temp16;
	}

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read  dev=%X reg=%X val=%X ret[%d: %s]"
            "\n", devAddr, regAddr, data, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Read - dev: 0x%X, reg: 0x%X, data = 0x%X \n", devAddr, regAddr, data);
    return 0;
}
int WriteRegister(void)
{
    MSD_STATUS retVal;
    MSD_U8 devAddr, regAddr;
    MSD_U16 data;

    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: rw <devAddr> <regAddr> <data> : Write register\n");
        return 1;
    }

    devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
    regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    data = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);

	/* Oak/Spruce support extended(bit 16) */
	if ((qddev->devName == MSD_OAK) || (qddev->devName == MSD_SPRUCE))
	{
		retVal = msdSetAnyExtendedReg(sohoDevNum, devAddr, regAddr, data);
	}
	else
	{
		retVal = msdSetAnyReg(sohoDevNum, devAddr, regAddr, data);
	}

    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Write  dev=%X reg=%X val=%X ret[%d: %s]"
            "\n", devAddr, regAddr, data, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}

int debugLvlSet(void)
{
	MSD_U8 dbgLvl;

	if (nargs != 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: debugLevelSet <Lvl> : Set debug level\n");
		CLI_ERROR("Parameter as follows:\n");
		CLI_ERROR("0x0: MSD_DBG_OFF_LVL\n");
		CLI_ERROR("0x1: MSD_DBG_ERR_LVL\n");
		CLI_ERROR("0x2: MSD_DBG_INF_LVL\n");
		CLI_ERROR("0x3: MSD_DBG_ALL_LVL\n");
		return 1;
	}

	dbgLvl = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);

	msdDbgLvlSet(dbgLvl);

	return 0;
}

/*
int ReadRegisterField(void)
{
MSD_STATUS retVal;
MSD_U8 devAddr, regAddr, offset, size;
MSD_U16 data = 0;

if (nargs != 5)
{
CLI_ERROR("Syntax Error, Using command as follows: rrf <devAddr> <regAddr> <offset> <size> : Read register field\n");
return 1;
}

devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
offset = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
size = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);

retVal = msdGetAnyRegField(sohoDevNum, devAddr, regAddr, offset, size, &data);
if (retVal != MSD_OK) {
if (retVal == MSD_NOT_SUPPORTED)
{
CLI_WARNING("The device maybe not support this feature, please double checked\n");
return MSD_NOT_SUPPORTED;
}
CLI_ERROR("Error Read Filed dev=%X reg=%X offset=%X, size=%X, val=%X ret[%d: %s]"
"\n", devAddr, regAddr, offset, size, data, retVal, reflectMSD_STATUS(retVal));
return MSD_FAIL;
}

CLI_INFO("ReadField - dev: %X, reg: %X, offset: %X, size: %X, data = %X \n", devAddr, regAddr, offset, size, data);
return 0;
}
int WriteRegisterField(void)
{
MSD_STATUS retVal;
MSD_U8 devAddr, regAddr, offset, size;
MSD_U16 data = 0;

if (nargs != 6)
{
CLI_ERROR("Syntax Error, Using command as follows: rwf <devAddr> <regAddr> <offset> <size> <data>: Write register Field\n");
return 1;
}

devAddr = (MSD_U8)strtoul(CMD_ARGS[1], NULL, 0);
regAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
offset = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
size = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);
data = (MSD_U16)strtoul(CMD_ARGS[5], NULL, 0);

retVal = msdSetAnyRegField(sohoDevNum, devAddr, regAddr, offset, size, data);
if (retVal != MSD_OK) {
if (retVal == MSD_NOT_SUPPORTED)
{
CLI_WARNING("The device maybe not support this feature, please double checked\n");
return MSD_NOT_SUPPORTED;
}
CLI_ERROR("Error Read Filed dev=%X reg=%X offset=%X, size=%X, val=%X ret[%d: %s]"
"\n", devAddr, regAddr, offset, size, data, retVal, reflectMSD_STATUS(retVal));
return MSD_FAIL;
}

CLI_INFO("ReadField - dev: %X, reg: %X, offset: %X, size: %X, data = %X \n", devAddr, regAddr, offset, size, data);
return 0;
}


int setBusInterface(void)
{
MSD_STATUS retVal = MSD_OK;
MSD_INTERFACE channel;

if (nargs != 2)
{
CLI_INFO("Syntax Error, Using command as follows: setBus <busInterface> : Set the bus interface, SMI, RMU or SMI multichip\n");
return 1;
}

if (strIcmp(CMD_ARGS[1], "help") == 0)
{
CLI_INFO("setBus <busInterface>\n");
CLI_INFO("	busInterface:\n");
CLI_INFO("		0: MSD_INTERFACE_SMI\n");
CLI_INFO("		1: MSD_INTERFACE_SMI_MULTICHIP\n");
CLI_INFO("		2: MSD_INTERFACE_RMU\n");
}
else
{
channel = (MSD_INTERFACE)strtoul(CMD_ARGS[1], NULL, 0);
retVal = gtSetDriverInterface(sohoDevNum, channel);
if (retVal != MSD_OK) {
CLI_ERROR("Error set dirver bus interface ret[%d: %s]"
"\n", retVal, reflectMSD_STATUS(retVal));
return MSD_FAIL;
}
}

return retVal;
}
*/
int getBusInterface(void)
{
    MSD_STATUS retVal = MSD_OK;
    MSD_INTERFACE channel;
    char *str;

    retVal = msdGetDriverInterface(sohoDevNum, &channel);
    if (retVal != MSD_OK) {
        CLI_ERROR("Error get dirver bus interface ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    switch (channel)
    {
    case MSD_INTERFACE_SMI:
        str = "SMI";
        break;
    case MSD_INTERFACE_SMI_MULTICHIP:
        str = "SMI MultiChip";
        break;
    case MSD_INTERFACE_RMU:
        str = "RMU";
        break;
	case MSD_INTERFACE_FW_RMU:
		str = "FW RMU";
		break;
	case MSD_INTERFACE_VSW:
		str = "VSW";
		break;
    default:
        str = "Unkonwn";
        break;
    }

    CLI_INFO("Current Bus interface is: %s\n", str);
    return retVal;
}

int getVersion(void)
{
    MSD_STATUS retVal = MSD_OK;
    char *MCLIVersion = NULL;
    MSD_VERSION version;

    retVal = msdVersion(&version);
    if (retVal != MSD_OK) {
        CLI_ERROR("Error get UMSD API version ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    MCLIVersion = getMCLIVersion();

    CLI_INFO("UMSD API verson: %s\n", version.version);
    CLI_INFO("UMSD_MCLI version: %s\n", MCLIVersion);

    return retVal;
}

static int updateMemPortsArray(MSD_LPORT* memPorts, int len, int startIndex)
{
    int index = startIndex;
    char *value;
    int i;
    unsigned int tempPortsList[MSD_MAX_SWITCH_PORTS] = { 0 };

    value = CMD_ARGS[index];
    index++;

    if (updateArray(&index, value, len, tempPortsList) != 0)
    {
        return -1;
    }

    for (i = 0; i < len; i++)
    {
        memPorts[i] = tempPortsList[i];
    }
    return 0;
}
int portSetVlanPorts(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_LPORT *memPorts;
    MSD_U8  memPortsLen;
    int i;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memPortsLen = nargs - 3;
    for (i = 3; i < nargs; i++)
    {
        if (strcmp(CMD_ARGS[i], "{") == 0 || strcmp(CMD_ARGS[i], "}") == 0)
        {
			CLI_ERROR("Error input member ports array\n");
			return -1;
        }
		if (strcmp(CMD_ARGS[i], "{}") == 0)
		{
			memPortsLen--;
		}
    }
    memPorts = (MSD_LPORT*)malloc(memPortsLen*sizeof(MSD_LPORT));
    if (memPorts == NULL)
    {
        CLI_ERROR("malloc memery for memPorts error\n");
        return 1;
    }

	if (updateMemPortsArray(memPorts, memPortsLen, 3) != 0)
	{
		free(memPorts);
		return -1;
	}

    retVal = msdPortBasedVlanMapSet(sohoDevNum, port, memPorts, memPortsLen);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            free(memPorts);
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error setPortVlanPorts ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        free(memPorts);
        return MSD_FAIL;
    }

    free(memPorts);
    return retVal;
}
int portGetVlanPorts(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_LPORT memPorts[MSD_MAX_SWITCH_PORTS] = { 0 };
    char tempStr[200] = { 0 };
    MSD_U8  memPortsLen = 0;
    int i, ret = 0;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdPortBasedVlanMapGet(sohoDevNum, port, memPorts, &memPortsLen);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error setPortVlanPorts ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    ret = sprintf(tempStr, "Array of Vlan Ports for port%d: ", port);
    //CLI_INFO("Array of Vlan Ports for port%d: \n", port);
    for (i = 0; i < memPortsLen; i++)
    {
        //CLI_INFO("%d ", memPorts[i]);
        ret = sprintf(tempStr + strlen(tempStr), "%d ", memPorts[i]);
    }
    ret = sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);
    //CLI_INFO("\n");

    return retVal;
}
static int updateCutThrough(MSD_CUT_THROUGH* cutThru, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-enable") == 0)
        {
            cutThru->enable = (MSD_BOOL)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-queue") == 0)
        {
            cutThru->cutThruQueue = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutCutThrough(MSD_LPORT port, MSD_CUT_THROUGH* cutThru)
{
    CLI_INFO("Port %d Cut Through status:\n", port);
    CLI_INFO("  enable status:        %d\n", cutThru->enable);
    CLI_INFO("  Cut through queue:    0x%x\n", cutThru->cutThruQueue);
}
static void printOutOnePortStatus(MSD_LPORT port)
{
    MSD_STATUS retVal;
    MSD_BOOL state;
    MSD_PORT_SPEED speed;
    MSD_PORT_FC_MODE mode;
    int index, ret = 0;
    int strLenList[5] = { 0 };
    char tempStr[100] = { 0 };

    /*"Port    Link State    Speed    Duplex    FlowControl\n"*/
    strLenList[0] = 8;
    strLenList[1] = 22;
    strLenList[2] = 31;
    strLenList[3] = 41;
    strLenList[4] = 52;

    for (index = 0; index < strLenList[4]; index++)
        tempStr[index] = ' ';
    ret = sprintf(tempStr, "%d", port);
    tempStr[strlen(tempStr)] = ' ';
    retVal = msdPortLinkStatusGet(sohoDevNum, port, &state);
    if (retVal != MSD_OK)
	{
        return;
	}
    if (state == MSD_TRUE)
	{
        ret = sprintf(tempStr + strLenList[0], "UP");
	}
    else
	{
        ret = sprintf(tempStr + strLenList[0], "DOWN");
	}
    tempStr[strlen(tempStr)] = ' ';
    retVal = msdPortSpeedStatusGet(sohoDevNum, port, &speed);
    if (retVal != MSD_OK)
	{
        return;
	}
    switch (speed)
    {
    case MSD_PORT_SPEED_10M:
        ret = sprintf(tempStr + strLenList[1], "10M");
        break;
    case MSD_PORT_SPEED_100MB:
        ret = sprintf(tempStr + strLenList[1], "100M");
        break;
    case MSD_PORT_SPEED_1000M:
        ret = sprintf(tempStr + strLenList[1], "1G");
        break;
    default:
        ret = sprintf(tempStr + strLenList[1], "UNKNOW");
        break;
    }
    tempStr[strlen(tempStr)] = ' ';
    retVal = msdPortDuplexStatusGet(sohoDevNum, port, &state);
    if (retVal != MSD_OK)
	{
        return;
	}
    if (state == MSD_TRUE)
	{
        ret = sprintf(tempStr + strLenList[2], "FULL");
	}
    else
	{
        ret = sprintf(tempStr + strLenList[2], "HALF");
	}
    tempStr[strlen(tempStr)] = ' ';
    retVal = msdPortFlowControlEnableGet(sohoDevNum, port, &state, &mode);
    if (retVal != MSD_OK)
	{
        return;
	}
    if (state == MSD_TRUE)
	{
        ret = sprintf(tempStr + strLenList[3], "YES");
	}
    else
	{
        ret = sprintf(tempStr + strLenList[3], "NO");
	}

    ret = sprintf(tempStr + strlen(tempStr), "\n");
    CLI_INFO("%s", tempStr);

    return;
}
int portSetCutThrough(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_CUT_THROUGH cutThru;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&cutThru, 0, sizeof(MSD_CUT_THROUGH));
    if (updateCutThrough(&cutThru, 3) != 0)
        return -1;

    retVal = msdPortCutThroughEnableSet(sohoDevNum, port, &cutThru);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int portGetCutThrough(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_CUT_THROUGH cutThru;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    memset(&cutThru, 0, sizeof(MSD_CUT_THROUGH));

    retVal = msdPortCutThroughEnableGet(sohoDevNum, port, &cutThru);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutCutThrough(port, &cutThru);

    return 0;
}
int portGetPortStatus(void)
{
    MSD_LPORT port;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    
    CLI_INFO("Port    Link State    Speed    Duplex    FlowControl\n");
    if (strIcmp(CMD_ARGS[2], "all") == 0)
    {
        MSD_QD_DEV *dev;        

        dev = sohoDevGet(sohoDevNum);
        for (port = 0; port < dev->numOfPorts; port++)
        {
            printOutOnePortStatus(port);
        }
    }
    else if (strstr(CMD_ARGS[2], "-") != NULL)
    {
        char temp[50] = { 0 };
        char *token;
        MSD_U32 start, end;
		strcpy(temp, CMD_ARGS[2]);
		token = strtok(temp, "-");
		if (token == NULL)
		{
			CLI_ERROR("Get start port error,using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        start = (int)strtoul(token, NULL, 0);
        token = strtok(NULL, ",");
		if (token == NULL)
		{
			CLI_ERROR("Get end port error,using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        end = (int)strtoul(token, NULL, 0);
		if (end >= MSD_MAX_SWITCH_PORTS)
		{
			CLI_ERROR("End port larger than MSD_MAX_SWITCH_PORTS, using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        for (port = start; port <= end; port++)
        {
            printOutOnePortStatus(port);
        }
    }
    else if (strstr(CMD_ARGS[2], ",") != NULL)
    {
        char temp[50] = { 0 };
        char *token;
        strcpy(temp, CMD_ARGS[2]);
        token = strtok(temp, ",");
		if (token == NULL)
		{
			CLI_ERROR("Get port error,using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        port = (MSD_LPORT)strtoul(token, NULL, 0);
        printOutOnePortStatus(port);
        while (token != NULL){
            token = strtok(NULL, ",");
            if (token == NULL)
                break;
            port = (MSD_LPORT)strtoul(token, NULL, 0);
            printOutOnePortStatus(port);
        }
    }
    else
    {
        port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
        printOutOnePortStatus(port);
    }

    return 0;
}

static void displayCounter(MSD_STATS_COUNTER_SET *statsCounter, MSD_BOOL fullbank)
{
    char *cmd = "mib";
    /* Bank 0 */
    if (checkValidItem(cmd, "getPortCounter", "InGoodOctetsLo"))
        CLI_INFO("InGoodOctetsLo       %8lu   \n", statsCounter->InGoodOctetsLo);
    if (checkValidItem(cmd, "getPortCounter", "InGoodOctetsHi"))
        CLI_INFO("InGoodOctetsHi       %8lu   \n", statsCounter->InGoodOctetsHi);
    if (checkValidItem(cmd, "getPortCounter", "InBadOctets"))
        CLI_INFO("InBadOctets          %8lu   \n", statsCounter->InBadOctets);
    if (checkValidItem(cmd, "getPortCounter", "OutFCSErr"))
        CLI_INFO("OutFCSErr            %8lu   \n", statsCounter->OutFCSErr);
    if (checkValidItem(cmd, "getPortCounter", "InUnicasts"))
        CLI_INFO("InUnicasts           %8lu   \n", statsCounter->InUnicasts);
    if (checkValidItem(cmd, "getPortCounter", "Deferred"))
        CLI_INFO("Deferred/OutPFrames  %8lu   \n", statsCounter->Deferred);
    if (checkValidItem(cmd, "getPortCounter", "InBroadcasts"))
        CLI_INFO("InBroadcasts         %8lu   \n", statsCounter->InBroadcasts);
    if (checkValidItem(cmd, "getPortCounter", "InMulticasts"))
        CLI_INFO("InMulticasts         %8lu   \n", statsCounter->InMulticasts);
    /*
    Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
    (refer to Histogram Mode)
    */
    if (checkValidItem(cmd, "getPortCounter", "64Octets"))
        CLI_INFO("Octets64             %8lu   \n", statsCounter->Octets64);
    if (checkValidItem(cmd, "getPortCounter", "127Octets"))
        CLI_INFO("Octets127            %8lu   \n", statsCounter->Octets127);
    if (checkValidItem(cmd, "getPortCounter", "255Octets"))
        CLI_INFO("Octets255            %8lu   \n", statsCounter->Octets255);
    if (checkValidItem(cmd, "getPortCounter", "511Octets"))
        CLI_INFO("Octets511            %8lu   \n", statsCounter->Octets511);
    if (checkValidItem(cmd, "getPortCounter", "1023Octets"))
        CLI_INFO("Octets1023           %8lu   \n", statsCounter->Octets1023);
    if (checkValidItem(cmd, "getPortCounter", "MaxOctets"))
        CLI_INFO("OctetsMax            %8lu   \n", statsCounter->OctetsMax);
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsLo"))
        CLI_INFO("OutOctetsLo          %8lu   \n", statsCounter->OutOctetsLo);
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsHi"))
        CLI_INFO("OutOctetsHi          %8lu   \n", statsCounter->OutOctetsHi);
    if (checkValidItem(cmd, "getPortCounter", "OutUnicasts"))
        CLI_INFO("OutUnicasts          %8lu   \n", statsCounter->OutUnicasts);
    if (checkValidItem(cmd, "getPortCounter", "Excessive"))
        CLI_INFO("Excessive/InPFrames  %8lu   \n", statsCounter->Excessive);
    if (checkValidItem(cmd, "getPortCounter", "OutMulticasts"))
        CLI_INFO("OutMulticasts        %8lu   \n", statsCounter->OutMulticasts);
    if (checkValidItem(cmd, "getPortCounter", "OutBroadcasts"))
        CLI_INFO("OutBroadcasts        %8lu   \n", statsCounter->OutBroadcasts);
    if (checkValidItem(cmd, "getPortCounter", "Single"))
        CLI_INFO("Single/InBadPFrags   %8lu   \n", statsCounter->Single);

    if (checkValidItem(cmd, "getPortCounter", "OutPause"))
        CLI_INFO("OutPause             %8lu   \n", statsCounter->OutPause);
    if (checkValidItem(cmd, "getPortCounter", "InPause"))
        CLI_INFO("InPause              %8lu   \n", statsCounter->InPause);
    if (checkValidItem(cmd, "getPortCounter", "Multiple"))
        CLI_INFO("Multiple/InPFrags    %8lu   \n", statsCounter->Multiple);
    if (checkValidItem(cmd, "getPortCounter", "InUndersize"))
        CLI_INFO("InUndersize          %8lu   \n", statsCounter->InUndersize);
    if (checkValidItem(cmd, "getPortCounter", "InFragments"))
        CLI_INFO("InFragments          %8lu   \n", statsCounter->InFragments);
    if (checkValidItem(cmd, "getPortCounter", "InOversize"))
        CLI_INFO("InOversize           %8lu   \n", statsCounter->InOversize);
    if (checkValidItem(cmd, "getPortCounter", "InJabber"))
        CLI_INFO("InJabber             %8lu   \n", statsCounter->InJabber);
    if (checkValidItem(cmd, "getPortCounter", "InRxErr"))
        CLI_INFO("InRxErr              %8lu   \n", statsCounter->InRxErr);
    if (checkValidItem(cmd, "getPortCounter", "InFCSErr"))
        CLI_INFO("InFCSErr             %8lu   \n", statsCounter->InFCSErr);
    if (checkValidItem(cmd, "getPortCounter", "Collisions"))
        CLI_INFO("Collisions/OutPFrags %8lu   \n", statsCounter->Collisions);
    if (checkValidItem(cmd, "getPortCounter", "Late"))
        CLI_INFO("Late/InPAssemblyErr  %8lu   \n", statsCounter->Late);

    /* Bank 1 */
    if (fullbank == 1)
    {
        if (checkValidItem(cmd, "getPortCounter", "InDiscards"))
            CLI_INFO("InDiscards           %8lu   \n", statsCounter->InDiscards);
        if (checkValidItem(cmd, "getPortCounter", "InFiltered"))
            CLI_INFO("InFiltered           %8lu   \n", statsCounter->InFiltered);
        if (checkValidItem(cmd, "getPortCounter", "InAccepted"))
            CLI_INFO("InAccepted           %8lu   \n", statsCounter->InAccepted);
        if (checkValidItem(cmd, "getPortCounter", "InBadAccepted"))
            CLI_INFO("InBadAccepted        %8lu   \n", statsCounter->InBadAccepted);
        if (checkValidItem(cmd, "getPortCounter", "InGoodAvbClassA"))
            CLI_INFO("InGoodAvbClassA      %8lu   \n", statsCounter->InGoodAvbClassA);
        if (checkValidItem(cmd, "getPortCounter", "InGoodAvbClassB"))
            CLI_INFO("InGoodAvbClassB      %8lu   \n", statsCounter->InGoodAvbClassB);
        if (checkValidItem(cmd, "getPortCounter", "InBadAvbClassA"))
            CLI_INFO("InBadAvbClassA       %8lu   \n", statsCounter->InBadAvbClassA);
        if (checkValidItem(cmd, "getPortCounter", "InBadAvbClassB"))
            CLI_INFO("InBadAvbClassB       %8lu   \n", statsCounter->InBadAvbClassB);
        if (checkValidItem(cmd, "getPortCounter", "TCAMCounter0"))
            CLI_INFO("TCAMCounter0         %8lu   \n", statsCounter->TCAMCounter0);
        if (checkValidItem(cmd, "getPortCounter", "TCAMCounter1"))
            CLI_INFO("TCAMCounter1         %8lu   \n", statsCounter->TCAMCounter1);
        if (checkValidItem(cmd, "getPortCounter", "TCAMCounter2"))
            CLI_INFO("TCAMCounter2         %8lu   \n", statsCounter->TCAMCounter2);
        if (checkValidItem(cmd, "getPortCounter", "TCAMCounter3"))
            CLI_INFO("TCAMCounter3         %8lu   \n", statsCounter->TCAMCounter3);
        if (checkValidItem(cmd, "getPortCounter", "InDroppedAvbA"))
            CLI_INFO("InDroppedAvbA        %8lu   \n", statsCounter->InDroppedAvbA);
        if (checkValidItem(cmd, "getPortCounter", "InDroppedAvbB"))
            CLI_INFO("InDroppedAvbB        %8lu   \n", statsCounter->InDroppedAvbB);
        if (checkValidItem(cmd, "getPortCounter", "InDaUnknown"))
            CLI_INFO("InDaUnknown          %8lu   \n", statsCounter->InDaUnknown);
        if (checkValidItem(cmd, "getPortCounter", "InMGMT"))
            CLI_INFO("InMGMT               %8lu   \n", statsCounter->InMGMT);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue0"))
            CLI_INFO("OutQueue0            %8lu   \n", statsCounter->OutQueue0);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue1"))
            CLI_INFO("OutQueue1            %8lu   \n", statsCounter->OutQueue1);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue2"))
            CLI_INFO("OutQueue2            %8lu   \n", statsCounter->OutQueue2);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue3"))
            CLI_INFO("OutQueue3            %8lu   \n", statsCounter->OutQueue3);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue4"))
            CLI_INFO("OutQueue4            %8lu   \n", statsCounter->OutQueue4);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue5"))
            CLI_INFO("OutQueue5            %8lu   \n", statsCounter->OutQueue5);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue6"))
            CLI_INFO("OutQueue6            %8lu   \n", statsCounter->OutQueue6);
        if (checkValidItem(cmd, "getPortCounter", "OutQueue7"))
            CLI_INFO("OutQueue7            %8lu   \n", statsCounter->OutQueue7);
        if (checkValidItem(cmd, "getPortCounter", "OutCutThrough"))
            CLI_INFO("OutCutThrough        %8lu   \n", statsCounter->OutCutThrough);
        if (checkValidItem(cmd, "getPortCounter", "InBadQbv"))
            CLI_INFO("InBadQbv             %8lu   \n", statsCounter->InBadQbv);
        if (checkValidItem(cmd, "getPortCounter", "OutOctetsA"))
            CLI_INFO("OutOctetsA           %8lu   \n", statsCounter->OutOctetsA);
        if (checkValidItem(cmd, "getPortCounter", "OutOctetsB"))
            CLI_INFO("OutOctetsB           %8lu   \n", statsCounter->OutOctetsB);
        if (checkValidItem(cmd, "getPortCounter", "OutYel"))
            CLI_INFO("OutYellow            %8lu   \n", statsCounter->OutYel);
        if (checkValidItem(cmd, "getPortCounter", "OutDroppedYel"))
            CLI_INFO("OutDroppedYel        %8lu   \n", statsCounter->OutDroppedYel);
        if (checkValidItem(cmd, "getPortCounter", "OutDiscards"))
            CLI_INFO("OutDiscards          %8lu   \n", statsCounter->OutDiscards);
        if (checkValidItem(cmd, "getPortCounter", "OutMGMT"))
            CLI_INFO("OutMGMT              %8lu   \n", statsCounter->OutMGMT);
		if (checkValidItem(cmd, "getPortCounter", "DropEvents"))
			CLI_INFO("DropEvents           %8lu   \n", statsCounter->DropEvents);
        if (checkValidItem(cmd, "getPortCounter", "AtsFiltered"))
            CLI_INFO("AtsFiltered          %8lu   \n", statsCounter->AtsFiltered);
    }
}

static int getPortCounters(MSD_LPORT port, MSD_STATS_COUNTER_SET *stats)
{
    MSD_STATUS retVal;

    retVal = msdStatsPortAllCountersGet(sohoDevNum, port, stats);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}

static void displayDumpCounters(int portList[], MSD_STATS_COUNTER_SET *statsCounterList[])
{
    char *cmd = "mib";
    MSD_LPORT port;
    int index = 0;
	int ret = 0;

    char tempStr[500];
	for (index = 0; index < 500; index++)
	{
		tempStr[index] = ' ';
	}
    ret = sprintf(tempStr, "CounterName");
    tempStr[strlen(tempStr)] = ' ';
    index = 0;
    for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
    {
        if (portList[port] >= 0)
        {
            ret = sprintf(tempStr + 19 + index * 10, "port%d", port);
            tempStr[strlen(tempStr)] = ' ';
            index++;
        }
    }
    ret = sprintf(tempStr + 19 + index * 10, "\n\0");
    CLI_INFO("%s", tempStr);

    /* Bank 0 */
    if (checkValidItem(cmd, "getPortCounter", "InGoodOctetsLo"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InGoodOctetsLo");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InGoodOctetsLo);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InGoodOctetsHi"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InGoodOctetsHi");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InGoodOctetsHi);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBadOctets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InBadOctets");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBadOctets);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutFCSErr"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutFCSErr");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutFCSErr);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InUnicasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InUnicasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InUnicasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Deferred"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Deferred");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Deferred);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBroadcasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InBroadcasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBroadcasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InMulticasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InMulticasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InMulticasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    /*
    Histogram Counters : Rx Only, Tx Only, or both Rx and Tx
    (refer to Histogram Mode)
    */
    if (checkValidItem(cmd, "getPortCounter", "64Octets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Octets64");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Octets64);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "127Octets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Octets127");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Octets127);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "255Octets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Octets255");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Octets255);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "511Octets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Octets511");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Octets511);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "1023Octets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Octets1023");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Octets1023);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "MaxOctets"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OctetsMax");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OctetsMax);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsLo"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutOctetsLo");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutOctetsLo);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsHi"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutOctetsHi");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutOctetsHi);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutUnicasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutUnicasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutUnicasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Excessive"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Excessive");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Excessive);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutMulticasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutMulticasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutMulticasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutBroadcasts"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutBroadcasts");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutBroadcasts);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Single"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Single");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Single);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }

    if (checkValidItem(cmd, "getPortCounter", "OutPause"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutPause");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutPause);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InPause"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InPause");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InPause);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Multiple"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Multiple");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Multiple);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InUndersize"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InUndersize");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InUndersize);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InFragments"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InFragments");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InFragments);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InOversize"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InOversize");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InOversize);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InJabber"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InJabber");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InJabber);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InRxErr"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InRxErr");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InRxErr);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InFCSErr"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InFCSErr");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InFCSErr);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Collisions"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Collisions");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Collisions);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "Late"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "Late");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->Late);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }

    /* Bank 1 */
    if (checkValidItem(cmd, "getPortCounter", "InDiscards"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InDiscards");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InDiscards);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InFiltered"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InFiltered");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InFiltered);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InAccepted"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InAccepted");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InAccepted);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBadAccepted"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InBadAccepted");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBadAccepted);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InGoodAvbClassA"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InGoodAvbClassA");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InGoodAvbClassA);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InGoodAvbClassB"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InGoodAvbClassB");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InGoodAvbClassB);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBadAvbClassA"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InBadAvbClassA");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBadAvbClassA);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBadAvbClassB"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InBadAvbClassB");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBadAvbClassB);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "TCAMCounter0"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "TCAMCounter0");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->TCAMCounter0);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "TCAMCounter1"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "TCAMCounter1");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->TCAMCounter1);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "TCAMCounter2"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "TCAMCounter2");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->TCAMCounter2);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "TCAMCounter3"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "TCAMCounter3");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->TCAMCounter3);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InDroppedAvbA"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InDroppedAvbA");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InDroppedAvbA);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InDroppedAvbB"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InDroppedAvbB");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InDroppedAvbB);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InDaUnknown"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InDaUnknown");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InDaUnknown);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InMGMT"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "InMGMT");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InMGMT);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue0"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue0");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue0);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue1"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue1");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue1);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue2"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue2");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue2);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue3"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue3");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue3);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue4"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue4");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue4);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue5"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue5");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue5);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue6"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue6");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue6);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutQueue7"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutQueue7");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutQueue7);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutCutThrough"))
    {
        for (index = 0; index < 500; index++)
	    {
		   tempStr[index] = ' ';
	    }
        ret = sprintf(tempStr, "OutCutThrough");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutCutThrough);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "InBadQbv"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "InBadQbv");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->InBadQbv);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsA"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutOctetsA");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutOctetsA);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutOctetsB"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutOctetsB");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutOctetsB);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutYel"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutYel");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutYel);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutDroppedYel"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutDroppedYel");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutDroppedYel);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutDiscards"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutDiscards");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutDiscards);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
    if (checkValidItem(cmd, "getPortCounter", "OutMGMT"))
    {
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
        ret = sprintf(tempStr, "OutMGMT");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + index * 10, "%8lu", statsCounterList[port]->OutMGMT);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
	if (checkValidItem(cmd, "getPortCounter", "DropEvents"))
	{
		for (index = 0; index < 500; index++)
		{
			tempStr[index] = ' ';
		}
		ret = sprintf(tempStr, "DropEvents");
		tempStr[strlen(tempStr)] = ' ';
		int index = 0;
		for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
		{
			if (portList[port] >= 0)
			{
				ret = sprintf(tempStr + 16 + (index * 10), "%8lu", statsCounterList[port]->DropEvents);
				tempStr[strlen(tempStr)] = ' ';
				index++;
			}
		}
		ret = sprintf(tempStr + 16 + index * 10, "\n\0");
		CLI_INFO("%s", tempStr);
	}
    if (checkValidItem(cmd, "getPortCounter", "AtsFiltered"))
    {
        for (index = 0; index < 500; index++)
        {
            tempStr[index] = ' ';
        }
        ret = sprintf(tempStr, "AtsFiltered");
        tempStr[strlen(tempStr)] = ' ';
        int index = 0;
        for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
        {
            if (portList[port] >= 0)
            {
                ret = sprintf(tempStr + 16 + (index * 10), "%8lu", statsCounterList[port]->AtsFiltered);
                tempStr[strlen(tempStr)] = ' ';
                index++;
            }
        }
        ret = sprintf(tempStr + 16 + index * 10, "\n\0");
        CLI_INFO("%s", tempStr);
    }
}
int mibFlushAll(void)
{
    int index = 0;
    MSD_STATUS retVal;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdStatsFlushAll(sohoDevNum);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int mibFlushPort(void)
{
    int index = 0;
    MSD_STATUS retVal;
    MSD_LPORT port;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdStatsFlushPort(sohoDevNum, port);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int mibGetPortCounter(void)
{
    int index = 0;
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_STATS_COUNTERS counter;
    MSD_U32 statsData;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    counter = (MSD_STATS_COUNTERS)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdStatsPortCounterGet(sohoDevNum, port, counter, &statsData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("StatsData : 0x%lX\n", statsData);
    return 0;
}
int mibGetPortAllCounters(void)
{
    int index = 0;
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_STATS_COUNTER_SET    statsCounterSet;
    memset(&statsCounterSet, 0, sizeof(statsCounterSet));

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdStatsPortAllCountersGet(sohoDevNum, port, &statsCounterSet);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayCounter(&statsCounterSet, MSD_TRUE);
    return 0;
}
int mibDump(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_STATS_COUNTER_SET* statsCounterSet[MSD_MAX_SWITCH_PORTS] = { 0 };
    int portList[MSD_MAX_SWITCH_PORTS];

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
    {
        portList[port] = -1;
        //memset(&(statsCounterSet[port]), 0, sizeof(statsCounterSet));
    }

    if (strIcmp(CMD_ARGS[2], "all") == 0)
    {
        MSD_QD_DEV *dev;

        dev = sohoDevGet(sohoDevNum);
        for (port = 0; port < dev->numOfPorts; port++)
        {
            portList[port] = port;
            MSD_STATS_COUNTER_SET *statsCounter = (MSD_STATS_COUNTER_SET*)malloc(sizeof(MSD_STATS_COUNTER_SET));
            retVal = getPortCounters(port, statsCounter);
            if (retVal != MSD_OK) 
                return MSD_FAIL;
            statsCounterSet[port] = statsCounter;
        }
    }
    else if (strstr(CMD_ARGS[2], "-") != NULL)
    {
        char temp[50] = { 0 };
        char *token;
        MSD_U32 start, end;
        strcpy(temp,  CMD_ARGS[2]);
        token = strtok(temp, "-");
		if (token == NULL)
		{
			CLI_ERROR("Get start port error,using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        start = (int)strtoul(token, NULL, 0);
        token = strtok(NULL, ",");
		if (token == NULL)
		{
			CLI_ERROR("Get end port error,using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        end = (int)strtoul(token, NULL, 0);
        for (port = start; port <= end; port++)
        {
            portList[port] = port;
            MSD_STATS_COUNTER_SET *statsCounter = (MSD_STATS_COUNTER_SET*)malloc(sizeof(MSD_STATS_COUNTER_SET));
            retVal = getPortCounters(port, statsCounter);
            if (retVal != MSD_OK)
                return MSD_FAIL;
            statsCounterSet[port] = statsCounter;
        }
    }
    else if (strstr(CMD_ARGS[2], ",") != NULL)
    {
        char temp[50] = { 0 };
        char *token;
		strcpy(temp, CMD_ARGS[2]);
        token = strtok(temp, ",");
		if (token == NULL)
		{
			CLI_ERROR("Get port error,using command args as follows: %s", CMD_ARGS[2]);
			return 1;
		}
        port = (MSD_LPORT)strtoul(token, NULL, 0);
		if (port >= MSD_MAX_SWITCH_PORTS)
		{
			CLI_ERROR("Port larger than MSD_MAX_SWITCH_PORTS, using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        portList[port] = port;
        MSD_STATS_COUNTER_SET *statsCounter = (MSD_STATS_COUNTER_SET*)malloc(sizeof(MSD_STATS_COUNTER_SET));
        retVal = getPortCounters(port, statsCounter);
        if (retVal != MSD_OK)
            return MSD_FAIL;
        statsCounterSet[port] = statsCounter;
        while (token != NULL){
            token = strtok(NULL, ",");
            if (token == NULL)
                break;
            port = (MSD_LPORT)strtoul(token, NULL, 0);
			if (port >= MSD_MAX_SWITCH_PORTS)
			{
				CLI_ERROR("Port larger than MSD_MAX_SWITCH_PORTS, using command args as follows: %s", CMD_ARGS[2]);
				return MSD_FAIL;
			}
            portList[port] = port;
            MSD_STATS_COUNTER_SET *statsCounter = (MSD_STATS_COUNTER_SET*)malloc(sizeof(MSD_STATS_COUNTER_SET));
            retVal = getPortCounters(port, statsCounter);
            if (retVal != MSD_OK)
                return MSD_FAIL;
            statsCounterSet[port] = statsCounter;
        }
    }
    else
    {
        port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);
		if (port >= MSD_MAX_SWITCH_PORTS)
		{
			CLI_ERROR("Port larger than MSD_MAX_SWITCH_PORTS, using command args as follows: %s", CMD_ARGS[2]);
			return MSD_FAIL;
		}
        portList[port] = port;
        MSD_STATS_COUNTER_SET *statsCounter = (MSD_STATS_COUNTER_SET*)malloc(sizeof(MSD_STATS_COUNTER_SET));
        retVal = getPortCounters(port, statsCounter);
        if (retVal != MSD_OK)
            return MSD_FAIL;
        statsCounterSet[port] = statsCounter;
    }

    displayDumpCounters(portList, statsCounterSet);

    for (port = 0; port < MSD_MAX_SWITCH_PORTS; port++)
    {
        if (statsCounterSet[port] != NULL)
            free(statsCounterSet[port]);
    }

    return 0;
}
static int updateQosWeight(MSD_QoS_WEIGHT* weight, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-len") == 0)
        {
            weight->len = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-queue") == 0)
        {
            int i;
            unsigned int tempList[MSD_MAX_QOS_WEIGHTS] = { 0 };
            if (updateArray(&index, value, MSD_MAX_QOS_WEIGHTS, tempList) != 0)
            {
                return -1;
            }
            for (i = 0; i < MSD_MAX_QOS_WEIGHTS; i++)
            {
                weight->queue[i] = (MSD_U8)tempList[i];
            }
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }

    return 0;
}
static void printOutWeight(MSD_QoS_WEIGHT* weight)
{
    int i, ret = 0;
    char tempStr[500] = { 0 };
    char *cmd = "qc";

    CLI_INFO("Qos Weight Info:\n");
    if (checkValidItem(cmd, "setQosWeight", "-len"))
        CLI_INFO("	len: %X\n", weight->len);
    //CLI_INFO("	queue:	");
    if (checkValidItem(cmd, "setQosWeight", "-queue"))
    {
        ret = sprintf(tempStr, "	queue:	");
        for (i = 0; i < (int)weight->len; i++)
        {
            //CLI_INFO("%X ", weight->queue[i]);
            ret = sprintf(tempStr + strlen(tempStr), "%X ", weight->queue[i]);
        }
        //CLI_INFO("\n");
        ret = sprintf(tempStr + strlen(tempStr), "\n");
        CLI_INFO("%s", tempStr);
    }
}
static void printOutPortWeight(MSD_QoS_WEIGHT* weight)
{
    int i, ret = 0;
    char tempStr[500] = { 0 };
    char* cmd = "qc";

    CLI_INFO("Port Qos Weight Info:\n");
    if (checkValidItem(cmd, "setPortQosWeight", "-len"))
        CLI_INFO("	len: %X\n", weight->len);
    //CLI_INFO("	queue:	");
    if (checkValidItem(cmd, "setPortQosWeight", "-queue"))
    {
        ret = sprintf(tempStr, "	queue:	");
        for (i = 0; i < (int)weight->len; i++)
        {
            //CLI_INFO("%X ", weight->queue[i]);
            ret = sprintf(tempStr + strlen(tempStr), "%X ", weight->queue[i]);
        }
        //CLI_INFO("\n");
        ret = sprintf(tempStr + strlen(tempStr), "\n");
        CLI_INFO("%s", tempStr);
    }
}

int qcSetQosWeight(void)
{
    MSD_STATUS retVal;
    MSD_QoS_WEIGHT weight;

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));
    if (updateQosWeight(&weight, 2) != 0)
        return-1;

    retVal = msdSysQoSWeightSet(sohoDevNum, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int qcGetQosWeight(void)
{
    MSD_STATUS retVal;
    MSD_QoS_WEIGHT weight;

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));

    retVal = msdSysQoSWeightGet(sohoDevNum, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutWeight(&weight);

    return retVal;
}

int qcSetPortQosWeight(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_QoS_WEIGHT weight;

    if (nargs < 3)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));
    if (updateQosWeight(&weight, 3) != 0)
        return-1;

    retVal = msdPortQoSWeightSet(sohoDevNum, port, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return retVal;
}
int qcGetPortQosWeight(void)
{
    MSD_STATUS retVal;
    MSD_LPORT port;
    MSD_QoS_WEIGHT weight;

    if (nargs < 3)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    port = (MSD_LPORT)strtoul(CMD_ARGS[2], NULL, 0);

    memset(&weight, 0, sizeof(MSD_QoS_WEIGHT));

    retVal = msdPortQoSWeightGet(sohoDevNum, port, &weight);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    printOutPortWeight(&weight);

    return retVal;
}


static MSD_STATUS checkRMUInterface(void)
{
    MSD_STATUS retVal;
    MSD_INTERFACE channel;

    retVal = msdGetDriverInterface(sohoDevNum, &channel);
    if (retVal != MSD_OK) {
        CLI_ERROR("Error get dirver bus interface ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    if (channel != MSD_INTERFACE_RMU)
        return MSD_FAIL;

    return retVal;
}
int rmuGetID(void)
{
    MSD_STATUS retVal;
    MSD_U16 id;

    if (nargs != 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    retVal = msdRMUGetID(sohoDevNum, &id);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get the devcie ID ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("Get out Device ID: %X\n", id);
    return 0;
}
int rmuDumpAtu(void)
{
    MSD_STATUS retVal;
    MSD_U32 startAddr;
    int i;
    MSD_U32 numOfEntry = 48;
    MSD_ATU_ENTRY atuEntry[48];
    MSD_ATU_ENTRY *pAtuEntry = &(atuEntry[0]);

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(pAtuEntry, 0, sizeof(MSD_ATU_ENTRY)*numOfEntry);
    startAddr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMUAtuDump(sohoDevNum, &startAddr, &numOfEntry, &pAtuEntry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump ATU ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    for (i = 0; i < (int)numOfEntry; i++)
    {
        printOutFdbEntry(pAtuEntry + i);
    }
    CLI_INFO("There are %d entries found in this searching\n", numOfEntry);
    CLI_INFO("ATU searching continue code: 0x%lX\n", startAddr);
    return 0;
}
int rmuDumpEcid(void)
{
    MSD_STATUS retVal;
    MSD_U32 startAddr;
    int i;
    MSD_U32 numOfEntry = 48;
    MSD_ECID_ENTRY ecidEntry[48];
    MSD_ECID_ENTRY *pEcidEntry = &(ecidEntry[0]);

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(pEcidEntry, 0, sizeof(MSD_ECID_ENTRY)*numOfEntry);
    startAddr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMUEcidDump(sohoDevNum, &startAddr, &numOfEntry, &pEcidEntry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump ATU ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    for (i = 0; i < (int)numOfEntry; i++)
    {
        printOutEcidEntry(pEcidEntry + i);
    }
    CLI_INFO("There are %d entries found in this searching\n", numOfEntry);
    CLI_INFO("ECID searching continue code: 0x%lX\n", startAddr);
    return 0;
}
int rmuDumpMib(void)
{
    MSD_STATUS retVal;
    MSD_BOOL enClearOnRead;
    MSD_LPORT port;
    MSD_STATS_COUNTER_SET_LEGACY statsCounterSet;

    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error: rmu dumpMib <enClearOnRead> <port> : Dump MIB from the specified port\n");
        return 1;
    }

    memset(&statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET_LEGACY));
    enClearOnRead = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdRMUMibDump(sohoDevNum, enClearOnRead, port, &statsCounterSet);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump MIB ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayCounter((MSD_STATS_COUNTER_SET*)(&statsCounterSet), MSD_FALSE);

    return 0;
}
int rmuDumpMib2(void)
{
    MSD_STATUS retVal;
    MSD_BOOL enClearOnRead;
    MSD_LPORT port;
    MSD_STATS_COUNTER_SET statsCounterSet;

    if (nargs != 4)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(&statsCounterSet, 0, sizeof(MSD_STATS_COUNTER_SET));
    enClearOnRead = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
    port = (MSD_LPORT)strtoul(CMD_ARGS[3], NULL, 0);

    retVal = msdRMUMib2Dump(sohoDevNum, enClearOnRead, port, &statsCounterSet);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump MIB ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayCounter(&statsCounterSet, MSD_TRUE);

    return 0;
}
int rmuDumpReg(void)
{
    MSD_STATUS retVal;
    MSD_U8 devAddr;
    MSD_RegList dataList;
    int i;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(&dataList, 0, sizeof(MSD_RegList));
    devAddr = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMURegisterDump(sohoDevNum, devAddr, &dataList);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump Tcam ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    CLI_INFO("registers value for devAddr%d:\n", devAddr);
    for (i = 0; i < 32; i++)
    {
        CLI_INFO("reg%02d = 0x%04x\n", i, dataList.data[i]);
    }

    return 0;
}
int rmuDumpTcam(void)
{
    MSD_STATUS retVal;
    MSD_U32 startAddr;
    int i;
    MSD_U32 numOfEntry = 3;
    MSD_TCAM_DATA tcamEntry[3];
    MSD_TCAM_DATA *pTcamEntry = &(tcamEntry[0]);

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(pTcamEntry, 0, sizeof(MSD_TCAM_DATA)*numOfEntry);
    startAddr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMUTcamDump(sohoDevNum, &startAddr, &numOfEntry, &pTcamEntry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error dump Tcam ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    for (i = 0; i < (int)numOfEntry; i++)
    {
        displayTcamData(pTcamEntry + i);
    }
    CLI_INFO("There are %d entries found in this searching\n", numOfEntry);
    CLI_INFO("TCAM searching continue code: 0x%lX\n", startAddr);
    return 0;
}
int rmuTcamRead(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;

    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdRMUTcamEntryRead(sohoDevNum, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displayTcamData(&tcamData);
    return 0;
}
int rmuTcamWrite(void)
{
    MSD_STATUS retVal;
    MSD_U32 tcamPointer;
    MSD_TCAM_DATA tcamData;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (checkRMUInterface() != MSD_OK)
    {
        CLI_ERROR("This method only support RMU interface\n");
        return 1;
    }

    memset(&tcamData, 0, sizeof(MSD_TCAM_DATA));
    tcamPointer = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	if (updateTcamData(&tcamData, 3) != 0)
	{
		return -1;
	}

    retVal = msdRMUTcamEntryAdd(sohoDevNum, tcamPointer, &tcamData);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)tcamPointer, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    return 0;
}


int qcrSetGrpId(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId, grpId;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    grpId = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdQcrSetGrpId(sohoDevNum, entryId, grpId);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int qcrGetGrpId(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId, grpId;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdQcrGetGrpId(sohoDevNum, entryId, &grpId);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("QCR Entry %d: GroupId = 0x%X \n", entryId, grpId);
    return MSD_OK;
}
int qcrSetCbsLimit(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U32 cbsLimit;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    cbsLimit = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdQcrSetCbsLimit(sohoDevNum, entryId, cbsLimit);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int qcrGetCbsLimit(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U32 cbsLimit;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdQcrGetCbsLimit(sohoDevNum, entryId, &cbsLimit);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("QCR Entry %d: CBSLimit = 0x%X \n", entryId, cbsLimit);
    return MSD_OK;
}
int qcrSetRateLimit(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U32 rateLimit;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    rateLimit = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdQcrSetRateLimit(sohoDevNum, entryId, rateLimit);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int qcrGetRateLimit(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U32 rateLimit;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdQcrGetRateLimit(sohoDevNum, entryId, &rateLimit);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("QCR Entry %d: Rate Limit = 0x%X \n", entryId, rateLimit);
    return MSD_OK;
}
int qcrSetMaxResidenceTime(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U16 maxResidenceTime;
    MSD_U16 maxTimeScale;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 5)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    maxTimeScale = (MSD_U16)strtoul(CMD_ARGS[3], NULL, 0);
    maxResidenceTime = (MSD_U16)strtoul(CMD_ARGS[4], NULL, 0);
    retVal = msdQcrSetMaxResidenceTime(sohoDevNum, entryId, maxTimeScale, maxResidenceTime);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error set qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int qcrGetMaxResidenceTime(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;
    MSD_U16 maxResidenceTime;
    MSD_U16 maxTimeScale;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdQcrGetMaxResidenceTime(sohoDevNum, entryId, &maxTimeScale, &maxResidenceTime);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get qcr entryNum=%d ret[%d: %s]"
            "\n", (int)entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("QCR Entry %d: maxTimeScale= 0x%X , maxResidenceTime = 0x%X \n", maxTimeScale, maxResidenceTime);
    return MSD_OK;
}
int qcrFlushAll(void)
{
    MSD_STATUS retVal;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 2)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdQcrFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush QCR all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int qcrFlushOne(void)
{
    MSD_STATUS retVal;
    MSD_U16 entryId;

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    entryId = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdQcrFlushOne(sohoDevNum, entryId);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush QCR entry %d, ret[%d: %s]"
            "\n", entryId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}

static int updateIndvRcvyData(MSD_FRER_INDVRCVY *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs;)
    {
        cmd = CMD_ARGS[index];
        index++;

        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
            index++;
        }

        if (strIcmp(cmd, "-indvRcvyEn") == 0)
        {
            entry->indvRcvyEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-indvRcvyAlg") == 0)
        {
            entry->indvRcvyAlg = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-indvRcvyTakeNoSeq") == 0)
        {
            entry->indvRcvyTakeNoSeq = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-indvRcvySeqHisLen") == 0)
        {
            entry->indvRcvySeqHisLen = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-indvRcvyRstRate") == 0)
        {
            entry->indvRcvyRstRate = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-indvRcvyRstTime") == 0)
        {
            entry->indvRcvyRstTime = (MSD_U16)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static void displayIndvRcvyData(MSD_FRER_INDVRCVY *entry, MSD_U16 indvRcvyResets)
{
    char* cmd = "frer";
    CLI_INFO("----- Indv Rcvy Data -----\n");
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyId")) 
        CLI_INFO("indvRcvyId            : %04x\n", entry->indvRcvyId);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyEn"))
        CLI_INFO("indvRcvyEn            : %04x\n", entry->indvRcvyEn);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyAlg"))
        CLI_INFO("indvRcvyAlg           : %04x\n", entry->indvRcvyAlg);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyTakeNoSeq"))
        CLI_INFO("indvRcvyTakeNoSeq     : %04x\n", entry->indvRcvyTakeNoSeq);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvySeqHisLen"))
        CLI_INFO("indvRcvySeqHisLen     : %04x\n", entry->indvRcvySeqHisLen);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyRstRate"))
        CLI_INFO("indvRcvyRstRate       : %04x\n", entry->indvRcvyRstRate);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyRstTime"))
        CLI_INFO("indvRcvyRstTime       : %04x\n", entry->indvRcvyRstTime);
    if (checkValidItem(cmd, "indvRcvyReadEntry", "indvRcvyResets"))
        CLI_INFO("indvRcvyResets        : %04x\n", indvRcvyResets);   
}

int frerSeqGenFlushAll(void)
{
    MSD_STATUS retVal;
 
    retVal = msdFrerSeqGenFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Seq Gen all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerSeqGenFlushOne(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQGEN entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_SEQGEN));
    entry.seqGenId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerSeqGenFlushEntry(sohoDevNum, entry.seqGenId);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqGenId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerSeqGenLoad(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQGEN entry;
 
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_SEQGEN));
    entry.seqGenId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    if (nargs == 5)
    {
        if (strIcmp(CMD_ARGS[3], "-genseqnum") == 0)
        {
            entry.genSeqNum = (MSD_U16)strtoul(CMD_ARGS[4], NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", CMD_ARGS[3]);
            return -1;
        }
    }
    else
    {
        entry.genSeqNum = 0;
    }
    retVal = msdFrerSeqGenLoad(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqGenId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerSeqGenRead(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQGEN entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_SEQGEN));
    entry.seqGenId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
   
    retVal = msdFrerSeqGenRead(sohoDevNum, entry.seqGenId, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqGenId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    CLI_INFO("FRER SeqGen ReadEntry %d: GenSeqNum = 0x%X \n", entry.seqGenId, entry.genSeqNum);
    return 0;
}

int frerIndvRcvyFlushAll(void)
{
    MSD_STATUS retVal;

    retVal = msdFrerIndvRcvyFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Indv Rcvy all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerIndvRcvyFlushOne(void)
{
    MSD_STATUS retVal;
    MSD_FRER_INDVRCVY entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_INDVRCVY));
    entry.indvRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerIndvRcvyFlushEntry(sohoDevNum, entry.indvRcvyId);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Indv Rcvy entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.indvRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerIndvRcvyLoad(void)
{
    MSD_STATUS retVal;
    MSD_FRER_INDVRCVY entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_INDVRCVY));
    entry.indvRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    if (updateIndvRcvyData(&entry, 3) != 0)
        return -1;
    retVal = msdFrerIndvRcvyLoad(sohoDevNum, &entry);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.indvRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return 0;
}
int frerIndvRcvyRead(void)
{
    MSD_STATUS retVal;
    MSD_FRER_INDVRCVY entry;
    MSD_U16 indvRcvyResets = 0;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_INDVRCVY));
    entry.indvRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerIndvRcvyRead(sohoDevNum, entry.indvRcvyId, &entry, &indvRcvyResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.indvRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayIndvRcvyData(&entry, indvRcvyResets);
    return 0;
}
int frerIndvRcvyGetNext(void)
{
    MSD_STATUS retVal;
    MSD_FRER_INDVRCVY entry;
    MSD_U16 indvRcvyResets = 0;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_INDVRCVY));
    entry.indvRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerIndvRcvyGetNext(sohoDevNum, entry.indvRcvyId, &entry, &indvRcvyResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Next single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.indvRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayIndvRcvyData(&entry, indvRcvyResets);
    return 0;
}
int frerIndvRcvyReadClear(void)
{
    MSD_STATUS retVal;
    MSD_FRER_INDVRCVY entry;
    MSD_U16 indvRcvyResets = 0;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_INDVRCVY));
    entry.indvRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerIndvRcvyReadClear(sohoDevNum, entry.indvRcvyId, &entry, &indvRcvyResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read Clear single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.indvRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayIndvRcvyData(&entry, indvRcvyResets);
    return 0;
}


static int updateSeqRcvyData(MSD_FRER_SEQRCVY *entry, int startIndex)
{
    int index;
    char* cmd, *value;
    for (index = startIndex; index < nargs; index++)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
        }
        if (strIcmp(cmd, "-seqRcvyEn") == 0)
        {
            entry->seqRcvyEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyPort") == 0)
        {
            entry->seqRcvyPort = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyIndex") == 0)
        {
            entry->seqRcvyIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyAlg") == 0)
        {
            entry->seqRcvyAlg = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyTakeNoseq") == 0)
        {
            entry->seqRcvyTakeNoSeq = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrIntEn") == 0)
        {
            entry->latErrIntEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrStatus") == 0)
        {
            entry->latErrStatus = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvySeqHisLen") == 0)
        {
            entry->seqRcvySeqHisLen = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyRstRate") == 0)
        {
            entry->seqRcvyRstRate = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-seqRcvyRstTime") == 0)
        {
            entry->seqRcvyRstTime = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrEn") == 0)
        {
            entry->latErrEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-rdantPaths") == 0)
        {
            entry->rdantPaths = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrDiff") == 0)
        {
            entry->latErrDiff = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrTestRate") == 0)
        {
            entry->latErrTestRate = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrTestTime") == 0)
        {
            entry->latErrTestTime = (MSD_U16)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrRstRate") == 0)
        {
            entry->latErrRstRate = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-latErrRstTime") == 0)
        {
            entry->latErrRstTime = (MSD_U16)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    return 0;
}
static int displaySeqRcvyData(MSD_FRER_SEQRCVY *entry, MSD_U16 seqRcvyResets, MSD_U16 latErrResets)
{
    char* cmd = "frer";
    CLI_INFO("----- FRER SeqRcvy -----\n");
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyId"))
        CLI_INFO("seqRcvyId             : %04x\n", entry->seqRcvyId);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyEn"))
        CLI_INFO("seqRcvyEn             : %04x\n", entry->seqRcvyEn);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyPort"))
        CLI_INFO("seqRcvyPort           : %04x\n", entry->seqRcvyPort);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyIndex"))
        CLI_INFO("seqRcvyIndex          : %04x\n", entry->seqRcvyIndex);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyAlg"))
        CLI_INFO("seqRcvyAlg            : %04x\n", entry->seqRcvyAlg);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyTakeNoSeq"))
        CLI_INFO("seqRcvyTakeNoSeq      : %04x\n", entry->seqRcvyTakeNoSeq);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrIntEn"))
        CLI_INFO("latErrIntEn           : %04x\n", entry->latErrIntEn);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrStatus"))
        CLI_INFO("latErrStatus          : %04x\n", entry->latErrStatus);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvySeqHisLen"))
        CLI_INFO("seqRcvySeqHisLen      : %04x\n", entry->seqRcvySeqHisLen);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyRstRate"))
        CLI_INFO("seqRcvyRstRate        : %04x\n", entry->seqRcvyRstRate);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyRstTime"))
        CLI_INFO("seqRcvyRstTime        : %04x\n", entry->seqRcvyRstTime);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrEn"))
        CLI_INFO("latErrEn              : %04x\n", entry->latErrEn);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "rdantPaths"))
        CLI_INFO("rdantPaths            : %04x\n", entry->rdantPaths);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrDiff"))
        CLI_INFO("latErrDiff            : %04x\n", entry->latErrDiff);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrTestRate"))
        CLI_INFO("latErrTestRate        : %04x\n", entry->latErrTestRate);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrTestTime"))
        CLI_INFO("latErrTestTime        : %04x\n", entry->latErrTestTime);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrRstRate"))
        CLI_INFO("latErrRstRate         : %04x\n", entry->latErrRstRate);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrRstTime"))
        CLI_INFO("latErrRstTime         : %04x\n", entry->latErrRstTime);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "seqRcvyResets"))
        CLI_INFO("seqRcvyResets         : %04x\n", seqRcvyResets);
    if (checkValidItem(cmd, "seqRcvyReadEntry", "latErrResets"))
        CLI_INFO("latErrResets          : %04x\n", latErrResets);
    return 0;
}
int frerSeqRcvyFlushAll(void)
{
    MSD_STATUS retVal;
    if (nargs < 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdFrerSeqRcvyFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Seq Rcvy all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerSeqRcvyFlushOne(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerIndvRcvyFlushEntry(sohoDevNum, entry.seqRcvyId);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerSeqRcvyLoad(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    /*Add default value for entry*/
    entry.latErrTestRate = (MSD_U8)0x3;
    entry.latErrTestTime = (MSD_U16)0x2;
    entry.latErrRstRate = (MSD_U8)0x3;
    entry.latErrRstTime = (MSD_U16)0x1e;

    if (updateSeqRcvyData(&entry, 3) != 0)
        return -1;
    retVal = msdFrerSeqRcvyLoad(sohoDevNum, &entry);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerSeqRcvyRead(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;
    MSD_U16 seqRcvyResets = 0;
    MSD_U16 latErrResets = 0;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    
    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerSeqRcvyRead(sohoDevNum, entry.seqRcvyId, &entry, &seqRcvyResets, &latErrResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error read single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displaySeqRcvyData(&entry, seqRcvyResets, latErrResets);
    return 0;
}
int frerSeqRcvyGetNext(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;
    MSD_U16 seqRcvyResets = 0;
    MSD_U16 latErrResets = 0;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerSeqRcvyGetNext(sohoDevNum, entry.seqRcvyId, &entry, &seqRcvyResets, &latErrResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error get next single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displaySeqRcvyData(&entry, seqRcvyResets, latErrResets);
    return 0;

}
int frerSeqRcvyGetViolation(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;
    MSD_U16 seqRcvyResets = 0;
    MSD_U16 latErrResets = 0;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerSeqRcvyGetViolation(sohoDevNum, entry.seqRcvyId, &entry, &seqRcvyResets, &latErrResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Violation single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displaySeqRcvyData(&entry, seqRcvyResets, latErrResets);
    return 0;

}
int frerSeqRcvyReadClear(void)
{
    MSD_STATUS retVal;
    MSD_FRER_SEQRCVY entry;
    MSD_U16 seqRcvyResets = 0;
    MSD_U16 latErrResets = 0;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_SEQRCVY));
    entry.seqRcvyId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdFrerSeqRcvyReadClear(sohoDevNum, entry.seqRcvyId, &entry, &seqRcvyResets, &latErrResets);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Violation single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.seqRcvyId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }

    displaySeqRcvyData(&entry, seqRcvyResets, latErrResets);
    return 0;

}

static int displayB0SCtrData(MSD_FRER_BANK0_CONFIG* entry, MSD_FRER_BANK0_COUNTER* counter)
{
    char* cmd = "frer";
    CLI_INFO("----- FRER Bank0SCtr -----\n");
    if (checkValidItem(cmd, "bank0SCtrRead", "bk0SCtrId"))
        CLI_INFO("bk0SCtrId             : %04x\n", entry->bk0SCtrId);
    if (checkValidItem(cmd, "bank0SCtrRead", "bk0SCtrEn"))
        CLI_INFO("bk0SCtrEn             : %04x\n", entry->bk0SCtrEn);
    if (checkValidItem(cmd, "bank0SCtrRead", "bk0SCtrPort"))
        CLI_INFO("bk0SCtrPort           : %04x\n", entry->bk0SCtrPort);
    if (checkValidItem(cmd, "bank0SCtrRead", "bk0SCtrIndex"))
        CLI_INFO("bk0SCtrIndex          : %04x\n", entry->bk0SCtrIndex);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyTagless"))
        CLI_INFO("indvRcvyTagless       : %04x\n", counter->indvRcvyTagless);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyPassed"))
        CLI_INFO("indvRcvyPassed        : %04x\n", counter->indvRcvyPassed);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyDiscarded"))
        CLI_INFO("indvRcvyDiscarded     : %04x\n", counter->indvRcvyDiscarded);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyOutofOrder"))
        CLI_INFO("indvRcvyOutofOrder    : %04x\n", counter->indvRcvyOutofOrder);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyRogue"))
        CLI_INFO("indvRcvyRogue         : %04x\n", counter->indvRcvyRogue);
    if (checkValidItem(cmd, "bank0SCtrRead", "indvRcvyLost"))
        CLI_INFO("indvRcvyLost          : %04x\n", counter->indvRcvyLost);
    if (checkValidItem(cmd, "bank0SCtrRead", "sidInput"))
        CLI_INFO("sidInput              : %04x\n", counter->sidInput);
    if (checkValidItem(cmd, "bank0SCtrRead", "seqEncErrored"))
        CLI_INFO("seqEncErrored         : %04x\n", counter->seqEncErrored);
    return 0;
}
int frerBank0SCtrFlushAll(void)
{
    MSD_STATUS retVal;
    if (nargs < 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdFrerBank0SCtrFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Seq Rcvy all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank0SCtrFlushCtr(void)
{
    MSD_STATUS retVal;
    if (nargs < 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdFrerBank0SCtrFlushCtr(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error delete entry counters ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank0SCtrLoad(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK0_CONFIG entry;
    int index;
    char* cmd, *value;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_BANK0_CONFIG));
    entry.bk0SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    for (index = 3; index < nargs; index++)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
        }
        if (strIcmp(cmd, "-bk0SCtrEn") == 0)
        {
            entry.bk0SCtrEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bk0SCtrPort") == 0)
        {
            entry.bk0SCtrPort = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bk0SCtrIndex") == 0)
        {
            entry.bk0SCtrIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    retVal = msdFrerBank0SCtrLoad(sohoDevNum, &entry);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk0SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank0SCtrRead(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK0_CONFIG entry;
    MSD_FRER_BANK0_COUNTER counter;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    
    memset(&entry, 0, sizeof(MSD_FRER_BANK0_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK0_COUNTER));
    entry.bk0SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank0SCtrRead(sohoDevNum, entry.bk0SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk0SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB0SCtrData(&entry, &counter);
    
    return 0;
}
int frerBank0SCtrGetNext(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK0_CONFIG entry;
    MSD_FRER_BANK0_COUNTER counter;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_BANK0_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK0_COUNTER));
    entry.bk0SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank0SCtrGetNext(sohoDevNum, entry.bk0SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Next single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk0SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB0SCtrData(&entry, &counter);
    return 0;
}
int frerBank0SCtrReadClear(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK0_CONFIG entry;
    MSD_FRER_BANK0_COUNTER counter;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_BANK0_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK0_COUNTER));
    entry.bk0SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank0SCtrReadClear(sohoDevNum, entry.bk0SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read Clear single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk0SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB0SCtrData(&entry, &counter);
    return 0;
}

static int displayB1SCtrData(MSD_FRER_BANK1_CONFIG* entry, MSD_FRER_BANK1_COUNTER* counter)
{
    char* cmd = "frer";
    CLI_INFO("----- FRER Bank1SCtr -----\n");
    if (checkValidItem(cmd, "bank1SCtrRead", "bk1SCtrId"))
        CLI_INFO("bk1SCtrId            : %04x\n", entry->bk1SCtrId);
    if (checkValidItem(cmd, "bank1SCtrRead", "bk1SCtrEn"))
        CLI_INFO("bk1SCtrEn            : %04x\n", entry->bk1SCtrEn);
    if (checkValidItem(cmd, "bank1SCtrRead", "bk1SCtrPort"))
        CLI_INFO("bk1SCtrPort          : %04x\n", entry->bk1SCtrPort);
    if (checkValidItem(cmd, "bank1SCtrRead", "bk1SCtrIndex"))
        CLI_INFO("bk1SCtrIndex         : %04x\n", entry->bk1SCtrIndex);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyTagless"))
        CLI_INFO("seqRcvyTagless       : %04x\n", counter->seqRcvyTagless);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyPassed"))
        CLI_INFO("seqRcvyPassed        : %04x\n", counter->seqRcvyPassed);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyDiscarded"))
        CLI_INFO("seqRcvyDiscarded     : %04x\n", counter->seqRcvyDiscarded);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyOutofOrder"))
        CLI_INFO("seqRcvyOutofOrder    : %04x\n", counter->seqRcvyOutofOrder);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyRogue"))
        CLI_INFO("seqRcvyRogue         : %04x\n", counter->seqRcvyRogue);
    if (checkValidItem(cmd, "bank1SCtrRead", "seqRcvyLost"))
        CLI_INFO("seqRcvyLost          : %04x\n", counter->seqRcvyLost);
    if (checkValidItem(cmd, "bank1SCtrRead", "sidOutput"))
        CLI_INFO("sidOutput            : %04x\n", counter->sidOutput);
    return 0;
}
int frerBank1SCtrFlushAll(void)
{
    MSD_STATUS retVal;
    if (nargs < 2)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdFrerBank1SCtrFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush Seq Rcvy all entry, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank1SCtrFlushCtr(void)
{
    MSD_STATUS retVal;
    if (nargs < 2)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    retVal = msdFrerBank1SCtrFlushCtr(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error flush bank 1 counters, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank1SCtrLoad(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK1_CONFIG entry;
    int index;
    char* cmd, *value;
    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&entry, 0, sizeof(MSD_FRER_BANK1_CONFIG));
    entry.bk1SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    for (index = 3; index < nargs; index++)
    {
        cmd = CMD_ARGS[index];
        index++;
        if (*cmd != '-')
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
        else
        {
            value = CMD_ARGS[index];
        }
        if (strIcmp(cmd, "-bk1SCtrEn") == 0)
        {
            entry.bk1SCtrEn = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bk1SCtrPort") == 0)
        {
            entry.bk1SCtrPort = (MSD_U8)strtoul(value, NULL, 0);
        }
        else if (strIcmp(cmd, "-bk1SCtrIndex") == 0)
        {
            entry.bk1SCtrIndex = (MSD_U8)strtoul(value, NULL, 0);
        }
        else
        {
            CLI_ERROR("Invalid option input: %s\n", cmd);
            return -1;
        }
    }
    retVal = msdFrerBank1SCtrLoad(sohoDevNum, &entry);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk1SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
}
int frerBank1SCtrRead(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK1_CONFIG entry;
    MSD_FRER_BANK1_COUNTER counter;

    if (nargs < 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_BANK1_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK1_COUNTER));
    entry.bk1SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank1SCtrRead(sohoDevNum, entry.bk1SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error load single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk1SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB1SCtrData(&entry, &counter);
   
    return 0;
}
int frerBank1SCtrGetNext(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK1_CONFIG entry;
    MSD_FRER_BANK1_COUNTER counter;

    if (nargs < 3)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_BANK1_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK1_COUNTER));
    entry.bk1SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank1SCtrGetNext(sohoDevNum, entry.bk1SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Get Next single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk1SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB1SCtrData(&entry, &counter);

    return 0;
}
int frerBank1SCtrReadClear(void)
{
    MSD_STATUS retVal;
    MSD_FRER_BANK1_CONFIG entry;
    MSD_FRER_BANK1_COUNTER counter;

    if (nargs < 3)
    {
        cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    memset(&entry, 0, sizeof(MSD_FRER_BANK1_CONFIG));
    memset(&counter, 0, sizeof(MSD_FRER_BANK1_COUNTER));
    entry.bk1SCtrId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);

    retVal = msdFrerBank1SCtrReadClear(sohoDevNum, entry.bk1SCtrId, &entry, &counter);
    if (retVal != MSD_OK) {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error Read Clear single entry entryNum=%d ret[%d: %s]"
            "\n", (int)entry.bk1SCtrId, retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayB1SCtrData(&entry, &counter);

    return 0;
}

/* Qci related */
static int updateStreamGateEntryData(MSD_STREAM_GATE_ENTRY *entry, int startIndex)
{
	int index;
	char* cmd, *value;
	for (index = startIndex; index < nargs;)
	{
		cmd = CMD_ARGS[index];
		index++;

		if (*cmd != '-')
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
		else
		{
			value = CMD_ARGS[index];
			index++;
		}

		if (strIcmp(cmd, "-gateState") == 0)
		{
			entry->gateState = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-gateIPVEn") == 0)
		{
			entry->gateIPVEn = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-gateIPV") == 0)
		{
			entry->gateIPV = (MSD_U8)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-gateWindowTime") == 0)
		{
			entry->gateWindowTime = (MSD_U16)strtoul(value, NULL, 0);
		}
		else if (strIcmp(cmd, "-gateAllowedBytes") == 0)
		{
			entry->gateAllowedBytes = (MSD_U16)strtoul(value, NULL, 0);
		}
		else
		{
			CLI_ERROR("Invalid option input: %s\n", cmd);
			return -1;
		}
	}

	return 0;
}
int qciStreamFilterFlushAll(void)
{
	MSD_STATUS retVal;

	cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	retVal = msdStreamFilterFlushAll(sohoDevNum);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error stream filter all, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return MSD_OK;
}
int qciStreamGateFlushAll(void)
{
	MSD_STATUS retVal;

	cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	retVal = msdStreamGateFlushAll(sohoDevNum);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error stream gate all, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return MSD_OK;
}
int qciStreamGateAddEntry(void)
{
	MSD_STATUS retVal;
	MSD_U8   streamGateId;
    MSD_U8   pointer;
	MSD_STREAM_GATE_ENTRY streamGateEntry;

	if (nargs < 4)
	{
		cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
		cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}

	memset(&streamGateEntry, 0, sizeof(MSD_STREAM_GATE_ENTRY));
	streamGateId = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	pointer = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
	if (updateStreamGateEntryData(&streamGateEntry, 4) != 0)
		return -1;

	retVal = msdStreamGateEntryAdd(sohoDevNum, streamGateId, pointer, &streamGateEntry);
	if (retVal != MSD_OK) {
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error load stream gate entry Id = %d pointer=%d ret[%d %d: %s]"
			"\n", (int)streamGateId, (int)pointer, retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}

	return 0;
}
int qciStreamGateTimeGenFlushAll(void)
{
	MSD_STATUS retVal;

	cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	retVal = msdStreamGateTimeGenFlushAll(sohoDevNum);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error stream gate time generation all, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return MSD_OK;
}
int qciFlowMeterFlushAll(void)
{
	MSD_STATUS retVal;

	cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	retVal = msdFlowMeterFlushAll(sohoDevNum);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error flow meter flush all, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return MSD_OK;
}

int macsecConfigureEgress(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8  ruleIndex;
	MSD_U8  scIndex;
	MSD_U8  secYIndex;
	MSD_CIPHER_SUITE cipherSuite;
	MSD_BOOL isAutoRekey;
	MSD_U8   tci;
	MSD_U64  nextPN;
	MSD_U64  thresholdPN;
	MSD_BOOL isActiveSA1;
	MSD_U64  sci;
	MSD_U8   secYCounterIndex;
	MSD_U8   scCounterIndex;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs != 14)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	ruleIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	scIndex = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);
	cipherSuite = (MSD_CIPHER_SUITE)strtoul(CMD_ARGS[5], NULL, 0);
	isAutoRekey = (MSD_BOOL)strtoul(CMD_ARGS[6], NULL, 0);
	tci = (MSD_U8)strtoul(CMD_ARGS[7], NULL, 0);
	nextPN = (MSD_U64)strtoull(CMD_ARGS[8], NULL, 0);
	thresholdPN = (MSD_U64)strtoull(CMD_ARGS[9], NULL, 0);
	isActiveSA1 = (MSD_BOOL)strtoul(CMD_ARGS[10], NULL, 0);
	sci = (MSD_U64)strtoull(CMD_ARGS[11], NULL, 0);
	secYCounterIndex = (MSD_U8)strtoul(CMD_ARGS[12], NULL, 0);
	scCounterIndex = (MSD_U8)strtoul(CMD_ARGS[13], NULL, 0);
	retVal = msdMacsecConfigureEgress(sohoDevNum, ruleIndex, scIndex, secYIndex, cipherSuite, isAutoRekey, tci, nextPN, thresholdPN, isActiveSA1, sci, secYCounterIndex, scCounterIndex);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec configure egress, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return retVal;
}
int macsecConfigureIngress(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8  ruleIndex;
	MSD_U8  scIndex;
	MSD_U8  secYIndex;
    MSD_CIPHER_SUITE  cipherSuite;
    MSD_VALIDATE_FRAME  validateFrame;
    MSD_U64  nextPN;
    MSD_U64  sci;
    MSD_U8  secYCounterIndex;
    MSD_U8  scCounterIndex;
	MSD_BOOL allowZeroRxPN;
    MSD_BOOL  replayProtect;
    MSD_32  replayWindow;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 14)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	ruleIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	scIndex = (MSD_U8)strtoul(CMD_ARGS[3], NULL, 0);
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[4], NULL, 0);
    cipherSuite = (MSD_CIPHER_SUITE)strtoul(CMD_ARGS[5], NULL, 0);
    validateFrame = (MSD_VALIDATE_FRAME)strtoul(CMD_ARGS[6], NULL, 0);
    nextPN = (MSD_U64)strtoull(CMD_ARGS[7], NULL, 0);
    sci = (MSD_U64)strtoull(CMD_ARGS[8], NULL, 0);
    secYCounterIndex = (MSD_U8)strtoul(CMD_ARGS[9], NULL, 0);
    scCounterIndex = (MSD_U8)strtoul(CMD_ARGS[10], NULL, 0);
	allowZeroRxPN = (MSD_BOOL)strtoul(CMD_ARGS[11], NULL, 0);
    replayProtect = (MSD_BOOL)strtoul(CMD_ARGS[12], NULL, 0);
    replayWindow = (MSD_32)strtoul(CMD_ARGS[13], NULL, 0);
    retVal = msdMacsecConfigureIngress(sohoDevNum, ruleIndex, scIndex, secYIndex, cipherSuite, validateFrame, nextPN, sci, secYCounterIndex, scCounterIndex, allowZeroRxPN, replayProtect, replayWindow);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec configure ingress, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
	return retVal;
}
int macsecFlushAll(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    retVal = msdMacsecFlushAll(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec flush all, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecCreateRule(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 ruleIndex;
    MSD_MSEC_RULE  rule;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	ruleIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    parseRule(&rule, 3);
    retVal = msdMacsecCreateRule(sohoDevNum, ruleIndex, &rule);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec create rule, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecDeleteRule(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  ruleIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    ruleIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteRule(sohoDevNum, ruleIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete rule, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetRule(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  ruleIndex;
    MSD_MSEC_RULE rule;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&rule, 0, sizeof(MSD_MSEC_RULE));
    ruleIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetRule(sohoDevNum, ruleIndex, &rule);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get rule, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRule(&rule);
    return retVal;
}
int macsecCreateTxSecY(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8 secYIndex;
	MSD_SECY_EGRESS secY;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	msdMemSet(&secY, 0, sizeof(MSD_SECY_EGRESS));
	if ((nargs < 3) || ((nargs % 2) != 1))
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	parseTxSecY(&secY, 3);
	retVal = msdMacsecCreateTxSecY(sohoDevNum, secYIndex, &secY);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec create secY, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return retVal;
}
int macsecCreateRxSecY(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8 secYIndex;
	MSD_SECY_INGRESS secY;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	msdMemSet(&secY, 0, sizeof(MSD_SECY_INGRESS));
	if ((nargs < 3) || ((nargs % 2) != 1))
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	parseRxSecY(&secY, 3);
	retVal = msdMacsecCreateRxSecY(sohoDevNum, secYIndex, &secY);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec create secY, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return retVal;
}
int macsecDeleteTxSecY(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8 secYIndex;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	retVal = msdMacsecDeleteTxSecY(sohoDevNum, secYIndex);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec delete secY, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return MSD_OK;
	return retVal;
}
int macsecDeleteRxSecY(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 secYIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteRxSecY(sohoDevNum,secYIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete secY, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetRxSecY(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_SECY_INGRESS secY;
	MSD_U8 secYIndex;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	msdMemSet(&secY, 0, sizeof(MSD_SECY_INGRESS));
	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	retVal = msdMacsecGetRxSecY(sohoDevNum, secYIndex, &secY);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec get secY, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	displayRxSecY(&secY);
	return retVal;
}
int macsecGetTxSecY(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_SECY_EGRESS secY;
	MSD_U8 secYIndex;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	msdMemSet(&secY, 0, sizeof(MSD_SECY_EGRESS));
	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	secYIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	retVal = msdMacsecGetTxSecY(sohoDevNum, secYIndex, &secY);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec get secY, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	displayTxSecY(&secY);
	return retVal;
}
int macsecCreateRxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 scIndex;
    MSD_MSEC_RX_SC rxSC;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    msdMemSet(&rxSC, 0, sizeof(MSD_MSEC_RX_SC));
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    parseRxSC(&rxSC, 3);
    retVal = msdMacsecCreateRxSC(sohoDevNum, scIndex, &rxSC);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec create RXSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecDeleteRxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 scIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteRxSC(sohoDevNum, scIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete RXSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetRxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  scIndex;
    MSD_MSEC_RX_SC rxSC;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&rxSC, 0, sizeof(MSD_MSEC_RX_SC));
    scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetRxSC(sohoDevNum, scIndex, &rxSC);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RXSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRxSC(&rxSC);
    return retVal;
}
int macsecCreateRxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 saIndex;
    MSD_MSEC_RX_SA rxSA;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    msdMemSet(&rxSA, 0, sizeof(MSD_MSEC_RX_SA));
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    parseRxSA(&rxSA, 3);
    retVal = msdMacsecCreateRxSA(sohoDevNum, saIndex, &rxSA);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec create RXSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecDeleteRxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 saIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteRxSA(sohoDevNum, saIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete RXSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetRxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  saIndex;
    MSD_MSEC_RX_SA rxSA;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&rxSA, 0, sizeof(MSD_MSEC_RX_SA));
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetRxSA(sohoDevNum, saIndex, &rxSA);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RXSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRxSA(&rxSA);
    return retVal;
}
int macsecGetRxSANextPN(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  saIndex;
    MSD_U64 nextPN;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    memset(&nextPN, 0, sizeof(MSD_U64));
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetRxSANextPN(sohoDevNum, saIndex, &nextPN);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RXSA next PN, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRxSANextPN(nextPN);
    return retVal;
}
int macsecCreateTxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 scIndex;
    MSD_MSEC_TX_SC txSC;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    msdMemSet(&txSC, 0, sizeof(MSD_MSEC_TX_SC));
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    parseTxSC(&txSC, 3);
    retVal = msdMacsecCreateTxSC(sohoDevNum, scIndex, &txSC);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec create TxSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecDeleteTxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 scIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteTxSC(sohoDevNum, scIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete TxSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetTxSC(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  scIndex;
    MSD_MSEC_TX_SC txSC;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&txSC, 0, sizeof(MSD_MSEC_TX_SC));
    scIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetTxSC(sohoDevNum, scIndex, &txSC);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TxSC, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTxSC(&txSC);
    return retVal;
}
int macsecCreateTxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 saIndex;
    MSD_MSEC_TX_SA txSA;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    msdMemSet(&txSA, 0, sizeof(MSD_MSEC_TX_SA));
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    parseTxSA(&txSA, 3);
    retVal = msdMacsecCreateTxSA(sohoDevNum, saIndex, &txSA);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec create TxSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecDeleteTxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 saIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecDeleteTxSA(sohoDevNum, saIndex);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec delete TxSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetTxSA(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  saIndex;
    MSD_MSEC_TX_SA txSA;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&txSA, 0, sizeof(MSD_MSEC_TX_SA));
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetTxSA(sohoDevNum, saIndex, &txSA);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TxSA, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTxSA(&txSA);
    return retVal;
}
int macsecGetTxSANextPN(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  saIndex;
    MSD_U64 nextPN;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&nextPN, 0, sizeof(MSD_U64));
    saIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetTxSANextPN(sohoDevNum, saIndex, &nextPN);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TxSA next PN, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTxSANextPN(nextPN);
    return retVal;
}
int macsecGetRXSCStates(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  scCounterIndex;
    MSD_AUTO_CLEAR autoclear;
    MSD_MSEC_RX_SC_COUNTER states;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&states, 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
    scCounterIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    autoclear = (MSD_AUTO_CLEAR)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecGetRXSCStates(sohoDevNum, scCounterIndex, autoclear, &states);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RXSC states, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRXSCStates(&states);
    return retVal;
}
int macsecGetRXSecYStates(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  secYCounterIndex;
    MSD_AUTO_CLEAR autoclear;
    MSD_MSEC_RX_SECY_COUNTER states;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&states, 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
    secYCounterIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    autoclear = (MSD_AUTO_CLEAR)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecGetRXSecYStates(sohoDevNum, secYCounterIndex, autoclear, &states);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RXSecY states, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRXSecYStates(&states);
    return retVal;
}
int macsecGetTXSCStates(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  scCounterIndex;
    MSD_AUTO_CLEAR autoclear;
    MSD_MSEC_TX_SC_COUNTER states;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&states, 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
    scCounterIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    autoclear = (MSD_AUTO_CLEAR)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecGetTXSCStates(sohoDevNum, scCounterIndex, autoclear, &states);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TXSC states, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTXSCStates(&states);
    return retVal;
}
int macsecGetTXSecYStates(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  secYCounterIndex;
    MSD_AUTO_CLEAR autoclear;
    MSD_MSEC_TX_SECY_COUNTER states;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&states, 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
    secYCounterIndex = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    autoclear = (MSD_AUTO_CLEAR)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecGetTXSecYStates(sohoDevNum, secYCounterIndex, autoclear, &states);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TXSecY states, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTXSecYStates(&states);
    return retVal;
}
int macsecGetAllStates(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U8 index;
	MSD_AUTO_CLEAR autoclear;
	MSD_MSEC_RX_SC_COUNTER rxscStates[MAXMIBINDEX];
	MSD_MSEC_TX_SC_COUNTER txscStates[MAXMIBINDEX];
	MSD_MSEC_RX_SECY_COUNTER rxsecyStates[MAXMIBINDEX];
	MSD_MSEC_TX_SECY_COUNTER txsecyStates[MAXMIBINDEX];
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	for (index = 0; index < MAXMIBINDEX; index++)
	{
		msdMemSet(&(rxscStates[index]), 0, sizeof(MSD_MSEC_RX_SC_COUNTER));
		msdMemSet(&(txscStates[index]), 0, sizeof(MSD_MSEC_TX_SC_COUNTER));
		msdMemSet(&(rxsecyStates[index]), 0, sizeof(MSD_MSEC_RX_SECY_COUNTER));
		msdMemSet(&(txsecyStates[index]), 0, sizeof(MSD_MSEC_TX_SECY_COUNTER));
	}
	autoclear = (MSD_AUTO_CLEAR)strtoul(CMD_ARGS[3], NULL, 0);
	for (index = 0; index< MAXMIBINDEX; index++)
	{
		retVal = msdMacsecGetRXSCStates(sohoDevNum, index, autoclear, &(rxscStates[index]));
		if (retVal != MSD_OK)
		{
			if (retVal == MSD_NOT_SUPPORTED)
			{
				CLI_WARNING("The device maybe not support this feature, please double checked\n");
				return MSD_NOT_SUPPORTED;
			}
			CLI_ERROR("Error macsec get RXSC states, ret[%d: %s]"
				"\n", retVal, reflectMSD_STATUS(retVal));
			return MSD_FAIL;
		}
	}
	CLI_INFO("index                        :     0         1         2         3         4         5         6         7         8         9        10       11        12        13        14        15     \n");
	displayAllRXSCStates(rxscStates);
	displayAllRXSecYStates(rxsecyStates);
	displayAllTXSCStates(txscStates);
	displayAllTXSecYStates(txsecyStates);
	return retVal;
}
int macsecSetTxPNThreshold(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U32 pNThr;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs != 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	pNThr = (MSD_U32)strtoul(CMD_ARGS[2], NULL, 0);
	retVal = msdMacsecSetTxPNThreshold(sohoDevNum, pNThr);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec set TxPN threshold, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return retVal;
}
int macsecGetTxPNThreshold(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U32 pNThr;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs != 2)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	msdMemSet(&pNThr, 0, sizeof(MSD_U32));
	retVal = msdMacsecGetTxPNThreshold(sohoDevNum, &pNThr);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec get TxPNThreshold, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	displayTxPNThreshold(pNThr);
	return retVal;
}
int macsecSetTxXPNThreshold(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U64 pNThr;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    pNThr = (MSD_U64)strtoull(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecSetTxXPNThreshold(sohoDevNum, pNThr);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec set TxXPN threshold, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecGetTxXPNThreshold(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U64 pNThr;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 2)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&pNThr, 0, sizeof(MSD_U64));
    retVal = msdMacsecGetTxXPNThreshold(sohoDevNum, &pNThr);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get TxXPNThreshold, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTxXPNThreshold(pNThr);
    return retVal;
}
int macsecSetRxDefaultSCI(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U16 portNum;
    MSD_U64 defaultSCI;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    defaultSCI = (MSD_U64)strtoull(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecSetRxDefaultSCI(sohoDevNum, portNum, defaultSCI);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec set Rx default SCI, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return MSD_OK;
    return retVal;
}
int macsecGetRxDefaultSCI(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U16 portNum;
    MSD_U64 defaultSCI;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    msdMemSet(&defaultSCI, 0, sizeof(MSD_U64));
    portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetRxDefaultSCI(sohoDevNum, portNum, &defaultSCI);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get RxDefaultSCI, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayRxDefaultSCI(defaultSCI);
    return retVal;
}
int macsecEnableEgressSet(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U16 portNum;
	MSD_BOOL isEnable;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 4)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	isEnable = (MSD_BOOL)strtoul(CMD_ARGS[3], NULL, 0);
	retVal = msdMacsecEnableEgressSet(sohoDevNum, portNum, isEnable);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec set enable, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	return retVal;
}
int macsecEnableIngressSet(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U16 portNum;
	MSD_BOOL isEnable;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	isEnable = (MSD_BOOL)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecEnableIngressSet(sohoDevNum, portNum, isEnable);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec set enable, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecEnableEgressGet(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U16 portNum;
	MSD_BOOL isEnable;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecEnableEgressGet(sohoDevNum, portNum, &isEnable);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get enable, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayEnableEgressGet(isEnable);
    return retVal;
}
int macsecEnableIngressGet(void)
{
	MSD_STATUS retVal = MSD_FAIL;
	MSD_U16 portNum;
	MSD_BOOL isEnable;
	cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
	cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
	if (nargs < 3)
	{
		CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
		return 1;
	}
	portNum = (MSD_U16)strtoul(CMD_ARGS[2], NULL, 0);
	retVal = msdMacsecEnableIngressGet(sohoDevNum, portNum, &isEnable);
	if (retVal != MSD_OK)
	{
		if (retVal == MSD_NOT_SUPPORTED)
		{
			CLI_WARNING("The device maybe not support this feature, please double checked\n");
			return MSD_NOT_SUPPORTED;
		}
		CLI_ERROR("Error macsec get enable, ret[%d: %s]"
			"\n", retVal, reflectMSD_STATUS(retVal));
		return MSD_FAIL;
	}
	displayEnableIngressGet(isEnable);
	return retVal;
}
int macsecSetTxSCIntCtrl(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8 index;
	MSD_MACSEC_INT_CONTROL_TXSC txscIntCtrl;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs < 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
	msdMemSet(&txscIntCtrl, 0, sizeof(MSD_MACSEC_INT_CONTROL_TXSC));
	index = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
	parseTxSCIntCtrl(&txscIntCtrl, 3);
    retVal = msdMacsecSetTxSCIntCtrl(sohoDevNum, index, &txscIntCtrl);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec set interrupt control, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    return retVal;
}
int macsecGetTxSCIntCtrl(void)
{
    MSD_STATUS retVal = MSD_FAIL;
    MSD_U8  index;
	MSD_MACSEC_INT_CONTROL_TXSC txscIntCtrl;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 3)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    index = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    retVal = msdMacsecGetTxSCIntCtrl(sohoDevNum, index, &txscIntCtrl);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get interrupt control, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayTxSCIntCtrl(&txscIntCtrl);
    return retVal;
}
int macsecGetNextTxSCIntStatus(void)
{
    MSD_STATUS retVal = MSD_FAIL;
	MSD_MACSEC_INT_TXSC txscInt;
    MSD_U8 index;
    MSD_BOOL isAUTOClear;
    MSD_U8 nextIndex;
    cJSON* cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    cJSON* subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    if (nargs != 4)
    {
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }
    index = (MSD_U8)strtoul(CMD_ARGS[2], NULL, 0);
    isAUTOClear = (MSD_BOOL)strtoul(CMD_ARGS[3], NULL, 0);
    retVal = msdMacsecGetNextTxSCIntStatus(sohoDevNum, index, isAUTOClear, &nextIndex, &txscInt);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_WARNING("The device maybe not support this feature, please double checked\n");
            return MSD_NOT_SUPPORTED;
        }
        CLI_ERROR("Error macsec get next interrupt status, ret[%d: %s]"
            "\n", retVal, reflectMSD_STATUS(retVal));
        return MSD_FAIL;
    }
    displayNextInterruptStatus1(nextIndex, &txscInt);
    return retVal;
}

/* Enable hardware semaphore */
int enableHwSemaphore(void)
{
    MSD_STATUS retVal = 0;

    retVal = msdEnableHwSemaphore(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_INFO("This device do not support hardware semaphore\n");
        }
        else
        {
            CLI_ERROR("msdEnableHwSemaphore return error\n");
        }
    }

    return retVal;
}
/* Disable hardware semaphore */
int disableHwSemaphore(void)
{
    MSD_STATUS retVal = 0;

    retVal = msdDisableHwSemaphore(sohoDevNum);
    if (retVal != MSD_OK)
    {
        if (retVal == MSD_NOT_SUPPORTED)
        {
            CLI_INFO("This device do not support hardware semaphore\n");
        }
        else
        {
            CLI_ERROR("msdEnableHwSemaphore return error\n");
        }
    }

    return retVal;
}

static char* replace_str(char* str, char *orig, char *rep)
{
    static char buffer[MAX_ARGS];
    char *p;
	int ret = 0;

    memset(buffer, 0, MAX_ARGS);

	p = strstr(str, orig);
	if (p == NULL)
	{
		return str;
	}

    strncpy(buffer, str, p - str);
    buffer[p - str] = '\0';

    ret = sprintf(buffer + (p - str), "%s%s", rep, p + strlen(orig));

    return buffer;
}
static char* stristr(const char* str1, const char* str2)
{
    const char* p1 = str1;
    const char* p2 = str2;
    const char* r = *p2 == 0 ? str1 : 0;

    while ((*p1 != 0) && (*p2 != 0))
    {
        if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
        {
            if (r == 0)
            {
                r = p1;
            }

            p2++;
        }
        else
        {
            p2 = str2;
            if (r != 0)
            {
                p1 = r + 1;
            }

            if (tolower((unsigned char)*p1) == tolower((unsigned char)*p2))
            {
                r = p1;
                p2++;
            }
            else
            {
                r = 0;
            }
        }

        p1++;
    }

    return *p2 == 0 ? (char*)r : 0;
}
int umsdApiSearch(void)
{
    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (rootAPIJSON != NULL)
    {
        cJSON *temp = NULL;
        cJSON *funcJSON = cJSON_GetObjectItem(rootAPIJSON, "Functions");
		if (funcJSON != NULL)
		{
			temp = funcJSON->child;
		}
        char *special = NULL;
		if (stristr(CMD_ARGS[2], "vlan") != NULL)
		{
			special = replace_str(CMD_ARGS[2], "vlan", "vtu");
		}
		else if (stristr(CMD_ARGS[2], "atu") != NULL)
		{
			special = replace_str(CMD_ARGS[2], "atu", "fdb");
		}

        while (temp != NULL)
        {
            if (stristr(temp->string, CMD_ARGS[2]) != NULL)
            {
                CLI_INFO("%s\n", temp->string);
            }
            if (special != NULL)
            {
                if (stristr(temp->string, special) != NULL)
                {
                    CLI_INFO("%s\n", temp->string);
                }
            }

            temp = temp->next;
        }
    }

    return 0;
}
int umsdApiMan(void)
{
    if (nargs != 3)
    {
        cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
        cJSON *subJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
        CLI_ERROR("Syntax Error, Using command as follows: %s", cJSON_GetObjectItem(subJSON, "help")->valuestring);
        return 1;
    }

    if (rootAPIJSON != NULL)
    {
        cJSON *temp = NULL;
        cJSON *funcJSON = cJSON_GetObjectItem(rootAPIJSON, "Functions");
        if (funcJSON != NULL)
        {
            temp = cJSON_GetObjectItem(funcJSON, CMD_ARGS[2]);
            if (temp != NULL)
            {
                cJSON *temp1 = cJSON_GetObjectItem(temp, "comment");
                if (temp1 != NULL)
                    CLI_INFO("%s\n", temp1->valuestring);
            }
            else
            {
                CLI_INFO("Can not find the API: %s\n", CMD_ARGS[2]);
            }
        }
    }


    return 0;
}

cJSON *parseJSONfile(void)
{
    FILE* fp = NULL;
    char *filename = NULL;
    cJSON *root = NULL;

    if (qddev->devName == MSD_AMETHYST)
    {
		if (qddev->deviceId == MSD_88E6393X || qddev->deviceId == MSD_88E6391X)
		{
			filename = "./resource/amethyst/Amethyst_6393X.json";
		}
		else if (qddev->deviceId == MSD_88E6193X)
		{
			filename = "./resource/amethyst/Amethyst_6193X.json";
		}
		else if (qddev->deviceId == MSD_88E6191X)
		{
			filename = "./resource/amethyst/Amethyst_6191X.json";
		}
    }
    else if (qddev->devName == MSD_PERIDOT)
    {
        filename = "./resource/peridot/Peridot.json";
    }
    else if (qddev->devName == MSD_TOPAZ)
    {
        filename = "./resource/topaz/Topaz.json";
    }
    else if (qddev->devName == MSD_PEARL)
    {
        filename = "./resource/pearl/Pearl.json";
    }
    else if (qddev->devName == MSD_AGATE)
    {
        filename = "./resource/agate/Agate.json";
    }
	else if (qddev->devName == MSD_OAK)
	{
		filename = "./resource/oak/Oak.json";
	}
    else if (qddev->devName == MSD_SPRUCE)
    {
        filename = "./resource/spruce/Spruce.json";
    }
    else if (qddev->devName == MSD_BONSAIZ1)
    {
        filename = "./resource/bonsaiz1/BonsaiZ1.json";
    }
	else if (qddev->devName == MSD_FIR)
	{
		//filename = "./resource/fir/Fir.json";
		filename = "/etc/resource/fir/Fir.json";
	}
	else if (qddev->devName == MSD_BONSAI)
	{
		filename = "./resource/bonsai/Bonsai.json";
	}
    if (filename != NULL)
    {
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            CLI_ERROR("Can not open the configure file: %s", filename);
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        long len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(len + 1);
		if (NULL == data)
		{
			CLI_ERROR("malloc return failed \n");
			fclose(fp);
			return NULL;
		}
		if (fread(data, 1, len, fp) == 0)
		{
			printf("Real length is less than expected\n");
		}
        fclose(fp);

        root = cJSON_Parse(data);
        rootJSON = root;
		free(data);
        data = NULL;
    }

    return root;
}

cJSON *parseAPIJSONfile(void)
{
    FILE* fp = NULL;
    char *filename = NULL;
    cJSON *root = NULL;

    //filename = "./resource/umsdAPI/umsdAPI.json";
    filename = "/etc/resource/umsdAPI/umsdAPI.json";

    if (filename != NULL)
    {
        fp = fopen(filename, "r");
        if (fp == NULL)
        {
            CLI_ERROR("Can not open the file: %s", filename);
            return NULL;
        }
        fseek(fp, 0, SEEK_END);
        long len = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *data = (char*)malloc(len + 1);
        if (NULL == data)
        {
            CLI_ERROR("malloc return failed \n");
            fclose(fp);
            return NULL;
        }
		if (fread(data, 1, len, fp) == 0)
		{
			printf("real len is less the expected\n");
		}
        fclose(fp);

        root = cJSON_Parse(data);
        free(data);
    }

    return root;
}

int execSystemCmd(void)
{
    char sys_cmd[MAX_CMD_LENGTH] = "";
    int rval;
    int i;

    for (i = 1; i<nargs; i++)
	{
        strncat(sys_cmd, CMD_ARGS[i],sizeof(sys_cmd)-1);
		strncat(sys_cmd, " ", sizeof(sys_cmd)-1);
    }

    rval = system(sys_cmd);

    if (rval == -1){
        CLI_INFO("Failed to execute \"%s\"\n", sys_cmd);
    }

    return 0;
}
int exitProgram(void)
{
    exit_flag = 1;
    return 0;
}

int parseCmd(char *cmd){
    char line[5000] = { 0 };
    char *token;
    int count = 0;
    int i;

	for (i = 0; i < MAX_ARGS; i++)
	{
		memset(CMD_ARGS[i], 0, strlen(CMD_ARGS[i]) + 1);
	}
    strncpy(line, cmd, sizeof(line)-1);
    token = strtok(line, " \t\n");
    count = 0;
    while (token != NULL){
        strcpy(CMD_ARGS[count], token);
        token = strtok(NULL, " \t\n");
        count++;
    }

    nargs = count;
    return count;
}
int parseCmd_cli(const char *command, char *argv[], int argc)
{
    char line[50] = { 0 };
    char *token;
    int count = 0;
    int i;

#ifdef J_DEBUG
	printf("parseCmd_cli( %s ", command);
	for(int j=0; j<argc; j++)
	{
		printf(", %s", argv[j]);
	}
	printf(" )\n");
#endif

	for (i = 0; i < MAX_ARGS; i++)
	{
		memset(CMD_ARGS[i], 0, strlen(CMD_ARGS[i]) + 1);
	}

    strncpy(line, command, sizeof(line)-1);
    token = strtok(line, " \t\n");
    count = 0;
    while (token != NULL){
        strcpy(CMD_ARGS[count], token);
        token = strtok(NULL, " \t\n");
        count++;
    }
    for (i = 0; i < argc; i++)
    {
        strcpy(CMD_ARGS[count], argv[i]);
        count++;
    }

    nargs = count;

#ifdef J_DEBUG
	printf("parseCmd_cli() : nargs=%d\n", nargs);
#endif

    return count;
}
int runCommand(FILE* fp, char* prompt)
{
    int i, rval = 0;
    char *buf = NULL;
    int len = 0;

    buf = (char *)malloc(MAX_CMD_LENGTH*sizeof(char));
    if (buf == NULL){
        CLI_ERROR("Error: Cannot allocate memory\n");
        return -1;
    }
    len = (int)MAX_CMD_LENGTH;
    memset(buf, 0, MAX_CMD_LENGTH*sizeof(char));

    while (fgets(buf, MAX_CMD_LENGTH, fp) != NULL)
    {
        exit_flag = 0;
        nargs = parseCmd(buf);
        g_filename = strstr(buf, "\"");

        if ((nargs == 0) && (prompt_cnt == 0)){
            printf("%s", prompt);
            continue;
        }

        if (*(CMD_ARGS[0]) != '#')
        {
            cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
            if (cmdJSON == NULL)
            {
                CLI_ERROR("Invalid command: %s\n", CMD_ARGS[0]);
				free(buf);
                return 0;
            }

			/* top level command */
            CALLBACE callback = (CALLBACE)(intptr_t)cmdJSON->valueint;
            if (callback != NULL)
            {
                callback();
				CLI_INFO("\n");
				/*free(buf);
                return 0;*/
            }

			/* sub level command */
            cJSON *subcmdJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
			if (callback == NULL)
			{
				if (subcmdJSON == NULL)
				{
					CLI_ERROR("Invalid sub command: %s\n", CMD_ARGS[1]);
					free(buf);
					return 0;
				}
				callback = (CALLBACE)(intptr_t)subcmdJSON->valueint;
				if (callback != NULL)
				{
					callback();
					CLI_INFO("\n");
				}
			}
        }

		if (exit_flag == 1)
		{
			break;
		}

        memset(buf, 0, MAX_CMD_LENGTH);

		for (i = 0; i < MAX_ARGS; i++)
		{
			memset(CMD_ARGS[i], 0, strlen(CMD_ARGS[i]) + 1);
		}

		if (prompt_cnt == 0)
		{
			printf("%s", prompt);
		}
    }

	if (buf)
	{
		free(buf);
	}
    return rval;
}
void apiCLI(void)
{
    int len = 0;
    int i, rval = 0;
    FILE *fp = NULL;
    char *prompt = PROMPT_STRING;

    for (i = 0; i<MAX_ARGS; i++){
        CMD_ARGS[i] = (char *)malloc(MAX_ARGS_LENGTH*sizeof(char));
        memset(CMD_ARGS[i], 0, MAX_ARGS_LENGTH*sizeof(char));
    }

    fp = stdin;

	if (prompt_cnt == 0)
	{
		printf("%s", prompt);
	}

    rval = runCommand(fp, prompt);

    for (i = 0; i<MAX_ARGS; i++){
        free(CMD_ARGS[i]);
    }
}

void apiCLI_alloc_buf(void)
{
    int i;
    for (i = 0; i<MAX_ARGS; i++){
        CMD_ARGS[i] = (char *)malloc(MAX_ARGS_LENGTH*sizeof(char));
        memset(CMD_ARGS[i], 0, MAX_ARGS_LENGTH*sizeof(char));
    }
}
void apiCLI_free_buf(void)
{
    int i;
    for (i = 0; i<MAX_ARGS; i++){
        free(CMD_ARGS[i]);
    }
}

/*Callback function*/
int cmd_operation(struct cli_def *cli, const char *command, char *argv[], int argc)
{
    parseCmd_cli(command, argv, argc);

    cJSON *cmdJSON = cJSON_GetObjectItem(rootJSON, CMD_ARGS[0]);
    CALLBACE callback = (CALLBACE)(intptr_t)cmdJSON->valueint;
    if (callback != NULL)
    {
        callback();
        return 0;
    }

    cJSON *subcmdJSON = cJSON_GetObjectItem(cJSON_GetObjectItem(cmdJSON, "subcmd"), CMD_ARGS[1]);
    callback = (CALLBACE)(intptr_t)subcmdJSON->valueint;
	if (callback != NULL)
	{
		callback();
	}

    return 0;
}
int cmd_register_callback(cJSON *temp1, cJSON *temp2)
{
    int index = 0;
    while (externCLICMDList[index].cmd != NULL)
    {
        int flag = 0;
        if (strIcmp(temp1->string, externCLICMDList[index].cmd) == 0)
        {
            int cmdIndex = 0;
            while (externCLICMDList[index].cmdList[cmdIndex].callback != NULL)
            {
                if (strIcmp(externCLICMDList[index].cmdList[cmdIndex].cmd, temp2->string) == 0)
                {
                    temp2->valueint = (intptr_t)(externCLICMDList[index].cmdList[cmdIndex].callback);
                    flag = 1;
                    break;
                }
                cmdIndex++;
            }
        }
        if (flag == 1)
            break;
        index++;
    }
    return 0;
}
int cmd_direct_register_callback(cJSON *temp)
{
    int retVal = 0;

    int index = 0;
    while (directCMDList[index].callback != NULL)
    {
        if (strIcmp(directCMDList[index].cmd, temp->string) == 0)
        {
            temp->valueint = (intptr_t)(directCMDList[index].callback);
            retVal = 1;
            break;
        }
        index++;
    }

#ifdef J_DEBUG
	printf("cmd_direct_register_callback() : retVal=%d\n", retVal);
#endif

    return retVal;
}
