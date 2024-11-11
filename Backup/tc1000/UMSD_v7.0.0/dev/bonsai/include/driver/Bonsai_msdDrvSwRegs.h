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
#ifndef Bonsai_msdDrvSwRegs_H
#define Bonsai_msdDrvSwRegs_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/*																			*/
/* Block: Switch Registers													*/
/*																			*/
/****************************************************************************/

/* Define Switch SMI Device Address */
#define BONSAI_PORT_START_ADDR	    0x0U	    /* Start Port Address */
#define BONSAI_GLOBAL1_DEV_ADDR	0x1BU	/* Global 1 Device Address */
#define BONSAI_GLOBAL2_DEV_ADDR	0x1CU	/* Global 2 Device Address */
#define BONSAI_TCAM_DEV_ADDR		0x1FU	/* TCAM Device Address */

/* Switch Per Port Registers */
#define BONSAI_PORT_STATUS          0x0U	    /* Port Status Register */
#define BONSAI_PHY_CONTROL          0x1U		/* Physical Control Register */
#define BONSAI_LIMIT_PAUSE_CONTROL  0x2U		/* Flow Control Register */
#define BONSAI_SWITCH_ID            0x3U		/* Switch Identifier Register */
#define BONSAI_PORT_CONTROL         0x4U		/* Port Control Register */
#define BONSAI_PORT_CONTROL1        0x5U		/* Port Control 1 Register */
#define BONSAI_PORT_VLAN_MAP        0x6U		/* Port based VLAN Map Register */
#define BONSAI_PVID                 0x7U		/* Port VLAN ID & Priority Register */
#define BONSAI_PORT_CONTROL2        0x8U		/* Port Control 2 Register */
#define BONSAI_EGRESS_RATE_CTRL     0x9U		/* Egress Rate Control Register */
#define BONSAI_EGRESS_RATE_CTRL2    0xAU		/* Egress Rate Control 2 Register */
#define BONSAI_PAV                  0xBU		/* Port Association Vector Register */
#define BONSAI_PORT_ATU_CONTROL     0xCU		/* Port ATU Control Register */
#define BONSAI_PRI_OVERRIDE         0xDU		/* Override Register */
#define BONSAI_POLICY_CONTROL       0xEU		/* Policy & MGMT Control Register */
#define BONSAI_EXT_PORT_CTRL_CMD	 0x10U	    /*Extended Port Control Operation Register*/
#define BONSAI_EXT_PORT_CTRL_DATA   0x11U	    /*Extended Port Control Data Register*/
#define BONSAI_PREEMPTION_CONTROL   0x15U		/* Preemption Control Register */
#define BONSAI_LED_CONTROL          0x16U		/* LED Control Register */
#define BONSAI_IP_PRI_MAPPING_TBL   0x17U		/* IP Priority Mapping Table Register */
#define BONSAI_IEEE_PRI_MAPPING_TBL 0x18U		/* IEEE Priority Mapping Table Register */
#define BONSAI_PORT_CONTROL3		0x19U		/* Port Control 3 Register */
#define BONSAI_Q_COUNTER           0x1BU		/* Queue Counter Register */
#define BONSAI_Q_CONTROL           0x1CU		/* Port Queue Control Register */
#define BONSAI_CT_CONTROL          0x1EU		/* Cut Through Control Register */
#define BONSAI_DBG_COUNTER         0x1FU		/* Debug Counters Register */

/* Switch Global Registers */
#define BONSAI_GLOBAL_STATUS       0x0U		/* Global Status Register */
#define BONSAI_ATU_FID_REG        	0x1U		/* ATU FID Register */
#define BONSAI_VTU_FID_REG        	0x2U		/* VTU FID Register */
#define BONSAI_STU_SID_REG        	0x3U		/* VTU SID Register */
#define BONSAI_GLOBAL_CONTROL      0x4U		/* Global Control Register */
#define BONSAI_VTU_OPERATION      	0x5U		/* VTU Operation Register */
#define BONSAI_VTU_VID_REG        	0x6U		/* VTU VID Register */
#define BONSAI_VTU_DATA1_REG      	0x7U		/* VTU/STU Data Register Ports 0 to 7 */
#define BONSAI_VTU_DATA2_REG      	0x8U		/* VTU/STU Data Register Ports 8 to 10 and Priority */
#define BONSAI_ATU_CTRL_REG        0xAU		/* ATU Control Register */
#define BONSAI_ATU_OPERATION      	0xBU		/* ATU Operation Register */
#define BONSAI_ATU_DATA_REG       	0xCU		/* ATU Data Register */
#define BONSAI_ATU_MAC_BASE       	0xDU		/* ATU MAC Address Register Base */
#define BONSAI_MONITOR_MGMT_CTRL    0x1AU    /* Monitor & MGMT Control Register */
#define BONSAI_TOTAL_FREE_COUNTER   0x1BU    /* Total Free Counter Register */
#define BONSAI_GLOBAL_CONTROL2      0x1CU    /* Global Control 2 Register */
#define BONSAI_STATS_OPERATION    	0x1DU    /* Stats Operation Register */
#define BONSAI_STATS_COUNTER3_2   	0x1EU    /* Stats Counter Register Bytes 3 & 2 */
#define BONSAI_STATS_COUNTER1_0   	0x1FU    /* Stats Counter Register Bytes 1 & 0 */

