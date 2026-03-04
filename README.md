# Engineering Portfolio

Embedded and automotive software engineering portfolio across three tracks: automotive platform integration, rehabilitation firmware, and deep learning research.

---

## Tech Stack

![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![C++](https://img.shields.io/badge/C++-00599C?style=flat&logo=cplusplus&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=flat&logo=python&logoColor=white)
![Shell](https://img.shields.io/badge/Shell-4EAA25?style=flat&logo=gnubash&logoColor=white)
![CMake](https://img.shields.io/badge/CMake-064F8C?style=flat&logo=cmake&logoColor=white)
![Docker](https://img.shields.io/badge/Docker-2496ED?style=flat&logo=docker&logoColor=white)
![Yocto](https://img.shields.io/badge/Yocto-4D9955?style=flat)
![TensorFlow](https://img.shields.io/badge/TensorFlow-FF6F00?style=flat&logo=tensorflow&logoColor=white)
![AUTOSAR](https://img.shields.io/badge/Adaptive_AUTOSAR-0070C0?style=flat)
![TSN](https://img.shields.io/badge/TSN%2FgPTP-FF8C00?style=flat)

---

## Projects

### Automotive Platform — KATECH

> Adaptive AUTOSAR platform bring-up, autonomous driving module integration, and TSN-based deterministic networking on embedded Linux targets.

| Project | Description | Stack |
|---|---|---|
| [Adaptive AUTOSAR](katech-automotive/projects/adaptive-autosar/) | Platform bring-up for S32G and Axon targets; Yocto-based BSP integration, UCM deployment scripting, Docker-based cross-build flow | C++, Yocto, CMake, Docker |
| [Autonomous Driving Stack](katech-automotive/projects/autonomous-driving-stack/) | Modular AEB / control / decision / remote-control stack with CAN gateway; private implementation stub architecture for public sharing | C/C++, CAN, Shell |
| [Time Synchronization Daemon](katech-automotive/projects/time-synchronization-daemon/) | Source mirrors and packaged artifacts for `linuxptp`, `gPTP`, and Open-AVB; reference deployment for deterministic automotive networking | C, gPTP, Open-AVB |
| [TSN Communication Stack](katech-automotive/projects/tsn-communication-stack/) | PHC clock-synchronization verification utilities for automotive boards; prebuilt `aarch64` binaries and gPTP daemon log tooling | C, aarch64, TSN |

**Platform:** NXP S32G · Axon
**Middleware:** Adaptive AUTOSAR (ARA::COM, UCM, IAM)
**Build toolchain:** Yocto / ALB · CMake / Ninja · Docker
→ [Track README](katech-automotive/README.md)

---

### Rehabilitation Firmware — Korec

> EMG-based prosthetic firmware profile catalog with a validation CLI, written in C11 against embedded MCU constraints.

- Registers firmware behavior variants (offset-control, lock-mode, double-lock, transport-serial) in a catalog.
- CLI (`list` / `show` / `validate`) lets reviewers inspect and token-check each profile source file without hardware.
- Build is a single `make` step; no external dependencies.

**Stack:** C11 · Make
→ [Track README](korec-rehab/README.md)

---

### Deep Learning Research — Masters

> Two-pipeline image classification framework (CNN-LSTM and transfer learning) designed for repeatable experiment runs.

- Trains classifiers from class-subfolder datasets with configurable epochs, batch size, and learning rate.
- Produces per-run artifacts: trained model (`.keras`), accuracy/loss curves, confusion matrix (image + Excel).
- Early stopping (`val_loss`, patience = 5) prevents overfitting across both pipelines.

**Stack:** Python 3 · TensorFlow / Keras
**Models:** CNN-LSTM · Transfer Learning (fine-tuning)
→ [Track README](masters-research/README.md)

---

## Repository Layout

```text
portfolio/
├── README.md              ← this file
├── PROJECTS.md            ← short summaries
├── katech-automotive/     ← automotive platform track
│   ├── projects/
│   │   ├── adaptive-autosar/
│   │   ├── autonomous-driving-stack/
│   │   ├── time-synchronization-daemon/
│   │   └── tsn-communication-stack/
│   └── scripts/
├── korec-rehab/           ← rehabilitation firmware track
│   ├── src/
│   └── include/
└── masters-research/      ← deep learning research track
    ├── src/
    └── experiments/
```

## Public/Private Boundary

- Architecture, orchestration scripts, and runnable entry points are public.
- Sensitive datasets, proprietary implementation, credentials, and generated artifacts are excluded via `.gitignore`.
- Where private implementation is absent, binaries print a notice and exit gracefully rather than silently failing.
