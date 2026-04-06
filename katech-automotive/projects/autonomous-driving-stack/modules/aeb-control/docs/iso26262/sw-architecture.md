# AEB Control — 소프트웨어 아키텍처 설계 (Software Architectural Design)

> ISO 26262-6:2018, Clause 7 — Software architectural design

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-ARCH-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 7 |
| **ASIL 등급** | ASIL D |
| **대상 HW** | NXP S32G (aarch64) |
| **작성일** | 2026-04-06 |
| **작성자** | SuYeol Kim |
| **검토자** | <!-- 대기 중: 검토자 지정 후 갱신 --> |
| **승인자** | <!-- 대기 중: 승인자 지정 후 갱신 --> |
| **버전** | 1.0 |
| **상태** | Released |

---

## 1. 목적 (Purpose)

본 문서는 AEB Control 모듈의 소프트웨어 아키텍처를 정의한다.
ISO 26262-6 Clause 7에 따라 소프트웨어 안전 요구사항(SW-AEB-REQ-001)을 충족하는
아키텍처 설계를 기술하며, 모듈 분해, 인터페이스, 데이터 흐름, 안전 메커니즘을 명세한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-REQ-001 | AEB SW Safety Requirements | Part 6, Clause 6 |
| SW-AEB-UD-001 | AEB SW Unit Design | Part 6, Clause 8 |
| SW-AEB-IT-001 | AEB SW Integration Test | Part 6, Clause 10 |
| SW-AEB-VR-001 | AEB SW Verification Report | Part 6, Clause 11 |
| <!-- 대기 중: HSI 문서 확정 후 갱신 --> | HW-SW Interface Specification | HSI 문서 |

---

## 3. 아키텍처 개요 (Architecture Overview)

### 3.1 시스템 컨텍스트 (System Context)

```
┌─────────────────────────────────────────────────────┐
│                  AEB Control Module                  │
│                  (NXP S32G / aarch64)                │
│                                                     │
│  ┌──────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ Sensor   │→│  Decision    │→│  Actuator    │  │
│  │ Input    │  │  Engine      │  │  Command     │  │
│  │ Handler  │  │  (TTC-based) │  │  Dispatcher  │  │
│  └──────────┘  └──────────────┘  └──────────────┘  │
│       ↑              ↑                   ↓          │
│  ┌──────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ CAN Rx   │  │  Config      │  │  CAN Tx      │  │
│  │(SignalCodec)│  │  Manager     │  │(CanSpecProv.)│  │
│  └──────────┘  └──────────────┘  └──────────────┘  │
└─────────────────────────────────────────────────────┘
         ↑                                   ↓
    [센서 CAN Bus]                   [브레이크 CAN Bus]
```

### 3.2 소프트웨어 컴포넌트 분해 (Component Decomposition)

| 컴포넌트 | 소스 파일 | 책임 | ASIL |
|----------|-----------|------|------|
| AEBControl (Main) | `src/AEBControl_S32G_v2.cpp` | 메인 제어 루프, 컴포넌트 오케스트레이션 | D |
| Communicator | `src/Communicator.cpp` | CAN 송수신 관리 | D |
| CanSpecProvider | `../common/src/` | CAN 메시지 스펙 제공 | D |
| CrcProvider | `../common/src/` | CRC 계산 및 검증 | D |
| SignalCodec | `../common/src/` | CAN 신호 인코딩/디코딩 | D |
| ConfigManager | `../common/src/` (ConfigParser) | 설정 파일 로드 및 파라미터 검증 | D |
| SafetyMonitor | <!-- 대기 중: 설계 완료 후 갱신 --> | 안전 메커니즘 감시 (watchdog, 진단) | D |

---

## 4. 정적 아키텍처 (Static Architecture)

### 4.1 컴포넌트 의존성 (Component Dependencies)

```
AEBControl_S32G_v2
    ├── Communicator
    │       ├── CanSpecProvider
    │       ├── CrcProvider
    │       └── SignalCodec
    ├── ConfigManager
    │       └── config.ini → ../../config/aeb-control.ini
    └── SafetyMonitor  <!-- 대기 중: 구현 완료 후 갱신 -->
            ├── Watchdog Timer
            └── Diagnostic Handler
```

