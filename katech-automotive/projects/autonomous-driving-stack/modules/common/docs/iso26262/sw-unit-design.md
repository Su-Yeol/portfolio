# SW-CMN-UD-001: Software Unit Design Specification

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-UD-001                                                       |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 단위 설계 명세서                                 |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 8 — Software unit design and implementation           |
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

본 문서는 ISO 26262-6:2018 Clause 8에 따라 Common 모듈의 소프트웨어 단위 설계 및 구현 지침을 기술한다.
각 소프트웨어 단위(unit)의 상세 설계, 내부 동작, 데이터 흐름, 코딩 가이드라인 준수 사항을 정의한다.

---

## 2. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Part 6, Clause 8 — Software unit design and implementation|
| SW-CMN-REQ-001  | Common 모듈 소프트웨어 안전 요구사항 명세서                  |
| SW-CMN-ARCH-001 | Common 모듈 소프트웨어 아키텍처 설계서                       |
| SW-CMN-UT-001   | Common 모듈 소프트웨어 단위 테스트 명세서                    |

---

## 3. 소프트웨어 단위 목록 (Software Unit Inventory)

| Unit ID         | 단위명                    | 소스 파일                    | 헤더 파일            | 설명                           |
|-----------------|--------------------------|-----------------------------|--------------------|-------------------------------|
| CMN-U-CRC-PUB   | CrcProviderPublic        | `CrcProviderPublic.cpp`     | `CrcProvider.h`    | CRC-16/CCITT 기본 구현체         |
| CMN-U-CRC-PRIV  | CrcProviderPrivate       | `CrcProviderPrivate.cpp`    | `CrcProvider.h`    | CRC-16/CCITT Private 구현체     |
| CMN-U-SIG       | SignalCodec              | (header-only)               | `SignalCodec.h`    | CAN 신호 인코딩/디코딩 유틸리티    |
| CMN-U-CANSPEC   | CanSpecConfigLoader      | `CanSpecConfigLoader.cpp`   | `CanSpecProvider.h`| CAN 사양 설정 로더              |
| CMN-U-CFGPARSE  | ConfigParser             | `ConfigParser.cpp`          | (내부 헤더)          | INI 파일 파싱 유틸리티            |

---

## 4. 단위 상세 설계 (Unit Detailed Design)

### 4.1 CMN-U-CRC-PUB: CrcProviderPublic

**인터페이스:**
```cpp
class ICrcProvider {
public:
    virtual ~ICrcProvider() = default;
    virtual std::uint16_t ComputeCrc16CcittWithDataId(
        const std::uint8_t *payload,
        std::size_t payload_len,
        std::uint16_t seed,
        std::uint8_t data_id_l,
        std::uint8_t data_id_h) const = 0;
};

const ICrcProvider &GetCrcProvider();
```

**내부 동작:**
- CRC-16/CCITT 다항식(0x1021) 기반 CRC 계산
- `seed` 초기값에 `data_id_l`, `data_id_h`를 결합하여 Data ID 반영
- `payload` 바이트 배열을 순차적으로 처리하여 최종 CRC 값 반환
- `GetCrcProvider()`는 정적 지역 변수를 통한 싱글턴 패턴

**입력 제약 조건:**
| 매개변수       | 타입           | 유효 범위                    | 위반 시 동작              |
|---------------|---------------|-----------------------------|--------------------------| 
| `payload`     | `uint8_t*`    | Non-NULL                    | <!-- TODO: 에러 처리 정의 --> |
| `payload_len` | `size_t`      | 0 ~ MAX_PAYLOAD_SIZE        | <!-- TODO: 상한 정의 -->     |
| `seed`        | `uint16_t`    | 0x0000 ~ 0xFFFF             | 전 범위 유효                |
| `data_id_l/h` | `uint8_t`     | 0x00 ~ 0xFF                 | 전 범위 유효                |

**할당된 요구사항:** SW-CMN-REQ-CRC-001 ~ CRC-004

