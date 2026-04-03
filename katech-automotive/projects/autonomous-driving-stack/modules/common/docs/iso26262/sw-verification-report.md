# SW-CMN-VR-001: Software Verification Report

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-VR-001                                                       |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 검증 보고서                                     |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 11 — Verification of software safety requirements     |
| **ASIL 등급 (ASIL)**       | ASIL-B (상위 모듈 상속) <!-- TODO: 시스템 안전 분석 후 확정 -->          |
| **모듈 (Module)**          | Common (Shared Utilities)                                           |
| **상위 프로젝트 (Project)** | Autonomous Driving Stack                                             |
| **Target HW**             | NXP S32G (aarch64)                                                  |
| **작성자 (Author)**        | <!-- TODO: 작성자 기입 -->                                            |
| **검토자 (Reviewer)**      | <!-- TODO: 검토자 기입 -->                                            |
| **승인자 (Approver)**      | <!-- TODO: 승인자 기입 -->                                            |
| **버전 (Version)**         | 0.1 (Draft)                                                         |
| **작성일 (Date)**          | 2026-04-03                                                          |
| **상태 (Status)**          | Draft                                                               |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262-6:2018 Clause 11에 따라 Common 모듈의 소프트웨어 안전 요구사항 검증 결과를 종합 보고한다.
단위 테스트(Clause 9), 통합 테스트(Clause 10) 결과를 취합하여 모든 소프트웨어 안전 요구사항이
충족되었는지 평가하고, 잔여 결함 및 미완료 항목을 식별한다.

---

## 2. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Part 6, Clause 11 — Verification of software safety requirements |
| SW-CMN-REQ-001  | Common 모듈 소프트웨어 안전 요구사항 명세서                  |
| SW-CMN-ARCH-001 | Common 모듈 소프트웨어 아키텍처 설계서                       |
| SW-CMN-UD-001   | Common 모듈 소프트웨어 단위 설계 명세서                      |
| SW-CMN-UT-001   | Common 모듈 소프트웨어 단위 테스트 명세서                    |
| SW-CMN-IT-001   | Common 모듈 소프트웨어 통합 테스트 명세서                    |

---

## 3. 검증 범위 (Verification Scope)

### 3.1 대상 컴포넌트

| 컴포넌트                | 소스 파일                    | 검증 대상 여부 |
|------------------------|----------------------------|-------------|
| CrcProviderPublic      | `CrcProviderPublic.cpp`     | 예          |
| CrcProviderPrivate     | `CrcProviderPrivate.cpp`    | 예          |
| SignalCodec            | `SignalCodec.h`             | 예          |
| CanSpecConfigLoader    | `CanSpecConfigLoader.cpp`   | 예          |
| ConfigParser           | `ConfigParser.cpp`          | 예          |

### 3.2 검증 활동 요약

| 검증 활동                              | 문서 참조         | 상태                        |
|---------------------------------------|-----------------|----------------------------|
| 소프트웨어 안전 요구사항 리뷰             | SW-CMN-REQ-001  | <!-- TODO: 완료/미완료 -->   |
| 아키텍처 설계 리뷰                       | SW-CMN-ARCH-001 | <!-- TODO: 완료/미완료 -->   |
| 단위 설계 리뷰                          | SW-CMN-UD-001   | <!-- TODO: 완료/미완료 -->   |
| 단위 테스트 수행                         | SW-CMN-UT-001   | <!-- TODO: 완료/미완료 -->   |
| 통합 테스트 수행                         | SW-CMN-IT-001   | <!-- TODO: 완료/미완료 -->   |
| 정적 코드 분석                          | —               | <!-- TODO: 완료/미완료 -->   |
| 코드 커버리지 측정                       | —               | <!-- TODO: 완료/미완료 -->   |

---

## 4. 단위 테스트 결과 요약 (Unit Test Results) — Clause 9

### 4.1 테스트 실행 통계

| 항목                         | 결과                                |
|-----------------------------|-------------------------------------|
| 총 테스트 케이스 수            | <!-- TODO: 수치 기입 -->              |
| 통과 (Pass)                  | <!-- TODO: 수치 기입 -->              |
| 실패 (Fail)                  | <!-- TODO: 수치 기입 -->              |
| 차단 (Blocked)               | <!-- TODO: 수치 기입 -->              |
| 미실행 (Not Executed)         | <!-- TODO: 수치 기입 -->              |
| **통과율 (Pass Rate)**        | <!-- TODO: % 기입 -->                |

### 4.2 컴포넌트별 단위 테스트 결과