/*Multi Chip Directly access Registers*/
#define BONSAI_TOP_LEVEL_INTERRUPT         0x4U
#define BONSAI_ATU_FID_REG_MULTICHIP     	0x9U
#define BONSAI_IGR_RATE_COMMAND_MULTICHIP  0x14U
#define BONSAI_IGR_RATE_DATA_MULTICHIP   	0x15U

/* Switch Global 2 Registers */
#define BONSAI_INT_SOURCE     		0x0U		/* Interrupt Source Register */
#define BONSAI_INT_MASK       		0x1U		/* Interrupt Mask Register */
#define BONSAI_FLOWCTRL_DELAY    	0x4U		/* Flow Control Delay Register */
#define BONSAI_MANAGEMENT        	0x5U		/* Switch Management Register */
#define BONSAI_ROUTING_TBL       	0x6U		/* Device Mapping Table Register */
#define BONSAI_TRUNK_MASK_TBL    	0x7U		/* Trunk Mask Table Register */
#define BONSAI_TRUNK_ROUTING     	0x8U		/* Trunk Mapping Register */	
#define BONSAI_IGR_RATE_COMMAND    0x9U		/* Ingress Rate Command Register */	
#define BONSAI_IGR_RATE_DATA   	0xAU		/* Ingress Rate Data Register */	
#define BONSAI_PVT_ADDR            0xBU		/* Cross Chip Port VLAN Addr Register */	
#define BONSAI_PVT_DATA            0xCU		/* Cross Chip Port VLAN Data Register */	
#define BONSAI_SWITCH_MAC          0xDU		/* Switch MAC/WoL/WoF Register */	
#define BONSAI_ATU_STATS           0xEU		/* ATU Stats Register */	
#define BONSAI_PRIORITY_OVERRIDE 	0xFU		/* Priority Override Table Register */	
#define BONSAI_ENERGE_MNT        	0x12U	/* Energy Management Register */
#define BONSAI_IMP_COMM_DBG		0x13U	/* IMP Comm/Debug Register */
#define BONSAI_EEPROM_COMMAND    	0x14U	/* EEPROM Command Register */	
#define BONSAI_EEPROM_DATA      	0x15U	/* EEPROM Address Register */	
#define BONSAI_AVB_COMMAND     	0x16U	/* AVB Command Register */	
#define BONSAI_AVB_DATA          	0x17U	/* AVB Data Register */
#define BONSAI_SMI_PHY_CMD       	0x18U	/* SMI PHY Command Register */	
#define BONSAI_SMI_PHY_DATA      	0x19U	/* SMI PHY Data Register */	
#define BONSAI_SCRATCH_MISC      	0x1AU	/* Scratch & Misc Register */	
#define BONSAI_WD_CONTROL        	0x1BU	/* Watch Dog Control Register */	
#define BONSAI_QOS_WEIGHT        	0x1CU	/* QoS Weights Register */	
#define BONSAI_MISC				0x1DU	/* Misc Register */	
#define BONSAI_CUT_THROU_CTRL    	0x1FU	/* Cut Through Control Register */	

