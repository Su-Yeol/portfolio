/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdPhyCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell PhyCtrl functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Amethyst_msdPhyCtrl_h
#define __Amethyst_msdPhyCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Phy Control Types			 			                        */
/****************************************************************************/

/*
* typedef: enum AMETHYST_MSD_PHY_AUTO_MODE
*
* Description: Enumeration of Autonegotiation mode.
*    Auto for both speed and duplex.
*    Auto for speed only and Full duplex.
*    Auto for speed only and Half duplex. (1000Mbps is not supported)
*    Auto for duplex only and speed 1000Mbps.
*    Auto for duplex only and speed 100Mbps.
*    Auto for duplex only and speed 10Mbps.
*    1000Mbps Full duplex.
*    100Mbps Full duplex.
*    100Mbps Half duplex.
*    10Mbps Full duplex.
*    10Mbps Half duplex.
*/

typedef enum
{
    AMETHYST_SPEED_AUTO_DUPLEX_AUTO,
    AMETHYST_SPEED_1000_DUPLEX_AUTO,
    AMETHYST_SPEED_100_DUPLEX_AUTO,
    AMETHYST_SPEED_10_DUPLEX_AUTO,
    AMETHYST_SPEED_AUTO_DUPLEX_FULL,
    AMETHYST_SPEED_AUTO_DUPLEX_HALF,
    AMETHYST_SPEED_1000_DUPLEX_FULL,
    AMETHYST_SPEED_1000_DUPLEX_HALF,
    AMETHYST_SPEED_100_DUPLEX_FULL,
    AMETHYST_SPEED_100_DUPLEX_HALF,
    AMETHYST_SPEED_10_DUPLEX_FULL,
    AMETHYST_SPEED_10_DUPLEX_HALF
}AMETHYST_MSD_PHY_AUTO_MODE;

/*
* typedef: enum AMETHYST_MSD_PHY_SPEED
*
* Description: Enumeration of Phy Speed
*
* Enumerations:
*    AMETHYST_PHY_SPEED_10_MBPS   - 10Mbps
*    AMETHYST_PHY_SPEED_100_MBPS    - 100Mbps
*    AMETHYST_PHY_SPEED_1000_MBPS - 1000Mbps
*/
typedef enum
{
    AMETHYST_PHY_SPEED_10_MBPS,
    AMETHYST_PHY_SPEED_100_MBPS,
    AMETHYST_PHY_SPEED_1000_MBPS
} AMETHYST_MSD_PHY_SPEED;

/****************************************************************************/
/* Exported Phy Control Functions		 			                        */
/****************************************************************************/

/*******************************************************************************
* Amethyst_gphyReset
*
* DESCRIPTION:
*       This routine preforms PHY reset.
*       After reset, phy will be in Autonegotiation mode.
*
* INPUTS:
* 		port - The logical port number, unless SERDES device is accessed
*        	   The physical address, if SERDES device is accessed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
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
MSD_STATUS Amethyst_gphyReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
);

/*******************************************************************************
* Amethyst_serdesReset
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
*       Amethyst: Port0, Port9 and Port10
*                 Port operation register, Device4 Register0xF002
*                 10G-BaseR PCS Control, Devcie4Register0x2000
*                 1000BaseX Control register, Devcie4Register0x1000
*
*
*******************************************************************************/
MSD_STATUS Amethyst_serdesReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
);


/*******************************************************************************
* Amethyst_gphySetPortLoopback
*
* DESCRIPTION:
*       Enable/Disable Internal Port Loopback. 
*       For 10/100 Fast Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled, this routine disables Auto-Negotiation and 
*         forces speed to be 10Mbps.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         Disabling Loopback simply clears bit 14 of control register(0.14). Therefore,
*         it is recommended to call Amethyst_gprtSetPortAutoMode for PHY configuration after 
*         Loopback test.
*       For 10/100/1000 Gigagbit Ethernet PHY, speed of Loopback is determined as follows:
*         If Auto-Negotiation is enabled and Link is active, the current speed is used.
*         If Auto-Negotiation is disabled, the forced speed is used.
*         All other cases, default MAC Interface speed is used. Please refer to the data
*         sheet for the information of the default MAC Interface speed.
*   
*
* INPUTS:
*       port - The logical port number, unless SERDES device is accessed
*              The physical address, if SERDES device is accessed
*       enable - If MSD_TRUE, enable loopback mode
*       		   If MSD_FALSE, disable loopback mode
*
* OUTPUTS:
* 		  None.
*
* RETURNS:
* 		MSD_OK - on success
* 		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.14 - Loop_back
*
*******************************************************************************/
MSD_STATUS Amethyst_gphySetPortLoopback
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN MSD_BOOL   enable
);

