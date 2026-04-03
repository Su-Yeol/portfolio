# Software Unit Design Specification

## 소프트웨어 단위 설계 명세서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-UD-001                                         |
| **Version**           | 0.1 Draft                                             |
| **ISO 26262 Reference** | Part 6, Clause 8 — Software unit design and implementation |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | <!-- TODO: 작성자 이름 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | <!-- TODO: 승인일자 (YYYY-MM-DD) -->                   |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 8에 따라 Autonomous Driving Stack의 소프트웨어
단위(Software Unit) 설계 및 구현에 대한 명세를 프로젝트 수준에서 제공한다.
아키텍처 설계(SW-ADS-ARCH-001)에서 정의된 각 모듈의 소프트웨어 단위를 상세 설계하고,
구현 지침을 정의한다.

## 2. 적용 범위 (Scope)

- 대상 모듈: aeb-control, control-module, decision-module (K-City / Gateway), remote-control, common
- 구현 언어: C++ (주 언어), C (하위 드라이버/인터페이스), Shell (빌드/배포 스크립트)
- 대상 플랫폼: NXP S32G (aarch64), Linux 기반

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                        | 비고                  |
|-----------------|-----------------------------------------------|-----------------------|
| SW-ADS-REQ-001  | Software Safety Requirements Specification    | 안전 요구사항          |
| SW-ADS-ARCH-001 | Software Architecture Description             | 아키텍처 설계서        |
| SW-ADS-UT-001   | Software Unit Test Specification              | 단위 테스트 명세서     |
| <!-- TODO -->   | Coding Guidelines (C/C++)                     | 코딩 가이드라인        |

## 4. 코딩 가이드라인 (Coding Guidelines)

ISO 26262 Part 6, Clause 8에서 요구하는 코딩 가이드라인 적용 범위:

### 4.1 적용 코딩 표준

| 표준              | 적용 범위             | 비고                                  |
|-------------------|-----------------------|---------------------------------------|
| MISRA C:2012      | C 코드 전체            | ASIL C/D 모듈 필수 적용               |
| MISRA C++:2008    | C++ 코드 전체          | <!-- TODO: 최신 버전 검토 -->          |
| CERT C            | 보안 관련 코드         | 원격제어, 통신 모듈 보충 적용          |
| <!-- TODO -->     | Shell 스크립트         | 빌드 스크립트 가이드라인              |

<!-- TODO: 정적 분석 도구 (예: Polyspace, PC-lint, cppcheck) 및 설정 기술 -->

### 4.2 USE_PRIVATE_IMPL 코딩 규칙

- Private 구현부(`USE_PRIVATE_IMPL=ON`)와 Public stub 간 함수 시그니처 동일성 필수
- Private 구현부에는 안전 필수 알고리즘, 캘리브레이션 값, 진단 로직 포함
- Public stub에는 기능이 없는 기본 반환값 또는 시뮬레이션 로직 포함
- 전처리기 매크로를 통한 조건부 컴파일로 구현 전환

```c
/* 예시: 조건부 컴파일 패턴 */
#ifdef USE_PRIVATE_IMPL
  #include "private/aeb_algorithm_impl.h"
#else
  #include "public/aeb_algorithm_stub.h"
#endif
```

<!-- TODO: 실제 코드 구조와 대조하여 예시 갱신 -->

## 5. 소프트웨어 단위 목록 (Software Unit Inventory)

### 5.1 aeb-control 모듈 단위

| Unit ID        | 단위명                  | 파일(예상)                    | ASIL | 설명                      |
|----------------|-------------------------|-------------------------------|------|---------------------------|
| SU-AEB-001     | AEB 판단 로직           | <!-- TODO -->                 | D    | 전방 충돌 위험도 판단      |
| SU-AEB-002     | 제동 명령 생성          | <!-- TODO -->                 | D    | 제동 강도 계산 및 명령 생성 |
| SU-AEB-003     | AEB 상태 관리           | <!-- TODO -->                 | D    | AEB 활성/비활성 상태 머신  |
| <!-- TODO: 추가 단위 식별 --> | | | | |

