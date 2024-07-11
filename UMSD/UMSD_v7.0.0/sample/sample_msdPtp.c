/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdPtp.c
*
* DESCRIPTION:
*    This sample will demonstrate how to
*    1. Enable Port PTP.
*    2. Enable time stamp capture for IEEE 802.1as or IEEE 1588 PTP frame
*    3. Capture the arrival time and departure time.
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"
#include "time.h"
#include "stdint.h"

typedef struct {
	unsigned long long seconds;
	MSD_U32 nanoseconds;
}TimeInfo;

/**
* @brief  Subtracts a nanosecond value from the timestamp
* @param  ts [inout] Timestamp value
* @param  ns Nanoseconds value to subtract from ts
* @return void
*/
#ifndef NS_PER_SECOND
#define NS_PER_SECOND 1000000000		/*!< Amount of nanoseconds in a second*/
#endif // !NS_PER_SECOND

#ifndef LS_SEC_MAX
#define LS_SEC_MAX 0xFFFFFFFFull		/*!< Maximum value of seconds LSB field */
#endif // !LS_SEC_MAX

#ifndef PPM_MAX
#define PPM_MAX   1000000      		    /*!< Maximum value of one PPM */
#endif // !PPM_MAX

/*Suppose the ToD time is loaded at loadPoint time = Current Global time + TOD_LOAD_DELAY ns*/
//#define TOD_LOAD_DELAY (2 * 1000000000)
#define TOD_LOAD_DELAY (50000000)

/*Suppose the device PTP module using the internal 250MHz clock for amethyst, so the clock period is 4 ns*/
#ifdef PTP_CLOCK_125M
#define TOD_COMP_PER_ONE_PPM 0x431E
#define TAI_CLOCK_PERIOD 8
#else
#define TOD_COMP_PER_ONE_PPM 0x218E
#define TAI_CLOCK_PERIOD 4
#endif

