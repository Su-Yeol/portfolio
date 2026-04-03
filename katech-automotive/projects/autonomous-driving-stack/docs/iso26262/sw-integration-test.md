# Software Integration Test Specification

## 소프트웨어 통합 테스트 명세서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-IT-001                                         |
| **Version**           | 0.1 Draft                                             |
| **ISO 26262 Reference** | Part 6, Clause 10 — Software integration and testing |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | <!-- TODO: 작성자 이름 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | <!-- TODO: 승인일자 (YYYY-MM-DD) -->                   |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 10에 따라 Autonomous Driving Stack의
소프트웨어 통합 전략 및 통합 테스트 계획을 프로젝트 수준에서 정의한다.
단위 테스트(SW-ADS-UT-001)를 통과한 소프트웨어 단위들의 통합 과정에서
인터페이스 정합성, 데이터 흐름 정확성, 모듈 간 상호작용의 안전성을 검증한다.

## 2. 적용 범위 (Scope)

- 대상 모듈: aeb-control, control-module, decision-module (K-City / Gateway), remote-control, common
- 통합 수준: 모듈 내 단위 통합 → 모듈 간 통합 → 프로젝트 수준 시스템 통합
- 통합 환경: 호스트 시뮬레이션 + 타겟(NXP S32G) 실행
- CAN 통신: 가상 CAN(vCAN) + 실물 CAN Bus 양쪽 검증

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                        | 비고                    |
|-----------------|-----------------------------------------------|-------------------------|
| SW-ADS-REQ-001  | Software Safety Requirements Specification    | 안전 요구사항            |
| SW-ADS-ARCH-001 | Software Architecture Description             | 아키텍처 설계서          |
| SW-ADS-UD-001   | Software Unit Design Specification            | 단위 설계 명세서         |
| SW-ADS-UT-001   | Software Unit Test Specification              | 단위 테스트 명세서       |
| SW-ADS-VR-001   | Software Verification Report                  | 검증 보고서              |
| <!-- TODO -->   | CAN DBC Specification                         | CAN 메시지 정의          |

## 4. 통합 전략 (Integration Strategy)

### 4.1 통합 순서 (Integration Order)

ISO 26262 Part 6, Clause 10.4.2에 따라 단계적(incremental) 통합 전략을 적용한다.

**Phase 1: 모듈 내 단위 통합 (Intra-module Integration)**

| 단계 | 통합 대상                                    | 의존성        |
|------|----------------------------------------------|---------------|
| 1-1  | common 모듈 내부 단위 통합                    | 없음 (기반)   |
| 1-2  | aeb-control 모듈 내부 단위 통합              | common        |
| 1-3  | control-module 모듈 내부 단위 통합           | common        |
| 1-4  | decision-module 모듈 내부 단위 통합          | common        |
| 1-5  | remote-control 모듈 내부 단위 통합           | common        |

**Phase 2: 모듈 간 통합 (Inter-module Integration)**

| 단계 | 통합 대상                                    | 인터페이스           |
|------|----------------------------------------------|----------------------|
| 2-1  | common + aeb-control                         | 내부 API             |
| 2-2  | common + control-module                      | 내부 API             |
| 2-3  | decision-module + control-module             | CAN 메시지           |
| 2-4  | aeb-control + control-module                 | CAN 메시지           |
| 2-5  | remote-control + decision-module             | CAN 메시지           |
| 2-6  | decision-module + CAN Gateway 연동           | CAN Gateway 인터페이스|

**Phase 3: 프로젝트 수준 통합 (Project-level Integration)**

| 단계 | 통합 대상                                    | 비고                   |
|------|----------------------------------------------|------------------------|
| 3-1  | 전 모듈 통합 (K-City 변형)                    | K-City 환경 구성       |
| 3-2  | 전 모듈 통합 (Gateway 변형)                   | Gateway 환경 구성      |
| 3-3  | 타겟 HW(NXP S32G) 통합 실행                  | 타겟 보드 검증         |

<!-- TODO: 각 통합 단계의 입출력 조건(entry/exit criteria) 상세 정의 -->

### 4.2 빌드 통합 구성

| 통합 빌드 구성            | USE_PRIVATE_IMPL | 대상 환경          | 목적                      |
|---------------------------|------------------|--------------------|---------------------------|
| Host integration (public) | OFF              | x86_64 호스트      | 인터페이스 검증            |
| Host integration (private)| ON               | x86_64 호스트      | 안전 구현 통합 검증        |
| Target integration        | ON               | NXP S32G (aarch64) | 타겟 실행 및 타이밍 검증   |

