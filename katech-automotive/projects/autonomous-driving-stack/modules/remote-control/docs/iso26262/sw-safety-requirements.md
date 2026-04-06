# Software Safety Requirements Specification

## Remote Control Module — UDP/CAN Remote Communication

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-RMT-REQ-001                                            |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 6 — Specification of Software Safety Requirements |
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

본 문서는 ISO 26262 Part 6, Clause 6에 따라 Remote Control 모듈의 소프트웨어 안전 요구사항을 정의한다. 본 모듈은 원격 PC(remotePC)에서 UDP를 통해 제어 명령을 송신하고, NXP S32G 보드(remoteS32G)에서 이를 수신하여 CAN 버스를 통해 차량에 전달하는 원격 통신 기능을 포함한다.

원격 명령이 차량 운행에 직접적 영향을 미치므로, 통신 손실 감지 및 안전 상태 전이에 대한 요구사항을 중점적으로 다룬다.

## 2. 범위 (Scope)

- remotePC (x86): UDP 기반 원격 제어 명령 송신
- remoteS32G (aarch64): UDP 수신 → CAN 포워딩
- UDP 통신 채널 (LTE 네트워크 경유, Port 5200)
- CAN/CAN-FD 차량 인터페이스
- 통신 손실(communication loss) 감지 및 타임아웃 처리
- 안전 상태 전이 (safe state transition)

## 3. 참조 문서 (Reference Documents)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-RMT-ARCH-001 | Software Architecture Description          | 아키텍처 설계 문서    |
| SW-RMT-UD-001   | Software Unit Design Specification         | 유닛 설계 문서        |
| <!-- TODO -->   | Technical Safety Concept (TSC)              | 상위 안전 컨셉 참조   |
| <!-- TODO -->   | System Safety Requirements Specification    | 시스템 안전 요구사항  |
| <!-- TODO -->   | Hardware-Software Interface (HSI) Document  | HW/SW 인터페이스 정의 |

## 4. 용어 및 약어 (Terms and Abbreviations)

| 약어   | 정의                                                      |
|--------|-----------------------------------------------------------|
| UDP    | User Datagram Protocol — 비연결형 전송 프로토콜            |
| CAN    | Controller Area Network — 차량 내부 통신 프로토콜           |
| CAN-FD | CAN with Flexible Data-rate                               |
| LTE    | Long-Term Evolution — 이동통신 네트워크                    |
| S32G   | NXP S32G Vehicle Network Processor                        |
| ASIL   | Automotive Safety Integrity Level                         |
| FTTI   | Fault Tolerant Time Interval                              |

## 5. 소프트웨어 안전 요구사항 도출 (Derivation of SW Safety Requirements)

<!-- TODO: 시스템 안전 요구사항에서 소프트웨어 안전 요구사항으로의 분해/할당 근거 작성 -->

### 5.1 상위 요구사항 추적 (Traceability to System Requirements)

| System Req ID   | SW Safety Req ID | 할당 근거                                    |
|-----------------|------------------|----------------------------------------------|
| <!-- TODO -->   | SW-RMT-SR-001    | 원격 제어 명령의 무결성 보장 → SW 수신 검증   |
| <!-- TODO -->   | SW-RMT-SR-002    | 통신 손실 시 안전 상태 전이 → SW 타임아웃 처리 |
| <!-- TODO -->   | SW-RMT-SR-010    | CAN 포워딩 정확성 → SW 데이터 매핑 검증       |
| <!-- TODO -->   | SW-RMT-SR-020    | <!-- TODO: 추가 추적 관계 작성 -->            |

## 6. 소프트웨어 안전 요구사항 목록 (Software Safety Requirements)

### 6.1 UDP 통신 요구사항 (UDP Communication Requirements)

| Req ID         | 요구사항                                                          | ASIL  | 검증 방법       | 상태     |
|----------------|-------------------------------------------------------------------|-------|-----------------|----------|
| SW-RMT-SR-001  | remotePC는 지정된 IP/Port(LTE 경유)로 UDP 패킷을 정상 송신해야 한다 | B     | Test / Review   | Draft    |
| SW-RMT-SR-002  | remoteS32G는 UDP 패킷 수신 실패 시 에러를 기록하고 안전 상태로 전이해야 한다 | B | Test / Analysis | Draft |
| SW-RMT-SR-003  | UDP 수신 타임아웃은 <!-- TODO: 타임아웃 값 정의 --> ms 이내로 설정해야 한다 | B | Test / Review | Draft |
| SW-RMT-SR-004  | 수신된 UDP 데이터의 길이 및 범위 검증(plausibility check)을 수행해야 한다 | B | Test / Analysis | Draft |
| SW-RMT-SR-005  | <!-- TODO: 추가 요구사항 작성 -->                                  | <!-- TODO --> | <!-- TODO --> | Draft |

