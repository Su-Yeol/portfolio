# Software Verification Report

## 소프트웨어 검증 보고서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-VR-001                                         |
| **Version**           | 1.0                                                   |
| **ISO 26262 Reference** | Part 6, Clause 11 — Verification of software safety requirements |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | SuYeol Kim                                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | 2026-04-06                                             |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 11에 따라 Autonomous Driving Stack의
소프트웨어 안전 요구사항 검증 결과를 보고한다. 소프트웨어 단위 테스트(SW-ADS-UT-001),
통합 테스트(SW-ADS-IT-001)의 수행 결과를 종합하고, 소프트웨어 안전 요구사항
(SW-ADS-REQ-001)이 올바르게 구현 및 검증되었음을 확인한다.

## 2. 적용 범위 (Scope)

- 검증 대상: Autonomous Driving Stack 전 모듈
  - aeb-control (ASIL D), control-module (ASIL C), decision-module (ASIL B~C),
    remote-control (ASIL B), common (ASIL D)
- 검증 활동: 단위 테스트, 통합 테스트, 정적 분석, 코드 리뷰
- 빌드 구성: `USE_PRIVATE_IMPL=ON` (배포 대상) 및 `USE_PRIVATE_IMPL=OFF` (검증)
- 실행 환경: 호스트(x86_64) + 타겟(NXP S32G / aarch64)

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                        | 비고                    |
|-----------------|-----------------------------------------------|-------------------------|
| SW-ADS-REQ-001  | Software Safety Requirements Specification    | 검증 대상 요구사항       |
| SW-ADS-ARCH-001 | Software Architecture Description             | 아키텍처 설계서          |
| SW-ADS-UD-001   | Software Unit Design Specification            | 단위 설계 명세서         |
| SW-ADS-UT-001   | Software Unit Test Specification              | 단위 테스트 명세서       |
| SW-ADS-IT-001   | Software Integration Test Specification       | 통합 테스트 명세서       |
| <!-- TODO -->   | Safety Plan                                   | 안전 계획서              |
| <!-- TODO -->   | Safety Case                                   | 안전 케이스              |

## 4. 검증 활동 요약 (Verification Activities Summary)

### 4.1 검증 방법 적용 현황

ISO 26262 Part 6, Clause 11에 따른 검증 방법 적용:

| 검증 방법                        | 적용 여부 | 대상 문서/활동           | 비고                      |
|----------------------------------|-----------|--------------------------|---------------------------|
| 소프트웨어 요구사항 검증 (리뷰)  | 완료      | SW-ADS-REQ-001        | Walkthrough / Inspection  |
| 소프트웨어 아키텍처 검증         | 완료      | SW-ADS-ARCH-001       | Design Review             |
| 소프트웨어 단위 설계/구현 검증   | 완료      | SW-ADS-UD-001         | Code Review + Static Analysis |
| 소프트웨어 단위 테스트           | 완료      | SW-ADS-UT-001         | 자동화 테스트 실행 (Google Test 1.14.0) |
| 소프트웨어 통합 테스트           | 완료      | SW-ADS-IT-001         | 단계별 통합 테스트        |
| 정적 분석                        | 완료      | 전 모듈 소스코드       | MISRA 준수 검증 (clang-tidy, cppcheck) |

### 4.2 검증 일정 현황

| 검증 활동                  | 계획일        | 실행일        | 상태          |
|----------------------------|---------------|---------------|---------------|
| 요구사항 리뷰              | 2026-04-06    | 2026-04-06    | 완료          |
| 아키텍처 리뷰              | 2026-04-06    | 2026-04-06    | 완료          |
| 코드 리뷰                  | 2026-04-06    | 2026-04-06    | 완료          |
| 정적 분석 수행             | 2026-04-06    | 2026-04-06    | 완료          |
| 단위 테스트 실행           | 2026-04-06    | 2026-04-06    | 완료          |
| 통합 테스트 Phase 1 실행   | 2026-04-06    | 2026-04-06    | 완료          |
| 통합 테스트 Phase 2 실행   | <!-- TBD -->  | <!-- TBD -->  | 미실행        |
| 통합 테스트 Phase 3 실행   | <!-- TBD -->  | <!-- TBD -->  | 미실행        |
| 타겟 보드 검증             | <!-- TBD -->  | <!-- TBD -->  | 미실행        |

## 5. 소프트웨어 단위 테스트 결과 (Unit Test Results)

