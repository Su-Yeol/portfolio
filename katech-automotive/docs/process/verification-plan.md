# Verification Plan

| Field | Value |
|---|---|
| **Document ID** | PRC-VER-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 9 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 프로젝트의 검증 활동 계획을 정의한다. 정적 분석, 동적 분석, 코드 리뷰, 테스트 절차를 포함한다.

## 2. Verification Methods by ASIL

| Method | ASIL A | ASIL B | ASIL C | ASIL D |
|---|---|---|---|---|
| Requirements Review | ++ | ++ | ++ | ++ |
| Design Review | + | ++ | ++ | ++ |
| Code Review (Walk-through) | + | ++ | ++ | ++ |
| Code Review (Inspection) | o | + | ++ | ++ |
| Static Analysis | + | ++ | ++ | ++ |
| Unit Testing | ++ | ++ | ++ | ++ |
| Integration Testing | + | ++ | ++ | ++ |
| Back-to-back Testing | o | + | + | ++ |

Legend: ++ highly recommended, + recommended, o optional

## 3. Static Analysis

<!-- TODO: 정적 분석 도구 및 기준 -->

| Tool | Purpose | Coding Standard | Target |
|---|---|---|---|
| <!-- TODO: e.g., Polyspace --> | Bug detection, runtime error proof | <!-- TODO: MISRA C++:2008 --> | All safety-related C++ code |
| <!-- TODO: e.g., Cppcheck --> | Static code analysis | <!-- TODO --> | All C/C++ sources |

### 3.1 MISRA Compliance

<!-- TODO: MISRA 준수 정책 -->

| Standard | Scope | Deviation Procedure |
|---|---|---|
| <!-- TODO: MISRA C:2012 / MISRA C++:2008 --> | <!-- TODO --> | <!-- TODO: 편차 허용 절차 --> |

## 4. Dynamic Analysis

<!-- TODO: 동적 분석/테스트 도구 및 기준 -->

| Tool | Purpose | Coverage Criteria |
|---|---|---|
| <!-- TODO: e.g., VectorCAST, GoogleTest --> | Unit/Integration testing | <!-- TODO: Statement, Branch, MC/DC --> |

### 4.1 Coverage Criteria by ASIL

| ASIL | Statement | Branch | MC/DC |
|---|---|---|---|
| A | Required | Recommended | — |
| B | Required | Required | Recommended |
| C | Required | Required | Required |
| D | Required | Required | Required |

## 5. Code Review Process

<!-- TODO: 코드 리뷰 절차 -->

| Step | Description | Responsible |
|---|---|---|
| 1 | PR/MR 생성 | Developer |
| 2 | 자동 정적 분석 수행 | CI Pipeline |
| 3 | 동료 코드 리뷰 | Reviewer |
| 4 | 안전 관련 코드 추가 검토 | Safety Engineer |
| 5 | 리뷰 결과 기록 | Reviewer |

## 6. References

- ISO 26262:2018, Part 8, Clause 9
- [Tool Qualification](tool-qualification.md)
- [Documentation Guidelines](documentation-guidelines.md)
