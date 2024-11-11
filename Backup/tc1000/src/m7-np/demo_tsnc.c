/*
***************************************************************************************************
*
*   FileName : app_tsnc.c
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

#include "app_cfg.h"
#if (SIC_BSP_SUPPORT_TEST_APP_TPA == 1)
#include "rt_tbl.h"
#include "demo_tsnc.h"
#include <sys/time.h> // 241106 sy.kim

#include "tpa_drv.h"

/**************************************************************************************************
*                                            DEFINITIONS
**************************************************************************************************/


/**************************************************************************************************
*                                          LOCAL VARIABLES
**************************************************************************************************/
#define MAC_SIZE       6u
#define VLAN_TYPE      0x81u
#define UDP_TYPE       0x11u
#define VLAN_OFFSET    4u
#define CAN_FD_PORT    6u
/*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "28U" (unsigned) is not the same as that of the right operand "7"(signed).*/
#define CAN_HEADER     7u

#define IP_HEAD_SIZE    sizeof(IPHeader_t)
#define UDP_HEAD_SIZE   sizeof(UDPHeader_t)

#define IP_HEAD_SUM    9u

//static uint8 TSNC_Task_cnt;

static EthFrame_t ethFrame_tx;
static EthFrame_t ethFrame_rx;

static uint16 ethSize;
static uint16 rxEthSize;

static uint16 ip_header_sum[9]; // ip header�� 20����Ʈ -> word�� 10, ip checksum ���� 9

/*misra_c_2012_rule_8_4_violation:	Object definition does not have a visible prototype.*/
static CANMessage_t TSNC_CANMsg;
/*misra_c_2012_rule_5_9_violation:	Identifier "gSN" is already used to represent an object with internal linkage.*/
static uint16 gSN_tsnc;
/*[misra_c_2012_rule_5_9_violation] internal_linkage_symbol:	Declaring an internal linkage object with identifier "txRequestBuffer".*/
static uint8 txReqBuff_tsnc[128];

// 241106 sy.kim
struct timeval start, end;
long elapsed_time;

/**************************************************************************************************
*                                        FUNCTION PROTOTYPES
**************************************************************************************************/
static void TSNC_Uart_list(void);
static uint8 get_dlc(uint8 data_len);
//static uint8 get_CAN_length(uint8 dlc);
static uint16 trans_8_to_16(uint8 h_byte, uint8 l_byte);
static uint16 cal_checksum(uint16 header[], uint8 arr_size);

//static void TSNC_EthToCAN(void);
//static void TSNC_CANToEth(void);

/**************************************************************************************************
*                                             FUNCTIONS
**************************************************************************************************/
void TSNC_Init(void)
{
    add_TSNC_routeTable();
    /* Temp UDP Packet */
	(void)SAL_MemCopy(ethFrame_tx.src, np_addrInfo->dstMac, sizeof(np_addrInfo->dstMac));

    ethFrame_tx.type[0] = 0x08u;
    ethFrame_tx.type[1] = 0x00u;

	ethFrame_tx.ipHeader.versionHeaderLength = 0x45u;
	ethFrame_tx.ipHeader.service = 0x01u;
	ethFrame_tx.ipHeader.length[0] = 0x00u;
	ethFrame_tx.ipHeader.length[1] = 0x00u;
	ethFrame_tx.ipHeader.identification[0] = 0x00u;
	ethFrame_tx.ipHeader.identification[1] = 0x14u;
	ethFrame_tx.ipHeader.fragmentOffset[0] = 0x00u;
	ethFrame_tx.ipHeader.fragmentOffset[1] = 0x00u;
	ethFrame_tx.ipHeader.timeToLive = 0xeeu;
	ethFrame_tx.ipHeader.protocol = 0x11u;
	ethFrame_tx.ipHeader.headerChecksum[0] = 0x90u;
	ethFrame_tx.ipHeader.headerChecksum[1] = 0x73u;
	ethFrame_tx.ipHeader.srcIPAddr[0] = 0x28u;
	ethFrame_tx.ipHeader.srcIPAddr[1] = 0x28u;
	ethFrame_tx.ipHeader.srcIPAddr[2] = 0x28u;
	ethFrame_tx.ipHeader.srcIPAddr[3] = 0x28u;

	(void)SAL_MemCopy(ethFrame_tx.udpHeader.srcPort, np_addrInfo->dstIp , sizeof(np_addrInfo->dstIp));
	//ethFrame_tx.udpHeader.srcPort[0] = 0x01;
	//ethFrame_tx.udpHeader.srcPort[1] = 0x01;
	ethFrame_tx.udpHeader.length[0] = 0x00u;
	ethFrame_tx.udpHeader.length[1] = 23u;
	ethFrame_tx.udpHeader.checksum[0] = 0xb9u;
	ethFrame_tx.udpHeader.checksum[1] = 0x8cu;
}

