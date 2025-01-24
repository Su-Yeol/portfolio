/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/******************************************************************************
* Fir_msdPTP.h
*
* DESCRIPTION:
*       API definitions for PTP
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef Fir_msdPTP_h
#define Fir_msdPTP_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef USE_SINGLE_READ
#define MAX_PTP_CONSECUTIVE_READ    4
#define FIRMSD_PTP_BUILD_TIME(_time1, _time2)       (((_time1) << 16) | (_time2))

#define FIR_PTP_ATTEMPT(status, xFuncToTry)                                                  \
        {                                                                                    \
            (status) = (xFuncToTry);                                                         \
            if(status != MSD_OK)                                                             \
            {                                                                                \
                return status;                                                               \
            }                                                                                \
        }

/*
*  typedef: struct FIR_PTP_PORT_INTERRUPT_EN
*
*  Description: Port PTP interrupt Enable/Disable
*
*  Fields:
*      ptpArrIntEn      - Port Arrival Interrupt enable     MSD_FALSE:Disable MSD_TRUE:Enable
*      ptpDepIntEn      - Port Departure Interrupt enable   MSD_FALSE:Disable MSD_TRUE:Enable
*/
typedef struct
{
    MSD_BOOL        ptpArrIntEn;
    MSD_BOOL        ptpDepIntEn;
}FIR_PTP_PORT_INTERRUPT_EN;

/*
* Typedef: enum FIR_PTP_TIME
*
* Description: Defines the PTP Time to be read
*
* Fields:
*      PTP_ARR0_TIME            - PTP Arrival Time 0
*      PTP_ARR1_TIME            - PTP Arrival Time 1
*      PTP_DEP_TIME            - PTP Departure Time
*/
typedef enum
{
    FIR_PTP_ARR0_TIME = 0x0,
    FIR_PTP_ARR1_TIME = 0x1,
    FIR_PTP_DEP_TIME = 0x2,          /* PTP Departure 0 Time */
    FIR_PTP_DEP0_TIME = 0x2,         /* PTP Departure 0 Time, same as above */
    FIR_PTP_DEP1_TIME = 0x3
} FIR_PTP_TIME;

/*
* Typedef: enum FIR_PTP_INT_STATUS
*
* Description: Defines the PTP Port interrupt status for time stamp
*
* Fields:
*      PTP_INT_NORMAL        - No error condition occurred
*      PTP_INT_OVERWRITE     - PTP logic has to process more than one PTP frame
*                                  that needs time stamping before the current read.
*                                Only the latest one is saved.
*      PTP_INT_DROP          - PTP logic has to process more than one PTP frame
*                                  that needs time stamping before the current read.
*                                Only the oldest one is saved.
*
*/
typedef enum
{
    FIR_PTP_INT_NORMAL = 0x0,
    FIR_PTP_INT_OVERWRITE = 0x1,
    FIR_PTP_INT_DROP = 0x2
} FIR_PTP_INT_STATUS;

/*
*  typedef: struct FIR_PTP_TS_STATUS
*
*  Description: PTP port status of time stamp
*
*  Fields:
*      isValid        - time stamp is valid
*      status        - time stamp error status
*      timeStamped    - time stamp value of a PTP frame that needs to be time stamped
*      ptpSeqId    - sequence ID of the frame whose time stamp information has been captured
*/
typedef struct
{
    MSD_BOOL    isValid;
    MSD_U32    timeStamped;
    MSD_U32    ptpSeqId;
    MSD_PTP_INT_STATUS    status;
} FIR_PTP_TS_STATUS;

/*
*  typedef: struct FIR_PTP_INTERRUPT_STATUS
*
*  Description: PTP global interrupt status structure after Opus
*
*  Fields:
*     ptpTrigGenInt;                Trigger generate mode Interrupt
*     ptpEventInt;                  Event Capture Interrupt
*     ptpIntStPortVect;             Precise Time Protocol Interrupt for Ports.
*/
typedef struct
{
    MSD_U8     ptpTrigGenInt;
    MSD_U8     ptpEventInt;
    MSD_U32    ptpIntStPortVect;
} FIR_PTP_INTERRUPT_STATUS;

