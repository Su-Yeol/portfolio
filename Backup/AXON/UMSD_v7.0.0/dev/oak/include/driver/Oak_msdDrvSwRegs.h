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
#ifndef Oak_msdDrvSwRegs_H
#define Oak_msdDrvSwRegs_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*																			*/
/* Block: Switch Registers													*/
/*																			*/
/****************************************************************************/

/* Define Switch SMI Device Address */
#define OAK_PORT_START_ADDR	    0x0U	    /* Start Port Address */
#define OAK_GLOBAL1_DEV_ADDR	0x1BU	/* Global 1 Device Address */
#define OAK_GLOBAL2_DEV_ADDR	0x1CU	/* Global 2 Device Address */
#define OAK_TCAM_DEV_ADDR		0x1FU	/* TCAM Device Address */

/* Switch Per Port Registers */
#define OAK_PORT_STATUS          0x0U	    /* Port Status Register */
#define OAK_PHY_CONTROL          0x1U		/* Physical Control Register */
#define OAK_LIMIT_PAUSE_CONTROL  0x2U		/* Flow Control Register */
#define OAK_SWITCH_ID            0x3U		/* Switch Identifier Register */
#define OAK_PORT_CONTROL         0x4U		/* Port Control Register */
#define OAK_PORT_CONTROL1        0x5U		/* Port Control 1 Register */
#define OAK_PORT_VLAN_MAP        0x6U		/* Port based VLAN Map Register */
#define OAK_PVID                 0x7U		/* Port VLAN ID & Priority Register */
#define OAK_PORT_CONTROL2        0x8U		/* Port Control 2 Register */
#define OAK_EGRESS_RATE_CTRL     0x9U		/* Egress Rate Control Register */
#define OAK_EGRESS_RATE_CTRL2    0xAU		/* Egress Rate Control 2 Register */
#define OAK_PAV                  0xBU		/* Port Association Vector Register */
#define OAK_PORT_ATU_CONTROL     0xCU		/* Port ATU Control Register */
#define OAK_PRI_OVERRIDE         0xDU		/* Override Register */
#define OAK_POLICY_CONTROL       0xEU		/* Policy & MGMT Control Register */
#define OAK_EXT_PORT_CTRL_CMD	 0x10U	    /*Extended Port Control Operation Register*/
#define OAK_EXT_PORT_CTRL_DATA   0x11U	    /*Extended Port Control Data Register*/
#define OAK_PREEMPTION_CONTROL   0x15U		/* Preemption Control Register */
#define OAK_LED_CONTROL          0x16U		/* LED Control Register */
#define OAK_IP_PRI_MAPPING_TBL   0x17U		/* IP Priority Mapping Table Register */
#define OAK_IEEE_PRI_MAPPING_TBL 0x18U		/* IEEE Priority Mapping Table Register */
#define OAK_PORT_CONTROL3		0x19U		/* Port Control 3 Register */
#define OAK_Q_COUNTER           0x1BU		/* Queue Counter Register */
#define OAK_Q_CONTROL           0x1CU		/* Port Queue Control Register */
#define OAK_CT_CONTROL          0x1EU		/* Cut Through Control Register */
#define OAK_DBG_COUNTER         0x1FU		/* Debug Counters Register */

/* Switch Global Registers */
#define OAK_GLOBAL_STATUS       0x0U		/* Global Status Register */
#define OAK_ATU_FID_REG        	0x1U		/* ATU FID Register */
#define OAK_VTU_FID_REG        	0x2U		/* VTU FID Register */
#define OAK_STU_SID_REG        	0x3U		/* VTU SID Register */
#define OAK_GLOBAL_CONTROL      0x4U		/* Global Control Register */
#define OAK_VTU_OPERATION      	0x5U		/* VTU Operation Register */
#define OAK_VTU_VID_REG        	0x6U		/* VTU VID Register */
#define OAK_VTU_DATA1_REG      	0x7U		/* VTU/STU Data Register Ports 0 to 7 */
#define OAK_VTU_DATA2_REG      	0x8U		/* VTU/STU Data Register Ports 8 to 10 and Priority */
#define OAK_ATU_CTRL_REG        0xAU		/* ATU Control Register */
#define OAK_ATU_OPERATION      	0xBU		/* ATU Operation Register */
#define OAK_ATU_DATA_REG       	0xCU		/* ATU Data Register */
#define OAK_ATU_MAC_BASE       	0xDU		/* ATU MAC Address Register Base */
#define OAK_MONITOR_MGMT_CTRL    0x1AU    /* Monitor & MGMT Control Register */
#define OAK_TOTAL_FREE_COUNTER   0x1BU    /* Total Free Counter Register */
#define OAK_GLOBAL_CONTROL2      0x1CU    /* Global Control 2 Register */
#define OAK_STATS_OPERATION    	0x1DU    /* Stats Operation Register */
#define OAK_STATS_COUNTER3_2   	0x1EU    /* Stats Counter Register Bytes 3 & 2 */
#define OAK_STATS_COUNTER1_0   	0x1FU    /* Stats Counter Register Bytes 1 & 0 */

