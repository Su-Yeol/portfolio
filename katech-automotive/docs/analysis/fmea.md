# Failure Mode and Effects Analysis (FMEA)

| Field | Value |
|---|---|
| **Document ID** | ANA-FMEA-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 9, Clause 6 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 시스템의 고장 모드 및 영향 분석(FMEA)을 수행하여, 잠재적 고장의 영향과 위험도를 평가하고 대응 조치를 정의한다.

## 2. Scope

<!-- TODO: FMEA 분석 범위 (시스템/SW/HW 수준) -->

## 3. FMEA Worksheet

<!-- TODO: 각 컴포넌트/기능별 FMEA 수행 -->

### 3.1 AEB Control Module

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO: e.g., 센서 데이터 처리 지연 --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| <!-- TODO --> | <!-- TODO: e.g., 제동 명령 미생성 --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 3.2 Control Module

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 3.3 Decision Module

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 3.4 Communication (CAN/TSN)

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO: e.g., 메시지 손실 --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| <!-- TODO --> | <!-- TODO: e.g., 시간 동기화 오류 --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 3.5 Adaptive AUTOSAR Platform

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO: e.g., UCM 업데이트 실패 --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 4. Summary of Findings

<!-- TODO: FMEA 결과 요약, 주요 위험 고장 모드 및 대응 조치 -->

## 5. References

- ISO 26262:2018, Part 9, Clause 6
- [FTA](fta.md)
- [Dependent Failure Analysis](dependent-failure-analysis.md)
