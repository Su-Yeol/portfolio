# Software Architecture Description

## 소프트웨어 아키텍처 설계서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-ARCH-001                                       |
| **Version**           | 1.0                                                   |
| **ISO 26262 Reference** | Part 6, Clause 7 — Software architectural design   |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | SuYeol Kim                                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | 2026-04-06                                             |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Autonomous Driving Stack의 소프트웨어
아키텍처 설계를 기술한다. 시스템 아키텍처로부터 도출된 소프트웨어 구성요소의 구조,
인터페이스, 데이터 흐름 및 안전 메커니즘을 프로젝트 수준에서 정의한다.

## 2. 적용 범위 (Scope)

본 문서는 NXP S32G (aarch64) 플랫폼에서 동작하는 자율주행 스택의 전체 소프트웨어
아키텍처를 다룬다. 대상 모듈: aeb-control, control-module, decision-module
(K-City / Gateway), remote-control, common.

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-ADS-REQ-001  | Software Safety Requirements Specification  | 안전 요구사항 명세서   |
| SW-ADS-UD-001   | Software Unit Design Specification          | 단위 설계 명세서       |
| <!-- TODO -->   | System Architecture Document                | 시스템 아키텍처 문서   |
| <!-- TODO -->   | HSI Specification                           | HW-SW 인터페이스      |

## 4. 아키텍처 설계 원칙 (Design Principles)

ISO 26262 Part 6, Clause 7.4.1에 따른 설계 원칙:

1. **모듈화 (Modularity)**: 기능별 독립 모듈 분리 (aeb-control, control-module 등)
2. **캡슐화 (Encapsulation)**: `USE_PRIVATE_IMPL` 플래그를 통한 안전 필수 구현부 분리
3. **제한된 결합도 (Limited Coupling)**: 모듈 간 CAN 메시지 기반 느슨한 결합
4. **높은 응집도 (High Cohesion)**: 각 모듈 내 관련 기능 집중
5. **방어적 프로그래밍 (Defensive Programming)**: CAN 메시지 유효성 검증, 범위 확인

**ASIL 분해 적용:** aeb-control(ASIL D)과 control-module(ASIL C) 간 CAN 메시지 기반 인터페이스를 통해 기능 분리를 수행한다. decision-module은 ASIL B~C로 분해되어, 경로 계획(ASIL C)과 Gateway 데이터 처리(ASIL B)를 분리 설계한다. common 모듈은 전 모듈에서 사용되므로 최고 ASIL D 수준으로 설계/검증한다.

## 5. 소프트웨어 아키텍처 개요 (Architecture Overview)

### 5.1 정적 아키텍처 (Static Architecture)

```
┌─────────────────────────────────────────────────────────────────┐
│                    Autonomous Driving Stack                      │
│                    (NXP S32G / aarch64)                          │
│                                                                  │
│  ┌──────────────┐  ┌────────────────┐  ┌─────────────────────┐  │
│  │  aeb-control │  │ control-module │  │  decision-module    │  │
│  │  (ASIL D)    │  │ (ASIL C)       │  │  (ASIL B~C)         │  │
│  │              │  │                │  │  ┌───────────────┐  │  │
│  │  AEB 판단/   │  │  조향/가감속   │  │  │ K-City variant│  │  │
│  │  제동 제어   │  │  제어 출력     │  │  │ GW variant    │  │  │
│  │              │  │                │  │  └───────────────┘  │  │
│  └──────┬───────┘  └───────┬────────┘  └──────────┬──────────┘  │
│         │                  │                       │             │
│         └──────────────────┼───────────────────────┘             │
│                            │                                     │
│                    ┌───────┴────────┐   ┌──────────────────┐    │
│                    │    common      │   │  remote-control  │    │
│                    │  (ASIL D 준수) │   │  (ASIL B)        │    │
│                    │  공통 라이브러리│   │  원격 제어 수신  │    │
│                    └───────┬────────┘   └────────┬─────────┘    │
│                            │                      │              │
│  ──────────────────────────┴──────────────────────┘──────────── │
│                         CAN Bus Interface                        │
│                        (CAN Gateway 연동)                        │
└─────────────────────────────────────────────────────────────────┘
```

<!-- TODO: 상세 아키텍처 다이어그램을 별도 도구(Enterprise Architect 등)로 작성하여 첨부 -->

### 5.2 모듈 구성 (Module Composition)

