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
// 241202 sy.kim
#include "katech_database.h" 
#include "katech_can_app.h"

/**************************************************************************************************
 *                                            DEFINITIONS
 **************************************************************************************************/

#define LPA_IPC_WAIT_FLAG (0x00000001UL)

// 241106 sy.kim
#define TCC_IPC_CMD_AP_TEST                 (0xffu)		//cmd1
#define TCC_IPC_CMD_AP_SEND					(0x0fffu)   //cmd2
#define IPC_A65_TEST						(1)

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

// 241107 sy.kim
static uint8 mcu_to_ap_flag = 0;
static uint16 AP_rxcanId = 0;
static uint8 add = 0U;

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
/************************************** 241202 sy.kim IONIQ5 RX ************************************/
/* CAN0(C-CAN) RX 
	0x4A, 0x65, 0xa0, 0x125, 0x130, 0x175, 0x1AA, 0x255, 0x1e5
*/
void RxCAN0_YRS_01_10ms(uint8_t *YRS_RxData);
void RxCAN0_IEB_01_10ms(uint8_t *BrkPedal_RxData);
void RxCAN0_WHL_01_10ms(uint8_t *WHL_RxData);
void RxCAN0_SAS_01_10ms(uint8_t *SAS_RxData);
void RxCAN0_Unknown_0x130(uint8_t *Unknown_RxData);
void RxCAN0_ESC_03_20ms(uint8_t *ESC03_RxData);
void RxCAN0_CLU_01_20ms(uint8_t *CLU_RxData);
void RxCAN0_CLU_02_100ms(uint8_t *CLU2_RxData);
void RxCAN0_RR_C_RDR_01_50ms(uint8_t *RR_C_RDR_01_50ms);

/* CAN1(E-CAN) RX
	0x35, 0xa0, 0x175, 0x1AA, 0x1B5, 0x1BA, 0x1cf, 0x3c1
*/
void RxCAN1_Unknown_0x35(uint8_t *Unknown_0x35);
void RxCAN1_WHL_01_10ms(uint8_t *WHL_01_10ms);
void RxCAN1_ESC_03_20ms(uint8_t *ESC_03_20ms);
void RxCAN1_CLU_01_20ms(uint8_t *CLU_01_20ms);
void RxCAN1_CLU_01_20ms2(uint8_t *LaneInfoRxData);
void RxCAN1_RR_C_RDR_02_50ms(uint8_t *RR_C_RDR_02_50ms);
void RxCAN1_SWRC_03_20ms(uint8_t *SWRC_03_20ms);
void RxCAN1_MFSW_01_200ms(uint8_t *MFSW_01_200ms);

/* CAN2(MDPS) RX
	0xEA, 0x377, 0x378, 0x379
*/
void RxCAN2_MDPS_01_10ms(uint8_t *MDPS_RxData);
void RxCAN2_Unknown_0x377(uint8_t *CMD_RxData);
void RxCAN2_Unknown_0x378(uint8_t *CMD_RxData);
void RxCAN2_Unknown_0x379(uint8_t *CMD_RxData);

/* CAN3(DRV) RX
	0x1EA, 0x1A0, 
*/
void RxCAN3_ADAS_CMD_20_20ms(uint8_t *data)
void RxCAN3_ADAS_CMD_32_50ms(uint8_t *ADAS_CMD_20_20ms_Temp);
void RxCAN3_ADAS_CMD_10_20ms(uint8_t *ADAS_CMD_10_20ms);
void RxCAN0_ADAS_PRK_20_20ms(uint8_t *ADAS_PRK_20_20ms);
/************************************** 241202 sy.kim IONIQ5 TX ************************************/






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
 *                                        IONIQ5 RX FUNCTIONS
 **************************************************************************************************/
uint8_t katech_can_app_mode;
int8_t katech_adas_mode=0;

/*
 * rx CAN0(C-CAN)
 */
// can0 - 0x4A
void RxCAN0_YRS_01_10ms(uint8_t *YRS_RxData)
{
	DB_IONIQ_RxCAN0_YRS_01_10ms *this_DB_CAN0_YRS_01_10ms;

	this_DB_CAN0_YRS_01_10ms = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_YRS_01_10ms);

	this_DB_CAN0_YRS_01_10ms->LatAccel = ((YRS_RxData[10]+(YRS_RxData[11]<<8))*0.000127465)-4.17677312;
	this_DB_CAN0_YRS_01_10ms->LongAccel = ((YRS_RxData[12]+(YRS_RxData[13]<<8))*0.000127465)-4.17677312;
	this_DB_CAN0_YRS_01_10ms->YawRate = ((YRS_RxData[8]+(YRS_RxData[9]<<8))*0.005)-163.84;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_YRS_01_10ms);
}

