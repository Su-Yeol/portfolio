# Requirements Traceability Matrix

| Field | Value |
|---|---|
| **Document ID** | PRC-RTM-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 6 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 안전 목표(Safety Goal)부터 코드 및 테스트까지의 양방향 추적성(Bidirectional Traceability)을 제공한다.

## 2. Traceability Chain

```
Safety Goal → Functional Safety Req → Technical Safety Req → SW Safety Req → SW Design → Code → Test
```

## 3. Forward Traceability (Top-Down)

<!-- TODO: 안전 목표 → 테스트까지 순방향 추적 -->

| Safety Goal | FSR | TSR | SW Req | SW Design | Source File | Test Case |
|---|---|---|---|---|---|---|
| SG-001 | FSR-001 | TSR-001 | <!-- TODO --> | <!-- TODO --> | <!-- TODO: e.g., aeb-control/src/*.cpp --> | <!-- TODO --> |
| SG-002 | FSR-002 | TSR-002 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SG-003 | FSR-003 | TSR-003 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 4. Backward Traceability (Bottom-Up)

<!-- TODO: 코드 → 안전 목표까지 역방향 추적 -->

| Source File / Module | SW Req | TSR | FSR | Safety Goal |
|---|---|---|---|---|
| aeb-control/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| control-module/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| decision-module/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| remote-control/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| common/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| adaptive-autosar/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| time-synchronization-daemon/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| tsn-communication-stack/ | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 5. Coverage Analysis

<!-- TODO: 추적성 커버리지 분석 -->

| Metric | Value | Target |
|---|---|---|
| Safety Goals with FSR | <!-- TODO --> / <!-- TODO --> | 100% |
| FSR with TSR | <!-- TODO --> / <!-- TODO --> | 100% |
| TSR with SW Req | <!-- TODO --> / <!-- TODO --> | 100% |
| SW Req with Test | <!-- TODO --> / <!-- TODO --> | 100% |
| Orphan Requirements (no parent) | <!-- TODO --> | 0 |
| Untested Requirements | <!-- TODO --> | 0 |

## 6. References

- ISO 26262:2018, Part 8, Clause 6
- [Safety Goals](../concept/safety-goals.md)
- [HARA](../concept/hara.md)
- [Functional Safety Concept](../concept/functional-safety-concept.md)
- [Technical Safety Concept](../system/technical-safety-concept.md)
