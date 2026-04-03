# AEB Control — 소프트웨어 통합 테스트 (Software Integration Testing)

> ISO 26262-6:2018, Clause 10 — Software integration and testing

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-IT-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 10 |
| **ASIL 등급** | ASIL D |
| **대상 HW** | NXP S32G (aarch64) |
| **작성일** | TODO: YYYY-MM-DD |
| **작성자** | TODO: 작성자 |
| **검토자** | TODO: 검토자 |
| **승인자** | TODO: 승인자 |
| **버전** | 0.1 (초안) |
| **상태** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 AEB Control 모듈의 소프트웨어 통합 테스트 계획 및 사양을 정의한다.
ISO 26262-6 Clause 10에 따라, 아키텍처 설계(SW-AEB-ARCH-001)에서 정의된
컴포넌트 간 인터페이스 및 상호작용의 정확성을 검증한다.
특히 센서 입력 → 판단 → CAN 출력의 E2E 데이터 흐름과 타이밍을 중점 검증한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-REQ-001 | AEB SW Safety Requirements | Part 6, Clause 6 |
| SW-AEB-ARCH-001 | AEB SW Architecture | Part 6, Clause 7 |
| SW-AEB-UD-001 | AEB SW Unit Design | Part 6, Clause 8 |
| SW-AEB-UT-001 | AEB SW Unit Test | Part 6, Clause 9 |
| SW-AEB-VR-001 | AEB SW Verification Report | Part 6, Clause 11 |

---

## 3. 통합 전략 (Integration Strategy)

### 3.1 통합 순서

ASIL D 요구에 따라 **점진적 통합(Incremental Integration)** 전략을 적용한다.

```
Step 1: Communicator + CanSpecProvider + CrcProvider + SignalCodec
        → CAN 송수신 기본 통합
        
Step 2: Step 1 + DecisionEngine
        → 센서 데이터 수신 → TTC 판단 흐름
        
Step 3: Step 2 + ConfigLoader
        → 설정 기반 파라미터 적용 확인
        
Step 4: Step 3 + SafetyMonitor
        → 안전 메커니즘 통합
        
Step 5: AEBControlMain (전체 통합)
        → E2E 통합 및 타이밍 검증
```

### 3.2 통합 환경

| 환경 | 용도 | 비고 |
|------|------|------|
| SIL (Software-in-the-Loop) | 호스트 PC에서 로직 통합 검증 | TODO: 시뮬레이터 구성 |
| HIL (Hardware-in-the-Loop) | NXP S32G 타겟에서 실시간 검증 | TODO: HIL 장비 구성 |
| 실차 | 최종 통합 검증 | TODO: 실차 테스트 계획 별도 |

---

## 4. 테스트 방법 (Test Methods)

ISO 26262-6 Table 12에 따른 통합 테스트 방법:

| 방법 | ASIL D 권장 | 적용 여부 | 비고 |
|------|------------|-----------|------|
| 요구사항 기반 테스트 | 강력 권장 | 적용 예정 | SWREQ 기반 |
| 인터페이스 테스트 | 강력 권장 | 적용 예정 | 컴포넌트 간 I/F |
| 결함 주입 테스트 | 강력 권장 | 적용 예정 | CAN, 센서 결함 |
| 리소스 사용 테스트 | 권장 | TODO | CPU, 메모리, 스택 |

---

## 5. 통합 테스트 사양 (Integration Test Specifications)

### 5.1 Step 1: CAN 통신 스택 통합

#### TC-IT-001: CAN 수신 → 디코딩 → CRC 검증 E2E

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-001 |
| **통합 대상** | Communicator + SignalCodec + CrcProvider |
| **SWREQ** | SWREQ-AEB-002, SWREQ-AEB-004 |
| **목적** | 실제 CAN 프레임 수신 → SignalCodec 디코딩 → CRC 검증 파이프라인 정합성 |
| **사전 조건** | CanSpecProvider에 유효한 CAN DB 로드 |
| **입력** | CAN 시뮬레이터로 센서 메시지 전송 (유효 CRC) |
| **기대 출력** | SensorData 필드값 정확, CRC 검증 통과 |
| **환경** | SIL (CAN 시뮬레이션) |