// can0 - 0x65
void RxCAN0_IEB_01_10ms(uint8_t *BrkPedal_RxData)
{
	DB_IONIQ_RxCAN0_IEB_01_10ms *_DB_AN0_IEB_01_10ms;

	_DB_AN0_IEB_01_10ms = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_IEB_01_10ms);


	_DB_AN0_IEB_01_10ms->BrakeAct = BrkPedal_RxData[13]; // Brake SW Pressure State
	_DB_AN0_IEB_01_10ms->BrakePedalValue = BrkPedal_RxData[14]*0.390625;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_IEB_01_10ms);
}

// can0 - 0xa0
uint8_t WHL_01_10ms_data[KATECH_APP_BUFF_SIZE][24];
uint8_t WHL_01_10ms_cnt=0;

void RxCAN0_WHL_01_10ms(uint8_t *WHL_RxData)
{
	uint8_t pos;

	DB_IONIQ_RxCAN0_WHL_01_10ms *_DB_CAN0_WHL_01_10ms;
	_DB_CAN0_WHL_01_10ms = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_WHL_01_10ms);

	_DB_CAN0_WHL_01_10ms->WheelSpdFL = (WHL_RxData[8]+((WHL_RxData[9]&0x3F)<<8))*0.03125;
	_DB_CAN0_WHL_01_10ms->WheelSpdFR = (WHL_RxData[10]+((WHL_RxData[11]&0x3F)<<8))*0.03125;
	_DB_CAN0_WHL_01_10ms->WheelSpdRL = (WHL_RxData[12]+((WHL_RxData[13]&0x3F)<<8))*0.03125;
	_DB_CAN0_WHL_01_10ms->WheelSpdRR = (WHL_RxData[14]+((WHL_RxData[15]&0x3F)<<8))*0.03125;
	_DB_CAN0_WHL_01_10ms->AvgWheelSpeed = ((_DB_CAN0_WHL_01_10ms->WheelSpdFL + _DB_CAN0_WHL_01_10ms->WheelSpdFR + _DB_CAN0_WHL_01_10ms->WheelSpdRL + _DB_CAN0_WHL_01_10ms->WheelSpdRR) / 4) / 3.6;
	_DB_CAN0_WHL_01_10ms->WheelPulseFL = (WHL_RxData[3])*0.5;
	_DB_CAN0_WHL_01_10ms->WheelPulseFR = (WHL_RxData[4])*0.5;
	_DB_CAN0_WHL_01_10ms->WheelPulseRL = (WHL_RxData[5])*0.5;
	_DB_CAN0_WHL_01_10ms->WheelPulseRR = (WHL_RxData[6])*0.5;


	pos = (WHL_01_10ms_cnt+1)%KATECH_APP_BUFF_SIZE;
	memcpy(WHL_01_10ms_data[pos],WHL_RxData,24);

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_WHL_01_10ms);

	WHL_01_10ms_cnt=pos;
}

// can0 - 0x125
void RxCAN0_SAS_01_10ms(uint8_t *SAS_RxData)
{
	DB_IONIQ_RxCAN0_SAS_01_10ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_SAS_01_10ms);

	dbaddr->StrAngle = ((SAS_RxData[3]+(SAS_RxData[4]<<8)))*0.1;
	if (dbaddr->StrAngle > 3276.7)
		dbaddr->StrAngle = dbaddr->StrAngle - 6553.5;

	dbaddr->StrSpeed = SAS_RxData[5]*4.0;
	if(dbaddr->StrSpeed > 1020) //jyhannn
		dbaddr->StrSpeed = 1020;
	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_SAS_01_10ms);
}

// can0 - 0x130
void RxCAN0_Unknown_0x130(uint8_t *Unknown_RxData)
{
	DB_IONIQ_RxCAN0_Unknown_0x130 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_Unknown_0x130);

	dbaddr->GearPosition = Unknown_RxData[8];  //P:1  R:2  N:3  D:4

	if (dbaddr->GearPosition == 1)
		dbaddr->ADCMGearPosition = 1;
	else if (dbaddr->GearPosition == 2)
		dbaddr->ADCMGearPosition = 4;
	else if (dbaddr->GearPosition == 3)
		dbaddr->ADCMGearPosition = 2;
	else if (dbaddr->GearPosition == 4)
		dbaddr->ADCMGearPosition = 3;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_Unknown_0x130);
}

