# Failure Mode and Effects Analysis (FMEA)

| Field | Value |
|---|---|
| **Document ID** | ANA-FMEA-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 9, Clause 6 |
| **Author** | SuYeol Kim |
| **Reviewer** | - |
| **Approval Date** | 2026-04-06 |

---

## 1. Purpose

본 문서는 KATECH Automotive 시스템의 고장 모드 및 영향 분석(FMEA)을 수행하여, 잠재적 고장의 영향과 위험도를 평가하고 대응 조치를 정의한다.

## 2. Scope

- **분석 수준**: Software FMEA (SW-FMEA)
- **대상 시스템**: Autonomous Driving Stack (NXP S32G), Adaptive AUTOSAR Platform
- **대상 모듈**: AEB Control (ASIL D), Control Module (ASIL C), Decision Module (ASIL B~C), Remote Control (ASIL B), Common (ASIL D)
- **Severity 기준**: ISO 26262 Part 3, Table 1 (S0~S3)

### Severity Scale

| Level | Definition |
|---|---|
| S3 | 생명 위협적 부상, 치명적 (Life-threatening, fatal) |
| S2 | 중상, 생존 가능 (Severe, survival probable) |
| S1 | 경상 (Light/moderate injuries) |
| S0 | 부상 없음 (No injuries) |

## 3. FMEA Worksheet

### 3.1 AEB Control Module (ASIL D)

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| AEB 긴급 제동 판단 (DecisionEngine) | 판단 지연 (>20ms) | 제동 명령 생성 지연 | 충돌 회피 실패, 추돌 사고 | S3 | Watchdog 타이머, WCET 분석 | WCET 분석 수행, 데드라인 모니터링 구현 |
| AEB 긴급 제동 판단 | 오탐 (False Positive) | 불필요한 급제동 명령 | 후속 차량 추돌, 승객 상해 | S2 | TTC 임계값 교차 검증, 센서 퓨전 | 다중 센서 교차 검증, TTC 임계값 튜닝 |
| AEB 긴급 제동 판단 | 미탐 (False Negative) | 제동 명령 미생성 | 전방 장애물 충돌 | S3 | 독립 감시 경로, 센서 이중화 | 이중 센서 경로 구현, 안전 여유 설정 |
| CAN 제동 명령 전송 (Communicator) | 메시지 손실 | 액추에이터 미응답 | 제동 미작동 | S3 | Alive counter, CRC 검증 | 재전송 메커니즘 (최대 N회), Fail-safe 전이 |
| CAN 제동 명령 전송 | 메시지 손상 | 잘못된 제동력 적용 | 과도/부족 제동 | S2 | CRC-16/CCITT 검증 | E2E Protection (AUTOSAR), 수신측 CRC 검증 |
| CRC 계산 (CrcProvider) | CRC 오계산 | 유효 메시지 폐기 또는 오류 메시지 수용 | 통신 무결성 훼손 | S2 | 다항식 검증, 테스트 벡터 | CRC 알고리즘 테스트 벡터 검증 (UT-CRC-001), 코드 리뷰 |
| 설정 파라미터 로딩 (ConfigManager) | 파라미터 범위 초과 | TTC 임계값 비정상 | AEB 과민/둔감 반응 | S2 | 범위 검사, 기본값 fallback | config.ini 범위 검증, 기본 안전 파라미터 적용 |
| Watchdog 감시 (SafetyMonitor) | 감시 미동작 | 주기 초과 미탐지 | 제어 루프 행(hang) 미대응 | S3 | 외부 HW watchdog, 이중 감시 | HW watchdog 연동, SW/HW 이중 감시 구조 |

### 3.2 Control Module (ASIL C)

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| 횡방향 제어 (Pure Pursuit) | 조향 명령 오류 | 잘못된 조향각 계산 | 차선 이탈, 대향차선 침범 | S3 | 조향각 범위 제한 (±400°), 편차 모니터링 | 조향 명령 클램핑, 독립 경로 편차 감시 |
| 종방향 제어 (SCC) | 가속도 범위 초과 | 과도한 가감속 | 급가속/급감속으로 인한 사고 | S2 | 가속도 물리 제한 값 검사 | 가감속 명령 범위 제한 (max_accel, max_decel) |
| 제어 루프 타이밍 | 주기 지연 (>50ms) | 제어 명령 갱신 지연 | 경로 추종 성능 저하 | S1 | 주기 모니터링, jitter 측정 | WCET 분석, 우선순위 기반 스케줄링 |
| CAN 액추에이터 명령 전송 | 전송 실패 | 이전 명령 유지 | 차량 제어 불능 | S2 | 전송 확인, 재전송 | Alive counter 기반 수신 확인, Fail-safe |
| 경로 데이터 수신 (Decision→Control) | 데이터 타임아웃 | 경로 정보 갱신 중단 | 차량 정지/이탈 | S2 | 수신 타임아웃 감시 | 타임아웃 시 감속 → 정지 (안전 상태) |

