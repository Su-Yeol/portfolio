# AEB Control — 소프트웨어 안전 요구사항 (Software Safety Requirements)

> ISO 26262-6:2018, Clause 6 — Specification of software safety requirements

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-REQ-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 6 |
| **ASIL 등급** | ASIL D |
| **대상 HW** | NXP S32G (aarch64) |
| **작성일** | 2026-04-06 |
| **작성자** | SuYeol Kim |
| **검토자** | <!-- 대기 중: 검토자 지정 후 갱신 --> |
| **승인자** | <!-- 대기 중: 승인자 지정 후 갱신 --> |
| **버전** | 1.0 |
| **상태** | Released |

---

## 1. 목적 (Purpose)

본 문서는 AEB Control 모듈의 소프트웨어 안전 요구사항을 정의한다.
ISO 26262-6 Clause 6에 따라 기술 안전 요구사항(TSR)으로부터 도출된 소프트웨어 안전 요구사항을 기술하며,
ASIL D 등급에 적합한 요구사항 명세를 수립한다.

---

## 2. 적용 범위 (Scope)

- AEB 런타임 모듈: 장애물 감지 → 긴급 제동 판단 → CAN 명령 전송
- 빌드 아티팩트: `./build.sh` → `build/run_aeb`
- 의존성: `../common/src/` (CanSpecProvider, CrcProvider, SignalCodec), `../common/include/`
- 설정: `conf/config.ini` → `../../config/aeb-control.ini`

---

## 3. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-ARCH-001 | AEB SW Architecture | Part 6, Clause 7 |
| SW-AEB-UD-001 | AEB SW Unit Design | Part 6, Clause 8 |
| SW-AEB-UT-001 | AEB SW Unit Test | Part 6, Clause 9 |
| SW-AEB-IT-001 | AEB SW Integration Test | Part 6, Clause 10 |
| SW-AEB-VR-001 | AEB SW Verification Report | Part 6, Clause 11 |
| <!-- 대기 중: TSC 문서 확정 후 갱신 --> | Technical Safety Concept (TSC) | 상위 기술 안전 개념 |
| <!-- 대기 중: 시스템 요구사항 확정 후 갱신 --> | System Safety Requirements | 시스템 안전 요구사항 |

---

## 4. 소프트웨어 안전 요구사항 도출 (Derivation of SW Safety Requirements)

### 4.1 기술 안전 요구사항과의 추적성 (Traceability to TSR)

| TSR ID | TSR 내용 (요약) | SW 요구사항 ID | ASIL |
|--------|-----------------|----------------|------|
| TSR-AEB-001 (대기 중: TSR 문서 확정 후 갱신) | 전방 장애물 감지 시 자동 긴급 제동 실행 | SWREQ-AEB-001 | D |
| TSR-AEB-002 (대기 중: TSR 문서 확정 후 갱신) | 제동 명령 전달 시 최대 지연 시간 준수 | SWREQ-AEB-002 | D |
| TSR-AEB-003 (대기 중: TSR 문서 확정 후 갱신) | 센서 데이터 이상 시 안전 상태 전환 | SWREQ-AEB-003 | D |
| TSR-AEB-004 (대기 중: TSR 문서 확정 후 갱신) | CAN 통신 오류 시 Fail-safe 동작 | SWREQ-AEB-004 | D |

---

## 5. 소프트웨어 안전 요구사항 명세 (SW Safety Requirements Specification)

### SWREQ-AEB-001: 긴급 제동 판단 (Emergency Braking Decision)

| 항목 | 내용 |
|------|------|
| **ID** | SWREQ-AEB-001 |
| **ASIL** | D |
| **설명** | 전방 센서 데이터(레이더, 카메라 퓨전 결과)를 수신하여 TTC(Time-To-Collision) 기반 긴급 제동 여부를 판단한다. |
| **입력** | 장애물 거리, 상대 속도, 자차 속도, 센서 유효성 플래그 |
| **출력** | 제동 요청 플래그, 목표 감속도 |
| **타이밍** | 판단 주기: ≤ 20ms 이내 |
| **안전 메커니즘** | 센서 데이터 유효성 검증 후 판단 수행; 유효하지 않으면 SWREQ-AEB-003 적용 |
| **검증 방법** | 단위 테스트, HIL 시뮬레이션 |

### SWREQ-AEB-002: 제동 명령 전달 지연 (Braking Command Latency)

| 항목 | 내용 |
|------|------|
| **ID** | SWREQ-AEB-002 |
| **ASIL** | D |
| **설명** | 긴급 제동 판단 결과가 CAN 버스를 통해 브레이크 액추에이터에 전달되기까지의 E2E 지연 시간을 보장한다. |
| **최대 지연** | ≤ 50ms (센서 입력 → CAN 출력 기준) |
| **CAN 인터페이스** | CanSpecProvider를 통한 CAN 메시지 인코딩, CrcProvider를 통한 CRC 검증 |
| **안전 메커니즘** | Watchdog 타이머 기반 deadline 모니터링 |
| **검증 방법** | 타이밍 분석, 통합 테스트 |

