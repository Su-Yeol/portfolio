#ifndef R_MUTEX_H
#define R_MUTEX_H

/////////
// Header
#include "r_type.h"

///////////////////
// Macro definition

/**
 * @brief r_mutex_init 을 실행 하지 않고도 r_mutex_t 정보 변수를 초기화 한다.\n
 * 단, r_mutex_init 을 이미 수행 한 r_mutex_t 정보 변수에는 사용 할 수 없다.
 * @ingroup MUTEX
 */
#define R_MUTEX_INITIALIZER						{ 0, NULL }

//////////////////
// Type definition

/**
 * @enum r_mutex_init_t r_mutex.h
 * @brief Mutex 가 초기화 되어 있는지 여부
 * @ingroup MUTEX
 */
typedef enum
{
	R_MUTEX_NONE = 0, /**< Mutex 가 초기화 되지 않은 상태 */
	R_MUTEX_INIT = 1, /**< Mutex 가 초기화 된 상태 */
} r_mutex_init_t;

/**
 * @struct r_mutex_t r_mutex.h
 * @brief Mutex 의 정보를 저장
 * @ingroup MUTEX
 */
typedef struct
{
	r_mutex_init_t init; /**< Mutex 의 초기화 상태 */
	void *mutex; /**< Mutex 정보 저장 */
} r_mutex_t;

/////////////////
// Extern variable

//////////////////
// Extern function

// Mutex

/**
 * @brief Mutex 를 생성 및 초기화 한다.
 * @param[in] mutex Mutex 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup MUTEX
 * @code
	r_mutex_t mutex;
	r_ret_t ret = R_SUCCESS;

	ret = r_mutex_init(&mutex);
 * @endcode
 */
extern r_ret_t r_mutex_init(r_mutex_t *mutex);

/**
 * @brief Mutex 를 Lock 시킨다.
 * @param[in] mutex Mutex 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup MUTEX
 * @code
	ret = r_mutex_lock(&mutex);
 * @endcode
 */
extern r_ret_t r_mutex_lock(r_mutex_t *mutex);

/**
 * @brief Mutex 를 Lock 시도 후 이미 lock 상태이면 대기 하지 않고 곧바로 리턴 시킨다.
 * @param[in] mutex Mutex 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup MUTEX
 * @code
	ret = r_mutex_trylock(&mutex);
 * @endcode
 */
extern r_ret_t r_mutex_trylock(r_mutex_t *mutex);

/**
 * @brief Mutex 를 Unlock 시킨다.
 * @param[in] mutex Mutex 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup MUTEX
 * @code
	ret = r_mutex_unlock(&mutex);
 * @endcode
 */
extern r_ret_t r_mutex_unlock(r_mutex_t *mutex);

/**
 * @brief Mutex 를 제거 한다.
 * @param[in] mutex Mutex 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup MUTEX
 * @code
	ret = r_mutex_destroy(&mutex);
 * @endcode
 */
extern r_ret_t r_mutex_destroy(r_mutex_t *mutex);

#endif /* R_MUTEX_H */