// can0 - 0x175
void RxCAN0_ESC_03_20ms(uint8_t *ESC03_RxData)
{
	DB_IONIQ_RxCAN0_ESC_03_20ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_ESC_03_20ms);

	dbaddr->ParkingBrk = (ESC03_RxData[10]&0xC0)>>6;  //Engaged : 1 DisEngaged : 0

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_ESC_03_20ms);
}

// can0 - 0x1AA
void RxCAN0_CLU_01_20ms(uint8_t *CLU_RxData)
{
	DB_IONIQ_RxCAN0_CLU_01_20ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_CLU_01_20ms);

	dbaddr->DisplaySpd = CLU_RxData[8]+((CLU_RxData[9]&0x01)<<8);

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_CLU_01_20ms);
}

// can0 - 0x255
void RxCAN0_CLU_02_100ms(uint8_t *CLU2_RxData)
{
	DB_IONIQ_RxCAN0_CLU_02_100ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_CLU_02_100ms);

	dbaddr->FuelLevel = ((CLU2_RxData[13]&0xF0)>>4)+((CLU2_RxData[14]&0x07)<<4);
	dbaddr->AvgFuelConsum = (((CLU2_RxData[5]&0xC0)>>6)+(CLU2_RxData[6]<<2))*0.1;
	dbaddr->Odometer = (CLU2_RxData[9]+(CLU2_RxData[10]<<8)+(CLU2_RxData[11]<<16))*0.1;
	dbaddr->DTE = CLU2_RxData[4]+((CLU2_RxData[5]&0x03)<<8);

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_CLU_02_100ms);
}

// can0 - 0x1e5
void RxCAN0_RR_C_RDR_01_50ms(uint8_t *RR_C_RDR_01_50ms)
{
	DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms);

	dbaddr->RadarFusion = (RR_C_RDR_01_50ms[13]&0x18)>>3;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN0_RR_C_RDR_01_50ms);
}



/*
 * rx CAN1(E-CAN)
 */
// can1 - 0x35
void RxCAN1_Unknown_0x35(uint8_t *Unknown_0x35)
{
	DB_IONIQ_RxCAN1_Unknown_0x35 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_Unknown_0x35);

	dbaddr->AccelAct = (Unknown_0x35[5] & 0xFF);

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_Unknown_0x35);
}

// can1 - 0xa0
void RxCAN1_WHL_01_10ms(uint8_t *WHL_01_10ms)
{
	DB_IONIQ_RxCAN1_WHL_01_10ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_WHL_01_10ms);

	dbaddr->WHL_SpdFLVal = (WHL_01_10ms[8] +((WHL_01_10ms[9] &0x3F)<<8))*0.03125;
	dbaddr->WHL_SpdFRVal = (WHL_01_10ms[10]+((WHL_01_10ms[11]&0x3F)<<8))*0.03125;
	dbaddr->WHL_SpdRLVal = (WHL_01_10ms[12]+((WHL_01_10ms[13]&0x3F)<<8))*0.03125;
	dbaddr->WHL_SpdRRVal = (WHL_01_10ms[14]+((WHL_01_10ms[15]&0x3F)<<8))*0.03125;
	dbaddr->Avg_WhlSpd = ((((dbaddr->WHL_SpdFLVal + dbaddr->WHL_SpdFRVal + dbaddr->WHL_SpdRLVal + dbaddr->WHL_SpdRRVal) / 4) * 5) / 18); // km/h to m/s

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_WHL_01_10ms);
}

// can1 - 0x175
void RxCAN1_ESC_03_20ms(uint8_t *ESC_03_20ms)
{
	DB_IONIQ_RxCAN1_ESC_03_20ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_ESC_03_20ms);

	dbaddr->PrkBrakeAct = (ESC_03_20ms[10] & 0xC0) >> 6;
	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_ESC_03_20ms);
}

// can1 - 0x1AA
void RxCAN1_CLU_01_20ms(uint8_t *CLU_01_20ms)
{

	DB_IONIQ_RxCAN1_CLU_01_20ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms);

	dbaddr->CurrentSpeed = CLU_01_20ms[6] + ((CLU_01_20ms[7] & 0x03) << 8); // signal name in E-CAN dbc file is "CLU_DisSpdVal"
	if((dbaddr->CurrentSpeed % 2) == 1)
		dbaddr->CurrentSpeed = (dbaddr->CurrentSpeed/2) + 1;
	else
		dbaddr->CurrentSpeed = dbaddr->CurrentSpeed/2;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms);
}

