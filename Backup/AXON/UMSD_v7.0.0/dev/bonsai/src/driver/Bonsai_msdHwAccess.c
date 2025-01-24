/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* Bonsai_msdHwAccess.c
*
* DESCRIPTION:
*       Functions definition for HW accessing PHY, Serdes and Switch registers.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <bonsai/include/driver/Bonsai_msdHwAccess.h>
#include <bonsai/include/driver/Bonsai_msdDrvSwRegs.h>
#include <bonsai/include/api/Bonsai_msdApiInternal.h>
#include <driver/msdHwAccess.h>
#include <platform/msdSem.h>
#include <utils/msdUtils.h>


/****************************************************************************/
/* SMI PHY Registers indirect R/W functions.                                         */
/****************************************************************************/
/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdSetSMIC45PhyReg_MultiChip
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
)
{
    MSD_U32 timeOut;
    MSD_U16 smiReg;

    MSD_DBG(("(LOG BONSAI SMI PHY C45 RW): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
        if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_ERROR(("read SMI Phy BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_INFO(("read SMI Phy BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    return MSD_OK;
}

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdGetSMIC45PhyReg_MultiChip
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
)
{
	MSD_U32 timeOut;
    MSD_U16 smiReg;
    MSD_U16 tmp;

    MSD_DBG_INFO(("Read Phy register while PPU Enabled\n"));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_READ_45 << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write TOPAZ_QD_REG_SMI_PHY_CMD register Failed\n"));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, &tmp) != MSD_OK)
    {
        MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    *data = tmp;

    MSD_DBG(("(LOG BONSAI SMI PHY C45 RR): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}

/*****************************************************************************
* msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdSetSMIPhyReg_MultiChip
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN MSD_U16   data
)
{
    MSD_U32 timeOut;
    MSD_U16 smiReg;

    MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    return MSD_OK;
}

/*****************************************************************************
* msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdGetSMIPhyReg_MultiChip
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT  MSD_U16   *data
)
{
    MSD_U32 timeOut;
    MSD_U16 smiReg;

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_ERROR(("Read BONSAI_QD_REG_SMI_PHY_CMD Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_READ_22 << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do
    {
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if (timeOut < 1U)
        {
            MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, &smiReg) != MSD_OK)
    {
        MSD_DBG_INFO(("Reading Phy register Failed\n"));
        return MSD_FAIL;
    }
    *data = smiReg;

    MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}


/*****************************************************************************
* msdSetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly write internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdSetSMIC45PhyReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8    devAddr,
    IN MSD_U8    phyAddr,
    IN MSD_U16   regAddr,
    IN MSD_U16   data
)
{
    MSD_U32 timeOut; 
    MSD_U16 smiReg;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_msdSetSMIC45PhyReg_MultiChip(dev, devAddr, phyAddr, regAddr, data);
    }

    MSD_DBG(("(LOG BONSAI SMI PHY C45 RW): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_ERROR(("read SMI Phy BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_INFO(("read SMI Phy BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

	return MSD_OK;
}

/*****************************************************************************
* msdGetSMIC45PhyReg
*
* DESCRIPTION:
*       This function indirectly read internal SERDES register through SMI PHY command.
*
* INPUTS:
*       devAddr - The device address.
*       phyAddr - The PHY address.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register data to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdGetSMIC45PhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8     devAddr,
    IN  MSD_U8     phyAddr,
    IN  MSD_U16    regAddr,
    OUT MSD_U16    *data
)
{
    MSD_U32 timeOut;
    MSD_U16 smiReg;
	MSD_U16 tmp;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_msdGetSMIC45PhyReg_MultiChip(dev, devAddr, phyAddr, regAddr, data);
    }

    MSD_DBG_INFO(("Read Phy register while PPU Enabled\n"));

     /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, regAddr) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE_ADDR << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)phyAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_READ_45 << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)devAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE45 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("Write TOPAZ_QD_REG_SMI_PHY_CMD register Failed\n"));
        return MSD_FAIL;
    }

    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, &tmp) != MSD_OK)
    {
        MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	
    *data = tmp;

    MSD_DBG(("(LOG BONSAI SMI PHY C45 RR): devAddr 0x%02x, phyAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, phyAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}

/*****************************************************************************
* msdSetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly write internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdSetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN MSD_U16   data
)
{
    MSD_U32 timeOut;
    MSD_U16 smiReg;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_msdSetSMIPhyReg_MultiChip(dev, devAddr, regAddr, data);
    }

    MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", data));

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }

    return MSD_OK;
}

/*****************************************************************************
* msdGetSMIPhyReg
*
* DESCRIPTION:
*       This function indirectly read internal PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdGetSMIPhyReg
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    OUT  MSD_U16   *data
)
{
    MSD_U32 timeOut; 
    MSD_U16 smiReg;

    if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
    {
        return Bonsai_msdGetSMIPhyReg_MultiChip(dev, devAddr, regAddr, data);
    }

    /* first check that it is not busy */
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_ERROR(("Read BONSAI_QD_REG_SMI_PHY_CMD Timed Out\n"));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_READ_22 << MSD_SMI_OP_BIT) |
		(MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
    {
        MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
        return MSD_FAIL;
    }
	
    timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

    do 
    {
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
        {
            MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
        if(timeOut < 1U) 
        {
            MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
            return MSD_FAIL;
        }
		timeOut -= 1U;

    } while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, &smiReg) != MSD_OK)
    {
        MSD_DBG_INFO(("Reading Phy register Failed\n"));
        return MSD_FAIL;
    }
    *data = smiReg;

    MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
        devAddr, regAddr));
    MSD_DBG(("data 0x%04x.\n", *data));

    return MSD_OK;
}

/*****************************************************************************
* msdSetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly write internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*		data		- register date to be written.
*
* OUTPUTS:
*		None
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdSetSMIPhyRegField
(
    IN MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN MSD_U16    data
)
{
    MSD_U16 mask;
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal =  Bonsai_msdGetSMIPhyReg(dev,devAddr,regAddr,&tmpData);
    if(retVal != MSD_OK)
    {
        return retVal;
    }

    MSD_CALC_MASK(fieldOffset,fieldLength,mask);

    /* Set the desired bits to 0.                       */
	tmpData &= (MSD_U16)~mask;
    /* Set the given data into the above reset bits.    */
	tmpData |= (MSD_U16)(data << fieldOffset) & mask;
    MSD_DBG_INFO(("Write to devAddr(%d): regAddr 0x%x, \n",
              devAddr,regAddr));
    MSD_DBG_INFO(("fieldOff %d, fieldLen %d, data 0x%x.\n",fieldOffset,
              fieldLength,data));

    retVal = Bonsai_msdSetSMIPhyReg(dev,devAddr,regAddr,tmpData);

    return retVal;
}

/*****************************************************************************
* msdGetSMIPhyRegField
*
* DESCRIPTION:
*       This function indirectly read internal PHY specified register field through SMI PHY command.
*
* INPUTS:
*       devAddr     - The PHY address to be read.
*       regAddr     - The register address to read.
*       fieldOffset - The field start bit index. (0 - 15)
*       fieldLength - Number of bits to write.
*
* OUTPUTS:
*       data       - The storage where register date to be saved.
*
* RETURNS:
*       MSD_TRUE   - on success
*       MSD_FALSE  - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdGetSMIPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    devAddr,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT  MSD_U16   *data
)
{
    MSD_U16 mask;            /* Bits mask to be read */
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal =  Bonsai_msdGetSMIPhyReg(dev,devAddr,regAddr,&tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
        
    MSD_CALC_MASK(fieldOffset,fieldLength,mask);

    tmpData = (tmpData & mask) >> fieldOffset;
    *data = tmpData;
    MSD_DBG_INFO(("Read from deviceAddr(%d): regAddr 0x%x, \n",
              devAddr,regAddr));
    MSD_DBG_INFO(("fOff %d, fLen %d, data 0x%x.\n",fieldOffset,fieldLength,*data));

    return MSD_OK;
}

MSD_STATUS Bonsai_msdSetPagedPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
)	
{
	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	retVal = Bonsai_msdSetSMIPhyRegField(dev, portNum, regAddr, fieldOffset, fieldLength, data);
	return retVal;
}	

MSD_STATUS Bonsai_msdGetPagedPhyRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   *data
)	
{
	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	retVal = Bonsai_msdGetSMIPhyRegField(dev, portNum, regAddr, fieldOffset, fieldLength, data);
	return retVal;
}

