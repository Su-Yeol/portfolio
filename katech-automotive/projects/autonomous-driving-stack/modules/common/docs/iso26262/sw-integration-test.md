# SW-CMN-IT-001: Software Integration Test Specification

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-IT-001                                                       |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 통합 테스트 명세서                               |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 10 — Software integration and testing                |
| **ASIL 등급 (ASIL)**       | ASIL-B (상위 모듈 상속) <!-- TODO: 시스템 안전 분석 후 확정 -->          |
| **모듈 (Module)**          | Common (Shared Utilities)                                           |
| **상위 프로젝트 (Project)** | Autonomous Driving Stack                                             |
| **Target HW**             | NXP S32G (aarch64)                                                  |
| **작성자 (Author)**        | <!-- TODO: 작성자 기입 -->                                            |
| **검토자 (Reviewer)**      | <!-- TODO: 검토자 기입 -->                                            |
| **승인자 (Approver)**      | <!-- TODO: 승인자 기입 -->                                            |
| **버전 (Version)**         | 0.1 (Draft)                                                         |
| **작성일 (Date)**          | 2026-04-03                                                          |
| **상태 (Status)**          | Draft                                                               |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262-6:2018 Clause 10에 따라 Common 모듈의 소프트웨어 통합 테스트 계획 및 명세를 기술한다.
Common 라이브러리 내부 컴포넌트 간 통합, 그리고 Common 라이브러리를 소비하는 상위 모듈
(AEB Control, Control Module, Decision Module)과의 통합을 검증한다.

---

## 2. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Part 6, Clause 10 — Software integration and testing     |
| SW-CMN-REQ-001  | Common 모듈 소프트웨어 안전 요구사항 명세서                  |
| SW-CMN-ARCH-001 | Common 모듈 소프트웨어 아키텍처 설계서                       |
| SW-CMN-UT-001   | Common 모듈 소프트웨어 단위 테스트 명세서                    |
| <!-- TODO -->   | AEB Control, Control, Decision 모듈 통합 테스트 문서 참조    |

---

## 3. 통합 전략 (Integration Strategy) — ISO 26262-6 Clause 10.4.2

### 3.1 통합 방식

Common 모듈은 정적 링크 라이브러리이므로 **상향식(bottom-up) 통합** 전략을 적용한다:

1. **Level 1 — 내부 통합:** ConfigParser + CanSpecConfigLoader 통합
2. **Level 2 — 라이브러리 통합:** CrcProvider + SignalCodec + CanSpecProvider 전체 라이브러리 빌드 검증
3. **Level 3 — 소비 모듈 통합:** Common 라이브러리와 상위 모듈(AEB Control, Control, Decision) 간 링크 및 호출 검증

### 3.2 통합 순서

```
ConfigParser ──┐
               ├─► CanSpecConfigLoader ──┐
can_spec.ini ──┘                         │
                                         ├─► Common Library (.a)
CrcProviderPublic ───────────────────────┤    (static link)
CrcProviderPrivate ──────────────────────┤
SignalCodec (header-only) ───────────────┘
                                         │
                    ┌────────────────────┤
                    ▼                    ▼                    ▼
             AEB Control          Control Module       Decision Module
```

---

## 4. 테스트 환경 (Test Environment)

| 항목                  | 내용                                                        |
|----------------------|-------------------------------------------------------------|
| 호스트 환경            | x86_64 Linux — 크로스 모듈 호출 검증                           |
| 타겟 환경             | NXP S32G (aarch64) — 실제 타겟 통합 검증                       |
| 테스트 프레임워크      | <!-- TODO: Google Test / Catch2 등 선정 -->                   |
| 빌드 시스템            | <!-- TODO: CMake / Makefile 구성 명시 -->                     |
| CI/CD                | <!-- TODO: 통합 테스트 자동화 파이프라인 구성 -->                 |

---