| 모듈명            | 역할                              | ASIL  | 언어     | 비고                          |
|-------------------|-----------------------------------|-------|----------|-------------------------------|
| aeb-control       | 자동 긴급 제동 판단 및 제어        | D     | C/C++    | 최고 안전 등급                 |
| control-module    | 조향/가감속 제어 명령 생성 및 전송 | C     | C/C++    | 구동기 인터페이스              |
| decision-module   | 경로 계획 및 주행 판단             | B ~ C | C/C++    | K-City / Gateway 변형 존재    |
| remote-control    | 원격 조종 명령 수신 및 처리        | B     | C/C++    | 통신 단절 안전 처리            |
| common            | 공통 유틸리티 및 CAN 처리 라이브러리| D     | C/C++    | 전 모듈에서 사용               |

### 5.3 빌드 구성 (Build Configuration)

| 빌드 모드              | 설명                                                    |
|------------------------|---------------------------------------------------------|
| Module-local build     | 각 모듈 독립 빌드 (단위 테스트, 개발용)                   |
| Project-level build    | decision-module의 프로젝트 수준 통합 빌드                 |
| `USE_PRIVATE_IMPL=ON`  | 안전 필수 구현부 포함 빌드 (배포용)                       |
| `USE_PRIVATE_IMPL=OFF` | Public stub만 포함 빌드 (오픈소스/데모용)                 |

**빌드 체인:** CMake 3.x 기반 빌드 시스템을 사용한다. 각 모듈은 독립적인 `CMakeLists.txt`를 보유하며, `FetchContent`를 통해 Google Test 1.14.0을 자동 획득한다. 컴파일러는 호스트(GCC 11.x, x86_64)와 타겟(aarch64-linux-gnu-g++)을 지원하며, C++17 표준을 적용한다. `cmake/CompilerWarnings.cmake`에서 `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion` 경고 플래그를 전 모듈에 적용한다.

## 6. 모듈 간 인터페이스 (Inter-module Interfaces)

### 6.1 CAN 메시지 인터페이스

| 인터페이스 ID | 송신 모듈        | 수신 모듈         | CAN ID       | 주기        | 설명                    |
|---------------|------------------|-------------------|--------------|-------------|-------------------------|
| IF-CAN-001    | decision-module  | control-module    | 0x165, 0x1A0, 0x1CF | 50 ms | 경로/속도 명령 전송     |
| IF-CAN-002    | control-module   | aeb-control       | 0x35, 0x60, 0xA0   | 50 ms | 현재 제어 상태 공유     |
| IF-CAN-003    | aeb-control      | control-module    | 0x160 (len=8)       | 10 ms | AEB 개입 명령           |
| IF-CAN-004    | remote-control   | decision-module   | <!-- TBD -->        | <!-- TBD --> ms | 원격 조종 명령          |
| IF-CAN-005    | CAN Gateway      | decision-module   | 0xEA 등             | 이벤트 기반 | 외부 센서/인프라 데이터 |

<!-- TODO: CAN DBC 파일 참조 및 메시지 상세 레이아웃 기술 -->

### 6.2 내부 API 인터페이스

| 인터페이스 ID | 제공 모듈 | 사용 모듈        | API / 함수명          | 설명                      |
|---------------|-----------|------------------|-----------------------|---------------------------|
| IF-API-001    | common    | 전 모듈          | GetCanSpecProvider() -> ICanSpecProvider | CAN spec 조회 유틸리티     |
| IF-API-002    | common    | 전 모듈          | GetCrcProvider() -> ICrcProvider         | CRC-16/CCITT 계산          |
| IF-API-003    | common    | aeb-control      | signal_codec::WriteU16Le/WriteU32Le/WriteU16LeSafe/WriteU32LeSafe | 시그널 인코딩 |
| IF-API-004    | common    | 전 모듈          | CConfigParser                            | INI 설정 파일 파싱         |

<!-- TODO: 각 API의 함수 시그니처, 파라미터, 반환값 상세 기술 -->

## 7. 동적 아키텍처 (Dynamic Architecture)

### 7.1 태스크/스레드 구조

| 태스크명             | 소속 모듈        | 우선순위      | 주기           | 설명                    |
|----------------------|------------------|---------------|----------------|-------------------------|
| AEB MainCycle        | aeb-control      | 높음 (RT)     | 10 ms            | AEB 판단 루프           |
| Control MainCycle    | control-module   | 높음          | 50 ms            | 제어 출력 루프           |
| Decision MainCycle   | decision-module  | 보통          | 50 ms            | 경로 계획 루프           |
| Remote RxLoop        | remote-control   | 보통          | 이벤트 기반      | 원격 명령 수신 루프      |

<!-- TODO: 태스크 간 동기화 메커니즘 (뮤텍스, 세마포어, 메시지 큐 등) 기술 -->

