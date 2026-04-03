# SW-CMN-UT-001: Software Unit Test Specification

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-UT-001                                                       |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 단위 테스트 명세서                               |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 9 — Software unit testing                            |
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

본 문서는 ISO 26262-6:2018 Clause 9에 따라 Common 모듈의 소프트웨어 단위 테스트 계획 및 명세를 기술한다.
각 소프트웨어 단위가 SW-CMN-UD-001에 정의된 상세 설계 및 SW-CMN-REQ-001의 안전 요구사항을
올바르게 구현하였는지 검증한다.

---

## 2. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Part 6, Clause 9 — Software unit testing                 |
| SW-CMN-REQ-001  | Common 모듈 소프트웨어 안전 요구사항 명세서                  |
| SW-CMN-UD-001   | Common 모듈 소프트웨어 단위 설계 명세서                      |
| SW-CMN-ARCH-001 | Common 모듈 소프트웨어 아키텍처 설계서                       |

---

## 3. 테스트 환경 (Test Environment)

| 항목                  | 내용                                                        |
|----------------------|-------------------------------------------------------------|
| 테스트 프레임워크      | <!-- TODO: Google Test / Catch2 등 선정 -->                   |
| 호스트 빌드 환경       | x86_64 Linux (호스트 단위 테스트)                               |
| 타겟 빌드 환경         | NXP S32G (aarch64) — 크로스 컴파일 후 타겟 실행               |
| 컴파일러              | <!-- TODO: GCC/Clang 버전 명시 -->                            |
| 코드 커버리지 도구     | <!-- TODO: gcov / lcov / BullseyeCoverage 등 선정 -->         |
| 정적 분석 도구         | <!-- TODO: cppcheck / Polyspace / PC-lint 등 선정 -->         |
| 모킹 프레임워크        | <!-- TODO: Google Mock 등 선정 -->                            |

---

## 4. 테스트 방법론 (Test Methods) — ISO 26262-6 Table 9

| 방법                                          | ASIL B 권장  | 적용 여부 |
|----------------------------------------------|-------------|----------|
| 요구사항 기반 테스트 (Requirements-based)        | 강력 추천     | 예       |
| 인터페이스 테스트 (Interface testing)            | 강력 추천     | 예       |
| 경계값 분석 (Boundary value analysis)           | 강력 추천     | 예       |
| 등가 분할 (Equivalence partitioning)            | 추천         | 예       |
| 에러 추측 (Error guessing)                     | 추천         | 예       |
| <!-- TODO: 추가 방법론 적용 여부 결정 -->         |             |          |

### 구조적 커버리지 목표 (Structural Coverage) — ISO 26262-6 Table 10

| 커버리지 기준                    | ASIL B 권장  | 목표치              |
|-------------------------------|-------------|---------------------|
| Statement coverage            | 강력 추천     | >= 90% <!-- TODO: 확정 --> |
| Branch coverage (Decision)    | 강력 추천     | >= 80% <!-- TODO: 확정 --> |
| MC/DC                         | 추천         | <!-- TODO: 적용 여부 결정 --> |

---

## 5. 단위 테스트 케이스 명세 (Unit Test Cases)

### 5.1 CMN-U-CRC-PUB: CrcProviderPublic

| TC ID               | 테스트 설명                                              | 입력                                           | 기대 출력                    | 검증 요구사항         | 우선순위 |
|--------------------|--------------------------------------------------------|------------------------------------------------|-----------------------------|--------------------|---------|
| UT-CRC-001         | 알려진 테스트 벡터로 CRC-16/CCITT 계산 정확성 검증            | 표준 테스트 벡터 payload, seed=0xFFFF             | 표준 기대 CRC 값              | SW-CMN-REQ-CRC-001 | High    |
| UT-CRC-002         | Data ID(low/high)가 CRC 결과에 정확히 반영되는지 검증         | 동일 payload, 다른 data_id_l/h                   | 서로 다른 CRC 값              | SW-CMN-REQ-CRC-004 | High    |
| UT-CRC-003         | seed 값 변경에 따른 CRC 결과 변화 검증                       | 동일 payload, 다른 seed                          | 서로 다른 CRC 값              | SW-CMN-REQ-CRC-004 | High    |
| UT-CRC-004         | 빈 payload (len=0) 처리                                  | payload=valid_ptr, len=0                       | 정의된 동작 수행               | SW-CMN-REQ-CRC-002 | High    |
| UT-CRC-005         | NULL payload 에러 처리                                    | payload=NULL, len>0                            | <!-- TODO: 에러 처리 정의 --> | SW-CMN-REQ-CRC-002 | High    |
| UT-CRC-006         | 최대 길이 payload 경계값 테스트                              | payload_len = MAX                              | 올바른 CRC 값                 | SW-CMN-REQ-CRC-001 | Medium  |
| UT-CRC-007         | `GetCrcProvider()` 싱글턴 반환 일관성                       | 두 번 호출                                       | 동일 참조 반환                 | SW-CMN-REQ-CRC-003 | Medium  |
| <!-- TODO -->      | Private CRC Provider 테스트 케이스 추가                     |                                                |                             |                    |         |

