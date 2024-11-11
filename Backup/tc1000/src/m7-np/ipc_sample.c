/*
***************************************************************************************************
*
*   FileName : system_monitoring.c
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
 

#include <app_cfg.h>


#if (VNG_APP_IPC_Sample_EN == 1)

/*
#include <sal_com.h>
#include <sal_internal.h>
#include "bsp.h"
*/
#include "debug.h"
#include "ipc.h"
#include <ipc_sample.h>




 
/*
***************************************************************************************************
*                                             DEFINITIONS 
***************************************************************************************************
*/
#define IPC_APP_DBG_ENABLE 
#ifdef IPC_APP_DBG_ENABLE 
#define IPC_APP_DBG(fmt, args...) mcu_printf(fmt, ## args)
#else
#define IPC_APP_DBG(fmt, args...)  
#endif

 
/*
***************************************************************************************************
*                                         FUNCTION PROTOTYPES
***************************************************************************************************
*/
static void udelay(uint32 us)
{
    uint32  i = 0;
    uint32  cnt = 0;
    uint32 cycle_per_1us = (uint32)400000000 / 1000000U;

    cnt = us * cycle_per_1us;
    for (i = 0UL; i < cnt; i++)
    {
         BSP_NOP_DELAY();
    }
}



static void IPC_App_Sample_Task 
(
    void *                              pArg
);
 
// Need Check Later : Remove to avoid compile warning 
//static void SMA_ExternSignal
//(
//    void
//);
 
static void Make_IPC_Send_Data 
(
    uint8 		add
);
 
static void IPC_IpcCbFunc_CM7_1 
(
    uint16                              uhwCmd,
    uint8 *                             pucData,
    uint16                              uhwLength 
); // CS : Dangerous Function Cast 
 
static void IPC_IpcCbFunc_CM7_0 
(
    uint16                              uhwCmd,
    uint8 *                             pucData,
    uint16                              uhwLength 
);


/*
***************************************************************************************************
*                                         FUNCTIONS 
***************************************************************************************************
*/

uint8	IPC_Send_Data[256];
uint8	IPC_Receive_Data[IPC_SVC_CH_MAX][256];


static void Make_IPC_Send_Data(uint8 add)
{
    uint32 i;
 
    (void)SAL_MemSet((void *) &IPC_Send_Data[0], 0x00, sizeof(IPC_Send_Data));
 
    for (i =0UL; i < (uint32)256; i++)
    {
        IPC_Send_Data[i] = (uint8)0x1 + add;
    }
}

#define	send_num	250
//#define	send_length	117
#define	send_length	245
//#define	send_length	501

//#define	send_length_shm	117
#define	send_length_shm	245
//#define	send_length_shm	501
//#define	send_length_shm	1013
//#define	send_length_shm	2037




#define	UdelayTime	2000



#if ( CORTEX_M7_NP == 0)
static void IPC_IpcCbFunc_CM7_NP_USER(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_NP_USER][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_NP_USER][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_NP_USER]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_NP_USER][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_NP_USER][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_NP_USER][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_NP_TPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_NP_TPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_NP_TPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_NP_TPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_NP_TPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_NP_TPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_NP_TPA][send_length_shm-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_NP_LPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_NP_LPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_NP_LPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_NP_LPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_NP_LPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_NP_LPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_NP_LPA][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}



#endif


#if ( CORTEX_M7_2 == 0)
static void IPC_IpcCbFunc_CM7_2_USER(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;

	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_2_USER][0];

	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_2_USER][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_2_USER]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {        
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_2_USER][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_2_USER][0];
//	        mcu_printf("[%s]: Length : %d,  Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_2_USER][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//	        udelay(UdelayTime);


			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}


        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_2_TPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_2_TPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_2_TPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_2_TPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_2_TPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_2_TPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_2_TPA][send_length_shm-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_2_LPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_2_LPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_2_LPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_2_LPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_2_LPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_2_LPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_2_LPA][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

#endif





#if ( CORTEX_M7_1 == 0)
static void IPC_IpcCbFunc_CM7_1_USER(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;

	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_1_USER][0];

	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_1_USER][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_1_USER]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_1_USER][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_1_USER][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_1_USER][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);


			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_1_TPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_1_TPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_1_TPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_1_TPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_1_TPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_1_TPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_1_TPA][send_length_shm-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_1_LPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_1_LPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_1_LPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_1_LPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_1_LPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_1_LPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_1_LPA][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

#endif

#if ( CORTEX_M7_0 == 0)
static void IPC_IpcCbFunc_CM7_0_USER(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;

	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_0_USER][0];

	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_0_USER][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_0_USER]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {        
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_0_USER][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_0_USER][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_0_USER][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);


			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_0_TPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;

	old_data = IPC_Receive_Data[IPC_CH_CM7_0_TPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_0_TPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_0_TPA]));
