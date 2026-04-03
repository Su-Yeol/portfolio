# Software Safety Requirements Specification

## 소프트웨어 안전 요구사항 명세서

---

| 항목 (Field)          | 내용 (Value)                                          |
|-----------------------|-------------------------------------------------------|
| **Document ID**       | SW-ADS-REQ-001                                        |
| **Version**           | 0.1 Draft                                             |
| **ISO 26262 Reference** | Part 6, Clause 6 — Specification of software safety requirements |
| **ASIL Scope**        | ASIL B ~ ASIL D (모듈별 상이)                          |
| **Project**           | Autonomous Driving Stack (자율주행 스택)                |
| **Target HW**         | NXP S32G (aarch64)                                    |
| **Author**            | <!-- TODO: 작성자 이름 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 -->                             |
| **Approval Date**     | <!-- TODO: 승인일자 (YYYY-MM-DD) -->                   |
| **Classification**    | Confidential                                          |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 6에 따라 Autonomous Driving Stack 프로젝트의
소프트웨어 안전 요구사항을 정의한다. 기술 안전 요구사항(TSR)으로부터 도출된
소프트웨어 안전 요구사항(SW Safety Requirements)을 프로젝트 수준에서 명세하며,
각 모듈(aeb-control, control-module, decision-module, remote-control, common)에
대한 요구사항 할당을 포함한다.

<!-- TODO: 프로젝트 배경 및 차량 시스템 컨텍스트 추가 기술 -->

## 2. 적용 범위 (Scope)

- **대상 시스템**: Autonomous Driving Stack (AEB, 제어, 판단, 원격제어 기능)
- **대상 모듈**: aeb-control, control-module, decision-module (K-City / Gateway 변형), remote-control, common
- **대상 HW 플랫폼**: NXP S32G (aarch64)
- **통신 인터페이스**: CAN Bus (CAN Gateway 연동)
- **빌드 구성**: Module-local builds + project-level decision builds, `USE_PRIVATE_IMPL` flag 적용

## 3. 참조 문서 (References)

| 문서 ID         | 문서명                                      | 비고              |
|-----------------|---------------------------------------------|-------------------|
| <!-- TODO -->   | Technical Safety Concept (기술 안전 개념서)   | 상위 TSR 문서      |
| <!-- TODO -->   | System Architecture Document                 | 시스템 아키텍처    |
| SW-ADS-ARCH-001 | Software Architecture Description            | 본 프로젝트 아키텍처 |
| <!-- TODO -->   | Hardware-Software Interface Specification    | HSI 문서           |
| <!-- TODO -->   | Safety Plan                                  | 안전 계획서        |

## 4. 용어 및 약어 (Terms and Abbreviations)

| 약어            | 정의                                                       |
|-----------------|------------------------------------------------------------|
| AEB             | Autonomous Emergency Braking (자동 긴급 제동)               |
| ASIL            | Automotive Safety Integrity Level                           |
| CAN             | Controller Area Network                                     |
| TSR             | Technical Safety Requirement (기술 안전 요구사항)            |
| SWSR            | Software Safety Requirement (소프트웨어 안전 요구사항)       |
| FTTI            | Fault Tolerant Time Interval (고장 허용 시간 간격)           |
| USE_PRIVATE_IMPL| 안전 필수(safety-critical) 구현부 분리를 위한 빌드 플래그     |

## 5. 소프트웨어 안전 요구사항 도출 기준 (Derivation Criteria)

ISO 26262 Part 6, Clause 6.4.1에 따라, 소프트웨어 안전 요구사항은 다음으로부터 도출된다:

1. 기술 안전 요구사항 (TSR) — 시스템 수준에서 할당된 안전 요구사항
2. 하드웨어-소프트웨어 인터페이스 명세 (HSI)
3. 시스템 설계 제약사항 (NXP S32G 플랫폼 제약 포함)
4. 안전 분석 결과 (FMEA, FTA 등)

<!-- TODO: TSR 문서로부터 도출 근거 추적 매트릭스 작성 -->

## 6. 소프트웨어 안전 요구사항 (Software Safety Requirements)

### 6.1 AEB Control 모듈 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-AEB-001     | D    | AEB 제어 모듈은 전방 충돌 감지 시 100ms 이내 제동 명령을 생성해야 한다 | <!-- TODO --> | Test / Analysis |
| SWSR-AEB-002     | D    | AEB 모듈 내부 고장 감지 시 안전 상태(safe state)로 전환해야 한다       | <!-- TODO --> | Test            |
| SWSR-AEB-003     | D    | CAN 통신 타임아웃 발생 시 비상 제동을 수행해야 한다                    | <!-- TODO --> | Test            |
| <!-- TODO: 추가 AEB 요구사항 작성 --> | | | | |

### 6.2 Control Module 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-CTL-001     | C    | 조향/가감속 제어 명령은 지정된 주기(cycle time) 내에 CAN 메시지로 전송되어야 한다 | <!-- TODO --> | Test |
| SWSR-CTL-002     | C    | 제어 명령 범위(range)는 물리적 구동기 한계를 초과하지 않아야 한다        | <!-- TODO --> | Test / Review  |
| <!-- TODO: 추가 Control 요구사항 작성 --> | | | | |

