/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Fir_msdMACSec.c
*
* DESCRIPTION:
*       API definitions for MACSec feature
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <fir/include/api/Fir_msdMACSec.h>
#include <fir/include/api/Fir_msdApiInternal.h>
#include <fir/include/driver/Fir_msdHwAccess.h>
#include <fir/include/driver/Fir_msdDrvSwRegs.h>
#include <utils/msdUtils.h>
#include <platform/msdSem.h>
#include <driver/msdHwAccess.h>

#define regBusWidth 0x4U
#define APB_PHYADDR 0x1CU
#define APB_REGADDR 0x11U
#define GLOBAL_CTRL_ADDR 0x50071000U
#define MIB_OP_ADDR 0x5007100CU
#define INT_OP_ADDR 0x50071008U
#define BASE_TCAM 0x50071080U
#define BASE_TCAM_ACTION 0x50071100U
#define BASE_RX_SECY 0x50071180U
#define BASE_TX_SECY 0x50071200U
#define BASE_RX_SC 0x50071280U
#define BASE_TX_SC 0x50071300U
#define BASE_RX_SA 0x50071380U
#define BASE_TX_SA 0x50071400U
#define BASE_SECY_MIB 0x50071800U
#define BASE_SC_MIB 0x50071880U
#define BASE_SAK 0x50078000U
#define BASE_TXNEXTPN_THR 0x50071010U
#define BASE_DEFRXSCI 0x50070004U
#define BASE_RXPortCTRL 0x50070000U
#define BASE_TXPortCTRL 0x50070040U
#define BASE_MEMCTRL_ADDR 0x50077F80U

#define TCAM_WIDTH 0xDU
#define TCAM_ACTION_WIDTH 0x2U
#define RX_SECY_WIDTH 0x3U
#define TX_SECY_WIDTH 0x3U
#define RX_SC_WIDTH 0x7U
#define TX_SC_WIDTH 0x7U
#define RX_SA_WIDTH 0x8U
#define TX_SA_WIDTH 0x8U
#define SAK_WIDTH 0xAU
#define GET_MEM_ADDR(memType, regAddr) \
		{ \
			switch (memType){\
				case FIR_TYPE_RULE: \
					regAddr = BASE_TCAM; \
					break;\
				case FIR_TYPE_ACTION: \
					regAddr = BASE_TCAM_ACTION; \
					break;\
				case FIR_TYPE_RX_SECY: \
					regAddr =  BASE_RX_SECY; \
					break;\
				case FIR_TYPE_TX_SECY: \
					regAddr =  BASE_TX_SECY; \
					break;\
				case FIR_TYPE_RX_SC: \
					regAddr =  BASE_RX_SC; \
					break;\
				case FIR_TYPE_TX_SC: \
					regAddr =  BASE_TX_SC; \
					break;\
				case FIR_TYPE_RX_SA: \
					regAddr =  BASE_RX_SA; \
					break;\
				case FIR_TYPE_TX_SA: \
					regAddr =  BASE_TX_SA; \
					break;\
				default: \
					regAddr =  -1; \
					break;\
			} \
		}

/*Limits*/
#define FIR_MAXPORTINDEX 7U
#define FIR_MAXRULEINDEX 256U
#define FIR_MAXSECYINDEX 16U
#define FIR_MAXSCINDEX 16U
#define FIR_MAXSAINDEX 32U
#define FIR_MAXMIBINDEX 16U
#define FIR_MAXSAKINDEX 32U

typedef enum
{
	FIR_MACSec_FLUSH_ALL = 0x1,
	FIR_MACSec_FLUSH_ONE = 0x2,
	FIR_MACSec_LOAD_ENTRY = 0x3,
	FIR_MACSec_LOAD_PN_ONLY_GET_NEXT_ENTRY = 0x4,
	FIR_MACSec_READ_ENTRY = 0x5
} FIR_MACSec_OPERATION;

typedef enum
{
	FIR_SAK_LOAD_RX = 0x1,
	FIR_SAK_LOAD_TX = 0x2,
	FIR_SAK_LOAD_BOTH = 0x3,
	FIR_SAK_READ_RX = 0x5,
	FIR_SAK_READ_TX = 0x6
} FIR_MACSec_SAK_OPERATION;

typedef enum
{
	FIR_TRANSFER_CONTROL = 0x0,
	FIR_ADDRESS_B0 = 0x4,
	FIR_ADDRESS_B1 = 0x5,
	FIR_ADDRESS_B2 = 0x6,
	FIR_ADDRESS_B3 = 0x7,
	FIR_DATA_B0 = 0x10,
	FIR_DATA_B1 = 0x11,
	FIR_DATA_B2 = 0x12,
	FIR_DATA_B3 = 0x13,
} FIR_MACSec_APB_OPERATION;

typedef enum
{
	FIR_CAPTURE_RX_SECY_NO_AUTO_CLEAR = 1,
	FIR_CAPTURE_RX_SC_NO_AUTO_CLEAR = 0x2,
	FIR_CAPTURE_TX_SECY_NO_AUTO_CLEAR = 0x3,
	FIR_CAPTURE_TX_SC_NO_AUTO_CLEAR = 0x4,
	FIR_CAPTURE_RX_SECY_AUTO_CLEAR = 0x9,
	FIR_CAPTURE_RX_SC_AUTO_CLEAR = 0xA,
	FIR_CAPTURE_TX_SECY_AUTO_CLEAR = 0xB,
	FIR_CAPTURE_TX_SC_AUTO_CLEAR = 0xC,
} FIR_MACSec_MIB_OPERATION;
typedef enum
{
	FIR_NEXT_RX_SECY_INT_NO_AUTO_CLEAR = 1,
	FIR_NEXT_RX_SC_INT_NO_AUTO_CLEAR = 0x2,
	FIR_NEXT_TX_SECY_INT_NO_AUTO_CLEAR = 0x3,
	FIR_NEXT_TX_SC_INT_NO_AUTO_CLEAR = 0x4,
	FIR_NEXT_RX_SECY_INT_AUTO_CLEAR = 9,
	FIR_NEXT_RX_SC_INT_AUTO_CLEAR = 0xA,
	FIR_NEXT_TX_SECY_INT_AUTO_CLEAR = 0xB,
	FIR_NEXT_TX_SC_INT_AUTO_CLEAR = 0xC,
} FIR_MACSec_INT_OPERATION;

typedef enum
{
	FIR_APB_READ = 0,
	FIR_APB_WRITE = 1
} FIR_RW_OP;

typedef enum
{
	FIR_TYPE_RULE = 0,
	FIR_TYPE_ACTION = 1,
	FIR_TYPE_RX_SECY = 2,
	FIR_TYPE_TX_SECY = 3,
	FIR_TYPE_RX_SC = 4,
	FIR_TYPE_TX_SC = 5,
	FIR_TYPE_RX_SA = 6,
	FIR_TYPE_TX_SA = 7
} FIR_MEMTYPE;

typedef union
{
	/* TCAM Table registers */
	FIR_MSEC_RULE rule;
	/* Action Table registers */
	FIR_MSEC_TCAM_ACTION action;
	/* RX SECY registers */
	FIR_SECY_INGRESS rxSecy;
	/* TX SECY registers */
	FIR_SECY_EGRESS txSecy;
	/* RX SC registers */
	FIR_MSEC_RX_SC rxSc;
	/* TX SC registers */
	FIR_MSEC_TX_SC txSc;
	/* RX SA registers */
	FIR_MSEC_RX_SA rxSa;
	/* TX SC registers */
	FIR_MSEC_TX_SA txSa;

} FIR_MACSec_DATA_HW;

static MSD_U32* ConvertRuleToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertActionToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertRXSecYToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertTXSecYToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertRXSCToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertTXSCToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertRXSAToArray(IN FIR_MACSec_DATA_HW* dataRw);
static MSD_U32* ConvertTXSAToArray(IN FIR_MACSec_DATA_HW* dataRw);

static FIR_MSEC_RULE* ConvertArrayToRule(IN  MSD_QD_DEV dev);
static FIR_MSEC_TCAM_ACTION* ConvertArrayToAction(IN MSD_QD_DEV dev);
static FIR_SECY_INGRESS* ConvertArrayToRxSecY(IN  MSD_QD_DEV dev);
static FIR_SECY_EGRESS* ConvertArrayToTxSecY(IN  MSD_QD_DEV dev);
static FIR_MSEC_RX_SC* ConvertArrayToRxSc(IN  MSD_QD_DEV dev);
static FIR_MSEC_TX_SC* ConvertArrayToTxSc(IN  MSD_QD_DEV dev);
static FIR_MSEC_RX_SA* ConvertArrayToRxSa(IN  MSD_QD_DEV dev);
static FIR_MSEC_TX_SA* ConvertArrayToTxSa(IN  MSD_QD_DEV dev);