// can1 - 0x1B5
void RxCAN1_CLU_01_20ms2(uint8_t *LaneInfoRxData)
{
	DB_IONIQ_RxCAN1_CLU_01_20ms2 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms2);

	dbaddr->LaneQualityL = (LaneInfoRxData[3]&0x07);
	dbaddr->ThisLanePositionL = (((LaneInfoRxData[3]&0xE0)>>5) + (LaneInfoRxData[4]<<3) + ((LaneInfoRxData[5]&0x07)<<11));
	if (dbaddr->ThisLanePositionL > 8192)
		dbaddr->ThisLanePositionL = dbaddr->ThisLanePositionL-16384;
	dbaddr->LanePositionL = dbaddr->ThisLanePositionL * 0.0039625;

	dbaddr->ThisHeadingAngleL = ((LaneInfoRxData[5]&0xF8)>>3)+((LaneInfoRxData[6]&0x1F)<<5);
	if (dbaddr->ThisHeadingAngleL > 512)
		dbaddr->ThisHeadingAngleL = dbaddr->ThisHeadingAngleL - 1024;
	dbaddr->HeadingAngleL =  dbaddr->ThisHeadingAngleL * 0.000976563;

	dbaddr->ThisCurvatureL = LaneInfoRxData[8]+(LaneInfoRxData[9]<<8);
	if (dbaddr->ThisCurvatureL > 32767)
		dbaddr->ThisCurvatureL = dbaddr->ThisCurvatureL - 65535;
	dbaddr->CurvatureL = dbaddr->ThisCurvatureL * 0.0000005;  //실제DBC Factor 0.000001 보다 1/2
	dbaddr->ThisCurvatureRateL = LaneInfoRxData[10]+(LaneInfoRxData[11]<<8);
	if (dbaddr->ThisCurvatureRateL > 32767)
		dbaddr->ThisCurvatureRateL = dbaddr->ThisCurvatureRateL - 65535;
	dbaddr->CurvatureRateL = dbaddr->ThisCurvatureRateL * 0.0000000020; //실제DBC Factor 0.000000004 보다 1/2

	dbaddr->LaneQualityR = (LaneInfoRxData[12]&0x07);
	dbaddr->ThisLanePositionR = ((LaneInfoRxData[12]&0xE0)>>5)+(LaneInfoRxData[13]<<3)+((LaneInfoRxData[14]&0x07)<<11);
	if (dbaddr->ThisLanePositionR > 8192)
		dbaddr->ThisLanePositionR = dbaddr->ThisLanePositionR-16384;
	dbaddr->LanePositionR = dbaddr->ThisLanePositionR * 0.0039625;
	dbaddr->ThisHeadingAngleR = ((LaneInfoRxData[14]&0xF8)>>3)+((LaneInfoRxData[15]&0x1F)<<5);
	if (dbaddr->ThisHeadingAngleR > 512)
		dbaddr->ThisHeadingAngleR = dbaddr->ThisHeadingAngleR - 1024;
	dbaddr->HeadingAngleR =  dbaddr->ThisHeadingAngleR * 0.000976563;
	dbaddr->ThisCurvatureR = LaneInfoRxData[16]+(LaneInfoRxData[17]<<8);
	if (dbaddr->ThisCurvatureR > 32767)
		dbaddr->ThisCurvatureR = dbaddr->ThisCurvatureR - 65535;
	dbaddr->CurvatureR = dbaddr->ThisCurvatureR * 0.0000005;
	dbaddr->ThisCurvatureRateR = (LaneInfoRxData[18]+(LaneInfoRxData[19]<<8));
	if (dbaddr->ThisCurvatureRateR > 32767)
		dbaddr->ThisCurvatureRateR = dbaddr->ThisCurvatureRateR - 65535;
	dbaddr->CurvatureRateR = dbaddr->ThisCurvatureRateR * 0.0000000020;

	if ((dbaddr->LanePositionL > -2.0) && (dbaddr->LanePositionL != 0.0) && (dbaddr->LanePositionR < 2.0) && (dbaddr->LanePositionR != 0.0))
	{
		dbaddr->LaneWidthEstimation = dbaddr->LanePositionR - dbaddr->LanePositionL;
	}
	else if ((dbaddr->LanePositionL < -2.0) || (dbaddr->LanePositionL == 0.0))
	{
		dbaddr->LanePositionL = dbaddr->LanePositionR - dbaddr->LaneWidthEstimation;
	}
	else if ((dbaddr->LanePositionR > 2.0) || (dbaddr->LanePositionR == 0.0))
	{
		dbaddr->LanePositionR = dbaddr->LaneWidthEstimation + dbaddr->LanePositionL;
	}

	dbaddr->CAMStatus  = 1;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_CLU_01_20ms2);
}

