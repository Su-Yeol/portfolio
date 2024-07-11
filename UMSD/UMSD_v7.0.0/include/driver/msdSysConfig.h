/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdSysConfig.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell System configuration.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdSysConfig_h
#define msdSysConfig_h

#include <driver/msdApiTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported System Config Types 			                                   */
/****************************************************************************/
#define MSD_REG_SWITCH_ID    3
#define MSD_VERSION_MAX_LEN     30


#ifdef MSD_USE_SIMPLE_PORT_MAPPING
#define MSD_LPORT_2_PORT(_lport)      (MSD_U8)((_lport) & 0xff)
#define MSD_PORT_2_LPORT(_port)       (MSD_LPORT)((_port) & 0xff)
#define MSD_LPORTVEC_2_PORTVEC(_lvec)      (MSD_U32)((_lvec) & 0xffff)
#define MSD_PORTVEC_2_LPORTVEC(_pvec)       (MSD_U32)((_pvec) & 0xffff)
#define MSD_LPORT_2_PHY(_lport)      (MSD_U8)((_lport) & 0xff)
#else
#define MSD_LPORT_2_PHY(_lport)       msdlport2phy(dev,_lport)
#define MSD_LPORT_2_SERDES(_lport)    msdlport2serdes(dev,_lport)

#define MSD_LPORT_2_PORT(_lport)      msdlport2port(dev->validPortVec, _lport)
#define MSD_PORT_2_LPORT(_port)       msdport2lport(dev->validPortVec, _port)
#define MSD_LPORTVEC_2_PORTVEC(_lvec) msdlportvec2portvec(dev->validPortVec, _lvec)
#define MSD_PORTVEC_2_LPORTVEC(_pvec) msdportvec2lportvec(dev->validPortVec, _pvec)

#endif

#define IS_RMU_SUPPORTED(dev)    \
    ((dev->InterfaceChannel == MSD_INTERFACE_RMU || dev->InterfaceChannel == MSD_INTERFACE_FW_RMU) ? 1 : 0)

#define IS_FW_RMU_SUPPORTED(dev)    \
    ((dev->InterfaceChannel == MSD_INTERFACE_FW_RMU) ? 1 : 0)

#define IS_SMI_SUPPORTED(dev)    \
    ((dev->InterfaceChannel == MSD_INTERFACE_SMI) ? 1 : 0)

#define IS_SMI_MULTICHIP_SUPPORTED(dev)    \
    ((dev->InterfaceChannel == MSD_INTERFACE_SMI_MULTICHIP) ? 1 : 0)

#define MAX_SOHO_DEVICES	32U
/*
 * Typedef: struct MSD_VERSION
 *
 * Description: This struct holds the package version.
 *
 * Fields:
 *      version - string array holding the version.
 *
 */
typedef struct
{
    MSD_U8   version[MSD_VERSION_MAX_LEN];
}MSD_VERSION;

/* Define the different device type that may exist in system        
 * Typedef: struct MSD_VERSION
 *
 * Description: This struct holds the package version.
 *
 * Fields:
 *      version - string array holding the version.
 *
 */ 
typedef enum
{
	MSD_88E6141    = 0x340,   /* Topaz Low Cost version, not support TCAM and AVB */
	MSD_88E6341    = 0x341,   /* Topaz Full Featured Device version */

    MSD_88E6390X = 0x0A1,  /* 88E6390X - BGA package */
    MSD_88E6390 = 0x390,  /* 88E6390  - PQFP package */
    MSD_88E6190X = 0x0A0,  /* 88E6190X - BGA package */
    MSD_88E6190 = 0x190,  /* 88E6190  - PQFP package */
	MSD_88E6290 = 0x290,

	MSD_88E6352 = 0x352, /*Agate*/
	MSD_88E6172 = 0x172,
	MSD_88E6176 = 0x176, /*with serdes*/
	MSD_88E6240 = 0x240,

	MSD_88E6320 = 0x115, /*Pearl not support TCAM and Cut thru*/	
	MSD_88E6321 = 0x310,  /* Pearl Full Featured Device version */

	MSD_88E6191X = 0x192,  /* Amethyst 88E6191X  - BGA package - No AVB, No Routing, No Cut-through, No TCAM, No 802.1BR*/
	MSD_88E6393X = 0x393,   /* 88E6393X  - BGA package - Full Featured Device*/
	MSD_88E6193X = 0x193,   /* 88E6193X  - BGA package - No AVB, No Routing, No Cut-through*/
	MSD_88E6391X = 0x391,    /* 88E6391X - Full Featured Device */

	MSD_88Q5072 = 0xA72,   /* 88Q5072  - Oak b0*/
	MSD_88Q6113 = 0xF13,   /* 88Q6113  - Spruce b0*/
	MSD_88Q5040Z1 = 0xA40,   /* 88Q5040  - Bonsai Z1*/
	MSD_88Q5151 = 0xB51,   /* 88Q5151  - Fir*/
	MSD_88Q5152 = 0xB52,   /* 88Q5152  - Fir*/
	MSD_88Q5192 = 0xB92,   /* 88Q5192  - Fir Plus*/
	MSD_88Q5040 = 0x124    /* 88Q5040  - Bonsai*/
}MSD_DEVICE;


/* Definition for the revision number of the device        */
typedef enum
{
    MSD_REV_0 = 0,
    MSD_REV_1,
    MSD_REV_2,
    MSD_REV_3
}MSD_DEVICE_REV;

typedef struct MSD_QD_DEV_ MSD_QD_DEV;

typedef MSD_QD_DEV* MSD_QD_DEV_PTR;


/*
 * definitions for registering MII access functions.
 *
*/
typedef MSD_STATUS (*MSD_FMSD_READ_MII)(
                        MSD_U8	    devNum,
                        MSD_U8		phyAddr, 
						MSD_U8		miiReg, 
                        MSD_U16*		value);
typedef MSD_STATUS(*MSD_FMSD_WRITE_MII)(
                        MSD_U8	     devNum,
                        MSD_U8		 phyAddr, 
                        MSD_U8		 miiReg, 
                        MSD_U16		 value);

/*
send_and_receive_packet
* Override DA+SA
	MSD_ETHERADDR swMacAddr;
	MSD_ETHERADDR cpuMacAddr;
* req_packet: SA = CPU Mac, From_DSA_Tag: Trg_Dev+SeqNum
* rsp_packet: DA = CPU Mac, To_DSA_Tag: Src_Dev+SeqNum
* Check:
	1) rsp_packet DA == CPU Mac
	2) rsp_packet Src_Dev == req_packet Trg_Dev
	3) rsp_packet SeqNum == req_packet SeqNum
	4) rsp_packet Format/Code = req_packet Format/Code
				  
* If not match, return failed with empty resp packets length!
*/
typedef MSD_STATUS(*MSD_RMU_TX_RX_PAK)(
	MSD_U8	*req_pkt,
	MSD_U32	req_pkt_len,
	MSD_U8	**rsp_pkt,
	MSD_U32	*rsp_pkt_len);

typedef enum
{
    MSD_INTERFACE_SMI = 0x0,
    MSD_INTERFACE_SMI_MULTICHIP,
    MSD_INTERFACE_RMU,
	MSD_INTERFACE_FW_RMU,
	MSD_INTERFACE_VSW
}MSD_INTERFACE;

typedef enum
{
	MSD_HW_SEM_DISABLE  = 0x0,  /* 0 means do not use hardware semaphore */
	MSD_HW_SEM_SMI = 0x1,
	MSD_HW_SEM_RMU = 0x2,
	MSD_HW_SEM_SPI = 0x3,
	MSD_HW_SEM_APB = 0x4
}MSD_HWSEMAPHORE;

typedef MSD_STATUS (*FMSD_gfdbAddMacEntry)(MSD_QD_DEV*  dev, MSD_ATU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gfdbGetAtuEntryNext)(MSD_QD_DEV*  dev, MSD_ATU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gfdbFlush)(MSD_QD_DEV*  dev, MSD_FLUSH_CMD flushCmd);
typedef MSD_STATUS(*FMSD_gfdbFlushInDB)(MSD_QD_DEV*  dev, MSD_FLUSH_CMD flushCmd, MSD_U32 fid);
typedef MSD_STATUS(*FMSD_gfdbMove)(MSD_QD_DEV*  dev, MSD_MOVE_CMD  moveCmd, MSD_LPORT  moveFrom, MSD_LPORT  moveTo);
typedef MSD_STATUS(*FMSD_gfdbMoveInDB)(MSD_QD_DEV*  dev, MSD_MOVE_CMD  moveCmd, MSD_U32 fid, MSD_LPORT  moveFrom, MSD_LPORT  moveTo);
typedef MSD_STATUS(*FMSD_gfdbDelAtuEntry)(MSD_QD_DEV*  dev, MSD_ETHERADDR* macAddr, MSD_U32 fid);
typedef MSD_STATUS(*FMSD_gfdbGetViolation)(MSD_QD_DEV*  dev, MSD_ATU_INT_STATUS* atuIntStatus);
typedef MSD_STATUS(*FMSD_gfdbFindAtuMacEntry)(MSD_QD_DEV*  dev, MSD_ETHERADDR *macAddr, MSD_U32 fid, MSD_ATU_ENTRY* entry, MSD_BOOL* found);
typedef MSD_STATUS(*FMSD_gfdbPortRemove)(MSD_QD_DEV*  dev, MSD_MOVE_CMD  moveCmd, MSD_LPORT  portNum);
typedef MSD_STATUS(*FMSD_gfdbPortRemoveInDB)(MSD_QD_DEV*  dev, MSD_MOVE_CMD  moveCmd, MSD_U32  fid, MSD_LPORT  portNum);
typedef MSD_STATUS(*FMSD_gfdbSetAgingTimeout)(MSD_QD_DEV*  dev, MSD_U32 timeout);
typedef MSD_STATUS(*FMSD_gfdbGetAgingTimeout)(MSD_QD_DEV*  dev, MSD_U32* timeout);
typedef MSD_STATUS(*FMSD_gfdbSetLearn2All)(MSD_QD_DEV*  dev, MSD_BOOL mode);
typedef MSD_STATUS(*FMSD_gfdbGetLearn2All)(MSD_QD_DEV*  dev, MSD_BOOL* mode);
typedef MSD_STATUS(*FMSD_gfdbSetPortLearnLimit)(MSD_QD_DEV*  dev, MSD_LPORT  portNum, MSD_U32  limit);
typedef MSD_STATUS(*FMSD_gfdbGetPortLearnLimit)(MSD_QD_DEV*  dev, MSD_LPORT  portNum, MSD_U32*  limit);
typedef MSD_STATUS(*FMSD_gfdbGetPortLearnCount)(MSD_QD_DEV*  dev, MSD_LPORT  portNum, MSD_U32*  count);
typedef MSD_STATUS(*FMSD_gfdbGetEntryCount)(MSD_QD_DEV*  dev, MSD_U32*  count);
typedef MSD_STATUS(*FMSD_gfdbGetEntryCountPerFid)(MSD_QD_DEV*  dev, MSD_U32  fid, MSD_U32*  count);
typedef MSD_STATUS(*FMSD_gfdbGetNonStaticEntryCount)(MSD_QD_DEV*  dev, MSD_U32*  count);
typedef MSD_STATUS(*FMSD_gfdbGetNonStaticEntryCountPerFid)(MSD_QD_DEV*  dev, MSD_U32  fid, MSD_U32*  count);
typedef MSD_STATUS(*FMSD_gfdbDump)(MSD_QD_DEV*  dev, MSD_U32  fid);

typedef MSD_STATUS(*FMSD_gBpeEnableSet)(MSD_QD_DEV* dev, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gBpeEnableGet)(MSD_QD_DEV* dev, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gecidAddEntry)(MSD_QD_DEV* dev, MSD_ECID_ENTRY *ecidEntry);
typedef MSD_STATUS(*FMSD_gecidFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gecidFlushEntry)(MSD_QD_DEV* dev, MSD_U8  group, MSD_U16 ecid);
typedef MSD_STATUS(*FMSD_gecidMove)(MSD_QD_DEV* dev, MSD_LPORT moveFrom, MSD_LPORT moveTo);
typedef MSD_STATUS(*FMSD_gecidGetEcidEntryNext)(MSD_QD_DEV* dev, MSD_U8  group, MSD_U16 ecid, MSD_ECID_ENTRY* ecidEntry);
typedef MSD_STATUS(*FMSD_gecidFindEcidEntry)(MSD_QD_DEV* dev, MSD_U8  group, MSD_U16 ecid, MSD_ECID_ENTRY* ecidEntry, MSD_BOOL* found);
typedef MSD_STATUS(*FMSD_gecidGetEntryCount)(MSD_QD_DEV* dev, MSD_U32 *count);
typedef MSD_STATUS(*FMSD_gecidDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_gvctGetAdvCableDiag)(MSD_QD_DEV*  dev, MSD_LPORT port, MSD_ADV_VCT_MODE mode, MSD_ADV_CABLE_STATUS* cableStatus);
typedef MSD_STATUS(*FMSD_gvctGetAdvExtendedStatus)(MSD_QD_DEV*  dev, MSD_LPORT port, MSD_ADV_EXTENDED_STATUS* extendedStatus);

typedef MSD_STATUS(*FMSD_gstuGetEntryNext)(MSD_QD_DEV*  dev, MSD_STU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gstuAddEntry)(MSD_QD_DEV*  dev, MSD_STU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gstuDelEntry)(MSD_QD_DEV*  dev, MSD_U16 sid);
typedef MSD_STATUS(*FMSD_gstuFindSidEntry)(MSD_QD_DEV* dev, MSD_U16 sid, MSD_STU_ENTRY* entry, MSD_BOOL* found);
typedef MSD_STATUS(*FMSD_gstuGetEntryCount)(MSD_QD_DEV* dev, MSD_U16* count);
typedef MSD_STATUS(*FMSD_gstuDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_gvtuFindVidEntry)(MSD_QD_DEV*  dev, MSD_U16 vlanId, MSD_VTU_ENTRY* entry, MSD_BOOL* found);
typedef MSD_STATUS(*FMSD_gvtuExistVidEntry)(MSD_QD_DEV* dev, MSD_U16 vlanId, MSD_BOOL* isExist);
typedef MSD_STATUS(*FMSD_gvtuFlush)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gvtuAddEntry)(MSD_QD_DEV*  dev, MSD_VTU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gvtuGetEntryNext)(MSD_QD_DEV*  dev, MSD_VTU_ENTRY* entry);
typedef MSD_STATUS(*FMSD_gvtuDelEntry)(MSD_QD_DEV*  dev, MSD_U16 vlanId);
typedef MSD_STATUS(*FMSD_gvtuSetMode)(MSD_QD_DEV*  dev, MSD_VTU_MODE    mode);
typedef MSD_STATUS(*FMSD_gvtuGetMode)(MSD_QD_DEV*  dev, MSD_VTU_MODE*   mode);
typedef MSD_STATUS(*FMSD_gvtuGetViolation)(MSD_QD_DEV*  dev, MSD_VTU_INT_STATUS* vtuIntStatus);
typedef MSD_STATUS(*FMSD_gvtuGetEntryCount)(MSD_QD_DEV* dev, MSD_U16* count);
typedef MSD_STATUS(*FMSD_gvtuSetMemberTag)(MSD_QD_DEV* dev, MSD_U16 vlanId, MSD_LPORT portNum, MSD_PORT_MEMBER_TAG memberTag);
typedef MSD_STATUS(*FMSD_gvtuGetMemberTag)(MSD_QD_DEV* dev, MSD_U16 vlanId, MSD_LPORT portNum, MSD_PORT_MEMBER_TAG* memberTag);
typedef MSD_STATUS(*FMSD_gvtuDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_impRun)(MSD_QD_DEV*  dev, MSD_U32 addr);
typedef MSD_STATUS(*FMSD_impReset)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_impWriteEEPROM)(MSD_QD_DEV*  dev, MSD_U16 addr, MSD_U8 data);
typedef MSD_STATUS(*FMSD_impLoadToEEPROM)(MSD_QD_DEV*  dev, MSD_U16 dataSize, MSD_U8* data);
typedef MSD_STATUS(*FMSD_impLoadToRAM)(MSD_QD_DEV*  dev, MSD_U32 addr, MSD_U32 dataSize, MSD_U8* data);
typedef MSD_STATUS(*FMSD_impStop)(MSD_QD_DEV*  dev);

