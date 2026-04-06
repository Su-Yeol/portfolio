# Software Unit Test Specification

## Remote Control Module — UDP/CAN Remote Communication

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-RMT-UT-001                                             |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 9 — Software Unit Testing                |
| **ASIL Scope**        | ASIL B                                                            |
| **Project**           | Autonomous Driving Stack                                  |
| **Module**            | Remote Control (remotePC / remoteS32G)                    |
| **Target HW**         | NXP S32G (aarch64) — receiver / x86 PC — sender          |
| **Author**            | SuYeol Kim                                                  |
| **Reviewer**          | <!-- TODO: 검토자 이름 기입 -->                             |
| **Approval Date**     | 2026-04-06                                                  |
| **Status**            | Draft                                                     |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 9에 따라 Remote Control 모듈의 소프트웨어 유닛 테스트 사양을 정의한다. 각 소프트웨어 유닛(UDPClass, CANClass, remotePC main, remoteS32G main)에 대한 테스트 케이스, 테스트 방법 및 합격 기준을 명세한다.

## 2. 범위 (Scope)

- UDPClass 유닛 테스트 (소켓 생성, 송수신, 에러 처리)
- CANClass 유닛 테스트 (소켓 생성, 프레임 전송, 에러 처리)
- remotePC main 유닛 테스트 (송신 루프 동작)
- remoteS32G main 유닛 테스트 (수신 및 포워딩 동작)

## 3. 참조 문서 (Reference Documents)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-RMT-REQ-001  | Software Safety Requirements Specification | 안전 요구사항 문서    |
| SW-RMT-UD-001   | Software Unit Design Specification         | 유닛 설계 문서        |
| SW-RMT-ARCH-001 | Software Architecture Description          | 아키텍처 설계 문서    |

## 4. 테스트 환경 (Test Environment)

| 항목               | 내용                                                    |
|--------------------|---------------------------------------------------------|
| 테스트 프레임워크   | Google Test 1.14.0 + Google Mock                         |
| 컴파일러 (x86)     | GCC 11.x (x86_64)                                       |
| 컴파일러 (aarch64) | aarch64-linux-gnu-g++ (target)                           |
| 목표 플랫폼        | x86 PC (sender 테스트), NXP S32G (receiver 테스트)       |
| 네트워크 시뮬레이션 | UDP loopback (대기 중: 구현/측정 후 갱신)                |
| CAN 시뮬레이션     | vCAN (가상 CAN) (대기 중: 구현/측정 후 갱신)             |
| 코드 커버리지 도구  | gcov/lcov                                                |

## 5. 테스트 방법론 (Test Methodology)

ISO 26262 Part 6, Clause 9, Table 10에 따른 ASIL B 테스트 방법:

| 테스트 방법                                          | ASIL B 권장 수준 | 적용 계획                        |
|------------------------------------------------------|-------------------|----------------------------------|
| 요구사항 기반 테스트 (Requirements-based testing)      | 강력 권장 (++)    | 적용 예정                        |
| 인터페이스 테스트 (Interface testing)                  | 강력 권장 (++)    | 적용 예정                        |
| 경계값 분석 (Boundary value analysis)                  | 권장 (+)          | 적용 예정                        |
| 등가 분할 (Equivalence partitioning)                   | 강력 권장 (++)    | 적용 예정                        |
| 에러 추정 (Error guessing)                             | 권장 (+)          | 적용 예정                        |
| 구문 커버리지 (Statement coverage)                     | 강력 권장 (++)    | 목표: >= 95%                     |
| 분기 커버리지 (Branch coverage)                        | 강력 권장 (++)    | 목표: >= 90%                     |

## 6. 유닛 테스트 케이스 (Unit Test Cases)

### 6.1 UDPClass 유닛 테스트

