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
#ifndef Spruce_msdDrvSwRegs_H
#define Spruce_msdDrvSwRegs_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*																			*/
/* Block: Switch Registers													*/
/*																			*/
/****************************************************************************/

/* Define Switch SMI Device Address */
#define SPRUCE_PORT_START_ADDR	    0x0U	    /* Start Port Address */
#define SPRUCE_GLOBAL1_DEV_ADDR	0x1BU	/* Global 1 Device Address */
#define SPRUCE_GLOBAL2_DEV_ADDR	0x1CU	/* Global 2 Device Address */
#define SPRUCE_TCAM_DEV_ADDR		0x1FU	/* TCAM Device Address */

/* Switch Per Port Registers */
#define SPRUCE_PORT_STATUS          0x0U	    /* Port Status Register */
#define SPRUCE_PHY_CONTROL          0x1U		/* Physical Control Register */
#define SPRUCE_LIMIT_PAUSE_CONTROL  0x2U		/* Flow Control Register */
#define SPRUCE_SWITCH_ID            0x3U		/* Switch Identifier Register */
#define SPRUCE_PORT_CONTROL         0x4U		/* Port Control Register */
#define SPRUCE_PORT_CONTROL1        0x5U		/* Port Control 1 Register */
#define SPRUCE_PORT_VLAN_MAP        0x6U		/* Port based VLAN Map Register */
#define SPRUCE_PVID                 0x7U		/* Port VLAN ID & Priority Register */
#define SPRUCE_PORT_CONTROL2        0x8U		/* Port Control 2 Register */
#define SPRUCE_EGRESS_RATE_CTRL     0x9U		/* Egress Rate Control Register */
#define SPRUCE_EGRESS_RATE_CTRL2    0xAU		/* Egress Rate Control 2 Register */
#define SPRUCE_PAV                  0xBU		/* Port Association Vector Register */
#define SPRUCE_PORT_ATU_CONTROL     0xCU		/* Port ATU Control Register */
#define SPRUCE_PRI_OVERRIDE         0xDU		/* Override Register */
#define SPRUCE_POLICY_CONTROL       0xEU		/* Policy & MGMT Control Register */
#define SPRUCE_EXT_PORT_CTRL_CMD	 0x10U	    /*Extended Port Control Operation Register*/
#define SPRUCE_EXT_PORT_CTRL_DATA   0x11U	    /*Extended Port Control Data Register*/
#define SPRUCE_PREEMPTION_CONTROL   0x15U		/* Preemption Control Register */
#define SPRUCE_LED_CONTROL          0x16U		/* LED Control Register */
#define SPRUCE_IP_PRI_MAPPING_TBL   0x17U		/* IP Priority Mapping Table Register */
#define SPRUCE_IEEE_PRI_MAPPING_TBL 0x18U		/* IEEE Priority Mapping Table Register */
#define SPRUCE_PORT_CONTROL3		0x19U		/* Port Control 3 Register */
#define SPRUCE_Q_COUNTER           0x1BU		/* Queue Counter Register */
#define SPRUCE_Q_CONTROL           0x1CU		/* Port Queue Control Register */
#define SPRUCE_CT_CONTROL          0x1EU		/* Cut Through Control Register */
#define SPRUCE_DBG_COUNTER         0x1FU		/* Debug Counters Register */

