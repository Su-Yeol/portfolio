/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Pearl_msdPhyCtrl.c
*
* DESCRIPTION:
*       API implementation for switch internal Copper PHY control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <pearl/include/api/Pearl_msdPhyCtrl.h>
#include <pearl/include/driver/Pearl_msdHwAccess.h>
#include <pearl/include/api/Pearl_msdApiInternal.h>
#include <pearl/include/driver/Pearl_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>

/*
* This routine set Auto-Negotiation Ad Register for Copper
*/
static
MSD_STATUS Pearl_gigCopperSetAutoMode
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 hwPort,
	IN PEARL_MSD_PHY_AUTO_MODE mode
)
{
	MSD_U16             u16Data, u16Data1;

	if (Pearl_msdReadPagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_AUTONEGO_AD_REG, &u16Data) != MSD_OK)
	{
        return MSD_FAIL;
	}

	/* Mask out all auto mode related bits. */
	u16Data &= ~PEARL_QD_PHY_MODE_AUTO_AUTO;

	if (Pearl_msdReadPagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_AUTONEGO_1000AD_REG, &u16Data1) != MSD_OK)
	{
        return MSD_FAIL;
	}

	/* Mask out all auto mode related bits. */
	u16Data1 &= ~(PEARL_QD_GIGPHY_1000T_FULL | PEARL_QD_GIGPHY_1000T_HALF);

	switch (mode)
	{
		case PEARL_SPEED_AUTO_DUPLEX_AUTO:
			u16Data |= PEARL_QD_PHY_MODE_AUTO_AUTO;
		case PEARL_SPEED_1000_DUPLEX_AUTO:
			u16Data1 |= PEARL_QD_GIGPHY_1000T_FULL | PEARL_QD_GIGPHY_1000T_HALF;
			break;
		case PEARL_SPEED_AUTO_DUPLEX_FULL:
			u16Data |= PEARL_QD_PHY_MODE_AUTO_FULL;
			u16Data1 |= PEARL_QD_GIGPHY_1000T_FULL;
			break;
		case PEARL_SPEED_1000_DUPLEX_FULL:
			u16Data1 |= PEARL_QD_GIGPHY_1000T_FULL;
			break;
		case PEARL_SPEED_1000_DUPLEX_HALF:
			u16Data1 |= PEARL_QD_GIGPHY_1000T_HALF;
			break;
		case PEARL_SPEED_AUTO_DUPLEX_HALF:
			u16Data |= PEARL_QD_PHY_MODE_AUTO_HALF;
			u16Data1 |= PEARL_QD_GIGPHY_1000T_HALF;
			break;
		case PEARL_SPEED_100_DUPLEX_AUTO:
			u16Data |= PEARL_QD_PHY_MODE_100_AUTO;
			break;
		case PEARL_SPEED_10_DUPLEX_AUTO:
			u16Data |= PEARL_QD_PHY_MODE_10_AUTO;
			break;
		case PEARL_SPEED_100_DUPLEX_FULL:
			u16Data |= PEARL_QD_PHY_100_FULL;
			break;
		case PEARL_SPEED_100_DUPLEX_HALF:
			u16Data |= PEARL_QD_PHY_100_HALF;
			break;
		case PEARL_SPEED_10_DUPLEX_FULL:
			u16Data |= PEARL_QD_PHY_10_FULL;
			break;
		case PEARL_SPEED_10_DUPLEX_HALF:
			u16Data |= PEARL_QD_PHY_10_HALF;
			break;
		default:
            return MSD_BAD_PARAM;
	}

	/* Write to Phy AutoNegotiation Advertisement Register.  */
	if (Pearl_msdWritePagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_AUTONEGO_AD_REG, u16Data) != MSD_OK)
	{
        return MSD_FAIL;
	}

	/* Write to Phy AutoNegotiation 1000B Advertisement Register.  */
	if (Pearl_msdWritePagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_AUTONEGO_1000AD_REG, u16Data1) != MSD_OK)
	{
        return MSD_FAIL;
	}

	return MSD_OK;
}

/*
* This routine sets Auto Mode and Reset the phy
*/
static
MSD_STATUS Pearl_phySetAutoMode
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 hwPort,
	IN PEARL_MSD_PHY_AUTO_MODE mode
)
{
	MSD_U16         u16Data;
	MSD_STATUS    status;

	if ((status = Pearl_gigCopperSetAutoMode(dev, hwPort, mode)) != MSD_OK)
	{
		return status;
	}

	/* Read to Phy Control Register.  */
	if (Pearl_msdReadPagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, &u16Data) != MSD_OK)
		return MSD_FAIL;

	u16Data |= PEARL_QD_PHY_AUTONEGO;

	/* Write to Phy Control Register.  */
	if (Pearl_msdWritePagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, u16Data) != MSD_OK)
		return MSD_FAIL;

	return Pearl_msdPhyReset(dev, hwPort, 0, 0xFF);
}