static MSD_STATUS Fir_MACSecGetAnyRegField(IN  MSD_QD_DEV dev, IN MSD_U32 regAddr, IN MSD_U32 fieldOffset, IN MSD_U32 fieldLength, OUT MSD_U32* data);
static MSD_STATUS Fir_WaitOperationReady(IN  MSD_QD_DEV dev, IN MSD_32 regAddr);
static MSD_STATUS Fir_OperationPerform(IN  MSD_QD_DEV* dev, IN	MSD_32	regAddr, IN	FIR_MEMTYPE memType, IN  FIR_MACSec_OPERATION  tcamOp, INOUT FIR_MACSec_DATA_HW* opData, IN MSD_U8 index);
static MSD_STATUS  Fir_SAKOperation(IN  MSD_QD_DEV* dev, IN  FIR_MACSec_SAK_OPERATION  sakOp, IN MSD_U8 index);
static MSD_STATUS MACSec_SetMEMData(IN  MSD_QD_DEV dev, IN FIR_MEMTYPE memType, IN FIR_MACSec_DATA_HW* tcamDataP);
static MSD_STATUS MACSec_GetMEMData(IN  MSD_QD_DEV dev, IN FIR_MEMTYPE memType, OUT FIR_MACSec_DATA_HW* tcamDataP);

static MSD_STATUS Wait_APBReady(IN  MSD_QD_DEV dev);
static MSD_STATUS Operation_APB(IN  MSD_QD_DEV dev, IN FIR_MACSec_APB_OPERATION opCode, IN FIR_RW_OP rwOp, INOUT MSD_U8* value);

/*MACSec mibs*/
static MSD_STATUS Operation_MIB(IN  MSD_QD_DEV dev, IN FIR_MACSec_MIB_OPERATION opCode, IN MSD_U8 MIB_Index);
static MSD_STATUS Operation_INT(IN  MSD_QD_DEV dev, IN FIR_MACSec_INT_OPERATION opCode, IN MSD_U8 MIB_Index);

/****************************************************************************************************/

/*MACSec related API*/