/********************************************************************************
* Sample #1:
*       enable port0 and port1 PTP feature and support the IEEE 802.1as frame time
*       capture both for arrival time and departure time
*
* Description:
*
* Note: Oak/Spruce/Fir/BonsaiZ1 does not support
*
********************************************************************************/
MSD_STATUS sample_msdPtp(MSD_U8 devNum)
{
	MSD_STATUS status;
	MSD_PTP_GLOBAL_CONFIG ptpGlobalCfg;
	MSD_PTP_PORT_CONFIG ptpPortCfg;
	MSD_PTP_TS_STATUS ptpStatus;
	MSD_PTP_PORT_INTERRUPT_EN ptpInt;
	MSD_LPORT port;
	MSD_U8 data;
	MSD_QD_DEV* dev;
	MSD_SWITCH_NAME tempDevName;

	dev = sohoDevGet(devNum);

	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	tempDevName = dev->devName;
	if ((tempDevName == MSD_OAK) || (tempDevName == MSD_SPRUCE) || (tempDevName == MSD_FIR) || (tempDevName == MSD_BONSAIZ1) || (tempDevName == MSD_BONSAI))
	{

		MSG_PRINT(("This sample do not support Oak, Spruce, Fir, BonsaiZ1 and Bonsai\n"));
		return MSD_NOT_SUPPORTED;
	}

	msdMemSet(&ptpGlobalCfg, 0, sizeof(MSD_PTP_GLOBAL_CONFIG));
	msdMemSet(&ptpPortCfg, 0, sizeof(MSD_PTP_PORT_CONFIG));
	ptpInt.ptpArrIntEn = MSD_TRUE;
	ptpInt.ptpDepIntEn = MSD_TRUE;

	/*Program PTPEType as 0x88F7, enable all PTP message type time stamp capture and using port Arr0 time register to capture*/
	ptpGlobalCfg.ptpEType = 0x88F7;
	ptpGlobalCfg.msgIdTSEn = 0xFFFF;
	ptpGlobalCfg.tsArrPtr = 0;
	if ((status = msdPtpGlobalConfigSet(devNum, &ptpGlobalCfg)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpGlobalConfigSet returned fail.\n"));
		return status;
	}

	for (port = 0; port < 2; port++)
	{
		/*Enable the PTP for port 0 and port 1*/
		if ((status = msdPtpPortPTPEnSet(devNum, port, MSD_TRUE)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortPTPEnSet returned fail.\n"));
			return status;
		}

		/*Enable the PTP interrupt for port 0 and port 1*/
		if ((status = msdPtpIntEnableSet(devNum, port, &ptpInt)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpIntEnableSet returned fail.\n"));
			return status;
		}

		/*Configure device to support 802.1as */
		if ((status = msdPtpPortConfigGet(devNum, port, &ptpPortCfg)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortConfigGet returned fail.\n"));
			return status;
		}
		ptpPortCfg.transSpec = 1;
		if ((status = msdPtpPortConfigSet(devNum, port, &ptpPortCfg)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpPortConfigSet returned fail.\n"));
			return status;
		}
	}

	/*Set the port1 as the PTP destination Port for Peridot*/
	if (dev->devName == MSD_PERIDOT)
	{
		if ((status = msdSysMonitorMgmtCtrlGet(devNum, 0x32, &data)) != MSD_OK)
		{
			MSG_PRINT(("msdSysMonitorMgmtCtrlGet returned fail.\n"));
			return status;
		}
		data = (data & 0xe0) | 0x1;
		if ((status = msdSysMonitorMgmtCtrlSet(devNum, 0x32, data)) != MSD_OK)
		{
			MSG_PRINT(("msdSysMonitorMgmtCtrlSet returned fail.\n"));
			return status;
		}
	}

	/*Set the port0's PTP destination Port to port1 for Amethyst*/
	if (dev->devName == MSD_AMETHYST)
	{
		if ((status = msdPolicyMgmtCtrlGet(devNum, 0, 0x3F, &data)) != MSD_OK)
		{
			MSG_PRINT(("msdPolicyMgmtCtrlGet returned fail.\n"));
			return status;
		}
		data = (data & 0xe0) | 0x1;
		if ((status = msdPolicyMgmtCtrlSet(devNum, 0, 0x3F, data)) != MSD_OK)
		{
			MSG_PRINT(("msdPolicyMgmtCtrlSet returned fail.\n"));
			return status;
		}
	}

	/**************************************************************************************
	*   After this setup, if one PTP frame entered to port 0 and been forward to port 1   *
	*   the arrival time stamp be captured by the PTP logic will be indicated in port 0   *
	*   PTPArr0Time and the departure time stamp will indicated in port 1 PTPDepTime      *
	**************************************************************************************/

	/*Get port0 Arr0 ptp status*/
	if ((status = msdPtpTimeStampGet(devNum, 0, MSD_PTP_ARR0_TIME, &ptpStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpTimeStampGet returned fail.\n"));
		return status;
	}
	MSG_PRINT(("PTPArr0Time Valid: %d\n", ptpStatus.isValid));
	MSG_PRINT(("PTPArr0Time SeqId: %d\n", ptpStatus.ptpSeqId));
	MSG_PRINT(("PTPArr0Time Stamp: 0x%x\n", ptpStatus.timeStamped));

	/*Get port1 Departure ptp status*/
	if ((status = msdPtpTimeStampGet(devNum, 1, MSD_PTP_DEP_TIME, &ptpStatus)) != MSD_OK)
	{
		MSG_PRINT(("msdPtpTimeStampGet returned fail.\n"));
		return status;
	}
	MSG_PRINT(("PTPDepTime Valid: %d\n", ptpStatus.isValid));
	MSG_PRINT(("PTPDepTime SeqId: %d\n", ptpStatus.ptpSeqId));
	MSG_PRINT(("PTPDepTime Stamp: 0x%x\n", ptpStatus.timeStamped));

	MSG_PRINT(("sample_msdPtp success.\n"));
	return MSD_OK;
}

/**************************************************************************************
*                                                                                     *
*              Following Sample only supported by Peridot/Amethyst                    *
*                                                                                     *
**************************************************************************************/

static void subNS(MSD_PTP_TIME_ARRAY *ts, uint64_t ns) {
	uint64_t secs = (uint64_t)ts->todSecondsLow | (((uint64_t)ts->todSecondsHigh) << 32);
	uint64_t nanos = (uint64_t)ts->todNanoseconds;

	secs -= ns / NS_PER_SECOND;
	ns = ns % NS_PER_SECOND;

	if (ns > nanos)
	{  //borrow
		nanos += NS_PER_SECOND;
		--secs;
	}

	nanos -= ns;

	ts->todSecondsHigh = (uint16_t)(secs >> 32);
	ts->todSecondsLow = (uint32_t)(secs & LS_SEC_MAX);
	ts->todNanoseconds = (uint32_t)nanos;
}
static void addNS(MSD_PTP_TIME_ARRAY *ts, uint64_t ns) {
	uint64_t secs = (uint64_t)ts->todSecondsLow | (((uint64_t)ts->todSecondsHigh) << 32);
	uint64_t nanos = (uint64_t)ts->todNanoseconds;

	secs += ns / NS_PER_SECOND;
	nanos += ns % NS_PER_SECOND;

	if (nanos > NS_PER_SECOND)
	{  //carry
		nanos -= NS_PER_SECOND;
		++secs;
	}

	ts->todSecondsHigh = (uint16_t)(secs >> 32);
	ts->todSecondsLow = (uint32_t)(secs & LS_SEC_MAX);
	ts->todNanoseconds = (uint32_t)nanos;
}

/********************************************************************************
* setCurrentTime
*
*	This sample API will demonstrate how to load one time array to device:
*	Suppose the divice PTP module using the internal 125MHz clock, so the Global Time is incremented of 8ns
*	The time array load occurs at the instant the PTP Global Time matched the load point time
*
*******************************************************************************/
static MSD_STATUS setCurrentTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, MSD_U8 domainNumber, TimeInfo *timeinfo)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;
	TimeInfo temptime;

	temptime.nanoseconds = (timeinfo->nanoseconds + TOD_LOAD_DELAY) % 1000000000;
	temptime.seconds = timeinfo->seconds + (timeinfo->nanoseconds + TOD_LOAD_DELAY) / 1000000000;

	/*Initialise the time array*/
	timeArray.clkValid = MSD_TRUE;
	timeArray.domainNumber = domainNumber;
	timeArray.todSecondsHigh = (temptime.seconds >> 32) & 0xffff;
	timeArray.todSecondsLow = temptime.seconds & 0xffffffff;
	timeArray.todNanoseconds = temptime.nanoseconds;
	timeArray.Nanoseconds1722High = 0;
	timeArray.Nanoseconds1722Low = 0;
	timeArray.todCompensation = 0;
	/*Get the current Global time and add a delay for loadPoint, the value is incremented onece by 8n, so the loadPoint value = TOD_LOAD_DELAY / TAI_CLOCK_PERIOD*/
	msdGetPTPGlobalTime(devNum, &timeArray.todLoadPoint);
	timeArray.todLoadPoint = timeArray.todLoadPoint + TOD_LOAD_DELAY / TAI_CLOCK_PERIOD;

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	return status;
}
/********************************************************************************
* getCurrentTime
*
*	This sample API will demonstrate how to get time array form device:
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*
*******************************************************************************/
static MSD_STATUS getCurrentTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, TimeInfo *timeinfo)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODCaptureAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	timeinfo->nanoseconds = timeArray.todNanoseconds;
	timeinfo->seconds = timeArray.todSecondsHigh;
	timeinfo->seconds = (timeinfo->seconds << 32) | timeArray.todSecondsLow;

	return status;
}
/********************************************************************************
* adjustTime
*
*	This sample API will demonstrate how to adjust device time of day based on master's time:
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*	timeoffset - nanoseconds based
*	direction - 0 = Local time is slow than master time
*				1 = Local time is fast than master time
*
*******************************************************************************/
static MSD_STATUS adjustTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, int64_t timeoffset)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	MSD_BOOL busyStatus;

	unsigned long long lp;
	long double delta;

	timeoffset += TAI_CLOCK_PERIOD;

	///////*Adjust the ToD based on the delta*//////
	/*Get current ToD firstly*/
	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			printf("msdPtpTODBusyBitStatusGet, error code: %d\n", status);
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODCaptureAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		printf("adjustTime msdPtpTODCaptureAll error, error code: %d\n", status);
		return status;
	}

	lp = timeArray.todLoadPoint + TOD_LOAD_DELAY / TAI_CLOCK_PERIOD;
	timeArray.todLoadPoint = lp & 0xFFFFFFFF;
	lp = timeArray.todCompensation;
	if (lp > 0x80000000)
	{

		delta = (long double)(timeoffset - ((TOD_LOAD_DELAY / PPM_MAX) * ((long double)(lp & 0x7FFFFFF) * 1.0 / TOD_COMP_PER_ONE_PPM)));
	}
	else
	{
		delta = (long double)(timeoffset + ((TOD_LOAD_DELAY / PPM_MAX) * ((long double)(lp & 0x7FFFFFF) * 1.0 / TOD_COMP_PER_ONE_PPM)));
	}

	if (delta >= 0)
	{
		lp = delta;
		addNS(&timeArray, lp);
		addNS(&timeArray, TOD_LOAD_DELAY);
	}
	else
	{
		lp = -delta;
		subNS(&timeArray, lp);
		addNS(&timeArray, TOD_LOAD_DELAY);
	}

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			printf("msdPtpTODBusyBitStatusGet, error code: %d\n", status);
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		printf("Load time array error, error code: %d\n", status);
		return status;
	}

	return status;

}
/********************************************************************************
* adjustFrequency
*
*	This sample API will demonstrate how to adjust PPM difference between the local crystal clocking this PTP block and the PTP Grand Master device that this Time Array is tracking
*	Suppose the divice PTP module using the internal 125MHz clock
*	The compensation register is incremented of 465.661 zeptoseconds(4.65661E-19 seconds). A difference of 1 PPM for a 125 MHz local PTP clock is 8 femtoseconds(8.0E-15 seconds)
*	And the value of compensation register should be 0x431E
*	direction - 0 = Local clock is slow than master clock
*				1 = Local clock is fast than master clock
*
*******************************************************************************/
static MSD_STATUS adjustFrequency(MSD_U8 devNum, MSD_U8 timeArrayIndex, long double ppm)
{
	MSD_STATUS status = MSD_OK;
	MSD_BOOL busyStatus;
	MSD_PTP_TIME_ARRAY timeArray;
	long double temp;
	static MSD_U32 preCompensation = 0;

	timeArray.clkValid = MSD_TRUE;
	if (ppm == 0)
	{
		return MSD_OK;
	}
	else if (ppm > 0)
	{
		// DUT is slower than master
		temp = ((ppm * TOD_COMP_PER_ONE_PPM) >= 0x80000000 ? 0x7fffffff : (ppm * TOD_COMP_PER_ONE_PPM));
		timeArray.todCompensation = (MSD_U32)(temp);
	}
	else
	{
		// DUT is faster than master
		temp = (((-ppm) * TOD_COMP_PER_ONE_PPM) >= 0x80000000 ? 0x7fffffff : ((-ppm) * TOD_COMP_PER_ONE_PPM));
		timeArray.todCompensation = (MSD_U32)(0x80000000 | (MSD_U32)(temp));
	}

	if (preCompensation == timeArray.todCompensation)
	{
		return MSD_OK;
	}
	else
	{
		preCompensation = timeArray.todCompensation;
	}

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			printf("msdPtpTODBusyBitStatusGet, error code: %d\n", status);
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODStoreCompensation(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		printf("msdPtpTODStoreCompensation error, error code: %d\n", status);
		return status;
	}

	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	return status;
}
/********************************************************************************
* ptpTAIConvertToTODTime
*
*	This sample API will demonstrate how to convert one TAI time to ToD time:
*	Suppose the device PTP module using the internal 250MHz clock, so the Global Time is incremented of 4ns
*	Assume the ToD time already load into device, PTP Global Time warp around no more than once and
*	requested TAI time before captured timeArray of ToD time
*
*   TAI:    |------------|---------------------|----------------|
*          0x0      Requested TAI         Snapped TAI        0xFFFFFFFF
*                  (Register value)      (Captured load point)
*                        |                     |
*                        |                     |
*   ToD:    -------------|---------------------|---------------->
*                   Requested ToD         Snapped ToD
*                       (??)             (Captured timeArray)
*
*******************************************************************************/
static MSD_STATUS ptpTAIConvertToTODTime(MSD_U8 devNum, MSD_U8 timeArrayIndex, MSD_U32 TAIRequest)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_TIME_ARRAY timeArray;
	TimeInfo timeinfo, timeinfoSnap;
	MSD_BOOL busyStatus;
	MSD_DOUBLE clockFreq;
	MSD_U32 TAIDiff;
	MSD_DOUBLE ToDDiff, tempToDDiff0, tempToDDiff1, tempToDDiff2, tempToDDiff3, tempToDDiff4, ToDComp;
	struct tm *ptime;
	MSD_QD_DEV *dev;

	msdMemSet(&timeArray, 0, sizeof(MSD_PTP_TIME_ARRAY));

	/*Only one ToD operation can be executing at one time, so the busy bit must be zero before any ToD operation*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);
	if ((status = msdPtpTODCaptureAll(devNum, timeArrayIndex, &timeArray)) != MSD_OK)
	{
		MSG_PRINT(("Load time array error, error code: %d\n", status));
		return status;
	}
	/*When the requested ToD operation completes, the busy bit will automatically be cleared to a zero, here wait busy bit to make sure the ToD operation have completed*/
	do
	{
		if ((status = msdPtpTODBusyBitStatusGet(devNum, &busyStatus)) != MSD_OK)
		{
			MSG_PRINT(("msdPtpTODBusyBitStatusGet, error code: %d\n", status));
			return status;
		}
	} while (busyStatus);

	/* Get device information */
	dev = sohoDevGet(devNum);
	if (NULL == dev)
	{
		MSG_PRINT(("devNum %d is NULL. \n", devNum));
		return MSD_FAIL;
	}

	/* Amethyst and Peridot default clock frequency is different */
	if (dev->devName == MSD_AMETHYST)
	{
		clockFreq = 4.0; /* Amethyst default clock is 250MHz */
	}
	else
	{
		clockFreq = 8.0; /* Peridot default clock is 125MHz */
	}

	/* Record snapped ToD time */
	timeinfoSnap.nanoseconds = timeArray.todNanoseconds;
	timeinfoSnap.seconds = timeArray.todSecondsHigh;
	timeinfoSnap.seconds = ((timeinfoSnap.seconds * 1LL) << 32) | (timeArray.todSecondsLow);

	/* Please load tod time firstly */
	if (timeArray.todNanoseconds == 0 && timeArray.todSecondsLow == 0 && timeArray.todSecondsHigh == 0)
	{
		MSG_PRINT(("Please Load ToD Time Firstly.\n"));
	}
	else
	{
		/* ptp global time wrap around */
		if (timeArray.todLoadPoint < TAIRequest)
		{
			TAIDiff = (0xFFFFFFFF - TAIRequest) + timeArray.todLoadPoint + 1; /* TAI time different*/
		}
		else
		{
			TAIDiff = timeArray.todLoadPoint - TAIRequest; /* TAI time different*/
		}

		/* ToD compensation */
		ToDComp = ((timeArray.todCompensation & 0x7FFFFFFF) * 0.00465661) / (0x7FFFFFFF * 0.00465661);

		/* Need ToD compensation */
		if (ToDComp > 0)
		{
			if ((timeArray.todCompensation & 0x80000000) == 0x80000000)
			{
				ToDDiff = TAIDiff * (clockFreq - ToDComp); /* TAI time different convert to ToD time different */
			}
			else
			{
				ToDDiff = TAIDiff * (clockFreq + ToDComp); /* TAI time different convert to ToD time different */
			}
		}
		else
		{
			ToDDiff = TAIDiff * clockFreq; /* TAI time different convert to ToD time different */
		}

		/* Calculate ToD time according to ToD time different */
		/* Snapped Tod nano-seconds greater than ToD time different */
		if (timeArray.todNanoseconds > ToDDiff)
		{
			timeinfo.nanoseconds = timeArray.todNanoseconds - ToDDiff;
			timeinfo.seconds = timeArray.todSecondsHigh;
			timeinfo.seconds = (timeinfo.seconds << 32) | timeArray.todSecondsLow;
		}
		else
		{
			tempToDDiff0 = (MSD_U32)(ToDDiff / 1000000000.0); /* ToD diff convert to seconds */
			tempToDDiff1 = ToDDiff - (tempToDDiff0 * 1000000000.0); /* Remainder nano-seconds */

			tempToDDiff2 = timeArray.todNanoseconds + 1000000000.0 - tempToDDiff1; /* Snapped ToD nano-seconds add one second, substract remainder nano-seconds */

			tempToDDiff3 = (MSD_U32)(tempToDDiff2 / 1000000000.0); /* Convert to seconds*/
			tempToDDiff4 = tempToDDiff2 - (tempToDDiff3 * 1000000000.0); /* Remainder nano-seconds*/

			/* Record the requested ToD time */
			timeinfo.nanoseconds = tempToDDiff4;
			timeinfo.seconds = timeArray.todSecondsHigh;
			timeinfo.seconds = ((timeinfo.seconds << 32) | timeArray.todSecondsLow) - tempToDDiff0 - 1 + tempToDDiff3;
		}

		MSG_PRINT(("Requested TAI time: %u\n", TAIRequest));
		ptime = localtime((time_t*)&timeinfo.seconds);
		MSG_PRINT(("Requested ToD time: %d-%d-%d %d:%d:%d  %d ns\n", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday,
			ptime->tm_hour, ptime->tm_min, ptime->tm_sec, timeinfo.nanoseconds));

		MSG_PRINT(("Snapped  TAI time : %u\n", timeArray.todLoadPoint));
		ptime = localtime((time_t*)&timeinfoSnap.seconds);
		MSG_PRINT(("Snapped  ToD time : %d-%d-%d %d:%d:%d  %d ns\n", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday,
			ptime->tm_hour, ptime->tm_min, ptime->tm_sec, timeinfoSnap.nanoseconds));

	}

	return status;
}
/********************************************************************************
* becomeMaster
*
*	This sample API will set the device to Master
*
* Note: Oak/Spruce/Fir/Bonsai do not support
*
*******************************************************************************/

