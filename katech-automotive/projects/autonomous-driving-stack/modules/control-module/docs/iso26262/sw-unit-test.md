# Software Unit Test Specification

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-UT-001                                             |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 9 — Software Unit Testing                |
| **ASIL Scope**        | ASIL C                                                    |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Language**          | C++17                                                     |
| **Author**            | SuYeol Kim                                                |
| **Reviewer**          | <!-- 대기 중: 검토자 지정 후 갱신 -->                       |
| **Approval Date**     | 2026-04-06                                                |
| **Status**            | Released                                                  |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 9에 따라 Control Module의 소프트웨어 유닛 테스트 사양을 정의한다. 각 소프트웨어 유닛이 상세 설계(SW-CTL-UD-001)에서 정의한 동작을 올바르게 구현하였는지 검증하기 위한 테스트 케이스, 방법, 환경 및 합격 기준을 명세한다.

## 2. 범위 (Scope)

- LateralController 유닛 테스트
- LongitudinalController 유닛 테스트
- SafetyMonitor 유닛 테스트
- InputManager 유닛 테스트
- OutputManager 유닛 테스트

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-ARCH-001   | Software Architecture Description             | 아키텍처 설계 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-IT-001     | Software Integration Test Specification       | 통합 테스트 문서        |

## 4. 테스트 방법론 (Test Methodology)

### 4.1 ISO 26262 Part 6, Clause 9 Table 10 — ASIL C 적용 방법

| 테스트 방법                              | 적용 수준       | 비고                              |
|------------------------------------------|-----------------|-----------------------------------|
| 요구사항 기반 테스트 (Requirements-based) | 강력 권고 (HR)  | 모든 안전 요구사항 커버           |
| 인터페이스 테스트 (Interface testing)     | 강력 권고 (HR)  | 유닛 간 인터페이스 검증           |
| 경계값 분석 (Boundary value analysis)     | 강력 권고 (HR)  | 입출력 경계 조건 테스트           |
| 동등 분할 (Equivalence partitioning)      | 강력 권고 (HR)  | 입력 도메인 분할 테스트           |
| 에러 추측 (Error guessing)               | 권고 (R)        | 경험 기반 결함 시나리오           |

### 4.2 구조적 커버리지 목표 (Structural Coverage)

ISO 26262 Part 6, Clause 9 Table 11 — ASIL C:

| 커버리지 메트릭          | 목표     | 비고                              |
|--------------------------|----------|-----------------------------------|
| 구문 커버리지 (Statement) | 100%    | 기본 요구                         |
| 분기 커버리지 (Branch)    | ≥95%    | ASIL C 강력 권고                  |
| MC/DC 커버리지            | 권고     | ASIL D 강력 권고, ASIL C 권고     |

### 4.3 테스트 환경

| 항목              | 내용                                                    |
|-------------------|---------------------------------------------------------|
| 테스트 프레임워크 | Google Test 1.14.0                                      |
| 빌드 시스템       | CMake 3.x with FetchContent                             |
| 빌드 환경         | `./build.sh` (aarch64 cross-compile or host), Ubuntu 22.04 LTS |
| 컴파일러          | GCC 11.x (host), aarch64-linux-gnu-g++ (target)        |
| C++ 표준 / 플래그 | C++17, `-Wall -Wextra -Wpedantic -Wconversion`         |
| 모킹 프레임워크   | Google Mock (Google Test 1.14.0 내장)                   |
| 커버리지 도구     | gcov/lcov (branch coverage 포함)                        |
| 정적 분석         | clang-tidy (프로젝트 루트 .clang-tidy), cppcheck       |
| CI 연동           | <!-- 대기 중: CI/CD 파이프라인 구성 후 갱신 -->         |

## 5. 유닛 테스트 케이스 (Unit Test Cases)

### 5.1 LateralController 유닛 테스트

