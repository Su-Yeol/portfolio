# Software Integration Test Specification

## ISO 26262 Part 6, Clause 10 — 소프트웨어 통합 및 통합 테스트

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-IT-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 10 (Software integration and testing) |
| **ASIL** | B~C (통합 수준별 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | SuYeol Kim |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 1.0 |
| **Date** | 2026-04-06 |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 10에 따라 Decision Module의 소프트웨어 통합 테스트 계획 및 사양을 정의한다. 소프트웨어 컴포넌트 간 통합, 외부 모듈 인터페이스 검증, 그리고 variant별(K-City / CAN Gateway) 통합 검증을 기술한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-ARCH-001 | Software Architecture Description | 아키텍처 문서 |
| SW-DEC-UD-001 | Software Unit Design Specification | 단위 설계 문서 |
| SW-DEC-UT-001 | Software Unit Test Specification | 단위 테스트 문서 |
| SW-DEC-REQ-001 | Software Safety Requirements Specification | 안전 요구사항 |
| ISO 26262:2018 Part 6, Clause 10 | Software integration and testing | 국제 표준 |

---

## 3. 통합 전략 (Integration Strategy)

### 3.1 통합 순서

ISO 26262 Part 6, Clause 10.4.2에 따라 점진적 통합(Incremental Integration) 방식을 적용한다.

**K-City Variant 통합 순서:**

```
Phase 1: Common Library 통합
  ../common/src/ + ../common/include/ → 공통 라이브러리 빌드

Phase 2: K-City 내부 컴포넌트 통합
  PathManager + Common → 경로 관리 기능 검증
  Communicator + Common → 통신 기능 검증

Phase 3: K-City 전체 통합
  MainModule + Communicator + PathManager + Common → run_kcity

Phase 4: 외부 인터페이스 통합
  run_kcity + GPS Module
  run_kcity + LiDAR(Ibeo) Module
  run_kcity + Path Planning Module
  run_kcity + Pedestrian Detection Module
```

**CAN Gateway Variant 통합 순서:**

```
Phase 1: Common Library 통합
  ../common/src/ + ../common/include/ → 공통 라이브러리 빌드

Phase 2: Gateway 내부 컴포넌트 통합
  CRC + CANGateway + Common → run_gateway

Phase 3: 외부 인터페이스 통합
  run_gateway + CAN Bus Interface
  run_gateway + Vehicle Control System
```

### 3.2 통합 빌드 검증

| Variant | Build Command | Expected Output | 비고 |
|---------|--------------|----------------|------|
| K-City | `./build.sh` | `build/run_kcity` | 정상 빌드 확인 |
| K-City (Private) | `USE_PRIVATE_IMPL=1 ./build.sh` | `build/run_kcity` | Private 구현 빌드 |
| CAN Gateway | `./cangateway.sh` | `build/run_gateway` | 정상 빌드 확인 |
| CAN Gateway (Private) | `USE_PRIVATE_IMPL=1 ./cangateway.sh` | `build/run_gateway` | Private 구현 빌드 |

---

## 4. 테스트 환경 (Test Environment)

| 항목 | Host 환경 | Target 환경 |
|------|----------|------------|
| **하드웨어** | x86_64 PC | NXP S32G (aarch64) |
| **OS** | Ubuntu 22.04 LTS | Linux BSP (대기 중: 구현/측정 후 갱신) |
| **테스트 프레임워크** | Google Test 1.14.0 + Google Mock | Google Test 1.14.0 (cross-compiled) |
| **시뮬레이터** | 대기 중: 구현/측정 후 갱신 | N/A (실차/HIL) |
| **CAN 인터페이스** | Virtual CAN (vcan) | 실제 CAN 버스 |
| **GPS 시뮬레이션** | 대기 중: 구현/측정 후 갱신 | 실제 GPS 수신기 |
| **LiDAR 시뮬레이션** | 대기 중: 구현/측정 후 갱신 | 실제 Ibeo LiDAR |

---

## 5. 테스트 방법 (Test Methods)

ISO 26262 Part 6, Table 12에 따른 ASIL별 통합 테스트 방법:

| 방법 | ASIL A | ASIL B | ASIL C | ASIL D |
|------|--------|--------|--------|--------|
| 요구사항 기반 테스트 | ++ | ++ | ++ | ++ |
| 인터페이스 테스트 | ++ | ++ | ++ | ++ |
| Fault Injection 테스트 | + | + | ++ | ++ |
| 리소스 사용량 테스트 | + | + | ++ | ++ |

---

## 6. 통합 테스트 사양 (Integration Test Specifications)

### 6.1 K-City Variant 통합 테스트

#### 6.1.1 내부 컴포넌트 통합 테스트

| Test ID | 테스트 항목 | 통합 대상 | 시나리오 | 기대 결과 | 상태 |
|---------|-----------|----------|---------|----------|------|
| IT-K-001 | Communicator → MainModule 데이터 전달 | Communicator + MainModule | GPS/LiDAR 데이터 수신 후 MainModule에 전달 | MainModule이 올바른 센서 데이터 수신 | TODO |
| IT-K-002 | PathManager → MainModule 경로 전달 | PathManager + MainModule | 경로 Waypoints 갱신 후 MainModule에 전달 | MainModule이 올바른 경로 정보 수신 | TODO |
| IT-K-003 | 전체 내부 통합 (run_kcity) | MainModule + Communicator + PathManager + Common | 정상 주행 시나리오 시뮬레이션 | 올바른 주행 판단 출력 | TODO |
| IT-K-004 | Common Library 호환성 | K-City 소스 + Common | Common 함수 호출 정상 동작 | 링크 오류 없음, 정상 동작 | TODO |

#### 6.1.2 외부 인터페이스 통합 테스트

| Test ID | 테스트 항목 | 통합 대상 | 시나리오 | 기대 결과 | 상태 |
|---------|-----------|----------|---------|----------|------|
| IT-K-011 | GPS 모듈 연동 | run_kcity + GPS Module | GPS 데이터 수신 및 위치 갱신 | 올바른 위치 정보 반영 | TODO |
| IT-K-012 | LiDAR(Ibeo) 모듈 연동 | run_kcity + LiDAR Module | 장애물 데이터 수신 및 처리 | 올바른 장애물 인식 | TODO |
| IT-K-013 | Path Planning 모듈 연동 | run_kcity + Path Planning | 경로 데이터 수신 및 추종 | 올바른 경로 추종 동작 | TODO |
| IT-K-014 | Pedestrian Detection 연동 | run_kcity + Pedestrian Det. | 보행자 감지 이벤트 수신 | 긴급 판단 수행 | TODO |
| IT-K-015 | 다중 센서 동시 입력 | run_kcity + GPS + LiDAR + Path | 모든 센서 동시 활성 | 센서 융합 판단 정상 | TODO |

#### 6.1.3 Fault Injection 테스트

| Test ID | 테스트 항목 | 주입 오류 | 기대 결과 | ASIL | 상태 |
|---------|-----------|----------|----------|------|------|
| IT-K-F01 | GPS 신호 유실 | GPS 데이터 중단 | Safe State 전환 (SWR-DEC-003) | TODO | TODO |
| IT-K-F02 | LiDAR 데이터 손상 | 비정상 LiDAR 프레임 주입 | 데이터 폐기 + Fallback | TODO | TODO |
| IT-K-F03 | 경로 데이터 유실 | Path Planning 통신 중단 | 이전 경로 유지 | TODO | TODO |
| IT-K-F04 | config.ini 변조 | 비정상 설정값 주입 | 기본값 적용 또는 안전 종료 | TODO | TODO |

---

### 6.2 CAN Gateway Variant 통합 테스트

#### 6.2.1 내부 컴포넌트 통합 테스트

| Test ID | 테스트 항목 | 통합 대상 | 시나리오 | 기대 결과 | 상태 |
|---------|-----------|----------|---------|----------|------|
| IT-G-001 | CRC + CANGateway 통합 | CRC + CANGateway | CAN 메시지 송수신 시 CRC 자동 적용 | CRC 검증 통과 | TODO |
| IT-G-002 | 전체 Gateway 빌드 통합 | CANGateway + CRC + Common | `./cangateway.sh` 빌드 후 실행 | 정상 실행 | TODO |
| IT-G-003 | Common Library 호환성 | Gateway 소스 + Common | Common 함수 호출 정상 동작 | 링크 오류 없음 | TODO |

#### 6.2.2 외부 인터페이스 통합 테스트

| Test ID | 테스트 항목 | 통합 대상 | 시나리오 | 기대 결과 | 상태 |
|---------|-----------|----------|---------|----------|------|
| IT-G-011 | CAN Bus 송신 | run_gateway + CAN Interface | 제어 명령 CAN 프레임 송신 | 올바른 CAN ID/데이터 송신 | TODO |
| IT-G-012 | CAN Bus 수신 | run_gateway + CAN Interface | 외부 CAN 메시지 수신 및 파싱 | 올바른 데이터 추출 | TODO |
| IT-G-013 | CRC 오류 메시지 필터링 | run_gateway + CAN Interface | CRC 불일치 메시지 수신 | 메시지 폐기 + 오류 로그 | TODO |

#### 6.2.3 Fault Injection 테스트

| Test ID | 테스트 항목 | 주입 오류 | 기대 결과 | ASIL | 상태 |
|---------|-----------|----------|----------|------|------|
| IT-G-F01 | CAN Bus-off | CAN 인터페이스 오류 | 오류 검출 + 복구 시도 | TODO | TODO |
| IT-G-F02 | 연속 CRC 오류 | 다수 CRC 불일치 메시지 | TODO: 정의된 Failsafe 동작 | TODO | TODO |

---

### 6.3 Cross-Variant 통합 테스트

| Test ID | 테스트 항목 | 시나리오 | 기대 결과 | 상태 |
|---------|-----------|---------|----------|------|
| IT-X-001 | K-City → CAN Gateway 연동 | run_kcity 판단 결과를 run_gateway로 전달 | 올바른 CAN 명령 출력 | TODO |
| IT-X-002 | 공통 라이브러리 일관성 | 동일 Common 라이브러리로 양쪽 variant 빌드 | 양쪽 모두 정상 동작 | TODO |

---

## 7. 테스트 결과 요약 (Test Result Summary)

| 테스트 영역 | 전체 TC | Pass | Fail | N/A | 비고 |
|------------|---------|------|------|-----|------|
| K-City 내부 통합 | TODO | TODO | TODO | TODO | |
| K-City 외부 인터페이스 | TODO | TODO | TODO | TODO | |
| K-City Fault Injection | TODO | TODO | TODO | TODO | |
| Gateway 내부 통합 | TODO | TODO | TODO | TODO | |
| Gateway 외부 인터페이스 | TODO | TODO | TODO | TODO | |
| Gateway Fault Injection | TODO | TODO | TODO | TODO | |
| Cross-Variant | TODO | TODO | TODO | TODO | |

---

## 8. 추적성 매트릭스 (Traceability Matrix)

| Integration Test | Architecture Element | SW Safety Req | Unit Test |
|-----------------|---------------------|--------------|-----------|
| IT-K-001 ~ 004 | Communicator, MainModule, PathManager | SWR-DEC-001, 006 | UT-K-xxx |
| IT-K-011 ~ 015 | External Interfaces | SWR-DEC-001 ~ 004 | UT-K-xxx |
| IT-K-F01 ~ F04 | Safety Mechanisms | SWR-DEC-002, 003, 004 | UT-K-xxx |
| IT-G-001 ~ 003 | CANGateway, CRC | SWR-DEC-005, 006 | UT-G-xxx |
| IT-G-011 ~ 013 | CAN Interface | SWR-DEC-005 | UT-G-xxx |
| IT-G-F01 ~ F02 | Safety Mechanisms | SWR-DEC-005 | UT-G-xxx |

---

## 9. 미해결 사항 (Open Items)

- [ ] TODO: 센서 시뮬레이터 구축 (GPS, LiDAR, Pedestrian Detection)
- [ ] TODO: Virtual CAN (vcan) 기반 Host 통합 테스트 환경 구축
- [ ] TODO: NXP S32G Target 환경 통합 테스트 절차 수립
- [ ] TODO: K-City ↔ CAN Gateway 간 통신 인터페이스 확정
- [ ] TODO: HIL(Hardware-in-the-Loop) 테스트 환경 계획

---

## 10. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0 | 2026-04-06 | SuYeol Kim | 테스트 환경, ASIL, 메타데이터 갱신 |
