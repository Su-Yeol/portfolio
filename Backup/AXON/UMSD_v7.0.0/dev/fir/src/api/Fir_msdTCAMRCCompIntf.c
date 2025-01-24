/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/*******************************************************************************
* Fir_msdTCAMRCCompIntf.c
*
* DESCRIPTION:
*       API definitions for TCAM range check comparator
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*******************************************************************************/

#include <fir/include/api/Fir_msdTCAMRCComp.h>


/*******************************************************************************
* Fir_gtcamRCCompLoadIntf
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
MSD_STATUS Fir_gtcamRCCompLoadIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT portNum,
IN  MSD_TCAM_RC_COMP_DATA *tcamRCCompData
)
{
	FIR_MSD_TCAM_RC_COMP_DATA tcamRCComp;

	if (NULL == tcamRCCompData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_COMP_DATA in Fir_gtcamRCCompLoadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcamRCComp.Comp0Sel = tcamRCCompData->Comp0Sel;
	tcamRCComp.Comp0Op = tcamRCCompData->Comp0Op;
	tcamRCComp.Comp0LoLimit = tcamRCCompData->Comp0LoLimit;
	tcamRCComp.Comp0HiLimit = tcamRCCompData->Comp0HiLimit;

	tcamRCComp.Comp1Sel = tcamRCCompData->Comp1Sel;
	tcamRCComp.Comp1Op = tcamRCCompData->Comp1Op;
	tcamRCComp.Comp1LoLimit = tcamRCCompData->Comp1LoLimit;
	tcamRCComp.Comp1HiLimit = tcamRCCompData->Comp1HiLimit;

	tcamRCComp.Comp2Sel = tcamRCCompData->Comp2Sel;
	tcamRCComp.Comp2Op = tcamRCCompData->Comp2Op;
	tcamRCComp.Comp2LoLimit = tcamRCCompData->Comp2LoLimit;
	tcamRCComp.Comp2HiLimit = tcamRCCompData->Comp2HiLimit;

	tcamRCComp.Comp3Sel = tcamRCCompData->Comp3Sel;
	tcamRCComp.Comp3Op = tcamRCCompData->Comp3Op;
	tcamRCComp.Comp3LoLimit = tcamRCCompData->Comp3LoLimit;
	tcamRCComp.Comp3HiLimit = tcamRCCompData->Comp3HiLimit;

	tcamRCComp.Comp4Sel = tcamRCCompData->Comp4Sel;
	tcamRCComp.Comp4Op = tcamRCCompData->Comp4Op;
	tcamRCComp.Comp4LoLimit = tcamRCCompData->Comp4LoLimit;
	tcamRCComp.Comp4HiLimit = tcamRCCompData->Comp4HiLimit;

	tcamRCComp.Comp5Sel = tcamRCCompData->Comp5Sel;
	tcamRCComp.Comp5Op = tcamRCCompData->Comp5Op;
	tcamRCComp.Comp5LoLimit = tcamRCCompData->Comp5LoLimit;
	tcamRCComp.Comp5HiLimit = tcamRCCompData->Comp5HiLimit;

	tcamRCComp.Comp6Sel = tcamRCCompData->Comp6Sel;
	tcamRCComp.Comp6Op = tcamRCCompData->Comp6Op;
	tcamRCComp.Comp6LoLimit = tcamRCCompData->Comp6LoLimit;
	tcamRCComp.Comp6HiLimit = tcamRCCompData->Comp6HiLimit;

	tcamRCComp.Comp7Sel = tcamRCCompData->Comp7Sel;
	tcamRCComp.Comp7Op = tcamRCCompData->Comp7Op;
	tcamRCComp.Comp7LoLimit = tcamRCCompData->Comp7LoLimit;
	tcamRCComp.Comp7HiLimit = tcamRCCompData->Comp7HiLimit;

	return Fir_gtcamRCCompLoad(dev, portNum, &tcamRCComp);
}

/*******************************************************************************
* Fir_gtcamRCCompReadIntf
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
MSD_STATUS Fir_gtcamRCCompReadIntf
(
IN  MSD_QD_DEV     *dev,
IN  MSD_LPORT portNum,
OUT MSD_TCAM_RC_COMP_DATA *tcamRCCompData
)
{
	MSD_STATUS           retVal;
	FIR_MSD_TCAM_RC_COMP_DATA tcamRCComp;

	retVal = Fir_gtcamRCCompRead(dev, portNum, &tcamRCComp);
	if (MSD_OK != retVal)
	{
		return retVal;
	}

	if (NULL == tcamRCCompData)
	{
		MSD_DBG_ERROR(("Input param MSD_TCAM_RC_COMP_DATA in Fir_gtcamRCCompReadIntf is NULL. \n"));
		return MSD_BAD_PARAM;
	}

	tcamRCCompData->Comp0Sel = tcamRCComp.Comp0Sel;
	tcamRCCompData->Comp0Op = tcamRCComp.Comp0Op;
	tcamRCCompData->Comp0LoLimit = tcamRCComp.Comp0LoLimit;
	tcamRCCompData->Comp0HiLimit = tcamRCComp.Comp0HiLimit;

	tcamRCCompData->Comp1Sel = tcamRCComp.Comp1Sel;
	tcamRCCompData->Comp1Op = tcamRCComp.Comp1Op;
	tcamRCCompData->Comp1LoLimit = tcamRCComp.Comp1LoLimit;
	tcamRCCompData->Comp1HiLimit = tcamRCComp.Comp1HiLimit;

	tcamRCCompData->Comp2Sel = tcamRCComp.Comp2Sel;
	tcamRCCompData->Comp2Op = tcamRCComp.Comp2Op;
	tcamRCCompData->Comp2LoLimit = tcamRCComp.Comp2LoLimit;
	tcamRCCompData->Comp2HiLimit = tcamRCComp.Comp2HiLimit;

	tcamRCCompData->Comp3Sel = tcamRCComp.Comp3Sel;
	tcamRCCompData->Comp3Op = tcamRCComp.Comp3Op;
	tcamRCCompData->Comp3LoLimit = tcamRCComp.Comp3LoLimit;
	tcamRCCompData->Comp3HiLimit = tcamRCComp.Comp3HiLimit;

	tcamRCCompData->Comp4Sel = tcamRCComp.Comp4Sel;
	tcamRCCompData->Comp4Op = tcamRCComp.Comp4Op;
	tcamRCCompData->Comp4LoLimit = tcamRCComp.Comp4LoLimit;
	tcamRCCompData->Comp4HiLimit = tcamRCComp.Comp4HiLimit;

	tcamRCCompData->Comp5Sel = tcamRCComp.Comp5Sel;
	tcamRCCompData->Comp5Op = tcamRCComp.Comp5Op;
	tcamRCCompData->Comp5LoLimit = tcamRCComp.Comp5LoLimit;
	tcamRCCompData->Comp5HiLimit = tcamRCComp.Comp5HiLimit;

	tcamRCCompData->Comp6Sel = tcamRCComp.Comp6Sel;
	tcamRCCompData->Comp6Op = tcamRCComp.Comp6Op;
	tcamRCCompData->Comp6LoLimit = tcamRCComp.Comp6LoLimit;
	tcamRCCompData->Comp6HiLimit = tcamRCComp.Comp6HiLimit;

	tcamRCCompData->Comp7Sel = tcamRCComp.Comp7Sel;
	tcamRCCompData->Comp7Op = tcamRCComp.Comp7Op;
	tcamRCCompData->Comp7LoLimit = tcamRCComp.Comp7LoLimit;
	tcamRCCompData->Comp7HiLimit = tcamRCComp.Comp7HiLimit;

	return retVal;
}