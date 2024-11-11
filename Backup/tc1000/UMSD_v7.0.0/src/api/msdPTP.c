/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdPTP.c
*
* DESCRIPTION:
*       API definitions for PTP 
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <api/msdPTP.h>
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

/*******************************************************************************
* msdPtpIntEnableSet
*
* DESCRIPTION:
*       This routine enables or disables port PTP interrupt.
*
* INPUTS:
*       portNum - The logical port number
*       interruptEn    - Port PTP interrupt Enable/Disable
*                        ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
*                        ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntEnableSet
(
    IN MSD_U8    devNum,
    IN MSD_LPORT portNum,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetIntEnable != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetIntEnable(dev, portNum, interruptEn);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpIntEnableGet
*
* DESCRIPTION:
*       This routine gets port PTP interrupt enable/disable.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*       interruptEn    - Port PTP interrupt Enable/Disable
*                        ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
*                        ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntEnableGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetIntEnable != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetIntEnable(dev, portNum, interruptEn);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPortPTPEnSet
*
* DESCRIPTION:
*       This routine enables or disables PTP on a port.
*
* INPUTS:
*       portNum - The logical port number
*       en   - MSD_TRUE to enable PTP, MSD_FALSE to disable PTP
*
* OUTPUTS:
*       None
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortPTPEnSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_BOOL  en
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetProtPTPEn(dev, portNum, en);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPortPTPEnGet
*
* DESCRIPTION:
*       This routine gets PTP enable status on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*       en   - MSD_TRUE to enable PTP, MSD_FALSE to disable PTP
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortPTPEnGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_BOOL  *en
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetProtPTPEn(dev, portNum, en);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPortConfigSet
*
* DESCRIPTION:
*       This routine sets PTP port configuration.
*
* INPUTS:
*       portNum    - The logical port number
*       ptpData - PTP port configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortConfigSet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetPortConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetPortConfig(dev, portNum, ptpData);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPortConfigGet
*
* DESCRIPTION:
*       This routine gets PTP port configuration.
*
* INPUTS:
*       portNum    - The logical port number
*
* OUTPUTS:
*       ptpData - PTP port configuration parameters.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpPortConfigGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_PTP_PORT_CONFIG *ptpData
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetPortConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetPortConfig(dev, portNum, ptpData);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTimeStampGet
*
* DESCRIPTION:
*        This routine retrieves the PTP port status that includes time stamp value
*        and sequce Id that are captured by PTP logic for a PTP frame that needs
*        to be time stamped.
*
* INPUTS:
*       portNum - logical port number.
*       timeToRead    - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type);
*
* OUTPUTS:
*        ptpStatus    - PTP port status
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTimeStampGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetTimeStamp != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetTimeStamp(dev, portNum, timeToRead, ptpStatus);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTSValidStGet
*
* DESCRIPTION:
*        This routine gets PTP Time valid bit.
*
* INPUTS:
*       portNum       - logical port number.
*       timeToRead - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type)
*
* OUTPUTS:
*    isValid        - MSD_TRUE: valid TS, MSD_FALSE: invalid TS.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTSValidStGet
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetTSValidSt != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetTSValidSt(dev, portNum, timeToRead, isValid);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTimeStampReset
*
* DESCRIPTION:
*        This routine resets PTP Time valid bit so that PTP logic can time stamp
*        a next PTP frame that needs to be time stamped.
*
* INPUTS:
*       portNum - logical port number.
*       timeToReset    - Arr0, Arr1, or Dep time (MSD_PTP_TIME enum type);
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpTimeStampReset
(
    IN  MSD_U8    devNum,
    IN  MSD_LPORT    portNum,
    IN  MSD_PTP_TIME    timeToReset
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpResetTimeStamp != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpResetTimeStamp(dev, portNum, timeToReset);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpGlobalConfigSet
*
* DESCRIPTION:
*       This routine sets port PTP global configuration.
*
* INPUTS:
*       ptpData  - PTP global configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalConfigSet
(
    IN  MSD_U8    devNum,
    IN  MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetGlobalConfig(dev, ptpData);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpGlobalConfigGet
*
* DESCRIPTION:
*       This routine gets port PTP global configuration.
*
* INPUTS:
*
* OUTPUTS:
*       ptpData  - PTP global configuration parameters.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalConfigGet
(
    IN  MSD_U8    devNum,
    OUT MSD_PTP_GLOBAL_CONFIG    *ptpData
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetGlobalConfig(dev, ptpData);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpIntStatusGet
*
* DESCRIPTION:
*       This routine gets PTP interrupt status for a port.
*
* INPUTS:
*
* OUTPUTS:
*       ptpIntSt   - interrupt status for
*                       ptpTrigGenInt;      Trigger generate mode Interrupt
*                       ptpEventInt;        Event Capture Interrupt
*                       ptpUpperPortInt;    Upper Ports Interrupt
*                       ptpIntStPortVect
*                         interrupt status for each port (bit 0 for port 0, bit 1 for port 1, etc.)
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetIntStatus != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetIntStatus(dev, ptpIntSt);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpMeanPathDelaySet
*
* DESCRIPTION:
*        This routine sets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*        delay    - cable delay between this port and its link partner in unsiged
*                  nano seconds
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelaySet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    delay
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetMeanPathDelay(dev, portNum, delay);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpMeanPathDelayGet
*
* DESCRIPTION:
*        This routine gets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*        delay    - cable delay between this port and its link partner in unsiged
*                  nano seconds
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelayGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *delay
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetMeanPathDelay(dev, portNum, delay);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpIgrPathDelayAsymSet
*
* DESCRIPTION:
*        This routine sets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*        asym    - bit 15 is path delay asymmetry sign
*                        0 - the path delay asymmetry number is added
*                        1 - the path delay asymmetry number is subtracted
*
*                  bit 14:0 indicates the asymmetry value beyond the mean path delay
*                  that needs to be added or subtracted for more accurate cable
*                  measurements.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetIgrPathDelayAsym(dev, portNum, asym);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpIgrPathDelayAsymGet
*
* DESCRIPTION:
*        This routine gets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*        asym    - bit 15 is path delay asymmetry sign
*                        0 - the path delay asymmetry number is added
*                        1 - the path delay asymmetry number is subtracted
*
*                  bit 14:0 indicates the asymmetry value beyond the mean path delay
*                  that needs to be added or subtracted for more accurate cable
*                  measurements.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetIgrPathDelayAsym(dev, portNum, asym);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpEgrPathDelayAsymSet
*
* DESCRIPTION:
*        This routine sets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*        asym    - bit 15 is path delay asymmetry sign
*                        0 - the path delay asymmetry number is added
*                        1 - the path delay asymmetry number is subtracted
*
*                  bit 14:0 indicates the asymmetry value beyond the mean path delay
*                  that needs to be added or subtracted for more accurate cable
*                  measurements.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetEgrPathDelayAsym(dev, portNum, asym);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpEgrPathDelayAsymGet
*
* DESCRIPTION:
*        This routine gets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*        asym    - bit 15 is path delay asymmetry sign
*                        0 - the path delay asymmetry number is added
*                        1 - the path delay asymmetry number is subtracted
*
*                  bit 14:0 indicates the asymmetry value beyond the mean path delay
*                  that needs to be added or subtracted for more accurate cable
*                  measurements.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetEgrPathDelayAsym(dev, portNum, asym);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }
    return retVal;
}

/*******************************************************************************
* msdPtpModeSet
*
* DESCRIPTION:
*        Set PTP mode configuration.
*
* INPUTS:
*        mode -
*          altScheme  - Alternate Scheme
*          grandMstr  - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetMode != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetMode(dev, mode);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpModeGet
*
* DESCRIPTION:
*        Get PTP mode configuration.
*
* INPUTS:
*
* OUTPUTS:
*        mode -
*          altScheme  - Alternate Scheme
*          grandMstr  - Grand Master Enable.
*          oneStep    - OneStep Enable.
*          ptpMode    - PTP Mode as follows:
*                   00 = Boundary Clock
*                   01 = Peer to Peer Transparent Clock
*                   10 = End to End Transparent Clock
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MODE_CONFIG *mode
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetMode != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetMode(dev, mode);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpMasterSlaveSet
*
* DESCRIPTION:
*        Set PTP as Master or Slave.
*
* INPUTS:
*        role - PTP_ROLE_SLAVE:Set device as slave   PTP_ROLE_MASTER:Set device as master
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MASTER_SLAVE role
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetMasterSlave != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetMasterSlave(dev, role);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpMasterSlaveGet
*
* DESCRIPTION:
*        Get PTP role, Master or Slave.
*
* INPUTS:
*
* OUTPUTS:
*        role - PTP_ROLE_SLAVE:Set device as slave   PTP_ROLE_MASTER:Set device as master
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MASTER_SLAVE *role
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetMasterSlave != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetMasterSlave(dev, role);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTODBusyBitStatusGet
*
* DESCRIPTION:
*        Check the busy bit to see if the hardware is busy. Make sure the busy status is False befor any other ToD operation
*
* INPUTS:
*
* OUTPUTS:
*        busyStatus - MSD_TRUE: Hardware is busy, should wait. MSD_FALSE: busy bit has been self clear, can do other operation
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODBusyBitStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL    *busyStatus
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetTODBusyBitStatus(dev, busyStatus);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTODStoreCompensation
*
* DESCRIPTION:
*        Store Compensation register only to selected TimeArray
*
* INPUTS:
*        timeArrayIndex - Time array index
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BUSY - TOD busy bit not cleared
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODStoreCompensation
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpTODStoreCompensation(dev, timeArrayIndex, timeArray);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTODStoreAll
*
* DESCRIPTION:
*        Store all register to selected TimeArray
*
* INPUTS:
*        timeArrayIndex - Time array index
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* OUTPUTS:
*        None.
*
* RETURNS:
*        MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BUSY - TOD busy bit not cleared
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODStoreAll
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpTODStoreAll != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpTODStoreAll(dev, timeArrayIndex, timeArray);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpTODCaptureAll
*
* DESCRIPTION:
*        Capture selected TimeArray.
*
* INPUTS:
*        timeArrayIndex - Time array index
*
* OUTPUTS:
*        timeArray -    TOD load pointer
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
* RETURNS:
*        MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BUSY - TOD busy bit not cleared
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODCaptureAll
(
    IN  MSD_U8  devNum,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpTODCaptureAll != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpTODCaptureAll(dev, timeArrayIndex, timeArray);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPulseSet
*
* DESCRIPTION:
*        Set PTP Pulse parameters.
*
* INPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptpAltTCAMTimeSel;
*             ptp1ppsWidthRange;
*             ptpTCAMTimeSel
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetPulse != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetPulse(dev, pulsePara);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdPtpPulseGet
*
* DESCRIPTION:
*        Get PTP Pulse parameters.
*
* INPUTS:
*
* OUTPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptpAltTCAMTimeSel;
*             ptp1ppsWidthRange;
*             ptpTCAMTimeSel;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetPulse != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetPulse(dev, pulsePara);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdGetPTPGlobalTime
*
* DESCRIPTION:
*       This routine gets the global timer value that is running off of the free
*        running clock.
*
* INPUTS:
*
* OUTPUTS:
*        ptpTime    - PTP global time
*
* RETURNS:
*       MSD_OK - On success
*        MSD_FAIL - On error
*        MSD_BAD_PARAM - If invalid parameter is given
*        MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdGetPTPGlobalTime
(
    IN  MSD_U8  devNum,
    OUT MSD_U32 *ptpTime
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gtaiGetPTPGlobalTime(dev, ptpTime);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPortRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetPortReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetPortReg(dev, portNum, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPortRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetPortReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetPortReg(dev, portNum, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpGlobalRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetGlobalReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetGlobalReg(dev, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpGlobalRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetGlobalReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetGlobalReg(dev, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdTaiRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetTaiReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetTaiReg(dev, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdTaiRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetTaiReg != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetTaiReg(dev, regOffset, data);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtp1588v2ConfigSet
(
    IN  MSD_U8        devNum,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSet1588v2Config != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSet1588v2Config(dev, portNum, timeArrayIndex, ptpMode, ptpStep);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtp8021asConfigSet
(
    IN  MSD_U8              devNum,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSet8021asConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSet8021asConfig(dev, portNum, timeArrayIndex, relayMode);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtp8021asRevConfigSet
(
    IN  MSD_U8              devNum,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSet8021asRevConfig(dev, portNum, timeArrayIndex, relayMode);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPreemptTimestampSelSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL   tsSelect
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetPreemptTsSel(dev, portNum, tsSelect);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPreemptTimestampSelGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    OUT  MSD_BOOL  *tsSelect
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetPreemptTsSel(dev, portNum, tsSelect);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPreemptCtrlSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     preemptCtrl
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpSetPreemptCtrl(dev, portNum, preemptCtrl);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

MSD_STATUS msdPtpPreemptCtrlGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    OUT  MSD_U8    *preemptCtrl
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gptpGetPreemptCtrl(dev, portNum, preemptCtrl);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdTaiTrigGenTimerSelSet
*
* DESCRIPTION:
*       This routine is used to set TAI <PTP Trigger Generation Timer Select>
* INPUTS:
*    devNum - device number
*    trigGenTimerSel - PTP Trigger Generation Timer Select.
*        0 : Trigger Generation clock is based on PTP Global Timer
*        1 : Trigger Generation clock is based on PTP ToD Timer
*        2 : Trigger Generation clock is based on PTP 1722 Timer
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTaiTrigGenTimerSelSet
(
IN  MSD_U8     devNum,
IN  MSD_U8     trigGenTimerSel
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gtaiSetTrigGenTimerSel(dev, trigGenTimerSel);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}

/*******************************************************************************
* msdTaiTrigGenTimerSelGet
*
* DESCRIPTION:
*       This routine is used to get TAI <PTP Trigger Generation Timer Select>
* INPUTS:
*    devNum - device number
*
* OUTPUTS:
*    trigGenTimerSel - PTP Trigger Generation Timer Select.
*        0 : Trigger Generation clock is based on PTP Global Timer
*        1 : Trigger Generation clock is based on PTP ToD Timer
*        2 : Trigger Generation clock is based on PTP 1722 Timer
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdTaiTrigGenTimerSelGet
(
IN  MSD_U8     devNum,
OUT  MSD_U8    *trigGenTimerSel
)
{
    MSD_STATUS retVal;
    MSD_QD_DEV* dev = sohoDevGet(devNum);
    if (NULL == dev)
    {
        MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
        retVal = MSD_FAIL;
    }
    else
    {
        if (dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel != NULL)
        {
            retVal = dev->SwitchDevObj.PTPObj.gtaiGetTrigGenTimerSel(dev, trigGenTimerSel);
        }
        else
        {
            retVal = MSD_NOT_SUPPORTED;
        }
    }

    return retVal;
}