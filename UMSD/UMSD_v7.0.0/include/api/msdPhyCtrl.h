/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdPhyCtrl.h
*
* DESCRIPTION:
*       API definitions for Phy Control.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdPhyCtrl_h
#define msdPhyCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
* msdPhyReset
*
* DESCRIPTION:
*       This routine preforms PHY reset.
*       After reset, phy will be in Autonegotiation mode.
*
* INPUTS:
*       devNum  - physical device number
* 		portNum - The logical PHY port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.15 - Reset
* 		data sheet register 0.13 - Speed(LSB)
* 		data sheet register 0.12 - Autonegotiation
* 		data sheet register 0.8  - Duplex Mode
*		data sheet register 0.6  - Speed(MSB)
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyReset
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);

/*******************************************************************************
* msdSerdesReset
*
* DESCRIPTION:
*       This routine preforms internal serdes reset.
*       different devcie have different register location for the reset bit
*
* INPUTS:
*       devNum  - physical device number
* 		portNum - The logical PHY port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*     Pearl:   Port0 and Port1, Page1 Register0 (two serdes, port0 and port1, phyAddr is 0xc and 0xd)
*     Agate:   Port4 and Port5, Page1 Register0 (one serdes, shared by port4 and port5, the phyAddr is 0xf)
*     Peridot: Port9 and Port10, Device4 Register0x1000
*     	       Port2 to Port7 (offset 0x12 - 0x17), Device4 Register0x2000
*     Topaz:   Port5 (offset 0x15), Device4 Register0x2000
*     Amethyst: Port0, Port9 and Port10
*     	        Port operation register, Device4 Register0xF002
*     	        10G-BaseR PCS Control, Devcie4Register0x2000
*     	        1000BaseX Control register, Devcie4Register0x1000
*******************************************************************************/
MSD_STATUS msdSerdesReset
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum
);

/*******************************************************************************
* msdPhyPortLoopbackSet
*
* DESCRIPTION:
*       This routine enables or disables PHY loopback.
*       For 10/100 Fast Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled, this routine disables Auto-Negotiation and
*         forces speed to be 10Mbps.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         Disabling Loopback simply clears bit 14 of control register(0.14). Therefore,
*         it is recommended to call gprtSetPortAutoMode for PHY configuration after
*         Loopback test.
*       For 10/100/1000 Gigagbit Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled and Link is active, the current speed is used.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         All other cases, default MAC Interface speed is used. Please refer to the data
*         sheet for the information of the default MAC Interface speed.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       enable - If MSD_TRUE, enable loopback mode
*       		 If MSD_FALSE, disable loopback mode
*
* OUTPUTS:
* 		  None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.14 - Loop_back
*
*******************************************************************************/
MSD_STATUS msdPhyPortLoopbackSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  enable
);

/*******************************************************************************
* msdPhyPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for a specific logical PHY port. This function will keep the duplex
*       mode and loopback mode to the previous value, but disable others, such as
*       Autonegotiation.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       speed - port speed.
*               PHY_SPEED_10_MBPS for 10Mbps
*               PHY_SPEED_100_MBPS for 100Mbps
*               PHY_SPEED_1000_MBPS for 1000Mbps
*
* OUTPUTS:
* 		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.13 - Speed Selection (LSB)
* 		data sheet register 0.6  - Speed Selection (MSB)
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortSpeedSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PHY_SPEED  speed
);

/*******************************************************************************
* msdPhyPortPowerDown
*
* DESCRIPTION:
*       Enable/disable (power down) on specific logical PHY port. Phy configuration
*       remains unchanged after Power down.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       state - MSD_TRUE: power down
*                MSD_FALSE: normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       data sheet register 0.11 - Power Down
*
*******************************************************************************/
MSD_STATUS msdPhyPortPowerDown
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  state
);

