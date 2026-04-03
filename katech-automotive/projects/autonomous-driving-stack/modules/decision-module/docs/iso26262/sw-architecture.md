# Software Architecture Description

## ISO 26262 Part 6, Clause 7 — 소프트웨어 아키텍처 설계

| 항목 | 내용 |
|------|------|
| **Document ID** | SW-DEC-ARCH-001 |
| **Module** | Decision Module (K-City / CAN Gateway) |
| **ISO 26262 Reference** | Part 6, Clause 7 (Software architectural design) |
| **ASIL** | B–D (기능별 상이) |
| **Target HW** | NXP S32G (aarch64) |
| **Author** | TODO: 작성자 |
| **Reviewer** | TODO: 검토자 |
| **Approval** | TODO: 승인자 |
| **Version** | 0.1 (Draft) |
| **Date** | TODO: YYYY-MM-DD |
| **Status** | Draft |

---

## 1. 목적 (Purpose)

본 문서는 ISO 26262 Part 6, Clause 7에 따라 Decision Module의 소프트웨어 아키텍처를 기술한다. K-City variant와 CAN Gateway variant의 공통 및 개별 아키텍처 요소를 정의하고, 안전 메커니즘 및 소프트웨어 컴포넌트 간 인터페이스를 명세한다.

---

## 2. 참조 문서 (References)

| 문서 ID | 문서명 | 비고 |
|---------|--------|------|
| SW-DEC-REQ-001 | Software Safety Requirements Specification | 안전 요구사항 문서 |
| SW-DEC-UD-001 | Software Unit Design | 단위 설계 문서 |
| TODO | Hardware-Software Interface (HSI) Specification | HW-SW 인터페이스 |
| ISO 26262:2018 Part 6, Clause 7 | Software architectural design | 국제 표준 |

---

## 3. 아키텍처 개요 (Architecture Overview)

### 3.1 시스템 컨텍스트

```
┌──────────────────────────────────────────────────────┐
│              Autonomous Driving Stack                 │
│                                                      │
│  ┌──────────┐  ┌──────────┐  ┌───────────────────┐  │
│  │ GPS      │  │ LiDAR    │  │ Path Planning     │  │
│  │ Module   │  │ (Ibeo)   │  │ Module            │  │
│  └────┬─────┘  └────┬─────┘  └────────┬──────────┘  │
│       │              │                 │             │
│       ▼              ▼                 ▼             │
│  ┌──────────────────────────────────────────────┐    │
│  │         Decision Module                       │    │
│  │  ┌────────────────┐  ┌─────────────────────┐ │    │
│  │  │  K-City        │  │  CAN Gateway        │ │    │
│  │  │  (run_kcity)   │  │  (run_gateway)      │ │    │
│  │  └────────────────┘  └─────────────────────┘ │    │
│  │           │  Common Library (../common/)  │   │    │
│  └──────────────────────────────────────────────┘    │
│                        │                             │
│                        ▼                             │
│              ┌──────────────────┐                    │
│              │ Vehicle Control  │                    │
│              │ (CAN Bus)        │                    │
│              └──────────────────┘                    │
└──────────────────────────────────────────────────────┘
```

### 3.2 빌드 구성

| Variant | Build Script | Output | Source Directory |
|---------|-------------|--------|-----------------|
| K-City | `./build.sh` | `build/run_kcity` | `source_kcity/` |
| CAN Gateway | `./cangateway.sh` | `build/run_gateway` | `source_gateway/` |

공통 소스: `../common/src/`, `../common/include/`

---

## 4. 소프트웨어 컴포넌트 (Software Components)

### 4.1 K-City Variant 컴포넌트

| Component | Source File | 역할 | ASIL |
|-----------|------------|------|------|
| MainModule | `source_kcity/MainModule.cpp` | 주행 판단 메인 로직, 센서 데이터 통합 | TODO |
| Communicator | `source_kcity/Communicator.cpp` | 외부 모듈 통신 (센서, 경로 계획) | TODO |
| PathManager | `source_kcity/PathManager.cpp` | 경로 관리 및 추종 판단 | TODO |
| PathViewer | `source_kcity/PathViewer.py` | 경로 시각화 (비안전, QM) | QM |
| Viewer | `source_kcity/Viewer.py` | 상태 시각화 (비안전, QM) | QM |

### 4.2 CAN Gateway Variant 컴포넌트

| Component | Source File | 역할 | ASIL |
|-----------|------------|------|------|
| CANGateway | `source_gateway/CANGateway.cpp` | CAN 메시지 송수신 및 게이트웨이 처리 | TODO |
| CRC | `source_gateway/CRC.cpp` | CAN 메시지 CRC 무결성 검증 | TODO |

### 4.3 공통 라이브러리 (Common Library)

