# AEB Control — 소프트웨어 단위 설계 (Software Unit Design)

> ISO 26262-6:2018, Clause 8 — Software unit design and implementation

---

## 문서 메타데이터 (Document Metadata)

| 항목 | 내용 |
|------|------|
| **Doc ID** | SW-AEB-UD-001 |
| **모듈** | AEB Control (Autonomous Emergency Braking) |
| **ISO 26262 참조** | Part 6, Clause 8 |
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

본 문서는 AEB Control 모듈의 소프트웨어 단위(Software Unit) 수준 상세 설계를 기술한다.
ISO 26262-6 Clause 8에 따라, 아키텍처 설계(SW-AEB-ARCH-001)에서 정의된 각 컴포넌트를
구현 가능한 단위로 상세화하며, 코딩 가이드라인 및 구현 제약사항을 명세한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-AEB-REQ-001 | AEB SW Safety Requirements | Part 6, Clause 6 |
| SW-AEB-ARCH-001 | AEB SW Architecture | Part 6, Clause 7 |
| SW-AEB-UT-001 | AEB SW Unit Test | Part 6, Clause 9 |
| SW-AEB-VR-001 | AEB SW Verification Report | Part 6, Clause 11 |

---

## 3. 단위 목록 (Software Unit List)

| Unit ID | 단위명 | 소스 파일 | 아키텍처 컴포넌트 | ASIL |
|---------|--------|-----------|-------------------|------|
| SWU-AEB-001 | AEBControlMain | `src/AEBControl_S32G_v2.cpp` | AEBControl | D |
| SWU-AEB-002 | Communicator | `src/Communicator.cpp` | Communicator | D |
| SWU-AEB-003 | DecisionEngine | `src/AEBControl_S32G_v2.cpp` 내부 (TTC 판단 로직) | DecisionEngine | D |
| SWU-AEB-004 | ConfigLoader | `../common/src/` (ConfigParser) | ConfigManager | D |
| SWU-AEB-005 | SafetyMonitor | 대기 중: 구현 완료 후 갱신 | SafetyMonitor | D |

---

## 4. 단위 상세 설계 (Unit Detailed Design)

### 4.1 SWU-AEB-001: AEBControlMain

#### 4.1.1 책임 (Responsibility)

메인 제어 루프를 실행하며, 각 서브 유닛을 초기화하고 주기적으로 호출한다.

#### 4.1.2 인터페이스

```cpp
// TODO: 실제 코드 기반으로 갱신
class AEBControl {
public:
    bool initialize(const ConfigParams& config);
    void runCycle();       // 1회 주기 실행
    void shutdown();
    
    // TODO: 상태 조회
    AEBState getState() const;
};
```

#### 4.1.3 내부 로직

```
initialize():
    1. ConfigLoader.load(config_path)
    2. Communicator.init(can_config)
    3. DecisionEngine.init(decision_params)
    4. SafetyMonitor.init(watchdog_params)
    5. 초기화 실패 시 → 안전 상태 전환

runCycle():
    1. SafetyMonitor.kickWatchdog()
    2. sensorData = Communicator.receive()
    3. if (!sensorData.isValid()) → SWREQ-AEB-003 처리
    4. decision = DecisionEngine.evaluate(sensorData)
    5. if (decision.brakeRequired)
         brakeCmd = encodeBrakeCommand(decision)
         Communicator.send(brakeCmd)
    6. SafetyMonitor.reportStatus()
```

#### 4.1.4 SWREQ 추적성

| SWREQ | 구현 위치 | 비고 |
|-------|-----------|------|
| SWREQ-AEB-001 | `runCycle()` step 4 | TTC 판단 호출 |
| SWREQ-AEB-002 | `runCycle()` step 5 | CAN 명령 전송 |
| SWREQ-AEB-003 | `runCycle()` step 3 | 센서 유효성 검사 |

---

### 4.2 SWU-AEB-002: Communicator

#### 4.2.1 책임 (Responsibility)

CAN 버스를 통한 센서 데이터 수신 및 제동 명령 송신을 담당한다.
CanSpecProvider, CrcProvider, SignalCodec을 활용한다.

#### 4.2.2 인터페이스

