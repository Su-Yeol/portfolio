# Configuration Management Plan

| Field | Value |
|---|---|
| **Document ID** | PRC-CM-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 7 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 KATECH Automotive 프로젝트의 형상 관리(Configuration Management) 계획을 정의한다. 소프트웨어, 문서, 도구의 버전 관리 및 변경 통제 절차를 포함한다.

## 2. Configuration Items

<!-- TODO: 형상 관리 대상 항목 정의 -->

| CI Category | Examples | Identification Method |
|---|---|---|
| Source Code | C/C++ sources, build scripts | Git commit hash |
| Documentation | ISO 26262 work products (.md) | Document ID + Version |
| Build Artifacts | Binaries (run_aeb, run_kcity, etc.) | Build tag + timestamp |
| Configuration | .ini files, ARXML manifests | Git-tracked versioning |
| Tools | Compiler, CMake, Docker images | Version pinning |

## 3. Version Control

| Item | Tool | Repository | Branching Strategy |
|---|---|---|---|
| All source/docs | Git | <!-- TODO: repository URL --> | <!-- TODO: e.g., GitFlow, trunk-based --> |

### 3.1 Branch Naming Convention

<!-- TODO: 브랜치 명명 규칙 -->

| Branch Type | Pattern | Example |
|---|---|---|
| Main | `main` | `main` |
| Feature | `feature/<description>` | `feature/iso26262-docs` |
| Release | `release/<version>` | `release/v1.0` |
| Hotfix | `hotfix/<description>` | `hotfix/aeb-timing-fix` |

## 4. Baseline Management

<!-- TODO: 베이스라인 관리 정책 -->

| Baseline Type | Trigger | Approval | Tag Format |
|---|---|---|---|
| Development | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Release | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| Safety | <!-- TODO: 안전 관련 산출물 베이스라인 --> | <!-- TODO --> | <!-- TODO --> |

## 5. Access Control

<!-- TODO: 접근 권한 정책 -->

| Role | Read | Write | Approve |
|---|---|---|---|
| Developer | All | Feature branches | N/A |
| Safety Engineer | All | Safety docs | Safety docs |
| Release Manager | All | Release branches | Release tags |

## 6. Audit Trail

<!-- TODO: 변경 이력 추적 방법 -->

- Git commit history with signed commits
- <!-- TODO: PR/MR review records -->
- <!-- TODO: 변경 요청(CR) 추적 도구 -->

## 7. References

- ISO 26262:2018, Part 8, Clause 7
- [Change Management](change-management.md)
