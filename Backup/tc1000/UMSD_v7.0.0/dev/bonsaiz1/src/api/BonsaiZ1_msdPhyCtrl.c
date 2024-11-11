/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* BonsaiZ1_msdPhyCtrl.c
*
* DESCRIPTION:
*       API implementation for switch internal Copper PHY control.
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <bonsaiz1/include/api/BonsaiZ1_msdPhyCtrl.h>
#include <bonsaiz1/include/driver/BonsaiZ1_msdHwAccess.h>
#include <bonsaiz1/include/driver/BonsaiZ1_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>


/*******************************************************************************
* BonsaiZ1_serdesReset
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
*       BonsaiZ1: Port9, Port10 and Port11
*                 Port operation register, Device4 Register 0xF002
*                 10G-BaseR PCS Control, Devcie4 Register 0x2000
*                 1000BaseX Control register, Devcie4 Register 0x1000
*
*
*******************************************************************************/
MSD_STATUS BonsaiZ1_serdesReset
(
    IN MSD_QD_DEV *dev,
    IN MSD_LPORT  port
)
{
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
    MSD_U16          data;

    MSD_DBG_INFO(("BonsaiZ1_serdesReset Called.\n"));

    /* translate LPORT to hardware port */
    hwPort = MSD_LPORT_2_SERDES(port);
    if (hwPort == (MSD_U8)MSD_INVALID_PHY)
    {
        MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
    }
	else
	{
		msdSemTake(dev->devNum, dev->phyRegsSem, OS_WAIT_FOREVER);

		retVal = BonsaiZ1_msdGetSMIC45PhyReg(dev, (MSD_U8)4, hwPort, (MSD_U16)0x2000, &data);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("read 1000BASE-X/SGMI Control register error %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			data = data | (MSD_U16)0x8000;

			retVal = BonsaiZ1_msdSetSMIC45PhyReg(dev, (MSD_U8)4, hwPort, (MSD_U16)0x2000, data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("write 1000BASE-X/SGMI Control register error %s.\n", msdDisplayStatus(retVal)));
			}
		}

		msdSemGive(dev->devNum, dev->phyRegsSem);
	}

    MSD_DBG_INFO(("BonsaiZ1_serdesReset Exit.\n"));
    return retVal;
}