// can1 - 0x1BA
void RxCAN1_RR_C_RDR_02_50ms(uint8_t *RR_C_RDR_02_50ms)
{
	DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms);

	dbaddr->LKALHLnWaringState = (RR_C_RDR_02_50ms[3]&0xC0)>>6;   //1 means Object, 2 means LaneChange in Object
	dbaddr->LKARHLnWaringState = RR_C_RDR_02_50ms[4]&0x03;        //1 means Object, 2 means LaneChange in Object

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_RR_C_RDR_02_50ms);
}

// can1 - 0x1cf
void RxCAN1_SWRC_03_20ms(uint8_t *SWRC_03_20ms)
{
	DB_IONIQ_RxCAN1_SWRC_03_20ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_SWRC_03_20ms);

	dbaddr->SWRC_CrsMainSwSta = (SWRC_03_20ms[2] & 0x18) >> 3;
	dbaddr->SWRC_PlusMinusSwSta = SWRC_03_20ms[2] & 0x07; // signal name in E-CAN dbc file is "SWRC_CrsSwSta"
	dbaddr->PlusSW = dbaddr->SWRC_PlusMinusSwSta & 0x01;
	dbaddr->MinusSW = (dbaddr->SWRC_PlusMinusSwSta & 0x02) >> 1;
	dbaddr->PauseResumeSW = (dbaddr->SWRC_PlusMinusSwSta & 0x04) >> 2;

	if((dbaddr->SWRC_CrsMainSwSta == 1) && (dbaddr->SCC_OpSta != 1) && (dbaddr->SCC_StayTune == 0)) // to start ACC
	{
		dbaddr->SCC_OpSta = 1;
		dbaddr->SCC_MainOnOffSta = 1;
		dbaddr->SCC_StayTune = 1;

		if(dbaddr->CurrentSpeed <= 50)
			dbaddr->SCC_TrgtSpdSetVal = 50; // reset ACC setting speed
		else
			dbaddr->SCC_TrgtSpdSetVal = dbaddr->CurrentSpeed;

		dbaddr->SCC_AccelReqVal = 1023; 	// reset previous value to default
		dbaddr->SCC_AccelReqRawVal = 1023;  // reset previous value to default
		katech_adas_mode=1;
	}
	else if ((dbaddr->SWRC_CrsMainSwSta == 1) && (dbaddr->SCC_OpSta == 1) && (dbaddr->SCC_StayTune == 0) || (katech_adas_mode<0)) // to finish ACC
	{
		dbaddr->SCC_OpSta = 0;
		dbaddr->SCC_MainOnOffSta = 0;
		dbaddr->SCC_StayTune = 1;
		//SCC_TrgtSpdSetVal = 0;
		dbaddr->SCC_AccelReqVal = 1023; 	// reset previous value to default
		dbaddr->SCC_AccelReqRawVal = 1023;  // reset previous value to default
		katech_adas_mode=0;
	}

	if (dbaddr->SWRC_CrsMainSwSta == 0)
	{
		dbaddr->SCC_StayTune = 0;
	}

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_SWRC_03_20ms);
}

// can1 - 0x3c1
void RxCAN1_MFSW_01_200ms(uint8_t *MFSW_01_200ms)
{
	DB_IONIQ_RxCAN1_MFSW_01_200ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN1_MFSW_01_200ms);

	dbaddr->TurnSigLeft = (MFSW_01_200ms[3]&0xC0)>>6;
	dbaddr->TurnSigRight = MFSW_01_200ms[4]&0x03;

	if (dbaddr->TurnSigLeft == 1)
	{
		dbaddr->LeftTurnFlag = 1;
		dbaddr->RightTurnFlag = 0;
	}
	else if (dbaddr->TurnSigRight == 1)
	{
		dbaddr->LeftTurnFlag = 0;
		dbaddr->RightTurnFlag = 1;
	}
	else
	{
		dbaddr->LeftTurnFlag = 0;
		dbaddr->RightTurnFlag = 0;
	}

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN1_MFSW_01_200ms);
}


/*
 * rx CAN2(MDPS)
 */