```cpp
// TODO: 실제 코드 기반으로 갱신
class Communicator {
public:
    bool init(const CanConfig& config);
    
    // 수신
    SensorData receive();           // SignalCodec.decode() + CrcProvider.verify()
    bool isDataFresh() const;       // timeout 검사
    
    // 송신
    bool send(const BrakeCommand& cmd);  // CanSpecProvider.encode() + CrcProvider.compute()
    uint32_t getRetryCount() const;
    
private:
    CanSpecProvider canSpec_;
    CrcProvider     crc_;
    SignalCodec     codec_;
    // TODO: 타임스탬프, alive counter
};
```

#### 4.2.3 CAN 메시지 처리 흐름

**수신 (Rx):**
```
CAN Raw Frame
  → SignalCodec.decode(rawFrame) → 물리값 변환
  → CrcProvider.verify(payload, receivedCrc)
  → if CRC 실패: 데이터 폐기, 오류 카운터 증가
  → if alive counter 불연속: 오류 보고
  → SensorData 구조체 반환
```

**송신 (Tx):**
```
BrakeCommand 구조체
  → CanSpecProvider.encode(brakeCmd) → CAN raw 데이터
  → CrcProvider.compute(payload) → CRC 첨부
  → CAN 드라이버 송신
  → if 실패: 재전송 (최대 N회), 이후 Fail-safe
```

#### 4.2.4 SWREQ 추적성

| SWREQ | 구현 위치 | 비고 |
|-------|-----------|------|
| SWREQ-AEB-002 | `send()` | 타이밍 보장 |
| SWREQ-AEB-003 | `receive()`, `isDataFresh()` | 센서 유효성 |
| SWREQ-AEB-004 | `send()` 재전송 로직 | CAN 오류 처리 |

---

### 4.3 SWU-AEB-003: DecisionEngine

#### 4.3.1 책임 (Responsibility)

TTC(Time-To-Collision) 기반 긴급 제동 판단을 수행한다.

#### 4.3.2 인터페이스

```cpp
// TODO: 실제 구현 확인 후 갱신
class DecisionEngine {
public:
    bool init(const DecisionParams& params);
    
    BrakeDecision evaluate(const SensorData& data);
    
private:
    float computeTTC(float distance, float relativeVelocity) const;
    float computeTargetDecel(float ttc, float egoSpeed) const;
    bool  isBrakingRequired(float ttc) const;
    
    DecisionParams params_;  // TTC 임계값, 감속도 한계 등
};

struct BrakeDecision {
    bool  brakeRequired;
    float targetDeceleration;   // m/s^2
    float ttc;                  // seconds
    uint8_t confidence;         // TODO: 신뢰도 레벨
};
```

#### 4.3.3 TTC 계산 로직

```
TTC = distance / |relativeVelocity|   (접근 시에만 유효)

if relativeVelocity >= 0:   → 이격 중, 제동 불필요
if distance <= 0:           → 비정상 입력, SWREQ-AEB-003
if TTC <= TTC_THRESHOLD:    → 긴급 제동 요청
    targetDecel = f(TTC, egoSpeed)   // TODO: 감속도 프로파일 정의
```

#### 4.3.4 SWREQ 추적성

| SWREQ | 구현 위치 | 비고 |
|-------|-----------|------|
| SWREQ-AEB-001 | `evaluate()`, `computeTTC()` | 핵심 판단 로직 |

---

### 4.4 SWU-AEB-004: ConfigLoader

#### 4.4.1 책임

설정 파일(`conf/config.ini` → `../../config/aeb-control.ini`)을 파싱하고
파라미터 유효성을 검증한다.

#### 4.4.2 인터페이스

```cpp
// TODO: 실제 구현 확인 후 갱신
class ConfigLoader {
public:
    static ConfigParams load(const std::string& path);
    static bool validate(const ConfigParams& params);
    static ConfigParams getDefaultSafeParams();
};
```

#### 4.4.3 파라미터 범위 검증

| 파라미터 | 타입 | 유효 범위 | 기본 안전값 | 비고 |
|----------|------|-----------|------------|------|
| ttc_threshold | float | TODO: 0.5 ~ 5.0 s | TODO | TTC 임계값 |
| max_deceleration | float | TODO: 1.0 ~ 10.0 m/s^2 | TODO | 최대 감속도 |
| cycle_time_ms | uint32_t | TODO: 5 ~ 50 ms | 10 | 실행 주기 |
| can_timeout_ms | uint32_t | TODO: 10 ~ 200 ms | TODO | 센서 timeout |
| max_retry_count | uint32_t | TODO: 1 ~ 5 | 3 | CAN 재전송 횟수 |

