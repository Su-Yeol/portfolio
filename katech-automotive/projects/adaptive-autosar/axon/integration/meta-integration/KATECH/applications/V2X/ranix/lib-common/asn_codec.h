#ifndef ASN_CODEC_H
#define ASN_CODEC_H

/////////////////
// Header include
extern "C"{
#include "r_asn_sae.h"
#include "r_asn.h"

///////////////////
// MACRO definition

//////////////////
// Type definition

/////////////////
// Extern variable

//////////////////
// Extern function



/**
 * info_frame 의 저장된 정보를 제거하고 메모리를 해제 한다.
 * @param[in] out_info_frame 디코딩된 J2735 구조체와 정보가 저장된 버퍼
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 */
extern r_ret_t asn_free_info_frame(r_asn_info_t **info_frame);

/**
 * J2735 Frame 을 디코딩 한다.
 * @param[in] j2735_frame J2735 Frame 이 저장된 버퍼
 * @param[in] j2735_frame_len j2735_frame 버퍼의 크기 (Bytes)
 * @param[out] out_info_frame 디코딩된 J2735 구조체와 정보가 저장된 버퍼\n
 * out_info_frame 은 메모리가 할당 되어 전달 되므로 사용 후에 r_asn_free_info 로 제거 하여야 한다.
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 */
extern r_ret_t asn_decode_j2735(const u8 *j2735_frame, u32 j2735_frame_len,
            r_asn_info_t **out_info_frame);

/**
 * 디코딩된 ASN 정보를 출력 한다.
 * @param[in] info_asn 디코딩된 ASN Frame 정보
 * @return 성공 > 0 : 출력된 XER 의 크기(Bytes)\n
 * 실패 < 0 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 */
extern s32 print_info_asn(r_asn_info_t *info_asn);

}
#endif /* ASN_CODEC_H */

