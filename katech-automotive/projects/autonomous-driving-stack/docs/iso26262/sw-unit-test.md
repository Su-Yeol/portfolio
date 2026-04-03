# Software Unit Test Specification

## 소프트웨어 단위 테스트 명세서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-UT-001                                         |
| **Version**           | 0.1 Draft                                             |
| **ISO 26262 Reference** | Part 6, Clause 9 — Software unit testing            |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | <!-- TODO: 작성자 이름 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | <!-- TODO: 승인일자 (YYYY-MM-DD) -->                   |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 9에 따라 Autonomous Driving Stack의
소프트웨어 단위 테스트 계획, 방법, 기준을 프로젝트 수준에서 정의한다.
각 모듈의 소프트웨어 단위(SW-ADS-UD-001에서 정의)에 대한 테스트 전략 및
테스트 케이스 구조를 명세한다.

## 2. 적용 범위 (Scope)

- 대상 모듈: aeb-control, control-module, decision-module (K-City / Gateway), remote-control, common
- 테스트 환경: 호스트(x86_64) 단위 테스트 + 타겟(NXP S32G / aarch64) 검증
- 빌드 모드: Module-local builds 기반 단위 테스트 수행
- `USE_PRIVATE_IMPL` 대상 단위: Private 및 Public 구현 모두 개별 테스트 필요

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                        | 비고                    |
|-----------------|-----------------------------------------------|-------------------------|
| SW-ADS-REQ-001  | Software Safety Requirements Specification    | 안전 요구사항            |
| SW-ADS-ARCH-001 | Software Architecture Description             | 아키텍처 설계서          |
| SW-ADS-UD-001   | Software Unit Design Specification            | 단위 설계 명세서         |
| SW-ADS-IT-001   | Software Integration Test Specification       | 통합 테스트 명세서       |
| <!-- TODO -->   | Test Framework Configuration                  | 테스트 프레임워크 설정   |

## 4. 단위 테스트 방법론 (Unit Test Methods)

### 4.1 ISO 26262 Part 6, Table 9 적용

| 테스트 방법                           | ASIL B | ASIL C | ASIL D | 적용 대상                    |
|---------------------------------------|--------|--------|--------|------------------------------|
| 요구사항 기반 테스트                   | ++     | ++     | ++     | 전 모듈                      |
| 인터페이스 테스트                      | ++     | ++     | ++     | 전 모듈                      |
| 결함 주입 테스트 (Fault Injection)     | +      | +      | ++     | aeb-control, common          |
| 리소스 사용 테스트                     | +      | +      | ++     | aeb-control, control-module  |
| Back-to-back 테스트                    | +      | +      | ++     | <!-- TODO: 해당 단위 식별 --> |

(`++` = highly recommended, `+` = recommended)

### 4.2 구조적 커버리지 기준 (Structural Coverage)

ISO 26262 Part 6, Table 10에 따른 커버리지 기준:

| 커버리지 메트릭             | ASIL B | ASIL C | ASIL D | 도구               |
|-----------------------------|--------|--------|--------|--------------------|
| 구문 커버리지 (Statement)    | ++     | ++     | ++     | <!-- TODO: gcov/llvm-cov 등 --> |
| 분기 커버리지 (Branch)       | +      | ++     | ++     | <!-- TODO -->      |
| MC/DC 커버리지              | +      | +      | ++     | <!-- TODO -->      |

**프로젝트 목표 커버리지:**

| 모듈             | ASIL | Statement | Branch | MC/DC    |
|------------------|------|-----------|--------|----------|
| aeb-control      | D    | 100%      | 100%   | ≥ 95%    |
| control-module   | C    | 100%      | ≥ 95%  | -        |
| decision-module  | B~C  | ≥ 95%    | ≥ 90%  | -        |
| remote-control   | B    | ≥ 95%    | ≥ 90%  | -        |
| common           | D    | 100%      | 100%   | ≥ 95%    |

<!-- TODO: 커버리지 미달 시 정당화(justification) 절차 기술 -->

## 5. 테스트 환경 (Test Environment)

### 5.1 호스트 테스트 환경