/*******************************************************************************
* Pearl_gphyReset
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
MSD_STATUS Pearl_gphyReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gphyReset Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	
    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Page Register return %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    /* set Auto Negotiation AD Register */
    retVal = Pearl_phySetAutoMode(dev, hwPort, PEARL_SPEED_AUTO_DUPLEX_AUTO);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Pearl_phySetAutoMode PEARL_SPEED_AUTO_DUPLEX_AUTO returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
    }

	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gphyReset Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_serdesReset
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
*       Pearl:   Port0 and Port1, Page1 Register0 (two serdes, port0 and port1, phyAddr is 0xc and 0xd)
*
*******************************************************************************/
MSD_STATUS Pearl_serdesReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16  data;

    MSD_DBG_INFO(("Pearl_serdesReset Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_SERDES(port);
    if (hwPort == MSD_INVALID_PHY)
    {
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);
    if ((retVal = Pearl_msdReadPagedPhyReg(dev, hwPort, 1, 0, &data)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read 1000BASE-X/SGMI Control register error %s.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }

    data = data | 0x8000;

    if ((retVal = Pearl_msdWritePagedPhyReg(dev, hwPort, 1, 0, data)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write 1000BASE-X/SGMI Control register error %s.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->phyRegsSem);

    MSD_DBG_INFO(("Pearl_serdesReset Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gphySetPortLoopback
*
* DESCRIPTION:
*       Enable/Disable Internal Port Loopback. 
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
*       port - The logical port number, unless SERDES device is accessed
*              The physical address, if SERDES device is accessed
*       enable - If MSD_TRUE, enable loopback mode
*       		 If MSD_FALSE, disable loopback mode
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
MSD_STATUS Pearl_gphySetPortLoopback
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN MSD_BOOL   enable
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16             u16Data;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gphySetPortLoopback Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	
    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    MSD_BOOL_2_BIT(enable,u16Data);

    /* Write to Phy Control Register.  */
	retVal = Pearl_msdSetPagedPhyRegField(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, 14, 1, u16Data);
    if(retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("write Phy Control Register  %s.\n", msdDisplayStatus(retVal))); 
        msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
    }
   
	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetPortLoopback Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gphySetPortSpeed
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
*               PEARL_PHY_SPEED_10_MBPS for 10Mbps
*               PEARL_PHY_SPEED_100_MBPS for 100Mbps
*               PEARL_PHY_SPEED_1000_MBPS for 1000Mbps
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
MSD_STATUS Pearl_gphySetPortSpeed
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN PEARL_MSD_PHY_SPEED speed
)
{
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16             u16Data;
    MSD_STATUS        retVal;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gphySetPortSpeed Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	
    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if (Pearl_msdReadPagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, &u16Data) != MSD_OK)
    {

        MSD_DBG_ERROR(("read Phy Control Register %s.\n", msdDisplayStatus(retVal))); 
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return MSD_FAIL;
    }

    switch(speed)
    {
    case PEARL_PHY_SPEED_10_MBPS:
            u16Data = u16Data & (PEARL_QD_PHY_LOOPBACK | PEARL_QD_PHY_DUPLEX);
            break;
    case PEARL_PHY_SPEED_100_MBPS:
            u16Data = (u16Data & (PEARL_QD_PHY_LOOPBACK | PEARL_QD_PHY_DUPLEX)) | PEARL_QD_PHY_SPEED;
            break;
    case PEARL_PHY_SPEED_1000_MBPS:
            u16Data = (u16Data & (PEARL_QD_PHY_LOOPBACK | PEARL_QD_PHY_DUPLEX)) | PEARL_QD_PHY_SPEED_MSB;
            break;
        default:
            msdSemGive(dev->devNum, dev->phyRegsSem);
            MSD_DBG_ERROR(("Failed (Bad speed).\n"));
            return MSD_BAD_PARAM;
    }

	retVal = Pearl_msdPhyReset(dev, hwPort, 0, u16Data);
	if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Pearl_msdPhyReset returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}
    
	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetPortSpeed Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gprtPortPowerDown
*
* DESCRIPTION:
*        Enable/disable (power down) on specific logical port.
*        Phy configuration remains unchanged after Power down.
*
* INPUTS:
*       port -  The logical port number, unless SERDES device is accessed
*               The physical address, if SERDES device is accessed
*       state - MSD_TRUE: power down
*                MSD_FALSE: normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       data sheet register 0.11 - Power Down
*
*******************************************************************************/
MSD_STATUS Pearl_gphyPortPowerDown
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   state
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          u16Data;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gprtPortPowerDown Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	
    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("Not able to read Phy Page Register.\n"));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    MSD_BOOL_2_BIT(state,u16Data);

	if ((retVal = Pearl_msdSetPagedPhyRegField(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, 11, 1, u16Data)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Control Register %s.\n", msdDisplayStatus(retVal))); 
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }

	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gprtPortPowerDown Exit.\n"));
    return MSD_OK;
}

/*******************************************************************************
* Pearl_gphySetPortDuplexMode
*
* DESCRIPTION:
*		Sets duplex mode for a specific logical port. This function will keep 
*		the speed and loopback mode to the previous value, but disable others,
*		such as Autonegotiation.
*
* INPUTS:
*		port -  The logical port number, unless SERDES device is accessed
*                The physical address, if SERDES device is accessed
*		dMode - dulpex mode
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
MSD_STATUS Pearl_gphySetPortDuplexMode
(
	IN MSD_QD_DEV *dev,
	IN MSD_LPORT  port,
	IN MSD_BOOL   dMode
)
{
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16             u16Data;
    MSD_STATUS        retVal;
	MSD_U16  pageReg;

    MSD_DBG_INFO(("Pearl_gphySetPortDuplexMode Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}
	
    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if (Pearl_msdReadPagedPhyReg(dev, hwPort, 0, PEARL_QD_PHY_CONTROL_REG, &u16Data) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Control Register %s.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return MSD_FAIL;
    }

    if(dMode)
    {
        u16Data = (u16Data & (PEARL_QD_PHY_LOOPBACK | PEARL_QD_PHY_SPEED | PEARL_QD_PHY_SPEED_MSB)) | PEARL_QD_PHY_DUPLEX;
    }
    else
    {
        u16Data = u16Data & (PEARL_QD_PHY_LOOPBACK | PEARL_QD_PHY_SPEED | PEARL_QD_PHY_SPEED_MSB);
    }

    /* Write to Phy Control Register.  */
	retVal = Pearl_msdPhyReset(dev, hwPort, 0, u16Data);
	if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Pearl_msdPhyReset returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}
    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetPortDuplexMode Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Pearl_gphySetPortAutoMode
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
*       MSD_BAD_PARAM - if invalid parameter is given
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
MSD_STATUS Pearl_gphySetPortAutoMode
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port,
    IN PEARL_MSD_PHY_AUTO_MODE mode
)
{

    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U16  pageReg;
	
    MSD_DBG_INFO(("Pearl_gphySetPortAutoMode Called.\n"));
    
    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(port);
	if (hwPort == MSD_INVALID_PHY)
	{
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)port));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->phyRegsSem,OS_WAIT_FOREVER);
	if ((retVal = Pearl_msdGetSMIPhyRegField(dev, hwPort, 22, 0, 8, &pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("read Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	retVal = Pearl_phySetAutoMode(dev, hwPort, mode);
	if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Pearl_phySetAutoMode PEARL_SPEED_AUTO_DUPLEX_AUTO returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}

	if ((retVal = Pearl_msdSetSMIPhyRegField(dev, hwPort, 22, 0, 8, pageReg)) != MSD_OK)
    {
        MSD_DBG_ERROR(("write back Phy Page Register %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum,  dev->phyRegsSem);
		return retVal;
	}
	
    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetPortAutoMode Exit.\n"));
    return retVal;

}

MSD_STATUS Pearl_gphySetEEE
(
    IN MSD_QD_DEV *dev,
    MSD_LPORT portNum,
    MSD_BOOL en
)
{
    MSD_STATUS retVal;
    MSD_U16 data;
    MSD_U8 hwPort;

	MSD_DBG_INFO(("Pearl_gphySetEEE Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_PHY(portNum);
    if (hwPort == MSD_INVALID_PHY)
    {
        MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)portNum));
        return MSD_BAD_PARAM;
    }
    switch (en)
    {
    case MSD_TRUE:
        data = 0x06;
        break;
    case MSD_FALSE:
        data = 0;
        break;
    default:
        data = 0x06;
        break;
    }

    msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

    /*enable/Disable the EEE for 100Base-T and 1000Base-T*/
    retVal = Pearl_msdSetSMIPhyXMDIOReg(dev, (MSD_U8)hwPort, 7, 60, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("enable/Disable the EEE for 100Base-T and 1000Base-T.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }
    /*Software reset*/
    retVal = Pearl_msdReadPagedPhyReg(dev, (MSD_U8)hwPort, 0, 0, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("get status.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }
    data = data | 0x8000;
    retVal = Pearl_msdWritePagedPhyReg(dev, (MSD_U8)hwPort, 0, 0, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("Software reset.\n", msdDisplayStatus(retVal)));
        msdSemGive(dev->devNum, dev->phyRegsSem);
        return retVal;
    }

    msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetEEE Exit.\n"));
    return retVal;
}

MSD_STATUS Pearl_gphySetFlowControlEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_BOOL en
)
{
	MSD_STATUS retVal;
	MSD_U16 data, tmpdata;
	MSD_U8 hwPort;

	MSD_DBG_INFO(("Pearl_gphySetFlowControlEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PHY(portNum);
	if (hwPort == MSD_INVALID_PHY)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	switch (en)
	{
	case MSD_TRUE:
		data = 0x1;
		break;
	case MSD_FALSE:
		data = 0x0;
		break;
	default:
		data = 0x1;
		break;
	}

	msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

	/*enable/Disable the flow control page 0 reg 4 bit 10*/
	retVal = Pearl_msdReadPagedPhyReg(dev, (MSD_U8)hwPort, 0, 4, &tmpdata);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("enable/Disable flow control.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	data = (tmpdata & 0xFBFF) | (data << 10);

	retVal = Pearl_msdWritePagedPhyReg(dev, (MSD_U8)hwPort, 0, 4, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("enable/Disable flow control.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}
	/*Software reset*/
	retVal = Pearl_msdReadPagedPhyReg(dev, (MSD_U8)hwPort, 0, 0, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("get status.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}
	data = data | 0x8000;
	retVal = Pearl_msdWritePagedPhyReg(dev, (MSD_U8)hwPort, 0, 0, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Software reset.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->phyRegsSem);
	MSD_DBG_INFO(("Pearl_gphySetFlowControlEnable Exit.\n"));
	return retVal;
}

/*******************************************************************************
* Pearl_gphySetIntEnable
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
MSD_STATUS Pearl_gphySetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
IN MSD_U16 en
)
{
	MSD_STATUS retVal;
	MSD_U8 hwPort;

	MSD_DBG_INFO(("Pearl_gphySetIntEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PHY(portNum);
	if (hwPort == MSD_INVALID_PHY)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

	/*enable/Disable page 0 reg 18 interrupt enable*/
	retVal = Pearl_msdWritePagedPhyReg(dev, (MSD_U8)hwPort, 0, 18, (MSD_U16)en);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("enable/Disable interrupt.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gphySetIntEnable Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_gphyGetIntEnable
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
MSD_STATUS Pearl_gphyGetIntEnable
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *en
)
{
	MSD_STATUS retVal;
	MSD_U8 hwPort;
	MSD_U16 data;

	MSD_DBG_INFO(("Pearl_gphyGetIntEnable Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PHY(portNum);
	if (hwPort == MSD_INVALID_PHY)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

	/*enable/Disable page 0 reg 18 interrupt enable*/
	retVal = Pearl_msdReadPagedPhyReg(dev, (MSD_U8)hwPort, 0, 18, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("enable/Disable interrupt.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	*en = (MSD_U16)data;

	msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gphyGetIntEnable Exit.\n"));

	return retVal;
}

/*******************************************************************************
* Pearl_gphyGetIntStatus
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
MSD_STATUS Pearl_gphyGetIntStatus
(
IN MSD_QD_DEV *dev,
IN MSD_LPORT portNum,
OUT MSD_U16 *stat
)
{
	MSD_STATUS retVal;
	MSD_U8 hwPort;
	MSD_U16 data;

	MSD_DBG_INFO(("Pearl_gphyGetIntStatus Called.\n"));

	/* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PHY(portNum);
	if (hwPort == MSD_INVALID_PHY)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}

	msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

	/*enable/Disable page 0 reg 19 interrupt status*/
	retVal = Pearl_msdReadPagedPhyReg(dev, (MSD_U8)hwPort, 0, 19, &data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("enable/Disable interrupt.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->phyRegsSem);
		return retVal;
	}

	*stat = (MSD_U16)data;

	msdSemGive(dev->devNum, dev->phyRegsSem);

	MSD_DBG_INFO(("Pearl_gphyGetIntStatus Exit.\n"));

	return retVal;
}