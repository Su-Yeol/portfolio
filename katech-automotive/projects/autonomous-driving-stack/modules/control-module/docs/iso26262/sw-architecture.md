# Software Architecture Description

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-ARCH-001                                           |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 7 — Software Architectural Design        |
| **ASIL Scope**        | ASIL C                                                    |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Author**            | SuYeol Kim                                                |
| **Reviewer**          | <!-- 대기 중: 검토자 지정 후 갱신 -->                       |
| **Approval Date**     | 2026-04-06                                                |
| **Status**            | Released                                                  |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Control Module의 소프트웨어 아키텍처를 기술한다. 횡방향(조향) 및 종방향(가감속) 제어 기능의 구조적 분해, 컴포넌트 간 인터페이스, 데이터 흐름, 타이밍 설계 및 안전 메커니즘을 정의한다.

## 2. 범위 (Scope)

- Control Module 내부 소프트웨어 컴포넌트 구조
- 컴포넌트 간 인터페이스 및 데이터 흐름
- 제어 알고리즘 아키텍처 (Pure Pursuit, SCC)
- CAN 통신 레이어 아키텍처
- 안전 메커니즘 아키텍처 (모니터링, 출력 제한, 안전 전이)
- 빌드 산출물: `build/run_control`

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-IT-001     | Software Integration Test Specification       | 통합 테스트 문서        |
| <!-- 대기 중: TSC 문서 확정 후 갱신 --> | Technical Safety Concept (TSC) | 상위 안전 컨셉 참조 |

## 4. 용어 및 약어 (Terms and Abbreviations)

| 약어     | 정의                                                      |
|----------|-----------------------------------------------------------|
| MPC      | Model Predictive Control — 모델 예측 제어                 |
| PID      | Proportional-Integral-Derivative Controller               |
| CAN      | Controller Area Network                                   |
| EPS      | Electric Power Steering — 전동 파워 스티어링              |
| ESC      | Electronic Stability Control — 전자식 안정성 제어         |
| HAL      | Hardware Abstraction Layer                                |
| WCET     | Worst-Case Execution Time                                 |

## 5. 아키텍처 개요 (Architecture Overview)

### 5.1 정적 구조 (Static Structure)

<!-- 대기 중: UML Component Diagram 삽입 -->

Control Module은 다음 주요 컴포넌트로 구성된다:

```
┌─────────────────────────────────────────────────────────┐
│                    Control Module                        │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────────┐  │
│  │  Input        │  │  Control     │  │  Output       │  │
│  │  Manager      │──│  Engine      │──│  Manager      │  │
│  │              │  │              │  │               │  │
│  │ - Path recv  │  │ - Lat Ctrl   │  │ - CAN Tx      │  │
│  │ - Vel recv   │  │ - Lon Ctrl   │  │ - Cmd limit   │  │
│  │ - Validity   │  │ - MPC/PID    │  │ - Alive cnt   │  │
│  └──────────────┘  └──────────────┘  └───────────────┘  │
│         │                  │                  │          │
│  ┌──────────────────────────────────────────────────┐   │
│  │              Safety Monitor                       │   │
│  │  - Plausibility check  - Output limiter           │   │
│  │  - Timeout watchdog    - Safe state transition    │   │
│  └──────────────────────────────────────────────────┘   │
│         │                                               │
│  ┌──────────────────────────────────────────────────┐   │
│  │              Platform / HAL                        │   │
│  │  - CAN driver (FlexCAN)  - Timer service          │   │
│  │  - Shared memory IPC     - Config loader          │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 5.2 컴포넌트 설명 (Component Description)

| 컴포넌트          | 역할                                                              | ASIL  |
|-------------------|-------------------------------------------------------------------|-------|
| Input Manager     | Decision Module로부터 목표 경로/속도 수신, 입력 유효성 검사       | C     |
| Control Engine    | 횡/종방향 제어 알고리즘 연산 (Pure Pursuit lateral, SCC longitudinal) | C     |
| Output Manager    | CAN 프레임 구성, alive counter/CRC 생성, 액추에이터 명령 전송     | C     |
| Safety Monitor    | 출력 범위 검증, plausibility check, 안전 상태 전이 관리           | C     |
| Platform / HAL    | HW 추상화, CAN 드라이버, 타이머, 공유 메모리 IPC                  | QM    |

## 6. 데이터 흐름 (Data Flow)

### 6.1 제어 루프 데이터 흐름

<!-- 대기 중: 데이터 흐름 다이어그램 삽입 -->

```
Decision Module ──(경로/속도 프로파일)──> Input Manager
                                            │
                                     (validated input)
                                            │
                                            v
                                      Control Engine
                                       ├── Lateral Controller ──> steering_cmd
                                       └── Longitudinal Controller ──> accel_cmd / brake_cmd
                                            │
                                     (raw commands)
                                            │
                                            v
                                      Safety Monitor ──(제한/검증)──> Output Manager
                                                                          │
                                                                   (CAN frame)
                                                                          │
                                                                          v
                                                                    EPS / ESC Actuator