#### 5.1.1 정상 동작 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-LAT-001     | 직진 경로에서 조향 명령 0 출력 확인                  | 직선 경로, 편차 0                       | steering_cmd ≈ 0                       | SW-CTL-SR-001   | Draft |
| UT-LAT-002     | 좌측 편향 시 우측 보정 조향 출력 확인                | 좌측 횡방향 편차 +0.5m                  | steering_cmd < 0 (우측 보정)           | SW-CTL-SR-001   | Draft |
| UT-LAT-003     | 우측 편향 시 좌측 보정 조향 출력 확인                | 우측 횡방향 편차 -0.5m                  | steering_cmd > 0 (좌측 보정)           | SW-CTL-SR-001   | Draft |
| UT-LAT-004     | 곡선 경로 추종 시 적절한 조향 출력 확인              | 곡률 r=<!-- TODO --> 경로               | 경로 추종 오차 허용 범위 이내          | SW-CTL-SR-001   | Draft |
| UT-LAT-005     | <!-- TODO: 추가 테스트 케이스 -->                    | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

#### 5.1.2 경계값 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-LAT-010     | 최대 조향각 경계에서의 출력 제한 확인                | 극대 횡방향 편차 (조향 한계 초과 유도)  | steering_cmd == MAX_STEER              | SW-CTL-SR-002   | Draft |
| UT-LAT-011     | 최소 조향각 경계에서의 출력 제한 확인                | 극대 반대 편차                          | steering_cmd == -MAX_STEER             | SW-CTL-SR-002   | Draft |
| UT-LAT-012     | 조향 변화율 제한 경계 테스트                         | 급격한 경로 변화                        | |d(steering)/dt| <= MAX_RATE           | SW-CTL-SR-003   | Draft |
| UT-LAT-013     | <!-- TODO: 추가 경계값 테스트 -->                    | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

#### 5.1.3 이상 조건 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-LAT-020     | MPC 솔버 실패 시 PID fallback 전환 확인              | MPC 수렴 불가 입력                      | PID 모드 전환, 유효한 조향 출력        | SW-CTL-SR-001   | Draft |
| UT-LAT-021     | NaN/Inf 입력 시 거부 확인                            | path_input에 NaN 포함                   | compute() return false                 | SW-CTL-SR-001   | Draft |
| UT-LAT-022     | reset() 후 내부 상태 초기화 확인                     | 이전 상태 존재 후 reset()               | 적분기 0, 이전 출력 0                  | SW-CTL-SR-001   | Draft |
| UT-LAT-023     | <!-- TODO: 추가 이상 조건 테스트 -->                 | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.2 LongitudinalController 유닛 테스트

#### 5.2.1 정상 동작 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-LON-001     | 목표 속도 도달 시 가속 명령 ≈ 0 확인                 | v_target == v_current                   | accel_cmd ≈ 0                          | SW-CTL-SR-010   | Draft |
| UT-LON-002     | 목표 속도 미달 시 양의 가속 명령 출력 확인           | v_target > v_current                    | accel_cmd > 0                          | SW-CTL-SR-010   | Draft |
| UT-LON-003     | 목표 속도 초과 시 제동 명령 출력 확인                | v_target < v_current                    | accel_cmd < 0 (brake)                  | SW-CTL-SR-010   | Draft |
| UT-LON-004     | <!-- TODO: 추가 테스트 케이스 -->                    | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

#### 5.2.2 경계값 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-LON-010     | 최대 가속도 경계 테스트                              | 매우 큰 속도 차이 (가속 방향)           | accel_cmd == MAX_ACCEL                 | SW-CTL-SR-011   | Draft |
| UT-LON-011     | 최대 감속도 경계 테스트                              | 매우 큰 속도 차이 (감속 방향)           | accel_cmd == -MAX_DECEL                | SW-CTL-SR-012   | Draft |
| UT-LON-012     | 비상 제동 응답 시간 테스트                           | 비상 제동 요청 신호                     | MAX_BRAKE within FTTI                  | SW-CTL-SR-013   | Draft |
| UT-LON-013     | <!-- TODO: 추가 경계값 테스트 -->                    | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.3 SafetyMonitor 유닛 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-SAF-001     | 출력 범위 제한 동작 확인                             | MAX_STEER + 10%                         | 출력이 MAX_STEER로 클램핑             | SW-CTL-SR-002   | Draft |
| UT-SAF-002     | 변화율 제한 동작 확인                                | 이전 명령 대비 급격한 변화              | 변화율이 MAX_RATE로 제한              | SW-CTL-SR-003   | Draft |
| UT-SAF-003     | 입력 타임아웃 감지 확인                              | 입력 없이 timeout 초과                  | timeout 플래그 설정                    | SW-CTL-SR-023   | Draft |
| UT-SAF-004     | 안전 상태 전이 동작 확인                             | 타임아웃 또는 CAN 오류 발생             | 안전 상태(출력 0) 전이                 | SW-CTL-SR-004   | Draft |
| UT-SAF-005     | Plausibility check 위반 감지 확인                    | 명령 vs 피드백 편차 초과                | 진단 이벤트 발생                       | SW-CTL-SR-020   | Draft |
| UT-SAF-006     | <!-- TODO: 추가 안전 모니터 테스트 -->               | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.4 OutputManager 유닛 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-OUT-001     | CAN 프레임 alive counter 순환 확인                   | 연속 N회 전송                           | counter 0→1→...→14→0 순환             | SW-CTL-SR-030   | Draft |
| UT-OUT-002     | CAN 프레임 CRC 정확성 확인                           | 알려진 payload                          | 기대 CRC 값과 일치                     | SW-CTL-SR-030   | Draft |
| UT-OUT-003     | CAN 전송 실패 처리 확인                              | CAN 드라이버 전송 실패 mock             | 재전송 시도 후 에러 보고               | SW-CTL-SR-031   | Draft |
| UT-OUT-004     | <!-- TODO: 추가 출력 관리자 테스트 -->               | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