/*
*  typedef: struct FIR_PTP_TIME_ARRAY
*
*  Description: PTP Time Array consists of:
*                       10 byte ToD time,
*                       8 byte 1722 time,
*                       4 byte Compensation,
*                       1 byte Domain Number,
*                       And a 1 bit Clock Valid.
*
*  Fields:
*     todLoadPoint:        Time of Day Load Point
*     clkValid:            Clock Valid bit
*     domainNumber:        Domain Number, to be associated with the selected Time Array
*     todNanoseconds:      Time Array Time of Day, Nano second 32 bits
*     todSecondsLow:       Time Array Time of Day, second portion 0-31 bits
*     todSecondsHigh:      Time Array Time of Day, second portion 32-47 bits
*     Nanoseconds1722Low:  Time Array 1722 Time of Day in Nano seconds 0-31 bits
*     Nanoseconds1722High: Time Array 1722 Time of Day in Nano seconds 32-63 bits
*     todCompensation:     Time Array Time of Day Compensation 32 bits
*/
typedef struct
{
    MSD_U32    todLoadPoint;
    MSD_BOOL   clkValid;
    MSD_U8     domainNumber;
    MSD_U32    todNanoseconds;
    MSD_U32    todSecondsLow;
    MSD_U16    todSecondsHigh;
    MSD_U32    Nanoseconds1722Low;
    MSD_U32    Nanoseconds1722High;
    MSD_U32    todCompensation;
}FIR_PTP_TIME_ARRAY;

/*
* Typedef: enum FIR_PTP_TOD_STORE_OPERATION
*
* Description: Defines the PTP time array Store Operation type
*
* Fields:
*      PTP_TOD_STORE_COMP           - Store Comp register only to selected TimeArray
*      PTP_TOD_STORE_ALL            - Store All Registers to selected TimeArray
*/
typedef enum
{
    FIR_PTP_TOD_STORE_COMP = 0x2,
    FIR_PTP_TOD_STORE_ALL = 0x3,
} FIR_PTP_TOD_STORE_OPERATION;

/*
*  typedef: struct FIR_PTP_PULSE_STRUCT
*
*  Description: PTP 1pps Pulse parameters
*
*  Fields:
*     ptpPulseWidth; Pulse Width for the 1 Pulse Per Second on the Second signal
*     ptp1ppsWidthRange; Pulse Width Range for the 1 Pulse Per Second on the Second signal
*     ptp1ppsPhase; Phase of the 1 Pulse Per Second on the Second signal
*     ptp1ppsSelect; Select for the 1 Pulse Per Second on the Second signal.
*/
typedef struct
{
    MSD_U8     ptpPulseWidth;
    MSD_U8     ptpAltTCAMTimeSel;
    MSD_U8     ptp1ppsWidthRange;
    MSD_U8     ptpTCAMTimeSel;
    MSD_U8     ptp1ppsPhase;
    MSD_U8     ptp1ppsSelect;
} FIR_PTP_PULSE_STRUCT;

typedef enum
{
    FIR_PTP_BC     = 0,    /* Boundary Clock */
    FIR_PTP_P2P_TC = 1,    /* Peer to Peer Transparent Clock */
    FIR_PTP_E2E_TC = 2     /* End to End Transparent Clock */
} FIR_PTP_MODE;

typedef enum
{
    FIR_PTP_TWO_STEP = 0,
    FIR_PTP_ONE_STEP = 1
} FIR_PTP_STEP;

typedef enum
{
    FIR_PTP_END_STATION = 0,    /* end-station mode for 802.1AS */
    FIR_PTP_RELAY       = 1     /* relay mode for 802.1AS  */
} FIR_PTP_RELAY_MODE;
/****************************************************************************/
/* Exported PTP Types                                                         */
/****************************************************************************/


MSD_STATUS Fir_gptpSetIntEnable
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN FIR_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Fir_gptpSetIntEnableIntf
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT port,
    IN MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Fir_gptpGetIntEnable
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT FIR_PTP_PORT_INTERRUPT_EN *interruptEn
);
MSD_STATUS Fir_gptpGetIntEnableIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_PTP_PORT_INTERRUPT_EN *interruptEn
);

MSD_STATUS Fir_gptpSetPortPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    IN  MSD_BOOL  en
);
MSD_STATUS Fir_gptpGetPortPTPEn
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT port,
    OUT MSD_BOOL  *en
);

MSD_STATUS Fir_gptpGetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  FIR_PTP_TIME    timeToRead,
    OUT FIR_PTP_TS_STATUS    *ptpStatus
);
MSD_STATUS Fir_gptpGetTimeStampIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_PTP_TS_STATUS    *ptpStatus
);

MSD_STATUS Fir_gptpGetTSValidSt
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToRead,
    OUT MSD_BOOL    *isValid
);

MSD_STATUS Fir_gptpResetTimeStamp
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT    port,
    IN  MSD_PTP_TIME    timeToReset
);