#### 4.4.4 SWREQ 추적성

| SWREQ | 구현 위치 | 비고 |
|-------|-----------|------|
| SWREQ-AEB-005 | `load()`, `validate()` | 설정 무결성 |

---

### 4.5 SWU-AEB-005: SafetyMonitor

#### 4.5.1 책임

런타임 안전 감시: Watchdog 관리, 진단 코드 처리, 안전 상태 전환 관리

#### 4.5.2 인터페이스

```cpp
// TODO: 상세 설계 필요
class SafetyMonitor {
public:
    bool init(const WatchdogParams& params);
    void kickWatchdog();
    void reportStatus(const DiagInfo& info);
    bool isSystemHealthy() const;
    void transitionToSafeState(SafeStateReason reason);
};
```

#### 4.5.3 SWREQ 추적성

| SWREQ | 구현 위치 | 비고 |
|-------|-----------|------|
| SWREQ-AEB-002 | `kickWatchdog()` | 타이밍 감시 |
| SWREQ-AEB-003 | `transitionToSafeState()` | 안전 상태 전환 |
| SWREQ-AEB-004 | `transitionToSafeState()` | Fail-safe |

---

## 5. 코딩 가이드라인 (Coding Guidelines)

### 5.1 적용 코딩 표준

| 표준 | 적용 범위 | 검증 도구 | 비고 |
|------|-----------|-----------|------|
| MISRA C++:2008 | 전체 모듈 | clang-tidy (프로젝트 루트 .clang-tidy 설정) | ASIL D 필수 |
| cppcheck | 전체 모듈 | cppcheck | 보조 정적 분석 |

### 5.2 ASIL D 구현 제약사항

- 동적 메모리 할당 금지 (런타임): `new`, `malloc`, STL 동적 컨테이너 사용 불가
- 재귀 호출 금지
- 포인터 연산 최소화; 배열 경계 검사 필수
- 부동소수점 비교 시 epsilon 적용
- 모든 switch 문에 default 케이스 포함
- 모든 함수 반환값 검사
- 정적 분석 도구: clang-tidy (MISRA C++:2008), cppcheck

### 5.3 네이밍 규칙

| 대상 | 규칙 | 예시 |
|------|------|------|
| 클래스 | PascalCase | `DecisionEngine` |
| 함수 | camelCase | `computeTTC()` |
| 멤버 변수 | trailing underscore | `params_` |
| 상수 | UPPER_SNAKE_CASE | `TTC_THRESHOLD` |
| 파일명 | PascalCase.cpp/.h | `AEBControl_S32G_v2.cpp` |

---

## 6. 단위 설계 검증 방법 (Unit Design Verification)

ISO 26262-6 Table 3에 따른 설계 검증 방법:

| 방법 | ASIL D 권장 | 적용 여부 | 비고 |
|------|------------|-----------|------|
| 워크스루 (Walk-through) | 권장 | 대기 중: 리뷰 수행 후 갱신 | 설계 문서 리뷰 |
| 인스펙션 (Inspection) | 강력 권장 | 대기 중: 리뷰 수행 후 갱신 | 코드 인스펙션 |
| 정적 분석 (Static analysis) | 강력 권장 | 적용 예정 | clang-tidy (MISRA C++:2008), cppcheck |
| 시뮬레이션 (Simulation) | 권장 | 대기 중: 시뮬레이션 환경 구축 후 갱신 | 로직 시뮬레이션 |

---

## 7. TODO 항목

- [ ] 실제 소스 코드(`AEBControl_S32G_v2.cpp`, `Communicator.cpp`) 분석 후 인터페이스 갱신
- [ ] DecisionEngine 단위 신규 설계 또는 기존 코드 내 위치 확인
- [ ] ConfigLoader 구현 확인 및 상세화
- [ ] SafetyMonitor 컴포넌트 상세 설계
- [ ] 파라미터 유효 범위 수치 확정
- [ ] TTC 감속도 프로파일 알고리즘 상세 정의
- [x] 코딩 표준 최종 확정 — MISRA C++:2008 (clang-tidy 기반)
- [x] 정적 분석 도구 선정 및 적용 — clang-tidy, cppcheck
- [ ] 단위 설계 리뷰 수행 및 기록
- [ ] private impl (`../common/src/private/aeb-control/`) 내용 반영