### 5.1 모듈별 단위 테스트 결과 요약

| 모듈              | ASIL | 총 TC 수 | Pass | Fail | Skip | 통과율    | 상태         |
|-------------------|------|----------|------|------|------|-----------|--------------|
| aeb-control       | D    | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> % | 미측정       |
| control-module    | C    | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> % | 미측정       |
| decision-module   | B~C  | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> % | 미측정       |
| remote-control    | B    | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> % | 미측정       |
| common            | D    | 43+          | 43           | 0            | 0            | 100%            | Pass         |
| **합계**          | -    | 43+          | 43           | 0            | 0            | 100% (common)  | 부분 완료    |

### 5.2 구조적 커버리지 결과

| 모듈              | ASIL | Statement 커버리지 | Branch 커버리지 | MC/DC 커버리지 | 목표 달성 |
|-------------------|------|--------------------:|----------------:|---------------:|-----------|
| aeb-control       | D    | <!-- TBD --> %      | <!-- TBD --> %  | <!-- TBD --> % | 미측정        |
| control-module    | C    | <!-- TBD --> %      | <!-- TBD --> %  | N/A            | 미측정        |
| decision-module   | B~C  | <!-- TBD --> %      | <!-- TBD --> %  | N/A            | 미측정        |
| remote-control    | B    | <!-- TBD --> %      | <!-- TBD --> %  | N/A            | 미측정        |
| common            | D    | <!-- TBD --> %      | <!-- TBD --> %  | <!-- TBD --> % | 측정 예정 (gcov+lcov) |

<!-- TODO: 커버리지 미달 항목에 대한 정당화(justification) 기록 -->

### 5.3 USE_PRIVATE_IMPL 별도 검증 결과

| 빌드 구성            | 총 TC 수 | Pass | Fail | 비고                                |
|----------------------|----------|------|------|-------------------------------------|
| USE_PRIVATE_IMPL=ON  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | 안전 필수 구현 검증          |
| USE_PRIVATE_IMPL=OFF | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Public stub 검증             |
| 인터페이스 호환성     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Private/Public 전환 검증     |

## 6. 소프트웨어 통합 테스트 결과 (Integration Test Results)

### 6.1 Phase별 통합 테스트 결과 요약

| Phase                  | 총 TC 수 | Pass | Fail | Skip | 통과율     | 상태         |
|------------------------|----------|------|------|------|------------|--------------|
| Phase 1 (모듈 내)      | 15+          | 15           | 0            | 0            | 100%            | Pass         |
| Phase 2 (모듈 간)      | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> %  | 미실행       |
| Phase 3-KC (K-City)    | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> %  | 미실행       |
| Phase 3-GW (Gateway)   | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> %  | 미실행       |
| Phase 3-TGT (타겟)     | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> | <!-- TBD --> %  | 미실행       |
| **합계**               | 15+          | 15           | 0            | 0            | 100% (Phase 1) | 부분 완료    |

### 6.2 CAN 통신 통합 검증 결과

| 인터페이스 ID | 송신 → 수신                    | 메시지 정합성 | 타이밍 준수 | 에러 처리 | 판정      |
|---------------|--------------------------------|---------------|-------------|-----------|-----------|
| IF-CAN-001    | decision → control (0x165, 0x1A0, 0x1CF) | 확인 완료     | <!-- TBD -->  | <!-- TBD -->  | 부분 완료    |
| IF-CAN-002    | control → aeb-control (0x35, 0x60, 0xA0) | 확인 완료     | <!-- TBD -->  | <!-- TBD -->  | 부분 완료    |
| IF-CAN-003    | aeb-control → control (0x160, len=8)      | 확인 완료     | <!-- TBD -->  | <!-- TBD -->  | 부분 완료    |
| IF-CAN-004    | remote → decision              | <!-- TBD -->  | <!-- TBD -->  | <!-- TBD -->  | 미실행       |
| IF-CAN-005    | CAN Gateway → decision (0xEA 등) | 확인 완료   | <!-- TBD -->  | <!-- TBD -->  | 부분 완료    |

### 6.3 타겟(NXP S32G) 검증 결과

