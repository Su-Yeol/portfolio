# Software Unit Design Specification (소프트웨어 단위 설계 명세서)

## Document Metadata

| 항목 (Field)           | 내용 (Value)                                                        |
|------------------------|----------------------------------------------------------------------|
| Document ID            | SW-ARA-UD-001                                                        |
| Version                | 1.0                                                                  |
| ISO 26262 Reference    | Part 6, Clause 8 — Software Unit Design and Implementation           |
| ASIL Scope             | ASIL-B / ASIL-D (component-dependent)                                |
| Project                | Adaptive AUTOSAR Platform (NXP S32G / Axon)                         |
| Author                 | SuYeol Kim                                                           |
| Reviewer               | <!-- TODO: 검토자 이름 기입 -->                                      |
| Approval Date          | 2026-04-06                                                           |
| Status                 | Draft                                                                |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 8에 따라 Adaptive AUTOSAR 플랫폼의 소프트웨어 단위(SW Unit) 수준 설계를 기술한다. SW-ARA-ARCH-001에서 정의된 아키텍처 컴포넌트를 소프트웨어 단위로 분해하고, 각 단위의 상세 설계와 구현 지침을 제공한다.

## 2. 적용 범위 (Scope)

본 문서는 다음 Functional Cluster의 소프트웨어 단위 설계를 포함한다:

- EXEC (Execution Management)
- PHM (Platform Health Management)
- COM (Communication Management)
- UCM (Update and Configuration Management)
- IAM (Identity and Access Management)
- ara-api (ARA Runtime API)

구현 언어: **C++17** (ISO/IEC 14882:2017)
코딩 표준: MISRA C++:2008 (clang-tidy 기반 검증), .clang-format (Google-based) 적용

## 3. 참조 문서 (Input References)

| Ref ID | Document Title                                    | Document ID      |
|--------|---------------------------------------------------|------------------|
| REF-01 | Software Safety Requirements Specification         | SW-ARA-REQ-001   |
| REF-02 | Software Architecture Description                  | SW-ARA-ARCH-001  |
| REF-03 | C++ Coding Standard / Guidelines                   | <!-- TODO -->    |
| REF-04 | AUTOSAR Adaptive Platform API Specification        | <!-- TODO -->    |

## 4. 설계 원칙 및 코딩 가이드라인 (Design Principles and Coding Guidelines)

### 4.1 설계 원칙 (ISO 26262-6 Table 4)

| 설계 원칙 (Principle)                         | ASIL-B | ASIL-D | 적용 방법                                          |
|----------------------------------------------|--------|--------|-----------------------------------------------------|
| One entry and one exit point per function     | ++     | ++     | 모든 함수에서 단일 return point 유지                  |
| No dynamic objects after initialization       | ++     | ++     | Init phase 이후 heap allocation 금지                 |
| No unconditional jumps (goto)                 | ++     | ++     | goto 문 사용 금지                                    |
| Limited use of pointers                       | +      | ++     | Smart pointer 사용, raw pointer 최소화               |
| No implicit type conversions                  | +      | ++     | explicit 키워드, static_cast 사용                    |
| No recursion                                  | +      | ++     | 재귀 호출 금지 (stack overflow 방지)                  |
| Bounded loop execution                        | +      | ++     | 모든 루프에 최대 반복 횟수 보장                       |

Legend: ++ Highly recommended, + Recommended

<!-- TODO: 프로젝트 C++ 코딩 표준 문서를 별도 작성하고 여기에 참조 추가 -->

### 4.2 코딩 표준 적용

| 항목                        | 적용 표준/도구                                         |
|-----------------------------|--------------------------------------------------------|
| Coding Standard             | MISRA C++:2008 via clang-tidy, .clang-format (Google-based) |
| Static Analysis Tool        | cppcheck (Docker 내 실행), clang-tidy                  |
| Code Review Tool            | GitHub PR                                              |
| Code Complexity Metric      | Cyclomatic complexity <= 대기 중: 구현/측정 후 갱신    |

## 5. 소프트웨어 단위 식별 (SW Unit Identification)

### 5.1 단위 식별 기준

소프트웨어 단위는 다음 기준에 따라 식별한다:
- 독립적으로 테스트 가능한 최소 단위 (C++ class 또는 free function group)
- 단일 책임 원칙 (Single Responsibility Principle) 준수
- 명확한 인터페이스를 가진 모듈

### 5.2 단위 목록 (Unit Inventory)

#### EXEC — Execution Management

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-EXEC-001   | MachineStateManager          | <!-- TODO: 소스 파일 경로 -->         | SwRS-EXEC-002   | <!-- TODO --> |
| SWU-EXEC-002   | ProcessManager               | <!-- TODO: 소스 파일 경로 -->         | SwRS-EXEC-001   | <!-- TODO --> |
| SWU-EXEC-003   | ApplicationClient            | <!-- TODO: 소스 파일 경로 -->         | SwRS-EXEC-001   | <!-- TODO --> |
| SWU-EXEC-004   | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

