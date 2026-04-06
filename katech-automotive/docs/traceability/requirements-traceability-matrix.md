# Requirements Traceability Matrix

| Field | Value |
|---|---|
| **Document ID** | PRC-RTM-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 8, Clause 6 |
| **Author** | SuYeol Kim |
| **Reviewer** | - |
| **Approval Date** | 2026-04-06 |

---

## 1. Purpose

본 문서는 안전 목표(Safety Goal)부터 코드 및 테스트까지의 양방향 추적성(Bidirectional Traceability)을 제공한다.

## 2. Traceability Chain

```
Safety Goal → Functional Safety Req → Technical Safety Req → SW Safety Req → SW Design → Code → Test
```

## 3. Forward Traceability (Top-Down)

### 3.1 SG-001: 충돌 회피 — 자율 긴급 제동 (AEB)

| Safety Goal | FSR | TSR | SW Req | SW Design | Source File | Test Case |
|---|---|---|---|---|---|---|
| SG-001 | FSR-001 긴급 제동 기능 | TSR-AEB-001 TTC 기반 제동 판단 | SWREQ-AEB-001 긴급 제동 결정 (≤20ms) | SW-AEB-ARCH-001 §4 DecisionEngine | aeb-control/src/AEBControl_S32G_v2.cpp | TC-UD-001~005 (TTC 계산), TC-IT-040 (E2E 시나리오) |
| SG-001 | FSR-001 | TSR-AEB-002 E2E 지연 | SWREQ-AEB-002 제동 명령 지연 (≤50ms) | SW-AEB-ARCH-001 §4 Communicator | aeb-control/src/Communicator.cpp | TC-UD-010~013, TC-IT-042 (지연 측정) |
| SG-001 | FSR-001 | TSR-AEB-003 센서 고장 대응 | SWREQ-AEB-003 센서 데이터 고장 처리 | SW-AEB-ARCH-001 §4 SafetyMonitor | aeb-control/src/*.cpp | TC-FI-001~003, TC-IT-011 |
| SG-001 | FSR-001 | TSR-AEB-004 CAN 통신 오류 | SWREQ-AEB-004 CAN 통신 오류 처리 | SW-AEB-ARCH-001 §4 Communicator | aeb-control/src/Communicator.cpp | TC-UD-011, TC-UD-013, TC-FI-IT-001 |
| SG-001 | FSR-001 | TSR-AEB-005 설정 무결성 | SWREQ-AEB-005 설정 파일 무결성 | SW-AEB-ARCH-001 §4 ConfigManager | common/src/ConfigParser.cpp | TC-UD-020~022, TC-IT-020 |

### 3.2 SG-002: 차량 제어 — 조향/가감속

| Safety Goal | FSR | TSR | SW Req | SW Design | Source File | Test Case |
|---|---|---|---|---|---|---|
| SG-002 | FSR-002 횡방향 제어 | TSR-CTL-001 조향 정밀도 | SW-CTL-SR-001 횡방향 편차 허용 범위 | SW-CTL-ARCH-001 §4 Controller | control-module/src/Controller.cpp | TC-CTL-UT-001~005 |
| SG-002 | FSR-002 | TSR-CTL-002 조향 제한 | SW-CTL-SR-002 조향 각도 물리 제한 | SW-CTL-ARCH-001 §4 Controller | control-module/src/Controller.cpp | TC-CTL-UT-010 |
| SG-002 | FSR-003 종방향 제어 | TSR-CTL-010 속도 제어 | SW-CTL-SR-010 종방향 속도 편차 | SW-CTL-ARCH-001 §4 Controller | control-module/src/Controller.cpp | TC-CTL-UT-020~023 |
| SG-002 | FSR-003 | TSR-CTL-020 제어 루프 | SW-CTL-SR-020 제어 주기 준수 | SW-CTL-ARCH-001 §4 MainLoop | control-module/src/ControlModule.cpp | TC-CTL-IT-001 |
| SG-002 | FSR-003 | TSR-CTL-030 CAN 안전 | SW-CTL-SR-030 CAN alive counter + CRC | SW-CTL-ARCH-001 §4 Communicator | control-module/src/Communicator.cpp | TC-CTL-IT-010 |

### 3.3 SG-003: 경로 결정 — 자율주행 판단

| Safety Goal | FSR | TSR | SW Req | SW Design | Source File | Test Case |
|---|---|---|---|---|---|---|
| SG-003 | FSR-004 경로 추종 | TSR-DEC-001 경로 데이터 검증 | SWR-DEC-001 경로 데이터 유효성 | SW-DEC-ARCH-001 §4 PathManager | decision-module/source_kcity/PathManager.cpp | TC-DEC-UT-001 |
| SG-003 | FSR-004 | TSR-DEC-002 보행자 감지 | SWR-DEC-002 보행자 감지 판단 | SW-DEC-ARCH-001 §4 DecisionEngine | decision-module/source_kcity/MainModule.cpp | TC-DEC-UT-010 |
| SG-003 | FSR-004 | TSR-DEC-003 GPS 손실 | SWR-DEC-003 GPS 손실 시 안전 상태 | SW-DEC-ARCH-001 §4 Communicator | decision-module/source_kcity/Communicator.cpp | TC-DEC-FI-001 |
| SG-003 | FSR-005 원격 제어 | TSR-RMT-001 통신 손실 | SW-RMT-SR-020 통신 손실 감지 | SW-RMT-ARCH-001 §4 | remote-control/src/remoteS32G.cpp | TC-RMT-UT-001 |

### 3.4 공통 모듈 (ASIL D 상속)

| Safety Goal | FSR | TSR | SW Req | SW Design | Source File | Test Case |
|---|---|---|---|---|---|---|
| (상속) | - | - | SW-CMN-REQ-CRC-001 CRC 연산 | SW-CMN-ARCH-001 CrcProvider | common/src/CrcProviderPublic.cpp | UT-CRC-001~010 |
| (상속) | - | - | SW-CMN-REQ-CRC-002 NULL 처리 | SW-CMN-ARCH-001 CrcProvider | common/src/CrcProviderPublic.cpp | UT-CRC-005~006 |
| (상속) | - | - | SW-CMN-REQ-SIG-001~005 Signal Codec | SW-CMN-ARCH-001 SignalCodec | common/include/SignalCodec.h | UT-SIG-001~010 |
| (상속) | - | - | SW-CMN-REQ-CFG-001~004 Config Loading | SW-CMN-ARCH-001 CanSpecProvider | common/src/CanSpecConfigLoader.cpp | UT-CFG-001~013 |
| (상속) | - | - | SW-CMN-REQ-GEN-001 빌드 호환 | SW-CMN-ARCH-001 | common/src/*.cpp | IT-INT-001~004 |

## 4. Backward Traceability (Bottom-Up)

| Source File / Module | SW Req | TSR | FSR | Safety Goal |
|---|---|---|---|---|
| aeb-control/src/AEBControl_S32G_v2.cpp | SWREQ-AEB-001, 003, 005 | TSR-AEB-001, 003, 005 | FSR-001 | SG-001 |
| aeb-control/src/Communicator.cpp | SWREQ-AEB-002, 004 | TSR-AEB-002, 004 | FSR-001 | SG-001 |
| control-module/src/Controller.cpp | SW-CTL-SR-001, 002, 010~014 | TSR-CTL-001, 002, 010 | FSR-002, 003 | SG-002 |
| control-module/src/Communicator.cpp | SW-CTL-SR-030~032 | TSR-CTL-030 | FSR-002 | SG-002 |
| control-module/src/ControlModule.cpp | SW-CTL-SR-020~024 | TSR-CTL-020 | FSR-003 | SG-002 |
| control-module/src/PathManager.cpp | SW-CTL-SR-023 | TSR-CTL-020 | FSR-003 | SG-002 |
| decision-module/source_kcity/*.cpp | SWR-DEC-001~004, 006 | TSR-DEC-001~003 | FSR-004 | SG-003 |
| decision-module/source_gateway/*.cpp | SWR-DEC-005, 006 | TSR-DEC-004 | FSR-004 | SG-003 |
| remote-control/src/remoteS32G.cpp | SW-RMT-SR-002~004, 010~012, 020~023 | TSR-RMT-001 | FSR-005 | SG-003 |
| remote-control/src/remotePC.cpp | SW-RMT-SR-001 | TSR-RMT-001 | FSR-005 | SG-003 |
| common/src/CrcProviderPublic.cpp | SW-CMN-REQ-CRC-001~004 | (상속) | (상속) | SG-001, SG-002 |
| common/src/CanSpecConfigLoader.cpp | SW-CMN-REQ-CFG-001~004 | (상속) | (상속) | SG-001, SG-002 |
| common/src/ConfigParser.cpp | SW-CMN-REQ-CFG-001~002 | (상속) | (상속) | SG-001, SG-002, SG-003 |
| common/include/SignalCodec.h | SW-CMN-REQ-SIG-001~005 | (상속) | (상속) | SG-001, SG-002 |

## 5. Coverage Analysis

| Metric | Value | Target |
|---|---|---|
| Safety Goals with FSR | 3 / 3 | 100% |
| FSR with TSR | 5 / 5 | 100% |
| TSR with SW Req | 12 / 12 | 100% |
| SW Req with Test | 40+ / 40+ | 100% |
| Orphan Requirements (no parent) | 0 | 0 |
| Untested Requirements | 0 (common module tested; stub modules pending private impl) | 0 |

### 5.1 Test Coverage by Module

| Module | ASIL | Unit Tests | Integration Tests | Fault Injection |
|---|---|---|---|---|
| common | D | UT-CRC (10), UT-SIG (10), UT-PARSE (10), UT-CFG (13) | IT-INT (4), IT-LIB (4) | TC-FI (7) |
| aeb-control | D | TC-UD-001~022 (정의됨, private impl 필요) | TC-IT-001~042 (정의됨) | TC-FI-001~005 |
| control-module | C | TC-CTL-UT (정의됨) | TC-CTL-IT (정의됨) | - |
| decision-module | B~C | TC-DEC-UT (정의됨) | TC-DEC-IT (정의됨) | TC-DEC-FI (정의됨) |
| remote-control | B | TC-RMT-UT (정의됨) | TC-RMT-IT (정의됨) | - |

## 6. References

- ISO 26262:2018, Part 8, Clause 6
- [Safety Goals](../concept/safety-goals.md)
- [HARA](../concept/hara.md)
- [Functional Safety Concept](../concept/functional-safety-concept.md)
- [Technical Safety Concept](../system/technical-safety-concept.md)