static uint8 get_dlc(uint8 data_len)
{
    uint8 dlc = 8u;
    //uint8 rxDataLen = data_len - 16;

	/*misra_c_2012_rule_15_6_violation:	The body of the "else" branch of the "if" statement is not a compound statement.*/
	/*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "data_len" (unsigned) is not the same as that of the right operand "8"(signed)*/
    if(data_len <= 8u)
    {
        dlc = data_len;
    }
    else if(data_len <= 24u)
    {
        dlc += (data_len / 4u) - 2u;
	}
    else if(data_len == 32u)
	{
        dlc = 13u;
	}
    else if(data_len == 48u)
	{
        dlc = 14u;
	}
    else if(data_len == 64u)
	{
        dlc = 15u;
	}
    else
    {
    /*cert_int02_c_violation: Casting "-1" from "int" to "unsigned char" without checking its value may result in lost or misinterpreted data.*/
        dlc = 0u;
	}
    return dlc;
}

#if 0
static uint8 get_CAN_length(uint8 dlc)
{
    uint8 len;

    if (dlc <= 8)
        len = dlc;
    else if (dlc== 9)
        len = 12;
    else if (dlc == 10)
        len = 16;
    else if (dlc == 11)
        len = 20;
    else if (dlc == 12)
        len = 24;
    else if (dlc == 13)
        len = 32;
    else if (dlc == 14)
        len = 48;
    else if (dlc == 15)
        len = 64;

    return len;
}
#endif

static uint16 trans_8_to_16(uint8 h_byte, uint8 l_byte)
{
   uint16 result = 0u;

   result = ((uint16)(h_byte) << 8u) + l_byte;

   return result;
}

static uint16 cal_checksum(uint16 header[], uint8 arr_size)
{
    uint32 sum = 0u;
    uint8 carry_bit = 0u;
    uint16 result = 0u;
    uint16 checksum = 0u;

    for(uint8 i = 0u; i < arr_size; i++)
    {
        sum += header[i];
    }

    carry_bit = (sum & 0xF0000u) >> 16u;
    result = (sum & 0xFFFFu) + carry_bit;

    checksum = ~result;
    checksum &= 0xFFFFu;

    return checksum;
}