## 5. 통합 테스트 방법 (Test Methods) — ISO 26262-6 Table 11

| 방법                                              | ASIL B 권장  | 적용 여부 |
|--------------------------------------------------|-------------|----------|
| 인터페이스 테스트 (Interface testing)                | 강력 추천     | 예       |
| 요구사항 기반 테스트 (Requirements-based)             | 강력 추천     | 예       |
| 결함 주입 테스트 (Fault injection)                   | 추천         | 예       |
| 리소스 사용 테스트 (Resource usage)                  | 추천         | <!-- TODO: 적용 여부 결정 --> |
| 백투백 테스트 (Back-to-back testing)                | 추천         | <!-- TODO: 적용 여부 결정 --> |

---

## 6. 통합 테스트 케이스 명세 (Integration Test Cases)

### 6.1 Level 1 — 내부 통합 (ConfigParser + CanSpecConfigLoader)

| TC ID               | 테스트 설명                                              | 전제 조건                    | 기대 결과                         | 검증 요구사항         |
|--------------------|--------------------------------------------------------|-----------------------------|---------------------------------|--------------------|
| IT-INT-001         | `can_spec.ini` 정상 로드 후 `GetU32` 값 조회              | 유효한 `can_spec.ini` 존재    | INI 정의 값과 일치                  | SW-CMN-REQ-CFG-001 |
| IT-INT-002         | `can_spec.ini` 미존재 시 전체 파이프라인 에러 처리           | INI 파일 제거                 | 안전한 에러 처리, fallback 동작     | SW-CMN-REQ-CFG-002 |
| IT-INT-003         | 대용량 INI 파일 로드 및 다수 키 조회 성능                    | 1000+ 키 INI 파일            | 정상 로드 및 응답                   | SW-CMN-REQ-CFG-001 |
| IT-INT-004         | `ActiveConfigPath()` 경로와 실제 로드 파일 일관성           | 정상 로드 후                   | 로드된 파일 경로 일치               | SW-CMN-REQ-CFG-004 |

### 6.2 Level 2 — 라이브러리 전체 빌드 통합

| TC ID               | 테스트 설명                                              | 전제 조건                    | 기대 결과                         | 검증 요구사항              |
|--------------------|--------------------------------------------------------|-----------------------------|---------------------------------|-------------------------|
| IT-LIB-001         | Public 빌드(`USE_PRIVATE_IMPL=0`) 라이브러리 링크 검증     | 기본 빌드 설정                | 정상 빌드, 모든 심볼 해결            | SW-CMN-REQ-GEN-001     |
| IT-LIB-002         | Private 빌드(`USE_PRIVATE_IMPL=1`) 라이브러리 링크 검증    | Private 빌드 설정             | 정상 빌드, Private 심볼 포함         | SW-CMN-REQ-GEN-001     |
| IT-LIB-003         | Public/Private 빌드 간 공개 API 인터페이스 동일성 검증       | 양 빌드 결과물                 | 동일한 public API 심볼 세트         | SW-CMN-REQ-GEN-003     |
| IT-LIB-004         | CRC 계산 후 SignalCodec으로 결과 인코딩 — 연쇄 호출 검증     | 정상 빌드                     | CRC 값이 올바르게 패킹됨             | SW-CMN-REQ-CRC-001, SIG-001 |
| IT-LIB-005         | CanSpec에서 설정 로드 후 CRC seed로 사용 — 연쇄 호출 검증    | 유효한 INI 파일               | 설정값 기반 CRC 계산 정상            | SW-CMN-REQ-CFG-001, CRC-004 |

### 6.3 Level 3 — 소비 모듈 통합

