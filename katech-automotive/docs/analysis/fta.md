# Fault Tree Analysis (FTA)

| Field | Value |
|---|---|
| **Document ID** | ANA-FTA-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 9, Clause 7 |
| **Author** | SuYeol Kim |
| **Reviewer** | - |
| **Approval Date** | 2026-04-06 |

---

## 1. Purpose

본 문서는 안전 목표 위반을 최상위 사건(Top Event)으로 하여, 그 원인을 논리적으로 분석하는 결함 트리 분석(FTA)을 수행한다.

## 2. Scope

- **Top Event 1**: SG-001 위반 — AEB 미작동 (긴급 제동 실패)
- **Top Event 2**: SG-002 위반 — 조향 제어 오류 (차선 이탈)
- **분석 수준**: Software FTA (SW 원인 중심)
- **대상 모듈**: AEB Control, Control Module, Decision Module, Common

## 3. Fault Trees

### 3.1 Top Event 1: AEB 미작동 (SG-001 위반)

```
                         [TE-001]
                    AEB 긴급 제동 실패
                          │
                     ┌────OR────┐
                     │           │
              [G-001]            [G-002]
          센서 데이터 고장      SW 처리 고장
                │                  │
           ┌────OR────┐       ┌────OR────┐
           │           │       │           │
      [G-003]     [G-004]  [G-005]     [G-006]
    LiDAR/Ibeo   Radar    판단 SW     CAN 전송
      데이터     데이터     오류        실패
       고장       고장       │           │
        │          │    ┌───OR───┐   ┌──OR──┐
        │          │    │         │   │       │
     [BE-001]  [BE-002] [BE-003] [BE-004] [BE-005] [BE-006]
     LiDAR     Radar    TTC 계산  설정 파일  CRC    Socket
     통신 단절  CRC 오류  알고리즘  파라미터   계산    전송
                         오류     오류      오류    오류
```

**Basic Events (기본 사건):**

| ID | Description | Module | SWREQ |
|---|---|---|---|
| BE-001 | LiDAR/Ibeo 센서 통신 단절 (CAN 타임아웃) | Decision | SWR-DEC-004 |
| BE-002 | Radar CAN 메시지 CRC 오류 | Decision | SWR-DEC-005 |
| BE-003 | TTC 계산 알고리즘 오류 (잘못된 분모, overflow) | AEB Control | SWREQ-AEB-001 |
| BE-004 | config.ini 파라미터 범위 초과 (TTC 임계값 비정상) | AEB Control | SWREQ-AEB-005 |
| BE-005 | CRC-16/CCITT 계산 오류 (다항식 불일치) | Common | SW-CMN-REQ-CRC-001 |
| BE-006 | CAN 소켓 전송 오류 (bus-off, 큐 overflow) | AEB Control | SWREQ-AEB-004 |

### 3.2 Top Event 2: 조향 제어 오류 (SG-002 위반)

```
                         [TE-002]
                    조향 제어 오류
                    (차선 이탈)
                          │
                     ┌────OR────┐
                     │           │
              [G-010]            [G-011]
          경로 데이터 오류      제어 알고리즘 오류
                │                  │
           ┌────OR────┐       ┌────OR────┐
           │           │       │           │
      [BE-010]    [BE-011]  [BE-012]   [BE-013]
      GPS 좌표    경로 계획   Pure Pursuit EPS CAN
       오류       데이터      계산 오류    통신 실패
                  타임아웃
```

**Basic Events:**

| ID | Description | Module | SWREQ |
|---|---|---|---|
| BE-010 | GPS 수신 데이터 오류 (좌표 점프, 정밀도 저하) | Decision | SWR-DEC-003 |
| BE-011 | Decision → Control 경로 데이터 CAN 타임아웃 | Control | SW-CTL-SR-023 |
| BE-012 | Pure Pursuit 알고리즘 look-ahead distance 오류 | Control | SW-CTL-SR-001 |
| BE-013 | EPS CAN 명령 전송 실패 (bus-off, 소켓 오류) | Control | SW-CTL-SR-031 |

## 4. Minimal Cut Sets

### 4.1 TE-001: AEB 미작동

