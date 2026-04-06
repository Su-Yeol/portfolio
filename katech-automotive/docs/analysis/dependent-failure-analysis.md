# Dependent Failure Analysis

| Field | Value |
|---|---|
| **Document ID** | ANA-DFA-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 9, Clause 7 |
| **Author** | SuYeol Kim |
| **Reviewer** | - |
| **Approval Date** | 2026-04-06 |

---

## 1. Purpose

본 문서는 공통 원인 고장(Common Cause Failure), 연쇄 고장(Cascading Failure) 등 종속 고장을 분석하여, 안전 메커니즘의 독립성을 검증한다.

## 2. Common Cause Failure Analysis

### 2.1 Common Module 공유에 의한 공통 원인

Common 모듈(CrcProvider, SignalCodec, ConfigParser, CanSpecConfigLoader)은 **모든 안전 모듈**에서 사용되므로, 이 모듈의 결함은 모든 안전 기능에 동시 영향을 미치는 공통 원인 고장이 된다.

| Pair | Element A | Element B | Common Cause | Coupling Factor | Countermeasure |
|---|---|---|---|---|---|
| CCF-001 | AEB Control CRC 검증 | Control Module CRC 검증 | 동일 CrcProvider (CrcProviderPublic.cpp) | SW (공유 라이브러리) | ASIL D 기준 검증: UT-CRC 100% MC/DC, 테스트 벡터 검증, 코드 인스펙션 |
| CCF-002 | AEB Control Signal 인코딩 | Decision Module Signal 디코딩 | 동일 SignalCodec.h | SW (공유 헤더) | ASIL D 기준 검증: UT-SIG 전체 경계값 테스트, 라운드트립 검증 |
| CCF-003 | AEB Control 설정 로딩 | Control Module 설정 로딩 | 동일 ConfigParser / CanSpecConfigLoader | SW (공유 파서) | ASIL D 기준 검증: UT-PARSE, UT-CFG 전체 테스트, 폴백 메커니즘 확인 |
| CCF-004 | AEB 판단 로직 | Control 제어 로직 | 동일 NXP S32G CPU | HW (공유 프로세서) | HW watchdog 독립 감시, 프로세스 격리 (별도 스레드/cgroup) |
| CCF-005 | CAN TX (AEB) | CAN TX (Control) | 동일 CAN 버스/컨트롤러 | HW (공유 버스) | 메시지 우선순위 분리, CAN 에러 관리, Bus-off 복구 절차 |
| CCF-006 | AEB 센서 수신 | Decision 센서 수신 | 동일 CAN 수신 스레드 | SW (공유 통신 스레드) | 수신 처리 분리, 타임아웃 독립 감시 |

### 2.2 빌드 시스템 공통 원인

| Pair | Element A | Element B | Common Cause | Coupling Factor | Countermeasure |
|---|---|---|---|---|---|
| CCF-010 | Public 구현 | Private 구현 | 동일 컴파일러 (g++/clang) | Tool (공유 도구체인) | 도구 검증 (PRC-TQ-001), 두 컴파일러 교차 검증 권장 |
| CCF-011 | 호스트 빌드 | 타겟 빌드 | 동일 소스 코드 | SW (동일 코드베이스) | 호스트/타겟 Back-to-back 테스트 |

## 3. Cascading Failure Analysis

### 3.1 연쇄 고장 시나리오

