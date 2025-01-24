/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* msdPTP.h
*
* DESCRIPTION:
*       API definitions for PTP feature.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/
#ifndef msdPTP_h
#define msdPTP_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

/*******************************************************************************
* msdPtpIntStatusGet
*
* DESCRIPTION:
*       This routine gets PTP interrupt status.
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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdPtpIntStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
);

/*******************************************************************************
* msdPtpMeanPathDelaySet
*
* DESCRIPTION:
*		This routine sets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelaySet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	delay
);

/*******************************************************************************
* msdPtpMeanPathDelayGet
*
* DESCRIPTION:
*		This routine gets ingress mean path delay on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		delay	- cable delay between this port and its link partner in unsiged
*				  nano seconds
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpMeanPathDelayGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*delay
);

/*******************************************************************************
* msdPtpIgrPathDelayAsymSet
*
* DESCRIPTION:
*		This routine sets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
);

/*******************************************************************************
* msdPtpIgrPathDelayAsymGet
*
* DESCRIPTION:
*		This routine gets ingress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpIgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
);

/*******************************************************************************
* msdPtpEgrPathDelayAsymSet
*
* DESCRIPTION:
*		This routine sets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* OUTPUTS:
*		None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymSet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    IN  MSD_U16	asym
);

/*******************************************************************************
* msdPtpEgrPathDelayAsymGet
*
* DESCRIPTION:
*		This routine gets egress path delay asymmetry on a port.
*
* INPUTS:
*       portNum - The logical port number
*
* OUTPUTS:
*		asym	- bit 15 is path delay asymmetry sign
*						0 - the path delay asymmetry number is added
*						1 - the path delay asymmetry number is subtracted
*
*				  bit 14:0 indicates the asymmetry value beyond the mean path delay
*				  that needs to be added or subtracted for more accurate cable
*				  measurements.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdPtpEgrPathDelayAsymGet
(
    IN  MSD_U8  devNum,
    IN  MSD_LPORT portNum,
    OUT MSD_U16	*asym
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MODE_CONFIG *mode
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpModeGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MODE_CONFIG *mode
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_MASTER_SLAVE role
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpMasterSlaveGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_MASTER_SLAVE *role
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpTODBusyBitStatusGet
(
    IN  MSD_U8  devNum,
    OUT MSD_BOOL	*busyStatus
);

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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
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
);

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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
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
);

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
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
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
);

/*******************************************************************************
* msdPtpPulseSet
*
* DESCRIPTION:
*        Set PTP Pulse parameters.
*
* INPUTS:
*        pulsePara -  following parameters:
*             ptpPulseWidth;
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseSet
(
    IN  MSD_U8  devNum,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
);

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
*             ptp1ppsWidthRange;
*             ptp1ppsPhase;
*             ptp1ppsSelect;
*
* RETURNS:
*       MSD_OK - On success
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS msdPtpPulseGet
(
    IN  MSD_U8  devNum,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
);

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
*		MSD_FAIL - On error
*		MSD_BAD_PARAM - If invalid parameter is given
*		MSD_NOT_SUPPORTED - Device not support
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS msdGetPTPGlobalTime
(
    IN  MSD_U8  devNum,
    OUT MSD_U32 *ptpTime
);

/*******************************************************************************
* msdPtpPortRegGet
*
* DESCRIPTION:
*       This routine is used to read value from a designated PTP port register
*
* INPUTS:
*       devNum - device number
*       portNum - The logical port number
*       regOffset - PTP port register address offset
*
* OUTPUTS:
*       data - data read from PTP register
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*    None
*
*******************************************************************************/
MSD_STATUS msdPtpPortRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

/*******************************************************************************
* msdPtpPortRegSet
*
* DESCRIPTION:
*       This routine is used to write value to a designated PTP port register
*
* INPUTS:
*       devNum - device number
*       portNum - The logical port number
*       regOffset - PTP port register address offset
*       data - data written to PTP register
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
*    None
*
*******************************************************************************/
MSD_STATUS msdPtpPortRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

/*******************************************************************************
* msdPtpGlobalRegGet
*
* DESCRIPTION:
*       This routine is used to read value from a designated PTP global register
*
* INPUTS:
*       devNum - device number
*       regOffset - PTP global register address offset
*
* OUTPUTS:
*       data - data read from PTP register
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*    None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

/*******************************************************************************
* msdPtpGlobalRegSet
*
* DESCRIPTION:
*       This routine is used to write value to a designated PTP global register
*
* INPUTS:
*       devNum - device number
*       regOffset - PTP global register address offset
*       data - data written to PTP register
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
*    None
*
*******************************************************************************/
MSD_STATUS msdPtpGlobalRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

