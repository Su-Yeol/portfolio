/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* sample_msdQcr.c
*
* DESCRIPTION:
*    This sample will demonstrate how to configure Qcr.
*
* DEPENDENCIES:   None.
*
*
*******************************************************************************/
#include "msdSample.h"
#include "msdApi.h"

/********************************************************************************
* Sample #1:
*       Configure ats scheduler to limit stream with layer2 10Mbps.
*
*
* Note:
*		None.
*
* Comment:
*		Only support Fir
*
********************************************************************************/

MSD_STATUS sample_msdAtsScheduler(MSD_U8 devNum)
{
    MSD_STATUS     status;
    MSD_U32        tcamPointer;
    MSD_TCAM_DATA  tcamData;

    MSD_LPORT port;
    MSD_TCAM_MODE tcamMode;
    MSD_U8 streamFilterId = 2;
    MSD_U16 maxSDUSize = 500;
    MSD_U8 blockEn = 0;

    MSD_U8 qPri = 0;
    MSD_U8 atsId = 1;
    MSD_U32 rateLimit = 10000;
    MSD_U32 cbsLimit = 10000;
    MSD_U16 maxTime = 10;
    MSD_U16 maxTimeScale = 1;
    MSD_QD_DEV *dev = sohoDevGet(devNum);

    if (dev == NULL)
    {
        MSG_PRINT(("Failed. Dev is Null.\n"));
        return MSD_FAIL;
    }

    /**********************************************************************
    *    First, set TCAM Mode for all ports
    *    In the real operation, customize this part of code according to your hardware setup
    *    Set the TCAM Mode for the ports which want to use tcam
    ***********************************************************************/
    tcamMode = MSD_TCAM_MODE_ENABLE_48; /* enable 48 bytes match tcam mode, single entry match */
    for (port = 0; port < dev->maxPorts; port++)
    {
        if ((status = msdPortTcamModeSet(devNum, port, tcamMode)) != MSD_OK)
        {
            MSG_PRINT(("msdPortTcamModeSet returned fail.\n"));
            return status;
        }
    }

    /*
    *    Flush all entries for both ingress and egress TCAM table
    */
    if ((status = msdTcamAllDelete(devNum)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamAllDelete returned fail.\n"));
        return status;
    }

    /*
    *    Initialize a TCAM entry
    */
    msdMemSet(&tcamData, 0, sizeof(MSD_TCAM_DATA)); /* Set all bit to 0 by default */
    tcamData.frameType = 0x0;   /* Normal Port */
    tcamData.frameTypeMask = 0x3;     /* Mask Type */

    /*
    *   use frameOctet and framOctetMask to match Ethernet frame format (unit: byte), with the DA = 01:00:5E:00:00:00
    *                            DA                      SA                   Tag     Length/Type   Data and Pad         FCS
    *                           6bytes                  6bytes               4bytes     2bytes      46~1500bytes        4bytes
    *                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
    *                 |                       |                       |         | vid |       |       ......     |               |
    *                 +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+------------------+---+---+---+---+
    *   frameOctet     0 1 0 0 5 E 0 0 0 0 0 0
    *   frameOctetMask F F F F F F F F F F F F
    */

    /* Match DA = 01:00:5E:00:00:00 */
    tcamData.frameOctet[0] = 0x01;
    tcamData.frameOctetMask[0] = 0xff;
    tcamData.frameOctet[1] = 0x00;
    tcamData.frameOctetMask[1] = 0xff;
    tcamData.frameOctet[2] = 0x5E;
    tcamData.frameOctetMask[2] = 0xff;
    tcamData.frameOctet[3] = 0x00;
    tcamData.frameOctetMask[3] = 0xff;
    tcamData.frameOctet[4] = 0x00;
    tcamData.frameOctetMask[4] = 0xff;
    tcamData.frameOctet[5] = 0x00;
    tcamData.frameOctetMask[5] = 0xff;

    tcamData.continu = 0x0;  /* no cascade, continue should be set to 0, no next entry*/
    tcamData.interrupt = 0x1; /* if the frame data matched this entry, there will be a tcam interrupt */
    tcamData.streamFilterEn = 0x1; /* enable stream filter */
    tcamData.streamFilterId = 0x2; /* use stream filter instance id = 2 */


    /*
    *    Load this TCAM entry.
    */
    tcamPointer = 1;  /* 0 ~ 0xff */
    if ((status = msdTcamEntryAdd(devNum, tcamPointer, &tcamData)) != MSD_OK)
    {
        MSG_PRINT(("msdTcamEntryAdd returned fail.\n"));
        return status;
    }

    /**********************************************************************
    *    Second, set stream filter instance and enable ats instance
    ***********************************************************************/
    /*
    *    Flush all stream filter instance.
    */
    if ((status = msdStreamFilterFlushAll(devNum)) != MSD_OK)
    {
        MSG_PRINT(("msdStreamFilterFlushAll returned fail.\n"));
        return status;
    }

    /*
    *    Configure stream filter instance.
    */
    if ((status = msdStreamFilterSet(devNum, streamFilterId, maxSDUSize, blockEn)) != MSD_OK)
    {
        MSG_PRINT(("msdStreamFilterSet returned fail.\n"));
        return status;
    }
    /*
    *    Enable ats instance.
    */
    if ((status = msdStreamFilterEnAtsSet(devNum, streamFilterId, atsId)) != MSD_OK)
    {
        MSG_PRINT(("msdStreamFilterEnAtsSet returned fail.\n"));
        return status;
    }

    /**********************************************************************
    *    Third, Configure ats instance. 
    ***********************************************************************/
    /*
    *    Set queue priority as default pri in egress port and enable ats queue in this port.
    */
    port = 2;
    if ((status = msdPortDefQPriSet(devNum, port, qPri)) != MSD_OK)
    {
        MSG_PRINT(("msdQcrFlushAll returned fail.\n"));
        return status;
    }

    if ((status = msdQcrSetQueueEnables(devNum, port, qPri, MSD_TRUE)) != MSD_OK)
    {
        MSG_PRINT(("msdPortQueueCtrlSet returned fail.\n"));
        return status;
    }

    if ((status = msdQcrFlushAll(devNum)) != MSD_OK)
    {
        MSG_PRINT(("msdQcrFlushAll returned fail.\n"));
        return status;
    }
    /*
    *    Set ats instance rate limit.
    */
    if ((status = msdQcrSetRateLimit(devNum, atsId, rateLimit)) != MSD_OK)
    {
        MSG_PRINT(("msdQcrSetRateLimit returned fail.\n"));
        return status;
    }
    /*
    *    Set ats instance cbs limit and max residence time.
    */
    if ((status = msdQcrSetCbsLimit(devNum, atsId, cbsLimit)) != MSD_OK)
    {
        MSG_PRINT(("msdQcrSetCbsLimit returned fail.\n"));
        return status;
    }
    if ((status = msdQcrSetMaxResidenceTime(devNum, atsId, maxTimeScale, maxTime)) != MSD_OK)
    {
        MSG_PRINT(("msdQcrSetMaxResidenceTime returned fail.\n"));
        return status;
    }
    /**************************************************************************************
    *   After this above configuration, the stream with the DA = 01:00:5E:00:00:00 egress  *
    *   from port2 will be limited to 10Mbps.                                                *
    **************************************************************************************/

    MSG_PRINT(("sample_msdStreamFilter success.\n"));
    return MSD_OK;
}
