# Software Integration Test Specification

## Remote Control Module — UDP/CAN Remote Communication

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-RMT-IT-001                                             |
| **Version**           | 1.0                                                       |
| **ISO 26262 Reference** | Part 6, Clause 10 — Software Integration and Testing    |
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

본 문서는 ISO 26262 Part 6, Clause 10에 따라 Remote Control 모듈의 소프트웨어 통합 테스트 사양을 정의한다. 개별 유닛(UDPClass, CANClass)이 통합된 remotePC 및 remoteS32G 실행 파일의 end-to-end 동작, 그리고 두 프로세스 간의 UDP 통신 및 CAN 포워딩 연동을 검증한다.

## 2. 범위 (Scope)

- remotePC ↔ remoteS32G 간 UDP end-to-end 통신 검증
- remoteS32G 내부 UDPClass → CANClass 연동 검증
- 통신 손실 시나리오 통합 검증
- 이종 플랫폼 (x86 ↔ aarch64) 간 통신 호환성
- LTE 네트워크 경유 통신 안정성

## 3. 참조 문서 (Reference Documents)

| 문서 ID         | 문서명                                      | 비고                  |
|-----------------|---------------------------------------------|-----------------------|
| SW-RMT-REQ-001  | Software Safety Requirements Specification | 안전 요구사항 문서    |
| SW-RMT-ARCH-001 | Software Architecture Description          | 아키텍처 설계 문서    |
| SW-RMT-UD-001   | Software Unit Design Specification         | 유닛 설계 문서        |
| SW-RMT-UT-001   | Software Unit Test Specification           | 유닛 테스트 문서      |
| SW-RMT-VR-001   | Software Verification Report               | 검증 보고서           |

## 4. 통합 전략 (Integration Strategy)

### 4.1 통합 순서 (Integration Order)

ISO 26262 Part 6, Clause 10에 따라 점진적 통합(incremental integration) 전략을 적용한다:

| 단계 | 통합 범위                             | 검증 초점                              |
|------|---------------------------------------|----------------------------------------|
| 1    | UDPClass 단독 (loopback 테스트)        | 소켓 생성, 송수신 기본 기능             |
| 2    | CANClass 단독 (vCAN 테스트)            | CAN 소켓 생성, 프레임 전송 기본 기능    |
| 3    | remoteS32G 내부 통합 (UDP + CAN)       | 수신 데이터의 CAN 포워딩 정확성         |
| 4    | remotePC ↔ remoteS32G 통합             | end-to-end UDP 통신, 데이터 무결성      |
| 5    | 전체 시스템 통합 (LTE + CAN 버스)       | 실 환경 통신, 지연, 손실 검증           |

### 4.2 통합 환경 (Integration Environment)

| 항목                  | 내용                                                      |
|-----------------------|-----------------------------------------------------------|
| Sender 플랫폼         | x86 PC (Ubuntu/Linux)                                    |
| Receiver 플랫폼       | NXP S32G (aarch64, Linux)                                |
| 네트워크 (단계 1-3)   | localhost loopback / 유선 Ethernet                        |
| 네트워크 (단계 4-5)   | LTE 네트워크 (IP: 10.167.216.215 / 223.130.136.21)       |
| CAN 인터페이스         | vCAN (단계 2-3) / 실제 CAN 버스 (단계 5)                  |
| 바이너리               | output/remotePC (x86), output/remoteS32G (aarch64 prebuilt) |
| 모니터링 도구          | <!-- TODO: Wireshark, candump, cansend 등 도구 정의 -->   |

## 5. 통합 테스트 방법론 (Integration Test Methodology)

ISO 26262 Part 6, Clause 10, Table 12에 따른 ASIL B 테스트 방법:

| 테스트 방법                                          | ASIL B 권장 수준 | 적용 계획                        |
|------------------------------------------------------|-------------------|----------------------------------|
| 요구사항 기반 테스트 (Requirements-based testing)      | 강력 권장 (++)    | 적용 예정                        |
| 인터페이스 테스트 (Interface testing)                  | 강력 권장 (++)    | 적용 예정                        |
| 결함 주입 테스트 (Fault injection testing)             | 권장 (+)          | 적용 예정 (통신 손실 시나리오)   |
| 자원 사용량 테스트 (Resource usage testing)             | 권장 (+)          | <!-- TODO: 적용 여부 결정 -->    |

