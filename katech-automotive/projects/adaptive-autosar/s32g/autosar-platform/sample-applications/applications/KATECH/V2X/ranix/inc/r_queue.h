#ifndef R_QUEUE_H
#define R_QUEUE_H

/////////
// Header

#include "r_type.h"

///////////////////
// Macro definition

/**
 * Queue 의 최대 저장 가능 개수
 * @ingroup QUEUE
 */
#define R_QUEUE_MAX_NUM							(10240)

//////////////////
// Type definition

/**
 * @enum r_queue_type_t r_queue.h
 * @brief Queue 의 Data 저장 과 관련된 Type
 * @ingroup QUEUE
 */
typedef enum
{
	R_QUEUE_FULL, /**< Full 이 되면 더이상 Push 가 되지 않는다 */
	R_QUEUE_NO_FULL /**< Full 이 되면 가장 오래전에 Push 된 Data 를 삭제 하고 계속 Push 한다 */
} r_queue_type_t;

/**
 * @struct r_queue_t r_queue.h
 * @brief Queue 정보
 * @ingroup QUEUE
 */
typedef struct
{
	s16 max_num; /**< 최대 Data 저장 개수 */

	r_queue_type_t qtype; /**< Queue 의 Type */

	s16 sidx; /**< 시작 Index */
	s16 eidx; /**< 종료 Index */

	u8 **data; /**< Data 저장 공간 */
	u32 *data_size; /**< 각 Data 의 크기 저장 공간 */
} r_queue_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @brief max_num 의 저장 개수로 Queue 를 초기화 한다.
 * @param[in] info_q Queue 정보
 * @param[in] max_num 최대 저장 Data 개수 (@ref R_QUEUE_MAX_NUM)
 * @param[in] qtype Queue 의 Push Type
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	r_ret_t ret = R_SUCCESS;
	r_queue_t *q_info = NULL;

	q_info = r_calloc(sizeof(r_queue_t));

	if (q_info != NULL) {
		ret = r_queue_open(q_info, R_QUEUE_MAX_NUM, R_QUEUE_FULL);
	}
 * @endcode
 */
extern r_ret_t r_queue_open(r_queue_t *info_q, u16 max_num, r_queue_type_t qtype);

/**
 * @brief Queue 를 제거 한다.
 * @param[in] info_q Queue 정보
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	if (q_info != NULL) {
		ret = r_queue_close(q_info);
		(void) r_free(q_info);
	}
 * @endcode
 */
extern r_ret_t r_queue_close(r_queue_t *info_q);

/**
 * @brief Queue 에 Data 를 저장 한다.
 * @param[in] info_q Queue 정보
 * @param[in] data 저장하려는 Data 의 Pointer\n
 * data 는 별도로 allocation 하여 저장 하지 않고 전달된 pointer 그대로 저장 한다.
 * @param[in] data_size Data 의 길이 (Bytes)
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	#define DATA_SIZE		(256)

	u8 *data = NULL;

	data = R_CALLOC(DATA_SIZE);
	if(data != NULL) {
		(void) strcpy(data, "RANiX");
		ret = r_queue_push(q_info, (u8*)data, strlen(data));
	}
 * @endcode
 */
extern r_ret_t r_queue_push(r_queue_t *info_q, u8 *data, u32 data_size);

/**
 * @brief 저장된 Data 를 Queue 에서 가져 온다.\n
 * Data 는 Queue 에서 제거 된다.
 * @param[in] info_q Queue 정보
 * @param[out] data 저장된 Data 의 Pointer
 * @param[out] data_size 저장된 Data 의 길이 (Bytes)
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	u8 *data = NULL;
	u32 len = 0;

	ret = r_queue_pop(q_info, &data, &len);
 * @endcode
 */
extern r_ret_t r_queue_pop(r_queue_t *info_q, u8 **data, u32 *data_size);

/**
 * @brief 저장된 Data 의 Index 에 해당 하는 Item 을 가져 온다.\n
 * Data 는 Pop 이 되지 않아 Queue 내에서 제거 되지 않고 포인터만 전달 받는다.\n
 * ex) Queue 개수 10개 : Index 0 ~ 9
 * @param[in] info_q Queue 정보
 * @param[in] index Item 의 Index, Index 는 저장된 Data 개수 보다 작아야 한다.\n
 * r_queue_get_count() -1 => 가장 마지막으로 Push 한 Data 의 Index\n
 * 0 => 현재 Q 에 저정된 Data 중 가장 오래된 Index
 * @param[out] data 저장된 Data 의 Pointer
 * @param[out] data_size 저장된 Data 의 길이 (Bytes)
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	ret = r_queue_get(q_info, index, &data, &len);
 * @endcode
 */
extern r_ret_t r_queue_get(r_queue_t *info_q, u16 index, u8 **data, u32 *data_size);

/**
 * @brief Queue 에 저장된 Data 개수를 구한다.
 * @param[in] info_q Queue 정보
 * @return 성공 : >= 0, 저장된 data의 개수\n
 * 실패 : < 0
 * @ingroup QUEUE
 * @code{c}
	s16 count = 0;

	count = r_queue_get_count(q_info);
 * @endcode
 */
extern s16 r_queue_get_count(r_queue_t *info_q);

/**
 * @brief Queue 정보 출력을 위한 디버그 함수
 * @param[in] info_q Queue 정보
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	ret = r_queue_print(q_info);
 * @endcode
 */
extern r_ret_t r_queue_print(r_queue_t *info_q);

#endif /* R_QUEUE_H */
