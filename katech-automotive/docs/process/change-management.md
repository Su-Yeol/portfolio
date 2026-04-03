# Change Management Procedure

| Field | Value |
|---|---|
| **Document ID** | PRC-CHG-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 8 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 안전 관련 변경 요청의 접수, 분석, 승인, 구현, 검증 절차를 정의한다.

## 2. Change Request Process

```
변경 요청 접수 → 영향 분석 → 승인/반려 → 구현 → 검증 → 종결
```

### 2.1 Change Request Form

| Field | Description |
|---|---|
| CR ID | 고유 변경 요청 번호 |
| Requestor | 요청자 |
| Date | 요청일 |
| Description | 변경 내용 기술 |
| Reason | 변경 사유 |
| Affected Items | 영향받는 형상 항목 |
| Safety Impact | 안전 영향 여부 (Yes/No) |
| ASIL Impact | 영향받는 ASIL 등급 |

## 3. Impact Analysis

<!-- TODO: 변경 영향 분석 절차 -->

| Analysis Area | Description | Responsible |
|---|---|---|
| Safety Impact | 안전 목표/요구사항에 대한 영향 | Safety Engineer |
| Technical Impact | 코드, 아키텍처에 대한 영향 | SW Developer |
| Test Impact | 테스트 재수행 필요 범위 | Verification Engineer |
| Schedule Impact | 일정에 대한 영향 | Project Manager |

## 4. Approval Authority

<!-- TODO: 변경 승인 권한 -->

| Change Type | Safety Impact | Approval Authority |
|---|---|---|
| Minor (cosmetic, docs) | No | Developer + Reviewer |
| Moderate (function change) | Possible | Safety Engineer + Lead |
| Major (architecture change) | Yes | Safety Manager + CCB |
| Safety-critical | Yes | CCB + Independent Assessor |

## 5. Change Log

<!-- TODO: 변경 이력 기록 -->

| CR ID | Date | Description | Safety Impact | Status |
|---|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 6. References

- ISO 26262:2018, Part 8, Clause 8
- [Configuration Management](configuration-management.md)
