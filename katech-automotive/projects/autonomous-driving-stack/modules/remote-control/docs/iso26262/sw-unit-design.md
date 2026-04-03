# Software Unit Design Specification

## Remote Control Module — UDP/CAN Remote Communication

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-RMT-UD-001                                             |
| **Version**           | 0.1 Draft                                                 |
| **ISO 26262 Reference** | Part 6, Clause 8 — Software Unit Design and Implementation |
| **ASIL Scope**        | ASIL B (target) <!-- TODO: 최종 ASIL 등급 확정 후 업데이트 -->      |
| **Project**           | Autonomous Driving Stack                                  |
| **Module**            | Remote Control (remotePC / remoteS32G)                    |
| **Target HW**         | NXP S32G (aarch64) — receiver / x86 PC — sender          |
| **Author**            | <!-- TODO: 작성자 이름 기입 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 기입 -->                             |
| **Approval Date**     | <!-- TODO: 승인 일자 기입 (YYYY-MM-DD) -->                  |
| **Status**            | Draft                                                     |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 8에 따라 Remote Control 모듈의 소프트웨어 유닛 수준 상세 설계를 기술한다. 각 소프트웨어 유닛(UDPClass, CANClass, remotePC main, remoteS32G main)의 내부 로직, 인터페이스, 데이터 구조 및 구현 지침을 정의한다.

## 2. 범위 (Scope)

- remotePC main() 유닛 — UDP 송신 제어 루프
- remoteS32G main() 유닛 — UDP 수신 및 CAN 포워딩 제어 루프
- UDPClass 유닛 — UDP 소켓 관리 및 데이터 송수신
- CANClass 유닛 — CAN 소켓 관리 및 프레임 송수신

## 3. 참조 문서 (Reference Documents)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-RMT-REQ-001  | Software Safety Requirements Specification | 안전 요구사항 문서    |
| SW-RMT-ARCH-001 | Software Architecture Description          | 아키텍처 설계 문서    |
| SW-RMT-UT-001   | Software Unit Test Specification           | 유닛 테스트 문서      |
| <!-- TODO -->   | C++ Coding Guidelines                       | 코딩 표준 문서        |

## 4. 소프트웨어 유닛 목록 (Software Unit List)

| 유닛 ID        | 유닛명                | 소스 파일                  | 설명                    |
|----------------|-----------------------|----------------------------|-------------------------|
| SU-RMT-001     | remotePC_main         | src/remotePC.cpp           | UDP 송신 메인 루프       |
| SU-RMT-002     | remoteS32G_main       | src/remoteS32G.cpp         | UDP 수신 + CAN 포워딩    |
| SU-RMT-003     | UDPClass              | common/include/.../remote.h | UDP 통신 추상화 클래스   |
| SU-RMT-004     | CANClass              | common/include/.../remote.h | CAN 통신 추상화 클래스   |

## 5. 유닛 상세 설계 (Unit Detailed Design)

### 5.1 SU-RMT-001: remotePC_main

**파일**: `src/remotePC.cpp`

**상수 정의:**
| 상수명    | 값                  | 설명                     |
|-----------|---------------------|--------------------------|
| LTEIp     | "10.167.216.215"    | UDP 목적지 IP (S32G)     |
| LTEPort   | 5200                | UDP 목적지 포트           |

**처리 흐름:**
1. UDPClass 인스턴스 `s32g` 생성
2. Buffer[0..3] 초기값 설정: {0, 1, 2, 3}
3. `SetSocket(LTEIp, LTEPort)` 호출 — 소켓 초기화
4. 무한 루프 진입:
   - `s32g.Send(4)` — 4바이트 전송
   - 예외 발생 시 `std::cerr` 출력 후 루프 계속
5. `s32g.CloseSocket()` — 정상 종료 시 소켓 해제

**안전 고려사항:**
- <!-- TODO: Buffer 값이 하드코딩되어 있음 — 실제 제어 명령 매핑 설계 필요 -->
- <!-- TODO: 송신 주기(rate limiting) 미적용 — 과도 전송 방지 설계 필요 -->
- <!-- TODO: 무한 루프 탈출 조건 미정의 — graceful shutdown 설계 필요 -->

### 5.2 SU-RMT-002: remoteS32G_main

**파일**: `src/remoteS32G.cpp`

**상수 정의:**
| 상수명    | 값                  | 설명                     |
|-----------|---------------------|--------------------------|
| LTEIp     | "223.130.136.21"    | UDP 바인딩 IP             |
| LTEPort   | 5200                | UDP 수신 포트             |

