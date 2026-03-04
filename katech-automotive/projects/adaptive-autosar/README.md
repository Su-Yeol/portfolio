# Adaptive AUTOSAR

Platform bring-up and integration workspace for Adaptive AUTOSAR across two target environments: NXP S32G and Axon.

## Stack

![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![Yocto](https://img.shields.io/badge/Yocto-4D9955?style=flat)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat&logo=cmake&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-2496ED?style=flat&logo=docker&logoColor=white)
![AUTOSAR](https://img.shields.io/badge/Adaptive_AUTOSAR-0070C0?style=flat)

---

## Targets

| Track | Platform | Toolchain |
|---|---|---|
| `s32g/` | NXP S32G (aarch64) | Yocto / ALB · CMake / Ninja · Docker |
| `axon/` | Axon | Adaptive AUTOSAR middleware · UCM |

## Directory Layout

```text
adaptive-autosar/
├── s32g/
│   ├── autosar-platform/    ← platform layer assets
│   ├── integration/         ← integration configuration
│   └── scripts/             ← build and environment scripts
├── axon/
│   ├── autosar-platform/
│   └── integration/
└── common-sample/           ← shared sample assets
```

## Key Scripts

| Script | Purpose |
|---|---|
| `s32g/scripts/run_docker.sh` | Copies `docker_build.sh` into the native Docker build path and runs configured targets |
| `s32g/scripts/docker_build.sh` | CMake/Ninja build for `RadarFusionMachine` (default target) inside Docker |
| `s32g/scripts/katech-setup-s32g-autosar.sh` | Environment setup for Yocto / ALB flow |

## Quick Start (S32G)

```bash
cd katech-automotive/projects/adaptive-autosar/s32g/scripts
./run_docker.sh
```

## Public Snapshot Note

- `ara-api/` and `yocto-layers/` directories are placeholder stubs that document where private/vendor content is expected.
- Full build requires proprietary vendor sources not included in the public repository.
