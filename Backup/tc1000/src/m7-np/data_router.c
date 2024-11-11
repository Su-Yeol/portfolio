/*
***************************************************************************************************
*
*   FileName : data_router.c
*
*   Copyright (c) Telechips Inc.
*
*   Description :
*
*
***************************************************************************************************
*
*   TCC Version 1.0
*
*   This source code contains confidential information of Telechips.
*
*   Any unauthorized use without a written permission of Telechips including not limited to
*   re-distribution in source or binary form is strictly prohibited.
*
*   This source code is provided "AS IS" and nothing contained in this source code shall constitute
*   any express or implied warranty of any kind, including without limitation, any warranty of
*   merchantability, fitness for a particular purpose or non-infringement of any patent, copyright
*   or other third party intellectual property right. No warranty is made, express or implied,
*   regarding the information's accuracy,completeness, or performance.
*
*   In no event shall Telechips be liable for any claim, damages or other liability arising from,
*   out of or in connection with this source code or the use in the source code.
*
*   This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement between
*   Telechips and Company.
*   This source code is provided "AS IS" and nothing contained in this source code shall constitute
*   any express or implied warranty of any kind, including without limitation, any warranty
*   (of merchantability, fitness for a particular purpose or non-infringement of any patent,
*   copyright or other third party intellectual property right. No warranty is made, express or
*   implied, regarding the information's accuracy, completeness, or performance.
*   In no event shall Telechips be liable for any claim, damages or other liability arising from,
*   out of or in connection with this source code or the use in the source code.
*   This source code is provided subject to the terms of a Mutual Non-Disclosure Agreement
*   between Telechips and Company.
*
***************************************************************************************************
*/

/**************************************************************************************************
 *                                           INCLUDE FILES
 **************************************************************************************************/
#include <errno.h>
#include <string.h>

#include "app_cfg.h"
#include "data_router.h"
#include "route_cfg.h"
#include "ip_control.h"
#include "lpa_std.h"
// #include "ipc.h" // 241106 sy.kim

/**************************************************************************************************
 *                                            DEFINITIONS
 **************************************************************************************************/

#define LPA_IPC_WAIT_FLAG (0x00000001UL)

// 241106 sy.kim
#define TCC_IPC_CMD_AP_TEST                 (0xffu)		//cmd1
#define TCC_IPC_CMD_AP_SEND					(0x0fffu)   //cmd2
#define IPC_A65_TEST 1

/**************************************************************************************************
 *                                          global VARIABLES
 **************************************************************************************************/
/* for TSNC latency */
/*
uint32 tick_canc_tx;

uint32 tick_tpa_rx;
uint32 tick_lpa_enqueue;

uint32 tick_lpa_rx;
uint32 tick_tpa_write;
*/

/***** Task Quehandle ***********/
uint32 gTpaTxQueueHandle = 0U;
uint32 gLpaTxQueueHandle = 0U;

/**************************************************************************************************
 *                                          LOCAL VARIABLES
 **************************************************************************************************/
/*misra_c_2012_rule_8_4_violation:	Object definition does not have a visible prototype*/
static uint8 rx_sourcePort;
static uint32 rx_extCan_id;
static uint8 rx_ide; // Extended ID or Normal ID
static uint8 rx_fdf; // CAN FD or Classic CAN
static uint8 rx_rtr; // remote frame
uint16 proc_dataLen = 8u;
static uint8 rx_protocol_type; // CAN or LIN
static uint32 rx_timeStamp_us_H;
static uint32 rx_timeStamp_us_L;
static uint16 rx_timeStamp_ns;
static uint16 rx_can_id;
static uint16 rx_lin_id;

static MM_RESPONSE mrs_data;
static LpaRegCallback fLpaRegCB = NULL;
static LpaDataCallback fLpaDataCB = NULL;
static LpaTsCallback fLpaTsCB = NULL;
static LpaStatusCallback fLpaStatusCB = NULL;

/* TSNC */
static CANtoEthCallback fCANtoEthCB = NULL;

/* CANC */
static CANCnvtExpandCallback fCANCnvtExpandCB = NULL;

static const uint8 CanDlcLen[7U] = {12U, 16U, 20U, 24U, 32U, 48U, 64U};

/* Tx, Rx Task variables */
static uint8 rx_frame_type; // Timestamp or data

static uint8 rxRequestBuffer[32];
static uint8 rxResponseBuffer[LPA_TO_HOST_SIZE];
/*misra_c_2012_rule_2_2_violation:	variable "ipc_rxResponseBuffer" was declared but never referenced*/
// static uint8 ipc_rxResponseBuffer[264];
static TaskHandle_t RX_Task_Handle;
static uint8 rx_task_state = 0U; /* 0:idle, 1: normal operation */

static uint16 gSN;
/* cert_exp37_c_violation:	Calling function "IPC_SendPacket(IPCSvcCh_t, uint16, uint16, uint8 const *, uint16)" with the argument "gM7_N", which has an incompatible type "uint8" instead of "IPCSvcCh_t".*/
static IPCSvcCh_t gM7_N;

static uint8 txRequestBuffer[128];

static MM_READ mrr_data;

static uint8 Rx_Data[64];

static uint32 LpaRouteMutexId;
static uint32 LpaIpcRxEventId;
/*cert_exp37_c_violation:	Calling function "frm_findRouteTbl(uint8, uint32, uint8)" with the argument "tbl_cnt", which has an incompatible type "uint32" instead of "uint8".*/
/*misra_c_2012_rule_8_4_violation:	Object definition does not have a visible prototype*/
static uint32 tbl_cnt = 0u;
static routeTbl_t route_tbl[ROUTE_NUM];

lpaIpcMessage_t ipc_msg;
uint8 ipc_cnt_flag = 0U;

uint32 ipc_cnt = 0U;

static uint16 slave_ack = 0U;
static uint16 master_ack = 0U;

static uint8 txReqBuff_cnvt[128];
#if 0
#define TSNC_NUM 4
const tsncTbl_t tsnc_cfg[TSNC_NUM] = {
    {2, 0x700, TSNC},
    {7, 0x701, CANC},
    {2, 0x20, IPC},
    {6, 0x30, TSNC}
};
#endif


/**************************************************************************************************
 *                                        FUNCTION PROTOTYPES
 **************************************************************************************************/
// Rx API
static void parse_mm_response(const uint8 *buffer);
static void parse_proc_frame(const uint8 *buffer, uint16 data_length);
static uint8 parse_status_frame(const uint8 *buffer);

// Tx API
static void ClassicCAN_Send(uint8 PortNum, uint32 CAN_ID, uint8 Data_Len, const uint8 *pData, uint8 ExtEnable);
static void CANFD_Send(uint8 PortNum, uint32 CAN_ID, uint8 Data_Len, const uint8 *pData, uint8 ExtEnable);
static void LIN_Send(uint8 PortNum, uint32 LIN_ID, uint8 OpMode, uint8 Data_Len, const uint8 *pData);
static int32a lengthValidCheck(LPA_FRAME_TYPE type, uint8 data_len);

// 241106 sy.kim
static void NP_IpcCbFunc_A65_Test(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);