MSD_STATUS Fir_gptpGetIntStatus
(
    IN  MSD_QD_DEV* dev,
    OUT FIR_PTP_INTERRUPT_STATUS *ptpIntSt
);
MSD_STATUS Fir_gptpGetIntStatusIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_INTERRUPT_STATUS *ptpIntSt
);

MSD_STATUS Fir_gptpSetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    delay
);

MSD_STATUS Fir_gptpGetMeanPathDelay
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *delay
);

MSD_STATUS Fir_gptpSetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
);

MSD_STATUS Fir_gptpGetIgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
);

MSD_STATUS Fir_gptpSetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    IN  MSD_U16    asym
);

MSD_STATUS Fir_gptpGetEgrPathDelayAsym
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT portNum,
    OUT MSD_U16    *asym
);

MSD_STATUS Fir_gptpGetTODBusyBitStatus
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_BOOL    *busyStatus
);

MSD_STATUS Fir_gptpTODStoreCompensation
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  FIR_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Fir_gptpTODStoreCompenIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Fir_gptpTODStoreAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  FIR_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Fir_gptpTODStoreAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    IN  MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Fir_gptpTODCaptureAll
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT FIR_PTP_TIME_ARRAY *timeArray
);
MSD_STATUS Fir_gptpTODCaptureAllIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_U8 timeArrayIndex,
    OUT MSD_PTP_TIME_ARRAY *timeArray
);

MSD_STATUS Fir_gptpSetPulse
(
    IN  MSD_QD_DEV* dev,
    IN  FIR_PTP_PULSE_STRUCT *pulsePara
);
MSD_STATUS Fir_gptpSetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_PTP_PULSE_STRUCT *pulsePara
);

MSD_STATUS Fir_gptpGetPulse
(
    IN  MSD_QD_DEV* dev,
    OUT FIR_PTP_PULSE_STRUCT *pulsePara
);
MSD_STATUS Fir_gptpGetPulseIntf
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_PTP_PULSE_STRUCT *pulsePara
);

MSD_STATUS Fir_gtaiGetPTPGlobalTime
(
    IN  MSD_QD_DEV* dev,
    OUT MSD_U32 *ptpTime
);

MSD_STATUS Fir_gptpPortRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

MSD_STATUS Fir_gptpPortRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

MSD_STATUS Fir_gptpGlobalRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

MSD_STATUS Fir_gptpGlobalRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

MSD_STATUS Fir_gTaiRegGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    OUT  MSD_U16    *data
);

MSD_STATUS Fir_gTaiRegSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U32     regOffset,
    IN  MSD_U16     data
);

MSD_STATUS Fir_gptp1588v2ConfigSet
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  FIR_PTP_MODE  ptpMode,
    IN  FIR_PTP_STEP  ptpStep
);

MSD_STATUS Fir_gptp1588v2ConfigSetIntf
(
    IN  MSD_QD_DEV    *dev,
    IN  MSD_LPORT     portNum,
    IN  MSD_U8        timeArrayIndex,
    IN  MSD_PTP_MODE  ptpMode,
    IN  MSD_PTP_STEP  ptpStep
);

MSD_STATUS Fir_gptp8021asConfigSet
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  FIR_PTP_RELAY_MODE  relayMode
);

MSD_STATUS Fir_gptp8021asConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
);

MSD_STATUS Fir_gptp8021asRevConfigSet
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  FIR_PTP_RELAY_MODE  relayMode
);

MSD_STATUS Fir_gptp8021asRevConfigSetIntf
(
    IN  MSD_QD_DEV          *dev,
    IN  MSD_LPORT           portNum,
    IN  MSD_U8              timeArrayIndex,
    IN  MSD_PTP_RELAY_MODE  relayMode
);

MSD_STATUS Fir_gptpPreemptTimestampSelSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_BOOL    tsSelect
);

MSD_STATUS Fir_gptpPreemptTimestampSelGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    OUT  MSD_BOOL   *tsSelect
);

MSD_STATUS Fir_gptpPreemptCtrlSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    IN  MSD_U8      preemptCtrl
);

MSD_STATUS Fir_gptpPreemptCtrlGet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_LPORT   portNum,
    OUT  MSD_U8     *preemptCtrl
);

MSD_STATUS Fir_gtaiTrigGenTimerSelGet
(
    IN  MSD_QD_DEV  *dev,
    OUT  MSD_U8     *trigGenTimerSel
);

MSD_STATUS Fir_gtaiTrigGenTimerSelSet
(
    IN  MSD_QD_DEV  *dev,
    IN  MSD_U8      trigGenTimerSel
);

#ifdef __cplusplus
}
#endif

#endif /* __Fir_msdPTP_h */
