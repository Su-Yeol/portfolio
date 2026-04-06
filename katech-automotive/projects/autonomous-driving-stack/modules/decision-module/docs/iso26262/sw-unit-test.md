# Software Unit Test Specification

## ISO 26262 Part 6, Clause 9 — 소프트웨어 단위 테스트

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-UT-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 9 (Software unit testing) |
| **ASIL** | B~C (단위별 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | SuYeol Kim |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 1.0 |
| **Date** | 2026-04-06 |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 9에 따라 Decision Module의 소프트웨어 단위 테스트 계획 및 사양을 정의한다. 각 단위(Unit)가 설계 사양을 충족하는지 검증하기 위한 테스트 케이스, 방법, 환경을 기술한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-UD-001 | Software Unit Design Specification | 단위 설계 문서 |
| SW-DEC-REQ-001 | Software Safety Requirements Specification | 안전 요구사항 |
| SW-DEC-ARCH-001 | Software Architecture Description | 아키텍처 문서 |
| ISO 26262:2018 Part 6, Clause 9 | Software unit testing | 국제 표준 |

---

## 3. 테스트 환경 (Test Environment)

| 항목 | 내용 |
|------|------|
| **테스트 프레임워크** | Google Test 1.14.0 |
| **컴파일러** | GCC 11.x (host), aarch64-linux-gnu-g++ (target) |
| **Host 테스트 환경** | Ubuntu 22.04 LTS (x86_64) |
| **Target 테스트 환경** | NXP S32G (aarch64) |
| **코드 커버리지 도구** | gcov/lcov |
| **정적 분석 도구** | clang-tidy, cppcheck |
| **Mock/Stub 프레임워크** | Google Mock (Google Test 1.14.0 포함) |

---

## 4. 테스트 방법 (Test Methods)

ISO 26262 Part 6, Table 10에 따른 ASIL별 테스트 방법 적용:

| 방법 | ASIL A | ASIL B | ASIL C | ASIL D |
|------|--------|--------|--------|--------|
| 요구사항 기반 테스트 (Requirements-based) | ++ | ++ | ++ | ++ |
| 인터페이스 테스트 | + | ++ | ++ | ++ |
| 등가 분할 (Equivalence Classes) | + | + | ++ | ++ |
| 경계값 분석 (Boundary Values) | + | + | ++ | ++ |
| 오류 추측 (Error Guessing) | + | + | + | ++ |

**커버리지 목표 (ISO 26262 Part 6, Table 11):**

| 커버리지 기준 | ASIL A | ASIL B | ASIL C | ASIL D |
|-------------|--------|--------|--------|--------|
| Statement Coverage | ++ | ++ | ++ | ++ |
| Branch Coverage | + | ++ | ++ | ++ |
| MC/DC | + | + | ++ | ++ |

**본 모듈 커버리지 목표 (ASIL B~C):**
- Statement Coverage: >= 95%
- Branch Coverage: >= 90%
- MC/DC: 대기 중: 구현/측정 후 갱신

---

## 5. 단위 테스트 사양 (Unit Test Specifications)

### 5.1 K-City Variant Unit Tests

#### 5.1.1 MainModule 테스트 (SWU-DEC-K-001)

| Test ID | 테스트 항목 | 입력 | 기대 결과 | ASIL | 상태 |
|---------|-----------|------|----------|------|------|
| UT-K-001-01 | 정상 센서 데이터 기반 주행 판단 | TODO: 정상 GPS/LiDAR/경로 데이터 | TODO: 정상 주행 명령 생성 | TODO | TODO |
| UT-K-001-02 | 보행자 감지 시 긴급 판단 | TODO: 보행자 감지 이벤트 | TODO: xx ms 이내 감속/정지 명령 | TODO | TODO |
| UT-K-001-03 | GPS 신호 유실 시 Safe State 전환 | TODO: GPS 타임아웃 시뮬레이션 | Safe State 전환 | TODO | TODO |
| UT-K-001-04 | config.ini 로드 실패 시 기본값 적용 | TODO: 설정 파일 부재/오류 | 기본 안전값으로 동작 | TODO | TODO |
| UT-K-001-05 | 비정상 경로 데이터 처리 | TODO: 범위 초과 Waypoint | 이전 경로 유지 | TODO | TODO |

#### 5.1.2 Communicator 테스트 (SWU-DEC-K-002)

| Test ID | 테스트 항목 | 입력 | 기대 결과 | ASIL | 상태 |
|---------|-----------|------|----------|------|------|
| UT-K-002-01 | GPS 데이터 정상 파싱 | TODO: 유효 GPS 메시지 | 올바른 좌표/속도 파싱 | TODO | TODO |
| UT-K-002-02 | LiDAR(Ibeo) 장애물 데이터 파싱 | TODO: 유효 LiDAR 프레임 | 올바른 장애물 좌표 추출 | TODO | TODO |
| UT-K-002-03 | 통신 타임아웃 검출 | TODO: 지정 시간 내 데이터 미수신 | 타임아웃 이벤트 발생 | TODO | TODO |
| UT-K-002-04 | 데이터 범위 초과 검출 | TODO: 범위 밖 센서값 | 데이터 폐기 + 경고 | TODO | TODO |

#### 5.1.3 PathManager 테스트 (SWU-DEC-K-003)

| Test ID | 테스트 항목 | 입력 | 기대 결과 | ASIL | 상태 |
|---------|-----------|------|----------|------|------|
| UT-K-003-01 | Waypoint 추종 계산 | TODO: 경로 데이터 + 현재 위치 | 올바른 추종 목표점 | TODO | TODO |
| UT-K-003-02 | Cross-Track Error 계산 | TODO: 경로 이탈 시나리오 | 올바른 오차 값 | TODO | TODO |
| UT-K-003-03 | 경로 이탈 임계치 판단 | TODO: 임계치 초과 이탈 | 경로 이탈 경고 | TODO | TODO |
| UT-K-003-04 | 경계값: Waypoint 개수 최소/최대 | 0개, 1개, 최대 개수 | TODO: 적절한 처리 | TODO | TODO |

---

### 5.2 CAN Gateway Variant Unit Tests

#### 5.2.1 CANGateway 테스트 (SWU-DEC-G-001)

| Test ID | 테스트 항목 | 입력 | 기대 결과 | ASIL | 상태 |
|---------|-----------|------|----------|------|------|
| UT-G-001-01 | CAN 메시지 정상 송신 | TODO: 유효 제어 명령 | CAN 프레임 정상 송신 | TODO | TODO |
| UT-G-001-02 | CAN 메시지 정상 수신 및 파싱 | TODO: 유효 CAN 프레임 | 올바른 데이터 파싱 | TODO | TODO |
| UT-G-001-03 | CRC 검증 실패 메시지 처리 | TODO: CRC 오류 CAN 프레임 | 메시지 폐기 | TODO | TODO |
| UT-G-001-04 | CAN 버스 오류 처리 | TODO: Bus-off 시뮬레이션 | 오류 보고 + 재시도 | TODO | TODO |

#### 5.2.2 CRC 테스트 (SWU-DEC-G-002)

| Test ID | 테스트 항목 | 입력 | 기대 결과 | ASIL | 상태 |
|---------|-----------|------|----------|------|------|
| UT-G-002-01 | CRC 계산 정확성 | TODO: 알려진 테스트 벡터 | 올바른 CRC 값 | TODO | TODO |
| UT-G-002-02 | CRC 검증 (정상 메시지) | TODO: 유효 메시지 + CRC | 검증 통과 | TODO | TODO |
| UT-G-002-03 | CRC 검증 (변조 메시지) | TODO: 변조된 메시지 | 검증 실패 검출 | TODO | TODO |
| UT-G-002-04 | 경계값: 빈 메시지 CRC | 길이 0 바이트 배열 | TODO: 정의된 동작 | TODO | TODO |
| UT-G-002-05 | 경계값: 최대 길이 메시지 CRC | TODO: 최대 CAN 페이로드 | 올바른 CRC 값 | TODO | TODO |

---

## 6. 테스트 결과 요약 (Test Result Summary)

| Variant | 단위 | 전체 TC | Pass | Fail | N/A | Statement Cov. | Branch Cov. | MC/DC |
|---------|------|---------|------|------|-----|---------------|-------------|-------|
| K-City | MainModule | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| K-City | Communicator | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| K-City | PathManager | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Gateway | CANGateway | TODO | TODO | TODO | TODO | TODO | TODO | TODO |
| Gateway | CRC | TODO | TODO | TODO | TODO | TODO | TODO | TODO |

---

## 7. 테스트-설계 추적성 (Test-to-Design Traceability)

| Test ID 범위 | Unit ID | SW Safety Req |
|-------------|---------|--------------|
| UT-K-001-xx | SWU-DEC-K-001 | SWR-DEC-001, 002, 003, 004 |
| UT-K-002-xx | SWU-DEC-K-002 | SWR-DEC-001, 003, 004 |
| UT-K-003-xx | SWU-DEC-K-003 | SWR-DEC-001 |
| UT-G-001-xx | SWU-DEC-G-001 | SWR-DEC-005 |
| UT-G-002-xx | SWU-DEC-G-002 | SWR-DEC-005 |

---

## 8. 미해결 사항 (Open Items)

- [x] 테스트 프레임워크 선정: Google Test 1.14.0 + Google Mock (CMake FetchContent 사용)
- [x] 커버리지 목표치 확정: Statement >= 95%, Branch >= 90%
- [ ] TODO: Mock/Stub 전략 확정 (센서 입력, CAN 인터페이스) — 대기 중: 구현/측정 후 갱신
- [ ] TODO: CI 파이프라인에 단위 테스트 통합 — 대기 중: 구현/측정 후 갱신
- [ ] TODO: Host(x86_64) 테스트와 Target(aarch64) 테스트 전략 구분 — 대기 중: 구현/측정 후 갱신

---

## 9. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0 | 2026-04-06 | SuYeol Kim | 테스트 환경, 커버리지 목표, 프레임워크 정보 갱신 |
