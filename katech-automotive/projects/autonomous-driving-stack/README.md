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

## Public Snapshot Note

`build_decision_kcity.sh` requires `modules/decision-module/include/` and will fail in this snapshot when that path is absent.
