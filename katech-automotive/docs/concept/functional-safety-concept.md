# Functional Safety Concept

| Field | Value |
|---|---|
| **Document ID** | CON-FSC-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 3, Clause 8 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 안전 목표(Safety Goal)로부터 기능안전 요구사항(Functional Safety Requirements)을 도출하고, 이를 아이템의 예비 아키텍처 요소에 할당하는 기능안전 컨셉을 정의한다.

## 2. Functional Safety Requirements

<!-- TODO: 각 안전 목표에 대한 기능안전 요구사항 도출 -->

### From SG-001

| FSR ID | Description | ASIL | Allocated To |
|---|---|---|---|
| FSR-001 | <!-- TODO: e.g., AEB 시스템은 전방 장애물 감지 시 100ms 이내에 제동을 개시해야 한다 --> | <!-- TODO --> | <!-- TODO: e.g., AEB Control Module --> |
| FSR-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### From SG-002

| FSR ID | Description | ASIL | Allocated To |
|---|---|---|---|
| FSR-003 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### From SG-003

| FSR ID | Description | ASIL | Allocated To |
|---|---|---|---|
| FSR-004 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 3. Safety Mechanisms

<!-- TODO: 안전 메커니즘 정의 (오류 감지, 오류 처리, 안전 상태 전이) -->

| Mechanism ID | Type | Description | Related FSR |
|---|---|---|---|
| SM-001 | Detection | <!-- TODO: e.g., E2E protection for CAN messages --> | <!-- TODO --> |
| SM-002 | Handling | <!-- TODO: e.g., 안전 상태 전이 (degraded mode) --> | <!-- TODO --> |
| SM-003 | Notification | <!-- TODO: e.g., 운전자 경고 (visual + audible) --> | <!-- TODO --> |

## 4. Preliminary Architecture Allocation

<!-- TODO: 기능안전 요구사항의 예비 아키텍처 요소 할당 -->

| FSR ID | SW Component | HW Component | Communication |
|---|---|---|---|
| FSR-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| FSR-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 5. Operating Modes

<!-- TODO: 시스템 운용 모드 정의 -->

| Mode | Description | Active Safety Mechanisms |
|---|---|---|
| Normal | 정상 운행 모드 | <!-- TODO --> |
| Degraded | 일부 기능 제한 모드 | <!-- TODO --> |
| Emergency | 비상 정지 모드 | <!-- TODO --> |
| Safe State | 안전 상태 | <!-- TODO --> |

## 6. References

- ISO 26262:2018, Part 3, Clause 8
- [Safety Goals](safety-goals.md)
- [Technical Safety Concept](../system/technical-safety-concept.md)
