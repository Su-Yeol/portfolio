# Software Safety Requirements Specification (소프트웨어 안전 요구사항 명세서)

## Document Metadata

| 항목 (Field)           | 내용 (Value)                                                        |
|------------------------|----------------------------------------------------------------------|
| Document ID            | SW-ARA-REQ-001                                                       |
| Version                | 1.0                                                                  |
| ISO 26262 Reference    | Part 6, Clause 6 — Specification of Software Safety Requirements     |
| ASIL Scope             | ASIL-B / ASIL-D (component-dependent)                                |
| Project                | Adaptive AUTOSAR Platform (NXP S32G / Axon)                         |
| Author                 | SuYeol Kim                                                           |
| Reviewer               | <!-- TODO: 검토자 이름 기입 -->                                      |
| Approval Date          | 2026-04-06                                                           |
| Status                 | Draft                                                                |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 6에 따라 Adaptive AUTOSAR 플랫폼의 소프트웨어 안전 요구사항을 정의한다. Technical Safety Requirements (TSR)로부터 도출된 Software Safety Requirements (SwRS)를 명세하여 소프트웨어 아키텍처 설계 및 구현의 기반을 제공한다.

<!-- TODO: 프로젝트 고유 목적 및 범위를 상세 기술 -->

## 2. 적용 범위 (Scope)

본 문서는 다음 Adaptive AUTOSAR 플랫폼 구성요소에 적용된다:

| Component | Description                          | Target ASIL |
|-----------|--------------------------------------|-------------|
| EXEC      | Execution Management                 | <!-- TODO: ASIL 등급 기입 --> |
| PHM       | Platform Health Management           | <!-- TODO: ASIL 등급 기입 --> |
| UCM       | Update and Configuration Management  | <!-- TODO: ASIL 등급 기입 --> |
| COM       | Communication Management             | <!-- TODO: ASIL 등급 기입 --> |
| IAM       | Identity and Access Management       | <!-- TODO: ASIL 등급 기입 --> |
| ara-api   | ARA Runtime API layer                | <!-- TODO: ASIL 등급 기입 --> |

Target Hardware:
- **Primary**: NXP S32G (aarch64) — Vehicle gateway / domain controller
- **Secondary**: Axon — Application processor target

<!-- TODO: 각 구성요소별 안전 관련 기능(safety-related function) 여부를 명확히 구분할 것 -->

## 3. 참조 문서 (Input References)

<!-- TODO: 아래 문서 ID를 실제 프로젝트 문서 ID로 갱신 -->

| Ref ID | Document Title                                    | Version |
|--------|---------------------------------------------------|---------|
| REF-01 | System-level Technical Safety Concept              | <!-- TODO --> |
| REF-02 | Technical Safety Requirements (TSR)                | <!-- TODO --> |
| REF-03 | Hardware-Software Interface Specification (HSI)    | <!-- TODO --> |
| REF-04 | AUTOSAR Adaptive Platform Specification (AUTOSAR R22-11 이상) | <!-- TODO --> |
| REF-05 | NXP S32G Reference Manual                         | <!-- TODO --> |

## 4. 안전 요구사항 도출 방법론 (Derivation Methodology)

### 4.1 도출 프로세스

TSR에서 SwRS로의 도출은 다음 절차를 따른다:

1. TSR 분석 및 소프트웨어 할당 (TSR Allocation to SW)
2. HSI 요구사항 반영 (HSI Consideration)
3. 안전 메커니즘 식별 (Safety Mechanism Identification)
4. ASIL 분해 적용 여부 검토 (ASIL Decomposition Review)
5. 요구사항 검증 기준 정의 (Verification Criteria Definition)

<!-- TODO: 프로젝트에 적용된 구체적 도출 방법 기술 -->

### 4.2 요구사항 속성 정의 (Requirement Attributes)

각 소프트웨어 안전 요구사항은 다음 속성을 포함한다:

- **SwRS ID**: 고유 식별자 (e.g., SwRS-EXEC-001)
- **관련 TSR**: 상위 TSR ID
- **ASIL 등급**: A / B / C / D / QM
- **안전 메커니즘**: 적용되는 안전 메커니즘 명시
- **검증 방법**: Review / Analysis / Simulation / Testing
- **상태**: Draft / Reviewed / Approved

## 5. 소프트웨어 안전 요구사항 목록 (Software Safety Requirements)

### 5.1 Execution Management (EXEC)

| SwRS ID        | Description (요구사항 설명)                                                | 관련 TSR | ASIL | 검증 방법 | 상태   |
|----------------|---------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-EXEC-001  | EXEC는 애플리케이션 프로세스의 비정상 종료를 감지하고 PHM에 보고하여야 한다. | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-EXEC-002  | EXEC는 Machine State 전환 시 정의된 timeout 내에 완료하여야 한다.           | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-EXEC-003  | <!-- TODO: 추가 요구사항 기입 -->                                          | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

<!-- TODO: EXEC 관련 안전 요구사항을 TSR 분석 후 완성할 것 -->

### 5.2 Platform Health Management (PHM)

| SwRS ID       | Description (요구사항 설명)                                                          | 관련 TSR | ASIL | 검증 방법 | 상태   |
|---------------|--------------------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-PHM-001  | PHM은 Supervision 대상 프로세스의 alive/deadline/logical supervision을 수행하여야 한다. | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-PHM-002  | PHM은 supervision 위반 감지 시 설정된 Recovery Action을 수행하여야 한다.                | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-PHM-003  | <!-- TODO: 추가 요구사항 기입 -->                                                     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

<!-- TODO: PHM 관련 안전 요구사항을 TSR 분석 후 완성할 것 -->

### 5.3 Update and Configuration Management (UCM)

