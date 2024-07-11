/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* BonsaiZ1_msdPortRmon.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell MIBS functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef BonsaiZ1_msdPortRmon_h
#define BonsaiZ1_msdPortRmon_h

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
#define BONSAIZ1_MSD_TYPE_BANK  0x400
#define BONSAIZ1_MSD_TYPE_BANK2 0x800

typedef enum
{
	/* Bank 0 */
	BonsaiZ1_STATS_InGoodOctetsLo = 0,
	BonsaiZ1_STATS_InGoodOctetsHi = 1,
	BonsaiZ1_STATS_InBadOctets = 2,

	BonsaiZ1_STATS_OutFCSErr = 3,
	BonsaiZ1_STATS_InUnicasts = 4,
	BonsaiZ1_STATS_Deferred = 5,             /* offset 5 */
	BonsaiZ1_STATS_InBroadcasts = 6,
	BonsaiZ1_STATS_InMulticasts = 7,
	BonsaiZ1_STATS_64Octets = 8,
	BonsaiZ1_STATS_127Octets = 9,
	BonsaiZ1_STATS_255Octets = 0xa,            /* offset 10 */
	BonsaiZ1_STATS_511Octets = 0xb,
	BonsaiZ1_STATS_1023Octets = 0xc,
	BonsaiZ1_STATS_MaxOctets = 0xd,
	BonsaiZ1_STATS_OutOctetsLo = 0xe,
	BonsaiZ1_STATS_OutOctetsHi = 0xf,
	BonsaiZ1_STATS_OutUnicasts = 0x10,          /* offset 16 */
	BonsaiZ1_STATS_Excessive = 0x11,
	BonsaiZ1_STATS_OutMulticasts = 0x12,
	BonsaiZ1_STATS_OutBroadcasts = 0x13,
	BonsaiZ1_STATS_Single = 0x14,
	BonsaiZ1_STATS_OutPause = 0x15,
	BonsaiZ1_STATS_InPause = 0x16,
	BonsaiZ1_STATS_Multiple = 0x17,
	BonsaiZ1_STATS_InUndersize = 0x18,          /* offset 24 */
	BonsaiZ1_STATS_InFragments = 0x19,
	BonsaiZ1_STATS_InOversize = 0x1a,
	BonsaiZ1_STATS_InJabber = 0x1b,
	BonsaiZ1_STATS_InRxErr = 0x1c,
	BonsaiZ1_STATS_InFCSErr = 0x1d,
	BonsaiZ1_STATS_Collisions = 0x1e,
	BonsaiZ1_STATS_Late = 0x1f,                 /* offset 31 */
	/* Bank 1 */
    BonsaiZ1_STATS_InDiscards      = BONSAIZ1_MSD_TYPE_BANK+0x00,
    BonsaiZ1_STATS_InFiltered      = BONSAIZ1_MSD_TYPE_BANK+0x01,
    BonsaiZ1_STATS_InAccepted      = BONSAIZ1_MSD_TYPE_BANK+0x02,
    BonsaiZ1_STATS_InBadAccepted   = BONSAIZ1_MSD_TYPE_BANK+0x03,
    BonsaiZ1_STATS_InGoodAvbClassA = BONSAIZ1_MSD_TYPE_BANK+0x04,
    BonsaiZ1_STATS_InGoodAvbClassB = BONSAIZ1_MSD_TYPE_BANK+0x05,
    BonsaiZ1_STATS_InBadAvbClassA  = BONSAIZ1_MSD_TYPE_BANK+0x06,
    BonsaiZ1_STATS_InBadAvbClassB  = BONSAIZ1_MSD_TYPE_BANK+0x07,
    BonsaiZ1_STATS_TCAMCounter0    = BONSAIZ1_MSD_TYPE_BANK+0x08,
    BonsaiZ1_STATS_TCAMCounter1    = BONSAIZ1_MSD_TYPE_BANK+0x09,
    BonsaiZ1_STATS_TCAMCounter2    = BONSAIZ1_MSD_TYPE_BANK+0x0a,
    BonsaiZ1_STATS_TCAMCounter3    = BONSAIZ1_MSD_TYPE_BANK+0x0b,
    BonsaiZ1_STATS_InDroppedAvbA   = BONSAIZ1_MSD_TYPE_BANK+0x0c,
    BonsaiZ1_STATS_InDroppedAvbB   = BONSAIZ1_MSD_TYPE_BANK+0x0d, 
    BonsaiZ1_STATS_InDaUnknown     = BONSAIZ1_MSD_TYPE_BANK+0x0e,
    BonsaiZ1_STATS_InMGMT          = BONSAIZ1_MSD_TYPE_BANK+0x0f,
    BonsaiZ1_STATS_OutQueue0       = BONSAIZ1_MSD_TYPE_BANK+0x10,
    BonsaiZ1_STATS_OutQueue1       = BONSAIZ1_MSD_TYPE_BANK+0x11,
    BonsaiZ1_STATS_OutQueue2       = BONSAIZ1_MSD_TYPE_BANK+0x12,
    BonsaiZ1_STATS_OutQueue3       = BONSAIZ1_MSD_TYPE_BANK+0x13,
    BonsaiZ1_STATS_OutQueue4       = BONSAIZ1_MSD_TYPE_BANK+0x14,
    BonsaiZ1_STATS_OutQueue5       = BONSAIZ1_MSD_TYPE_BANK+0x15,
    BonsaiZ1_STATS_OutQueue6       = BONSAIZ1_MSD_TYPE_BANK+0x16,
    BonsaiZ1_STATS_OutQueue7       = BONSAIZ1_MSD_TYPE_BANK+0x17,
    BonsaiZ1_STATS_OutCutThrough   = BONSAIZ1_MSD_TYPE_BANK+0x18,
	BonsaiZ1_STATS_InBadQbv		  = BONSAIZ1_MSD_TYPE_BANK+0x19,
    BonsaiZ1_STATS_OutOctetsA      = BONSAIZ1_MSD_TYPE_BANK+0x1a,
    BonsaiZ1_STATS_OutOctetsB      = BONSAIZ1_MSD_TYPE_BANK+0x1b,
    BonsaiZ1_STATS_OutYel          = BONSAIZ1_MSD_TYPE_BANK+0x1c,
    BonsaiZ1_STATS_OutDroppedYel   = BONSAIZ1_MSD_TYPE_BANK+0x1d, 
    BonsaiZ1_STATS_OutDiscards     = BONSAIZ1_MSD_TYPE_BANK+0x1e, 
    BonsaiZ1_STATS_OutMGMT         = BONSAIZ1_MSD_TYPE_BANK+0x1f,

	/* Bank 2 */
	BonsaiZ1_STATS_DropEvents      = BONSAIZ1_MSD_TYPE_BANK2 + 0x00

} BONSAIZ1_MSD_STATS_COUNTERS;