#### PHM — Platform Health Management

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-PHM-001    | AliveSupervision             | <!-- TODO: 소스 파일 경로 -->         | SwRS-PHM-001    | <!-- TODO --> |
| SWU-PHM-002    | DeadlineSupervision          | <!-- TODO: 소스 파일 경로 -->         | SwRS-PHM-001    | <!-- TODO --> |
| SWU-PHM-003    | LogicalSupervision           | <!-- TODO: 소스 파일 경로 -->         | SwRS-PHM-001    | <!-- TODO --> |
| SWU-PHM-004    | RecoveryActionHandler        | <!-- TODO: 소스 파일 경로 -->         | SwRS-PHM-002    | <!-- TODO --> |
| SWU-PHM-005    | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

#### COM — Communication Management

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-COM-001    | SomeIpBinding                | <!-- TODO: 소스 파일 경로 -->         | SwRS-COM-001    | <!-- TODO --> |
| SWU-COM-002    | E2EProtectionWrapper         | <!-- TODO: 소스 파일 경로 -->         | SwRS-COM-001    | <!-- TODO --> |
| SWU-COM-003    | ServiceDiscovery             | <!-- TODO: 소스 파일 경로 -->         | SwRS-COM-002    | <!-- TODO --> |
| SWU-COM-004    | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

#### UCM — Update and Configuration Management

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-UCM-001    | PackageManager               | <!-- TODO: 소스 파일 경로 -->         | SwRS-UCM-001    | <!-- TODO --> |
| SWU-UCM-002    | IntegrityVerifier            | <!-- TODO: 소스 파일 경로 -->         | SwRS-UCM-002    | <!-- TODO --> |
| SWU-UCM-003    | RollbackController           | <!-- TODO: 소스 파일 경로 -->         | SwRS-UCM-001    | <!-- TODO --> |
| SWU-UCM-004    | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

#### IAM — Identity and Access Management

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-IAM-001    | AccessPolicyEngine           | <!-- TODO: 소스 파일 경로 -->         | SwRS-IAM-001    | <!-- TODO --> |
| SWU-IAM-002    | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

#### ara-api — ARA Runtime API

| Unit ID         | Unit Name                    | Source File(s)                        | 관련 SwRS       | ASIL   |
|-----------------|------------------------------|---------------------------------------|-----------------|--------|
| SWU-ARA-001    | ErrorCodeHandler             | <!-- TODO: 소스 파일 경로 -->         | SwRS-ARA-001    | <!-- TODO --> |
| SWU-ARA-002    | <!-- TODO: 추가 단위 -->     | <!-- TODO -->                         | <!-- TODO -->   | <!-- TODO --> |

## 6. 소프트웨어 단위 상세 설계 (Detailed Unit Design)

<!-- TODO: 각 단위에 대해 아래 템플릿을 복사하여 상세 설계를 작성할 것 -->

### 6.1 [Unit Template] SWU-EXEC-001: MachineStateManager

#### 6.1.1 개요 (Overview)

<!-- TODO: 단위의 목적과 책임을 간결하게 기술 -->

#### 6.1.2 인터페이스 (Interface)

```cpp
// TODO: 실제 클래스 인터페이스로 교체
class MachineStateManager {
public:
    /**
     * @brief 현재 Machine State를 반환한다.
     * @return 현재 MachineState 열거값
     */
    MachineState GetCurrentState() const;

    /**
     * @brief Machine State 전환을 요청한다.
     * @param target_state 목표 상태
     * @param timeout_ms 전환 timeout (ms)
     * @return ara::core::Result<void> 성공/실패
     * @safety SwRS-EXEC-002: timeout 내 전환 완료 보장
     */
    ara::core::Result<void> RequestStateTransition(
        MachineState target_state, uint32_t timeout_ms);

private:
    // TODO: 내부 멤버 변수 설계
};
```

#### 6.1.3 상태 다이어그램 (State Diagram)

<!-- TODO: Machine State 전환 상태 다이어그램 (PlantUML 또는 Mermaid) -->

```
[*] --> Startup
Startup --> Running : init complete
Running --> Shutdown : shutdown requested
Shutdown --> [*] : cleanup complete
Running --> Restart : restart requested
Restart --> Startup : restart initiated
```

#### 6.1.4 알고리즘 / 로직 (Algorithm / Logic)

<!-- TODO: 핵심 알고리즘 또는 처리 로직을 pseudo-code 또는 flow chart로 기술 -->

#### 6.1.5 에러 처리 (Error Handling)

| Error Condition              | Detection Method       | Response Action                    |
|-----------------------------|------------------------|------------------------------------|
| State transition timeout    | Timer expiry           | Report to PHM, force transition    |
| Invalid state request       | Parameter validation   | Return error code                  |
| <!-- TODO: 추가 에러 -->    | <!-- TODO -->          | <!-- TODO -->                      |

