/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdTCAMRC.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell TCAM range check functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdTCAMRC_h
#define Spruce_msdTCAMRC_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/
/* Exported TCAM Types			 			                                   */
/****************************************************************************/

#define SPRUCE_MAX_IGR_TCAM_ENTRY 256U


typedef struct {
	/* Keys */
	MSD_U8        rcIndex;

	/* Data & Mask*/
	MSD_U8        rcResult;
	MSD_U8        rcResultMask;

	/* Range Check Action */
	MSD_U8		continu;
	MSD_BOOL	vidOverride;
	MSD_U16		vidData;

	MSD_U8		nextId;
	MSD_BOOL	qpriOverride;
	MSD_U8		qpriData;
	MSD_BOOL	fpriOverride;
	MSD_U8		fpriData;

	MSD_U8		dpvSF;
	MSD_U32		dpvData;

	MSD_TCAM_DPV_MODE		dpvMode;
	MSD_U8		colorMode;
	MSD_BOOL	vtuPageOverride;
	MSD_U8		vtuPage;
	MSD_U8		unKnownFilter;
	MSD_U8		egActPoint;

	MSD_BOOL	ldBalanceOverride;
	MSD_U8		ldBalanceData;
	MSD_BOOL	DSCPOverride;
	MSD_U8		DSCP;
	MSD_BOOL	tcamTunnel;
	MSD_BOOL	ipMulticast;
	MSD_BOOL	ip2me;
	MSD_BOOL	routeEntry;

	MSD_BOOL	factionOverride;
	MSD_U16		factionData;

	MSD_U8		interrupt;
	MSD_U8		IncTcamCtr;
	MSD_U8		tcamCtr;

}  SPRUCE_MSD_TCAM_RC_DATA;

