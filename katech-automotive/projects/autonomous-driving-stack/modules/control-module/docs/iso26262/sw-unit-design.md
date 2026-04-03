# Software Unit Design Specification

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-UD-001                                             |
| **Version**           | 0.1 Draft                                                 |
| **ISO 26262 Reference** | Part 6, Clause 8 — Software Unit Design and Implementation |
| **ASIL Scope**        | ASIL D (target) <!-- TODO: 최종 ASIL 등급 확정 후 업데이트 --> |
| **Project**           | Autonomous Driving Stack — Control Module                 |
| **Target HW**         | NXP S32G (aarch64)                                        |
| **Language**          | C++                                                       |
| **Author**            | <!-- TODO: 작성자 이름 기입 -->                             |
| **Reviewer**          | <!-- TODO: 검토자 이름 기입 -->                             |
| **Approval Date**     | <!-- TODO: 승인 일자 기입 (YYYY-MM-DD) -->                  |
| **Status**            | Draft                                                     |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 8에 따라 Control Module의 소프트웨어 유닛 상세 설계를 기술한다. 각 소프트웨어 유닛의 내부 로직, 알고리즘, 자료구조, 인터페이스 및 구현 지침을 정의하여 아키텍처 설계(SW-CTL-ARCH-001)의 구체적 실현 방법을 명세한다.

## 2. 범위 (Scope)

- 횡방향 제어기 유닛 (Lateral Controller Unit) 상세 설계
- 종방향 제어기 유닛 (Longitudinal Controller Unit) 상세 설계
- 안전 모니터 유닛 (Safety Monitor Unit) 상세 설계
- 입력 관리자 유닛 (Input Manager Unit) 상세 설계
- 출력 관리자 유닛 (Output Manager Unit) 상세 설계
- 공통 라이브러리 연동 (`../common/src/`, `../common/include/`)

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-ARCH-001   | Software Architecture Description             | 아키텍처 설계 문서      |
| SW-CTL-UT-001     | Software Unit Test Specification              | 유닛 테스트 문서        |
| <!-- TODO -->     | C++ Coding Standard                           | 코딩 표준 문서          |

## 4. 설계 지침 (Design Guidelines)

ISO 26262 Part 6, Clause 8 Table 3에 따라 ASIL D 수준에서 다음 설계 원칙을 적용한다:

| 설계 원칙                          | 적용 수준       | 비고                                      |
|------------------------------------|-----------------|-------------------------------------------|
| 단일 진입/단일 종료 (One entry/exit) | 강력 권고 (HR) | 모든 함수에 적용                           |
| 동적 메모리 할당 금지               | 강력 권고 (HR)  | 런타임 중 malloc/new 사용 금지             |
| 재귀 호출 금지                      | 강력 권고 (HR)  | 스택 오버플로우 방지                       |
| 전역 변수 최소화                    | 권고 (R)        | 불가피한 경우 접근 제어 적용               |
| 포인터 연산 최소화                  | 강력 권고 (HR)  | 배열 인덱스 접근 선호                      |
| 암시적 형변환 금지                  | 강력 권고 (HR)  | static_cast 명시 사용                      |

## 5. 소프트웨어 유닛 상세 설계 (Unit Detailed Design)

### 5.1 Lateral Controller Unit

#### 5.1.1 유닛 개요

- **유닛명**: `LateralController`
- **소스 파일**: <!-- TODO: 소스 파일 경로 기입 -->
- **요구사항 추적**: SW-CTL-SR-001 ~ SW-CTL-SR-005
- **ASIL**: D

#### 5.1.2 클래스/모듈 인터페이스

```cpp
// <!-- TODO: 실제 인터페이스에 맞게 업데이트 -->
class LateralController {
public:
    /**
     * @brief 횡방향 제어기 초기화
     * @param config 제어기 파라미터 설정
     * @return 초기화 성공 여부
     */
    bool init(const LateralConfig& config);

    /**
     * @brief 제어 주기마다 호출되는 연산 함수
     * @param path_input 목표 경로 데이터
     * @param vehicle_state 현재 차량 상태
     * @param[out] steering_cmd 조향 명령 출력
     * @return 연산 성공 여부
     */
    bool compute(const PathInput& path_input,
                 const VehicleState& vehicle_state,
                 SteeringCommand& steering_cmd);

    void reset();
};
```

#### 5.1.3 알고리즘 설계 (MPC)

<!-- TODO: MPC 알고리즘 상세 설계 기술 -->

