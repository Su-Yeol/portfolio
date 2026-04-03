# Dependent Failure Analysis

| Field | Value |
|---|---|
| **Document ID** | ANA-DFA-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 9, Clause 7 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 공통 원인 고장(Common Cause Failure), 연쇄 고장(Cascading Failure) 등 종속 고장을 분석하여, 안전 메커니즘의 독립성을 검증한다.

## 2. Common Cause Failure Analysis

<!-- TODO: 공통 원인으로 인한 동시 고장 가능성 분석 -->

| Pair | Element A | Element B | Common Cause | Coupling Factor | Countermeasure |
|---|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO: e.g., AEB SW --> | <!-- TODO: e.g., Watchdog --> | <!-- TODO: e.g., 동일 CPU --> | <!-- TODO: HW, SW, Environmental --> | <!-- TODO: e.g., 독립 프로세서 사용 --> |

## 3. Cascading Failure Analysis

<!-- TODO: 하나의 고장이 다른 요소의 고장을 유발하는 경우 분석 -->

| Source Failure | Affected Element | Propagation Path | Countermeasure |
|---|---|---|---|
| <!-- TODO: e.g., 시간 동기화 실패 --> | <!-- TODO: e.g., 센서 데이터 정합성 --> | <!-- TODO --> | <!-- TODO --> |

## 4. Independence Assessment

<!-- TODO: ASIL 분해 적용 시, 분해된 요소 간 독립성 평가 -->

| Element A | Element B | Independence Criteria | Met? | Evidence |
|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO: 물리적/논리적 독립 --> | <!-- TODO --> | <!-- TODO --> |

## 5. References

- ISO 26262:2018, Part 9, Clause 7
- [FMEA](fmea.md)
- [FTA](fta.md)
- [Safety Requirements Allocation](../system/safety-requirements-allocation.md)