**처리 흐름:**
1. UDPClass 인스턴스 `client` 생성
2. `SetSocket(LTEIp, LTEPort)` 호출 — 소켓 초기화
3. 무한 루프 진입:
   - `client.Receive(4)` — 4바이트 수신 대기
   - Buffer[0..3] 내용 printf 출력
   - <!-- TODO: CAN 포워딩 로직 호출 (CANClass 연동) 구현/확인 필요 -->
   - 예외 발생 시 `std::cerr` 출력 후 루프 계속
4. `client.CloseSocket()` — 정상 종료 시 소켓 해제

**안전 고려사항:**
- <!-- TODO: 수신 타임아웃 미설정 — Receive() 블로킹 호출 시 무한 대기 가능 -->
- <!-- TODO: 수신 데이터 검증(plausibility check) 미구현 -->
- <!-- TODO: 통신 손실 감지 로직 미구현 -->

### 5.3 SU-RMT-003: UDPClass

**파일**: `common/include/modules/remote-control/remote.h`

| 메서드/멤버        | 시그니처                                    | 설명                        |
|--------------------|---------------------------------------------|-----------------------------|
| Buffer             | `uint8_t Buffer[]` <!-- TODO: 정확한 타입 확인 --> | 송수신 데이터 버퍼       |
| sock               | `int sock`                                  | 소켓 파일 디스크립터        |
| SetSocket          | `void SetSocket(string ip, int port)`       | UDP 소켓 생성 및 바인딩     |
| Send               | `void Send(int len)`                        | Buffer 내용을 len 바이트 전송 |
| Receive            | `void Receive(int len)`                     | len 바이트 수신 → Buffer 저장 |
| CloseSocket        | `void CloseSocket()`                        | 소켓 자원 해제              |

**설계 고려사항:**
- <!-- TODO: Send/Receive 실패 시 반환값 또는 예외 사양 상세화 -->
- <!-- TODO: Buffer 크기 제한 및 오버플로우 방지 검증 -->
- <!-- TODO: 소켓 옵션 (SO_RCVTIMEO 등) 설정 여부 확인 -->

### 5.4 SU-RMT-004: CANClass

**파일**: `common/include/modules/remote-control/remote.h`

| 메서드/멤버        | 시그니처                                        | 설명                          |
|--------------------|-------------------------------------------------|-------------------------------|
| sock               | `int sock`                                      | CAN 소켓 파일 디스크립터       |
| SetSocket          | `void SetSocket(string ifname, int canfd)`      | CAN 소켓 생성 (PF_CAN, SOCK_RAW, CAN_RAW) |
| Send               | <!-- TODO: 시그니처 확인 -->                     | CAN 프레임 전송               |
| CloseSocket        | `void CloseSocket()`                            | 소켓 자원 해제                |

**설계 고려사항:**
- 소켓 생성 실패 시 `perror("<CAN> socket open error")` 출력 — <!-- TODO: 에러 복구 전략 정의 필요 -->
- <!-- TODO: CAN-FD 지원 시 canfd 파라미터 활용 방안 문서화 -->
- <!-- TODO: CAN 프레임 ID 및 DLC 매핑 정의 필요 -->

## 6. 코딩 가이드라인 준수 (Coding Guidelines Compliance)

ISO 26262 Part 6, Clause 8 Table 8에 따른 코딩 가이드라인 적용:

| 항목                                    | 적용 기준           | 현재 상태                          |
|-----------------------------------------|---------------------|------------------------------------|
| MISRA C++ 적용                           | ASIL B 권장         | <!-- TODO: 정적 분석 수행 여부 --> |
| 방어적 프로그래밍 (Defensive programming) | ASIL B 권장         | try-catch 적용 (부분)              |
| 명시적 타입 캐스팅                        | ASIL B 권장         | <!-- TODO: 검토 필요 -->           |
| 동적 메모리 할당 제한                     | ASIL B 권장         | <!-- TODO: new/delete 사용 검토 --> |
| 무한 루프 안전 종료                       | ASIL B 권장         | <!-- TODO: 미구현 — 종료 조건 설계 필요 --> |

## 7. 유닛-요구사항 추적 매트릭스 (Unit-to-Requirements Traceability)

| 유닛 ID      | SW Safety Req ID                     | 비고                          |
|--------------|--------------------------------------|-------------------------------|
| SU-RMT-001   | SW-RMT-SR-001                        | UDP 송신 정상 동작             |
| SU-RMT-002   | SW-RMT-SR-002, SR-010, SR-020, SR-021 | UDP 수신 + CAN 포워딩 + 안전  |
| SU-RMT-003   | SW-RMT-SR-001, SR-002, SR-003, SR-004 | UDP 통신 계층                 |
| SU-RMT-004   | SW-RMT-SR-010, SR-011, SR-012        | CAN 통신 계층                  |

## 8. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | <!-- TODO: 날짜 --> | <!-- TODO: 작성자 --> | Initial draft |