/* Switch Global Registers */
#define SPRUCE_GLOBAL_STATUS       0x0U		/* Global Status Register */
#define SPRUCE_ATU_FID_REG        	0x1U		/* ATU FID Register */
#define SPRUCE_VTU_FID_REG        	0x2U		/* VTU FID Register */
#define SPRUCE_STU_SID_REG        	0x3U		/* VTU SID Register */
#define SPRUCE_GLOBAL_CONTROL      0x4U		/* Global Control Register */
#define SPRUCE_VTU_OPERATION      	0x5U		/* VTU Operation Register */
#define SPRUCE_VTU_VID_REG        	0x6U		/* VTU VID Register */
#define SPRUCE_VTU_DATA1_REG      	0x7U		/* VTU/STU Data Register Ports 0 to 7 */
#define SPRUCE_VTU_DATA2_REG      	0x8U		/* VTU/STU Data Register Ports 8 to 10 and Priority */
#define SPRUCE_ATU_CTRL_REG        0xAU		/* ATU Control Register */
#define SPRUCE_ATU_OPERATION      	0xBU		/* ATU Operation Register */
#define SPRUCE_ATU_DATA_REG       	0xCU		/* ATU Data Register */
#define SPRUCE_ATU_MAC_BASE       	0xDU		/* ATU MAC Address Register Base */
#define SPRUCE_MONITOR_MGMT_CTRL    0x1AU    /* Monitor & MGMT Control Register */
#define SPRUCE_TOTAL_FREE_COUNTER   0x1BU    /* Total Free Counter Register */
#define SPRUCE_GLOBAL_CONTROL2      0x1CU    /* Global Control 2 Register */
#define SPRUCE_STATS_OPERATION    	0x1DU    /* Stats Operation Register */
#define SPRUCE_STATS_COUNTER3_2   	0x1EU    /* Stats Counter Register Bytes 3 & 2 */
#define SPRUCE_STATS_COUNTER1_0   	0x1FU    /* Stats Counter Register Bytes 1 & 0 */

/*Multi Chip Directly access Registers*/
#define SPRUCE_TOP_LEVEL_INTERRUPT         0x4U
#define SPRUCE_ATU_FID_REG_MULTICHIP     	0x9U
#define SPRUCE_IGR_RATE_COMMAND_MULTICHIP  0x14U
#define SPRUCE_IGR_RATE_DATA_MULTICHIP   	0x15U

/* Switch Global 2 Registers */
#define SPRUCE_INT_SOURCE     		0x0U		/* Interrupt Source Register */
#define SPRUCE_INT_MASK       		0x1U		/* Interrupt Mask Register */
#define SPRUCE_FLOWCTRL_DELAY    	0x4U		/* Flow Control Delay Register */
#define SPRUCE_MANAGEMENT        	0x5U		/* Switch Management Register */
#define SPRUCE_ROUTING_TBL       	0x6U		/* Device Mapping Table Register */
#define SPRUCE_TRUNK_MASK_TBL    	0x7U		/* Trunk Mask Table Register */
#define SPRUCE_TRUNK_ROUTING     	0x8U		/* Trunk Mapping Register */	
#define SPRUCE_IGR_RATE_COMMAND    0x9U		/* Ingress Rate Command Register */	
#define SPRUCE_IGR_RATE_DATA   	0xAU		/* Ingress Rate Data Register */	
#define SPRUCE_PVT_ADDR            0xBU		/* Cross Chip Port VLAN Addr Register */	
#define SPRUCE_PVT_DATA            0xCU		/* Cross Chip Port VLAN Data Register */	
#define SPRUCE_SWITCH_MAC          0xDU		/* Switch MAC/WoL/WoF Register */	
#define SPRUCE_ATU_STATS           0xEU		/* ATU Stats Register */	
#define SPRUCE_PRIORITY_OVERRIDE 	0xFU		/* Priority Override Table Register */	
#define SPRUCE_ENERGE_MNT        	0x12U	/* Energy Management Register */
#define SPRUCE_IMP_COMM_DBG		0x13U	/* IMP Comm/Debug Register */
#define SPRUCE_EEPROM_COMMAND    	0x14U	/* EEPROM Command Register */	
#define SPRUCE_EEPROM_DATA      	0x15U	/* EEPROM Address Register */	
#define SPRUCE_AVB_COMMAND     	0x16U	/* AVB Command Register */	
#define SPRUCE_AVB_DATA          	0x17U	/* AVB Data Register */
#define SPRUCE_SMI_PHY_CMD       	0x18U	/* SMI PHY Command Register */	
#define SPRUCE_SMI_PHY_DATA      	0x19U	/* SMI PHY Data Register */	
#define SPRUCE_SCRATCH_MISC      	0x1AU	/* Scratch & Misc Register */	
#define SPRUCE_WD_CONTROL        	0x1BU	/* Watch Dog Control Register */	
#define SPRUCE_QOS_WEIGHT        	0x1CU	/* QoS Weights Register */	
#define SPRUCE_MISC				0x1DU	/* Misc Register */	
#define SPRUCE_CUT_THROU_CTRL    	0x1FU	/* Cut Through Control Register */	

