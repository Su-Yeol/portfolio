# Software Verification Report

## Control Module - Lateral/Longitudinal Vehicle Control

---

| 항목 (Field)          | 내용 (Value)                                              |
|-----------------------|-----------------------------------------------------------|
| **Document ID**       | SW-CTL-VR-001                                             |
| **Version**           | 0.1 Draft                                                 |
| **ISO 26262 Reference** | Part 6, Clause 11 — Software Unit and Integration Verification |
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

본 문서는 ISO 26262 Part 6, Clause 11에 따라 Control Module의 소프트웨어 검증 결과를 종합 보고한다. 유닛 테스트(SW-CTL-UT-001), 통합 테스트(SW-CTL-IT-001) 및 기타 검증 활동의 결과를 취합하여 소프트웨어 안전 요구사항(SW-CTL-REQ-001)의 충족 여부를 판정한다.

## 2. 범위 (Scope)

- 소프트웨어 유닛 테스트 결과 검증
- 소프트웨어 통합 테스트 결과 검증
- 구조적 커버리지 분석 결과
- 정적 분석 결과
- 코드 리뷰 결과
- 안전 요구사항 충족 판정
- 잔존 위험 분석

## 3. 참조 문서 (Reference Documents)

| 문서 ID           | 문서명                                        | 비고                    |
|-------------------|-----------------------------------------------|-------------------------|
| SW-CTL-REQ-001    | Software Safety Requirements Specification    | 안전 요구사항 문서      |
| SW-CTL-ARCH-001   | Software Architecture Description             | 아키텍처 설계 문서      |
| SW-CTL-UD-001     | Software Unit Design Specification            | 유닛 설계 문서          |
| SW-CTL-UT-001     | Software Unit Test Specification              | 유닛 테스트 문서        |
| SW-CTL-IT-001     | Software Integration Test Specification       | 통합 테스트 문서        |

## 4. 검증 활동 요약 (Verification Activities Summary)

### 4.1 검증 활동 목록

| 활동                            | ISO 26262 참조        | 수행 여부   | 비고                        |
|---------------------------------|-----------------------|-------------|-----------------------------|
| 소프트웨어 유닛 테스트          | Part 6, Clause 9      | <!-- TODO --> | SW-CTL-UT-001 참조        |
| 소프트웨어 통합 테스트          | Part 6, Clause 10     | <!-- TODO --> | SW-CTL-IT-001 참조        |
| 구조적 커버리지 분석            | Part 6, Clause 9      | <!-- TODO --> | MC/DC 포함                |
| 정적 코드 분석                  | Part 6, Clause 8      | <!-- TODO --> | <!-- TODO: 도구명 -->     |
| 코드 리뷰 / 워크스루           | Part 6, Clause 8      | <!-- TODO --> | <!-- TODO: 리뷰 기록 -->  |
| 요구사항 기반 테스트 완전성 검토 | Part 6, Clause 9/10   | <!-- TODO --> | 추적 매트릭스 기반        |
| Back-to-back 테스트             | Part 6, Clause 9      | <!-- TODO --> | MIL/SIL/HIL 비교          |

### 4.2 검증 환경

| 항목                  | 내용                                                    |
|-----------------------|---------------------------------------------------------|
| 빌드 버전             | <!-- TODO: 검증 대상 빌드 버전/해시 -->                 |
| 빌드 산출물           | `build/run_control`                                     |
| 타겟 HW               | NXP S32G (aarch64)                                     |
| 테스트 프레임워크     | <!-- TODO: e.g., Google Test -->                        |
| 정적 분석 도구        | <!-- TODO: e.g., cppcheck, Polyspace, LDRA -->          |
| 커버리지 도구         | <!-- TODO: e.g., gcov, lcov -->                         |
| CAN 시뮬레이터        | <!-- TODO -->                                           |

## 5. 유닛 테스트 결과 (Unit Test Results)

### 5.1 테스트 실행 결과 요약

<!-- TODO: 실제 테스트 실행 후 결과 기입 -->

| 유닛                     | 총 TC | Pass | Fail | N/A  | 통과율  |
|--------------------------|-------|------|------|------|---------|
| LateralController        | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| LongitudinalController   | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| SafetyMonitor            | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| InputManager             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| OutputManager            | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| **합계**                 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |

### 5.2 구조적 커버리지 결과

<!-- TODO: 커버리지 측정 결과 기입 -->

| 유닛                     | Statement | Branch | MC/DC  | 판정    |
|--------------------------|-----------|--------|--------|---------|
| LateralController        | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> |
| LongitudinalController   | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> |
| SafetyMonitor            | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> |
| InputManager             | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> |
| OutputManager            | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> % | <!-- TODO --> |

