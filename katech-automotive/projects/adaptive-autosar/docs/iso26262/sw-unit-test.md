# Software Unit Test Specification (소프트웨어 단위 테스트 명세서)

## Document Metadata

| 항목 (Field)           | 내용 (Value)                                                        |
|------------------------|----------------------------------------------------------------------|
| Document ID            | SW-ARA-UT-001                                                        |
| Version                | 1.0                                                                  |
| ISO 26262 Reference    | Part 6, Clause 9 — Software Unit Testing                             |
| ASIL Scope             | ASIL-B / ASIL-D (component-dependent)                                |
| Project                | Adaptive AUTOSAR Platform (NXP S32G / Axon)                         |
| Author                 | SuYeol Kim                                                           |
| Reviewer               | <!-- TODO: 검토자 이름 기입 -->                                      |
| Approval Date          | 2026-04-06                                                           |
| Status                 | Draft                                                                |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 9에 따라 Adaptive AUTOSAR 플랫폼의 소프트웨어 단위 테스트를 계획하고 명세한다. SW-ARA-UD-001에서 정의된 각 소프트웨어 단위가 SW-ARA-REQ-001의 안전 요구사항을 올바르게 구현하였는지 검증한다.

## 2. 적용 범위 (Scope)

본 단위 테스트 명세는 SW-ARA-UD-001에 정의된 모든 소프트웨어 단위를 대상으로 하며, 특히 안전 관련(safety-related) 단위를 우선 대상으로 한다.

## 3. 참조 문서 (Input References)

| Ref ID | Document Title                                    | Document ID      |
|--------|---------------------------------------------------|------------------|
| REF-01 | Software Safety Requirements Specification         | SW-ARA-REQ-001   |
| REF-02 | Software Architecture Description                  | SW-ARA-ARCH-001  |
| REF-03 | Software Unit Design Specification                 | SW-ARA-UD-001    |

## 4. 테스트 환경 (Test Environment)

### 4.1 테스트 인프라

| 항목                    | 내용                                                             |
|-------------------------|------------------------------------------------------------------|
| Host OS                 | Ubuntu 22.04 LTS                                                 |
| Target OS               | Yocto Linux (NXP S32G aarch64) / Axon                            |
| Compiler                | GCC 11.x (host), aarch64-linux-gnu-g++ (target)                  |
| Build System            | CMake 3.15+ (ARA_ENABLE_TESTS flag)                              |
| Unit Test Framework     | Google Test (gtest)                                               |
| Mocking Framework       | Google Mock (gmock)                                               |
| Code Coverage Tool      | gcov/lcov                                                         |
| CI/CD Integration       | 대기 중: 구현/측정 후 갱신                                        |
| Docker Environment      | Docker containerized build (cppcheck 포함)                        |

### 4.2 Host vs. Target 테스트 전략

| 테스트 유형             | 실행 환경          | 비고                                              |
|------------------------|--------------------|---------------------------------------------------|
| 순수 로직 단위 테스트   | Host (x86_64)      | HW 의존성 없는 단위                                |
| HW-dependent 단위 테스트 | Target (aarch64)  | NXP S32G / Axon 보드 필요                          |
| Mock 기반 단위 테스트   | Host (x86_64)      | 외부 의존성을 mock/stub으로 대체                    |

<!-- TODO: Host 환경에서 aarch64 target 코드 테스트 시 cross-compilation 또는 QEMU 에뮬레이션 전략 기술 -->

## 5. 테스트 방법론 (Test Methods)

### 5.1 ISO 26262-6 Table 9 적용 (단위 테스트 방법)

| 테스트 방법 (Method)                          | ASIL-B | ASIL-D | 적용 여부  | 비고                        |
|----------------------------------------------|--------|--------|------------|-----------------------------|
| Requirements-based testing                    | ++     | ++     | <!-- TODO --> | 각 SwRS에 대한 TC 작성    |
| Interface testing                             | ++     | ++     | <!-- TODO --> | 함수 인터페이스 검증       |
| Fault injection testing                       | +      | ++     | <!-- TODO --> | 오류 주입 시 동작 확인     |
| Resource usage testing                        | +      | ++     | <!-- TODO --> | 메모리, CPU 등 사용량 검증 |
| Back-to-back comparison testing               | +      | +      | <!-- TODO --> | 모델 vs 구현 비교         |

Legend: ++ Highly recommended, + Recommended

### 5.2 ISO 26262-6 Table 10 적용 (구조적 커버리지)

| 커버리지 메트릭 (Coverage Metric)       | ASIL-B | ASIL-D | 목표치          | 비고                  |
|----------------------------------------|--------|--------|-----------------|------------------------|
| Statement coverage                      | ++     | +      | >= 95%            | 최소 기준             |
| Branch coverage (Decision coverage)     | ++     | ++     | >= 90%            | 주요 기준             |
| MC/DC (Modified Condition/Decision)     | +      | ++     | 대기 중: 구현/측정 후 갱신 | ASIL-D 필수 기준      |