| Cut Set | Basic Events | Order | Analysis |
|---|---|---|---|
| CS-001 | {BE-001} | 1 | LiDAR 단독 고장 시 AEB 판단 불가 (센서 이중화 없는 경우) |
| CS-002 | {BE-002} | 1 | Radar 단독 CRC 오류 시 유효 데이터 없음 |
| CS-003 | {BE-003} | 1 | TTC 알고리즘 오류 — 단일 고장점 (Single Point of Failure) |
| CS-004 | {BE-004} | 1 | 설정 파일 오류 — 기본값 fallback으로 완화 가능 |
| CS-005 | {BE-005} | 1 | CRC 연산 오류 — 모든 E2E 검증 무효화 (공통 원인) |
| CS-006 | {BE-006} | 1 | CAN 전송 오류 — 재전송 메커니즘으로 완화 |

### 4.2 TE-002: 조향 제어 오류

| Cut Set | Basic Events | Order | Analysis |
|---|---|---|---|
| CS-010 | {BE-010} | 1 | GPS 오류 — 경로 플라우시빌리티 체크로 완화 |
| CS-011 | {BE-011} | 1 | 경로 타임아웃 — 안전 상태 전이(감속→정지)로 완화 |
| CS-012 | {BE-012} | 1 | Pure Pursuit 오류 — 조향각 범위 제한으로 완화 |
| CS-013 | {BE-013} | 1 | EPS 통신 실패 — Fail-safe 전이로 완화 |

### 4.3 Single Point of Failure Analysis

| SPOF | Severity | Mitigation | Residual Risk |
|---|---|---|---|
| BE-003 (TTC 알고리즘) | S3 | UT 100% MC/DC, 코드 리뷰, 정적 분석 | Low (SW 검증으로 대응) |
| BE-005 (CRC 연산) | S2 | UT 테스트 벡터 검증, 다항식 상수 확인 | Low (검증된 알고리즘) |
| BE-001 (LiDAR 단절) | S3 | Radar 교차 검증 (이중화 시 2차 절단집합) | Medium (이중화 구현 시 Low) |

## 5. Quantitative Analysis

| Basic Event | Estimated Failure Rate (FIT) | Source |
|---|---|---|
| BE-001 (LiDAR 통신 단절) | 100 | 센서 제조사 데이터시트 (추정) |
| BE-002 (Radar CRC 오류) | 10 | CAN 프로토콜 BER 기반 (추정) |
| BE-003 (TTC 알고리즘 오류) | 1 | SW 결함률 (검증 후 추정) |
| BE-004 (설정 파일 오류) | 5 | 운용 오류율 (추정) |
| BE-005 (CRC 연산 오류) | 0.1 | 검증된 알고리즘 잔여 결함 (추정) |
| BE-006 (CAN 전송 오류) | 50 | CAN 프로토콜 에러율 (추정) |

**Note**: FIT = Failures In Time (10^9 hours 당 고장 횟수). 상기 수치는 초기 추정치이며, 실제 시험 및 필드 데이터로 갱신 필요.

## 6. Findings and Recommendations

### 6.1 주요 발견사항

1. **단일 고장점 존재**: TTC 알고리즘 (BE-003)과 CRC 연산 (BE-005)은 단일 고장점임. SW 검증(UT/IT/정적분석)으로 발생 확률을 최소화해야 함.
2. **센서 이중화 필요**: LiDAR 단독 의존 시 1차 절단집합이 존재. Radar + LiDAR + Vision 교차 검증 시 2차 이상으로 격상.
3. **Common 모듈 공통 원인**: CrcProvider와 SignalCodec은 모든 안전 모듈에서 공유되어 공통 원인 고장 가능성 있음 → DFA에서 상세 분석.

### 6.2 권고사항

| Priority | Recommendation | Related SWREQ |
|---|---|---|
| 높음 | TTC 알고리즘 MC/DC 100% 단위 테스트 | SWREQ-AEB-001 |
| 높음 | CRC 연산 테스트 벡터 검증 (UT-CRC-001) | SW-CMN-REQ-CRC-001 |
| 높음 | 센서 퓨전 교차 검증 로직 구현 | SWR-DEC-002 |
| 중간 | 설정 파일 범위 검증 및 기본값 fallback | SWREQ-AEB-005 |
| 중간 | CAN 재전송 메커니즘 구현 | SWREQ-AEB-004 |
| 낮음 | 정량적 고장률 데이터 갱신 (필드 데이터) | - |

## 7. References

- ISO 26262:2018, Part 9, Clause 7
- [FMEA](fmea.md)
- [Dependent Failure Analysis](dependent-failure-analysis.md)
