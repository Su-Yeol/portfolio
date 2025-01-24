/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/******************************************************************************
* Bonsai_msdPTPIntf.c
*
* DESCRIPTION:
*       API definitions for Precise Time Protocol logic
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/
#include <bonsai/include/api/Bonsai_msdPTP.h>

MSD_STATUS Bonsai_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN* interruptEn
)
{
    BONSAI_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("Input param PTP_PORT_INTERRUPT_EN in Bonsai_gptpSetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tempIntEn.ptpArrIntEn = interruptEn->ptpArrIntEn;
    tempIntEn.ptpDepIntEn = interruptEn->ptpDepIntEn;

    return Bonsai_gptpSetIntEnable(dev, port, &tempIntEn);
}

MSD_STATUS Bonsai_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_PORT_INTERRUPT_EN tempIntEn;

    if (interruptEn == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PORT_INTERRUPT_EN in Bonsai_gptpGetIntEnableIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Bonsai_gptpGetIntEnable(dev, port, &tempIntEn);
    if (MSD_OK != retVal)
    {
        return retVal;
    }
        
    interruptEn->ptpArrIntEn = tempIntEn.ptpArrIntEn;
    interruptEn->ptpDepIntEn = tempIntEn.ptpDepIntEn;

    return retVal;
}

MSD_STATUS Bonsai_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_TS_STATUS tempStatus;

    if (ptpStatus == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TS_STATUS in Bonsai_gptpGetTimeStampIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Bonsai_gptpGetTimeStamp(dev, port, (BONSAI_PTP_TIME)timeToRead, &tempStatus);
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

MSD_STATUS Bonsai_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_INTERRUPT_STATUS tempIntSt;

    if (ptpIntSt == NULL)
    {
        MSD_DBG_ERROR(("param PTP_INTERRUPT_STATUS in Bonsai_gptpGetIntStatusIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Bonsai_gptpGetIntStatus(dev, &tempIntSt);
    if (retVal != MSD_OK)
    {
        return retVal;
    } 

    ptpIntSt->ptpEventInt = tempIntSt.ptpEventInt;
    ptpIntSt->ptpIntStPortVect = tempIntSt.ptpIntStPortVect;
    ptpIntSt->ptpTrigGenInt = tempIntSt.ptpTrigGenInt;

    return retVal;
}

MSD_STATUS Bonsai_gptpTODStoreCompenIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    BONSAI_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Bonsai_gptpTODStoreCompenIntf is NULL. \n"));
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
        
    return Bonsai_gptpTODStoreCompensation(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Bonsai_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    BONSAI_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Bonsai_gptpTODStoreAllIntf is NULL. \n"));
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

    return Bonsai_gptpTODStoreAll(dev, timeArrayIndex, &tmpArray);
}

MSD_STATUS Bonsai_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_TIME_ARRAY tmpArray;

    if (timeArray == NULL)
    {
        MSD_DBG_ERROR(("param PTP_TIME_ARRAY in Bonsai_gptpTODStoreAllIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Bonsai_gptpTODCaptureAll(dev, timeArrayIndex, &tmpArray);
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

MSD_STATUS Bonsai_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    BONSAI_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in Bonsai_gptpSetPulseIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    tmpData.ptp1ppsPhase = pulsePara->ptp1ppsPhase;
    tmpData.ptpAltTCAMTimeSel = pulsePara->ptpAltTCAMTimeSel;
    tmpData.ptpTCAMTimeSel = pulsePara->ptpTCAMTimeSel;
    tmpData.ptp1ppsSelect = pulsePara->ptp1ppsSelect;
    tmpData.ptp1ppsWidthRange = pulsePara->ptp1ppsWidthRange;
    tmpData.ptpPulseWidth = pulsePara->ptpPulseWidth;

    return Bonsai_gptpSetPulse(dev, &tmpData);
}

MSD_STATUS Bonsai_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_PULSE_STRUCT tmpData;

    if (pulsePara == NULL)
    {
        MSD_DBG_ERROR(("param PTP_PULSE_STRUCT in msdPtpPulseGetIntf is NULL. \n"));
        return MSD_BAD_PARAM;
    }

    retVal = Bonsai_gptpGetPulse(dev, &tmpData);

    pulsePara->ptp1ppsPhase = tmpData.ptp1ppsPhase;
    pulsePara->ptp1ppsSelect = tmpData.ptp1ppsSelect;
    pulsePara->ptp1ppsWidthRange = tmpData.ptp1ppsWidthRange;
    pulsePara->ptpPulseWidth = tmpData.ptpPulseWidth;
    pulsePara->ptpAltTCAMTimeSel = tmpData.ptpAltTCAMTimeSel;
    pulsePara->ptpTCAMTimeSel = tmpData.ptpTCAMTimeSel;

    return retVal;
}

MSD_STATUS Bonsai_gptp1588v2ConfigSetIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
)
{
    MSD_STATUS retVal;
    MSD_PTP_MODE bonsaiPtpMode = (MSD_PTP_MODE)ptpMode;
    MSD_PTP_STEP bonsaiPtpStep = (MSD_PTP_STEP)ptpStep;
    BONSAI_PTP_ATTEMPT(retVal, Bonsai_gptp1588v2ConfigSet(dev, portNum, timeArrayIndex, bonsaiPtpMode, bonsaiPtpStep));
    return MSD_OK;
}

MSD_STATUS Bonsai_gptp8021asConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    BONSAI_PTP_RELAY_MODE bonsaiRelayMode = (BONSAI_PTP_RELAY_MODE)relayMode;
    BONSAI_PTP_ATTEMPT(retVal, Bonsai_gptp8021asConfigSet(dev, portNum, timeArrayIndex, bonsaiRelayMode));
    return MSD_OK;
}

MSD_STATUS Bonsai_gptp8021asRevCfgSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
     MSD_STATUS retVal;
    BONSAI_PTP_RELAY_MODE bonsaiRelayMode = (BONSAI_PTP_RELAY_MODE)relayMode;
    BONSAI_PTP_ATTEMPT(retVal, Bonsai_gptp8021asRevConfigSet(dev, portNum, timeArrayIndex, bonsaiRelayMode));
    return MSD_OK;
}