#if 1	        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_0_TPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_0_TPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_0_TPA][send_length_shm-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}

static void IPC_IpcCbFunc_CM7_0_LPA(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;
	uint8 old_data = 0;
	uint8 temp = 0;
/*
	mcu_printf("%d.%03d\n",TC32MCNT_Val[Time_Test_cnt],((TC32Prescale_Val[Time_Test_cnt]*41666667)/1000000)); //QAC
	if(++Time_Test_cnt >= 250)
	{
		Time_Test_cnt = 0;
	}

*/

	old_data = IPC_Receive_Data[IPC_CH_CM7_0_LPA][0];
	
	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CM7_0_LPA][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CM7_0_LPA]));
	
#if 1        
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {     
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CM7_0_LPA][i] = pucData[i];
			}

//			temp = IPC_Receive_Data[IPC_CH_CM7_0_LPA][0];
//			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			temp = IPC_Receive_Data[IPC_CH_CM7_0_LPA][send_length-1];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC
//			udelay(UdelayTime);



			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("[%s]:Failure\n", __FUNCTION__);
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("[%s]:Failure\n", __FUNCTION__);
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}



#endif



static void IPC_IpcCbFunc_CA65_Nonsecure(uint16 uhwCmd, uint8 *pucData, uint16 uhwLength) // CS : Dangerous Function Cast 
{
	uint16 i = 0;

	uint8 old_data = 0;
	uint8 temp = 0;


	old_data = IPC_Receive_Data[IPC_CH_CA65_NS_USER][0];

	(void)SAL_MemSet((void *) &IPC_Receive_Data[IPC_CH_CA65_NS_USER][0], 0x00, sizeof(IPC_Receive_Data[IPC_CH_CA65_NS_USER]));
#if 1	
    if (uhwCmd == (uint16)TCC_IPC_CMD_LPA_CAN)
    {
        if (pucData != NULL_PTR)
        {        
			for(i = 0 ; i < uhwLength ; i++)
    		{
    			IPC_Receive_Data[IPC_CH_CA65_NS_USER][i] = pucData[i];
			}

			temp = IPC_Receive_Data[IPC_CH_CA65_NS_USER][0];
			mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, uhwLength, temp); //QAC

			if(old_data == send_num)
			{
				if(temp != 1)
				{
					mcu_printf("Failure\n");
				}
			}
			else if((temp - old_data) != 1)
			{
				mcu_printf("Failure\n");
			}

        }
        else{}
   	}
    else 
    {
        IPC_APP_DBG("%s: uhwCmd invalid !!!\n",__FUNCTION__); //QAC
    }
#endif	
}


uint8 flag = 1U;

