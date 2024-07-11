/**********************************************************************************************
* Copyright (c) 2020 Marvell Semiconductor Ltd.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/********************************************************************************
* msdSample.h
*
* DESCRIPTION:
*       Types definitions for Sample program
*
* DEPENDENCIES:   Platform.
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#ifndef MSDSAMPLE_H_
#define MSDSAMPLE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define MSG_PRINT(x) testPrint x
#define testPrint  printf

#ifdef __cplusplus
}
#endif

#endif /* MSDSAMPLE_H_ */