// can2 - 0xEA
uint8_t g_SPASStatus=3;
void RxCAN2_MDPS_01_10ms(uint8_t *MDPS_RxData)
{
	DB_IONIQ_RxCAN2_MDPS_01_10ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN2_MDPS_01_10ms);

	dbaddr->MDPS_PaModeSta = MDPS_RxData[5]&0x0F;

	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN2_MDPS_01_10ms);
}

// can2 - 0x377
void RxCAN2_Unknown_0x377(uint8_t *CMD_RxData)
{
	uint8_t CMD = 0;
	uint8_t pos = 0;
	DB_IONIQ_RxCAN2_Unknown_0x377 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377);

	CMD = CMD_RxData[4];
	if(CMD == 0x01) dbaddr->SPASStatus = 3;
	g_SPASStatus =3;
}

// can2 - 0x378
void RxCAN2_Unknown_0x378(uint8_t *CMD_RxData)
{
	uint8_t CMD = 0;
	uint8_t pos = 0;
	DB_IONIQ_RxCAN2_Unknown_0x377 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377);

	CMD = CMD_RxData[5];
	if(CMD == 0x01) dbaddr->SPASStatus = 4;
	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377);
}

// can2 - 0x379
void RxCAN2_Unknown_0x379(uint8_t *CMD_RxData)
{
	uint8_t CMD = 0;
	uint8_t pos = 0;
	DB_IONIQ_RxCAN2_Unknown_0x377 *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377);

	CMD = CMD_RxData[6];
	if(CMD == 0x01) dbaddr->SPASStatus = 1;
	if(CMD == 0x02) dbaddr->SPASStatus = 2;
	if(CMD == 0x05) dbaddr->SPASStatus = 5;
	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN2_Unknown_0x377);
}


/*
 * rx CAN3(DRV)
 */
// can3 - 0x1EA
uint8_t ADAS_CMD_32_50ms[4][32];
uint8_t ADAS_CMD_32_50ms_cnt=0;
//ADAS_CMD_20_20ms
void RxCAN3_ADAS_CMD_20_20ms(uint8_t *data)
{
	//uint8_t ADAS_CMD_32_50ms[32];
	uint8_t i;
	uint8_t this_cnt;
	this_cnt = ADAS_CMD_32_50ms_cnt%4;
	//if(ADAS_CMD_32_50ms_cnt>=4) this_cnt=0;

	memcpy(ADAS_CMD_32_50ms[this_cnt],data,32);

	ADAS_CMD_32_50ms_cnt=this_cnt;
}

