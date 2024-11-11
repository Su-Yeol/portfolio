/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/


/********************************************************************************
* msdVersion.c
*
* DESCRIPTION:
*       Includes software version information for the QuarterDeck software
*       suite.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#define MSDAPI_VERSION "7.0.0"

char msdApiVersion[] = MSDAPI_VERSION;

/*******************************************************************************
* msdVersion
*
* DESCRIPTION:
*       This function returns the version of the QuarterDeck SW suite.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       version     - QuarterDeck software version.
*
* RETURNS:
*       MSD_OK on success,
*       MSD_BAD_PARAM on bad parameters,
*       MSD_FAIL otherwise.
*
* COMMENTS:
*
*******************************************************************************/
MSD_STATUS msdVersion
(
    OUT MSD_VERSION   *version
)
{
	MSD_DBG_INFO(("msdVersion Called.\n"));
    if(version == NULL)
        return MSD_BAD_PARAM;

    if(msdStrlen(msdApiVersion) > MSD_VERSION_MAX_LEN)
    {
        return MSD_FAIL;
    }

    msdMemCpy(version->version,msdApiVersion,msdStrlen(msdApiVersion));
    version->version[msdStrlen(msdApiVersion)] = '\0';
	MSD_DBG_INFO(("OK.\n"));
    return MSD_OK;
}