| 컴포넌트              | 테스트 수 | Pass | Fail | 통과율    | 비고                          |
|----------------------|----------|------|------|----------|------------------------------|
| CrcProviderPublic    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| CrcProviderPrivate   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| SignalCodec          | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| CanSpecConfigLoader  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| ConfigParser         | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |

### 4.3 구조적 커버리지 결과

| 커버리지 기준          | 목표      | 측정값              | 판정                        |
|----------------------|----------|---------------------|----------------------------|
| Statement coverage   | >= 90%   | <!-- TODO: % 기입 --> | <!-- TODO: Pass/Fail -->   |
| Branch coverage      | >= 80%   | <!-- TODO: % 기입 --> | <!-- TODO: Pass/Fail -->   |
| MC/DC (해당 시)       | —        | <!-- TODO: % 기입 --> | <!-- TODO: Pass/Fail -->   |

---

## 5. 통합 테스트 결과 요약 (Integration Test Results) — Clause 10

### 5.1 테스트 실행 통계

| 항목                         | 결과                                |
|-----------------------------|-------------------------------------|
| 총 테스트 케이스 수            | <!-- TODO: 수치 기입 -->              |
| 통과 (Pass)                  | <!-- TODO: 수치 기입 -->              |
| 실패 (Fail)                  | <!-- TODO: 수치 기입 -->              |
| 미실행 (Not Executed)         | <!-- TODO: 수치 기입 -->              |
| **통과율 (Pass Rate)**        | <!-- TODO: % 기입 -->                |

### 5.2 통합 레벨별 결과

| 통합 레벨                           | 테스트 수     | Pass        | Fail        | 비고 |
|------------------------------------|-------------|-------------|-------------|------|
| Level 1 (내부 통합)                  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| Level 2 (라이브러리 통합)             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| Level 3 (소비 모듈 통합)             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |
| 결함 주입 (Fault Injection)          | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | |

### 5.3 빌드 검증 결과

| 빌드 구성                           | 타겟         | 결과                        |
|------------------------------------|-------------|----------------------------|
| `USE_PRIVATE_IMPL=0` (Public)      | x86_64      | <!-- TODO: Pass/Fail -->   |
| `USE_PRIVATE_IMPL=1` (Private)     | x86_64      | <!-- TODO: Pass/Fail -->   |
| `USE_PRIVATE_IMPL=0` (Public)      | aarch64 S32G| <!-- TODO: Pass/Fail -->   |
| `USE_PRIVATE_IMPL=1` (Private)     | aarch64 S32G| <!-- TODO: Pass/Fail -->   |

---

## 6. 안전 요구사항 검증 추적성 (Safety Requirement Verification Traceability)

| 안전 요구사항 ID          | 검증 방법         | 검증 증거 (TC ID)                    | 판정                        |
|--------------------------|-----------------|-------------------------------------|----------------------------|
| SW-CMN-REQ-CRC-001      | 단위 테스트       | UT-CRC-001, UT-CRC-006              | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CRC-002      | 단위 테스트       | UT-CRC-004, UT-CRC-005              | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CRC-003      | 코드 리뷰        | UT-CRC-007                          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CRC-004      | 단위 테스트       | UT-CRC-002, UT-CRC-003              | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-SIG-001      | 단위 테스트       | UT-SIG-001 ~ 003, 009, 012, 013    | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-SIG-002      | 단위 테스트       | UT-SIG-004                          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-SIG-003      | 단위 테스트       | UT-SIG-005 ~ 007                    | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-SIG-004      | 단위 테스트       | UT-SIG-010, 011                     | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-SIG-005      | 단위 테스트       | UT-SIG-008                          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CFG-001      | 단위/통합 테스트   | UT-CFG-001~002, IT-INT-001          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CFG-002      | 단위/통합 테스트   | UT-CFG-006, IT-INT-002, IT-FI-002   | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CFG-003      | 단위 테스트       | UT-CFG-005                          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-CFG-004      | 단위/통합 테스트   | UT-CFG-007, IT-INT-004              | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-GEN-001      | 통합 테스트       | IT-LIB-001~002, IT-MOD-005          | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-GEN-002      | 코드 리뷰        | <!-- TODO: 리뷰 기록 참조 -->         | <!-- TODO: Pass/Fail -->   |
| SW-CMN-REQ-GEN-003      | 통합 테스트       | IT-LIB-003                          | <!-- TODO: Pass/Fail -->   |

---

## 7. 정적 분석 결과 (Static Analysis Results)

