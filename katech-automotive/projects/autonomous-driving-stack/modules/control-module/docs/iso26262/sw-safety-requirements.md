# Software Safety Requirements Specification

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-REQ-001                                            |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 6 — Specification of Software Safety Requirements |
| **ASIL Scope**        | ASIL C                                                    |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Author**            | SuYeol Kim                                                |
| **Reviewer**          | <!-- 대기 중: 검토자 지정 후 갱신 -->                       |
| **Approval Date**     | 2026-04-06                                                |
| **Status**            | Released                                                  |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 6에 따라 Control Module의 소프트웨어 안전 요구사항을 정의한다. 본 모듈은 자율주행 스택에서 횡방향(Lateral) 조향 제어 및 종방향(Longitudinal) 가감속 제어를 수행하며, 차량 액추에이터에 대한 직접적인 제어 명령을 생성한다. 안전 관련 기능 결함 시 차량 동역학에 즉각적인 영향을 미치므로 ASIL C 등급의 엄격한 안전 요구사항을 적용한다.

## 2. 범위 (Scope)

- 횡방향 제어: 조향각 연산 및 EPS(Electric Power Steering) 명령 출력
- 종방향 제어: 가속/제동 토크 연산 및 액추에이터 명령 출력
- 제어 알고리즘: Pure Pursuit (횡방향), SCC (종방향) 제어기 구현
- CAN 통신 인터페이스를 통한 액추에이터 명령 전송
- NXP S32G 타겟 보드에서의 실시간 제어 루프 동작
- Decision Module 출력(경로/속도 프로파일)을 입력으로 수신

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-ARCH-001   | Software Architecture Description             | 아키텍처 설계 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-UT-001     | Software Unit Test Specification              | 유닛 테스트 문서        |
| <!-- 대기 중: TSC 문서 확정 후 갱신 --> | Technical Safety Concept (TSC) | 상위 안전 컨셉 참조 |
| <!-- 대기 중: 시스템 요구사항 확정 후 갱신 --> | System Safety Requirements Specification | 시스템 안전 요구사항 |
| <!-- 대기 중: HSI 문서 확정 후 갱신 --> | Hardware-Software Interface (HSI) Spec | HW/SW 인터페이스 정의 |

## 4. 용어 및 약어 (Terms and Abbreviations)

| 약어   | 정의                                                        |
|--------|-------------------------------------------------------------|
| EPS    | Electric Power Steering — 전동 파워 스티어링                |
| MPC    | Model Predictive Control — 모델 예측 제어                   |
| PID    | Proportional-Integral-Derivative Controller                 |
| CAN    | Controller Area Network                                     |
| ASIL   | Automotive Safety Integrity Level                           |
| S32G   | NXP S32G Vehicle Network Processor                          |
| FTTI   | Fault Tolerant Time Interval — 결함 허용 시간 간격          |
| EPS    | Electric Power Steering                                     |
| ESC    | Electronic Stability Control                                |
| WCET   | Worst-Case Execution Time — 최악 실행 시간                  |

## 5. 소프트웨어 안전 요구사항 도출 (Derivation of SW Safety Requirements)

<!-- 대기 중: 시스템 안전 요구사항에서 소프트웨어 안전 요구사항으로의 분해/할당 근거 작성 -->

### 5.1 상위 요구사항 추적 (Traceability to System Requirements)

| System Req ID   | SW Safety Req ID | 할당 근거                                          |
|-----------------|------------------|----------------------------------------------------|
| <!-- 대기 중: 시스템 요구사항 확정 후 갱신 --> | SW-CTL-SR-001 | 횡방향 제어 Pure Pursuit → 경로 추종 |
| <!-- 대기 중 --> | SW-CTL-SR-002 | 조향각 ±400° saturation 제한 |
| <!-- 대기 중 --> | SW-CTL-SR-010 | 종방향 제어 SCC → 속도 추종 |
| <!-- 대기 중 --> | SW-CTL-SR-020 | 제어 루프 50ms MainCycle |

## 6. 소프트웨어 안전 요구사항 목록 (Software Safety Requirements)

### 6.1 횡방향 제어 요구사항 (Lateral Control)

| Req ID         | 요구사항                                                                         | ASIL  | 검증 방법       | 상태     |
|----------------|----------------------------------------------------------------------------------|-------|-----------------|----------|
| SW-CTL-SR-001  | 횡방향 제어기(Pure Pursuit)는 목표 경로 대비 횡방향 편차를 허용 범위 이내로 유지해야 한다 (wheelbase=2.865m) | C     | Test / Analysis | Draft    |
| SW-CTL-SR-002  | 조향각 명령은 물리적 조향 한계(±400°, handle saturation) 이내로 제한되어야 한다 | C | Test / Review   | Draft    |
| SW-CTL-SR-003  | 조향각 변화율(slew rate)은 <!-- 대기 중: max rate 확정 후 갱신 -->°/s 이하로 제한되어야 한다 | C | Test / Analysis | Draft    |
| SW-CTL-SR-004  | EPS 통신 두절 감지 시 <!-- 대기 중: FTTI 확정 후 갱신 --> ms 이내에 안전 상태로 전이해야 한다 | C | Test            | Draft    |
| SW-CTL-SR-005  | <!-- TODO: 추가 횡방향 제어 요구사항 작성 -->                                     | <!-- TODO --> | <!-- TODO --> | Draft |

### 6.2 종방향 제어 요구사항 (Longitudinal Control)

