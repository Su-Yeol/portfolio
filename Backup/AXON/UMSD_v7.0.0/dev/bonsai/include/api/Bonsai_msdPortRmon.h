/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Bonsai_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Bonsai_msdPortRmon_h
#define Bonsai_msdPortRmon_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported MIBS Types			 			                                   */
/****************************************************************************/
/*
 *  typedef: struct MSD_STATS_COUNTERS
 *
 *  Description: MIB counter
 *     
 */
#define BONSAI_MSD_TYPE_BANK  0x400
#define BONSAI_MSD_TYPE_BANK2 0x800

typedef enum
{
	/* Bank 0 */
	Bonsai_STATS_InGoodOctetsLo = 0,
	Bonsai_STATS_InGoodOctetsHi = 1,
	Bonsai_STATS_InBadOctets = 2,

	Bonsai_STATS_OutFCSErr = 3,
	Bonsai_STATS_InUnicasts = 4,
	Bonsai_STATS_Deferred = 5,             /* offset 5 */
	Bonsai_STATS_InBroadcasts = 6,
	Bonsai_STATS_InMulticasts = 7,
	Bonsai_STATS_64Octets = 8,
	Bonsai_STATS_127Octets = 9,
	Bonsai_STATS_255Octets = 0xa,            /* offset 10 */
	Bonsai_STATS_511Octets = 0xb,
	Bonsai_STATS_1023Octets = 0xc,
	Bonsai_STATS_MaxOctets = 0xd,
	Bonsai_STATS_OutOctetsLo = 0xe,
	Bonsai_STATS_OutOctetsHi = 0xf,
	Bonsai_STATS_OutUnicasts = 0x10,          /* offset 16 */
	Bonsai_STATS_Excessive = 0x11,
	Bonsai_STATS_OutMulticasts = 0x12,
	Bonsai_STATS_OutBroadcasts = 0x13,
	Bonsai_STATS_Single = 0x14,
	Bonsai_STATS_OutPause = 0x15,
	Bonsai_STATS_InPause = 0x16,
	Bonsai_STATS_Multiple = 0x17,
	Bonsai_STATS_InUndersize = 0x18,          /* offset 24 */
	Bonsai_STATS_InFragments = 0x19,
	Bonsai_STATS_InOversize = 0x1a,
	Bonsai_STATS_InJabber = 0x1b,
	Bonsai_STATS_InRxErr = 0x1c,
	Bonsai_STATS_InFCSErr = 0x1d,
	Bonsai_STATS_Collisions = 0x1e,
	Bonsai_STATS_Late = 0x1f,                 /* offset 31 */
	/* Bank 1 */
    Bonsai_STATS_InDiscards      = BONSAI_MSD_TYPE_BANK+0x00,
    Bonsai_STATS_InFiltered      = BONSAI_MSD_TYPE_BANK+0x01,
    Bonsai_STATS_InAccepted      = BONSAI_MSD_TYPE_BANK+0x02,
    Bonsai_STATS_InBadAccepted   = BONSAI_MSD_TYPE_BANK+0x03,
    Bonsai_STATS_InGoodAvbClassA = BONSAI_MSD_TYPE_BANK+0x04,
    Bonsai_STATS_InGoodAvbClassB = BONSAI_MSD_TYPE_BANK+0x05,
    Bonsai_STATS_InBadAvbClassA  = BONSAI_MSD_TYPE_BANK+0x06,
    Bonsai_STATS_InBadAvbClassB  = BONSAI_MSD_TYPE_BANK+0x07,
    Bonsai_STATS_TCAMCounter0    = BONSAI_MSD_TYPE_BANK+0x08,
    Bonsai_STATS_TCAMCounter1    = BONSAI_MSD_TYPE_BANK+0x09,
    Bonsai_STATS_TCAMCounter2    = BONSAI_MSD_TYPE_BANK+0x0a,
    Bonsai_STATS_TCAMCounter3    = BONSAI_MSD_TYPE_BANK+0x0b,
    Bonsai_STATS_InDroppedAvbA   = BONSAI_MSD_TYPE_BANK+0x0c,
    Bonsai_STATS_InDroppedAvbB   = BONSAI_MSD_TYPE_BANK+0x0d, 
    Bonsai_STATS_InDaUnknown     = BONSAI_MSD_TYPE_BANK+0x0e,
    Bonsai_STATS_InMGMT          = BONSAI_MSD_TYPE_BANK+0x0f,
    Bonsai_STATS_OutQueue0       = BONSAI_MSD_TYPE_BANK+0x10,
    Bonsai_STATS_OutQueue1       = BONSAI_MSD_TYPE_BANK+0x11,
    Bonsai_STATS_OutQueue2       = BONSAI_MSD_TYPE_BANK+0x12,
    Bonsai_STATS_OutQueue3       = BONSAI_MSD_TYPE_BANK+0x13,
    Bonsai_STATS_OutQueue4       = BONSAI_MSD_TYPE_BANK+0x14,
    Bonsai_STATS_OutQueue5       = BONSAI_MSD_TYPE_BANK+0x15,
    Bonsai_STATS_OutQueue6       = BONSAI_MSD_TYPE_BANK+0x16,
    Bonsai_STATS_OutQueue7       = BONSAI_MSD_TYPE_BANK+0x17,
    Bonsai_STATS_OutCutThrough   = BONSAI_MSD_TYPE_BANK+0x18,
	Bonsai_STATS_InBadQbv		  = BONSAI_MSD_TYPE_BANK+0x19,
    Bonsai_STATS_OutOctetsA      = BONSAI_MSD_TYPE_BANK+0x1a,
    Bonsai_STATS_OutOctetsB      = BONSAI_MSD_TYPE_BANK+0x1b,
    Bonsai_STATS_OutYel          = BONSAI_MSD_TYPE_BANK+0x1c,
    Bonsai_STATS_OutDroppedYel   = BONSAI_MSD_TYPE_BANK+0x1d, 
    Bonsai_STATS_OutDiscards     = BONSAI_MSD_TYPE_BANK+0x1e, 
    Bonsai_STATS_OutMGMT         = BONSAI_MSD_TYPE_BANK+0x1f,

	/* Bank 2 */
	Bonsai_STATS_DropEvents      = BONSAI_MSD_TYPE_BANK2 + 0x00

} BONSAI_MSD_STATS_COUNTERS;