### 4.2 공통 라이브러리 의존성 (Common Library Dependencies)

| 라이브러리 | 경로 | 용도 |
|-----------|------|------|
| CanSpecProvider | `../common/src/`, `../common/include/` | CAN DB 기반 메시지 스펙 |
| CrcProvider | `../common/src/`, `../common/include/` | E2E CRC 보호 |
| SignalCodec | `../common/src/`, `../common/include/` | 물리값 ↔ raw 변환 |
| Private impl | `../common/src/private/aeb-control/` | AEB 전용 구현 (선택) |

---

## 5. 동적 아키텍처 (Dynamic Architecture)

### 5.1 메인 실행 루프 (Main Execution Loop)

```
[시작] → ConfigManager.load()
            ↓
      ┌─→ Communicator.receiveCanMsg()
      │       ↓
      │   SignalCodec.decode() → 센서 데이터 파싱
      │       ↓
      │   CrcProvider.verify() → E2E CRC 검증
      │       ↓
      │   DecisionEngine.evaluate(sensorData)
      │       ↓ (TTC 계산 및 제동 판단)
      │   CanSpecProvider.encode(brakeCmd)
      │       ↓
      │   CrcProvider.compute() → CRC 첨부
      │       ↓
      │   Communicator.sendCanMsg(brakeCmd)
      │       ↓
      │   SafetyMonitor.check() → Watchdog kick
      │       ↓
      └── [주기 반복: ≤10 ms]
```

### 5.2 타이밍 요구사항 (Timing Requirements)

| 항목 | 목표 | SWREQ 참조 | 비고 |
|------|------|------------|------|
| 메인 루프 주기 | ≤ 10 ms | SWREQ-AEB-NF-001 | 주기적 실행 |
| 센서 → 판단 지연 | ≤ 20 ms | SWREQ-AEB-001 | 처리 지연 |
| E2E 지연 (센서 → CAN 출력) | ≤ 50 ms | SWREQ-AEB-002 | 전체 경로 |
| WCET | <!-- 대기 중: 타겟 보드 검증 후 갱신 --> | SWREQ-AEB-NF-003 | 최악 실행 시간 |

---

## 6. 인터페이스 명세 (Interface Specification)

### 6.1 외부 인터페이스

| 인터페이스 | 방향 | 프로토콜 | 내용 |
|-----------|------|----------|------|
| 센서 CAN 수신 | Input | CAN 2.0B / CAN-FD | 레이더/카메라 퓨전 결과 (거리, 속도) |
| 브레이크 CAN 송신 | Output | CAN 2.0B / CAN-FD | 제동 요청, 목표 감속도 |
| Config 파일 | Input | INI 파일 | 파라미터 설정 |
| 진단 출력 | Output | <!-- 대기 중: 진단 프로토콜 확정 후 갱신 --> | DTC, 상태 보고 |

### 6.2 내부 인터페이스

| From | To | 데이터 | 비고 |
|------|----|--------|------|
| Communicator | DecisionEngine | SensorData struct | 대기 중: 구조체 상세 정의 후 갱신 |
| DecisionEngine | Communicator | BrakeCommand struct (CAN ID: 0x160) | 대기 중: 구조체 상세 정의 후 갱신 |
| ConfigManager | All | ConfigParams | 초기화 시 1회 전달 |

---

## 7. 안전 메커니즘 (Safety Mechanisms)

### 7.1 오류 검출 (Error Detection)

| 메커니즘 | 대상 | SWREQ 참조 | 구현 상태 |
|----------|------|------------|-----------|
| E2E CRC 검증 | CAN 메시지 | SWREQ-AEB-004 | CrcProvider 활용 |
| 센서 데이터 범위 검사 | 입력 데이터 | SWREQ-AEB-003 | 대기 중: 구현 완료 후 갱신 |
| Alive counter 검사 | CAN 메시지 | SWREQ-AEB-003 | 대기 중: 구현 완료 후 갱신 |
| Watchdog 타이머 | 실행 주기 | SWREQ-AEB-002 | 대기 중: 구현 완료 후 갱신 |
| Config 파라미터 범위 검증 | 설정값 | SWREQ-AEB-005 | ConfigParser 활용 |