/* TCAM Registers - Page 0 */
#define SPRUCE_TCAM_OPERATION         0x0U		/* TCAM Operation Register */
#define SPRUCE_TCAM_P0_KEYS_1         0x2U		/* TCAM Key Register 1 */
#define SPRUCE_TCAM_P0_KEYS_2         0x3U		/* TCAM Key Register 2 */
#define SPRUCE_TCAM_P0_KEYS_3         0x4U		/* TCAM Key Register 3 */
#define SPRUCE_TCAM_P0_KEYS_4         0x5U		/* TCAM Key Register 4 */
#define SPRUCE_TCAM_P0_MATCH_DATA_1   0x6U		/* TCAM Match Data Register 1 */
#define SPRUCE_TCAM_P0_MATCH_DATA_2   0x7U		/* TCAM Match Data Register 2 */
#define SPRUCE_TCAM_P0_MATCH_DATA_3   0x8U
#define SPRUCE_TCAM_P0_MATCH_DATA_4   0x9U
#define SPRUCE_TCAM_P0_MATCH_DATA_5   0xaU
#define SPRUCE_TCAM_P0_MATCH_DATA_6   0xbU
#define SPRUCE_TCAM_P0_MATCH_DATA_7   0xcU
#define SPRUCE_TCAM_P0_MATCH_DATA_8   0xdU
#define SPRUCE_TCAM_P0_MATCH_DATA_9   0xeU
#define SPRUCE_TCAM_P0_MATCH_DATA_10  0xfU
#define SPRUCE_TCAM_P0_MATCH_DATA_11  0x10U
#define SPRUCE_TCAM_P0_MATCH_DATA_12  0x11U
#define SPRUCE_TCAM_P0_MATCH_DATA_13  0x12U
#define SPRUCE_TCAM_P0_MATCH_DATA_14  0x13U
#define SPRUCE_TCAM_P0_MATCH_DATA_15  0x14U
#define SPRUCE_TCAM_P0_MATCH_DATA_16  0x15U
#define SPRUCE_TCAM_P0_MATCH_DATA_17  0x16U
#define SPRUCE_TCAM_P0_MATCH_DATA_18  0x17U
#define SPRUCE_TCAM_P0_MATCH_DATA_19  0x18U
#define SPRUCE_TCAM_P0_MATCH_DATA_20  0x19U
#define SPRUCE_TCAM_P0_MATCH_DATA_21  0x1aU
#define SPRUCE_TCAM_P0_MATCH_DATA_22  0x1bU

