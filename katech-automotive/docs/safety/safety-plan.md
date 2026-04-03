# Safety Plan

| Field | Value |
|---|---|
| **Document ID** | SAF-PLAN-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 2, Clause 6 |
| **ASIL Scope** | All (A–D) |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

<!-- TODO: 본 안전 계획서의 목적 및 적용 범위를 기술 -->

본 문서는 KATECH Automotive 프로젝트의 기능안전 활동을 계획하고, ISO 26262에 따른 안전 수명주기 전반의 활동, 역할, 일정을 정의한다.

## 2. Scope

<!-- TODO: 대상 프로젝트/시스템 범위 명시 -->

| Project | Description | Target HW |
|---|---|---|
| Adaptive AUTOSAR | Platform bring-up, Yocto integration, UCM | NXP S32G, Axon |
| Autonomous Driving Stack | AEB, Control, Decision, Remote-Control | NXP S32G |
| Time Synchronization Daemon | linuxptp, gPTP, Open-AVB | NXP S32G |
| TSN Communication Stack | PHC clock-sync verification | NXP S32G |

## 3. Safety Lifecycle Tailoring

<!-- TODO: ISO 26262 안전 수명주기 중 본 프로젝트에 적용/비적용되는 단계 명시 -->

| Phase | ISO 26262 Part | Applicable | Justification |
|---|---|---|---|
| Concept | Part 3 | <!-- TODO --> | |
| System Design | Part 4 | <!-- TODO --> | |
| HW Development | Part 5 | <!-- TODO --> | |
| SW Development | Part 6 | <!-- TODO --> | |
| Production | Part 7 | <!-- TODO --> | |
| Supporting Processes | Part 8 | <!-- TODO --> | |
| Safety Analysis | Part 9 | <!-- TODO --> | |

## 4. Roles and Responsibilities

<!-- TODO: 안전 관련 역할 및 책임 정의 -->

| Role | Responsibility | Assigned To |
|---|---|---|
| Safety Manager | 안전 활동 총괄, 안전 계획 유지 | <!-- TODO --> |
| Safety Engineer | HARA, 안전 분석, 안전 요구사항 도출 | <!-- TODO --> |
| SW Developer | 안전 요구사항 구현, 단위 테스트 | <!-- TODO --> |
| Verification Engineer | 검증 활동, 테스트 수행 | <!-- TODO --> |
| Independent Assessor | 기능안전 평가 (독립성 확보) | <!-- TODO --> |

## 5. Safety Activities Schedule

<!-- TODO: 각 안전 활동의 일정 및 마일스톤 -->

| Activity | Planned Start | Planned End | Deliverable |
|---|---|---|---|
| HARA | <!-- TODO --> | <!-- TODO --> | hara.md |
| Safety Goals Definition | <!-- TODO --> | <!-- TODO --> | safety-goals.md |
| Functional Safety Concept | <!-- TODO --> | <!-- TODO --> | functional-safety-concept.md |
| Technical Safety Concept | <!-- TODO --> | <!-- TODO --> | technical-safety-concept.md |
| SW Safety Requirements | <!-- TODO --> | <!-- TODO --> | sw-safety-requirements.md |
| SW Architecture Design | <!-- TODO --> | <!-- TODO --> | sw-architecture.md |
| SW Unit/Integration Test | <!-- TODO --> | <!-- TODO --> | sw-unit-test.md, sw-integration-test.md |
| Safety Validation | <!-- TODO --> | <!-- TODO --> | sw-verification-report.md |

## 6. Methods and Tools

<!-- TODO: 안전 활동에 사용되는 방법론 및 도구 -->

| Activity | Method | Tool |
|---|---|---|
| Hazard Analysis | HARA (ISO 26262 Part 3) | <!-- TODO --> |
| Safety Analysis | FMEA, FTA | <!-- TODO --> |
| Static Analysis | <!-- TODO --> | <!-- TODO: e.g., Polyspace, LDRA --> |
| Dynamic Analysis | <!-- TODO --> | <!-- TODO: e.g., VectorCAST --> |
| Requirements Management | <!-- TODO --> | <!-- TODO: e.g., DOORS, Polarion --> |
| Configuration Management | Git | Git + <!-- TODO: branching strategy --> |

## 7. Verification and Confirmation Measures

<!-- TODO: 검증 및 확인 조치 계획 (리뷰, 감사, 평가 등) -->

| Measure | Frequency | Responsible |
|---|---|---|
| Safety Plan Review | <!-- TODO --> | Safety Manager |
| Work Product Review | <!-- TODO --> | Reviewer |
| Safety Audit | <!-- TODO --> | Independent Assessor |
| Safety Assessment | <!-- TODO --> | Independent Assessor |

## 8. References

- ISO 26262:2018, Part 2 — Management of functional safety
- [Safety Case](safety-case.md)
- [Confirmation Review](confirmation-review.md)
- [HARA](../concept/hara.md)
