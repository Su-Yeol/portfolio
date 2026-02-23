/////////
// Header
extern "C" {
#include "r_type.h"
#include "r_debug.h"
#include "r_mem.h"

#include "asn_codec.h"

///////////////////
// Macro definition

#define ASN_XER_BUFF_MAX						(1024 * 50)

#undef R_DBG_ERR(...)
#define R_DBG_ERR(...)

//////////////////
// Type Definition

//////////////////
// Extern variable

//////////////////
// Extern function

//////////////////
// Global variable

/////////////////
// Local variable

//////////////////
// Global function

// info_frame 의 저장된 정보를 제거하고 메모리를 해제 한다.
r_ret_t asn_free_info_frame(r_asn_info_t **info_frame)
{
	r_ret_t ret = R_FAIL;

	ret = r_asn_free_info(info_frame);

	return ret;
}

// J2735 Frame 을 디코딩 한다.

r_ret_t asn_decode_j2735(const u8 *j2735_frame, u32 j2735_frame_len, r_asn_info_t **out_info_frame)
{
	r_ret_t ret = R_FAIL;

	ret = r_asn_decode(R_ASN_UPER, R_ASN_PDU_MessageFrame, j2735_frame, j2735_frame_len,
	            out_info_frame);

	if (ret != R_SUCCESS) {
		R_DBG_ERR("Decoding failed[%d]!!\n", ret);
	}

	return ret;
}

// 디코딩된 ASN 정보를 출력 한다.
s32 print_info_asn(r_asn_info_t *info_asn)
{
	u8 *xer_buff = NULL;
	s32 xer_buff_len = 0;

	if (info_asn != NULL) {
		xer_buff = r_calloc_u8(ASN_XER_BUFF_MAX);

		if (xer_buff != NULL) {

			xer_buff_len = r_asn_to_xer(info_asn, xer_buff, ASN_XER_BUFF_MAX);

			if (xer_buff_len > 0) {

				xer_buff[xer_buff_len] = '\0';

				R_DBG_INFO("-pdu[%d]-rule[%d]-xer[%d bytes]\n", info_asn->pdu, info_asn->rule,
				            xer_buff_len);
				R_DBG_INFO("\n%s\n", xer_buff);
				R_DBG_INFO("-------------------------------\n");
			} else {
				if (xer_buff_len == 0) {
					xer_buff_len = R_FAIL;
				}

				R_DBG_ERR("ASN info to XER failed[%d]!!\n", xer_buff_len);
			}

			(void) r_free(xer_buff);
		}
	}

	return xer_buff_len;

}
}