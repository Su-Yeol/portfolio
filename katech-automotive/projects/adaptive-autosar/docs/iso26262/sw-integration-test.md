# Software Integration Test Specification (소프트웨어 통합 테스트 명세서)

## Document Metadata

| 항목 (Field)           | 내용 (Value)                                                           |
|------------------------|-------------------------------------------------------------------------|
| Document ID            | SW-ARA-IT-001                                                           |
| Version                | 1.0                                                                     |
| ISO 26262 Reference    | Part 6, Clause 10 — Software Integration and Testing                    |
| ASIL Scope             | ASIL-B / ASIL-D (component-dependent)                                   |
| Project                | Adaptive AUTOSAR Platform (NXP S32G / Axon)                            |
| Author                 | SuYeol Kim                                                              |
| Reviewer               | <!-- TODO: 검토자 이름 기입 -->                                         |
| Approval Date          | 2026-04-06                                                              |
| Status                 | Draft                                                                    |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 10에 따라 Adaptive AUTOSAR 플랫폼의 소프트웨어 통합 테스트를 계획하고 명세한다. 소프트웨어 단위를 통합하여 아키텍처 수준의 인터페이스 및 상호작용이 SW-ARA-ARCH-001의 설계와 SW-ARA-REQ-001의 안전 요구사항을 충족하는지 검증한다.

## 2. 적용 범위 (Scope)

본 통합 테스트는 다음 수준의 통합을 대상으로 한다:

1. **단위 간 통합 (Intra-component)**: 동일 Functional Cluster 내 단위 간 통합
2. **컴포넌트 간 통합 (Inter-component)**: FC 간 IPC/API를 통한 통합
3. **플랫폼-애플리케이션 통합**: ara-api를 통한 sample-applications과 미들웨어 통합

Target 플랫폼:
- NXP S32G (aarch64) — 실차 탑재 대상
- Axon — 보조 검증 대상

## 3. 참조 문서 (Input References)

| Ref ID | Document Title                                    | Document ID      |
|--------|---------------------------------------------------|------------------|
| REF-01 | Software Safety Requirements Specification         | SW-ARA-REQ-001   |
| REF-02 | Software Architecture Description                  | SW-ARA-ARCH-001  |
| REF-03 | Software Unit Design Specification                 | SW-ARA-UD-001    |
| REF-04 | Software Unit Test Specification                   | SW-ARA-UT-001    |

## 4. 통합 전략 (Integration Strategy)

### 4.1 통합 순서 (Integration Order)

ISO 26262-6 Clause 10.4.2에 따라 다음 통합 전략을 적용한다:

<!-- TODO: 프로젝트에 적합한 전략을 선택하고 상세 기술 -->

| 통합 전략               | 적용 여부      | 설명                                                    |
|-------------------------|----------------|---------------------------------------------------------|
| Bottom-up integration   | <!-- TODO -->  | 하위 단위부터 상위 컴포넌트로 단계적 통합                |
| Top-down integration    | <!-- TODO -->  | 상위 컴포넌트부터 하위 단위로 stub 기반 통합             |
| Incremental integration | <!-- TODO -->  | 기능 단위 점진적 통합                                    |
| Big-bang integration    | 비적용         | 안전 관련 소프트웨어에 부적합                            |

### 4.2 통합 단계 (Integration Steps)

```
Step 1: Intra-FC Unit Integration
  ├── EXEC 내부 단위 통합 (MachineStateManager + ProcessManager + ...)
  ├── PHM 내부 단위 통합 (AliveSupervision + DeadlineSupervision + RecoveryActionHandler + ...)
  ├── COM 내부 단위 통합 (SomeIpBinding + E2EProtectionWrapper + ServiceDiscovery + ...)
  └── UCM 내부 단위 통합 (PackageManager + IntegrityVerifier + RollbackController + ...)

Step 2: Inter-FC Integration
  ├── EXEC ↔ PHM 통합 (프로세스 감시 연동)
  ├── COM ↔ E2E 통합 (통신 보호 연동)
  ├── UCM ↔ EXEC 통합 (업데이트 후 프로세스 관리)
  └── IAM ↔ COM/EXEC 통합 (접근 제어 연동)

Step 3: Platform-Application Integration
  ├── ara-api ↔ Middleware FC 통합
  └── sample-applications ↔ ara-api 통합
```