#if (IPC_EN == 1u)
#if (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u)
static void NP_IpcCbFunc_CM7_0_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_1_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_2_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
/*declaration is incompatible with previous "ipc_receive_msg_M7" (declared at line 1170)*/
static void ipc_receive_msg_M7(IPCSvcCh_t tx_core, uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
#endif
static void NP_IpcCbFunc_CM7_0_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_1_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_2_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void ipc_receive_ip_ctrl_M7(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);

/*misra_c_2012_rule_8_3_violation:	Declaration uses a different parameter name than "void NP_IpcCbFunc_CM7_2_IpStat(uint16, uint8 *, uint16)".*/
static void NP_IpcCbFunc_CM7_0_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_1_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void NP_IpcCbFunc_CM7_2_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
static void ipc_receive_show_status_M7(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength);
#endif

/* Tx, Rx Task functions */
// static void LPA_Rx_Task(void *pArg);
// static void LPA_Tx_Task(void *pArg);

/**************************************************************************************************
 *                                             FUNCTIONS
 **************************************************************************************************/
/* Rx API */
// void LPA_Rx_Init(void)
//{
#if 0
	frm_lpa_rx_setRegCB(print_reg_data_info);
	frm_lpa_rx_setDataCB(print_data_frame_info);
	frm_lpa_rx_setTsCB(print_ts_frame_info);
	frm_lpa_rx_setStatusCB(print_status_frame_info);
#endif
//}

void frm_lpa_rx_setRegCB(LpaRegCallback cbFunc)
{
	fLpaRegCB = cbFunc;
}

void frm_lpa_rx_setDataCB(LpaDataCallback cbFunc)
{
	fLpaDataCB = cbFunc;
}

void frm_lpa_rx_setTsCB(LpaTsCallback cbFunc)
{
	fLpaTsCB = cbFunc;
}

void frm_lpa_rx_setStatusCB(LpaStatusCallback cbFunc)
{
	fLpaStatusCB = cbFunc;
}

void frm_tsnc_setCANtoEthCB(CANtoEthCallback cbFunc)
{
	fCANtoEthCB = cbFunc;
}

void frm_canc_setExpandCB(CANCnvtExpandCallback cbFunc)
{
	fCANCnvtExpandCB = cbFunc;
}

#if (IPC_EN == 1u)
static void LPA_IPC_rxIrq_eventCreate(void)
{
	(void)SAL_EventCreate(&LpaIpcRxEventId, (const uint8 *)"LPA IPC RxIrq wait flag group", 0);
}

static int32 LPA_IPC_rxIrq_waitEvent(void)
{
	int32 ret;
	SALRetCode_t err;
	uint32 retFlag;

	err = SAL_EventGet(LpaIpcRxEventId, LPA_IPC_WAIT_FLAG, 0UL, ((uint32)SAL_EVENT_OPT_SET_ANY | (uint32)SAL_OPT_BLOCKING), &retFlag);

	if ((err == SAL_RET_SUCCESS))
	{
		if (retFlag == 0U)
		{
			ret = -1; // timeout
		}
		else
		{
			ret = 0; // success
		}
	}
	else
	{
		ret = -1;
	}

	return ret;
}

static void LPA_IPC_rxIrq_clearEvent(void)
{
	(void)SAL_EventSet(LpaIpcRxEventId, LPA_IPC_WAIT_FLAG, SAL_EVENT_OPT_CLR_ALL);
}

static void LPA_IPC_rxIrq_wakeUp(void)
{
	(void)SAL_EventSet(LpaIpcRxEventId, LPA_IPC_WAIT_FLAG, SAL_EVENT_OPT_FLAG_SET);
}
#endif

static void parse_mm_response(const uint8 *buffer)
{
	LPA_REG_DATA reg;

	SAL_MemCopy(&mrs_data, buffer, sizeof(MM_RESPONSE));
	if (mrs_data.type == MM_RESPONSE_TYPE)
	{
		reg.addr = (mrs_data.data.RD_ADDR0 & 0x00FFu) << 8U;
		reg.addr |= ((mrs_data.data.RD_ADDR0 & 0xFF00u) >> 8U);
		reg.data[0] = lpa_swap32(mrs_data.data.data0);
		reg.data[1] = lpa_swap32(mrs_data.data.data1);
		reg.data[2] = lpa_swap32(mrs_data.data.data2);
		reg.data[3] = lpa_swap32(mrs_data.data.data3);

		if (fLpaRegCB != NULL)
		{
			fLpaRegCB(&reg);
		}
	}
	else
	{
		mcu_printf("[%s] no mm_response\n", __func__);
	}
}
/*misra_c_2012_rule_8_8_violation:	missing static storage modifier for "parse_proc_frame" which has internal linkage*/
static void parse_proc_frame(const uint8 *buffer, uint16 data_length)
{
	uint16 i;
	uint8 dst = 0u;
	RX_DATA_FRAME data_frame;
	RX_TS_FRAME ts_frame;
	CNVT_CAN_DATA cnvt_data;
	uint32 route_id;

	rx_frame_type = buffer[0] & 0x01U;
	rx_sourcePort = ((buffer[0] & 0xFEU) >> 1U) + ((buffer[1] & 0x01U) << 7U);

	rx_timeStamp_ns = (((uint16)buffer[1] & 0xFEU) >> 1U);
	rx_timeStamp_ns |= (((uint16)buffer[2] & 0x01U) << 7U);
	rx_timeStamp_ns *= 10U;

	rx_timeStamp_us_L = ((uint32)buffer[2] >> 1U);
	rx_timeStamp_us_L |= ((uint32)buffer[3] << 7U);
	rx_timeStamp_us_L |= ((uint32)buffer[4] << 15U);
	rx_timeStamp_us_L |= ((uint32)buffer[5] << 23U);
	rx_timeStamp_us_L |= (((uint32)buffer[6] & 0x01U) << 31U);
	rx_timeStamp_us_H = ((uint32)buffer[6] >> 1U);
	rx_timeStamp_us_H |= ((uint32)buffer[7] << 7U);
	rx_timeStamp_us_H |= ((uint32)buffer[8] << 15U);
	rx_timeStamp_us_H |= ((uint32)buffer[9] << 23U);
	rx_timeStamp_us_H |= (((uint32)buffer[10] & 0x01U) << 31U);

	rx_protocol_type = ((buffer[10] & 0x80U) == 0x80U) ? 1U : 0U;

	rx_can_id = lpa_u16add((uint16)buffer[11], ((uint16)buffer[12] & 0x07U) << 8U);
	rx_lin_id = (uint16)buffer[11] & 0x3FU;
	rx_extCan_id = (uint32)buffer[11];
	rx_extCan_id |= ((uint32)buffer[12] << 8U);
	rx_extCan_id |= ((uint32)buffer[13] << 16U);
	rx_extCan_id |= (((uint32)buffer[14] & 0x1FU) << 24U);

	rx_fdf = ((buffer[14] & 0x20U) == 0x20U) ? 1U : 0U;
	rx_rtr = ((buffer[14] & 0x40U) == 0x40U) ? 1U : 0U;
	rx_ide = ((buffer[14] & 0x80U) == 0x80U) ? 1U : 0U;

	if (rx_frame_type == DATA_FRAME)
	{
		data_frame.port = rx_sourcePort;
		data_frame.proto = rx_protocol_type;
		data_frame.ts_us_high = rx_timeStamp_us_H;
		data_frame.ts_us_low = rx_timeStamp_us_L;
		data_frame.ts_ns = rx_timeStamp_ns;
		for (i = 15U; i < data_length; i++)
		{
			data_frame.Data[i - 15U] = buffer[i];
			Rx_Data[i - 15U] = buffer[i];
		}
		i = lpa_u16sub(data_length, 15U);
		if (i <= 255U)
		{
			data_frame.data_len = (uint8)i;
		}
		else
		{
			data_frame.data_len = 8U;
		}
		if (rx_protocol_type == PROTOCOL_CAN)
		{
			if (rx_ide == STANDARD_CAN)
			{
				data_frame.ftype = FRAME_TYPE_CAN_BASE;
				if (rx_fdf == CAN_FD)
				{
					data_frame.ftype = FRAME_TYPE_CANFD_BASE;
				}
				data_frame.ID = rx_can_id;
				route_id = data_frame.ID;
			}
			else
			{
				data_frame.ftype = FRAME_TYPE_CAN_EXT;
				if (rx_fdf == CAN_FD)
				{
					data_frame.ftype = FRAME_TYPE_CANFD_EXT;
				}
				data_frame.ID = rx_extCan_id;
				route_id = (data_frame.ID | 0x80000000U);
			}

			dst = frm_findRouteTbl(data_frame.port, route_id, tbl_cnt);

			if (dst != 0u)
			{
				cnvt_data.port = rx_sourcePort;
				cnvt_data.ide = rx_ide;
				cnvt_data.len = proc_dataLen;
				SAL_MemCopy(cnvt_data.data, Rx_Data, proc_dataLen);

				switch (dst)
				{
				case TSNC:
#if (SIC_BSP_SUPPORT_TEST_APP_TPA == 1u)
					cnvt_data.id = rx_extCan_id;
					if (fCANtoEthCB != NULL)
					{
						fCANtoEthCB(&cnvt_data);
					}
#endif
					break;
				case CANC:
					cnvt_data.id = route_id;
					if (fCANCnvtExpandCB != NULL)
					{
						fCANCnvtExpandCB(&cnvt_data);
					}
					break;
#if (IPC_EN == 1) && (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u)
				case IPC:
					/*misra_c_2012_rule_5_3_violation:	Declaration with identifier "i" hides another declaration.*/
					for (i = 0u; i < IPC_NUM; i++)
					{
						if ((ipcTbl[i].src_ch == (rx_sourcePort - 5u)) && (ipcTbl[i].src_id == route_id))
						{
							(void)IPC_SendPacket(ipcTbl[i].dst_core, (uint16)TCC_IPC_CMD_CAN_MSG, (uint16)ipcTbl[i].src_ch, rxResponseBuffer, data_length);
						}
					}
					break;
#endif
				default:
					break;
				}
			}
		}
		else
		{
			data_frame.ftype = FRAME_TYPE_LIN_RX;
			data_frame.ID = rx_lin_id;
			route_id = data_frame.ID;
			dst = frm_findRouteTbl(data_frame.port, route_id, tbl_cnt);

			if (dst != 0u)
			{
				switch (dst)
				{
#if (IPC_EN == 1) && (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u)
				case IPC:
					for (i = 0u; i < IPC_NUM; i++)
					{
						if ((ipcTbl[i].src_ch == (rx_sourcePort - 5u)) && (ipcTbl[i].src_id == route_id))
						{
							(void)IPC_SendPacket(ipcTbl[i].dst_core, (uint16)TCC_IPC_CMD_LIN_MSG, (uint16)ipcTbl[i].src_ch, rxResponseBuffer, data_length);
						}
					}
					break;
#endif
				default:
					break;
				}
			}
		}

		if (fLpaDataCB != NULL)
		{
			fLpaDataCB(&data_frame);
		}
	}
	else
	{
		ts_frame.port = rx_sourcePort;
		ts_frame.ts_us_high = rx_timeStamp_us_H;
		ts_frame.ts_us_low = rx_timeStamp_us_L;
		ts_frame.ts_ns = rx_timeStamp_ns;

		if (fLpaTsCB != NULL)
		{
			fLpaTsCB(&ts_frame);
		}
	}
}
/*misra_c_2012_rule_8_8_violation:	missing static storage modifier for "parse_status_frame" which has internal linkage*/
static uint8 parse_status_frame(const uint8 *buffer)
{
	uint8 port;
	uint32 upper32;
	uint32 lower32;
	uint8 i;
	uint8 error_bit = 0;
	uint32 count = 0U;
	uint32 v;
	uint32 mask;
	uint8 flag[32] = {
		0,
	};
	RX_STATUS_FRAME st;

	port = buffer[0];
	lower32 = (uint32)buffer[1];
	lower32 |= ((uint32)buffer[2] << 8);
	lower32 |= ((uint32)buffer[3] << 16);
	lower32 |= ((uint32)buffer[4] << 24);
	upper32 = (uint32)buffer[5];
	upper32 |= ((uint32)buffer[6] << 8);
	upper32 |= ((uint32)buffer[7] << 16);
	upper32 |= ((uint32)buffer[8] << 24);

	/*misra_c_2012_rule_2_2_violation:	Assigning value "0" to "error_bit" here, but that stored value is overwritten before it can be used.*/
	// error_bit = 0;
	st.error_num = 0u;
	st.port = port;
	for (i = 0u; i < 32U; i++)
	{
		v = 1UL << i;
		mask = lower32 & v;
		if (mask == v)
		{
			flag[count] = i;
			count = lpa_u32add(count, 1U);
			if (st.error_num <= 3U)
			{
				st.error[st.error_num] = i;
				st.error_num = lpa_u8add(st.error_num, 1U);
			}
		}
	}
	for (i = 0u; i < 32U; i++)
	{
		v = 1UL << i;
		mask = upper32 & v;
		if (mask == v)
		{
			flag[count] = lpa_u8add(i, 32U);
			count = lpa_u32add(count, 1U);

			if (st.error_num <= 3U)
			{
				st.error[st.error_num] = lpa_u8add(i, 32U);
				st.error_num = lpa_u8add(st.error_num, 1U);
			}
		}
	}
	error_bit = flag[0];

	for (i = 0u; i < st.error_num; i++)
	{
		if (st.error[i] == 29U)
		{
			error_bit = 29U;
			break;
		}
	}

	if (fLpaStatusCB != NULL)
	{
		fLpaStatusCB(&st);
	}

	return error_bit;
}


static int32a lengthValidCheck(LPA_FRAME_TYPE type, uint8 data_len)
{
	int32a ret = -1;
	uint8 i;

	if (type <= FRAME_TYPE_CAN_EXT)
	{
		if (data_len <= 8U)
		{
			ret = 0;
		}
	}
	else if (type <= FRAME_TYPE_CANFD_EXT)
	{
		if (data_len <= 8U)
		{
			ret = 0;
		}
		else if (data_len <= 64U)
		{
			for (i = 0u; i < 7U; i++)
			{
				if (data_len == CanDlcLen[i])
				{
					ret = 0;
					break;
				}
			}
		}
		else
		{
			/**/
		}
	}
	else if (type <= FRAME_TYPE_LIN_RX)
	{
		if ((data_len > 0U) && (data_len <= 8U))
		{
			ret = 0;
		}
	}
	else
	{
		/**/
	}
	return ret;
}

static void ClassicCAN_Send(uint8 PortNum, uint32 CAN_ID, uint8 Data_Len, const uint8 *pData, uint8 ExtEnable)
{
	LPA_TX_MESSAGE msg;

	if ((pData != NULL) && (Data_Len <= 8U))
	{
		build_CANHeader(msg.MsgData, TIMESTAMP_ON, (uint32)CAN_ID, 0U, ExtEnable, 0U);

		SAL_MemCopy(&msg.MsgData[LPA_TX_HDR_SIZE], pData, Data_Len);
		msg.size = lpa_u16add(LPA_TX_HDR_SIZE, (uint16)Data_Len);
		msg.MsgID = TX_LPA_CMD_FRAME_SEND;
		msg.idt = lpa_u16add((uint16)PortNum, CAN_PORT_MASK);

#if (MCAL_TX_CONFIRM_SUPPORT == 1)
		msg.request_core = (uint8)IPC_SVC_CH_MAX;
#endif

		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
		mcu_printf("[%s] port=%d, Id=0x%x(ext:%c), size=%d\n", __func__, PortNum, CAN_ID, (ExtEnable == 1U) ? 'Y' : 'N', Data_Len);
	}
}

static void CANFD_Send(uint8 PortNum, uint32 CAN_ID, uint8 Data_Len, const uint8 *pData, uint8 ExtEnable)
{
	LPA_TX_MESSAGE msg;

	if ((pData != NULL) && (Data_Len <= 64U))
	{
		build_CANHeader(msg.MsgData, TIMESTAMP_ON, (uint32)CAN_ID, 1U, ExtEnable, 1U);

		SAL_MemCopy(&msg.MsgData[LPA_TX_HDR_SIZE], pData, Data_Len);
		msg.size = lpa_u16add(LPA_TX_HDR_SIZE, (uint16)Data_Len);
		msg.MsgID = TX_LPA_CMD_FRAME_SEND;
		msg.idt = lpa_u16add((uint16)PortNum, CAN_PORT_MASK);

#if (MCAL_TX_CONFIRM_SUPPORT == 1)
		msg.request_core = (uint8)IPC_SVC_CH_MAX;
#endif

		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
		mcu_printf("[%s] port=%d, Id=0x%x(ext:%c), size=%d\n", __func__, PortNum, CAN_ID, (ExtEnable == 1U) ? 'Y' : 'N', Data_Len);
	}
}

static void LIN_Send(uint8 PortNum, uint32 LIN_ID, uint8 OpMode, uint8 Data_Len, const uint8 *pData)
{
	LPA_TX_MESSAGE msg;
	if ((pData != NULL) && ((Data_Len <= 8U) && (Data_Len > 0U)))
	{
		/*cert_exp37_c_violation:    Calling function "build_LINHeader(uint8 *, uint8, uint8, uint8, uint8, uint8)" with the argument "LIN_ID", which has an incompatible type "uint32" instead of "uint8".*/
		build_LINHeader(msg.MsgData, TIMESTAMP_ON, OpMode, 1U, Data_Len, (uint8)LIN_ID);

		SAL_MemCopy(&msg.MsgData[LPA_TX_HDR_SIZE], pData, Data_Len);
		msg.size = lpa_u16add(LPA_TX_HDR_SIZE, (uint16)Data_Len);
		msg.MsgID = TX_LPA_CMD_FRAME_SEND;
		msg.idt = lpa_u16add((uint16)PortNum, LIN_PORT_MASK);

#if (MCAL_TX_CONFIRM_SUPPORT == 1)
		msg.request_core = (uint8)IPC_SVC_CH_MAX;
#endif

		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
		mcu_printf("[%s] port=%d, Id=0x%x(%cx), size=%d\n", __func__, PortNum, LIN_ID, (OpMode == LIN_TX_OPERATON) ? 'T' : 'R', Data_Len);
	}
}

void frm_cnvt_msg_send(uint8 port_num, uint8 can_type, uint8 is_ext, uint32 id, uint length, const uint8 *data)
{
	stLPA_REQUEST_t req;
	stLPA_RESPONSE_t resp;
	uint8 loop_data;

	req.buffer = txReqBuff_cnvt;
	resp.buffer = NULL;

	if ((port_num != 0u) && (id != 0x0u))
	{
		if (can_type == 0u) // classic can
		{
			build_CANHeader(&req.buffer[0], TIMESTAMP_ON, id, can_type, is_ext, 0);

			if (length > 8u)
			{
				mcu_printf("=== wrong DLC : classic CAN only under 8  ===\n");
				return;
			}
		}
		else if (can_type == 1) // FD can
		{
			build_CANHeader(&req.buffer[0], TIMESTAMP_ON, id, can_type, is_ext, 1);
		}
		else
		{
			mcu_printf("=== wrong cantype : only '0' or '1'  ===\n");
			return;
		}

		for (loop_data = 0u; loop_data < length; loop_data++)
		{
			req.buffer[LPA_TX_HDR_SIZE + loop_data] = data[loop_data];
		}

		req.idt = lpa_u16add((uint16)port_num, CAN_PORT_MASK);
		req.sn = gSN;
		gSN = lpa_u16add(gSN, 1U);
		req.size = lpa_u16add(LPA_TX_HDR_SIZE, length);

		/*
		   SAL_GetTickCount(&tick_canc_tx);
		   mcu_printf("===================================================\n");
		   mcu_printf("lpa_write = %d lpa_rx = %d\n", tick_canc_tx, tick_lpa_rx);
		   mcu_printf("%s_%d canc latency = %d (ms)\n",__func__, __LINE__, (tick_canc_tx - tick_lpa_rx));
		   mcu_printf("===================================================\n");
		 */
		(void)lpa_write(&req, &resp);
	}
	else
	{
		// Dont send the CAN message
	}
}

void frm_lpa_sendFrame(LPA_FRAME_TYPE ftype, uint8 PortNum, uint32 ID, uint8 DataLen, const uint8 *pData)
{
	if ((pData != NULL) && (ftype <= FRAME_TYPE_LIN_RX))
	{
		if (lengthValidCheck(ftype, DataLen) == 0)
		{
			switch (ftype)
			{
			case FRAME_TYPE_CAN_BASE:
				ClassicCAN_Send(PortNum, ID, DataLen, pData, 0U);
				break;
			case FRAME_TYPE_CAN_EXT:
				ClassicCAN_Send(PortNum, ID, DataLen, pData, 1U);
				break;
			case FRAME_TYPE_CANFD_BASE:
				CANFD_Send(PortNum, ID, DataLen, pData, 0U);
				break;
			case FRAME_TYPE_CANFD_EXT:
				CANFD_Send(PortNum, ID, DataLen, pData, 1U);
				break;
			case FRAME_TYPE_LIN_TX:
				LIN_Send(PortNum, ID, LIN_TX_OPERATON, DataLen, pData);
				break;
			case FRAME_TYPE_LIN_RX:
				LIN_Send(PortNum, ID, LIN_RX_OPERATON, DataLen, pData);
				break;
			default:
				mcu_printf("[%s] unkonwn frame type[%d]\n", __func__, ftype);
				break;
			}
		}
		else
		{
			mcu_printf("[%s] wrong data length\n", __func__);
		}
	}
	else
	{
		mcu_printf("[%s] wrong input parameter.\n", __func__);
	}
}

#if (IPC_EN == 1u)
#if (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u)
static void NP_IpcCbFunc_CM7_0_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_msg_M7(IPC_CH_CM7_0_LPA, uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_1_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_msg_M7(IPC_CH_CM7_1_LPA, uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_2_Msg(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_msg_M7(IPC_CH_CM7_2_LPA, uhwCmd, pucData, uhwLength);
}

static void ipc_receive_msg_M7(IPCSvcCh_t tx_core, uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	/* misra_c_2012_rule_2_2_violation:	variable "i" was declared but never referenced*/
	// uint16 i = 0;
	LPA_TX_MESSAGE msg;
	uint8 protocol;

	if (pucData != NULL_PTR)
	{
		/*Null-checking "pucData" suggests that it may be null, but it has already been dereferenced on all paths leading to the check*/
		protocol = (pucData[0] & 0x40u) >> 6u;

		SAL_MemCopy(&msg.MsgData[0], pucData, uhwLength);
		msg.size = uhwLength;
		msg.MsgID = TX_LPA_CMD_FRAME_SEND;
		if (protocol == PROTOCOL_CAN)
		{
			msg.idt = lpa_u16add((uint16)uhwCmd, CAN_PORT_MASK);
		}
		else if (protocol == PROTOCOL_LIN)
		{
			msg.idt = lpa_u16add((uint16)uhwCmd, LIN_PORT_MASK);
		}
		/*misra_c_2012_rule_15_7_violation:	No non-empty terminating "else" statement.*/
		else
		{
			/*nop*/
		}

#if (MCAL_TX_CONFIRM_SUPPORT == 1)
		msg.request_core = (uint8)tx_core;
		msg.proto = protocol;
		msg.port = (uint8)uhwCmd;
#endif

		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
	}
	else
	{
		mcu_printf("%s_%d callback to Data empty !!!\n", __func__, __LINE__);
	}
}
#endif

static void NP_IpcCbFunc_CM7_0_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_ip_ctrl_M7(uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_1_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_ip_ctrl_M7(uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_2_IpCtrl(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_receive_ip_ctrl_M7(uhwCmd, pucData, uhwLength);
}

static void ipc_receive_ip_ctrl_M7(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	// uint16 i = 0;
	/* misra_c_2012_rule_2_2_violation:	variable "msg" was declared but never referenced*/
	// LPA_TX_MESSAGE msg;

	if (uhwCmd == TCC_IPC_CMD_IP_CTRL_RESET)
	{
		if (pucData != NULL_PTR)
		{
			LPA_SW_Reset();
		}
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_INITIAL)
	{
		(void)frm_lpa_write_ccf(Base_Conf[0]);
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_CLK_CAN)
	{
		if (pucData != NULL_PTR)
		{
			frm_CAN_speed_config(Base_Conf, pucData[0], pucData[1], pucData[2]);
		}
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_TYPE)
	{
		if (pucData != NULL_PTR)
		{
			frm_CAN_type_config(Base_Conf, pucData[0], pucData[1]);
		}
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_CLK_LIN)
	{
		frm_LIN_speed_config(Base_Conf, pucData[0], pucData[1]);
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_CKSUM)
	{
		frm_LIN_checksum_config(Base_Conf, pucData[0], pucData[1], pucData[2]);
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_AUTORX)
	{
		frm_LIN_checksum_config(Base_Conf, pucData[0], pucData[1], pucData[2]);
	}
	else if (uhwCmd == TCC_IPC_CMD_IP_CTRL_TRCV_SET_MODE)
	{
		frm_lpa_set_trxIC((uint32)pucData[0], (uint32)pucData[1], (uint32)pucData[2], (uint32)pucData[3]);
	}
	else
	{
		mcu_printf("%s_%d uhwCmd invalid !!!\n", __func__, __LINE__);
	}
}

static void NP_IpcCbFunc_CM7_0_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	gM7_N = IPC_CH_CM7_0_LPA;
	ipc_receive_show_status_M7(uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_1_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	gM7_N = IPC_CH_CM7_1_LPA;
	ipc_receive_show_status_M7(uhwCmd, pucData, uhwLength);
}

static void NP_IpcCbFunc_CM7_2_IpStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	gM7_N = IPC_CH_CM7_2_LPA;
	ipc_receive_show_status_M7(uhwCmd, pucData, uhwLength);
}

/* test function, slave call set_slave_ack finished queue processing */
static void NP_IpcCbFunc_CM7_1_QueueStat(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	set_slave_ack(1U);
}

/* Slave call set_slave_ack finished queue processing */
void set_slave_ack(uint16 ack)
{
	slave_ack = ack;
}

uint16 get_slave_ack(void)
{
	return slave_ack;
}

/* master call set_master_ack finished queue processing */
void set_master_ack(uint16 ack)
{
	master_ack = ack;
}

uint16 get_master_ack(void)
{
	return master_ack;
}

static void LPA_IPC_writeMessage(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	ipc_msg.cmd = uhwCmd;
	SAL_MemCopy(&ipc_msg.data, pucData, uhwLength);
	ipc_msg.length = uhwLength;
}

static void ipc_receive_show_status_M7(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	//    SALRetCode_t ret;
	//    LPA_TX_MESSAGE msg;
	//    eLPA_MODE_t stat_val;
	/*cert_exp37_c_violation: Calling function "frm_lpa_getCanCnt(uint16)" with the argument "input_port", which has an incompatible type "uint32" instead of "uint16".*/
	// uint16 input_port;

	ipc_cnt_flag = 0U;

	LPA_IPC_writeMessage(uhwCmd, pucData, uhwLength);

	if (ipc_msg.data != NULL)
	{
		LPA_IPC_rxIrq_wakeUp();
	}
}
#endif
/*misra_c_2012_rule_10_3_violation:	Implicit conversion of "IPC" from essential type "anonymous enum" to different or narrower essential type "unsigned 8-bit int".*/
void frm_build_routeTable(uint8 src_ch, uint32 src_id, LPA_DATA_DST type)
{
	(void)SAL_SemaphoreWait(LpaRouteMutexId, 0, SAL_OPT_BLOCKING);
	for (uint32 i = 0; i < tbl_cnt; i++)
	{
		if ((route_tbl[i].src == src_ch) && (route_tbl[i].id == src_id))
		{
			(void)SAL_SemaphoreRelease(LpaRouteMutexId);
			return;
		}
	}
	/*misra_c_2012_rule_14_3_violation:	Controlling expression "tbl_cnt < 256" is invariant.*/
	/// if(tbl_cnt < 256) tbl_cnt is alwalys under 256 because of type uint8
	//{
	route_tbl[tbl_cnt].src = src_ch;
	route_tbl[tbl_cnt].id = src_id;
	route_tbl[tbl_cnt].dst = type;

	tbl_cnt++;
	//}
#if 0
    for(uint8 i = 0; i < tbl_cnt; i++)
    {
        mcu_printf("[%d] src = %d, id = 0x%X, dst = %d\n", i, route_tbl[i].src, route_tbl[i].id, route_tbl[i].dst);
    }
#endif

	(void)SAL_SemaphoreRelease(LpaRouteMutexId);
}

uint8 frm_findRouteTbl(uint8 input_port, uint32 input_id, uint8 index)
{
	uint8 ret = 0u;

	for (uint32 i = 0u; i < index; i++)
	{
		/*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "input_port" (unsigned) is not the same as that of the right operand "5"(signed).*/
		if ((route_tbl[i].src == (input_port - 5u)) && (route_tbl[i].id == input_id))
		{
			ret = route_tbl[i].dst;
		}
	}

	/*cert_int02_c_violation: Casting "-1" from "int" to "unsigned char" without checking its value may result in lost or misinterpreted data.*/
	// return -1;
	return ret;
}

uint8 frm_get_CAN_length(uint8 dlc)
{
	uint8 len = 0U;

	if (dlc <= 8U)
	{
		len = dlc;
	}
	else if (dlc == 9U)
	{
		len = 12U;
	}
	else if (dlc == 10U)
	{
		len = 16U;
	}
	else if (dlc == 11U)
	{
		len = 20U;
	}
	else if (dlc == 12U)
	{
		len = 24U;
	}
	else if (dlc == 13U)
	{
		len = 32U;
	}
	else if (dlc == 14U)
	{
		len = 48U;
	}
	else if (dlc == 15U)
	{
		len = 64U;
	}
	else
	{
		/**/
	}

	return len;
}

void frm_lpa_write(const stLPA_REQUEST_t *pstRequest, stLPA_RESPONSE_t *pstResponse)
{
	(void)lpa_write(pstRequest, pstResponse);
}
void LPA_Tx_readRegRequest(uint16 addr)
{
	LPA_TX_MESSAGE msg;

	msg.MsgData[0] = (uint8)(addr & 0x00FFu);
	msg.MsgData[1] = (uint8)(addr >> 8u);

	msg.MsgID = TX_LPA_READ_REG_REQ;
	msg.idt = 0xFFFF;
	msg.size = 2;

	(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
}

void frm_lpa_rx_taskNotifyStop(void)
{
	(void)xTaskNotify(RX_Task_Handle, (uint32)eLPA_RX_TASK_NOTIFY_STOP, eSetBits);
}

void LPA_Rx_TaskNotifyRestart(void)
{
	(void)xTaskNotify(RX_Task_Handle, (uint32)eLPA_RX_TASK_NOTIFY_RESTART, eSetBits);
}

uint8 frm_lpa_rx_getTaskState(void)
{
	return rx_task_state;
}

/* Tx, Rx Task Create */
/* Rx Task */
static void LPA_Rx_Task(void *pArg)
{
	(void)pArg;
	uint32 ulInterruptStatus;
	stLPA_REQUEST_t req;
	stLPA_RESPONSE_t resp;
	uint8 data_flag;
	uint8 status_flag;
	int32 ret;
	uint8 done;
	TickType_t wait_time;

	RX_Task_Handle = xTaskGetCurrentTaskHandle();
	(void)lpa_startProc(RX_Task_Handle);

	SAL_MemSet(&req, 0U, sizeof(stLPA_REQUEST_t));
	SAL_MemSet(&resp, 0U, sizeof(stLPA_RESPONSE_t));

	req.buffer = rxRequestBuffer;
	resp.buffer = rxResponseBuffer;
	rx_task_state = 1U;
	mcu_printf("\n Master Framework v%d.%d.%d\n",
			   verInfo.majorVer, verInfo.minorVer, verInfo.patchVer);

	while (TRUE)
	{

		if (gDataIntMode == 1U)
		{
			wait_time = portMAX_DELAY;
			status_flag = 0U;
			data_flag = 0U;
		}
		else
		{
			wait_time = (TickType_t)0;
			status_flag = 1U;
			data_flag = 1U;
		}

		if (xTaskNotifyWait(0x00UL, NOTI_SIG_ALL, &ulInterruptStatus, wait_time) != pdTRUE)
		{
			/**/
		}
		else
		{
			if ((ulInterruptStatus & (uint32)eLPA_RX_TASK_NOTIFY_DATA_INT) == (uint32)eLPA_RX_TASK_NOTIFY_DATA_INT)
			{
				data_flag = 1U;
			}
			if ((ulInterruptStatus & (uint32)eLPA_RX_TASK_NOTIFY_STATUS_INT) == (uint32)eLPA_RX_TASK_NOTIFY_STATUS_INT)
			{
				status_flag = 1U;
			}
			if ((ulInterruptStatus & (uint32)eLPA_RX_TASK_NOTIFY_STOP) == (uint32)eLPA_RX_TASK_NOTIFY_STOP)
			{
				rx_task_state = 0U;
				mcu_printf("RX task is a sleep state\n");
				continue;
			}
			if ((ulInterruptStatus & (uint32)eLPA_RX_TASK_NOTIFY_RESTART) == (uint32)eLPA_RX_TASK_NOTIFY_RESTART)
			{
				rx_task_state = 1U;
				mcu_printf("RX task is restarted\n");
				continue;
			}
		}

		if (rx_task_state == 1U)
		{
			if (status_flag == 1U)
			{
				done = 0;
				do
				{
					req.idt = 0xFFFEu;
					ret = lpa_read(&req, &resp);
					if (ret == (int32)eLPA_RET_OK)
					{
						if (resp.ret == (uint16)eLPA_RET_OK)
						{
							if (resp.size > 0U)
							{
#if (IPC_EN == 1)
								uint16 ipc_cmd1 = TCC_IPC_CMD_CAN_STAS;
								if (rxResponseBuffer[0] >= 22U) // port number
								{
									ipc_cmd1 = TCC_IPC_CMD_LIN_STAS;
								}
								(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, ipc_cmd1, (uint16)TCC_IPC_CMD_STAS_SEND, &rxResponseBuffer[0], sizeof(rxResponseBuffer));
								(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, ipc_cmd1, (uint16)TCC_IPC_CMD_STAS_SEND, &rxResponseBuffer[0], sizeof(rxResponseBuffer));
								(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, ipc_cmd1, (uint16)TCC_IPC_CMD_STAS_SEND, &rxResponseBuffer[0], sizeof(rxResponseBuffer));
#endif
								(void)parse_status_frame(rxResponseBuffer);
							}
						}
						else
						{
							done = 1U;
						}
					}
					else
					{
						done = 1U;
					}
				} while (done == 0U);
				/*misra_c_2012_rule_17_7_violation:	The return value of a non-void function "NVIC_IntSrcEn" is unused.*/
				(void)NVIC_IntSrcEn(LPA_ERROR_AXI_IRQn);
			}

			if (data_flag == 1U)
			{
				done = 0;
				do
				{
					req.idt = 0x0000u;
					ret = lpa_read(&req, &resp);
					if (ret == (int32)eLPA_RET_OK)
					{
						if (resp.ret == (uint16)eLPA_RET_OK)
						{
							if (resp.size == 132U)
							{
#if (IPC_EN == 1)
								if (ipc_cnt_flag == 1U)
								{
									ipc_cnt++;
									(void)IPC_SendPacket(gM7_N, (uint16)TCC_IPC_CMD_RESP, (uint16)TCC_IPC_CMD_RESP_CNT, &rxResponseBuffer[0], sizeof(rxResponseBuffer));
									if (ipc_cnt == 2U)
									{
										ipc_cnt = 0U;
										ipc_cnt_flag = 0U;
									}
								}
								else
								{
									parse_mm_response(rxResponseBuffer);
								}
#else
								parse_mm_response(rxResponseBuffer);
#endif

								done = 1U; // ES version (MM_Response)
							}
							else if (resp.size > 4U)
							{
								/* for TSNC latency */
								// SAL_GetTickCount(&tick_lpa_rx);
								proc_dataLen = lpa_u16sub(resp.size, 4U);
								parse_proc_frame(rxResponseBuffer, proc_dataLen);
								// 241106 sy.kim
#if (IPC_A65_TEST == 1)
								mcu_printf("M7-NP to AP IPC Send Packet\n");
								(void)IPC_SendPacket(IPC_CH_CA65_NS_USER, (uint16)TCC_IPC_CMD_AP_TEST, TCC_IPC_CMD_AP_SEND, rxResponseBuffer, proc_dataLen);
#endif
							}
							else
							{
								/**/
							}
						}
						else
						{
							done = 1U;
						}
					}
					else
					{
						done = 1U;
					}
				} while (done == 0U);
			}
		}
		else
		{
			(void)SAL_TaskSleep(10);
		}
	}
}

void LPA_Rx_CreateAppTasks(void)
{
	static uint32 uiLPA_RxTaskID;
	static uint32 uiLPA_RxTaskStk[LPA_RX_TASK_STK_SIZE];

	if (SAL_TaskCreate(&uiLPA_RxTaskID,
					   (const uint8 *)"RXTask",
					   (SALTaskFunc)&LPA_Rx_Task,
					   (uint32 *const)&uiLPA_RxTaskStk[0],
					   LPA_RX_TASK_STK_SIZE,
					   SAL_PRIO_LPA_RX,
					   NULL_PTR) != SAL_RET_SUCCESS)
	{
		mcu_printf("%s Fatal!! Task Create Fail. \n", __func__);
	}
}

/* Tx Task */
static void LPA_Tx_Task(void *pArg)
{
	(void)pArg;
	LPA_TX_MESSAGE msg;
	uint32 copiedSize = 0U;
	uint8 task_exit = 0u;
	stLPA_REQUEST_t req;
	stLPA_RESPONSE_t resp;

	req.buffer = txRequestBuffer;
	resp.buffer = NULL;

// 241106 sy.kim
#if (IPC_A65_TEST == 1)
	IPC_RegisterCbFunc(IPC_CH_CA65_NS_USER, (uint8)TCC_IPC_CMD_AP_TEST, (IPCCallback)&NP_IpcCbFunc_A65_Test, NULL_PTR, NULL_PTR);
#endif

	(void)memset(&msg, 0, sizeof(LPA_TX_MESSAGE));

#if (IPC_EN == 1)
	/* CM7 -> NP */
#if (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1)
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_CAN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_0_Msg, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_CAN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_1_Msg, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_CAN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_2_Msg, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_LIN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_0_Msg, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_LIN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_1_Msg, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_LIN_MSG, (IPCCallback)&NP_IpcCbFunc_CM7_2_Msg, NULL_PTR, NULL_PTR);
#endif
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_IP_CTRL, (IPCCallback)&NP_IpcCbFunc_CM7_0_IpCtrl, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_IP_CTRL, (IPCCallback)&NP_IpcCbFunc_CM7_1_IpCtrl, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_IP_CTRL, (IPCCallback)&NP_IpcCbFunc_CM7_2_IpCtrl, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_IP_STAS, (IPCCallback)&NP_IpcCbFunc_CM7_0_IpStat, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_IP_STAS, (IPCCallback)&NP_IpcCbFunc_CM7_1_IpStat, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_IP_STAS, (IPCCallback)&NP_IpcCbFunc_CM7_2_IpStat, NULL_PTR, NULL_PTR);
	/* add ipc callback function for status queue processing */
	IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_IP_CTRL, (IPCCallback)&NP_IpcCbFunc_CM7_1_QueueStat, NULL_PTR, NULL_PTR);

#endif

	mcu_printf("[%s] task start\n", __func__);

	while (task_exit == 0U)
	{
#if 0
        if(isTsncEthSend == TRUE)
        {
            Test_CAN_Send(canMsg.portNum, 1, canMsg.mId, 8, canMsg.mFDFormat, canMsg.mRemoteTransmitRequest, canMsg.mExtendedId);
            isTsncEthSend = FALSE;
        }
#endif

		if (SAL_QueueGet(gLpaTxQueueHandle, (void *)&msg, &copiedSize, portMAX_DELAY, SAL_OPT_BLOCKING) != SAL_RET_SUCCESS)
		{
			mcu_printf("[%s] Fatal!! Get TX Queue Fail. \n", __func__);
		}

		switch (msg.MsgID)
		{
		case TX_LPA_CMD_FRAME_SEND:
		case TX_LPA_TSNC_FRAME_SEND:
		{
			req.idt = msg.idt;
			req.sn = gSN;
			gSN = lpa_u16add(gSN, 1U);
			req.size = msg.size;
#if ((IPC_EN == 1u) &&                        \
	 (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u) && \
	 (MCAL_TX_CONFIRM_SUPPORT == 1u))
			/*
				MCAL - CAN
				MCAL requires a match between the request message and the TX_CONFIRM message.
				The last buffer contains the buffer_index value of MCAL.
			*/
			if ((msg.request_core < (uint8)IPC_SVC_CH_MAX) && (msg.proto == PROTOCOL_CAN))
			{
				req.size = msg.size - 1U;
			}
#endif

			SAL_MemCopy(&req.buffer[0], msg.MsgData, req.size);

#if ((IPC_EN == 1u) &&                        \
	 (SIC_BSP_SUPPORT_IPC_DATA_SEND == 1u) && \
	 (MCAL_TX_CONFIRM_SUPPORT == 1u))
			/* notify that the transmission is completed. */
			if ((msg.request_core < (uint8)IPC_SVC_CH_MAX) && (msg.proto == PROTOCOL_CAN))
			{
				uint8 ucData[3u];
#if 0
						if (msg.proto == PROTOCOL_LIN)
						{
							ucData[0u] = msg.port;
							(void)IPC_SendPacket((IPCSvcCh_t)msg.request_core,
												(uint16)TCC_IPC_CMD_MCAL_LIN,
												(uint16)TCC_IPC_CMD_MCAL_TX_CONFIRM,
												ucData,
												1U);
						}
						else
#endif
				{
					ucData[0u] = msg.port;
					ucData[1u] = msg.proto;
					ucData[2u] = msg.MsgData[msg.size - 1u]; // buffer_index value of MCAL
					(void)IPC_SendPacket((IPCSvcCh_t)msg.request_core,
										 (uint16)TCC_IPC_CMD_MCAL_CAN,
										 (uint16)TCC_IPC_CMD_MCAL_TX_CONFIRM,
										 ucData,
										 3U);
				}
			}
#endif

			(void)lpa_write(&req, &resp);
		}
		break;

		case TX_LPA_READ_REG_REQ:
		{
			req.idt = msg.idt;
			req.sn = gSN;
			gSN = lpa_u16add(gSN, 1U);
			req.size = sizeof(MM_READ);

			SAL_MemSet(&mrr_data, 0, sizeof(MM_READ));

			mrr_data.type = MM_READ_TYPE;
			mrr_data.addr1 = 0x0700u;
			mrr_data.reserved2[1] = 0x4u;
			mrr_data.addr2 = msg.MsgData[1];
			mrr_data.addr2 |= ((uint16)msg.MsgData[0] << 8u);

			SAL_UtilCrc32((uint8 *)&mrr_data.crc32, (uint8 *)&mrr_data, sizeof(mrr_data) - sizeof(uint32)); // calculate CRC

			SAL_MemCopy(req.buffer, &mrr_data, sizeof(MM_READ));

			(void)lpa_write(&req, &resp);
		}
		break;
#if (IPC_EN == 1)
		case TX_IPC_RESP_SEND:
			(void)IPC_SendPacket(gM7_N, (uint16)TCC_IPC_CMD_RESP, (uint16)TCC_IPC_CMD_RESP_VAL, msg.MsgData, msg.size);
			break;
#endif
		case TX_LPA_CMD_EXIT:
			task_exit = 1U;
			break;
		default:
			/**/
			break;
		}
	}
	(void)SAL_QueueDelete(gLpaTxQueueHandle);
}