/*Multi Chip Directly access Registers*/
#define OAK_TOP_LEVEL_INTERRUPT         0x4U
#define OAK_ATU_FID_REG_MULTICHIP     	0x9U
#define OAK_IGR_RATE_COMMAND_MULTICHIP  0x14U
#define OAK_IGR_RATE_DATA_MULTICHIP   	0x15U

/* Switch Global 2 Registers */
#define OAK_INT_SOURCE     		0x0U		/* Interrupt Source Register */
#define OAK_INT_MASK       		0x1U		/* Interrupt Mask Register */
#define OAK_FLOWCTRL_DELAY    	0x4U		/* Flow Control Delay Register */
#define OAK_MANAGEMENT        	0x5U		/* Switch Management Register */
#define OAK_ROUTING_TBL       	0x6U		/* Device Mapping Table Register */
#define OAK_TRUNK_MASK_TBL    	0x7U		/* Trunk Mask Table Register */
#define OAK_TRUNK_ROUTING     	0x8U		/* Trunk Mapping Register */	
#define OAK_IGR_RATE_COMMAND    0x9U		/* Ingress Rate Command Register */	
#define OAK_IGR_RATE_DATA   	0xAU		/* Ingress Rate Data Register */	
#define OAK_PVT_ADDR            0xBU		/* Cross Chip Port VLAN Addr Register */	
#define OAK_PVT_DATA            0xCU		/* Cross Chip Port VLAN Data Register */	
#define OAK_SWITCH_MAC          0xDU		/* Switch MAC/WoL/WoF Register */	
#define OAK_ATU_STATS           0xEU		/* ATU Stats Register */	
#define OAK_PRIORITY_OVERRIDE 	0xFU		/* Priority Override Table Register */	
#define OAK_ENERGE_MNT        	0x12U	/* Energy Management Register */
#define OAK_IMP_COMM_DBG		0x13U	/* IMP Comm/Debug Register */
#define OAK_EEPROM_COMMAND    	0x14U	/* EEPROM Command Register */	
#define OAK_EEPROM_DATA      	0x15U	/* EEPROM Address Register */	
#define OAK_AVB_COMMAND     	0x16U	/* AVB Command Register */	
#define OAK_AVB_DATA          	0x17U	/* AVB Data Register */
#define OAK_SMI_PHY_CMD       	0x18U	/* SMI PHY Command Register */	
#define OAK_SMI_PHY_DATA      	0x19U	/* SMI PHY Data Register */	
#define OAK_SCRATCH_MISC      	0x1AU	/* Scratch & Misc Register */	
#define OAK_WD_CONTROL        	0x1BU	/* Watch Dog Control Register */	
#define OAK_QOS_WEIGHT        	0x1CU	/* QoS Weights Register */	
#define OAK_MISC				0x1DU	/* Misc Register */	
#define OAK_CUT_THROU_CTRL    	0x1FU	/* Cut Through Control Register */	

