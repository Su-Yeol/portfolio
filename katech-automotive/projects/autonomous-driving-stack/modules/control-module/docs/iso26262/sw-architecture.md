# Software Architecture Description

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-ARCH-001                                           |
| **Version**           | 0.1 Draft                                                 |
| **ISO 26262 Reference** | Part 6, Clause 7 — Software Architectural Design        |
| **ASIL Scope**        | ASIL D (target) <!-- TODO: 최종 ASIL 등급 확정 후 업데이트 --> |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Author**            | <!-- TODO: 작성자 이름 기입 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 기입 -->                             |
| **Approval Date**     | <!-- TODO: 승인 일자 기입 (YYYY-MM-DD) -->                  |
| **Status**            | Draft                                                     |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Control Module의 소프트웨어 아키텍처를 기술한다. 횡방향(조향) 및 종방향(가감속) 제어 기능의 구조적 분해, 컴포넌트 간 인터페이스, 데이터 흐름, 타이밍 설계 및 안전 메커니즘을 정의한다.

## 2. 범위 (Scope)

- Control Module 내부 소프트웨어 컴포넌트 구조
- 컴포넌트 간 인터페이스 및 데이터 흐름
- 제어 알고리즘 아키텍처 (PID, MPC)
- CAN 통신 레이어 아키텍처
- 안전 메커니즘 아키텍처 (모니터링, 출력 제한, 안전 전이)
- 빌드 산출물: `build/run_control`

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-IT-001     | Software Integration Test Specification       | 통합 테스트 문서        |
| <!-- TODO -->     | Technical Safety Concept (TSC)                | 상위 안전 컨셉 참조     |

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

<!-- TODO: 아키텍처 다이어그램 삽입 (UML Component Diagram 또는 블록 다이어그램) -->

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
| Input Manager     | Decision Module로부터 목표 경로/속도 수신, 입력 유효성 검사       | D     |
| Control Engine    | 횡/종방향 제어 알고리즘 연산 (PID, MPC)                           | D     |
| Output Manager    | CAN 프레임 구성, alive counter/CRC 생성, 액추에이터 명령 전송     | D     |
| Safety Monitor    | 출력 범위 검증, plausibility check, 안전 상태 전이 관리           | D     |
| Platform / HAL    | HW 추상화, CAN 드라이버, 타이머, 공유 메모리 IPC                  | C     |

## 6. 데이터 흐름 (Data Flow)

### 6.1 제어 루프 데이터 흐름

<!-- TODO: 데이터 흐름 다이어그램 삽입 -->

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
| 경로 입력               | Decision Module   | Input Manager     | 목표 경로 waypoints             | <!-- TODO --> ms |
| 속도 프로파일 입력      | Decision Module   | Input Manager     | 목표 속도/가속도 프로파일       | <!-- TODO --> ms |
| 조향 명령 출력          | Output Manager    | EPS (CAN)         | 목표 조향각, 조향 토크          | <!-- TODO --> ms |
| 가감속 명령 출력        | Output Manager    | ESC (CAN)         | 가속 토크, 제동 압력            | <!-- TODO --> ms |
| 차량 상태 피드백        | Vehicle CAN       | Input Manager     | 현재 속도, 조향각, yaw rate     | <!-- TODO --> ms |

## 7. 제어 알고리즘 아키텍처 (Control Algorithm Architecture)

### 7.1 횡방향 제어기 (Lateral Controller)

<!-- TODO: 횡방향 제어 알고리즘 상세 아키텍처 기술 -->

- **Primary**: MPC 기반 경로 추종 제어기
  - 예측 구간(prediction horizon): <!-- TODO --> steps
  - 제어 구간(control horizon): <!-- TODO --> steps
  - 상태 변수: [lateral_error, heading_error, curvature, ...]
- **Fallback**: PID 기반 Stanley/Pure Pursuit 제어기
  - 전환 조건: <!-- TODO: MPC → PID fallback 조건 정의 -->

### 7.2 종방향 제어기 (Longitudinal Controller)

<!-- TODO: 종방향 제어 알고리즘 상세 아키텍처 기술 -->

- **Primary**: 속도 프로파일 추종 PID 제어기
  - P/I/D 게인: <!-- TODO: 초기 게인 값 또는 참조 -->
- **Feedforward**: 경사/저항 보상 (road grade compensation)
- **Brake blending**: 회생 제동 / 유압 제동 배분 로직

## 8. 안전 메커니즘 아키텍처 (Safety Mechanisms)

<!-- TODO: 각 안전 메커니즘의 상세 설계 완성 -->

| 안전 메커니즘                | 설명                                                        | 대상 위험               |
|------------------------------|-------------------------------------------------------------|-------------------------|
| Output Range Limiter         | 조향각/토크 명령의 물리적 한계 초과 방지                     | 과도한 조향/가속        |
| Rate Limiter                 | 명령 변화율 제한 (slew rate limiting)                        | 급격한 조향/가속 변화   |
| Input Timeout Watchdog       | 입력 데이터 수신 타임아웃 감시                               | 통신 두절               |
| Plausibility Check           | 제어 입출력 간 일관성 검증                                   | 센서/연산 오류          |
| CAN Alive Counter & CRC      | 전송 메시지 무결성 확보                                      | 통신 오류               |
| Safe State Transition        | 이상 감지 시 안전 상태(제어 출력 0 / 비상 제동)로 전이       | 모든 안전 위반          |
| Redundant Path Check         | <!-- TODO: 이중 경로 검증 메커니즘 정의 -->                  | 단일 경로 오류          |

## 9. 타이밍 아키텍처 (Timing Architecture)

| 항목                          | 목표값                  | 비고                        |
|-------------------------------|-------------------------|-----------------------------|
| 제어 루프 주기                | <!-- TODO: e.g., 10 --> ms | 횡/종방향 동기 실행       |
| 제어 연산 WCET                | <!-- TODO --> ms        | 루프 주기의 50% 이하 목표   |
| CAN Tx 전송 지연              | <!-- TODO --> ms        | 명령 출력 → 버스 전송       |
| 입력 타임아웃 임계값          | <!-- TODO --> ms        | 안전 전이 트리거            |
| 안전 상태 전이 시간           | <!-- TODO --> ms        | 이상 감지 → 안전 출력       |

## 10. 소스 코드 구조 (Source Code Structure)

```
control-module/
├── src/                         # Module-specific sources
│   └── <!-- TODO: 소스 파일 목록 -->
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
- **ASIL 분해 (ASIL Decomposition)**: <!-- TODO: ASIL 분해 적용 여부 및 근거 기술 -->

## 12. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | <!-- TODO: 날짜 --> | <!-- TODO: 작성자 --> | Initial draft |