<!-- TODO: 각 단계의 선행 조건(prerequisite), 완료 기준(exit criteria) 정의 -->

## 5. 테스트 환경 (Test Environment)

### 5.1 통합 테스트 환경

| 항목                    | Host 환경                           | Target 환경 (NXP S32G)             | Target 환경 (Axon)     |
|-------------------------|--------------------------------------|-------------------------------------|------------------------|
| OS                      | Ubuntu 22.04 LTS                     | Yocto Linux (aarch64)               | 대기 중: 구현/측정 후 갱신 |
| Build                   | CMake 3.15+ + Docker                 | Yocto/bitbake cross-build           | 대기 중: 구현/측정 후 갱신 |
| Test Framework          | Google Test (gtest) + Google Mock    | Google Test (cross-compiled)        | 대기 중: 구현/측정 후 갱신 |
| Communication           | Loopback / Virtual VLAN              | Real Ethernet                       | <!-- TODO -->          |
| 디버깅 도구             | <!-- TODO: GDB, valgrind 등 -->      | <!-- TODO: JTAG, trace 등 -->       | <!-- TODO -->          |

### 5.2 테스트 하네스 및 시뮬레이터

<!-- TODO: 통합 테스트에 필요한 test harness, 시뮬레이터, stub/mock 구성 기술 -->

| 항목                     | 설명                                                       |
|--------------------------|------------------------------------------------------------|
| SOME/IP Simulator        | <!-- TODO: vsomeip test mode 또는 외부 시뮬레이터 -->       |
| PHM Test Stub            | <!-- TODO: supervision 위반을 인위적으로 발생시키는 stub --> |
| UCM Test Package         | <!-- TODO: 테스트용 SW 패키지 (정상/변조) -->               |
| Network Impairment Tool  | <!-- TODO: 패킷 loss/delay 주입 도구 -->                    |

## 6. 테스트 방법론 (Test Methods)

### 6.1 ISO 26262-6 Table 11 적용 (통합 테스트 방법)

| 테스트 방법                                      | ASIL-B | ASIL-D | 적용 여부   | 비고                          |
|--------------------------------------------------|--------|--------|-------------|-------------------------------|
| Requirements-based testing                        | ++     | ++     | <!-- TODO --> | 아키텍처 요구사항 기반 TC    |
| Interface testing                                 | ++     | ++     | <!-- TODO --> | 컴포넌트 간 인터페이스 검증  |
| Fault injection testing                           | +      | ++     | <!-- TODO --> | 통합 수준 오류 주입          |
| Resource usage testing                            | +      | ++     | <!-- TODO --> | 통합 환경 리소스 사용량 검증 |

Legend: ++ Highly recommended, + Recommended

### 6.2 ISO 26262-6 Table 12 적용 (구조적 커버리지 — 통합 수준)

| 커버리지 메트릭                                  | ASIL-B | ASIL-D | 목표치           |
|--------------------------------------------------|--------|--------|------------------|
| Function coverage                                 | ++     | +      | >= 95%            |
| Call coverage                                     | ++     | ++     | >= 90%            |

## 7. 통합 테스트 케이스 명세 (Integration Test Cases)

### 7.1 Step 1: Intra-FC Integration

#### EXEC 내부 통합

| TC ID           | Test Description                                                   | 통합 대상                            | 기대 결과                                   | 관련 SwRS / IF    | Pass/Fail |
|-----------------|--------------------------------------------------------------------|--------------------------------------|----------------------------------------------|-------------------|-----------|
| IT-EXEC-001     | MachineStateManager가 ProcessManager를 통해 프로세스 시작/종료 확인 | SWU-EXEC-001 + SWU-EXEC-002         | State 전환 시 프로세스 정상 시작/종료          | SwRS-EXEC-001, 002| <!-- TODO --> |
| IT-EXEC-002     | <!-- TODO: 추가 테스트 케이스 -->                                  | <!-- TODO -->                        | <!-- TODO -->                                | <!-- TODO -->     | <!-- TODO --> |

#### PHM 내부 통합