// can3 - 0x1A0
uint8_t TempArray1A0[4][32];
uint8_t TempArray1A0_ori[4][32];
uint8_t TempArray1A0_cnt =0;
void RxCAN3_ADAS_CMD_32_50ms(uint8_t *ADAS_CMD_20_20ms_Temp)
{
	//uint8_t TempArray1A0[32]; //ADAS_CMD_20_20ms_Temp[32]; //<-temparray1a0[32] -> M7
	//uint8_t SCC_ObjSta, SCC_VehStpReq, RADARStatus=0;
	char Temp1A0_PushData;
	//uint16_t FrontSCC_ObjDstVal = 0x7FE;
	//float RadarSCC_ObjRelSpdVal = 0;
	uint8_t pos2 = 0;
	DB_IONIQ_RxCAN3_ADAS_CMD_32_50ms *dbaddr;
	dbaddr = katech_database_write_addr_get(KATECH_NAME_DB_IONIQ_RxCAN3_ADAS_CMD_32_50ms);

	pos2 = TempArray1A0_cnt+1;
	if(pos2>=4) pos2 =0;

	memcpy(TempArray1A0_ori[pos2],ADAS_CMD_20_20ms_Temp,32);

	TempArray1A0[pos2][3] = ADAS_CMD_20_20ms_Temp[3];
	TempArray1A0[pos2][4] = ADAS_CMD_20_20ms_Temp[4];
	TempArray1A0[pos2][5] = ADAS_CMD_20_20ms_Temp[5];
	TempArray1A0[pos2][6] = ADAS_CMD_20_20ms_Temp[6];
	TempArray1A0[pos2][7] = ADAS_CMD_20_20ms_Temp[7];
	//byte8 controls SCC_OpSta & SCC_MainOnOffSta
	TempArray1A0[pos2][9] = ADAS_CMD_20_20ms_Temp[9];
	TempArray1A0[pos2][10] = ADAS_CMD_20_20ms_Temp[10];
	TempArray1A0[pos2][11] = ADAS_CMD_20_20ms_Temp[11];
	//byte12 controls SCC_TrgtSpdSetVal
	//TempArray1A0[13] = ADAS_CMD_20_20ms_Temp[13];
	dbaddr->SCC_ObjSta = (ADAS_CMD_20_20ms_Temp[13] & 0x70) >> 4;
	TempArray1A0[pos2][13] = 0x08; // + TempArray1A0[13];
	TempArray1A0[pos2][14] = ADAS_CMD_20_20ms_Temp[14];
	TempArray1A0[pos2][15] = ADAS_CMD_20_20ms_Temp[15];
	// SCC_AccelReqVal & SCC_AccelReqRawVal can read from byte16,17,18
	for(Temp1A0_PushData = 21; Temp1A0_PushData < 32; Temp1A0_PushData++)
	{
		if(Temp1A0_PushData == 23) // byte23 controls SCC_VehStpReq
			continue;
		TempArray1A0[pos2][Temp1A0_PushData] = ADAS_CMD_20_20ms_Temp[Temp1A0_PushData];
		// SCC_JrkUpp & SCC_JrkLwr can read from byte19,20
	}
	dbaddr->FrontSCC_ObjDstVal = ADAS_CMD_20_20ms_Temp[3] + ((ADAS_CMD_20_20ms_Temp[4] & 0x07) << 8);
	//dbaddr->FrontSCC_ObjDstVal = dbaddr->FrontSCC_ObjDstVal*0.1;
	//dbaddr->FrontSCC_ObjDstVal=777;
	//SCC_ObjDstVal = SCC_ObjDstVal * 0.1;
	dbaddr->RadarSCC_ObjRelSpdVal = ((ADAS_CMD_20_20ms_Temp[4] & 0xF8) >> 3) + ((ADAS_CMD_20_20ms_Temp[5] & 0x7F) << 5);
	dbaddr->RadarSCC_ObjRelSpdVal = (dbaddr->RadarSCC_ObjRelSpdVal - 1700) * 0.1;

	dbaddr->SCC_VehStpReq = (ADAS_CMD_20_20ms_Temp[23] & 0x03);
	dbaddr->RADARStatus = 1;
	katech_database_update(KATECH_NAME_DB_IONIQ_RxCAN3_ADAS_CMD_32_50ms);
	TempArray1A0_cnt=pos2;
}


uint8_t ADAS_CMD_10_20ms_data[4][16];
uint8_t ADAS_CMD_10_20ms_cnt =0;
void RxCAN3_ADAS_CMD_10_20ms(uint8_t *ADAS_CMD_10_20ms)
{
	uint16_t CameraFCA_TimetoCllsn = 0;
	uint8_t pos;

	pos = (ADAS_CMD_10_20ms_cnt+1)%4;

	memcpy(ADAS_CMD_10_20ms_data[pos],ADAS_CMD_10_20ms,16);
	ADAS_CMD_10_20ms_cnt=pos;
	//CAN_MB_READ_DATA(CAN3_BUF[25], ADAS_CMD_20_20ms_Temp, 16);
	//CAN_MB_WRITE_DATA(CAN1_BUF[25], ADAS_CMD_10_20ms, 16);    //Test for AEB on S32G
	CameraFCA_TimetoCllsn = (((ADAS_CMD_10_20ms[10]&0x01)<<7) + (ADAS_CMD_10_20ms[9]&0xFE)>>1)*10;   //ms
}


uint8_t ADAS_PRK_20_20ms_data[4][24];
uint8_t ADAS_PRK_20_20ms_cnt =0;
void RxCAN0_ADAS_PRK_20_20ms(uint8_t *ADAS_PRK_20_20ms)
{
	uint8_t pos;

	pos = (ADAS_PRK_20_20ms_cnt+1)%4;

	memcpy(ADAS_PRK_20_20ms_data[pos],ADAS_PRK_20_20ms,24);
	ADAS_PRK_20_20ms_cnt=pos;
	//CAN_MB_READ_DATA(CAN3_BUF[25], ADAS_CMD_20_20ms_Temp, 16);
	//CAN_MB_WRITE_DATA(CAN1_BUF[25], ADAS_CMD_10_20ms, 16);    //Test for AEB on S32G
	//CameraFCA_TimetoCllsn = (((ADAS_CMD_10_20ms[10]&0x01)<<7) + (ADAS_CMD_10_20ms[9]&0xFE)>>1)*10;   //ms
}


