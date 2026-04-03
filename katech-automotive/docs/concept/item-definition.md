# Item Definition

| Field | Value |
|---|---|
| **Document ID** | CON-ITEM-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 3, Clause 5 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 프로젝트에서 개발하는 아이템(Item)의 기능, 인터페이스, 환경 조건, 경계를 정의한다.

## 2. Item Overview

<!-- TODO: 아이템의 전체적인 기능 설명 -->

### 2.1 Functional Description

<!-- TODO: 아이템이 차량 내에서 수행하는 기능 기술 -->

### 2.2 Item Boundary

<!-- TODO: 아이템의 물리적/논리적 경계 정의 (무엇이 포함되고, 무엇이 포함되지 않는지) -->

## 3. Item Components

| Component | Project | Description | Target HW |
|---|---|---|---|
| Adaptive AUTOSAR Platform | adaptive-autosar | AUTOSAR Adaptive 미들웨어 플랫폼 | NXP S32G, Axon |
| AEB Control | autonomous-driving-stack | 자동 긴급 제동 제어 모듈 | NXP S32G |
| Vehicle Control | autonomous-driving-stack | 횡/종방향 차량 제어 모듈 | NXP S32G |
| Decision Module | autonomous-driving-stack | 주행 판단 모듈 (K-City, Gateway) | NXP S32G |
| Remote Control | autonomous-driving-stack | 원격 UDP/CAN 통신 모듈 | NXP S32G |
| Time Sync Daemon | time-synchronization-daemon | gPTP/TSN 시간 동기화 | NXP S32G |
| TSN Stack | tsn-communication-stack | PHC clock 검증 도구 | NXP S32G |

## 4. Interfaces

<!-- TODO: 아이템의 외부 인터페이스 정의 -->

### 4.1 External Interfaces

| Interface | Type | Connected To | Protocol |
|---|---|---|---|
| CAN Bus | Communication | Vehicle CAN network | CAN 2.0B / CAN-FD |
| Ethernet (TSN) | Communication | In-vehicle Ethernet | IEEE 802.1AS (gPTP) |
| Sensor Input | Data | LiDAR, Radar, Camera, GPS | <!-- TODO --> |
| Actuator Output | Control | Steering, Brake, Throttle | <!-- TODO: CAN signal --> |

### 4.2 Internal Interfaces

<!-- TODO: 컴포넌트 간 내부 인터페이스 정의 -->

## 5. Operating Conditions

<!-- TODO: 아이템의 운용 환경 조건 -->

| Parameter | Condition |
|---|---|
| Operating Temperature | <!-- TODO: e.g., -40°C ~ +85°C --> |
| Supply Voltage | <!-- TODO --> |
| Operating Mode | <!-- TODO: Normal, Degraded, Emergency --> |

## 6. Known Limitations and Assumptions

<!-- TODO: 알려진 제약사항 및 가정 -->

| ID | Description | Type |
|---|---|---|
| ASM-001 | <!-- TODO --> | Assumption |
| LIM-001 | <!-- TODO --> | Limitation |

## 7. Regulatory Requirements

<!-- TODO: 관련 법규 및 규격 요구사항 -->

- ISO 26262:2018 — Functional Safety
- <!-- TODO: 기타 관련 규격 (e.g., UN-R157 for ALKS, SOTIF ISO 21448) -->

## 8. References

- ISO 26262:2018, Part 3, Clause 5
- [HARA](hara.md)
- [Safety Goals](safety-goals.md)
