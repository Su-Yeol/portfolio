#ifndef R_THREAD_TIMER_H
#define R_THREAD_TIMER_H

/////////
// Header
#include <time.h>

///////////////////
// Macro definition

/**
 * @def R_THREAD_TIMER_REPEAT_MAX
 * @brief Thread Timer 의 최대 반복 회수
 * @ingroup THREAD_TIMER
 */
#define R_THREAD_TIMER_REPEAT_MAX				(100000)

/**
 * @def R_THREAD_TIMER_REPEAT_ON
 * @brief Thread Timer 의 반복 설정
 * @ingroup THREAD_TIMER
 */
#define R_THREAD_TIMER_REPEAT_ON				(-1)

/**
 * @def R_THREAD_TIMER_REPEAT_ONCE
 * @brief Thread Timer 가 1회만 수행 하도록 설정
 * @ingroup THREAD_TIMER
 */
#define R_THREAD_TIMER_REPEAT_ONCE				(1)

//////////////////
// Type definition

/**
 * @brief Thread 로 수행될 Timer Handler 함수
 * @param[in] param Handler가 수행 될때 전달이 되는 Argument
 * @ingroup THREAD_TIMER
 */
typedef void (*r_thread_timer_func_t)(void *param);

/**
 * @struct r_thread_timer_t r_thread_timer.h
 * @brief 생성된 Thread Timer 의 정보 저장을 위한 구조체
 *
 * Timer Start/Stop 함수 호출 전에 id를 제외한 나머지 변수 들을 반드시 설정 해야 함.
 * @ingroup THREAD_TIMER
 */
typedef struct
{

	timer_t id;									//!< Timer 의 고유한 id

	u32 ms;									//!< Timer 반복 주기\n 단위 : Milliseconds
	s32 repeat;			//!< Timer 반복 회수\n 최대값 : @ref R_THREAD_TIMER_REPEAT_MAX
	r_thread_timer_func_t func;					//!< handler 함수 포인터
	void *param;							//!< Handler 에 전달 되는 Parameter 의 포인터

	void *tdata;								//!< System timer 정보
} r_thread_timer_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @brief Thread Timer 를 생성 한 후 Timer 동작을 실행 한다.
 * @param[in] timer Thread Timer 의 정보
 * @param[in] ms Timer 구동 시간 (Milliseconds)
 * @param[in] repeat Timer 반복 회수\n 최대값 : @ref R_THREAD_TIMER_REPEAT_MAX
 * @param[in] func ms 시간 후 실행 할 Callback 함수 포인터
 * @param[in] param Callback 함수에 전달할 Parameter 포인터
 * @return @ref r_ret_t 타입의 실행 결과
 * @ingroup THREAD_TIMER
 * @code{c}

	void *thread_func(void *param)
	{
		R_DBG_INFO("thread func\n");
	}

	r_ret_t func_tt_start(void)
	{
		r_ret_t ret = R_INVALID_PARAMETER;
		r_thread_timer_t tx_timer = { 0 };
		u32 ms = 500;
		s32 repeat = R_THREAD_TIMER_REPEAT_MAX;

		ret = r_thread_timer_start(&tx_timer, ms, repeat, (void *)thread_func, NULL);
	}
 * @endcode
 */
extern r_ret_t r_thread_timer_start(r_thread_timer_t *timer, u32 ms, s32 repeat,
            r_thread_timer_func_t func, void *param);
/**
 * @brief Thread Timer 를 종료 한다.
 * @param[in] timer 생성된 Timer 정보
 * @return @ref r_ret_t 타입의 실행 결과
 * @ingroup THREAD_TIMER
 * @code{c}
	ret = r_thread_timer_stop(&tx_timer);
 * @endcode
 */
extern r_ret_t r_thread_timer_stop(r_thread_timer_t *timer);

/**
 * @brief Timer 가 현재 동작 중인지 확인 한다.
 * @param[in] timer 생성된 Timer 정보
 * @return @ref R_SUCCESS : 동작 중 \n @ref R_FAIL : 동작 안함
 * @ingroup THREAD_TIMER
 * @code{c}
	ret = r_thread_timer_pending(&tx_timer);
	if (ret == R_SUCCESS) {
		// thread tiemr is working.
	}
 * @endcode
 */
extern r_ret_t r_thread_timer_pending(r_thread_timer_t *timer);

#endif /* THREAD_TIMER_H */