/****************************************************************************************************/
MSD_STATUS Fir_gmacsecRead
(
	IN MSD_QD_DEV dev,
	IN MSD_U32 regAddr,
	OUT MSD_U32* data
)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U8 value = 0;
	/*step 1 wait Ready*/
	msdSemTake(dev.devNum, dev.apbRegsSem, OS_WAIT_FOREVER);
	/* Wait until the tcam in ready. */
	retVal = Wait_APBReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	/*Step 2, write address*/
	value = regAddr & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B0, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 8) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B1, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 16) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B2, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 24) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B3, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	/*Step 3, action read*/
	value = 0;
	retVal = Operation_APB(dev, FIR_TRANSFER_CONTROL, FIR_APB_READ, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}

	value = (1 << 1) | FIR_APB_READ | ((value | 0x80) & 0xFC);
	retVal = Operation_APB(dev, FIR_TRANSFER_CONTROL, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	/*Step 4, read value*/
	retVal = Operation_APB(dev, FIR_DATA_B0, FIR_APB_READ, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	*data = (MSD_U32)(value & 0xFF);
	retVal = Operation_APB(dev, FIR_DATA_B1, FIR_APB_READ, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	*data += (MSD_U32)(value & 0xFF) << 8;
	retVal = Operation_APB(dev, FIR_DATA_B2, FIR_APB_READ, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	*data += (MSD_U32)(value & 0xFF) << 16;
	retVal = Operation_APB(dev, FIR_DATA_B3, FIR_APB_READ, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	*data += (MSD_U32)(value & 0xFF) << 24;
	msdSemGive(dev.devNum, dev.apbRegsSem);
	return retVal;
}

MSD_STATUS Fir_gmacsecWrite
(
	IN MSD_QD_DEV dev,
	IN MSD_U32 regAddr,
	IN MSD_U32 data
)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U8 value = 0;
	msdSemTake(dev.devNum, dev.apbRegsSem, OS_WAIT_FOREVER);
	retVal = Wait_APBReady(dev);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	/*Step 2, write address*/
	value = regAddr & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B0, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 8) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B1, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 16) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B2, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (regAddr >> 24) & 0xFF;
	retVal = Operation_APB(dev, FIR_ADDRESS_B3, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}

	/*Step 3, write data*/
	value = data & 0xFF;
	retVal = Operation_APB(dev, FIR_DATA_B0, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (data >> 8) & 0xFF;
	retVal = Operation_APB(dev, FIR_DATA_B1, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (data >> 16) & 0xFF;
	retVal = Operation_APB(dev, FIR_DATA_B2, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	value = (data >> 24) & 0xFF;
	retVal = Operation_APB(dev, FIR_DATA_B3, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	/*Step 4, action Write*/
	value = (1 << 1) | FIR_APB_WRITE | 0x80;
	retVal = Operation_APB(dev, FIR_TRANSFER_CONTROL, FIR_APB_WRITE, &value);
	if (retVal != MSD_OK)
	{
		msdSemGive(dev.devNum, dev.apbRegsSem);
		return retVal;
	}
	msdSemGive(dev.devNum, dev.apbRegsSem);
	return retVal;
}

MSD_STATUS Fir_gmacsecFlushAll
(
	IN MSD_QD_DEV *dev
)
{
	/*Flush all*/
	MSD_STATUS status = MSD_OK;

	status = Fir_OperationPerform(dev, BASE_TCAM, FIR_TYPE_RULE, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_TCAM_ACTION, FIR_TYPE_ACTION, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_RX_SECY, FIR_TYPE_RX_SECY, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_TX_SECY, FIR_TYPE_TX_SECY, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_RX_SC, FIR_TYPE_RX_SC, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_TX_SC, FIR_TYPE_TX_SC, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_RX_SA, FIR_TYPE_RX_SA, FIR_MACSec_FLUSH_ALL, NULL, 0);
	status = Fir_OperationPerform(dev, BASE_TX_SA, FIR_TYPE_RX_SA, FIR_MACSec_FLUSH_ALL, NULL, 0);
	return status;
}

MSD_STATUS Fir_gmacsecCreateRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	IN FIR_MSEC_RULE* rule
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (ruleIndex >= FIR_MAXRULEINDEX)
	{
		MSD_DBG_ERROR(("Bad ruleIndex: It should be within %u\n", FIR_MAXRULEINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		/*TCAM load operation.*/
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.rule), rule, sizeof(FIR_MSEC_RULE));
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM, FIR_TYPE_RULE, FIR_MACSec_LOAD_ENTRY, &tmp, ruleIndex);
		if (tempStatus != MSD_OK)
		{
			return MSD_FAIL;
		}
		/*TCAM Action*/
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.action), &(rule->action), sizeof(FIR_MSEC_TCAM_ACTION));
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM_ACTION, FIR_TYPE_ACTION, FIR_MACSec_LOAD_ENTRY, &tmp, ruleIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex
)
{
	MSD_STATUS tempStatus = MSD_OK;
	if (ruleIndex >= FIR_MAXRULEINDEX)
	{
		MSD_DBG_ERROR(("Bad ruleIndex: It should be within %u\n", FIR_MAXRULEINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		/*TCAM delete operation.*/
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM, FIR_TYPE_RULE, FIR_MACSec_FLUSH_ONE, NULL, ruleIndex);
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM_ACTION, FIR_TYPE_ACTION, FIR_MACSec_FLUSH_ONE, NULL, ruleIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetRule
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 ruleIndex,
	OUT FIR_MSEC_RULE* rule
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (ruleIndex >= FIR_MAXRULEINDEX)
	{
		MSD_DBG_ERROR(("Bad ruleIndex: It should be within %u\n", FIR_MAXRULEINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		/*TCAM read operation.*/
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM, FIR_TYPE_RULE, FIR_MACSec_READ_ENTRY, &tmp, ruleIndex);
		msdMemCpy(rule, &(tmp.rule), sizeof(FIR_MSEC_RULE));
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_TCAM_ACTION, FIR_TYPE_ACTION, FIR_MACSec_READ_ENTRY, &tmp, ruleIndex);
		msdMemCpy(&(rule->action), &(tmp.action), sizeof(FIR_MSEC_TCAM_ACTION));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecCreateRxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	IN FIR_SECY_INGRESS* rxsecY
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.rxSecy), rxsecY, sizeof(FIR_SECY_INGRESS));
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SECY, FIR_TYPE_RX_SECY, FIR_MACSec_LOAD_ENTRY, &tmp, secYIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecCreateTxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	IN FIR_SECY_EGRESS* txsecY
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.txSecy), txsecY, sizeof(FIR_SECY_EGRESS));
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SECY, FIR_TYPE_TX_SECY, FIR_MACSec_LOAD_ENTRY, &tmp, secYIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteTxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex
)
{
	MSD_STATUS tempStatus = MSD_OK;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SECY, FIR_TYPE_TX_SECY, FIR_MACSec_FLUSH_ONE, NULL, secYIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteRxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex
	)
{
	MSD_STATUS tempStatus = MSD_OK;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SECY, FIR_TYPE_RX_SECY, FIR_MACSec_FLUSH_ONE, NULL, secYIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetTxSecY
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYIndex,
	OUT FIR_SECY_EGRESS* secY
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SECY, FIR_TYPE_TX_SECY, FIR_MACSec_READ_ENTRY, &tmp, secYIndex);
		msdMemCpy(secY, &(tmp.txSecy), sizeof(FIR_SECY_EGRESS));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetRxSecY
(
	IN MSD_QD_DEV* dev,
	IN MSD_U8 secYIndex,
	OUT FIR_SECY_INGRESS* secY
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (secYIndex >= FIR_MAXSECYINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSECYINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SECY, FIR_TYPE_RX_SECY, FIR_MACSec_READ_ENTRY, &tmp, secYIndex);
		msdMemCpy(secY, &(tmp.rxSecy), sizeof(FIR_SECY_INGRESS));
	}

	return tempStatus;
}
MSD_STATUS Fir_gmacsecCreateRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	IN FIR_MSEC_RX_SC* rxSC
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.rxSc), rxSC, sizeof(FIR_MSEC_RX_SC));
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SC, FIR_TYPE_RX_SC, FIR_MACSec_LOAD_ENTRY, &tmp, scIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SC, FIR_TYPE_RX_SC, FIR_MACSec_FLUSH_ONE, NULL, scIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetRxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT FIR_MSEC_RX_SC* rxSC
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SC, FIR_TYPE_RX_SC, FIR_MACSec_READ_ENTRY, &tmp, scIndex);
		msdMemCpy(rxSC, &(tmp.rxSc), sizeof(FIR_MSEC_RX_SC));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecCreateTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	IN FIR_MSEC_TX_SC* txSC
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.txSc), txSC, sizeof(FIR_MSEC_TX_SC));
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SC, FIR_TYPE_TX_SC, FIR_MACSec_LOAD_ENTRY, &tmp, scIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SC, FIR_TYPE_TX_SC, FIR_MACSec_FLUSH_ONE, NULL, scIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetTxSC
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scIndex,
	OUT FIR_MSEC_TX_SC* txSC
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (scIndex >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad scIndex: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SC, FIR_TYPE_TX_SC, FIR_MACSec_READ_ENTRY, &tmp, scIndex);
		msdMemCpy(txSC, &(tmp.txSc), sizeof(FIR_MSEC_TX_SC));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecCreateRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN FIR_MSEC_RX_SA* rxSA
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.rxSa), rxSA, sizeof(FIR_MSEC_RX_SA));
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SA, FIR_TYPE_RX_SA, FIR_MACSec_LOAD_ENTRY, &tmp, saIndex);
		tempStatus |= Fir_SAKOperation(dev, FIR_SAK_LOAD_RX, saIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_RX_SA, FIR_TYPE_RX_SA, FIR_MACSec_FLUSH_ONE, NULL, saIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetRxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT FIR_MSEC_RX_SA* rxSA
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));		
		tempStatus = Fir_SAKOperation(dev, FIR_SAK_READ_RX, saIndex);
		tempStatus |= Fir_OperationPerform(dev, BASE_RX_SA, FIR_TYPE_RX_SA, FIR_MACSec_READ_ENTRY, &tmp, saIndex);
		msdMemCpy(rxSA, &(tmp.rxSa), sizeof(FIR_MSEC_RX_SA));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecCreateTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	IN FIR_MSEC_TX_SA* txSA
) {
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		msdMemCpy(&(tmp.txSa), txSA, sizeof(FIR_MSEC_TX_SA));
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SA, FIR_TYPE_TX_SA, FIR_MACSec_LOAD_ENTRY, &tmp, saIndex);
		tempStatus |= Fir_SAKOperation(dev, FIR_SAK_LOAD_TX, saIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecDeleteTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex
) 
{
	MSD_STATUS tempStatus = MSD_OK;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		tempStatus = Fir_OperationPerform(dev, BASE_TX_SA, FIR_TYPE_TX_SA, FIR_MACSec_FLUSH_ONE, NULL, saIndex);
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetTxSA
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 saIndex,
	OUT FIR_MSEC_TX_SA* txSA
)
{
	MSD_STATUS tempStatus = MSD_OK;
	FIR_MACSec_DATA_HW tmp;
	if (saIndex >= FIR_MAXSAINDEX)
	{
		MSD_DBG_ERROR(("Bad saIndex: It should be within %u\n", FIR_MAXSAINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		msdMemSet(&tmp, 0, sizeof(FIR_MACSec_DATA_HW));
		tempStatus = Fir_SAKOperation(dev, FIR_SAK_READ_TX, saIndex);
		tempStatus |= Fir_OperationPerform(dev, BASE_TX_SA, FIR_TYPE_TX_SA, FIR_MACSec_READ_ENTRY, &tmp, saIndex);
		msdMemCpy(txSA, &(tmp.txSa), sizeof(FIR_MSEC_TX_SA));
	}

	return tempStatus;
}

MSD_STATUS Fir_gmacsecGetTXSecYStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_TX_SECY_COUNTER* states
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 data = 0;
	if (secYCounterIndex >= FIR_MAXMIBINDEX)
	{
		MSD_DBG_ERROR(("Bad secYCounterIndex: It should be within %u\n", FIR_MAXMIBINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		if (FIR_ENABLE == autoClear)
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_TX_SECY_AUTO_CLEAR, secYCounterIndex);
		}
		else
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_TX_SECY_NO_AUTO_CLEAR, secYCounterIndex);
		}
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_CPTOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYControlledPort = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_CPTOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYControlledPort |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_ENCOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYEncrypted = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_ENCOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYEncrypted |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_PROOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYProtected = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_PROOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outOctetsSecYProtected |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_CPTUNIFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsSecYUnicast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_CPTMULFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsSecYMulticast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_CPTBRDFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktSecYBroadcast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_UNTAG_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsSecYuntagged = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_TOOLONG_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsSecYTooLong = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_TX_NOSAERROR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsSecYNoSAError = (data & 0xFFFFFFFF);
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetTXSCStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_TX_SC_COUNTER* states
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 data = 0;
	if (scCounterIndex >= FIR_MAXMIBINDEX)
	{
		MSD_DBG_ERROR(("Bad scCounterIndex: It should be within %u\n", FIR_MAXMIBINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		if (FIR_ENABLE == autoClear)
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_TX_SC_AUTO_CLEAR, scCounterIndex);
		}
		else
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_TX_SC_NO_AUTO_CLEAR, scCounterIndex);
		}
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_TX_SC_ENC_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsEncrypted = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_TX_SC_PRO_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetTXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->outPktsProtected = (data & 0xFFFFFFFF);
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetRXSecYStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 secYCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_RX_SECY_COUNTER* states
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 data = 0;
	if (secYCounterIndex >= FIR_MAXMIBINDEX)
	{
		MSD_DBG_ERROR(("Bad secYCounterIndex: It should be within %u\n", FIR_MAXMIBINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		if (FIR_ENABLE == autoClear)
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_RX_SECY_AUTO_CLEAR, secYCounterIndex);
		}
		else
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_RX_SECY_NO_AUTO_CLEAR, secYCounterIndex);
		}
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_CPTOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYControlledPort = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_CPTOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYControlledPort |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_VLDOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYValidated = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_VLDOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYValidated |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_DECOCT_HICAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYDecrypted = (MSD_U64)(((MSD_U64)data & 0xFFFFFFFF) << 32);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_DECOCT_LOCAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inOctetsSecYDecrypted |= (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_CPTUNIFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYUnicast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_CPTMULFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYMulticast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_CPTBRDFR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYBroadCast = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_UNTAGGED_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYUnTagged = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_NOTAG_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYNoTag = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_BADTAG_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYBadTag = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_NOSA_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYNoSA = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SECY_MIB + FIR_MSEC_RX_NOSAERR_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSecYStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSecYNoSAErr = (data & 0xFFFFFFFF);
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetRXSCStates
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 scCounterIndex,
	IN FIR_AUTO_CLEAR autoClear,
	OUT FIR_MSEC_RX_SC_COUNTER* states
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 data = 0;
	if (scCounterIndex >= FIR_MAXMIBINDEX)
	{
		MSD_DBG_ERROR(("Bad scCounterIndex: It should be within %u\n", FIR_MAXMIBINDEX));
		return MSD_BAD_PARAM;
	}
	else
	{
		if (FIR_ENABLE == autoClear)
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_RX_SC_AUTO_CLEAR, scCounterIndex);
		}
		else
		{
			status = Operation_MIB(*dev, FIR_CAPTURE_RX_SC_NO_AUTO_CLEAR, scCounterIndex);
		}
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_OK_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCOK = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_UNCHK_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCUnchecked = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_DLYED_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCDelayed = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_LATE_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCLate = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_INVLD_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCInvalid = (data & 0xFFFFFFFF);
		status = Fir_gmacsecRead(*dev, BASE_SC_MIB + FIR_MSEC_RX_SC_NOVLD_CAP_OFFSET, &data);
		if (MSD_OK != status)
		{
			MSD_DBG_ERROR(("FIR_MACSecGetRXSCStates function returned: %s.\n", msdDisplayStatus(status)));
			return MSD_FAIL;
		}
		states->inPktsSCNotValid = (data & 0xFFFFFFFF);
	}
	return status;
}
MSD_STATUS Fir_gmacsecSetTxPNThreshold
(
	IN MSD_QD_DEV *dev,
	IN MSD_U32 pnThr
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	regAddr = BASE_TXNEXTPN_THR;
	status = Fir_gmacsecWrite(*dev, regAddr, (MSD_U32)pnThr & 0xFFFFFFFF); 
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecSetTxPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetTxPNThreshold
(
	IN MSD_QD_DEV *dev,
	OUT MSD_U32* pnThr
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	regAddr = BASE_TXNEXTPN_THR;
	status = Fir_gmacsecRead(*dev, regAddr, &data); 
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecGetTxPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*pnThr = (MSD_U32)data;
	return status;
}
MSD_STATUS Fir_gmacsecSetTxXPNThreshold
(
	IN MSD_QD_DEV *dev,
	IN MSD_U64 xpnThr
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	regAddr = BASE_TXNEXTPN_THR;
	status = Fir_gmacsecWrite(*dev, regAddr + 8, (MSD_U32)xpnThr & 0xFFFFFFFF);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecSetTxXPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	status = Fir_gmacsecWrite(*dev, regAddr + 4, (MSD_U32)(xpnThr >> 32) & 0xFFFFFFFF);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecSetTxXPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetTxXPNThreshold
(
	IN MSD_QD_DEV *dev,
	OUT MSD_U64 *xpnThr
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	regAddr = BASE_TXNEXTPN_THR;
	status = Fir_gmacsecRead(*dev, regAddr + 8, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecGetTxXPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*xpnThr = (MSD_U64)data;
	status = Fir_gmacsecRead(*dev, regAddr + 4, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecGetTxXPNThreshold function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*xpnThr = (*xpnThr) | ((MSD_U64)data << 32);
	return status;
}
MSD_STATUS Fir_gmacsecSetRxDefaultSCI
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_U64 defaultRxSCI
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_DEFRXSCI + (portNum << 7);
	status = Fir_gmacsecWrite(*dev, regAddr, (MSD_U32)((defaultRxSCI >> 32) & 0xFFFFFFFF));
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecSetRxDefaultSCI function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	status = Fir_gmacsecWrite(*dev, regAddr + 4, (MSD_U32)defaultRxSCI & 0xFFFFFFFF);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecSetRxDefaultSCI function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	return status;
}
MSD_STATUS Fir_gmacsecGetRxDefaultSCI
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	OUT MSD_U64 *defaultRxSCI
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_DEFRXSCI + (portNum << 7);
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecGetRxDefaultSCI function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*defaultRxSCI = (MSD_U64)data << 32;
	status = Fir_gmacsecRead(*dev, regAddr + 4, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecGetRxDefaultSCI function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*defaultRxSCI = (*defaultRxSCI) | (MSD_U64)data;
	return status;
}

MSD_STATUS Fir_gmacsecSetTxSCIntCtrl
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	IN FIR_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr;
	MSD_U32 tempData;
	tempData = 0;

	if (index >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad SC Index: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	/* Global enable TXSC Interrupt.*/
	tempData = Fir_gmacsecRead(*dev, GLOBAL_CTRL_ADDR, &tempData);
	tempData = (tempData | (0x1 << 19));
	status = Fir_gmacsecWrite(*dev, GLOBAL_CTRL_ADDR, tempData);

	regAddr = BASE_TX_SC + 0x14;
	tempData = (txscIntCtrl->txPNThrIntEn << 18) + (txscIntCtrl->txSAInvalidIntEn << 17) + (txscIntCtrl->txAutoRekeyIntEn << 16);
	status = Fir_gmacsecWrite(*dev, regAddr, tempData);

	regAddr = BASE_TX_SC;
	/*Wait until the tcam in ready. */
	status = Fir_WaitOperationReady(*dev, regAddr);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return status;
	}

	tempData = (MSD_U32)((1 << 15) | (FIR_MACSec_LOAD_ENTRY << 12) | index);
	status = Fir_gmacsecWrite(*dev, regAddr, tempData);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(status)));
		return status;
	}

	/*Wait until the tcam in ready. */
	status = Fir_WaitOperationReady(*dev, regAddr);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return status;
	}
	return status;
}