typedef MSD_STATUS(*FMSD_geepromWrite)(MSD_QD_DEV*  dev, MSD_U16  addr, MSD_U16  data);
typedef MSD_STATUS(*FMSD_geepromRead)(MSD_QD_DEV*  dev, MSD_U16  addr, MSD_U16*  data);
typedef MSD_STATUS(*FMSD_geepromSetMChipSel)(MSD_QD_DEV*  dev, MSD_U8  chipSelect);
typedef MSD_STATUS(*FMSD_geepromGetMChipSel)(MSD_QD_DEV*  dev, MSD_U8*  chipSelect);

typedef MSD_STATUS(*FMSD_gphyReset)(MSD_QD_DEV*  dev, MSD_LPORT  port); 
typedef MSD_STATUS(*FMSD_serdesReset)(MSD_QD_DEV*  dev, MSD_LPORT  port);
typedef MSD_STATUS(*FMSD_gphySetPortLoopback)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL   enable);
typedef MSD_STATUS(*FMSD_gphySetPortSpeed)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PHY_SPEED speed);
typedef MSD_STATUS(*FMSD_gphyPortPowerDown)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL   state);
typedef MSD_STATUS(*FMSD_gphySetPortDuplexMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL   dMode);
typedef MSD_STATUS(*FMSD_gphySetPortAutoMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PHY_AUTO_MODE mode);
typedef MSD_STATUS(*FMSD_gphySetEEE)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gphySetFlowControlEnable)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gphySetIntEnable)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 en);
typedef MSD_STATUS(*FMSD_gphyGetIntEnable)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 *en);
typedef MSD_STATUS(*FMSD_gphyGetIntStatus)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 *stat);

typedef MSD_STATUS(*FMSD_gpirlInitialize)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gpirlInitResource)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes);
typedef MSD_STATUS(*FMSD_gpirlCustomSetup)(MSD_U32  tgtRate, MSD_U32  tgtBstSize, MSD_PIRL_COUNT_MODE countMode, MSD_PIRL_CUSTOM_RATE_LIMIT *  customSetup);
typedef MSD_STATUS(*FMSD_gpirlReadResource)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes, MSD_PIRL_DATA*    pirlData);
typedef MSD_STATUS(*FMSD_gpirlWriteResource)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes, MSD_PIRL_DATA*    pirlData);
typedef MSD_STATUS(*FMSD_gpirlGetResReg)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes, MSD_U32  regOffset, MSD_U16*   data);
typedef MSD_STATUS(*FMSD_gpirlSetResReg)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes, MSD_U32  regOffset, MSD_U16    data);
typedef MSD_STATUS(*FMSD_gpirlAdvConfig)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes, MSD_U32  tgtRate, MSD_U32  tgtBstSize, MSD_PIRL_COUNT_MODE countMode, MSD_PIRL_PKTTYPE_OR_PRI *pktTypeOrPri);
typedef MSD_STATUS(*FMSD_gpirlAdvConfigDisable)(MSD_QD_DEV*  dev, MSD_LPORT   irlPort, MSD_U32    irlRes);

typedef MSD_STATUS(*FMSD_gvlnSetPortVid)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U16    vid);
typedef MSD_STATUS(*FMSD_gvlnGetPortVid)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U16*  vid);
typedef MSD_STATUS(*FMSD_gvlnSetPortVlanDot1qMode)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_8021Q_MODE    mode);
typedef MSD_STATUS(*FMSD_gvlnGetPortVlanDot1qMode)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_8021Q_MODE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetDiscardTagged)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetDiscardTagged)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetDiscardUntagged)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetDiscardUntagged)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetUnicastFloodBlock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetUnicastFloodBlock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetMulticastFloodBlock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetMulticastFloodBlock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetIGMPSnoopEnable)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    en);
typedef MSD_STATUS(*FMSD_gprtGetIGMPSnoopEnable)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  en);
typedef MSD_STATUS(*FMSD_gprtSetHeaderMode)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    en);
typedef MSD_STATUS(*FMSD_gprtGetHeaderMode)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  en);
typedef MSD_STATUS(*FMSD_gprtSetDropOnLock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL    en);
typedef MSD_STATUS(*FMSD_gprtGetDropOnLock)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  en);
typedef MSD_STATUS(*FMSD_gprtSetDiscardBCastMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL   en);
typedef MSD_STATUS(*FMSD_gprtGetDiscardBCastMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  en);
typedef MSD_STATUS(*FMSD_gprtSetJumboMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_MTU_SIZE   mode);
typedef MSD_STATUS(*FMSD_gprtGetJumboMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_MTU_SIZE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetSpecifiedMTU)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16   size);
typedef MSD_STATUS(*FMSD_gprtGetSpecifiedMTU)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16*  size);
typedef MSD_STATUS(*FMSD_gprtSetLearnEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetLearnEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetVTUPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE    mode);
typedef MSD_STATUS(*FMSD_gprtGetVTUPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetSAPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE    mode);
typedef MSD_STATUS(*FMSD_gprtGetSAPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetDAPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE    mode);
typedef MSD_STATUS(*FMSD_gprtGetDAPriorityOverride)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PRI_OVERRIDE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetMessagePort)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetMessagePort)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetEtherType)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  etype);
typedef MSD_STATUS(*FMSD_gprtGetEtherType)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  *etype);
typedef MSD_STATUS(*FMSD_gprtSetAllowVidZero)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetAllowVidZero)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetFlowCtrl)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL    en, MSD_PORT_FC_MODE  mode);
typedef MSD_STATUS(*FMSD_gprtGetFlowCtrl)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  en, MSD_PORT_FC_MODE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetForceSpeed)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCED_SPEED_MODE    mode);
typedef MSD_STATUS(*FMSD_gprtGetForceSpeed)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCED_SPEED_MODE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetEgressMonitorSource)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetEgressMonitorSource)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetIngressMonitorSource)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL    mode);
typedef MSD_STATUS(*FMSD_gprtGetIngressMonitorSource)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_grcSetEgressRate)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_ELIMIT_MODE   mode, MSD_U32 rate);
typedef MSD_STATUS(*FMSD_grcGetEgressRate)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_ELIMIT_MODE*  mode, MSD_U32* rate);
typedef MSD_STATUS(*FMSD_gprtGetDuplex)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCE_DUPLEX_MODE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetDuplex)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCE_DUPLEX_MODE mode);
typedef MSD_STATUS(*FMSD_gprtGetDuplexStatus)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtGetForceLink)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCE_LINK_MODE*  mode);
typedef MSD_STATUS(*FMSD_gprtSetForceLink)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_FORCE_LINK_MODE mode);
typedef MSD_STATUS(*FMSD_gprtGetLinkState)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  state);
typedef MSD_STATUS(*FMSD_gprtGetSpeed)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_SPEED*  speed);
typedef MSD_STATUS(*FMSD_gprtSetCutThrouthEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_CUT_THROUGH *cutThru);
typedef MSD_STATUS(*FMSD_gprtGetCutThrouthEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_CUT_THROUGH *cutThru);
typedef MSD_STATUS(*FMSD_gprtSetIgnoreFcs)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetIgnoreFcs)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetAllowBadFcs)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetAllowBadFcs)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetPauseLimitOut)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  limit);
typedef MSD_STATUS(*FMSD_gprtGetPauseLimitOut)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  *limit);
typedef MSD_STATUS(*FMSD_gprtSetPauseLimitIn)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  limit);
typedef MSD_STATUS(*FMSD_gprtGetPauseLimitIn)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16  *limit);
typedef MSD_STATUS(*FMSD_gprtSetQueueToPause)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  queue);
typedef MSD_STATUS(*FMSD_gprtGetQueueToPause)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  *queue);
typedef MSD_STATUS(*FMSD_gprtSetRxPriorityFlowControlEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetRxPriorityFlowControlEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtSetRxPriorityFlowControlToQueue)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_U8 queue);
typedef MSD_STATUS(*FMSD_gprtGetRxPriorityFlowControlToQueue)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_U8 *queue);
typedef MSD_STATUS(*FMSD_gprtSetTxPriorityFlowControlEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetTxPriorityFlowControlEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  pfcPri, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtGetOutQueueSize)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  queue, MSD_U16 *count);
typedef MSD_STATUS(*FMSD_gprtSetMapDA)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetMapDA)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetPortLocked)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetPortLocked)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetHoldAt1)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetHoldAt1)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetInt0nAgeOut)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetInt0nAgeOut)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetRefreshLocked)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetRefreshLocked)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetIgnoreWrongData)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetIgnoreWrongData)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetARPMirror)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  en);
typedef MSD_STATUS(*FMSD_gprtGetARPMirror)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL  *en);
typedef MSD_STATUS(*FMSD_gprtSetVlanPorts)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_LPORT *memPorts, MSD_U8 memPortsLen);
typedef MSD_STATUS(*FMSD_gprtGetVlanPorts)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_LPORT *memPorts, MSD_U8 *memPortsLen); 
typedef MSD_STATUS(*FMSD_gprtSetForceDefaultVid)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetForceDefaultVid)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtSetFrameMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_FRAME_MODE mode);
typedef MSD_STATUS(*FMSD_gprtGetFrameMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_FRAME_MODE *mode);
typedef MSD_STATUS(*FMSD_gprtSetPortECID)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16 ecid);
typedef MSD_STATUS(*FMSD_gprtGetPortECID)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16 *ecid);
typedef MSD_STATUS(*FMSD_gprtSetIgnoreETag)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetIgnoreETag)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtSetEgressMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_EGRESS_MODE mode);
typedef MSD_STATUS(*FMSD_gprtGetEgressMode)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_EGRESS_MODE *mode);
typedef MSD_STATUS(*FMSD_gprtSetExtendedPortCtrlReg)(MSD_QD_DEV*  dev, MSD_LPORT port, MSD_U8 index, MSD_U8 fieldOffset, MSD_U8 fieldLength, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gprtGetExtendedPortCtrlReg)(MSD_QD_DEV*  dev, MSD_LPORT port, MSD_U8 index, MSD_U8 fieldOffset, MSD_U8 fieldLength, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_gprtSetPortState)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_STP_STATE mode);
typedef MSD_STATUS(*FMSD_gprtGetPortState)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_STP_STATE *mode);
typedef MSD_STATUS(*FMSD_gprtSetPreemptEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetPreemptEnable)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtSetPreemptSize)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16 size);
typedef MSD_STATUS(*FMSD_gprtGetPreemptSize)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U16 *size);
typedef MSD_STATUS(*FMSD_gprtSetPreemptQueue)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8 queue, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gprtGetPreemptQueue)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8 queue, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gprtGetPortState)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_STP_STATE *mode);
typedef MSD_STATUS(*FMSD_gprtRegDump)(MSD_QD_DEV*  dev, MSD_LPORT  port);

typedef MSD_STATUS(*FMSD_gsysSetTrunkMaskTable)(MSD_QD_DEV*  dev, MSD_U32    maskNum, MSD_U32    trunkMask);
typedef MSD_STATUS(*FMSD_gsysGetTrunkMaskTable)(MSD_QD_DEV*  dev, MSD_U32    maskNum, MSD_U32*    trunkMask);
typedef MSD_STATUS(*FMSD_gsysSetHashTrunk)(MSD_QD_DEV*  dev, MSD_BOOL    en);
typedef MSD_STATUS(*FMSD_gsysGetHashTrunk)(MSD_QD_DEV*  dev, MSD_BOOL*    en);
typedef MSD_STATUS(*FMSD_gsysSetTrunkRouting)(MSD_QD_DEV*  dev, MSD_U32    trunkId, MSD_U32    trunkRoute);
typedef MSD_STATUS(*FMSD_gsysGetTrunkRouting)(MSD_QD_DEV*  dev, MSD_U32    trunkId, MSD_U32*    trunkRoute);
typedef MSD_STATUS(*FMSD_gprtSetTrunkPort)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL    en, MSD_U32        trunkId);
typedef MSD_STATUS(*FMSD_gprtGetTrunkPort)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_BOOL*  en, MSD_U32 *     trunkId);

typedef MSD_STATUS(*FMSD_gqosSetPrioMapRule)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL   mode);
typedef MSD_STATUS(*FMSD_gqosGetPrioMapRule)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_BOOL*  mode);
typedef MSD_STATUS(*FMSD_gprtSetInitialPri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_INITIAL_PRI   mode);
typedef MSD_STATUS(*FMSD_gprtGetInitialPri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_INITIAL_PRI*  mode);
typedef MSD_STATUS(*FMSD_gprtSetDefQpri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U8   defque);
typedef MSD_STATUS(*FMSD_gprtGetDefQpri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U8*  defque);
typedef MSD_STATUS(*FMSD_gprtSetDefFpri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U8   defFPri);
typedef MSD_STATUS(*FMSD_gprtGetDefFpri)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_U8*  defFPri);
typedef MSD_STATUS(*FMSD_gprtSetIEEEPrioMap)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_IEEE_PRIORITY_TABLE_SET table, MSD_U8    entryPtr, MSD_IEEE_PRIORITY_TABLE_STRUCT* entryStruct);
typedef MSD_STATUS(*FMSD_gprtGetIEEEPrioMap)(MSD_QD_DEV*  dev, MSD_LPORT   Port, MSD_IEEE_PRIORITY_TABLE_SET table, MSD_U8    entryPtr, MSD_IEEE_PRIORITY_TABLE_STRUCT* entryStruct);
typedef MSD_STATUS(*FMSD_gprtSetCosToQueue)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 cosVal, MSD_U8 queue);
typedef MSD_STATUS(*FMSD_gprtGetCosToQueue)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 cosVal, MSD_U8* queue);
typedef MSD_STATUS(*FMSD_gprtSetCosToDscp)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 cosVal, MSD_U8 dscp);
typedef MSD_STATUS(*FMSD_gprtGetCosToDscp)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 cosVal, MSD_U8* dscp);
typedef MSD_STATUS(*FMSD_gprtRewriteDscp)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_BOOL en);

typedef MSD_STATUS(*FMSD_gpolicySetMgmtCtrl)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 point, MSD_U8 data);
typedef MSD_STATUS(*FMSD_gpolicyGetMgmtCtrl)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 point, MSD_U8 *data);
typedef MSD_STATUS(*FMSD_gpolicySetCpuDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT cpuPort);
typedef MSD_STATUS(*FMSD_gpolicyGetCpuDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT *cpuPort);
typedef MSD_STATUS(*FMSD_gpolicySetIngressMonitorDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT imd);
typedef MSD_STATUS(*FMSD_gpolicyGetIngressMonitorDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT *imd);
typedef MSD_STATUS(*FMSD_gpolicySetRsvd2CpuEnables)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16 enBits);
typedef MSD_STATUS(*FMSD_gpolicyGetRsvd2CpuEnables)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16 *enBits);
typedef MSD_STATUS(*FMSD_gpolicySetMGMTPri)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16 pri);
typedef MSD_STATUS(*FMSD_gpolicyGetMGMTPri)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16 *pri);
typedef MSD_STATUS(*FMSD_gpolicySetRouteDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT routePort);
typedef MSD_STATUS(*FMSD_gpolicyGetRouteDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT *routePort);
typedef MSD_STATUS(*FMSD_gpolicySetMirrorDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT mirrorPort);
typedef MSD_STATUS(*FMSD_gpolicyGetMirrorDest)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LPORT *mirrorPort);
typedef MSD_STATUS(*FMSD_gpolicySetIllegalIPAddr)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetIllegalIPAddr)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetDipDaMismatch)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetDipDaMismatch)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetZeroSIP)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetZeroSIP)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetMcTtlExceeded)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetMcTtlExceeded)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetUcTtlExceeded)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetUcTtlExceeded)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gprtSetPortTtlThreshold)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_TTL_THRESHOLD ttlThreshold);
typedef MSD_STATUS(*FMSD_gprtGetPortTtlThreshold)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_TTL_THRESHOLD *ttlThreshold);
typedef MSD_STATUS(*FMSD_gpolicySetIpHdrErr)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetIpHdrErr)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_ROUTE_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPFinUrgPsh)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPFinUrgPsh)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPFlagsSeqNum)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPFlagsSeqNum)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPSYN)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPSYN)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPHSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPHSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gprtSetPortTCPMinHdrSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 minTCPHdrSize);
typedef MSD_STATUS(*FMSD_gprtGetPortTCPMinHdrSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 *minTCPHdrSize);
typedef MSD_STATUS(*FMSD_gpolicySetICMPSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetICMPSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gprtSetPortICMPMaxSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16  maxICMPSize);
typedef MSD_STATUS(*FMSD_gprtGetPortICMPMaxSize)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U16  *maxICMPSize);
typedef MSD_STATUS(*FMSD_gpolicySetUDPPort)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetUDPPort)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPPort)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPPort)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPSynFin)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPSynFin)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetTCPFrag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetTCPFrag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetNearNonTPMR)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetNearNonTPMR)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetETag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetETag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicySetNonETag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION action);
typedef MSD_STATUS(*FMSD_gpolicyGetNonETag)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_POLICY_ACTION *action);
typedef MSD_STATUS(*FMSD_gpolicyDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_gstatsFlushAll)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gstatsFlushPort)(MSD_QD_DEV*  dev, MSD_LPORT   port);
typedef MSD_STATUS(*FMSD_gstatsGetPortCounter)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_STATS_COUNTERS    counter, MSD_U32*      statsData);
typedef MSD_STATUS(*FMSD_gstatsGetPortAllCounters)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_STATS_COUNTER_SET*    statsCounterSet);
typedef MSD_STATUS(*FMSD_gstatsGetHistogramMode)(MSD_QD_DEV*  dev, MSD_HISTOGRAM_MODE*    mode);
typedef MSD_STATUS(*FMSD_gstatsSetHistogramMode)(MSD_QD_DEV*  dev, MSD_HISTOGRAM_MODE        mode);
typedef MSD_STATUS(*FMSD_gmibDump)(MSD_QD_DEV*  dev, MSD_LPORT        port);