| 항목              | 내용                                              |
|-------------------|---------------------------------------------------|
| 호스트 OS         | <!-- TODO: Ubuntu/CentOS 버전 -->                 |
| 컴파일러          | <!-- TODO: GCC/Clang 버전 -->                     |
| 테스트 프레임워크 | <!-- TODO: Google Test, CUnit, Unity 등 -->       |
| 목(Mock) 프레임워크 | <!-- TODO: Google Mock, CMock 등 -->            |
| 커버리지 도구     | <!-- TODO: gcov, lcov, llvm-cov 등 -->            |
| 빌드 시스템       | <!-- TODO: CMake/Make 구성 -->                    |
| CI/CD             | <!-- TODO: Jenkins/GitLab CI 등 -->               |

### 5.2 타겟 테스트 환경

| 항목              | 내용                                              |
|-------------------|---------------------------------------------------|
| 타겟 HW           | NXP S32G (aarch64)                                |
| 타겟 OS           | <!-- TODO: Linux 커널 버전 -->                    |
| 크로스 컴파일러   | <!-- TODO: aarch64 cross-toolchain 버전 -->       |
| 전송 방식         | <!-- TODO: SSH/SCP/JTAG 등 -->                    |
| CAN 인터페이스    | <!-- TODO: 가상 CAN / 실물 CAN -->                |

<!-- TODO: 호스트-타겟 간 테스트 결과 차이 관리 방안 기술 -->

## 6. 테스트 케이스 설계 (Test Case Design)

### 6.1 테스트 케이스 작성 기준

각 테스트 케이스는 다음 정보를 포함해야 한다:

| 필드                | 설명                                    |
|---------------------|-----------------------------------------|
| Test Case ID        | 고유 식별자 (TC-[모듈]-[번호])          |
| Related Unit ID     | 대상 소프트웨어 단위 ID                  |
| Related SWSR ID     | 검증 대상 안전 요구사항 ID               |
| Test Description     | 테스트 목적 및 시나리오 설명            |
| Preconditions       | 사전 조건                               |
| Test Steps          | 테스트 수행 단계                         |
| Expected Results    | 기대 결과                               |
| Pass/Fail Criteria  | 합격/불합격 판정 기준                    |
| ASIL                | 해당 단위의 ASIL 등급                    |

### 6.2 모듈별 테스트 케이스 개요

#### 6.2.1 aeb-control 단위 테스트

| Test Case ID  | Unit ID     | 테스트 설명                                        | SWSR ID       |
|---------------|-------------|----------------------------------------------------|---------------|
| TC-AEB-001    | SU-AEB-001  | 정상 충돌 감지 시 제동 명령 생성 확인               | SWSR-AEB-001  |
| TC-AEB-002    | SU-AEB-001  | 충돌 위험 없을 때 미작동 확인                       | SWSR-AEB-001  |
| TC-AEB-003    | SU-AEB-002  | 제동 강도 계산 범위 유효성                          | SWSR-AEB-002  |
| TC-AEB-004    | SU-AEB-003  | 내부 고장 시 안전 상태 전환                         | SWSR-AEB-002  |
| TC-AEB-005    | SU-AEB-001  | CAN 타임아웃 시 비상 제동 수행                      | SWSR-AEB-003  |
| <!-- TODO: 추가 AEB 테스트 케이스 --> | | | |

#### 6.2.2 control-module 단위 테스트

| Test Case ID  | Unit ID     | 테스트 설명                                        | SWSR ID       |
|---------------|-------------|----------------------------------------------------|---------------|
| TC-CTL-001    | SU-CTL-001  | 조향 명령 계산 정확성 검증                          | SWSR-CTL-001  |
| TC-CTL-002    | SU-CTL-002  | 가감속 명령 범위 초과 시 클램핑 확인                | SWSR-CTL-002  |
| TC-CTL-003    | SU-CTL-003  | CAN 메시지 송신 주기 준수 확인                      | SWSR-CTL-001  |
| <!-- TODO: 추가 Control 테스트 케이스 --> | | | |

#### 6.2.3 decision-module 단위 테스트

| Test Case ID  | Unit ID     | 테스트 설명                                        | SWSR ID       |
|---------------|-------------|----------------------------------------------------|---------------|
| TC-DEC-001    | SU-DEC-001  | 경로 계획 출력 유효성 검증                          | SWSR-DEC-001  |
| TC-DEC-002    | SU-DEC-003  | CAN GW 수신 데이터 유효성 검증 (정상/비정상 입력)   | SWSR-DEC-003  |
| TC-DEC-003    | SU-DEC-002  | K-City/Gateway 변형 간 인터페이스 호환성            | SWSR-DEC-002  |
| <!-- TODO: 추가 Decision 테스트 케이스 --> | | | |