/* TCAM Registers - Page 0 */
#define OAK_TCAM_OPERATION         0x0U		/* TCAM Operation Register */
#define OAK_TCAM_P0_KEYS_1         0x2U		/* TCAM Key Register 1 */
#define OAK_TCAM_P0_KEYS_2         0x3U		/* TCAM Key Register 2 */
#define OAK_TCAM_P0_KEYS_3         0x4U		/* TCAM Key Register 3 */
#define OAK_TCAM_P0_KEYS_4         0x5U		/* TCAM Key Register 4 */
#define OAK_TCAM_P0_MATCH_DATA_1   0x6U		/* TCAM Match Data Register 1 */
#define OAK_TCAM_P0_MATCH_DATA_2   0x7U		/* TCAM Match Data Register 2 */
#define OAK_TCAM_P0_MATCH_DATA_3   0x8U
#define OAK_TCAM_P0_MATCH_DATA_4   0x9U
#define OAK_TCAM_P0_MATCH_DATA_5   0xaU
#define OAK_TCAM_P0_MATCH_DATA_6   0xbU
#define OAK_TCAM_P0_MATCH_DATA_7   0xcU
#define OAK_TCAM_P0_MATCH_DATA_8   0xdU
#define OAK_TCAM_P0_MATCH_DATA_9   0xeU
#define OAK_TCAM_P0_MATCH_DATA_10  0xfU
#define OAK_TCAM_P0_MATCH_DATA_11  0x10U
#define OAK_TCAM_P0_MATCH_DATA_12  0x11U
#define OAK_TCAM_P0_MATCH_DATA_13  0x12U
#define OAK_TCAM_P0_MATCH_DATA_14  0x13U
#define OAK_TCAM_P0_MATCH_DATA_15  0x14U
#define OAK_TCAM_P0_MATCH_DATA_16  0x15U
#define OAK_TCAM_P0_MATCH_DATA_17  0x16U
#define OAK_TCAM_P0_MATCH_DATA_18  0x17U
#define OAK_TCAM_P0_MATCH_DATA_19  0x18U
#define OAK_TCAM_P0_MATCH_DATA_20  0x19U
#define OAK_TCAM_P0_MATCH_DATA_21  0x1aU
#define OAK_TCAM_P0_MATCH_DATA_22  0x1bU

/* TCAM Registers - Page 1 */
#define OAK_TCAM_P1_MATCH_DATA_23   0x2U		/* TCAM Match Data Register 23 */
#define OAK_TCAM_P1_MATCH_DATA_24   0x3U		/* TCAM Match Data Register 24 */
#define OAK_TCAM_P1_MATCH_DATA_25   0x4U
#define OAK_TCAM_P1_MATCH_DATA_26   0x5U
#define OAK_TCAM_P1_MATCH_DATA_27   0x6U
#define OAK_TCAM_P1_MATCH_DATA_28   0x7U
#define OAK_TCAM_P1_MATCH_DATA_29   0x8U
#define OAK_TCAM_P1_MATCH_DATA_30   0x9U
#define OAK_TCAM_P1_MATCH_DATA_31   0xaU
#define OAK_TCAM_P1_MATCH_DATA_32   0xbU
#define OAK_TCAM_P1_MATCH_DATA_33   0xcU
#define OAK_TCAM_P1_MATCH_DATA_34   0xdU
#define OAK_TCAM_P1_MATCH_DATA_35   0xeU
#define OAK_TCAM_P1_MATCH_DATA_36   0xfU
#define OAK_TCAM_P1_MATCH_DATA_37   0x10U
#define OAK_TCAM_P1_MATCH_DATA_38   0x11U
#define OAK_TCAM_P1_MATCH_DATA_39   0x12U
#define OAK_TCAM_P1_MATCH_DATA_40   0x13U
#define OAK_TCAM_P1_MATCH_DATA_41   0x14U
#define OAK_TCAM_P1_MATCH_DATA_42   0x15U
#define OAK_TCAM_P1_MATCH_DATA_43   0x16U
#define OAK_TCAM_P1_MATCH_DATA_44   0x17U
#define OAK_TCAM_P1_MATCH_DATA_45   0x18U
#define OAK_TCAM_P1_MATCH_DATA_46   0x19U
#define OAK_TCAM_P1_MATCH_DATA_47   0x1aU
#define OAK_TCAM_P1_MATCH_DATA_48   0x1bU

/* TCAM Registers - Page 2 */
#define OAK_TCAM_P2_ACTION_1        0x2U	    /* TCAM Action 1 Register */
#define OAK_TCAM_P2_ACTION_2        0x3U		/* TCAM Action 2 Register */
#define OAK_TCAM_P2_ACTION_3        0x4U		/* TCAM Action 3 Register */
#define OAK_TCAM_P2_ACTION_5        0x6U		/* TCAM Action 5 Register */
#define OAK_TCAM_P2_ACTION_6        0x7U		/* TCAM Action 6 Register */
#define OAK_TCAM_P2_ACTION_7        0x8U		/* TCAM Action 7 Register */
#define OAK_TCAM_P2_DEBUG_PORT      0x1CU		/* TCAM Debug Register */
#define OAK_TCAM_P2_ALL_HIT         0x1FU		/* TCAM Debug Register 2 */

/* TCAM Registers - Page 3 */
#define OAK_TCAM_EGR_PORT           0x1U		/* Egress TCAM Port Register */
#define OAK_TCAM_EGR_ACTION_1       0x2U		/* Egress TCAM Action 1 Register */
#define OAK_TCAM_EGR_ACTION_2       0x3U		/* Egress TCAM Action 2 Register */
#define OAK_TCAM_EGR_ACTION_3       0x4U		/* Egress TCAM Action 3 Register */


