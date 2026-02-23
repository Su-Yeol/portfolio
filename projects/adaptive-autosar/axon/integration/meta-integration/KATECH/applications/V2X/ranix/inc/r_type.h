#ifndef R_TYPE_H
#define R_TYPE_H

/////////
// Header

///////////////////
// Macro definition

/**
 * @ingroup DATA_TYPE
 * @{
 */

#ifndef NULL
#define NULL									((void*) 0)
#endif

/**
 * 성공, 참
 */
#ifndef TRUE
#define TRUE									(1)
#endif

/**
 * 실패, 거짓
 */
#ifndef FALSE
#define FALSE									(0)
#endif

// Interface, IP and MAC Address
/**
 * Interface 이름의 최대 길이
 *
 * NULL로 끝나는 문자열\n
 * @ref s8 타입
 */
#define R_IF_NAME_MAX_LEN						(256) // string type

/**
 * MAC Address 의 최대 길이
 *
 * @ref u8 타입
 */
#define R_MAC_ADDR_MAX_LEN						(6) // u8 type

/**
 * IPv6 Address 의 최대 길이
 *
 * @ref u8 타입
 */
#define R_IP_V6_ADDR_MAX_LEN					(16) // u8 type

/**
 * IPv4 Address 의 최대 길이
 *
 * @ref u8 타입
 */
#define R_IP_V4_ADDR_MAX_LEN					(4) // u8 type

/**
 * 배열 형태의 MAC Address 를 MAC Address 표시 형태로 변환 하기 위한 중간 변환 매크로
 */
#define MAC_TO_STR(M)							(M)[0], (M)[1], (M)[2], (M)[3], (M)[4], (M)[5]

/**
 * 중간 변환된 MAC Address 를 MAC Addres 의 문자열 표시 형태로 변환 하기 위한 포멧
 */
#define MAC_STR									"%02X:%02X:%02X:%02X:%02X:%02X"

/**
 * 배열 형태의 IPv4 Address 를 IPv4 Address 표시 형태로 변환 하기 위한 중간 변환 매크로
 */
#define IP_V4_TO_STR(I)							(I)[0], (I)[1], (I)[2], (I)[3]

/**
 * 중간 변환된 IPv4 Address 를 IPv4 Addres 의 문자열 표시 형태로 변환 하기 위한 포멧
 */
#define IP_V4_STR								"%d.%d.%d.%d"

/**
 * 배열 형태의 IPv6 Address 를 IPv6 Address 표시 형태로 변환 하기 위한 중간 변환 매크로
 */
#define IP_V6_TO_STR(I)							(I)[0], (I)[1], (I)[2], (I)[3], (I)[4], (I)[5], (I)[6], (I)[7], (I)[8], (I)[9], (I)[10], (I)[11], (I)[12], (I)[13], (I)[14], (I)[15]

/**
 * 중간 변환된 IPv6 Address 를 IPv6 Addres 의 문자열 표시 형태로 변환 하기 위한 포멧
 */
#define IP_V6_STR								"%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X"

/**
 * BYTE 타입 연산시 한번에 처리 가능한 단위
 */
#define BYTE_BUFF_BITS							(8) // sizeof(BYTE_BUFF_T)
//#define BYTE_BUFF_BITS						(32) // sizeof(BYTE_BUFF_T)

#if (BYTE_BUFF_BITS==8)

/**
 * Big Number 의 최대 자리수
 */
#define BYTE_LEN								(32)
/**
 * BYTE 타입의 기본 Data Type
 */
#define BYTE_BUFF_T								u8
/**
 * BYTE 의 Double Data Type
 */
#define BYTE_BUFF_DOUBLE_T						u16

#elif (BYTE_BUFF_BITS==32)

#define BYTE_LEN								(8)
#define BYTE_BUFF_T								u32
#define BYTE_BUFF_DOUBLE_T						u64

#endif

/**
 * BYTE 타입에서 사용 하는 실제 Size
 */
#define BYTE_BUFF_LEN							(BYTE_LEN * 2)

