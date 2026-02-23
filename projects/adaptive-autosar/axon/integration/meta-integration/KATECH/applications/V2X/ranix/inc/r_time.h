#ifndef R_TIME_H
#define R_TIME_H

/////////
// Header

///////////////////
// Macro definition

/**
 * 시간 측정을 위한 Timer
 */
#ifdef R_DEBUG
#define TIMER_DEF()								static struct timeval timer_start, timer_now;
#define TIMER_START()							(void) gettimeofday(&timer_start, NULL)
#define TIMER_STOP()							(void) gettimeofday(&timer_now, NULL)
#define TIMER_ELLAPSED()						(R_TIMEVAL_TO_TIME64(timer_now) - R_TIMEVAL_TO_TIME64(timer_start))
#define TIMER_PRINT(str)						R_DBG_DBG("[%09.06f]:%s\n", \
												TIMER_ELLAPSED() / (R_ONE_SECOND_TO_USEC * 1.0), \
												 str)

#else
#define TIMER_DEF()
#define TIMER_START()
#define TIMER_STOP()
#define TIMER_ELLAPSED()
#define TIMER_STOP_PRINT()
#endif

/**
 * @def R_IEEE16092_START_TIME_US
 * @brief 미국(US)의 IEEE 1609.2 기준 시간(UTC)\n
 * UTC 기준 시간 "2004/01/01 00:00:00"\n
 * KST 기준 시간 "2004/01/01 09:00:00"\n
 * EST 기준 시간 "2003/12/31 19:00:00"
 * @ingroup TIME
 */
#define R_IEEE16092_START_TIME_US				"2004/01/01 00:00:00"

/**
 * @def R_IEEE16092_CERT_START_TIME_US
 * @brief 미국(US)의 IEEE 1609.2 인증서 기준 시간(UTC)\n
 * i Value 시작 시간이다. Global i = 0\n
 * UTC 기준 시간 "2015/01/06 09:00:00"\n
 * KST 기준 시간 "2015/01/06 18:00:00"\n
 * EST 기준 시간 "2015/01/06 04:00:00"
 * @ingroup TIME
 */
#define R_IEEE16092_CERT_START_TIME_US			"2015/01/06 09:00:00"

/**
 * @def R_IEEE16092_START_TIME_KR
 * @brief 한국(KR)의 IEEE 1609.2 기준 시간(UTC)\n
 * i Value 시작 시간이다. Global i = 0\n
 * UTC 기준 시간 "2019/10/05 17:00:00"\n
 * KST 기준 시간 "2019/10/06 02:00:00"\n
 * EST 기준 시간 "2019/10/05 13:00:00"
 * @ingroup TIME
 */
#define R_IEEE16092_START_TIME_KR				"2019/10/05 17:00:00"

////////////////////////////////////////////////////////////////////////////////

/**
 * @def R_YDT_START_TIME_CN
 * @brief 중국 C-V2X 규격 GB/T 37374-2019, YD/T 3957-2021 기준 시간(UTC)\n
 * UTC 기준 시간 "2004/01/01 00:00:00"\n
 * KST 기준 시간 "2004/01/01 09:00:00"\n
 * CST 기준 시간 "2004/01/01 08:00:00"
 * @ingroup TIME
 */
#define R_YDT_START_TIME_CN						"2004/01/01 00:00:00"

/**
 * @def R_YDT_START_TIME_CN_YDT
 * @brief 중국 C-V2X 규격 YD/T 3957-2021 인증서 기준 시간(UTC)\n
 * UTC 기준 시간 "2020/01/06 20:00:00"\n
 * KST 기준 시간 "2020/01/07 05:00:00"\n
 * CST 기준 시간 "2020/01/07 04:00:00"
 * @ingroup TIME
 */
#define R_YDT_CERT_START_TIME_CN				"2020/01/06 20:00:00"

////////////////////////////////////////////////////////////////////////////////

/**
 * @def R_ONE_MILLISECOND_TO_USEC
 * @brief 1초를 Milliseconds 로 변경한 값\n
 * 값 : 1000\n
 * 형식 : ssssssssssssssmmm
 * @ingroup TIME
 */
#define R_ONE_MILLISECOND_TO_USEC				(1000)