/* TCAM Registers - Page 1 */
#define SPRUCE_TCAM_P1_MATCH_DATA_23   0x2U		/* TCAM Match Data Register 23 */
#define SPRUCE_TCAM_P1_MATCH_DATA_24   0x3U		/* TCAM Match Data Register 24 */
#define SPRUCE_TCAM_P1_MATCH_DATA_25   0x4U
#define SPRUCE_TCAM_P1_MATCH_DATA_26   0x5U
#define SPRUCE_TCAM_P1_MATCH_DATA_27   0x6U
#define SPRUCE_TCAM_P1_MATCH_DATA_28   0x7U
#define SPRUCE_TCAM_P1_MATCH_DATA_29   0x8U
#define SPRUCE_TCAM_P1_MATCH_DATA_30   0x9U
#define SPRUCE_TCAM_P1_MATCH_DATA_31   0xaU
#define SPRUCE_TCAM_P1_MATCH_DATA_32   0xbU
#define SPRUCE_TCAM_P1_MATCH_DATA_33   0xcU
#define SPRUCE_TCAM_P1_MATCH_DATA_34   0xdU
#define SPRUCE_TCAM_P1_MATCH_DATA_35   0xeU
#define SPRUCE_TCAM_P1_MATCH_DATA_36   0xfU
#define SPRUCE_TCAM_P1_MATCH_DATA_37   0x10U
#define SPRUCE_TCAM_P1_MATCH_DATA_38   0x11U
#define SPRUCE_TCAM_P1_MATCH_DATA_39   0x12U
#define SPRUCE_TCAM_P1_MATCH_DATA_40   0x13U
#define SPRUCE_TCAM_P1_MATCH_DATA_41   0x14U
#define SPRUCE_TCAM_P1_MATCH_DATA_42   0x15U
#define SPRUCE_TCAM_P1_MATCH_DATA_43   0x16U
#define SPRUCE_TCAM_P1_MATCH_DATA_44   0x17U
#define SPRUCE_TCAM_P1_MATCH_DATA_45   0x18U
#define SPRUCE_TCAM_P1_MATCH_DATA_46   0x19U
#define SPRUCE_TCAM_P1_MATCH_DATA_47   0x1aU
#define SPRUCE_TCAM_P1_MATCH_DATA_48   0x1bU

/* TCAM Registers - Page 2 */
#define SPRUCE_TCAM_P2_ACTION_1        0x2U	    /* TCAM Action 1 Register */
#define SPRUCE_TCAM_P2_ACTION_2        0x3U		/* TCAM Action 2 Register */
#define SPRUCE_TCAM_P2_ACTION_3        0x4U		/* TCAM Action 3 Register */
#define SPRUCE_TCAM_P2_ACTION_5        0x6U		/* TCAM Action 5 Register */
#define SPRUCE_TCAM_P2_ACTION_6        0x7U		/* TCAM Action 6 Register */
#define SPRUCE_TCAM_P2_ACTION_7        0x8U		/* TCAM Action 7 Register */
#define SPRUCE_TCAM_P2_DEBUG_PORT      0x1CU		/* TCAM Debug Register */
#define SPRUCE_TCAM_P2_ALL_HIT         0x1FU		/* TCAM Debug Register 2 */

/* TCAM Registers - Page 3 */
#define SPRUCE_TCAM_EGR_PORT           0x1U		/* Egress TCAM Port Register */
#define SPRUCE_TCAM_EGR_ACTION_1       0x2U		/* Egress TCAM Action 1 Register */
#define SPRUCE_TCAM_EGR_ACTION_2       0x3U		/* Egress TCAM Action 2 Register */
#define SPRUCE_TCAM_EGR_ACTION_3       0x4U		/* Egress TCAM Action 3 Register */


/****************************************************************************/
/*																			*/
/* Block: INTERNAL PHY Registers											*/
/*																			*/
/****************************************************************************/

/* Internal Copper PHY registers*/
#define SPRUCE_PHY_CONTROL_REG          0U
#define SPRUCE_PHY_AUTONEGO_AD_REG      4U
#define SPRUCE_PHY_AUTONEGO_1000AD_REG  9U
#define SPRUCE_PHY_SPEC_CONTROL_REG		16U
#define SPRUCE_PHY_PAGE_ANY_REG			22U

/* Definitions for VCT registers */
#define SPRUCE_MDI0_VCT_STATUS     16U
#define SPRUCE_MDI1_VCT_STATUS     17U
#define SPRUCE_MDI2_VCT_STATUS     18U
#define SPRUCE_MDI3_VCT_STATUS     19U
#define SPRUCE_ADV_VCT_CONTROL_5   23U
#define SPRUCE_ADV_VCT_SMPL_DIST   24U
#define SPRUCE_ADV_VCT_CONTROL_8   20U
#define SPRUCE_PAIR_SKEW_STATUS    20U
#define SPRUCE_PAIR_SWAP_STATUS    21U

