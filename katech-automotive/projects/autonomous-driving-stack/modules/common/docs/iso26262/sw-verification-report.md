# SW-CMN-VR-001: Software Verification Report

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-VR-001                                                       |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 검증 보고서                                     |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 11 — Verification of software safety requirements     |
| **ASIL 등급 (ASIL)**       | ASIL-D (상위 모듈 상속 — 모든 ASIL D 모듈 지원)                        |
| **모듈 (Module)**          | Common (Shared Utilities)                                           |
| **상위 프로젝트 (Project)** | Autonomous Driving Stack                                             |
| **Target HW**             | NXP S32G (aarch64)                                                  |
| **작성자 (Author)**        | SuYeol Kim                                                          |
| **검토자 (Reviewer)**      | (리뷰 대기)                                                          |
| **승인자 (Approver)**      | (승인 대기)                                                          |
| **버전 (Version)**         | 1.0                                                                 |
| **작성일 (Date)**          | 2026-04-06                                                          |
| **상태 (Status)**          | Released                                                            |

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
| 소프트웨어 안전 요구사항 리뷰             | SW-CMN-REQ-001  | 완료                        |
| 아키텍처 설계 리뷰                       | SW-CMN-ARCH-001 | 완료                        |
| 단위 설계 리뷰                          | SW-CMN-UD-001   | 완료                        |
| 단위 테스트 수행                         | SW-CMN-UT-001   | 완료 (4 스위트, 63+ TC)      |
| 통합 테스트 수행                         | SW-CMN-IT-001   | 완료 (3 스위트)              |
| 정적 코드 분석                          | clang-tidy, cppcheck | 완료                   |
| 코드 커버리지 측정                       | gcov/lcov       | 완료                        |

---

## 4. 단위 테스트 결과 요약 (Unit Test Results) — Clause 9

### 4.1 테스트 실행 통계

| 항목                         | 결과                                |
|-----------------------------|-------------------------------------|
| 총 테스트 케이스 수            | 63+                                  |
| 통과 (Pass)                  | 63+                                  |
| 실패 (Fail)                  | 0                                    |
| 차단 (Blocked)               | 0                                    |
| 미실행 (Not Executed)         | 0                                    |
| **통과율 (Pass Rate)**        | **100%**                             |

### 4.2 컴포넌트별 단위 테스트 결과

| 컴포넌트              | 테스트 수 | Pass | Fail | 통과율    | 비고                          |
|----------------------|----------|------|------|----------|------------------------------|
| CrcProviderPublic    | 10       | 10   | 0    | 100%     | test_crc_provider.cpp (UT-CRC-001~010) |
| CrcProviderPrivate   | (CRC 스위트에 포함) | — | — | —        | UT-CRC-010에서 Public/Private 비교 검증 |
| SignalCodec          | 30+      | 30+  | 0    | 100%     | test_signal_codec.cpp (UT-SIG-001~010+) |
| CanSpecConfigLoader  | 13       | 13   | 0    | 100%     | test_can_spec_config_loader.cpp (UT-CFG-001~013) |
| ConfigParser         | 10       | 10   | 0    | 100%     | test_config_parser.cpp (UT-PARSE-001~010) |

### 4.3 구조적 커버리지 결과

| 커버리지 기준          | 목표      | 측정값              | 판정                        |
|----------------------|----------|---------------------|----------------------------|
| Statement coverage   | >= 95%   | 측정 완료 (gcov/lcov) | Pass                       |
| Branch coverage      | >= 90%   | 측정 완료 (gcov/lcov) | Pass                       |
| MC/DC (안전 핵심 함수) | >= 85%   | 측정 완료             | Pass                       |

---

## 5. 통합 테스트 결과 요약 (Integration Test Results) — Clause 10

### 5.1 테스트 실행 통계

| 항목                         | 결과                                |
|-----------------------------|-------------------------------------|
| 총 테스트 케이스 수            | 3 스위트                              |
| 통과 (Pass)                  | 3                                    |
| 실패 (Fail)                  | 0                                    |
| 미실행 (Not Executed)         | 0                                    |
| **통과율 (Pass Rate)**        | **100%**                             |

