/*
***************************************************************************************************
*
*   FileName : ipc_parser.c
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

/****************************************
 *				Include					*
 ****************************************/
#include "ipc_api.h"
#include "ipc_os.h"
#include "ipc_ctl.h"
#include "ipc_cmd.h"
#include "ipc_parser.h"

/****************************************
 *		  Variable Definitions			*
 ****************************************/
IPCCbFunc_t			cbFunc[IPC_SVC_CH_MAX][IPC_PROCESS_NUM];

static uint8        packetReceive[IPC_SVC_CH_MAX][MAX_IPC_PACKET_SIZE];
static IPCChInfo_t  ipcChInfo[IPC_SVC_CH_MAX] =
{
    {IPC_CH_CA65_NS_USER, NULL},
    {IPC_CH_CA65_NS_TPA, NULL},
    {IPC_CH_CA65_NS_LPA, NULL},

#if ( CORTEX_M7_0 == 0)
    {IPC_CH_CM7_0_USER, NULL},
    {IPC_CH_CM7_0_TPA, NULL},
    {IPC_CH_CM7_0_LPA, NULL},
#endif

#if ( CORTEX_M7_1 == 0)
    {IPC_CH_CM7_1_USER, NULL},
    {IPC_CH_CM7_1_TPA, NULL},
    {IPC_CH_CM7_1_LPA, NULL},
#endif

#if ( CORTEX_M7_2 == 0)
    {IPC_CH_CM7_2_USER, NULL},
    {IPC_CH_CM7_2_TPA, NULL},
    {IPC_CH_CM7_2_LPA, NULL},
#endif

#if ( CORTEX_M7_NP == 0)
    {IPC_CH_CM7_NP_USER, NULL},
    {IPC_CH_CM7_NP_TPA, NULL},
    {IPC_CH_CM7_NP_LPA, NULL},
#endif
};


/***************************************************
*          Local function prototypes               *
****************************************************/
static int32 IPC_ReceivePacket(    IPCSvcCh_t siCh);
static int32 IPC_PacketParser(    IPCSvcCh_t siCh);
static void IPC_ParserTask_CA65NS_USER(    const void*       pArg);
static void IPC_ParserTask_CA65NS_TPA(    const void*       pArg);
static void IPC_ParserTask_CA65NS_LPA(    const void*       pArg);

#if ( CORTEX_M7_0 == 0)
static void IPC_ParserTask_CM7_0_USER(    const void* pArg);
static void IPC_ParserTask_CM7_0_TPA(    const void* pArg);
static void IPC_ParserTask_CM7_0_LPA(    const void* pArg);
#endif

#if ( CORTEX_M7_1 == 0)
static void IPC_ParserTask_CM7_1_USER(    const void* pArg);
static void IPC_ParserTask_CM7_1_TPA(    const void* pArg);
static void IPC_ParserTask_CM7_1_LPA(    const void* pArg);
#endif

#if ( CORTEX_M7_2 == 0)
static void IPC_ParserTask_CM7_2_USER(    const void* pArg);
static void IPC_ParserTask_CM7_2_TPA(    const void* pArg);
static void IPC_ParserTask_CM7_2_LPA(    const void* pArg);
#endif


#if ( CORTEX_M7_NP == 0)
static void IPC_ParserTask_CM7_NP_USER(    const void* pArg);
static void IPC_ParserTask_CM7_NP_TPA(    const void* pArg);
static void IPC_ParserTask_CM7_NP_LPA(    const void* pArg);
#endif


static int32 CM7_Core_Open_sub(IPCSvcCh_t	       siCh);
static int32 CM7_Core_Open(void);
static void IPC_OpenTask(    const void* pArg);


