# Software Unit Design Specification

## ISO 26262 Part 6, Clause 8 — 소프트웨어 단위 설계 및 구현

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-UD-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 8 (Software unit design and implementation) |
| **ASIL** | B~C (단위별 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | SuYeol Kim |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 1.0 |
| **Date** | 2026-04-06 |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 8에 따라 Decision Module을 구성하는 소프트웨어 단위(Unit)의 상세 설계를 기술한다. 각 단위의 내부 로직, 데이터 흐름, 인터페이스, 오류 처리를 명세하며 코딩 가이드라인 준수 사항을 정의한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-REQ-001 | Software Safety Requirements Specification | 안전 요구사항 |
| SW-DEC-ARCH-001 | Software Architecture Description | 아키텍처 문서 |
| SW-DEC-UT-001 | Software Unit Test Specification | 단위 테스트 문서 |
| TODO | C++ Coding Guidelines | 코딩 규약 |
| ISO 26262:2018 Part 6, Clause 8 | Software unit design and implementation | 국제 표준 |

---

## 3. 코딩 가이드라인 (Coding Guidelines)

ISO 26262 Part 6, Clause 8.4.5에 따른 코딩 가이드라인:

| 항목 | 기준 | 적용 여부 |
|------|------|----------|
| MISRA C++:2008 | 안전 관련 C++ 코딩 규칙 | 적용 (clang-tidy MISRA 체커 활용) |
| AUTOSAR C++14 Guidelines | AUTOSAR C++ 코딩 표준 | 참조 적용 |
| 정적 분석 도구 | clang-tidy, cppcheck | 적용 |
| 코드 복잡도 상한 | Cyclomatic Complexity ≤ 대기 중: 구현/측정 후 갱신 | 대기 중: 구현/측정 후 갱신 |
| 함수 길이 상한 | 대기 중: 구현/측정 후 갱신 lines | 대기 중: 구현/측정 후 갱신 |

> TODO: 프로젝트에 적용할 코딩 가이드라인을 확정하고 정적 분석 설정을 문서화할 것

---

## 4. 소프트웨어 단위 설계 (Unit Designs)

### 4.1 K-City Variant Units

#### 4.1.1 MainModule (`source_kcity/MainModule.cpp`)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-K-001 |
| **역할** | 주행 판단 메인 루프, 센서 데이터 통합 및 판단 실행 |
| **ASIL** | ASIL C |
| **입력** | Communicator로부터 센서 데이터 (GPS, LiDAR/Ibeo, Radar, Mobileye), PathManager로부터 경로 정보 |
| **출력** | 주행 제어 명령 (조향각, 가감속) |
| **안전 요구사항** | SWR-DEC-001, SWR-DEC-002, SWR-DEC-003, SWR-DEC-004 |

**주요 로직:**

```
TODO: 의사 코드 또는 상태 다이어그램

1. 초기화 (config.ini 로드)
2. 메인 루프:
   a. 센서 데이터 수신 (GPS, LiDAR, 보행자 감지)
   b. 경로 추종 상태 갱신
   c. 장애물 판단
   d. 주행 명령 생성
   e. 제어 명령 출력
3. 종료 처리
```

**오류 처리:**

| 오류 유형 | 검출 방법 | 대응 | 비고 |
|----------|----------|------|------|
| 센서 데이터 타임아웃 | TODO | Safe State 전환 | TODO |
| config.ini 로드 실패 | TODO | 기본값 적용 | SWR-DEC-NF-004 |
| 비정상 경로 데이터 | TODO | 이전 경로 유지 | TODO |

#### 4.1.2 Communicator (`source_kcity/Communicator.cpp`)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-K-002 |
| **역할** | GPS, LiDAR(Ibeo), Radar, Mobileye, Path Planning, Pedestrian Detection 모듈과의 통신 |
| **ASIL** | ASIL B |
| **입력** | 외부 센서/모듈 데이터 (네트워크/IPC) |
| **출력** | 파싱된 센서 데이터를 MainModule에 전달 |
| **안전 요구사항** | SWR-DEC-001, SWR-DEC-003, SWR-DEC-004 |

**주요 로직:**

```
TODO: 통신 프로토콜별 수신 및 파싱 로직 기술

- GPS 데이터 수신: TODO: 프로토콜, 주기, 데이터 포맷
- LiDAR(Ibeo) 데이터 수신: TODO: 프로토콜, 메시지 타입
- Path Planning 데이터 수신: TODO
- Pedestrian Detection 수신: TODO
```

**오류 처리:**

| 오류 유형 | 검출 방법 | 대응 | 비고 |
|----------|----------|------|------|
| 통신 타임아웃 | TODO | 타임아웃 이벤트 보고 | TODO |
| 데이터 범위 초과 | TODO | 데이터 폐기 + 경고 | TODO |

#### 4.1.3 PathManager (`source_kcity/PathManager.cpp`)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-K-003 |
| **역할** | 경로 데이터 관리, Waypoint 추종, 경로 이탈 판단 |
| **ASIL** | ASIL C |
| **입력** | 경로 Waypoints, 현재 GPS 위치 |
| **출력** | 추종 목표점, 조향 명령 기초 데이터 |
| **안전 요구사항** | SWR-DEC-001 |

**주요 로직:**

```
TODO: 경로 추종 알고리즘 설계 기술

- Waypoint 관리
- 현재 위치와 경로 간 Cross-Track Error 계산
- 경로 이탈 판단 임계치: TODO
- Look-ahead distance: TODO
```

#### 4.1.4 PathViewer / Viewer (Python, QM)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-K-004 / SWU-DEC-K-005 |
| **역할** | 경로 및 상태 시각화 (디버깅/모니터링 용도) |
| **ASIL** | QM (안전 무관) |
| **비고** | 안전 기능과 독립적으로 동작, Freedom from Interference 보장 필요 |

> TODO: 시각화 모듈이 안전 기능에 간섭하지 않음을 입증할 것 (FFI 분석)

---

### 4.2 CAN Gateway Variant Units

#### 4.2.1 CANGateway (`source_gateway/CANGateway.cpp`)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-G-001 |
| **역할** | CAN 버스 메시지 송수신 (CAN ID: 0x165, 0x1A0, 0x1CF, 0x35, 0x60, 0xA0, 0xEA), 프로토콜 변환, 게이트웨이 처리 |
| **ASIL** | ASIL B |
| **입력** | Decision Module 제어 명령, 외부 CAN 메시지 |
| **출력** | CAN 프레임 (차량 제어 명령) |
| **안전 요구사항** | SWR-DEC-005 |

**주요 로직:**

```
TODO: CAN 메시지 처리 흐름 기술

1. CAN 인터페이스 초기화
2. 수신 메시지 파싱 및 CRC 검증
3. 프로토콜 변환
4. 송신 메시지 생성 및 CRC 부가
5. CAN 프레임 송신
```

#### 4.2.2 CRC (`source_gateway/CRC.cpp`)

| 항목 | 내용 |
|------|------|
| **Unit ID** | SWU-DEC-G-002 |
| **역할** | CAN 메시지 CRC 계산 및 검증 |
| **ASIL** | ASIL B |
| **입력** | CAN 메시지 바이트 배열 |
| **출력** | CRC 값 (계산), 유효성 판정 (검증) |
| **안전 요구사항** | SWR-DEC-005 |

**주요 로직:**

```
TODO: CRC 알고리즘 상세 기술

- CRC 다항식: TODO (예: CRC-8 SAE J1850)
- 초기값: TODO
- 반영(Reflect) 여부: TODO
- 최종 XOR 값: TODO
```

---

### 4.3 공통 라이브러리 Units

| Unit ID | 소스 위치 | 역할 | ASIL | 비고 |
|---------|----------|------|------|------|
| TODO | `../common/src/` | TODO | TODO | 공통 유틸리티 |
| TODO | `../common/include/` | TODO | TODO | 공통 인터페이스 |

> TODO: common 라이브러리의 개별 단위를 식별하고 상세 설계를 작성할 것

---

## 5. 설계-요구사항 추적성 (Design-to-Requirements Traceability)

| Unit ID | SW Safety Requirement | 비고 |
|---------|----------------------|------|
| SWU-DEC-K-001 | SWR-DEC-001, 002, 003, 004 | MainModule |
| SWU-DEC-K-002 | SWR-DEC-001, 003, 004 | Communicator |
| SWU-DEC-K-003 | SWR-DEC-001 | PathManager |
| SWU-DEC-G-001 | SWR-DEC-005 | CANGateway |
| SWU-DEC-G-002 | SWR-DEC-005 | CRC |

---

## 6. 설계 검증 (Design Verification)

ISO 26262 Part 6, Clause 8.4.6에 따른 검증:

- [ ] TODO: 단위 설계가 아키텍처에 부합하는지 검증
- [ ] TODO: 코딩 가이드라인 준수 검증 (정적 분석)
- [ ] TODO: 단위 설계 리뷰(Walkthrough / Inspection) 수행
- [ ] TODO: 자원 사용량이 제약 조건 내인지 검증

---

## 7. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | 2026-04-06 | SuYeol Kim | Initial draft |
| 1.0 | 2026-04-06 | SuYeol Kim | ASIL 등급, 코딩 가이드라인, 센서/CAN 정보 갱신 |
