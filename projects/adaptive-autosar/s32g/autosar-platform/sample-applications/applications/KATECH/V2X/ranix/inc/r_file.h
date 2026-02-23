#ifndef R_FILE_H
#define R_FILE_H

/////////
// Header
#include "r_type.h"

///////////////////
// Macro definition

//////////////////
// Type definition

/**
 * @enum r_file_mode_t r_file.h
 * @brief File Open 시 지정할 Mode
 * @ingroup FILE
 */
typedef enum
{
	R_FILE_READ, /**< 읽기 모드 */
	R_FILE_READWRITE, /**< 읽고 쓰기 모드 */

	R_FILE_MODE_MAX
} r_file_mode_t;

/**
 * @struct r_file_t r_file.h
 * @brief File 정보
 * @ingroup FILE
 */
typedef struct
{
	r_file_mode_t mode; /**< File Read/Write 모드 */
	void *file; /**< file 정보 */
} r_file_t;

/**
 * @enum r_ffind_mode_t r_file.h
 * @brief File 및 디렉토리 목록을 찾기 위한 모드
 * @ingroup FILE
 */
typedef enum
{
	R_FFIND_FILE, /**< 파일 찾기 모드 */
	R_FFIND_DIR, /**< 디렉토리 찾기 모드 */
	R_FFIND_ALL, /**< 파일 및 디렉토리 찾기 모드 */

	R_FFIND_MAX
} r_ffind_mode_t;

/////////////////
// Extern variable

//////////////////
// Extern function

/**
 * File 을 Open 한다.
 * @param[in] fhandle Open File 정보
 * @param[in] fname File 의 Full Path Name
 * @param[in] mode File Open 모드(@ref r_file_mode_t)
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup FILE
 * @code{c}
	r_ret_t ret = R_SUCCESS;
	s8 *filename = "/home/root/exam/file.cfg";
	r_file_t file = { 0 };

	ret = r_fopen(&file, filename, R_FILE_READ);
 * @endcode
 */
extern r_ret_t r_fopen(r_file_t *fhandle, const s8 *fname, r_file_mode_t mode);

/**
 * Open 된 File 을 Close 한다.
 * @param[in] fhandle Open File 정보
 * @return @ref R_SUCCESS : 성공 \n @ref R_FAIL : 실패
 * @ingroup FILE
 * @code{c}
	ret = r_fclose(&file);
 * @endcode
 */
extern r_ret_t r_fclose(r_file_t *fhandle);

/**
 * File 을 Read 한다.
 * @param[in] fhandle Open File 정보
 * @param[out] read_buff Read 후 저장되는 버퍼
 * @param[in] len Read 하고자 하는 Byte 수
 * @return 성공 : >= 0, 실제 읽은 byte 수\n
 * 실패 : < 0, @ref r_ret_t 참조
 * @ingroup FILE
 * @code{c}
	u8 *buf = NULL;
	s32 len = 0;
	s32 fsize = 0;

	fsize = r_fsize(filename);
	if (fsize > 0) {
		buf = R_CALLOC((u32)fsize);
		if (buf != NULL) {
			len = r_fread(&file, buf, (u32)fsize);
		}
	}
 * @endcode
 */
extern s32 r_fread(r_file_t *fhandle, u8 *read_buff, u32 len);

/**
 * File 을 Write 한다.
 * @param[in] fhandle Open File 정보
 * @param[in] write_buff Write 하고자 하는 버퍼
 * @param[in] len Write 하고자 하는 Byte 수
 * @return 성공 : >= 0, 실제 Write 한 byte 수\n
 * 실패 : < 0, @ref r_ret_t 참조
 * @ingroup FILE
 * @code{c}
	u8 *write_buf = "rvp";
	s32 write_len = strlen(write_buf);

	len = r_fwrite(&file, write_buf, write_len);
 * @endcode
 */
extern s32 r_fwrite(r_file_t *fhandle, u8 *write_buff, u32 len);

/**
 * File 의 크기를 구한다.
 * @param[in] full_path 전체 경로(or 상대 경로) 를 포함한 file 이름
 * @return 성공 : >= 0, 파일 크기\n
 * 실패 : < 0, @ref r_ret_t 참조
 * @ingroup FILE
 * @code{c}
	s32 file_size = 0;

	file_size = r_fsize(filename);
 * @endcode
 */
