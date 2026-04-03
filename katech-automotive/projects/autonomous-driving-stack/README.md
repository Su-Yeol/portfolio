# Autonomous Driving Stack

Module-oriented driving stack with separated build/run paths for AEB, control, decision, and remote-control functions. Includes a CAN gateway integration and project-level decision builds for two deployment targets (gateway, K-City).

## Stack

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![Shell](https://img.shields.io/badge/Shell-4EAA25?style=flat&logo=gnubash&logoColor=white)
![CAN](https://img.shields.io/badge/CAN_Bus-333333?style=flat)

---

## Directory Overview

```text
autonomous-driving-stack/
├── config/
│   ├── aeb-control.ini
│   ├── control-module.ini
│   └── decision-module.ini
├── modules/
│   ├── aeb-control/
│   ├── common/
│   ├── control-module/
│   ├── decision-module/
│   └── remote-control/
├── scripts/
│   ├── build_decision_gateway.sh
│   └── build_decision_kcity.sh
└── artifacts/decision-module/
```

## Build

### Module-local builds

```bash
cd modules/aeb-control      && ./build.sh
cd modules/control-module   && ./build.sh
cd modules/decision-module  && ./build.sh
cd modules/decision-module  && ./cangateway.sh
```

### Project-level decision builds

```bash
cd katech-automotive/projects/autonomous-driving-stack
./scripts/build_decision_gateway.sh
./scripts/build_decision_kcity.sh
```

## Output Locations

| Build path | Output |
|---|---|
| `modules/*/build/` | `run_*` per module |
| `artifacts/decision-module/` | `run_gateway`, `run_kcity` |

## Public vs Private Runtime

- Public sources compile successfully.
- Without `USE_PRIVATE_IMPL=1`, major runtime binaries print `"private implementation is not available"` and exit.
- Private implementation files are expected under `modules/common/src/private/`.

## ISO 26262 Documentation

ISO 26262 Part 6 SW 수준 기능안전 산출물:

### Project Level

| Document | Path |
|---|---|
| SW Safety Requirements | [docs/iso26262/sw-safety-requirements.md](docs/iso26262/sw-safety-requirements.md) |
| SW Architecture | [docs/iso26262/sw-architecture.md](docs/iso26262/sw-architecture.md) |
| SW Unit Design | [docs/iso26262/sw-unit-design.md](docs/iso26262/sw-unit-design.md) |
| SW Unit Test | [docs/iso26262/sw-unit-test.md](docs/iso26262/sw-unit-test.md) |
| SW Integration Test | [docs/iso26262/sw-integration-test.md](docs/iso26262/sw-integration-test.md) |
| SW Verification Report | [docs/iso26262/sw-verification-report.md](docs/iso26262/sw-verification-report.md) |

### Module Level

각 모듈별 ISO 26262 문서는 `modules/<module>/docs/iso26262/` 에 위치:

| Module | Path |
|---|---|
| AEB Control | [modules/aeb-control/docs/iso26262/](modules/aeb-control/docs/iso26262/) |
| Control Module | [modules/control-module/docs/iso26262/](modules/control-module/docs/iso26262/) |
| Decision Module | [modules/decision-module/docs/iso26262/](modules/decision-module/docs/iso26262/) |
| Remote Control | [modules/remote-control/docs/iso26262/](modules/remote-control/docs/iso26262/) |
| Common | [modules/common/docs/iso26262/](modules/common/docs/iso26262/) |

공통 안전 문서는 [docs/iso26262-index.md](../../docs/iso26262-index.md) 참조.

## Public Snapshot Note

`build_decision_kcity.sh` requires `modules/decision-module/include/` and will fail in this snapshot when that path is absent.