## 5. 통합 테스트 방법 (Integration Test Methods)

### 5.1 ISO 26262 Part 6, Table 11 적용

| 테스트 방법                                  | ASIL B | ASIL C | ASIL D | 적용 대상                    |
|----------------------------------------------|--------|--------|--------|------------------------------|
| 요구사항 기반 테스트                          | ++     | ++     | ++     | 전 통합 단계                 |
| 인터페이스 테스트                             | ++     | ++     | ++     | 전 통합 단계                 |
| 결함 주입 테스트                              | +      | +      | ++     | Phase 2~3 (CAN 통신 중심)   |
| 리소스 사용 테스트                            | +      | +      | ++     | Phase 3 (타겟 환경)         |

### 5.2 CAN 통신 통합 테스트 방법

| 방법                          | 설명                                                    |
|-------------------------------|---------------------------------------------------------|
| vCAN Loopback                 | 호스트 환경에서 가상 CAN을 이용한 모듈 간 메시지 교환    |
| CAN Stub/Mock                 | 미통합 모듈을 Mock으로 대체하여 단계적 검증              |
| CAN Bus Monitor               | 실물 CAN Bus에서 메시지 캡처 및 분석 (candump 등)       |
| CAN Gateway Simulation        | Gateway 동작을 시뮬레이션하여 외부 데이터 수신 테스트    |

<!-- TODO: 각 CAN 테스트 방법의 구체적 설정 및 도구 기술 -->

## 6. 통합 테스트 케이스 (Integration Test Cases)

### 6.1 Phase 1: 모듈 내 통합 테스트

| Test Case ID   | 통합 대상           | 테스트 설명                                     | 관련 SWSR        |
|----------------|---------------------|-------------------------------------------------|------------------|
| IT-P1-CMN-001  | common 내부         | CAN 파서 + 로깅 연동 시 정상 동작               | SWSR-CMN-001/002 |
| IT-P1-AEB-001  | aeb-control 내부    | AEB 판단 → 제동 명령 생성 연동                  | SWSR-AEB-001     |
| IT-P1-CTL-001  | control-module 내부 | 조향/가감속 제어 → CAN 송신 연동                | SWSR-CTL-001     |
| IT-P1-DEC-001  | decision-module 내부| 경로 계획 → 주행 판단 연동                      | SWSR-DEC-001     |
| IT-P1-RMT-001  | remote-control 내부 | 원격 명령 수신 → 통신 상태 감시 연동            | SWSR-RMT-001     |
| <!-- TODO: 추가 Phase 1 테스트 케이스 --> | | | |

### 6.2 Phase 2: 모듈 간 통합 테스트

| Test Case ID   | 통합 대상                           | 테스트 설명                                    | 관련 SWSR        |
|----------------|-------------------------------------|------------------------------------------------|------------------|
| IT-P2-001      | decision → control (CAN)            | 경로 명령 CAN 전송 및 수신 정합성               | SWSR-DEC-001, SWSR-CTL-001 |
| IT-P2-002      | aeb-control → control (CAN)         | AEB 개입 명령 전달 및 제어 반응                 | SWSR-AEB-001     |
| IT-P2-003      | remote → decision (CAN)             | 원격 조종 명령의 판단 모듈 전달 정확성          | SWSR-RMT-001     |
| IT-P2-004      | CAN Gateway → decision              | 외부 데이터 수신 및 유효성 검증                 | SWSR-DEC-003     |
| IT-P2-005      | aeb + control 동시 제어             | AEB 개입 시 control-module 명령 우선순위 검증   | SWSR-AEB-002     |
| IT-P2-006      | CAN 타임아웃 시나리오               | 모듈 간 CAN 메시지 타임아웃 시 안전 동작 검증   | SWSR-AEB-003     |
| <!-- TODO: 추가 Phase 2 테스트 케이스 --> | | | |

### 6.3 Phase 3: 프로젝트 수준 통합 테스트