MSD_STATUS Bonsai_msdWritePagedPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    IN  MSD_U16   data
)
{
	
	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	retVal = Bonsai_msdSetSMIPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Bonsai_msdReadPagedPhyReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    portNum,
    IN  MSD_U8    pageNum,
    IN  MSD_U8    regAddr,
    OUT MSD_U16   *data
)
{
	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	retVal = Bonsai_msdGetSMIPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Bonsai_msdSetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16 regAddr,
    IN MSD_U16 data
)
{
    MSD_STATUS retVal;
    MSD_U16 tmpData;
    MSD_U8 hwPort;

    hwPort = MSD_LPORT_2_PHY((MSD_LPORT)portNum);
    if (hwPort == MSD_INVALID_PORT)
    {
        MSD_DBG_ERROR(("Bad Port: %d.\n", portNum));
        return MSD_BAD_PARAM;
    }

    /* Set MMD access control address is Address | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_ADDR | devAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, hwPort, (MSD_U8)0, (MSD_U8)13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
        return retVal;
    }

    /* Set MMD access Data is regAddr */
    tmpData = regAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, hwPort, (MSD_U8)0, (MSD_U8)14, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
        return retVal;
    }

    /* Set MMD access control address is Data no inc | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_DATA_NO_INC | devAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, hwPort, (MSD_U8)0, (MSD_U8)13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
        return retVal;
    }

    /* Write MMD access Data */
	retVal = Bonsai_msdWritePagedPhyReg(dev, hwPort, (MSD_U8)0, (MSD_U8)14, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Write MMD access Data.\n"));
        return retVal;
    }

    return MSD_OK;
}