void cb_tsnc_eth_to_can_cnvt(const TSNC_ETH_DATA *eth_data)
{
    uint8 dlc = 0u;
    uint8 data_length = 0u;
    uint8 vlan_index = 0u;

    /* Temp UDP Packet */
    for(uint16 i = 0u; i < MAC_SIZE; i++)
    {
        ethFrame_rx.dst[i] = eth_data->msg_data[i];
    }

    for(uint16 i = 0u; i < MAC_SIZE; i++)
    {
        ethFrame_rx.src[i] = eth_data->msg_data[i + MAC_SIZE];
    }

    if(eth_data->msg_data[12] == VLAN_TYPE)
    {
        vlan_index = VLAN_OFFSET;
    }
    else
    {
        vlan_index = 0u;
    }

    ethFrame_rx.type[0] = eth_data->msg_data[12u + vlan_index];
    ethFrame_rx.type[1] = eth_data->msg_data[13u + vlan_index];

    ethFrame_rx.ipHeader.versionHeaderLength = eth_data->msg_data[14u + vlan_index];;
    ethFrame_rx.ipHeader.service = eth_data->msg_data[15u + vlan_index];
    ethFrame_rx.ipHeader.length[0] = eth_data->msg_data[16u + vlan_index];
    ethFrame_rx.ipHeader.length[1] = eth_data->msg_data[17u + vlan_index];
    ethFrame_rx.ipHeader.identification[0] = eth_data->msg_data[18u + vlan_index];
    ethFrame_rx.ipHeader.identification[1] = eth_data->msg_data[19u + vlan_index];
    ethFrame_rx.ipHeader.fragmentOffset[0] = eth_data->msg_data[20u + vlan_index];
    ethFrame_rx.ipHeader.fragmentOffset[1] = eth_data->msg_data[21u + vlan_index];
    ethFrame_rx.ipHeader.timeToLive = eth_data->msg_data[22u + vlan_index];
    ethFrame_rx.ipHeader.protocol = eth_data->msg_data[23u + vlan_index];
    ethFrame_rx.ipHeader.headerChecksum[0] = eth_data->msg_data[24u + vlan_index];
    ethFrame_rx.ipHeader.headerChecksum[1] = eth_data->msg_data[25u + vlan_index];
    ethFrame_rx.ipHeader.srcIPAddr[0] = eth_data->msg_data[26u + vlan_index];
    ethFrame_rx.ipHeader.srcIPAddr[1] = eth_data->msg_data[27u + vlan_index];
    ethFrame_rx.ipHeader.srcIPAddr[2] = eth_data->msg_data[28u + vlan_index];
    ethFrame_rx.ipHeader.srcIPAddr[3] = eth_data->msg_data[29u + vlan_index];
    ethFrame_rx.ipHeader.dstIPAddr[0] = eth_data->msg_data[30u + vlan_index];
    ethFrame_rx.ipHeader.dstIPAddr[1] = eth_data->msg_data[31u + vlan_index];
    ethFrame_rx.ipHeader.dstIPAddr[2] = eth_data->msg_data[32u + vlan_index];
    ethFrame_rx.ipHeader.dstIPAddr[3] = eth_data->msg_data[33u + vlan_index];

    ethFrame_rx.udpHeader.srcPort[0] = eth_data->msg_data[34u + vlan_index];
    ethFrame_rx.udpHeader.srcPort[1] = eth_data->msg_data[35u + vlan_index];
    ethFrame_rx.udpHeader.dstPort[0] = eth_data->msg_data[36u + vlan_index];
    ethFrame_rx.udpHeader.dstPort[1] = eth_data->msg_data[37u + vlan_index];
    ethFrame_rx.udpHeader.length[0] = eth_data->msg_data[38u + vlan_index];
    ethFrame_rx.udpHeader.length[1] = eth_data->msg_data[39u + vlan_index];
    ethFrame_rx.udpHeader.checksum[0] = eth_data->msg_data[40u + vlan_index];
    ethFrame_rx.udpHeader.checksum[1] = eth_data->msg_data[41u + vlan_index];

    ethFrame_rx.payload.ecuId = eth_data->msg_data[42u + vlan_index];
    ethFrame_rx.payload.extId= eth_data->msg_data[43u + vlan_index];
    ethFrame_rx.payload.id[0] = eth_data->msg_data[44u + vlan_index];
    ethFrame_rx.payload.id[1] = eth_data->msg_data[45u + vlan_index];
    ethFrame_rx.payload.id[2] = eth_data->msg_data[46u + vlan_index];
    ethFrame_rx.payload.id[3] = eth_data->msg_data[47u + vlan_index];
    /*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "48" (signed) is not the same as that of the right operand "vlan_index"(unsigned).*/
    ethFrame_rx.payload.dlc = eth_data->msg_data[48u + vlan_index];        // HERE Change

    if((ethFrame_rx.type[0] == 0x08u) &&
            (ethFrame_rx.type[1] == 0x00u) &&
            (ethFrame_rx.ipHeader.protocol == UDP_TYPE))
    {
        dlc = ethFrame_rx.payload.dlc;
        data_length = frm_get_CAN_length(dlc);

        rxEthSize = sizeof(ethFrame_rx);

        /* Random fill of can data area */
        //    for(uint16 i = CAN_DATA; i < CAN_DATA + data_size; i++)
        for(uint16 i = 0; i < data_length; i++)
        {
            ethFrame_rx.payload.data[i] = eth_data->msg_data[i + 49u + vlan_index];
        }

        /* Make CetraC Format */
        TSNC_CANMsg.portNum = ethFrame_rx.payload.ecuId;
        TSNC_CANMsg.mExtendedId = ethFrame_rx.payload.extId;
        TSNC_CANMsg.mId = ((uint32)ethFrame_rx.payload.id[0] << 24u)
            |((uint32)ethFrame_rx.payload.id[1] << 16u)
            |((uint32)ethFrame_rx.payload.id[2] << 8u)
            |((uint32)ethFrame_rx.payload.id[3]);
        TSNC_CANMsg.mRemoteTransmitRequest = 0u;
        /*misra_c_2012_rule_12_1_violation:	Missing explicit parentheses on sub-expression: "TSNC_CANMsg.portNum < 6".*/
        TSNC_CANMsg.mFDFormat = (TSNC_CANMsg.portNum < CAN_FD_PORT) ? 0 : 1;
        TSNC_CANMsg.mBitRateSwitching = TSNC_CANMsg.mFDFormat ? 1 : 0;

        (void*)SAL_MemCopy(TSNC_CANMsg.mData, ethFrame_rx.payload.data, data_length);

        // 241106 sy.kim
        gettimeofday(&start, NULL);
        frm_cnvt_msg_send(TSNC_CANMsg.portNum, TSNC_CANMsg.mFDFormat, TSNC_CANMsg.mExtendedId, TSNC_CANMsg.mId, data_length, TSNC_CANMsg.mData);
        gettimeofday(&end, NULL);
	    elapsed_time = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	    mcu_printf("ETH to CAN Convert Time: %ld us\n", elapsed_time);
    }
    else
    {
        mcu_printf("[TSNC_ERROR_%d] This packet is not UDP type.\n", __LINE__);
    }
}

