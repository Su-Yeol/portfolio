/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* msdDevObj.h
*
* DESCRIPTION:
*       API/Structure definitions for Marvell Device Object initialize.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*******************************************************************************/

#ifndef msdDevObj_h
#define msdDevObj_h

#include <driver/msdApiTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

MSD_STATUS InitDevObj(MSD_QD_DEV *dev);

#ifdef __cplusplus
}
#endif
#endif /* __msdDevObj_h */
