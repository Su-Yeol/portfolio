# SW-CMN-REQ-001: Software Safety Requirements Specification

## 문서 메타데이터 (Document Metadata)

| 항목 (Field)              | 내용 (Value)                                                        |
|--------------------------|---------------------------------------------------------------------|
| **문서 ID (Doc ID)**       | SW-CMN-REQ-001                                                      |
| **문서 제목 (Title)**       | Common 모듈 소프트웨어 안전 요구사항 명세서                              |
| **ISO 26262 참조 (Ref)**   | Part 6, Clause 6 — Specification of software safety requirements     |
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

본 문서는 ISO 26262-6:2018 Clause 6에 따라 Common 모듈의 소프트웨어 안전 요구사항을 정의한다.
Common 모듈은 Autonomous Driving Stack 내 모든 안전 관련 모듈이 공유하는 유틸리티 라이브러리로서,
CRC 무결성 검증, CAN 신호 코덱, 설정 파싱 기능을 제공한다.

This document specifies the software safety requirements for the Common module per ISO 26262-6:2018 Clause 6.

---

## 2. 적용 범위 (Scope)

- **대상 소스 (Key Sources):** `CanSpecConfigLoader.cpp`, `ConfigParser.cpp`, `CrcProviderPublic.cpp`, `CrcProviderPrivate.cpp`
- **대상 헤더 (Key Headers):** `CanSpecProvider.h`, `CrcProvider.h`, `SignalCodec.h`
- **설정 파일 (Config):** `config/can_spec.ini`
- **Private 확장:** `src/private/` (`USE_PRIVATE_IMPL=1` 빌드 시)

---

## 3. 참조 문서 (References)

| ID              | 문서명                                                   |
|-----------------|----------------------------------------------------------|
| ISO 26262-6     | Road vehicles — Functional safety — Part 6: Product development at the software level |
| ISO 26262-8     | Part 8 — Supporting processes (configuration management) |
| SW-CMN-ARCH-001 | Common 모듈 소프트웨어 아키텍처 설계서                       |
| <!-- TODO -->   | 시스템 안전 요구사항 명세서 (System Safety Requirements)     |
| <!-- TODO -->   | 기술 안전 요구사항 명세서 (Technical Safety Requirements)    |

---

## 4. 소프트웨어 안전 요구사항 도출 근거 (Derivation from Technical Safety Requirements)

<!-- TODO: 시스템 안전 분석 결과로부터 도출된 기술 안전 요구사항(TSR)과의 추적성 매트릭스 작성 -->

Common 모듈은 직접적인 안전 기능을 수행하지 않으나, 모든 안전 관련 모듈(AEB Control, Control Module, Decision Module)이
본 라이브러리의 CRC 계산, 신호 인코딩/디코딩, 설정 로딩 결과에 의존하므로 **지원 라이브러리(supporting element)** 로서
상위 ASIL 등급을 상속받는다.

---

## 5. 소프트웨어 안전 요구사항 목록 (Software Safety Requirements)

### 5.1 CRC 무결성 (CRC Integrity)

| Req ID          | 요구사항                                                                                         | ASIL | 검증 방법         |
|-----------------|--------------------------------------------------------------------------------------------------|------|-------------------|
| SW-CMN-REQ-CRC-001 | `ComputeCrc16CcittWithDataId()`는 CRC-16/CCITT 다항식(0x1021)을 준수하여 CRC를 계산해야 한다.         | B    | 단위 테스트, 리뷰   |
| SW-CMN-REQ-CRC-002 | CRC 계산 시 payload가 NULL이거나 payload_len이 0인 경우 정의된 에러 처리를 수행해야 한다.                | B    | 단위 테스트         |
| SW-CMN-REQ-CRC-003 | `GetCrcProvider()`와 `GetPrivateCrcProvider()`는 thread-safe하게 Provider 인스턴스를 반환해야 한다.   | B    | 코드 리뷰, 정적 분석 |
| SW-CMN-REQ-CRC-004 | CRC seed 값과 Data ID(low/high)는 호출자가 지정한 값을 정확히 반영해야 한다.                           | B    | 단위 테스트         |
| <!-- TODO -->   | Private CRC 구현체(`CrcProviderPrivate`)의 추가 안전 요구사항 도출 필요                                | B    | <!-- TODO -->      |

### 5.2 CAN Signal Codec (신호 코덱)

| Req ID               | 요구사항                                                                                      | ASIL | 검증 방법         |
|-----------------------|-----------------------------------------------------------------------------------------------|------|-------------------|
| SW-CMN-REQ-SIG-001   | `PackField()`, `Pack2()`, `Pack3()` 함수는 지정된 mask/shift에 따라 비트 정확한 패킹을 수행해야 한다. | B    | 단위 테스트         |
| SW-CMN-REQ-SIG-002   | `ExtractField()` 함수는 mask/shift 역연산으로 원본 값을 정확히 복원해야 한다.                        | B    | 단위 테스트         |
| SW-CMN-REQ-SIG-003   | `JoinU16Le()`, `JoinU32Le()`, `JoinI32Le()`는 리틀 엔디안 바이트 순서를 준수해야 한다.               | B    | 단위 테스트         |
| SW-CMN-REQ-SIG-004   | `WriteU16Le()`, `WriteU32Le()` 함수는 buffer overflow 없이 올바른 오프셋에 기록해야 한다.             | B    | 단위 테스트, 정적 분석 |
| SW-CMN-REQ-SIG-005   | `ComposeU16()` 함수는 두 바이트의 부분 비트를 결합하여 정확한 16비트 값을 생성해야 한다.                  | B    | 단위 테스트         |