## 6. 테스트 케이스 명세 (Test Case Specification)

### 6.1 EXEC — Execution Management

#### SWU-EXEC-001: MachineStateManager

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)                  | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|----------------------------------------------|-----------------|-----------|
| TC-EXEC-001-01  | 정상적인 Machine State 전환 성공 확인                        | target=Running, timeout=5000ms        | State 전환 완료, Result 성공 반환              | SwRS-EXEC-002   | <!-- TODO --> |
| TC-EXEC-001-02  | State 전환 timeout 초과 시 오류 처리 확인                    | target=Running, timeout=1ms (강제 초과)| PHM에 오류 보고, 에러 Result 반환              | SwRS-EXEC-002   | <!-- TODO --> |
| TC-EXEC-001-03  | 잘못된 State 요청 시 거부 확인                              | target=InvalidState                   | 에러 코드 반환                                | SwRS-EXEC-002   | <!-- TODO --> |
| TC-EXEC-001-04  | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                                | <!-- TODO -->   | <!-- TODO --> |

#### SWU-EXEC-002: ProcessManager

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)                  | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|----------------------------------------------|-----------------|-----------|
| TC-EXEC-002-01  | 프로세스 비정상 종료 감지 확인                               | 자식 프로세스 SIGKILL 종료             | PHM에 비정상 종료 이벤트 보고                  | SwRS-EXEC-001   | <!-- TODO --> |
| TC-EXEC-002-02  | 프로세스 정상 종료 시 PHM 미보고 확인                        | 자식 프로세스 정상 exit(0)             | PHM 보고 없음                                 | SwRS-EXEC-001   | <!-- TODO --> |
| TC-EXEC-002-03  | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                                | <!-- TODO -->   | <!-- TODO --> |

### 6.2 PHM — Platform Health Management

#### SWU-PHM-001: AliveSupervision

| TC ID           | Test Description                                           | 입력 조건 (Input)                          | 기대 결과 (Expected Output)              | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------------|------------------------------------------|-----------------|-----------|
| TC-PHM-001-01   | 정상 alive indication 수신 시 감시 통과                      | 기대 횟수 내 alive indication 도착           | Supervision 상태 = OK                    | SwRS-PHM-001    | <!-- TODO --> |
| TC-PHM-001-02   | Alive indication 부족 시 위반 감지                          | 기대 횟수 미달                               | Supervision 위반 이벤트 발생              | SwRS-PHM-001    | <!-- TODO --> |
| TC-PHM-001-03   | Alive indication 과다 시 위반 감지                          | 기대 횟수 초과 (maxMargin 초과)              | Supervision 위반 이벤트 발생              | SwRS-PHM-001    | <!-- TODO --> |
| TC-PHM-001-04   | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                               | <!-- TODO -->                            | <!-- TODO -->   | <!-- TODO --> |

#### SWU-PHM-004: RecoveryActionHandler

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)              | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|------------------------------------------|-----------------|-----------|
| TC-PHM-004-01   | Recovery action (process restart) 정상 수행                  | Supervision 위반 이벤트 + restart 설정 | 해당 프로세스 재시작                      | SwRS-PHM-002    | <!-- TODO --> |
| TC-PHM-004-02   | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                            | <!-- TODO -->   | <!-- TODO --> |

### 6.3 COM — Communication Management

#### SWU-COM-002: E2EProtectionWrapper

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)              | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|------------------------------------------|-----------------|-----------|
| TC-COM-002-01   | E2E Protection 적용 후 정상 데이터 수신 확인                 | 유효한 E2E protected 메시지            | 데이터 정상 전달, E2E check OK            | SwRS-COM-001    | <!-- TODO --> |
| TC-COM-002-02   | Corrupted 데이터 수신 시 E2E 에러 감지                      | CRC 불일치 메시지                      | E2E 에러 플래그 반환                      | SwRS-COM-001    | <!-- TODO --> |
| TC-COM-002-03   | 통신 timeout 발생 시 오류 통지 확인                         | Timeout 경과, 메시지 미수신            | 애플리케이션에 timeout 오류 통지           | SwRS-COM-002    | <!-- TODO --> |
| TC-COM-002-04   | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                            | <!-- TODO -->   | <!-- TODO --> |

### 6.4 UCM — Update and Configuration Management