### 5.2 CMN-U-SIG: SignalCodec

| TC ID               | 테스트 설명                                              | 입력                                           | 기대 출력                    | 검증 요구사항         | 우선순위 |
|--------------------|--------------------------------------------------------|------------------------------------------------|-----------------------------|--------------------|---------|
| UT-SIG-001         | `PackField` 기본 동작 — 단일 필드 패킹                     | value=0x03, mask=0x0C, shift=2                 | 0x0C                        | SW-CMN-REQ-SIG-001 | High    |
| UT-SIG-002         | `Pack2` — 두 필드 OR 결합                                | v1=1,m1=0x01,s1=0; v2=1,m2=0x02,s2=1          | 0x03                        | SW-CMN-REQ-SIG-001 | High    |
| UT-SIG-003         | `Pack3` — 세 필드 OR 결합                                | 3개 필드 조합                                    | 결합된 바이트                  | SW-CMN-REQ-SIG-001 | High    |
| UT-SIG-004         | `ExtractField` — PackField의 역연산 검증                  | PackField 출력을 입력으로                         | 원본 value 복원               | SW-CMN-REQ-SIG-002 | High    |
| UT-SIG-005         | `JoinU16Le` 리틀 엔디안 결합                              | low=0x34, high=0x12                            | 0x1234                      | SW-CMN-REQ-SIG-003 | High    |
| UT-SIG-006         | `JoinU32Le` 리틀 엔디안 결합                              | b0=0x78, b1=0x56, b2=0x34, b3=0x12            | 0x12345678                  | SW-CMN-REQ-SIG-003 | High    |
| UT-SIG-007         | `JoinI32Le` 음수값 처리                                   | 0xFF로 구성된 32비트                              | -1 (signed)                 | SW-CMN-REQ-SIG-003 | High    |
| UT-SIG-008         | `ComposeU16` 부분 비트 결합                               | 다양한 mask/shift 조합                            | 올바른 16비트 결합             | SW-CMN-REQ-SIG-005 | High    |
| UT-SIG-009         | `Byte0` ~ `Byte3` 바이트 추출                            | 0x12345678                                     | 0x78, 0x56, 0x34, 0x12     | SW-CMN-REQ-SIG-001 | Medium  |
| UT-SIG-010         | `WriteU16Le` 버퍼 기록 검증                               | buffer[4], offset=1, value=0xABCD              | buffer[1]=0xCD, [2]=0xAB   | SW-CMN-REQ-SIG-004 | High    |
| UT-SIG-011         | `WriteU32Le` 버퍼 기록 검증                               | buffer[8], offset=0, value=0x12345678          | LE 순서로 4바이트 기록         | SW-CMN-REQ-SIG-004 | High    |
| UT-SIG-012         | 경계값: mask=0xFF, shift=0 (전체 바이트)                   | `PackField(0xFF, 0xFF, 0)`                     | 0xFF                        | SW-CMN-REQ-SIG-001 | Medium  |
| UT-SIG-013         | 경계값: mask=0x00 (빈 마스크)                             | `PackField(0xFF, 0x00, 0)`                     | 0x00                        | SW-CMN-REQ-SIG-001 | Medium  |

### 5.3 CMN-U-CANSPEC: CanSpecConfigLoader