#### 6.2.4 remote-control 단위 테스트

| Test Case ID  | Unit ID     | 테스트 설명                                        | SWSR ID       |
|---------------|-------------|----------------------------------------------------|---------------|
| TC-RMT-001    | SU-RMT-001  | 원격 명령 수신 및 파싱 정확성                       | SWSR-RMT-002  |
| TC-RMT-002    | SU-RMT-002  | 통신 단절 감지 및 안전 전환                         | SWSR-RMT-001  |
| TC-RMT-003    | SU-RMT-003  | Failsafe 타이머 만료 시 안전 정지                   | SWSR-RMT-001  |
| <!-- TODO: 추가 Remote Control 테스트 케이스 --> | | | |

#### 6.2.5 common 모듈 단위 테스트

| Test Case ID  | Unit ID     | 테스트 설명                                        | SWSR ID       |
|---------------|-------------|----------------------------------------------------|---------------|
| TC-CMN-001    | SU-CMN-001  | CAN 메시지 인코딩/디코딩 정확성                     | SWSR-CMN-002  |
| TC-CMN-002    | SU-CMN-001  | 잘못된 DLC/ID에 대한 방어적 처리                    | SWSR-CMN-002  |
| TC-CMN-003    | SU-CMN-003  | 타이머 정밀도 및 타임아웃 동작                      | SWSR-CMN-001  |
| <!-- TODO: 추가 Common 테스트 케이스 --> | | | |

### 6.3 USE_PRIVATE_IMPL 관련 테스트

| Test Case ID  | 테스트 설명                                                        | SWSR ID       |
|---------------|--------------------------------------------------------------------|---------------|
| TC-PVT-001    | `USE_PRIVATE_IMPL=ON` 빌드에서 안전 필수 구현 정상 동작 확인      | SWSR-PVT-001  |
| TC-PVT-002    | `USE_PRIVATE_IMPL=OFF` 빌드에서 Public stub 정상 동작 확인        | SWSR-PVT-001  |
| TC-PVT-003    | Private/Public 전환 시 인터페이스 호환성 (동일 테스트 입력에 대한 결과 비교) | SWSR-PVT-002 |
| <!-- TODO: 추가 Private Impl 테스트 케이스 --> | | |

## 7. 테스트 실행 절차 (Test Execution Procedure)

### 7.1 호스트 단위 테스트 실행

```bash
# 예시: Module-local 빌드 및 테스트 실행
# <!-- TODO: 실제 빌드/테스트 명령어로 교체 -->
cd modules/<module-name>
mkdir build && cd build
cmake -DUSE_PRIVATE_IMPL=ON -DENABLE_TESTING=ON ..
make
ctest --output-on-failure
```

### 7.2 커버리지 리포트 생성

```bash
# <!-- TODO: 실제 커버리지 수집 명령어로 교체 -->
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_report
```

<!-- TODO: CI/CD 파이프라인에서의 자동화된 테스트 실행 절차 기술 -->

## 8. 합격 기준 (Pass Criteria)

| 기준 항목                        | 기준값                                | 비고              |
|----------------------------------|---------------------------------------|-------------------|
| 전체 테스트 케이스 통과율         | 100%                                  | 0 failure 필수    |
| 구조적 커버리지 달성              | 모듈별 목표 충족 (Section 4.2 참조)   | 미달 시 정당화 필요 |
| MISRA 위반 0건 (필수 규칙)       | 0 violations                          | ASIL C/D 모듈     |
| 미결 이슈                         | Critical/Major 0건                    | 테스트 완료 전 해결 |

## 9. 테스트 결과 기록 양식 (Test Result Template)

| 필드                  | 내용                                        |
|-----------------------|---------------------------------------------|
| 테스트 실행일          | <!-- TODO: YYYY-MM-DD -->                  |
| 실행 환경              | 호스트 / 타겟 (버전 정보)                   |
| 빌드 구성              | USE_PRIVATE_IMPL=ON/OFF                    |
| 총 테스트 수           | <!-- TODO -->                              |
| Pass / Fail / Skip    | <!-- TODO --> / <!-- TODO --> / <!-- TODO --> |
| 커버리지 결과          | Statement: __% / Branch: __% / MC/DC: __% |
| 실행자                 | <!-- TODO -->                              |

<!-- TODO: 테스트 결과 보관 위치 및 아카이빙 정책 기술 -->

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | <!-- TODO --> | <!-- TODO --> | Initial draft 작성    |