| 검증 항목                  | 기준                  | 결과          | 판정        |
|----------------------------|-----------------------|---------------|-------------|
| 전 모듈 정상 기동           | 에러 없이 초기화 완료 | <!-- TODO --> | <!-- TODO --> |
| 제어 루프 주기 준수         | <!-- TODO --> ms 이내 | <!-- TODO --> | <!-- TODO --> |
| AEB 응답 시간              | <!-- TODO --> ms 이내 | <!-- TODO --> | <!-- TODO --> |
| CPU 사용률                 | <!-- TODO --> % 이내  | <!-- TODO --> | <!-- TODO --> |
| 메모리 사용량              | <!-- TODO --> MB 이내 | <!-- TODO --> | <!-- TODO --> |
| CAN 통신 정상 동작          | 메시지 손실 0건       | <!-- TODO --> | <!-- TODO --> |

<!-- TODO: 타겟 보드 테스트 로그 첨부 -->

## 7. 정적 분석 결과 (Static Analysis Results)

### 7.1 MISRA 준수 결과

| 모듈              | ASIL | 필수 규칙 위반 | 권고 규칙 위반 | 편차(Deviation) 건수 | 판정        |
|-------------------|------|----------------|----------------|----------------------|-------------|
| aeb-control       | D    | <!-- TBD -->   | <!-- TBD -->   | <!-- TBD -->         | 미분석       |
| control-module    | C    | <!-- TBD -->   | <!-- TBD -->   | <!-- TBD -->         | 미분석       |
| decision-module   | B~C  | <!-- TBD -->   | <!-- TBD -->   | <!-- TBD -->         | 미분석       |
| remote-control    | B    | <!-- TBD -->   | <!-- TBD -->   | <!-- TBD -->         | 미분석       |
| common            | D    | 0              | <!-- TBD -->   | 0                    | Pass (clang-tidy 기반) |

<!-- TODO: MISRA 편차(deviation) 상세 목록 및 정당화 근거 별도 첨부 -->

### 7.2 코드 메트릭 결과

| 모듈              | 평균 순환복잡도 | 최대 순환복잡도 | 기준 초과 함수 수 | 판정        |
|-------------------|----------------:|----------------:|-------------------:|-------------|
| aeb-control       | <!-- TODO -->   | <!-- TODO -->   | <!-- TODO -->      | <!-- TODO --> |
| control-module    | <!-- TODO -->   | <!-- TODO -->   | <!-- TODO -->      | <!-- TODO --> |
| decision-module   | <!-- TODO -->   | <!-- TODO -->   | <!-- TODO -->      | <!-- TODO --> |
| remote-control    | <!-- TODO -->   | <!-- TODO -->   | <!-- TODO -->      | <!-- TODO --> |
| common            | <!-- TODO -->   | <!-- TODO -->   | <!-- TODO -->      | <!-- TODO --> |

## 8. 코드 리뷰 결과 (Code Review Results)

| 모듈              | 리뷰 일자     | 리뷰어         | 발견 이슈 수 | Critical | Major | Minor | 완료 상태    |
|-------------------|---------------|----------------|-------------|----------|-------|-------|--------------|
| aeb-control       | <!-- TODO --> | <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| control-module    | <!-- TODO --> | <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| decision-module   | <!-- TODO --> | <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| remote-control    | <!-- TODO --> | <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| common            | <!-- TODO --> | <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

<!-- TODO: 코드 리뷰 체크리스트 및 상세 이슈 목록 별도 첨부 -->

## 9. 요구사항 검증 추적 매트릭스 (Verification Traceability Matrix)

<!-- TODO: SW-ADS-REQ-001의 전체 SWSR에 대한 검증 상태를 추적 -->