static void set_ethDstAddrInfo(uint8 rx_sourcePort, uint32 route_id)
{
    uint8 src_ch;
    uint8 i;

    src_ch = rx_sourcePort - 5u;

    for(i = 0; i < CAN2ETH_NUM; i++)
    {
        if((src_ch == can2ethTbl[i].src_ch) && (route_id == can2ethTbl[i].src_id))
        {
            SAL_MemCopy(ethFrame_tx.dst, can2ethTbl[i].tpa_dst.dstMac, 6);
            SAL_MemCopy(ethFrame_tx.ipHeader.dstIPAddr, can2ethTbl[i].tpa_dst.dstIp, 4);
        }
    }
}

void cb_tsnc_can_to_eth_cnvt(const CNVT_CAN_DATA *can_data)
{
/*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "can_data->len" (unsigned) is not the same as that of the right operand "15"(signed).*/
    uint8 rxDataLen = can_data->len - 15u;
    uint16 ip_total_len = 0u;
    uint16 udp_len = 0u;
    /*misra_c_2012_rule_5_8_violation:	Identifier "ip_checksum" is already used to represent a function with external linkage.*/
    uint16 ip_chksum = 0u;
    uint16 udp_checksum = 0u;
    uint16 udp_header_sum[9u + CAN_HEADER + 64u];
    uint8 checksum_payload_size = (CAN_HEADER + rxDataLen) / 2u;
    uint8 udp_checksum_size = 9u + CAN_HEADER + rxDataLen;
    uint32 tbl_id;

    SAL_MemSet(ip_header_sum, 0x00u, sizeof(ip_header_sum));
    SAL_MemSet(udp_header_sum, 0x00u, sizeof(udp_header_sum));

    if(can_data->ide == STANDARD_CAN)
    {
        tbl_id = can_data->id;
    }
    else
    {
        tbl_id = (can_data->id | 0x80000000u);
    }

    set_ethDstAddrInfo(can_data->port, tbl_id);

	ethFrame_tx.udpHeader.dstPort[0] = 0x02u;
	ethFrame_tx.udpHeader.dstPort[1] = 0x02u;

    /* IPv4 Header Total Length 2byte Field */
    /*	misra_c_2012_rule_10_6_violation:	Assigning composite expression "35U + rxDataLen" of width 8 to a target of width 16.*/
    ip_total_len = IP_HEAD_SIZE + UDP_HEAD_SIZE + CAN_HEADER + (uint16)rxDataLen;
    udp_len = ip_total_len - IP_HEAD_SIZE;

	ethFrame_tx.ipHeader.length[0] = (uint8)((ip_total_len & 0xFF00u) >> 8u);
	ethFrame_tx.ipHeader.length[1] = (uint8)(ip_total_len & 0xFFu);

	ethFrame_tx.udpHeader.length[0] = (uint8)((udp_len & 0xFF00u) >> 8u);
	ethFrame_tx.udpHeader.length[1] = (uint8)(udp_len & 0xFFu);

    /* IP Checksum */
    ip_header_sum[0] = trans_8_to_16(ethFrame_tx.ipHeader.versionHeaderLength, ethFrame_tx.ipHeader.service);
    ip_header_sum[1] = trans_8_to_16(ethFrame_tx.ipHeader.length[0], ethFrame_tx.ipHeader.length[1]);
    ip_header_sum[2] = trans_8_to_16(ethFrame_tx.ipHeader.identification[0], ethFrame_tx.ipHeader.identification[1]);
    ip_header_sum[3] = trans_8_to_16(ethFrame_tx.ipHeader.fragmentOffset[0], ethFrame_tx.ipHeader.fragmentOffset[1]);
    ip_header_sum[4] = trans_8_to_16(ethFrame_tx.ipHeader.timeToLive, ethFrame_tx.ipHeader.protocol);
    ip_header_sum[5] = trans_8_to_16(ethFrame_tx.ipHeader.srcIPAddr[0], ethFrame_tx.ipHeader.srcIPAddr[1]);
    ip_header_sum[6] = trans_8_to_16(ethFrame_tx.ipHeader.srcIPAddr[2], ethFrame_tx.ipHeader.srcIPAddr[3]);
    ip_header_sum[7] = trans_8_to_16(ethFrame_tx.ipHeader.dstIPAddr[0], ethFrame_tx.ipHeader.dstIPAddr[1]);
    ip_header_sum[8] = trans_8_to_16(ethFrame_tx.ipHeader.dstIPAddr[2], ethFrame_tx.ipHeader.dstIPAddr[3]);

    ip_chksum = cal_checksum(ip_header_sum, sizeof(ip_header_sum)/2u);

    ethFrame_tx.ipHeader.headerChecksum[0] = (uint8)((ip_chksum & 0xFF00u) >> 8u);
    ethFrame_tx.ipHeader.headerChecksum[1] = (uint8)(ip_chksum & 0xFFu);

	/*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "can_data->port" (unsigned) is not the same as that of the right operand "5"(signed).*/
    ethFrame_tx.payload.ecuId = can_data->port - 5u;
    ethFrame_tx.payload.extId = can_data->ide;
    /*misra_c_2012_rule_7_2_violation:	Numeric literal "4278190080U" is unsigned but does not use a "u" or "U" suffi*/
    ethFrame_tx.payload.id[0] = (uint8)((can_data->id & 0xFF000000u) >> 24u);
    ethFrame_tx.payload.id[1] = (uint8)((can_data->id & 0x00FF0000u) >> 16u);
    ethFrame_tx.payload.id[2] = (uint8)((can_data->id & 0x0000FF00u) >> 8u);
    ethFrame_tx.payload.id[3] = (uint8)(can_data->id & 0x000000FFu);
    ethFrame_tx.payload.dlc = get_dlc(rxDataLen);
    SAL_MemCopy(ethFrame_tx.payload.data, can_data->data, rxDataLen);

    /* UDP Checksum */
    /* IPv4 Pseudo Header */
    udp_header_sum[0] = trans_8_to_16(ethFrame_tx.ipHeader.srcIPAddr[0], ethFrame_tx.ipHeader.srcIPAddr[1]);
    udp_header_sum[1] = trans_8_to_16(ethFrame_tx.ipHeader.srcIPAddr[2], ethFrame_tx.ipHeader.srcIPAddr[3]);
    udp_header_sum[2] = trans_8_to_16(ethFrame_tx.ipHeader.dstIPAddr[0], ethFrame_tx.ipHeader.dstIPAddr[1]);
    udp_header_sum[3] = trans_8_to_16(ethFrame_tx.ipHeader.dstIPAddr[2], ethFrame_tx.ipHeader.dstIPAddr[3]);
    udp_header_sum[4] = trans_8_to_16(0x00, ethFrame_tx.ipHeader.protocol);
    udp_header_sum[5] = trans_8_to_16(ethFrame_tx.udpHeader.length[0], ethFrame_tx.udpHeader.length[1]);
    /* UDP Header */
    udp_header_sum[6] = trans_8_to_16(ethFrame_tx.udpHeader.srcPort[0], ethFrame_tx.udpHeader.srcPort[1]);
    udp_header_sum[7] = trans_8_to_16(ethFrame_tx.udpHeader.dstPort[0], ethFrame_tx.udpHeader.dstPort[1]);
    udp_header_sum[8] = udp_header_sum[5];
    udp_header_sum[9] = trans_8_to_16(ethFrame_tx.payload.ecuId, ethFrame_tx.payload.extId);
    udp_header_sum[10] = trans_8_to_16(ethFrame_tx.payload.id[0], ethFrame_tx.payload.id[1]);
    udp_header_sum[11] = trans_8_to_16(ethFrame_tx.payload.id[2], ethFrame_tx.payload.id[3]);
    udp_header_sum[12] = trans_8_to_16(ethFrame_tx.payload.dlc, ethFrame_tx.payload.data[0]);
    /* Ethernet Payload */
    /*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "i" (signed) is not the same as that of the right operand "checksum_payload_size"(unsigned).*/
    for(uint8 i = 0; i < checksum_payload_size; i++)
    {
    /*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "i" (unsigned) is not the same as that of the right operand "2"(signed).*/
        udp_header_sum[13u + i] = trans_8_to_16(ethFrame_tx.payload.data[(i*2u) + 1u], ethFrame_tx.payload.data[(i*2u) + 2u]);
    }
    /* Last byte 0x00 for even alignment. */
    /*misra_c_2012_rule_10_4_violation:	Essential type of the left hand operand "13" (signed) is not the same as that of the right operand "checksum_payload_size"(unsigned).*/
    udp_header_sum[13u + checksum_payload_size - 1u] = 0x00u;


    udp_checksum = cal_checksum(udp_header_sum, udp_checksum_size);

    ethFrame_tx.udpHeader.checksum[0] = (uint8)((udp_checksum & 0xFF00u) >> 8);
    ethFrame_tx.udpHeader.checksum[1] = (uint8)(udp_checksum & 0xFFu);

    ethSize = sizeof(EthFrame_t);

#if 0
    tpa_msg.MsgID = TX_TPA_TSNC_SEND;
    SAL_MemCopy(tpa_msg.MsgData, &ethFrame_tx, ethSize);

    (void)SAL_QueuePut(gTpaTxQueueHandle, (void*)&tpa_msg, sizeof(TPA_TX_MESSAGE), portMAX_DELAY, SAL_OPT_BLOCKING);
#else
/* for TSNC latency */
    /*
    SAL_GetTickCount(&tick_tpa_write);

    mcu_printf("===================================================\n");
    mcu_printf("tpa_write = %d lpa_rx = %d\n", tick_tpa_write, tick_lpa_rx);
    mcu_printf("%s_%d can2eth latency = %d (ms)\n", __func__, __LINE__, (tick_tpa_write - tick_lpa_rx));
    mcu_printf("===================================================\n");
    */
    /*misra_c_2012_rule_17_7_violation:	The return value of a non-void function "tpa_write" is unused.*/
    // 241106 sy.kim
    gettimeofday(&start, NULL);
	(void)tpa_write((uint8*)&ethFrame_tx, ethSize);
    gettimeofday(&end, NULL);
	elapsed_time = (end.tv_sec - start.tv_sec) * 1000000L + (end.tv_usec - start.tv_usec);
	mcu_printf("CAN to ETH Convert Time: %ld us\n", elapsed_time);
	
#endif
}
#endif // #if (SIC_BSP_SUPPORT_TEST_APP_TPA == 1)