extern s32 r_fsize(const s8 *full_path);

/**
 *  Path 에서 find_str 조건의 파일 이름들을 모두 찾는다.
 *  find_str : find_str : 특정 문자 포함 찾기 -> "ab", 파일 모두 찾기 -> ""
 *  out_names : "abc1.c\0abc2.der\0abc3.oer\0abcd4.bin\0babb5.txt\0\0"
 *  out_names_len : out_names 의 길이 bytes -> 46
 *  return > 0 : 찾은 파일 개수 -> 5 *
 * @param[in] full_path 전체 경로(or 상대 경로) 의 Path
 * @param[in] find_str 찾기를 원하는 파일 이름 Mask
 * @param[in] mode 파일 및 디렉토리 찾기 모드
 * @param[out] out_names 찾은 파일 리스트
 * 파일 이름은 구분자 '\0' 로 구분 되고 '\0\0' 으로 끝남.\n
 * 파일 리스트가 최대 out_names_len 길이를 넘지 않을 만큼만 파일 목록을 생성함.\n
 * NULL 인 경우에는 찾은 파일의 개수만 Return 값으로 구한다.\n
 * @param[in] out_names_len out_names 의 할당된 버퍼 길이(Bytes)
 * @return >= 0 : 찾은 파일 이름 개수, < 0 : 실패 (@ref r_ret_t)
 * @ingroup FILE
 * @code{c}
	#define FILE_OUTPUT_STR_LEN		(2048)
	#define STR_MAX_LEN				(128)

	s8 *dirname = "/home/root/exam";
	s8 findstr[STR_MAX_LEN] = { 0 };
	u8 outputstr[FILE_OUTPUT_STR_LEN] = { 0 };
	s32 num = -1;

	num = r_ffind(dirname, findstr, R_FFIND_FILE, outputstr, FILE_OUTPUT_STR_LEN);
 * @endcode
 */
extern s32 r_ffind(const s8 *full_path, const s8 *find_str, r_ffind_mode_t mode, s8 *out_names,
            u32 out_names_len);

/**
 *  Path 에서 find_str 조건의 파일 이름들을 모두 찾는다.
 *  find_str : find_str : 특정 문자 포함 찾기 -> "ab", 파일 모두 찾기 -> ""
 *  out_names : "abc1.c\0abc2.der\0abc3.oer\0abcd4.bin\0babb5.txt\0\0"
 *  out_names_len : out_names 의 길이 bytes -> 46
 *  return > 0 : 찾은 파일 개수 -> 5 *
 * @param[in] full_path 전체 경로(or 상대 경로) 의 Path
 * @param[in] find_str 찾기를 원하는 파일 이름 Mask
 * @param[in] exclude_str 특정 문자열이 포함되어 있는 파일은 제외함 -> "cd" 포함 파일 제외
 * @param[in] mode 파일 및 디렉토리 찾기 모드
 * @param[out] out_names 찾은 파일 리스트
 * 파일 이름은 구분자 '\0' 로 구분 되고 '\0\0' 으로 끝남.
 * 파일 리스트가 최대 out_names_len 길이를 넘지 않을 만큼만 파일 목록을 생성함.
 * NULL 인 경우에는 찾은 파일의 개수만 Return 값으로 구한다.
 * @param[in] out_names_len out_names 의 할당된 버퍼 길이(Bytes)
 * @return >= 0 : 찾은 파일 이름 개수, < 0 : 실패 (@r_ret_t)
 * @ingroup FILE
 */
extern s32 r_ffind_ex(const s8 *full_path, const s8 *find_str, const s8 *exclude_str,
            r_ffind_mode_t mode, s8 *out_names, u32 out_names_len);

