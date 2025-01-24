#ifndef _CCF_H_
#define _CCF_H_
/** ==================================================================== **/
/*** Company : Telechips                                                  */
/** Project Name : AXON LPA configure                                     */
/** Create Date  :  2023-5-22                                  */
/** Author       : KangSH                                                 */
/** ==================================================================== **/
#include <stdio.h>
#include <stdlib.h>
#include "sal_com.h"
#define DATA_SIZE     (116UL)
#define CONFIG_SIZE   (28UL)
#define BASE_IDT      (0xFE1U)

#define CCF_LOAD_HEADER		(0)	/* 0: header file, 1: T32 */

extern uint16 RoutID[CONFIG_SIZE];
#if (CCF_LOAD_HEADER == 0)
extern uint64 Base_Conf[CONFIG_SIZE][DATA_SIZE];
#else
extern uint64 *(Base_Conf[CONFIG_SIZE]);
extern uint32 lpa_ccf_start__;	/* defined in tcn1000.ld */
#endif
#endif /* _CCF_H_ */