### 5.2 control-module 단위

| Unit ID        | 단위명                  | 파일(예상)                    | ASIL | 설명                      |
|----------------|-------------------------|-------------------------------|------|---------------------------|
| SU-CTL-001     | 조향 제어               | <!-- TODO -->                 | C    | 조향 명령 계산 및 출력     |
| SU-CTL-002     | 가감속 제어             | <!-- TODO -->                 | C    | 가속/감속 명령 계산        |
| SU-CTL-003     | CAN 송신 관리           | <!-- TODO -->                 | C    | 제어 명령 CAN 메시지 송신  |
| <!-- TODO: 추가 단위 식별 --> | | | | |

### 5.3 decision-module 단위

| Unit ID        | 단위명                  | 파일(예상)                    | ASIL | 설명                      |
|----------------|-------------------------|-------------------------------|------|---------------------------|
| SU-DEC-001     | 경로 계획               | <!-- TODO -->                 | C    | 목표 경로 생성             |
| SU-DEC-002     | 주행 판단               | <!-- TODO -->                 | C    | 주행/정지/차선변경 판단    |
| SU-DEC-003     | CAN GW 데이터 처리      | <!-- TODO -->                 | B    | Gateway 수신 데이터 파싱   |
| SU-DEC-004     | K-City 특화 로직        | <!-- TODO -->                 | C    | K-City 환경 특화 판단 로직 |
| <!-- TODO: 추가 단위 식별 --> | | | | |

### 5.4 remote-control 단위

| Unit ID        | 단위명                  | 파일(예상)                    | ASIL | 설명                      |
|----------------|-------------------------|-------------------------------|------|---------------------------|
| SU-RMT-001     | 원격 명령 수신          | <!-- TODO -->                 | B    | 원격 조종 명령 수신/파싱   |
| SU-RMT-002     | 통신 상태 감시          | <!-- TODO -->                 | B    | 연결 상태 모니터링         |
| SU-RMT-003     | Failsafe 처리           | <!-- TODO -->                 | B    | 통신 단절 시 안전 전환     |
| <!-- TODO: 추가 단위 식별 --> | | | | |

### 5.5 common 모듈 단위

| Unit ID        | 단위명                  | 파일(예상)                    | ASIL | 설명                      |
|----------------|-------------------------|-------------------------------|------|---------------------------|
| SU-CMN-001     | CAN 메시지 파서         | <!-- TODO -->                 | D    | CAN 프레임 인코딩/디코딩   |
| SU-CMN-002     | 로깅 유틸리티           | <!-- TODO -->                 | QM   | 디버그/진단 로깅           |
| SU-CMN-003     | 타이머 유틸리티         | <!-- TODO -->                 | C    | 주기적 실행, 타임아웃 관리 |
| <!-- TODO: 추가 단위 식별 --> | | | | |

## 6. 단위 상세 설계 템플릿 (Unit Detail Design Template)

각 소프트웨어 단위에 대해 아래 형식으로 상세 설계를 작성한다.

### 6.1 단위 설계 기술서 양식

```
----------------------------------------------------------------------
Unit ID       : SU-XXX-NNN
Unit Name     : [단위명]
Module        : [소속 모듈]
ASIL          : [ASIL 등급]
Related SWSR  : [관련 안전 요구사항 ID 목록]
----------------------------------------------------------------------

1. 기능 설명 (Functional Description)
   [단위의 기능을 자연어로 기술]

2. 인터페이스 정의 (Interface Definition)
   - 입력: [입력 데이터, 타입, 범위, 단위]
   - 출력: [출력 데이터, 타입, 범위, 단위]
   - 전역 데이터: [사용하는 전역 변수/공유 리소스]

3. 알고리즘 / 로직 (Algorithm / Logic)
   [유사 코드(pseudocode) 또는 상태 다이어그램으로 기술]

4. 에러 처리 (Error Handling)
   [입력 유효성 검증, 예외 상황 처리 방법]

5. 제약사항 (Constraints)
   [타이밍, 메모리, 플랫폼 종속성 등]

6. USE_PRIVATE_IMPL 적용 여부
   [Private 구현 분리 대상 여부 및 분리 범위]
----------------------------------------------------------------------
```