// 241106 sy.kim
#if (IPC_A65_TEST == 1)
static void NP_IpcCbFunc_A65_Test(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength)
{
	// 241106 sy.kim
	mcu_printf("\nM7-NP Receive Data from AP\n");
	// uint16 i = 0;
	LPA_TX_MESSAGE msg;
	uint8 protocol;
	if (pucData != NULL_PTR)
	{
		/*Null-checking "pucData" suggests that it may be null, but it has already been
	   dereferenced on all paths leading to the check*/
		protocol = (pucData[0] & 0x40u) >> 6u;
		SAL_MemCopy(&msg.MsgData[0], pucData, uhwLength);
		msg.size = uhwLength;
		msg.MsgID = TX_LPA_CMD_FRAME_SEND;
		if (protocol == PROTOCOL_CAN)
		{
			msg.idt = lpa_u16add((uint16)uhwCmd, CAN_PORT_MASK);
		}
		else if (protocol == PROTOCOL_LIN)
		{
			msg.idt = lpa_u16add((uint16)uhwCmd, LIN_PORT_MASK);
		}
		/*misra_c_2012_rule_15_7_violation: No non-empty terminating "else" statement.*/
		else
		{
			/*nop*/
		}
		msg.proto = protocol;
		msg.port = (uint8)uhwCmd;
		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0,
						   SAL_OPT_BLOCKING);

		// 241106 sy.kim
		mcu_printf("**************************************\n");
		mcu_printf("CAN RX data Protocol %d, Port %d, Msg len %d\n", msg.proto, msg.port, msg.size);
		for (int i=0; i<msg.size; i++)
		{
			mcu_printf("%d ", msg.MsgData[i]);
		}
		mcu_printf("\n**************************************\n");
	}
	else
	{
		mcu_printf("%s_%d callback to Data empty !!!\n", __func__, __LINE__);
	}
}
#endif