#### 6.1.6 리소스 사용 (Resource Usage)

| Resource        | Usage                                            |
|-----------------|--------------------------------------------------|
| Stack Size      | <!-- TODO: 예상 스택 사용량 (bytes) -->           |
| Heap Usage      | <!-- TODO: Init 시 할당량 -->                     |
| CPU Budget      | <!-- TODO: WCET 추정값 -->                        |
| Shared Resources| <!-- TODO: 공유 자원 목록 및 보호 메커니즘 -->    |

### 6.2 [Unit Template] SWU-PHM-001: AliveSupervision

<!-- TODO: 6.1과 동일한 구조로 상세 설계 작성 -->

#### 6.2.1 개요 (Overview)
<!-- TODO: Alive supervision의 목적, checkpoint 기반 동작 원리 기술 -->

#### 6.2.2 인터페이스 (Interface)
<!-- TODO: AliveSupervision 클래스 인터페이스 -->

#### 6.2.3 감시 파라미터 (Supervision Parameters)

| Parameter                 | Type     | Description                                    | Default       |
|--------------------------|----------|-------------------------------------------------|---------------|
| expectedAliveIndications | uint32_t | 기대 alive 신호 수                               | <!-- TODO --> |
| minMargin                | uint32_t | 최소 허용 마진                                    | <!-- TODO --> |
| maxMargin                | uint32_t | 최대 허용 마진                                    | <!-- TODO --> |
| aliveReferenceCycle      | uint32_t | 감시 주기 (ms)                                    | <!-- TODO --> |

#### 6.2.4 에러 처리 (Error Handling)
<!-- TODO: supervision 위반 시 처리 로직 -->

### 6.3 추가 단위 설계

<!-- TODO: 나머지 SWU에 대해서도 위와 동일한 구조로 상세 설계 섹션을 추가할 것 -->
<!-- 우선순위: 안전 관련(safety-related) 단위를 먼저 작성 -->

## 7. 구현 제약사항 (Implementation Constraints)

### 7.1 C++ 언어 서브셋 제한

<!-- TODO: 프로젝트에서 제한하는 C++ 기능 목록 -->

| 제한 항목                | 허용 여부 | 근거                                         |
|--------------------------|-----------|----------------------------------------------|
| Dynamic memory (new/delete) after init | 금지 | 런타임 메모리 고갈 방지                |
| Exceptions (throw/catch) | <!-- TODO: 허용/금지 결정 --> | <!-- TODO: 근거 --> |
| RTTI (dynamic_cast, typeid) | <!-- TODO: 허용/금지 결정 --> | <!-- TODO: 근거 --> |
| Multiple inheritance     | <!-- TODO: 허용/금지 결정 --> | <!-- TODO: 근거 -->        |
| goto statement           | 금지      | ISO 26262-6 Table 4                          |
| Recursion                | 금지      | Stack overflow 방지                          |

### 7.2 Target-Specific 제약

| Platform       | 제약사항                                                       |
|----------------|---------------------------------------------------------------|
| NXP S32G       | <!-- TODO: 타겟 고유 제약 (엔디안, 정렬, 캐시 정책 등) -->     |
| Axon           | <!-- TODO: 타겟 고유 제약 -->                                  |

## 8. 정적 분석 및 코드 검증 계획 (Static Analysis and Code Verification)

ISO 26262-6 Table 5에 따른 단위 수준 검증 방법:

| 검증 방법                                        | ASIL-B | ASIL-D | Tool / Method               |
|--------------------------------------------------|--------|--------|------------------------------|
| Walk-through of SW unit design                   | ++     | +      | Code review meeting          |
| Inspection of SW unit design                     | +      | ++     | Checklist-based review       |
| Static code analysis                             | ++     | ++     | cppcheck, clang-tidy         |
| Abstract interpretation                          | +      | +      | 대기 중: 구현/측정 후 갱신   |
| Code metrics (complexity, coupling)              | +      | ++     | 대기 중: 구현/측정 후 갱신   |

<!-- TODO: 정적 분석 규칙 셋(ruleset) 및 위반 처리 절차 정의 -->

## 9. References

| Document ID      | Title                                              |
|------------------|----------------------------------------------------|
| SW-ARA-REQ-001   | Software Safety Requirements Specification         |
| SW-ARA-ARCH-001  | Software Architecture Description                  |
| SW-ARA-UT-001    | Software Unit Test Specification                   |
| SW-ARA-IT-001    | Software Integration Test Specification            |
| SW-ARA-VR-001    | Software Verification Report                       |

---

## Revision History

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성   |
| 1.0     | 2026-04-06 | SuYeol Kim | 코딩 표준, 정적 분석 도구, 메타데이터 갱신 |
