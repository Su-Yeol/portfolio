# ISO 26262 문서 읽기 가이드

## 전체 구조

```
docs/                                    ← 프로젝트 공통 문서
├── iso26262-index.md                    ← ★ 시작점: 전체 61개 작업산출물 목록
├── iso26262-reading-guide.md            ← 본 문서
├── process/                             ← 프로세스 문서
│   ├── verification-plan.md             ← 검증 전략 (어떤 도구로 어떻게 검증하나)
│   ├── code-review-checklist.md         ← 코드 리뷰 체크리스트 (10항목)
│   ├── code-inspection-template.md      ← 인스펙션 기록 양식
│   ├── tool-qualification.md            ← 도구 적격성
│   └── ...
├── analysis/                            ← 안전 분석
│   ├── fmea.md                          ← 고장 모드 영향 분석 (21개 고장 모드)
│   ├── fta.md                           ← 결함 트리 분석 (2개 Top Event)
│   └── dependent-failure-analysis.md    ← 종속 고장 분석 (CCF/연쇄 고장)
└── traceability/
    └── requirements-traceability-matrix.md  ← 양방향 추적성 매트릭스

projects/autonomous-driving-stack/
├── docs/iso26262/                       ← 프로젝트 레벨 ISO 문서
│   ├── sw-safety-requirements.md        ← SW 안전 요구사항
│   ├── sw-architecture.md               ← SW 아키텍처
│   ├── sw-unit-design.md                ← SW 단위 설계
│   ├── sw-unit-test.md                  ← SW 단위 테스트 명세
│   ├── sw-integration-test.md           ← SW 통합 테스트 명세
│   └── sw-verification-report.md        ← ★ 검증 보고서 (최종 결과)
└── modules/{모듈명}/docs/iso26262/      ← 모듈별 동일 6종 문서
```

---

## 목적별 문서 안내

### "전체 현황을 파악하고 싶다"

1. **[iso26262-index.md](iso26262-index.md)** → 전체 작업산출물 목록과 상태
2. **[sw-verification-report.md](../projects/autonomous-driving-stack/docs/iso26262/sw-verification-report.md)** → 테스트/분석 결과 종합

### "어떤 안전 요구사항이 있는지 알고 싶다"

1. **[sw-safety-requirements.md](../projects/autonomous-driving-stack/docs/iso26262/sw-safety-requirements.md)** → 프로젝트 전체 안전 요구사항
2. 모듈별 상세: `modules/{모듈}/docs/iso26262/sw-safety-requirements.md`

### "코드가 안전하게 설계되었는지 확인하고 싶다"

1. **[sw-architecture.md](../projects/autonomous-driving-stack/docs/iso26262/sw-architecture.md)** → 모듈 구조, 인터페이스, 안전 메커니즘
2. **[sw-unit-design.md](../projects/autonomous-driving-stack/docs/iso26262/sw-unit-design.md)** → 코딩 가이드라인, 단위 설계 원칙

### "테스트가 어떻게 수행되었는지 알고 싶다"

1. **[sw-unit-test.md](../projects/autonomous-driving-stack/docs/iso26262/sw-unit-test.md)** → 테스트 전략, 환경, 커버리지 기준
2. **[sw-integration-test.md](../projects/autonomous-driving-stack/docs/iso26262/sw-integration-test.md)** → 통합 테스트 단계
3. **[sw-verification-report.md](../projects/autonomous-driving-stack/docs/iso26262/sw-verification-report.md)** → 실제 테스트 결과

### "요구사항이 코드와 테스트까지 추적되는지 확인하고 싶다"

1. **[requirements-traceability-matrix.md](traceability/requirements-traceability-matrix.md)** → Safety Goal → 코드 → 테스트 양방향 추적

### "어떤 위험이 있고 어떻게 대응했는지 알고 싶다"

1. **[fmea.md](analysis/fmea.md)** → 고장 모드별 영향과 대책
2. **[fta.md](analysis/fta.md)** → 결함 원인 트리 (어떤 조합이 사고를 일으키나)
3. **[dependent-failure-analysis.md](analysis/dependent-failure-analysis.md)** → 공통 원인 고장 (한 버그가 전체에 영향?)

### "코드 리뷰/정적 분석은 어떻게 하나"

1. **[verification-plan.md](process/verification-plan.md)** → 도구 + 절차 + ASIL별 기준
2. **[code-review-checklist.md](process/code-review-checklist.md)** → ISO 26262 Table 3 기반 10항목

---

## ISO 26262 Part 6 문서 6종 관계도

```
[1] sw-safety-requirements.md   "무엇을 만족해야 하는가"
         │
         ▼
[2] sw-architecture.md          "어떤 구조로 만들었는가"
         │
         ▼
[3] sw-unit-design.md           "각 단위를 어떻게 설계했는가"
         │
         ├──────────────────┐
         ▼                  ▼
[4] sw-unit-test.md         [5] sw-integration-test.md
    "단위별 테스트"              "모듈 간 통합 테스트"
         │                  │
         └──────┬───────────┘
                ▼
[6] sw-verification-report.md   "결과가 요구사항을 만족하는가"
```

**읽는 순서:**
- 전체 파악: 1 → 2 → 6 (요구사항 → 구조 → 결과)
- 테스트 검증: 4 → 5 → 6 (단위 테스트 → 통합 테스트 → 결과)
- 설계 검토: 1 → 2 → 3 (요구사항 → 아키텍처 → 단위 설계)

---

## 모듈별 ASIL 등급과 핵심 문서

| 모듈 | ASIL | 핵심 확인 문서 | 현재 상태 |
|------|------|--------------|----------|
| **common** | D | `modules/common/docs/iso26262/sw-verification-report.md` | **PASS** (66 TC, 커버리지 99.6%) |
| **aeb-control** | D | `modules/aeb-control/docs/iso26262/sw-verification-report.md` | CONDITIONAL (private impl 필요) |
| **control-module** | C | `modules/control-module/docs/iso26262/sw-verification-report.md` | CONDITIONAL (private impl 필요) |
| **decision-module** | B~C | `modules/decision-module/docs/iso26262/sw-verification-report.md` | CONDITIONAL (private impl 필요) |
| **remote-control** | B | `modules/remote-control/docs/iso26262/sw-verification-report.md` | CONDITIONAL (private impl 필요) |
| **adaptive-autosar** | B~D | `projects/adaptive-autosar/docs/iso26262/sw-verification-report.md` | CONDITIONAL (플랫폼 검증 필요) |

---

## 검증 실행 명령어 요약

```bash
cd projects/autonomous-driving-stack

# 1. 빌드
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON
cmake --build build

# 2. 테스트 실행
cd build && ctest --output-on-failure

# 3. 커버리지 리포트
lcov --capture --directory . --output-file coverage.info --rc lcov_branch_coverage=1
lcov --remove coverage.info '/usr/*' '*/_deps/*' '*/tests/*' --output-file filtered.info --rc lcov_branch_coverage=1
lcov --list filtered.info --rc lcov_branch_coverage=1

# 4. 정적 분석
cmake -B build -DENABLE_CLANG_TIDY=ON && cmake --build build
cppcheck --enable=all --std=c++17 --inline-suppr -I modules/common/include modules/
```