/**
 * @def R_ONE_SECOND_TO_USEC
 * @brief 1초를 Microseconds 로 변경한 값\n
 * 값 : 1,000,000\n
 * 형식 : ssssssssssssssmmmuuu
 * @ingroup TIME
 */
#define R_ONE_SECOND_TO_USEC					(1000000)

/**
 * @def R_ONE_MINUTE_TO_USEC
 * @brief 1분을 Microseconds 로 변경한 값\n
 * 값 : 60,000,000\n
 * 형식 : ssssssssssssssmmmuuu
 * @ingroup TIME
 */
#define R_ONE_MINUTE_TO_USEC					((time64_t)R_ONE_SECOND_TO_USEC * (60))

/**
 * @def R_ONE_HOUR_TO_USEC
 * @brief 1시간을 Microseconds 로 변경한 값\n
 * 값 : 3,600,000,000\n
 * 형식 : ssssssssssssssmmmuuu
 * @ingroup TIME
 */
#define R_ONE_HOUR_TO_USEC						(R_ONE_MINUTE_TO_USEC * (60))

/**
 * @def R_ONE_YEAR_TO_USEC
 * @brief 1년을 Microseconds 로 변경한 값\n
 * 값 : 31,536,000,000,000\n
 * 형식 : ssssssssssssssmmmuuu
 * @ingroup TIME
 */
#define R_ONE_YEAR_TO_USEC						(R_ONE_HOUR_TO_USEC * (24) * (365))

/**
 * @def R_USECS_TO_TIME64
 * @brief Microseconds 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] usecs Microsecond 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * mmmuuu -> TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_USECS_TO_TIME64(usecs)				((time64_t)(usecs))

/**
 * @def R_MSECS_TO_TIME64
 * @brief Milliseconds 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] msecs Millisecond 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * mmm -> TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_MSECS_TO_TIME64(msecs)				((time64_t)((msecs) * (time64_t)R_ONE_MILLISECOND_TO_USEC))

/**
 * @def R_SECS_TO_TIME64
 * @brief 초 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] secs 초(Second) 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * ssssssssssssss -> TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_SECS_TO_TIME64(secs)					((time64_t)((secs) * (time64_t)R_ONE_SECOND_TO_USEC))

/**
 * @def R_MINUTE_TO_TIME64
 * @brief 분 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] min 분(Minute) 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * minute ->  TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_MINUTE_TO_TIME64(min)					((time64_t)((min) * (time64_t)R_ONE_MINUTE_TO_USEC))

/**
 * @def R_HOUR_TO_TIME64
 * @brief 시간 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] hour 시(Hour) 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * hour ->  TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_HOUR_TO_TIME64(hour)					((time64_t)((hour) * (time64_t)R_ONE_HOUR_TO_USEC))

/**
 * @def R_YEAR_TO_TIME64
 * @brief 년 까지만 있는 시간 형식을 time64_t 형식으로 변환
 * @param[in] year 년(Year) 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * year ->  TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_YEAR_TO_TIME64(year)					((time64_t)((year) * (time64_t)R_ONE_YEAR_TO_USEC))

/**
 * @def R_TIMEVAL_TO_TIME64
 * @brief struct timeval 의 시간 정보를 time64_t 의 정보로 변경
 * @param[in] tv Unix(POSIX) struct timeval 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * 형식 : ssssssssssssssmmmuuu
 * @ingroup TIME
 */
#define R_TIMEVAL_TO_TIME64(tv)					((time64_t)((tv).tv_sec) * (time64_t)R_ONE_SECOND_TO_USEC + (time64_t)((tv).tv_usec))

/**
 * @def R_TIME64_TO_TIMEVAL
 * @brief @ref time64_t 형식의 시간 정보를 Unix(POSIX) struct timeval 의 형식으로 변환 한다.
 * @param[in] time64 시간 정보
 * @param[out] tv Unix(POSIX)의 struct timeval 변수
 * @ingroup TIME
 */
#define R_TIME64_TO_TIMEVAL(time64, tv)			{ \
							(tv).tv_sec = R_TIME64_TO_SECS(time64); \
							(tv).tv_usec = R_TIME64_TO_USECS(time64); \
												}

