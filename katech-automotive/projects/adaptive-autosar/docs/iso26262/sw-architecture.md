# Software Architecture Description (소프트웨어 아키텍처 설계서)

## Document Metadata

| 항목 (Field)           | 내용 (Value)                                                              |
|------------------------|---------------------------------------------------------------------------|
| Document ID            | SW-ARA-ARCH-001                                                           |
| Version                | 0.1 Draft                                                                 |
| ISO 26262 Reference    | Part 6, Clause 7 — Software Architectural Design                          |
| ASIL Scope             | ASIL-B / ASIL-D (component-dependent)                                     |
| Project                | Adaptive AUTOSAR Platform (NXP S32G / Axon)                              |
| Author                 | <!-- TODO: 작성자 이름 기입 -->                                           |
| Reviewer               | <!-- TODO: 검토자 이름 기입 -->                                           |
| Approval Date          | <!-- TODO: 승인일 기입 (YYYY-MM-DD) -->                                   |
| Status                 | Draft                                                                     |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Adaptive AUTOSAR 플랫폼의 소프트웨어 아키텍처를 기술한다. SW-ARA-REQ-001에서 정의된 소프트웨어 안전 요구사항을 충족하는 아키텍처 구조, 컴포넌트 간 인터페이스, 안전 메커니즘의 배치를 정의한다.

<!-- TODO: 아키텍처 설계의 구체적 목표 및 설계 원칙 기술 -->

## 2. 적용 범위 (Scope)

본 아키텍처는 Adaptive AUTOSAR 미들웨어 플랫폼 전체를 대상으로 하며, 다음 레이어를 포함한다:

- **Application Layer**: Sample applications, Adaptive Applications
- **ARA (AUTOSAR Runtime for Adaptive) Layer**: ara-api
- **Middleware/Service Layer**: COM, UCM, PHM, IAM, EXEC
- **OS/Foundation Layer**: POSIX OS (Linux/Yocto), Hardware Abstraction

Target Hardware:
- **NXP S32G (aarch64)**: Vehicle gateway / domain controller — Cortex-A53 cluster
- **Axon**: Application processor target

Build System: CMake + Yocto (bitbake) + Docker containerized build environment

## 3. 참조 문서 (Input References)

| Ref ID | Document Title                                    | Document ID      |
|--------|---------------------------------------------------|------------------|
| REF-01 | Software Safety Requirements Specification         | SW-ARA-REQ-001   |
| REF-02 | Hardware-Software Interface Specification (HSI)    | <!-- TODO -->    |
| REF-03 | System Architecture Description                    | <!-- TODO -->    |
| REF-04 | AUTOSAR Adaptive Platform Specification            | <!-- TODO -->    |
| REF-05 | NXP S32G Technical Reference Manual                | <!-- TODO -->    |

## 4. 아키텍처 설계 원칙 (Design Principles)

ISO 26262-6 Clause 7.4.1에 따른 아키텍처 설계 원칙:

| 원칙 (Principle)                        | 적용 방법 (Application)                                                  |
|-----------------------------------------|--------------------------------------------------------------------------|
| Modularity (모듈성)                      | 각 FC(Functional Cluster)를 독립 프로세스로 분리                          |
| Encapsulation (캡슐화)                   | ara-api를 통한 인터페이스 추상화, 내부 구현 은닉                          |
| Restricted complexity (복잡도 제한)      | 컴포넌트 간 의존성 최소화, 순환 의존 금지                                 |
| Freedom from interference (간섭 방지)    | 프로세스 격리, 메모리 보호, 통신 채널 분리                                |
| Appropriate granularity (적절한 세분화)  | FC 단위 분리 및 내부 모듈 구조 정의                                       |

<!-- TODO: 각 원칙의 프로젝트 적용 사례를 구체적으로 기술 -->

## 5. 소프트웨어 아키텍처 개요 (Architecture Overview)

### 5.1 계층 구조 (Layered Architecture)

```
┌─────────────────────────────────────────────────────┐
│              Adaptive Applications                   │
│         (sample-applications, user apps)             │
├─────────────────────────────────────────────────────┤
│                    ara-api                            │
│      (ara::com, ara::exec, ara::phm, ara::ucm, ...) │
├──────┬──────┬──────┬──────┬──────┬──────────────────┤
│ EXEC │ PHM  │ COM  │ UCM  │ IAM  │  기타 FC         │
│      │      │      │      │      │  (추가 예정)      │
├──────┴──────┴──────┴──────┴──────┴──────────────────┤
│            OS / Foundation (Linux / Yocto)            │
├─────────────────────────────────────────────────────┤
│         Hardware (NXP S32G / Axon)                   │
└─────────────────────────────────────────────────────┘
```

<!-- TODO: 상세 아키텍처 다이어그램을 별도 파일(draw.io, PlantUML 등)로 작성 후 링크 -->

### 5.2 컴포넌트 설명 (Component Descriptions)

#### 5.2.1 Execution Management (EXEC)