#### TC-IT-002: CAN 송신 → 인코딩 → CRC 첨부 E2E

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-002 |
| **통합 대상** | Communicator + CanSpecProvider + CrcProvider |
| **SWREQ** | SWREQ-AEB-002 |
| **목적** | BrakeCommand → CAN 인코딩 → CRC 첨부 → 송신 정합성 |
| **입력** | BrakeCommand(targetDecel=5.0m/s^2) |
| **기대 출력** | CAN 프레임 내 올바른 raw 값 및 유효 CRC |
| **환경** | SIL + CAN 모니터 |

---

### 5.2 Step 2: 센서 → 판단 흐름 통합

#### TC-IT-010: 센서 수신 → TTC 판단 → 제동 명령 생성

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-010 |
| **통합 대상** | Communicator + DecisionEngine |
| **SWREQ** | SWREQ-AEB-001, SWREQ-AEB-002 |
| **목적** | 센서 CAN 메시지 수신 → DecisionEngine TTC 평가 → BrakeCommand 생성 흐름 |
| **시나리오** | 전방 30m, 상대속도 -20m/s → TTC=1.5s → 긴급 제동 판단 |
| **기대 출력** | brakeRequired=true, 유효한 targetDeceleration 산출 |

#### TC-IT-011: 센서 데이터 이상 시 결함 전파 확인

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-011 |
| **통합 대상** | Communicator + DecisionEngine |
| **SWREQ** | SWREQ-AEB-003 |
| **목적** | CRC 오류 또는 timeout 발생 시, DecisionEngine이 유효하지 않은 데이터로 판단하지 않음을 확인 |
| **입력** | CRC 오류 CAN 프레임, 이후 timeout 발생 |
| **기대 출력** | 오류 데이터 폐기, 안전 상태 전환 트리거 |

---

### 5.3 Step 3: 설정 통합

#### TC-IT-020: 설정 파일 → 판단 파라미터 적용

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-020 |
| **통합 대상** | ConfigLoader + DecisionEngine + AEBControlMain |
| **SWREQ** | SWREQ-AEB-005 |
| **목적** | config.ini의 TTC 임계값이 DecisionEngine에 정확히 반영됨을 확인 |
| **입력** | 사용자 정의 ttc_threshold 값을 가진 config.ini |
| **기대 출력** | 해당 임계값 기준으로 제동 판단 동작 |

---

### 5.4 Step 4: 안전 메커니즘 통합

#### TC-IT-030: Watchdog 타이머 통합

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-030 |
| **통합 대상** | SafetyMonitor + AEBControlMain |
| **SWREQ** | SWREQ-AEB-002 |
| **목적** | 메인 루프가 정상 주기로 watchdog을 kick하며, 주기 이탈 시 검출 |
| **시나리오** | 정상 동작 시 watchdog 정상, 인위적 지연 주입 시 watchdog 만료 |
| **기대 출력** | watchdog 만료 시 안전 상태 전환 |

#### TC-IT-031: 다중 결함 시나리오

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-031 |
| **통합 대상** | SafetyMonitor + Communicator + DecisionEngine |
| **SWREQ** | SWREQ-AEB-003, SWREQ-AEB-004 |
| **목적** | 센서 timeout + CAN 송신 실패 동시 발생 시 안전 상태 전환 |
| **입력** | 센서 메시지 중단 + CAN Tx 오류 모킹 |
| **기대 출력** | Fail-safe 동작, 진단 코드 기록 |

---

### 5.5 Step 5: 전체 E2E 통합

#### TC-IT-040: E2E 정상 시나리오 — 긴급 제동

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-040 |
| **통합 대상** | 전체 AEB Control 모듈 |
| **SWREQ** | SWREQ-AEB-001, SWREQ-AEB-002 |
| **목적** | 센서 CAN 입력 → 전체 파이프라인 → 브레이크 CAN 출력 E2E 검증 |
| **시나리오** | 시뮬레이터에서 접근 차량 시나리오 재현 |
| **기대 출력** | 정확한 제동 명령 CAN 메시지 출력 |
| **타이밍** | E2E 지연 ≤ TODO ms (목표 50ms) 확인 |

