# AEB Control — 소프트웨어 단위 테스트 (Software Unit Testing)

> ISO 26262-6:2018, Clause 9 — Software unit testing

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-UT-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 9 |
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

본 문서는 AEB Control 모듈의 소프트웨어 단위 테스트 계획 및 사양을 정의한다.
ISO 26262-6 Clause 9에 따라, 단위 설계(SW-AEB-UD-001)에서 정의된 각 소프트웨어 단위의
정확성을 검증하기 위한 테스트 케이스, 방법, 커버리지 기준을 기술한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-REQ-001 | AEB SW Safety Requirements | Part 6, Clause 6 |
| SW-AEB-ARCH-001 | AEB SW Architecture | Part 6, Clause 7 |
| SW-AEB-UD-001 | AEB SW Unit Design | Part 6, Clause 8 |
| SW-AEB-VR-001 | AEB SW Verification Report | Part 6, Clause 11 |

---

## 3. 테스트 환경 (Test Environment)

| 항목 | 내용 |
|------|------|
| 테스트 프레임워크 | TODO: (예: Google Test, CppUTest) |
| 목/스텁 프레임워크 | TODO: (예: Google Mock, FFF) |
| 커버리지 도구 | TODO: (예: gcov, LCOV, BullseyeCoverage) |
| 호스트 빌드 환경 | TODO: x86_64 호스트 크로스 컴파일 또는 네이티브 |
| 타겟 환경 | NXP S32G (aarch64) — 타겟 단위 테스트 필요 여부 TODO |
| CI 통합 | TODO: Jenkins, GitLab CI 등 |

---

## 4. 커버리지 기준 (Coverage Criteria)

ISO 26262-6 Table 10에 따른 ASIL D 커버리지 목표:

| 커버리지 유형 | ASIL D 권장 | 목표 | 현재 달성 | 비고 |
|--------------|------------|------|-----------|------|
| 구문 커버리지 (Statement) | 강력 권장 | 100% | TODO | 기본 |
| 분기 커버리지 (Branch) | 강력 권장 | 100% | TODO | 모든 분기 |
| MC/DC | 강력 권장 | TODO% | TODO | ASIL D 핵심 |
| 함수 커버리지 (Function) | 권장 | 100% | TODO | 보조 지표 |

---

## 5. 테스트 방법 (Test Methods)

ISO 26262-6 Table 9에 따른 단위 테스트 방법:

| 방법 | ASIL D 권장 | 적용 여부 | 비고 |
|------|------------|-----------|------|
| 요구사항 기반 테스트 | 강력 권장 | 적용 예정 | SWREQ 추적 |
| 인터페이스 테스트 | 강력 권장 | 적용 예정 | 경계값, 비정상 입력 |
| 결함 주입 테스트 | 강력 권장 | 적용 예정 | ASIL D 필수 |
| 리소스 사용 테스트 | 권장 | TODO | 메모리, CPU |
| 등가 분할 (Equivalence partitioning) | 강력 권장 | 적용 예정 | 입력 도메인 분할 |
| 경계값 분석 (Boundary value analysis) | 강력 권장 | 적용 예정 | 임계값 경계 |

---

## 6. 테스트 사양 (Test Specifications)

### 6.1 SWU-AEB-003: DecisionEngine 단위 테스트

#### TC-UD-001: 정상 TTC 계산

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-001 |
| **대상 단위** | SWU-AEB-003 (DecisionEngine) |
| **SWREQ** | SWREQ-AEB-001 |
| **목적** | TTC 계산 정확성 검증 |
| **사전 조건** | DecisionEngine 초기화 완료, 유효한 파라미터 설정 |
| **입력** | distance=30.0m, relativeVelocity=-15.0m/s, egoSpeed=20.0m/s |
| **기대 출력** | TTC=2.0s, brakeRequired 및 targetDeceleration 정상 산출 |
| **판정 기준** | TTC 계산값 오차 ≤ 0.01s |

#### TC-UD-002: TTC 임계값 경계 테스트

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-002 |
| **대상 단위** | SWU-AEB-003 (DecisionEngine) |
| **SWREQ** | SWREQ-AEB-001 |
| **목적** | TTC 임계값 경계에서의 제동 판단 검증 |
| **입력 세트** | TTC = threshold - epsilon → 제동 요청, TTC = threshold + epsilon → 제동 미요청 |
| **판정 기준** | 경계값 기준 정확한 분기 동작 |

#### TC-UD-003: 이격 차량 (No Brake)

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-003 |
| **대상 단위** | SWU-AEB-003 (DecisionEngine) |
| **SWREQ** | SWREQ-AEB-001 |
| **목적** | 상대 속도 양수(이격) 시 제동 미발생 확인 |
| **입력** | distance=30.0m, relativeVelocity=+5.0m/s |
| **기대 출력** | brakeRequired=false |

#### TC-UD-004: 비정상 입력 — 음수 거리

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-004 |
| **대상 단위** | SWU-AEB-003 (DecisionEngine) |
| **SWREQ** | SWREQ-AEB-003 |
| **목적** | 비정상 센서 데이터(음수 거리) 시 방어 동작 검증 |
| **입력** | distance=-1.0m |
| **기대 출력** | 오류 플래그 설정, 안전 상태 전환 요청 |

#### TC-UD-005: 거리 0 입력 (충돌 임박)

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-005 |
| **대상 단위** | SWU-AEB-003 (DecisionEngine) |
| **SWREQ** | SWREQ-AEB-001 |
| **목적** | 거리 0 시 즉시 최대 감속 적용 확인 |
| **입력** | distance=0.0m, relativeVelocity=-10.0m/s |
| **기대 출력** | brakeRequired=true, targetDeceleration=최대 감속도 |