static void IPC_App_Sample_Task(void * pArg)
{
	uint8 add = 0U;
	
	uint32 temp;
	uint32 temp2;

//	volatile uint32 TC32MCNT_Val[send_num];
//	volatile uint32 TC32Prescale_Val[send_num];	
	uint32 TC32EN_Addr = 0x4B400080;

	//temp = SAL_ReadReg(TC32EN_Addr);
#if 0
	temp = 0;
	temp &= (uint32)(0UL);			// System Timer 0 32-Bit Timer Init
	temp &= (uint32)~(1 << 24);		// Counter Disable
	temp &= (uint32)~(1 << 29);		// LDM1 = 0
	temp |= (uint32)(1 << 28);		// LDM0 = 1
	temp |= (uint32)(1 << 26);		// Oneshot Mode
	temp |= (uint32)(1 << 25);		// Counter Start from zero (not LOADVAL)
	temp |= (uint32)(0x3A980);		// Prescaler Load Value = 240000

	SAL_WriteReg(temp, TC32EN_Addr);


	////////////////////////////////////////////
	//// TC32LDV
	SAL_WriteReg(0xffffffff, 0x4B400084);		// LOADVAL

	////////////////////////////////////////////
	//// TC32CMP0
	SAL_WriteReg(0xffffffff, 0x4B400088);		// comparison value

#endif

#if 0
	////////////////////////////////////////////
	//// TC32EN
	(*((volatile uint32 *)(0x4B400080))) &= (uint32)(0UL);			// System Timer 0 32-Bit Timer Init
	(*((volatile uint32 *)(0x4B400080))) &= (uint32)~(1 << 24);		// Counter Disable
	(*((volatile uint32 *)(0x4B400080))) &= (uint32)~(1 << 29);		// LDM1 = 0
	(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 28);		// LDM0 = 1
	(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 26);		// Oneshot Mode
	(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 25);		// Counter Start from zero (not LOADVAL)
	(*((volatile uint32 *)(0x4B400080))) |= (uint32)(0x3A980);		// Prescaler Load Value = 240000

	temp2 = SAL_ReadReg(TC32EN_Addr);

	mcu_printf("WD 0x%X,  RD 0x%x\n",temp,temp2); //QAC
	//mcu_printf("0x%X\n",(*((volatile uint32 *)(0x4B400080)))); //QAC

	////////////////////////////////////////////
	//// TC32LDV
	(*((volatile uint32 *)(0x4B400084))) = (uint32)(0xffffffff);		// LOADVAL

	////////////////////////////////////////////
	//// TC32CMP0
	(*((volatile uint32 *)(0x4B400088))) = (uint32)(0xffffffff);		// comparison value

#endif	

    (void)pArg;


//    IPC_RegisterCbFunc(IPC_CH_CA65_NS_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CA65_Nonsecure, NULL_PTR, NULL_PTR);
	
#if ( CORTEX_M7_0 == 1)
    IPC_RegisterCbFunc(IPC_CH_CM7_1_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_USER, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_1_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_LPA, NULL_PTR, NULL_PTR);

	IPC_RegisterCbFunc(IPC_CH_CM7_2_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_LPA, NULL_PTR, NULL_PTR);
	
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_NP_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_LPA, NULL_PTR, NULL_PTR);
#elif ( CORTEX_M7_1 == 1)
	IPC_RegisterCbFunc(IPC_CH_CM7_0_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_TPA, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_LPA, NULL_PTR, NULL_PTR);

	IPC_RegisterCbFunc(IPC_CH_CM7_2_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_LPA, NULL_PTR, NULL_PTR);
	
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_NP_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_LPA, NULL_PTR, NULL_PTR);
#elif ( CORTEX_M7_2 == 1)
	IPC_RegisterCbFunc(IPC_CH_CM7_0_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_TPA, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_LPA, NULL_PTR, NULL_PTR);

	IPC_RegisterCbFunc(IPC_CH_CM7_1_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_LPA, NULL_PTR, NULL_PTR);
	
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_NP_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_NP_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_NP_LPA, NULL_PTR, NULL_PTR);
#elif ( CORTEX_M7_NP == 1)
	IPC_RegisterCbFunc(IPC_CH_CM7_0_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_TPA, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_0_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_0_LPA, NULL_PTR, NULL_PTR);
		
	IPC_RegisterCbFunc(IPC_CH_CM7_1_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_1_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_1_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_1_LPA, NULL_PTR, NULL_PTR);
	
	IPC_RegisterCbFunc(IPC_CH_CM7_2_USER, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_USER, NULL_PTR, NULL_PTR);
	IPC_RegisterCbFunc(IPC_CH_CM7_2_TPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_TPA, NULL_PTR, NULL_PTR);
    IPC_RegisterCbFunc(IPC_CH_CM7_2_LPA, (uint8)TCC_IPC_CMD_LPA, (IPCCallback)&IPC_IpcCbFunc_CM7_2_LPA, NULL_PTR, NULL_PTR);
#endif
 
    (void)SAL_TaskSleep(5000);

#if ( CORTEX_M7_NP == 1) // 241106 sy.kim 0 to 1
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
#endif	

	while(1)
	{

#if ( CORTEX_M7_0 == 1)

		if(flag == 1)
		{
			for (add = 0; add < send_num; add++)
			{
			
				Make_IPC_Send_Data(add);
#if 0

				(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 24);		// Counter Enable

				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				//(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				(*((volatile uint32 *)(0x4B400080))) &= (uint32)~(1 << 24);		// Counter Disable
				TC32MCNT_Val[add] = SAL_ReadReg(0x4B400094);		// System timer 0, 32Bit Timer Main count
				TC32Prescale_Val[add] = SAL_ReadReg(0x4B400090);	// System timer 0, 32Bit Timer Prescale count

//				mcu_printf("%d.%03d\n",TC32MCNT_Val[add],((TC32Prescale_Val[add]*41666667)/1000000)); //QAC
#endif	

#if 1				
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);	
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
#endif	

#if 0			

				Make_IPC_Send_Data(0);
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(1);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(2);
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(10);
				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(11);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(12);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(20);												
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(21);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(22);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
#endif
			}
			add = 0;
			flag = 0;
#if 0
//			(void)SAL_TaskSleep(5000);

			for (add = 0; add < send_num; add++)
			{
				mcu_printf("%d.%03d\n",TC32MCNT_Val[add],((TC32Prescale_Val[add]*41666667)/1000000)); //QAC
			}
			add = 0;
			Time_Test_cnt = 0;
#endif			
		}


#if 0
//		(void)SAL_TaskSleep(100);

		Make_IPC_Send_Data(add++);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_1, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_2, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_NP, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

		if(add >= send_num)
		{
			add = 0;
		}