### 7.2 오류 처리 (Error Handling)

| 오류 유형 | 처리 방법 | 안전 상태 |
|----------|-----------|-----------|
| 센서 데이터 timeout | 안전 상태 전환 (대기 중: 상세 정의 후 갱신) | 대기 중 |
| CAN 송신 실패 | 재전송 (최대 3회) → Fail-safe | 대기 중 |
| CRC 불일치 | 데이터 폐기, 이전 유효 데이터 사용 | CrcProvider 활용 |
| Config 오류 | 기본 안전 파라미터 적용 | Degraded |
| WCET 초과 | 대기 중: 정의 필요 | 대기 중 |

---

## 8. 설계 원칙 및 제약 (Design Principles & Constraints)

### 8.1 ASIL D 설계 원칙

- **모듈성 (Modularity)**: 컴포넌트 간 명확한 인터페이스 분리
- **캡슐화 (Encapsulation)**: 내부 상태 직접 접근 금지
- **단방향 데이터 흐름**: 센서 입력 → 판단 → 명령 출력
- **방어적 프로그래밍**: 모든 입력 검증, 범위 검사
- **동적 메모리 금지**: 런타임 중 `new`/`malloc` 사용 금지

### 8.2 빌드 및 배포

| 항목 | 내용 |
|------|------|
| 빌드 명령 | `./build.sh` |
| 출력 바이너리 | `build/run_aeb` |
| 타겟 아키텍처 | aarch64 (NXP S32G) |
| 컴파일러 | GCC 11.x (host), aarch64-linux-gnu-g++ (target) |
| C++ 표준 | C++17 (`-Wall -Wextra -Wpedantic -Wconversion`) |
| 코딩 표준 | MISRA C++:2008 (clang-tidy 기반 검증) |
| 빌드 시스템 | CMake 3.x with FetchContent |
| 정적 분석 | clang-tidy (프로젝트 루트 .clang-tidy), cppcheck |

---

## 9. 아키텍처 평가 기준 (Architecture Evaluation Criteria)

ISO 26262-6 Table 2에 따른 아키텍처 설계 원칙 적용 현황:

| 원칙 | 적용 여부 | 비고 |
|------|-----------|------|
| 계층적 구조 (Hierarchical structure) | 적용 | 컴포넌트 계층 분리 |
| 적절한 세분화 (Appropriate granularity) | 적용 | 소스 파일별 기능 분리: AEBControl_S32G_v2.cpp, Communicator.cpp |
| 간결한 인터페이스 (Restricted interface complexity) | 적용 | 구조체 기반 데이터 전달 |
| 높은 응집도 (High cohesion) | 적용 | 기능별 컴포넌트 분리 |
| 낮은 결합도 (Low coupling) | 적용 | CrcProvider, SignalCodec, CanSpecProvider, ConfigParser를 통한 인터페이스 기반 의존 |

---

## 10. TODO 항목

- [ ] 컴포넌트 상세 인터페이스(구조체, 함수 시그니처) 정의
- [ ] SafetyMonitor 컴포넌트 설계 및 구현
- [x] 타이밍 요구사항 수치 확정 — 메인 루프 ≤10ms, E2E ≤50ms
- [ ] 안전 상태(Safe State) 천이 다이어그램 작성
- [ ] WCET 분석 계획 수립
- [x] 컴파일러 및 빌드 도구 버전 확정 — GCC 11.x, CMake 3.x
- [ ] 공통 라이브러리(common) 결합도 분석
- [ ] HW-SW Interface (HSI) 문서 참조 추가
- [ ] 아키텍처 리뷰 수행 및 기록
- [x] 정적 분석 도구 적용 — clang-tidy (MISRA C++:2008), cppcheck