| TC ID          | 테스트 목적                                           | 입력 / 전제 조건                    | 기대 결과                          | 요구사항 참조    | 상태   |
|----------------|-------------------------------------------------------|-------------------------------------|------------------------------------|------------------|--------|
| UT-RMT-001     | SetSocket 정상 소켓 생성 확인                          | 유효한 IP, Port                     | 소켓 fd > 0                        | SW-RMT-SR-001   | Draft  |
| UT-RMT-002     | SetSocket 잘못된 IP 전달 시 에러 처리                   | 잘못된 IP 문자열                    | 예외 또는 에러 코드 반환            | SW-RMT-SR-002   | Draft  |
| UT-RMT-003     | Send 정상 전송 확인 (4바이트)                           | SetSocket 완료, Buffer 설정         | 4바이트 전송 성공                   | SW-RMT-SR-001   | Draft  |
| UT-RMT-004     | Send 소켓 미초기화 상태에서 호출                        | SetSocket 미호출                    | 예외 발생 또는 에러 반환            | SW-RMT-SR-001   | Draft  |
| UT-RMT-005     | Receive 정상 수신 확인 (4바이트)                        | 사전 Send 수행, SetSocket 완료      | Buffer에 정확한 데이터 저장         | SW-RMT-SR-002   | Draft  |
| UT-RMT-006     | Receive 타임아웃 동작 확인                              | 수신 데이터 없음, 타임아웃 설정     | 타임아웃 후 에러/예외 발생          | SW-RMT-SR-003   | Draft  |
| UT-RMT-007     | CloseSocket 정상 소켓 해제 확인                         | SetSocket 완료 상태                 | 소켓 fd 해제, 재사용 불가           | —                | Draft  |
| UT-RMT-008     | 수신 데이터 길이 검증 (경계값)                           | len=0, len=MAX <!-- TODO: MAX 정의 --> | 비정상 길이 시 에러 처리        | SW-RMT-SR-004   | Draft  |
| UT-RMT-009     | <!-- TODO: 추가 테스트 케이스 작성 -->                  |                                     |                                    |                  | Draft  |

### 6.2 CANClass 유닛 테스트

| TC ID          | 테스트 목적                                           | 입력 / 전제 조건                    | 기대 결과                          | 요구사항 참조    | 상태   |
|----------------|-------------------------------------------------------|-------------------------------------|------------------------------------|------------------|--------|
| UT-RMT-010     | CAN SetSocket 정상 소켓 생성 (PF_CAN)                  | 유효한 ifname, canfd=0              | 소켓 fd > 0                        | SW-RMT-SR-010   | Draft  |
| UT-RMT-011     | CAN SetSocket 실패 시 에러 처리                         | 잘못된 ifname                       | perror 출력, 에러 상태             | SW-RMT-SR-011   | Draft  |
| UT-RMT-012     | CAN-FD 모드 소켓 생성                                   | canfd=1                             | CAN-FD 소켓 정상 생성              | SW-RMT-SR-010   | Draft  |
| UT-RMT-013     | CAN Send 정상 프레임 전송                               | SetSocket 완료, 유효한 프레임 데이터 | CAN 프레임 전송 성공              | SW-RMT-SR-010   | Draft  |
| UT-RMT-014     | CAN Send 소켓 미초기화 상태에서 호출                    | SetSocket 미호출                    | 에러 처리                          | SW-RMT-SR-011   | Draft  |
| UT-RMT-015     | <!-- TODO: 추가 테스트 케이스 작성 -->                  |                                     |                                    |                  | Draft  |

### 6.3 remotePC main 유닛 테스트

| TC ID          | 테스트 목적                                           | 입력 / 전제 조건                    | 기대 결과                          | 요구사항 참조    | 상태   |
|----------------|-------------------------------------------------------|-------------------------------------|------------------------------------|------------------|--------|
| UT-RMT-020     | Buffer 초기값 정상 설정 확인                            | 프로그램 시작                       | Buffer = {0, 1, 2, 3}             | SW-RMT-SR-001   | Draft  |
| UT-RMT-021     | 송신 루프 예외 발생 시 루프 지속 확인                    | Send 중 네트워크 에러 주입          | 에러 출력 후 루프 계속              | SW-RMT-SR-001   | Draft  |
| UT-RMT-022     | <!-- TODO: 추가 테스트 케이스 작성 -->                  |                                     |                                    |                  | Draft  |