/*******************************************************************************
* msdPhyPortDuplexModeSet
*
* DESCRIPTION:
*		Sets duplex mode for a specific logical PHY port. This function will keep
*		the speed and loopback mode to the previous value, but disable others,
*		such as Autonegotiation.
*
* INPUTS:
*       devNum  - physical device number
*		portNum - The logical PHY port number
*		dMode - dulpex mode
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
* 		data sheet register 0.8 - Duplex Mode
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortDuplexModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL  dMode
);

/*******************************************************************************
* msdPhyPortAutoModeSet
*
* DESCRIPTION:
*		This routine sets the PHY port with given auto mode.
*		Supported mode is as follows:
*        - Auto for both speed and duplex.
*        - Auto for speed only and Full duplex.
*        - Auto for speed only and Half duplex.
*        - Auto for duplex only and speed 1000Mbps.
*        - Auto for duplex only and speed 100Mbps.
*        - Auto for duplex only and speed 10Mbps.
*        - Speed 1000Mbps and Full duplex.
*        - Speed 1000Mbps and Half duplex.
*        - Speed 100Mbps and Full duplex.
*        - Speed 100Mbps and Half duplex.
*        - Speed 10Mbps and Full duplex.
*        - Speed 10Mbps and Half duplex.
*
*
* INPUTS:
*       devNum  - physical device number
*		portNum -	The logical PHY port number
*		mode - 	Auto Mode to be written
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*		data sheet register 4.6, 4.5 Autonegotiation Advertisement for 10BT
*		data sheet register 4.8, 4.7 Autonegotiation Advertisement for 100BT
*		data sheet register 9.9, 9.8 Autonegotiation Advertisement for 1000BT
*
*       If DUT is in power down or loopback mode, port will link down,
*       in this case, no need to do software reset to restart AN.
*       When port recover from link down, AN will restart automatically.
*
*******************************************************************************/
MSD_STATUS msdPhyPortAutoModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_PHY_AUTO_MODE  mode
);

/*******************************************************************************
* msdPhyEEESet
*
* DESCRIPTION:
*       This routine enables or disables the EEE (Energy efficient ethernet) feature.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       en      - MSD_TRUE: enable eee
*                 MSD_FALSE: diable eee
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPhyEEESet
(
    IN MSD_U8 devNum,
    IN MSD_LPORT portNum,
    IN MSD_BOOL en
);

/*******************************************************************************
* msdPhyFlowControlEnableSet
*
* DESCRIPTION:
*       This routine enables or disables the flow control feature.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       en      - MSD_TRUE: enable eee
*                 MSD_FALSE: diable eee
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPhyFlowControlEnableSet
(
IN MSD_U8 devNum,
IN MSD_LPORT portNum,
IN MSD_BOOL en
);

/*******************************************************************************
* msdPhyInterruptEnableSet
*
* DESCRIPTION:
*       This routine enables or disables one PHY Interrupt, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur.
*       If a certain interrupt event is not enabled for the INTn pin, it will still
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*       en      - the data of interrupt to enable/disable. any combination of below
*                 MSD_PHY_AUTO_NEG_ERROR
*                 MSD_PHY_SPEED_CHANGED
*                 MSD_PHY_DUPLEX_CHANGED
*                 MSD_PHY_PAGE_RECEIVED
*                 MSD_PHY_AUTO_NEG_COMPLETED
*                 MSD_PHY_LINK_STATUS_CHANGED
*                 MSD_PHY_SYMBOL_ERROR
*                 MSD_PHY_FALSE_CARRIER
*                 MSD_PHY_MDI_CROSSOVER_CHANGED
*                 MSD_PHY_DOWNSHIFT
*                 MSD_PHY_ENERGY_DETECT
*                 MSD_PHY_FLP_EXCHANGE_COMPLETE
*                 MSD_PHY_DTE_POWER_DETECTION
*                 MSD_PHY_POLARITY_CHANGED
*                 MSD_PHY_JABBER
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPhyInterruptEnableSet
(
IN MSD_U8 devNum,
IN MSD_LPORT portNum,
IN MSD_U16 en
);

/*******************************************************************************
* msdPhyInterruptEnableGet
*
* DESCRIPTION:
*       This routine gets one PHY Interrupt enable/disable, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur. 
*       If a certain interrupt event is not enabled for the INTn pin, it will still 
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*
* OUTPUTS:
*       en      - the data of interrupt to enable/disable.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPhyInterruptEnableGet
(
IN MSD_U8 devNum,
IN MSD_LPORT portNum,
OUT MSD_U16 *en
);

/*******************************************************************************
* msdPhyInterruptStatusGet
*
* DESCRIPTION:
*       This routine gets one PHY Interrupt status, When an interrupt occurs,
*       the corresponding bit is set and remains set until register 19 is read. When
*       interrupt enable bits are not set in register 18, interrupt status bits in
*       register 19 are still set when the corresponding interrupt events occur.
*       If a certain interrupt event is not enabled for the INTn pin, it will still
*       be indicated by the corresponding Interrupt status bits if the interrupt event occurs.
*       The unselected events will not cause the INTn pin to be activated.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - The logical PHY port number
*
* OUTPUTS:
*       en      - the data of interrupt to enable/disable.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPhyInterruptStatusGet
(
IN MSD_U8 devNum,
IN MSD_LPORT portNum,
OUT MSD_U16 *status
);

#ifdef __cplusplus
}
#endif

#endif /* __msdBrgFdb_h */