/**************************************************************************************************
*                                  IPC_ReceivePacket
*
* [Internal] IPC_ReceivePacket
*
* @param        siCh
* @return       IPC_Ret
*
* Notes
*
***************************************************************************************************/
static int32 IPC_ReceivePacket(    IPCSvcCh_t siCh)
{
    int32  ret;
    uint32 read_size;
    uint16 packet_size;

    read_size   = IPC_PACKET_PREPARE_SIZE;
    packet_size = 0;
    ret         = IPC_Ioctl((uint32) siCh, IOCTL_IPC_READ, (void *) &packetReceive[siCh][0], (void *) &read_size, NULL_PTR, NULL);
	
    if (ret > 0)
    {
        packet_size = (uint16) packetReceive[siCh][7];
        packet_size = packet_size << (uint16) 8;
        packet_size |= (uint16) packetReceive[siCh][8];

        if (packet_size > (((uint16) gIPCHandler[siCh].Ipc_Max_Packet_Size) - ((uint16) IPC_PACKET_PREPARE_SIZE) - ((uint16) 2)) /* crc byte */)
        {
            ret = IPC_ERR_READ; //IPC read size error
        }
        else
        {
            if (packet_size < ((uint16) 1))
            {
                packet_size = ((uint16) 1) + ((uint16) 2); // 1 is data length , 2 is crc byte
            }
            else
            {
                packet_size += ((uint16) 2); // 2 is crc byte
            }

            read_size = packet_size;
            (void) IPC_Ioctl(((uint32) siCh), IOCTL_IPC_READ,  (void *) &packetReceive[siCh][9], (void *) &read_size, NULL_PTR, NULL);
            ret = ((read_size <= (uint32) packet_size) ? ((int32) read_size + IPC_PACKET_PREPARE_SIZE) : IPC_ERR_INVALID_DL);
        }
    }

    return ret;
}


/**************************************************************************************************
*                                  IPC_PacketParser
*
* [Internal] IPC_PacketParser
*
* @param        siCh
* @return       IPC_Ret
*
* Notes
*
***************************************************************************************************/
static int32 IPC_PacketParser(    IPCSvcCh_t siCh)
{
    int32  ret;
    uint16 crc1;
    uint16 crc2;
    uint16 cmd1;
    uint16 cmd2;
    uint16 totalLength;
    uint16 dataLength;

    ret        = IPC_SUCCESS;
    dataLength = (uint16) packetReceive[siCh][7];
    dataLength = dataLength << ((uint16) 8);
    dataLength |= (uint16) packetReceive[siCh][8];
	
    if(dataLength == ((uint16) 0))
    {
        dataLength++; //dummy data length
    }

    if(dataLength <= ((uint16) gIPCHandler[siCh].Ipc_Max_Packet_Size - (uint16) IPC_PACKET_PREPARE_SIZE))
    {
        totalLength = ((uint16) IPC_PACKET_PREPARE_SIZE) + dataLength;

        if (totalLength < ((uint16) gIPCHandler[siCh].Ipc_Max_Packet_Size - (uint16) 1))
        {				        
            crc1 = IPC_CalcCrc16(&packetReceive[siCh][0], totalLength, 0);
            crc2 = (uint16)((uint16)((uint16)packetReceive[siCh][totalLength] << 8) |
                   (uint16)(packetReceive[siCh][totalLength + ((uint16) 1)]));
            if (crc1 == crc2)
            {			
				if ((packetReceive[siCh][0] == (uint8) IPC_SYNC) &&
                    (packetReceive[siCh][1] == (uint8) IPC_START1) &&
                    (packetReceive[siCh][2] == (uint8) IPC_START2))
                {
                    cmd1 = (uint16)((uint16)((uint16)packetReceive[siCh][3] << 8) | (uint16)packetReceive[siCh][4]);
                    cmd2 = (uint16)((uint16)((uint16)packetReceive[siCh][5] << 8) | (uint16)packetReceive[siCh][6]);					
					
                    if(cbFunc[siCh][cmd1].ipcCbFunc != NULL_PTR) // null pointer checking
                    {
                        cbFunc[siCh][cmd1].ipcCbFunc(cmd2, &packetReceive[siCh][9], dataLength);
                    }
                }
            }
        }
        else
        {
            ret = IPC_ERR_READ;
        }
    }
    else
    {
        ret = IPC_ERR_INVALID_DL;
    }

    return ret;
}