---

### 6.2 SWU-AEB-002: Communicator 단위 테스트

#### TC-UD-010: CAN 메시지 수신 및 디코딩

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-010 |
| **대상 단위** | SWU-AEB-002 (Communicator) |
| **SWREQ** | SWREQ-AEB-002 |
| **목적** | CAN raw 프레임 → SensorData 변환 정확성 |
| **사전 조건** | SignalCodec, CrcProvider 모킹 |
| **입력** | 유효한 CAN raw 프레임 (CRC 정상) |
| **기대 출력** | SensorData 필드값이 물리값과 일치 |

#### TC-UD-011: CRC 불일치 검출

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-011 |
| **대상 단위** | SWU-AEB-002 (Communicator) |
| **SWREQ** | SWREQ-AEB-004 |
| **목적** | CRC 불일치 시 데이터 폐기 및 오류 보고 확인 |
| **입력** | CRC가 틀린 CAN 프레임 |
| **기대 출력** | 데이터 폐기, 오류 카운터 증가 |

#### TC-UD-012: 센서 데이터 timeout

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-012 |
| **대상 단위** | SWU-AEB-002 (Communicator) |
| **SWREQ** | SWREQ-AEB-003 |
| **목적** | 센서 데이터 미수신 시 timeout 검출 |
| **입력** | CAN 수신 없이 timeout 시간 경과 |
| **기대 출력** | isDataFresh()=false, 안전 상태 전환 트리거 |

#### TC-UD-013: CAN 송신 실패 및 재전송

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-013 |
| **대상 단위** | SWU-AEB-002 (Communicator) |
| **SWREQ** | SWREQ-AEB-004 |
| **목적** | CAN 송신 실패 시 재전송 로직 검증 |
| **입력** | CAN 드라이버 송신 실패 모킹 (N회 연속) |
| **기대 출력** | N회 재시도 후 Fail-safe 전환 |

---

### 6.3 SWU-AEB-004: ConfigLoader 단위 테스트

#### TC-UD-020: 정상 설정 로드

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-020 |
| **대상 단위** | SWU-AEB-004 (ConfigLoader) |
| **SWREQ** | SWREQ-AEB-005 |
| **목적** | 유효한 config.ini 파싱 및 파라미터 로드 |
| **기대 출력** | 모든 파라미터가 정상 범위 내 값으로 로드됨 |

#### TC-UD-021: 범위 초과 파라미터 검출

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-021 |
| **대상 단위** | SWU-AEB-004 (ConfigLoader) |
| **SWREQ** | SWREQ-AEB-005 |
| **목적** | 범위 초과 파라미터 검출 시 기본 안전값 적용 |
| **입력** | ttc_threshold=-1.0 (범위 밖) |
| **기대 출력** | 기본 안전 파라미터 적용, 경고 로그 |

#### TC-UD-022: 설정 파일 미존재

| 항목 | 내용 |
|------|------|
| **Test ID** | TC-UD-022 |
| **대상 단위** | SWU-AEB-004 (ConfigLoader) |
| **SWREQ** | SWREQ-AEB-005 |
| **목적** | 설정 파일 부재 시 안전 기본값 로드 |
| **입력** | 존재하지 않는 파일 경로 |
| **기대 출력** | getDefaultSafeParams() 반환 |

---

## 7. 결함 주입 테스트 (Fault Injection Tests)

| Test ID | 대상 | 주입 결함 | 기대 동작 | SWREQ |
|---------|------|-----------|-----------|-------|
| TC-FI-001 | Communicator | CAN 프레임 CRC 비트 반전 | 데이터 폐기 | SWREQ-AEB-004 |
| TC-FI-002 | Communicator | 센서 메시지 중단 (timeout 유발) | 안전 상태 전환 | SWREQ-AEB-003 |
| TC-FI-003 | DecisionEngine | NaN/Inf 센서 값 주입 | 방어 로직 작동 | SWREQ-AEB-003 |
| TC-FI-004 | ConfigLoader | config.ini 손상 (잘못된 형식) | 기본 안전값 적용 | SWREQ-AEB-005 |
| TC-FI-005 | TODO | CAN 드라이버 레벨 오류 | Fail-safe 전환 | SWREQ-AEB-004 |

---

## 8. 테스트 결과 요약 (Test Results Summary)

| 단위 | 총 TC 수 | Pass | Fail | N/A | 커버리지 (Stmt/Branch/MC/DC) |
|------|----------|------|------|-----|-------------------------------|
| DecisionEngine | TODO | TODO | TODO | TODO | TODO / TODO / TODO |
| Communicator | TODO | TODO | TODO | TODO | TODO / TODO / TODO |
| ConfigLoader | TODO | TODO | TODO | TODO | TODO / TODO / TODO |
| SafetyMonitor | TODO | TODO | TODO | TODO | TODO / TODO / TODO |
| AEBControlMain | TODO | TODO | TODO | TODO | TODO / TODO / TODO |

---

## 9. TODO 항목

- [ ] 테스트 프레임워크 선정 및 환경 구축 (Google Test/Mock 등)
- [ ] 커버리지 도구 선정 및 CI 연동
- [ ] DecisionEngine 테스트 케이스 확장 (등가 분할, 추가 경계값)
- [ ] SafetyMonitor 단위 테스트 케이스 작성
- [ ] AEBControlMain 통합 초기화/종료 단위 테스트 작성
- [ ] 결함 주입 테스트 상세 시나리오 확정
- [ ] MC/DC 커버리지 목표 수치 확정
- [ ] 호스트 vs 타겟 테스트 실행 전략 확정
- [ ] 테스트 결과 자동 리포팅 체계 구축
- [ ] 테스트 리뷰 수행 및 기록
