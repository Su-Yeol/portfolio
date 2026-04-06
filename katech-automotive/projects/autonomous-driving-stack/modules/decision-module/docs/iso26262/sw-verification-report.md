# Software Verification Report

## ISO 26262 Part 6, Clause 11 — 소프트웨어 검증

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-VR-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 11 (Verification of software safety requirements) |
| **ASIL** | B~C (기능별 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | SuYeol Kim |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 1.0 |
| **Date** | 2026-04-06 |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 11에 따라 Decision Module의 소프트웨어 안전 요구사항 검증 결과를 종합적으로 보고한다. 단위 테스트, 통합 테스트, 정적 분석, 코드 리뷰 등 모든 검증 활동의 결과를 취합하고 잔존 위험을 평가한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-REQ-001 | Software Safety Requirements Specification | 안전 요구사항 |
| SW-DEC-ARCH-001 | Software Architecture Description | 아키텍처 문서 |
| SW-DEC-UD-001 | Software Unit Design Specification | 단위 설계 문서 |
| SW-DEC-UT-001 | Software Unit Test Specification | 단위 테스트 문서 |
| SW-DEC-IT-001 | Software Integration Test Specification | 통합 테스트 문서 |
| ISO 26262:2018 Part 6, Clause 11 | Verification of software safety requirements | 국제 표준 |

---

## 3. 검증 활동 요약 (Verification Activities Summary)

### 3.1 검증 활동 목록

| 활동 | 대상 | 방법 | 결과 문서 | 상태 |
|------|------|------|----------|------|
| 안전 요구사항 리뷰 | SW-DEC-REQ-001 | Inspection | TODO | TODO |
| 아키텍처 리뷰 | SW-DEC-ARCH-001 | Walkthrough | TODO | TODO |
| 단위 설계 리뷰 | SW-DEC-UD-001 | Inspection | TODO | TODO |
| 정적 코드 분석 | 전체 소스 코드 | 자동 분석 도구 | TODO | TODO |
| 단위 테스트 | 각 소프트웨어 단위 | 테스트 실행 | SW-DEC-UT-001 | TODO |
| 통합 테스트 | 통합된 컴포넌트 | 테스트 실행 | SW-DEC-IT-001 | TODO |
| 코드 커버리지 분석 | 전체 소스 코드 | 커버리지 도구 | TODO | TODO |
| 코딩 가이드라인 준수 확인 | 전체 소스 코드 | 정적 분석 + 리뷰 | TODO | TODO |

---

## 4. 안전 요구사항 검증 결과 (Safety Requirements Verification Results)

### 4.1 요구사항별 검증 현황

| SW Safety Req | 설명 | 검증 방법 | 검증 결과 | 잔존 이슈 |
|---------------|------|----------|----------|----------|
| SWR-DEC-001 | 경로 계획 데이터 수신 및 검증 | UT + IT | TODO | TODO |
| SWR-DEC-002 | 보행자 감지 시 긴급 판단 | UT + IT + HIL | TODO | TODO |
| SWR-DEC-003 | GPS 신호 유실 대응 | UT + IT + Fault Injection | TODO | TODO |
| SWR-DEC-004 | LiDAR 장애물 감지 처리 | UT + IT | TODO | TODO |
| SWR-DEC-005 | CAN Gateway 메시지 무결성 | UT + IT | TODO | TODO |
| SWR-DEC-006 | Variant 간 공통 안전 메커니즘 | UT + IT + Code Review | TODO | TODO |
| SWR-DEC-NF-001 | 주행 판단 주기 | 성능 테스트 | TODO | TODO |
| SWR-DEC-NF-002 | 메모리 사용량 | 리소스 모니터링 | TODO | TODO |
| SWR-DEC-NF-003 | CPU 점유율 | 리소스 모니터링 | TODO | TODO |
| SWR-DEC-NF-004 | config.ini 로드 실패 시 기본값 | UT | TODO | TODO |

### 4.2 검증 커버리지 요약

| 항목 | 전체 | 검증 완료 | 미검증 | 비율 |
|------|------|----------|--------|------|
| 기능 안전 요구사항 | 6 | TODO | TODO | TODO% |
| 비기능 안전 요구사항 | 4 | TODO | TODO | TODO% |

---

## 5. 단위 테스트 결과 (Unit Test Results)

### 5.1 K-City Variant

| Unit | 전체 TC | Pass | Fail | Block | Statement Cov. | Branch Cov. | MC/DC |
|------|---------|------|------|-------|---------------|-------------|-------|
| MainModule | TODO | TODO | TODO | TODO | TODO% | TODO% | TODO% |
| Communicator | TODO | TODO | TODO | TODO | TODO% | TODO% | TODO% |
| PathManager | TODO | TODO | TODO | TODO | TODO% | TODO% | TODO% |

### 5.2 CAN Gateway Variant

| Unit | 전체 TC | Pass | Fail | Block | Statement Cov. | Branch Cov. | MC/DC |
|------|---------|------|------|-------|---------------|-------------|-------|
| CANGateway | TODO | TODO | TODO | TODO | TODO% | TODO% | TODO% |
| CRC | TODO | TODO | TODO | TODO | TODO% | TODO% | TODO% |

### 5.3 커버리지 목표 달성 현황

| 커버리지 기준 | 목표 (ASIL별) | K-City 달성 | Gateway 달성 | 판정 |
|-------------|-------------|------------|-------------|------|
| Statement Coverage | >= 95% | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 |
| Branch Coverage | >= 90% | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 |
| MC/DC | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 |

---

## 6. 통합 테스트 결과 (Integration Test Results)

### 6.1 K-City Variant

| 테스트 영역 | 전체 TC | Pass | Fail | Block | 비고 |
|------------|---------|------|------|-------|------|
| 내부 컴포넌트 통합 | TODO | TODO | TODO | TODO | IT-K-001 ~ 004 |
| 외부 인터페이스 통합 | TODO | TODO | TODO | TODO | IT-K-011 ~ 015 |
| Fault Injection | TODO | TODO | TODO | TODO | IT-K-F01 ~ F04 |

### 6.2 CAN Gateway Variant

| 테스트 영역 | 전체 TC | Pass | Fail | Block | 비고 |
|------------|---------|------|------|-------|------|
| 내부 컴포넌트 통합 | TODO | TODO | TODO | TODO | IT-G-001 ~ 003 |
| 외부 인터페이스 통합 | TODO | TODO | TODO | TODO | IT-G-011 ~ 013 |
| Fault Injection | TODO | TODO | TODO | TODO | IT-G-F01 ~ F02 |

### 6.3 Cross-Variant

| 테스트 영역 | 전체 TC | Pass | Fail | Block | 비고 |
|------------|---------|------|------|-------|------|
| Cross-Variant 통합 | TODO | TODO | TODO | TODO | IT-X-001 ~ 002 |

---

## 7. 정적 분석 결과 (Static Analysis Results)

### 7.1 정적 분석 도구 실행 결과

| 도구 | 대상 | 결과 | 미해결 항목 | 비고 |
|------|------|------|-----------|------|
| cppcheck | 전체 C++ 소스 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 정적 분석 |
| clang-tidy | 전체 C++ 소스 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 정적 분석 |
| MISRA C++:2008 (clang-tidy) | 안전 관련 소스 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | MISRA 준수 검증 |

### 7.2 코딩 가이드라인 준수 현황

| 가이드라인 | 전체 규칙 | 준수 | 위반 | 면제(Deviation) | 비고 |
|-----------|----------|------|------|----------------|------|
| MISRA C++:2008 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | clang-tidy MISRA 체커 |
| AUTOSAR C++14 | 참조 적용 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 | 참조 표준 |

> TODO: 위반 사항에 대한 면제 근거를 작성할 것

---

## 8. 코드 리뷰 결과 (Code Review Results)

| 리뷰 대상 | 리뷰어 | 일자 | 발견 사항 | 해결 상태 | 비고 |
|----------|--------|------|----------|----------|------|
| source_kcity/MainModule.cpp | TODO | TODO | TODO | TODO | TODO |
| source_kcity/Communicator.cpp | TODO | TODO | TODO | TODO | TODO |
| source_kcity/PathManager.cpp | TODO | TODO | TODO | TODO | TODO |
| source_gateway/CANGateway.cpp | TODO | TODO | TODO | TODO | TODO |
| source_gateway/CRC.cpp | TODO | TODO | TODO | TODO | TODO |
| ../common/src/ | TODO | TODO | TODO | TODO | TODO |

---

## 9. 안전 메커니즘 검증 (Safety Mechanism Verification)

| ID | 안전 메커니즘 | 검증 방법 | 검증 결과 | 비고 |
|----|-------------|----------|----------|------|
| SM-DEC-001 | CRC 검증 | UT-G-002-xx, IT-G-013 | TODO | CAN 메시지 무결성 |
| SM-DEC-002 | GPS 신호 타임아웃 감시 | UT-K-001-03, IT-K-F01 | TODO | Safe State 전환 |
| SM-DEC-003 | LiDAR 데이터 유효성 검사 | UT-K-002-04, IT-K-F02 | TODO | Fallback 동작 |
| SM-DEC-004 | 경로 계획 데이터 범위 검사 | UT-K-001-05, IT-K-F03 | TODO | 이전 경로 유지 |
| SM-DEC-005 | Watchdog 타이머 | TODO | TODO | 시스템 리셋 |

---

## 10. Target 환경 검증 (Target Environment Verification)

| 항목 | Host (x86_64) 결과 | Target (NXP S32G aarch64) 결과 | 비고 |
|------|-------------------|-------------------------------|------|
| 빌드 성공 | TODO | TODO | Cross-compile |
| 단위 테스트 | TODO | TODO | |
| 통합 테스트 | TODO | TODO | |
| 실행 시간 측정 | TODO | TODO | Real-time 요구사항 |
| 리소스 사용량 | TODO | TODO | RAM, CPU, Stack |

> TODO: Host 환경 결과와 Target 환경 결과의 차이 분석 및 동등성 논거(Equivalence Argument) 작성

---

## 11. 잔존 이슈 및 편차 (Residual Issues and Deviations)

### 11.1 미해결 결함 (Open Defects)

| Defect ID | 심각도 | 설명 | 영향 분석 | 대응 계획 | 상태 |
|----------|--------|------|----------|----------|------|
| TODO | TODO | TODO | TODO | TODO | TODO |

### 11.2 면제/편차 (Deviations)

| Deviation ID | 대상 | 사유 | 위험 평가 | 승인자 | 상태 |
|-------------|------|------|----------|--------|------|
| TODO | TODO | TODO | TODO | TODO | TODO |

---

## 12. 검증 결론 (Verification Conclusion)

### 12.1 종합 판정

| 평가 항목 | 판정 | 비고 |
|----------|------|------|
| 모든 SW 안전 요구사항 검증 완료 여부 | TODO: Pass/Fail/Incomplete | TODO |
| 코드 커버리지 목표 달성 여부 | TODO: Pass/Fail/Incomplete | TODO |
| 정적 분석 미해결 항목 유무 | TODO: None/Exists | TODO |
| 잔존 결함의 수용 가능성 | TODO: Acceptable/Unacceptable | TODO |
| Target 환경 검증 완료 여부 | TODO: Pass/Fail/Incomplete | TODO |

### 12.2 종합 의견

> TODO: 검증 결과에 대한 종합 의견을 작성할 것
>
> - Decision Module(K-City / CAN Gateway)의 소프트웨어 안전 요구사항 충족 여부
> - 잔존 위험의 수용 가능성
> - 추가 검증이 필요한 영역
> - 릴리스 권고 사항

---

## 13. 완전 추적성 매트릭스 (Full Traceability Matrix)

| System TSR | SW Safety Req | Architecture | Unit Design | Unit Test | Integration Test | Verification Result |
|-----------|---------------|-------------|------------|-----------|-----------------|-------------------|
| TODO | SWR-DEC-001 | MainModule, Communicator, PathManager | SWU-DEC-K-001~003 | UT-K-001~003 | IT-K-001~003, IT-K-011~015 | TODO |
| TODO | SWR-DEC-002 | MainModule | SWU-DEC-K-001 | UT-K-001-02 | IT-K-014, IT-K-F01 | TODO |
| TODO | SWR-DEC-003 | Communicator, MainModule | SWU-DEC-K-001~002 | UT-K-001-03 | IT-K-F01 | TODO |
| TODO | SWR-DEC-004 | Communicator, MainModule | SWU-DEC-K-001~002 | UT-K-002-02 | IT-K-012, IT-K-F02 | TODO |
| TODO | SWR-DEC-005 | CANGateway, CRC | SWU-DEC-G-001~002 | UT-G-001~002 | IT-G-001~003, IT-G-011~013 | TODO |
| TODO | SWR-DEC-006 | Common Library | TODO | TODO | IT-K-004, IT-G-003, IT-X-002 | TODO |

---

## 14. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0 | 2026-04-06 | SuYeol Kim | 정적 분석 도구, 커버리지 목표, ASIL, 메타데이터 갱신 |