MSD_STATUS Fir_gmacsecGetTxSCIntCtrl
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	OUT FIR_MACSEC_INT_CONTROL_TXSC* txscIntCtrl
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr;
	MSD_U32 data = 0;
	MSD_U32 tempData;
	
	if (index >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad SC Index: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_TX_SC;

	/*Wait until the tcam in ready. */
	status = Fir_WaitOperationReady(*dev, regAddr);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return status;
	}

	tempData = 0;
	tempData = (MSD_U32)((1 << 15) | (FIR_MACSec_READ_ENTRY << 12) | index);
	status = Fir_gmacsecWrite(*dev, regAddr, tempData);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(status)));
		return status;
	}

	/*Wait until the tcam in ready. */
	status = Fir_WaitOperationReady(*dev, regAddr);
	if (status != MSD_OK)
	{
		msdSemGive(dev->devNum, dev->tblRegsSem);
		return status;
	}

	regAddr = BASE_TX_SC + 0x14;
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	txscIntCtrl->txPNThrIntEn = (MSD_BOOL)((data>>18)&1);
	txscIntCtrl->txSAInvalidIntEn = (MSD_BOOL)((data >> 17) & 1);
	txscIntCtrl->txAutoRekeyIntEn = (MSD_BOOL)((data >> 16) & 1);

	return status;
}


MSD_STATUS Fir_gmacsecGetNextTxSCIntStatus
(
	IN MSD_QD_DEV *dev,
	IN MSD_U8 index,
	IN MSD_BOOL isAutoClear,
	OUT MSD_U8* nextIndex,
	OUT FIR_MACSEC_INT_TXSC* txscInt
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 data = 0;
	FIR_MACSec_INT_OPERATION opMode;
	msdMemSet(txscInt, 0, sizeof(FIR_MACSEC_INT_TXSC));

	if (index >= FIR_MAXSCINDEX)
	{
		MSD_DBG_ERROR(("Bad SC Index: It should be within %u\n", FIR_MAXSCINDEX));
		return MSD_BAD_PARAM;
	}
	if (MSD_TRUE == isAutoClear)
	{
		opMode = FIR_NEXT_TX_SC_INT_AUTO_CLEAR;
	}
	else
	{
		opMode = FIR_NEXT_TX_SC_INT_NO_AUTO_CLEAR;
	}
	status = Operation_INT(*dev, opMode, index);
	status = Fir_gmacsecRead(*dev, INT_OP_ADDR, &data);
	if ((data & 0xF) == 0xF && (data & 0x7F0000) == 0)
	{
		return MSD_NO_SUCH;
	}
	else
	{
		*nextIndex = data & 0xF;
		txscInt->txAutoRekeyInt = (data >> 16) & 0x1;
		txscInt->txSAInvalidInt = (data >> 17) & 0x1;
		txscInt->txPNThrInt = (data >> 18) & 0x1;
	}
	return status;
}

MSD_STATUS Fir_gmacsecEnableIngressSet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_RXPortCTRL + (portNum << 7);
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecRead function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	data = (isEnable << 31) | (data & 0x7FFFFFFF);
	status = Fir_gmacsecWrite(*dev, regAddr, data & 0xFFFFFFFF);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function MPDU returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	return status;
}

MSD_STATUS Fir_gmacsecEnableEgressSet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	IN MSD_BOOL isEnable
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_TXPortCTRL + (portNum << 7);
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecRead function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	data = (isEnable << 31) | (data & 0x7FFFFFFF);
	status = Fir_gmacsecWrite(*dev, regAddr, data & 0xFFFFFFFF);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	return status;
}

MSD_STATUS Fir_gmacsecEnableIngressGet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_RXPortCTRL + (portNum << 7);
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*isEnable = (data >> 31) & 0x1;
	return status;
}

