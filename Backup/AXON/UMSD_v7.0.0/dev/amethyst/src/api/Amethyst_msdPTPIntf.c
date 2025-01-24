/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Amethyst_msdPTPIntf.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <amethyst/include/api/Amethyst_msdPTP.h>

MSD_STATUS Amethyst_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    AMETHYST_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Amethyst_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return Amethyst_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Amethyst_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Amethyst_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
        return retVal;

    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS Amethyst_gptpSetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
    AMETHYST_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_CONFIG in Amethyst_gptpSetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.arrLedCtrl = ptpData->arrLedCtrl;
    tempData.arrTSMode = ptpData->arrTSMode;
    tempData.depLedCtrl = ptpData->depLedCtrl;
    tempData.disTSOverwrite = ptpData->disTSOverwrite;
    tempData.disTSpec = ptpData->disTSpec;
    tempData.etJump = ptpData->etJump;
    tempData.filterAct = ptpData->filterAct;
    tempData.hwAccPtp = ptpData->hwAccPtp;
    tempData.ExthwAcc = ptpData->ExthwAcc;
    tempData.ipJump = ptpData->ipJump;
    tempData.kpFramwSa = ptpData->kpFramwSa;
    tempData.ptpArrIntEn = ptpData->ptpArrIntEn;
    tempData.ptpDepIntEn = ptpData->ptpDepIntEn;
    tempData.transSpec = ptpData->transSpec;

    return Amethyst_gptpSetPortConfig(dev, port, &tempData);
}

MSD_STATUS Amethyst_gptpGetPortConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_PORT_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_CONFIG in Amethyst_gptpGetPortConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetPortConfig(dev, port, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->arrLedCtrl = tempData.arrLedCtrl;
    ptpData->arrTSMode = tempData.arrTSMode;
    ptpData->depLedCtrl = tempData.depLedCtrl;
    ptpData->disTSOverwrite = tempData.disTSOverwrite;
    ptpData->disTSpec = tempData.disTSpec;
    ptpData->etJump = tempData.etJump;
    ptpData->filterAct = tempData.filterAct;
    ptpData->hwAccPtp = tempData.hwAccPtp;
    ptpData->ExthwAcc = tempData.ExthwAcc;
    ptpData->ipJump = tempData.ipJump;
    ptpData->kpFramwSa = tempData.kpFramwSa;
    ptpData->ptpArrIntEn = tempData.ptpArrIntEn;
    ptpData->ptpDepIntEn = tempData.ptpDepIntEn;
    ptpData->transSpec = tempData.transSpec;

    return retVal;
}

MSD_STATUS Amethyst_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in Amethyst_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetTimeStamp(dev, port, (AMETHYST_PTP_TIME)timeToRead, &tempStatus);
    if (retVal != MSD_OK)
        return retVal;

    ptpStatus->isValid = tempStatus.isValid;
    ptpStatus->ptpSeqId = tempStatus.ptpSeqId;
    ptpStatus->status = tempStatus.status;
    ptpStatus->timeStamped = tempStatus.timeStamped;

    return retVal;
}

MSD_STATUS Amethyst_gptpSetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    AMETHYST_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Amethyst_gptpSetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempData.msgIdTSEn = ptpData->msgIdTSEn;
    tempData.ptpEType = ptpData->ptpEType;
    tempData.tsArrPtr = ptpData->tsArrPtr;

    return Amethyst_gptpSetGlobalConfig(dev, &tempData);
}