MSD_STATUS Bonsai_msdGetSMIPhyXMDIOReg
(
    IN MSD_QD_DEV *dev,
    IN MSD_U8  portNum,
    IN MSD_U8  devAddr,
    IN MSD_U16  regAddr,
    OUT MSD_U16 *data
)
{
    MSD_STATUS retVal;
    MSD_U16 tmpData;

    /* Set MMD access control address is Address | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_ADDR | devAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
        return retVal;
    }

    /* Set MMD access Data is regAddr */
    tmpData = regAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
        return retVal;
    }

    /* Set MMD access control address is Data no inc | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_DATA_NO_INC | devAddr;
	retVal = Bonsai_msdWritePagedPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
        return retVal;
    }

    /* Read MMD access Data */
	retVal = Bonsai_msdReadPagedPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, &tmpData);
    if (retVal != MSD_OK)
    {
        MSD_DBG_ERROR(("ERROR to Read MMD access Data.\n"));
        return retVal;
    }

    *data = tmpData;

    return MSD_OK;
}

/*******************************************************************************
* msdPhyReset
*
* DESCRIPTION:
*       This function performs softreset and waits until reset completion.
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       u16Data     - data should be written into Phy control register.
*                      if this value is 0xFF, normal operation occcurs (read, 
*                      update, and write back.)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdPhyReset
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_U8        portNum,
	IN  MSD_U8		 pageNum,
    IN  MSD_U16       u16Data
)
{
    MSD_U16 tmpData = 0;
    MSD_STATUS   retVal;
    MSD_U32 retryCount;
    MSD_BOOL    pd = MSD_FALSE;

	retVal = Bonsai_msdReadPagedPhyReg(dev, portNum, pageNum, 0, &tmpData);
	if(retVal != MSD_OK)
    {
		MSD_DBG_INFO(("Reading Register failed\n"));
		return retVal;
    }

    if ((tmpData & (MSD_U16)0x800) != 0U)
    {
        pd = MSD_TRUE;
    }

	if (u16Data != (MSD_U16)0xFF)
    {
        tmpData = u16Data;
    }

    /* Set the desired bits to 0. */
    if (pd == MSD_TRUE)
    {
		tmpData |= (MSD_U16)0x800;
    }
    else
    {
		if (((tmpData & (MSD_U16)0x4000) == 0U) || (u16Data == (MSD_U16)0xFF)) /* setting loopback do not set reset */
		{
			tmpData |= (MSD_U16)0x8000;
		}
    }

	retVal = Bonsai_msdWritePagedPhyReg(dev, portNum, pageNum, 0, tmpData);
	if(retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Writing to register failed\n"));
        return retVal;
    }

    if (pd == MSD_TRUE)
    {
        return MSD_OK;
    }

    for (retryCount = 0x1000U; retryCount > 0U; retryCount--)
    {
		retVal = Bonsai_msdReadPagedPhyReg(dev, portNum, pageNum, 0, &tmpData);
		if(retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Reading register failed\n"));
            return retVal;
        }
		if ((tmpData & (MSD_U16)0x8000) == 0U)
		{
			break;
		}
    }

    if (retryCount == 0U)
    {
        MSD_DBG_INFO(("Reset bit is not cleared\n"));
        return MSD_FAIL;
    }

    return MSD_OK;
}