### 5.3 CAN Spec 설정 파싱 (Configuration Parsing)

| Req ID               | 요구사항                                                                                      | ASIL | 검증 방법         |
|-----------------------|-----------------------------------------------------------------------------------------------|------|-------------------|
| SW-CMN-REQ-CFG-001   | `ICanSpecProvider`는 키가 존재하지 않을 때 지정된 fallback 값을 반환해야 한다.                        | B    | 단위 테스트         |
| SW-CMN-REQ-CFG-002   | `can_spec.ini` 파일 로드 실패 시 적절한 에러 핸들링(로그/리턴 코드)을 수행해야 한다.                    | B    | 단위 테스트         |
| SW-CMN-REQ-CFG-003   | `GetU32List()` 반환 값은 INI 파일 내 정의된 순서를 보존해야 한다.                                    | B    | 단위 테스트         |
| SW-CMN-REQ-CFG-004   | `ActiveConfigPath()`는 현재 로드된 설정 파일의 절대 경로를 정확히 반환해야 한다.                        | QM   | 단위 테스트         |
| <!-- TODO -->         | 설정 값 범위 검증(range check) 요구사항 추가 필요                                                  | B    | <!-- TODO -->      |

### 5.4 공유 라이브러리 일반 요구사항 (General Shared Library Requirements)

| Req ID               | 요구사항                                                                                      | ASIL | 검증 방법         |
|-----------------------|-----------------------------------------------------------------------------------------------|------|-------------------|
| SW-CMN-REQ-GEN-001   | Common 라이브러리는 NXP S32G (aarch64) 타겟에서 정상 빌드 및 링크되어야 한다.                         | B    | 빌드 검증           |
| SW-CMN-REQ-GEN-002   | 모든 public API는 재진입 가능(reentrant)해야 하며 전역 상태 변경 없이 동작해야 한다.                    | B    | 코드 리뷰, 정적 분석 |
| SW-CMN-REQ-GEN-003   | `USE_PRIVATE_IMPL=1` 빌드와 기본 빌드 간 public API 인터페이스는 동일해야 한다.                       | B    | 통합 테스트         |
| <!-- TODO -->         | MISRA C++ 준수 범위 및 예외 사항 정의 필요                                                       | B    | <!-- TODO -->      |

---

## 6. 안전 요구사항 속성 (Safety Requirement Attributes)

<!-- TODO: 각 요구사항에 대해 아래 속성 완성 -->

각 소프트웨어 안전 요구사항은 다음 속성을 포함해야 한다 (ISO 26262-6 Clause 6.4.2):

- **고유 식별자 (Unique ID):** 상기 Req ID 참조
- **ASIL 등급:** 기술 안전 요구사항으로부터 상속
- **안전 상태 기여:** 해당 요구사항 위반 시 시스템에 미치는 영향
- **검증 기준:** 수용 가능한 검증 방법 및 판정 기준
- **추적성:** 상위 기술 안전 요구사항 ID와의 매핑

---

## 7. 추적성 매트릭스 (Traceability Matrix)

<!-- TODO: TSR(기술 안전 요구사항) <-> SW Safety Req <-> 아키텍처 요소 매핑 완성 -->

| 기술 안전 요구사항 (TSR ID) | SW 안전 요구사항 (SW Req ID)      | 아키텍처 요소                  |
|----------------------------|----------------------------------|-------------------------------|
| <!-- TODO: TSR-xxx -->     | SW-CMN-REQ-CRC-001 ~ CRC-004    | CrcProvider (Public/Private)  |
| <!-- TODO: TSR-xxx -->     | SW-CMN-REQ-SIG-001 ~ SIG-005    | SignalCodec                   |
| <!-- TODO: TSR-xxx -->     | SW-CMN-REQ-CFG-001 ~ CFG-004    | CanSpecConfigLoader           |
| <!-- TODO: TSR-xxx -->     | SW-CMN-REQ-GEN-001 ~ GEN-003    | Common Library (전체)          |

---

## 8. TODO 요약

- [ ] 시스템 안전 분석 완료 후 ASIL 등급 최종 확정
- [ ] 기술 안전 요구사항(TSR) 문서 참조 ID 기입
- [ ] 추적성 매트릭스 TSR ID 매핑 완성
- [ ] Private CRC 구현체 추가 안전 요구사항 도출
- [ ] 설정 값 범위 검증 요구사항 추가
- [ ] MISRA C++ 준수 범위 정의
- [ ] 작성자/검토자/승인자 기입 및 리뷰 수행
