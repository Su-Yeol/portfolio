#pragma once // 헤더 중복 선언 방지 = #ifndef ~ #endif
/* config 파일(설정, 프로그램의 실행 여부 등을 저장해둔 파일)을 파싱(데이터 분해, 분석을 통해 원하는 형태로 조립 후 빼냄)하는 클래스 */

#include <string>
#include <map>
 
class CConfigParser {
public:
	// 생성자(인스턴스 초기화 메소드), 설정 파일의 경로를 받아와 해당 파일을 파싱하여 설정값을 맵에 저장
	CConfigParser(const std::string& path);
	// 설정 파일 파싱에 성공했는지 여부를 반환.
	bool IsSuccess() { return m_table.size() != 0; }
	// 지정된 이름의 설정값이 맵에 존재하는지 확인
	bool Contain(const std::string& name);
	// 지정된 이름의 설정값을 bool 형태로 반환 
	bool GetBool(const std::string& name);
	// 지정된 이름의 설정값을 문자열로 반환
	std::string GetString(const std::string& name);
	// 지정된 이름의 설정값을 float으로 반환
	float GetFloat(const std::string& name);
	// 지정된 이름의 설정값을 int으로 반환
	int GetInt(const std::string& name);
 
private:
	/* map<key, value>이름 : 설정값을 저장하기 위한 맵.
		설정 이름을 key로, 해당 설정값을 문자열로 저장
		key 기분 자동 오름차순 정렬 */
	std::map < std::string, std::string > m_table;
};