/*******************************************************************************
* Bonsai_msdDirectSetMultiChipReg
*
* DESCRIPTION:
*       This function performs multi chip mode register write can minimize overhead 
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       u16Data     - data should be written into register.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdDirectSetMultiChipReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  regAddr,
    IN  MSD_U16 data
)
{
    return dev->fgtWriteMii(dev->devNum, dev->phyAddr, regAddr, data);
}

/*******************************************************************************
* Bonsai_msdDirectGetMultiChipReg
*
* DESCRIPTION:
*       This function performs multi chip mode register read can minimize overhead 
*
* INPUTS:
*       portNum     - Port number to write the register for.
*
*
* OUTPUTS:
*       data        - register value read back
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdDirectGetMultiChipReg
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8  regAddr,
    OUT MSD_U16 *data
)
{
    return dev->fgtReadMii(dev->devNum, dev->phyAddr, regAddr, data);
}

MSD_STATUS Bonsai_msdDirectSetMultiChipRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    IN  MSD_U16   data
)
{
    MSD_U16 mask;
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal = Bonsai_msdDirectGetMultiChipReg(dev, regAddr, &tmpData);
    if (retVal != MSD_OK)
    {
        return retVal;
    }

    MSD_CALC_MASK(fieldOffset, fieldLength, mask);

    /* Set the desired bits to 0.                       */
    tmpData &= (MSD_U16)~mask;
    /* Set the given data into the above reset bits.    */
	tmpData |= (MSD_U16)(data << fieldOffset) & mask;

    retVal = Bonsai_msdDirectSetMultiChipReg(dev, regAddr, tmpData);

    return retVal;
}

MSD_STATUS Bonsai_msdDirectGetMultiChipRegField
(
    IN  MSD_QD_DEV *dev,
    IN  MSD_U8    regAddr,
    IN  MSD_U8    fieldOffset,
    IN  MSD_U8    fieldLength,
    OUT MSD_U16   *data
)
{
    MSD_U16 mask;            /* Bits mask to be read */
    MSD_U16 tmpData;
    MSD_STATUS   retVal;

    retVal = Bonsai_msdDirectGetMultiChipReg(dev, regAddr, &tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

    MSD_CALC_MASK(fieldOffset, fieldLength, mask);

    tmpData = (tmpData & mask) >> fieldOffset;
    *data = tmpData;

    return MSD_OK;
}

/*******************************************************************************
* Bonsai_msdDirectSetMultiChipExtendedReg
*
* DESCRIPTION:
*       This function performs multi chip mode extended register write can minimize overhead
*
* INPUTS:
*       portNum     - Port number to write the register for.
*       u16Data     - data should be written into register.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdDirectSetMultiChipExtendedReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  regAddr,
IN  MSD_U32 data
)
{
	dev->fgtWriteMii(dev->devNum, 0x1B, 0x16, data >> 16);
	return dev->fgtWriteMii(dev->devNum, dev->phyAddr, regAddr, data & 0xFFF);
}

/*******************************************************************************
* Bonsai_msdDirectGetMultiChipExtendedReg
*
* DESCRIPTION:
*       This function performs multi chip mode extended register read can minimize overhead
*
* INPUTS:
*       portNum     - Port number to write the register for.
*
*
* OUTPUTS:
*       data        - register value read back
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*		None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdDirectGetMultiChipExtendedReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8  regAddr,
OUT MSD_U32 *data
)
{
	MSD_STATUS  status;
	MSD_U16     dataHi;
	MSD_U16     dataLo;

	status = dev->fgtReadMii(dev->devNum, dev->phyAddr, regAddr, &dataLo);
	status = dev->fgtReadMii(dev->devNum, 0x1B, 0x16, &dataHi);

	*data = (MSD_U32)((MSD_U32)dataHi << 16) | (MSD_U32)dataLo;
	return status;
}

MSD_STATUS Bonsai_msdDirectSetMultiChipExtendedRegField
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    regAddr,
IN  MSD_U8    fieldOffset,
IN  MSD_U8    fieldLength,
IN  MSD_U32   data
)
{
	MSD_U32 mask;
	MSD_U32 tmpData;
	MSD_STATUS   retVal;

	retVal = Bonsai_msdDirectGetMultiChipExtendedReg(dev, regAddr, &tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	MSD_CALC_MASK_32(fieldOffset, fieldLength, mask);

	/* Set the desired bits to 0.                       */
	tmpData &= (MSD_U32)~mask;
	/* Set the given data into the above reset bits.    */
	tmpData |= (MSD_U32)(data << fieldOffset) & mask;

	retVal = Bonsai_msdDirectSetMultiChipExtendedReg(dev, regAddr, tmpData);

	return retVal;
}