| Test Case ID   | 변형              | 테스트 설명                                       | 관련 SWSR        |
|----------------|-------------------|---------------------------------------------------|------------------|
| IT-P3-KC-001   | K-City            | K-City 환경 전 모듈 통합 End-to-End 시나리오      | 전체             |
| IT-P3-KC-002   | K-City            | K-City 환경 긴급 정지 시나리오                     | SWSR-AEB-001/002 |
| IT-P3-GW-001   | Gateway           | Gateway 환경 전 모듈 통합 End-to-End 시나리오     | 전체             |
| IT-P3-GW-002   | Gateway           | Gateway 통신 단절 시 Failsafe 동작                | SWSR-RMT-001     |
| IT-P3-TGT-001  | NXP S32G 타겟     | 타겟 보드에서 전 모듈 실행 및 타이밍 검증          | 전체             |
| IT-P3-TGT-002  | NXP S32G 타겟     | 타겟 보드에서 CAN 실물 통신 검증                   | 전체             |
| <!-- TODO: 추가 Phase 3 테스트 케이스 --> | | | |

### 6.4 USE_PRIVATE_IMPL 통합 테스트

| Test Case ID   | 테스트 설명                                                         | 관련 SWSR        |
|----------------|---------------------------------------------------------------------|------------------|
| IT-PVT-001     | USE_PRIVATE_IMPL=ON 전체 빌드 통합 정상 동작                       | SWSR-PVT-001     |
| IT-PVT-002     | USE_PRIVATE_IMPL=OFF 전체 빌드 통합 (Public stub) 정상 동작        | SWSR-PVT-001     |
| IT-PVT-003     | Private ↔ Public 전환 후 인터페이스 호환성 (링크 에러 없음) 확인   | SWSR-PVT-002     |
| <!-- TODO: 추가 Private Impl 통합 테스트 케이스 --> | | |

## 7. 통합 테스트 환경 (Integration Test Environment)

### 7.1 호스트 통합 테스트 환경

| 항목              | 내용                                              |
|-------------------|---------------------------------------------------|
| 호스트 OS         | <!-- TODO: Ubuntu/CentOS 버전 -->                 |
| 가상 CAN          | <!-- TODO: vcan 설정 (ip link add ...) -->        |
| CAN 분석 도구     | <!-- TODO: can-utils (candump, cansend 등) -->    |
| 시뮬레이터        | <!-- TODO: CAN Gateway 시뮬레이터 유무 -->        |
| 빌드 구성         | Project-level decision build 사용                  |

### 7.2 타겟 통합 테스트 환경

| 항목              | 내용                                              |
|-------------------|---------------------------------------------------|
| 타겟 HW           | NXP S32G Evaluation Board                         |
| CAN 인터페이스    | <!-- TODO: CAN 채널 수, 전송률(baud rate) -->     |
| 외부 장비         | <!-- TODO: 오실로스코프, CAN 분석기 등 -->        |
| 전원 조건         | <!-- TODO: 전원 공급 조건 -->                     |

<!-- TODO: 테스트 환경 구성도(diagram) 첨부 -->

## 8. 합격 기준 (Pass Criteria)

| 기준 항목                        | 기준값                                    |
|----------------------------------|-------------------------------------------|
| 전체 통합 테스트 통과율           | 100%                                      |
| CAN 메시지 정합성                 | 전 인터페이스 100% 일치                    |
| 타이밍 요구사항 충족 (타겟)       | 지정 주기/응답 시간 이내                   |
| 안전 메커니즘 동작 검증           | 모든 에러 시나리오에서 안전 상태 전환 확인  |
| 미결 이슈                         | Critical/Major 0건                        |

## 9. 통합 테스트 결과 기록 양식 (Test Result Template)

| 필드                  | 내용                                        |
|-----------------------|---------------------------------------------|
| 테스트 실행일          | <!-- TODO: YYYY-MM-DD -->                  |
| 통합 Phase             | Phase 1 / Phase 2 / Phase 3               |
| 실행 환경              | 호스트 vCAN / 타겟 NXP S32G               |
| 빌드 구성              | K-City / Gateway, USE_PRIVATE_IMPL=ON/OFF |
| 총 테스트 수           | <!-- TODO -->                              |
| Pass / Fail / Skip    | <!-- TODO --> / <!-- TODO --> / <!-- TODO --> |
| 발견 결함 수           | <!-- TODO -->                              |
| 실행자                 | <!-- TODO -->                              |

<!-- TODO: 결함 추적 및 재테스트 절차 기술 -->

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | <!-- TODO --> | <!-- TODO --> | Initial draft 작성    |