void LPA_Tx_CreateAppTasks(void)
{
	static uint32 uiLPA_TxTaskID;
	static uint32 uiLPA_TxTaskStk[LPA_TX_TASK_STK_SIZE];
	static uint8 ucLPA_TxMsgBuffer[LPA_TX_MSG_QUEUE_NUM * sizeof(LPA_TX_MESSAGE)];
	(void)SAL_SemaphoreCreate(&LpaRouteMutexId, (const uint8 *)"LPA Route Config Mutex", 1UL, SAL_OPT_BLOCKING);

	if (SAL_QueueCreate(&gLpaTxQueueHandle,
						(const uint8 *)"LpaTxQ",
						(void *)ucLPA_TxMsgBuffer,
						LPA_TX_MSG_QUEUE_NUM,
						sizeof(LPA_TX_MESSAGE)) != SAL_RET_SUCCESS)
	{
		mcu_printf("%s Fatal!! TX Queue Create Fail. \n", __func__);
	}

	if (SAL_TaskCreate(&uiLPA_TxTaskID,
					   (const uint8 *)"LpaTxTsk",
					   (SALTaskFunc)&LPA_Tx_Task,
					   (uint32 *const)&uiLPA_TxTaskStk[0],
					   LPA_TX_TASK_STK_SIZE,
					   SAL_PRIO_LPA_TX,
					   NULL_PTR) != SAL_RET_SUCCESS)
	{
		mcu_printf("%s Fatal!! Task Create Fail. \n", __func__);
	}
}

