/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Fir_msdQueueCtrl.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Queue Control functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Fir_msdQueueCtrl_h
#define Fir_msdQueueCtrl_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported Port Control Types			 			                                   */
/****************************************************************************/

#define FIR_MAX_QOS_WEIGHTS  128

/*
*  typedef: enum FIR_MSD_PORT_SCHED_MODE
*
*  Description: Enumeration of port scheduling mode
*
*  Fields:
*
*		   FIR_WRR_PRI7_6_5_4_3_2_1_0 - use 33,25,17,12,6,3,2,1 weighted fair
*										scheduling
*	       FIR_WRR_PRI6_5_4_3_2_1_0 - use a strict for priority 7 and weighted
*										round robin for priorities 6,5,4,3,2,1 and 0
*		   FIR_WRR_PRI5_4_3_2_1_0 - use a strict for priority 7,6 and weighted
*										round robin for priorities 5,4,3,2,1 and 0
*		   FIR_WRR_PRI4_3_2_1_0 - use a strict for priority 7,6,5 and weighted
*										round robin for priorities 4,3,2,1 and 0
*		   FIR_WRR_PRI3_2_1_0 - use a strict for priority 7,6,5,4 and
*										weighted round robin for priorities 3,2,1 and 0
*		   FIR_WRR_PRI2_1_0 - use a strict for priority 7,6,5,4,3 and
*										weighted round robin for priorities 2,1 and 0
*		   FIR_WRR_PRI1_0 - use a strict for priority 7,6,5,4,3,2
*										and weighted round robin for priorities 1 and 0
*		   FIR_STRICT_PRI_ALL - use a strict priority scheme for all priorities
*
*  Comment:
*/
typedef enum
{
    FIR_WRR_PRI7_6_5_4_3_2_1_0 = 0x0,
    FIR_WRR_PRI6_5_4_3_2_1_0,
    FIR_WRR_PRI5_4_3_2_1_0,
    FIR_WRR_PRI4_3_2_1_0,
    FIR_WRR_PRI3_2_1_0,
    FIR_WRR_PRI2_1_0,
    FIR_WRR_PRI1_0,
    FIR_STRICT_PRI_ALL
} FIR_MSD_PORT_SCHED_MODE;

/*
* Typedef: struct MSD_QoS_WEIGHT
*
* Description: This structure is used for Programmable Round Robin Weights.
*
* Fields:
*      len    - length of the valid queue data
*      queue  - upto 128 queue data
*/
typedef struct
{
    MSD_U32        len;
    MSD_U8         queue[FIR_MAX_QOS_WEIGHTS];
}FIR_MSD_QoS_WEIGHT;


/****************************************************************************/
/* Exported Port Control Functions		 			                                   */
/****************************************************************************/

/*******************************************************************************
* gprtSetQueueCtrl
*
* DESCRIPTION:
*        Set Queue control data to the Queue Control register. 
*        The register(pointer) of Queue control are:
*            MSD_QUEUE_CFG_PORT_SCHEDULE
*            MSD_QUEUE_CFG_FILTER_Q_EN
*            MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 MSD_QUEUE_CFG_FC_THRESHOLD_END
*            MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        port  - the logical port number
*        point - Pointer to the Queue control register.
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    IN  MSD_U8             data
);
MSD_STATUS Fir_gprtSetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
IN  MSD_U8             data
);
/*******************************************************************************
* gprtGetQueueCtrl
*
* DESCRIPTION:
*        Get Queue control data from the Queue Control register. 
*        The register(pointer) of Queue control are:
*            MSD_QUEUE_CFG_PORT_SCHEDULE
*            MSD_QUEUE_CFG_FILTER_Q_EN
*            MSD_QUEUE_CFG_PORT_SCRATCH_REG,     (acratch0 - 1 inc: 1)
*            MSD_QUEUE_CFG_H_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_Y_Q_LIMIT_REG         (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_BEGIN   (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_PFC_THRESHOLD_END     (Q0 - Q7 inc: 2)
*            MSD_QUEUE_CFG_IN_Q_COUNT            (Q0 - Q7 inc: 1)
*            MSD_QUEUE_CFG_FC_THRESHOLD_BEGIN	
*			 MSD_QUEUE_CFG_FC_THRESHOLD_END
*            MSD_QUEUE_CFG_IN_PORT_COUNT
*         
* INPUTS:
*        port  - the logical port number
*        point - Pointer to the Queue control register.
*
* OUTPUTS:
*        data  - Queue Control data written to the register 
*                pointed to by the point above.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetQueueCtrl
(
    IN  MSD_QD_DEV         *dev,
    IN  MSD_LPORT          port,
    IN  MSD_U8             point,
    OUT  MSD_U8            *data
);
MSD_STATUS Fir_gprtGetQueueCtrlIntf
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT          port,
IN  MSD_U8             point,
OUT  MSD_U8            *data
);
/*******************************************************************************
* gprtSetPortSched
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS: 
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtSetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    IN  FIR_MSD_PORT_SCHED_MODE        mode
);
MSD_STATUS Fir_gprtSetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
IN  MSD_PORT_SCHED_MODE        mode
);
/*******************************************************************************
* gprtGetPortSched
*
* DESCRIPTION:
*        This routine gets Port Scheduling Mode.
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
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS: 
*		None.
*
*******************************************************************************/
MSD_STATUS Fir_gprtGetPortSched
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     port,
    OUT FIR_MSD_PORT_SCHED_MODE        *mode
);
MSD_STATUS Fir_gprtGetPortSchedIntf
(
IN  MSD_QD_DEV    *dev,
IN  MSD_LPORT     port,
OUT MSD_PORT_SCHED_MODE        *mode
);

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
*       port - the logical port number
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
);
MSD_STATUS Fir_gprtSetQoSWeight
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT		port,
IN  FIR_MSD_QoS_WEIGHT    *weight
);

/******************************************************************************
* gprtGetQoSWeight
*
* DESCRIPTION:
*       Programmable Round Robin Weights.
*        Each port has 8 output Queues. Queue 7 has the highest priority
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
IN  MSD_LPORT		    port,
OUT MSD_QoS_WEIGHT    *weight
);
MSD_STATUS Fir_gprtGetQoSWeight
(
IN  MSD_QD_DEV         *dev,
IN  MSD_LPORT		    port,
OUT FIR_MSD_QoS_WEIGHT    *weight
);

#ifdef __cplusplus
}
#endif

#endif /* __Fir_msdQueueCtrl_h */