## 6. 통합 테스트 케이스 (Integration Test Cases)

### 6.1 UDP End-to-End 통신 테스트

| TC ID          | 테스트 목적                                              | 전제 조건                          | 테스트 절차                                    | 기대 결과                           | 요구사항 참조         | 상태   |
|----------------|----------------------------------------------------------|------------------------------------|------------------------------------------------|-------------------------------------|-----------------------|--------|
| IT-RMT-001     | remotePC → remoteS32G 정상 데이터 전달 확인               | 양쪽 프로세스 실행, 네트워크 연결   | 1) remotePC 실행 2) remoteS32G 실행 3) Buffer 값 확인 | S32G 측 Buffer[0..3] = {0,1,2,3}  | SW-RMT-SR-001, SR-002 | Draft  |
| IT-RMT-002     | 다수 패킷 연속 전송 시 데이터 무결성 확인                  | 양쪽 프로세스 실행                  | 1000회 이상 연속 송수신 후 데이터 검증          | 모든 패킷 데이터 일치               | SW-RMT-SR-001, SR-004 | Draft  |
| IT-RMT-003     | 네트워크 지연 환경에서의 통신 안정성                       | 네트워크 지연 주입 (tc netem)      | <!-- TODO: 지연값 정의 -->ms 지연 환경에서 송수신 | 허용 범위 내 수신 성공             | SW-RMT-SR-003        | Draft  |
| IT-RMT-004     | <!-- TODO: 추가 테스트 케이스 작성 -->                    |                                    |                                                |                                     |                       | Draft  |

### 6.2 통신 손실 시나리오 테스트

| TC ID          | 테스트 목적                                              | 전제 조건                          | 테스트 절차                                    | 기대 결과                           | 요구사항 참조         | 상태   |
|----------------|----------------------------------------------------------|------------------------------------|------------------------------------------------|-------------------------------------|-----------------------|--------|
| IT-RMT-010     | UDP 통신 완전 차단 시 타임아웃 감지                        | 정상 통신 중 네트워크 차단          | 1) 정상 통신 확인 2) iptables로 차단 3) 타임아웃 동작 확인 | 지정 시간 내 통신 손실 감지       | SW-RMT-SR-020        | Draft  |
| IT-RMT-011     | 통신 손실 시 CAN 안전 정지 명령 전송 확인                  | 통신 손실 감지 상태                 | candump로 CAN 버스 모니터링                     | 안전 정지 CAN 프레임 확인           | SW-RMT-SR-021        | Draft  |
| IT-RMT-012     | 통신 복구 후 정상 운영 재개 확인                           | 통신 손실 → 복구 시나리오           | 1) 차단 2) 복구 3) 정상 데이터 흐름 확인        | 정상 운영 복귀                      | SW-RMT-SR-022        | Draft  |
| IT-RMT-013     | 간헐적 패킷 손실 환경에서의 동작                           | tc netem으로 패킷 손실 주입        | <!-- TODO: 손실률 정의 -->% 손실 환경에서 동작 확인 | 안전 기능 정상 동작              | SW-RMT-SR-020        | Draft  |
| IT-RMT-014     | <!-- TODO: 추가 테스트 케이스 작성 -->                    |                                    |                                                |                                     |                       | Draft  |

### 6.3 UDP → CAN 포워딩 통합 테스트

