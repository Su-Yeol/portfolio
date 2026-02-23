#ifndef R_THREAD_H
#define R_THREAD_H

/////////
// Header

#include "r_type.h"

///////////////////
// Macro definition

//////////////////
// Type definition

/**
 * @struct r_thread_t r_thread.h
 * @brief Thread 의 ID 를 저장
 * @ingroup THREAD
 */
typedef u64 r_thread_t;

/**
 * @enum r_thread_status_t r_thread.h
 * Thread 의 상태
 * @ingroup THREAD
 */
typedef enum
{
	R_THREAD_EXIT_WAIT = 1, /**< Thread 가 종료 하기 위한 상태 */
	R_THREAD_EXIT = 2, /**< Thread 가 종료 된 상태 */
	R_THREAD_RUN = 3, /**< Thread 가 동작 상태 */
} r_thread_status_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @brief Thread 를 생성 한다.
 * @param[in] id Thread ID
 * @param[in] thread_func Thread Callback 함수
 * @param[in] arg Thread Callback 함수 호출시 전달 되는 Argument
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup THREAD
 * @code{c}
	r_ret_t ret = R_SUCCESS;
	r_thread_t thread_id = 0;

	ret = r_thread_create(&thread_id, thread_func, (void *)"thread func");
 * @endcode
 */
extern r_ret_t r_thread_create(r_thread_t *id, void* (*thread_func)(void*),
            void *arg);

/**
 * @brief Thread 가 종료 될때 까지 대기 한다.
 * @param[in] id Thread ID
 * @param[in] thread_ret Thread 종료시 전달 되는 return 값
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup THREAD
 * @code{c}
	void *ret_val = NULL;

	ret = r_thread_join(thread_id, (void **)&ret_val);
 * @endcode
 */
extern r_ret_t r_thread_join(r_thread_t id, void **thread_ret);

/**
 * @brief 부모의 Thread 에서 분리 한다.
 * @param[in] id Thread ID
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup THREAD
 * @code{c}
	ret = r_thread_detach(thread_id);
 * @endcode
 */
extern r_ret_t r_thread_detach(r_thread_t id);

/**
 * @brief 현재 호출 시점의 Thread ID 를 구한다.
 * @return Thread ID
 * @ingroup THREAD
 * @code{c}
	r_thread_t t_id = 0;

	t_id = r_thread_self();
 * @endcode
 */
extern r_thread_t r_thread_self(void);

/**
 * @brief Thread 를 종료 한다.
 * @param[in] exit_val Thread 종료시 전달되는 return 값
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup THREAD
 * @code
	void *thread_func(void *param)
	{
		s8 *data = (s8 *)param;
		s16 retVal = 0;

		R_DBG_INFO("%s\n", data);

		ret = r_thread_exit((void *)&retVal);
	}

	r_ret_t func_thread_exam(void)
	{
		ret = r_thread_create(&thread_id, thread_func, (void *)"thread func");
	}
 * @endcode
 */
extern r_ret_t r_thread_exit(void *exit_val);

#endif /* R_THREAD_H */