/**
 * @def R_TIME64_TO_SECS
 * @brief @ref time64_t 형식의 시간 중 초만 구한다.
 * @param[in] time64 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * TIME64(ssssssssssssssmmmuuu) -> ssssssssssssss
 * @ingroup TIME
 */
#define R_TIME64_TO_SECS(time64)				((time64) / (time64_t)R_ONE_SECOND_TO_USEC)

/**
 * @def R_TIME64_TO_USECS
 * @brief @ref time64_t 형식의 시간 중 Milliseconds 와 Microseconds 부분만 구한다.
 * @param[in] time64 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * TIME64(ssssssssssssssmmmuuu) -> mmmuuu
 * @ingroup TIME
 */
#define R_TIME64_TO_USECS(time64)				((time64) - ((time64_t)R_TIME64_TO_SECS(time64) * (time64_t)R_ONE_SECOND_TO_USEC))

///////////////////////////////////////////////////////////////////////

////////////////////
// TCI Standard Time

/**
 * @def R_TIME_TCI_TO_UNIX
 * @brief TCI 시간 형식을 @ref time64_t 형식의 Unix 기준의 시간으로 변환 한다.
 * @param[in] tci_time TCI 시간 정보
 * @return @ref time64_t 형식의 시간 정보\n
 * TCI 시간(ssssssssssssssmmm) -> TIME64(ssssssssssssssmmmuuu)
 * @ingroup TIME
 */
#define R_TIME_TCI_TO_UNIX(tci_time)			((time64_t)(tci_time * (time64_t)1000))