### 4.2 CMN-U-CRC-PRIV: CrcProviderPrivate

**인터페이스:** `ICrcProvider` 동일 (동일 추상 인터페이스 구현)

**특이사항:**
- `USE_PRIVATE_IMPL=1` 빌드 조건에서만 컴파일 포함
- `GetPrivateCrcProvider()` 팩토리 함수로 접근
- <!-- TODO: Private 구현의 차별 알고리즘 또는 HW 가속 사용 여부 기술 -->

**할당된 요구사항:** SW-CMN-REQ-CRC-001 ~ CRC-004

### 4.3 CMN-U-SIG: SignalCodec

**네임스페이스:** `signal_codec`

**함수 목록 및 동작:**

| 함수                | 입력                              | 출력         | 동작 설명                              |
|--------------------|----------------------------------|-------------|---------------------------------------|
| `PackField`        | value, mask, left_shift          | `uint8_t`   | `(value << left_shift) & mask`        |
| `Pack2`            | v1,m1,s1, v2,m2,s2              | `uint8_t`   | 2개 필드 OR 결합                        |
| `Pack3`            | v1,m1,s1, v2,m2,s2, v3,m3,s3   | `uint8_t`   | 3개 필드 OR 결합                        |
| `ExtractField`     | value, mask, right_shift         | `uint8_t`   | `(value & mask) >> right_shift`       |
| `JoinU16Le`        | low, high                        | `uint16_t`  | 리틀 엔디안 16비트 결합                   |
| `JoinU32Le`        | b0, b1, b2, b3                   | `uint32_t`  | 리틀 엔디안 32비트 결합                   |
| `JoinI32Le`        | b0, b1, b2, b3                   | `int32_t`   | `JoinU32Le` 결과의 signed 캐스팅         |
| `ComposeU16`       | left/right value, mask, shift    | `uint16_t`  | 두 바이트의 부분 비트 결합                 |
| `Byte0` ~ `Byte3`  | value (16/32-bit)                | `uint8_t`   | N번째 바이트 추출                        |
| `WriteU16Le`       | buffer, offset, value            | void        | 버퍼에 LE 16비트 기록                    |
| `WriteU32Le`       | buffer, offset, value            | void        | 버퍼에 LE 32비트 기록                    |

**설계 특성:**
- 모든 함수는 `inline` 정의 (헤더 전용, 컴파일 시 인라이닝)
- 부수 효과 없음 (순수 함수) — `WriteU16Le`, `WriteU32Le` 제외 (buffer 변경)
- 정적 캐스팅(`static_cast`)으로 암묵적 형변환 방지

**할당된 요구사항:** SW-CMN-REQ-SIG-001 ~ SIG-005

### 4.4 CMN-U-CANSPEC: CanSpecConfigLoader

**인터페이스:**
```cpp
class ICanSpecProvider {
public:
    virtual ~ICanSpecProvider() = default;
    virtual std::uint32_t GetU32(const std::string &key, std::uint32_t fallback) const = 0;
    virtual std::int32_t  GetI32(const std::string &key, std::int32_t fallback) const = 0;
    virtual double        GetF64(const std::string &key, double fallback) const = 0;
    virtual std::uint8_t  GetU8(const std::string &key, std::uint8_t fallback) const = 0;
    virtual std::vector<std::uint32_t> GetU32List(
        const std::string &key, const std::vector<std::uint32_t> &fallback) const = 0;
    virtual std::string   ActiveConfigPath() const = 0;
};

const ICanSpecProvider &GetCanSpecProvider();
```

**내부 동작:**
- `config/can_spec.ini` 파일을 런타임에 로드하여 키-값 쌍으로 파싱
- 키 미존재 시 caller가 지정한 fallback 값 반환 (방어적 설계)
- `GetCanSpecProvider()`는 싱글턴 패턴으로 단일 인스턴스 보장
- <!-- TODO: 파일 로드 실패 시 에러 처리 흐름 상세 기술 -->