| TC ID          | 테스트 목적                                              | 전제 조건                          | 테스트 절차                                    | 기대 결과                           | 요구사항 참조         | 상태   |
|----------------|----------------------------------------------------------|------------------------------------|------------------------------------------------|-------------------------------------|-----------------------|--------|
| IT-RMT-020     | UDP 수신 데이터의 CAN 프레임 변환 정확성                   | remoteS32G 실행, vCAN 설정         | 1) UDP 데이터 전송 2) candump로 CAN 프레임 확인 | UDP 데이터와 CAN 페이로드 일치     | SW-RMT-SR-010        | Draft  |
| IT-RMT-021     | CAN 전송 주기 준수 확인                                    | 연속 UDP 수신 중                   | CAN 프레임 전송 타임스탬프 분석                 | 전송 주기 <!-- TODO: ms --> 이내   | SW-RMT-SR-012        | Draft  |
| IT-RMT-022     | CAN 소켓 장애 시 에러 처리                                 | CAN 인터페이스 다운                 | 1) CAN ifdown 2) remoteS32G 동작 확인          | 에러 기록, 안전 상태 전이           | SW-RMT-SR-011        | Draft  |
| IT-RMT-023     | <!-- TODO: 추가 테스트 케이스 작성 -->                    |                                    |                                                |                                     |                       | Draft  |

### 6.4 이종 플랫폼 호환성 테스트

| TC ID          | 테스트 목적                                              | 전제 조건                          | 테스트 절차                                    | 기대 결과                           | 요구사항 참조         | 상태   |
|----------------|----------------------------------------------------------|------------------------------------|------------------------------------------------|-------------------------------------|-----------------------|--------|
| IT-RMT-030     | x86 → aarch64 바이트 오더 호환성 확인                     | 양쪽 바이너리 실행                  | 다양한 값의 Buffer 전송 후 수신 측 비교         | 바이트 오더 불일치 없음             | SW-RMT-SR-004        | Draft  |
| IT-RMT-031     | Prebuilt 바이너리 (output/) 실행 정상 동작                 | output/remotePC, output/remoteS32G | prebuilt 바이너리로 end-to-end 테스트           | 소스 빌드 결과와 동일 동작          | SW-RMT-SR-001        | Draft  |
| IT-RMT-032     | <!-- TODO: 추가 테스트 케이스 작성 -->                    |                                    |                                                |                                     |                       | Draft  |

## 7. 통합 테스트 결과 요약 (Integration Test Results Summary)

<!-- TODO: 테스트 수행 후 결과 기입 -->

| 테스트 카테고리         | 총 TC 수 | Pass | Fail | Not Run | 비고              |
|------------------------|----------|------|------|---------|-------------------|
| UDP E2E 통신            | <!-- TODO --> | — | — | — | <!-- TODO -->   |
| 통신 손실 시나리오       | <!-- TODO --> | — | — | — | <!-- TODO -->   |
| UDP → CAN 포워딩        | <!-- TODO --> | — | — | — | <!-- TODO -->   |
| 플랫폼 호환성           | <!-- TODO --> | — | — | — | <!-- TODO -->   |

## 8. 테스트-요구사항 추적 매트릭스 (Test-to-Requirements Traceability)

| SW Safety Req ID | 통합 테스트 케이스 ID              | 커버리지 상태         |
|------------------|------------------------------------|-----------------------|
| SW-RMT-SR-001   | IT-RMT-001, 002, 031              | <!-- TODO: 검토 -->  |
| SW-RMT-SR-002   | IT-RMT-001                        | <!-- TODO: 검토 -->  |
| SW-RMT-SR-003   | IT-RMT-003                        | <!-- TODO: 검토 -->  |
| SW-RMT-SR-004   | IT-RMT-002, 030                   | <!-- TODO: 검토 -->  |
| SW-RMT-SR-010   | IT-RMT-020, 021                   | <!-- TODO: 검토 -->  |
| SW-RMT-SR-011   | IT-RMT-022                        | <!-- TODO: 검토 -->  |
| SW-RMT-SR-012   | IT-RMT-021                        | <!-- TODO: 검토 -->  |
| SW-RMT-SR-020   | IT-RMT-010, 013                   | <!-- TODO: 검토 -->  |
| SW-RMT-SR-021   | IT-RMT-011                        | <!-- TODO: 검토 -->  |
| SW-RMT-SR-022   | IT-RMT-012                        | <!-- TODO: 검토 -->  |

## 9. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0     | 2026-04-06 | SuYeol Kim | 메타데이터, ASIL 갱신 |