| Req ID         | 요구사항                                                                         | ASIL  | 검증 방법       | 상태     |
|----------------|----------------------------------------------------------------------------------|-------|-----------------|----------|
| SW-CTL-SR-010  | 종방향 제어기(SCC)는 목표 속도 프로파일 대비 속도 편차를 허용 범위 이내로 유지해야 한다 | C     | Test / Analysis | Draft    |
| SW-CTL-SR-011  | 가속 토크 명령은 최대 허용 가속도(<!-- 대기 중: max accel 확정 후 갱신 --> m/s^2) 이내여야 한다 | C     | Test / Review   | Draft    |
| SW-CTL-SR-012  | 제동 명령은 최대 허용 감속도(<!-- 대기 중: max decel 확정 후 갱신 --> m/s^2) 이내여야 한다 | C     | Test / Review   | Draft    |
| SW-CTL-SR-013  | 비상 제동 요청 수신 시 <!-- 대기 중: FTTI 확정 후 갱신 --> ms 이내에 최대 제동력을 인가해야 한다 | C     | Test            | Draft    |
| SW-CTL-SR-014  | <!-- TODO: 추가 종방향 제어 요구사항 작성 -->                                     | <!-- TODO --> | <!-- TODO --> | Draft |

### 6.3 제어 루프 및 타이밍 요구사항

| Req ID         | 요구사항                                                                         | ASIL  | 검증 방법       | 상태     |
|----------------|----------------------------------------------------------------------------------|-------|-----------------|----------|
| SW-CTL-SR-020  | 제어 루프 주기(MainCycle)는 50ms이며 주기 편차는 ±<!-- 대기 중: jitter 허용치 확정 후 갱신 -->% 이내여야 한다 | C | Test / Analysis | Draft |
| SW-CTL-SR-021  | 제어 연산 WCET는 제어 루프 주기의 50% (25ms) 이하여야 한다                        | C     | Analysis        | Draft    |
| SW-CTL-SR-022  | CAN 메시지 전송 지연은 <!-- 대기 중: 확정 후 갱신 --> ms 이내여야 한다            | C     | Test            | Draft    |
| SW-CTL-SR-023  | 입력 데이터(경로/속도) 타임아웃(<!-- 대기 중: timeout 확정 후 갱신 --> ms) 감지 시 안전 상태로 전이해야 한다 | C | Test            | Draft    |
| SW-CTL-SR-024  | <!-- TODO: 추가 타이밍 요구사항 작성 -->                                          | <!-- TODO --> | <!-- TODO --> | Draft |

### 6.4 CAN 통신 안전 요구사항

| Req ID         | 요구사항                                                                         | ASIL  | 검증 방법       | 상태     |
|----------------|----------------------------------------------------------------------------------|-------|-----------------|----------|
| SW-CTL-SR-030  | 액추에이터 CAN 명령에는 alive counter 및 CRC를 포함하여 전송해야 한다             | C     | Test / Review   | Draft    |
| SW-CTL-SR-031  | CAN 버스 오프(bus-off) 감지 시 <!-- 대기 중: FTTI 확정 후 갱신 --> ms 이내에 안전 상태로 전이해야 한다 | C  | Test            | Draft    |
| SW-CTL-SR-032  | <!-- TODO: 추가 CAN 통신 요구사항 작성 -->                                        | <!-- TODO --> | <!-- TODO --> | Draft |

## 7. 안전 요구사항 속성 (Safety Requirements Attributes)

<!-- 대기 중: 각 요구사항에 대해 아래 속성을 확정 -->

각 소프트웨어 안전 요구사항은 다음 속성을 포함해야 한다:

- **ASIL 등급**: 해당 요구사항에 할당된 ASIL (본 모듈은 ASIL C)
- **안전 메커니즘**: 위반 시 적용되는 안전 메커니즘 (예: output limiting, watchdog, plausibility check)
- **Fault Tolerance Time Interval (FTTI)**: 결함 허용 시간 간격
- **Safe state**: 해당 요구사항 위반 시 전이할 안전 상태 (예: 제어 출력 0, 비상 제동)
- **Degradation mode**: 성능 저하 운영 모드 정의

## 8. 하드웨어-소프트웨어 인터페이스 (HW/SW Interface)

<!-- 대기 중: NXP S32G 하드웨어와 액추에이터 인터페이스에 대한 상세 정의 -->

| 인터페이스 항목             | 설명                                                        |
|-----------------------------|-------------------------------------------------------------|
| CAN Controller              | NXP S32G FlexCAN 모듈을 통한 액추에이터 명령 송수신         |
| Timer / Counter             | 제어 루프 주기 생성을 위한 하드웨어 타이머                   |
| GPIO / PWM                  | <!-- 대기 중: 직접 액추에이터 구동 시 GPIO/PWM 인터페이스 정의 --> |
| Shared Memory               | Decision Module과의 데이터 교환을 위한 공유 메모리 인터페이스 |
| Watchdog                    | <!-- 대기 중: 하드웨어 watchdog 연동 메커니즘 정의 -->       |

## 9. 검증 기준 (Verification Criteria)

<!-- 대기 중: 각 요구사항의 합격/불합격 기준 상세 기술 -->

- 횡방향 편차 허용 범위: ±<!-- 대기 중: 허용 오차 확정 후 갱신 --> m
- 종방향 속도 편차 허용 범위: ±<!-- 대기 중: 허용 오차 확정 후 갱신 --> km/h
- 제어 루프 주기 (MainCycle): 50ms, jitter: ±<!-- 대기 중: 확정 후 갱신 --> ms 이내
- CAN 메시지 전송 성공률: <!-- 대기 중: 확정 후 갱신 -->% 이상
- 안전 상태 전이 응답 시간: <!-- 대기 중: 확정 후 갱신 --> ms 이내
- 비상 제동 응답 시간: <!-- 대기 중: 확정 후 갱신 --> ms 이내

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | Fill concrete values: ASIL C, Pure Pursuit/SCC, 50ms cycle, ±400° steering |