### 5.5 InputManager 유닛 테스트

| TC ID          | 테스트 설명                                          | 입력                                    | 기대 출력                              | 요구사항 추적   | 상태  |
|----------------|------------------------------------------------------|-----------------------------------------|----------------------------------------|-----------------|-------|
| UT-INP-001     | 유효한 경로 데이터 수신 확인                         | 정상 범위 경로 데이터                   | 유효성 검사 통과                       | SW-CTL-SR-023   | Draft |
| UT-INP-002     | 범위 초과 경로 데이터 거부 확인                      | 좌표값 범위 초과                        | 데이터 무효화 처리                     | SW-CTL-SR-023   | Draft |
| UT-INP-003     | 입력 타임아웃 감지 확인                              | 지정 시간 내 수신 없음                  | timeout 이벤트 발생                    | SW-CTL-SR-023   | Draft |
| UT-INP-004     | <!-- TODO: 추가 입력 관리자 테스트 -->               | <!-- TODO -->                           | <!-- TODO -->                          | <!-- TODO -->   | Draft |

## 6. 테스트 추적 매트릭스 (Test Traceability Matrix)

<!-- 대기 중: 테스트 실행 후 전체 추적 매트릭스 완성 -->

| 요구사항 ID     | 유닛 테스트 ID                       | 커버리지 상태   |
|-----------------|--------------------------------------|-----------------|
| SW-CTL-SR-001   | UT-LAT-001 ~ UT-LAT-023             | <!-- TODO -->   |
| SW-CTL-SR-002   | UT-LAT-010, UT-LAT-011, UT-SAF-001  | <!-- TODO -->   |
| SW-CTL-SR-003   | UT-LAT-012, UT-SAF-002              | <!-- TODO -->   |
| SW-CTL-SR-010   | UT-LON-001 ~ UT-LON-004             | <!-- TODO -->   |
| SW-CTL-SR-011   | UT-LON-010                           | <!-- TODO -->   |
| SW-CTL-SR-012   | UT-LON-011                           | <!-- TODO -->   |
| SW-CTL-SR-013   | UT-LON-012                           | <!-- TODO -->   |
| SW-CTL-SR-023   | UT-SAF-003, UT-INP-001 ~ UT-INP-003 | <!-- TODO -->   |
| SW-CTL-SR-030   | UT-OUT-001, UT-OUT-002               | <!-- TODO -->   |
| SW-CTL-SR-031   | UT-OUT-003                           | <!-- TODO -->   |

## 7. 테스트 결과 요약 (Test Result Summary)

<!-- 대기 중: 테스트 실행 후 결과 기입 -->

| 항목                        | 결과                |
|-----------------------------|---------------------|
| 총 테스트 케이스 수         | <!-- TODO -->       |
| 통과 (Pass)                 | <!-- TODO -->       |
| 실패 (Fail)                 | <!-- TODO -->       |
| 미실행 (Not Executed)       | <!-- TODO -->       |
| 구문 커버리지 (Statement)   | <!-- TODO --> %     |
| 분기 커버리지 (Branch)      | <!-- TODO --> %     |
| MC/DC 커버리지              | <!-- TODO --> %     |

## 8. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | Fill concrete values: ASIL C, test environment, coverage targets |
