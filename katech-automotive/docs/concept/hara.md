# Hazard Analysis and Risk Assessment (HARA)

| Field | Value |
|---|---|
| **Document ID** | CON-HARA-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 3, Clause 7 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 아이템에 대한 위험원 분석 및 리스크 평가(HARA)를 수행하여, 각 위험원의 ASIL 등급을 결정하고 안전 목표를 도출한다.

## 2. Methodology

ISO 26262 Part 3, Clause 7에 따라 다음 절차를 수행한다:
1. 아이템의 기능적 오동작(Malfunction) 식별
2. 위험 상황(Hazardous Event) 도출
3. Severity(S), Exposure(E), Controllability(C) 평가
4. ASIL 등급 결정
5. 안전 목표(Safety Goal) 도출

## 3. ASIL Determination Criteria

### Severity (S)
| Level | Description |
|---|---|
| S0 | No injuries |
| S1 | Light and moderate injuries |
| S2 | Severe and life-threatening injuries (survival probable) |
| S3 | Life-threatening injuries (survival uncertain), fatal injuries |

### Exposure (E)
| Level | Description |
|---|---|
| E0 | Incredible |
| E1 | Very low probability |
| E2 | Low probability |
| E3 | Medium probability |
| E4 | High probability |

### Controllability (C)
| Level | Description |
|---|---|
| C0 | Controllable in general |
| C1 | Simply controllable |
| C2 | Normally controllable |
| C3 | Difficult to control or uncontrollable |

## 4. Hazard Identification

<!-- TODO: 각 기능/모듈별 위험원 식별 -->

### 4.1 AEB Control Module

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-AEB-001 | <!-- TODO: e.g., AEB 미작동 (false negative) --> | <!-- TODO --> | <!-- TODO --> |
| H-AEB-002 | <!-- TODO: e.g., AEB 오작동 (false positive) --> | <!-- TODO --> | <!-- TODO --> |

### 4.2 Control Module (Lateral/Longitudinal)

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-CTL-001 | <!-- TODO: e.g., 조향 제어 오류 --> | <!-- TODO --> | <!-- TODO --> |
| H-CTL-002 | <!-- TODO: e.g., 제동/가속 제어 오류 --> | <!-- TODO --> | <!-- TODO --> |

### 4.3 Decision Module

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-DEC-001 | <!-- TODO: e.g., 잘못된 경로 판단 --> | <!-- TODO --> | <!-- TODO --> |
| H-DEC-002 | <!-- TODO: e.g., 장애물 미감지 --> | <!-- TODO --> | <!-- TODO --> |

### 4.4 Remote Control Module

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-RMT-001 | <!-- TODO: e.g., 통신 단절 시 제어 불가 --> | <!-- TODO --> | <!-- TODO --> |

### 4.5 Time Synchronization / TSN

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-TSN-001 | <!-- TODO: e.g., 시간 동기화 실패로 인한 데이터 불일치 --> | <!-- TODO --> | <!-- TODO --> |

### 4.6 Adaptive AUTOSAR Platform

| Hazard ID | Malfunction | Hazardous Event | Operating Scenario |
|---|---|---|---|
| H-ARA-001 | <!-- TODO: e.g., UCM 업데이트 실패로 인한 불일치 상태 --> | <!-- TODO --> | <!-- TODO --> |

## 5. ASIL Classification

<!-- TODO: 각 위험원에 대한 S/E/C 평가 및 ASIL 결정 -->

| Hazard ID | S | E | C | ASIL | Safety Goal ID |
|---|---|---|---|---|---|
| H-AEB-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO: e.g., ASIL D --> | SG-001 |
| H-AEB-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-002 |
| H-CTL-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-003 |
| H-CTL-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-004 |
| H-DEC-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-005 |
| H-RMT-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-006 |
| H-TSN-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-007 |
| H-ARA-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | SG-008 |

## 6. References

- ISO 26262:2018, Part 3, Clause 7
- [Item Definition](item-definition.md)
- [Safety Goals](safety-goals.md)