/**
 * r_ffind 로 찾은 결과에서 index 에 해당 하는 file name 구한다.
 * @param[in] file_names r_ffind 결과 out_names 변수
 * @param[in] index 파일 목록의 원하는 index
 * @return NULL 로 끝나는 파일 이름 문자열의 시작 포인터
 * @ingroup FILE
 * @code{c}
	s8 *strbyidx = NULL;
	u8 idx = 0; // Set the index number.

	num = r_ffind(dirname, findstr, R_FFIND_FILE, outputstr, FILE_OUTPUT_STR_LEN);
	if(idx < num) {
		strbyidx = r_ffind_get_name(outputstr, idx);
	}
 * @endcode
 */
extern s8* r_ffind_get_name(s8 *file_names, u8 index);

/**
 * full_path 에서 find_str 조건의 파일 이름들을 모두 찾는다.\n
 * find_str : 특정 문자 포함 찾기 -> "ab", 파일 모두 찾기 -> ""\n
 * out_name_list : 찾은 파일 이름을 r_malloc 으로 메모리를 할당 하여 2차원 배열로 전달 한다.\n
 * out_name_list_max : out_name_list 의 최대 이름 저장 가능 개수\n
 * ex)\n
 * s8 *name_list[3] = { 0 };\n
 * r_ffind_array("..", "cert", name_list, 3);
 * @param[in] full_path 전체 경로(or 상대 경로) 의 Path
 * @param[in] find_str 찾기를 원하는 파일 이름 Mask
 * @param[in] mode 파일 및 디렉토리 찾기 모드
 * @param[out] out_name_list 찾은 파일 리스트, 2차원 배열의 포인터
 * @param[in] out_name_list_max out_name 2차원 배열의 최대 ROW 개수
 * @return >= 0 : 찾은 파일 이름 개수, < 0 : 실패 (@r_ret_t)
 * @ingroup FILE
 * @code{c}
	#define NAME_LIST_MAX	 		(32)

	u8 *name_list[NAME_LIST_MAX] = { 0 };

	num = r_ffind_array(dirname, findstr, R_FFIND_ALL, name_list, NAME_LIST_MAX);
 * @endcode
 */
extern s32 r_ffind_array(const s8 *full_path, const s8 *find_str, r_ffind_mode_t mode,
            s8 *out_name_list[], u32 out_name_list_max);

/**
 * full_path 에서 find_str 조건의 파일 이름들을 모두 찾는다.
 * find_str : 특정 문자 포함 찾기 -> "ab", 파일 모두 찾기 -> ""
 * out_name_list : 찾은 파일 이름을 r_malloc 으로 메모리를 할당 하여 2차원 배열로 전달 한다.
 * out_name_list_max : out_name_list 의 최대 이름 저장 가능 개수
 * ex)
 * s8 *name_list[3] = { 0 };
 * r_ffind_array("..", "cert", name_list, 3);
 * @param[in] full_path 전체 경로(or 상대 경로) 의 Path
 * @param[in] find_str 찾기를 원하는 파일 이름 Mask
 * @param[in] exclude_str 특정 문자열이 포함되어 있는 파일은 제외함 -> "cd" 포함 파일 제외
 * @param[in] mode 파일 및 디렉토리 찾기 모드
 * @param[out] out_name_list 찾은 파일 리스트, 2차원 배열의 포인터
 * @param[in] out_name_list_max out_name 2차원 배열의 최대 ROW 개수
 * @return >= 0 : 찾은 파일 이름 개수, < 0 : 실패 (@r_ret_t)
 * @ingroup FILE
 */
extern s32 r_ffind_array_ex(const s8 *full_path, const s8 *find_str, const s8 *exclude_str,
            r_ffind_mode_t mode, s8 *out_name_list[], u32 out_name_list_max);

/**
 * r_ffind_array 결과로 전달받은 2차원 배열 name_list 에 할당된 파일 이름 변수들을 제거(r_free) 한다.
 * @param[in] name_list 찾은 파일 리스트, 2차원 배열의 포인터
 * @param[in] name_list_max name_list 2차원 배열의 최대 ROW 개수
 * @return >= 0 : 제거된 파일 이름 개수, < 0 : 실패 (@r_ret_t)
 * @ingroup FILE
 * @code{c}
	num = r_ffind_array_destroy(name_list, NAME_LIST_MAX);
 * @endcode
 */
extern s32 r_ffind_array_destroy(s8 *name_list[], u32 name_list_max);

#endif /* R_FILE_H */