/**************************************************************************************************
 *                                        IONIQ5 TX FUNCTIONS
 **************************************************************************************************/


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

	if (rx_frame_type == DATA_FRAME) // CAN to AP
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

				// 241111 sy.kim
				AP_rxcanId = rx_can_id;
				if(route_id == 160 || AP_rxcanId == 1)
				{
					mcu_to_ap_flag = 1;
				}
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
					// cnvt_data.id = rx_extCan_id;
					cnvt_data.id = route_id; // 241114 sy.kim
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
	else // CAN to CAN
	{
		ts_frame.port = rx_sourcePort;
		ts_frame.ts_us_high = rx_timeStamp_us_H;
		ts_frame.ts_us_low = rx_timeStamp_us_L;
		ts_frame.ts_ns = rx_timeStamp_ns;
		
		if (fLpaTsCB != NULL)
		{
			fLpaTsCB(&ts_frame);
		}

		mcu_to_ap_flag = 0; // 241111 sy.kim
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
static void LPA_Rx_Task(void *pArg) // When M7-NP receive CAN data
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

#if 1 
/* 241112 sy.kim MCU to AP time test*/
	uint32 temp;
	volatile uint32 TC32MCNT_Val[250];
	volatile uint32 TC32Prescale_Val[250];	
	uint32 TC32EN_Addr = 0x4B400080;
	
	temp = SAL_ReadReg(TC32EN_Addr);
	temp = 0;
	temp &= (uint32)(0UL);			// System Timer 0 32-Bit Timer Init
	temp &= (uint32)~(1 << 24);		// Counter Disable
	temp &= (uint32)~(1 << 29);		// LDM1 = 0
	temp |= (uint32)(1 << 28);		// LDM0 = 1
	temp |= (uint32)(1 << 26);		// Oneshot Mode
	temp |= (uint32)(1 << 25);		// Counter Start from zero (not LOADVAL)
	//temp |= (uint32)(0x3A980);		// Prescaler Load Value = 240000
	temp |= (uint32)(0x17);		// Prescaler Load Value = 24

	SAL_WriteReg(temp, TC32EN_Addr);

	////////////////////////////////////////////
	//// TC32LDV
	SAL_WriteReg(0xffffffff, 0x4B400084);		// LOADVAL

	////////////////////////////////////////////
	//// TC32CMP0
	SAL_WriteReg(0xffffffff, 0x4B400088);		// comparison value
	if (add > 250)
	{
		add = 0;
	}
#endif

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
// CAN data가 receive 될 때, M7-NP에서 AP로 Data 송신
#if (IPC_A65_TEST == 1)
							if(mcu_to_ap_flag == 1) // 241111 sy.kim
							{
								(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 24);		// Counter Enable

								(void)IPC_SendPacket(IPC_CH_CA65_NS_USER, (uint16)TCC_IPC_CMD_AP_TEST, TCC_IPC_CMD_AP_SEND, rxResponseBuffer, proc_dataLen);

								(*((volatile uint32 *)(0x4B400080))) &= (uint32)~(1 << 24); 	// Counter Disable
								TC32MCNT_Val[add] = SAL_ReadReg(0x4B400094);		// System timer 0, 32Bit Timer Main count
								TC32Prescale_Val[add] = SAL_ReadReg(0x4B400090);	// System timer 0, 32Bit Timer Prescale count
								mcu_printf("MCU to AP Execution Time: %d.%03d us\n",TC32MCNT_Val[add],((TC32Prescale_Val[add]*41666667)/1000000)); //QAC
							}
							add++;
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
static void LPA_Tx_Task(void *pArg) // When AP send M7-NP
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
	mcu_printf("M7-NP Receive Data from AP\n");
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

		// 241106 sy.kim
		mcu_printf("**************************************\n");
		mcu_printf("CAN data Port %d -> ", msg.port);

		// 241111 sy.kim
		/*
			Change route from M7-NP to CAN path
		*/
		if(msg.port == 1 && AP_rxcanId == 160)
		{
			msg.port = 3;
			msg.idt = lpa_u16add((uint16)msg.port, CAN_PORT_MASK);
		}
		
		mcu_printf("Port %d, RouteID 0x%X, CAN ID 0x%X, Msg len %d\n", msg.port, msg.idt, AP_rxcanId, msg.size);
		mcu_printf("Data ");
		for (int i=0; i<msg.size; i++)
		{
			mcu_printf("0x%X ", msg.MsgData[i]);
		}
		mcu_printf("\n**************************************\n");

		(void)SAL_QueuePut(gLpaTxQueueHandle, (void *)&msg, sizeof(LPA_TX_MESSAGE), 0, SAL_OPT_BLOCKING); // CAN으로 전송
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
