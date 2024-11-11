/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/*******************************************************************************
* Spruce_msdTCAMRCComp.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell TCAM range check comparator functionality.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef Spruce_msdTCAMRCComp_h
#define Spruce_msdTCAMRCComp_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

	/*
	* Typedef: struct SPRUCE_MSD_TCAM_RC_COMP_DATA
	*
	* Description: TCAM range check Comparators data.
	*
	* Fields:
	*      Comp0Sel			- Compare 0 Selections:
	*								0x0 每 GRP: E-ECID_Base (from ETag frames)
	*								0x1 每 Ingress_E-ECID_Base (from ETag frames)
	*								0x2 每 CVLAN-ID (the VID from Q-Tagged frames)
	*								0x3 每 SVLAN-ID (the VID from S-Tagged frames)
	*								0x4 每 TCP Source Port
	*								0x5 每 TCP Destination Port
	*								0x6 每 UDP Soure Port
	*								0x7 每 UDP Destination Port
	*      Comp0Op			- Comparator 0＊s Operation.
	*								MSD_TRUE 每 Inside, match if field is inside this comparator＊s values
	*								MSD_FALSE 每 Outside, match if field is outside this comparator＊s values
	*      Comp0LoLimit		- Compare Low Limit 0.
	*	   Comp0HiLimit		- Compare High Limit 0.
	*/
	typedef struct {
		/* Comparator 0 */
		MSD_U8        Comp0Sel;
		MSD_BOOL      Comp0Op;
		MSD_U16       Comp0LoLimit;
		MSD_U16       Comp0HiLimit;

		/* Comparator 1 */
		MSD_U8        Comp1Sel;
		MSD_BOOL      Comp1Op;
		MSD_U16       Comp1LoLimit;
		MSD_U16       Comp1HiLimit;

		/* Comparator 2 */
		MSD_U8        Comp2Sel;
		MSD_BOOL      Comp2Op;
		MSD_U16       Comp2LoLimit;
		MSD_U16       Comp2HiLimit;

		/* Comparator 3 */
		MSD_U8        Comp3Sel;
		MSD_BOOL      Comp3Op;
		MSD_U16       Comp3LoLimit;
		MSD_U16       Comp3HiLimit;

		/* Comparator 4 */
		MSD_U8        Comp4Sel;
		MSD_BOOL      Comp4Op;
		MSD_U16       Comp4LoLimit;
		MSD_U16       Comp4HiLimit;

		/* Comparator 5 */
		MSD_U8        Comp5Sel;
		MSD_BOOL      Comp5Op;
		MSD_U16       Comp5LoLimit;
		MSD_U16       Comp5HiLimit;

		/* Comparator 6 */
		MSD_U8        Comp6Sel;
		MSD_BOOL      Comp6Op;
		MSD_U16       Comp6LoLimit;
		MSD_U16       Comp6HiLimit;

		/* Comparator 7 */
		MSD_U8        Comp7Sel;
		MSD_BOOL      Comp7Op;
		MSD_U16       Comp7LoLimit;
		MSD_U16       Comp7HiLimit;

	} SPRUCE_MSD_TCAM_RC_COMP_DATA;

/*******************************************************************************
* Spruce_gtcamRCCompFlushAll
*
* DESCRIPTION:
*       This routine is to flush all Port Range Comparators.Based on the function
*		of Entry Delete. Set TCAM Page2 Block6 Reg1 to 0x1F.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*        None.
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
MSD_STATUS Spruce_gtcamRCCompFlushAll
(
IN  MSD_QD_DEV     *dev
);
/*******************************************************************************
* Spruce_gtcamRCCompFlushPort
*
* DESCRIPTION:
*       This routine is to flush a single Range Checker entry.
*
* INPUTS:
*       portNum			- Logical port number to set
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
MSD_STATUS Spruce_gtcamRCCompFlushPort
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT      portNum
);
/*******************************************************************************
* Spruce_gtcamRCCompLoadIntf
*
* DESCRIPTION:
*       This routine configures a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS Spruce_gtcamRCCompLoadIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT portNum,
IN  MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);
/*******************************************************************************
* Spruce_gtcamRCCompLoad
*
* DESCRIPTION:
*       This routine configures a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS Spruce_gtcamRCCompLoad
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT portNum,
IN  SPRUCE_MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);
/*******************************************************************************
* Spruce_gtcamRCCompReadIntf
*
* DESCRIPTION:
*       This routine reads a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*
* OUTPUTS:
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS Spruce_gtcamRCCompReadIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT portNum,
OUT MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);
/*******************************************************************************
* Spruce_gtcamRCCompRead
*
* DESCRIPTION:
*       This routine reads a Range Check Compatator for the specified port.
*
* INPUTS:
*       portNum			- Logical port number to set
*
* OUTPUTS:
*       tcamRCCompData	- Range Check Comparator Data
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
MSD_STATUS Spruce_gtcamRCCompRead
(
IN  MSD_QD_DEV  *dev,
IN  MSD_LPORT portNum,
OUT SPRUCE_MSD_TCAM_RC_COMP_DATA *tcamRCCompData
);
#ifdef __cplusplus
}
#endif

#endif /* Spruce_msdTCAMRCComp_h */