**커버리지 목표 (ASIL D)**: Statement 100%, Branch 100%, MC/DC 100%

### 5.3 미달 커버리지 분석

<!-- TODO: 커버리지 미달 항목에 대한 정당화 근거 기술 -->

| 유닛 / 파일              | 미커버 코드 영역            | 사유                          | 수용 근거                |
|--------------------------|-----------------------------|-------------------------------|--------------------------|
| <!-- TODO -->            | <!-- TODO -->               | <!-- TODO -->                 | <!-- TODO -->            |

## 6. 통합 테스트 결과 (Integration Test Results)

### 6.1 테스트 실행 결과 요약

<!-- TODO: 실제 테스트 실행 후 결과 기입 -->

| 통합 단계                | 총 TC | Pass | Fail | N/A  | 통과율  |
|--------------------------|-------|------|------|------|---------|
| Phase 1: 내부 통합       | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| Phase 2: 플랫폼 통합     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| Phase 3: 외부 통합       | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| 결함 주입 테스트         | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |
| **합계**                 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> % |

### 6.2 타이밍 검증 결과

<!-- TODO: 타이밍 측정 결과 기입 -->

| 측정 항목                      | 목표값              | 측정값              | 판정       |
|--------------------------------|---------------------|---------------------|------------|
| 제어 루프 주기 평균            | <!-- TODO --> ms    | <!-- TODO --> ms    | <!-- TODO --> |
| 제어 루프 주기 jitter (max)    | ±<!-- TODO --> ms   | <!-- TODO --> ms    | <!-- TODO --> |
| 제어 연산 WCET                 | <!-- TODO --> ms    | <!-- TODO --> ms    | <!-- TODO --> |
| CAN 전송 지연 (max)            | <!-- TODO --> ms    | <!-- TODO --> ms    | <!-- TODO --> |
| E2E 지연 (Input → CAN Tx)     | <!-- TODO --> ms    | <!-- TODO --> ms    | <!-- TODO --> |
| 안전 상태 전이 시간 (max)      | <!-- TODO --> ms    | <!-- TODO --> ms    | <!-- TODO --> |

### 6.3 자원 사용량 검증 결과

<!-- TODO: 자원 사용량 측정 결과 기입 -->

| 자원 항목                | 한계값              | 측정값              | 판정       |
|--------------------------|---------------------|---------------------|------------|
| CPU 사용률 (peak)        | <!-- TODO --> %     | <!-- TODO --> %     | <!-- TODO --> |
| RAM 사용량               | <!-- TODO --> KB    | <!-- TODO --> KB    | <!-- TODO --> |
| 스택 사용량 (peak)       | <!-- TODO --> KB    | <!-- TODO --> KB    | <!-- TODO --> |
| CAN 버스 부하율          | <!-- TODO --> %     | <!-- TODO --> %     | <!-- TODO --> |

## 7. 정적 분석 결과 (Static Analysis Results)

<!-- TODO: 정적 분석 실행 후 결과 기입 -->

### 7.1 정적 분석 도구 및 룰셋

| 도구                     | 버전           | 적용 룰셋                              |
|--------------------------|----------------|----------------------------------------|
| <!-- TODO: 도구명 -->    | <!-- TODO -->  | <!-- TODO: e.g., MISRA C++ 2023 -->    |
| <!-- TODO: 도구명 -->    | <!-- TODO -->  | <!-- TODO: e.g., CERT C++ -->          |

### 7.2 정적 분석 결과 요약

| 심각도          | 발견 수    | 해결 수    | 미해결 수  | 수용 처리 수 |
|-----------------|------------|------------|------------|--------------|
| Critical        | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Major           | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Minor           | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Info            | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 7.3 MISRA 준수 현황

<!-- TODO: MISRA 준수 현황 기입 -->

- 적용 룰셋: <!-- TODO: MISRA C++:2023 / MISRA C++:2008 -->
- 총 위반 수: <!-- TODO -->
- 승인된 편차(Deviation) 수: <!-- TODO -->
- 미해결 위반 수: <!-- TODO -->

## 8. 코드 리뷰 결과 (Code Review Results)

<!-- TODO: 코드 리뷰 결과 기입 -->

| 리뷰 대상               | 리뷰 일자      | 리뷰어             | 발견 사항 수 | 해결 상태  |
|--------------------------|----------------|---------------------|-------------|------------|
| LateralController        | <!-- TODO -->  | <!-- TODO -->       | <!-- TODO --> | <!-- TODO --> |
| LongitudinalController   | <!-- TODO -->  | <!-- TODO -->       | <!-- TODO --> | <!-- TODO --> |
| SafetyMonitor            | <!-- TODO -->  | <!-- TODO -->       | <!-- TODO --> | <!-- TODO --> |
| InputManager             | <!-- TODO -->  | <!-- TODO -->       | <!-- TODO --> | <!-- TODO --> |
| OutputManager            | <!-- TODO -->  | <!-- TODO -->       | <!-- TODO --> | <!-- TODO --> |