typedef MSD_STATUS(*FMSD_gprtSetQueueCtrl)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  point, MSD_U8  data);
typedef MSD_STATUS(*FMSD_gprtGetQueueCtrl)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_U8  point, MSD_U8* data);
typedef MSD_STATUS(*FMSD_gprtSetPortSched)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_SCHED_MODE mode);
typedef MSD_STATUS(*FMSD_gprtGetPortSched)(MSD_QD_DEV*  dev, MSD_LPORT  port, MSD_PORT_SCHED_MODE* mode);
typedef MSD_STATUS(*FMSD_gsysSetQoSWeight)(MSD_QD_DEV*  dev, MSD_QoS_WEIGHT* mode);
typedef MSD_STATUS(*FMSD_gsysGetQoSWeight)(MSD_QD_DEV*  dev, MSD_QoS_WEIGHT* mode);
typedef MSD_STATUS(*FMSD_gprtSetQoSWeight)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_QoS_WEIGHT* mode);
typedef MSD_STATUS(*FMSD_gprtGetQoSWeight)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_QoS_WEIGHT* mode);

typedef MSD_STATUS(*FMSD_gsysSetFloodBC)(MSD_QD_DEV*  dev, MSD_BOOL    en);
typedef MSD_STATUS(*FMSD_gsysGetFloodBC)(MSD_QD_DEV*  dev, MSD_BOOL*    en);
typedef MSD_STATUS(*FMSD_gsysSetMonitorMgmtCtrl)(MSD_QD_DEV*  dev, MSD_U8    point, MSD_U8    data);
typedef MSD_STATUS(*FMSD_gsysGetMonitorMgmtCtrl)(MSD_QD_DEV*  dev, MSD_U8    point, MSD_U8*    data);
typedef MSD_STATUS(*FMSD_gsysSetCPUDest)(MSD_QD_DEV*  dev, MSD_LPORT   port);
typedef MSD_STATUS(*FMSD_gsysGetCPUDest)(MSD_QD_DEV*  dev, MSD_LPORT*   port);
typedef MSD_STATUS(*FMSD_gsysSetIngressMonitorDest)(MSD_QD_DEV*  dev, MSD_LPORT   port);
typedef MSD_STATUS(*FMSD_gsysGetIngressMonitorDest)(MSD_QD_DEV*  dev, MSD_LPORT*   port);
typedef MSD_STATUS(*FMSD_gsysSetEgressMonitorDest)(MSD_QD_DEV*  dev, MSD_LPORT   port);
typedef MSD_STATUS(*FMSD_gsysGetEgressMonitorDest)(MSD_QD_DEV*  dev, MSD_LPORT*   port);
typedef MSD_STATUS(*FMSD_gsysSetRsvd2CpuEnables)(MSD_QD_DEV*  dev, MSD_U16    enBits);
typedef MSD_STATUS(*FMSD_gsysGetRsvd2CpuEnables)(MSD_QD_DEV*  dev, MSD_U16*    enBits);
typedef MSD_STATUS(*FMSD_gsysSetMGMTPri)(MSD_QD_DEV*  dev, MSD_U16   pri);
typedef MSD_STATUS(*FMSD_gsysGetMGMTPri)(MSD_QD_DEV*  dev, MSD_U16*    pri);
typedef MSD_STATUS(*FMSD_gsysSetSMISetup)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U16   data); 
typedef MSD_STATUS(*FMSD_gsysGetSMISetup)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U16*    data); 
typedef MSD_STATUS(*FMSD_gsysGlobal1RegDump)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gsysGlobal2RegDump)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gsysSetRMUMode)(MSD_QD_DEV* dev, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gsysGetRMUMode)(MSD_QD_DEV* dev, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_gsysQbvWrite)(MSD_QD_DEV* dev, MSD_U8 qbvPort, MSD_U8 qbvRegAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gsysQbvRead)(MSD_QD_DEV* dev, MSD_U8 qbvPort, MSD_U8 qbvRegAddr, MSD_U16 *data);

typedef MSD_STATUS(*FMSD_gqavSetPortQpriXRate)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 queue, MSD_U32 rate);
typedef MSD_STATUS(*FMSD_gqavGetPortQpriXRate)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 queue, MSD_U32* rate);
typedef MSD_STATUS(*FMSD_gqavSetPortQpriXHiLimit)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 queue, MSD_U16 hiLimit);
typedef MSD_STATUS(*FMSD_gqavGetPortQpriXHiLimit)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U8 queue, MSD_U16* hiLimit);

typedef MSD_STATUS(*FMSD_gptpSetIntEnable)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_PORT_INTERRUPT_EN* interruptEn);
typedef MSD_STATUS(*FMSD_gptpGetIntEnable)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_PORT_INTERRUPT_EN* interruptEn);
typedef MSD_STATUS(*FMSD_gptpSetProtPTPEn)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_BOOL en);
typedef MSD_STATUS(*FMSD_gptpGetProtPTPEn)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_BOOL *en);
typedef MSD_STATUS(*FMSD_gptpSetPortConfig)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_PORT_CONFIG* ptpData);
typedef MSD_STATUS(*FMSD_gptpGetPortConfig)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_PORT_CONFIG* ptpData);
typedef MSD_STATUS(*FMSD_gptpGetTimeStamp)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_TIME timeToRead, MSD_PTP_TS_STATUS *ptpStatus);
typedef MSD_STATUS(*FMSD_gptpGetTSValidSt)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_TIME timeToRead, MSD_BOOL *isValid);
typedef MSD_STATUS(*FMSD_gptpResetTimeStamp)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_PTP_TIME timeToReset);
typedef MSD_STATUS(*FMSD_gptpSetGlobalConfig)(MSD_QD_DEV* dev, MSD_PTP_GLOBAL_CONFIG *ptpData);
typedef MSD_STATUS(*FMSD_gptpGetGlobalConfig)(MSD_QD_DEV* dev, MSD_PTP_GLOBAL_CONFIG *ptpData);
typedef MSD_STATUS(*FMSD_gptpGetIntStatus)(MSD_QD_DEV* dev, MSD_PTP_INTERRUPT_STATUS *ptpIntSt);
typedef MSD_STATUS(*FMSD_gptpSetMeanPathDelay)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 delay);
typedef MSD_STATUS(*FMSD_gptpGetMeanPathDelay)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 *delay);
typedef MSD_STATUS(*FMSD_gptpSetIgrPathDelayAsym)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 asym);
typedef MSD_STATUS(*FMSD_gptpGetIgrPathDelayAsym)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 *asym);
typedef MSD_STATUS(*FMSD_gptpSetEgrPathDelayAsym)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 asym);
typedef MSD_STATUS(*FMSD_gptpGetEgrPathDelayAsym)(MSD_QD_DEV* dev, MSD_LPORT port, MSD_U16 *asym);
typedef MSD_STATUS(*FMSD_gptpSetMode)(MSD_QD_DEV* dev, MSD_PTP_MODE_CONFIG *mode);
typedef MSD_STATUS(*FMSD_gptpGetMode)(MSD_QD_DEV* dev, MSD_PTP_MODE_CONFIG *mode);
typedef MSD_STATUS(*FMSD_gptpSetMasterSlave)(MSD_QD_DEV* dev, MSD_PTP_MASTER_SLAVE role);
typedef MSD_STATUS(*FMSD_gptpGetMasterSlave)(MSD_QD_DEV* dev, MSD_PTP_MASTER_SLAVE *role);
typedef MSD_STATUS(*FMSD_gptpGetTODBusyBitStatus)(MSD_QD_DEV* dev, MSD_BOOL *busyStatus);
typedef MSD_STATUS(*FMSD_gptpTODStoreCompensation)(MSD_QD_DEV* dev, MSD_U8 timeArrayIndex, MSD_PTP_TIME_ARRAY *timeArray);
typedef MSD_STATUS(*FMSD_gptpTODStoreAll)(MSD_QD_DEV* dev, MSD_U8 timeArrayIndex, MSD_PTP_TIME_ARRAY *timeArray);
typedef MSD_STATUS(*FMSD_gptpTODCaptureAll)(MSD_QD_DEV* dev, MSD_U8 timeArrayIndex, MSD_PTP_TIME_ARRAY *timeArray);
typedef MSD_STATUS(*FMSD_gptpSetPulse)(MSD_QD_DEV* dev, MSD_PTP_PULSE_STRUCT *pulsePara);
typedef MSD_STATUS(*FMSD_gptpGetPulse)(MSD_QD_DEV* dev, MSD_PTP_PULSE_STRUCT *pulsePara);
typedef MSD_STATUS(*FMSD_gtaiGetPTPGlobalTime)(MSD_QD_DEV* dev, MSD_U32 *ptpTime);
typedef MSD_STATUS(*FMSD_gptpGetPortReg)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U32 regOffset, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_gptpSetPortReg)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U32 regOffset, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gptpGetGlobalReg)(MSD_QD_DEV* dev, MSD_U32 regOffset, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_gptpSetGlobalReg)(MSD_QD_DEV* dev, MSD_U32 regOffset, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gptpGetTaiReg)(MSD_QD_DEV* dev, MSD_U32 regOffset, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_gptpSetTaiReg)(MSD_QD_DEV* dev, MSD_U32 regOffset, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gptpSet1588v2Config)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 timeArrayIndex, MSD_PTP_MODE ptpMode, MSD_PTP_STEP ptpStep);
typedef MSD_STATUS(*FMSD_gptpSet8021asConfig)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 timeArrayIndex, MSD_PTP_RELAY_MODE relayMode);
typedef MSD_STATUS(*FMSD_gptpSet8021asRevConfig)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 timeArrayIndex, MSD_PTP_RELAY_MODE relayMode);
typedef MSD_STATUS(*FMSD_gptpSetPreemptTsSel)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_BOOL tsSelect);
typedef MSD_STATUS(*FMSD_gptpGetPreemptTsSel)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_BOOL *tsSelect);
typedef MSD_STATUS(*FMSD_gptpSetPreemptCtrl)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 preemptCtrl);
typedef MSD_STATUS(*FMSD_gptpGetPreemptCtrl)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 *preemptCtrl);
typedef MSD_STATUS(*FMSD_gtaiSetTrigGenTimerSel)(MSD_QD_DEV* dev, MSD_U8 trigGenTimerSel);
typedef MSD_STATUS(*FMSD_gtaiGetTrigGenTimerSel)(MSD_QD_DEV* dev, MSD_U8 *trigGenTimerSel);

typedef MSD_STATUS(*FMSD_gtcamFlushAll)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gtcamFlushEntry)(MSD_QD_DEV*  dev, MSD_U32  tcamPointer);
typedef MSD_STATUS(*FMSD_gtcamLoadEntry)(MSD_QD_DEV*  dev, MSD_U32  tcamPointer, MSD_TCAM_DATA*   tcamData);
typedef MSD_STATUS(*FMSD_gtcamReadTCAMData)(MSD_QD_DEV*  dev, MSD_U32  tcamPointer, MSD_TCAM_DATA*   tcamData);
typedef MSD_STATUS(*FMSD_gtcamGetNextTCAMData)(MSD_QD_DEV*  dev, MSD_U32*   tcamPointer, MSD_TCAM_DATA*   tcamData);
typedef MSD_STATUS(*FMSD_gtcamFindEntry)(MSD_QD_DEV*  dev, MSD_U32   tcamPointer, MSD_TCAM_DATA*   tcamData, MSD_BOOL* found);
typedef MSD_STATUS(*FMSD_gtcamSetMode)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_TCAM_MODE mode);
typedef MSD_STATUS(*FMSD_gtcamGetMode)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_TCAM_MODE *mode);
typedef MSD_STATUS(*FMSD_gtcamAddEntryPort)(MSD_QD_DEV* dev, MSD_U32 tcamPointer, MSD_LPORT portNum);
typedef MSD_STATUS(*FMSD_gtcamRemoveEntryPort)(MSD_QD_DEV* dev, MSD_U32 tcamPointer, MSD_LPORT portNum);
typedef MSD_STATUS(*FMSD_gtcamEgrFlushEntry)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U32  tcamPointer);
typedef MSD_STATUS(*FMSD_gtcamEgrFlushEntryAllPorts)(MSD_QD_DEV*  dev, MSD_U32  tcamPointer);
typedef MSD_STATUS(*FMSD_gtcamEgrLoadEntry)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U32  tcamPointer, MSD_TCAM_EGR_DATA*  tcamData);
typedef MSD_STATUS(*FMSD_gtcamEgrGetNextTCAMData)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U32*   tcamPointer, MSD_TCAM_EGR_DATA*  tcamData);
typedef MSD_STATUS(*FMSD_gtcamEgrReadTCAMData)(MSD_QD_DEV*  dev, MSD_LPORT   port, MSD_U32  tcamPointer, MSD_TCAM_EGR_DATA*  tcamData);
typedef MSD_STATUS(*FMSD_gtcamDump)(MSD_QD_DEV*  dev);
typedef MSD_STATUS(*FMSD_gtcamAdvConfig)(MSD_QD_DEV* dev, MSD_TCAM_PKT_TYPE pktType, MSD_U32 tcamPointer, MSD_U32 tcamPointer2,
    MSD_TCAM_ENT_KEY *keyMaskPtr, MSD_TCAM_ENT_KEY *keyPtr, MSD_TCAM_ENT_PKT_UNT *maskPtr, MSD_TCAM_ENT_PKT_UNT *patternPtr, MSD_TCAM_ENT_ACT *actionPtr, MSD_BOOL *entry2Used);

typedef MSD_STATUS(*FMSD_gtcamRCLoadEntry)(MSD_QD_DEV* dev, MSD_U32  rcEntryNum, MSD_TCAM_RC_DATA *rcData);
typedef MSD_STATUS(*FMSD_gtcamRCFlushAllEntry)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gtcamRCFlushEntry)(MSD_QD_DEV* dev, MSD_U32  rcEntryNum);
typedef MSD_STATUS(*FMSD_gtcamRCReadEntry)(MSD_QD_DEV* dev, MSD_U32  rcEntryNum, MSD_TCAM_RC_DATA *rcData);
typedef MSD_STATUS(*FMSD_gtcamRCGetNextEntry)(MSD_QD_DEV* dev, MSD_U32  *rcEntryNum, MSD_TCAM_RC_DATA *rcData);
typedef MSD_STATUS(*FMSD_gtcamRCFindEntry)(MSD_QD_DEV* dev, MSD_U32  rcEntryNum, MSD_TCAM_RC_DATA *rcData, MSD_BOOL  *found);
typedef MSD_STATUS(*FMSD_gtcamRCEntryDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_gtcamRCCompLoad)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_TCAM_RC_COMP_DATA *tcamRCCompData);
typedef MSD_STATUS(*FMSD_gtcamRCCompFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gtcamRCCompFlushPort)(MSD_QD_DEV* dev, MSD_LPORT portNum);
typedef MSD_STATUS(*FMSD_gtcamRCCompRead)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_TCAM_RC_COMP_DATA *tcamRCCompData);

