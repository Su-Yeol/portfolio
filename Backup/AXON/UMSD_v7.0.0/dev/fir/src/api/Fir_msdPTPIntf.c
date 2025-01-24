/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Fir_msdPTPIntf.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <fir/include/api/Fir_msdPTP.h>

MSD_STATUS Fir_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN* interruptEn
)
{
    FIR_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Fir_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return Fir_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Fir_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    FIR_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Fir_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Fir_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
    {
        return retVal;
    }
        
    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS Fir_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    FIR_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in Fir_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Fir_gptpGetTimeStamp(dev, port, (FIR_PTP_TIME)timeToRead, &tempStatus);
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

MSD_STATUS Fir_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    FIR_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Fir_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Fir_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
    {
        return retVal;
    } 

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}

MSD_STATUS Fir_gptpTODStoreCompenIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    FIR_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Fir_gptpTODStoreCompenIntf is NULL. \n"));
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
        
    return Fir_gptpTODStoreCompensation(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Fir_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    FIR_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Fir_gptpTODStoreAllIntf is NULL. \n"));
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

    return Fir_gptpTODStoreAll(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Fir_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    FIR_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Fir_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Fir_gptpTODCaptureAll(dev, timeArrayIndex, &tmpArray);
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

MSD_STATUS Fir_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    FIR_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in Fir_gptpSetPulseIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpData.ptp1ppsPhase = pulsePara->ptp1ppsPhase;
    tmpData.ptpAltTCAMTimeSel = pulsePara->ptpAltTCAMTimeSel;
    tmpData.ptpTCAMTimeSel = pulsePara->ptpTCAMTimeSel;
    tmpData.ptp1ppsSelect = pulsePara->ptp1ppsSelect;
    tmpData.ptp1ppsWidthRange = pulsePara->ptp1ppsWidthRange;
    tmpData.ptpPulseWidth = pulsePara->ptpPulseWidth;

    return Fir_gptpSetPulse(dev, &tmpData);
}

MSD_STATUS Fir_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    FIR_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in msdPtpPulseGetIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Fir_gptpGetPulse(dev, &tmpData);

    pulsePara->ptp1ppsPhase = tmpData.ptp1ppsPhase;
    pulsePara->ptp1ppsSelect = tmpData.ptp1ppsSelect;
    pulsePara->ptp1ppsWidthRange = tmpData.ptp1ppsWidthRange;
    pulsePara->ptpPulseWidth = tmpData.ptpPulseWidth;
    pulsePara->ptpAltTCAMTimeSel = tmpData.ptpAltTCAMTimeSel;
    pulsePara->ptpTCAMTimeSel = tmpData.ptpTCAMTimeSel;

    return retVal;
}

MSD_STATUS Fir_gptp1588v2ConfigSetIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
)
{
    MSD_STATUS retVal;
    MSD_PTP_MODE firPtpMode = (MSD_PTP_MODE)ptpMode;
    MSD_PTP_STEP firPtpStep = (MSD_PTP_STEP)ptpStep;
    FIR_PTP_ATTEMPT(retVal, Fir_gptp1588v2ConfigSet(dev, portNum, timeArrayIndex, firPtpMode, firPtpStep));
    return MSD_OK;
}

MSD_STATUS Fir_gptp8021asConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    FIR_PTP_RELAY_MODE firRelayMode = (FIR_PTP_RELAY_MODE)relayMode;
    FIR_PTP_ATTEMPT(retVal, Fir_gptp8021asConfigSet(dev, portNum, timeArrayIndex, firRelayMode));
    return MSD_OK;
}

MSD_STATUS Fir_gptp8021asRevConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
     MSD_STATUS retVal;
    FIR_PTP_RELAY_MODE firRelayMode = (FIR_PTP_RELAY_MODE)relayMode;
    FIR_PTP_ATTEMPT(retVal, Fir_gptp8021asRevConfigSet(dev, portNum, timeArrayIndex, firRelayMode));
    return MSD_OK;
}