| SwRS ID       | Description (요구사항 설명)                                                      | 관련 TSR | ASIL | 검증 방법 | 상태   |
|---------------|----------------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-UCM-001  | UCM은 소프트웨어 패키지 설치 실패 시 이전 버전으로 rollback할 수 있어야 한다.       | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-UCM-002  | UCM은 패키지 무결성 검증(authentication/integrity check)을 수행하여야 한다.        | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-UCM-003  | <!-- TODO: 추가 요구사항 기입 -->                                                 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

### 5.4 Communication Management (COM)

| SwRS ID       | Description (요구사항 설명)                                                  | 관련 TSR | ASIL | 검증 방법 | 상태   |
|---------------|------------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-COM-001  | COM은 SOME/IP 통신 시 E2E Protection을 적용할 수 있어야 한다.                 | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-COM-002  | COM은 통신 timeout 발생 시 상위 애플리케이션에 오류를 통지하여야 한다.          | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-COM-003  | <!-- TODO: 추가 요구사항 기입 -->                                             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

### 5.5 Identity and Access Management (IAM)

| SwRS ID       | Description (요구사항 설명)                                                      | 관련 TSR | ASIL | 검증 방법 | 상태   |
|---------------|----------------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-IAM-001  | IAM은 인가되지 않은 서비스 접근 요청을 차단하여야 한다.                            | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-IAM-002  | <!-- TODO: 추가 요구사항 기입 -->                                                 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

### 5.6 ARA Runtime API (ara-api)

| SwRS ID        | Description (요구사항 설명)                                                    | 관련 TSR | ASIL | 검증 방법 | 상태   |
|----------------|--------------------------------------------------------------------------------|----------|------|-----------|--------|
| SwRS-ARA-001   | ara-api는 호출자에게 안전 관련 오류 코드를 명확히 반환하여야 한다.                | <!-- TODO --> | <!-- TODO --> | Test | Draft |
| SwRS-ARA-002   | <!-- TODO: 추가 요구사항 기입 -->                                               | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | Draft |

## 6. 안전 메커니즘 요약 (Safety Mechanisms Summary)

<!-- TODO: 식별된 안전 메커니즘을 아래 표에 정리할 것 -->

| SM ID    | 안전 메커니즘 명칭                   | 적용 대상 Component | 감지 대상 Fault            | 관련 SwRS        |
|----------|--------------------------------------|----------------------|----------------------------|------------------|
| SM-001   | Process alive supervision            | EXEC, PHM            | Process hang/crash         | SwRS-PHM-001     |
| SM-002   | E2E Communication Protection         | COM                  | Data corruption/loss       | SwRS-COM-001     |
| SM-003   | SW Package integrity verification    | UCM                  | Corrupted update package   | SwRS-UCM-002     |
| SM-004   | <!-- TODO: 추가 안전 메커니즘 -->    | <!-- TODO -->        | <!-- TODO -->              | <!-- TODO -->    |

## 7. ASIL 분해 (ASIL Decomposition)

<!-- TODO: ASIL 분해가 적용되는 요구사항이 있을 경우 아래 표에 기록 -->

| 원래 ASIL | 분해 결과         | 적용 SwRS    | 근거 (Rationale)             |
|-----------|-------------------|--------------|-------------------------------|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO: 독립성 논거 기술 --> |

## 8. 요구사항 검증 기준 (Verification Criteria)

각 SwRS는 다음 중 하나 이상의 방법으로 검증한다 (ISO 26262-6 Table 1 참조):

| 검증 방법        | 설명                                              | 적용 ASIL 권고      |
|------------------|---------------------------------------------------|----------------------|
| Walk-through     | 비형식적 검토                                      | QM, ASIL-A           |
| Inspection       | 체계적 검토 (체크리스트 기반)                       | ASIL-A ~ ASIL-D      |
| Semi-formal verification | 모델 기반 분석                              | ASIL-C, ASIL-D       |
| Formal verification      | 수학적 증명                                 | ASIL-D (권고)        |
| Testing          | 단위/통합/시스템 테스트                             | All ASIL              |

<!-- TODO: 각 SwRS별 구체적 검증 기준 및 pass/fail criteria 정의 -->

## 9. 추적성 매트릭스 (Traceability Matrix)

<!-- TODO: 요구사항 관리 도구(예: DOORS, Polarion, Jama)와 연계하여 추적성 확보 -->

| TSR ID       | SwRS ID         | 아키텍처 요소 (SW-ARA-ARCH-001) | 단위 설계 (SW-ARA-UD-001) | 테스트 케이스 (SW-ARA-UT-001) |
|--------------|-----------------|----------------------------------|---------------------------|-------------------------------|
| <!-- TODO --> | SwRS-EXEC-001  | <!-- TODO -->                    | <!-- TODO -->             | <!-- TODO -->                 |
| <!-- TODO --> | SwRS-PHM-001   | <!-- TODO -->                    | <!-- TODO -->             | <!-- TODO -->                 |
| <!-- TODO --> | SwRS-COM-001   | <!-- TODO -->                    | <!-- TODO -->             | <!-- TODO -->                 |

## 10. References

| Document ID      | Title                                              |
|------------------|----------------------------------------------------|
| SW-ARA-ARCH-001  | Software Architecture Description                  |
| SW-ARA-UD-001    | Software Unit Design Specification                 |
| SW-ARA-UT-001    | Software Unit Test Specification                   |
| SW-ARA-IT-001    | Software Integration Test Specification            |
| SW-ARA-VR-001    | Software Verification Report                       |

---

## Revision History

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성   |
| 1.0     | 2026-04-06 | SuYeol Kim | 메타데이터 갱신 |