<!-- TODO: 각 단위별 상세 설계 기술서를 별도 파일 또는 본 문서 부록으로 작성 -->

## 7. 설계 및 구현 방법론 (Design and Implementation Methods)

ISO 26262 Part 6, Table 4에 따른 설계 방법 적용:

| 방법                                      | ASIL B | ASIL C | ASIL D | 적용 여부  |
|-------------------------------------------|--------|--------|--------|------------|
| 자연어 + 비정형 표기법                     | ++     | +      | +      | 적용       |
| 준정형 표기법 (상태 머신 다이어그램 등)     | +      | ++     | ++     | <!-- TODO --> |
| 정형 표기법                                | +      | +      | ++     | <!-- TODO --> |

ISO 26262 Part 6, Table 5에 따른 구현 시 설계 원칙:

| 원칙                          | ASIL B | ASIL C | ASIL D | 적용 대상 모듈                |
|-------------------------------|--------|--------|--------|-------------------------------|
| 매개변수 범위 검사             | ++     | ++     | ++     | 전 모듈                       |
| 포인터 유효성 검사             | +      | ++     | ++     | C/C++ 코드 전체               |
| 동적 메모리 할당 제한          | +      | ++     | ++     | aeb-control (금지), 기타 (제한)|
| 재진입성 보장                  | +      | +      | ++     | 공유 데이터 접근 코드          |
| 암묵적 형변환 방지             | +      | ++     | ++     | 전 모듈                       |

<!-- TODO: 각 원칙의 프로젝트 적용 방침 상세 기술 -->

## 8. NXP S32G 플랫폼 고려사항 (Platform-specific Considerations)

| 항목                    | 내용                                              |
|-------------------------|---------------------------------------------------|
| 아키텍처                | aarch64 (ARMv8-A)                                 |
| 바이트 오더             | Little Endian                                      |
| 정수 크기               | LP64 모델 (int=32bit, long=64bit, pointer=64bit)  |
| 부동소수점              | IEEE 754, 하드웨어 FPU 지원                        |
| CAN 컨트롤러            | <!-- TODO: NXP S32G CAN 컨트롤러 사양 기술 -->     |
| 컴파일러                | <!-- TODO: GCC/Clang 버전 및 크로스 컴파일 설정 --> |
| 최적화 레벨             | <!-- TODO: 안전 코드 최적화 정책 (-O0/-O2 등) -->   |

<!-- TODO: 타겟 보드 특성에 따른 코딩 제약사항 추가 -->

## 9. 정적 분석 및 코드 메트릭 기준 (Static Analysis and Code Metrics)

| 메트릭                    | 기준값                | 적용 범위          |
|---------------------------|-----------------------|--------------------|
| 순환 복잡도 (McCabe)      | <!-- TODO --> 이하    | 전 모듈 함수       |
| 함수 길이                 | <!-- TODO --> lines   | 전 모듈 함수       |
| 중첩 깊이                 | <!-- TODO --> 이하    | 전 모듈 함수       |
| MISRA 위반                | 0 (필수 규칙)         | ASIL C/D 모듈      |
| 주석 비율                 | <!-- TODO --> % 이상  | 안전 관련 코드     |

<!-- TODO: 정적 분석 도구 설정 파일 경로 및 실행 방법 기술 -->

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | <!-- TODO --> | <!-- TODO --> | Initial draft 작성    |