/*
 *  typedef: struct BONSAI_MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct BONSAI_MSD_STATS_COUNTER_SET_
{
	/* Bank 0 */
    MSD_U32    InGoodOctetsLo;     /* offset 0 */
    MSD_U32    InGoodOctetsHi;     /* offset 1 */
    MSD_U32    InBadOctets;        /* offset 2 */
    MSD_U32    OutFCSErr;          /* offset 3 */
    MSD_U32    InUnicasts;         /* offset 4 */
    MSD_U32    Deferred;           /* offset 5 */
    MSD_U32    InBroadcasts;       /* offset 6 */
    MSD_U32    InMulticasts;       /* offset 7 */
    /* 
        Histogram Counters : Rx Only, Tx Only, or both Rx and Tx 
        (refer to Histogram Mode) 
    */
    MSD_U32    Octets64;         /* 64 Octets, offset 8 */
    MSD_U32    Octets127;        /* 65 to 127 Octets, offset 9 */
    MSD_U32    Octets255;        /* 128 to 255 Octets, offset 10 */
    MSD_U32    Octets511;        /* 256 to 511 Octets, offset 11 */
    MSD_U32    Octets1023;       /* 512 to 1023 Octets, offset 12 */
    MSD_U32    OctetsMax;        /* 1024 to Max Octets, offset 13 */
    MSD_U32    OutOctetsLo;      /* offset 14 */
    MSD_U32    OutOctetsHi;      /* offset 15 */
    MSD_U32    OutUnicasts;      /* offset 16 */
    MSD_U32    Excessive;        /* offset 17 */
    MSD_U32    OutMulticasts;    /* offset 18 */
    MSD_U32    OutBroadcasts;    /* offset 19 */
    MSD_U32    Single;           /* offset 20 */

    MSD_U32    OutPause;         /* offset 21 */
    MSD_U32    InPause;          /* offset 22 */
    MSD_U32    Multiple;         /* offset 23 */
    MSD_U32    InUndersize;        /* offset 24 */
    MSD_U32    InFragments;        /* offset 25 */
    MSD_U32    InOversize;         /* offset 26 */
    MSD_U32    InJabber;           /* offset 27 */
    MSD_U32    InRxErr;          /* offset 28 */
    MSD_U32    InFCSErr;         /* offset 29 */
    MSD_U32    Collisions;       /* offset 30 */
    MSD_U32    Late;             /* offset 31 */
	/* Bank 1 */
    MSD_U32    InDiscards;       /* offset 0x00 */
    MSD_U32    InFiltered;       /* offset 0x01 */
    MSD_U32    InAccepted;       /* offset 0x02 */
    MSD_U32    InBadAccepted;    /* offset 0x03 */
    MSD_U32    InGoodAvbClassA;  /* offset 0x04 */
    MSD_U32    InGoodAvbClassB;  /* offset 0x05 */
    MSD_U32    InBadAvbClassA ;  /* offset 0x06 */
    MSD_U32    InBadAvbClassB ;  /* offset 0x07 */
    MSD_U32    TCAMCounter0;     /* offset 0x08 */
    MSD_U32    TCAMCounter1;     /* offset 0x09 */
    MSD_U32    TCAMCounter2;     /* offset 0x0a */
    MSD_U32    TCAMCounter3;     /* offset 0x0b */
    MSD_U32    InDroppedAvbA;    /* offset 0x0c */
    MSD_U32    InDroppedAvbB;    /* offset 0x0d */
    MSD_U32    InDaUnknown ;     /* offset 0x0e */
    MSD_U32    InMGMT;           /* offset 0x0f */
    MSD_U32    OutQueue0;        /* offset 0x10 */
    MSD_U32    OutQueue1;        /* offset 0x11 */
    MSD_U32    OutQueue2;        /* offset 0x12 */
    MSD_U32    OutQueue3;        /* offset 0x13 */
    MSD_U32    OutQueue4;        /* offset 0x14 */
    MSD_U32    OutQueue5;        /* offset 0x15 */
    MSD_U32    OutQueue6;        /* offset 0x16 */
    MSD_U32    OutQueue7;        /* offset 0x17 */
    MSD_U32    OutCutThrough;    /* offset 0x18 */
	MSD_U32    InBadQbv;		 /* offset 0x19 */
    MSD_U32    OutOctetsA;       /* offset 0x1a */
    MSD_U32    OutOctetsB;       /* offset 0x1b */
    MSD_U32    OutYel;           /* offset 0x1c */
    MSD_U32    OutDroppedYel;    /* offset 0x1d */
    MSD_U32    OutDiscards;      /* offset 0x1e */
    MSD_U32    OutMGMT;          /* offset 0x1f */

	/* Bank 2 */
	MSD_U32    DropEvents;       /* offset 0x00 */

}BONSAI_MSD_STATS_COUNTER_SET;

