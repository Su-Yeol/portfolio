#ifndef R_DEBUG_H
#define R_DEBUG_H

/////////////////
// Header include

///////////////////
// MACRO definition

/**
 * @def R_DEBUG
 * @brief 디버깅을 위한 로그 출력 설정
 * @ingroup DEBUG
 */
#define R_DEBUG

/**
 * @def R_DBG_ERR
 * @brief 에러 로그 출력
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 * @ingroup DEBUG
 * @code{c}
 R_DBG_ERR("Error Message [%d]\n", ret);
 * @endcode
 */
#define R_DBG_ERR(fmt, ...)	\
						do { \
							if (r_debug_level & R_DBG_LEVEL_ERR) { \
								r_dbg_level_printf(R_DBG_LEVEL_ERR, NULL, __func__, __LINE__, fmt, ##__VA_ARGS__); \
							} \
						} while(0)

/**
 * @def R_DBG_INFO
 * @brief R_DBG_INFO 설정시 디버깅 로그 출력
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 * @ingroup DEBUG
 * @code{c}
 R_DBG_INFO("Debug Message [%d]\n", ret);
 * @endcode
 */
#ifdef R_DEBUG
#define R_DBG_INFO(fmt, ...) \
						do { \
							if (r_debug_level & R_DBG_LEVEL_INFO) { \
								r_dbg_level_printf(R_DBG_LEVEL_INFO, NULL, __func__, __LINE__, fmt, ##__VA_ARGS__); \
							} \
						} while(0)
#else
#define R_DBG_INFO(...)							do { } while(0)
#endif

/**
 * @def R_DBG_INFO_HEX
 * @brief R_DBG_LEVEL_DBG 설정에서 HEXBIN 과 디버깅 로그 출력
 * @param[in] hexbin Binary 데이터가 저장된 버퍼
 * @param[in] hexbin_len hexbin 의 크기 (Bytes)
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 * @ingroup DEBUG
 */
#ifdef R_DEBUG
#define R_DBG_INFO_HEX(hexbin, hexbin_len, fmt, ...)	\
						do { \
							if (r_debug_level & R_DBG_LEVEL_INFO) { \
								r_dbg_level_printf(R_DBG_LEVEL_INFO, NULL, __func__, __LINE__, fmt, ##__VA_ARGS__); \
								r_dbg_hex_print(hexbin, hexbin_len); \
								r_dbg_printf("\n"); \
							} \
						} while(0)
#else
#define R_DBG_INFO_HEX(...)						do { } while(0)
#endif

/**
 * @def R_DBG_DBG
 * @brief R_DBG_LEVEL_DBG 설정시 디버깅 로그 출력
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 * @ingroup DEBUG
 */
#ifdef R_DEBUG
#define R_DBG_DBG(fmt, ...)	\
						do { \
							if (r_debug_level & R_DBG_LEVEL_DBG) { \
								r_dbg_level_printf(R_DBG_LEVEL_DBG, NULL, __func__, __LINE__, fmt, ##__VA_ARGS__); \
							} \
						} while(0)
#else
#define R_DBG_DBG(...)							do { } while(0)
#endif

/**
 * @def R_DBG_DBG_HEX
 * @brief R_DBG_LEVEL_DBG 설정에서 HEXBIN 과 디버깅 로그 출력
 * @param[in] hexbin Binary 데이터가 저장된 버퍼
 * @param[in] hexbin_len hexbin 의 크기 (Bytes)
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 * @ingroup DEBUG
 */
#ifdef R_DEBUG
#define R_DBG_DBG_HEX(hexbin, hexbin_len, fmt, ...)	\
						do { \
							if (r_debug_level & R_DBG_LEVEL_DBG) { \
								r_dbg_level_printf(R_DBG_LEVEL_DBG, NULL, __func__, __LINE__, fmt, ##__VA_ARGS__); \
								r_dbg_hex_print(hexbin, hexbin_len); \
								r_dbg_printf("\n"); \
							} \
						} while(0)
#else
#define R_DBG_DBG_HEX(...)						do { } while(0)
#endif

/**
 * 모든 Debug Level 선언값
 * @ingroup DEBUG
 */
#define R_DBG_LEVEL_ALL							(R_DBG_LEVEL_INFO | R_DBG_LEVEL_DBG | R_DBG_LEVEL_ERR)

//////////////////
// Type definition

/**
 * @enum r_dbg_level_t r_debug.h
 * 디버그 메시지 출력을 위한 디버그 레벨
 * @ingroup DEBUG
 */
typedef enum
{
	R_DBG_LEVEL_NONE = 0x0,						//!< 모든 디버그 메시지 출력 제거
	R_DBG_LEVEL_INFO = 0x1,							//!< 정보 메시지 출력
	R_DBG_LEVEL_DBG = 0x2,						//!< 디버그 메시지 출력
	R_DBG_LEVEL_ERR = 0x10,						//!< 에러 메시지 출력, 기본값
} r_dbg_level_t;

//////////////////
// Global variable

/**
 * RVP 의 debug level
 */
extern u32 r_debug_level;

//////////////////
// Global function

/**
 * 디버그 메시지 출력 레벨을 설정 한다.
 * @param[in] dbg_level 디버그 메시지 레벨 (@ref r_dbg_level_t)
 * @return @ref R_SUCCESS : 성공\n
 * @ref R_FAIL : 실패
 * @ingroup DEBUG
 */
extern r_ret_t r_dbg_set_level(u32 dbg_level);

/**
 * 표준 함수인 printf 과 동일한 형식으로 디버그 메시지를 출력 한다.\n
 * 이 함수를 직접 사용 하지 말고, R_DBG_ERR, R_DBG_INFO, R_DBG_DBG 등의 매크로를 이용 하여야 한다.
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 */
extern void r_dbg_printf(const char *fmt, ...);

/**
 * 설정한 level 의 디버그 메시지 출력 함수\n
 * 이 함수를 직접 사용 하지 말고, R_DBG_ERR, R_DBG_INFO, R_DBG_DBG 등의 매크로를 이용 하여야 한다.
 * @param[in] dbg_level 디버그 메시지 레벨 (@ref r_dbg_level_t)
 * @param[in] hdr 디버그 메시지의 해더에 출력할 문자열
 * @param[in] func 디버그 메시지가 삽입된 함수명
 * @param[in] line 디버그 메시지가 삽입된 라인
 * @param[in] fmt 표준 함수인 printf 과 동일한 출력 형식
 */
extern void r_dbg_level_printf(u32 dbg_level, const char *hdr, const char *func, s32 line,
            const char *fmt, ...);

/**
 * HEX Bin 과 디버그 메시지 출력\n
 * 이 함수를 직접 사용 하지 말고, R_DBG_INFO_HEX, R_DBG_DBG_HEX 매크로를 이용 하여야 한다.
 * @param[in] hexbin Binary 데이터가 저장된 버퍼
 * @param[in] hexbin_len hexbin 의 크기 (Bytes)
 */
extern void r_dbg_hex_print(const u8 *hexbin, s32 hexbin_len);

#endif /* R_DEBUG_H */