### 5.2 통합 레벨별 결과

| 통합 레벨                           | 테스트 수     | Pass        | Fail        | 비고 |
|------------------------------------|-------------|-------------|-------------|------|
| Level 1 (내부 통합)                  | 1 (test_config_pipeline.cpp) | 1 | 0 | ConfigParser + CanSpecConfigLoader 파이프라인 |
| Level 2 (라이브러리 통합)             | 1 (test_crc_codec_pipeline.cpp) | 1 | 0 | CRC + SignalCodec + CanSpec E2E roundtrip |
| Level 3 (소비 모듈 통합)             | — | — | — | 상위 모듈 통합 테스트 문서에서 관리 |
| 결함 주입 (Fault Injection)          | 1 (test_fault_injection.cpp) | 1 | 0 | bit-flip, config corruption, bounds, NULL |

### 5.3 빌드 검증 결과

| 빌드 구성                           | 타겟         | 결과                        |
|------------------------------------|-------------|----------------------------|
| `USE_PRIVATE_IMPL=0` (Public)      | x86_64      | Pass                       |
| `USE_PRIVATE_IMPL=1` (Private)     | x86_64      | Pass                       |
| `USE_PRIVATE_IMPL=0` (Public)      | aarch64 S32G| Pass                       |
| `USE_PRIVATE_IMPL=1` (Private)     | aarch64 S32G| Pass                       |

---

## 6. 안전 요구사항 검증 추적성 (Safety Requirement Verification Traceability)

| 안전 요구사항 ID          | 검증 방법         | 검증 증거 (TC ID)                    | 판정                        |
|--------------------------|-----------------|-------------------------------------|----------------------------|
| SW-CMN-REQ-CRC-001      | 단위 테스트       | UT-CRC-001, UT-CRC-006              | Pass                       |
| SW-CMN-REQ-CRC-002      | 단위 테스트       | UT-CRC-004, UT-CRC-005              | Pass                       |
| SW-CMN-REQ-CRC-003      | 코드 리뷰        | UT-CRC-007                          | Pass                       |
| SW-CMN-REQ-CRC-004      | 단위 테스트       | UT-CRC-002, UT-CRC-003              | Pass                       |
| SW-CMN-REQ-SIG-001      | 단위 테스트       | UT-SIG-001 ~ 003, 009, 012, 013    | Pass                       |
| SW-CMN-REQ-SIG-002      | 단위 테스트       | UT-SIG-004                          | Pass                       |
| SW-CMN-REQ-SIG-003      | 단위 테스트       | UT-SIG-005 ~ 007                    | Pass                       |
| SW-CMN-REQ-SIG-004      | 단위 테스트       | UT-SIG-010, 011                     | Pass                       |
| SW-CMN-REQ-SIG-005      | 단위 테스트       | UT-SIG-008                          | Pass                       |
| SW-CMN-REQ-CFG-001      | 단위/통합 테스트   | UT-CFG-001~002, IT-INT-001          | Pass                       |
| SW-CMN-REQ-CFG-002      | 단위/통합 테스트   | UT-CFG-006, IT-INT-002, IT-FI-002   | Pass                       |
| SW-CMN-REQ-CFG-003      | 단위 테스트       | UT-CFG-005                          | Pass                       |
| SW-CMN-REQ-CFG-004      | 단위/통합 테스트   | UT-CFG-007, IT-INT-004              | Pass                       |
| SW-CMN-REQ-GEN-001      | 통합 테스트       | IT-LIB-001~002, IT-MOD-005          | Pass                       |
| SW-CMN-REQ-GEN-002      | 코드 리뷰        | 코드 리뷰 완료 (reentrant 확인)       | Pass                       |
| SW-CMN-REQ-GEN-003      | 통합 테스트       | IT-LIB-003                          | Pass                       |

---

## 7. 정적 분석 결과 (Static Analysis Results)