| TC ID               | 테스트 설명                                              | 입력                                           | 기대 출력                    | 검증 요구사항         | 우선순위 |
|--------------------|--------------------------------------------------------|------------------------------------------------|-----------------------------|--------------------|---------|
| UT-CFG-001         | 유효한 키로 `GetU32` 호출                                 | 존재하는 키, fallback=0                          | INI 파일에 정의된 값           | SW-CMN-REQ-CFG-001 | High    |
| UT-CFG-002         | 존재하지 않는 키로 `GetU32` 호출 — fallback 반환 검증        | 미존재 키, fallback=999                          | 999                         | SW-CMN-REQ-CFG-001 | High    |
| UT-CFG-003         | `GetI32` 음수값 파싱 검증                                 | 음수값 키                                        | 올바른 음수 int32 값           | SW-CMN-REQ-CFG-001 | High    |
| UT-CFG-004         | `GetF64` 부동소수점 파싱 검증                              | 소수점 키                                        | 올바른 double 값              | SW-CMN-REQ-CFG-001 | High    |
| UT-CFG-005         | `GetU32List` 순서 보존 검증                               | 리스트 키                                        | 정의 순서대로 반환             | SW-CMN-REQ-CFG-003 | High    |
| UT-CFG-006         | INI 파일 미존재 시 에러 처리                                | 잘못된 파일 경로                                  | <!-- TODO: 기대 동작 정의 --> | SW-CMN-REQ-CFG-002 | High    |
| UT-CFG-007         | `ActiveConfigPath()` 경로 반환 검증                       | 정상 로드 후 호출                                 | 로드된 파일의 절대 경로         | SW-CMN-REQ-CFG-004 | Medium  |
| UT-CFG-008         | `GetCanSpecProvider()` 싱글턴 일관성                      | 두 번 호출                                       | 동일 참조 반환                 | SW-CMN-REQ-CFG-001 | Medium  |
| <!-- TODO -->      | 잘못된 형식의 INI 파일 입력 테스트 케이스 추가                |                                                |                             |                    |         |

### 5.4 CMN-U-CFGPARSE: ConfigParser

| TC ID               | 테스트 설명                                              | 입력                                           | 기대 출력                    | 검증 요구사항         | 우선순위 |
|--------------------|--------------------------------------------------------|------------------------------------------------|-----------------------------|--------------------|---------|
| UT-PARSE-001       | 유효한 INI 파일 파싱                                      | 정상 INI 문자열                                  | 올바른 섹션/키/값 구조         | SW-CMN-REQ-CFG-001 | High    |
| UT-PARSE-002       | 빈 파일 파싱                                             | 빈 문자열                                        | 빈 결과, 에러 없음            | SW-CMN-REQ-CFG-002 | High    |
| UT-PARSE-003       | 잘못된 형식 (key without value)                           | `key_only_line`                                | <!-- TODO: 기대 동작 정의 --> | SW-CMN-REQ-CFG-002 | Medium  |
| UT-PARSE-004       | 주석 라인 무시 검증                                       | `; comment` 또는 `# comment`                    | 주석 무시, 나머지 파싱         | SW-CMN-REQ-CFG-001 | Medium  |
| <!-- TODO -->      | 추가 엣지 케이스 (UTF-8 BOM, 중복 키, 빈 값 등)            |                                                |                             |                    |         |

---

## 6. 테스트 수행 절차 (Test Execution Procedure)

1. 호스트 환경(x86_64)에서 단위 테스트 빌드 및 실행
2. 코드 커버리지 측정 및 목표치 대비 확인
3. 실패 케이스 분석 및 결함 추적
4. 타겟 환경(NXP S32G)에서 크로스 컴파일 빌드 검증
5. <!-- TODO: 타겟 환경 단위 테스트 실행 절차 확정 -->

---

## 7. 합격/불합격 기준 (Pass/Fail Criteria)

| 기준                                    | 목표                                |
|----------------------------------------|-------------------------------------|
| 모든 테스트 케이스 통과                    | 100% pass                          |
| Statement coverage                     | >= 90% <!-- TODO: 확정 -->           |
| Branch coverage                        | >= 80% <!-- TODO: 확정 -->           |
| 미해결 결함 (Critical/High)              | 0건                                 |
| 미해결 결함 (Medium/Low)                 | <!-- TODO: 허용 기준 정의 -->          |

---

## 8. TODO 요약

- [ ] 테스트 프레임워크 및 코드 커버리지 도구 선정
- [ ] NULL payload, 파일 미존재 등 에러 처리 기대 동작 확정
- [ ] Private CRC Provider 단위 테스트 케이스 추가
- [ ] 잘못된 형식 INI 파일 테스트 케이스 추가
- [ ] 구조적 커버리지 목표치 확정 (MC/DC 적용 여부 포함)
- [ ] 타겟 환경 단위 테스트 실행 절차 확정
- [ ] 테스트 결과 보고서 템플릿 준비
- [ ] 작성자/검토자/승인자 기입 및 리뷰 수행