```

### 6.2 주요 데이터 인터페이스

| 인터페이스              | 소스              | 목적지            | 데이터                          | 주기         |
|-------------------------|-------------------|-------------------|---------------------------------|--------------|
| 경로 입력               | Decision Module   | Input Manager     | 목표 경로 waypoints             | 50 ms |
| 속도 프로파일 입력      | Decision Module   | Input Manager     | 목표 속도/가속도 프로파일       | 50 ms |
| 조향 명령 출력          | Output Manager    | EPS (CAN)         | 목표 조향각 (±400 deg saturation) | 50 ms |
| 가감속 명령 출력        | Output Manager    | ESC (CAN)         | 가속 토크, 제동 압력            | 50 ms |
| 차량 상태 피드백        | Vehicle CAN       | Input Manager     | 현재 속도, 조향각, yaw rate     | 50 ms |

## 7. 제어 알고리즘 아키텍처 (Control Algorithm Architecture)

### 7.1 횡방향 제어기 (Lateral Controller)

- **Primary**: Pure Pursuit 기반 경로 추종 제어기
  - Lookahead distance: 속도 비례 동적 산출
  - 차량 모델: Kinematic bicycle model (wheelbase = 2.865m)
  - 조향각 출력 범위: ±400 degrees (핸들 saturation)
  - 소스 파일: `src/Controller.cpp`, `src/PathManager.cpp`
- **Fallback**: <!-- 대기 중: fallback 전략 정의 후 갱신 -->

### 7.2 종방향 제어기 (Longitudinal Controller)

- **Primary**: SCC (Smart Cruise Control) 기반 속도 추종 제어기
  - 소스 파일: `src/Controller.cpp`
  - 가감속 명령 생성
- **Feedforward**: <!-- 대기 중: 경사 보상 로직 정의 후 갱신 -->
- **Brake blending**: <!-- 대기 중: 제동 배분 로직 정의 후 갱신 -->

## 8. 안전 메커니즘 아키텍처 (Safety Mechanisms)

<!-- 대기 중: 각 안전 메커니즘의 상세 설계 완성 -->

| 안전 메커니즘                | 설명                                                        | 대상 위험               |
|------------------------------|-------------------------------------------------------------|-------------------------|
| Output Range Limiter         | 조향각/토크 명령의 물리적 한계 초과 방지                     | 과도한 조향/가속        |
| Rate Limiter                 | 명령 변화율 제한 (slew rate limiting)                        | 급격한 조향/가속 변화   |
| Input Timeout Watchdog       | 입력 데이터 수신 타임아웃 감시                               | 통신 두절               |
| Plausibility Check           | 제어 입출력 간 일관성 검증                                   | 센서/연산 오류          |
| CAN Alive Counter & CRC      | 전송 메시지 무결성 확보                                      | 통신 오류               |
| Safe State Transition        | 이상 감지 시 안전 상태(제어 출력 0 / 비상 제동)로 전이       | 모든 안전 위반          |
| Redundant Path Check         | <!-- 대기 중: 이중 경로 검증 메커니즘 정의 -->               | 단일 경로 오류          |

## 9. 타이밍 아키텍처 (Timing Architecture)

| 항목                          | 목표값                  | 비고                        |
|-------------------------------|-------------------------|-----------------------------|
| 제어 루프 주기 (MainCycle)    | 50 ms                     | 횡/종방향 동기 실행         |
| 제어 연산 WCET                | 대기 중: 타겟 보드 검증 후 갱신 | 루프 주기의 50% 이하 목표 |
| CAN Tx 전송 지연              | 대기 중: 타겟 보드 검증 후 갱신 | 명령 출력 → 버스 전송     |
| 입력 타임아웃 임계값          | 대기 중: 안전 분석 후 갱신 | 안전 전이 트리거            |
| 안전 상태 전이 시간           | 대기 중: 안전 분석 후 갱신 | 이상 감지 → 안전 출력       |

## 10. 소스 코드 구조 (Source Code Structure)

```
control-module/
├── src/                         # Module-specific sources
│   ├── ControlModule.cpp
│   ├── Communicator.cpp
│   ├── Controller.cpp
│   └── PathManager.cpp
├── config/
│   └── config.ini → ../../config/control-module.ini
├── build.sh                     # Build script → build/run_control
├── build/
│   └── run_control              # Output binary
└── (dependencies)
    ├── ../common/src/           # Shared utility sources
    ├── ../common/include/       # Shared headers
    └── ../common/src/private/control-module/  # Optional private impl
```

## 11. 아키텍처 설계 원칙 (Design Principles)

- **모듈화 (Modularity)**: 횡방향/종방향 제어기를 독립 컴포넌트로 분리
- **방어적 프로그래밍 (Defensive Programming)**: 모든 입력에 대한 범위/유효성 검사
- **결정론적 실행 (Deterministic Execution)**: 제어 루프의 일정한 실행 주기 보장
- **안전 독립성 (Safety Independence)**: Safety Monitor는 Control Engine과 독립적으로 동작
- **ASIL 분해 (ASIL Decomposition)**: ASIL C 등급 적용, Platform/HAL은 QM으로 분해

## 12. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | Fill concrete values: ASIL C, source files, timing, control algorithms |