| Component | Location | 역할 | ASIL |
|-----------|---------|------|------|
| Common Sources | `../common/src/` | 공유 유틸리티 및 데이터 구조 | TODO |
| Common Headers | `../common/include/` | 공유 인터페이스 정의 | TODO |
| Private Impl | `../common/src/private/decision-module/` | 비공개 구현부 (선택적) | TODO |

> TODO: 공통 라이브러리의 개별 컴포넌트 목록을 작성할 것

---

## 5. 소프트웨어 컴포넌트 인터페이스 (Component Interfaces)

### 5.1 외부 인터페이스

| Interface | Direction | Protocol | Data | 비고 |
|-----------|----------|----------|------|------|
| GPS 수신 | Input | TODO: UDP/Serial | 위치 좌표, 속도, 방위 | TODO: 메시지 포맷 정의 |
| LiDAR (Ibeo) 수신 | Input | TODO: Ethernet/UDP | 장애물 좌표, 분류 | TODO: 메시지 포맷 정의 |
| Path Planning 수신 | Input | TODO | 경로 Waypoints | TODO: 인터페이스 정의 |
| Pedestrian Detection 수신 | Input | TODO | 보행자 위치, 신뢰도 | TODO: 인터페이스 정의 |
| Vehicle Control 출력 | Output | CAN Bus | 조향, 가감속 명령 | CAN Gateway 경유 |

### 5.2 내부 인터페이스

| Interface | From | To | Data | 비고 |
|-----------|------|-----|------|------|
| Sensor Fusion Data | Communicator | MainModule | 통합 센서 데이터 | K-City variant |
| Path Data | PathManager | MainModule | 경로 추종 정보 | K-City variant |
| CAN Message | CANGateway | CRC | CAN 프레임 | Gateway variant |

> TODO: 인터페이스 상세 사양 (ICD)을 별도 문서로 작성할 것

---

## 6. 아키텍처 설계 원칙 (Design Principles)

ISO 26262 Part 6, Clause 7.4.3에 따른 설계 원칙:

### 6.1 모듈화 (Modularity)

- K-City와 CAN Gateway variant는 독립 빌드 및 독립 실행 가능
- 공통 라이브러리를 통한 코드 재사용
- TODO: 모듈 결합도(coupling) 및 응집도(cohesion) 분석 결과 기술

### 6.2 캡슐화 (Encapsulation)

- Private 구현부(`USE_PRIVATE_IMPL=1`)와 Public stub 분리
- TODO: 인터페이스 추상화 수준 기술

### 6.3 단순성 (Simplicity)

- TODO: 복잡도 메트릭(Cyclomatic Complexity 등) 목표치 기술

---

## 7. 안전 메커니즘 (Safety Mechanisms)

ISO 26262 Part 6, Clause 7.4.4에 따른 안전 메커니즘:

| ID | 안전 메커니즘 | 대상 오류 | 검출/대응 | ASIL |
|----|-------------|----------|----------|------|
| SM-DEC-001 | CRC 검증 | CAN 메시지 손상 | 검출 + 메시지 폐기 | TODO |
| SM-DEC-002 | GPS 신호 타임아웃 감시 | GPS 신호 유실 | 검출 + Safe State 전환 | TODO |
| SM-DEC-003 | LiDAR 데이터 유효성 검사 | 센서 데이터 오류 | 검출 + Fallback | TODO |
| SM-DEC-004 | 경로 계획 데이터 범위 검사 | 비정상 경로 데이터 | 검출 + 이전 경로 유지 | TODO |
| SM-DEC-005 | Watchdog 타이머 | 소프트웨어 행(Hang) | 검출 + 시스템 리셋 | TODO |

> TODO: 각 안전 메커니즘의 상세 설계 및 FMEA 결과 반영

---

## 8. 설정 관리 (Configuration)

- **Config 파일**: `conf/config.ini` (symlink: `../../config/decision-module.ini`)
- TODO: config.ini 파라미터 목록 및 안전 관련 파라미터 식별
- TODO: 설정값 범위 검증 메커니즘 기술

---

## 9. 리소스 사용 (Resource Usage)

| 리소스 | 제한 | 현재 사용량 | 비고 |
|--------|------|-----------|------|
| RAM | TODO | TODO | NXP S32G 제약 |
| CPU | TODO | TODO | Real-time 요구사항 |
| Stack | TODO | TODO | 각 태스크별 |
| Flash | TODO | TODO | 바이너리 크기 |

---

## 10. 아키텍처 검증 (Architecture Verification)

ISO 26262 Part 6, Clause 7.4.7에 따른 검증:

- [ ] TODO: 아키텍처가 모든 SW 안전 요구사항을 충족하는지 검증
- [ ] TODO: 아키텍처 리뷰(Walkthrough / Inspection) 수행
- [ ] TODO: 인터페이스 일관성 검증
- [ ] TODO: ASIL 분해의 타당성 검증

---

## 11. 변경 이력 (Revision History)

| Version | Date | Author | Description |
|---------|------|--------|-------------|
| 0.1 | TODO | TODO | Initial draft |
