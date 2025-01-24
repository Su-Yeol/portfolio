/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdQbv.c
*
* DESCRIPTION:
*    This sample will demonstrate how to configure Qbv.
*
* DEPENDENCIES:   None.
*
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Configure Qbv setting for egress port 3
*       Set cycle time to 1ms. Inside of each cycle, from 0 to 200us, 
*       open the gate for priority 3, from 200us to 300us, open the gate for 
*       priority 2, from 300us to 350us, open the gate for priority 1, from 350us 
*       to 1000us, close the gate for all priority.
*
* Note:
*		Assume the clock is pre-configured
*
* Comment:
*		Only support Amethyst, Peridot and Topaz
*
********************************************************************************/
static MSD_STATUS ptpTriggerGenClk(MSD_U8 devNum, MSD_U32 trigTime, MSD_U32 clkPeriod);

MSD_STATUS sample_msdQbv(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_U32 trigTime, clkPeriod;
	MSD_U16 data;
	MSD_LPORT portNum = 3;

	MSD_QD_DEV *dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/*Assume TAI clock is pre-configured, in this sample, Set clock period is 1ms*/
	if (dev->devName != MSD_TOPAZ)
	{
		status = msdGetPTPGlobalTime(devNum, &trigTime);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdGetPTPGlobalTime fail.\n"));
			return status;
		}
	}
	else
	{
		trigTime = 0x0;
	}

	if (dev->devName != MSD_AMETHYST && dev->devName != MSD_OAK && dev->devName != MSD_SPRUCE && dev->devName != MSD_BONSAIZ1 && dev->devName != MSD_FIR && dev->devName != MSD_BONSAI)
	{
		clkPeriod = 0x1E848; /*Peridot/Topaz PTP clock 125 MHz*/
	}
	else
	{
		clkPeriod = 0x3D090; /*Bonsai/BonsaiZ1/Fir/Oak/Spruce/Amethyst PTP clock 250 MHz*/
	}
	
	status = ptpTriggerGenClk(devNum, trigTime, clkPeriod);
	if (status != MSD_OK)
	{
		MSG_PRINT(("ptpTriggerGenClk fail.\n"));
		return status;
	}

	/*********************** below is Qbv entry setting ***************************/
	/*Write priority 3, open window time 200us*/
	data = 0x61A8;
	status = msdQbvWrite(devNum, portNum, 0x3, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	data = (1 << 15) | (0x0 << 8) | 0x8;
	status = msdQbvWrite(devNum, portNum, 0x2, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	/*Write other priority 2, open window time 100us*/
	data = 0x30D4;
	status = msdQbvWrite(devNum, portNum, 0x3, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	data = (1 << 15) | (0x1 << 8) | 0x4;
	status = msdQbvWrite(devNum, portNum, 0x2, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	/*Write other priority 1, open window time 50us*/
	data = 0x186A;
	status = msdQbvWrite(devNum, portNum, 0x3, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	data = (1 << 15) | (0x2 << 8) | 0x2;
	status = msdQbvWrite(devNum, portNum, 0x2, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	/*Write other window time closed*/
	data = 0x0;
	status = msdQbvWrite(devNum, portNum, 0x3, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	data = (1 << 15) | (0x3 << 8) | 0x0;
	status = msdQbvWrite(devNum, portNum, 0x2, data);
	if (status != MSD_OK)
	{
		MSG_PRINT(("msdQbvWrite fail.\n"));
		return status;
	}

	MSG_PRINT(("sample_msdQbv success.\n"));
	return MSD_OK;
}

/********************************************************************************
* ptpTriggerGenClk:
*
* Description:
*
*		Configure ptp trigger generation clock
*
* Inputs:
*		trigTime	- generate ptp trigger clock time, compare with PTP Global Time to generate clock
*		clkPeriod	- generate ptp trigger clock period, units is nano second
*
* Comment:
*		None.
*
********************************************************************************/
static MSD_STATUS ptpTriggerGenClk(MSD_U8 devNum, MSD_U32 trigTime, MSD_U32 clkPeriod)
{
	MSD_STATUS status;
	MSD_U16 data;

	/* Write TAI TrigGenAmt*/
	{
		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdSetAnyReg(devNum, 0x1C, 0x17, (clkPeriod & 0xffff));
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*Access global2 offset 0x16 AVB/TSN Command Register(write TAI offset 0x2 TrigGenAmt)*/
		data = (1 << 15) | (3 << 13) | (0x1E << 8) | (0x0 << 5) | 0x2;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}

		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}

		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdSetAnyReg(devNum, 0x1C, 0x17, ((clkPeriod & 0xffff0000) >> 16));
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*Access global2 offset 0x16 AVB/TSN Command Register(write TAI offset 0x2 TrigGenAmt)*/
		data = (1 << 15) | (3 << 13) | (0x1E << 8) | (0x0 << 5) | 0x3;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}

		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}
	}

	/* Write TAI TrigGenTime*/
	{
		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdSetAnyReg(devNum, 0x1C, 0x17, (trigTime & 0xffff));
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*Access global2 offset 0x16 AVB/TSN Command Register(write TAI offset 0x2 TrigGenAmt)*/
		data = (1 << 15) | (3 << 13) | (0x1E << 8) | (0x0 << 5) | 0x10;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}

		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}

		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdSetAnyReg(devNum, 0x1C, 0x17, ((trigTime & 0xffff0000) >> 16));
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*Access global2 offset 0x16 AVB/TSN Command Register(write TAI offset 0x2 TrigGenAmt)*/
		data = (1 << 15) | (3 << 13) | (0x1E << 8) | (0x0 << 5) | 0x11;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}

		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}
	}

	/* Write TrigMode and TrigGenReq*/
	{
		/*Access global2 offset 0x16 AVB/TSN Command Register(read TAI offset 0x0)*/
		data = (1 << 15) | (0x0 << 13) | (0x1E << 8) | (0x0 << 5) | 0x0;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}

		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdGetAnyReg(devNum, 0x1C, 0x17, &data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdGetAnyReg returned fail.\n"));
			return status;
		}

		data = (data & 0xFFFC) | 0x1;
		/*Access global2 offset 0x17 AVB/TSN Data Register*/
		status = msdSetAnyReg(devNum, 0x1C, 0x17, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}
		/*Access global2 offset 0x16 AVB/TSN Command Register(write TAI offset 0x0)*/
		data = (1 << 15) | (3 << 13) | (0x1E << 8) | (0x0 << 5) | 0x0;
		status = msdSetAnyReg(devNum, 0x1C, 0x16, data);
		if (status != MSD_OK)
		{
			MSG_PRINT(("msdSetAnyReg returned fail.\n"));
			return status;
		}

		/*wait AVB/TSN busy bit cleared*/
		data = 1;
		while (data)
		{
			status = msdGetAnyRegField(devNum, 0x1C, 0x16, 15, 1, &data);
			if (status != MSD_OK)
			{
				MSG_PRINT(("msdGetAnyRegField returned fail.\n"));
				return status;
			}
		}

	}

	return MSD_OK;
}