/**************************************************************************************************
*                           IPC_ParserTask_CA65NS_USER/TPA/LPA
*
* [Task] IPC_ParserTask_CA65NS_USER/TPA/LPA
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/
static void IPC_ParserTask_CA65NS_USER(    const void*       pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CA65_NS_USER);
		IPC_Parser_WaitEvent(IPC_CH_CA65_NS_USER);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CA65_NS_USER);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CA65_NS_USER);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CA65NS_TPA(    const void*       pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CA65_NS_TPA);
		IPC_Parser_WaitEvent(IPC_CH_CA65_NS_TPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CA65_NS_TPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CA65_NS_TPA);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CA65NS_LPA(    const void*       pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CA65_NS_LPA);
		IPC_Parser_WaitEvent(IPC_CH_CA65_NS_LPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CA65_NS_LPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CA65_NS_LPA);
	        }
		}while(ipc_ret);
    }
}



/**************************************************************************************************
*                           IPC_ParserTask_CM7_0_USER/TPA/LPA
*
* [Task] IPC_ParserTask_CM7_0_USER/TPA/LPA
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/
#if ( CORTEX_M7_0 == 0)
static void IPC_ParserTask_CM7_0_USER(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_0_USER);
		IPC_Parser_WaitEvent(IPC_CH_CM7_0_USER);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_0_USER);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_0_USER);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_0_TPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_0_TPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_0_TPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_0_TPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_0_TPA);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_0_LPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_0_LPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_0_LPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_0_LPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_0_LPA);
	        }
		}while(ipc_ret);
    }
}

#endif

/**************************************************************************************************
*                           IPC_ParserTask_CM7_1_USER/TPA/LPA
*
* [Task] IPC_ParserTask_CM7_1_USER/TPA/LPA
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/
#if ( CORTEX_M7_1 == 0)
static void IPC_ParserTask_CM7_1_USER(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_1_USER);
		IPC_Parser_WaitEvent(IPC_CH_CM7_1_USER);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_1_USER);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_1_USER);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_1_TPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_1_TPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_1_TPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_1_TPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_1_TPA);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_1_LPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_1_LPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_1_LPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_1_LPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_1_LPA);
	        }
		}while(ipc_ret);
    }
}

#endif

/**************************************************************************************************
*                           IPC_ParserTask_CM7_2_USER/TPA/LPA
*
* [Task] IPC_ParserTask_CM7_2_USER/TPA/LPA
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/

#if ( CORTEX_M7_2 == 0)
static void IPC_ParserTask_CM7_2_USER(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_2_USER);
		IPC_Parser_WaitEvent(IPC_CH_CM7_2_USER);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_2_USER);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_2_USER);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_2_TPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_2_TPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_2_TPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_2_TPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_2_TPA);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_2_LPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_2_LPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_2_LPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_2_LPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_2_LPA);
	        }
		}while(ipc_ret);
    }
}

#endif

/**************************************************************************************************
*                           IPC_ParserTask_CM7_NP_USER/TPA/LPA
*
* [Task] IPC_ParserTask_CM7_NP_USER/TPA/LPA
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/

#if ( CORTEX_M7_NP == 0)
static void IPC_ParserTask_CM7_NP_USER(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_NP_USER);
		IPC_Parser_WaitEvent(IPC_CH_CM7_NP_USER);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_NP_USER);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_NP_USER);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_NP_TPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_NP_TPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_NP_TPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_NP_TPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_NP_TPA);
	        }
		}while(ipc_ret);
    }
}

static void IPC_ParserTask_CM7_NP_LPA(    const void* pArg)
{
    int32 ipc_ret;

    (void) pArg;

    while (TRUE)
    {	
	    IPC_Parser_ClearEvent(IPC_CH_CM7_NP_LPA);
		IPC_Parser_WaitEvent(IPC_CH_CM7_NP_LPA);

		do{
	        ipc_ret = IPC_ReceivePacket(IPC_CH_CM7_NP_LPA);

	        if ( ipc_ret > 0)
	        {
	            (void) IPC_PacketParser(IPC_CH_CM7_NP_LPA);
	        }
		}while(ipc_ret);
    }
}


#endif


/**************************************************************************************************
*                           CM7_Core_Open
*
* [Internal] CM7_Core_Open
*
* @param        siCh
* @return       IPC_Ret
*
* Notes
*
***************************************************************************************************/
static int32 CM7_Core_Open_sub(IPCSvcCh_t	       siCh)
{
	int32	ret = SAL_RET_SUCCESS;

	if(IPC_GetStatus(siCh) == IPC_INIT)
	{
		ret = IPC_SendOpen(siCh);

		if(ret == SAL_RET_SUCCESS)
		{
			(void) IPC_SetStatus(siCh, IPC_OPENED, 0);
		}
	}

	return	ret;
}