/****************************************************************************/
/* Exported TCAM Functions		 			                                   */
/****************************************************************************/
/*******************************************************************************
* Spruce_gtcamRCFlushAllEntry
*
* DESCRIPTION:
*       This routine is to flush all TCAM Range Check entries.Based on the function
*		of Entry Delete, set Page 2 Block 7 Reg 1 to 0x1F.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCFlushAllEntry
(
IN  MSD_QD_DEV     *dev
);
/*******************************************************************************
* Spruce_gtcamRCFlushEntry
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry.
*
*
* INPUTS:
*       rcEntryNum  - pointer to the desired entry of RC table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCFlushEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        rcEntryNum
);
/*******************************************************************************
* Spruce_gtcamRCLoadEntry
*
* DESCRIPTION:
*       This routine loads a Range Check entry.
*
* INPUTS:
*       devNum	     - Device number
*		rcEntryNum   - Pointer to the desired entry of RC table
*		rcData       - Range Check entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error.
*       MSD_BAD_PARAM - if invalid parameter is given
*       MSD_NOT_SUPPORTED - device not supported
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCLoadEntry
(
IN  MSD_QD_DEV  *dev,
IN  MSD_U32		rcEntryNum,
IN  SPRUCE_MSD_TCAM_RC_DATA *rcData
);
/*******************************************************************************
* Spruce_gtcamRCLoadEntryIntf
*
* DESCRIPTION:
*       This routine loads a TCAM Range Check entry.
*       The load sequence of TCAM entry is critical. Each TCAM entry is made up of
*       2 blocks of data. All 2 blocks need to loaded in a particular order for the TCAM RC
*       to operate correctly while frames are flowing through the switch.
*       If the entry is currently valid, it must first be flushed.
*       Each page load requires its own write TCAMOp with these TCAM page bits set
*       accordingly.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*        rcData    - Tcam entry Data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCLoadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32			rcEntryNum,
IN  MSD_TCAM_RC_DATA *rcData
);
/*******************************************************************************
* Spruce_gtcamRCReadEntry
*
* DESCRIPTION:
*       This routine reads the global 2 Block 1&7 offsets 0x02 to 0x1B registers with
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       rcEntryNum - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       rcData     - Tcam Range Check entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCReadEntry
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32		rcEntryNum,
IN  SPRUCE_MSD_TCAM_RC_DATA *rcData
);
/*******************************************************************************
* Spruce_gtcamRCReadEntryIntf
*
* DESCRIPTION:
*       This routine reads the global 2 Block 1&7 offsets 0x02 to 0x1B registers with
*       the data found in the TCAM entry and its TCAM page pointed to by the TCAM
*       entry and TCAM page bits of this register (bits 6:0 and 11:10 respectively.
*
*
* INPUTS:
*       rcEntryNum - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*       rcData     - Tcam Range Check entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCReadEntryIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_U32        rcEntryNum,
OUT MSD_TCAM_RC_DATA   *rcData
);
/*******************************************************************************
* Spruce_gtcamRCGetNextEntry
*
* DESCRIPTION:
*       This routine finds the next higher TCAM RC Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry
*       register (bits 6:0) is used as the TCAM entry to start from. To find
*       the lowest number TCAM Entry that is valid, start the Get Next operation
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 255)
*        tcamData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCGetNextEntry
(
IN		MSD_QD_DEV     *dev,
INOUT   MSD_U32		*rcEntryNum,
IN		SPRUCE_MSD_TCAM_RC_DATA *rcData
);
/*******************************************************************************
* Spruce_gtcamRCGetNextEntryIntf
*
* DESCRIPTION:
*       This routine finds the next higher TCAM RC Entry number that is valid (i.e.,
*       any entry whose Page 0 offset 0x02 is not equal to 0x00FF). The TCAM Entry
*       register (bits 6:0) is used as the TCAM entry to start from. To find
*       the lowest number TCAM Entry that is valid, start the Get Next operation
*       with TCAM Entry set to 0xFF.
*
* INPUTS:
*        tcamPointer - pointer to the desired entry of TCAM (0 ~ 255)
*
* OUTPUTS:
*        tcamPointer - next pointer entry of TCAM (0 ~ 255)
*        rcData    - Tcam entry Data
*
* RETURNS:
*       MSD_OK      - on success
*       MSD_FAIL    - on error
*       MSD_BAD_PARAM - if invalid parameter is given
*
* COMMENTS:
*       None
*
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCGetNextEntryIntf
(
IN     MSD_QD_DEV     *dev,
INOUT  MSD_U32        *tcamPointer,
OUT    MSD_TCAM_RC_DATA  *rcData
);
/*******************************************************************************
* Spruce_gtcamRCFindEntry
*
* DESCRIPTION:
*       Find the specified valid tcam entry in ingress TCAM Table.
*
* INPUTS:
*       tcamPointer - the tcam entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       tcamData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCFindEntry
(
IN	MSD_QD_DEV     *dev,
IN  MSD_U32		rcEntryNum,
IN  SPRUCE_MSD_TCAM_RC_DATA *rcData,
OUT	MSD_BOOL		  *found
);
/*******************************************************************************
* Spruce_gtcamRCFindEntryIntf
*
* DESCRIPTION:
*       Find the specified valid tcam Range Check entry in ingress TCAM Range Check Table.
*
* INPUTS:
*       rcEntryNum - the tcam range check entry index to search.
*
* OUTPUTS:
*       found    - MSD_TRUE, if the appropriate entry exists.
*       rcData - the entry parameters.
*
* RETURNS:
*       MSD_OK      - on success.
*       MSD_FAIL    - on error or entry does not exist.
*       MSD_NO_SUCH - no more entries.
*       MSD_BAD_PARAM    - on bad parameter
*
* COMMENTS:
*       None
*******************************************************************************/
MSD_STATUS Spruce_gtcamRCFindEntryIntf
(
IN	MSD_QD_DEV     *dev,
IN	MSD_U32        tcamPointer,
OUT	MSD_TCAM_RC_DATA  *rcData,
OUT	MSD_BOOL		  *found
);
/*******************************************************************************
* Spruce_gtcamRCEntryDump
*
* DESCRIPTION:
*       Finds all valid TCAM entries and print it out.
*
* INPUTS:
*       devNum  - physical device number
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
MSD_STATUS Spruce_gtcamRCEntryDump
(
IN  MSD_QD_DEV *dev
);
#ifdef __cplusplus
}
#endif

#endif /* Spruce_msdTCAMRC_h */