| 도구                    | 대상 파일                           | 결과 요약                          |
|------------------------|------------------------------------|------------------------------------|
| <!-- TODO: 도구명 -->   | `CrcProviderPublic.cpp`            | <!-- TODO: 위반 건수, 심각도 -->    |
| <!-- TODO: 도구명 -->   | `CrcProviderPrivate.cpp`           | <!-- TODO: 위반 건수, 심각도 -->    |
| <!-- TODO: 도구명 -->   | `CanSpecConfigLoader.cpp`          | <!-- TODO: 위반 건수, 심각도 -->    |
| <!-- TODO: 도구명 -->   | `ConfigParser.cpp`                 | <!-- TODO: 위반 건수, 심각도 -->    |
| <!-- TODO: 도구명 -->   | `SignalCodec.h`                    | <!-- TODO: 위반 건수, 심각도 -->    |

### MISRA C++ 준수 요약

| 항목                    | 결과                                |
|------------------------|-------------------------------------|
| 총 규칙 위반 건수         | <!-- TODO: 수치 기입 -->              |
| Mandatory 위반           | <!-- TODO: 수치 기입 (목표: 0건) -->   |
| Required 위반            | <!-- TODO: 수치 기입 -->              |
| Advisory 위반            | <!-- TODO: 수치 기입 -->              |
| 승인된 일탈 (Deviation)   | <!-- TODO: 수치 기입 -->              |

---

## 8. 결함 요약 (Defect Summary)

### 8.1 발견된 결함

| 결함 ID     | 심각도     | 컴포넌트            | 설명                            | 상태                        |
|------------|-----------|--------------------|---------------------------------|----------------------------|
| <!-- TODO: 결함 기록 --> | | | | |

### 8.2 결함 통계

| 심각도      | 발견 건수 | 수정 완료 | 미수정 | 수용 (허용) |
|------------|----------|----------|--------|------------|
| Critical   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| High       | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Medium     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Low        | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

---

## 9. 검증 판정 (Verification Verdict)

### 9.1 판정 기준

| 기준                                              | 목표                    | 결과                        |
|--------------------------------------------------|------------------------|-----------------------------|
| 모든 안전 요구사항에 대한 검증 증거 존재               | 100%                   | <!-- TODO: % 기입 -->        |
| 단위 테스트 통과율                                   | 100%                   | <!-- TODO: % 기입 -->        |
| 통합 테스트 통과율                                   | 100%                   | <!-- TODO: % 기입 -->        |
| Statement coverage                                | >= 90%                 | <!-- TODO: % 기입 -->        |
| Branch coverage                                   | >= 80%                 | <!-- TODO: % 기입 -->        |
| 미수정 Critical/High 결함                           | 0건                    | <!-- TODO: 수치 기입 -->      |
| MISRA Mandatory 위반                               | 0건                    | <!-- TODO: 수치 기입 -->      |

### 9.2 종합 판정

<!-- TODO: 모든 검증 활동 완료 후 종합 판정 기입 -->

| 항목              | 판정                                       |
|------------------|--------------------------------------------|
| **종합 판정**      | <!-- TODO: PASS / CONDITIONAL PASS / FAIL --> |
| **조건부 수용 사유** | <!-- TODO: 해당 시 사유 기입 -->               |
| **잔여 리스크**     | <!-- TODO: 해당 시 리스크 기입 -->              |

---

## 10. 승인 (Approval)

| 역할         | 성명                        | 서명         | 일자                        |
|-------------|----------------------------|-------------|----------------------------|
| 작성자       | <!-- TODO: 성명 기입 -->     | <!-- TODO --> | <!-- TODO: 일자 기입 -->    |
| 검토자       | <!-- TODO: 성명 기입 -->     | <!-- TODO --> | <!-- TODO: 일자 기입 -->    |
| 승인자       | <!-- TODO: 성명 기입 -->     | <!-- TODO --> | <!-- TODO: 일자 기입 -->    |
| 안전 관리자   | <!-- TODO: 성명 기입 -->     | <!-- TODO --> | <!-- TODO: 일자 기입 -->    |

---

## 11. TODO 요약

- [ ] 모든 검증 활동 수행 후 결과 데이터 기입
- [ ] 단위 테스트 실행 결과 및 커버리지 수치 반영
- [ ] 통합 테스트 실행 결과 반영
- [ ] 정적 분석 도구 실행 결과 및 MISRA 준수 현황 반영
- [ ] 발견된 결함 기록 및 추적
- [ ] 안전 요구사항 검증 추적성 매트릭스 판정 완성
- [ ] 종합 판정 기입
- [ ] 작성자/검토자/승인자/안전 관리자 서명