static int32 CM7_Core_Open(void)
{

	int32	ret = SAL_RET_FAILED;
	int32	ret1 = SAL_RET_SUCCESS;
	int32	ret2 = SAL_RET_SUCCESS;	
	int32	ret3 = SAL_RET_SUCCESS;	
	int32	ret4 = SAL_RET_SUCCESS;
	int32	ret5 = SAL_RET_SUCCESS;	
	int32	ret6 = SAL_RET_SUCCESS;	
	int32	ret7 = SAL_RET_SUCCESS;
	int32	ret8 = SAL_RET_SUCCESS;	
	int32	ret9 = SAL_RET_SUCCESS;	

#if ( CORTEX_M7_0 == 1)
	ret1 = CM7_Core_Open_sub(IPC_CH_CM7_1_USER);
	ret2 = CM7_Core_Open_sub(IPC_CH_CM7_1_TPA);
	ret3 = CM7_Core_Open_sub(IPC_CH_CM7_1_LPA);

	ret4 = CM7_Core_Open_sub(IPC_CH_CM7_2_USER);
	ret5 = CM7_Core_Open_sub(IPC_CH_CM7_2_TPA);
	ret6 = CM7_Core_Open_sub(IPC_CH_CM7_2_LPA);

	ret7 = CM7_Core_Open_sub(IPC_CH_CM7_NP_USER);
	ret8 = CM7_Core_Open_sub(IPC_CH_CM7_NP_TPA);
	ret9 = CM7_Core_Open_sub(IPC_CH_CM7_NP_LPA);

#elif ( CORTEX_M7_1 == 1)
	ret1 = CM7_Core_Open_sub(IPC_CH_CM7_0_USER);
	ret2 = CM7_Core_Open_sub(IPC_CH_CM7_0_TPA);
	ret3 = CM7_Core_Open_sub(IPC_CH_CM7_0_LPA);

	ret4 = CM7_Core_Open_sub(IPC_CH_CM7_2_USER);
	ret5 = CM7_Core_Open_sub(IPC_CH_CM7_2_TPA);
	ret6 = CM7_Core_Open_sub(IPC_CH_CM7_2_LPA);

	ret7 = CM7_Core_Open_sub(IPC_CH_CM7_NP_USER);
	ret8 = CM7_Core_Open_sub(IPC_CH_CM7_NP_TPA);
	ret9 = CM7_Core_Open_sub(IPC_CH_CM7_NP_LPA);

#elif ( CORTEX_M7_2 == 1)
	ret1 = CM7_Core_Open_sub(IPC_CH_CM7_0_USER);
	ret2 = CM7_Core_Open_sub(IPC_CH_CM7_0_TPA);
	ret3 = CM7_Core_Open_sub(IPC_CH_CM7_0_LPA);

	ret4 = CM7_Core_Open_sub(IPC_CH_CM7_1_USER);
	ret5 = CM7_Core_Open_sub(IPC_CH_CM7_1_TPA);
	ret6 = CM7_Core_Open_sub(IPC_CH_CM7_1_LPA);

	ret7 = CM7_Core_Open_sub(IPC_CH_CM7_NP_USER);
	ret8 = CM7_Core_Open_sub(IPC_CH_CM7_NP_TPA);
	ret9 = CM7_Core_Open_sub(IPC_CH_CM7_NP_LPA);

#elif ( CORTEX_M7_NP == 1)
	ret1 = CM7_Core_Open_sub(IPC_CH_CM7_0_USER);
	ret2 = CM7_Core_Open_sub(IPC_CH_CM7_0_TPA);
	ret3 = CM7_Core_Open_sub(IPC_CH_CM7_0_LPA);

	ret4 = CM7_Core_Open_sub(IPC_CH_CM7_1_USER);
	ret5 = CM7_Core_Open_sub(IPC_CH_CM7_1_TPA);
	ret6 = CM7_Core_Open_sub(IPC_CH_CM7_1_LPA);

	ret7 = CM7_Core_Open_sub(IPC_CH_CM7_2_USER);
	ret8 = CM7_Core_Open_sub(IPC_CH_CM7_2_TPA);
	ret9 = CM7_Core_Open_sub(IPC_CH_CM7_2_LPA);

#endif

	if((ret1 == SAL_RET_SUCCESS) && (ret2 == SAL_RET_SUCCESS) && (ret3 == SAL_RET_SUCCESS)
		&& (ret4 == SAL_RET_SUCCESS) && (ret5 == SAL_RET_SUCCESS) && (ret6 == SAL_RET_SUCCESS)
		&& (ret7 == SAL_RET_SUCCESS) && (ret8 == SAL_RET_SUCCESS) && (ret9 == SAL_RET_SUCCESS))
	{
		ret = SAL_RET_SUCCESS;
	}
	return	ret;			
}