/*
 *  typedef: struct BONSAIZ1_MSD_STATS_COUNTER_SET
 *
 *  Description: MIB Counter Set
 *     
 */
typedef struct BONSAIZ1_MSD_STATS_COUNTER_SET_
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

}BONSAIZ1_MSD_STATS_COUNTER_SET;

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
	BONSAIZ1_MSD_COUNT_RX_ONLY = 1,
	BONSAIZ1_MSD_COUNT_TX_ONLY,
	BONSAIZ1_MSD_COUNT_RX_TX
} BONSAIZ1_MSD_HISTOGRAM_MODE;

/****************************************************************************/
/* Exported MIBS Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* BonsaiZ1_gstatsFlushAll
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
MSD_STATUS BonsaiZ1_gstatsFlushAll
(
    IN MSD_QD_DEV* dev
);

/*******************************************************************************
* BonsaiZ1_gstatsFlushPort
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
MSD_STATUS BonsaiZ1_gstatsFlushPort
(
    IN MSD_QD_DEV* dev,
    IN MSD_LPORT    port
);

/*******************************************************************************
* BonsaiZ1_gstatsGetPortCounter
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
MSD_STATUS BonsaiZ1_gstatsGetPortCounter
(
    IN  MSD_QD_DEV        *dev,
    IN  MSD_LPORT         port,
	IN  BONSAIZ1_MSD_STATS_COUNTERS    counter,
    OUT MSD_U32            *statsData
);
MSD_STATUS BonsaiZ1_gstatsGetPortCounterIntf
(
IN  MSD_QD_DEV        *dev,
IN  MSD_LPORT        port,
IN  MSD_STATS_COUNTERS    counter,
OUT MSD_U32            *statsData
);
/*******************************************************************************
* BonsaiZ1_gstatsGetPortAllCounters
*
* DESCRIPTION:
*       This routine gets all RMON counters of the given port
*
* INPUTS:
*       port - the logical port number.
*
* OUTPUTS:
*       statsCounterSet - points to BONSAIZ1_MSD_STATS_COUNTER_SET for the MIB counters
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
MSD_STATUS BonsaiZ1_gstatsGetPortAllCounters
(
    IN  MSD_QD_DEV* dev,
    IN  MSD_LPORT        port,
	OUT BONSAIZ1_MSD_STATS_COUNTER_SET    *statsCounterSet
);
MSD_STATUS BonsaiZ1_gstatsGetPortAllCntersIntf
(
IN  MSD_QD_DEV               *dev,
IN  MSD_LPORT        port,
OUT MSD_STATS_COUNTER_SET    *statsCounterSet
);
/*******************************************************************************
* BonsaiZ1_gstatsGetHistogramMode
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
MSD_STATUS BonsaiZ1_gstatsGetHistogramMode
(
    IN  MSD_QD_DEV   *dev,
	OUT BONSAIZ1_MSD_HISTOGRAM_MODE    *mode
);
MSD_STATUS BonsaiZ1_gstatsGetHistogramModeIntf
(
IN  MSD_QD_DEV   *dev,
OUT MSD_HISTOGRAM_MODE    *mode
);

/*******************************************************************************
* BonsaiZ1_gstatsSetHistogramMode
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
MSD_STATUS BonsaiZ1_gstatsSetHistogramMode
(
    IN MSD_QD_DEV   *dev,
	IN BONSAIZ1_MSD_HISTOGRAM_MODE        mode
);
MSD_STATUS BonsaiZ1_gstatsSetHistogramModeIntf
(
IN MSD_QD_DEV   *dev,
IN MSD_HISTOGRAM_MODE        mode
);

/*******************************************************************************
* BonsaiZ1_gmibDump
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
MSD_STATUS BonsaiZ1_gmibDump
(
IN   MSD_QD_DEV      *dev,
IN  MSD_LPORT  portNum
);

#ifdef __cplusplus
}
#endif

#endif /* __BonsaiZ1_msdPortRmon_h */