#### SWU-UCM-002: IntegrityVerifier

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)              | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|------------------------------------------|-----------------|-----------|
| TC-UCM-002-01   | 유효한 패키지 서명 검증 성공                                 | 정상 서명된 SW 패키지                  | 검증 통과                                | SwRS-UCM-002    | <!-- TODO --> |
| TC-UCM-002-02   | 위조된 패키지 서명 검증 실패                                 | 변조된 서명을 가진 패키지              | 검증 실패, 패키지 거부                    | SwRS-UCM-002    | <!-- TODO --> |
| TC-UCM-002-03   | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                            | <!-- TODO -->   | <!-- TODO --> |

#### SWU-UCM-003: RollbackController

| TC ID           | Test Description                                           | 입력 조건 (Input)                     | 기대 결과 (Expected Output)              | 관련 SwRS       | Pass/Fail |
|-----------------|------------------------------------------------------------|---------------------------------------|------------------------------------------|-----------------|-----------|
| TC-UCM-003-01   | 설치 실패 시 이전 버전 rollback 성공                         | 설치 중 오류 발생                      | 이전 SW 버전 복원, 시스템 정상 동작       | SwRS-UCM-001    | <!-- TODO --> |
| TC-UCM-003-02   | <!-- TODO: 추가 테스트 케이스 -->                           | <!-- TODO -->                         | <!-- TODO -->                            | <!-- TODO -->   | <!-- TODO --> |

### 6.5 IAM / ara-api

<!-- TODO: IAM 및 ara-api 단위에 대한 테스트 케이스를 위와 동일한 형식으로 작성 -->

## 7. Fault Injection 테스트 (Fault Injection Testing)

ISO 26262-6 Clause 9에 따라 안전 메커니즘의 효과를 검증하기 위한 오류 주입 테스트:

| FI Test ID     | 대상 Unit        | 주입 결함 (Injected Fault)           | 기대 동작 (Expected Behavior)              | 관련 SM   |
|----------------|------------------|--------------------------------------|---------------------------------------------|-----------|
| FI-001         | SWU-EXEC-002     | Process crash (SIGSEGV 주입)         | PHM이 감지하여 recovery action 수행          | SM-001    |
| FI-002         | SWU-COM-002      | Message CRC corruption               | E2E Protection이 corruption 감지             | SM-002    |
| FI-003         | SWU-UCM-002      | Package signature tampering          | IntegrityVerifier가 위조 감지, 패키지 거부   | SM-003    |
| FI-004         | <!-- TODO: 추가 fault injection --> | <!-- TODO -->         | <!-- TODO -->                               | <!-- TODO --> |

<!-- TODO: fault injection 도구 및 방법 (예: signal injection, bit-flip, mock 기반 주입) 기술 -->

## 8. 코드 커버리지 결과 (Coverage Results)

<!-- TODO: 테스트 실행 후 아래 표에 실제 커버리지 결과 기입 -->

| Component | Unit ID         | Statement (%) | Branch (%) | MC/DC (%) | 목표 달성 여부 |
|-----------|-----------------|---------------|------------|-----------|---------------|
| EXEC      | SWU-EXEC-001   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| EXEC      | SWU-EXEC-002   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| PHM       | SWU-PHM-001    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| PHM       | SWU-PHM-004    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| COM       | SWU-COM-002    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| UCM       | SWU-UCM-002    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| UCM       | SWU-UCM-003    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 9. 테스트 실행 결과 요약 (Test Execution Summary)

<!-- TODO: 테스트 실행 후 아래 표에 결과 요약 기입 -->

| 항목                      | 값              |
|---------------------------|-----------------|
| 총 테스트 케이스 수        | <!-- TODO -->   |
| Pass                      | <!-- TODO -->   |
| Fail                      | <!-- TODO -->   |
| Blocked / Not Run         | <!-- TODO -->   |
| Pass Rate                 | <!-- TODO --> % |
| 테스트 실행일              | <!-- TODO -->   |
| 테스트 환경 버전           | <!-- TODO -->   |

## 10. 미결 이슈 및 편차 (Open Issues and Deviations)

<!-- TODO: 커버리지 미달성 항목, 테스트 제외 사유 등 기록 -->

| Issue ID | Description                                    | 영향도 | 조치 계획                    | 상태   |
|----------|------------------------------------------------|--------|------------------------------|--------|
| <!-- TODO --> | <!-- TODO -->                             | <!-- TODO --> | <!-- TODO -->           | Open   |

## 11. References

| Document ID      | Title                                              |
|------------------|----------------------------------------------------|
| SW-ARA-REQ-001   | Software Safety Requirements Specification         |
| SW-ARA-ARCH-001  | Software Architecture Description                  |
| SW-ARA-UD-001    | Software Unit Design Specification                 |
| SW-ARA-IT-001    | Software Integration Test Specification            |
| SW-ARA-VR-001    | Software Verification Report                       |

---

## Revision History

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성   |
| 1.0     | 2026-04-06 | SuYeol Kim | 테스트 환경, 커버리지 목표, 프레임워크 갱신 |