/**************************************************************************************************
*                           IPC_OpenTask_CM7
*
* [Task] IPC_OpenTask_CM7
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/
static void IPC_OpenTask(    const void* pArg)
{
    (void) pArg;

    while (TRUE)
    {	
        if(SAL_RET_SUCCESS == CM7_Core_Open())
    	{   		
			break;
    	}

        (void) SAL_TaskSleep(5);
    }

	mcu_printf("%s, Ipc_Open Success!!!\n", __func__);
	vTaskDelete(NULL);
}


/**************************************************************************************************
*                           IPC_CreateIPCParserTask
*
* [External] IPC_CreateIPCParserTask
*
* @param        void
* @return       void
*
* Notes
*
***************************************************************************************************/
void IPC_CreateIPCParserTask(    void)
{
	// 241106 sy.kim
	// int32 IPC_MODE_U = IPC_MODE_0_MBOX;	
	int32 IPC_MODE_U = IPC_MODE_0_SHM;
	int32 IPC_MODE_T = IPC_MODE_0_SHM;	
	int32 IPC_MODE_L = IPC_MODE_0_MBOX;
	
    int32 i;
    uint8 isHalHandle;

    static uint32 IPC_PARSER_TASK_ID[IPC_SVC_CH_MAX];
    static uint32 IPC_PARSER_TASK_STK[IPC_SVC_CH_MAX][IPC_PARSER_TASK_STK_SIZE];
    static uint32 IPC_OPEN_TASK_ID;	
    static uint32 IPC_OPEN_TASK_STK[IPC_OPEN_TASK_STK_SIZE];

    isHalHandle = 0;

	ipcChInfo[IPC_CH_CA65_NS_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CA65_NS_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CA65_NS_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CA65_NS_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CA65_NS_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CA65_NS_LPA, IPC_MODE_L);
	
#if ( CORTEX_M7_0 == 1)
	ipcChInfo[IPC_CH_CM7_1_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_1_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_1_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_LPA, IPC_MODE_L);

    ipcChInfo[IPC_CH_CM7_2_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_USER, IPC_MODE_U);
    ipcChInfo[IPC_CH_CM7_2_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_TPA, IPC_MODE_T);
    ipcChInfo[IPC_CH_CM7_2_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_LPA, IPC_MODE_L);

	ipcChInfo[IPC_CH_CM7_NP_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_USER, IPC_MODE_U);	
    ipcChInfo[IPC_CH_CM7_NP_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_TPA, IPC_MODE_T);	
    ipcChInfo[IPC_CH_CM7_NP_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_LPA, IPC_MODE_L);		


#elif ( CORTEX_M7_1 == 1)
	ipcChInfo[IPC_CH_CM7_0_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_0_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_0_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_LPA, IPC_MODE_L);

    ipcChInfo[IPC_CH_CM7_2_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_USER, IPC_MODE_U);
    ipcChInfo[IPC_CH_CM7_2_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_TPA, IPC_MODE_T);
    ipcChInfo[IPC_CH_CM7_2_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_LPA, IPC_MODE_L);

    ipcChInfo[IPC_CH_CM7_NP_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_USER, IPC_MODE_U);	
    ipcChInfo[IPC_CH_CM7_NP_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_TPA, IPC_MODE_T);	
    ipcChInfo[IPC_CH_CM7_NP_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_LPA, IPC_MODE_L);	


#elif ( CORTEX_M7_2 == 1)
	ipcChInfo[IPC_CH_CM7_0_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_0_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_0_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_LPA, IPC_MODE_L);

	ipcChInfo[IPC_CH_CM7_1_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_1_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_1_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_LPA, IPC_MODE_L);

    ipcChInfo[IPC_CH_CM7_NP_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_USER, IPC_MODE_U);	
    ipcChInfo[IPC_CH_CM7_NP_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_TPA, IPC_MODE_T);	
    ipcChInfo[IPC_CH_CM7_NP_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_NP_LPA, IPC_MODE_L);	


#elif ( CORTEX_M7_NP == 1)
	ipcChInfo[IPC_CH_CM7_0_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_0_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_0_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_0_LPA, IPC_MODE_L);

	ipcChInfo[IPC_CH_CM7_1_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_USER, IPC_MODE_U);
	ipcChInfo[IPC_CH_CM7_1_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_TPA, IPC_MODE_T);
	ipcChInfo[IPC_CH_CM7_1_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_1_LPA, IPC_MODE_L);

    ipcChInfo[IPC_CH_CM7_2_USER].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_USER, IPC_MODE_U);
    ipcChInfo[IPC_CH_CM7_2_TPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_TPA, IPC_MODE_T);
    ipcChInfo[IPC_CH_CM7_2_LPA].ipcHandle = IPC_Open((uint32) IPC_CH_CM7_2_LPA, IPC_MODE_L);