### 6.4 remoteS32G main 유닛 테스트

| TC ID          | 테스트 목적                                           | 입력 / 전제 조건                    | 기대 결과                          | 요구사항 참조    | 상태   |
|----------------|-------------------------------------------------------|-------------------------------------|------------------------------------|------------------|--------|
| UT-RMT-030     | 정상 수신 및 Buffer 출력 확인                           | UDP 패킷 4바이트 수신               | Buffer[0..3] 정확히 출력           | SW-RMT-SR-002   | Draft  |
| UT-RMT-031     | 수신 루프 예외 발생 시 루프 지속 확인                    | Receive 중 에러 주입                | 에러 출력 후 루프 계속              | SW-RMT-SR-002   | Draft  |
| UT-RMT-032     | 수신 데이터의 CAN 포워딩 정확성                         | UDP 수신 후 CAN 전송 확인           | CAN 프레임 내용 일치               | SW-RMT-SR-010   | Draft  |
| UT-RMT-033     | 통신 손실 시 안전 동작 확인                              | UDP 수신 타임아웃 발생              | 안전 정지 명령 CAN 전송            | SW-RMT-SR-020, SR-021 | Draft |
| UT-RMT-034     | <!-- TODO: 추가 테스트 케이스 작성 -->                  |                                     |                                    |                  | Draft  |

## 7. 코드 커버리지 목표 (Code Coverage Targets)

| 커버리지 유형          | ASIL B 목표      | 현재 달성률                       |
|------------------------|-------------------|-----------------------------------|
| 구문 커버리지 (Statement) | >= 95%          | 대기 중: 구현/측정 후 갱신         |
| 분기 커버리지 (Branch)    | >= 90%          | 대기 중: 구현/측정 후 갱신         |
| MC/DC                     | 권장             | 대기 중: 구현/측정 후 갱신         |

## 8. 테스트 결과 요약 (Test Results Summary)

<!-- TODO: 테스트 수행 후 결과 기입 -->

| 유닛                | 총 TC 수 | Pass | Fail | Not Run | 커버리지    |
|---------------------|----------|------|------|---------|-------------|
| UDPClass            | <!-- TODO --> | — | — | — | <!-- TODO --> |
| CANClass            | <!-- TODO --> | — | — | — | <!-- TODO --> |
| remotePC main       | <!-- TODO --> | — | — | — | <!-- TODO --> |
| remoteS32G main     | <!-- TODO --> | — | — | — | <!-- TODO --> |

## 9. 테스트-요구사항 추적 매트릭스 (Test-to-Requirements Traceability)

| SW Safety Req ID | 테스트 케이스 ID                      | 커버리지 상태         |
|------------------|---------------------------------------|-----------------------|
| SW-RMT-SR-001   | UT-RMT-001, 003, 004, 020, 021       | <!-- TODO: 검토 -->  |
| SW-RMT-SR-002   | UT-RMT-002, 005, 006, 030, 031       | <!-- TODO: 검토 -->  |
| SW-RMT-SR-003   | UT-RMT-006                            | <!-- TODO: 검토 -->  |
| SW-RMT-SR-004   | UT-RMT-008                            | <!-- TODO: 검토 -->  |
| SW-RMT-SR-010   | UT-RMT-010, 013, 032                 | <!-- TODO: 검토 -->  |
| SW-RMT-SR-011   | UT-RMT-011, 014                      | <!-- TODO: 검토 -->  |
| SW-RMT-SR-020   | UT-RMT-033                            | <!-- TODO: 검토 -->  |
| SW-RMT-SR-021   | UT-RMT-033                            | <!-- TODO: 검토 -->  |

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | 테스트 환경, 커버리지 목표, 프레임워크 갱신 |
