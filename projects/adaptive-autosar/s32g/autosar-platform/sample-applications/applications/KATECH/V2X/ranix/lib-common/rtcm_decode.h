#ifndef RTCM_DECODE_H
#define RTCM_DECODE_H

/////////////////
// Header include

#include "asn_codec.h"

///////////////////
// MACRO definition

//////////////////
// Type definition

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * 디코딩된 RTCM 정보를 이용하여 저장된 내용을 출력 한다.
 * @param[in] info_spat 디코딩된 RTCM(J2735) 구조체와 정보가 저장된 버퍼
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 */
extern r_ret_t show_rtcm(r_asn_info_t *info_rtcm);

#endif /* RTCM_DECODE_H */