#endif

    for(i = 0 ; i < (int32) IPC_SVC_CH_MAX ; i++)
    {
        if(ipcChInfo[i].ipcHandle == IPC_SUCCESS)
        {
            isHalHandle = 1;
        }
    }

    if (isHalHandle == (uint8) 1)
    {
        (void) SAL_TaskCreate
        (
            &(IPC_PARSER_TASK_ID[IPC_CH_CA65_NS_USER]),
            (const uint8 *) "IPC_CA65NonSecure_ParserTask_User",
            (SALTaskFunc) &IPC_ParserTask_CA65NS_USER,
            &(IPC_PARSER_TASK_STK[IPC_CH_CA65_NS_USER][0]),
            IPC_PARSER_TASK_STK_SIZE,
            SAL_PRIO_IPC_PARSER, NULL_PTR
        );
        (void) SAL_TaskCreate
        (
            &(IPC_PARSER_TASK_ID[IPC_CH_CA65_NS_TPA]),
            (const uint8 *) "IPC_CA65NonSecure_ParserTask_TPA",
            (SALTaskFunc) &IPC_ParserTask_CA65NS_TPA,
            &(IPC_PARSER_TASK_STK[IPC_CH_CA65_NS_TPA][0]),
            IPC_PARSER_TASK_STK_SIZE,
            SAL_PRIO_IPC_PARSER, NULL_PTR
        );
		
        (void) SAL_TaskCreate
        (
            &(IPC_PARSER_TASK_ID[IPC_CH_CA65_NS_LPA]),
            (const uint8 *) "IPC_CA65NonSecure_ParserTask_LPA",
            (SALTaskFunc) &IPC_ParserTask_CA65NS_LPA,
            &(IPC_PARSER_TASK_STK[IPC_CH_CA65_NS_LPA][0]),
            IPC_PARSER_TASK_STK_SIZE,
            SAL_PRIO_IPC_PARSER, NULL_PTR
        );	

#if ( CORTEX_M7_0 == 0)
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_0_USER]),
			(const uint8 *) "IPC_CortexM7_0_ParserTask_User",
			(SALTaskFunc) &IPC_ParserTask_CM7_0_USER,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_0_USER][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_0_TPA]),
			(const uint8 *) "IPC_CortexM7_0_ParserTask_TPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_0_TPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_0_TPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_0_LPA]),
			(const uint8 *) "IPC_CortexM7_0_ParserTask_LPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_0_LPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_0_LPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR

		);		