#### TC-IT-041: E2E 정상 시나리오 — 제동 불필요

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-041 |
| **통합 대상** | 전체 AEB Control 모듈 |
| **SWREQ** | SWREQ-AEB-001 |
| **목적** | 위험하지 않은 상황에서 제동 미발생 확인 (오작동 방지) |
| **시나리오** | 이격 차량, 충분한 거리 유지 시나리오 |
| **기대 출력** | brakeRequired=false, 제동 CAN 메시지 미출력 |

#### TC-IT-042: E2E 타이밍 측정

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-IT-042 |
| **통합 대상** | 전체 AEB Control 모듈 |
| **SWREQ** | SWREQ-AEB-002, SWREQ-AEB-NF-001 |
| **목적** | 센서 CAN Rx 타임스탬프 → 브레이크 CAN Tx 타임스탬프 지연 측정 |
| **환경** | HIL (NXP S32G 타겟) |
| **판정 기준** | E2E 지연 ≤ TODO ms (목표 50ms), 주기 ≤ TODO ms |
| **측정 방법** | TODO: 오실로스코프 또는 CAN 로거 타임스탬프 분석 |

---

## 6. 결함 주입 통합 테스트 (Fault Injection Integration Tests)

| Test ID | 주입 결함 | 통합 레벨 | 기대 동작 | SWREQ |
|---------|-----------|-----------|-----------|-------|
| TC-FI-IT-001 | CAN Bus-off 상태 | Step 1 | 송수신 중단 검출, Fail-safe | SWREQ-AEB-004 |
| TC-FI-IT-002 | 센서 메시지 주기 2배 지연 | Step 2 | timeout 검출, 안전 상태 전환 | SWREQ-AEB-003 |
| TC-FI-IT-003 | config.ini 삭제 후 시작 | Step 3 | 기본 안전값으로 기동 | SWREQ-AEB-005 |
| TC-FI-IT-004 | CPU 부하 인위적 증가 | Step 5 | WCET 초과 검출, watchdog 동작 | SWREQ-AEB-NF-003 |
| TC-FI-IT-005 | TODO: 추가 결함 시나리오 | TODO | TODO | TODO |

---

## 7. 테스트 결과 요약 (Test Results Summary)

| 통합 단계 | 총 TC 수 | Pass | Fail | Blocked | 비고 |
|-----------|----------|------|------|---------|------|
| Step 1: CAN 스택 | TODO | TODO | TODO | TODO | |
| Step 2: 센서→판단 | TODO | TODO | TODO | TODO | |
| Step 3: 설정 | TODO | TODO | TODO | TODO | |
| Step 4: 안전 메커니즘 | TODO | TODO | TODO | TODO | |
| Step 5: 전체 E2E | TODO | TODO | TODO | TODO | |
| 결함 주입 | TODO | TODO | TODO | TODO | |
| **합계** | **TODO** | **TODO** | **TODO** | **TODO** | |

---

## 8. TODO 항목

- [ ] SIL 시뮬레이션 환경 구축 (CAN 시뮬레이터 포함)
- [ ] HIL 테스트 환경 구성 (NXP S32G 보드, CAN 인터페이스)
- [ ] 통합 테스트 자동화 스크립트 작성
- [ ] E2E 타이밍 측정 도구 및 방법론 확정
- [ ] 결함 주입 도구 선정 (SW FI, HW FI)
- [ ] 실차 테스트 계획 별도 수립
- [ ] 통합 테스트 결과 리포팅 체계 구축
- [ ] 각 Step별 통과 기준(Entry/Exit criteria) 상세화
- [ ] 회귀 테스트(Regression Test) 전략 수립
- [ ] 통합 테스트 리뷰 수행 및 기록
