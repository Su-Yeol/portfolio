/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Amethyst_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef __Amethyst_msdPortRmon_h
#define __Amethyst_msdPortRmon_h

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
#define AMETHYST_MSD_TYPE_BANK 0x400
typedef enum
{
	/* Bank 0 */
    Amethyst_STATS_InGoodOctetsLo = 0,
    Amethyst_STATS_InGoodOctetsHi,
    Amethyst_STATS_InBadOctets,
    
    Amethyst_STATS_OutFCSErr,
    Amethyst_STATS_InUnicasts,
    Amethyst_STATS_Deferred,             /* offset 5 */
    Amethyst_STATS_InBroadcasts,
    Amethyst_STATS_InMulticasts,
    Amethyst_STATS_64Octets,
    Amethyst_STATS_127Octets,
    Amethyst_STATS_255Octets,            /* offset 10 */
    Amethyst_STATS_511Octets,
    Amethyst_STATS_1023Octets,
    Amethyst_STATS_MaxOctets,
    Amethyst_STATS_OutOctetsLo,
    Amethyst_STATS_OutOctetsHi,
    Amethyst_STATS_OutUnicasts,          /* offset 16 */
    Amethyst_STATS_Excessive,
    Amethyst_STATS_OutMulticasts,
    Amethyst_STATS_OutBroadcasts,
    Amethyst_STATS_Single,
    Amethyst_STATS_OutPause,
    Amethyst_STATS_InPause,
    Amethyst_STATS_Multiple,
    Amethyst_STATS_InUndersize,          /* offset 24 */
    Amethyst_STATS_InFragments,
    Amethyst_STATS_InOversize,
    Amethyst_STATS_InJabber,
    Amethyst_STATS_InRxErr,
    Amethyst_STATS_InFCSErr,
    Amethyst_STATS_Collisions,
    Amethyst_STATS_Late,                 /* offset 31 */
	/* Bank 1 */
    Amethyst_STATS_InDiscards      = AMETHYST_MSD_TYPE_BANK+0x00,
    Amethyst_STATS_InFiltered      = AMETHYST_MSD_TYPE_BANK+0x01,
    Amethyst_STATS_InAccepted      = AMETHYST_MSD_TYPE_BANK+0x02,
    Amethyst_STATS_InBadAccepted   = AMETHYST_MSD_TYPE_BANK+0x03,
    Amethyst_STATS_InGoodAvbClassA = AMETHYST_MSD_TYPE_BANK+0x04,
    Amethyst_STATS_InGoodAvbClassB = AMETHYST_MSD_TYPE_BANK+0x05,
    Amethyst_STATS_InBadAvbClassA  = AMETHYST_MSD_TYPE_BANK+0x06,
    Amethyst_STATS_InBadAvbClassB  = AMETHYST_MSD_TYPE_BANK+0x07,
    Amethyst_STATS_TCAMCounter0    = AMETHYST_MSD_TYPE_BANK+0x08,
    Amethyst_STATS_TCAMCounter1    = AMETHYST_MSD_TYPE_BANK+0x09,
    Amethyst_STATS_TCAMCounter2    = AMETHYST_MSD_TYPE_BANK+0x0a,
    Amethyst_STATS_TCAMCounter3    = AMETHYST_MSD_TYPE_BANK+0x0b,
    Amethyst_STATS_InDroppedAvbA   = AMETHYST_MSD_TYPE_BANK+0x0c,
    Amethyst_STATS_InDroppedAvbB   = AMETHYST_MSD_TYPE_BANK+0x0d, 
    Amethyst_STATS_InDaUnknown     = AMETHYST_MSD_TYPE_BANK+0x0e,
    Amethyst_STATS_InMGMT          = AMETHYST_MSD_TYPE_BANK+0x0f,
    Amethyst_STATS_OutQueue0       = AMETHYST_MSD_TYPE_BANK+0x10,
    Amethyst_STATS_OutQueue1       = AMETHYST_MSD_TYPE_BANK+0x11,
    Amethyst_STATS_OutQueue2       = AMETHYST_MSD_TYPE_BANK+0x12,
    Amethyst_STATS_OutQueue3       = AMETHYST_MSD_TYPE_BANK+0x13,
    Amethyst_STATS_OutQueue4       = AMETHYST_MSD_TYPE_BANK+0x14,
    Amethyst_STATS_OutQueue5       = AMETHYST_MSD_TYPE_BANK+0x15,
    Amethyst_STATS_OutQueue6       = AMETHYST_MSD_TYPE_BANK+0x16,
    Amethyst_STATS_OutQueue7       = AMETHYST_MSD_TYPE_BANK+0x17,
    Amethyst_STATS_OutCutThrough   = AMETHYST_MSD_TYPE_BANK+0x18,
	Amethyst_STATS_InBadQbv		   = AMETHYST_MSD_TYPE_BANK+0x19,
    Amethyst_STATS_OutOctetsA      = AMETHYST_MSD_TYPE_BANK+0x1a,
    Amethyst_STATS_OutOctetsB      = AMETHYST_MSD_TYPE_BANK+0x1b,
    Amethyst_STATS_OutYel          = AMETHYST_MSD_TYPE_BANK+0x1c,
    Amethyst_STATS_OutDroppedYel   = AMETHYST_MSD_TYPE_BANK+0x1d, 
    Amethyst_STATS_OutDiscards     = AMETHYST_MSD_TYPE_BANK+0x1e, 
    Amethyst_STATS_OutMGMT         = AMETHYST_MSD_TYPE_BANK+0x1f

} AMETHYST_MSD_STATS_COUNTERS;

/*
 *  typedef: struct AMETHYST_MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct _AMETHYST_MSD_STATS_COUNTER_SET
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

}AMETHYST_MSD_STATS_COUNTER_SET;

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
	AMETHYST_MSD_COUNT_RX_ONLY = 1,
	AMETHYST_MSD_COUNT_TX_ONLY,
	AMETHYST_MSD_COUNT_RX_TX
} AMETHYST_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Amethyst_gstatsFlushAll
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
MSD_STATUS Amethyst_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* Amethyst_gstatsFlushPort
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
MSD_STATUS Amethyst_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* Amethyst_gstatsGetPortCounter
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
MSD_STATUS Amethyst_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT         port,
	IN  AMETHYST_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS Amethyst_gstatsGetPortCounterIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
IN  MSD_STATS_COUNTERS    counter,
OUT MSD_U32            *statsData
);
/*******************************************************************************
* Amethyst_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to AMETHYST_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS Amethyst_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
	OUT AMETHYST_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS Amethyst_gstatsGetPortAllCountersIntf
(
IN  MSD_QD_DEV               *dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* Amethyst_gstatsGetHistogramMode
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
MSD_STATUS Amethyst_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
	OUT AMETHYST_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS Amethyst_gstatsGetHistogramModeIntf
(
IN  MSD_QD_DEV   *dev,
OUT MSD_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* Amethyst_gstatsSetHistogramMode
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
MSD_STATUS Amethyst_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
	IN AMETHYST_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS Amethyst_gstatsSetHistogramModeIntf
(
IN MSD_QD_DEV   *dev,
IN MSD_HISTOGRAM_MODE        mode
);

/*******************************************************************************
* Amethyst_gmibDump
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
MSD_STATUS Amethyst_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);

#ifdef __cplusplus
}
#endif

#endif /* __Amethyst_msdPortRmon_h */
