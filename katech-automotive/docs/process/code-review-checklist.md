# Code Review Checklist (ISO 26262-6 Table 3)

| Field | Value |
|---|---|
| **Document ID** | PRC-CR-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 6, Clause 8, Table 3 |
| **Author** | SuYeol Kim |
| **Date** | 2026-04-06 |

---

## 1. Purpose

본 체크리스트는 ISO 26262-6 Table 3 (설계 원칙)에 기반하여 소프트웨어 유닛 설계 및 구현의 코드 리뷰/인스펙션 시 사용한다.

## 2. Applicability by ASIL

| # | Check Item | ASIL A | ASIL B | ASIL C | ASIL D |
|---|---|---|---|---|---|
| 1 | 올바른 매개변수 평가 순서 | o | + | ++ | ++ |
| 2 | 암묵적 형 변환 없음 | o | + | ++ | ++ |
| 3 | 숨겨진 데이터 흐름/제어 흐름 없음 | + | ++ | ++ | ++ |
| 4 | 무조건 점프(goto) 없음 | + | ++ | ++ | ++ |
| 5 | 재귀 호출 없음 | o | + | ++ | ++ |
| 6 | 런타임 동적 객체 생성 없음 | o | + | + | ++ |
| 7 | 포인터 사용 제한 및 검증 | o | + | ++ | ++ |
| 8 | 모든 변수 사용 전 초기화 | + | ++ | ++ | ++ |
| 9 | 함수 단일 진입/종료점 | o | + | + | ++ |
| 10 | 방어적 프로그래밍 (입력 범위 검사) | + | + | ++ | ++ |

Legend: ++ 강력 권장 (필수), + 권장, o 선택

## 3. Detailed Check Items

### 3.1 올바른 매개변수 평가 순서
- [ ] 함수 호출 인자에서 부작용이 있는 표현식이 사용되지 않는가?
- [ ] 복합 표현식에서 평가 순서에 의존하는 코드가 없는가?

### 3.2 암묵적 형 변환 없음
- [ ] signed/unsigned 혼합 연산이 없는가?
- [ ] 축소 변환(narrowing conversion)이 없는가?
- [ ] 명시적 static_cast를 사용하는가?

### 3.3 숨겨진 데이터 흐름/제어 흐름
- [ ] 전역 변수를 통한 암묵적 데이터 전달이 없는가?
- [ ] 매크로를 통한 숨겨진 제어 흐름이 없는가?
- [ ] 모든 데이터 흐름이 함수 인터페이스를 통해 명시적인가?

### 3.4 무조건 점프
- [ ] goto 문이 사용되지 않는가?
- [ ] switch에서 break 없는 fall-through가 없는가? (의도적인 경우 주석 필수)

### 3.5 재귀 호출
- [ ] 직접 재귀 호출이 없는가?
- [ ] 간접 재귀(A→B→A) 패턴이 없는가?

### 3.6 런타임 동적 객체 (ASIL D 필수)
- [ ] 초기화 이후 new/delete/malloc/free가 호출되지 않는가?
- [ ] STL 컨테이너의 런타임 크기 변경이 없는가?

### 3.7 포인터 사용
- [ ] 포인터 역참조 전 NULL 검사가 있는가?
- [ ] 배열 접근 시 경계 검사가 있는가?
- [ ] 스마트 포인터 사용을 선호하는가?

### 3.8 변수 초기화
- [ ] 모든 지역 변수가 선언 시 초기화되는가?
- [ ] 멤버 변수가 생성자에서 초기화되는가?
- [ ] 초기화되지 않은 변수 사용 경로가 없는가?

### 3.9 단일 진입/종료점
- [ ] 함수 중간의 early return이 최소화되었는가?
- [ ] 예외 경로에서도 리소스가 정리되는가?

### 3.10 방어적 프로그래밍
- [ ] 모든 외부 입력에 범위 검사가 있는가?
- [ ] CAN 데이터의 CRC 검증이 있는가?
- [ ] 설정 파라미터의 유효성 검사가 있는가?
- [ ] switch 문에 default 케이스가 있는가?
- [ ] 함수 반환값이 모두 검사되는가?

## 4. Additional Safety Checks

### 4.1 MISRA C++ Compliance
- [ ] MISRA C++:2008 필수(Required) 규칙 위반 없음
- [ ] 편차(Deviation)가 있는 경우 문서화되었는가?
- [ ] 정적 분석 도구(clang-tidy/cppcheck)에서 경고 없음

### 4.2 Concurrency Safety
- [ ] 공유 데이터에 mutex/lock 보호가 있는가?
- [ ] 데드락 가능성이 없는가?
- [ ] volatile 대신 atomic을 사용하는가?

### 4.3 Error Handling
- [ ] 모든 오류 경로가 안전 상태로 전이하는가?
- [ ] 예외가 catch되지 않고 전파되는 경우가 없는가?
- [ ] 리소스 누수(메모리, 파일, 소켓)가 없는가?

## 5. References

- ISO 26262:2018, Part 6, Clause 8, Table 3
- ISO 26262:2018, Part 6, Clause 8, Table 4
- MISRA C++:2008 Guidelines
- AUTOSAR C++14 Coding Guidelines
- [Verification Plan](verification-plan.md)
- [Code Inspection Template](code-inspection-template.md)