/**
 * BYTE 타입을 문자열로 변환시 필요한 Size
 */
#define BYTE_TO_STR_LEN(l)						(l * ((BYTE_BUFF_BITS / 8) * 2))

/**
 * BYTE 타입 변수의 Index 자리의 포인터
 */
#define BYTE(byte, idx)							(byte).b[idx]

/**
 * 포인터 타입의 BYTE 타입 변수의 Index 자리의 포인터
 */
#define BYTEP(byte, idx)						((byte_t *)(byte))->b[idx]

/**
 * BYTE 타입 변수의 실제 저장 변수
 */
#define BYTEB(byte)								(byte).b

/**
 * 포인터 타입의 BYTE 타입 변수의 실제 저장 변수
 */
#define BYTEPB(byte)							(byte)->b

/**
 * BITMASK 설정시 사용하는 파라메터 설정\n
 * mask 변수의 index 를 u64 타입의 Bitmask 로 변환
 */
#define R_BITMASK(mask)							((u64)1 << (mask))

//////////////////
// Type definition

/**
 * @enum r_ret_t r_type.h
 * @brief 함수 실행 결과 값
 *
 * Output 을 Return 값으로 전달 하는 함수를 제외한 나머지 모든 함수는 r_ret_t 타입으로 Return 값을 전달 한다.
 */
typedef enum
{
	R_SUCCESS = 0,								//!< 성공, 동작 완료
	R_FAIL = -1, 								//!< 실패

	R_INVALID_PARAMETER = -2,					//!< 파라메터 오류
	R_SIZE_EXCEEDED = -3, 						//!< 크기 초과

	R_MEMORY_ALLOC_FAIL = -50,					//!< 메모리 할당 실패
	R_MEMORY_SIZE_ERROR = -51,					//!< 메모리 Size 에러

	R_NOT_FOUND = -100,							//!< 검색 실패
	R_NOT_SUPPORT = -101,						//!< 지원 하지 않음

	R_EXIST = -200,								//!< 이미 존재 함
	R_BUSY = -201,								//!< 장치가 사용 중임
	R_AGAIN = -202,								//!< 재시도 요청

	R_FULL = -300,								//!< 더 이상 저장 공간이 없음
	R_EMPTY = -301,								//!< 저장되어 있는 Data 가 없음

	R_INVALID_SPDU_VER = -401,					//!< 지원 하지 않는 SPDU 버전
	R_NO_SIGNATURE = -402,						//!< 서명이 포함되어 있지 않음
	R_NO_CERTIFICATE = -403,					//!< 인증서가 없음
	R_INVALID_CERTIFICATE_VER = -404,			//!< 지원하지 않는 인증서 버전
	R_REVOKED_CERTIFICATE = -405,				//!< 취소된(Revoked) 인증서가 포함 되어 있음
	R_INVALID_CERTIFICATE_CHAIN = -406,			//!< Certificate Chain 에 문제가 있음
	R_INVALID_HASH_ALGO = -407,					//!< 지원하지 않는 Hash algorithm
	R_NO_TBS_DATA_TIME = -408,					//!< TBS Data 에 Generation Time 정보가 없음
	R_NO_TBS_DATA_LOCATION = -409,				//!< TBS Data 에 Generation Location 정보가 없음
	R_EXPIRED_TBS_DATA = -410,					//!< TBS Data 의 유효 기간이 만료됨
	R_EXPIRED_CERTIFICATE = -411,				//!< 인증서의 유효 기간이 만료됨

	R_ERROR_MAX									//!< R_ERROR_MAX
} r_ret_t;

/**
 * @enum r_cmp_t r_type.h
 * @brief 비교 결과값
 */
typedef enum
{
	R_EQ = 0,	//!< 같다, =
	R_LT = -1,	//!< 작다, <
	R_GT = 1	//!< 크다, >
} r_cmp_t;

/**
 * @enum r_v2x_t r_type.h
 * @brief V2X 규격 정의
 */