typedef MSD_STATUS(*FMSD_garpLoadMcEntry)(MSD_QD_DEV* dev, MSD_U32  arpEntryNum, MSD_ARP_MC_DATA  *arpMcData);
typedef MSD_STATUS(*FMSD_garpLoadUcEntry)(MSD_QD_DEV* dev, MSD_U32  arpEntryNum, MSD_ARP_UC_DATA  *arpUcData);
typedef MSD_STATUS(*FMSD_garpFlushEntry)(MSD_QD_DEV* dev, MSD_U32  arpEntryNum);
typedef MSD_STATUS(*FMSD_garpFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_garpReadEntry)(MSD_QD_DEV* dev, MSD_U32  arpEntryNum, MSD_ARP_DATA  *arpData);
typedef MSD_STATUS(*FMSD_garpGetNextEntry)(MSD_QD_DEV* dev, MSD_U32  *arpEntryNum, MSD_ARP_DATA  *arpData);

typedef MSD_STATUS(*FMSD_grmuGetID)(MSD_QD_DEV *dev, MSD_U16 *id);
typedef MSD_STATUS(*FMSD_grmuAtuDump)(MSD_QD_DEV *dev, MSD_U32 *startAddr, MSD_U32 *numOfEntry, MSD_ATU_ENTRY **atuEntry);
typedef MSD_STATUS(*FMSD_grmuEcidDump)(MSD_QD_DEV *dev, MSD_U32 *startAddr, MSD_U32 *numOfEntry, MSD_ECID_ENTRY **ecidEntry);
typedef MSD_STATUS(*FMSD_grmuMibDump)(MSD_QD_DEV *dev, MSD_BOOL enClearOnRead, MSD_LPORT port, MSD_STATS_COUNTER_SET_LEGACY *statsCounterSet);
typedef MSD_STATUS(*FMSD_grmuMib2Dump)(MSD_QD_DEV *dev, MSD_BOOL enClearOnRead, MSD_LPORT port, MSD_STATS_COUNTER_SET *statsCounterSet);
typedef MSD_STATUS(*FMSD_grmuReadRegister)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_grmuWriteRegister)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_grmuMultiRegAccess)(MSD_QD_DEV *dev, MSD_RegCmd **regCmdList, MSD_U32 nCmd);
typedef MSD_STATUS(*FMSD_grmuTCAMDump)(MSD_QD_DEV *dev, MSD_U32 *startAddr, MSD_U32 *numOfEntry, MSD_TCAM_DATA **tcamEntry);
typedef MSD_STATUS(*FMSD_grmuTCAMWrite)(MSD_QD_DEV *dev, MSD_U32 tcamPointer, MSD_TCAM_DATA *tcamEntry);
typedef MSD_STATUS(*FMSD_grmuTCAMRead)(MSD_QD_DEV *dev, MSD_U32 tcamPointer, MSD_TCAM_DATA *tcamEntry);
typedef MSD_STATUS(*FMSD_grmuRegDump)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_RegList *dataList);

typedef MSD_STATUS(*FMSD_msdSetSMIC45PhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 phyAddr, MSD_U16 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_msdGetSMIC45PhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 phyAddr, MSD_U16 regAddr, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_msdSetSMIPhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_msdGetSMIPhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_msdSetSMIPhyXMDIOReg)(MSD_QD_DEV *dev, MSD_U8 portNum, MSD_U8 devAddr, MSD_U16 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_msdGetSMIPhyXMDIOReg)(MSD_QD_DEV *dev, MSD_U8 portNum, MSD_U8 devAddr, MSD_U16 regAddr, MSD_U16* data);
typedef MSD_STATUS(*FMSD_msdSetSMIExtPhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_msdGetSMIExtPhyReg)(MSD_QD_DEV *dev, MSD_U8 devAddr, MSD_U8 regAddr, MSD_U16 *data);
typedef MSD_STATUS(*FMSD_msdSetSMIExtPhyXMDIOReg)(MSD_QD_DEV *dev, MSD_U8 portNum, MSD_U8 devAddr, MSD_U16 regAddr, MSD_U16 data);
typedef MSD_STATUS(*FMSD_msdGetSMIExtPhyXMDIOReg)(MSD_QD_DEV *dev, MSD_U8 portNum, MSD_U8 devAddr, MSD_U16 regAddr, MSD_U16* data);

typedef MSD_STATUS(*FMSD_gledForceOn)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 ledNum);
typedef MSD_STATUS(*FMSD_gledForceOff)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 ledNum);
typedef MSD_STATUS(*FMSD_gledLinkAct)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 ledNum);
typedef MSD_STATUS(*FMSD_gledModeSet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 ledNum, MSD_U8 mode);
typedef MSD_STATUS(*FMSD_gledModeGet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 ledNum, MSD_U8* mode);
typedef MSD_STATUS(*FMSD_gledBlinkRateSet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LED_BLINK_RATE rate);
typedef MSD_STATUS(*FMSD_gledBlinkRateGet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LED_BLINK_RATE* rate);
typedef MSD_STATUS(*FMSD_gledPulseStretchSet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LED_STRETCH_DURATION duration);
typedef MSD_STATUS(*FMSD_gledPulseStretchGet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_LED_STRETCH_DURATION* duration);
typedef MSD_STATUS(*FMSD_gledSpecialModeSet)(MSD_QD_DEV* dev, MSD_LED_SPECIAL_MODE	mode, MSD_U16  portVect);
typedef MSD_STATUS(*FMSD_gledSpecialModeGet)(MSD_QD_DEV* dev, MSD_LED_SPECIAL_MODE	mode, MSD_U16*  portVect);
typedef MSD_STATUS(*FMSD_gledCtrlSet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 pointer, MSD_U16 data);
typedef MSD_STATUS(*FMSD_gledCtrlGet)(MSD_QD_DEV* dev, MSD_LPORT portNum, MSD_U8 pointer, MSD_U16* data);
typedef MSD_STATUS(*FMSD_gledDump)(MSD_QD_DEV* dev);

typedef MSD_STATUS(*FMSD_gdevIntEnableSet)(MSD_QD_DEV* dev, MSD_U16 en);
typedef MSD_STATUS(*FMSD_gdevIntEnableGet)(MSD_QD_DEV* dev, MSD_U16 *en);
typedef MSD_STATUS(*FMSD_gdevIntStatusGet)(MSD_QD_DEV* dev, MSD_U16 *stat);
typedef MSD_STATUS(*FMSD_gactiveIntEnableSet)(MSD_QD_DEV* dev, MSD_U16 en);
typedef MSD_STATUS(*FMSD_gactiveIntEnableGet)(MSD_QD_DEV* dev, MSD_U16 *en);
typedef MSD_STATUS(*FMSD_gactiveIntStatusGet)(MSD_QD_DEV* dev, MSD_U16 *stat);

typedef MSD_STATUS(*FMSD_gqcrSetGrpId)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U16 atsEntryGrpId);
typedef MSD_STATUS(*FMSD_gqcrGetGrpId)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U16* atsEntryGrpId);
typedef MSD_STATUS(*FMSD_gqcrSetCbsLimit)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U32 atsCbsLimit);
typedef MSD_STATUS(*FMSD_gqcrGetCbsLimit)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U32* atsCbsLimit);
typedef MSD_STATUS(*FMSD_gqcrSetRateLimit)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U32 rateLimit);
typedef MSD_STATUS(*FMSD_gqcrGetRateLimit)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U32* rateLimit);
typedef MSD_STATUS(*FMSD_gqcrSetMaxResidenceTime)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U16 maxTimeScale, MSD_U16 maxResidenceTime);
typedef MSD_STATUS(*FMSD_gqcrGetMaxResidenceTime)(MSD_QD_DEV* dev, MSD_U16 atsEntryId, MSD_U16* maxTimeScale, MSD_U16* maxResidenceTime);
typedef MSD_STATUS(*FMSD_gqcrFlushOneEntry)(MSD_QD_DEV* dev, MSD_U16 atsEntryId);
typedef MSD_STATUS(*FMSD_gqcrFlushAllEntry)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gqcrSetQueueEnables)(MSD_QD_DEV* dev, MSD_U16 port, MSD_U16 queue, MSD_BOOL status);
typedef MSD_STATUS(*FMSD_gqcrGetQueueEnables)(MSD_QD_DEV* dev, MSD_U16 port, MSD_U16 queue, MSD_BOOL* status);

typedef MSD_STATUS(*FMSD_gfrerSeqGenFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerSeqGenFlushEntry)(MSD_QD_DEV* dev, MSD_U8	seqGenId);
typedef MSD_STATUS(*FMSD_gfrerSeqGenRead)(MSD_QD_DEV* dev, MSD_U8 seqGenId, MSD_FRER_SEQGEN *seqGenData);
typedef MSD_STATUS(*FMSD_gfrerSeqGenLoad)(MSD_QD_DEV* dev, MSD_FRER_SEQGEN *SeqGenData);

typedef MSD_STATUS(*FMSD_gfrerIndvRcvyFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerIndvRcvyFlushEntry)(MSD_QD_DEV* dev, MSD_U8 indvRcvyId);
typedef MSD_STATUS(*FMSD_gfrerIndvRcvyRead)(MSD_QD_DEV* dev, MSD_U8 indvRcvyId, MSD_FRER_INDVRCVY *indvRcvyData, MSD_U16* indvRcvyResets);
typedef MSD_STATUS(*FMSD_gfrerIndvRcvyLoad)(MSD_QD_DEV* dev, MSD_FRER_INDVRCVY *indvRcvyData);
typedef MSD_STATUS(*FMSD_gfrerIndvRcvyGetNext)(MSD_QD_DEV* dev, MSD_U8 indvRcvyId, MSD_FRER_INDVRCVY *indvRcvyData, MSD_U16* indvRcvyResets);
typedef MSD_STATUS(*FMSD_gfrerIndvRcvyReadClear)(MSD_QD_DEV* dev, MSD_U8 indvRcvyId, MSD_FRER_INDVRCVY *indvRcvyData, MSD_U16* indvRcvyResets);

typedef MSD_STATUS(*FMSD_gfrerSeqRcvyFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyFlushEntry)(MSD_QD_DEV* dev, MSD_U8 seqRcvyId);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyRead)(MSD_QD_DEV* dev, MSD_U8 seqRcvyId, MSD_FRER_SEQRCVY *seqRcvyData, MSD_U16* seqRcvyResets, MSD_U16* latErrResets);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyLoad)(MSD_QD_DEV* dev, MSD_FRER_SEQRCVY *seqRcvyData);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyGetNext)(MSD_QD_DEV* dev, MSD_U8 seqRcvyId, MSD_FRER_SEQRCVY *seqRcvyData, MSD_U16* seqRcvyResets, MSD_U16* latErrResets);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyGetViolation)(MSD_QD_DEV* dev, MSD_U8 seqRcvyId, MSD_FRER_SEQRCVY *seqRcvyData, MSD_U16* seqRcvyResets, MSD_U16* latErrResets);
typedef MSD_STATUS(*FMSD_gfrerSeqRcvyReadClear)(MSD_QD_DEV* dev, MSD_U8 seqRcvyId, MSD_FRER_SEQRCVY *seqRcvyData, MSD_U16* seqRcvyResets, MSD_U16* latErrResets);

typedef MSD_STATUS(*FMSD_gfrerBank0SCtrFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerB0SCtrFlushEntry)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerBank0SCtrRead)(MSD_QD_DEV* dev, MSD_U8 bk0Id, MSD_FRER_BANK0_CONFIG *bk0ConfigData, MSD_FRER_BANK0_COUNTER *bk0SCtrData);
typedef MSD_STATUS(*FMSD_gfrerBank0SCtrLoad)(MSD_QD_DEV* dev, MSD_FRER_BANK0_CONFIG *bk0ConfigData);
typedef MSD_STATUS(*FMSD_gfrerBank0SCtrGetNext)(MSD_QD_DEV* dev, MSD_U8 bk0Id, MSD_FRER_BANK0_CONFIG *bk0ConfigData, MSD_FRER_BANK0_COUNTER *bk0SCtrData);
typedef MSD_STATUS(*FMSD_gfrerBank0SCtrReadClear)(MSD_QD_DEV* dev, MSD_U8 bk0Id, MSD_FRER_BANK0_CONFIG *bk0ConfigData, MSD_FRER_BANK0_COUNTER *bk0SCtrData);

typedef MSD_STATUS(*FMSD_gfrerBank1SCtrFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerBank1SCtrFlushEntry)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gfrerBank1SCtrRead)(MSD_QD_DEV* dev, MSD_U8 bk1Id, MSD_FRER_BANK1_CONFIG *bk1ConfigData, MSD_FRER_BANK1_COUNTER *bk1SCtrData);
typedef MSD_STATUS(*FMSD_gfrerBank1SCtrLoad)(MSD_QD_DEV* dev, MSD_FRER_BANK1_CONFIG *bk1ConfigData);
typedef MSD_STATUS(*FMSD_gfrerBank1SCtrGetNext)(MSD_QD_DEV* dev, MSD_U8 bk1Id, MSD_FRER_BANK1_CONFIG *bk1ConfigData, MSD_FRER_BANK1_COUNTER *bk1SCtrData);
typedef MSD_STATUS(*FMSD_gfrerBank1SCtrReadClear)(MSD_QD_DEV* dev, MSD_U8 bk1Id, MSD_FRER_BANK1_CONFIG *bk1ConfigData, MSD_FRER_BANK1_COUNTER *bk1SCtrData);

typedef MSD_STATUS(*FMSD_gqciStreamFiltFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gqciStreamFiltSet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U16 maxSDUSize, MSD_U8 blockEn);
typedef MSD_STATUS(*FMSD_gqciStreamFiltBlockStatSet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U8 blockStatus);
typedef MSD_STATUS(*FMSD_gqciStreamFiltBlockStatGet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U8 *blockStatus);
typedef MSD_STATUS(*FMSD_gqciStreamFiltEnAts)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U8 atsId);
typedef MSD_STATUS(*FMSD_gqciStreamFiltEnGate)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U8 gateId);
typedef MSD_STATUS(*FMSD_gqciStreamFiltMatchCtrGet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U16 *counter);
typedef MSD_STATUS(*FMSD_gqciStreamFiltFilterCtrGet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U16 *counter);
typedef MSD_STATUS(*FMSD_gqciStreamGateFiltCtrGet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U16 *counter);
typedef MSD_STATUS(*FMSD_gqciFlowMeterFiltCtrGet)(MSD_QD_DEV* dev, MSD_U8 streamFilterId, MSD_U16* counter);

typedef MSD_STATUS(*FMSD_gqciStreamGateFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gqciStreamGateDefStateSet)(MSD_QD_DEV* dev, MSD_U8 streamGateId, MSD_U8 defGateState, MSD_U8 defGateIPVEn, MSD_U8 defGateIPV);
typedef MSD_STATUS(*FMSD_gqciStreamGateScaleSet)(MSD_QD_DEV* dev, MSD_U8 streamGateId, MSD_U8 timeScale, MSD_U8 byteScale);
typedef MSD_STATUS(*FMSD_gqciStreamGateTimeGenIdSet)(MSD_QD_DEV* dev, MSD_U8 streamGateId, MSD_U8 streamGateTimeGenId);
typedef MSD_STATUS(*FMSD_gqciStreamGateEntryAdd)(MSD_QD_DEV* dev, MSD_U8 streamGateId, MSD_U8 pointer, MSD_STREAM_GATE_ENTRY *streamGateEntry);

typedef MSD_STATUS(*FMSD_gqciStreamGateTimeGenFlush)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gqciStreamGateTimeGenSet)(MSD_QD_DEV* dev, MSD_U8 streamGateGenId, MSD_U32 gateCycleTime, MSD_U64 gateBaseTime);

typedef MSD_STATUS(*FMSD_gqciFlowMeterFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gqciFlowMeterCoupleFlagSet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 flag);
typedef MSD_STATUS(*FMSD_gqciFlowMeterCoupleFlagGet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 *flag);
typedef MSD_STATUS(*FMSD_gqciFlowMeterDropOnYelSet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 enable);
typedef MSD_STATUS(*FMSD_gqciFlowMeterDropOnYelGet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 *enable);
typedef MSD_STATUS(*FMSD_gqciFlowMeterRedBlockEnSet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 enable);
typedef MSD_STATUS(*FMSD_gqciFlowMeterRedBlockEnGet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 *enable);
typedef MSD_STATUS(*FMSD_gqciFlowMeterRedBlkStatSet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 status);
typedef MSD_STATUS(*FMSD_gqciFlowMeterRedBlkStatGet)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U8 *status);
typedef MSD_STATUS(*FMSD_gqciFlowMeterAdvConfig)(MSD_QD_DEV* dev, MSD_U8 flowMeterId, MSD_U32 tgtRate, MSD_U32 bstSize, MSD_U8 countMode);

