# System Design Specification

| Field | Value |
|---|---|
| **Document ID** | SYS-DES-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 4, Clause 7 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 시스템의 설계 사양을 정의한다. HW/SW 요소 분배, 통신 구조, 시스템 구성을 기술한다.

## 2. System Architecture Overview

<!-- TODO: 전체 시스템 아키텍처 -->

### 2.1 Hardware Architecture

<!-- TODO: 하드웨어 구성 (ECU, 센서, 액추에이터, 네트워크) -->

| HW Element | Description | Interface |
|---|---|---|
| NXP S32G | Main computing platform (aarch64) | CAN, Ethernet, GPIO |
| Axon | Secondary AUTOSAR target | Ethernet |
| <!-- TODO: Sensors --> | <!-- TODO --> | <!-- TODO --> |
| <!-- TODO: Actuators --> | <!-- TODO --> | <!-- TODO --> |

### 2.2 Software Architecture

<!-- TODO: 소프트웨어 구성 -->

| SW Element | Project | Function |
|---|---|---|
| Adaptive AUTOSAR Middleware | adaptive-autosar | 플랫폼 서비스 (UCM, COM, EXEC, PHM) |
| AEB Control | autonomous-driving-stack | 자동 긴급 제동 |
| Vehicle Control | autonomous-driving-stack | 횡/종방향 제어 |
| Decision Module | autonomous-driving-stack | 경로 판단 |
| Time Sync | time-synchronization-daemon | gPTP 시간 동기화 |
| TSN Tools | tsn-communication-stack | PHC 검증 |

### 2.3 Communication Architecture

<!-- TODO: 통신 구조 (CAN, Ethernet/TSN, 내부 IPC) -->

| Channel | Protocol | Participants | Cycle Time |
|---|---|---|---|
| CAN Bus | CAN 2.0B / CAN-FD | Sensor ECU ↔ S32G ↔ Actuator | <!-- TODO --> |
| In-vehicle Ethernet | TSN (IEEE 802.1AS) | S32G ↔ Axon | <!-- TODO --> |
| Internal | <!-- TODO: SOME/IP, DDS --> | SW modules | <!-- TODO --> |

## 3. Safety-Related System Properties

<!-- TODO: 안전 관련 시스템 속성 -->

| Property | Requirement | Verification Method |
|---|---|---|
| Timing | <!-- TODO: e.g., end-to-end latency < 50ms --> | <!-- TODO --> |
| Availability | <!-- TODO --> | <!-- TODO --> |
| Fault Tolerance | <!-- TODO --> | <!-- TODO --> |

## 4. References

- ISO 26262:2018, Part 4, Clause 7
- [Technical Safety Concept](technical-safety-concept.md)
- [Safety Requirements Allocation](safety-requirements-allocation.md)
