/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* Fir_msdQueueCtrlIntf.c
*
* DESCRIPTION:
*       API definitions for Switch Queue Control, including per-port setting & global setting
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <fir/include/api/Fir_msdQueueCtrl.h>


/*******************************************************************************
* Fir_gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            FIR_MSD_QUEUE_CFG_PORT_SCHEDULE
*            FIR_MSD_QUEUE_CFG_FILTER_Q_EN
*            FIR_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            FIR_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            FIR_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            FIR_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 FIR_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            FIR_MSD_QUEUE_CFG_IN_PORT_COUNT
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
MSD_STATUS Fir_gprtSetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
IN  MSD_U8             data
)
{
	return Fir_gprtSetQueueCtrl(dev, port, point, data);
}
/*******************************************************************************
* Fir_gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            FIR_MSD_QUEUE_CFG_PORT_SCHEDULE
*            FIR_MSD_QUEUE_CFG_FILTER_Q_EN
*            FIR_MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            FIR_MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            FIR_MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            FIR_MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            FIR_MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 FIR_MSD_QUEUE_CFG_FC_THRESHOLD_END
*            FIR_MSD_QUEUE_CFG_IN_PORT_COUNT
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
MSD_STATUS Fir_gprtGetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
OUT  MSD_U8            *data
)
{
	return Fir_gprtGetQueueCtrl(dev, port, point, data);
}
/*******************************************************************************
* Fir_gprtSetPortSched
*
* DESCRIPTION:
*        This routine sets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 FIR_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            FIR_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
*
* INPUTS:
*        port - the logical port number
*        mode - FIR_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Fir_gprtSetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_PORT_SCHED_MODE        mode
)
{
	return Fir_gprtSetPortSched(dev, port, (FIR_MSD_PORT_SCHED_MODE)mode);
}

/*******************************************************************************
* Fir_gprtGetPortSched
*
* DESCRIPTION:
*        This routine Fir_gets Port Scheduling Mode.
*        When usePortSched is enablied, this mode is used to select the Queue
*        controller's scheduling on the port as follows:
*
*			 FIR_MSD_PORT_SCHED_WEIGHTED_RRB_1 - use 33,25,17,12,6,3,2,1 weighted fair 
*											scheduling
*            FIR_MSD_PORT_SCHED_STRICT_PRI_1 - use a strict priority scheme
*
* INPUTS:
*        port - the logical port number
*
* OUTPUTS:
*        mode - FIR_MSD_PORT_SCHED_MODE enum type
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
MSD_STATUS Fir_gprtGetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_SCHED_MODE        *mode
)
{
	return Fir_gprtGetPortSched(dev, port, (FIR_MSD_PORT_SCHED_MODE*)mode);
}


/******************************************************************************
* gprtSetQoSWeight
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
*		port - the logical port number
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
MSD_STATUS Fir_gprtSetQoSWeightIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT		port,
IN  MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    FIR_MSD_QoS_WEIGHT fir_weight;
	if (weight->len > FIR_MAX_QOS_WEIGHTS)
	{
		return MSD_BAD_PARAM;
	}

    for (i = 0; i < weight->len; i++)
    {
        fir_weight.queue[i] = weight->queue[i];
    }
    fir_weight.len = weight->len;
    return Fir_gprtSetQoSWeight(dev, port, &fir_weight);
}

/******************************************************************************
* gprtGetQoSWeight
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
*       port - the logical port number
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
MSD_STATUS Fir_gprtGetQoSWeightIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT			port,
OUT MSD_QoS_WEIGHT    *weight
)
{
    MSD_U32 i = 0;
    MSD_STATUS status;
    FIR_MSD_QoS_WEIGHT fir_weight;

	status = Fir_gprtGetQoSWeight(dev, port, &fir_weight);
	if (status != MSD_OK)
	{
		return status;
	}

	if (fir_weight.len > FIR_MAX_QOS_WEIGHTS)
	{
		return MSD_BAD_PARAM;
	}

    for (i = 0; i < fir_weight.len; i++)
    {
        weight->queue[i] = fir_weight.queue[i];
    }
    weight->len = fir_weight.len;

    return status;
}