typedef MSD_STATUS(*FMSD_gmacsecRead)(MSD_QD_DEV dev, MSD_U32 regAddr, MSD_U32* data);
typedef MSD_STATUS(*FMSD_gmacsecWrite)(MSD_QD_DEV dev, MSD_U32 regAddr, MSD_U32 data);
typedef MSD_STATUS(*FMSD_gmacsecFlushAll)(MSD_QD_DEV* dev);
typedef MSD_STATUS(*FMSD_gmacsecGetEvent)(MSD_QD_DEV* dev, MSD_MSEC_EVENT* event);
typedef MSD_STATUS(*FMSD_gmacsecCreateRule)(MSD_QD_DEV* dev, MSD_U8 ruleIndex, MSD_MSEC_RULE* rule);
typedef MSD_STATUS(*FMSD_gmacsecDeleteRule)(MSD_QD_DEV* dev, MSD_U8 ruleIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetRule)(MSD_QD_DEV* dev, MSD_U8 ruleIndex, MSD_MSEC_RULE* rule);
typedef MSD_STATUS(*FMSD_gmacsecCreateTxSecY)(MSD_QD_DEV* dev, MSD_U8 ruleIndex, MSD_SECY_EGRESS* secY);
typedef MSD_STATUS(*FMSD_gmacsecCreateRxSecY)(MSD_QD_DEV* dev, MSD_U8 ruleIndex, MSD_SECY_INGRESS* secY);
typedef MSD_STATUS(*FMSD_gmacsecDeleteTxSecY)(MSD_QD_DEV* dev, MSD_U8 secYIndex);
typedef MSD_STATUS(*FMSD_gmacsecDeleteRxSecY)(MSD_QD_DEV* dev, MSD_U8 secYIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetTxSecY)(MSD_QD_DEV* dev, MSD_U8 secYIndex, MSD_SECY_EGRESS* secY);
typedef MSD_STATUS(*FMSD_gmacsecGetRxSecY)(MSD_QD_DEV* dev, MSD_U8 secYIndex, MSD_SECY_INGRESS* secY);
typedef MSD_STATUS(*FMSD_gmacsecCreateRxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex, MSD_MSEC_RX_SC* rxSC);
typedef MSD_STATUS(*FMSD_gmacsecDeleteRxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetRxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex, MSD_MSEC_RX_SC* rxSC);
typedef MSD_STATUS(*FMSD_gmacsecCreateRxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex, MSD_MSEC_RX_SA* rxSA);
typedef MSD_STATUS(*FMSD_gmacsecDeleteRxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetRxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex, MSD_MSEC_RX_SA* rxSA);
typedef MSD_STATUS(*FMSD_gmacsecCreateTxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex, MSD_MSEC_TX_SC* txSC);
typedef MSD_STATUS(*FMSD_gmacsecDeleteTxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetTxSC)(MSD_QD_DEV* dev, MSD_U8 scIndex, MSD_MSEC_TX_SC* txSC);
typedef MSD_STATUS(*FMSD_gmacsecCreateTxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex, MSD_MSEC_TX_SA* txSA);
typedef MSD_STATUS(*FMSD_gmacsecDeleteTxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex);
typedef MSD_STATUS(*FMSD_gmacsecGetTxSA)(MSD_QD_DEV* dev, MSD_U8 saIndex, MSD_MSEC_TX_SA* txSA);
typedef MSD_STATUS(*FMSD_gmacsecGetRXSCStates)(MSD_QD_DEV* dev, MSD_U8 scCounterIndex, MSD_AUTO_CLEAR autoClear, MSD_MSEC_RX_SC_COUNTER* states);
typedef MSD_STATUS(*FMSD_gmacsecGetRXSecYStates)(MSD_QD_DEV* dev, MSD_U8 secYCounterIndex, MSD_AUTO_CLEAR autoClear, MSD_MSEC_RX_SECY_COUNTER* states);
typedef MSD_STATUS(*FMSD_gmacsecGetTXSCStates)(MSD_QD_DEV* dev, MSD_U8 scCounterIndex, MSD_AUTO_CLEAR autoClear, MSD_MSEC_TX_SC_COUNTER* states);
typedef MSD_STATUS(*FMSD_gmacsecGetTXSecYStates)(MSD_QD_DEV* dev, MSD_U8 secYCounterIndex, MSD_AUTO_CLEAR autoClear, MSD_MSEC_TX_SECY_COUNTER* states);
typedef MSD_STATUS(*FMSD_gmacsecEnableEgressSet)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_BOOL isEnable);
typedef MSD_STATUS(*FMSD_gmacsecEnableIngressSet)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_BOOL isEnable); 
typedef MSD_STATUS(*FMSD_gmacsecEnableEgressGet)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_BOOL* isEnable);
typedef MSD_STATUS(*FMSD_gmacsecEnableIngressGet)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_BOOL* isEnable);
typedef MSD_STATUS(*FMSD_gmacsecSetTxPNThreshold)(MSD_QD_DEV* dev, MSD_U32 pnThr);
typedef MSD_STATUS(*FMSD_gmacsecGetTxPNThreshold)(MSD_QD_DEV* dev, MSD_U32* pnThr);
typedef MSD_STATUS(*FMSD_gmacsecSetTxXPNThreshold)(MSD_QD_DEV* dev, MSD_U64 xpnThr);
typedef MSD_STATUS(*FMSD_gmacsecGetTxXPNThreshold)(MSD_QD_DEV* dev, MSD_U64* xpnThr);
typedef MSD_STATUS(*FMSD_gmacsecSetRxDefaultSCI)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_U64 defaultRxSCI);
typedef MSD_STATUS(*FMSD_gmacsecGetRxDefaultSCI)(MSD_QD_DEV* dev, MSD_U16 portNum, MSD_U64* defaultRxSCI);
typedef MSD_STATUS(*FMSD_gmacsecSetTxSCIntCtrl)(MSD_QD_DEV* dev, MSD_U8 index, MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl);
typedef MSD_STATUS(*FMSD_gmacsecGetTxSCIntCtrl)(MSD_QD_DEV* dev, MSD_U8 index, MSD_MACSEC_INT_CONTROL_TXSC* txscIntCtrl);
typedef MSD_STATUS(*FMSD_gmacsecGetNextTxSCIntStatus)(MSD_QD_DEV* dev, MSD_U8 index, MSD_BOOL isAUTOClear, MSD_U8* nextIndex, MSD_MACSEC_INT_TXSC* txscInt);

typedef struct
{
	FMSD_gmacsecRead gmacsecRead;
	FMSD_gmacsecWrite gmacsecWrite;
	FMSD_gmacsecFlushAll gmacsecFlushAll;
	FMSD_gmacsecGetEvent gmacsecGetEvent;
	FMSD_gmacsecCreateRule gmacsecCreateRule;
	FMSD_gmacsecDeleteRule gmacsecDeleteRule;
	FMSD_gmacsecGetRule gmacsecGetRule;
	FMSD_gmacsecCreateTxSecY gmacsecCreateTxSecY;
	FMSD_gmacsecCreateRxSecY gmacsecCreateRxSecY;
	FMSD_gmacsecDeleteTxSecY gmacsecDeleteTxSecY;
	FMSD_gmacsecDeleteRxSecY gmacsecDeleteRxSecY;
	FMSD_gmacsecGetTxSecY gmacsecGetTxSecY;
	FMSD_gmacsecGetRxSecY gmacsecGetRxSecY;
	FMSD_gmacsecCreateRxSC gmacsecCreateRxSC;
	FMSD_gmacsecDeleteRxSC gmacsecDeleteRxSC;
	FMSD_gmacsecGetRxSC gmacsecGetRxSC;
	FMSD_gmacsecCreateRxSA gmacsecCreateRxSA;
	FMSD_gmacsecDeleteRxSA gmacsecDeleteRxSA;
	FMSD_gmacsecGetRxSA gmacsecGetRxSA;
	FMSD_gmacsecCreateTxSC gmacsecCreateTxSC;
	FMSD_gmacsecDeleteTxSC gmacsecDeleteTxSC;
	FMSD_gmacsecGetTxSC gmacsecGetTxSC;
	FMSD_gmacsecCreateTxSA gmacsecCreateTxSA;
	FMSD_gmacsecDeleteTxSA gmacsecDeleteTxSA;
	FMSD_gmacsecGetTxSA gmacsecGetTxSA;
	FMSD_gmacsecGetRXSCStates gmacsecGetRXSCStates;
	FMSD_gmacsecGetRXSecYStates gmacsecGetRXSecYStates;
	FMSD_gmacsecGetTXSCStates gmacsecGetTXSCStates;
	FMSD_gmacsecGetTXSecYStates gmacsecGetTXSecYStates;
	FMSD_gmacsecEnableEgressSet gmacsecEnableEgressSet;
	FMSD_gmacsecEnableIngressSet gmacsecEnableIngressSet;
	FMSD_gmacsecEnableEgressGet gmacsecEnableEgressGet;
	FMSD_gmacsecEnableIngressGet gmacsecEnableIngressGet;
	FMSD_gmacsecSetTxPNThreshold gmacsecSetTxPNThreshold;
	FMSD_gmacsecGetTxPNThreshold gmacsecGetTxPNThreshold;
	FMSD_gmacsecSetTxXPNThreshold gmacsecSetTxXPNThreshold;
	FMSD_gmacsecGetTxXPNThreshold gmacsecGetTxXPNThreshold;
	FMSD_gmacsecSetRxDefaultSCI gmacsecSetRxDefaultSCI;
	FMSD_gmacsecGetRxDefaultSCI gmacsecGetRxDefaultSCI;
	FMSD_gmacsecSetTxSCIntCtrl gmacsecSetTxSCIntCtrl;
	FMSD_gmacsecGetTxSCIntCtrl gmacsecGetTxSCIntCtrl;
	FMSD_gmacsecGetNextTxSCIntStatus gmacsecGetNextTxSCIntStatus;
} SwitchDevMACSECObj_;

typedef struct
{
    FMSD_gfdbAddMacEntry gfdbAddMacEntry;
	FMSD_gfdbGetAtuEntryNext gfdbGetAtuEntryNext;
	FMSD_gfdbFlush gfdbFlush;
	FMSD_gfdbFlushInDB gfdbFlushInDB;
	FMSD_gfdbMove gfdbMove;
	FMSD_gfdbMoveInDB gfdbMoveInDB;
    FMSD_gfdbPortRemove gfdbPortRemove;
    FMSD_gfdbPortRemoveInDB gfdbPortRemoveInDB;
	FMSD_gfdbDelAtuEntry gfdbDelAtuEntry;
	FMSD_gfdbGetViolation gfdbGetViolation;
	FMSD_gfdbFindAtuMacEntry gfdbFindAtuMacEntry;
	FMSD_gfdbSetAgingTimeout gfdbSetAgingTimeout;
    FMSD_gfdbGetAgingTimeout gfdbGetAgingTimeout;
    FMSD_gfdbSetLearn2All gfdbSetLearn2All;
    FMSD_gfdbGetLearn2All gfdbGetLearn2All;
    FMSD_gfdbSetPortLearnLimit gfdbSetPortLearnLimit;
    FMSD_gfdbGetPortLearnLimit gfdbGetPortLearnLimit;
    FMSD_gfdbGetPortLearnCount gfdbGetPortLearnCount;

    FMSD_gfdbGetEntryCount gfdbGetEntryCount;
    FMSD_gfdbGetNonStaticEntryCount gfdbGetNonStaticEntryCount;
    FMSD_gfdbGetEntryCountPerFid gfdbGetEntryCountPerFid;
    FMSD_gfdbGetNonStaticEntryCountPerFid gfdbGetNonStaticEntryCountPerFid;

    FMSD_gfdbDump gfdbDump;

} SwitchDevATUObj_;

typedef struct
{
    FMSD_gBpeEnableSet gBpeEnableSet;
    FMSD_gBpeEnableGet gBpeEnableGet;
	FMSD_gecidAddEntry gecidAddEntry;
	FMSD_gecidFlushAll gecidFlushAll;
	FMSD_gecidFlushEntry gecidFlushEntry;
	FMSD_gecidMove gecidMove;
	FMSD_gecidGetEcidEntryNext gecidGetEcidEntryNext;
	FMSD_gecidFindEcidEntry gecidFindEcidEntry;
	FMSD_gecidGetEntryCount gecidGetEntryCount;
	FMSD_gecidDump gecidDump;

} SwitchDevECIDObj_;

typedef struct
{	
	FMSD_gvctGetAdvCableDiag gvctGetAdvCableDiag;
	FMSD_gvctGetAdvExtendedStatus gvctGetAdvExtendedStatus;

} SwitchDevVCTObj_;

typedef struct
{
	FMSD_gstuAddEntry gstuAddEntry;
	FMSD_gstuDelEntry gstuDelEntry;
	FMSD_gstuGetEntryNext	gstuGetEntryNext;
	FMSD_gstuFindSidEntry gstuFindSidEntry;
    FMSD_gstuGetEntryCount gstuGetEntryCount;
	FMSD_gstuDump	gstuDump;
} SwitchDevSTUObj_;

typedef struct
{
	FMSD_gvtuFindVidEntry  gvtuFindVidEntry;
    FMSD_gvtuExistVidEntry gvtuExistVidEntry;
	FMSD_gvtuFlush  gvtuFlush;
	FMSD_gvtuAddEntry  gvtuAddEntry;
	FMSD_gvtuGetEntryNext	gvtuGetEntryNext;
	FMSD_gvtuDelEntry  gvtuDelEntry;
	FMSD_gvtuSetMode  gvtuSetMode;
	FMSD_gvtuGetMode  gvtuGetMode;
	FMSD_gvtuGetViolation  gvtuGetViolation;
    FMSD_gvtuGetEntryCount gvtuGetEntryCount;
    FMSD_gvtuSetMemberTag gvtuSetMemberTag;
    FMSD_gvtuGetMemberTag gvtuGetMemberTag;
	FMSD_gvtuDump	gvtuDump;
} SwitchDevVTUObj_;

typedef struct
{
	FMSD_impRun    impRun;
	FMSD_impReset    impReset;
	FMSD_impWriteEEPROM    impWriteEEPROM;
	FMSD_impLoadToEEPROM    impLoadToEEPROM;
	FMSD_impLoadToRAM    impLoadToRAM;
	FMSD_impStop    impStop;

} SwitchDevIMPObj_;

typedef struct
{
	FMSD_geepromWrite geepromWrite;
	FMSD_geepromRead  geepromRead;
	FMSD_geepromSetMChipSel geepromSetMChipSel;
	FMSD_geepromGetMChipSel geepromGetMChipSel;

} SwitchDevEEPROMObj_;

typedef struct
{
	FMSD_gphyReset    gphyReset;
    FMSD_serdesReset  serdesReset;
	FMSD_gphySetPortLoopback    gphySetPortLoopback;
	FMSD_gphySetPortSpeed    gphySetPortSpeed;
	FMSD_gphyPortPowerDown    gphyPortPowerDown;
	FMSD_gphySetPortDuplexMode    gphySetPortDuplexMode;
	FMSD_gphySetPortAutoMode    gphySetPortAutoMode;
    FMSD_gphySetEEE gphySetEEE;
	FMSD_gphySetFlowControlEnable gphySetFlowControlEnable;
	FMSD_gphySetIntEnable gphySetIntEnable;
	FMSD_gphyGetIntEnable gphyGetIntEnable;
	FMSD_gphyGetIntStatus gphyGetIntStatus;
} SwitchDevPHYCTRLObj_;

typedef struct
{
	FMSD_gpirlInitialize    gpirlInitialize;
	FMSD_gpirlInitResource    gpirlInitResource;
	FMSD_gpirlCustomSetup    gpirlCustomSetup;
	FMSD_gpirlReadResource    gpirlReadResource;
	FMSD_gpirlWriteResource    gpirlWriteResource;
	FMSD_gpirlGetResReg    gpirlGetResReg;
	FMSD_gpirlSetResReg    gpirlSetResReg;
    FMSD_gpirlAdvConfig   gpirlAdvConfig;
    FMSD_gpirlAdvConfigDisable   gpirlAdvConfigDisable;

} SwitchDevPIRLObj_;

