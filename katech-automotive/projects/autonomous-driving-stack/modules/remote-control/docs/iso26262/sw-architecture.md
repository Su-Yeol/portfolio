# Software Architecture Description

## Remote Control Module — UDP/CAN Remote Communication

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-RMT-ARCH-001                                           |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 7 — Software Architectural Design        |
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

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Remote Control 모듈의 소프트웨어 아키텍처 설계를 기술한다. 원격 PC에서 차량 제어 명령을 UDP로 송신하고, S32G 보드에서 수신하여 CAN 버스로 포워딩하는 전체 구조와 각 구성 요소 간의 인터페이스를 정의한다.

## 2. 범위 (Scope)

- remotePC (sender): UDP 기반 원격 명령 송신 소프트웨어
- remoteS32G (receiver): UDP 수신 및 CAN 포워딩 소프트웨어
- UDPClass: UDP 소켓 통신 추상화 계층
- CANClass: CAN 소켓 통신 추상화 계층
- 통신 인터페이스 (LTE 네트워크, 차량 CAN 버스)

## 3. 참조 문서 (Reference Documents)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-RMT-REQ-001  | Software Safety Requirements Specification | 안전 요구사항 문서    |
| SW-RMT-UD-001   | Software Unit Design Specification         | 유닛 설계 문서        |
| SW-RMT-IT-001   | Software Integration Test Specification    | 통합 테스트 문서      |
| <!-- TODO -->   | Technical Safety Concept (TSC)              | 상위 안전 컨셉 참조   |

## 4. 용어 및 약어 (Terms and Abbreviations)

| 약어       | 정의                                                      |
|------------|-----------------------------------------------------------|
| UDP        | User Datagram Protocol                                    |
| CAN        | Controller Area Network                                   |
| CAN-FD     | CAN with Flexible Data-rate                               |
| LTE        | Long-Term Evolution                                       |
| PF_CAN     | Protocol Family CAN (Linux SocketCAN)                     |
| S32G       | NXP S32G Vehicle Network Processor                        |
| SOCK_RAW   | Raw socket type for CAN communication                     |

## 5. 소프트웨어 아키텍처 개요 (Architectural Overview)

### 5.1 시스템 컨텍스트 (System Context)

```
┌──────────────┐     UDP (LTE)      ┌──────────────┐     CAN Bus      ┌──────────┐
│  Remote PC   │ ─────────────────► │  NXP S32G    │ ──────────────► │  Vehicle  │
│  (remotePC)  │   IP:Port 5200     │ (remoteS32G) │  PF_CAN/RAW     │  ECU(s)  │
│  x86 sender  │                    │  aarch64 rcv │                  │          │
└──────────────┘                    └──────────────┘                  └──────────┘
```

### 5.2 소프트웨어 구성 요소 (Software Components)

| 구성 요소        | 소스 파일              | 플랫폼    | 역할                              |
|------------------|------------------------|-----------|-----------------------------------|
| remotePC         | src/remotePC.cpp       | x86 PC    | UDP 명령 송신기                    |
| remoteS32G       | src/remoteS32G.cpp     | S32G      | UDP 수신기 + CAN 포워더            |
| UDPClass         | common/include (shared)| 공용      | UDP 소켓 생성, 송수신 추상화       |
| CANClass         | common/include (shared)| 공용      | CAN 소켓 생성, 프레임 송수신 추상화 |

### 5.3 아키텍처 다이어그램 (Architecture Diagram)

<!-- TODO: 정식 아키텍처 다이어그램 (예: SysML/UML block diagram) 첨부 -->

```
remotePC (x86)                          remoteS32G (S32G / aarch64)
┌─────────────────────┐                 ┌─────────────────────────────┐
│  main()             │                 │  main()                     │
│  ├─ UDPClass s32g   │                 │  ├─ UDPClass client         │
│  │  ├─ SetSocket()  │   UDP packet    │  │  ├─ SetSocket()          │
│  │  ├─ Send(4)  ────┼────────────────►│  │  ├─ Receive(4)          │
│  │  └─ Buffer[0..3] │                 │  │  └─ Buffer[0..3]        │
│  └─ CloseSocket()   │                 │  ├─ CANClass can            │
│                     │                 │  │  ├─ SetSocket(ifname,fd) │
│                     │                 │  │  ├─ Send(frame)          │
│                     │                 │  │  └─ CloseSocket()        │
│                     │                 │  └─ 통신 손실 처리           │
└─────────────────────┘                 └─────────────────────────────┘
```

## 6. 정적 아키텍처 (Static Architecture)

### 6.1 클래스 구조 (Class Structure)

<!-- TODO: 정식 UML 클래스 다이어그램 첨부 -->

| 클래스명    | 주요 멤버                          | 책임                              |
|-------------|------------------------------------|------------------------------------|
| UDPClass    | sock, Buffer[], SetSocket(), Send(), Receive(), CloseSocket() | UDP 소켓 통신 전담 |
| CANClass    | sock, SetSocket(ifname, canfd), Send(), CloseSocket() | CAN 소켓 통신 전담 |

### 6.2 데이터 흐름 (Data Flow)

