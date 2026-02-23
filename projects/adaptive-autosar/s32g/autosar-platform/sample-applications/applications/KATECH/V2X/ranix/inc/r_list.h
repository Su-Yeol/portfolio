#ifndef R_LIST_H
#define R_LIST_H

/////////
// Header

#include "r_type.h"

///////////////////
// Macro definition

/**
 * @struct r_list_t r_list.h
 * @brief List handler 의 정보
 * @ingroup LIST
 */
typedef struct
{
	void **list; /**< List 저장 변수 */
	s32 list_size; /**< List 의 크기 */

	s32 count; /**< 현재 저장된 Item Data 의 개수 */
} r_list_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @brief List 를 초기화 한다.
 * @param[in] info_list List 정보 handler
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL or @ref R_INVALID_PARAMETER : 실패
 * @ingroup LIST
 * @code{c}
	r_list_t *info_list = NULL;

	info_list = r_calloc(sizeof(r_list_t));

	if (info_list != NULL) {
		ret = r_list_open(info_list);
	}
 * @endcode
 */
extern r_ret_t r_list_open(r_list_t *info_list);

/**
 * @brief List 를 제거 한다.
 * @param[in] info_list List 정보 handler
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL or @ref R_INVALID_PARAMETER : 실패
 * @ingroup LIST
 * @code{c}
	if (info_list != NULL) {
		ret = r_list_close(info_list);
		(void) r_free(info_list);
	}
 * @endcode
 */
extern r_ret_t r_list_close(r_list_t *info_list);

/**
 * @brief List 에 Data 를 저장 한다.
 * @param[in] info_list List 정보 handler
 * @param[in] data 저장 하고자 하는 Data 의 pointer
 * data 는 별도로 allocation 하여 저장 하지 않고 전달된 pointer 그대로 저장 한다.
 * @param[in] data_size data 의 size (Bytes)
 * @return 성공 : >= 0, 저장된 Data 의 개수\n
 * 실패 : < 0 (@ref r_ret_t)
 * @ingroup LIST
 * @code{c}
	#define DATA_SIZE		(256)

	u8 *data = NULL;
	s32 list_num = 0;

	data = R_CALLOC(DATA_SIZE);
	if(data != NULL) {
		(void) strcpy(data, "RANiX");
		list_num = r_list_add(info_list, (u8*)data, strlen(data));\
	}
 * @endcode
 */
extern s32 r_list_add(r_list_t *info_list, u8 *data, u32 data_size);

/**
 * @brief List 에서 Index 의 Item 를 제거 한다.\n
 * List Item 에 저장된 Data 는 직접 제거(r_free 등) 해야 한다.
 * @param[in] info_list List 정보 handler
 * @param[in] list_idx 삭제 하고자 하는 List Item 의  Index
 * @return 성공 : >= 0 , 남아 있는 List Item 개수\n
 * 실패 : < 0 (@ref r_ret_t)
 * @ingroup LIST
 * @code{c}
	s32 list_index = 0; // Set the index number.

	list_num = r_list_del(info_list, list_index);
 * @endcode
 */
extern s32 r_list_del(r_list_t *info_list, s32 list_idx);

/**
 * @brief List 에서 Index 에 해당 하는 Item 의 Data 를 가져 온다.\n
 * Data 는 별도의 메모리 할당 없이 List 에 저장된 Data의 포인터만 전달 받는다.\n
 * @param[in] info_list List 정보 handler
 * @param[in] list_idx List Item 의  Index
 * @param[out] data List 에 저장 된 Item 의 Data pointer
 * @param[out] data_size data 의 Size (Bytes)
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL or @ref R_INVALID_PARAMETER : 실패
 * @ingroup LIST
 * @code{c}
	u8 *data = NULL;
	u32 data_size = 0;

	ret = r_list_get(info_list, list_idx, &data, &data_size);
 * @endcode
 */
extern r_ret_t r_list_get(r_list_t *info_list, s32 list_idx, u8 **data,
            u32 *data_size);

/**
 * @brief List 에 저장된 Data 개수를 구한다.
 * @param[in] info_list List 정보 handler
 * @return 성공:>=0, 저장된 Data 의 개수\n
 * 실패 : < 0, @ref r_ret_t 참조
 * @ingroup LIST
 * @code{c}
	list_num = r_list_get_count(info_list);
 * @endcode
 */
extern s32 r_list_get_count(r_list_t *info_list);

/**
 * @brief List 정보 출력을 위한 디버그 함수
 * @param[in] info_list List 정보 handler
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup LIST
 * @code{c}
	ret = r_list_print(info_list);
 * @endcode
 */
extern r_ret_t r_list_print(r_list_t *info_list);

#endif /* R_LIST_H */