typedef struct
{
	FMSD_gvlnSetPortVid    gvlnSetPortVid;
	FMSD_gvlnGetPortVid    gvlnGetPortVid;
	FMSD_gvlnSetPortVlanDot1qMode    gvlnSetPortVlanDot1qMode;
	FMSD_gvlnGetPortVlanDot1qMode    gvlnGetPortVlanDot1qMode;
	FMSD_gprtSetDiscardTagged    gprtSetDiscardTagged;
	FMSD_gprtGetDiscardTagged    gprtGetDiscardTagged;
	FMSD_gprtSetDiscardUntagged    gprtSetDiscardUntagged;
	FMSD_gprtGetDiscardUntagged    gprtGetDiscardUntagged;
	FMSD_gprtSetUnicastFloodBlock    gprtSetUnicastFloodBlock;
	FMSD_gprtGetUnicastFloodBlock    gprtGetUnicastFloodBlock;
	FMSD_gprtSetMulticastFloodBlock    gprtSetMulticastFloodBlock;
	FMSD_gprtGetMulticastFloodBlock    gprtGetMulticastFloodBlock;
	FMSD_gprtSetDiscardBCastMode    gprtSetDiscardBCastMode;
	FMSD_gprtGetDiscardBCastMode    gprtGetDiscardBCastMode;
    FMSD_gprtSetIGMPSnoopEnable  gprtSetIGMPSnoopEnable;
    FMSD_gprtGetIGMPSnoopEnable  gprtGetIGMPSnoopEnable;
    FMSD_gprtSetHeaderMode   gprtSetHeaderMode;
    FMSD_gprtGetHeaderMode   gprtGetHeaderMode;
    FMSD_gprtSetDropOnLock   gprtSetDropOnLock;
    FMSD_gprtGetDropOnLock   gprtGetDropOnLock;
	FMSD_gprtSetJumboMode    gprtSetJumboMode;
	FMSD_gprtGetJumboMode    gprtGetJumboMode;
    FMSD_gprtSetSpecifiedMTU gprtSetSpecifiedMTU;
    FMSD_gprtGetSpecifiedMTU gprtGetSpecifiedMTU;
	FMSD_gprtSetLearnEnable    gprtSetLearnEnable;
    FMSD_gprtGetLearnEnable    gprtGetLearnEnable;
    FMSD_gprtSetVTUPriorityOverride    gprtSetVTUPriorityOverride;
    FMSD_gprtGetVTUPriorityOverride    gprtGetVTUPriorityOverride;
    FMSD_gprtSetSAPriorityOverride    gprtSetSAPriorityOverride;
    FMSD_gprtGetSAPriorityOverride    gprtGetSAPriorityOverride;
    FMSD_gprtSetDAPriorityOverride    gprtSetDAPriorityOverride;
    FMSD_gprtGetDAPriorityOverride    gprtGetDAPriorityOverride; 
    FMSD_gprtSetMessagePort  gprtSetMessagePort;
    FMSD_gprtGetMessagePort  gprtGetMessagePort;
    FMSD_gprtSetEtherType    gprtSetEtherType;
    FMSD_gprtGetEtherType    gprtGetEtherType;
    FMSD_gprtSetAllowVidZero gprtSetAllowVidZero;
    FMSD_gprtGetAllowVidZero gprtGetAllowVidZero;
	FMSD_gprtSetFlowCtrl    gprtSetFlowCtrl;
	FMSD_gprtGetFlowCtrl    gprtGetFlowCtrl;
	FMSD_gprtSetForceSpeed    gprtSetForceSpeed;
	FMSD_gprtGetForceSpeed    gprtGetForceSpeed;
	FMSD_gprtSetEgressMonitorSource    gprtSetEgressMonitorSource;
	FMSD_gprtGetEgressMonitorSource    gprtGetEgressMonitorSource;
	FMSD_gprtSetIngressMonitorSource    gprtSetIngressMonitorSource;
	FMSD_gprtGetIngressMonitorSource    gprtGetIngressMonitorSource;
	FMSD_grcSetEgressRate    grcSetEgressRate;
	FMSD_grcGetEgressRate    grcGetEgressRate;
	FMSD_gprtGetDuplex    gprtGetDuplex;
    FMSD_gprtSetDuplex    gprtSetDuplex;
    FMSD_gprtGetDuplexStatus    gprtGetDuplexStatus;
    FMSD_gprtGetForceLink    gprtGetForceLink;
    FMSD_gprtSetForceLink    gprtSetForceLink;
	FMSD_gprtGetLinkState    gprtGetLinkState;
	FMSD_gprtGetSpeed    gprtGetSpeed;

    FMSD_gprtSetCutThrouthEnable gprtSetCutThrouthEnable;
    FMSD_gprtGetCutThrouthEnable gprtGetCutThrouthEnable;
    FMSD_gprtSetIgnoreFcs    gprtSetIgnoreFcs;
    FMSD_gprtGetIgnoreFcs    gprtGetIgnoreFcs;
    FMSD_gprtSetAllowBadFcs  gprtSetAllowBadFcs;
    FMSD_gprtGetAllowBadFcs  gprtGetAllowBadFcs;
    FMSD_gprtSetPauseLimitOut    gprtSetPauseLimitOut;
    FMSD_gprtGetPauseLimitOut    gprtGetPauseLimitOut;
    FMSD_gprtSetPauseLimitIn gprtSetPauseLimitIn;
    FMSD_gprtGetPauseLimitIn gprtGetPauseLimitIn;
    FMSD_gprtSetQueueToPause gprtSetQueueToPause;
    FMSD_gprtGetQueueToPause gprtGetQueueToPause;
    FMSD_gprtSetRxPriorityFlowControlEnable  gprtSetRxPriorityFlowControlEnable;
    FMSD_gprtGetRxPriorityFlowControlEnable  gprtGetRxPriorityFlowControlEnable;
    FMSD_gprtSetRxPriorityFlowControlToQueue gprtSetRxPriorityFlowControlToQueue;
    FMSD_gprtGetRxPriorityFlowControlToQueue gprtGetRxPriorityFlowControlToQueue;
    FMSD_gprtSetTxPriorityFlowControlEnable  gprtSetTxPriorityFlowControlEnable;
    FMSD_gprtGetTxPriorityFlowControlEnable  gprtGetTxPriorityFlowControlEnable;
    FMSD_gprtGetOutQueueSize gprtGetOutQueueSize;
    FMSD_gprtSetMapDA    gprtSetMapDA;
    FMSD_gprtGetMapDA    gprtGetMapDA;
    FMSD_gprtSetPortLocked   gprtSetPortLocked;
    FMSD_gprtGetPortLocked   gprtGetPortLocked;
    FMSD_gprtSetHoldAt1  gprtSetHoldAt1;
    FMSD_gprtGetHoldAt1  gprtGetHoldAt1;
    FMSD_gprtSetInt0nAgeOut  gprtSetInt0nAgeOut;
    FMSD_gprtGetInt0nAgeOut  gprtGetInt0nAgeOut;
    FMSD_gprtSetRefreshLocked    gprtSetRefreshLocked;
    FMSD_gprtGetRefreshLocked    gprtGetRefreshLocked;
    FMSD_gprtSetIgnoreWrongData  gprtSetIgnoreWrongData;
    FMSD_gprtGetIgnoreWrongData  gprtGetIgnoreWrongData;
    FMSD_gprtSetARPMirror    gprtSetARPMirror;
    FMSD_gprtGetARPMirror    gprtGetARPMirror;

    FMSD_gprtSetVlanPorts    gprtSetVlanPorts;
    FMSD_gprtGetVlanPorts    gprtGetVlanPorts;
	FMSD_gprtSetForceDefaultVid  gprtSetForceDefaultVid;
	FMSD_gprtGetForceDefaultVid  gprtGetForceDefaultVid;
    FMSD_gprtSetFrameMode    gprtSetFrameMode;
    FMSD_gprtGetFrameMode    gprtGetFrameMode;

	FMSD_gprtSetPortECID	gprtSetPortECID;
	FMSD_gprtGetPortECID	gprtGetPortECID;
	FMSD_gprtSetIgnoreETag	gprtSetIgnoreETag;
	FMSD_gprtGetIgnoreETag	gprtGetIgnoreETag;
	FMSD_gprtSetEgressMode  gprtSetEgressMode;
	FMSD_gprtGetEgressMode  gprtGetEgressMode;
	FMSD_gprtSetExtendedPortCtrlReg gprtSetExtendedPortCtrlReg;
	FMSD_gprtGetExtendedPortCtrlReg gprtGetExtendedPortCtrlReg;

    FMSD_gprtSetPortState    gprtSetPortState;
    FMSD_gprtGetPortState    gprtGetPortState;

    FMSD_gprtSetPreemptEnable  gprtSetPreemptEnable;
    FMSD_gprtGetPreemptEnable  gprtGetPreemptEnable;
    FMSD_gprtSetPreemptSize    gprtSetPreemptSize;
    FMSD_gprtGetPreemptSize    gprtGetPreemptSize;
    FMSD_gprtSetPreemptQueue   gprtSetPreemptQueue;
    FMSD_gprtGetPreemptQueue   gprtGetPreemptQueue;

	FMSD_gprtRegDump	gprtRegDump;
} SwitchDevPORTCTRLObj_;

typedef struct
{
	FMSD_gpolicySetMgmtCtrl gpolicySetMgmtCtrl;
	FMSD_gpolicyGetMgmtCtrl gpolicyGetMgmtCtrl;
	FMSD_gpolicySetCpuDest gpolicySetCpuDest;
	FMSD_gpolicyGetCpuDest gpolicyGetCpuDest;
	FMSD_gpolicySetIngressMonitorDest gpolicySetIngressMonitorDest;
	FMSD_gpolicyGetIngressMonitorDest gpolicyGetIngressMonitorDest;
	FMSD_gpolicySetRsvd2CpuEnables gpolicySetRsvd2CpuEnables;
	FMSD_gpolicyGetRsvd2CpuEnables gpolicyGetRsvd2CpuEnables;
	FMSD_gpolicySetMGMTPri gpolicySetMGMTPri;
	FMSD_gpolicyGetMGMTPri gpolicyGetMGMTPri;
	FMSD_gpolicySetRouteDest gpolicySetRouteDest;
	FMSD_gpolicyGetRouteDest gpolicyGetRouteDest;
	FMSD_gpolicySetMirrorDest gpolicySetMirrorDest;
	FMSD_gpolicyGetMirrorDest gpolicyGetMirrorDest;
	FMSD_gpolicySetIllegalIPAddr gpolicySetIllegalIPAddr;
	FMSD_gpolicyGetIllegalIPAddr gpolicyGetIllegalIPAddr;
	FMSD_gpolicySetDipDaMismatch gpolicySetDipDaMismatch;
	FMSD_gpolicyGetDipDaMismatch gpolicyGetDipDaMismatch;
	FMSD_gpolicySetZeroSIP gpolicySetZeroSIP;
	FMSD_gpolicyGetZeroSIP gpolicyGetZeroSIP;
	FMSD_gpolicySetMcTtlExceeded gpolicySetMcTtlExceeded;
	FMSD_gpolicyGetMcTtlExceeded gpolicyGetMcTtlExceeded;
	FMSD_gpolicySetUcTtlExceeded gpolicySetUcTtlExceeded;
	FMSD_gpolicyGetUcTtlExceeded gpolicyGetUcTtlExceeded;
	FMSD_gprtSetPortTtlThreshold gprtSetPortTtlThreshold;
	FMSD_gprtGetPortTtlThreshold gprtGetPortTtlThreshold;
	FMSD_gpolicySetIpHdrErr gpolicySetIpHdrErr;
	FMSD_gpolicyGetIpHdrErr gpolicyGetIpHdrErr;
	FMSD_gpolicySetTCPFinUrgPsh gpolicySetTCPFinUrgPsh;
	FMSD_gpolicyGetTCPFinUrgPsh gpolicyGetTCPFinUrgPsh;
	FMSD_gpolicySetTCPFlagsSeqNum gpolicySetTCPFlagsSeqNum;
	FMSD_gpolicyGetTCPFlagsSeqNum gpolicyGetTCPFlagsSeqNum;
	FMSD_gpolicySetTCPSYN gpolicySetTCPSYN;
	FMSD_gpolicyGetTCPSYN gpolicyGetTCPSYN;
	FMSD_gpolicySetTCPHSize gpolicySetTCPHSize;
	FMSD_gpolicyGetTCPHSize gpolicyGetTCPHSize;
	FMSD_gprtSetPortTCPMinHdrSize gprtSetPortTCPMinHdrSize;
	FMSD_gprtGetPortTCPMinHdrSize gprtGetPortTCPMinHdrSize;
	FMSD_gpolicySetICMPSize gpolicySetICMPSize;
	FMSD_gpolicyGetICMPSize gpolicyGetICMPSize;
	FMSD_gprtSetPortICMPMaxSize gprtSetPortICMPMaxSize;
	FMSD_gprtGetPortICMPMaxSize gprtGetPortICMPMaxSize;
	FMSD_gpolicySetUDPPort gpolicySetUDPPort;
	FMSD_gpolicyGetUDPPort gpolicyGetUDPPort;
	FMSD_gpolicySetTCPPort gpolicySetTCPPort;
	FMSD_gpolicyGetTCPPort gpolicyGetTCPPort;
	FMSD_gpolicySetTCPSynFin gpolicySetTCPSynFin;
	FMSD_gpolicyGetTCPSynFin gpolicyGetTCPSynFin;
	FMSD_gpolicySetTCPFrag gpolicySetTCPFrag;
	FMSD_gpolicyGetTCPFrag gpolicyGetTCPFrag;
	FMSD_gpolicySetNearNonTPMR gpolicySetNearNonTPMR;
	FMSD_gpolicyGetNearNonTPMR gpolicyGetNearNonTPMR;
	FMSD_gpolicySetETag gpolicySetETag;
	FMSD_gpolicyGetETag gpolicyGetETag;
	FMSD_gpolicySetNonETag gpolicySetNonETag;
	FMSD_gpolicyGetNonETag gpolicyGetNonETag;
	FMSD_gpolicyDump gpolicyDump;
} SwitchDevPolicyObj_;

typedef struct
{
    FMSD_gsysSetTrunkMaskTable   gsysSetTrunkMaskTable;
    FMSD_gsysGetTrunkMaskTable   gsysGetTrunkMaskTable;
    FMSD_gsysSetHashTrunk   gsysSetHashTrunk;
    FMSD_gsysGetHashTrunk   gsysGetHashTrunk;
    FMSD_gsysSetTrunkRouting   gsysSetTrunkRouting;
    FMSD_gsysGetTrunkRouting   gsysGetTrunkRouting;
    FMSD_gprtSetTrunkPort    gprtSetTrunkPort;
    FMSD_gprtGetTrunkPort    gprtGetTrunkPort;
} SwitchDevTrunkObj_;

typedef struct
{
	FMSD_gqosSetPrioMapRule   gqosSetPrioMapRule;
	FMSD_gqosGetPrioMapRule   gqosGetPrioMapRule;
	FMSD_gprtSetInitialPri   gprtSetInitialPri;
	FMSD_gprtGetInitialPri   gprtGetInitialPri;
	FMSD_gprtSetDefQpri   gprtSetDefQpri;
	FMSD_gprtGetDefQpri   gprtGetDefQpri;
    FMSD_gprtSetDefFpri gprtSetDefFpri;
    FMSD_gprtGetDefFpri gprtGetDefFpri;
	FMSD_gprtSetIEEEPrioMap   gprtSetIEEEPrioMap;
	FMSD_gprtGetIEEEPrioMap   gprtGetIEEEPrioMap;
    FMSD_gprtSetCosToQueue gprtSetCosToQueue;
    FMSD_gprtGetCosToQueue gprtGetCosToQueue;
    FMSD_gprtSetCosToDscp gprtSetCosToDscp;
    FMSD_gprtGetCosToDscp gprtGetCosToDscp;
    FMSD_gprtRewriteDscp  gprtRewriteDscp;
} SwitchDevQoSMAPObj_;

typedef struct
{
	FMSD_gstatsFlushAll   gstatsFlushAll;
	FMSD_gstatsFlushPort   gstatsFlushPort;
	FMSD_gstatsGetPortCounter   gstatsGetPortCounter;
	FMSD_gstatsGetPortAllCounters   gstatsGetPortAllCounters;
	FMSD_gstatsGetHistogramMode   gstatsGetHistogramMode;
	FMSD_gstatsSetHistogramMode   gstatsSetHistogramMode;
	FMSD_gmibDump   gmibDump;

} SwitchDevPORTRMONObj_;

