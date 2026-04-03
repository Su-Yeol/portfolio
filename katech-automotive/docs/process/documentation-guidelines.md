# Documentation Guidelines

| Field | Value |
|---|---|
| **Document ID** | PRC-DOC-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 11 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 ISO 26262 산출물 작성을 위한 문서화 지침을 정의한다.

## 2. Document Structure

모든 ISO 26262 산출물은 다음 메타데이터를 포함해야 한다:

| Field | Required | Description |
|---|---|---|
| Document ID | Yes | 고유 문서 식별자 (카테고리-타입-번호) |
| Version | Yes | 문서 버전 (Major.Minor) |
| ISO 26262 Reference | Yes | 해당 Part, Clause 번호 |
| ASIL Scope | Conditional | 안전 관련 문서일 경우 ASIL 범위 |
| Author | Yes | 작성자 |
| Reviewer | Yes | 검토자 |
| Approval Date | Yes | 승인일 |

## 3. Document ID Convention

```
<Category>-<Type>-<Number>
```

| Category | Description |
|---|---|
| SAF | Safety Management (Part 2) |
| CON | Concept Phase (Part 3) |
| SYS | System Level (Part 4) |
| SW | Software Level (Part 6) |
| PRC | Supporting Processes (Part 8) |
| ANA | Safety Analysis (Part 9) |

## 4. Review and Approval Process

| Step | Action | Responsible |
|---|---|---|
| 1 | Draft 작성 | Author |
| 2 | 기술 검토 | Reviewer |
| 3 | 안전 검토 (해당 시) | Safety Engineer |
| 4 | 승인 | Approver |
| 5 | 베이스라인 등록 | Configuration Manager |

## 5. Version Control

- 문서는 Git으로 버전 관리
- 변경 시 Document ID의 Version 필드 업데이트
- 주요 변경은 변경 이력(Revision History) 섹션에 기록

## 6. Language and Format

- 작성 언어: 한국어 (기술 용어는 영문 병기)
- 파일 형식: Markdown (.md)
- 다이어그램: Mermaid, PlantUML, 또는 이미지 파일 참조

## 7. References

- ISO 26262:2018, Part 8, Clause 11
- [Configuration Management](configuration-management.md)
