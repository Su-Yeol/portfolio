# AEB Control — 소프트웨어 검증 보고서 (Software Verification Report)

> ISO 26262-6:2018, Clause 11 — Verification of software safety requirements

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-VR-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 11 |
| **ASIL 등급** | ASIL D |
| **대상 HW** | NXP S32G (aarch64) |
| **작성일** | TODO: YYYY-MM-DD |
| **작성자** | TODO: 작성자 |
| **검토자** | TODO: 검토자 |
| **승인자** | TODO: 승인자 |
| **버전** | 0.1 (초안) |
| **상태** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 AEB Control 모듈의 소프트웨어 검증 결과를 종합적으로 보고한다.
ISO 26262-6 Clause 11에 따라, 소프트웨어 안전 요구사항(SW-AEB-REQ-001)이
설계, 구현, 테스트를 통해 충족되었는지 확인하고, 잔존 이슈 및 조건을 기록한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-REQ-001 | AEB SW Safety Requirements | Part 6, Clause 6 |
| SW-AEB-ARCH-001 | AEB SW Architecture | Part 6, Clause 7 |
| SW-AEB-UD-001 | AEB SW Unit Design | Part 6, Clause 8 |
| SW-AEB-UT-001 | AEB SW Unit Test | Part 6, Clause 9 |
| SW-AEB-IT-001 | AEB SW Integration Test | Part 6, Clause 10 |

---

## 3. 검증 범위 (Verification Scope)

### 3.1 대상 소프트웨어

| 항목 | 내용 |
|------|------|
| 모듈 | AEB Control |
| 소스 파일 | `src/AEBControl_S32G_v2.cpp`, `src/Communicator.cpp` |
| 공통 라이브러리 | CanSpecProvider, CrcProvider, SignalCodec |
| 빌드 출력 | `build/run_aeb` (via `./build.sh`) |
| 소프트웨어 버전 | TODO: v.X.Y.Z |
| 빌드 환경 | TODO: 컴파일러, 버전, 옵션 |

### 3.2 검증 활동 범위

| 검증 활동 | ISO 26262 참조 | 수행 여부 | 비고 |
|-----------|---------------|-----------|------|
| 안전 요구사항 검토 | Clause 6 | TODO | 요구사항 리뷰 |
| 아키텍처 설계 검토 | Clause 7 | TODO | 아키텍처 리뷰 |
| 단위 설계 검토 | Clause 8 | TODO | 코드 인스펙션, 정적 분석 |
| 단위 테스트 | Clause 9 | TODO | SW-AEB-UT-001 |
| 통합 테스트 | Clause 10 | TODO | SW-AEB-IT-001 |
| 안전 요구사항 검증 | Clause 11 | 본 문서 | 종합 검증 |

---

## 4. 소프트웨어 안전 요구사항 검증 결과 (SWREQ Verification Results)

### 4.1 요구사항별 검증 추적 매트릭스

| SWREQ ID | 요구사항 요약 | ASIL | 검증 방법 | 검증 근거 | 상태 |
|----------|-------------|------|-----------|-----------|------|
| SWREQ-AEB-001 | 긴급 제동 판단 (TTC 기반) | D | UT, IT, HIL | TC-UD-001~005, TC-IT-010, TC-IT-040 | TODO |
| SWREQ-AEB-002 | 제동 명령 전달 지연 | D | IT, Timing | TC-IT-002, TC-IT-042 | TODO |
| SWREQ-AEB-003 | 센서 데이터 이상 처리 | D | UT, FI | TC-UD-004, TC-UD-012, TC-IT-011 | TODO |
| SWREQ-AEB-004 | CAN 통신 오류 처리 | D | UT, FI, IT | TC-UD-011, TC-UD-013, TC-FI-IT-001 | TODO |
| SWREQ-AEB-005 | 설정 파일 무결성 | D | UT | TC-UD-020~022, TC-IT-020 | TODO |
| SWREQ-AEB-NF-001 | 실행 주기 | D | IT, Timing | TC-IT-042 | TODO |
| SWREQ-AEB-NF-002 | 동적 메모리 금지 | D | Static Analysis | TODO: 정적 분석 결과 | TODO |
| SWREQ-AEB-NF-003 | WCET | D | Timing Analysis | TODO: WCET 분석 결과 | TODO |
| SWREQ-AEB-NF-004 | 코드 커버리지 | D | UT Coverage | TODO: 커버리지 리포트 | TODO |
| SWREQ-AEB-NF-005 | 코딩 표준 준수 | D | Static Analysis | TODO: MISRA/AUTOSAR 검사 결과 | TODO |

---

## 5. 단위 테스트 결과 요약 (Unit Test Results Summary)

### 5.1 테스트 실행 결과

| 단위 (SWU) | TC 수 | Pass | Fail | Skip | Stmt Cov. | Branch Cov. | MC/DC |
|------------|-------|------|------|------|-----------|-------------|-------|
| SWU-AEB-001 (AEBControlMain) | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| SWU-AEB-002 (Communicator) | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| SWU-AEB-003 (DecisionEngine) | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| SWU-AEB-004 (ConfigLoader) | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| SWU-AEB-005 (SafetyMonitor) | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| **합계** | **TODO** | **TODO** | **TODO** | **TODO** | **TODO** | **TODO** | **TODO** |

### 5.2 ASIL D 커버리지 달성 현황

| 커버리지 유형 | 목표 | 달성 | 판정 | 비고 |
|--------------|------|------|------|------|
| 구문 (Statement) | 100% | TODO | TODO | |
| 분기 (Branch) | 100% | TODO | TODO | |
| MC/DC | TODO% | TODO | TODO | ASIL D 핵심 |

---

## 6. 통합 테스트 결과 요약 (Integration Test Results Summary)

