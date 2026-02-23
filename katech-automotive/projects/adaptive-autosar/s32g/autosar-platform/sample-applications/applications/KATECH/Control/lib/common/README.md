# Common

## Overview
This module contains Adaptive AUTOSAR platform, integration, and sample-application assets.
It is structured for platform bring-up, integration, and module-level validation.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout
- Platform/integration separation for AUTOSAR workflows

## Architecture
```text
common
├── README.md
├── config
│   └── can_spec.ini
├── include
│   ├── CanSpecProvider.h
│   └── SignalCodec.h
└── src
    ├── CanSpecConfigLoader.cpp
    └── private

4 directories, 5 files
```

## Tech Stack
- Language: C/C++
- Framework / Library: Platform and module-specific dependencies
- Build Tool: Project-specific scripts

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/sample-applications/applications/KATECH/Control/lib/common
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/sample-applications/applications/KATECH/Control/lib/common
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