typedef struct
{
	FMSD_gprtSetQueueCtrl  gprtSetQueueCtrl;
	FMSD_gprtGetQueueCtrl  gprtGetQueueCtrl;
	FMSD_gprtSetPortSched  gprtSetPortSched;
	FMSD_gprtGetPortSched  gprtGetPortSched;
    FMSD_gsysSetQoSWeight  gsysSetQoSWeight;
    FMSD_gsysGetQoSWeight  gsysGetQoSWeight;
	FMSD_gprtSetQoSWeight  gprtSetQoSWeight;
	FMSD_gprtGetQoSWeight  gprtGetQoSWeight;

} SwitchDevQUEUEObj_;

typedef struct
{
	FMSD_gsysSetFloodBC   gsysSetFloodBC;
	FMSD_gsysGetFloodBC   gsysGetFloodBC;
	FMSD_gsysSetMonitorMgmtCtrl   gsysSetMonitorMgmtCtrl;
	FMSD_gsysGetMonitorMgmtCtrl   gsysGetMonitorMgmtCtrl;
	FMSD_gsysSetCPUDest   gsysSetCPUDest;
	FMSD_gsysGetCPUDest   gsysGetCPUDest;
	FMSD_gsysSetIngressMonitorDest   gsysSetIngressMonitorDest;
	FMSD_gsysGetIngressMonitorDest   gsysGetIngressMonitorDest;
	FMSD_gsysSetEgressMonitorDest   gsysSetEgressMonitorDest;
	FMSD_gsysGetEgressMonitorDest   gsysGetEgressMonitorDest;
	FMSD_gsysSetRsvd2CpuEnables   gsysSetRsvd2CpuEnables;
	FMSD_gsysGetRsvd2CpuEnables   gsysGetRsvd2CpuEnables;
	FMSD_gsysSetMGMTPri   gsysSetMGMTPri;
	FMSD_gsysGetMGMTPri   gsysGetMGMTPri;
	FMSD_gsysSetSMISetup   gsysSetSMISetup;
	FMSD_gsysGetSMISetup   gsysGetSMISetup;
	FMSD_gsysGlobal1RegDump	gsysGlobal1RegDump;
	FMSD_gsysGlobal2RegDump	gsysGlobal2RegDump;
	FMSD_gsysSetRMUMode gsysSetRMUMode;
	FMSD_gsysGetRMUMode gsysGetRMUMode;
	FMSD_gsysQbvWrite gsysQbvWrite;
	FMSD_gsysQbvRead gsysQbvRead;
	
} SwitchDevSYSCTRLObj_;

typedef struct
{
    FMSD_gqavSetPortQpriXRate     gqavSetPortQpriXRate;
    FMSD_gqavGetPortQpriXRate     gqavGetPortQpriXRate;
    FMSD_gqavSetPortQpriXHiLimit  gqavSetPortQpriXHiLimit;
    FMSD_gqavGetPortQpriXHiLimit  gqavGetPortQpriXHiLimit;
} SwitchDevQavObj_;

typedef struct
{
    FMSD_gptpSetIntEnable    gptpSetIntEnable;
    FMSD_gptpGetIntEnable    gptpGetIntEnable; 
    FMSD_gptpSetProtPTPEn    gptpSetProtPTPEn;
    FMSD_gptpGetProtPTPEn    gptpGetProtPTPEn;
    FMSD_gptpSetPortConfig   gptpSetPortConfig;
    FMSD_gptpGetPortConfig   gptpGetPortConfig;
    FMSD_gptpGetTimeStamp    gptpGetTimeStamp;
    FMSD_gptpGetTSValidSt    gptpGetTSValidSt;
    FMSD_gptpResetTimeStamp  gptpResetTimeStamp;
    FMSD_gptpSetGlobalConfig gptpSetGlobalConfig;
    FMSD_gptpGetGlobalConfig gptpGetGlobalConfig;
    FMSD_gptpGetIntStatus    gptpGetIntStatus;
    FMSD_gptpSetMeanPathDelay     gptpSetMeanPathDelay;
    FMSD_gptpGetMeanPathDelay     gptpGetMeanPathDelay;
    FMSD_gptpSetIgrPathDelayAsym  gptpSetIgrPathDelayAsym;
    FMSD_gptpGetIgrPathDelayAsym  gptpGetIgrPathDelayAsym;
    FMSD_gptpSetEgrPathDelayAsym  gptpSetEgrPathDelayAsym;
    FMSD_gptpGetEgrPathDelayAsym  gptpGetEgrPathDelayAsym;
    FMSD_gptpSetMode              gptpSetMode;
    FMSD_gptpGetMode              gptpGetMode;
    FMSD_gptpSetMasterSlave       gptpSetMasterSlave;
    FMSD_gptpGetMasterSlave       gptpGetMasterSlave;
    FMSD_gptpGetTODBusyBitStatus  gptpGetTODBusyBitStatus;
    FMSD_gptpTODStoreCompensation gptpTODStoreCompensation;
    FMSD_gptpTODStoreAll          gptpTODStoreAll;
    FMSD_gptpTODCaptureAll        gptpTODCaptureAll;
    FMSD_gptpSetPulse             gptpSetPulse;
    FMSD_gptpGetPulse             gptpGetPulse;
    FMSD_gtaiGetPTPGlobalTime     gtaiGetPTPGlobalTime;
    FMSD_gptpGetPortReg           gptpGetPortReg;
    FMSD_gptpSetPortReg           gptpSetPortReg;
    FMSD_gptpGetGlobalReg         gptpGetGlobalReg;
    FMSD_gptpSetGlobalReg         gptpSetGlobalReg;
    FMSD_gptpGetTaiReg            gptpGetTaiReg;
    FMSD_gptpSetTaiReg            gptpSetTaiReg;
    FMSD_gptpSet1588v2Config      gptpSet1588v2Config;
    FMSD_gptpSet8021asConfig      gptpSet8021asConfig;
    FMSD_gptpSet8021asRevConfig   gptpSet8021asRevConfig;
    FMSD_gptpSetPreemptTsSel      gptpSetPreemptTsSel;
    FMSD_gptpGetPreemptTsSel      gptpGetPreemptTsSel;
    FMSD_gptpSetPreemptCtrl       gptpSetPreemptCtrl;
    FMSD_gptpGetPreemptCtrl       gptpGetPreemptCtrl;
    FMSD_gtaiSetTrigGenTimerSel   gtaiSetTrigGenTimerSel;
    FMSD_gtaiGetTrigGenTimerSel   gtaiGetTrigGenTimerSel;
}SwitchDevPTPObj_;

typedef struct
{
	FMSD_gtcamFlushAll   gtcamFlushAll;
	FMSD_gtcamFlushEntry   gtcamFlushEntry;
	FMSD_gtcamLoadEntry   gtcamLoadEntry;
	FMSD_gtcamReadTCAMData   gtcamReadTCAMData;
	FMSD_gtcamGetNextTCAMData   gtcamGetNextTCAMData;
	FMSD_gtcamFindEntry   gtcamFindEntry;
    FMSD_gtcamSetMode gtcamSetMode;
    FMSD_gtcamGetMode gtcamGetMode;
    FMSD_gtcamAddEntryPort gtcamAddEntryPort;
    FMSD_gtcamRemoveEntryPort gtcamRemoveEntryPort;
	FMSD_gtcamEgrFlushEntry   gtcamEgrFlushEntry;
	FMSD_gtcamEgrFlushEntryAllPorts   gtcamEgrFlushEntryAllPorts;
	FMSD_gtcamEgrLoadEntry   gtcamEgrLoadEntry;
	FMSD_gtcamEgrGetNextTCAMData   gtcamEgrGetNextTCAMData;
	FMSD_gtcamEgrReadTCAMData   gtcamEgrReadTCAMData;
	FMSD_gtcamDump gtcamDump;
    FMSD_gtcamAdvConfig gtcamAdvConfig;
} SwitchDevTCAMObj_;

typedef struct
{
	FMSD_gtcamRCLoadEntry gtcamRCLoadEntry;
	FMSD_gtcamRCFlushAllEntry gtcamRCFlushAllEntry;
	FMSD_gtcamRCFlushEntry gtcamRCFlushEntry;
	FMSD_gtcamRCReadEntry gtcamRCReadEntry;
	FMSD_gtcamRCGetNextEntry gtcamRCGetNextEntry;
	FMSD_gtcamRCFindEntry gtcamRCFindEntry;
	FMSD_gtcamRCEntryDump gtcamRCEntryDump;

} SwitchDevTCAMRCObj_;

typedef struct
{
	FMSD_gtcamRCCompLoad gtcamRCCompLoad;
	FMSD_gtcamRCCompFlushAll gtcamRCCompFlushAll;
	FMSD_gtcamRCCompFlushPort gtcamRCCompFlushPort;
	FMSD_gtcamRCCompRead gtcamRCCompRead;

} SwitchDevTCAMRCCompObj_;

typedef struct
{
	FMSD_garpLoadMcEntry garpLoadMcEntry;
	FMSD_garpLoadUcEntry garpLoadUcEntry;
	FMSD_garpFlushEntry garpFlushEntry;
	FMSD_garpFlushAll garpFlushAll;
	FMSD_garpReadEntry garpReadEntry;
	FMSD_garpGetNextEntry garpGetNextEntry;
} SwitchDevARPObj_;

typedef struct
{
	FMSD_grmuGetID	grmuGetID;
	FMSD_grmuAtuDump	grmuAtuDump;
	FMSD_grmuEcidDump grmuEcidDump;
	FMSD_grmuMibDump grmuMibDump;
	FMSD_grmuMib2Dump grmuMib2Dump;
	FMSD_grmuTCAMDump grmuTCAMDump;
	FMSD_grmuTCAMRead grmuTCAMRead;
	FMSD_grmuTCAMWrite grmuTCAMWrite;
	FMSD_grmuMultiRegAccess grmuMultiRegAccess;
    FMSD_grmuRegDump grmuRegDump;
} SwitchDevRMUObj_;

typedef struct
{
    FMSD_msdSetSMIC45PhyReg msdSetSMIC45PhyReg;
    FMSD_msdGetSMIC45PhyReg msdGetSMIC45PhyReg;
    FMSD_msdSetSMIPhyReg msdSetSMIPhyReg;
    FMSD_msdGetSMIPhyReg msdGetSMIPhyReg;
    FMSD_msdSetSMIPhyXMDIOReg msdSetSMIPhyXMDIOReg;
    FMSD_msdGetSMIPhyXMDIOReg msdGetSMIPhyXMDIOReg;
	FMSD_msdSetSMIExtPhyReg msdSetSMIExtPhyReg;
	FMSD_msdGetSMIExtPhyReg msdGetSMIExtPhyReg;
	FMSD_msdSetSMIExtPhyXMDIOReg msdSetSMIExtPhyXMDIOReg;
	FMSD_msdGetSMIExtPhyXMDIOReg msdGetSMIExtPhyXMDIOReg;
} SwitchDevSmiPhyRegAccess_;

typedef struct
{
	FMSD_gdevIntEnableSet gdevIntEnableSet;
	FMSD_gdevIntEnableGet gdevIntEnableGet;
	FMSD_gdevIntStatusGet gdevIntStatusGet;
	FMSD_gactiveIntEnableSet gactiveIntEnableSet;
	FMSD_gactiveIntEnableGet gactiveIntEnableGet;
	FMSD_gactiveIntStatusGet gactiveIntStatusGet;
} SwitchDevIntObj_;

typedef struct
{
	FMSD_gledForceOn gledForceOn;
	FMSD_gledForceOff gledForceOff;
	FMSD_gledLinkAct gledLinkAct;
	FMSD_gledModeSet gledModeSet;
	FMSD_gledModeGet gledModeGet;
	FMSD_gledBlinkRateSet gledBlinkRateSet;
	FMSD_gledBlinkRateGet gledBlinkRateGet;
	FMSD_gledPulseStretchSet gledPulseStretchSet;
	FMSD_gledPulseStretchGet gledPulseStretchGet;
	FMSD_gledSpecialModeSet gledSpecialModeSet;
	FMSD_gledSpecialModeGet gledSpecialModeGet;
	FMSD_gledCtrlSet gledCtrlSet;
	FMSD_gledCtrlGet gledCtrlGet;
	FMSD_gledDump gledDump;
} SwitchDevLEDObj_;

typedef struct
{
    FMSD_gfrerSeqGenFlushAll            gfrerSeqGenFlushAll;
    FMSD_gfrerSeqGenFlushEntry          gfrerSeqGenFlushEntry;
    FMSD_gfrerSeqGenRead                gfrerSeqGenRead;
    FMSD_gfrerSeqGenLoad                gfrerSeqGenLoad;

    FMSD_gfrerIndvRcvyFlushAll          gfrerIndvRcvyFlushAll;
    FMSD_gfrerIndvRcvyFlushEntry        gfrerIndvRcvyFlushEntry;
    FMSD_gfrerIndvRcvyRead              gfrerIndvRcvyRead;
    FMSD_gfrerIndvRcvyLoad              gfrerIndvRcvyLoad;
    FMSD_gfrerIndvRcvyGetNext           gfrerIndvRcvyGetNext;
    FMSD_gfrerIndvRcvyReadClear         gfrerIndvRcvyReadClear;

    FMSD_gfrerSeqRcvyFlushAll           gfrerSeqRcvyFlushAll;
    FMSD_gfrerSeqRcvyFlushEntry         gfrerSeqRcvyFlushEntry;
    FMSD_gfrerSeqRcvyRead               gfrerSeqRcvyRead;
    FMSD_gfrerSeqRcvyLoad               gfrerSeqRcvyLoad;
    FMSD_gfrerSeqRcvyGetNext            gfrerSeqRcvyGetNext;
    FMSD_gfrerSeqRcvyGetViolation       gfrerSeqRcvyGetViolation;
    FMSD_gfrerSeqRcvyReadClear          gfrerSeqRcvyReadClear;

    FMSD_gfrerBank0SCtrFlushAll            gfrerBank0SCtrFlushAll;
    FMSD_gfrerB0SCtrFlushEntry          gfrerBank0SCtrFlushEntry;
    FMSD_gfrerBank0SCtrRead                gfrerBank0SCtrRead;
    FMSD_gfrerBank0SCtrLoad                gfrerBank0SCtrLoad;
    FMSD_gfrerBank0SCtrGetNext             gfrerBank0SCtrGetNext;
    FMSD_gfrerBank0SCtrReadClear           gfrerBank0SCtrReadClear;

    FMSD_gfrerBank1SCtrFlushAll            gfrerBank1SCtrFlushAll;
    FMSD_gfrerBank1SCtrFlushEntry          gfrerBank1SCtrFlushEntry;
    FMSD_gfrerBank1SCtrRead                gfrerBank1SCtrRead;
    FMSD_gfrerBank1SCtrLoad                gfrerBank1SCtrLoad;
    FMSD_gfrerBank1SCtrGetNext             gfrerBank1SCtrGetNext;
    FMSD_gfrerBank1SCtrReadClear           gfrerBank1SCtrReadClear;
} SwitchDevFRERObj_;

typedef struct
{
    FMSD_gqcrSetGrpId           gqcrSetGrpId;
    FMSD_gqcrGetGrpId           gqcrGetGrpId;
    FMSD_gqcrSetCbsLimit        gqcrSetCbsLimit;
    FMSD_gqcrGetCbsLimit        gqcrGetCbsLimit;
    FMSD_gqcrSetRateLimit       gqcrSetRateLimit;
    FMSD_gqcrGetRateLimit       gqcrGetRateLimit;
    FMSD_gqcrSetMaxResidenceTime    gqcrSetMaxResidenceTime;
    FMSD_gqcrGetMaxResidenceTime    gqcrGetMaxResidenceTime;
    FMSD_gqcrFlushOneEntry          gqcrFlushOne;
    FMSD_gqcrFlushAllEntry          gqcrFlushAll;
    FMSD_gqcrSetQueueEnables           gqcrSetQueueEnables;
    FMSD_gqcrGetQueueEnables      gqcrGetQueueEnables;
} SwitchDevQCRObj_;