typedef enum
{
	R_V2X_NONE = 0,
	R_V2X_IEEE = 1,								//!< IEEE 1609
	R_V2X_CN = 2,								//!< China C-V2X
	R_V2X_ETSI = 3,								//!< ETSI V2X

	R_V2X_MAX
} r_v2x_t;

/**
 * @enum r_country_t r_type.h
 * @brief 운용하고자 하는 국가 설정\n
 * 지역의 규격에 따라 i value 계산이 달라 질 수 있다.
 */
typedef enum
{
	R_COUNTRY_KR = 1,							//!< Korea
	R_COUNTRY_US,								//!< USA

	R_COUNTRY_CN,								//!< China

	R_COUNTRY_MAX								//!< R_COUNTRY_MAX
} r_country_t;

/**
 * @brief 부호 있는 8 bit 데이터 타입\n
 * 범위 : -128 ~ 127
 */
#ifndef s8
typedef __signed__ char s8;
#endif

/**
 * @brief 부호 없는 8 bit 데이터 타입\n
 * 범위 : 0 ~ 255(0xFF)
 */
#ifndef u8
typedef unsigned char u8;
#endif

/**
 * @brief 부호 있는 16 bit 데이터 타입\n
 * 범위 : -32,768 ~ 32,767
 */
#ifndef s16
typedef __signed__ short s16;
#endif

/**
 * @brief 부호 없는 16 bit 데이터 타입\n
 * 범위 : 0 ~ 65,535(0xFFFF)
 */
#ifndef u16
typedef unsigned short u16;
#endif

/**
 * @brief 부호 있는 32 bit 데이터 타입\n
 * 범위 : -2,147,483,648 ~ 2,147,483,647
 */
#ifndef s32
typedef __signed__ int s32;
#endif

/**
 * @brief 부호 없는 32 bit 데이터 타입\n
 * 범위 : 0 ~ 4,294,967,295(0xFFFFFFFF)
 */
#ifndef u32
typedef unsigned int u32;
#endif

/**
 * @brief 부호 있는 부동 소수점 32 bit 데이터 타입\n
 * 범위 : 1.175494351e-38 to 3.40282347e+38 (normalized values)
 */
#ifndef f32
typedef float f32;
#endif

/**
 * @brief 부호 있는 64 bit 데이터 타입\n
 * 범위 : –9,223,372,036,854,775,808 ~ 9,223,372,036,854,775,807
 */
#ifndef s64
typedef __signed__ long long s64;
#endif

/**
 * @brief 부호 없는 64 bit 데이터 타입\n
 * 범위 : 0 ~ 18,446,744,073,709,551,615(0xFFFFFFFFFFFFFFFF)
 */
#ifndef u64
typedef unsigned long long u64;
#endif

/**
 * @brief 부동 소수점 64 bit 데이터 타입\n
 * 범위 : 2.22507385850720138e-308 to 1.79769313486231571e+308 (normalized values)
 */
#ifndef d64
typedef double d64;
#endif

// /**
//  * @brief boolean 타입
//  */
// #ifndef bool
// typedef u8 bool;
// #endif

/**
 * @brief 64bit 시간 타입
 *
 * RW Framework 에서 사용하는 기본 Time 데이터 타입
 *
 * 형식 : ssssssssssssssmmmuuu
 */
#ifndef time64_t
typedef u64 time64_t;
#endif

/**
 * @struct byte_t r_type.h
 * @brief Big Number 연산을 위한 Data 저장 타입
 *
 * RW Framework 에서 사용하는 기본 Big Number 데이터 타입
 */
#ifndef byte_t
// byte_t x = { { 0xAA, 0xBB, 0xCC, 0xDD} }
// -> 저장 구조 = { { 0xDD, 0xCC, 0xBB, 0xAA, 0, 0, ...., 0} }
typedef struct
{
	BYTE_BUFF_T b[BYTE_BUFF_LEN];
} byte_t;

#endif

/**
 * 무작위 설정
 */
typedef enum
{
	R_SHUFFLE_ON, /**< 무작위, Shuffle */
	R_SHUFFLE_OFF, /**< 순서대로 처리 */
} r_shuffle_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @}
 */

#endif /* R_TYPE_H */
