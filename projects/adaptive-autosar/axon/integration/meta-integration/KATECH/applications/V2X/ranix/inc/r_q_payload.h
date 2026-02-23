#ifndef R_Q_PAYLOAD_H
#define R_Q_PAYLOAD_H

/////////
// Header

#include "r_queue.h"

///////////////////
// Macro definition

//////////////////
// Type definition

/**
 * @struct r_q_payload_t r_q_payload.h
 * @brief Payload 의 비동기 처리를 위해 사용하는 Queue 의 저장 구조체
 * @ingroup QUEUE
 */
typedef struct
{
	r_queue_t *payload_q; /**< Payload 를 저장 하기 위한 Queue */
	r_mutex_t mutex; /**< 동시 처리 방지를 위한 Mutex */
} r_q_payload_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * Payload Queue 를 초기화 한다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @param[in] max_num 최대 저장 Data 개수 (@ref R_QUEUE_MAX_NUM)
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	r_ret_t ret = R_SUCCESS;
	r_q_payload_t *q_p_info = NULL;

	q_p_info = r_calloc(sizeof(r_q_payload_t));

	if (q_p_info != NULL) {
		ret = r_q_payload_open(q_p_info, R_QUEUE_MAX_NUM);
	}
 * @endcode
 */
extern r_ret_t r_q_payload_open(r_q_payload_t *info_q_payload, u16 max_num);

/**
 * Payload Queue 를 제거 한다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 * @code{c}
	if (q_p_info != NULL) {
		ret = r_q_payload_close(q_p_info);
		(void) r_free(q_p_info);
	}
 * @endcode
 */
extern r_ret_t r_q_payload_close(r_q_payload_t *info_q_payload);

/**
 * Payload 를 저장 한다.\n
 * payload 는 별도의 메모리를 할당(r_malloc) 하여 저장 한다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @param[in] payload 저장하고자 하는 Payload Data\n
 * payload 는 Q 내부에 메모리를 할당 후 별도로 저장 한다.
 * @param[in] payload_len payload 의 크기 (Bytes)
 * @param[in] psid Payload 의 PSID 값
 * @param[in] modem 사용중인 통신 장치의 타입 (@ref r_cfm_modem_t)_
 * @param[in] result payload 를 포함하고 있던 Frame 의 처리 결과 설정\n
 * V2X 보안 스택에서 사용 할 경우에는 서명 검증 결과 등이 설정 된다.
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 */
extern r_ret_t r_q_payload_push(r_q_payload_t *info_q_payload, const u8 *payload, u32 payload_len,
            u32 psid, u32 modem, r_ret_t result);

/**
 * Payload 를 Queue 에서 가져 온다.\n
 * payload 는 사용 후 반드시 메모리 해제(r_free) 를 하여야 한다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @param[out] payload Queue 에서 가져온 하는 Payload Data\n
 * NULL 일 경우, 저장된 payload 의 메모리 해제(r_free)를 한다.
 * @param[out] payload_len payload 의 크기 (Bytes)\n
 * NULL 일 경우, 크기를 전달 하지 않는다.
 * @param[out] psid Payload 의 PSID 값\n
 * NULL 일 경우, PSID 값을 전달 하지 않는다.
 * @param[out] modem 사용중인 통신 장치의 타입 (@ref r_cfm_modem_t)
 * NULL 일 경우, 값을 전달 하지 않는다.
 * @param[out] result payload 를 포함하고 있던 Frame 의 처리 결과\n
 * NULL 일 경우, result 값을 전달 하지 않는다.
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 */
extern r_ret_t r_q_payload_pop(r_q_payload_t *info_q_payload, u8 **payload, u32 *payload_len,
            u32 *psid, u32 *modem, r_ret_t *result);

/**
 * Payload Queue 에 남아 있는 Payload 들을 모두 제거 한다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @return @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup QUEUE
 */
extern r_ret_t r_q_payload_flush(r_q_payload_t *info_q_payload);

/**
 * Queue 에 남아 있는 Payload 개수를 가져 온다.
 * @param[in] info_q_payload Payload Queue 의 정보
 * @return 저장된 Payload Data 의 개수
 * @ingroup QUEUE
 * @code{c}
	s16 count = 0;

	count = r_q_payload_count(q_p_info);
 * @endcode
 */
extern s16 r_q_payload_count(r_q_payload_t *info_q_payload);

#endif /* R_Q_PAYLOAD_H */
