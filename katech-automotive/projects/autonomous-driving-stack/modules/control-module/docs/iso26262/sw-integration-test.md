# Software Integration Test Specification

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-IT-001                                             |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 10 — Software Integration and Testing    |
| **ASIL Scope**        | ASIL C                                                    |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Language**          | C++17                                                     |
| **Author**            | SuYeol Kim                                                |
| **Reviewer**          | <!-- 대기 중: 검토자 지정 후 갱신 -->                       |
| **Approval Date**     | 2026-04-06                                                |
| **Status**            | Released                                                  |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 10에 따라 Control Module의 소프트웨어 통합 테스트 사양을 정의한다. 개별 유닛이 통합되었을 때 아키텍처 설계(SW-CTL-ARCH-001)에서 정의한 컴포넌트 간 인터페이스, 데이터 흐름, 타이밍이 올바르게 동작하는지 검증한다. 또한 외부 모듈(Decision Module) 및 하드웨어 인터페이스(CAN, Timer)와의 통합을 검증한다.

## 2. 범위 (Scope)

- Control Module 내부 컴포넌트 간 통합 테스트
- 제어 파이프라인 End-to-End 테스트 (Input → Control Engine → Output)
- Decision Module과의 인터페이스 통합 테스트
- CAN 통신 인터페이스 통합 테스트
- 타이밍 및 실시간 제약 검증
- 안전 메커니즘 통합 동작 검증
- 공통 라이브러리(`../common/`) 연동 검증

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-ARCH-001   | Software Architecture Description             | 아키텍처 설계 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-UT-001     | Software Unit Test Specification              | 유닛 테스트 문서        |
| SW-CTL-VR-001     | Software Verification Report                  | 검증 보고서             |

## 4. 통합 전략 (Integration Strategy)

### 4.1 통합 순서

ISO 26262 Part 6, Clause 10에 따라 점진적(incremental) 통합 방식을 적용한다:

```
Phase 1: 내부 컴포넌트 통합
  Step 1: Input Manager + Control Engine (횡방향)
  Step 2: Input Manager + Control Engine (종방향)
  Step 3: Control Engine + Safety Monitor
  Step 4: Safety Monitor + Output Manager
  Step 5: 전체 내부 파이프라인 통합

Phase 2: 플랫폼 통합
  Step 6: Output Manager + CAN HAL (FlexCAN 드라이버)
  Step 7: Input Manager + Shared Memory IPC
  Step 8: 제어 루프 + Timer Service

Phase 3: 외부 인터페이스 통합
  Step 9: Decision Module → Control Module 인터페이스
  Step 10: Control Module → Actuator(EPS/ESC) 인터페이스
```

### 4.2 테스트 방법 (ISO 26262 Part 6, Clause 10 Table 12)

| 테스트 방법                                | 적용 수준       | 비고                            |
|--------------------------------------------|-----------------|----------------------------------|
| 기능 테스트 (Functional testing)           | 강력 권고 (HR)  | 아키텍처 수준 기능 검증         |
| 인터페이스 테스트 (Interface testing)       | 강력 권고 (HR)  | 컴포넌트 간 데이터 교환 검증    |
| 결함 주입 테스트 (Fault injection)          | 권고 (R)        | ASIL C — 결함 시나리오 검증     |
| 자원 사용 테스트 (Resource usage testing)   | 강력 권고 (HR)  | CPU, 메모리, 스택 사용량 검증   |

### 4.3 테스트 환경

| 항목                | 내용                                                      |
|---------------------|-----------------------------------------------------------|
| 빌드 산출물         | `build/run_control` (via `./build.sh`)                    |
| 타겟 환경           | NXP S32G (aarch64) 또는 QEMU aarch64 에뮬레이션          |
| 호스트 환경         | Ubuntu 22.04 LTS, GCC 11.x, CMake 3.x                    |
| CAN 시뮬레이터      | <!-- 대기 중: CAN 시뮬레이터 구성 후 갱신 (e.g., SocketCAN vcan) --> |
| 입력 시뮬레이터     | <!-- 대기 중: Decision Module 출력 시뮬레이터 구성 후 갱신 --> |
| 계측 도구           | <!-- 대기 중: 타이밍 분석 도구 선정 후 갱신 (e.g., ftrace, perf) --> |

## 5. 통합 테스트 케이스 (Integration Test Cases)

### 5.1 Phase 1: 내부 컴포넌트 통합

#### 5.1.1 Input Manager ↔ Control Engine 통합

