# Software Architecture Description

## 소프트웨어 아키텍처 설계서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-ARCH-001                                       |
| **Version**           | 0.1 Draft                                             |
| **ISO 26262 Reference** | Part 6, Clause 7 — Software architectural design   |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | <!-- TODO: 작성자 이름 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | <!-- TODO: 승인일자 (YYYY-MM-DD) -->                   |
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

<!-- TODO: ASIL 분해(decomposition) 적용 시 상세 근거 기술 -->

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

<!-- TODO: CMake/Makefile 기반 빌드 체인 상세 구조 기술 -->

## 6. 모듈 간 인터페이스 (Inter-module Interfaces)

### 6.1 CAN 메시지 인터페이스

| 인터페이스 ID | 송신 모듈        | 수신 모듈         | CAN ID       | 주기        | 설명                    |
|---------------|------------------|-------------------|--------------|-------------|-------------------------|
| IF-CAN-001    | decision-module  | control-module    | <!-- TODO --> | <!-- TODO --> ms | 경로/속도 명령 전송     |
| IF-CAN-002    | control-module   | aeb-control       | <!-- TODO --> | <!-- TODO --> ms | 현재 제어 상태 공유     |
| IF-CAN-003    | aeb-control      | control-module    | <!-- TODO --> | <!-- TODO --> ms | AEB 개입 명령           |
| IF-CAN-004    | remote-control   | decision-module   | <!-- TODO --> | <!-- TODO --> ms | 원격 조종 명령          |
| IF-CAN-005    | CAN Gateway      | decision-module   | <!-- TODO --> | <!-- TODO --> ms | 외부 센서/인프라 데이터 |

<!-- TODO: CAN DBC 파일 참조 및 메시지 상세 레이아웃 기술 -->

### 6.2 내부 API 인터페이스

| 인터페이스 ID | 제공 모듈 | 사용 모듈        | API / 함수명          | 설명                      |
|---------------|-----------|------------------|-----------------------|---------------------------|
| IF-API-001    | common    | 전 모듈          | <!-- TODO -->         | CAN 메시지 파싱 유틸리티   |
| IF-API-002    | common    | 전 모듈          | <!-- TODO -->         | 로깅/진단 공통 함수        |
| IF-API-003    | common    | aeb-control      | <!-- TODO -->         | 타이머/스케줄러 유틸리티   |

<!-- TODO: 각 API의 함수 시그니처, 파라미터, 반환값 상세 기술 -->

## 7. 동적 아키텍처 (Dynamic Architecture)

### 7.1 태스크/스레드 구조

| 태스크명             | 소속 모듈        | 우선순위      | 주기           | 설명                    |
|----------------------|------------------|---------------|----------------|-------------------------|
| <!-- TODO -->        | aeb-control      | <!-- TODO --> | <!-- TODO --> ms | AEB 판단 루프           |
| <!-- TODO -->        | control-module   | <!-- TODO --> | <!-- TODO --> ms | 제어 출력 루프           |
| <!-- TODO -->        | decision-module  | <!-- TODO --> | <!-- TODO --> ms | 경로 계획 루프           |
| <!-- TODO -->        | remote-control   | <!-- TODO --> | <!-- TODO --> ms | 원격 명령 수신 루프      |

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

<!-- TODO: 실제 디렉토리 구조와 대조하여 정확한 분리 구조 기술 -->
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

<!-- TODO: 아키텍처 리뷰 체크리스트 작성 및 첨부 -->

## 11. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | <!-- TODO --> | <!-- TODO --> | Initial draft 작성    |
