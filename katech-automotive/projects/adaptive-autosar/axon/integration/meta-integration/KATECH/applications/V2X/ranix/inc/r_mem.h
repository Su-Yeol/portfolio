#ifndef R_MEM_H
#define R_MEM_H

extern "C" {
/////////
// Header

#include "r_type.h"

///////////////////
// Macro definition

/**
 * @brief 0(NULL) 으로 초기화된 메모리를 동적으로 할당 한다.
 * @param[in] mem_size 할당 하고자 하는 메모리 사이즈 (Bytes)
 * @ingroup MEMORY
 */
#define R_CALLOC(mem_size)						r_calloc(mem_size, sizeof(u8))

//////////////////
// Type definition

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * @brief 메모리를 동적으로 할당 한다.
 * @param[in] mem_size 할당 하고자 하는 메모리 사이즈 (Bytes)
 * @return 할당된 메모리의 포인터
 * @ingroup MEMORY
 * @code{c}
	#define BUFF_MAX		(1024)

	r_ret_t ret = R_SUCCESS;
	u8 *buf = NULL;

	buf = r_malloc(BUFF_MAX);
	...
	if (buf != NULL) {
		ret = r_free(buf);
	}
 * @endcode
 */
extern void* r_malloc(u32 mem_size);

/**
 * @brief (u8 *) 의 형태로 R_CALLOC 을 이용하여 Memory 를 동적으로 할당 한다.
 * @param[in] mem_size 할당 하고자 하는 메모리 사이즈 (Bytes)
 * @return 할당된 메모리의 포인터
 * @ingroup MEMORY
 * @code{c}
	buf = r_calloc_u8(BUFF_MAX);
	...
	if (buf != NULL) {
		ret = r_free(buf);
	}
 * @endcode
 */
extern u8* r_calloc_u8(u32 mem_size);

/**
 * @brief 0(NULL) 으로 초기화 된 num * size 크기의 Memory 를 동적으로 할당 한다.\n
 * ex) r_calloc(10, sizeof(u8)) => 10 * 1 = 10 bytes
 * @param[in] num 할당 하고자 하는 size 의 개수
 * @param[in] size 할당 하고자 하는 메모리의 기본 size (Bytes)
 * @return 할당된 메모리의 포인터
 * @ingroup MEMORY
 * @code
	buf = r_calloc(BUFF_MAX, sizeof(u8));
	...
	if (buf != NULL) {
		ret = r_free(buf);
	}
 * @endcode
 */
extern void* r_calloc(u32 num, u32 size);

/**
 * @brief 새로운 mem_size 의 크기로 메모리를 할당 하고,\n
 * 기존에 할당되어 있던 mem_ptr 의 내용을 복사하여 그 포인터를 리턴 한다.
 * @param[in] mem_ptr 기존 할당 되어 있던 memory pointer\n
 * NULL 이면, r_malloc 과 같은 동작을 하게 된다.
 * @param[in] mem_size 할당 하고자 하는 메모리 사이즈 (Bytes)\n
 * 0 이면, r_free 와 같은 동작을 하게 된다.
 * @return 성공 : 할당된 새로운 메모리의 포인터\n실패 : NULL\n
 * 단, mem_size == 0 인 경우, mem_ptr 메모리 해제 후 NULL 리턴
 * @ingroup MEMORY
 * @code{c}
	#define FIRST_SIZE      (16)
	#define SECOND_SIZE     (32)

	buf = r_malloc(FIRST_SIZE);
	...
	if (buf != NULL) {
		buf = (u8*) r_realloc(buf, SECOND_SIZE);
	}
	...
	ret = r_free(buf);
 * @endcode
 */
extern void* r_realloc(void *mem_ptr, u32 mem_size);

/**
 * @brief mem_size 의 크기 만큼 메모리를 할당 하여 할당된 메모리에 mem_ptr 의 데이터를 mem_size 만큼 복사한 후 그 포인터를 리턴 한다.
 * @param[in] mem_ptr 데이터가 저장된 메모리의 포인터
 * @param[in] mem_size 복사하고자 하는 mem_ptr 의 크기 (Bytes)
 * @return 성공 : 할당된 새로운 메모리의 포인터\n
 * 실패 : NULL
 * @ingroup MEMORY
 */
extern void* r_memdup(void *mem_ptr, u32 mem_size);

/**
 * @brief 동적으로 할당된 메모리를 해제 한다.
 * @param[in] mem_ptr 할당된 메모리의 포인터
 * @return 동작 결과
 * @ingroup MEMORY
 * @code{c}
	ret = r_free(buf);
 * @endcode
 */
extern r_ret_t r_free(void *mem_ptr);
}
#endif /* R_MEM_H */