| TC ID          | 테스트 설명                                          | 사전 조건                               | 테스트 절차                            | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|----------------------------------------|-----------------|-------|
| IT-INT-001     | 경로 데이터 수신 → 횡방향 제어 연산 E2E 확인        | Input Manager, Lateral Controller 통합  | 유효한 경로 데이터 주입                | 유효한 조향 명령 출력                  | SW-CTL-SR-001   | Draft |
| IT-INT-002     | 속도 프로파일 수신 → 종방향 제어 연산 E2E 확인      | Input Manager, Longitudinal Ctrl 통합   | 유효한 속도 프로파일 주입              | 유효한 가감속 명령 출력                | SW-CTL-SR-010   | Draft |
| IT-INT-003     | 무효 입력 데이터 거부 및 전파 방지 확인              | 전체 내부 파이프라인                    | 범위 초과 / NaN 데이터 주입            | Input Manager에서 차단, Engine 미도달   | SW-CTL-SR-023   | Draft |
| IT-INT-004     | <!-- TODO: 추가 내부 통합 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->                          | <!-- TODO -->   | Draft |

#### 5.1.2 Control Engine ↔ Safety Monitor 통합

| TC ID          | 테스트 설명                                          | 사전 조건                               | 테스트 절차                            | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|----------------------------------------|-----------------|-------|
| IT-SAF-001     | 조향 명령 범위 제한 통합 확인                        | Lateral Ctrl + Safety Monitor           | 한계 초과 조향 유도                    | Safety Monitor가 출력 클램핑           | SW-CTL-SR-002   | Draft |
| IT-SAF-002     | 가속 명령 범위 제한 통합 확인                        | Longitudinal Ctrl + Safety Monitor      | 한계 초과 가속 유도                    | Safety Monitor가 출력 클램핑           | SW-CTL-SR-011   | Draft |
| IT-SAF-003     | 변화율 제한 통합 동작 확인                           | Control Engine + Safety Monitor         | 급격한 경로 변화 입력                  | 출력 변화율 제한 적용                  | SW-CTL-SR-003   | Draft |
| IT-SAF-004     | 안전 상태 전이 End-to-End 확인                       | 전체 내부 파이프라인                    | 입력 타임아웃 유발                     | 출력 0 전이 및 안전 상태 진입          | SW-CTL-SR-004   | Draft |
| IT-SAF-005     | <!-- TODO: 추가 안전 통합 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->                          | <!-- TODO -->   | Draft |

#### 5.1.3 Safety Monitor ↔ Output Manager 통합

| TC ID          | 테스트 설명                                          | 사전 조건                               | 테스트 절차                            | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|----------------------------------------|-----------------|-------|
| IT-OUT-001     | 정상 제어 명령의 CAN 프레임 출력 확인                | Safety Monitor + Output Manager         | 정상 범위 명령 입력                    | 올바른 CAN 프레임 생성 (alive, CRC)    | SW-CTL-SR-030   | Draft |
| IT-OUT-002     | 안전 상태 시 CAN 출력 확인                           | Safety Monitor + Output Manager         | 안전 상태 전이 유발                    | 안전 명령(0/비상 제동) CAN 출력        | SW-CTL-SR-031   | Draft |
| IT-OUT-003     | <!-- TODO: 추가 출력 통합 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.2 Phase 2: 플랫폼 통합

| TC ID          | 테스트 설명                                          | 사전 조건                               | 테스트 절차                            | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|----------------------------------------|-----------------|-------|
| IT-PLT-001     | CAN HAL 통합 — 프레임 전송 확인                      | Output Manager + FlexCAN 드라이버       | 명령 출력 → CAN 버스 확인              | CAN 버스에서 올바른 프레임 수신        | SW-CTL-SR-030   | Draft |
| IT-PLT-002     | Shared Memory IPC 통합 — Decision Module 데이터 수신 | Input Manager + SHM IPC                 | SHM에 경로 데이터 기록                 | Input Manager가 데이터 정상 수신       | SW-CTL-SR-023   | Draft |
| IT-PLT-003     | Timer Service 통합 — 제어 루프 주기 확인             | 전체 파이프라인 + Timer                 | 제어 루프 N회 실행                     | 주기 jitter가 허용 범위 이내           | SW-CTL-SR-020   | Draft |
| IT-PLT-004     | <!-- TODO: 추가 플랫폼 통합 테스트 -->               | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.3 Phase 3: 외부 인터페이스 통합

