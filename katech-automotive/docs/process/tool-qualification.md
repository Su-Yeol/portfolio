# Tool Qualification

| Field | Value |
|---|---|
| **Document ID** | PRC-TQ-001 |
| **Version** | 0.1 (Draft) |
| **ISO 26262 Reference** | Part 8, Clause 11 |
| **Author** | <!-- TODO: 작성자 --> |
| **Reviewer** | <!-- TODO: 검토자 --> |
| **Approval Date** | <!-- TODO: 승인일 --> |

---

## 1. Purpose

본 문서는 프로젝트에서 사용하는 소프트웨어 도구의 Tool Confidence Level(TCL)을 분류하고, 필요 시 도구 인증(Tool Qualification) 계획을 정의한다.

## 2. Tool Classification

ISO 26262 Part 8, Clause 11에 따라 도구를 분류한다.

### 2.1 Tool Impact (TI)

| Level | Description |
|---|---|
| TI1 | 도구의 오동작이 안전 관련 산출물에 영향을 주지 않음 |
| TI2 | 도구의 오동작이 안전 관련 산출물에 오류를 유발할 수 있음 |

### 2.2 Tool Error Detection (TD)

| Level | Description |
|---|---|
| TD1 | 높은 수준의 오류 감지 가능 |
| TD2 | 중간 수준의 오류 감지 가능 |
| TD3 | 낮은 수준의 오류 감지 가능 |

### 2.3 TCL Matrix

| | TD1 | TD2 | TD3 |
|---|---|---|---|
| **TI1** | TCL1 | TCL1 | TCL1 |
| **TI2** | TCL1 | TCL2 | TCL3 |

## 3. Tool Inventory

<!-- TODO: 프로젝트에서 사용하는 도구 목록 및 분류 -->

| Tool | Version | Purpose | TI | TD | TCL | Qualification Needed |
|---|---|---|---|---|---|---|
| GCC/G++ (aarch64) | <!-- TODO --> | C/C++ cross-compilation | TI2 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| CMake | <!-- TODO --> | Build system generation | TI1 | TD1 | TCL1 | No |
| Ninja | <!-- TODO --> | Build execution | TI1 | TD1 | TCL1 | No |
| Docker | <!-- TODO --> | Build environment isolation | TI1 | TD1 | TCL1 | No |
| Git | <!-- TODO --> | Version control | TI1 | TD1 | TCL1 | No |
| Yocto/Bitbake | <!-- TODO --> | Linux image build | TI2 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| <!-- TODO: Static analysis tool --> | <!-- TODO --> | Static code analysis | TI2 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |
| <!-- TODO: Test framework --> | <!-- TODO --> | Unit/Integration testing | TI2 | <!-- TODO --> | <!-- TODO --> | <!-- TODO --> |

## 4. Qualification Plan

<!-- TODO: TCL2/TCL3 도구에 대한 인증 계획 -->

| Tool | TCL | Qualification Method | Status |
|---|---|---|---|
| <!-- TODO --> | <!-- TODO --> | <!-- TODO: 1a (increased confidence from use), 1b (evaluation), 1c (validation), 1d (development per safety std) --> | <!-- TODO --> |

## 5. References

- ISO 26262:2018, Part 8, Clause 11
- [Verification Plan](verification-plan.md)