/* Bit Definition for QD_PHY_CONTROL_REG */
#define SPRUCE_PHY_RESET            0x8000U
#define SPRUCE_PHY_LOOPBACK         0x4000U
#define SPRUCE_PHY_SPEED            0x2000U
#define SPRUCE_PHY_AUTONEGO         0x1000U
#define SPRUCE_PHY_POWER            0x800U
#define SPRUCE_PHY_ISOLATE          0x400U
#define SPRUCE_PHY_RESTART_AUTONEGO 0x200U
#define SPRUCE_PHY_DUPLEX           0x100U
#define SPRUCE_PHY_SPEED_MSB        0x40U

/* Bit Definition for QD_PHY_AUTONEGO_AD_REG */
#define SPRUCE_PHY_NEXTPAGE           0x8000U
#define SPRUCE_PHY_REMOTEFAULT        0x4000U
#define SPRUCE_PHY_PAUSE              0x400U
#define SPRUCE_PHY_100_FULL           0x100U
#define SPRUCE_PHY_100_HALF           0x80U
#define SPRUCE_PHY_10_FULL            0x40U
#define SPRUCE_PHY_10_HALF            0x20U

#define SPRUCE_PHY_MODE_AUTO_AUTO    (SPRUCE_PHY_100_FULL | SPRUCE_PHY_100_HALF | SPRUCE_PHY_10_FULL | SPRUCE_PHY_10_HALF)
#define SPRUCE_PHY_MODE_100_AUTO     (SPRUCE_PHY_100_FULL | SPRUCE_PHY_100_HALF)
#define SPRUCE_PHY_MODE_10_AUTO      (SPRUCE_PHY_10_FULL  | SPRUCE_PHY_10_HALF)
#define SPRUCE_PHY_MODE_AUTO_FULL    (SPRUCE_PHY_100_FULL | SPRUCE_PHY_10_FULL)
#define SPRUCE_PHY_MODE_AUTO_HALF    (SPRUCE_PHY_100_HALF | SPRUCE_PHY_10_HALF)

#define SPRUCE_PHY_MODE_100_FULL    (SPRUCE_PHY_100_FULL)
#define SPRUCE_PHY_MODE_100_HALF    (SPRUCE_PHY_100_HALF)
#define SPRUCE_PHY_MODE_10_FULL     (SPRUCE_PHY_10_FULL)   
#define SPRUCE_PHY_MODE_10_HALF     (SPRUCE_PHY_10_HALF)   

/* Gigabit Phy related definition */
#define SPRUCE_GIGPHY_1000X_FULL_CAP    0x8U
#define SPRUCE_GIGPHY_1000X_HALF_CAP    0x4U
#define SPRUCE_GIGPHY_1000T_FULL_CAP    0x2U
#define SPRUCE_GIGPHY_1000T_HALF_CAP    0x1U

#define SPRUCE_GIGPHY_1000X_CAP        (SPRUCE_GIGPHY_1000X_FULL_CAP|SPRUCE_GIGPHY_1000X_HALF_CAP)
#define SPRUCE_GIGPHY_1000T_CAP        (SPRUCE_GIGPHY_1000T_FULL_CAP|SPRUCE_GIGPHY_1000T_HALF_CAP)

#define SPRUCE_GIGPHY_1000X_FULL        0x20U
#define SPRUCE_GIGPHY_1000X_HALF        0x40U

#define SPRUCE_GIGPHY_1000T_FULL        0x200U
#define SPRUCE_GIGPHY_1000T_HALF        0x100U

#ifdef __cplusplus
}
#endif
#endif /* __Spruce_msdDrvSwRegs_H */