| 항목               | 내용                                                              |
|--------------------|-------------------------------------------------------------------|
| 역할               | 애플리케이션 수명주기 관리, Machine State 관리, Process 시작/종료  |
| 구현 언어          | C++17                                                             |
| 프로세스 모델      | 독립 daemon 프로세스                                               |
| 관련 SwRS          | SwRS-EXEC-001, SwRS-EXEC-002                                      |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                                |

<!-- TODO: EXEC 내부 모듈 구조, 상태 머신, 주요 클래스 설계 기술 -->

#### 5.2.2 Platform Health Management (PHM)

| 항목               | 내용                                                                     |
|--------------------|--------------------------------------------------------------------------|
| 역할               | Alive/Deadline/Logical supervision, Recovery action 수행                  |
| 구현 언어          | C++17                                                                     |
| 프로세스 모델      | 독립 daemon 프로세스                                                      |
| 관련 SwRS          | SwRS-PHM-001, SwRS-PHM-002                                                |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                                       |

<!-- TODO: PHM supervision 메커니즘, watchdog 연동, recovery action 정책 기술 -->

#### 5.2.3 Communication Management (COM)

| 항목               | 내용                                                               |
|--------------------|--------------------------------------------------------------------|
| 역할               | SOME/IP 기반 서비스 통신, E2E Protection, 서비스 디스커버리          |
| 구현 언어          | C++17                                                               |
| 프로세스 모델      | 독립 daemon 프로세스 / 라이브러리 모드                               |
| 관련 SwRS          | SwRS-COM-001, SwRS-COM-002                                          |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                                  |

<!-- TODO: COM 통신 스택 구조, SOME/IP binding, E2E wrapper 설계 기술 -->

#### 5.2.4 Update and Configuration Management (UCM)

| 항목               | 내용                                                                  |
|--------------------|-----------------------------------------------------------------------|
| 역할               | SW 패키지 설치/업데이트/롤백, 무결성 검증                               |
| 구현 언어          | C++17                                                                  |
| 프로세스 모델      | 독립 daemon 프로세스                                                    |
| 관련 SwRS          | SwRS-UCM-001, SwRS-UCM-002                                             |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                                     |

<!-- TODO: UCM 상태 머신, 패키지 처리 파이프라인, rollback 메커니즘 기술 -->

#### 5.2.5 Identity and Access Management (IAM)

| 항목               | 내용                                                     |
|--------------------|----------------------------------------------------------|
| 역할               | 서비스 접근 제어, 인증/인가 정책 관리                      |
| 구현 언어          | C++17                                                     |
| 프로세스 모델      | 독립 daemon 프로세스 / 라이브러리 모드                     |
| 관련 SwRS          | SwRS-IAM-001                                               |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                         |

#### 5.2.6 ARA Runtime API (ara-api)

| 항목               | 내용                                                                |
|--------------------|---------------------------------------------------------------------|
| 역할               | 애플리케이션-미들웨어 간 표준 API 인터페이스 제공                     |
| 구현 언어          | C++17 (header + proxy/skeleton library)                              |
| 프로세스 모델      | 라이브러리 (application process 내 링크)                              |
| 관련 SwRS          | SwRS-ARA-001                                                         |
| 안전 관련 여부     | <!-- TODO: Yes/No 기입 및 근거 -->                                   |

## 6. 인터페이스 정의 (Interface Definitions)

### 6.1 컴포넌트 간 인터페이스 (Inter-Component Interfaces)

<!-- TODO: 각 인터페이스의 통신 방식(IPC, SOME/IP, shared memory 등), 데이터 포맷, 에러 처리 방식을 기술 -->

| Interface ID | Source Component | Target Component | 통신 방식      | Protocol / Mechanism         | 관련 SwRS       |
|-------------|------------------|------------------|---------------|------------------------------|-----------------|
| IF-001      | EXEC             | PHM              | IPC           | <!-- TODO: 구체 기술 -->      | SwRS-EXEC-001   |
| IF-002      | Application      | COM              | ara::com API  | SOME/IP                       | SwRS-COM-001    |
| IF-003      | Application      | EXEC             | ara::exec API | <!-- TODO: 구체 기술 -->      | SwRS-EXEC-002   |
| IF-004      | UCM              | EXEC             | IPC           | <!-- TODO: 구체 기술 -->      | SwRS-UCM-001    |
| IF-005      | Application      | IAM              | ara API       | <!-- TODO: 구체 기술 -->      | SwRS-IAM-001    |
| IF-006      | <!-- TODO -->    | <!-- TODO -->    | <!-- TODO --> | <!-- TODO -->                | <!-- TODO -->   |

### 6.2 하드웨어-소프트웨어 인터페이스 (HSI)

<!-- TODO: NXP S32G 및 Axon 플랫폼의 HW-SW 인터페이스 명세 (메모리 맵, 인터럽트, 타이머 등) -->

| HSI Item          | NXP S32G (aarch64)                    | Axon                          |
|-------------------|---------------------------------------|-------------------------------|
| CPU Architecture  | ARM Cortex-A53 (4-core)               | <!-- TODO -->                 |
| Memory Map        | <!-- TODO: DDR 영역, SRAM 등 기술 --> | <!-- TODO -->                 |
| Watchdog Timer    | <!-- TODO: HW watchdog 사양 -->       | <!-- TODO -->                 |
| Network Interface | <!-- TODO: Ethernet 등 -->            | <!-- TODO -->                 |

