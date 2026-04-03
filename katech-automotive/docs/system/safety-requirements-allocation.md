# Safety Requirements Allocation

| Field | Value |
|---|---|
| **Document ID** | SYS-ALLOC-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 4, Clause 6 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 기술안전 요구사항(TSR)을 하드웨어 및 소프트웨어 요소에 할당하는 매트릭스를 정의한다.

## 2. Allocation Matrix

<!-- TODO: 각 TSR의 HW/SW 할당 -->

| TSR ID | Description | ASIL | SW Component | HW Component | ASIL Decomposition |
|---|---|---|---|---|---|
| TSR-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO: N/A or ASIL B(D) + ASIL B(D) --> |
| TSR-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 3. ASIL Decomposition

<!-- TODO: ASIL 분해가 적용된 경우, 그 근거와 독립성 요구사항 기술 -->

| TSR ID | Original ASIL | Decomposed ASIL (SW) | Decomposed ASIL (HW) | Independence Requirement |
|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 4. Freedom from Interference

<!-- TODO: SW 요소 간 비간섭성(Freedom from Interference) 분석 -->

| SW Element | ASIL | Coexisting Element | Measure |
|---|---|---|---|
| <!-- TODO: e.g., AEB Control --> | <!-- TODO: ASIL D --> | <!-- TODO: e.g., Remote Control (QM) --> | <!-- TODO: e.g., Memory protection, timing monitoring --> |

## 5. References

- ISO 26262:2018, Part 4, Clause 6
- [Technical Safety Concept](technical-safety-concept.md)
- [System Design](system-design.md)
