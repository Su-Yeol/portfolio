# Katech Automotive

Automotive software engineering track covering Adaptive AUTOSAR platform bring-up, autonomous driving module integration, deterministic time synchronization, and TSN-based networking — all targeting embedded Linux on automotive-grade SoCs.

## Stack

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![Shell](https://img.shields.io/badge/Shell-4EAA25?style=flat&logo=gnubash&logoColor=white)
![Yocto](https://img.shields.io/badge/Yocto-4D9955?style=flat)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat&logo=cmake&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-2496ED?style=flat&logo=docker&logoColor=white)
![AUTOSAR](https://img.shields.io/badge/Adaptive_AUTOSAR-0070C0?style=flat)
![TSN](https://img.shields.io/badge/TSN%2FgPTP-FF8C00?style=flat)

**Target hardware:** NXP S32G · Axon

---

## Projects

| Project | Focus | Entry Point |
|---|---|---|
| [Adaptive AUTOSAR](projects/adaptive-autosar/) | Platform/BSP bring-up, Yocto integration, UCM deployment | `s32g/scripts/run_docker.sh` |
| [Autonomous Driving Stack](projects/autonomous-driving-stack/) | AEB / control / decision / remote-control modules with CAN gateway | `modules/*/build.sh` |
| [Time Synchronization Daemon](projects/time-synchronization-daemon/) | `linuxptp` / `gPTP` / Open-AVB source mirrors and packaged artifacts | artifact tar inspection |
| [TSN Communication Stack](projects/tsn-communication-stack/) | PHC clock-sync verification tools for target boards | `tools/get_phc*` |

---

## Quick Start

```bash
# Adaptive AUTOSAR — S32G Docker build
cd projects/adaptive-autosar/s32g/scripts
./run_docker.sh

# Autonomous Driving — module build
cd projects/autonomous-driving-stack/modules/decision-module
./build.sh

# TSN — inspect packaged artifact
cd projects/time-synchronization-daemon
tar -tzf artifacts/tar/240527_open-avb_RF.tar.gz | head
```

## ISO 26262 Functional Safety

ISO 26262 기능안전 프로세스 산출물 (Part 2–9) 문서 구조가 포함되어 있습니다.

| Category | Documents | Path |
|---|---|---|
| Full Index | 전체 산출물 목록 | [docs/iso26262-index.md](docs/iso26262-index.md) |
| Safety Management (Part 2) | Safety Plan, Safety Case, Confirmation Review | [docs/safety/](docs/safety/) |
| Concept Phase (Part 3) | Item Definition, HARA, Safety Goals, FSC | [docs/concept/](docs/concept/) |
| System Level (Part 4) | TSC, System Design, Requirements Allocation | [docs/system/](docs/system/) |
| SW Level (Part 6) | Per-project SW safety docs | Each project `docs/iso26262/` |
| Supporting Processes (Part 8) | CM, Change Mgmt, Verification, Tool Qual | [docs/process/](docs/process/) |
| Safety Analysis (Part 9) | FMEA, FTA, Dependent Failure Analysis | [docs/analysis/](docs/analysis/) |
| Traceability | Requirements Traceability Matrix | [docs/traceability/](docs/traceability/) |

## Supporting Docs

| File | Purpose |
|---|---|
| [PROJECTS.md](PROJECTS.md) | Short project summaries and keyword index |
| [DATA_POLICY.md](DATA_POLICY.md) | Public/private data handling policy |
| [SECURITY.md](SECURITY.md) | Security reporting and disclosure |

## Public/Private Boundary

- `archive/private-local/` and generated build artifacts are excluded from public tracking.
- Build wrappers, orchestration scripts, and structural layout are fully public.
- Runtime modules that require private implementation compile as stubs and print a notice on execution.