1. remotePC: Buffer[0..3]에 제어 데이터 설정 → `UDPClass::Send(4)` 호출
2. LTE 네트워크를 통한 UDP 패킷 전송 (IP: 10.167.216.215, Port: 5200)
3. remoteS32G: `UDPClass::Receive(4)` → Buffer[0..3] 수신
4. remoteS32G: Buffer 데이터를 CAN 프레임으로 변환 → `CANClass::Send()` 호출
5. CAN 버스를 통해 차량 ECU로 전달

## 7. 동적 아키텍처 (Dynamic Architecture)

### 7.1 실행 흐름 (Execution Flow)

**remotePC (Sender):**
1. UDPClass 인스턴스 생성 및 Buffer 초기화
2. `SetSocket(LTEIp, LTEPort)` — 소켓 설정
3. 무한 루프: `Send(4)` — 4바이트 데이터 연속 전송
4. 예외 발생 시 stderr 출력 후 루프 계속
5. (정상 종료 시) `CloseSocket()`

**remoteS32G (Receiver):**
1. UDPClass 인스턴스 생성
2. `SetSocket(LTEIp, LTEPort)` — 소켓 설정
3. 무한 루프: `Receive(4)` — 4바이트 데이터 수신 대기
4. 수신 데이터 출력 및 <!-- TODO: CAN 포워딩 로직 구현/확인 --> CAN 전송
5. 예외 발생 시 stderr 출력 후 루프 계속
6. (정상 종료 시) `CloseSocket()`

### 7.2 예외 처리 흐름 (Exception Handling Flow)

<!-- TODO: 예외 시나리오별 상세 처리 흐름 정의 -->

| 예외 시나리오          | 현재 처리                     | 안전 요구사항 매핑        |
|------------------------|-------------------------------|---------------------------|
| UDP Send 실패          | catch → stderr 출력 → 루프 계속 | SW-RMT-SR-001           |
| UDP Receive 실패       | catch → stderr 출력 → 루프 계속 | SW-RMT-SR-002           |
| CAN 소켓 생성 실패     | perror 출력                    | SW-RMT-SR-011           |
| 통신 손실 (timeout)    | <!-- TODO: 미구현 -->          | SW-RMT-SR-020, SR-021   |

## 8. 안전 메커니즘 아키텍처 (Safety Mechanisms)

<!-- TODO: 아래 안전 메커니즘의 구현 상태 확인 및 설계 상세화 -->

### 8.1 통신 손실 감지 (Communication Loss Detection)

- **메커니즘**: UDP 수신 타임아웃 모니터링
- **트리거 조건**: <!-- TODO: 타임아웃 임계값 정의 (예: 100ms 무수신) -->
- **안전 동작**: CAN 버스에 안전 정지 명령 전송
- **요구사항 참조**: SW-RMT-SR-020, SW-RMT-SR-021

### 8.2 데이터 무결성 검증 (Data Integrity Check)

- **메커니즘**: 수신 데이터 길이 및 범위 검증 (plausibility check)
- **현재 상태**: <!-- TODO: Buffer 범위 검증 로직 구현 여부 확인 -->
- **요구사항 참조**: SW-RMT-SR-004

### 8.3 Heartbeat 모니터링

- **메커니즘**: <!-- TODO: 주기적 heartbeat 패킷 설계 -->
- **요구사항 참조**: <!-- TODO: 해당 요구사항 ID 연결 -->

## 9. 아키텍처 설계 원칙 (Design Principles)

ISO 26262 Part 6, Clause 7에 따른 설계 원칙 적용 현황:

| 설계 원칙                        | 적용 현황                                         | 상태              |
|----------------------------------|---------------------------------------------------|-------------------|
| 모듈화 (Modularity)              | UDPClass/CANClass로 통신 계층 분리                 | 적용됨            |
| 캡슐화 (Encapsulation)           | 클래스 기반 소켓 관리                               | 적용됨            |
| 단순성 (Simplicity)              | 단일 루프 구조의 송수신                             | 적용됨            |
| 결함 감지 (Fault Detection)       | 예외 처리(try-catch) 적용                          | 부분 적용         |
| 결함 허용 (Fault Tolerance)       | 대기 중: 구현/측정 후 갱신 — 통신 손실 시 안전 상태 전이 구현 필요 | 대기 중: 구현/측정 후 갱신 |

## 10. 아키텍처 요구사항 추적 매트릭스 (Traceability Matrix)

| SW Safety Req ID | 아키텍처 구성 요소          | 설계 충족 여부        |
|------------------|-----------------------------|-----------------------|
| SW-RMT-SR-001   | remotePC / UDPClass::Send   | <!-- TODO: 검토 --> |
| SW-RMT-SR-002   | remoteS32G / UDPClass::Receive | <!-- TODO: 검토 --> |
| SW-RMT-SR-010   | remoteS32G / CANClass::Send | <!-- TODO: 검토 --> |
| SW-RMT-SR-020   | remoteS32G / timeout logic  | <!-- TODO: 미구현 확인 --> |
| SW-RMT-SR-021   | remoteS32G / safe-stop CAN  | <!-- TODO: 미구현 확인 --> |

## 11. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | 메타데이터, ASIL, 통신 정보 갱신 |
