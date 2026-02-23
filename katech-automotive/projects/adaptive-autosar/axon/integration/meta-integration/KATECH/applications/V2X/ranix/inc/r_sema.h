#ifndef R_SEMA_H
#define R_SEMA_H

/////////
// Header
#include "r_type.h"
#include "r_time.h"

///////////////////
// Macro definition

//////////////////
// Type definition

/**
 * @struct r_sema_t r_sema.h
 * @brief Semaphore 의 정보를 저장
 * @ingroup SEMA
 */
typedef struct
{
	void *sema; /**< Semaphore 정보 저장 */
} r_sema_t;

/////////////////
// Extern variable

//////////////////
// Extern function

// 동일 Process 내의 Thread 간의 Semaphore

/**
 * @brief Semaphore 를 초기화 한다.
 * @param[in] sem Semaphore 정보가 저장된 구조체
 * @param[in] pshared 0 이 아닌 값이면 다른 Thread 들 간에 Semaphore 를 공유 할 수 있다.
 * @param[in] init_value Semaphore 가 생성 될때 초기 값
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code
	r_sema_t semaphore;
	r_ret_t ret = R_SUCCESS;

	ret = r_sema_init(&semaphore, 0, 1);
 * @endcode
 */
extern r_ret_t r_sema_init(r_sema_t *sem, s16 pshared, u16 init_value);

/**
 * @brief Semaphore 를 제거 한다.
 * @param[in] sem Semaphore 정보가 저장된 구조체
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code
	ret = r_sema_destroy(&semaphore);
 * @endcode
 */

extern r_ret_t r_sema_destroy(r_sema_t *sem);

/**
 * @brief r_sema_post 에 의해 LOCK 이 해제 될때 까지 기다린다.
 * @param[in] sem Semaphore 정보 저장
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code{c}
	ret = r_sema_wait(&semaphore);
 * @endcode
 */
extern r_ret_t r_sema_wait(r_sema_t *sem);

/**
 * @brief 현재 Semaphore 가 LOCK 이 되어 있지 않다면 LOCK 시키고 대기 한다.
 * @param[in] sem Semaphore 정보 저장
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code{c}
	ret = r_sema_trywait(&semaphore);
 * @endcode
 */
extern r_ret_t r_sema_trywait(r_sema_t *sem);

/**
 * @brief r_sema_post 에 의해 LOCK 이 해제 되거나 wait_time이 종료 될때 까지 기다린다.
 * @param[in] sem Semaphore 정보 저장
 * @param[in] usec 기다리는 시간(Microsecond)
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code{c}
	ret = r_sema_timed_wait(&semaphore, R_ONE_SECOND_TO_USEC);
 * @endcode
 */
extern r_ret_t r_sema_timed_wait(r_sema_t *sem, time64_t usec);

/**
 * @brief LOCK 상태인 Semaphore 를 해제 한다.
 * @param[in] sem Semaphore 정보 저장
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup SEMA
 * @code{c}
	ret = r_sema_post(&semaphore);
 * @endcode
 */
extern r_ret_t r_sema_post(r_sema_t *sem);

#endif /* R_SEMA_H */
