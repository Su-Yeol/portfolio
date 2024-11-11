/**********************************************************************************************
* Copyright (c) 2023 Marvell.
* All rights reserved.
* Use of this source code is governed by a BSD3 license that
* can be found in the LICENSE file and also at https://opensource.org/licenses/BSD-3-Clause
**********************************************************************************************/

/******************************************************************************
* BonsaiZ1_msdQav.h
*
* DESCRIPTION:
*       API definitions for Qav
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
******************************************************************************/

#ifndef BonsaiZ1_msdQav_h
#define BonsaiZ1_msdQav_h

#include <driver/msdApiTypes.h>
#include <driver/msdSysConfig.h>
#include <utils/msdUtils.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef USE_SINGLE_READ
#define BONSAIZ1_MAX_PTP_CONSECUTIVE_READ    4

MSD_STATUS BonsaiZ1_gqavSetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    IN  MSD_U32        rate
);

MSD_STATUS BonsaiZ1_gqavGetPortQpriXRate
(
    IN  MSD_QD_DEV     *dev,
    IN  MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U32        *rate
);

MSD_STATUS BonsaiZ1_gqavSetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN    MSD_LPORT    port,
    IN  MSD_U8        queue,
    IN  MSD_U16        hiLimit
);

MSD_STATUS BonsaiZ1_gqavGetPortQpriXHiLimit
(
    IN  MSD_QD_DEV     *dev,
    IN    MSD_LPORT    port,
    IN  MSD_U8        queue,
    OUT MSD_U16        *hiLimit
);


#ifdef __cplusplus
}
#endif

#endif /* __BonsaiZ1_msdQav_h */