| TC ID               | 테스트 설명                                                     | 전제 조건                          | 기대 결과                         | 검증 요구사항         |
|--------------------|---------------------------------------------------------------|-----------------------------------|---------------------------------|--------------------|
| IT-MOD-001         | AEB Control 모듈에서 Common CRC API 호출 검증                     | AEB + Common 링크                  | CRC 정상 계산                     | SW-CMN-REQ-CRC-001 |
| IT-MOD-002         | Control Module에서 SignalCodec 패킹/언패킹 호출 검증               | Control + Common 링크              | 신호 인코딩/디코딩 정확             | SW-CMN-REQ-SIG-001 |
| IT-MOD-003         | Decision Module에서 CanSpec 설정 조회 검증                        | Decision + Common 링크             | 설정 값 정상 조회                   | SW-CMN-REQ-CFG-001 |
| IT-MOD-004         | 모든 모듈 동시 링크 시 심볼 충돌 없음 검증                           | 전체 빌드                          | 링크 에러 없음                     | SW-CMN-REQ-GEN-001 |
| IT-MOD-005         | NXP S32G 타겟 크로스 컴파일 전체 통합 빌드                          | aarch64 크로스 컴파일 환경           | 정상 빌드 및 바이너리 생성           | SW-CMN-REQ-GEN-001 |
| <!-- TODO -->      | 각 소비 모듈과의 상세 통합 시나리오 추가                              |                                   |                                 |                    |

### 6.4 결함 주입 테스트 (Fault Injection)

| TC ID               | 테스트 설명                                              | 주입 결함                    | 기대 결과                         | 검증 요구사항         |
|--------------------|--------------------------------------------------------|-----------------------------|---------------------------------|--------------------|
| IT-FI-001          | CRC 계산 입력 데이터 1비트 변조 후 검증                      | payload 1비트 플립            | 기존 CRC와 불일치 검출              | SW-CMN-REQ-CRC-001 |
| IT-FI-002          | INI 파일 내용 손상 후 로드                                 | INI 파일 바이너리 변조         | 에러 처리 또는 fallback 동작        | SW-CMN-REQ-CFG-002 |
| IT-FI-003          | CAN 신호 데이터 바이트 순서 변조                             | 엔디안 역전                   | 디코딩 결과 불일치 검출              | SW-CMN-REQ-SIG-003 |
| <!-- TODO -->      | 추가 결함 주입 시나리오 도출                                 |                             |                                 |                    |

---

## 7. 합격/불합격 기준 (Pass/Fail Criteria)

| 기준                                    | 목표                                |
|----------------------------------------|-------------------------------------|
| 모든 통합 테스트 케이스 통과               | 100% pass                          |
| 인터페이스 관련 결함                      | 0건 (Critical/High)                 |
| Public/Private 빌드 호환성               | 100% API 일치                       |
| 타겟(NXP S32G) 빌드 성공                 | 빌드 에러 0건                        |
| <!-- TODO -->                          | 추가 합격 기준 정의                    |

---

## 8. 통합 테스트 수행 절차 (Execution Procedure)

1. Level 1 내부 통합 테스트 수행 → 결과 기록
2. Level 2 라이브러리 전체 빌드 통합 테스트 수행 → 결과 기록
3. Level 3 소비 모듈 통합 테스트 수행 → 결과 기록
4. 결함 주입 테스트 수행 → 결과 기록
5. 불합격 항목 결함 분석 → 수정 후 재테스트
6. 통합 테스트 보고서 작성 → SW-CMN-VR-001에 반영

---

## 9. TODO 요약

- [ ] 테스트 프레임워크 및 CI/CD 파이프라인 구성
- [ ] 소비 모듈별 상세 통합 시나리오 추가
- [ ] 리소스 사용 테스트 / 백투백 테스트 적용 여부 결정
- [ ] 결함 주입 시나리오 추가 도출
- [ ] 타겟 환경 통합 테스트 실행 절차 상세화
- [ ] INI 파일 미존재/손상 시 기대 동작 확정
- [ ] 통합 테스트 결과 보고서 템플릿 준비
- [ ] 작성자/검토자/승인자 기입 및 리뷰 수행
