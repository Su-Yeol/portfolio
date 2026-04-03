# Fault Tree Analysis (FTA)

| Field | Value |
|---|---|
| **Document ID** | ANA-FTA-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 9, Clause 7 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 안전 목표 위반을 최상위 사건(Top Event)으로 하여, 그 원인을 논리적으로 분석하는 결함 트리 분석(FTA)을 수행한다.

## 2. Scope

<!-- TODO: FTA 분석 범위 및 대상 Top Event 정의 -->

## 3. Fault Trees

<!-- TODO: 각 안전 목표 위반에 대한 결함 트리 작성 -->

### 3.1 Top Event: <!-- TODO: e.g., SG-001 위반 — AEB 미작동 -->

```
<!-- TODO: 결함 트리 다이어그램 (텍스트 표현) -->
                    [Top Event]
                   AEB 미작동
                       │
                    ┌──OR──┐
                    │       │
              [Gate 1]   [Gate 2]
            센서 고장    SW 고장
               │           │
           ┌──OR──┐    ┌──OR──┐
           │       │    │       │
        [BE1]   [BE2] [BE3]  [BE4]
        LiDAR  Radar  AEB    Decision
        고장    고장   SW오류  SW오류
```

### 3.2 Top Event: <!-- TODO: e.g., SG-003 위반 — 조향 제어 오류 -->

```
<!-- TODO: 결함 트리 다이어그램 -->
```

## 4. Minimal Cut Sets

<!-- TODO: 최소 절단 집합(Minimal Cut Set) 분석 -->

| Top Event | Cut Set | Basic Events | Order |
|---|---|---|---|
| <!-- TODO --> | CS-1 | <!-- TODO: e.g., {BE1} --> | <!-- TODO: 1 (single point) --> |
| <!-- TODO --> | CS-2 | <!-- TODO: e.g., {BE3, BE4} --> | <!-- TODO: 2 --> |

## 5. Quantitative Analysis

<!-- TODO: (선택사항) 정량적 분석 — 기본 사건 확률 및 최상위 사건 발생 확률 -->

| Basic Event | Failure Rate | Source |
|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 6. Findings and Recommendations

<!-- TODO: FTA 결과 요약 및 권고사항 -->

## 7. References

- ISO 26262:2018, Part 9, Clause 7
- [FMEA](fmea.md)
- [Dependent Failure Analysis](dependent-failure-analysis.md)