/**
 * @def R_TIME_UNIX_TO_TCI
 * @brief @ref time64_t 형식의 Unix 기준의 시간을 TCI 시간 형식으로 변환
 * @param[in] unix_time 시간 정보
 * @return TCI 형식의 시간 정보\n
 * TIME64(ssssssssssssssmmmuuu) -> TCI Time(ssssssssssssssmmm)
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_TCI(unix_time)			(unix_time / (time64_t)1000)

///////////////////////////////
// IEEE 1609.2 US Standard Time

/**
 * @def R_TIME_UNIX_TO_16092_US
 * @brief Unix 시간을 IEEE 1609.2 US 표준시 기준으로 변환 한다.
 * @param[in] unix_time Unix 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_16092_US(unix_time) \
					r_time_unix_to_custom_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_START_TIME_US), \
					unix_time)

/**
 * @def R_TIME_16092_TO_UNIX_US
 * @brief IEEE 1609.2 US 표준시 기준의 입력 시간을 Unix 기준의 시간으로 변환 한다.
 * @param[in] time_16092 IEEE 1609.2 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_16092_TO_UNIX_US(time_16092)	 \
					r_time_custom_to_unix_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_START_TIME_US), \
					time_16092)

///////////////////////////////////////////
// IEEE 1609.2 Certificate US Standard Time

/**
 * @def R_TIME_UNIX_TO_16092_CERT_US
 * @brief Unix 시간을 IEEE 1609.2 인증서의 US 표준시 기준으로 변환 한다.
 * @param[in] unix_time Unix 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_16092_CERT_US(unix_time) \
					r_time_unix_to_custom_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_CERT_START_TIME_US), \
					unix_time)

/**
 * @def R_TIME_16092_CERT_TO_UNIX_US
 * @brief IEEE 1609.2 인증서의 US 표준시 기준의 입력 시간을 Unix 기준의 시간으로 변환 한다.
 * @param[in] time_16092_cert IEEE 1609.2 인증서 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_16092_CERT_TO_UNIX_US(time_16092_cert) \
					r_time_custom_to_unix_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_CERT_START_TIME_US), \
					time_16092_cert)

/**
 * @def R_TIME_UNIX_TO_16092_GLOBAL_I_US
 * @brief Unix 기준의 시간을 IEEE 1609.2 인증서의 US 표준시 기준의 Global i Value 를 구한다.
 * @param[in] unix_time Unix 시간 정보
 *
 * UTC 기준 시간 "2015/01/06 09:00:00"\n
 * KST 기준 시간 "2015/01/06 18:00:00"\n
 * EST 기준 시간 "2015/01/06 04:00:00"\n
 * -> Global i=0
 * @return @ref u32 형식의 Global i Value
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_16092_GLOBAL_I_US(unix_time) \
					r_time_get_i_value \
					(r_time_from_unix_to_custom(R_IEEE16092_CERT_START_TIME_US), \
					unix_time)

//////////////////////////////////
// IEEE 1609.2 Korea Standard Time

/**
 * @def R_TIME_UNIX_TO_16092_KR
 * @brief Unix 시간을 IEEE 1609.2 KR 표준시 기준으로 변환 한다.
 * @param[in] unix_time Unix 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_16092_KR(unix_time) \
					r_time_unix_to_custom_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_START_TIME_KR), \
					unix_time)

/**
 * @def R_TIME_16092_CERT_TO_KR
 * @brief IEEE 1609.2 KR 표준시 기준의 입력 시간을 Unix 기준의 시간으로 변환 한다.
 * @param[in] time_16092 IEEE 1609.2 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_16092_CERT_TO_KR(time_16092)	 \
					r_time_custom_to_unix_standard \
					(r_time_from_unix_to_custom(R_IEEE16092_START_TIME_KR), \
					time_16092)

/**
 * @def R_TIME_UNIX_TO_16092_GLOBAL_I_KR
 * @brief Unix 기준의 시간을 IEEE 1609.2 KR 표준시 기준의 Global i Value 를 구한다.
 * @param[in] unix_time Unix 시간 정보
 *
 * UTC 기준 시간 "2019/10/05 17:00:00"\n
 * KST 기준 시간 "2019/10/06 02:00:00"\n
 * EST 기준 시간 "2019/10/05 13:00:00"\n
 * -> Global i=0
 * @return @ref u32 형식의 Global i Value
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_16092_GLOBAL_I_KR(unix_time) \
					r_time_get_i_value \
					(r_time_from_unix_to_custom(R_IEEE16092_CERT_START_TIME_US), \
					unix_time)

/////////////////////////////////////
// YD/T 3957-2021 China Standard Time

/**
 * @def R_TIME_UNIX_TO_YDT_CN
 * @brief Unix 시간을 GB/T 37374, YD/T 3957-2021 표준시(UTC) 기준으로 변환 한다.
 * @param[in] unix_time Unix 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_YDT_CN(unix_time) \
					r_time_unix_to_custom_standard \
					(r_time_from_unix_to_custom(R_YDT_START_TIME_CN), \
					unix_time)

/**
 * @def R_TIME_YDT_CERT_TO_UNIX_CN
 * @brief YD/T 3957 인증서의 CN 표준시 기준의 입력 시간을 Unix 기준의 시간으로 변환 한다.
 * @param[in] time_ydt_cert YD/T 3957-2021 인증서 시간 정보\n
 * 형식 : @ref time64_t
 * @return @ref time64_t 형식의 시간 정보
 * @ingroup TIME
 */
#define R_TIME_YDT_CERT_TO_UNIX_CN(time_ydt_cert) \
					r_time_custom_to_unix_standard \
					(r_time_from_unix_to_custom(R_YDT_CERT_START_TIME_CN), \
					time_ydt_cert)

/**
 * @def R_TIME_UNIX_TO_YDT_GLOBAL_I_CN
 * @brief Unix 기준의 시간을 YD/T 3957-2021 표준시(UTC) 기준의 Global i Value 를 구한다.
 * @param[in] unix_time Unix 시간 정보
 *
 * UTC 기준 시간 "2020/01/06 20:00:00"\n
 * KST 기준 시간 "2020/01/07 05:00:00"\n
 * CST 기준 시간 "2020/01/07 04:00:00"
 * -> Global i=0
 * @return @ref u32 형식의 Global i Value
 * @ingroup TIME
 */
#define R_TIME_UNIX_TO_YDT_GLOBAL_I_CN(unix_time) \
					r_time_get_i_value \
					(r_time_from_unix_to_custom(R_YDT_CERT_START_TIME_CN), \
					unix_time)

///////////////////////////////////////////////////////////////////////

//////////////////
// Type definition

/////////////////
// Extern variable

//////////////////
// Extern function

////////////
// UNIX Time