## 9. 요구사항 충족 판정 (Requirements Fulfillment Assessment)

### 9.1 안전 요구사항 충족 매트릭스

<!-- TODO: 각 안전 요구사항의 검증 결과 종합 판정 -->

| 요구사항 ID     | 요구사항 요약                          | UT 결과 | IT 결과 | 정적 분석 | 리뷰 | 종합 판정   |
|-----------------|----------------------------------------|---------|---------|-----------|------|-------------|
| SW-CTL-SR-001   | 횡방향 편차 허용 범위 유지             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-002   | 조향각 물리적 한계 제한                | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-003   | 조향각 변화율 제한                     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-004   | EPS 통신 두절 시 안전 전이             | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-010   | 종방향 속도 편차 허용 범위 유지        | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-011   | 최대 가속도 제한                       | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-012   | 최대 감속도 제한                       | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-013   | 비상 제동 응답 시간                    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-020   | 제어 루프 주기 및 jitter               | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-021   | 제어 연산 WCET 제한                    | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-022   | CAN 전송 지연 제한                     | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-023   | 입력 타임아웃 안전 전이                | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-030   | CAN alive counter / CRC               | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| SW-CTL-SR-031   | CAN bus-off 안전 전이                  | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

### 9.2 미충족 요구사항 분석

<!-- TODO: 미충족 요구사항이 있는 경우 원인 분석 및 대책 기술 -->

| 요구사항 ID     | 미충족 사유                            | 위험 평가          | 대책                        | 목표 일정       |
|-----------------|----------------------------------------|--------------------|-----------------------------|-----------------|
| <!-- TODO -->   | <!-- TODO -->                          | <!-- TODO -->      | <!-- TODO -->               | <!-- TODO -->   |

## 10. 결함 관리 (Defect Management)

### 10.1 발견 결함 요약

<!-- TODO: 검증 과정에서 발견된 결함 목록 -->

| 결함 ID        | 심각도   | 발견 단계      | 설명                          | 상태       | 해결 버전   |
|----------------|----------|----------------|-------------------------------|------------|-------------|
| <!-- TODO -->  | <!-- TODO --> | <!-- TODO --> | <!-- TODO -->                | <!-- TODO --> | <!-- TODO --> |

### 10.2 결함 통계

| 항목                        | 수량                |
|-----------------------------|---------------------|
| 총 발견 결함                | <!-- TODO -->       |
| 해결 완료                   | <!-- TODO -->       |
| 미해결 (open)               | <!-- TODO -->       |
| 재현 불가 (not reproducible)| <!-- TODO -->       |

## 11. 잔존 위험 분석 (Residual Risk Analysis)

<!-- TODO: 검증 후 잔존하는 위험 요소 분석 -->

| 위험 항목                    | 설명                                    | 위험 수준   | 완화 조치                    |
|------------------------------|-----------------------------------------|-------------|------------------------------|
| <!-- TODO -->                | <!-- TODO -->                           | <!-- TODO -->| <!-- TODO -->               |

## 12. 검증 판정 (Verification Verdict)

<!-- TODO: 최종 검증 판정 기입 -->

| 판정 항목                              | 결과                |
|----------------------------------------|---------------------|
| 유닛 테스트 합격 판정                  | <!-- TODO: Pass / Fail / Conditional --> |
| 통합 테스트 합격 판정                  | <!-- TODO: Pass / Fail / Conditional --> |
| 구조적 커버리지 충족 판정              | <!-- TODO: Pass / Fail / Conditional --> |
| 정적 분석 합격 판정                    | <!-- TODO: Pass / Fail / Conditional --> |
| 안전 요구사항 충족 종합 판정           | <!-- TODO: Pass / Fail / Conditional --> |
| **소프트웨어 검증 최종 판정**          | **<!-- TODO: Pass / Fail / Conditional -->** |

**판정 근거**: <!-- TODO: 최종 판정 근거 요약 기술 -->

**판정자**: <!-- TODO: 판정자 이름 및 직책 -->

**판정 일자**: <!-- TODO: YYYY-MM-DD -->

## 13. 변경 이력 (Change History)

| Version | Date       | Author        | Description          |
|---------|------------|---------------|----------------------|
| 0.1     | <!-- TODO: 날짜 --> | <!-- TODO: 작성자 --> | Initial draft |