| 도구                    | 대상 파일                           | 결과 요약                          |
|------------------------|------------------------------------|------------------------------------|
| clang-tidy, cppcheck   | `CrcProviderPublic.cpp`            | Critical: 0, High: 0, 경고: 0    |
| clang-tidy, cppcheck   | `CrcProviderPrivate.cpp`           | Critical: 0, High: 0, 경고: 0    |
| clang-tidy, cppcheck   | `CanSpecConfigLoader.cpp`          | Critical: 0, High: 0, 경고: 0    |
| clang-tidy, cppcheck   | `ConfigParser.cpp`                 | Critical: 0, High: 0, 경고: 0    |
| clang-tidy, cppcheck   | `SignalCodec.h`                    | Critical: 0, High: 0, 경고: 0    |

### MISRA C++ 준수 요약

| 항목                    | 결과                                |
|------------------------|-------------------------------------|
| 총 규칙 위반 건수         | 0건                                  |
| Mandatory 위반           | 0건                                  |
| Required 위반            | 0건                                  |
| Advisory 위반            | 0건                                  |
| 승인된 일탈 (Deviation)   | 0건                                  |

---

## 8. 결함 요약 (Defect Summary)

### 8.1 발견된 결함

| 결함 ID     | 심각도     | 컴포넌트            | 설명                            | 상태                        |
|------------|-----------|--------------------|---------------------------------|----------------------------|
| DEF-CMN-001 | High      | CrcProviderPublic | NULL payload 미검사 — 잠재적 정의되지 않은 동작 | 수정 완료 (line 17 방어적 검사 추가) |
| DEF-CMN-002 | Medium    | SignalCodec       | WriteU16Le/WriteU32Le 버퍼 경계 미검증         | 수정 완료 (WriteU16LeSafe/WriteU32LeSafe 추가) |

### 8.2 결함 통계

| 심각도      | 발견 건수 | 수정 완료 | 미수정 | 수용 (허용) |
|------------|----------|----------|--------|------------|
| Critical   | 0        | 0        | 0      | 0          |
| High       | 1        | 1        | 0      | 0          |
| Medium     | 1        | 1        | 0      | 0          |
| Low        | 0        | 0        | 0      | 0          |

---

## 9. 검증 판정 (Verification Verdict)

### 9.1 판정 기준

| 기준                                              | 목표                    | 결과                        |
|--------------------------------------------------|------------------------|-----------------------------|
| 모든 안전 요구사항에 대한 검증 증거 존재               | 100%                   | 100%                         |
| 단위 테스트 통과율                                   | 100%                   | 100% (63+ TC)                |
| 통합 테스트 통과율                                   | 100%                   | 100% (3 스위트)               |
| Statement coverage                                | >= 95%                 | 달성 (gcov/lcov)              |
| Branch coverage                                   | >= 90%                 | 달성 (gcov/lcov)              |
| 미수정 Critical/High 결함                           | 0건                    | 0건                          |
| MISRA Mandatory 위반                               | 0건                    | 0건                          |

### 9.2 종합 판정

| 항목              | 판정                                       |
|------------------|--------------------------------------------|
| **종합 판정**      | **PASS**                                    |
| **조건부 수용 사유** | 해당 없음                                    |
| **잔여 리스크**     | 발견된 결함 2건 모두 수정 완료. 잔여 리스크 없음.  |

---

## 10. 승인 (Approval)

| 역할         | 성명                        | 서명         | 일자                        |
|-------------|----------------------------|-------------|----------------------------|
| 작성자       | SuYeol Kim                  | (전자서명)    | 2026-04-06                 |
| 검토자       | (리뷰 대기)                  | —           | —                          |
| 승인자       | (승인 대기)                  | —           | —                          |
| 안전 관리자   | (배정 대기)                  | —           | —                          |

---

## 11. 변경 이력 (Change History)

| 버전 | 일자       | 작성자      | 변경 내용                                                    |
|------|-----------|------------|-------------------------------------------------------------|
| 0.1  | 2026-04-03 | SuYeol Kim | 초안 작성                                                    |
| 1.0  | 2026-04-06 | SuYeol Kim | 전체 검증 결과 기입, 정적 분석/결함/커버리지 반영, 종합 판정 PASS |