/* TCAM Registers - Page 0 */
#define BONSAI_TCAM_OPERATION         0x0U		/* TCAM Operation Register */
#define BONSAI_TCAM_P0_KEYS_1         0x2U		/* TCAM Key Register 1 */
#define BONSAI_TCAM_P0_KEYS_2         0x3U		/* TCAM Key Register 2 */
#define BONSAI_TCAM_P0_KEYS_3         0x4U		/* TCAM Key Register 3 */
#define BONSAI_TCAM_P0_KEYS_4         0x5U		/* TCAM Key Register 4 */
#define BONSAI_TCAM_P0_MATCH_DATA_1   0x6U		/* TCAM Match Data Register 1 */
#define BONSAI_TCAM_P0_MATCH_DATA_2   0x7U		/* TCAM Match Data Register 2 */
#define BONSAI_TCAM_P0_MATCH_DATA_3   0x8U
#define BONSAI_TCAM_P0_MATCH_DATA_4   0x9U
#define BONSAI_TCAM_P0_MATCH_DATA_5   0xaU
#define BONSAI_TCAM_P0_MATCH_DATA_6   0xbU
#define BONSAI_TCAM_P0_MATCH_DATA_7   0xcU
#define BONSAI_TCAM_P0_MATCH_DATA_8   0xdU
#define BONSAI_TCAM_P0_MATCH_DATA_9   0xeU
#define BONSAI_TCAM_P0_MATCH_DATA_10  0xfU
#define BONSAI_TCAM_P0_MATCH_DATA_11  0x10U
#define BONSAI_TCAM_P0_MATCH_DATA_12  0x11U
#define BONSAI_TCAM_P0_MATCH_DATA_13  0x12U
#define BONSAI_TCAM_P0_MATCH_DATA_14  0x13U
#define BONSAI_TCAM_P0_MATCH_DATA_15  0x14U
#define BONSAI_TCAM_P0_MATCH_DATA_16  0x15U
#define BONSAI_TCAM_P0_MATCH_DATA_17  0x16U
#define BONSAI_TCAM_P0_MATCH_DATA_18  0x17U
#define BONSAI_TCAM_P0_MATCH_DATA_19  0x18U
#define BONSAI_TCAM_P0_MATCH_DATA_20  0x19U
#define BONSAI_TCAM_P0_MATCH_DATA_21  0x1aU
#define BONSAI_TCAM_P0_MATCH_DATA_22  0x1bU

/* TCAM Registers - Page 1 */
#define BONSAI_TCAM_P1_MATCH_DATA_23   0x2U		/* TCAM Match Data Register 23 */
#define BONSAI_TCAM_P1_MATCH_DATA_24   0x3U		/* TCAM Match Data Register 24 */
#define BONSAI_TCAM_P1_MATCH_DATA_25   0x4U
#define BONSAI_TCAM_P1_MATCH_DATA_26   0x5U
#define BONSAI_TCAM_P1_MATCH_DATA_27   0x6U
#define BONSAI_TCAM_P1_MATCH_DATA_28   0x7U
#define BONSAI_TCAM_P1_MATCH_DATA_29   0x8U
#define BONSAI_TCAM_P1_MATCH_DATA_30   0x9U
#define BONSAI_TCAM_P1_MATCH_DATA_31   0xaU
#define BONSAI_TCAM_P1_MATCH_DATA_32   0xbU
#define BONSAI_TCAM_P1_MATCH_DATA_33   0xcU
#define BONSAI_TCAM_P1_MATCH_DATA_34   0xdU
#define BONSAI_TCAM_P1_MATCH_DATA_35   0xeU
#define BONSAI_TCAM_P1_MATCH_DATA_36   0xfU
#define BONSAI_TCAM_P1_MATCH_DATA_37   0x10U
#define BONSAI_TCAM_P1_MATCH_DATA_38   0x11U
#define BONSAI_TCAM_P1_MATCH_DATA_39   0x12U
#define BONSAI_TCAM_P1_MATCH_DATA_40   0x13U
#define BONSAI_TCAM_P1_MATCH_DATA_41   0x14U
#define BONSAI_TCAM_P1_MATCH_DATA_42   0x15U
#define BONSAI_TCAM_P1_MATCH_DATA_43   0x16U
#define BONSAI_TCAM_P1_MATCH_DATA_44   0x17U
#define BONSAI_TCAM_P1_MATCH_DATA_45   0x18U
#define BONSAI_TCAM_P1_MATCH_DATA_46   0x19U
#define BONSAI_TCAM_P1_MATCH_DATA_47   0x1aU
#define BONSAI_TCAM_P1_MATCH_DATA_48   0x1bU

/* TCAM Registers - Page 2 */
#define BONSAI_TCAM_P2_ACTION_1        0x2U	    /* TCAM Action 1 Register */
#define BONSAI_TCAM_P2_ACTION_2        0x3U		/* TCAM Action 2 Register */
#define BONSAI_TCAM_P2_ACTION_3        0x4U		/* TCAM Action 3 Register */
#define BONSAI_TCAM_P2_ACTION_5        0x6U		/* TCAM Action 5 Register */
#define BONSAI_TCAM_P2_ACTION_6        0x7U		/* TCAM Action 6 Register */
#define BONSAI_TCAM_P2_ACTION_7        0x8U		/* TCAM Action 7 Register */
#define BONSAI_TCAM_P2_DEBUG_PORT      0x1CU		/* TCAM Debug Register */
#define BONSAI_TCAM_P2_ALL_HIT         0x1FU		/* TCAM Debug Register 2 */

/* TCAM Registers - Page 3 */
#define BONSAI_TCAM_EGR_PORT           0x1U		/* Egress TCAM Port Register */
#define BONSAI_TCAM_EGR_ACTION_1       0x2U		/* Egress TCAM Action 1 Register */
#define BONSAI_TCAM_EGR_ACTION_2       0x3U		/* Egress TCAM Action 2 Register */
#define BONSAI_TCAM_EGR_ACTION_3       0x4U		/* Egress TCAM Action 3 Register */


