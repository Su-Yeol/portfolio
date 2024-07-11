/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* BonsaiZ1_msdPTPIntf.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <bonsaiz1/include/api/BonsaiZ1_msdPTP.h>

MSD_STATUS BonsaiZ1_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN* interruptEn
)
{
    BONSAIZ1_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in BonsaiZ1_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return BonsaiZ1_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS BonsaiZ1_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in BonsaiZ1_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = BonsaiZ1_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
    {
        return retVal;
    }
        
    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS BonsaiZ1_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in BonsaiZ1_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = BonsaiZ1_gptpGetTimeStamp(dev, port, (BONSAIZ1_PTP_TIME)timeToRead, &tempStatus);
    if (retVal != MSD_OK)
    {
        return retVal;
    }   

    ptpStatus->isValid = tempStatus.isValid;
    ptpStatus->ptpSeqId = tempStatus.ptpSeqId;
    ptpStatus->status = tempStatus.status;
    ptpStatus->timeStamped = tempStatus.timeStamped;

    return retVal;
}

MSD_STATUS BonsaiZ1_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in BonsaiZ1_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = BonsaiZ1_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
    {
        return retVal;
    } 

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}

MSD_STATUS BonsaiZ1_gptpTODStoreCompenIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    BONSAIZ1_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in BonsaiZ1_gptpTODStoreCompenIntf is NULL. \n"));
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
        
    return BonsaiZ1_gptpTODStoreCompensation(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS BonsaiZ1_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    BONSAIZ1_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in BonsaiZ1_gptpTODStoreAllIntf is NULL. \n"));
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

    return BonsaiZ1_gptpTODStoreAll(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS BonsaiZ1_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in BonsaiZ1_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = BonsaiZ1_gptpTODCaptureAll(dev, timeArrayIndex, &tmpArray);
    if (retVal != MSD_OK)
    {
        return retVal;
    }   

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

MSD_STATUS BonsaiZ1_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    BONSAIZ1_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in BonsaiZ1_gptpSetPulseIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpData.ptp1ppsPhase = pulsePara->ptp1ppsPhase;
    tmpData.ptpAltTCAMTimeSel = pulsePara->ptpAltTCAMTimeSel;
    tmpData.ptpTCAMTimeSel = pulsePara->ptpTCAMTimeSel;
    tmpData.ptp1ppsSelect = pulsePara->ptp1ppsSelect;
    tmpData.ptp1ppsWidthRange = pulsePara->ptp1ppsWidthRange;
    tmpData.ptpPulseWidth = pulsePara->ptpPulseWidth;

    return BonsaiZ1_gptpSetPulse(dev, &tmpData);
}

MSD_STATUS BonsaiZ1_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in msdPtpPulseGetIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = BonsaiZ1_gptpGetPulse(dev, &tmpData);

    pulsePara->ptp1ppsPhase = tmpData.ptp1ppsPhase;
    pulsePara->ptp1ppsSelect = tmpData.ptp1ppsSelect;
    pulsePara->ptp1ppsWidthRange = tmpData.ptp1ppsWidthRange;
    pulsePara->ptpPulseWidth = tmpData.ptpPulseWidth;
    pulsePara->ptpAltTCAMTimeSel = tmpData.ptpAltTCAMTimeSel;
    pulsePara->ptpTCAMTimeSel = tmpData.ptpTCAMTimeSel;

    return retVal;
}

MSD_STATUS BonsaiZ1_gptp1588v2ConfigSetIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
)
{
    MSD_STATUS retVal;
    MSD_PTP_MODE bonsaiz1PtpMode = (MSD_PTP_MODE)ptpMode;
    MSD_PTP_STEP bonsaiz1PtpStep = (MSD_PTP_STEP)ptpStep;
    BONSAIZ1_PTP_ATTEMPT(retVal, BonsaiZ1_gptp1588v2ConfigSet(dev, portNum, timeArrayIndex, bonsaiz1PtpMode, bonsaiz1PtpStep));
    return MSD_OK;
}

MSD_STATUS BonsaiZ1_gptp8021asConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    BONSAIZ1_PTP_RELAY_MODE bonsaiz1RelayMode = (BONSAIZ1_PTP_RELAY_MODE)relayMode;
    BONSAIZ1_PTP_ATTEMPT(retVal, BonsaiZ1_gptp8021asConfigSet(dev, portNum, timeArrayIndex, bonsaiz1RelayMode));
    return MSD_OK;
}

MSD_STATUS BonsaiZ1_gptp8021asRevCfgSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
     MSD_STATUS retVal;
    BONSAIZ1_PTP_RELAY_MODE bonsaiz1RelayMode = (BONSAIZ1_PTP_RELAY_MODE)relayMode;
    BONSAIZ1_PTP_ATTEMPT(retVal, BonsaiZ1_gptp8021asRevConfigSet(dev, portNum, timeArrayIndex, bonsaiz1RelayMode));
    return MSD_OK;
}