### 6.3 Decision Module 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-DEC-001     | C    | 판단 모듈은 경로 계획 결과를 주기적으로 제어 모듈에 전달해야 한다       | <!-- TODO --> | Test            |
| SWSR-DEC-002     | B    | K-City 변형과 Gateway 변형 간 인터페이스 호환성을 보장해야 한다         | <!-- TODO --> | Test / Review   |
| SWSR-DEC-003     | C    | CAN Gateway를 통한 외부 데이터 수신 시 유효성 검증을 수행해야 한다      | <!-- TODO --> | Test            |
| <!-- TODO: 추가 Decision 요구사항 작성 --> | | | | |

### 6.4 Remote Control 모듈 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-RMT-001     | B    | 원격 제어 통신 단절 시 자율주행 모드로 복귀하거나 안전 정지해야 한다    | <!-- TODO --> | Test            |
| SWSR-RMT-002     | B    | 원격 명령의 인증 및 무결성 검증을 수행해야 한다                        | <!-- TODO --> | Test / Review   |
| <!-- TODO: 추가 Remote Control 요구사항 작성 --> | | | | |

### 6.5 Common 모듈 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-CMN-001     | D    | 공통 라이브러리 함수는 ASIL D 수준의 코딩 가이드라인을 준수해야 한다   | <!-- TODO --> | Review          |
| SWSR-CMN-002     | C    | CAN 메시지 파싱 공통 함수는 잘못된 DLC/ID에 대해 방어적 처리를 해야 한다 | <!-- TODO --> | Test           |
| <!-- TODO: 추가 Common 요구사항 작성 --> | | | | |

### 6.6 USE_PRIVATE_IMPL 관련 안전 요구사항

| 요구사항 ID      | ASIL | 요구사항 설명                                               | 상위 TSR ID    | 검증 방법       |
|------------------|------|-------------------------------------------------------------|----------------|-----------------|
| SWSR-PVT-001     | D    | `USE_PRIVATE_IMPL` 활성화 시 안전 필수 구현부가 빌드에 포함되어야 한다 | <!-- TODO --> | Review / Test   |
| SWSR-PVT-002     | D    | Private 구현과 Public stub 간 인터페이스 일관성을 보장해야 한다         | <!-- TODO --> | Review          |
| <!-- TODO: 추가 USE_PRIVATE_IMPL 관련 요구사항 작성 --> | | | | |

## 7. 비기능 안전 요구사항 (Non-functional Safety Requirements)

### 7.1 타이밍 요구사항

| 항목                | 요구값             | 비고                              |
|---------------------|--------------------|-----------------------------------|
| AEB 응답 시간        | <!-- TODO --> ms   | 감지 → 제동 명령 생성              |
| 제어 루프 주기       | <!-- TODO --> ms   | 조향/가감속 제어 주기              |
| CAN 메시지 전송 주기 | <!-- TODO --> ms   | 모듈 간 CAN 통신 주기             |
| FTTI                | <!-- TODO --> ms   | 고장 허용 시간 간격               |

### 7.2 리소스 제약사항 (NXP S32G)

| 항목                | 제한값              | 비고                              |
|---------------------|---------------------|-----------------------------------|
| CPU 사용률           | <!-- TODO --> %     | 안전 마진 포함                    |
| 메모리 사용량        | <!-- TODO --> MB    | Heap + Stack                      |
| 스택 크기            | <!-- TODO --> KB    | 태스크별 최대 스택 크기            |

<!-- TODO: NXP S32G 플랫폼 특성에 따른 추가 제약사항 기술 -->

## 8. 요구사항 추적 매트릭스 (Traceability Matrix)

<!-- TODO: 전체 TSR → SWSR 추적 매트릭스 작성 -->

| TSR ID         | SWSR ID(s)                  | 모듈 할당          | ASIL | 상태      |
|----------------|-----------------------------|--------------------|------|-----------|
| <!-- TODO -->  | SWSR-AEB-001, SWSR-AEB-002 | aeb-control        | D    | Draft     |
| <!-- TODO -->  | SWSR-CTL-001               | control-module     | C    | Draft     |
| <!-- TODO -->  | SWSR-DEC-001               | decision-module    | C    | Draft     |
| <!-- TODO -->  | SWSR-RMT-001               | remote-control     | B    | Draft     |

## 9. 안전 요구사항 속성 (Requirement Attributes)

각 소프트웨어 안전 요구사항은 다음 속성을 포함해야 한다:

- **고유 ID**: 요구사항 식별자
- **ASIL 등급**: 해당 요구사항의 ASIL 등급
- **상위 TSR 연결**: 도출 근거가 되는 기술 안전 요구사항
- **할당 모듈**: 요구사항이 구현될 소프트웨어 모듈
- **검증 방법**: Test, Review, Analysis, Simulation 중 택일 또는 복수
- **상태**: Draft / Reviewed / Approved / Implemented / Verified

<!-- TODO: 요구사항 관리 도구(예: DOORS, Polarion) 연동 방안 기술 -->

## 10. 변경 이력 (Change History)

| Version | Date       | Author        | Description           |
|---------|------------|---------------|-----------------------|
| 0.1     | <!-- TODO --> | <!-- TODO --> | Initial draft 작성    |