/*
 * typedef: enum MSD_HISTOGRAM_MODE
 *
 * Description: Enumeration of the histogram counters mode.
 *
 * Enumerations:
 *   MSD_COUNT_RX_ONLY - In this mode, Rx Histogram Counters are counted.
 *   MSD_COUNT_TX_ONLY - In this mode, Tx Histogram Counters are counted.
 *   MSD_COUNT_RX_TX   - In this mode, Rx and Tx Histogram Counters are counted.
 */
typedef enum
{
	BONSAI_MSD_COUNT_RX_ONLY = 1,
	BONSAI_MSD_COUNT_TX_ONLY,
	BONSAI_MSD_COUNT_RX_TX
} BONSAI_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Bonsai_gstatsFlushAll
*
* DESCRIPTION:
*       Flush All RMON counters for all ports.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK   - on success
*       MSD_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* Bonsai_gstatsFlushPort
*
* DESCRIPTION:
*       Flush All RMON counters for a given port.
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* Bonsai_gstatsGetPortCounter
*
* DESCRIPTION:
*        This routine gets a specific counter of the given port
*
* INPUTS:
*        port - the logical port number.
*        counter - the counter which will be read
*
* OUTPUTS:
*        statsData - points to 32bit data storage for the MIB counter
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT         port,
	IN  BONSAI_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS Bonsai_gstatsGetPortCounterIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
IN  MSD_STATS_COUNTERS    counter,
OUT MSD_U32            *statsData
);
/*******************************************************************************
* Bonsai_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to BONSAI_MSD_STATS_COUNTER_SET for the MIB counters
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
	OUT BONSAI_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS Bonsai_gstatsGetPortAllCntersIntf
(
IN  MSD_QD_DEV               *dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* Bonsai_gstatsGetHistogramMode
*
* DESCRIPTION:
*        This routine gets the Histogram Counters Mode.
*
* INPUTS:
*        None.
*
* OUTPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
	OUT BONSAI_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS Bonsai_gstatsGetHistogramModeIntf
(
IN  MSD_QD_DEV   *dev,
OUT MSD_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* Bonsai_gstatsSetHistogramMode
*
* DESCRIPTION:
*        This routine sets the Histogram Counters Mode.
*
* INPUTS:
*        mode - Histogram Mode (MSD_COUNT_RX_ONLY, MSD_COUNT_TX_ONLY, 
*                    and MSD_COUNT_RX_TX)
*
* OUTPUTS:
*        None.
*
* RETURNS:
*       MSD_OK - on success,
*       MSD_FAIL - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*        None.
*
*******************************************************************************/
MSD_STATUS Bonsai_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
	IN BONSAI_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS Bonsai_gstatsSetHistogramModeIntf
(
IN MSD_QD_DEV   *dev,
IN MSD_HISTOGRAM_MODE        mode
);

/*******************************************************************************
* Bonsai_gmibDump
*
* DESCRIPTION:
*       Dumps MIB from the specified port.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - logical port number.
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
MSD_STATUS Bonsai_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);

#ifdef __cplusplus
}
#endif

#endif /* __Bonsai_msdPortRmon_h */