MSD_STATUS Amethyst_gptpGetGlobalConfigIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_GLOBAL_CONFIG tempData;

    if (ptpData == NULL)
    {
        MSD_DBG_ERROR(("param PTP_GLOBAL_CONFIG in Amethyst_gptpGetGlobalConfigIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetGlobalConfig(dev, &tempData);
    if (retVal != MSD_OK)
        return retVal;

    ptpData->msgIdTSEn = tempData.msgIdTSEn;
    ptpData->ptpEType = tempData.ptpEType;
    ptpData->tsArrPtr = tempData.tsArrPtr;

    return retVal;
}

MSD_STATUS Amethyst_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Amethyst_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
        return retVal;

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}

MSD_STATUS Amethyst_gptpSetModeIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_MODE_CONFIG tempMode;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("param PTP_MODE_CONFIG in Amethyst_gptpSetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempMode.altScheme = mode->altScheme;
    tempMode.grandMstr = mode->grandMstr;
    tempMode.oneStep = mode->oneStep;
    tempMode.PTPMode = mode->PTPMode;

    retVal = Amethyst_gptpSetMode(dev, &tempMode);

    return retVal;
}

MSD_STATUS Amethyst_gptpGetModeIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_MODE_CONFIG tempMode;

    if (mode == NULL)
    {
        MSD_DBG_ERROR(("param PTP_MODE_CONFIG in Amethyst_gptpGetModeIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetMode(dev, &tempMode);
    if (retVal != MSD_OK)
        return retVal;

    mode->altScheme = tempMode.altScheme;
    mode->grandMstr = tempMode.grandMstr;
    mode->oneStep = tempMode.oneStep;
    mode->PTPMode = tempMode.PTPMode;

    return retVal;
}

MSD_STATUS Amethyst_gptpSetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE role
)
{
    return Amethyst_gptpSetMasterSlave(dev, (AMETHYST_PTP_MASTER_SLAVE)role);
}

MSD_STATUS Amethyst_gptpGetMasterSlaveIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_MASTER_SLAVE *role
)
{
    return Amethyst_gptpGetMasterSlave(dev, (AMETHYST_PTP_MASTER_SLAVE*)role);
}

MSD_STATUS Amethyst_gptpTODStoreCompensationIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    AMETHYST_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Amethyst_gptpTODStoreCompensationIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpArray.clkValid = timeArray->clkValid;
    tmpArray.domainNumber = timeArray->domainNumber;
    tmpArray.Nanoseconds1722High = timeArray->Nanoseconds1722High;
    tmpArray.Nanoseconds1722Low = timeArray->Nanoseconds1722Low;
    tmpArray.todCompensation = timeArray->todCompensation;
    tmpArray.todLoadPoint = timeArray->todLoadPoint;
    tmpArray.todNanoseconds = timeArray->todNanoseconds;
    tmpArray.todSecondsHigh = timeArray->todSecondsHigh;
    tmpArray.todSecondsLow = timeArray->todSecondsLow;
        
    return Amethyst_gptpTODStoreCompensation(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Amethyst_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    AMETHYST_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Amethyst_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpArray.clkValid = timeArray->clkValid;
    tmpArray.domainNumber = timeArray->domainNumber;
    tmpArray.Nanoseconds1722High = timeArray->Nanoseconds1722High;
    tmpArray.Nanoseconds1722Low = timeArray->Nanoseconds1722Low;
    tmpArray.todCompensation = timeArray->todCompensation;
    tmpArray.todLoadPoint = timeArray->todLoadPoint;
    tmpArray.todNanoseconds = timeArray->todNanoseconds;
    tmpArray.todSecondsHigh = timeArray->todSecondsHigh;
    tmpArray.todSecondsLow = timeArray->todSecondsLow;

    return Amethyst_gptpTODStoreAll(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Amethyst_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Amethyst_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpTODCaptureAll(dev, timeArrayIndex, &tmpArray);
    if (retVal != MSD_OK)
        return retVal;

    timeArray->clkValid = tmpArray.clkValid;
    timeArray->domainNumber = tmpArray.domainNumber;
    timeArray->Nanoseconds1722High = tmpArray.Nanoseconds1722High;
    timeArray->Nanoseconds1722Low = tmpArray.Nanoseconds1722Low;
    timeArray->todCompensation = tmpArray.todCompensation;
    timeArray->todLoadPoint = tmpArray.todLoadPoint;
    timeArray->todNanoseconds = tmpArray.todNanoseconds;
    timeArray->todSecondsHigh = tmpArray.todSecondsHigh;
    timeArray->todSecondsLow = tmpArray.todSecondsLow;

    return retVal;
}

MSD_STATUS Amethyst_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    AMETHYST_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in Amethyst_gptpSetPulseIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpData.ptp1ppsPhase = pulsePara->ptp1ppsPhase;
	tmpData.ptpAltTCAMTimeSel = pulsePara->ptpAltTCAMTimeSel;
	tmpData.ptpTCAMTimeSel = pulsePara->ptpTCAMTimeSel;
    tmpData.ptp1ppsSelect = pulsePara->ptp1ppsSelect;
    tmpData.ptp1ppsWidthRange = pulsePara->ptp1ppsWidthRange;
    tmpData.ptpPulseWidth = pulsePara->ptpPulseWidth;

    return Amethyst_gptpSetPulse(dev, &tmpData);
}

MSD_STATUS Amethyst_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    AMETHYST_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in msdPtpPulseGetIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Amethyst_gptpGetPulse(dev, &tmpData);

    pulsePara->ptp1ppsPhase = tmpData.ptp1ppsPhase;
    pulsePara->ptp1ppsSelect = tmpData.ptp1ppsSelect;
    pulsePara->ptp1ppsWidthRange = tmpData.ptp1ppsWidthRange;
    pulsePara->ptpPulseWidth = tmpData.ptpPulseWidth;
	pulsePara->ptpAltTCAMTimeSel = tmpData.ptpAltTCAMTimeSel;
	pulsePara->ptpTCAMTimeSel = tmpData.ptpTCAMTimeSel;

    return retVal;
}