| TC ID           | Test Description                                                   | 통합 대상                            | 기대 결과                                   | 관련 SwRS / IF    | Pass/Fail |
|-----------------|--------------------------------------------------------------------|--------------------------------------|----------------------------------------------|-------------------|-----------|
| IT-PHM-001      | AliveSupervision 위반 시 RecoveryActionHandler 동작 확인            | SWU-PHM-001 + SWU-PHM-004           | 위반 감지 후 설정된 recovery action 수행       | SwRS-PHM-001, 002 | <!-- TODO --> |
| IT-PHM-002      | <!-- TODO: 추가 테스트 케이스 -->                                  | <!-- TODO -->                        | <!-- TODO -->                                | <!-- TODO -->     | <!-- TODO --> |

### 7.2 Step 2: Inter-FC Integration

#### EXEC ↔ PHM 통합

| TC ID           | Test Description                                                   | 통합 대상          | 기대 결과                                         | 관련 IF / SwRS    | Pass/Fail |
|-----------------|--------------------------------------------------------------------|--------------------|---------------------------------------------------|-------------------|-----------|
| IT-XFC-001      | EXEC가 프로세스 비정상 종료를 PHM에 보고하고 PHM이 recovery 수행    | EXEC + PHM         | PHM이 보고 수신, recovery action 정상 수행          | IF-001, SwRS-EXEC-001 | <!-- TODO --> |
| IT-XFC-002      | PHM supervision 위반 시 EXEC에 프로세스 재시작 요청                 | PHM + EXEC         | EXEC가 해당 프로세스 재시작 수행                    | IF-001, SwRS-PHM-002  | <!-- TODO --> |
| IT-XFC-003      | <!-- TODO: 추가 테스트 케이스 -->                                  | <!-- TODO -->      | <!-- TODO -->                                     | <!-- TODO -->     | <!-- TODO --> |

#### COM E2E End-to-End 통합

| TC ID           | Test Description                                                   | 통합 대상          | 기대 결과                                         | 관련 IF / SwRS    | Pass/Fail |
|-----------------|--------------------------------------------------------------------|--------------------|---------------------------------------------------|-------------------|-----------|
| IT-XFC-010      | Application → COM → E2E Protection → Network → E2E Check → Application 경로 검증 | COM + ara-api + App | 송수신 데이터 무결성 확인, E2E status OK          | IF-002, SwRS-COM-001 | <!-- TODO --> |
| IT-XFC-011      | Network 상 데이터 변조 시 E2E 수신측 오류 감지                      | COM + E2E          | E2E Protection error 감지, 애플리케이션에 통지     | IF-002, SwRS-COM-001 | <!-- TODO --> |
| IT-XFC-012      | 통신 timeout 시 COM에서 애플리케이션으로 오류 통지                   | COM + ara-api      | Timeout 에러 코드 정상 전달                        | SwRS-COM-002     | <!-- TODO --> |

#### UCM ↔ EXEC 통합

| TC ID           | Test Description                                                   | 통합 대상          | 기대 결과                                         | 관련 IF / SwRS    | Pass/Fail |
|-----------------|--------------------------------------------------------------------|--------------------|---------------------------------------------------|-------------------|-----------|
| IT-XFC-020      | UCM 업데이트 완료 후 EXEC가 업데이트된 프로세스 재시작              | UCM + EXEC         | 새 버전 프로세스 정상 시작                         | IF-004, SwRS-UCM-001 | <!-- TODO --> |
| IT-XFC-021      | UCM 업데이트 실패 시 rollback 후 EXEC가 이전 버전 프로세스 유지     | UCM + EXEC         | 이전 버전 프로세스 정상 동작                       | IF-004, SwRS-UCM-001 | <!-- TODO --> |

### 7.3 Step 3: Platform-Application Integration

