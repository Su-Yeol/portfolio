# SW-CMN-ARCH-001: Software Architectural Design

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-ARCH-001                                                     |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 아키텍처 설계서                                  |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 7 — Software architectural design                    |
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

본 문서는 ISO 26262-6:2018 Clause 7에 따라 Common 모듈의 소프트웨어 아키텍처를 기술한다.
Common 모듈은 정적 링크 라이브러리로서, CRC 무결성 검증, CAN 신호 코덱, 설정 파싱 기능을
다른 모든 안전 관련 모듈(AEB Control, Control Module, Decision Module)에 제공한다.

---

## 2. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Part 6 — Product development at the software level       |
| SW-CMN-REQ-001  | Common 모듈 소프트웨어 안전 요구사항 명세서                  |
| SW-CMN-UD-001   | Common 모듈 소프트웨어 단위 설계서                          |
| <!-- TODO -->   | 시스템 아키텍처 설계서 (System Architecture)                |

---

## 3. 아키텍처 개요 (Architectural Overview)

### 3.1 모듈 역할 (Module Role)

Common 모듈은 **공유 유틸리티 라이브러리**로서, 자체 실행 바이너리를 갖지 않으며
다른 모듈의 빌드 시 정적으로 링크된다. 데이터 무결성과 신호 정확성을 보장하는
기본 연산(primitive operations)을 제공한다.

### 3.2 정적 구조 (Static Structure)

```
common/
├── include/
│   ├── CrcProvider.h          -- ICrcProvider 인터페이스
│   ├── SignalCodec.h          -- signal_codec 네임스페이스 (inline 유틸리티)
│   ├── CanSpecProvider.h      -- ICanSpecProvider 인터페이스
│   └── modules/               -- 모듈별 공유 헤더
│       ├── shared/            -- 공통 인터페이스 (config_parser, communicator_cd 등)
│       ├── aeb-control/       -- AEB 추상 인터페이스
│       ├── control-module/    -- Control 추상 인터페이스
│       ├── decision-module/   -- Decision 추상 인터페이스
│       └── remote-control/    -- Remote 인터페이스
├── src/
│   ├── CanSpecConfigLoader.cpp   -- ICanSpecProvider 구현체
│   ├── ConfigParser.cpp          -- INI 파싱 유틸리티
│   ├── CrcProviderPublic.cpp     -- ICrcProvider 기본 구현체
│   ├── CrcProviderPrivate.cpp    -- ICrcProvider Private 구현체
│   └── private/                  -- USE_PRIVATE_IMPL=1 전용 구현체
└── config/
    └── can_spec.ini              -- CAN 버스 사양 설정 파일
```

### 3.3 컴포넌트 다이어그램 (Component Diagram)

<!-- TODO: 실제 다이어그램 도구로 작성 후 이미지 삽입 -->

```
┌─────────────────────────────────────────────────────────┐
│                    Common Library                        │
│                                                         │
│  ┌──────────────┐  ┌──────────────┐  ┌───────────────┐  │
│  │ CrcProvider   │  │ SignalCodec  │  │CanSpecProvider│  │
│  │ (Public/Priv) │  │ (inline)     │  │(ConfigLoader) │  │
│  └──────┬───────┘  └──────┬───────┘  └──────┬────────┘  │
│         │                 │                  │           │
│  ┌──────┴─────────────────┴──────────────────┴────────┐  │
│  │              ICrcProvider / ICanSpecProvider         │  │
│  │              (Abstract Interfaces)                  │  │
│  └─────────────────────────────────────────────────────┘  │
└──────────────────────────┬──────────────────────────────┘
                           │ static link
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
 ┌──────────────┐  ┌──────────────┐  ┌──────────────┐
 │ AEB Control  │  │Control Module│  │Decision Module│
 └──────────────┘  └──────────────┘  └──────────────┘
```

---

## 4. 아키텍처 설계 원칙 (Design Principles) — ISO 26262-6 Clause 7.4.3

### 4.1 모듈성 (Modularity)

- 각 기능 영역(CRC, SignalCodec, CanSpec)은 독립된 헤더/소스 쌍으로 분리
- 추상 인터페이스(`ICrcProvider`, `ICanSpecProvider`)를 통한 의존성 역전
- `signal_codec` 네임스페이스로 코덱 함수 격리

### 4.2 캡슐화 (Encapsulation)

- Private 구현체(`CrcProviderPrivate`, `src/private/`)는 `USE_PRIVATE_IMPL=1` 조건부 컴파일로 격리
- 내부 구현 상세는 `.cpp` 파일에 은닉, 헤더는 순수 인터페이스만 노출

### 4.3 단순성 및 검증 용이성 (Simplicity & Verifiability)

- `SignalCodec.h`의 모든 함수는 `inline`으로 정의되어 부수 효과(side effect) 없음
- 각 함수는 단일 책임 원칙 준수 (하나의 비트 연산 또는 변환만 수행)

### 4.4 적절한 인터페이스 (Suitable Interfaces)

- `ICrcProvider`: CRC 계산 단일 메서드 (`ComputeCrc16CcittWithDataId`)
- `ICanSpecProvider`: 타입별 getter 메서드 (`GetU32`, `GetI32`, `GetF64`, `GetU8`, `GetU32List`)
- Fallback 매개변수를 통한 방어적 설계