| 통합 단계 | TC 수 | Pass | Fail | Blocked | 비고 |
|-----------|-------|------|------|---------|------|
| Step 1: CAN 통신 스택 | TODO | TODO | TODO | TODO | |
| Step 2: 센서→판단 | TODO | TODO | TODO | TODO | |
| Step 3: 설정 통합 | TODO | TODO | TODO | TODO | |
| Step 4: 안전 메커니즘 | TODO | TODO | TODO | TODO | |
| Step 5: 전체 E2E | TODO | TODO | TODO | TODO | |
| 결함 주입 (FI) | TODO | TODO | TODO | TODO | |
| **합계** | **TODO** | **TODO** | **TODO** | **TODO** | |

---

## 7. 타이밍 검증 결과 (Timing Verification Results)

| 항목 | 요구사항 | 측정값 | 판정 | 측정 환경 | 비고 |
|------|---------|--------|------|-----------|------|
| 메인 루프 주기 | ≤ TODO ms | TODO ms | TODO | TODO | SWREQ-AEB-NF-001 |
| 센서→판단 지연 | ≤ TODO ms | TODO ms | TODO | TODO | SWREQ-AEB-001 |
| E2E 지연 | ≤ TODO ms | TODO ms | TODO | HIL | SWREQ-AEB-002 |
| WCET | ≤ TODO us | TODO us | TODO | TODO | SWREQ-AEB-NF-003 |

---

## 8. 정적 분석 결과 (Static Analysis Results)

### 8.1 코딩 표준 검사

| 도구 | 적용 표준 | 총 위반 수 | Critical | Major | Minor | 비고 |
|------|----------|-----------|----------|-------|-------|------|
| TODO | MISRA C++:2008 | TODO | TODO | TODO | TODO | |
| TODO | AUTOSAR C++14 | TODO | TODO | TODO | TODO | 해당 시 |

### 8.2 정적 분석 (런타임 오류 검출)

| 도구 | 검출 항목 | 결과 | 비고 |
|------|-----------|------|------|
| TODO: (예: Polyspace) | 잠재적 런타임 오류 | TODO | |
| TODO | 데드 코드 | TODO | |
| TODO | 초기화되지 않은 변수 | TODO | |
| TODO | 범위 초과 접근 | TODO | |

---

## 9. 리뷰 결과 (Review Results)

| 리뷰 유형 | 대상 문서 | 일시 | 참석자 | 이슈 수 | 상태 |
|----------|-----------|------|--------|---------|------|
| 요구사항 리뷰 | SW-AEB-REQ-001 | TODO | TODO | TODO | TODO |
| 아키텍처 리뷰 | SW-AEB-ARCH-001 | TODO | TODO | TODO | TODO |
| 코드 인스펙션 | AEBControl_S32G_v2.cpp | TODO | TODO | TODO | TODO |
| 코드 인스펙션 | Communicator.cpp | TODO | TODO | TODO | TODO |
| 테스트 리뷰 | SW-AEB-UT-001 | TODO | TODO | TODO | TODO |
| 테스트 리뷰 | SW-AEB-IT-001 | TODO | TODO | TODO | TODO |

---

## 10. 미해결 이슈 및 잔존 리스크 (Open Issues & Residual Risks)

| Issue ID | 설명 | 심각도 | 영향 SWREQ | 대응 계획 | 상태 |
|----------|------|--------|-----------|-----------|------|
| ISSUE-001 | TODO: SafetyMonitor 미구현 | High | SWREQ-AEB-003, 004 | TODO | Open |
| ISSUE-002 | TODO: WCET 분석 미수행 | High | SWREQ-AEB-NF-003 | TODO | Open |
| ISSUE-003 | TODO: MC/DC 커버리지 목표 미정 | Medium | SWREQ-AEB-NF-004 | TODO | Open |
| ISSUE-004 | TODO: HIL 테스트 환경 미구축 | Medium | SWREQ-AEB-002 | TODO | Open |
| TODO | TODO: 추가 이슈 기록 | TODO | TODO | TODO | TODO |

---

## 11. 검증 판정 (Verification Verdict)

### 11.1 종합 판정

| 항목 | 결과 | 비고 |
|------|------|------|
| 모든 SWREQ 검증 완료 여부 | TODO: Pass / Fail / 일부 미완 | |
| ASIL D 커버리지 기준 충족 여부 | TODO | |
| 미해결 이슈 수용 가능 여부 | TODO | |
| **최종 판정** | **TODO: Approved / Conditionally Approved / Not Approved** | |

### 11.2 조건부 승인 조건 (해당 시)

| 조건 | 완료 기한 | 담당 | 비고 |
|------|-----------|------|------|
| TODO | TODO | TODO | |

---

## 12. 승인 이력 (Approval History)

| 역할 | 이름 | 서명 | 일시 |
|------|------|------|------|
| 작성자 | TODO | | TODO |
| 검토자 | TODO | | TODO |
| 안전 관리자 (Safety Manager) | TODO | | TODO |
| 승인자 | TODO | | TODO |

---

## 13. TODO 항목

- [ ] 단위 테스트 실행 및 결과 기록
- [ ] 통합 테스트 실행 및 결과 기록
- [ ] 타이밍 분석(WCET, E2E 지연) 수행 및 결과 기록
- [ ] 정적 분석 도구 실행 및 결과 기록
- [ ] MISRA/AUTOSAR 코딩 표준 검사 수행
- [ ] 모든 리뷰 수행 및 기록 완성
- [ ] 미해결 이슈 해결 또는 수용 결정
- [ ] MC/DC 커버리지 목표 확정 및 달성
- [ ] 최종 검증 판정 수행
- [ ] 승인자 서명 취득