/**
 * @brief System 시간을 설정 한다.\n
 * Unix 기준 System 시간은 UTC 1970/01/01 00:00:00 기준이다.
 * @param[in] unix_time : ssssssssssssssmmmuuu 형식의 시간 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup TIME
 * @code{c}
	r_ret_t ret = R_SUCCESS;
	time64_t nowtime64 = 0;

	nowtime64 = r_time_get_system_time64();
	ret = r_time_set_system_time64(nowtime64);
 * @endcode
 */
extern r_ret_t r_time_set_system_time64(time64_t unix_time);

/**
 * @brief 현재의 System 시간을 가져 온다.\n
 * Unix 기준 System 시간은 1970/01/01 00:00:00 기준이다.
 * @return unix_time : ssssssssssssssmmmuuu 형식의 시간 정보
 * @ingroup TIME
 * @code{c}
	time64_t systemtime = 0;

	systemtime = r_time_get_system_time64();
 * @endcode
 */
extern time64_t r_time_get_system_time64(void);

////////////////////
// 기준 시간 처리 함수

/**
 * @brief Unix 기준 시간 부터 Custom_time(UTC) 까지의 초를 계산 한다.\n
 * Unix 기준 (UTC) : 1970/01/01 00:00:00 부터 시작
 * @param[in] custom_standard Custom Standard Time\n
 *  ex) IEEE 1609.2 의 시작 시간 : @ref R_IEEE16092_START_TIME_US \n
 *      1970/01/01 00:00:00 ~ 2004/01/01 00:00:00(1609.2 기준 시간) 의 초
 * @return 초(Seconds)
 * @ingroup TIME
 * @code{c}
	time64_t Dot2timeinUTC = 0;

	Dot2timeinUTC = r_time_from_unix_to_custom(R_IEEE16092_START_TIME_US);
 * @endcode
 */
extern time64_t r_time_from_unix_to_custom(const s8 *custom_standard);

/**
 * @brief Unix 기준의 입력 시간을 Custom 기준의 시간(UTC)으로 변환 한다.\n
 * Unix 기준 시간 : UTC 1970/01/01 00:00:00 부터 시작
 * @param[in] custom_standard Custom Standard Time
 * @param[in] unix_time Unix 기준의 입력 시간 정보
 * @return Custom 기준으로 변환된 시간(Seconds)
 * @ingroup TIME
 * @code{c}
	time64_t nowtimeinDot2 = 0;

	nowtimeinDot2 = r_time_unix_to_custom_standard(Dot2timeinUTC, nowtime64);
 * @endcode
 */
extern time64_t r_time_unix_to_custom_standard(time64_t custom_standard, time64_t unix_time);

/**
 * @brief Custom 기준의 입력 시간(UTC)을 Unix 기준의 시간으로 변환 한다.\n
 * Unix 기준 시간 : UTC 1970/01/01 00:00:00 부터 시작
 * @param[in] custom_standard Custom Standard Time
 * @param[in] custom_time Custom 기준의 입력 시간 정보
 * @return Unix 기준의 시간 정보
 * @ingroup TIME
 * @code{c}
	time64_t nowtimeinUTC = 0;

	nowtimeinUTC = r_time_custom_to_unix_standard(Dot2timeinUTC, nowtimeinDot2);
 * @endcode
 */
extern time64_t r_time_custom_to_unix_standard(time64_t custom_standard, time64_t custom_time);

/**
 * @brief Custom 기준 시간(UTC) 부터 Custom 날짜 까지의 i Value 를 구한다.
 * @param[in] custom_standard Custom Standard Time
 * @param[in] custom_time Custom 기준의 입력 시간 정보
 * @return Global i Value
 * @ingroup TIME
 * @code{c}
	u32 week_count = 0;

	week_count = r_time_get_i_value(Dot2timeinUTC, nowtime64);
 * @endcode
 */
extern s32 r_time_get_i_value(time64_t custom_standard, time64_t custom_time);

///////////////
// Time Utility

/**
 * usleep 을 수행 한다.
 * @param[in] usec sleep 시간 (microseconds)
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup TIME
 * @code{c}
	ret = r_time_usleep(R_ONE_SECOND_TO_USEC);
 * @endcode
 */
extern r_ret_t r_time_usleep(u32 usec);

#endif /* R_TIME_H */
