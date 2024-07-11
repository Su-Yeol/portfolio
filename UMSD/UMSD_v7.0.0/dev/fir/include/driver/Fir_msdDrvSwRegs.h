/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
 * msdDrvSwRegs.h
 *
 * DESCRIPTION:
 *       definitions of the register map of Switch Device
 *
 * DEPENDENCIES:
 *
 * FILE REVISION NUMBER:
 ******************************************************************************/
#ifndef __Fir__msdDrvSwRegs_H
#define __Fir__msdDrvSwRegs_H
#ifdef __cplusplus
extern "C" {
#endif



/* Define Switch SMI Device Address */
#define FIR_PORT_START_ADDR     0x0     /* Start Port Address */
#define FIR_GLOBAL1_DEV_ADDR    0x1B    /* Global 1 Device Address */
#define FIR_GLOBAL2_DEV_ADDR    0x1CU    /* Global 2 Device Address */
#define FIR_TCAM_DEV_ADDR       0x1F    /* TCAM Device Address */

/* Switch Per Port Registers */
#define FIR_PORT_STATUS          0x0	    /* Port Status Register */
#define FIR_PHY_CONTROL          0x1		/* Physical Control Register */
#define FIR_LIMIT_PAUSE_CONTROL  0x2		/* Flow Control Register */
#define FIR_SWITCH_ID            0x3		/* Switch Identifier Register */
#define FIR_PORT_CONTROL         0x4		/* Port Control Register */
#define FIR_PORT_CONTROL1        0x5		/* Port Control 1 Register */
#define FIR_PORT_VLAN_MAP        0x6		/* Port based VLAN Map Register */
#define FIR_PVID                 0x7		/* Port VLAN ID & Priority Register */
#define FIR_PORT_CONTROL2        0x8		/* Port Control 2 Register */
#define FIR_EGRESS_RATE_CTRL     0x9		/* Egress Rate Control Register */
#define FIR_EGRESS_RATE_CTRL2    0xA		/* Egress Rate Control 2 Register */
#define FIR_PAV                  0xB		/* Port Association Vector Register */
#define FIR_PORT_ATU_CONTROL     0xC		/* Port ATU Control Register */
#define FIR_PRI_OVERRIDE         0xD		/* Override Register */
#define FIR_POLICY_CONTROL       0xE		/* Policy & MGMT Control Register */
#define FIR_EXT_PORT_CTRL_CMD	 0x10	    /*Extended Port Control Operation Register*/
#define FIR_EXT_PORT_CTRL_DATA   0x11	    /*Extended Port Control Data Register*/
#define FIR_PREEMPTION_CONTROL   0x15		/* Preemption Control Register */
#define FIR_LED_CONTROL          0x16		/* LED Control Register */
#define FIR_IP_PRI_MAPPING_TBL   0x17		/* IP Priority Mapping Table Register */
#define FIR_IEEE_PRI_MAPPING_TBL 0x18		/* IEEE Priority Mapping Table Register */
#define FIR_PORT_CONTROL3		0x19		/* Port Control 3 Register */
#define FIR_PORT_MISC_SCRATCH   0x1A        /* Port Misc & Scratch*/
#define FIR_Q_COUNTER           0x1B		/* Queue Counter Register */
#define FIR_Q_CONTROL           0x1C		/* Port Queue Control Register */
#define FIR_Q_CONTROL2          0x1D        /* Port Queue Control 2 Register */
#define FIR_CT_CONTROL          0x1E		/* Cut Through Control Register */
#define FIR_DBG_COUNTER         0x1F		/* Debug Counters Register */

    /* Switch Global Registers */
#define FIR_GLOBAL_STATUS       0x0		/* Global Status Register */
#define FIR_ATU_FID_REG        	0x1		/* ATU FID Register */
#define FIR_VTU_FID_REG        	0x2		/* VTU FID Register */
#define FIR_STU_SID_REG        	0x3		/* VTU SID Register */
#define FIR_GLOBAL_CONTROL      0x4		/* Global Control Register */
#define FIR_VTU_OPERATION      	0x5		/* VTU Operation Register */
#define FIR_VTU_VID_REG        	0x6		/* VTU VID Register */
#define FIR_VTU_DATA1_REG      	0x7		/* VTU/STU Data Register Ports 0 to 7 */
#define FIR_VTU_DATA2_REG      	0x8		/* VTU/STU Data Register Ports 8 to 10 and Priority */
#define FIR_ATU_CTRL2_REG       0x9     /* ATU */
#define FIR_ATU_CTRL_REG        0xA		/* ATU Control Register */
#define FIR_ATU_OPERATION      	0xB		/* ATU Operation Register */
#define FIR_ATU_DATA_REG       	0xC		/* ATU Data Register */
#define FIR_ATU_MAC_BASE       	0xD		/* ATU MAC Address Byte 0 & 1 Register Base or LAD ID */
#define FIR_ATU_MAC1_BASE       0xE		/* ATU MAC Address Byte 2 & 3 Register Base */
#define FIR_ATU_MAC2_BASE       0xF		/* ATU MAC Address Byte 4 & 5 Register Base or GRP & ECID */
#define FIR_EXTENDED_G_CONTROL_CMD      0x10    /* Extended Global Control CMD */
#define FIR_EXTENDED_G_CONTROL_DATA     0x11    /* Extended Global Control DATA */
#define FIR_SEMAPHONE            0x15    /* Semaphone */
#define FIR_RELIABLE_SMI_CRC     0x18    /* Reliable SMI CRC */
#define FIR_MONITOR_MGMT_CTRL    0x1A    /* Monitor & MGMT Control Register */
#define FIR_TOTAL_FREE_COUNTER   0x1B    /* Total Free Counter Register */
#define FIR_GLOBAL_CONTROL2      0x1C    /* Global Control 2 Register */
#define FIR_STATS_OPERATION    	0x1D    /* Stats Operation Register */
#define FIR_STATS_COUNTER3_2   	0x1E    /* Stats Counter Register Bytes 3 & 2 */
#define FIR_STATS_COUNTER1_0   	0x1F    /* Stats Counter Register Bytes 1 & 0 */

/*Multi Chip Directly access Registers*/
#define FIR_TOP_LEVEL_INTERRUPT         0x4
#define FIR_ATU_FID_REG_MULTICHIP     	0x9
#define FIR_IGR_RATE_COMMAND_MULTICHIP  0x14
#define FIR_IGR_RATE_DATA_MULTICHIP   	0x15

/* Switch Global 2 Registers */
#define FIR_INT_SOURCE     		0x0		/* Interrupt Source Register */
#define FIR_INT_MASK       		0x1		/* Interrupt Mask Register */
#define FIR_INT2_SOURCE     	0x2		/* Interrupt 2 Source Register */
#define FIR_INT2_MASK       	0x3		/* Interrupt 2 Mask Register */
#define FIR_FLOWCTRL_DELAY    	0x4		/* Flow Control Delay Register */
#define FIR_MANAGEMENT        	0x5		/* Switch Management Register */
#define FIR_ROUTING_TBL       	0x6		/* Device Mapping Table Register */
#define FIR_TRUNK_MASK_TBL    	0x7		/* Trunk Mask Table Register */
#define FIR_TRUNK_ROUTING     	0x8		/* Trunk Mapping Register */	
#define FIR_IGR_RATE_COMMAND    0x9		/* Ingress Rate Command Register */	
#define FIR_IGR_RATE_DATA   	0xA		/* Ingress Rate Data Register */	
#define FIR_PVT_ADDR            0xB		/* Cross Chip Port VLAN Addr Register */	
#define FIR_PVT_DATA            0xC		/* Cross Chip Port VLAN Data Register */	
#define FIR_SWITCH_MAC          0xD		/* Switch MAC/WoL/WoF Register */	
#define FIR_ATU_STATS           0xE		/* ATU Stats Register */	
#define FIR_PRIORITY_OVERRIDE 	0xF		/* Priority Override Table Register */	
#define FIR_I2C_BLOCK           0x10    /* I2C Block*/
#define FIR_AHB_MASTER_CTRL     0x11    /* AHB Master Controller*/
#define FIR_ENERGE_MNT        	0x12	/* Energy Management Register */
#define FIR_IMP_COMM_DBG		0x13	/* IMP Comm/Debug Register */
#define FIR_EEPROM_COMMAND    	0x14	/* EEPROM Command Register */	
#define FIR_EEPROM_DATA      	0x15	/* EEPROM Address Register */	
#define FIR_AVB_COMMAND     	0x16U	/* AVB Command Register */	
#define FIR_AVB_DATA          	0x17U	/* AVB Data Register */
#define FIR_SMI_PHY_CMD       	0x18	/* SMI PHY Command Register */	
#define FIR_SMI_PHY_DATA      	0x19	/* SMI PHY Data Register */	
#define FIR_SCRATCH_MISC      	0x1A	/* Scratch & Misc Register */	
#define FIR_WD_CONTROL        	0x1B	/* Watch Dog Control Register */	
#define FIR_QOS_WEIGHT        	0x1C	/* QoS Weights Register */	
#define FIR_MISC				0x1D	/* Misc Register */	
#define FIR_CUT_THROU_CTRL    	0x1F	/* Cut Through Control Register */	

/* TCAM Registers - Page 0 */
#define FIR_TCAM_OPERATION         0x0		/* TCAM Operation Register */
#define FIR_TCAM_P0_KEYS_1         0x2		/* TCAM Key Register 1 */
#define FIR_TCAM_P0_KEYS_2         0x3		/* TCAM Key Register 2 */
#define FIR_TCAM_P0_KEYS_3         0x4		/* TCAM Key Register 3 */
#define FIR_TCAM_P0_KEYS_4         0x5		/* TCAM Key Register 4 */
#define FIR_TCAM_P0_MATCH_DATA_1   0x6		/* TCAM Match Data Register 1 */
#define FIR_TCAM_P0_MATCH_DATA_2   0x7		/* TCAM Match Data Register 2 */
#define FIR_TCAM_P0_MATCH_DATA_3   0x8
#define FIR_TCAM_P0_MATCH_DATA_4   0x9
#define FIR_TCAM_P0_MATCH_DATA_5   0xa
#define FIR_TCAM_P0_MATCH_DATA_6   0xb
#define FIR_TCAM_P0_MATCH_DATA_7   0xc
#define FIR_TCAM_P0_MATCH_DATA_8   0xd
#define FIR_TCAM_P0_MATCH_DATA_9   0xe
#define FIR_TCAM_P0_MATCH_DATA_10  0xf
#define FIR_TCAM_P0_MATCH_DATA_11  0x10
#define FIR_TCAM_P0_MATCH_DATA_12  0x11
#define FIR_TCAM_P0_MATCH_DATA_13  0x12
#define FIR_TCAM_P0_MATCH_DATA_14  0x13
#define FIR_TCAM_P0_MATCH_DATA_15  0x14
#define FIR_TCAM_P0_MATCH_DATA_16  0x15
#define FIR_TCAM_P0_MATCH_DATA_17  0x16
#define FIR_TCAM_P0_MATCH_DATA_18  0x17
#define FIR_TCAM_P0_MATCH_DATA_19  0x18
#define FIR_TCAM_P0_MATCH_DATA_20  0x19
#define FIR_TCAM_P0_MATCH_DATA_21  0x1a
#define FIR_TCAM_P0_MATCH_DATA_22  0x1b

    /* TCAM Registers - Page 1 */
#define FIR_TCAM_P1_MATCH_DATA_23   0x2		/* TCAM Match Data Register 23 */
#define FIR_TCAM_P1_MATCH_DATA_24   0x3		/* TCAM Match Data Register 24 */
#define FIR_TCAM_P1_MATCH_DATA_25   0x4
#define FIR_TCAM_P1_MATCH_DATA_26   0x5
#define FIR_TCAM_P1_MATCH_DATA_27   0x6
#define FIR_TCAM_P1_MATCH_DATA_28   0x7
#define FIR_TCAM_P1_MATCH_DATA_29   0x8
#define FIR_TCAM_P1_MATCH_DATA_30   0x9
#define FIR_TCAM_P1_MATCH_DATA_31   0xa
#define FIR_TCAM_P1_MATCH_DATA_32   0xb
#define FIR_TCAM_P1_MATCH_DATA_33   0xc
#define FIR_TCAM_P1_MATCH_DATA_34   0xd
#define FIR_TCAM_P1_MATCH_DATA_35   0xe
#define FIR_TCAM_P1_MATCH_DATA_36   0xf
#define FIR_TCAM_P1_MATCH_DATA_37   0x10
#define FIR_TCAM_P1_MATCH_DATA_38   0x11
#define FIR_TCAM_P1_MATCH_DATA_39   0x12
#define FIR_TCAM_P1_MATCH_DATA_40   0x13
#define FIR_TCAM_P1_MATCH_DATA_41   0x14
#define FIR_TCAM_P1_MATCH_DATA_42   0x15
#define FIR_TCAM_P1_MATCH_DATA_43   0x16
#define FIR_TCAM_P1_MATCH_DATA_44   0x17
#define FIR_TCAM_P1_MATCH_DATA_45   0x18
#define FIR_TCAM_P1_MATCH_DATA_46   0x19
#define FIR_TCAM_P1_MATCH_DATA_47   0x1a
#define FIR_TCAM_P1_MATCH_DATA_48   0x1b

    /* TCAM Registers - Page 2 */
#define FIR_TCAM_P2_ACTION_1        0x2		/* TCAM Action 1 Register */
#define FIR_TCAM_P2_ACTION_2        0x3		/* TCAM Action 2 Register */
#define FIR_TCAM_P2_ACTION_3        0x4		/* TCAM Action 3 Register */
#define FIR_TCAM_P2_ACTION_5        0x6		/* TCAM Action 5 Register */
#define FIR_TCAM_P2_ACTION_6        0x7		/* TCAM Action 6 Register */
#define FIR_TCAM_P2_ACTION_7        0x8		/* TCAM Action 7 Register */
#define FIR_TCAM_P2_DEBUG_PORT      0x1C		/* TCAM Debug Register */
#define FIR_TCAM_P2_ALL_HIT         0x1F		/* TCAM Debug Register 2 */

    /* TCAM Registers - Page 3 */
#define FIR_TCAM_EGR_PORT           0x1		/* Egress TCAM Port Register */
#define FIR_TCAM_EGR_ACTION_1       0x2		/* Egress TCAM Action 1 Register */
#define FIR_TCAM_EGR_ACTION_2       0x3		/* Egress TCAM Action 2 Register */
#define FIR_TCAM_EGR_ACTION_3       0x4		/* Egress TCAM Action 3 Register */


/****************************************************************************/
/*																			*/
/* Block: INTERNAL PHY Registers											*/
/*																			*/
/****************************************************************************/

/* Internal Copper PHY registers*/
#define FIR_PHY_CONTROL_REG          0
#define FIR_PHY_AUTONEGO_AD_REG      4
#define FIR_PHY_AUTONEGO_1000AD_REG  9
#define FIR_PHY_SPEC_CONTROL_REG		16
#define FIR_PHY_PAGE_ANY_REG			22

    /* Definitions for VCT registers */
#define FIR_MDI0_VCT_STATUS     16
#define FIR_MDI1_VCT_STATUS     17
#define FIR_MDI2_VCT_STATUS     18
#define FIR_MDI3_VCT_STATUS     19
#define FIR_ADV_VCT_CONTROL_5   23
#define FIR_ADV_VCT_SMPL_DIST   24
#define FIR_ADV_VCT_CONTROL_8   20
#define FIR_PAIR_SKEW_STATUS    20
#define FIR_PAIR_SWAP_STATUS    21

    /* Bit Definition for QD_PHY_CONTROL_REG */
#define FIR_PHY_RESET            0x8000
#define FIR_PHY_LOOPBACK         0x4000
#define FIR_PHY_SPEED            0x2000
#define FIR_PHY_AUTONEGO         0x1000
#define FIR_PHY_POWER            0x800
#define FIR_PHY_ISOLATE          0x400
#define FIR_PHY_RESTART_AUTONEGO 0x200
#define FIR_PHY_DUPLEX           0x100
#define FIR_PHY_SPEED_MSB        0x40

    /* Bit Definition for QD_PHY_AUTONEGO_AD_REG */
#define FIR_PHY_NEXTPAGE           0x8000
#define FIR_PHY_REMOTEFAULT        0x4000
#define FIR_PHY_PAUSE              0x400
#define FIR_PHY_100_FULL           0x100
#define FIR_PHY_100_HALF           0x80
#define FIR_PHY_10_FULL            0x40
#define FIR_PHY_10_HALF            0x20

#define FIR_PHY_MODE_AUTO_AUTO    (FIR_PHY_100_FULL | FIR_PHY_100_HALF | FIR_PHY_10_FULL | FIR_PHY_10_HALF)
#define FIR_PHY_MODE_100_AUTO     (FIR_PHY_100_FULL | FIR_PHY_100_HALF)
#define FIR_PHY_MODE_10_AUTO      (FIR_PHY_10_FULL  | FIR_PHY_10_HALF)
#define FIR_PHY_MODE_AUTO_FULL    (FIR_PHY_100_FULL | FIR_PHY_10_FULL)
#define FIR_PHY_MODE_AUTO_HALF    (FIR_PHY_100_HALF | FIR_PHY_10_HALF)

#define FIR_PHY_MODE_100_FULL    (FIR_PHY_100_FULL)
#define FIR_PHY_MODE_100_HALF    (FIR_PHY_100_HALF)
#define FIR_PHY_MODE_10_FULL     (FIR_PHY_10_FULL)   
#define FIR_PHY_MODE_10_HALF     (FIR_PHY_10_HALF)   

    /* Gigabit Phy related definition */
#define FIR_GIGPHY_1000X_FULL_CAP    0x8
#define FIR_GIGPHY_1000X_HALF_CAP    0x4
#define FIR_GIGPHY_1000T_FULL_CAP    0x2
#define FIR_GIGPHY_1000T_HALF_CAP    0x1

#define FIR_GIGPHY_1000X_CAP        (FIR_GIGPHY_1000X_FULL_CAP|FIR_GIGPHY_1000X_HALF_CAP)
#define FIR_GIGPHY_1000T_CAP        (FIR_GIGPHY_1000T_FULL_CAP|FIR_GIGPHY_1000T_HALF_CAP)

#define FIR_GIGPHY_1000X_FULL        0x20
#define FIR_GIGPHY_1000X_HALF        0x40

#define FIR_GIGPHY_1000T_FULL        0x200
#define FIR_GIGPHY_1000T_HALF        0x100


/*************************************************************************************************/
/*																								 */
/* Block: Switch Registers																		 */
/*																								 */
/*************************************************************************************************/
#define RANGE4 4
#define RANGE8 8
#define RANGE16 16
#define RANGE32 32
#define RANGE64 64

#define RANGE128 128





/* Register */
#define FIR_MSEC_PORT_BASE 0x50070000
#define FIR_MSEC_GLOBAL_CTL_STS_BASE 0x50071000
#define FIR_MSEC_TCAM_TBL_BASE 0x50071080
#define FIR_MSEC_ACTION_TBL_BASE 0x50071100
#define FIR_MSEC_RX_SECY_BASE 0x50071180
#define FIR_MSEC_TX_SecY_BASE 0x50071200
#define FIR_MSEC_RX_SC_BASE 0x50071280
#define FIR_MSEC_TX_SC_BASE 0x50071300
#define FIR_MSEC_RX_SA_BASE 0x50071380
#define FIR_MSEC_TX_SA_BASE 0x50071400
#define FIR_MSEC_SECY_STAT_CAP_BASE 0x50071800
#define FIR_MSEC_SC_STAT_CAP_BASE 0x50071880
#define FIR_MSEC_SECY_STAT_DIR_BASE 0x50072000
#define FIR_MSEC_SC_STAT_DIR_BASE 0x50073000
#define FIR_MSEC_SAK_BASE 0x50078000
#define FIR_MSEC_ENGINEER_BASE 0x50077F80


/* MACsec Per Port Registers */
#define FIR_MSEC_RX_PORT_CTL_OFFSET         0x0
#define FIR_MSEC_DEF_RXSCI_7_4_OFFSET       0x4
#define FIR_MSEC_DEF_RXSCI_3_0_OFFSET       0x8
#define FIR_MSEC_RX_PORT_CTL_2_OFFSET       0xC
#define FIR_MSEC_TX_PORT_CTL_OFFSET         0x40


/* MACsec Global Control & Status Registers */
#define FIR_MSEC_INT_EN_OFFSET              0x0
#define FIR_MSEC_GLOBAL_CTL_OFFSET          0x4
#define FIR_MSEC_INT_OP_OFFSET              0x8
#define FIR_MSEC_STATS_OP_OFFSET            0xC
#define FIR_MSEC_TXPNTHR_3_0_OFFSET         0x10
#define FIR_MSEC_TXXPNTHR_7_4_OFFSET        0x14
#define FIR_MSEC_TXXPNTHR_3_0_OFFSET        0x18
#define FIR_MSEC_FUNC_SAFETY_CTL_OFFSET     0x40
#define FIR_MSEC_ECC_ERR_EVTS_OFFSET        0x44
#define FIR_MSEC_ECC_MISMAC_EVTS_OFFSET     0x48
#define FIR_MSEC_SMS_RPE_EVTS_OFFSET        0x4C


/* MACsec TACM Table Registers */
#define FIR_MSEC_TCAM_OP_OFFSET             0x0
#define FIR_MSEC_TCAM_SPV_OFFSET            0x4
#define FIR_MSEC_TCAM_KEYS_OFFSET           0xC
#define FIR_MSEC_TCAM_DA_5_4_OFFSET         0x10
#define FIR_MSEC_TCAM_DA_3_2_OFFSET         0x14
#define FIR_MSEC_TCAM_DA_1_0_OFFSET         0x18
#define FIR_MSEC_TCAM_SA_5_4_OFFSET         0x1C
#define FIR_MSEC_TCAM_SA_3_2_OFFSET         0x20
#define FIR_MSEC_TCAM_SA_1_0_OFFSET         0x24
#define FIR_MSEC_TCAM_SVLAN_TAG_OFFSET      0x28
#define FIR_MSEC_TCAM_QVLAN_TAG_OFFSET      0x2C
#define FIR_MSEC_TCAM_UN_ETYPE_OFFSET       0x30


/* MACsec ACtion Table Registers */
#define FIR_MSEC_ACTION_OP_OFFSET           0x0
#define FIR_MSEC_ACTION_DATA_OFFSET         0x4
#define FIR_MSEC_TCAM_DEBUG_OFFSET          0x7C


/* MACsec Receive SECY Registers */
#define FIR_MSEC_RX_SECY_OP_OFFSET          0x0
#define FIR_MSEC_RX_SECY_CTL_OFFSET         0x4
#define FIR_MSEC_RX_REP_WIN_OFFSET          0x8
#define FIR_MSEC_RX_SECY_INT_OFFSET         0xC


/* MACsec Transmit SECY Registers */
#define FIR_MSEC_TX_SECY_OP_OFFSET          0x0
#define FIR_MSEC_TX_SECY_CTL_OFFSET         0x4
#define FIR_MSEC_TX_SECTAG_CTL_OFFSET       0x8
#define FIR_MSEC_TX_SECY_INT_OFFSET         0xC


/* MACsec Receive SC Registers */
#define FIR_MSEC_RX_SC_OP_OFFSET            0x0
#define FIR_MSEC_RX_SC_CNT_INDX_OFFSET      0x4
#define FIR_MSEC_RX_SCI_7_4_OFFSET          0x8
#define FIR_MSEC_RX_SCI_3_0_OFFSET          0xC
#define FIR_MSEC_RX_SC_CTL_OFFSET           0x10
#define FIR_MSEC_RX_SC_INT_OFFSET           0x14
#define FIR_MSEC_RX_SC_DEBUG_OFFSET         0x7C


/* MACsec Transmit SC Registers */
#define FIR_MSEC_TX_SC_OP_OFFSET            0x0
#define FIR_MSEC_TX_SC_CNT_INDX_OFFSET      0x4
#define FIR_MSEC_TX_SCI_7_4_OFFSET          0x8
#define FIR_MSEC_TX_SCI_3_0_OFFSET          0xC
#define FIR_MSEC_TX_SC_CTL_OFFSET           0x10
#define FIR_MSEC_TX_SC_INT_OFFSET           0x14
#define FIR_MSEC_TX_SC_DEBUG_OFFSET         0x7C


/* MACsec Receive SA Registers */
#define FIR_MSEC_RX_SA_OP_OFFSET            0x0
#define FIR_MSEC_RX_SA_CTL_OFFSET           0x4
#define FIR_MSEC_RX_SSCI_OFFSET             0x8
#define FIR_MSEC_RX_SALT_11_8_OFFSET        0xC
#define FIR_MSEC_RX_SALT_7_4_OFFSET         0x10
#define FIR_MSEC_RX_SALT_3_0_OFFSET         0x14
#define FIR_MSEC_RX_NXTPN_7_4_OFFSET        0x18
#define FIR_MSEC_RX_NXTPN_3_0_OFFSET        0x1C


/* MACsec Transmit SA Registers */
#define FIR_MSEC_TX_SA_OP_OFFSET            0x0
#define FIR_MSEC_TX_SA_CTL_OFFSET           0x4
#define FIR_MSEC_TX_SSCI_OFFSET             0x8
#define FIR_MSEC_TX_SALT_11_8_OFFSET        0xC
#define FIR_MSEC_TX_SALT_7_4_OFFSET         0x10
#define FIR_MSEC_TX_SALT_3_0_OFFSET         0x14
#define FIR_MSEC_TX_NXTPN_7_4_OFFSET        0x18
#define FIR_MSEC_TX_NXTPN_3_0_OFFSET        0x1C


/* MACsec SECY Stat Counters (Captured) Registers */
#define FIR_MSEC_RX_CPTOCT_HICAP_OFFSET            0x0
#define FIR_MSEC_RX_CPTOCT_LOCAP_OFFSET            0x4
#define FIR_MSEC_RX_VLDOCT_HICAP_OFFSET            0x8
#define FIR_MSEC_RX_VLDOCT_LOCAP_OFFSET            0xC
#define FIR_MSEC_RX_DECOCT_HICAP_OFFSET            0x10
#define FIR_MSEC_RX_DECOCT_LOCAP_OFFSET            0x14
#define FIR_MSEC_RX_CPTUNIFR_CAP_OFFSET            0x18
#define FIR_MSEC_RX_CPTMULFR_CAP_OFFSET            0x1C
#define FIR_MSEC_RX_CPTBRDFR_CAP_OFFSET            0x20
#define FIR_MSEC_RX_UNTAGGED_CAP_OFFSET            0x24
#define FIR_MSEC_RX_NOTAG_CAP_OFFSET               0x28
#define FIR_MSEC_RX_BADTAG_CAP_OFFSET              0x2C
#define FIR_MSEC_RX_NOSA_CAP_OFFSET                0x30
#define FIR_MSEC_RX_NOSAERR_CAP_OFFSET             0x34

#define FIR_MSEC_TX_CPTOCT_HICAP_OFFSET            0x40
#define FIR_MSEC_TX_CPTOCT_LOCAP_OFFSET            0x44
#define FIR_MSEC_TX_ENCOCT_HICAP_OFFSET            0x48
#define FIR_MSEC_TX_ENCOCT_LOCAP_OFFSET            0x4C
#define FIR_MSEC_TX_PROOCT_HICAP_OFFSET            0x50
#define FIR_MSEC_TX_PROOCT_LOCAP_OFFSET            0x54
#define FIR_MSEC_TX_CPTUNIFR_CAP_OFFSET            0x58
#define FIR_MSEC_TX_CPTMULFR_CAP_OFFSET            0x5C
#define FIR_MSEC_TX_CPTBRDFR_CAP_OFFSET            0x60
#define FIR_MSEC_TX_UNTAG_CAP_OFFSET               0x64
#define FIR_MSEC_TX_TOOLONG_CAP_OFFSET             0x68
#define FIR_MSEC_TX_NOSAERROR_CAP_OFFSET           0x6C


/* MACsec SC Stat Counters (Captured) Registers */
#define FIR_MSEC_RX_SC_OK_CAP_OFFSET               0x0
#define FIR_MSEC_RX_SC_UNCHK_CAP_OFFSET            0x4
#define FIR_MSEC_RX_SC_DLYED_CAP_OFFSET            0x8
#define FIR_MSEC_RX_SC_LATE_CAP_OFFSET             0xC
#define FIR_MSEC_RX_SC_INVLD_CAP_OFFSET            0x10
#define FIR_MSEC_RX_SC_NOVLD_CAP_OFFSET            0x14
#define FIR_MSEC_TX_SC_ENC_CAP_OFFSET              0x40
#define FIR_MSEC_TX_SC_PRO_CAP_OFFSET              0x44


/* MACsec SECY Stat Counters (Direct) Registers */
#define FIR_MSEC_RX_CPTOCT_HIDIR_OFFSET            0x0
#define FIR_MSEC_RX_CPTOCT_LODIR_OFFSET            0x4
#define FIR_MSEC_RX_VLDOCT_HIDIR_OFFSET            0x8
#define FIR_MSEC_RX_VLDOCT_LODIR_OFFSET            0xC
#define FIR_MSEC_RX_DECOCT_HIDIR_OFFSET            0x10
#define FIR_MSEC_RX_DECOCT_LODIR_OFFSET            0x14
#define FIR_MSEC_RX_CPTUNIFR_DIR_OFFSET            0x18
#define FIR_MSEC_RX_CPTMULFR_DIR_OFFSET            0x1C
#define FIR_MSEC_RX_CPTBRDFR_DIR_OFFSET            0x20
#define FIR_MSEC_RX_UNTAGGED_DIR_OFFSET            0x24
#define FIR_MSEC_RX_NOTAG_DIR_OFFSET               0x28
#define FIR_MSEC_RX_BADTAG_DIR_OFFSET              0x2C
#define FIR_MSEC_RX_NOSA_DIR_OFFSET                0x30
#define FIR_MSEC_RX_NOSAERR_DIR_OFFSET             0x34

#define FIR_MSEC_TX_CPTOCT_HIDIR_OFFSET            0x40
#define FIR_MSEC_TX_CPTOCT_LODIR_OFFSET            0x44
#define FIR_MSEC_TX_ENCOCT_HIDIR_OFFSET            0x48
#define FIR_MSEC_TX_ENCOCT_LODIR_OFFSET            0x4C
#define FIR_MSEC_TX_PROOCT_HIDIR_OFFSET            0x50
#define FIR_MSEC_TX_PROOCT_LODIR_OFFSET            0x54
#define FIR_MSEC_TX_CPTUNIFR_DIR_OFFSET            0x58
#define FIR_MSEC_TX_CPTMULFR_DIR_OFFSET            0x5C
#define FIR_MSEC_TX_CPTBRDFR_DIR_OFFSET            0x60
#define FIR_MSEC_TX_UNTAG_DIR_OFFSET               0x64
#define FIR_MSEC_TX_TOOLONG_DIR_OFFSET             0x68
#define FIR_MSEC_TX_NOSAERROR_DIR_OFFSET           0x6C


/* MACsec SC Stat Counters (Direct) Registers */
#define FIR_MSEC_RX_SC_OK_DIR_OFFSET               0x0
#define FIR_MSEC_RX_SC_UNCHK_DIR_OFFSET            0x4
#define FIR_MSEC_RX_SC_DLYED_DIR_OFFSET            0x8
#define FIR_MSEC_RX_SC_LATE_DIR_OFFSET             0xC
#define FIR_MSEC_RX_SC_INVLD_DIR_OFFSET            0x10
#define FIR_MSEC_RX_SC_NOVLD_DIR_OFFSET            0x14
#define FIR_MSEC_TX_SC_ENC_DIR_OFFSET              0x40
#define FIR_MSEC_TX_SC_PRO_DIR_OFFSET              0x44


/* MACsec SAK Registers */
#define FIR_MSEC_RX_SAK_OP_OFFSET                  0x0
#define FIR_MSEC_RX_SAK_31_28_OFFSET               0x4
#define FIR_MSEC_RX_SAK_27_24_OFFSET               0x8
#define FIR_MSEC_RX_SAK_23_20_OFFSET               0xC
#define FIR_MSEC_RX_SAK_19_16_OFFSET               0x10
#define FIR_MSEC_RX_SAK_15_12_OFFSET               0x14
#define FIR_MSEC_RX_SAK_11_8_OFFSET                0x18
#define FIR_MSEC_RX_SAK_7_4_OFFSET                 0x1C
#define FIR_MSEC_RX_SAK_3_0_OFFSET                 0x20


/* MACsec Engineering Registers */
#define FIR_MSEC_MEM_CTL_0_OFFSET                  0x0
#define FIR_MSEC_MEM_CTL_1_OFFSET                  0x4

#ifdef __cplusplus
}
#endif
#endif /* __Fir__msdDrvSwRegs_H */
