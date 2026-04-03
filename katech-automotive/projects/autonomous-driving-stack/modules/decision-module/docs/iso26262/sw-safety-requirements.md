# Software Safety Requirements Specification

## ISO 26262 Part 6, Clause 6 — 소프트웨어 안전 요구사항 명세

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-REQ-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 6 (Software safety requirements specification) |
| **ASIL** | B–D (경로 계획 및 장애물 대응 결정에 따라 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | TODO: 작성자 |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 0.1 (Draft) |
| **Date** | TODO: YYYY-MM-DD |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 6에 따라 Decision Module의 소프트웨어 안전 요구사항을 정의한다. Decision Module은 자율주행 스택의 핵심 판단 모듈로서, 센서 입력(GPS, LiDAR, 경로 계획, 보행자 감지)을 기반으로 주행 결정을 수행한다.

### 1.1 적용 범위 (Scope)

- **K-City Variant** (`run_kcity`): 자율주행 시험도시(K-City) 환경에서의 주행 판단
- **CAN Gateway Variant** (`run_gateway`): CAN 통신 기반 차량 제어 게이트웨이

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-ARCH-001 | Software Architecture Description | 본 모듈 아키텍처 문서 |
| TODO | System-level Safety Requirements | 시스템 수준 안전 요구사항 |
| TODO | Technical Safety Concept | 기술 안전 개념 |
| ISO 26262:2018 Part 6 | Product development at the software level | 국제 표준 |

---

## 3. 소프트웨어 안전 요구사항 도출 (Derivation of SW Safety Requirements)

### 3.1 시스템 안전 요구사항으로부터의 도출

ISO 26262 Part 6, Clause 6.4.1에 따라, 시스템 수준 안전 요구사항(Technical Safety Requirements)으로부터 소프트웨어 안전 요구사항을 도출한다.

> TODO: 시스템 안전 요구사항 문서와의 Traceability Matrix를 작성할 것

### 3.2 ASIL 분해 (ASIL Decomposition)

| 기능 영역 | 할당 ASIL | 분해 여부 | 비고 |
|-----------|----------|----------|------|
| 경로 추종 판단 (Path Following) | TODO: ASIL C/D | TODO | 경로 이탈 시 직접적 위험 |
| 장애물 대응 (Obstacle Response) | TODO: ASIL D | TODO | 보행자 충돌 위험 |
| GPS 신호 처리 | TODO: ASIL B | TODO | 위치 정보 신뢰도 |
| LiDAR 데이터 처리 (Ibeo) | TODO: ASIL C | TODO | 장애물 감지 |
| CAN Gateway 통신 | TODO: ASIL B/C | TODO | 제어 명령 전달 |

---

## 4. 소프트웨어 안전 요구사항 (Software Safety Requirements)

### 4.1 기능 안전 요구사항 (Functional Safety Requirements)

#### SWR-DEC-001: 경로 계획 데이터 수신 및 검증

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-001 |
| **설명** | Decision Module은 경로 계획 모듈로부터 수신한 경로 데이터의 유효성을 검증해야 한다. |
| **ASIL** | TODO |
| **검증 방법** | 단위 테스트, 통합 테스트 |
| **Variant** | K-City, CAN Gateway (공통) |
| **상위 요구사항** | TODO: TSR-xxx |

#### SWR-DEC-002: 보행자 감지 시 긴급 판단

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-002 |
| **설명** | 보행자 감지 시스템으로부터 감지 이벤트를 수신하면, Decision Module은 TODO: xx ms 이내에 감속/정지 판단을 수행해야 한다. |
| **ASIL** | TODO |
| **검증 방법** | 단위 테스트, HIL 테스트 |
| **Variant** | K-City |
| **상위 요구사항** | TODO: TSR-xxx |

#### SWR-DEC-003: GPS 신호 유실 대응

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-003 |
| **설명** | GPS 신호가 TODO: xx초 이상 유실될 경우, Decision Module은 안전 상태(Safe State)로 전환해야 한다. |
| **ASIL** | TODO |
| **검증 방법** | 통합 테스트, Fault Injection |
| **Variant** | K-City, CAN Gateway (공통) |
| **상위 요구사항** | TODO: TSR-xxx |

#### SWR-DEC-004: LiDAR(Ibeo) 장애물 감지 처리

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-004 |
| **설명** | LiDAR(Ibeo) 센서로부터 수신한 장애물 데이터를 기반으로 회피/감속 판단을 수행해야 한다. 처리 지연은 TODO: xx ms를 초과하지 않아야 한다. |
| **ASIL** | TODO |
| **검증 방법** | 단위 테스트, 통합 테스트 |
| **Variant** | K-City |
| **상위 요구사항** | TODO: TSR-xxx |

#### SWR-DEC-005: CAN Gateway 메시지 무결성

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-005 |
| **설명** | CAN Gateway를 통한 제어 명령 전송 시 CRC 검증을 수행하여 메시지 무결성을 보장해야 한다. (`source_gateway/CRC.cpp` 참조) |
| **ASIL** | TODO |
| **검증 방법** | 단위 테스트 |
| **Variant** | CAN Gateway |
| **상위 요구사항** | TODO: TSR-xxx |

#### SWR-DEC-006: Variant 간 공통 안전 메커니즘

| 항목 | 내용 |
|------|------|
| **ID** | SWR-DEC-006 |
| **설명** | K-City 및 CAN Gateway variant 공통으로 사용되는 common 라이브러리(`../common/src/`, `../common/include/`)의 안전 관련 함수는 동일한 안전 수준을 충족해야 한다. |
| **ASIL** | TODO |
| **검증 방법** | 코드 리뷰, 단위 테스트 |
| **Variant** | 공통 |
| **상위 요구사항** | TODO: TSR-xxx |

> TODO: 추가 소프트웨어 안전 요구사항을 시스템 안전 요구사항으로부터 도출하여 작성할 것

### 4.2 비기능 안전 요구사항 (Non-functional Safety Requirements)

| ID | 설명 | ASIL | 비고 |
|----|------|------|------|
| SWR-DEC-NF-001 | 주행 판단 주기: TODO: xx ms 이내 | TODO | Real-time constraint |
| SWR-DEC-NF-002 | 메모리 사용량: TODO: xx MB 이내 (NXP S32G 제약) | TODO | Resource constraint |
| SWR-DEC-NF-003 | CPU 점유율: TODO: xx% 이내 | TODO | Resource constraint |
| SWR-DEC-NF-004 | 설정 파일(`config.ini`) 로드 실패 시 기본 안전값 적용 | TODO | Fallback 동작 |

---

## 5. 안전 요구사항 속성 (Safety Requirements Attributes)

각 요구사항은 다음 속성을 포함해야 한다 (ISO 26262 Part 6, Clause 6.4.3):

- 고유 식별자 (Unique ID)
- ASIL 등급
- 상위 요구사항 추적성 (Traceability to TSR)
- 검증 방법 (Verification method)
- 적용 Variant (K-City / CAN Gateway / 공통)
- 상태 (Draft / Reviewed / Approved)

> TODO: 요구사항 관리 도구(DOORS, Polarion 등)와 연동 방안 기술

---

## 6. 요구사항 추적성 매트릭스 (Traceability Matrix)

| SW Safety Req | System TSR | Architecture Element | Unit Test | Integration Test |
|---------------|-----------|---------------------|-----------|-----------------|
| SWR-DEC-001 | TODO | TODO | TODO | TODO |
| SWR-DEC-002 | TODO | TODO | TODO | TODO |
| SWR-DEC-003 | TODO | TODO | TODO | TODO |
| SWR-DEC-004 | TODO | TODO | TODO | TODO |
| SWR-DEC-005 | TODO | TODO | TODO | TODO |
| SWR-DEC-006 | TODO | TODO | TODO | TODO |

---

## 7. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | TODO | TODO | Initial draft |