### 6.2 CAN 포워딩 요구사항 (CAN Forwarding Requirements)

| Req ID         | 요구사항                                                          | ASIL  | 검증 방법       | 상태     |
|----------------|-------------------------------------------------------------------|-------|-----------------|----------|
| SW-RMT-SR-010  | remoteS32G는 수신된 UDP 데이터를 CAN 프레임으로 정확히 변환하여 송신해야 한다 | B | Test / Review | Draft |
| SW-RMT-SR-011  | CAN 소켓 생성 실패 시 에러를 기록하고 재시도 또는 안전 상태로 전이해야 한다 | B | Test / Analysis | Draft |
| SW-RMT-SR-012  | CAN 프레임 전송 주기는 <!-- TODO: 전송 주기 정의 --> ms를 초과하지 않아야 한다 | B | Test | Draft |
| SW-RMT-SR-013  | <!-- TODO: 추가 요구사항 작성 -->                                  | <!-- TODO --> | <!-- TODO --> | Draft |

### 6.3 통신 손실 및 안전 요구사항 (Communication Loss & Safety Requirements)

| Req ID         | 요구사항                                                          | ASIL  | 검증 방법       | 상태     |
|----------------|-------------------------------------------------------------------|-------|-----------------|----------|
| SW-RMT-SR-020  | 연속 <!-- TODO: N회 --> 회 UDP 수신 실패 시 통신 손실로 판단해야 한다 | B    | Test / Analysis | Draft    |
| SW-RMT-SR-021  | 통신 손실 감지 시 CAN 버스에 안전 정지 명령을 전송해야 한다          | B    | Test            | Draft    |
| SW-RMT-SR-022  | 통신 복구 시 안전 상태에서 정상 운영으로의 전이 조건을 검증해야 한다   | B    | Test / Review   | Draft    |
| SW-RMT-SR-023  | 예외 발생(exception) 시 catch 블록에서 안전 상태 전이를 수행해야 한다 | B    | Test / Review   | Draft    |
| SW-RMT-SR-024  | <!-- TODO: 추가 요구사항 작성 -->                                  | <!-- TODO --> | <!-- TODO --> | Draft |

## 7. 안전 요구사항 속성 (Safety Requirements Attributes)

<!-- TODO: 각 요구사항에 대해 아래 속성을 확정 -->

각 소프트웨어 안전 요구사항은 다음 속성을 포함해야 한다:

- **ASIL 등급**: 해당 요구사항에 할당된 ASIL
- **안전 메커니즘**: 위반 시 적용되는 안전 메커니즘 (예: timeout watchdog, heartbeat check, plausibility check)
- **Fault Tolerant Time Interval (FTTI)**: 결함 허용 시간 간격
- **Safe state**: 해당 요구사항 위반 시 전이할 안전 상태 (예: CAN 안전 정지 명령 전송, 통신 차단)

## 8. 하드웨어-소프트웨어 인터페이스 (HW/SW Interface)

<!-- TODO: NXP S32G 하드웨어 및 네트워크 인터페이스에 대한 상세 정의 -->

| 인터페이스 항목            | 설명                                                    |
|----------------------------|---------------------------------------------------------|
| UDP Socket (sender)        | x86 PC에서 LTE 네트워크를 경유하여 UDP 패킷 송신         |
| UDP Socket (receiver)      | S32G에서 LTE 네트워크를 통해 UDP 패킷 수신               |
| CAN Socket (PF_CAN)        | S32G의 SocketCAN 인터페이스를 통한 CAN 프레임 송신        |
| CAN-FD Support             | <!-- TODO: CAN-FD 지원 범위 정의 -->                     |
| LTE Network Interface      | IP: 10.167.216.215 / 223.130.136.21, Port: 5200         |

## 9. 검증 기준 (Verification Criteria)

<!-- TODO: 각 요구사항의 합격/불합격 기준 상세 기술 -->

- UDP 송수신 성공률: <!-- TODO: 허용 손실률 정의 --> % 이상
- 통신 손실 감지 시간: <!-- TODO: 감지 지연 기준 정의 --> ms 이내
- CAN 포워딩 지연: <!-- TODO: 허용 지연 시간 정의 --> ms 이내
- 안전 상태 전이 시간: <!-- TODO: 전이 시간 기준 정의 --> ms 이내

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | 메타데이터, ASIL 갱신 |