MSD_STATUS Bonsai_msdDirectGetMultiChipExtendedRegField
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    regAddr,
IN  MSD_U8    fieldOffset,
IN  MSD_U8    fieldLength,
OUT MSD_U32   *data
)
{
	MSD_U32 mask;            /* Bits mask to be read */
	MSD_U32 tmpData;
	MSD_STATUS   retVal;

	retVal = Bonsai_msdDirectGetMultiChipExtendedReg(dev, regAddr, &tmpData);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	MSD_CALC_MASK(fieldOffset, fieldLength, mask);

	tmpData = (tmpData & mask) >> fieldOffset;
	*data = tmpData;

	return MSD_OK;
}

/********************Below is external PHY register access*******************/

/*****************************************************************************
* msdSetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly write external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdSetSMIExtPhyReg_MultiChip
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
IN MSD_U16   data
)
{
	MSD_U32 timeOut;
	MSD_U16 smiReg;

	MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
		devAddr, regAddr));
	MSD_DBG(("data 0x%04x.\n", data));

	/* first check that it is not busy */
	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_EXTERNAL << MSD_SMI_FUNC_BIT) | 
		(MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) | (MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	return MSD_OK;
}

/*****************************************************************************
* msdGetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly read external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static MSD_STATUS Bonsai_msdGetSMIExtPhyReg_MultiChip
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
OUT  MSD_U16   *data
)
{
	MSD_U32 timeOut;
	MSD_U16 smiReg;

	/* first check that it is not busy */
	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_ERROR(("Read BONSAI_QD_REG_SMI_PHY_CMD Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_EXTERNAL << MSD_SMI_FUNC_BIT) |
		(MSD_U16)((MSD_U16)MSD_SMI_READ_22 << MSD_SMI_OP_BIT) | (MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (Bonsai_msdDirectSetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (Bonsai_msdDirectGetMultiChipReg(dev, BONSAI_SMI_PHY_DATA, &smiReg) != MSD_OK)
	{
		MSD_DBG_INFO(("Reading Phy register Failed\n"));
		return MSD_FAIL;
	}
	*data = smiReg;

	MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
		devAddr, regAddr));
	MSD_DBG(("data 0x%04x.\n", *data));

	return MSD_OK;
}

/*****************************************************************************
* msdSetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly write external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr - The PHY address.
*       regAddr - The register address.
*       data - data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdSetSMIExtPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
IN MSD_U16   data
)
{
	MSD_U32 timeOut;
	MSD_U16 smiReg;

	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		return Bonsai_msdSetSMIExtPhyReg_MultiChip(dev, devAddr, regAddr, data);
	}

	MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
		devAddr, regAddr));
	MSD_DBG(("data 0x%04x.\n", data));

	/* first check that it is not busy */
	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_INFO(("read BONSAI_QD_REG_SMI_PHY_CMD register Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);


	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, data) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}
	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_EXTERNAL << MSD_SMI_FUNC_BIT) |
		(MSD_U16)((MSD_U16)MSD_SMI_WRITE << MSD_SMI_OP_BIT) | (MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	return MSD_OK;
}

