/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* msdDebug.c
*
* DESCRIPTION:
*       Debug message display routine
*
* DEPENDENCIES:
*       OS Dependent
*
* FILE REVISION NUMBER:
*******************************************************************************/
#include <driver/msdApiTypes.h>
#include <utils/msdUtils.h>

static MSD_STRING_PRINT_CALLBACK printCallback = NULL;
void msdSetStringPrintCallback(MSD_STRING_PRINT_CALLBACK callback)
{
    printCallback = callback;
}

/*******************************************************************************
* msdDbgLvlSet
*
* DESCRIPTION:
*       This routine set debug level
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/

#ifdef MSD_DEBUG_QD
MSD_DBG_LEVEL dbg_level = MSD_DBG_OFF_LVL; /* Set default debug level */
#endif

void msdDbgLvlSet(MSD_U8 dbgLvl)
{
	#ifdef MSD_DEBUG_QD
	dbg_level = dbgLvl;
	#endif
	switch (dbgLvl)
	{
	case 0x0:
		MSG(("0x%x: Debug infomation off\n", dbgLvl));
		break;
	case 0x1:
		MSG(("0x%x: Print error information\n", dbgLvl));
		break;
	case 0x2:
		MSG(("0x%x: Print debug information\n", dbgLvl));
		break;
	case 0x3:
		MSG(("0x%x: Print all debug information\n", dbgLvl));
		break;
	default:
		MSG(("Debug level as follow:\n"));
		MSG(("0x0: MSD_DBG_OFF_LVL\n"));
		MSG(("0x1: MSD_DBG_ERR_LVL\n"));
		MSG(("0x2: MSD_DBG_INF_LVL\n"));
		MSG(("0x3: MSD_DBG_ALL_LVL\n"));
		break;
	}
}

#if defined(LINUX)
 #ifdef __KERNEL__
  #include "kernel.h"
 #else
  #include "stdio.h"
  #include "stdarg.h"
 #endif
#endif

/*******************************************************************************
* msdDbgPrint
*
* DESCRIPTION:
*       .
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*       None
*
*******************************************************************************/
#if defined(LINUX)
void msdDbgPrint(char* format, ...)
{
    va_list argP;
    char dbgStr[1000] = "";

    va_start(argP, format);

    vsprintf(dbgStr, format, argP);
#if defined(LINUX)
 #ifdef __KERNEL__
    if (printCallback != NULL)
        printCallback(dbgStr);
    else
        printk("%s", dbgStr);
 #else
    if (printCallback != NULL)
        printCallback(dbgStr);
    else
        printf("%s", dbgStr);
 #endif
#endif
    return;
}
#else
#include "stdio.h"
#include "stdarg.h"
void msdDbgPrint(char* format, ...)
{
    MSD_UNUSED_PARAM(format);
	va_list argP;
	char dbgStr[1000] = "";

	va_start(argP, format);

	vsprintf_s(dbgStr, 1000, format, argP);
	if (printCallback != NULL)
		printCallback(dbgStr);
	else
		printf("%s", dbgStr);

	va_end(argP);

}

#endif


char* msdDisplayStatus(MSD_STATUS status)
{
	char * str;
	switch (status)
	{
	case MSD_OK:
		str = "MSD_OK";
		break;
	case MSD_FAIL:
		str = "MSD_FAIL";
		break;
	case MSD_BAD_PARAM:
		str = "MSD_BAD_PARAM";
		break;
	case MSD_NO_SUCH:
		str = "MSD_NO_SUCH";
		break;
	case MSD_NOT_SUPPORTED:
		str = "MSD_NOT_SUPPORTED";
		break;
	case MSD_ALREADY_EXIST:
		str = "MSD_ALREADY_EXIST";
		break;
	case MSD_BAD_CPU_PORT:
		str = "MSD_BAD_CPU_PORT";
		break;
    case MSD_FEATURE_NOT_ENABLE:
        str = "MSD_FEATURE_NOT_ENABLE";
        break;
	default:
		str = "UnknowStatus";
		break;
	}
	return str;
}