### SWREQ-AEB-003: 센서 데이터 이상 처리 (Sensor Data Fault Handling)

| 항목 | 내용 |
|------|------|
| **ID** | SWREQ-AEB-003 |
| **ASIL** | D |
| **설명** | 센서 데이터가 유효하지 않거나 timeout 발생 시, 안전 상태(Safe State)로 전환한다. |
| **조건** | 센서 데이터 미수신 (<!-- 대기 중: timeout 임계값 확정 후 갱신 --> ms 초과), CRC 오류, 범위 초과 값 |
| **안전 상태** | 대기 중: 정의 필요 (예: 최대 감속 적용 또는 드라이버 경고 후 제어 이관) |
| **검증 방법** | 결함 주입 테스트(Fault Injection), 단위 테스트 |

### SWREQ-AEB-004: CAN 통신 오류 처리 (CAN Communication Error Handling)

| 항목 | 내용 |
|------|------|
| **ID** | SWREQ-AEB-004 |
| **ASIL** | D |
| **설명** | CAN 버스를 통한 제동 명령 전송 실패 시, 재전송 및 Fail-safe 로직을 수행한다. |
| **재전송** | 최대 3회 재시도 후 Fail-safe 전환 |
| **Fail-safe** | 대기 중: 정의 필요 (예: 독립 제동 경로 활성화, 운전자 경고) |
| **SignalCodec** | SignalCodec을 통한 메시지 직렬화/역직렬화 정합성 보장 |
| **검증 방법** | 통합 테스트, 결함 주입 테스트 |

### SWREQ-AEB-005: 설정 파일 무결성 (Configuration Integrity)

| 항목 | 내용 |
|------|------|
| **ID** | SWREQ-AEB-005 |
| **ASIL** | D |
| **설명** | `config.ini` 로드 시 파라미터 범위 검증 및 무결성 확인을 수행한다. |
| **검증 항목** | TTC 임계값, 감속도 한계, 타이밍 파라미터 범위 |
| **오류 시** | 기본 안전 파라미터(default safe parameters)로 동작 |
| **검증 방법** | 단위 테스트, 코드 리뷰 |

---

## 6. 비기능 안전 요구사항 (Non-Functional Safety Requirements)

| ID | 항목 | 내용 | ASIL |
|----|------|------|------|
| SWREQ-AEB-NF-001 | 실행 주기 | 메인 루프 실행 주기: ≤ 10ms | D |
| SWREQ-AEB-NF-002 | 메모리 사용 | 동적 메모리 할당 금지 (런타임 중) | D |
| SWREQ-AEB-NF-003 | WCET | Worst-Case Execution Time: <!-- 대기 중: 타겟 보드 검증 후 갱신 --> | D |
| SWREQ-AEB-NF-004 | 코드 커버리지 | 구문 커버리지 100%, 분기 커버리지 100%, MC/DC 커버리지 ≥95% (ASIL D) | D |
| SWREQ-AEB-NF-005 | 코딩 표준 | MISRA C++:2008 준수 (clang-tidy 기반 검증) | D |

---

## 7. 요구사항 속성 요약 (Requirements Attribute Summary)

| 요구사항 ID | ASIL | 상태 | 검증 방법 | 추적성 |
|-------------|------|------|-----------|--------|
| SWREQ-AEB-001 | D | Released | UT, HIL | TSR-AEB-001 (대기 중: 추적성 확정) |
| SWREQ-AEB-002 | D | Released | Timing, IT | TSR-AEB-002 (대기 중: 추적성 확정) |
| SWREQ-AEB-003 | D | Released | FI, UT | TSR-AEB-003 (대기 중: 추적성 확정) |
| SWREQ-AEB-004 | D | Released | IT, FI | TSR-AEB-004 (대기 중: 추적성 확정) |
| SWREQ-AEB-005 | D | Released | UT, Review | TSR-AEB-005 (대기 중: 추적성 확정) |

---

## 8. TODO 항목

- [ ] TSR(기술 안전 요구사항) 문서 확정 후 추적성 테이블 완성
- [x] 타이밍 요구사항 수치 확정 — 판단 주기 ≤20ms, E2E ≤50ms, 메인 루프 ≤10ms
- [ ] 안전 상태(Safe State) 정의 확정
- [ ] Fail-safe 전략 상세화
- [x] 코드 커버리지 목표치 확정 — Statement 100%, Branch 100%, MC/DC ≥95%
- [ ] 센서 퓨전 인터페이스 상세 명세 추가
- [ ] 요구사항 ID를 요구사항 관리 도구(예: DOORS, Polarion)에 등록
- [ ] 검토 및 승인 절차 수행