#if (IPC_EN == 1u)
static void LPA_IPC_rxTask(void *pArg)
{
	(void)pArg;
	lpaIpcMessage_t msg;
	LPA_TX_MESSAGE lpa_tx_msg;
	eLPA_MODE_t stat_val;
	/*cert_exp37_c_violation: Calling function "frm_lpa_getCanCnt(uint16)" with the argument "input_port", which has an incompatible type "uint32" instead of "uint16".*/
	uint16 input_port;
	int32 ret;

	while (TRUE)
	{
		LPA_IPC_rxIrq_clearEvent();
		LPA_IPC_rxIrq_waitEvent();

		SAL_MemCopy(&msg, &ipc_msg, sizeof(lpaIpcMessage_t));

		if (msg.cmd == TCC_IPC_CMD_IP_STAS_VAL)
		{
			ret = lpa_getMode(&stat_val);
			if (ret != eLPA_RET_DRV_NG_NOT_OPEN)
			{
				// mcu_printf("%s: ",__FUNCTION__); //QAC

				lpa_tx_msg.MsgData[0] = stat_val;
				lpa_tx_msg.size = sizeof(stat_val);
				lpa_tx_msg.MsgID = TX_IPC_RESP_SEND;
				(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&lpa_tx_msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING);
			}
		}
		else if (msg.cmd == TCC_IPC_CMD_IP_STAS_CNT_CAN)
		{
			/*cert_err30_c_violation: The variable "errno" must be zero before calling errno-setting function "strtoul".*/
			errno = 0u;
#if 0 /* strtoul return 0 */
            input_port = strtoul((int8 *)msg.data, NULL, 10);
#else
			input_port = (uint16)((uintptr_t)(msg.data));
#endif
			ipc_cnt_flag = 1U;
			if (errno == 0u)
			{
				(void)frm_lpa_getCanCnt(input_port);
			}
			else
			{
				mcu_printf("errno : %d !!!\n", errno, __func__, __LINE__);
			}
		}
		else if (msg.cmd == TCC_IPC_CMD_IP_STAS_CNT_LIN)
		{
			/*cert_err30_c_violation: The variable "errno" must be zero before calling errno-setting function "strtoul".*/
			errno = 0u;
#if 0 /* strtoul return 0 */
            input_port = strtoul((int8 *)msg.data, NULL, 10);
#else
			input_port = (uint16)((uintptr_t)(msg.data));
#endif
			ipc_cnt_flag = 1U;
			if (errno == 0u)
			{
				(void)frm_lpa_getLinCnt(input_port);
			}
			else
			{
				mcu_printf("errno : %d !!!\n", errno, __func__, __LINE__);
			}
		}
		/*misra_c_2012_rule_15_7_violation:	No non-empty terminating "else" statement.*/
		else
		{
			/*nop*/
		}
	}
}

void LPA_IPC_RxTaskCreate(void)
{
	static uint32 uiLPA_IPC_taskID;
	static uint32 ipcTaskstk[LPA_IPC_RX_TASK_STK_SIZE];

	LPA_IPC_rxIrq_eventCreate();

	if (SAL_TaskCreate(&uiLPA_IPC_taskID,
					   (const uint8 *)"LpaIpcTask",
					   (SALTaskFunc)&LPA_IPC_rxTask,
					   (uint32 *const)&ipcTaskstk[0],
					   LPA_IPC_RX_TASK_STK_SIZE,
					   SAL_PRIO_LPA_IPC,
					   NULL_PTR) != SAL_RET_SUCCESS)
	{
		mcu_printf("%s Fatal!! Task Create Fail. \n", __func__);
	}
}
#endif