---

## 5. SW 안전 요구사항 할당 (Safety Requirement Allocation)

<!-- TODO: SW-CMN-REQ-001 요구사항과의 완전한 매핑 완성 -->

| 아키텍처 컴포넌트          | 할당된 안전 요구사항                        | ASIL |
|--------------------------|------------------------------------------|------|
| CrcProvider (Public)     | SW-CMN-REQ-CRC-001 ~ CRC-004            | B    |
| CrcProvider (Private)    | SW-CMN-REQ-CRC-001 ~ CRC-004            | B    |
| SignalCodec              | SW-CMN-REQ-SIG-001 ~ SIG-005            | B    |
| CanSpecConfigLoader      | SW-CMN-REQ-CFG-001 ~ CFG-004            | B    |
| ConfigParser             | SW-CMN-REQ-CFG-001, CFG-002             | B    |

---

## 6. 안전 메커니즘 및 에러 검출 (Safety Mechanisms) — ISO 26262-6 Clause 7.4.4

### 6.1 데이터 무결성 (Data Integrity)

| 메커니즘                        | 설명                                                    |
|--------------------------------|---------------------------------------------------------|
| CRC-16/CCITT with Data ID     | 모든 CAN 메시지의 무결성 검증에 사용                        |
| Fallback 기본값                | 설정 파싱 실패 시 안전한 기본값(fallback) 반환               |
| <!-- TODO -->                  | 추가 안전 메커니즘 식별 필요 (watchdog, 범위 검증 등)        |

### 6.2 에러 검출 및 처리 (Error Detection & Handling)

- **NULL payload 검출:** CRC 계산 함수의 입력 검증
- **파일 로드 실패:** `CanSpecConfigLoader`의 INI 파일 로드 에러 핸들링
- **타입 변환 에러:** `ConfigParser`의 문자열->숫자 변환 시 에러 처리
- <!-- TODO: 구체적 에러 핸들링 전략 및 안전 상태(safe state) 정의 -->

---

## 7. 빌드 구성 아키텍처 (Build Configuration)

| 빌드 변수             | 값         | 영향                                              |
|----------------------|------------|--------------------------------------------------|
| `USE_PRIVATE_IMPL`   | `0` (기본)  | `CrcProviderPublic.cpp` 사용                      |
| `USE_PRIVATE_IMPL`   | `1`        | `CrcProviderPrivate.cpp` + `src/private/` 포함     |
| Target Architecture  | aarch64    | NXP S32G 크로스 컴파일                              |

<!-- TODO: 빌드 시스템(CMake/Makefile) 구성 상세 및 조건부 컴파일 영향 분석 -->

---

## 8. 외부 인터페이스 (External Interfaces)

| 인터페이스                  | 방향   | 설명                                           |
|---------------------------|--------|-----------------------------------------------|
| `GetCrcProvider()`        | Out    | CRC Provider 싱글턴 참조 반환                    |
| `GetPrivateCrcProvider()` | Out    | Private CRC Provider 싱글턴 참조 반환             |
| `GetCanSpecProvider()`    | Out    | CAN Spec Provider 싱글턴 참조 반환               |
| `signal_codec::*`         | Out    | Inline 함수군 (Pack, Extract, Join, Write 등)   |
| `config/can_spec.ini`     | In     | CAN 버스 사양 설정 파일 (런타임 로드)              |

---

## 9. 공존 회피 (Freedom from Interference) — ISO 26262-6 Clause 7.4.5

<!-- TODO: 다른 ASIL 등급 모듈과의 간섭 회피 분석 수행 -->

- Common 라이브러리는 전역 가변 상태를 최소화하며, `SignalCodec` 함수는 순수 함수(pure function)로 설계
- CRC Provider는 인터페이스 분리 원칙에 따라 Public/Private 구현을 격리
- 각 소비 모듈은 자체 스택/힙에서 Common 함수를 호출하므로 메모리 간섭 없음

---

## 10. 아키텍처 검증 방법 (Verification Methods) — ISO 26262-6 Table 3

| 방법                              | 적용 여부 | 비고                            |
|----------------------------------|----------|--------------------------------|
| 아키텍처 설계 리뷰 (Design review)  | 예       | <!-- TODO: 리뷰 일정 수립 -->     |
| 정적 분석 (Static analysis)        | 예       | <!-- TODO: 도구 선정 -->         |
| 시뮬레이션                         | 해당 없음 | 라이브러리 모듈이므로               |
| 프로토타이핑                       | 선택 사항 |                                |

---

## 11. TODO 요약

- [ ] 시스템 아키텍처 설계서와의 일관성 확인
- [ ] 컴포넌트 다이어그램 도구로 재작성 (PlantUML/draw.io)
- [ ] 안전 메커니즘 추가 식별 및 문서화
- [ ] Freedom from Interference 분석 완성
- [ ] 빌드 시스템 구성 상세 기술
- [ ] 아키텍처 설계 리뷰 일정 및 정적 분석 도구 선정
- [ ] 작성자/검토자/승인자 기입 및 리뷰 수행