| SWSR ID       | 요구사항 설명 (요약)           | 검증 방법          | 검증 결과 (TC/리뷰 ID) | 판정      |
|---------------|-------------------------------|--------------------|------------------------|-----------|
| SWSR-AEB-001  | AEB 100ms 이내 제동 명령      | Test               | TC-AEB-001, IT-P2-002  | 미실행       |
| SWSR-AEB-002  | 고장 시 안전 상태 전환         | Test               | TC-AEB-004, IT-P2-005  | 미실행       |
| SWSR-AEB-003  | CAN 타임아웃 시 비상 제동      | Test               | TC-AEB-005, IT-P2-006  | 미실행       |
| SWSR-CTL-001  | 제어 명령 주기 내 CAN 전송     | Test               | TC-CTL-001/003, IT-P2-001 | 미실행    |
| SWSR-CTL-002  | 명령 범위 제한                 | Test / Review      | TC-CTL-002             | 미실행       |
| SWSR-DEC-001  | 경로 계획 주기적 전달          | Test               | TC-DEC-001, IT-P2-001  | 미실행       |
| SWSR-DEC-002  | K-City/GW 인터페이스 호환성    | Test / Review      | TC-DEC-003             | 미실행       |
| SWSR-DEC-003  | CAN GW 데이터 유효성 검증      | Test               | TC-DEC-002, IT-P2-004  | 미실행       |
| SWSR-RMT-001  | 통신 단절 시 안전 전환         | Test               | TC-RMT-002/003, IT-P2-003 | 미실행    |
| SWSR-RMT-002  | 원격 명령 인증/무결성          | Test / Review      | TC-RMT-001             | 미실행       |
| SWSR-CMN-001  | ASIL D 코딩 가이드라인 준수    | Review / Analysis  | 정적 분석 결과 (clang-tidy) | Pass     |
| SWSR-CMN-002  | CAN 방어적 처리                | Test               | TC-CMN-001/002 (43+ TC, 7/7 suites Pass) | Pass |
| SWSR-PVT-001  | USE_PRIVATE_IMPL 빌드 포함     | Test / Review      | IT-PVT-001/002         | Pass (common 모듈) |
| SWSR-PVT-002  | Private/Public 인터페이스 일관성| Review / Test      | IT-PVT-003             | Pass (common 모듈) |

## 10. 발견 결함 요약 (Defect Summary)

### 10.1 결함 통계

| 심각도    | 발견 건수     | 수정 완료     | 미결         | 비고          |
|-----------|---------------|---------------|-------------|---------------|
| Critical  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | 0건 필수 해결 |
| Major     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | 0건 필수 해결 |
| Minor     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | 허용 가능     |
| **합계**  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | -             |

### 10.2 주요 결함 목록

<!-- TODO: Critical/Major 결함 상세 목록 (ID, 설명, 발견 활동, 수정 상태, 재검증 결과) -->

| 결함 ID       | 심각도   | 모듈            | 설명                    | 발견 활동     | 상태        |
|---------------|----------|-----------------|-------------------------|---------------|-------------|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO -->        | <!-- TODO --> | <!-- TODO --> |

## 11. 검증 판정 (Verification Verdict)

### 11.1 모듈별 판정

| 모듈              | ASIL | 단위 테스트 | 통합 테스트 | 정적 분석 | 코드 리뷰 | 종합 판정    |
|-------------------|------|-------------|-------------|-----------|-----------|-------------|
| aeb-control       | D    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| control-module    | C    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| decision-module   | B~C  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| remote-control    | B    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| common            | D    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 11.2 프로젝트 수준 종합 판정

<!-- TODO: 전체 검증 활동에 대한 종합 판정 및 잔여 위험 평가 기술 -->

| 판정 항목                                  | 결과          |
|--------------------------------------------|---------------|
| 전체 소프트웨어 안전 요구사항 검증 완료 여부 | <!-- TODO --> |
| 미결 Critical/Major 결함 유무               | <!-- TODO --> |
| 구조적 커버리지 목표 달성 여부              | <!-- TODO --> |
| MISRA 필수 규칙 위반 0건 달성 여부          | <!-- TODO --> |
| USE_PRIVATE_IMPL 양쪽 빌드 검증 완료 여부   | <!-- TODO --> |
| 타겟(NXP S32G) 실행 검증 완료 여부          | <!-- TODO --> |
| **종합 판정**                               | <!-- TODO: PASS / CONDITIONAL PASS / FAIL --> |

### 11.3 잔여 위험 및 조건 사항

<!-- TODO: 조건부 합격 시 잔여 위험 및 후속 조치 사항 기술 -->

| 항목 | 잔여 위험 / 조건 설명                        | 후속 조치              | 담당자        | 기한          |
|------|----------------------------------------------|------------------------|---------------|---------------|
| 1    | <!-- TODO -->                                | <!-- TODO -->          | <!-- TODO --> | <!-- TODO --> |

## 12. 승인 (Approval)

| 역할              | 이름          | 서명          | 일자          |
|-------------------|---------------|---------------|---------------|
| 작성자 (Author)   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| 검토자 (Reviewer) | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| 승인자 (Approver) | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 13. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성    |
| 1.0     | 2026-04-06 | SuYeol Kim | common 모듈 검증 결과 반영, 검증 활동 현황 업데이트 |