MSD_STATUS Fir_gmacsecEnableEgressGet
(
	IN MSD_QD_DEV *dev,
	IN MSD_U16 portNum,
	OUT MSD_BOOL* isEnable
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 data = 0;
	if (!((portNum <= 7) && (portNum >= 1)))
	{
		MSD_DBG_ERROR(("Bad Port: %u. It should be within [1,7]\n", (unsigned int)portNum));
		return MSD_BAD_PARAM;
	}
	regAddr = BASE_TXPortCTRL + (portNum << 7);	
	
	status = Fir_gmacsecRead(*dev, regAddr, &data);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	*isEnable = (data >> 31) & 0x1;
	return status;
}

MSD_STATUS Fir_gmacsecGetEvent
(
	IN MSD_QD_DEV *dev,
	OUT FIR_MSEC_EVENT* event
)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 regAddr = 0;
	MSD_U32 temp0 = 0;
	MSD_U32 temp1 = 0;
	MSD_U32 temp2 = 0;
	regAddr = GLOBAL_CTRL_ADDR;

	/* get the EVENT VALUE */
	status = Fir_gmacsecRead(*dev, regAddr + FIR_MSEC_ECC_ERR_EVTS_OFFSET, &temp0);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function MPDU returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	status = Fir_gmacsecRead(*dev, regAddr + FIR_MSEC_ECC_MISMAC_EVTS_OFFSET, &temp1);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function MPDU returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	status = Fir_gmacsecRead(*dev, regAddr + FIR_MSEC_SMS_RPE_EVTS_OFFSET, &temp2);
	if (MSD_OK != status)
	{
		MSD_DBG_ERROR(("FIR_MACSecEnableSet function MPDU returned: %s.\n", msdDisplayStatus(status)));
		return MSD_FAIL;
	}
	event->msecCECCErrEvent = (MSD_U32)((temp0 >> 16) & 0x1FF);
	event->msecUCECCErrEvent = (MSD_U32)(temp0 & 0x1FF);
	event->msecCECCErrInjectMisEvent = (MSD_U32)((temp1 >> 16) & 0x1FF);
	event->msecCECCOutMisEvent = (MSD_U32)(temp1 & 0x1FF);
	event->msecRegParityErrEvent = (MSD_BOOL)((temp2 >> 16) & 0x1);
	event->msecCECCErrEvent = (MSD_U32)(temp2 & 0x1FF);
	return status;
}

static MSD_STATUS Operation_MIB(IN  MSD_QD_DEV dev, IN FIR_MACSec_MIB_OPERATION opCode, IN MSD_U8 MIB_Index)
{
	MSD_STATUS retVal = MSD_OK;
	retVal = Fir_WaitOperationReady(dev, MIB_OP_ADDR);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_MIB function returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	retVal = Fir_gmacsecWrite(dev, MIB_OP_ADDR, (1 << 15) | (opCode << 11) | (MIB_Index));
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_MIB function returned: %s.\n", msdDisplayStatus(retVal)));
	}

	retVal = Fir_WaitOperationReady(dev, MIB_OP_ADDR);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_MIB function returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	return retVal;
}
static MSD_STATUS Operation_INT(IN  MSD_QD_DEV dev, IN FIR_MACSec_INT_OPERATION opCode, IN MSD_U8 Index)
{
	MSD_STATUS retVal = MSD_OK;
	retVal = Fir_WaitOperationReady(dev, INT_OP_ADDR);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_INT function returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	retVal = Fir_gmacsecWrite(dev, INT_OP_ADDR, (1 << 15) | (opCode << 11) | (Index));
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_INT function returned: %s.\n", msdDisplayStatus(retVal)));
	}
	retVal = Fir_WaitOperationReady(dev, INT_OP_ADDR);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("Operation_INT function returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}
	return retVal;
}
/*MACSec interrupt*/
static MSD_STATUS Operation_APB(IN  MSD_QD_DEV dev, IN FIR_MACSec_APB_OPERATION opCode, IN FIR_RW_OP rwOp, INOUT MSD_U8* value)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U16 tempData = 0;
	if (rwOp == FIR_APB_READ)
	{
		tempData = (opCode & 0x7F) << 8;
		retVal = msdSetAnyReg(dev.devNum, APB_PHYADDR, APB_REGADDR, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Operation_APB function returned: %s.\n", msdDisplayStatus(retVal)));
		}
		else
		{
			retVal = msdGetAnyReg(dev.devNum, APB_PHYADDR, APB_REGADDR, &tempData);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("Operation_APB function returned: %s.\n", msdDisplayStatus(retVal)));
			}
			*value = tempData & 0xFF;
		}
	}
	else
	{
		tempData = ((1 << 15) | (MSD_U16)((opCode & 0x7F) << 8) | (MSD_U16)((*value) & 0xFF));
		retVal = msdSetAnyReg(dev.devNum, APB_PHYADDR, APB_REGADDR, tempData);
	}
	return retVal;
}

static MSD_STATUS Wait_APBReady(IN  MSD_QD_DEV dev)
{
	MSD_STATUS retVal = MSD_OK;
	MSD_U8 tempData = 1 << 7;
	while (((tempData >> 7) & 0x1) == 1)
	{
		retVal = Operation_APB(dev, FIR_TRANSFER_CONTROL, FIR_APB_READ, &tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Wait_APBReady function returned: %s.\n", msdDisplayStatus(retVal)));
			break;
		}
	}
	return retVal;
}