/*******************************************************************************
* msdTaiRegGet
*
* DESCRIPTION:
*       This routine is used to read value from a designated TAI register
*
* INPUTS:
*       devNum - device number
*       regOffset - TAI register address offset
*
* OUTPUTS:
*       data - data read from PTP register
*
* RETURNS:
*       MSD_OK  - on success
*       MSD_FAIL  - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not support
*
* COMMENTS:
*    None
*
*******************************************************************************/
MSD_STATUS msdTaiRegGet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

/*******************************************************************************
* msdTaiRegSet
*
* DESCRIPTION:
*       This routine is used to write value to a designated TAI register
*
* INPUTS:
*       devNum - device number
*       regOffset - TAI register address offset
*       data - data written to PTP register
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
*    None
*
*******************************************************************************/
MSD_STATUS msdTaiRegSet
(
    IN  MSD_U8      devNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

/*******************************************************************************
* msdPtp1588v2ConfigSet
*
* DESCRIPTION:
*       This routine is used to configure PTP 1588 V2
*
* INPUTS:
*       devNum - device number
*       portNum - The logical port number
*       timeArrayIndex - time array num, 0 or 1
*       ptpMode - PTP Mode. BC, P2P TC or E2E TC
*       ptpStep - PTP Step. one-step or two-step
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
*    In this function we always do below configuration:
*        1. Set <DisPTP> to 0, enable PTP logic
*        2. Set <TransSpec> to 0
*        3. PTP Hardware Acceleration enable
*
*******************************************************************************/
MSD_STATUS msdPtp1588v2ConfigSet
(
    IN  MSD_U8        devNum,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
);

/*******************************************************************************
* msdPtp8021asConfigSet
*
* DESCRIPTION:
*       This routine is used to configure PTP 802.1AS
*
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*    timeArrayIndex - time array num, 0 or 1
*    relayMode - RelayMode. 0 means end-station, 1 means relay
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
*    In this function we always do below configuration:
*        1. Set <DisPTP> to 0, enable PTP logic
*        2. Set <TransSpec> to 1
*        3. Set <PTPMode> to 0, i.e. Boundary Clock(BC)
*        4. Set <OneStepSync> to 0
*        5. Set <OneStepEgress> & <OneStepIngress> to 0
*        6. PTP Hardware Acceleration enable
*
*******************************************************************************/
MSD_STATUS msdPtp8021asConfigSet
(
    IN  MSD_U8              devNum,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
);

/*******************************************************************************
* msdPtp8021asRevConfigSet
*
* DESCRIPTION:
*       This routine is used to configure PTP 802.1AS-Rev.
*
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*    timeArrayIndex - time array num, 0 or 1
*    relayMode - RelayMode. 0 means end-station, 1 means relay
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
*    In this function we always do below configuration:
*       1. Set <DisPTP> to 0, enable PTP logic
*       2. Set <TransSpec> to 1
*       3. Set <PTPMode> to 0, i.e. Boundary Clock(BC)
*       4. Set <OneStepEgress> & <OneStepIngress> to 1
*       5. Set <OneStepSync> to 1
*       6. PTP Hardware Acceleration enable
*
*******************************************************************************/
MSD_STATUS msdPtp8021asRevConfigSet
(
    IN  MSD_U8              devNum,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
);

/*******************************************************************************
* msdPtpPreemptTimestampSelSet
*
* DESCRIPTION:
*       This routine is used to select Preemption Timestamp(PTP port reg0, bit9)
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*    tsSelect - Preemption Timestamp select. 0 means select express frame timestamp group. 1 means select preemption frame timestamp group
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
MSD_STATUS msdPtpPreemptTimestampSelSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_BOOL   tsSelect
);

/*******************************************************************************
* msdPtpPreemptTimestampSelGet
*
* DESCRIPTION:
*       This routine is used to get Preemption Timestamp(PTP port reg0, bit9)
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*
* OUTPUTS:
*    tsSelect - Preemption Timestamp select. 0 means select express frame timestamp group. 1 means select preemption frame timestamp group
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
MSD_STATUS msdPtpPreemptTimestampSelGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    OUT  MSD_BOOL  *tsSelect
);

/*******************************************************************************
* msdPtpPreemptCtrlSet
*
* DESCRIPTION:
*       This routine is used to set PTP Preemption Control
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*    preemptCtrl - PTP Preemption Control. 0, 1 or 2.
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
MSD_STATUS msdPtpPreemptCtrlSet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    IN  MSD_U8     preemptCtrl
);

/*******************************************************************************
* msdPtpPreemptCtrlGet
*
* DESCRIPTION:
*       This routine is used to get PTP Preemption Control
* INPUTS:
*    devNum - device number
*    portNum - The logical port number
*
* OUTPUTS:
*    preemptCtrl - PTP Preemption Control. 0, 1 or 2.
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
MSD_STATUS msdPtpPreemptCtrlGet
(
    IN  MSD_U8     devNum,
    IN  MSD_LPORT  portNum,
    OUT  MSD_U8    *preemptCtrl
);

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
);

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
);


#ifdef __cplusplus
}
#endif

#endif /* msdPTP_h */