typedef struct
{
	FMSD_gqciStreamFiltFlushAll gqciStreamFiltFlushAll;
	FMSD_gqciStreamFiltSet gqciStreamFiltSet;
	FMSD_gqciStreamFiltBlockStatSet gqciStreamFiltBlockStatSet;
	FMSD_gqciStreamFiltBlockStatGet gqciStreamFiltBlockStatGet;
	FMSD_gqciStreamFiltEnAts gqciStreamFiltEnAts;
	FMSD_gqciStreamFiltEnGate gqciStreamFiltEnGate;
	FMSD_gqciStreamFiltMatchCtrGet gqciStreamFiltMatchCtrGet;
	FMSD_gqciStreamFiltFilterCtrGet gqciStreamFiltFilterCtrGet;
	FMSD_gqciStreamGateFiltCtrGet gqciStreamGateFiltCtrGet;
	FMSD_gqciFlowMeterFiltCtrGet gqciFlowMeterFiltCtrGet;

	FMSD_gqciStreamGateFlushAll gqciStreamGateFlushAll;
	FMSD_gqciStreamGateDefStateSet gqciStreamGateDefStateSet;
	FMSD_gqciStreamGateScaleSet gqciStreamGateScaleSet;
	FMSD_gqciStreamGateTimeGenIdSet gqciStreamGateTimeGenIdSet;
	FMSD_gqciStreamGateEntryAdd gqciStreamGateEntryAdd;

	FMSD_gqciStreamGateTimeGenFlush gqciStreamGateTimeGenFlush;
	FMSD_gqciStreamGateTimeGenSet gqciStreamGateTimeGenSet;

	FMSD_gqciFlowMeterFlushAll gqciFlowMeterFlushAll;
	FMSD_gqciFlowMeterCoupleFlagSet gqciFlowMeterCoupleFlagSet;
	FMSD_gqciFlowMeterCoupleFlagGet gqciFlowMeterCoupleFlagGet;
	FMSD_gqciFlowMeterDropOnYelSet gqciFlowMeterDropOnYelSet;
	FMSD_gqciFlowMeterDropOnYelGet gqciFlowMeterDropOnYelGet;
	FMSD_gqciFlowMeterRedBlockEnSet gqciFlowMeterRedBlockEnSet;
	FMSD_gqciFlowMeterRedBlockEnGet gqciFlowMeterRedBlockEnGet;
	FMSD_gqciFlowMeterRedBlkStatSet gqciFlowMeterRedBlkStatSet;
	FMSD_gqciFlowMeterRedBlkStatGet gqciFlowMeterRedBlkStatGet;
	FMSD_gqciFlowMeterAdvConfig gqciFlowMeterAdvConfig;

}SwitchDevQCIObj_;

typedef struct
{
    SwitchDevATUObj_ ATUObj;
	SwitchDevECIDObj_ ECIDObj;
	SwitchDevVCTObj_ VCTObj;
	SwitchDevSTUObj_ STUObj;
	SwitchDevVTUObj_ VTUObj;
	SwitchDevIMPObj_ IMPObj;
	SwitchDevEEPROMObj_ EEPROMObj;
	SwitchDevPHYCTRLObj_ PHYCTRLObj;
	SwitchDevPIRLObj_ PIRLObj;
	SwitchDevPORTCTRLObj_ PORTCTRLObj;
	SwitchDevPolicyObj_ POLICYObj;
	SwitchDevQoSMAPObj_ QOSMAPObj;
	SwitchDevPORTRMONObj_ PORTRMONObj;
    SwitchDevTrunkObj_  TrunkObj;
	SwitchDevQUEUEObj_ QUEUEObj;
	SwitchDevSYSCTRLObj_ SYSCTRLObj;
	SwitchDevQavObj_ QAVObj;
	SwitchDevPTPObj_ PTPObj;
	SwitchDevTCAMObj_ TCAMObj;
	SwitchDevTCAMRCObj_ TCAMRCObj;
	SwitchDevTCAMRCCompObj_ TCAMRCCompObj;
	SwitchDevARPObj_ ARPObj;
	SwitchDevRMUObj_  RMUObj;
	SwitchDevSmiPhyRegAccess_ SmiPhyRegAccessObj;
	SwitchDevLEDObj_ LEDObj;
	SwitchDevIntObj_ INTObj;
    SwitchDevQCRObj_ QCRObj;
    SwitchDevFRERObj_ FRERObj;
	SwitchDevQCIObj_ QCIObj;
	SwitchDevMACSECObj_ MACSECObj;
}SwitchDevObj_;

typedef enum
{
    MSD_UNKNOWN = 0,
    MSD_TOPAZ = 1,
	MSD_PERIDOT = 2,
	MSD_PEARL = 3,
	MSD_AGATE = 4,
	MSD_AMETHYST = 5,
	MSD_OAK = 6,
	MSD_SPRUCE = 7,
    MSD_BONSAIZ1 = 8,
    MSD_FIR = 9,
	MSD_BONSAI = 10
}MSD_SWITCH_NAME;

typedef enum
{
    MSD_SEM_EMPTY,
    MSD_SEM_FULL
} MSD_SEM_BEGIN_STATE;

typedef enum
{
    MSD_RMU_DSA_MODE,
    MSD_RMU_ETHERT_TYPE_DSA_MODE
}MSD_RMU_MODE;

/*
 * definitions for semaphore functions.
 *
*/
typedef MSD_SEM (*MSD_FMSD_SEM_CREATE)(
                        MSD_SEM_BEGIN_STATE state);
typedef MSD_STATUS (*MSD_FMSD_SEM_DELETE)(
                        MSD_SEM semId);
typedef MSD_STATUS (*MSD_FMSD_SEM_TAKE)(
                        MSD_SEM semId, MSD_U32 timOut);
typedef MSD_STATUS (*MSD_FMSD_SEM_GIVE)(
                        MSD_SEM semId);
						
/*
 * Typedef: struct MSD_QD_DEV
 *
 * Description: Includes Tapi layer switch configuration data.
 *
 * Fields:
 *   deviceId       - The device type identifier.
 *   revision       - The device revision number.
 *   baseRegAddr    - Switch Base Register address.
 *   numOfPorts     - Number of active ports.
 *   maxPorts       - max ports. This field is only for driver's use.
 *   cpuPortNum     - Logical port number whose physical port is connected to the CPU.
 *   maxPhyNum      - max configurable Phy address.
 *   stpMode        - current switch STP mode (0 none, 1 en, 2 dis)
 *   phyAddr        - SMI address used to access Switch registers(only for SMI_MULTI_ADDR_MODE).
 *   validPortVec   - valid port list in vector format
 *   validPhyVec    - valid phy list in vector format
 *   validSerdesVec - valid serdes list in vector format
 *   devName        - name of the device in group 0
 *   devName1       - name of the device in group 1
 *   multiAddrSem   - Semaphore for Accessing SMI Device
 *   atuRegsSem     - Semaphore for ATU access
 *   vtuRegsSem     - Semaphore for VTU access
 *   statsRegsSem   - Semaphore for RMON counter access
 *   pirlRegsSem    - Semaphore for PIRL Resource access
 *   ptpRegsSem     - Semaphore for PTP Resource access
 *   tblRegsSem     - Semaphore for various Table Resource access,
 *                    such as Trunk Tables and Device Table
 *   eepromRegsSem  - Semaphore for eeprom control access
 *   phyRegsSem     - Semaphore for PHY Device access
 *   fgtReadMii     - platform specific SMI register Read function
 *   fgtWriteMii    - platform specific SMI register Write function
 *   semCreate      - function to create semapore
 *   semDelete      - function to delete the semapore
 *   semTake        - function to get a semapore
 *   semGive        - function to return semaphore
 */
struct MSD_QD_DEV_
{
    MSD_DEVICE	deviceId;
    MSD_LPORT	cpuPortNum;
    MSD_U8		revision;
    MSD_U8		devNum;
    MSD_U8       devEnabled;
    MSD_U8       baseRegAddr;
	/*New*/
	MSD_U8       basePhyRegAddr;
    MSD_U8       numOfPorts;
    MSD_U8       maxPorts;
    MSD_U8       maxPhyNum;
    MSD_U8       stpMode;
    MSD_U8       accessMode;
    MSD_U8       phyAddr;
    MSD_U32      validPortVec;
    MSD_U16      validPhyVec;
    MSD_U16      validSerdesVec;
    MSD_SWITCH_NAME  devName;
    /* MSD_U32      devName1;*/

    MSD_SEM      multiAddrSem;
    MSD_SEM      atuRegsSem;
    MSD_SEM      vtuRegsSem;
    MSD_SEM      statsRegsSem;
    MSD_SEM      pirlRegsSem;
    MSD_SEM      ptpRegsSem;
    MSD_SEM      ptpTodSem;
    MSD_SEM      ptpModeSem;
    MSD_SEM      tblRegsSem;
    MSD_SEM      eepromRegsSem;
    MSD_SEM      phyRegsSem;
    MSD_SEM      qciRegsSem;
    MSD_SEM      qcrRegsSem;
	MSD_SEM      frerRegsSem;
	MSD_SEM      apbRegsSem;

	MSD_RMU_TX_RX_PAK rmu_tx_rx;
    MSD_RMU_MODE rmuMode;
    MSD_U32 eTypeValue;
	MSD_U8	reqSeqNum;

    MSD_FMSD_READ_MII  	fgtReadMii;
    MSD_FMSD_WRITE_MII 	fgtWriteMii;

    MSD_FMSD_SEM_CREATE  semCreate;     	/* create semaphore */
    MSD_FMSD_SEM_DELETE  semDelete;     	/* delete the semaphore */
    MSD_FMSD_SEM_TAKE    semTake;    	/* try to get a semaphore */
    MSD_FMSD_SEM_GIVE    semGive;    	/* return semaphore */

    MSD_INTERFACE    InterfaceChannel;

	MSD_BOOL           hwSemaphoreSupport;    /* true means the device support Hardware semaphore, false means do not support*/
	MSD_HWSEMAPHORE    HWSemaphore;

    SwitchDevObj_ SwitchDevObj;
};

/* Definition for BSP functions*/
typedef struct BSP_FUNCTIONS_
{
    MSD_RMU_TX_RX_PAK   rmu_tx_rx;      /* Send-Receive RMU Packets*/
 
    MSD_FMSD_READ_MII     readMii;       /* read MII Registers */
    MSD_FMSD_WRITE_MII     writeMii;     /* write MII Registers */

    MSD_FMSD_SEM_CREATE    semCreate;    /* create semapore */
    MSD_FMSD_SEM_DELETE    semDelete;    /* delete the semapore */
    MSD_FMSD_SEM_TAKE    semTake;        /* try to get a semapore */
    MSD_FMSD_SEM_GIVE    semGive;        /* return semaphore */
} MSD_BSP_FUNCTIONS;

/* System configuration Parameters struct*/
typedef struct
{
    MSD_U8         	devNum;		     /* Device Number, 0~31 */
	MSD_U8			baseAddr;	     /* Physical Device Address */
    MSD_RMU_MODE    rmuMode;         /* RMU mode*/
    MSD_U32         eTypeValue;      /* Ether type value for Ether type DSA mode*/

	MSD_BSP_FUNCTIONS	BSPFunctions;    /* register APIs */
	MSD_INTERFACE    InterfaceChannel;   /* select register access bus, SMI or SMI multi-chip mode or RMU */
	MSD_U16         tempDeviceId;       /* temp device Id for some device no eFuse */
}MSD_SYS_CONFIG;


/****************************************************************************/
/* Exported System Config Functions			                                   */
/****************************************************************************/

/*******************************************************************************
* msdLoadDriver
*
* DESCRIPTION:
*       QuarterDeck Driver Initialization Routine. 
*       This is the first routine that needs be called by system software. 
*       It takes *cfg from system software, and retures a pointer (*dev) 
*       to a data structure which includes infomation related to this QuarterDeck
*       device. This pointer (*dev) is then used for all the API functions. 
*
* INPUTS:
*       cfg  - Holds device configuration parameters provided by system software.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK               - on success
*       MSD_FAIL             - on error
*       MSD_ALREADY_EXIST    - if device already started
*       MSD_BAD_PARAM        - on bad parameters
*
* COMMENTS:
*     msdUnLoadDriver is also provided to do driver cleanup.
*
*******************************************************************************/
MSD_STATUS msdLoadDriver
(
    IN  MSD_SYS_CONFIG   *cfg
);

/*******************************************************************************
* msdUnLoadDriver
*
* DESCRIPTION:
*       This function unloads the QuaterDeck Driver.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK           - on success
*       MSD_FAIL         - on error
*
* COMMENTS:
*       1.  This function should be called only after successful execution of
*           msdLoadDriver().
*
*******************************************************************************/
MSD_STATUS msdUnLoadDriver
(
    IN MSD_U8 devNum
);

/*******************************************************************************
* msdSetDriverInterface
*
* DESCRIPTION:
*       This function set the interface channel.
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*       channel -   interface channel(smi, smi multichip, rmu)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdSetDriverInterface
(
    IN MSD_U8 devNum,
    IN MSD_INTERFACE channel
);

/*******************************************************************************
* msdGetDriverInterface
*
* DESCRIPTION:
*       This function get the current interface channel.
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*
* OUTPUTS:
*       channel -   interface channel(smi, smi multichip, rmu)
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdGetDriverInterface
(
    IN  MSD_U8 devNum,
    OUT MSD_INTERFACE *channel
);

/*******************************************************************************
* sohoDevGet
*
* DESCRIPTION:
*       get the soho qdDev by devNumm
*
* INPUTS:
*       devNum  - the device number
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       pointer to the soho qdDev   - on success.
*       NULL                        - on failure.
*
* COMMENTS:
*
*******************************************************************************/
MSD_QD_DEV* sohoDevGet
(
	IN  MSD_U8 devNum
);
/*******************************************************************************
* msdVersion
*
* DESCRIPTION:
*       This function returns the version of the QuarterDeck SW suite.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - QuarterDeck software version.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_BAD_PARAM on bad parameters,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVersion
(
    OUT MSD_VERSION   *version
);


/*******************************************************************************
* msdlport2port
*
* DESCRIPTION:
*       This function converts logical port number to physical port number
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdlport2port
(
    IN MSD_U32    portVec,
    IN MSD_LPORT     port
);

/*******************************************************************************
* msdport2lport
*
* DESCRIPTION:
*       This function converts physical port number to logical port number
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_LPORT msdport2lport
(
    IN MSD_U32    portVec,
    IN MSD_U8     hwPort
);

/*******************************************************************************
* msdlportvec2portvec
*
* DESCRIPTION:
*       This function converts logical port vector to physical port vector
*
* INPUTS:
*        portVec - physical port list in vector
*        lVec     - logical port vector
* OUTPUTS:
*        None.
* RETURNS:
*       physical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U32 msdlportvec2portvec
(
    IN MSD_U32    portVec,
    IN MSD_U32     lVec
);

/*******************************************************************************
* msdportvec2lportvec
*
* DESCRIPTION:
*       This function converts physical port vector to logical port vector
*
* INPUTS:
*        portVec - physical port list in vector
*        pVec     - physical port vector
* OUTPUTS:
*        None.
* RETURNS:
*       logical port vector
*
* COMMENTS:
*
*******************************************************************************/
MSD_U32 msdportvec2lportvec
(
    IN MSD_U32    portVec,
    IN MSD_U32     pVec
);

/*******************************************************************************
* msdlport2phy
*
* DESCRIPTION:
*       This function converts logical port number to physical phy number.
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical port number
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdlport2phy
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port
);

/*******************************************************************************
* msdlport2serdes
*
* DESCRIPTION:
*       This function converts logical port number to physical serdes phy address.
*
* INPUTS:
*        portVec - physical port list in vector
*        port    - logical port number
* OUTPUTS:
*        None.
* RETURNS:
*       physical serdes phy address
*
* COMMENTS:
*
*******************************************************************************/
MSD_U8 msdlport2serdes
(
    IN MSD_QD_DEV    *dev,
    IN MSD_LPORT     port
);

/*******************************************************************************
* msdEnableHwSemaphore
*
* DESCRIPTION:
*       This function is used to enable the device to use hardware semaphore
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - This request is not support
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdEnableHwSemaphore
(
	IN MSD_U8 devNum
);

/*******************************************************************************
* msdEnableHwSemaphore
*
* DESCRIPTION:
*       This function is used to disable the device to use hardware semaphore
*
* INPUTS:
*       devNum  -   device Num(0 ~ 31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   -   on success
*       MSD_FAIL -   on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - This request is not support
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS msdDisableHwSemaphore
(
	IN MSD_U8 devNum
);


#ifdef __cplusplus
}
#endif

#endif /* __msdSysConfig_h */