#endif	
#elif ( CORTEX_M7_1 == 1)


		if(flag == 1)
		{
			for (add = 0; add < send_num; add++)
			{

//				Make_IPC_Send_Data(add);
//				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
//				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
//				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);


#if 1
				
				Make_IPC_Send_Data(add);
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
			
				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
#endif			

#if 0

				Make_IPC_Send_Data(0);
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(1);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(2);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(10);
				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(11);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(12);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(20);												
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(21);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(22);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
#endif

				
			}
			add = 0;
			flag = 0;
		}

#if 0
		Make_IPC_Send_Data(add++);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_0, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);			
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_2, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_NP, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

		if(add >= send_num)
		{
			add = 0;
		}
#endif

#elif ( CORTEX_M7_2 == 1)



		if(flag == 1)
		{
			for (add = 0; add < send_num; add++)
			{
#if 1

				Make_IPC_Send_Data(add);
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

#endif			
#if 0

				Make_IPC_Send_Data(0);
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(1);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(2);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(10);
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(11);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(12);
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(20);												
				(void)IPC_SendPacket(IPC_CH_CM7_NP_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(21);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(22);
				(void)IPC_SendPacket(IPC_CH_CM7_NP_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
#endif

			}
			add = 0;
			flag = 0;
		}



#if 0

		Make_IPC_Send_Data(add++);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_0, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_1, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_NP, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

		if(add >= send_num)
		{
			add = 0;
		}
#endif


#elif ( CORTEX_M7_NP == 1)



		if(flag == 1)
		{
			for (add = 0; add < send_num; add++)
			{

				Make_IPC_Send_Data(add);

#if 1 // 241106 sy.kim 0 to 1

				(*((volatile uint32 *)(0x4B400080))) |= (uint32)(1 << 24);		// Counter Enable

				//(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CA65_NS_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				(*((volatile uint32 *)(0x4B400080))) &= (uint32)~(1 << 24); 	// Counter Disable
				TC32MCNT_Val[add] = SAL_ReadReg(0x4B400094);		// System timer 0, 32Bit Timer Main count
				TC32Prescale_Val[add] = SAL_ReadReg(0x4B400090);	// System timer 0, 32Bit Timer Prescale count
#endif


#if 1
//				mcu_printf("[%s]: Length : %d,	Data : %d \n",__FUNCTION__, send_length_shm, IPC_Send_Data[0]); //QAC
				
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				
				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length_shm);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

#endif	

#if 0

				Make_IPC_Send_Data(0);
				(void)IPC_SendPacket(IPC_CH_CM7_0_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(1);
				(void)IPC_SendPacket(IPC_CH_CM7_0_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(2);
				(void)IPC_SendPacket(IPC_CH_CM7_0_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);


				Make_IPC_Send_Data(10);
				(void)IPC_SendPacket(IPC_CH_CM7_1_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(11);
				(void)IPC_SendPacket(IPC_CH_CM7_1_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(12);
				(void)IPC_SendPacket(IPC_CH_CM7_1_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(20);												
				(void)IPC_SendPacket(IPC_CH_CM7_2_USER, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(21);
				(void)IPC_SendPacket(IPC_CH_CM7_2_TPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

				Make_IPC_Send_Data(22);
				(void)IPC_SendPacket(IPC_CH_CM7_2_LPA, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
#endif	



			}
			add = 0;
			flag = 0;

#if 1 // 241106 sy.kim 0 to 1
//			(void)SAL_TaskSleep(5000);

			for (add = 0; add < send_num; add++)
			{
				mcu_printf("%d.%03d\n",TC32MCNT_Val[add],((TC32Prescale_Val[add]*41666667)/1000000)); //QAC
			}
			add = 0;
			Time_Test_cnt = 0;
#endif				
		}



#if 0
		Make_IPC_Send_Data(add++);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_0, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_1, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);
		(void)IPC_SendPacket(IPC_SVC_CH_CM7_2, (uint16)TCC_IPC_CMD_LPA, (uint16)TCC_IPC_CMD_LPA_CAN, &IPC_Send_Data[0], send_length);

		if(add >= send_num)
		{
			add = 0;
		}
#endif		
		
#endif

	    (void)SAL_TaskSleep(1);
	}
}




void IPC_CreateAppTask(void)
{
    static uint32 IPCAppTaskID;
    static uint32 IPCAppTaskStk[1024];

#if 1
    (void)SAL_TaskCreate(&IPCAppTaskID,
                         (const uint8 *)"IPCAppsample",
                         (SALTaskFunc)&IPC_App_Sample_Task,
                         &IPCAppTaskStk[0],
                         1024,
                         7, NULL_PTR);
#endif
}
#endif