| TC ID           | Test Description                                                   | 통합 대상                    | 기대 결과                                   | 관련 SwRS       | Pass/Fail |
|-----------------|--------------------------------------------------------------------|------------------------------|----------------------------------------------|-----------------|-----------|
| IT-APP-001      | sample-application이 ara::exec API로 정상 시작/종료되는지 확인      | sample-app + ara-api + EXEC  | 애플리케이션 수명주기 정상 관리               | SwRS-EXEC-001   | <!-- TODO --> |
| IT-APP-002      | sample-application이 ara::com API로 SOME/IP 서비스 통신 성공        | sample-app + ara-api + COM   | 서비스 요청/응답 정상 동작                    | SwRS-COM-001    | <!-- TODO --> |
| IT-APP-003      | IAM 미인가 접근 시 sample-application의 서비스 접근 차단 확인        | sample-app + IAM             | 접근 거부 에러 반환                           | SwRS-IAM-001    | <!-- TODO --> |
| IT-APP-004      | <!-- TODO: 추가 테스트 케이스 -->                                  | <!-- TODO -->                | <!-- TODO -->                                | <!-- TODO -->   | <!-- TODO --> |

## 8. 통합 수준 Fault Injection 테스트

| FI Test ID     | 대상 통합 인터페이스 | 주입 결함                              | 기대 동작                                       | 관련 SM   |
|----------------|---------------------|----------------------------------------|--------------------------------------------------|-----------|
| FI-IT-001      | IF-001 (EXEC↔PHM)  | EXEC→PHM IPC 채널 중단                  | PHM이 EXEC 통신 장애 감지, 대체 경로 또는 safe state | SM-001   |
| FI-IT-002      | IF-002 (App↔COM)   | Network packet drop 50%                | E2E timeout 감지, 애플리케이션에 오류 통지        | SM-002    |
| FI-IT-003      | IF-004 (UCM↔EXEC)  | UCM 업데이트 중 전원 차단 시뮬레이션     | 재부팅 후 rollback 및 이전 버전 복원              | SM-003    |
| FI-IT-004      | <!-- TODO: 추가 --> | <!-- TODO -->                          | <!-- TODO -->                                    | <!-- TODO --> |

## 9. Target 플랫폼별 통합 테스트 (Platform-Specific Testing)

### 9.1 NXP S32G (aarch64)

<!-- TODO: S32G 보드에서의 통합 테스트 수행 절차, 보드 설정, 네트워크 구성 기술 -->

| 항목                    | 내용                                                    |
|-------------------------|---------------------------------------------------------|
| Board Setup             | <!-- TODO: 보드 구성, 전원, 디버그 연결 -->              |
| Boot Configuration      | <!-- TODO: U-Boot, Yocto rootfs 설정 -->                |
| Network Configuration   | <!-- TODO: Ethernet 설정, VLAN 구성 -->                  |
| Test Execution Method   | <!-- TODO: SSH, serial console, 자동화 스크립트 -->      |

### 9.2 Axon

<!-- TODO: Axon 플랫폼에서의 통합 테스트 구성 기술 -->

## 10. 테스트 실행 결과 요약 (Test Execution Summary)

<!-- TODO: 테스트 실행 후 기입 -->

| 통합 단계              | 총 TC 수 | Pass | Fail | Blocked | Pass Rate |
|------------------------|----------|------|------|---------|-----------|
| Step 1 (Intra-FC)     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Step 2 (Inter-FC)     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Step 3 (Platform-App) | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| **Total**              | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 11. 미결 이슈 및 편차 (Open Issues and Deviations)

<!-- TODO: 통합 테스트 중 발견된 이슈, 미해결 defect, 테스트 커버리지 편차 기록 -->

| Issue ID | Description                                    | 심각도     | 영향 분석                   | 조치 계획      | 상태   |
|----------|------------------------------------------------|-----------|------------------------------|----------------|--------|
| <!-- TODO --> | <!-- TODO -->                             | <!-- TODO --> | <!-- TODO -->             | <!-- TODO -->  | Open   |

## 12. References

| Document ID      | Title                                              |
|------------------|----------------------------------------------------|
| SW-ARA-REQ-001   | Software Safety Requirements Specification         |
| SW-ARA-ARCH-001  | Software Architecture Description                  |
| SW-ARA-UD-001    | Software Unit Design Specification                 |
| SW-ARA-UT-001    | Software Unit Test Specification                   |
| SW-ARA-VR-001    | Software Verification Report                       |

---

## Revision History

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | 2026-04-06 | SuYeol Kim | Initial draft 작성   |
| 1.0     | 2026-04-06 | SuYeol Kim | 테스트 환경, 커버리지 목표, 메타데이터 갱신 |
