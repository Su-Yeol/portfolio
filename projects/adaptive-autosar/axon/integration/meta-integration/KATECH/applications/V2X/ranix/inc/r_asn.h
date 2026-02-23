#ifndef R_ASN_H
#define R_ASN_H

extern "C" {
/////////
// Header

#include "r_type.h"

#include "asn_application.h"
#include "BIT_STRING.h"
#include "INTEGER.h"

///////////////////
// Macro definition

/**
 * BITMASK 설정시 사용하는 파라메터 설정\n
 * @ref r_asn_bitmask_set 함수의 bits_buf 파라메터 설정시 사용 한다.
 * @ingroup ASN1
 */
#define R_ASN_BITMASK(mask)						R_BITMASK(mask)

//////////////////
// Type definition

/**
 * ASN Encode / Decode 시 사용 하는 실제 Data 가 저장 되는 Struct Data Type
 * @ingroup ASN1
 */
typedef void ASN_STRUCT_DATA_T;

/**
 * @enum r_asn_codec_rule_t r_asn.h
 * @brief ASN Codec 에서 사용 하는 Encoding/Decoding Rules
 * @ingroup ASN1
 */
typedef enum
{
	R_ASN_OER, /**< X.696 : Octet Encoding Rules */
	R_ASN_COER, /**< X.696 : Canonical Octet Encoding Rules */

	R_ASN_UPER, /**< X.691 : Unaligned Packed Encoding Rules */
	R_ASN_UCPER, /**< X.691 : Unaligned Canonical Packed Encoding Rules */
	R_ASN_APER, /**< X.691 : Aligned Packed Encoding Rules */
	R_ASN_ACPER, /**< X.691 : Aligned Canonical Packed Encoding Rules */

	R_ASN_XER, /**< X.693 : XML Encoding Rules */
	R_ASN_CXER, /**< X.693 : Canonical XML Encoding Rules */
	R_ASN_JER, /**< X.697 : JSON Encoding Rules */

	R_ASN_RULE_MAX,
} r_asn_codec_rule_t;

/**
 * @struct r_asn_info_t r_asn.h
 * @brief Decoding 시 생성 되는 정보가 저장 되는 구조체
 * @ingroup ASN1
 */
typedef struct
{
	u32 pdu; /**< PDU Type */
	r_asn_codec_rule_t rule; /**< Rule 정보 */

	ASN_STRUCT_DATA_T *data; /**< 디코딩 후 생성된 정보가 저장되는 Structure Data */
} r_asn_info_t;

/**
 * @brief ASN List Type 정의
 * @ingroup ASN1
 */
typedef void r_asn_list_t;

/**
 * @brief ASN List Item Type 정의
 * @ingroup ASN1
 */
typedef void r_asn_list_item_t;

/**
 * @brief ASN BITMASK(BIT STRING) Type 정의
 * @ingroup ASN1
 */
typedef BIT_STRING_t r_asn_bitmask_t;

/**
 * @brief ASN 숫자(INTEGER, ENUMERATION) Type 정의
 * @ingroup ASN1
 */
typedef INTEGER_t r_asn_num_t;

/**
 * @brief ASN 문자열(OCTET STRING, IA5 STRING, UTF8 STRING) Type 정의
 * @ingroup ASN1
 */
typedef OCTET_STRING_t r_asn_str_t;

/////////////////
// Extern variable

//////////////////
// Extern function

////////////////////////////////////////////////////////////////////////////////
// Encoder

/**
 * 입력된 데이터를 Frame (rule, pdu) 으로 인코딩 한다.
 * @param[in] rule Encoding/Decoding Rules
 * @param[in] pdu PDU Type
 * @param[in] struct_data 실제 Data 가 저장 되는 구조체의 DATA
 * @param[out] out_frame Encoding 된 Frame Data (rule 타입의 hexbin)
 * @param[in] out_frame_len out_frame 버퍼의 크기 (Bytes)
 * @return 성공 > 0 : Encoding 된 Frame 의 크기 (Bytes) \n @ref R_FAIL : 실패
 * @ingroup ASN1
 * @code{c}
	#define WSM_FRAME_BUFF_MAX						(1024 * 5)

	r_ret_t ret = R_SUCCESS;
	WSM_T *wsm = NULL;
	u8 *wsm_frame = NULL;
	s32 f_size = 0;

	wsm = R_CALLOC(sizeof(WSM_T));
	// Fill in the WSM Structure Data //

	wsm_frame = R_CALLOC(WSM_FRAME_BUFF_MAX);
	f_size = r_asn_encode(R_ASN_UPER, R_ASN_PDU_ShortMsgNpdu,
	            (const ASN_STRUCT_DATA_T*) wsm, wsm_frame, WSM_FRAME_BUFF_MAX);
 * @endcode
 */
extern s32 r_asn_encode(r_asn_codec_rule_t rule, u32 pdu, const ASN_STRUCT_DATA_T *struct_data,
            u8 *out_frame, u32 out_frame_len);

/**
 * ASN_STRUCT_DATA_T 데이터를 메모리 해제 한다.
 * @param[in] pdu PDU Type
 * @param[in] struct_data 실제 Data 가 저장 되는 구조체의 DATA
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	if (f_size > 0) {
		ret = r_asn_free_struct_data(R_ASN_PDU_ShortMsgNpdu, (ASN_STRUCT_DATA_T**) &wsm);
	}
 * @endcode
 */
extern r_ret_t r_asn_free_struct_data(u32 pdu, ASN_STRUCT_DATA_T **struct_data);

////////////////////////////////////////////////////////////////////////////////
// Decoder

/**
 * Frame (rule, pdu) 을 디코딩 후 메모리를 할당 하여 r_asn_info_t 로 반환 한다.\n
 * info_frame 은 사용 후에 r_asn_free_info 로 제거 하여야 한다.
 * @param[in] rule Encoding/Decoding Rules
 * @param[in] pdu PDU Type
 * @param[in] frame Encoding 된 Frame Data (rule 타입의 hexbin)
 * @param[in] frame_len frame 버퍼의 크기 (Bytes)
 * @param[out] out_info_frame 디코딩된 Frame 의 정보
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_info_t *info_frame = NULL;

	ret = r_asn_decode(R_ASN_UPER, R_ASN_PDU_ShortMsgNpdu, wsm_frame, f_size, &info_frame);
 * @endcode
 */
extern r_ret_t r_asn_decode(r_asn_codec_rule_t rule, u32 pdu, const u8 *frame, u32 frame_len,
            r_asn_info_t **out_info_frame);

/**
 * info_frame 정보에서 디코딩된 ASN 구조체 Data 를 가져 온다.
 * @param[in] info_frame 디코딩된 Frame 의 정보
 * @return 성공 : 디코딩된 ASN 구조체 DATA 의 Pointer 가 전달 된다.\n
 * 전달된 Pointer 는 직접 r_free 등으로 메모리 해제를 하면 안되고, 반드시 r_asn_free_info 를 이용 하여야 한다.\n
 * 실패 : NULL
 * @ingroup ASN1
 * @code{c}
	void *struct_data = NULL;

	struct_data = r_asn_get_strcut_data(info_frame);
 * @endcode
 */
ASN_STRUCT_DATA_T* r_asn_get_strcut_data(r_asn_info_t *info_frame);

/**
 * rule, pdu 에 해당 하는 구조체로 info_frame 을 할당 하고 초기화 한다.\n
 * 사용 후에는 r_asn_free_info 로 메모리에서 제거 할 수 있다.
 * @param[in] rule Encoding/Decoding Rules
 * @param[in] pdu PDU Type
 * @param[out] out_info_frame 디코딩된 Frame 의 정보
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_init_info(R_ASN_UPER, R_ASN_PDU_ShortMsgNpdu, &info_frame);
 * @endcode
 */
extern r_ret_t r_asn_init_info(r_asn_codec_rule_t rule, u32 pdu, r_asn_info_t **out_info_frame);

/**
 * 디코딩된 info_frame 의 메모리를 제거 한다.
 * @param[in] info_frame
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_free_info(&info_frame);
 * @endcode
 */
extern r_ret_t r_asn_free_info(r_asn_info_t **info_frame);

////////////////////////////////////////////////////////////////////////////////
// XER 변환

/**
 * 디코딩된 Frame (rule, frame) 을 XER (XML Encoding Rules) 로 변환 한다.
 * @param[in] info_frame 디코딩된 Frame 의 정보
 * @param[out] out_xer out_xer 변환된 XER 이 저장되는 버퍼
 * @param[in] out_xer_len out_xer 버퍼의 할당된 크기(Bytes)
 * @return 성공 > 0 : XER 의 크기 (Bytes) \n @ref R_FAIL : 실패
 * @ingroup ASN1
 * @code{c}
	#define XER_BUFF_MAX		(1024 * 40)

	u8 *out_xer = NULL;
	s32 xer_size = 0;

	out_xer = R_CALLOC(XER_BUFF_MAX);
	xer_size = r_asn_to_xer(info_frame, out_xer, XER_BUFF_MAX);
 * @endcode
 */
extern s32 r_asn_to_xer(r_asn_info_t *info_frame, u8 *out_xer, u32 out_xer_len);

////////////////////////////////////////////////////////////////////////////////
// JER 변환

/**
 * 디코딩된 Frame (rule, frame) 을 JER (JSON Encoding Rules) 로 변환 한다.
 * @param[in] info_frame 디코딩된 Frame 의 정보
 * @param[out] out_xer out_xer 변환된 XER 이 저장되는 버퍼
 * @param[in] out_xer_len out_xer 버퍼의 할당된 크기(Bytes)
 * @return 성공 > 0 : XER 의 크기 (Bytes) \n @ref R_FAIL : 실패
 * @ingroup ASN1
 */
extern s32 r_asn_to_jer(r_asn_info_t *info_frame, u8 *out_xer, u32 out_xer_len);

////////////////////////////////////////////////////////////////////////////////
// BitMask (BIT_STRING) Type 을 위한 함수

/**
 * 메모리가 할당되지 않은 Bit Mask(BIT_STRING) ASN 변수에 메모리를 할당 하고 초기화 한다.
 * @param[out] asn_bitmask ASN Bit String 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_bitmask_t *bitmask = NULL;

	ret = r_asn_bitmask_alloc(&bitmask);
 * @endcode
 */
extern r_ret_t r_asn_bitmask_alloc(r_asn_bitmask_t **asn_bitmask);

/**
 * Bit Mask(BIT_STRING) ASN 변수 내의 Data 를 제거(메모리 해제) 한다.
 * @param[in] asn_bitmask ASN Bit String 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_bitmask_free(bitmask);
 * @endcode
 */
extern r_ret_t r_asn_bitmask_free(r_asn_bitmask_t *asn_bitmask);

/**
 * Bit Mask(BIT_STRING) 를 설정 한다.
 * @param[in] asn_bitmask ASN Bit String 변수
 * @param[in] bits_buf BitMask 가 저장된 버퍼
 * @param[in] num_bits bits_buf 의 Bit 수 (Bits)
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	typedef enum BitMaskStatus {
		BitMaskStatus_0	= 0,
		BitMaskStatus_1	= 1,
		BitMaskStatus_2	= 2,
		BitMaskStatus_3	= 3,
		BitMaskStatus_4	= 4
	} e_BitMaskStatus;

	u64 buf = 0;

	ret = r_asn_bitmask_alloc(&bitmask);
	if (bitmask != NULL) {
		buf = R_ASN_BITMASK(BitMaskStatus_1) | R_ASN_BITMASK(BitMaskStatus_3);
		ret = r_asn_bitmask_set(bitmask, buf, BitMaskStatus_4 + 1);
	}
 * @endcode
 */
extern r_ret_t r_asn_bitmask_set(r_asn_bitmask_t *asn_bitmask, u64 bits_buf, u16 num_bits);

/**
 * Bit Mask(BIT_STRING) 를 가져 온다.
 * @param[in] asn_bitmask ASN Bit String 변수
 * @return u64 type 의 BitMask
 * @ingroup ASN1
 * @code{c}
	u64 get = 0;

	get = r_asn_bitmask_get(bitmask);
 * @endcode
 */
extern u64 r_asn_bitmask_get(r_asn_bitmask_t *asn_bitmask);

////////////////////////////////////////////////////////////////////////////////
// Number(Interger 등) Type 을 위한 함수

/**
 * 메모리가 할당되지 않은 숫자(Number, Integer, Enumeration) ASN 변수에 메모리를 할당 하고 초기화 한다.
 * @param[in] asn_num ASN 숫자 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_num_t *num = NULL;

	ret = r_asn_num_alloc(&num);
 * @endcode
 */
extern r_ret_t r_asn_num_alloc(r_asn_num_t **asn_num);

/**
 * 숫자(Number, Integer, Enumeration) ASN 변수 내의 Data 를 제거(메모리 해제) 한다.
 * @param[in] asn_num ASN 숫자 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_num_free(num);
 * @endcode
 */
extern r_ret_t r_asn_num_free(r_asn_num_t *asn_num);

/**
 * 숫자(Number, Integer, Enumeration) 를 설정 한다.
 * @param[in] asn_num ASN 숫자 변수
 * @param[in] num 설정 하고자 하는 숫자
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	s64 val = 100;

	ret = r_asn_num_set(num, val);
 * @endcode
 */
extern r_ret_t r_asn_num_set(r_asn_num_t *asn_num, s64 num);

/**
 * 숫자(Number, Integer, Enumeration) 를 가져 온다.
 * @param[in] asn_num ASN 숫자 변수
 * @return 성공 >= 0 : 숫자의 길이 (Bytes) \n @ref R_FAIL : 실패
 * @ingroup ASN1
 * @code{c}
	s64 get = 0;

	get = r_asn_num_get(num);
 * @endcode
 */
extern s64 r_asn_num_get(r_asn_num_t *asn_num);

////////////////////////////////////////////////////////////////////////////////
// String Type 을 위한 함수

/**
 * 메모리가 할당되지 않은 문자열 ASN 변수에 메모리를 할당 하고 초기화 한다.
 * @param[in] asn_str ASN 문자열 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_str_t *str = NULL;

	ret = r_asn_str_alloc(&str);
 * @endcode
 */
extern r_ret_t r_asn_str_alloc(r_asn_str_t **asn_str);

/**
 * 문자열(OCTET STRING) ASN 변수 내의 Data 를 제거(메모리 해제) 한다.
 * @param[in] asn_str ASN 문자열 변수
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_str_free(str);
 * @endcode
 */
extern r_ret_t r_asn_str_free(r_asn_str_t *asn_str);

/**
 * 문자열(String, UTF8, IA5 등) 을 설정 한다.\n
 * 설정한 문자열은 r_asn_str_free 를 사용하여 제거 할 수 있다.
 * @param[in] asn_str ASN 문자열 변수
 * @param[in] str 설정 하고자 하는 문자열
 * @param[in] str_len 문자열의 길이 (Bytes)
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	u8 *name = "RANiX V2X Platform";

	ret = r_asn_str_set(str, name, strlen(name));
 * @endcode
 */
extern r_ret_t r_asn_str_set(r_asn_str_t *asn_str, const u8 *str, u32 str_len);

/**
 * 문자열(String, UTF8, IA5 등)을 가져 온다.
 * @param[in] asn_str ASN 문자열 변수
 * @param[out] out_str 문자열 - 메모리를 할당 하고 문자열을 복사하여 전달 된다.
 * @return 성공 >= 0 : 숫자의 길이 (Bytes) \n @ref R_FAIL : 실패
 * @ingroup ASN1
 * @code{c}
	u8 *get = NULL;
	s32 len = 0;

	len = r_asn_str_get(str, &get);
 * @endcode
 */
extern s32 r_asn_str_get(r_asn_str_t *asn_str, u8 **out_str);

////////////////////////////////////////////////////////////////////////////////
// List Type 을 위한 함수

/**
 * 메모리가 할당되지 않은 List ASN 변수에 메모리를 할당 하고 초기화 한다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_list_t *list = NULL;

	ret = r_asn_list_alloc((r_asn_list_t**)&list);
 * @endcode
 */
extern r_ret_t r_asn_list_alloc(r_asn_list_t **asn_list);

/**
 * List 에 저장 된 Item 들을 List 에서만 제거 한다.\n
 * 각각의 Item 의 Data 들은 메모리에서 제거 되지 않으므로, 필요시에 할당 하였던 방법과 동일한 방법으로 제거 하여야 한다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	ret = r_asn_list_init(list);
 * @endcode
 */
extern r_ret_t r_asn_list_init(r_asn_list_t *asn_list);

/**
 * List 에 Item 을 추가 한다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @param[in] item 추가 하고자 하는 Item 의 포인터
 * @return 성공 > 0 : List 내의 Item 개수\n
 * 실패 : R_FAIL (@ref r_ret_t)
 * @ingroup ASN1
 * @code{c}
	r_asn_list_item_t *item1 = NULL;
	s32 cnt = 0;

	item1 = R_CALLOC(sizeof(r_asn_list_item_t));
	if(item1 != NULL) {
		(void) strcpy(item1, "RANiX");
		cnt = r_asn_list_add((r_asn_list_t*)list, (r_asn_list_item_t*)item1);
	}
 * @endcode
 */
extern s32 r_asn_list_add(r_asn_list_t *asn_list, r_asn_list_item_t *item);

/**
 * List 의 해당 Index 의 Item 을 List 에서만 제거 한다.\n
 * 단, 추가 되었던 Item 의 Data 는 메모리에서 제거 되지 않는다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @param[in] index Item 의 Index
 * @return 성공 > 0 : List 내의 Item 개수 \n @ref R_FAIL : 실패
 * @ingroup ASN1
 * @code{c}
	u16 idx = 1;

	cnt = r_asn_list_del((r_asn_list_t*)list, idx);
 * @endcode
 */
extern s32 r_asn_list_del(r_asn_list_t *asn_list, u16 index);

/**
 * List 의 해당 Index 의 Item의 포인터를 가져 온다.\n
 * 가져온 값은 임의로 제거(r_free 등) 하면 안된다.\n
 * 단, List 의 Item 자체를 제거시에는 Item 을 할당 한 방법과 동일한 방법으로 제거 하여야 한다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @param[in] index Item 의 Index
 * @param[out] out_item List 에 저장된 Index 의 Item 포인터
 * @return 성공 : @ref R_SUCCESS \n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	r_asn_list_item_t *item_o = NULL;

	ret = r_asn_list_get((r_asn_list_t*)list, idx, (r_asn_list_item_t**)&item_o);
 * @endcode
 */
extern r_ret_t r_asn_list_get(r_asn_list_t *asn_list, u16 index, r_asn_list_item_t **out_item);

/**
 * List 내에 추가 되어 있는 Item 의 개수를 가져 온다.
 * @param[in] asn_list ASN 의 List 구조체 변수 (Sequence or SET)
 * @return 성공 >= 0 : List 내의 Item 개수\n
 * 실패 : @ref R_FAIL or @ref R_INVALID_PARAMETER
 * @ingroup ASN1
 * @code{c}
	cnt = r_asn_list_get_count((r_asn_list_t*)list);
 * @endcode
 */
extern s32 r_asn_list_get_count(r_asn_list_t *asn_list);
}
#endif /* R_ASN_H */