### 3.3 Decision Module (ASIL B~C)

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| 경로 계획 (PathManager) | 경로 좌표 오류 | 잘못된 목표 경로 생성 | 차량 이탈, 장애물 충돌 | S2 | 경로 연속성 검사, 급격한 변화 감지 | 경로 플라우시빌리티 체크, 이전 경로 참조 |
| 보행자 감지 판단 | 보행자 미감지 | 긴급 판단 미수행 | 보행자 사고 | S3 | 다중 센서 교차 확인 (Radar + LiDAR + Vision) | 센서 퓨전 결과 교차 검증, 최소 거리 임계값 |
| GPS 수신 | GPS 신호 손실 | 위치 추정 불능 | 경로 추종 불가 | S1 | GPS 수신 타임아웃 감시, 신호 품질 확인 | 타임아웃 시 안전 상태 전이 (감속 → 정지) |
| CAN Gateway 통신 | 메시지 무결성 오류 | 잘못된 센서 데이터 수신 | 잘못된 판단 수행 | S2 | CRC 검증 (source_gateway/CRC.cpp) | E2E CRC 검증, Alive counter 확인 |

### 3.4 Communication (CAN/UDP)

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| CAN 메시지 전송 | 메시지 손실 (bus congestion) | 수신 노드 타임아웃 | 제어 명령/센서 데이터 누락 | S2 | Alive counter 불연속 감지 | 메시지 우선순위 설정, 버스 부하 관리 |
| CAN 메시지 전송 | 메시지 손상 (EMI) | CRC 불일치 | 데이터 무결성 훼손 | S2 | CRC-16/CCITT 검증 | E2E Protection, 재전송 메커니즘 |
| CAN 컨트롤러 | Bus-off 상태 | CAN 통신 전면 중단 | 차량 제어 불가 | S3 | Bus-off 상태 감지 (오류 카운터) | Bus-off 복구 절차, 안전 상태 전이 |
| UDP 원격 통신 (Remote) | 패킷 손실 (LTE) | 원격 명령 미수신 | 원격 제어 불가 | S1 | 수신 타임아웃, 연속 실패 카운터 | N회 연속 실패 시 안전 정지 명령 전송 |

### 3.5 Adaptive AUTOSAR Platform

| Item/Function | Failure Mode | Failure Effect (Local) | Failure Effect (System) | Severity | Detection Method | Recommended Action |
|---|---|---|---|---|---|---|
| EXEC 프로세스 관리 | 프로세스 비정상 종료 (crash) | 서비스 중단 | 안전 기능 비가용 | S2 | PHM alive supervision (SwRS-PHM-001) | 자동 재시작, Machine State 전이 |
| PHM 건강 감시 | Alive supervision 미동작 | 프로세스 행(hang) 미탐지 | 안전 메커니즘 무효화 | S3 | 이중 감시, 자기진단 | 독립 watchdog, PHM 자기진단 루프 |
| UCM 소프트웨어 업데이트 | 설치 실패, 패키지 손상 | 잘못된 SW 버전 실행 | 안전 기능 오동작 | S2 | 패키지 서명 검증 (SwRS-UCM-002), 무결성 체크 | 설치 실패 시 Rollback (SwRS-UCM-001) |

## 4. Summary of Findings

### 4.1 Critical Failure Modes (S3)

| ID | Module | Failure Mode | Primary Countermeasure |
|---|---|---|---|
| FM-AEB-001 | AEB Control | 판단 지연 | Watchdog, WCET 분석 |
| FM-AEB-003 | AEB Control | 미탐 (False Negative) | 이중 센서 경로 |
| FM-AEB-004 | AEB Control | CAN 메시지 손실 | 재전송 + Fail-safe |
| FM-AEB-008 | AEB Control | Watchdog 미동작 | HW/SW 이중 감시 |
| FM-CTL-001 | Control Module | 조향 명령 오류 | 범위 제한 + 편차 감시 |
| FM-DEC-002 | Decision Module | 보행자 미감지 | 다중 센서 퓨전 |
| FM-CAN-003 | Communication | CAN Bus-off | 복구 절차 + 안전 상태 |
| FM-ARA-002 | Adaptive AUTOSAR | PHM 미동작 | 독립 watchdog |

### 4.2 Risk Mitigation Status

| Risk Level | Count | Mitigated | Remaining |
|---|---|---|---|
| S3 (Critical) | 8 | 8 (설계 대책 수립) | 0 |
| S2 (Major) | 10 | 10 (설계 대책 수립) | 0 |
| S1 (Minor) | 3 | 3 (설계 대책 수립) | 0 |

## 5. References

- ISO 26262:2018, Part 9, Clause 6
- [FTA](fta.md)
- [Dependent Failure Analysis](dependent-failure-analysis.md)
