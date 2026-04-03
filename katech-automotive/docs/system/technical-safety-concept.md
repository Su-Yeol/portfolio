# Technical Safety Concept

| Field | Value |
|---|---|
| **Document ID** | SYS-TSC-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 4, Clause 6 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 기능안전 컨셉의 기능안전 요구사항을 기술안전 요구사항(Technical Safety Requirements)으로 구체화하고, 시스템 아키텍처의 HW/SW 요소에 할당한다.

## 2. Technical Safety Requirements

<!-- TODO: 기능안전 요구사항 → 기술안전 요구사항 도출 -->

| TSR ID | FSR ID | Description | ASIL | Allocated To |
|---|---|---|---|---|
| TSR-001 | FSR-001 | <!-- TODO: e.g., AEB SW는 CAN 메시지 수신 후 10ms 이내에 제동 명령을 생성해야 한다 --> | <!-- TODO --> | <!-- TODO: SW/HW component --> |
| TSR-002 | FSR-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 3. System Architecture

<!-- TODO: 시스템 아키텍처 블록 다이어그램 (텍스트 또는 이미지 참조) -->

```
<!-- TODO: 시스템 블록 다이어그램 -->
┌─────────────┐     CAN/Ethernet     ┌──────────────┐
│   Sensors   │ ──────────────────── │  NXP S32G    │
│ (LiDAR,     │                      │  ┌─────────┐ │     CAN      ┌───────────┐
│  Radar,     │                      │  │Decision │ │ ──────────── │ Actuators │
│  Camera,    │                      │  │Control  │ │              │ (Brake,   │
│  GPS)       │                      │  │AEB      │ │              │  Steer,   │
└─────────────┘                      │  └─────────┘ │              │  Throttle)│
                                     └──────────────┘              └───────────┘
```

## 4. HW/SW Interface Specification

<!-- TODO: 하드웨어-소프트웨어 인터페이스 사양 -->

| Interface | HW Element | SW Element | Signal/Data | Timing |
|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 5. Safety Mechanisms (System Level)

<!-- TODO: 시스템 수준 안전 메커니즘 -->

| Mechanism | Type | Description | Diagnostic Coverage | Related TSR |
|---|---|---|---|---|
| E2E Protection | Error Detection | <!-- TODO: CAN/Ethernet 메시지 E2E 보호 --> | <!-- TODO --> | <!-- TODO --> |
| Watchdog | Monitoring | <!-- TODO: SW 실행 모니터링 --> | <!-- TODO --> | <!-- TODO --> |
| Redundancy | Fault Tolerance | <!-- TODO: 센서 이중화 등 --> | <!-- TODO --> | <!-- TODO --> |

## 6. References

- ISO 26262:2018, Part 4, Clause 6
- [Functional Safety Concept](../concept/functional-safety-concept.md)
- [System Design](system-design.md)
- [Safety Requirements Allocation](safety-requirements-allocation.md)
