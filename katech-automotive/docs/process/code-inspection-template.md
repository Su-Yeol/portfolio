# Code Inspection Record

| Field | Value |
|---|---|
| **Document ID** | INS-[MODULE]-[SEQ] |
| **Date** | YYYY-MM-DD |
| **Module** | [Module Name] |
| **ASIL Level** | [A / B / C / D] |
| **Files Reviewed** | [File paths] |
| **Checklist Version** | PRC-CR-001 v1.0 |
| **ISO 26262 Reference** | Part 6, Clause 8 |

---

## 1. Participants

| Role | Name | Organization |
|---|---|---|
| Author | | |
| Moderator | | |
| Inspector 1 | | |
| Inspector 2 | | |
| Recorder | | |

## 2. Scope

| Item | Description |
|---|---|
| Review type | Walk-through / Inspection |
| Source version | git commit hash |
| Build configuration | USE_PRIVATE_IMPL=ON/OFF |
| Related SWREQ | [SWREQ IDs] |

## 3. Findings

| ID | File:Line | Severity | Category | Description | Resolution | Status |
|---|---|---|---|---|---|---|
| F-001 | | Critical / Major / Minor | Table 3 item # | | | Open / Resolved / Accepted |
| F-002 | | | | | | |
| F-003 | | | | | | |

### Severity Definitions

| Severity | Definition | ASIL D Action |
|---|---|---|
| Critical | 안전 기능에 직접 영향, 데이터 손상 또는 시스템 실패 가능 | 출시 전 반드시 수정 |
| Major | 기능 동작에 영향, 성능 저하 또는 유지보수성 감소 | 출시 전 수정 권장 |
| Minor | 코딩 스타일, 가독성, 문서화 관련 | 차기 릴리스에서 수정 |

## 4. Metrics

| Metric | Value |
|---|---|
| Total LOC reviewed | |
| Review duration (hours) | |
| Findings count (Critical) | |
| Findings count (Major) | |
| Findings count (Minor) | |
| Defect density (per KLOC) | |

## 5. Checklist Compliance Summary

| Check Item | Pass | Fail | N/A | Notes |
|---|---|---|---|---|
| 1. 매개변수 평가 순서 | | | | |
| 2. 암묵적 형 변환 | | | | |
| 3. 숨겨진 데이터/제어 흐름 | | | | |
| 4. 무조건 점프 | | | | |
| 5. 재귀 호출 | | | | |
| 6. 런타임 동적 객체 | | | | |
| 7. 포인터 사용 | | | | |
| 8. 변수 초기화 | | | | |
| 9. 단일 진입/종료점 | | | | |
| 10. 방어적 프로그래밍 | | | | |

## 6. Verdict

| Verdict | Description |
|---|---|
| [ ] Approved | 모든 Critical/Major 발견사항 해결됨 |
| [ ] Conditionally Approved | Minor 발견사항만 잔존, 안전 영향 없음 확인 |
| [ ] Rejected | Critical/Major 발견사항 미해결, 재검토 필요 |

## 7. Sign-off

| Role | Name | Signature | Date |
|---|---|---|---|
| Moderator | | | |
| Author | | | |
| Safety Manager | | | |