| TC ID          | 테스트 설명                                          | 사전 조건                               | 테스트 절차                            | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|----------------------------------------|-----------------|-------|
| IT-EXT-001     | Decision Module → Control Module 인터페이스 E2E      | 양 모듈 실행                            | Decision Module이 경로/속도 출력       | Control Module이 수신 및 제어 수행     | SW-CTL-SR-001, SR-010 | Draft |
| IT-EXT-002     | 제어 명령 → EPS 액추에이터 인터페이스 확인           | Control Module + EPS 시뮬레이터         | 조향 명령 출력                         | EPS 시뮬레이터에서 올바른 명령 수신    | SW-CTL-SR-002   | Draft |
| IT-EXT-003     | 제어 명령 → ESC 액추에이터 인터페이스 확인           | Control Module + ESC 시뮬레이터         | 가감속 명령 출력                       | ESC 시뮬레이터에서 올바른 명령 수신    | SW-CTL-SR-011   | Draft |
| IT-EXT-004     | <!-- TODO: 추가 외부 통합 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.4 결함 주입 테스트 (Fault Injection Tests)

| TC ID          | 테스트 설명                                          | 주입 결함                               | 기대 결과                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| IT-FI-001      | 입력 통신 두절 시 안전 전이 확인                     | Decision Module 데이터 중단             | FTTI 이내 안전 상태 전이               | SW-CTL-SR-004, SR-023 | Draft |
| IT-FI-002      | CAN 버스 오프 시 안전 전이 확인                      | CAN 드라이버 bus-off 유발               | FTTI 이내 안전 상태 전이               | SW-CTL-SR-031   | Draft |
| IT-FI-003      | 제어 연산 지연 시 안전 동작 확인                     | Control Engine 연산 지연 주입           | 이전 유효 출력 유지 또는 안전 전이     | SW-CTL-SR-021   | Draft |
| IT-FI-004      | 센서 피드백 오류 시 plausibility 감지 확인           | 비정상 차량 상태 피드백 주입            | Plausibility check 트리거             | SW-CTL-SR-020   | Draft |
| IT-FI-005      | <!-- TODO: 추가 결함 주입 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

## 6. 타이밍 검증 (Timing Verification)

<!-- 대기 중: 타겟 보드 검증 후 타이밍 측정 결과 기입 -->

| 측정 항목                      | 목표값              | 측정값              | 판정   |
|--------------------------------|---------------------|---------------------|--------|
| 제어 루프 주기 평균 (MainCycle) | 50 ms              | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| 제어 루프 주기 jitter (max)    | ±대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| 제어 연산 WCET                 | ≤25 ms (50% of 50ms) | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| CAN 전송 지연 (max)            | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| 입력 → 출력 E2E 지연 (max)    | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| 안전 상태 전이 시간 (max)      | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |

## 7. 자원 사용량 검증 (Resource Usage Verification)

<!-- 대기 중: 타겟 보드 검증 후 자원 사용량 측정 결과 기입 -->

| 자원 항목                | 한계값              | 측정값              | 판정   |
|--------------------------|---------------------|---------------------|--------|
| CPU 사용률 (peak)        | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| RAM 사용량               | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| 스택 사용량 (peak)       | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |
| CAN 버스 부하율          | 대기 중: 확정 후 갱신 | 대기 중: 타겟 보드 검증 후 갱신 | 대기 중 |

## 8. 테스트 추적 매트릭스 (Test Traceability Matrix)

<!-- 대기 중: 테스트 실행 후 추적 매트릭스 커버리지 상태 갱신 -->

| 아키텍처 인터페이스                  | 통합 테스트 ID                    | 커버리지 상태   |
|--------------------------------------|-----------------------------------|-----------------|
| Input Manager ↔ Control Engine       | IT-INT-001, IT-INT-002, IT-INT-003 | <!-- TODO -->  |
| Control Engine ↔ Safety Monitor      | IT-SAF-001 ~ IT-SAF-004          | <!-- TODO -->   |
| Safety Monitor ↔ Output Manager      | IT-OUT-001, IT-OUT-002            | <!-- TODO -->   |
| Output Manager ↔ CAN HAL            | IT-PLT-001                        | <!-- TODO -->   |
| Input Manager ↔ SHM IPC             | IT-PLT-002                        | <!-- TODO -->   |
| Decision Module ↔ Control Module     | IT-EXT-001                        | <!-- TODO -->   |

## 9. 테스트 결과 요약 (Test Result Summary)

<!-- 대기 중: 테스트 실행 후 결과 기입 -->

| 항목                        | 결과                |
|-----------------------------|---------------------|
| 총 테스트 케이스 수         | <!-- TODO -->       |
| 통과 (Pass)                 | <!-- TODO -->       |
| 실패 (Fail)                 | <!-- TODO -->       |
| 미실행 (Not Executed)       | <!-- TODO -->       |
| 발견된 결함 수              | <!-- TODO -->       |
| 해결된 결함 수              | <!-- TODO -->       |

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | Fill concrete values: ASIL C, test environment, timing targets |