/****************************************************************************/
/*																			*/
/* Block: INTERNAL PHY Registers											*/
/*																			*/
/****************************************************************************/

/* Internal Copper PHY registers*/
#define OAK_PHY_CONTROL_REG          0U
#define OAK_PHY_AUTONEGO_AD_REG      4U
#define OAK_PHY_AUTONEGO_1000AD_REG  9U
#define OAK_PHY_SPEC_CONTROL_REG		16U
#define OAK_PHY_PAGE_ANY_REG			22U

/* Definitions for VCT registers */
#define OAK_MDI0_VCT_STATUS     16U
#define OAK_MDI1_VCT_STATUS     17U
#define OAK_MDI2_VCT_STATUS     18U
#define OAK_MDI3_VCT_STATUS     19U
#define OAK_ADV_VCT_CONTROL_5   23U
#define OAK_ADV_VCT_SMPL_DIST   24U
#define OAK_ADV_VCT_CONTROL_8   20U
#define OAK_PAIR_SKEW_STATUS    20U
#define OAK_PAIR_SWAP_STATUS    21U

/* Bit Definition for QD_PHY_CONTROL_REG */
#define OAK_PHY_RESET            0x8000U
#define OAK_PHY_LOOPBACK         0x4000U
#define OAK_PHY_SPEED            0x2000U
#define OAK_PHY_AUTONEGO         0x1000U
#define OAK_PHY_POWER            0x800U
#define OAK_PHY_ISOLATE          0x400U
#define OAK_PHY_RESTART_AUTONEGO 0x200U
#define OAK_PHY_DUPLEX           0x100U
#define OAK_PHY_SPEED_MSB        0x40U

/* Bit Definition for QD_PHY_AUTONEGO_AD_REG */
#define OAK_PHY_NEXTPAGE           0x8000U
#define OAK_PHY_REMOTEFAULT        0x4000U
#define OAK_PHY_PAUSE              0x400U
#define OAK_PHY_100_FULL           0x100U
#define OAK_PHY_100_HALF           0x80U
#define OAK_PHY_10_FULL            0x40U
#define OAK_PHY_10_HALF            0x20U

#define OAK_PHY_MODE_AUTO_AUTO    (OAK_PHY_100_FULL | OAK_PHY_100_HALF | OAK_PHY_10_FULL | OAK_PHY_10_HALF)
#define OAK_PHY_MODE_100_AUTO     (OAK_PHY_100_FULL | OAK_PHY_100_HALF)
#define OAK_PHY_MODE_10_AUTO      (OAK_PHY_10_FULL  | OAK_PHY_10_HALF)
#define OAK_PHY_MODE_AUTO_FULL    (OAK_PHY_100_FULL | OAK_PHY_10_FULL)
#define OAK_PHY_MODE_AUTO_HALF    (OAK_PHY_100_HALF | OAK_PHY_10_HALF)

#define OAK_PHY_MODE_100_FULL    (OAK_PHY_100_FULL)
#define OAK_PHY_MODE_100_HALF    (OAK_PHY_100_HALF)
#define OAK_PHY_MODE_10_FULL     (OAK_PHY_10_FULL)   
#define OAK_PHY_MODE_10_HALF     (OAK_PHY_10_HALF)   

/* Gigabit Phy related definition */
#define OAK_GIGPHY_1000X_FULL_CAP    0x8U
#define OAK_GIGPHY_1000X_HALF_CAP    0x4U
#define OAK_GIGPHY_1000T_FULL_CAP    0x2U
#define OAK_GIGPHY_1000T_HALF_CAP    0x1U

#define OAK_GIGPHY_1000X_CAP        (OAK_GIGPHY_1000X_FULL_CAP|OAK_GIGPHY_1000X_HALF_CAP)
#define OAK_GIGPHY_1000T_CAP        (OAK_GIGPHY_1000T_FULL_CAP|OAK_GIGPHY_1000T_HALF_CAP)

#define OAK_GIGPHY_1000X_FULL        0x20U
#define OAK_GIGPHY_1000X_HALF        0x40U

#define OAK_GIGPHY_1000T_FULL        0x200U
#define OAK_GIGPHY_1000T_HALF        0x100U

#ifdef __cplusplus
}
#endif
#endif /* __Oak_msdDrvSwRegs_H */
