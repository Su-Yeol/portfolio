#ifndef MAP_DECODE_H
#define MAP_DECODE_H

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
 * 디코딩된 MAP 정보를 이용하여 저장된 내용을 출력 한다.
 * @param[in] info_map 디코딩된 MAP(J2735) 구조체와 정보가 저장된 버퍼
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 */
extern r_ret_t show_map(r_asn_info_t *info_map);

#endif /* MAP_DECODE_H */