MSD_STATUS becomeMaster(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_MASTER_SLAVE role = MSD_PTP_ROLE_MASTER;
	MSD_QD_DEV* dev = NULL;
	MSD_SWITCH_NAME tempDevName;

	dev = sohoDevGet(devNum);

	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	tempDevName = dev->devName;
	if (tempDevName >= MSD_OAK)
	{

		MSG_PRINT(("This sample does not support this device\n"));
		return MSD_NOT_SUPPORTED;
	}

	if ((status = msdPtpMasterSlaveSet(devNum, role)) != MSD_OK)
	{
		MSG_PRINT(("Set device to Master error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* becomeSlave
*
*	This sample API will set the device to Slave
*
* Note: 
*	Oak/Spruce/Fir/Bonsai do not support
*
*******************************************************************************/
MSD_STATUS becomeSlave(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_MASTER_SLAVE role = MSD_PTP_ROLE_SLAVE;
	MSD_QD_DEV* dev = NULL;
	MSD_SWITCH_NAME tempDevName;

	dev = sohoDevGet(devNum);

	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	tempDevName = dev->devName;
	if (tempDevName >= MSD_OAK)
	{

		MSG_PRINT(("This sample does not support this device\n"));
		return MSD_NOT_SUPPORTED;
	}

	if ((status = msdPtpMasterSlaveSet(devNum, role)) != MSD_OK)
	{
		MSG_PRINT(("Set device to Slave error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* ptpSetCurTime
*
*	This sample API will demonstrate how to set current ToD (timeofday) in domain 0
*	This is sample code and using the time info struct which accuracy is second based, this should be modified based on real system to support Higher accuracy
*	1. Get the current time, this sample using the c function time() to get the system time
*	2. Initialise timeArrayIndex and domainNumber as 0 for sample
*	3. Store the time array to device
*
*******************************************************************************/
MSD_STATUS ptpSetCurTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	MSD_U8 domainNumber;
	time_t rawtime;
	TimeInfo timeinfo;

	phyAddr = 0;
	timeArrayIndex = 0;
	domainNumber = 0;
	time(&rawtime);
	timeinfo.seconds = rawtime;
	timeinfo.nanoseconds = 0;

	status = setCurrentTime(devNum, timeArrayIndex, domainNumber, &timeinfo);

	return status;
}

/********************************************************************************
* ptpGetCurTime
*
*	This sample API will demonstrate how to get current ToD (timeofday) in domain 0
*
*******************************************************************************/
MSD_STATUS ptpGetCurTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	TimeInfo timeinfo;
	struct tm *ptime;

	phyAddr = 0;
	timeArrayIndex = 0;

	if ((status = getCurrentTime(devNum, timeArrayIndex, &timeinfo)) != MSD_OK)
		return status;
	ptime = localtime((time_t*)&timeinfo.seconds);
	MSG_PRINT(("ToD time: %d-%d-%d %d:%d:%d  %d ns\n", ptime->tm_year + 1900, ptime->tm_mon + 1, ptime->tm_mday,
		ptime->tm_hour, ptime->tm_min, ptime->tm_sec, timeinfo.nanoseconds));
	return status;
}

/********************************************************************************
* ptpAdjTime
*
*	This sample API will demonstrate how to adjust the ToD
*	This sample using the c function time() to get the system time, real system should get the time form grand master
*	Suppose our device time is slower than master time with offset = 1 second
*
*******************************************************************************/
MSD_STATUS ptpAdjTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	unsigned long long offset;

	timeArrayIndex = 0;
	phyAddr = 0;
	offset = 1000000000;

	status = adjustTime(devNum, timeArrayIndex, offset);

	return status;
}

/********************************************************************************
* ptpAdjFreq
*
*	This sample API will demonstrate how to adjust the PPM difference between the local crystal clocking this PTP block and the PTP Grand Master device that this Time Array is tracking
*	Suppose our device clock is faster than master clock with 20 PPM difference
*******************************************************************************/
MSD_STATUS ptpAdjFreq(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 phyAddr;
	MSD_U8 timeArrayIndex;
	int ppm;

	timeArrayIndex = 0;
	phyAddr = 0;
	ppm = 20;

	status = adjustFrequency(devNum, timeArrayIndex, ppm);

	return status;
}

/********************************************************************************
* ptpGen1PPS
*
*	This sample API will demonstrate how to generate one pulse per second based on time array 0
*	   Configure device to generate one pulse per second
*	   Configure the 1PPS width as 0xf and width Range as 0x7, so the pulse width is 15*16.777ms = 251.655ms
*	   Select the time array 0, make sure the time array 0 is actived
*	   The selected 1 PPS signal is available on GPIO pin
*
*******************************************************************************/
MSD_STATUS ptpGen1PPS(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_PTP_PULSE_STRUCT pulsePara;

	/*Configure device to generate one pulse per second*/
	pulsePara.ptpPulseWidth = 0xf;
	pulsePara.ptp1ppsWidthRange = 0x7;
	pulsePara.ptp1ppsPhase = 0; /*the leading edge of the 1 PPS signal is the rising edge of the signal*/
	pulsePara.ptp1ppsSelect = 0;
	if ((status = msdPtpPulseSet(devNum, &pulsePara)) != MSD_OK)
	{
		MSG_PRINT(("Configure 1PPS error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* ptpRequstedTAITimeToToDTime
*
*	This sample API will demonstrate how to convert TAI time to ToD (timeofday) time
*	Note: ToD time must load firstly
*
*******************************************************************************/
MSD_STATUS ptpRequstedTAITimeToToDTime(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U32 RequestedTAI;
	MSD_U8 timeArrayIndex;

	timeArrayIndex = 0;

	if ((status = msdGetPTPGlobalTime(devNum, &RequestedTAI)) != MSD_OK)
	{
		MSG_PRINT(("Get global time error, error code: %d\n", status));
		return status;
	}

	if ((status = ptpTAIConvertToTODTime(devNum, timeArrayIndex, RequestedTAI)) != MSD_OK)
	{
		MSG_PRINT(("Convert TAI to TOD error, error code: %d\n", status));
		return status;
	}

	return status;
}

/********************************************************************************
* IEEE 1588v2 and 802.1AS config
*
*   This sample API will demonstrate set device to support both 1588v2 and 802.1AS.
*
*   This sample is for Oak and Spruce only.
*
*   Oak, Spruce and Fir PTP support two time array, and each time array has their own 
*   PTP mode config. In this sample, we set time array 0 to E2E mode to support
*   1588v2, set time array 1 to Relay mode to support 802.1AS.
*
*   After this sample function call, if 1588v2 PTP frames ingress and egress port 1~2,
*   and match time array 0, device is in 1588v2 E2E mode;
*   if 802.1as PTP frames ingress and egress port 3~4, and match time array 1,
*   device is in 8021.1as Relay mode.

*   Supported Device: Oak, Spruce, Fir, Bonsai
*   
*   Note: ToD of time array 0 and time array 1 must load firstly
*
*******************************************************************************/
MSD_STATUS ptpSet1588v2And8021as(MSD_U8 devNum)
{
	MSD_STATUS status = MSD_OK;
	MSD_U8 timeArrayIndex;
    MSD_PTP_MODE ptpMode;
    MSD_PTP_STEP ptpStep;
    MSD_PTP_RELAY_MODE relayMode;
    MSD_U8 i;
    MSD_LPORT port;
	MSD_QD_DEV* dev = NULL;
	MSD_SWITCH_NAME tempDevName;

	dev = sohoDevGet(devNum);

	if (NULL == dev)
	{
		MSD_DBG_ERROR(("Dev is NULL for devNum %d.\n", devNum));
		return MSD_FAIL;
	}

	tempDevName = dev->devName;
	if (tempDevName < MSD_OAK)
	{

		MSG_PRINT(("This sample does not support this device\n"));
		return MSD_NOT_SUPPORTED;
	}

    /* set port 1-2 to support 1588v2 one-step, use time array 0 */
    timeArrayIndex = 0;
    ptpMode = MSD_PTP_E2E_TC;
    ptpStep = MSD_PTP_ONE_STEP;
    for (i = 1; i <= 2; i++)
    {
        port = i;
        /* Actually, ptpMode is a global PTP config£¬ so in this "for" loop,  ptpMode is set 2 times repeatedly  */
        status = msdPtp1588v2ConfigSet(devNum, port, timeArrayIndex, MSD_PTP_E2E_TC, ptpStep);
        if (status != MSD_OK)
        {
            MSG_PRINT(("msdPtp1588v2ConfigSet error, error code: %d\n", status));
            return status;
        }
    }

    /* set port 3-4 to support 802.1as, use time array 1 */
    timeArrayIndex = 1;
    relayMode = MSD_PTP_RELAY;
    for (i = 3; i <= 4; i++)
    {
        port = i;
        /* Actually, relayMode is a global PTP config£¬ so in this "for" loop,  relayMode is set 2 times repeatedly  */
        status = msdPtp8021asConfigSet(devNum, port, timeArrayIndex, MSD_PTP_RELAY);
        if (status != MSD_OK)
        {
            MSG_PRINT(("msdPtp8021asConfigSet error, error code: %d\n", status));
            return status;
        }
    }

    return status;
}