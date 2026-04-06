# Verification Plan

| Field | Value |
|---|---|
| **Document ID** | PRC-VER-001 |
| **Version** | 1.0 |
| **ISO 26262 Reference** | Part 8, Clause 9 |
| **Author** | SuYeol Kim |
| **Reviewer** | - |
| **Approval Date** | 2026-04-06 |

---

## 1. Purpose

본 문서는 KATECH Automotive 프로젝트의 검증 활동 계획을 정의한다. 정적 분석, 동적 분석, 코드 리뷰, 테스트 절차를 포함한다.

## 2. Verification Methods by ASIL

| Method | ASIL A | ASIL B | ASIL C | ASIL D |
|---|---|---|---|---|
| Requirements Review | ++ | ++ | ++ | ++ |
| Design Review | + | ++ | ++ | ++ |
| Code Review (Walk-through) | + | ++ | ++ | ++ |
| Code Review (Inspection) | o | + | ++ | ++ |
| Static Analysis | + | ++ | ++ | ++ |
| Unit Testing | ++ | ++ | ++ | ++ |
| Integration Testing | + | ++ | ++ | ++ |
| Back-to-back Testing | o | + | + | ++ |

Legend: ++ highly recommended, + recommended, o optional

## 3. Static Analysis

| Tool | Purpose | Coding Standard | Target |
|---|---|---|---|
| clang-tidy | Bug detection, MISRA-adjacent rules, code pattern analysis | AUTOSAR C++14 naming, bugprone-*, cert-*, cppcoreguidelines-* | All safety-related C++ code |
| cppcheck | Static code analysis, undefined behavior detection | C++17, --enable=all | All C/C++ sources |
| Polyspace (권장) | MISRA C++:2008 full compliance, runtime error proof | MISRA C++:2008 | ASIL C/D modules (향후 도입) |

### 3.1 MISRA Compliance

| Standard | Scope | Deviation Procedure |
|---|---|---|
| MISRA C++:2008 | ASIL C/D 모듈 (aeb-control, common, control-module) | 편차 발생 시 MISRA Deviation Record 작성, Safety Manager 승인 필요 |
| AUTOSAR C++14 | 전체 C++ 코드 | clang-tidy 규칙 기반 자동 검사, 비준수 항목 문서화 |
| CERT C | 보안 관련 코드 (remote-control, 통신 모듈) | cert-* clang-tidy 규칙 적용 |

### 3.2 Static Analysis Integration

```bash
# CMake 정적 분석 활성화
cmake -B build -DENABLE_CLANG_TIDY=ON -DENABLE_CPPCHECK=ON
cmake --build build

# 독립 실행
clang-tidy --config-file=.clang-tidy src/*.cpp -- -std=c++17 -I modules/common/include
cppcheck --enable=all --std=c++17 modules/
```

### 3.3 Configuration Files
- `.clang-tidy`: `projects/autonomous-driving-stack/.clang-tidy`
- CMake 통합: `projects/autonomous-driving-stack/cmake/StaticAnalysis.cmake`

## 4. Dynamic Analysis

| Tool | Purpose | Coverage Criteria |
|---|---|---|
| Google Test (gtest) 1.14.0 | Unit / Integration testing | ISO 26262-6 Table 9-10 methods |
| gcov + lcov | Code coverage measurement | Statement, Branch, MC/DC |
| BullseyeCoverage (권장) | MC/DC coverage (ASIL D) | MC/DC >= 95% (향후 도입) |

### 4.1 Coverage Criteria by ASIL (ISO 26262-6 Table 10)

| ASIL | Statement | Branch | MC/DC |
|---|---|---|---|
| A | Required | Recommended | — |
| B | >= 95% | >= 90% | Recommended |
| C | >= 100% | >= 95% | Required |
| D | 100% | 100% | >= 95% (Required) |

### 4.2 Test Execution

```bash
# 빌드 및 테스트 실행
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build
cd build && ctest --output-on-failure

# 커버리지 리포트 생성
cmake --build build --target coverage
# 결과: build/coverage_report/index.html
```

### 4.3 Test Framework Structure

```
tests/
├── CMakeLists.txt
├── common/                          # Unit Tests (ISO 26262-6 Clause 9)
│   ├── test_crc_provider.cpp        # UT-CRC-001~010
│   ├── test_signal_codec.cpp        # UT-SIG-001~010
│   ├── test_config_parser.cpp       # UT-PARSE-001~010
│   ├── test_can_spec_config_loader.cpp  # UT-CFG-001~013
│   └── testdata/
└── integration/                     # Integration Tests (ISO 26262-6 Clause 10)
    ├── test_crc_codec_pipeline.cpp   # IT-LIB-001~004
    ├── test_config_pipeline.cpp      # IT-INT-001~004
    └── test_fault_injection.cpp      # TC-FI-001~007
```

## 5. Code Review Process

| Step | Description | Responsible | Tools |
|---|---|---|---|
| 1 | PR/MR 생성 및 변경 범위 기술 | Developer | Git |
| 2 | 자동 정적 분석 수행 (CI) | CI Pipeline | clang-tidy, cppcheck |
| 3 | 코드 리뷰 체크리스트 기반 검토 | Reviewer | PRC-CR-001 |
| 4 | ASIL C/D 모듈: 공식 인스펙션 수행 | Inspector + Moderator | INS 템플릿 |
| 5 | 안전 관련 코드 추가 검토 | Safety Engineer | FMEA/FTA 참조 |
| 6 | 리뷰 결과 기록 및 발견사항 추적 | Recorder | INS-[MODULE]-[SEQ] |
| 7 | Critical/Major 발견사항 해결 확인 | Moderator | - |

### 5.1 Review Type by ASIL

| ASIL | Walk-through | Inspection | Minimum Reviewers |
|---|---|---|---|
| A-B | Required | Optional | 1 |
| C | Required | Recommended | 2 |
| D | Required | Required | 2 + Safety Manager |

### 5.2 Review Documents
- 체크리스트: [Code Review Checklist](code-review-checklist.md) (PRC-CR-001)
- 인스펙션 기록: [Code Inspection Template](code-inspection-template.md)
- 참조: ISO 26262-6 Table 3 (설계 원칙), Table 4 (설계 검증 방법)

## 6. References

- ISO 26262:2018, Part 8, Clause 9
- [Tool Qualification](tool-qualification.md)
- [Documentation Guidelines](documentation-guidelines.md)
- [Code Review Checklist](code-review-checklist.md)
- [Code Inspection Template](code-inspection-template.md)