/****************************************************************************/
/*																			*/
/* Block: INTERNAL PHY Registers											*/
/*																			*/
/****************************************************************************/

/* Internal Copper PHY registers*/
#define BONSAI_PHY_CONTROL_REG          0U
#define BONSAI_PHY_AUTONEGO_AD_REG      4U
#define BONSAI_PHY_AUTONEGO_1000AD_REG  9U
#define BONSAI_PHY_SPEC_CONTROL_REG		16U
#define BONSAI_PHY_PAGE_ANY_REG			22U

/* Definitions for VCT registers */
#define BONSAI_MDI0_VCT_STATUS     16U
#define BONSAI_MDI1_VCT_STATUS     17U
#define BONSAI_MDI2_VCT_STATUS     18U
#define BONSAI_MDI3_VCT_STATUS     19U
#define BONSAI_ADV_VCT_CONTROL_5   23U
#define BONSAI_ADV_VCT_SMPL_DIST   24U
#define BONSAI_ADV_VCT_CONTROL_8   20U
#define BONSAI_PAIR_SKEW_STATUS    20U
#define BONSAI_PAIR_SWAP_STATUS    21U

/* Bit Definition for QD_PHY_CONTROL_REG */
#define BONSAI_PHY_RESET            0x8000U
#define BONSAI_PHY_LOOPBACK         0x4000U
#define BONSAI_PHY_SPEED            0x2000U
#define BONSAI_PHY_AUTONEGO         0x1000U
#define BONSAI_PHY_POWER            0x800U
#define BONSAI_PHY_ISOLATE          0x400U
#define BONSAI_PHY_RESTART_AUTONEGO 0x200U
#define BONSAI_PHY_DUPLEX           0x100U
#define BONSAI_PHY_SPEED_MSB        0x40U

/* Bit Definition for QD_PHY_AUTONEGO_AD_REG */
#define BONSAI_PHY_NEXTPAGE           0x8000U
#define BONSAI_PHY_REMOTEFAULT        0x4000U
#define BONSAI_PHY_PAUSE              0x400U
#define BONSAI_PHY_100_FULL           0x100U
#define BONSAI_PHY_100_HALF           0x80U
#define BONSAI_PHY_10_FULL            0x40U
#define BONSAI_PHY_10_HALF            0x20U

#define BONSAI_PHY_MODE_AUTO_AUTO    (BONSAI_PHY_100_FULL | BONSAI_PHY_100_HALF | BONSAI_PHY_10_FULL | BONSAI_PHY_10_HALF)
#define BONSAI_PHY_MODE_100_AUTO     (BONSAI_PHY_100_FULL | BONSAI_PHY_100_HALF)
#define BONSAI_PHY_MODE_10_AUTO      (BONSAI_PHY_10_FULL  | BONSAI_PHY_10_HALF)
#define BONSAI_PHY_MODE_AUTO_FULL    (BONSAI_PHY_100_FULL | BONSAI_PHY_10_FULL)
#define BONSAI_PHY_MODE_AUTO_HALF    (BONSAI_PHY_100_HALF | BONSAI_PHY_10_HALF)

#define BONSAI_PHY_MODE_100_FULL    (BONSAI_PHY_100_FULL)
#define BONSAI_PHY_MODE_100_HALF    (BONSAI_PHY_100_HALF)
#define BONSAI_PHY_MODE_10_FULL     (BONSAI_PHY_10_FULL)   
#define BONSAI_PHY_MODE_10_HALF     (BONSAI_PHY_10_HALF)   

/* Gigabit Phy related definition */
#define BONSAI_GIGPHY_1000X_FULL_CAP    0x8U
#define BONSAI_GIGPHY_1000X_HALF_CAP    0x4U
#define BONSAI_GIGPHY_1000T_FULL_CAP    0x2U
#define BONSAI_GIGPHY_1000T_HALF_CAP    0x1U

#define BONSAI_GIGPHY_1000X_CAP        (BONSAI_GIGPHY_1000X_FULL_CAP|BONSAI_GIGPHY_1000X_HALF_CAP)
#define BONSAI_GIGPHY_1000T_CAP        (BONSAI_GIGPHY_1000T_FULL_CAP|BONSAI_GIGPHY_1000T_HALF_CAP)

#define BONSAI_GIGPHY_1000X_FULL        0x20U
#define BONSAI_GIGPHY_1000X_HALF        0x40U

#define BONSAI_GIGPHY_1000T_FULL        0x200U
#define BONSAI_GIGPHY_1000T_HALF        0x100U

#ifdef __cplusplus
}
#endif
#endif /* __Bonsai_msdDrvSwRegs_H */