#endif

#if ( CORTEX_M7_1 == 0)
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_1_USER]),
			(const uint8 *) "IPC_CortexM7_1_ParserTask_User",
			(SALTaskFunc) &IPC_ParserTask_CM7_1_USER,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_1_USER][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_1_TPA]),
			(const uint8 *) "IPC_CortexM7_1_ParserTask_TPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_1_TPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_1_TPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_1_LPA]),
			(const uint8 *) "IPC_CortexM7_1_ParserTask_LPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_1_LPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_1_LPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR

		);	
#endif


#if ( CORTEX_M7_2 == 0)
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_2_USER]),
			(const uint8 *) "IPC_CortexM7_2_ParserTask_User",
			(SALTaskFunc) &IPC_ParserTask_CM7_2_USER,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_2_USER][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_2_TPA]),
			(const uint8 *) "IPC_CortexM7_2_ParserTask_TPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_2_TPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_2_TPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_2_LPA]),
			(const uint8 *) "IPC_CortexM7_2_ParserTask_LPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_2_LPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_2_LPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR

		);	
#endif

#if ( CORTEX_M7_NP == 0)
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_NP_USER]),
			(const uint8 *) "IPC_CortexM7_NP_ParserTask_User",
			(SALTaskFunc) &IPC_ParserTask_CM7_NP_USER,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_NP_USER][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_NP_TPA]),
			(const uint8 *) "IPC_CortexM7_NP_ParserTask_TPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_NP_TPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_NP_TPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
		(void) SAL_TaskCreate
		(
			&(IPC_PARSER_TASK_ID[IPC_CH_CM7_NP_LPA]),
			(const uint8 *) "IPC_CortexM7_NP_ParserTask_LPA",
			(SALTaskFunc) &IPC_ParserTask_CM7_NP_LPA,
			&(IPC_PARSER_TASK_STK[IPC_CH_CM7_NP_LPA][0]),
			IPC_PARSER_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR

		);	

#endif

		(void) SAL_TaskCreate
		(
			&(IPC_OPEN_TASK_ID),
			(const uint8 *) "IPC_Open Task",
			(SALTaskFunc) &IPC_OpenTask,
			&(IPC_OPEN_TASK_STK[0]),
			IPC_OPEN_TASK_STK_SIZE,
			SAL_PRIO_IPC_PARSER, NULL_PTR
		);
    }

}