**할당된 요구사항:** SW-CMN-REQ-CFG-001 ~ CFG-004

### 4.5 CMN-U-CFGPARSE: ConfigParser

**역할:** INI 파일 형식의 설정 파일을 파싱하여 섹션/키/값 구조로 변환

**내부 동작:**
- 파일 I/O를 통한 INI 파일 읽기
- 섹션(`[section]`), 키-값(`key=value`) 파싱
- `CanSpecConfigLoader`의 내부 의존성으로 사용
- <!-- TODO: 에러 처리 및 잘못된 형식 입력 시 동작 상세 기술 -->

**할당된 요구사항:** SW-CMN-REQ-CFG-001, CFG-002

---

## 5. 코딩 가이드라인 (Coding Guidelines) — ISO 26262-6 Clause 8.4.5

<!-- TODO: 프로젝트 MISRA C++ 준수 정책 확정 후 상세화 -->

| 항목                          | 적용 기준                                             |
|------------------------------|------------------------------------------------------|
| 코딩 표준                     | MISRA C++:2023 <!-- TODO: 적용 범위 확정 -->            |
| 명시적 타입 캐스팅             | `static_cast` 사용 (현재 구현 준수 확인)                 |
| 전역 변수 사용                 | 싱글턴 패턴의 정적 지역 변수로 제한                       |
| 동적 메모리 할당               | `std::vector`, `std::string` 사용 (할당 실패 처리 필요)  |
| 인라인 함수                   | `SignalCodec.h` — 부수 효과 없는 순수 함수에 한정         |
| 에러 처리                     | 반환 값 기반 (예외 사용 여부 <!-- TODO: 정책 확정 -->)     |

---

## 6. 단위 설계 검증 방법 (Design Verification) — ISO 26262-6 Table 5

| 방법                               | ASIL B 권장 | 적용 여부 | 비고                            |
|-----------------------------------|------------|----------|--------------------------------|
| 단위 설계의 워크스루/인스펙션          | 추천        | 예       | <!-- TODO: 일정 수립 -->         |
| 정적 코드 분석                      | 강력 추천    | 예       | <!-- TODO: 도구 선정 -->         |
| 시뮬레이션 / 모델 기반 검증           | 추천        | 해당 없음 |                                |
| 소스 코드와 설계의 일관성 검토         | 강력 추천    | 예       |                                |

---

## 7. 추적성 (Traceability)

| 단위 ID          | 아키텍처 컴포넌트      | 안전 요구사항                    |
|-----------------|---------------------|-------------------------------|
| CMN-U-CRC-PUB   | CrcProvider Public   | SW-CMN-REQ-CRC-001 ~ CRC-004 |
| CMN-U-CRC-PRIV  | CrcProvider Private  | SW-CMN-REQ-CRC-001 ~ CRC-004 |
| CMN-U-SIG       | SignalCodec          | SW-CMN-REQ-SIG-001 ~ SIG-005 |
| CMN-U-CANSPEC   | CanSpecConfigLoader  | SW-CMN-REQ-CFG-001 ~ CFG-004 |
| CMN-U-CFGPARSE  | ConfigParser         | SW-CMN-REQ-CFG-001, CFG-002  |

---

## 8. TODO 요약

- [ ] Private CRC 구현 상세 알고리즘/HW 가속 기술
- [ ] NULL payload 및 파일 로드 실패 에러 처리 흐름 상세화
- [ ] MISRA C++ 적용 범위 및 예외 규칙 확정
- [ ] 예외(exception) 사용 정책 확정
- [ ] MAX_PAYLOAD_SIZE 상한 정의
- [ ] 정적 코드 분석 도구 선정 (cppcheck, Polyspace 등)
- [ ] 단위 설계 워크스루/인스펙션 일정 수립
- [ ] 작성자/검토자/승인자 기입 및 리뷰 수행