## 7. 안전 메커니즘 아키텍처 배치 (Safety Mechanism Allocation)

ISO 26262-6 Clause 7.4.4에 따른 안전 메커니즘의 아키텍처 수준 배치:

| SM ID  | 안전 메커니즘                       | 배치 위치 (Component)    | 감지 시간 (Detection Time) | Recovery Action              |
|--------|-------------------------------------|--------------------------|----------------------------|------------------------------|
| SM-001 | Process alive supervision           | PHM                      | <!-- TODO: ms 단위 -->     | Process restart              |
| SM-002 | E2E Communication Protection        | COM (ara::com E2E)       | Per-message                | Error notification to app    |
| SM-003 | SW Package integrity check          | UCM                      | During update              | Reject package / rollback    |
| SM-004 | <!-- TODO: 추가 메커니즘 -->        | <!-- TODO -->            | <!-- TODO -->              | <!-- TODO -->                |

<!-- TODO: 각 안전 메커니즘의 diagnostic coverage 산출 근거 기술 -->

## 8. Freedom from Interference 분석 (간섭 방지 분석)

ISO 26262-6 Clause 7.4.5에 따라 서로 다른 ASIL 등급의 소프트웨어 컴포넌트 간 간섭 방지를 보장한다.

### 8.1 간섭 유형 및 대응

| 간섭 유형                          | 대응 메커니즘                                                  |
|------------------------------------|----------------------------------------------------------------|
| 메모리 간섭 (Memory interference)  | OS 프로세스 격리, MMU/MPU 기반 메모리 보호                       |
| 시간 간섭 (Timing interference)    | OS 스케줄러 기반 CPU 시간 분배, watchdog supervision             |
| 통신 간섭 (Communication interference) | 전용 IPC 채널, E2E Protection                               |

<!-- TODO: NXP S32G 및 Axon에서의 구체적 메모리 보호 설정, 스케줄링 정책 기술 -->

### 8.2 ASIL 분해 적용 시 독립성 논거

<!-- TODO: ASIL 분해가 적용되는 경우, 컴포넌트 간 독립성(충분한 독립성) 논거를 기술 -->

## 9. 아키텍처 설계 검증 (Architecture Verification)

ISO 26262-6 Table 3에 따른 아키텍처 수준 검증 방법:

| 검증 방법 (Method)                                   | ASIL-B | ASIL-D | 적용 여부 | 비고                    |
|------------------------------------------------------|--------|--------|-----------|-------------------------|
| Architecture design walk-through                      | ++     | +      | <!-- TODO --> | <!-- TODO -->        |
| Architecture design inspection                        | +      | ++     | <!-- TODO --> | <!-- TODO -->        |
| Simulation of dynamic behavior                        | +      | +      | <!-- TODO --> | <!-- TODO -->        |
| Formal verification of architecture                   | o      | +      | <!-- TODO --> | <!-- TODO -->        |
| Architecture design review with safety assessment     | ++     | ++     | <!-- TODO --> | <!-- TODO -->        |

Legend: ++ Highly recommended, + Recommended, o No recommendation

<!-- TODO: 각 검증 방법의 수행 계획 및 결과를 SW-ARA-VR-001에 기록 -->

## 10. 빌드 및 배포 아키텍처 (Build and Deployment)

| 항목                   | 내용                                                         |
|------------------------|--------------------------------------------------------------|
| Build System           | CMake 3.x + Yocto/bitbake                                    |
| Containerization       | Docker (빌드 환경 재현성 보장)                                 |
| Cross-compilation      | aarch64-poky-linux (NXP S32G target)                          |
| CI/CD                  | <!-- TODO: CI 파이프라인 기술 -->                              |
| Deployment Target      | NXP S32G rootfs, Axon rootfs                                  |

<!-- TODO: Yocto layer 구성, recipe 구조, 빌드 의존성 기술 -->

## 11. 제약사항 및 가정 (Constraints and Assumptions)

<!-- TODO: 아키텍처 설계 시 전제 조건 및 제약사항 기술 -->

1. <!-- TODO: OS 관련 제약사항 (예: Linux kernel version, PREEMPT_RT 적용 여부) -->
2. <!-- TODO: AUTOSAR Adaptive Platform 버전 관련 제약 -->
3. <!-- TODO: HW 리소스 제약 (메모리, CPU core 할당 등) -->

## 12. References

| Document ID      | Title                                              |
|------------------|----------------------------------------------------|
| SW-ARA-REQ-001   | Software Safety Requirements Specification         |
| SW-ARA-UD-001    | Software Unit Design Specification                 |
| SW-ARA-UT-001    | Software Unit Test Specification                   |
| SW-ARA-IT-001    | Software Integration Test Specification            |
| SW-ARA-VR-001    | Software Verification Report                       |

---

## Revision History

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | <!-- TODO: 작성일 --> | <!-- TODO --> | Initial draft 작성   |