| Source Failure | Affected Element | Propagation Path | Severity | Countermeasure |
|---|---|---|---|---|
| GPS 수신 실패 | 경로 계획 (Decision) → 경로 추종 (Control) → 차량 이탈 | GPS → PathManager.globalPath → Controller.targetPath → 조향 명령 | S2 | GPS 타임아웃 감지 → 안전 상태 전이 (감속→정지) |
| CAN Bus-off | 센서 수신 중단 → 판단 불가 → 제어 불가 → 차량 비제어 | CAN Controller → Communicator.rx() → 모든 모듈 | S3 | Bus-off 복구 절차, 독립 안전 정지 경로 (별도 GPIO) |
| Config 파일 손상 | 비정상 TTC 임계값 → AEB 과민/둔감 → 오탐/미탐 | config.ini → ConfigParser → DecisionEngine.ttcThreshold | S2 | 범위 검증 + 기본 안전 파라미터 fallback |
| CrcProvider 오류 | 모든 CRC 검증 무효 → E2E 보호 무력화 → 오류 데이터 수용 | CrcProviderPublic.cpp → 모든 CRC 사용 모듈 | S3 | ASIL D 검증 (100% MC/DC), 테스트 벡터 검증, 다항식 상수 정적 검사 |
| 스레드 교착 (Deadlock) | 제어 루프 행(hang) → 명령 갱신 중단 → Watchdog 타임아웃 | 공유 자원 경쟁 → mutex 순환 대기 | S2 | 고정 lock 순서 정의, 타임아웃 lock, Watchdog 복구 |

## 4. Independence Assessment

### 4.1 ASIL 분해 적용 시 독립성 평가

| Element A | Element B | Independence Criteria | Met? | Evidence |
|---|---|---|---|---|
| AEB Control (ASIL D) | Control Module (ASIL C) | 물리적 독립: 별도 프로세스/스레드 | Partial | 별도 실행 바이너리 (run_aeb, run_control), CAN 메시지 기반 통신 |
| AEB Control (ASIL D) | Decision Module (ASIL B~C) | 물리적 독립: 별도 프로세스 | Partial | 별도 실행 바이너리, CAN 통신으로 분리 |
| SafetyMonitor | 제어 로직 | 논리적 독립: 감시 경로 분리 | TBD | SafetyMonitor 미구현 — 설계 시 독립 경로 보장 필요 |
| HW Watchdog | SW Watchdog | 물리적 독립: 별도 HW 모듈 | TBD | NXP S32G HW watchdog 연동 미구현 |
| CrcProvider (Public) | CrcProvider (Private) | 논리적 독립: 별도 구현 | Yes | USE_PRIVATE_IMPL 플래그로 컴파일 시점 분리, ICrcProvider 인터페이스 동일 |

### 4.2 공통 모듈 독립성 확보 방안

Common 모듈이 ASIL D로 검증되면, 이를 사용하는 하위 ASIL 모듈의 독립성 요구사항은 충족된다 (ISO 26262 Part 9, "충분히 검증된 SW 요소는 공유 가능").

**확보 조건:**
1. Common 모듈 ASIL D 기준 단위 테스트 완료 (Statement 100%, Branch 100%, MC/DC >= 95%)
2. Common 모듈 코드 인스펙션 완료 (PRC-CR-001 체크리스트 전항목 Pass)
3. Common 모듈 정적 분석 완료 (MISRA Required 위반 0건)
4. Common 모듈 고장 주입 테스트 완료 (TC-FI-001~007 전항목 Pass)

## 5. Summary

### 5.1 Identified Dependencies

| Type | Count | Critical | Mitigated |
|---|---|---|---|
| Common Cause (SW) | 6 | 3 (CCF-001, 002, 003) | 6 (ASIL D 검증) |
| Common Cause (HW) | 2 | 2 (CCF-004, 005) | 2 (HW watchdog, 우선순위 분리) |
| Common Cause (Tool) | 2 | 0 | 2 (도구 검증) |
| Cascading Failure | 5 | 2 (CAN Bus-off, CRC 오류) | 5 (안전 메커니즘) |

### 5.2 Remaining Actions

| Priority | Action | Status |
|---|---|---|
| 높음 | Common 모듈 ASIL D 검증 완료 (UT + 인스펙션 + 정적 분석) | 진행 중 |
| 높음 | SafetyMonitor 설계 및 구현 (독립 감시 경로) | 미착수 |
| 중간 | HW Watchdog (NXP S32G) 연동 구현 | 미착수 |
| 중간 | CAN Bus-off 복구 절차 구현 | 미착수 |
| 낮음 | 호스트/타겟 Back-to-back 테스트 환경 구축 | 미착수 |

## 6. References

- ISO 26262:2018, Part 9, Clause 7
- [FMEA](fmea.md)
- [FTA](fta.md)
- [Safety Requirements Allocation](../system/safety-requirements-allocation.md)