- **상태 벡터**: x = [e_lat, e_heading, curvature_error, ...]
- **제어 입력**: u = [delta_steering]
- **차량 모델**: <!-- TODO: kinematic/dynamic bicycle model 선택 및 수식 기술 -->
- **비용 함수**: J = sum(x'Qx + u'Ru + du'Sdu)
  - Q: 경로 추종 가중치 <!-- TODO: 초기값 -->
  - R: 제어 입력 가중치 <!-- TODO: 초기값 -->
  - S: 제어 변화율 가중치 <!-- TODO: 초기값 -->
- **제약 조건**:
  - |delta| <= <!-- TODO: max steering angle --> rad
  - |d(delta)/dt| <= <!-- TODO: max steering rate --> rad/s

#### 5.1.4 PID Fallback 설계

<!-- TODO: PID fallback 제어기 상세 설계 -->

- 전환 조건: MPC 솔버 실패 또는 연산 시간 초과 시
- PID 게인: Kp=<!-- TODO -->, Ki=<!-- TODO -->, Kd=<!-- TODO -->
- Anti-windup: <!-- TODO: anti-windup 메커니즘 기술 -->

### 5.2 Longitudinal Controller Unit

#### 5.2.1 유닛 개요

- **유닛명**: `LongitudinalController`
- **소스 파일**: <!-- TODO: 소스 파일 경로 기입 -->
- **요구사항 추적**: SW-CTL-SR-010 ~ SW-CTL-SR-014
- **ASIL**: D

#### 5.2.2 클래스/모듈 인터페이스

```cpp
// <!-- TODO: 실제 인터페이스에 맞게 업데이트 -->
class LongitudinalController {
public:
    bool init(const LongitudinalConfig& config);

    /**
     * @brief 종방향 제어 연산
     * @param speed_profile 목표 속도 프로파일
     * @param vehicle_state 현재 차량 상태
     * @param[out] accel_cmd 가속/제동 명령 출력
     * @return 연산 성공 여부
     */
    bool compute(const SpeedProfile& speed_profile,
                 const VehicleState& vehicle_state,
                 AccelCommand& accel_cmd);

    void reset();
};
```

#### 5.2.3 알고리즘 설계 (PID + Feedforward)

<!-- TODO: 종방향 PID 제어기 상세 설계 -->

- **속도 오차**: e_v = v_target - v_current
- **PID 출력**: a_pid = Kp*e_v + Ki*integral(e_v) + Kd*d(e_v)/dt
- **Feedforward**: a_ff = <!-- TODO: 경사 보상, 공기저항 보상 수식 -->
- **최종 출력**: a_cmd = a_pid + a_ff
- **가속/제동 분배**:
  - a_cmd > 0: throttle 명령 생성
  - a_cmd < 0: brake 명령 생성 (회생/유압 제동 배분)

#### 5.2.4 제동 배분 로직

<!-- TODO: 회생 제동 / 유압 제동 배분 로직 상세 기술 -->

### 5.3 Safety Monitor Unit

#### 5.3.1 유닛 개요

- **유닛명**: `SafetyMonitor`
- **소스 파일**: <!-- TODO: 소스 파일 경로 기입 -->
- **요구사항 추적**: SW-CTL-SR-020 ~ SW-CTL-SR-032
- **ASIL**: D

#### 5.3.2 출력 제한 로직 (Output Limiter)

```
// Pseudo-code
function limitOutput(raw_cmd, prev_cmd, dt):
    // Range limiting
    cmd = clamp(raw_cmd, MIN_LIMIT, MAX_LIMIT)

    // Rate limiting (slew rate)
    max_delta = MAX_RATE * dt
    cmd = clamp(cmd, prev_cmd - max_delta, prev_cmd + max_delta)

    return cmd
```

#### 5.3.3 Plausibility Check

<!-- TODO: 입출력 일관성 검증 로직 상세 설계 -->

- 조향 명령 vs 실제 조향각 편차 감시
- 가속 명령 vs 실제 가속도 편차 감시
- 허용 임계값: <!-- TODO: 정의 -->

#### 5.3.4 안전 상태 전이 (Safe State Transition)

| 감지 이벤트                  | 안전 동작                        | 전이 시간 목표     |
|------------------------------|----------------------------------|--------------------|
| 입력 타임아웃                | 제어 출력 0 (coasting)           | <!-- TODO --> ms   |
| CAN 통신 오류                | 최종 유효 명령 유지 → 0 전이     | <!-- TODO --> ms   |
| 출력 범위 초과               | 즉시 클램핑                      | 1 cycle 이내       |
| MPC 솔버 실패                | PID fallback 전환                | 1 cycle 이내       |
| 비상 정지 요청               | 최대 제동력 인가                 | <!-- TODO --> ms   |

### 5.4 Input Manager Unit

#### 5.4.1 유닛 개요

- **유닛명**: `InputManager`
- **소스 파일**: <!-- TODO: 소스 파일 경로 기입 -->
- **요구사항 추적**: SW-CTL-SR-023
- **ASIL**: D

#### 5.4.2 입력 유효성 검사

<!-- TODO: 입력 데이터 유효성 검사 로직 상세 기술 -->

- 경로 데이터 범위 검사 (waypoint 좌표 범위, 곡률 범위)
- 속도 프로파일 범위 검사 (음수 속도 거부, 최대 속도 제한)
- 타임스탬프 연속성 검사
- 타임아웃 감시 (<!-- TODO --> ms 이내 미수신 시 timeout 플래그)

### 5.5 Output Manager Unit

#### 5.5.1 유닛 개요

- **유닛명**: `OutputManager`
- **소스 파일**: <!-- TODO: 소스 파일 경로 기입 -->
- **요구사항 추적**: SW-CTL-SR-030 ~ SW-CTL-SR-032
- **ASIL**: D

#### 5.5.2 CAN 프레임 구성

<!-- TODO: CAN 메시지 레이아웃 상세 정의 -->

| CAN ID         | 용도               | 주기            | 데이터 길이 | 비고                     |
|----------------|---------------------|-----------------|-------------|--------------------------|
| <!-- TODO -->  | 조향 명령           | <!-- TODO --> ms| 8 bytes     | EPS 인터페이스           |
| <!-- TODO -->  | 가감속 명령         | <!-- TODO --> ms| 8 bytes     | ESC 인터페이스           |

#### 5.5.3 Alive Counter / CRC

- Alive counter: 4-bit, 0-14 순환 (15 = invalid)
- CRC 알고리즘: <!-- TODO: CRC-8 / CRC-16 등 사양 정의 -->
- CRC 범위: 메시지 payload 전체 (alive counter 포함)

## 6. 자료구조 (Data Structures)

<!-- TODO: 주요 자료구조 정의 완성 -->

```cpp
struct VehicleState {
    double speed_mps;          // 현재 차량 속도 [m/s]
    double steering_angle_rad; // 현재 조향각 [rad]
    double yaw_rate_rads;      // yaw rate [rad/s]
    double accel_mps2;         // 종방향 가속도 [m/s^2]
    uint64_t timestamp_us;     // 타임스탬프 [us]
};

struct SteeringCommand {
    double target_angle_rad;   // 목표 조향각 [rad]
    double target_torque_nm;   // 목표 조향 토크 [Nm]
    uint8_t alive_counter;
    uint8_t crc;
};

struct AccelCommand {
    double accel_mps2;         // 목표 가속도 [m/s^2] (음수 = 제동)
    double brake_pressure_bar; // 제동 압력 [bar]
    uint8_t alive_counter;
    uint8_t crc;
};
```

## 7. 에러 처리 (Error Handling)

<!-- TODO: 각 유닛별 에러 처리 전략 상세 기술 -->

| 에러 유형                  | 처리 방법                                     | 복구 전략               |
|----------------------------|-----------------------------------------------|-------------------------|
| 입력 데이터 범위 초과      | 해당 입력 무효화, 이전 유효값 유지             | 타임아웃 시 안전 전이   |
| MPC 솔버 수렴 실패         | PID fallback 전환                             | 다음 주기 MPC 재시도    |
| CAN 전송 실패              | 재전송 1회 시도                                | 실패 지속 시 안전 전이  |
| 설정 파일 로드 실패        | 기본값(default) 사용                           | 진단 이벤트 기록        |

## 8. 설정 파라미터 (Configuration Parameters)

설정 파일: `conf/config.ini` → `../../config/control-module.ini`

<!-- TODO: 설정 파라미터 목록 완성 -->

| 파라미터                    | 타입    | 기본값         | 설명                          |
|-----------------------------|---------|----------------|-------------------------------|
| lateral.controller_type     | string  | "mpc"          | 횡방향 제어기 타입 (mpc/pid)  |
| lateral.mpc.horizon         | int     | <!-- TODO -->  | MPC 예측 구간                 |
| lateral.pid.kp              | double  | <!-- TODO -->  | PID 비례 게인                 |
| longitudinal.pid.kp         | double  | <!-- TODO -->  | 종방향 PID 비례 게인          |
| safety.input_timeout_ms     | int     | <!-- TODO -->  | 입력 타임아웃 임계값          |
| safety.max_steering_rad     | double  | <!-- TODO -->  | 최대 조향각 제한              |
| safety.max_accel_mps2       | double  | <!-- TODO -->  | 최대 가속도 제한              |
| safety.max_decel_mps2       | double  | <!-- TODO -->  | 최대 감속도 제한              |

## 9. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | <!-- TODO: 날짜 --> | <!-- TODO: 작성자 --> | Initial draft |