/*Convert Array to Mem Data*/
static FIR_MSEC_RULE* ConvertArrayToRule(IN  MSD_QD_DEV dev)
{
	static FIR_MSEC_RULE temp_rule;
	MSD_U8 i;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[TCAM_WIDTH] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_RULE, _data_addr);
	data_addr = (MSD_U32)_data_addr;

	for (i = 0; i < TCAM_WIDTH; i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}

	temp_rule.maskSPV = (array[1] >> 17) & 0x7F;
	temp_rule.keySPV = (array[1] >> 1) & 0x7F;

	temp_rule.maskSecTagC = (array[3] >> 23) & 0x1;
	temp_rule.keySecTagC = (array[3] >> 7) & 0x1;

	temp_rule.maskSecTagE = (array[3] >> 22) & 0x1;
	temp_rule.keySecTagE = (array[3] >> 6) & 0x1;

	temp_rule.maskSecTag = (array[3] >> 21) & 0x1;
	temp_rule.keySecTag = (array[3] >> 5) & 0x1;

	temp_rule.maskSkipTag = (array[3] >> 20) & 0x1;
	temp_rule.keySkipTag = (array[3] >> 4) & 0x1;

	temp_rule.maskRTag = (array[3] >> 19) & 0x1;
	temp_rule.keyRTag = (array[3] >> 3) & 0x1;

	temp_rule.maskQTag = (array[3] >> 18) & 0x1;
	temp_rule.keyQTag = (array[3] >> 2) & 0x1;

	temp_rule.maskADTag = (array[3] >> 17) & 0x1;
	temp_rule.keyADTag = (array[3] >> 1) & 0x1;

	temp_rule.maskDirection = (array[3] >> 16) & 0x1;
	temp_rule.keyDirection = array[3] & 0x1;

	for (i = 0; i < 3; i++)
	{
		temp_rule.keyMacDA[2 * i] = (array[4 + i] >> 8) & 0xFF;
		temp_rule.maskMacDA[2 * i] = (array[4 + i] >> 24) & 0xFF;

		temp_rule.keyMacDA[2 * i + 1] = array[4 + i] & 0xFF;
		temp_rule.maskMacDA[2 * i + 1] = (array[4 + i] >> 16) & 0xFF;
	}

	for (i = 0; i < 3; i++)
	{
		temp_rule.keyMacSA[2 * i] = (array[7 + i] >> 8) & 0xFF;
		temp_rule.maskMacSA[2 * i] = (array[7 + i] >> 24) & 0xFF;

		temp_rule.keyMacSA[2 * i + 1] = array[7 + i] & 0xFF;
		temp_rule.maskMacSA[2 * i + 1] = (array[7 + i] >> 16) & 0xFF;
	}

	temp_rule.maskAdPCP = (array[10] >> 28) & 0xF;
	temp_rule.keyAdPCP = (array[10] >> 12) & 0xF;

	temp_rule.maskAdVID = (array[10] >> 16) & 0xFFF;
	temp_rule.keyAdVID = array[10] & 0xFFF;

	temp_rule.maskQPCP = (array[11] >> 28) & 0xF;
	temp_rule.keyQPCP = (array[11] >> 12) & 0xF;

	temp_rule.maskQVID = (array[11] >> 16) & 0xFFF;
	temp_rule.keyQVID = array[11] & 0xFFF;

	temp_rule.maskLengthType = (array[12] >> 16) & 0xFFFF;
	temp_rule.keyLengthType = array[12] & 0xFFFF;

	return &temp_rule;
}
static FIR_MSEC_TCAM_ACTION* ConvertArrayToAction(IN MSD_QD_DEV dev)
{
	static FIR_MSEC_TCAM_ACTION action;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 tempData = 0;
	GET_MEM_ADDR(FIR_TYPE_ACTION, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	Fir_gmacsecRead(dev, data_addr, &tempData);
	action.isControlledPort = (((tempData >> 15) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	action.secYIndex = tempData & 0xF;
	action.txSCIndex = (tempData >> 16) & 0xF;
	return &action;
}
static FIR_SECY_INGRESS* ConvertArrayToRxSecY(IN  MSD_QD_DEV dev)
{
	static FIR_SECY_INGRESS rxSecY;
	MSD_U8 i;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[RX_SECY_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_RX_SECY, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (RX_SECY_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	rxSecY.allowZeroRxPN = (((array[0] >> 24) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	rxSecY.secYCounterIndex = (array[0] >> 16) & 0xF;
	rxSecY.rxConfOffset = (array[0] >> 8) & 0x3F;
	rxSecY.replayProtect = (((array[0] >> 7) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	rxSecY.cipher = (array[0] >> 4) & 0x3;
	rxSecY.validateFrame = (array[0] >> 1) & 0x3;
	rxSecY.rxSecYEn = ((array[0] & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	rxSecY.replayWindow = array[1];
	return &rxSecY;
}
static FIR_SECY_EGRESS* ConvertArrayToTxSecY(IN  MSD_QD_DEV dev)
{
	static FIR_SECY_EGRESS txSecY;
	MSD_U8 i;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[TX_SECY_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_TX_SECY, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (TX_SECY_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	txSecY.secYCounterIndex = (array[0] >> 16) & 0xF;
	txSecY.txConfOffset = (array[0] >> 8) & 0x3F;
	txSecY.cipher = (MSD_CIPHER_SUITE)((array[0] >> 4) & 0x3);
	txSecY.protectFrame = (((array[0] >> 1) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSecY.txSecYEn = ((array[0] & 1) == 1) ? MSD_TRUE : MSD_FALSE;

	txSecY.afterSkipTag = (((array[1] >> 11) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSecY.afterRTag = (((array[1] >> 10) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSecY.afterQTag = (((array[1] >> 9) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSecY.afterADTag = (((array[1] >> 8) & 1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSecY.secTagTCI = array[1] & 0x3F;
	return &txSecY;
}
static FIR_MSEC_RX_SC* ConvertArrayToRxSc(IN  MSD_QD_DEV dev)
{
	static FIR_MSEC_RX_SC rxSC;
	MSD_U8 i;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[RX_SC_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_RX_SC, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (RX_SC_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	rxSC.scCounterIndex = (array[0] >> 16) & 0xF;
	rxSC.secYIndex = array[0] & 0xF;
	rxSC.sci = (((MSD_U64)array[1] << 32) | array[2]);
	rxSC.sa0An = (array[3] >> 12) & 0x3;
	rxSC.sa0Index = array[3] & 0x1F;
	rxSC.sa0Valid = (((array[3] >> 14) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	rxSC.sa1An = (array[3] >> 28) & 0x3;
	rxSC.sa1Index = (array[3] >> 16) & 0x1F;
	rxSC.sa1Valid = (((array[3] >> 30) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	return &rxSC;
}
static FIR_MSEC_TX_SC* ConvertArrayToTxSc(IN  MSD_QD_DEV dev)
{
	static FIR_MSEC_TX_SC txSC;
	MSD_U8 i;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[TX_SC_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_TX_SC, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (TX_SC_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	txSC.scCounterIndex = (array[0] >> 16) & 0xF;
	txSC.txNextPNRolloEn = ((array[0] & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSC.sci = (((MSD_U64)array[1] << 32) | array[2]);
	txSC.enableAutoRekey = (((array[3] >> 31) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSC.sa1Valid = (((array[3] >> 30) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSC.sa1An = (array[3] >> 28) & 0x3;
	txSC.sa1Index = (array[3] >> 16) & 0x1F;
	txSC.isActiveSA1 = (((array[3] >> 15) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSC.sa0Valid = (((array[3] >> 14) & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSC.sa0An = (array[3] >> 12) & 0x3;
	txSC.sa0Index = array[3] & 0x1F;
	return &txSC;
}
static FIR_MSEC_RX_SA* ConvertArrayToRxSa(IN  MSD_QD_DEV dev)
{
	static FIR_MSEC_RX_SA rxSA;
	MSD_U8 i, j;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[RX_SA_WIDTH + SAK_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_RX_SA, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (RX_SA_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	rxSA.rxSAValid = ((array[0] & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	rxSA.ssci = array[1] & 0xFFFFFFFFU;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 4; j++)
		{
			rxSA.salt[i * 4 + j] = (array[i + 2] >> (3 - j) * 8) & 0xFF;
		}
	}
	rxSA.nextPN = ((MSD_U64)array[5] << 32) | array[6];
	data_addr = (MSD_U32)BASE_SAK;
	for (i =(RX_SA_WIDTH -1); i < (RX_SA_WIDTH + SAK_WIDTH - 2); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * (i - RX_SA_WIDTH + 2), &array[i]);
	}
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 4; j++)
		{
			rxSA.sak[i * 4 + j] = (array[i + 7] >> (3 - j) * 8) & 0xFF;
		}
	}
	return &rxSA;
}
static FIR_MSEC_TX_SA* ConvertArrayToTxSa(IN  MSD_QD_DEV dev)
{
	static FIR_MSEC_TX_SA txSA;
	MSD_U8 i, j;
	MSD_U32 data_addr = 0;
	MSD_32 _data_addr = 0;
	MSD_U32 array[TX_SA_WIDTH + SAK_WIDTH - 1] = { 0 };
	GET_MEM_ADDR(FIR_TYPE_TX_SA, _data_addr);
	data_addr = (MSD_U32)_data_addr;
	data_addr += regBusWidth;
	for (i = 0; i < (TX_SA_WIDTH - 1); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * i, &array[i]);
	}
	txSA.txSAValid = ((array[0] & 0x1) == 1) ? MSD_TRUE : MSD_FALSE;
	txSA.ssci = array[1] & 0xFFFFFFFFU;
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 4; j++)
		{
			txSA.salt[i * 4 + j] = (array[i + 2] >> (3-j) * 8) & 0xFF;
		}
	}
	txSA.nextPN = ((MSD_U64)array[5] << 32) | array[6];
	data_addr = (MSD_U32)BASE_SAK;
	for (i = (TX_SA_WIDTH - 1); i < (TX_SA_WIDTH + SAK_WIDTH - 2); i++)
	{
		Fir_gmacsecRead(dev, data_addr + regBusWidth * (i - TX_SA_WIDTH + 2), &array[i]);
	}
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 4; j++)
		{
			txSA.sak[i * 4 + j] = (array[i + 7] >> (3 - j) * 8) & 0xFF;
		}
	}
	return &txSA;
}


/*Convert Mem Data to Array*/
static MSD_U32* ConvertRuleToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[TCAM_WIDTH] = { 0 };
	MSD_U8 i;

	array[1] = (dataRw->rule.maskSPV << 17) + (dataRw->rule.keySPV << 1);
	array[3] = ((dataRw->rule.maskSecTagC) << 23) + ((dataRw->rule.keySecTagC) << 7) + ((dataRw->rule.maskSecTagE) << 22) + ((dataRw->rule.keySecTagE) << 6)
		+ ((dataRw->rule.maskSecTag) << 21) + ((dataRw->rule.keySecTag) << 5) + ((dataRw->rule.maskSkipTag) << 20) + ((dataRw->rule.keySkipTag) << 4)
		+ ((dataRw->rule.maskRTag) << 19) + ((dataRw->rule.keyRTag) << 3) + ((dataRw->rule.maskQTag) << 18) + ((dataRw->rule.keyQTag) << 2)
		+ ((dataRw->rule.maskADTag) << 17) + ((dataRw->rule.keyADTag) << 1) + ((dataRw->rule.maskDirection) << 16) + dataRw->rule.keyDirection;

	for (i = 0; i < 3; i++)
	{
		array[4 + i] = ((dataRw->rule.maskMacDA[2 * i]) << 24) + ((dataRw->rule.maskMacDA[2 * i + 1]) << 16) + ((dataRw->rule.keyMacDA[2 * i]) << 8) + dataRw->rule.keyMacDA[2 * i + 1];
		array[7 + i] = ((dataRw->rule.maskMacSA[2 * i]) << 24) + ((dataRw->rule.maskMacSA[2 * i + 1]) << 16) + ((dataRw->rule.keyMacSA[2 * i]) << 8) + dataRw->rule.keyMacSA[2 * i + 1];
	}

	array[10] = ((dataRw->rule.maskAdPCP) << 28) + ((dataRw->rule.maskAdVID) << 16) + ((dataRw->rule.keyAdPCP) << 12) + dataRw->rule.keyAdVID;
	array[11] = ((dataRw->rule.maskQPCP) << 28) + ((dataRw->rule.maskQVID) << 16) + ((dataRw->rule.keyQPCP) << 12) + dataRw->rule.keyQVID;
	array[12] = ((dataRw->rule.maskLengthType) << 16) + dataRw->rule.keyLengthType;

	return array;
}
static MSD_U32* ConvertActionToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[TCAM_ACTION_WIDTH] = { 0 };

	array[1] = ((dataRw->action.txSCIndex) << 16) + ((dataRw->action.isControlledPort) << 15) + dataRw->action.secYIndex;

	return array;
}
static MSD_U32* ConvertRXSecYToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[RX_SECY_WIDTH] = { 0 };

	array[1] = ((dataRw->rxSecy.allowZeroRxPN) << 24) 
		+ ((dataRw->rxSecy.secYCounterIndex) << 16) + ((dataRw->rxSecy.rxConfOffset) << 8)
		+ ((dataRw->rxSecy.replayProtect) << 7) + ((dataRw->rxSecy.cipher) << 4)
		+ ((dataRw->rxSecy.validateFrame) << 1) + dataRw->rxSecy.rxSecYEn;

	array[2] = dataRw->rxSecy.replayWindow;

	return array;
}
static MSD_U32* ConvertTXSecYToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[TX_SECY_WIDTH] = { 0 };

	array[1] = ((dataRw->txSecy.secYCounterIndex) << 16) + ((dataRw->txSecy.txConfOffset) << 8)
		+ ((dataRw->txSecy.cipher) << 4) + ((dataRw->txSecy.protectFrame) << 1) + dataRw->txSecy.txSecYEn;

	array[2] = ((dataRw->txSecy.afterSkipTag) << 11) + ((dataRw->txSecy.afterRTag) << 10)
		+ ((dataRw->txSecy.afterQTag) << 9) + ((dataRw->txSecy.afterADTag) << 8) + dataRw->txSecy.secTagTCI;

	return array;
}
static MSD_U32* ConvertRXSCToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[RX_SC_WIDTH] = { 0 };

	array[1] = ((dataRw->rxSc.scCounterIndex) << 16) + dataRw->rxSc.secYIndex;
	array[2] = (dataRw->rxSc.sci >> 32) & 0xFFFFFFFFU;
	array[3] = dataRw->rxSc.sci & 0xFFFFFFFFU;
	array[4] = ((dataRw->rxSc.sa1Valid) << 30) + ((dataRw->rxSc.sa1An) << 28) + ((dataRw->rxSc.sa1Index) << 16)
		+ ((dataRw->rxSc.sa0Valid) << 14) + ((dataRw->rxSc.sa0An) << 12) + dataRw->rxSc.sa0Index;


	return array;
}
static MSD_U32* ConvertTXSCToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 array[TX_SC_WIDTH] = { 0 };

	array[1] = ((dataRw->txSc.scCounterIndex) << 16) + dataRw->txSc.txNextPNRolloEn;
	array[2] = ((dataRw->txSc.sci) >> 32) & 0xFFFFFFFFU;
	array[3] = (dataRw->txSc.sci) & 0xFFFFFFFFU;
	array[4] = ((dataRw->txSc.enableAutoRekey) << 31) + ((dataRw->txSc.sa1Valid) << 30) + ((dataRw->txSc.sa1An) << 28) 
		+ ((dataRw->txSc.sa1Index) << 16) + ((dataRw->txSc.isActiveSA1) << 15) + ((dataRw->txSc.sa0Valid) << 14) 
		+ ((dataRw->txSc.sa0An) << 12) + dataRw->txSc.sa0Index;

	return array;
}

static MSD_U32* ConvertRXSAToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 Array[RX_SA_WIDTH + SAK_WIDTH] = { 0 };
	MSD_U8 i;

	Array[1] = dataRw->rxSa.rxSAValid;
	Array[2] = dataRw->rxSa.ssci;

	for (i = 0; i < 3; i++)
	{
		Array[3 + i] = ((dataRw->rxSa.salt[4 * i]) << 24) + ((dataRw->rxSa.salt[4 * i + 1]) << 16)
			+ ((dataRw->rxSa.salt[4 * i + 2]) << 8) + dataRw->rxSa.salt[4 * i + 3];
	}

	Array[6] = ((dataRw->rxSa.nextPN) >> 32) & 0xFFFFFFFFU;
	Array[7] = (dataRw->rxSa.nextPN) & 0xFFFFFFFFU;

	for (i = 0; i < 8; i++)
	{
		Array[9 + i] = ((dataRw->rxSa.sak[4 * i]) << 24) + ((dataRw->rxSa.sak[4 * i + 1]) << 16)
			+ ((dataRw->rxSa.sak[4 * i + 2]) << 8) + dataRw->rxSa.sak[4 * i + 3];
	}

	return Array;
}
static MSD_U32* ConvertTXSAToArray(IN FIR_MACSec_DATA_HW* dataRw)
{
	static MSD_U32 Array[TX_SA_WIDTH + SAK_WIDTH] = { 0 };
	MSD_U8 i;

	Array[1] = dataRw->txSa.txSAValid;
	Array[2] = dataRw->txSa.ssci;

	for (i = 0; i < 3; i++)
	{
		Array[3 + i] = ((dataRw->txSa.salt[4 * i]) << 24) + ((dataRw->txSa.salt[4 * i + 1]) << 16)
			+ ((dataRw->txSa.salt[4 * i + 2]) << 8) + dataRw->txSa.salt[4 * i + 3];
	}

	Array[6] = ((dataRw->txSa.nextPN) >> 32) & 0xFFFFFFFFU;
	Array[7] = (dataRw->txSa.nextPN) & 0xFFFFFFFFU;

	for (i = 0; i < 8; i++)
	{
		Array[9 + i] = ((dataRw->txSa.sak[4 * i]) << 24) + ((dataRw->txSa.sak[4 * i + 1]) << 16)
			+ ((dataRw->txSa.sak[4 * i + 2]) << 8) + dataRw->txSa.sak[4 * i + 3];
	}
	return Array;
}


/*In case MACSec Read\Write change*/
static MSD_STATUS Fir_MACSecGetAnyRegField
(
	IN  MSD_QD_DEV dev,
	IN  MSD_U32    regAddr,
	IN  MSD_U32    fieldOffset,
	IN  MSD_U32    fieldLength,
	OUT MSD_U32* data
)
{
	MSD_U32 mask;            /* Bits mask to be read */
	MSD_U32 tmpData = 0;
	MSD_STATUS   retVal;

	retVal = Fir_gmacsecRead(dev, regAddr, &tmpData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("MACSecRead function returned: %s.\n", msdDisplayStatus(retVal)));
	}
	else
	{
		MSD_CALC_MASK_32(fieldOffset, fieldLength, mask);

		tmpData = (tmpData & mask) >> fieldOffset;
		*data = tmpData;
		MSD_DBG(("(LOG RR): regAddr 0x%02x, ", regAddr));
		MSD_DBG(("fOff %d, fLen %d, data 0x%04x.\n", fieldOffset, fieldLength, *data));
	}

	return retVal;
}

static MSD_STATUS Fir_WaitOperationReady
(
	IN  MSD_QD_DEV dev,
	IN MSD_32 regAddr
)
{
	MSD_STATUS       retVal = MSD_FAIL;    /* Functions return value */
	MSD_U32          tempData;     /* temporary Data storage */
	tempData = 1;

	while (tempData == 1)
	{
		retVal = Fir_MACSecGetAnyRegField(dev, regAddr, 15, 1, &tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("Fir_MACSecGetAnyRegField returned: %s.\n", msdDisplayStatus(retVal)));
			break;
		}
	}
	return retVal;
}

static MSD_STATUS Fir_SAKOperation
(
	IN  MSD_QD_DEV* dev,
	IN  FIR_MACSec_SAK_OPERATION  sakOp,
	IN	MSD_U8 index
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U32          tempData;     /* temporary Data storage */
	MSD_32			 regAddr;    /* SAK register address */

	regAddr = BASE_SAK;
	/* Wait until the tcam in ready. */
	retVal = Fir_WaitOperationReady(*dev, regAddr);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	tempData = 0;
	tempData = (MSD_U32)((1 << 15) | (sakOp << 12) | index);
	retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
	if (retVal != MSD_OK)
	{
		MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
		return retVal;
	}

	/* Wait until the tcam in ready. */
	retVal = Fir_WaitOperationReady(*dev, regAddr);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	return retVal;
}
static MSD_STATUS Fir_OperationPerform
(
	IN  MSD_QD_DEV* dev,
	IN	MSD_32	regAddr,
	IN	FIR_MEMTYPE memType,
	IN  FIR_MACSec_OPERATION   tcamOp,
	INOUT FIR_MACSec_DATA_HW* opData,
	IN MSD_U8 index
)
{
	MSD_STATUS       retVal;    /* Functions return value */
	MSD_U32          tempData;     /* temporary Data storage */

	/* Wait until the tcam in ready. */
	retVal = Fir_WaitOperationReady(*dev, regAddr);
	if (retVal != MSD_OK)
	{
		return retVal;
	}

	/* Set the TCAM Operation register */
	switch (tcamOp)
	{
	case FIR_MACSec_FLUSH_ALL:
	{
		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		tempData = 0;
		tempData = (MSD_U32)((1 << 15) | (tcamOp << 12));
		retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}
	break;

	case FIR_MACSec_FLUSH_ONE:
	{
		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		tempData = 0;
		tempData = (MSD_U32)((1 << 15) | (tcamOp << 12) | index);
		retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
	}
	break;

	case FIR_MACSec_LOAD_ENTRY:
	{
		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		retVal = MACSec_SetMEMData(*dev, memType, opData);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		tempData = 0;
		tempData = (MSD_U32)((1 << 15) | (tcamOp << 12) | index);
		retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

	}
	break;

	case FIR_MACSec_LOAD_PN_ONLY_GET_NEXT_ENTRY:
	{
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		tempData = 0;
		tempData = (MSD_U32)((1 << 15) | (tcamOp << 12) | index);
		retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		/* read the updated entry index*/
		retVal = Fir_gmacsecRead(*dev, regAddr, &tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecRead function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}

		if ((tempData & (MSD_U32)0xff) == (MSD_U32)0xff)
		{
			MSD_U32 data1;
			retVal = Fir_gmacsecRead(*dev, regAddr + FIR_MSEC_TCAM_DA_5_4_OFFSET, &data1);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecRead function returned: %s.\n", msdDisplayStatus(retVal)));
				return retVal;
			}
			if (data1 == (MSD_U32)0xffff)
			{
				/* No higher valid TCAM entry */
				return MSD_NO_SUCH;
			}
			else
			{
				/* The highest valid TCAM entry found*/
			}
		}

		/* Get next entry and read the entry */
		retVal = MACSec_GetMEMData(*dev, memType, opData);   
		if (retVal != MSD_OK)
		{
			return retVal;
		}

	}
	case FIR_MACSec_READ_ENTRY:
	{

		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		tempData = 0;
		tempData = (MSD_U32)((1 << 15) | (tcamOp << 12) | index);
		retVal = Fir_gmacsecWrite(*dev, regAddr, tempData);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			return retVal;
		}
		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		retVal = MACSec_GetMEMData(*dev, memType, opData);   
		if (retVal != MSD_OK)
		{
			return retVal;
		}

		/* Wait until the tcam in ready. */
		retVal = Fir_WaitOperationReady(*dev, regAddr);
		if (retVal != MSD_OK)
		{
			return retVal;
		}
	}
	break;

	default:
		return MSD_FAIL;
	}

	return retVal;
}

static MSD_STATUS MACSec_SetMEMData
(
	IN MSD_QD_DEV dev,
	IN FIR_MEMTYPE memType,
	IN FIR_MACSec_DATA_HW* tcamDataP
)
{
	MSD_STATUS retVal = MSD_OK;    /* Functions return value */
	MSD_U8 i;
	MSD_U32 array[RX_SA_WIDTH + SAK_WIDTH] = { 0 };
	MSD_U32 regAddr = 0;
	MSD_32 _regAddr = 0;

	/* Set MEM Data register */
	switch (memType)
	{
	case FIR_TYPE_RULE:
	{
		msdMemCpy(array, ConvertRuleToArray(tcamDataP), TCAM_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_RULE, _regAddr);
		regAddr = (MSD_U32)_regAddr;

		/*Set TCAM data*/
		for (i = 1; i < TCAM_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
	}
	break;

	case FIR_TYPE_ACTION:
	{
		msdMemCpy(array, ConvertActionToArray(tcamDataP), TCAM_ACTION_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_ACTION, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*Action Data*/
		retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth, array[1]);
		if (retVal != MSD_OK)
		{
			MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
			break;
		}
	}
	break;

	case FIR_TYPE_RX_SECY:
	{
		msdMemCpy(array, ConvertRXSecYToArray(tcamDataP), RX_SECY_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_RX_SECY, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*RX SecY Data*/
		for (i = 1; i < RX_SECY_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
	}
	break;

	case FIR_TYPE_TX_SECY:
	{
		msdMemCpy(array, ConvertTXSecYToArray(tcamDataP), TX_SECY_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_TX_SECY, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*TX SecY Data*/
		for (i = 1; i < TX_SECY_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
	}
	break;

	case FIR_TYPE_RX_SC:
	{
		msdMemCpy(array, ConvertRXSCToArray(tcamDataP), RX_SC_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_RX_SC, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*RX SC Data*/
		for (i = 1; i < RX_SC_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}

	}
	break;

	case FIR_TYPE_TX_SC:
	{
		msdMemCpy(array, ConvertTXSCToArray(tcamDataP), TX_SC_WIDTH * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_TX_SC, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*TX SC Data*/
		for (i = 1; i < TX_SC_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}

	}
	break;

	case FIR_TYPE_RX_SA:
	{
		msdMemCpy(array, ConvertRXSAToArray(tcamDataP), (RX_SA_WIDTH + SAK_WIDTH) * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_RX_SA, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*RX SA Data*/
		for (i = 1; i < RX_SA_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
		regAddr = (MSD_U32)BASE_SAK;
		/*RX SAK Data*/
		for (i = RX_SA_WIDTH + 1; i < RX_SA_WIDTH + SAK_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * (i - RX_SA_WIDTH), array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
	}
	break;

	case FIR_TYPE_TX_SA:
	{

		msdMemCpy(array, ConvertTXSAToArray(tcamDataP), (TX_SA_WIDTH + SAK_WIDTH) * sizeof(MSD_U32));
		GET_MEM_ADDR(FIR_TYPE_TX_SA, _regAddr);
		regAddr = (MSD_U32)_regAddr;
		/*TX SA Data*/
		for (i = 1; i < TX_SA_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * i, array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
		regAddr = (MSD_U32)BASE_SAK;
		/*TX SAK Data*/
		for (i = TX_SA_WIDTH + 1; i < TX_SA_WIDTH + SAK_WIDTH; i++)
		{
			retVal = Fir_gmacsecWrite(dev, regAddr + regBusWidth * (i - TX_SA_WIDTH), array[i]);
			if (retVal != MSD_OK)
			{
				MSD_DBG_ERROR(("MACSecWrite function returned: %s.\n", msdDisplayStatus(retVal)));
				break;
			}
		}
	}
	break;

	default:
	{
		break;
	}
	}

	return retVal;

}

static MSD_STATUS MACSec_GetMEMData
(
	IN  MSD_QD_DEV dev,
	IN FIR_MEMTYPE memType,
	OUT FIR_MACSec_DATA_HW* tcamDataP
)
{
	MSD_STATUS retVal = MSD_OK;
	switch (memType)
	{
	case FIR_TYPE_RULE:
		msdMemCpy(&(tcamDataP->rule), ConvertArrayToRule(dev), sizeof(FIR_MSEC_RULE));
		break;
	case FIR_TYPE_ACTION:
		msdMemCpy(&(tcamDataP->action), ConvertArrayToAction(dev), sizeof(FIR_MSEC_TCAM_ACTION));
		break;
	case FIR_TYPE_RX_SECY:
		msdMemCpy(&(tcamDataP->rxSecy), ConvertArrayToRxSecY(dev), sizeof(FIR_SECY_INGRESS));
		break;
	case FIR_TYPE_TX_SECY:
		msdMemCpy(&(tcamDataP->txSecy), ConvertArrayToTxSecY(dev), sizeof(FIR_SECY_EGRESS));
		break;
	case FIR_TYPE_RX_SC:
		msdMemCpy(&(tcamDataP->rxSc), ConvertArrayToRxSc(dev), sizeof(FIR_MSEC_RX_SC));
		break;
	case FIR_TYPE_TX_SC:
		msdMemCpy(&(tcamDataP->txSc), ConvertArrayToTxSc(dev), sizeof(FIR_MSEC_TX_SC));
		break;
	case FIR_TYPE_RX_SA:
		msdMemCpy(&(tcamDataP->rxSa), ConvertArrayToRxSa(dev), sizeof(FIR_MSEC_RX_SA));
		break;
	case FIR_TYPE_TX_SA:
		msdMemCpy(&(tcamDataP->txSa), ConvertArrayToTxSa(dev), sizeof(FIR_MSEC_TX_SA));
		break;
	default:
		retVal = MSD_FAIL;
		break;
	}
	return retVal;
}