### 7.2 시동/종료 시퀀스 (Startup / Shutdown Sequence)

<!-- TODO: 시스템 시동 시 모듈 초기화 순서 및 종료 시 안전 상태 진입 절차 기술 -->

1. 시스템 부팅 → common 모듈 초기화 → CAN 인터페이스 설정
2. 각 기능 모듈 초기화 (aeb-control → control-module → decision-module → remote-control)
3. 정상 운영 모드 진입
4. 종료 요청 시: 역순 종료 → 안전 상태(safe state) 전환

## 8. 안전 메커니즘 (Safety Mechanisms)

### 8.1 에러 감지 메커니즘

| ID          | 메커니즘              | 적용 모듈            | 설명                                      |
|-------------|-----------------------|----------------------|-------------------------------------------|
| SM-DET-001  | CAN 타임아웃 감시     | 전 모듈              | 메시지 수신 타임아웃 시 에러 플래그 설정    |
| SM-DET-002  | 레인지 체크           | control-module       | 제어 명령 값 범위 초과 감지                 |
| SM-DET-003  | 프로그램 흐름 감시    | aeb-control          | Watchdog 기반 실행 흐름 모니터링            |
| SM-DET-004  | 데이터 무결성 체크    | common               | CAN 메시지 CRC/카운터 검증                  |
| <!-- TODO: 추가 에러 감지 메커니즘 --> | | | |

### 8.2 에러 처리 메커니즘

| ID          | 메커니즘              | 적용 모듈            | 설명                                      |
|-------------|-----------------------|----------------------|-------------------------------------------|
| SM-HDL-001  | 안전 상태 전환        | aeb-control          | 고장 감지 시 비상 제동 또는 안전 정지       |
| SM-HDL-002  | 기본값 출력           | control-module       | 통신 이상 시 사전 정의된 안전 기본값 출력   |
| SM-HDL-003  | 모드 전환             | decision-module      | 판단 불가 시 수동 모드 또는 안전 정지 모드  |
| <!-- TODO: 추가 에러 처리 메커니즘 --> | | | |

## 9. USE_PRIVATE_IMPL 아키텍처 분리 (Private Implementation Separation)

`USE_PRIVATE_IMPL` 빌드 플래그를 통해 안전 필수(safety-critical) 코드와 공개 가능
코드를 아키텍처 수준에서 분리한다.

```
module/
├── include/            # Public 헤더 (항상 포함)
├── src/
│   ├── public/         # Public 구현 (USE_PRIVATE_IMPL=OFF 시 사용)
│   └── private/        # Safety-critical 구현 (USE_PRIVATE_IMPL=ON 시 사용)
└── CMakeLists.txt      # 조건부 빌드 설정
```

**실제 디렉토리 구조 예시 (common 모듈):**
```
modules/common/
├── include/           # Public 헤더 (CrcProvider.h, CanSpecProvider.h, SignalCodec.h)
├── src/
│   ├── public/        # Public 구현 (CrcProviderPublic.cpp)
│   └── private/       # Safety-critical 구현 (CrcProviderPrivate.cpp)
├── config/            # can_spec.ini 등 설정 파일
├── test/              # Google Test 기반 단위/통합 테스트
└── CMakeLists.txt     # CMake 빌드 설정 (FetchContent for GoogleTest)
```
<!-- TODO: Private/Public 전환 시 인터페이스 일관성 보장 방안 기술 -->

## 10. 아키텍처 설계 검증 기준 (Verification Criteria)

ISO 26262 Part 6, Table 3에 따른 아키텍처 설계 검증 방법:

| 검증 항목                                | ASIL B | ASIL C | ASIL D | 방법                |
|------------------------------------------|--------|--------|--------|---------------------|
| 아키텍처 일관성 검증                      | ++     | ++     | ++     | Inspection          |
| 요구사항 적합성 (SW-ADS-REQ-001 대비)     | ++     | ++     | ++     | Walkthrough         |
| 인터페이스 일관성                         | ++     | ++     | ++     | Inspection          |
| 안전 메커니즘 적절성                      | +      | ++     | ++     | Analysis            |
| 설계 원칙 준수                            | +      | +      | ++     | Inspection          |

(`++` = highly recommended, `+` = recommended)

아키텍처 리뷰는 PRC-CR-001 Code Review Checklist (ISO 26262-6 Table 3 기반)를 사용하여 수행한다.

## 11. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성    |
| 1.0     | 2026-04-06 | SuYeol Kim | 구현 세부사항 반영, CAN ID/API/태스크 정보 확정 |