/*****************************************************************************
* msdGetSMIExtPhyReg
*
* DESCRIPTION:
*       This function indirectly read external PHY register through SMI PHY command.
*
* INPUTS:
*       devAddr  - The PHY address to be read.
*       regAddr  - The register address to read.
*
* OUTPUTS:
*       data     - The storage where register date to be saved.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
MSD_STATUS Bonsai_msdGetSMIExtPhyReg
(
IN MSD_QD_DEV *dev,
IN  MSD_U8    devAddr,
IN  MSD_U8    regAddr,
OUT  MSD_U16   *data
)
{
	MSD_U32 timeOut;
	MSD_U16 smiReg;

	if (IS_SMI_MULTICHIP_SUPPORTED(dev) == 1)
	{
		return Bonsai_msdGetSMIExtPhyReg_MultiChip(dev, devAddr, regAddr, data);
	}

	/* first check that it is not busy */
	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_ERROR(("Read BONSAI_QD_REG_SMI_PHY_CMD Timed Out\n"));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);

	smiReg = (MSD_U16)MSD_SMI_BUSY | (MSD_U16)((MSD_U16)devAddr << MSD_SMI_DEV_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_EXTERNAL << MSD_SMI_FUNC_BIT) |
		(MSD_U16)((MSD_U16)MSD_SMI_READ_22 << MSD_SMI_OP_BIT) | (MSD_U16)((MSD_U16)regAddr << MSD_SMI_REG_ADDR_BIT) | (MSD_U16)((MSD_U16)MSD_SMI_CLAUSE22 << MSD_SMI_MODE_BIT);

	if (msdSetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, smiReg) != MSD_OK)
	{
		MSD_DBG_ERROR(("write BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
		return MSD_FAIL;
	}

	timeOut = MSD_SMI_ACCESS_LOOP; /* initialize the loop count */

	do
	{
		if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_CMD, &smiReg) != MSD_OK)
		{
			MSD_DBG_ERROR(("read BONSAI_QD_REG_SMI_PHY_CMD register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		if (timeOut < 1U)
		{
			MSD_DBG_ERROR(("read TOPAZ_QD_REG_SMI_PHY_DATA register returned: %s.\n", msdDisplayStatus(MSD_FAIL)));
			return MSD_FAIL;
		}
		timeOut -= 1U;

	} while ((smiReg & MSD_SMI_BUSY) != 0U);

	if (msdGetAnyReg(dev->devNum, BONSAI_GLOBAL2_DEV_ADDR, BONSAI_SMI_PHY_DATA, &smiReg) != MSD_OK)
	{
		MSD_DBG_INFO(("Reading Phy register Failed\n"));
		return MSD_FAIL;
	}
	*data = smiReg;

	MSD_DBG(("(LOG BONSAI SMI PHY C22 RW): devAddr 0x%02x, regAddr 0x%02x, \n",
		devAddr, regAddr));
	MSD_DBG(("data 0x%04x.\n", *data));

	return MSD_OK;
}

MSD_STATUS Bonsai_msdWritePagedExtPhyReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
IN  MSD_U16   data
)
{

	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIExtPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	retVal = Bonsai_msdSetSMIExtPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Bonsai_msdReadPagedExtPhyReg
(
IN  MSD_QD_DEV *dev,
IN  MSD_U8    portNum,
IN  MSD_U8    pageNum,
IN  MSD_U8    regAddr,
OUT MSD_U16   *data
)
{
	MSD_STATUS retVal;
	retVal = Bonsai_msdSetSMIExtPhyReg(dev, portNum, (MSD_U8)22, (MSD_U16)pageNum);
	if (retVal != MSD_OK)
	{
		return retVal;
	}
	retVal = Bonsai_msdGetSMIExtPhyReg(dev, portNum, regAddr, data);
	return retVal;
}

MSD_STATUS Bonsai_msdSetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16 regAddr,
IN MSD_U16 data
)
{
	MSD_STATUS retVal;
	MSD_U16 tmpData;

	/* Set MMD access control address is Address | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_ADDR | devAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
		return retVal;
	}

	/* Set MMD access Data is regAddr */
	tmpData = regAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
		return retVal;
	}

	/* Set MMD access control address is Data no inc | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_DATA_NO_INC | devAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
		return retVal;
	}

	/* Write MMD access Data */
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, data);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Write MMD access Data.\n"));
		return retVal;
	}

	return MSD_OK;
}

MSD_STATUS Bonsai_msdGetSMIExtPhyXMDIOReg
(
IN MSD_QD_DEV *dev,
IN MSD_U8  portNum,
IN MSD_U8  devAddr,
IN MSD_U16  regAddr,
OUT MSD_U16 *data
)
{
	MSD_STATUS retVal;
	MSD_U16 tmpData;

	/* Set MMD access control address is Address | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_ADDR | devAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access control address is Address | devAddr.\n"));
		return retVal;
	}

	/* Set MMD access Data is regAddr */
	tmpData = regAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access Data is regAddr.\n"));
		return retVal;
	}

	/* Set MMD access control address is Data no inc | devAddr */
	tmpData = (MSD_U16)MSD_PHY_MMD_DATA_NO_INC | devAddr;
	retVal = Bonsai_msdWritePagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)13, tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Set MMD access control address is Data no inc | devAddr.\n"));
		return retVal;
	}

	/* Read MMD access Data */
	retVal = Bonsai_msdReadPagedExtPhyReg(dev, portNum, (MSD_U8)0, (MSD_U8)14, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("ERROR to Read MMD access Data.\n"));
		return retVal;
	}

	*data = tmpData;

	return MSD_OK;
}
