/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Oak_msdQueueCtrl.c
*
* DESCRIPTION:
*       API definitions for Switch Queue Control, including per-port setting & global setting
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <oak/include/api/Oak_msdQueueCtrl.h>
#include <oak/include/api/Oak_msdApiInternal.h>
#include <oak/include/driver/Oak_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>


/*******************************************************************************
* Oak_gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            OAK_MSD_QUEUE_CFG_PORT_SCHEDULE
*            OAK_MSD_QUEUE_CFG_FILTER_Q_EN
*            OAK_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            OAK_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            OAK_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            OAK_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 OAK_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            OAK_MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gprtSetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    IN  MSD_U8             data
)
{
    MSD_STATUS      retVal;         /* Functions return value.      */
    MSD_U8          hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16         tmpData;
	MSD_U16         count = (MSD_U16)0x10;

    MSD_DBG_INFO(("Oak_gprtSetQueueCtrl Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = OAK_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    do {
		retVal = msdGetAnyReg(dev->devNum, phyAddr, OAK_Q_CONTROL, &tmpData);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read port Queue Control register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == (MSD_U16)0)
		{
			MSD_DBG_ERROR(("Wait port Queue Control register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((tmpData & (MSD_U16)0x8000) != 0U);

	tmpData = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)(((MSD_U16)point & (MSD_U16)0x7f) << 8) | data);

	retVal = msdSetAnyReg(dev->devNum, phyAddr, OAK_Q_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
		 MSD_DBG_ERROR(("Set port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		 msdSemGive(dev->devNum, dev->tblRegsSem);
		 return retVal;
	}

	msdSemGive(dev->devNum, dev->tblRegsSem);

	MSD_DBG_INFO(("Oak_gprtSetQueueCtrl Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Oak_gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            OAK_MSD_QUEUE_CFG_PORT_SCHEDULE
*            OAK_MSD_QUEUE_CFG_FILTER_Q_EN
*            OAK_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            OAK_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            OAK_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            OAK_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            OAK_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 OAK_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            OAK_MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gprtGetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
)
{
    MSD_STATUS      retVal;         /* Functions return value.      */
    MSD_U8          hwPort;         /* the physical port number     */
	MSD_U8			phyAddr;
    MSD_U16         tmpData;
	MSD_U16         count = (MSD_U16)0x10;

    MSD_DBG_INFO(("Oak_gprtSetQueueCtrl Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	phyAddr = OAK_MSD_CALC_SMI_DEV_ADDR(dev, hwPort);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		return MSD_BAD_PARAM;
	}

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    do {
		retVal = msdGetAnyReg(dev->devNum, phyAddr, OAK_Q_CONTROL, &tmpData);
        if(retVal != MSD_OK)
        {
			MSD_DBG_ERROR(("Read port Queue Control register busy bit returned: %s.\n", msdDisplayStatus(retVal)));
			msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
		if (count == (MSD_U16)0)
		{
			MSD_DBG_ERROR(("Wait port Queue Control register busy bit time out.\n"));
			msdSemGive(dev->devNum, dev->tblRegsSem);
			return MSD_FAIL;
		}
		count -= 1U;

	} while ((tmpData & (MSD_U16)0x8000) != 0U);

	tmpData = (MSD_U16)(((MSD_U16)point & (MSD_U8)0x7f) << 8);

	retVal = msdSetAnyReg(dev->devNum, phyAddr, OAK_Q_CONTROL, tmpData);
	if(retVal != MSD_OK)
	{
		 MSD_DBG_ERROR(("Set port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		 msdSemGive(dev->devNum, dev->tblRegsSem);
		 return retVal;
	}

	retVal = msdGetAnyReg(dev->devNum, phyAddr, OAK_Q_CONTROL, &tmpData);
    if(retVal != MSD_OK)
    {
		MSD_DBG_ERROR(("Read port Queue Control register returned: %s.\n", msdDisplayStatus(retVal)));
		msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

	*data = (MSD_U8)(tmpData & (MSD_U8)0xff);

    msdSemGive(dev->devNum, dev->tblRegsSem);

	MSD_DBG_INFO(("Oak_gprtSetQueueCtrl Exit.\n"));
    return retVal;
}

/*******************************************************************************
* Oak_gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 OAK_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            OAK_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - OAK_MSD_PORT_SCHED_MODE enum type
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gprtSetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  OAK_MSD_PORT_SCHED_MODE        mode
)
{
    MSD_U16          data = 0;           
    MSD_STATUS       retVal = MSD_OK;    /* Functions return value.      */
    MSD_U8           hwPort;             /* the physical port number     */

    MSD_DBG_INFO(("Oak_gprtSetPortSched Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		switch (mode)
		{
		case OAK_WRR_PRI7_6_5_4_3_2_1_0:
			data = 0;
			break;
		case OAK_WRR_PRI6_5_4_3_2_1_0:
			data = (MSD_U16)1;
			break;
		case OAK_WRR_PRI5_4_3_2_1_0:
			data = (MSD_U16)2;
			break;
		case OAK_WRR_PRI4_3_2_1_0:
			data = (MSD_U16)3;
			break;
		case OAK_WRR_PRI3_2_1_0:
			data = (MSD_U16)4;
			break;
		case OAK_WRR_PRI2_1_0:
			data = (MSD_U16)5;
			break;
		case OAK_WRR_PRI1_0:
			data = (MSD_U16)6;
			break;
		case OAK_STRICT_PRI_ALL:
			data = (MSD_U16)7;
			break;
		default:
			MSD_DBG_ERROR(("Bad Mode: %u.\n", mode));
			retVal = MSD_BAD_PARAM;
			break;
		}

		if (retVal != MSD_BAD_PARAM)
		{
			retVal = Oak_gprtSetQueueCtrl(dev, port, (MSD_U8)0, (MSD_U8)data);
			if (retVal != MSD_OK)
			{
				MSD_DBG_INFO(("Oak_gprtSetQueueCtrl returned: %s.\n", msdDisplayStatus(retVal)));
			}
		}
	}

    MSD_DBG_INFO(("Oak_gprtSetPortSched Exit.\n"));	
    return retVal;
}

/*******************************************************************************
* Oak_gprtGetPortSched
*
* DESCRIPTION:
*        This routine Oak_gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 OAK_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            OAK_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - OAK_MSD_PORT_SCHED_MODE enum type
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Oak_gprtGetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT OAK_MSD_PORT_SCHED_MODE        *mode
)
{      
    MSD_STATUS       retVal;         /* Functions return value.      */
    MSD_U8           hwPort;         /* the physical port number     */
	MSD_U8           datac;

    MSD_DBG_INFO(("Oak_gprtGetPortSched Called.\n"));

    /* translate LPORT to hardware port */
	hwPort = MSD_LPORT_2_PORT(port);
	if(hwPort == MSD_INVALID_PORT)
	{
		MSD_DBG_ERROR(("Bad Port: %u.\n", port));
		retVal = MSD_BAD_PARAM;
	}
	else
	{
		retVal = Oak_gprtGetQueueCtrl(dev, port, (MSD_U8)0, (MSD_U8 *)&datac);
		if (retVal != MSD_OK)
		{
			MSD_DBG_INFO(("Oak_gprtGetQueueCtrl returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			switch (datac)
			{
			case 0x0:
				*mode = OAK_WRR_PRI7_6_5_4_3_2_1_0;
				break;
			case 0x1U:
				*mode = OAK_WRR_PRI6_5_4_3_2_1_0;
				break;
			case 0x2U:
				*mode = OAK_WRR_PRI5_4_3_2_1_0;
				break;
			case 0x3U:
				*mode = OAK_WRR_PRI4_3_2_1_0;
				break;
			case 0x4U:
				*mode = OAK_WRR_PRI3_2_1_0;
				break;
			case 0x5U:
				*mode = OAK_WRR_PRI2_1_0;
				break;
			case 0x6U:
				*mode = OAK_WRR_PRI1_0;
				break;
			case 0x7U:
				*mode = OAK_STRICT_PRI_ALL;
				break;
			default:
				MSD_DBG_ERROR(("Bad Mode: %u.\n", datac));
				retVal = MSD_BAD_PARAM;
				break;
			}
		}
	}
	
	MSD_DBG_INFO(("Oak_gprtGetPortSched Exit.\n"));
    return retVal;
}

/******************************************************************************
* gsysSetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4/8 output Queues. Queue 3/7 has the highest priority
*        Queue 0 has the lowest priority. When a scheduling mode of port is
*        configured as Weighted Round Robin queuing mode, the access sequece of
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        (Queue is 7,6,5,7,1,6,7,4 by default. That is after 6390.)
*        This sequence can be configured with this API.
*
* INPUTS:
*       weight - access sequence of the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
******************************************************************************/
MSD_STATUS Oak_gsysSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  OAK_MSD_QoS_WEIGHT    *weight
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16       data;
    MSD_U32       len, i;
    MSD_U32 length_loc, entry_num;

    MSD_DBG_INFO(("gsysSetQoSWeight Called.\n"));

	length_loc = (MSD_U32)0x40;
	entry_num = (MSD_U32)2;

	if (weight->len > (MSD_U32)128)
    {
        MSD_DBG_INFO(("MSD_BAD_PARAM\n"));
        return MSD_BAD_PARAM;
    }

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    len = weight->len / entry_num;

    /* program QoS Weight Table, 4/2 sequences at a time */
    for (i = 0; i<len; i++)
    {
        /* Wait until the QoS Weight Table is ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->tblRegsSem);
                return retVal;
            }
        }


		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)i << 8) |
			(MSD_U16)(weight->queue[i * 2U] & (MSD_U16)0x7) |
			(MSD_U16)((weight->queue[(i * 2U) + 1U] & (MSD_U16)0x7) << 4));

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

    /* program remaining sequences if any */
    i = weight->len % entry_num;
    if (i != 0U)
    {
        /* Wait until the QoS Weight Table is ready. */
		data = (MSD_U16)1;
		while (data == (MSD_U16)1)
        {
			retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, (MSD_U8)15, (MSD_U8)1, &data);
            if (retVal != MSD_OK)
            {
                msdSemGive(dev->devNum, dev->tblRegsSem);
                return retVal;
            }
        }

		data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)len << 8));

		data |= (MSD_U16)((weight->queue[(len * 2U) + 0U] & (MSD_U16)0x7) << 0);

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

    /* Write the lengh of the sequence */

    /* Wait until the QoS Weight Table is ready. */
	data = (MSD_U16)1;
	while (data == (MSD_U16)1)
    {
		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

	data = (MSD_U16)((MSD_U16)((MSD_U16)1 << 15) | (MSD_U16)((MSD_U16)length_loc << 8) | (MSD_U16)weight->len);

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);

    msdSemGive(dev->devNum, dev->tblRegsSem);

    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed.\n"));
        return retVal;
    }

    return MSD_OK;
}

/******************************************************************************
* gsysGetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 4/8 output Queues. Queue 3/7 has the highest priority
*        Queue 0 has the lowest priority. When a scheduling mode of port is
*        configured as Weighted Round Robin queuing mode, the access sequece of
*        Queue is 3,2,3,1,3,2,3,0,3,2,3,1,3,2,3 by default.
*        (Queue is 7,6,5,7,1,6,7,4 by default. That is after 6390.)
*        This routine retrieves the access sequence of the Queue.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       weight - access sequence of the queue
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*        MSD_NOT_SUPPORTED - if current device does not support this feature.
*
* COMMENTS:
*       None
*
******************************************************************************/
MSD_STATUS Oak_gsysGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
OUT OAK_MSD_QoS_WEIGHT    *weight
)
{
    MSD_STATUS    retVal;         /* Functions return value.      */
    MSD_U16        data, i;
    MSD_U32        len;
    MSD_U16 length_loc, entry_num;

    MSD_DBG_INFO(("gsysGetQoSWeight Called.\n"));

	length_loc = (MSD_U16)0x40;
	entry_num = (MSD_U16)2;

    msdSemTake(dev->devNum, dev->tblRegsSem, OS_WAIT_FOREVER);

    /* Read the lengh of the sequence */
    /* Wait until the QoS Weight Table is ready. */
    data = 1U;
    while (data == 1U) 
	{

		retVal = msdGetAnyRegField(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, (MSD_U8)15, (MSD_U8)1, &data);
        if (retVal != MSD_OK)
        {
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }
    }

	data = (MSD_U16)(length_loc << 8);

	retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed.\n"));
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

	retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, &data);
    if (retVal != MSD_OK)
    {
        MSD_DBG_INFO(("Failed.\n"));
        msdSemGive(dev->devNum, dev->tblRegsSem);
        return retVal;
    }

	weight->len = (MSD_U32)(data & (MSD_U32)0xFF);

	len = weight->len / (MSD_U32)entry_num;

    /* read QoS Weight Table, 4/2 sequences at a time */
    for (i = 0; i<len; i++)
    {
        data = i << 8;

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, &data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

		weight->queue[i * 2U] = (MSD_U8)((MSD_U8)data & (MSD_U8)0x7);
		weight->queue[(i * 2U) + 1U] = (MSD_U8)(((MSD_U8)data >> 4) & (MSD_U8)0x7);
    }

    /* read remaining sequences if any */
    i = (MSD_U16)(weight->len % entry_num);
    if (i != 0U)
    {
        data = (MSD_U16)(len << 8);

		retVal = msdSetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

		retVal = msdGetAnyReg(dev->devNum, OAK_GLOBAL2_DEV_ADDR, OAK_QOS_WEIGHT, &data);
        if (retVal != MSD_OK)
        {
            MSD_DBG_INFO(("Failed.\n"));
            msdSemGive(dev->devNum, dev->tblRegsSem);
            return retVal;
        }

		weight->queue[len * 2U] = (MSD_U8)data & (MSD_U8)0x7;
    }

    msdSemGive(dev->devNum, dev->tblRegsSem);
    return MSD_OK;
}