/*******************************************************************************
* Amethyst_gphySetPortSpeed
*
* DESCRIPTION:
*       Sets speed for a specific logical port. This function will keep the duplex 
*       mode and loopback mode to the previous value, but disable others, such as 
*       Autonegotiation.
*
* INPUTS:
*       port -  The logical port number, unless SERDES device is accessed
*               The physical address, if SERDES device is accessed
*       speed - port speed.
*               AMETHYST_PHY_SPEED_10_MBPS for 10Mbps
*               AMETHYST_PHY_SPEED_100_MBPS for 100Mbps
*               AMETHYST_PHY_SPEED_1000_MBPS for 1000Mbps
*		
* OUTPUTS:
* 		None.
*
* RETURNS:
* 		MSD_OK - on success
* 		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
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
MSD_STATUS Amethyst_gphySetPortSpeed
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN AMETHYST_MSD_PHY_SPEED speed
);
MSD_STATUS Amethyst_gphySetPortSpeedIntf
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT  port,
IN MSD_PHY_SPEED speed
);
/*******************************************************************************
* Amethyst_gprtPortPowerDown
*
* DESCRIPTION:
*        Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*        port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*        state - MSD_TRUE: power down
*                 MSD_FALSE: normal operation
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*         data sheet register 0.11 - Power Down
*
*******************************************************************************/
MSD_STATUS Amethyst_gphyPortPowerDown
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   state
);

/*******************************************************************************
* Amethyst_gphySetPortDuplexMode
*
* DESCRIPTION:
*		Sets duplex mode for a specific logical port. This function will keep 
*		the speed and loopback mode to the previous value, but disable others,
*		such as Autonegotiation.
*
* INPUTS:
*		port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*		dMode    - dulpex mode
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MSD_OK  - on success
*		MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
* 		data sheet register 0.8 - Duplex Mode
*
*       If DUT is in power down or loopback mode, port will link down, 
*       in this case, no need to do software reset to force take effect .
*       When port recover from link down, configure will take effect automatically.
*
*******************************************************************************/
MSD_STATUS Amethyst_gphySetPortDuplexMode
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   dMode
);

/*******************************************************************************
* Amethyst_gphySetPortAutoMode
*
* DESCRIPTION:
*		This routine sets up the port with given Auto Mode.
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
*		port -	The logical port number, unless SERDES device is accessed
*               The physical address, if SERDES device is accessed
*		mode - 	Auto Mode to be written
*
* OUTPUTS:
*		None.
*
* RETURNS:
*		MSD_OK   - on success
*		MSD_FAIL - on error
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
MSD_STATUS Amethyst_gphySetPortAutoMode
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN AMETHYST_MSD_PHY_AUTO_MODE mode
);
MSD_STATUS Amethyst_gphySetPortAutoModeIntf
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT  port,
IN MSD_PHY_AUTO_MODE mode
);

MSD_STATUS Amethyst_gphySetEEE
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT portNum,
    IN MSD_BOOL en
);

MSD_STATUS Amethyst_gphySetFlowControlEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_BOOL en
);

/*
* This routine enable or disable internal PHY interrupt, that is set or clear page 0 register 18 (Interrupt Enable Register). 
*/
MSD_STATUS Amethyst_gphySetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_U16 en
);

/*
* This routine enable or disable internal PHY interrupt, that is set or clear page 0 register 18 (Interrupt Enable Register).
*/
MSD_STATUS Amethyst_gphyGetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *en
);

/*
* This routine enable or disable internal PHY interrupt, that is set or clear page 0 register 18 (Interrupt Enable Register).
*/
MSD_STATUS Amethyst_gphyGetIntStatus
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *stat
);

#ifdef __cplusplus
}
#endif

#endif /* __Amethyst_msdPhyCtrl_h */
