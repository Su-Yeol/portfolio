# Katech Automotive

## Overview
This directory contains project-specific source, configuration, and documentation assets.
The structure is maintained for clear module ownership and reproducible workflows.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout

## Architecture
```text
katech-automotive
├── DATA_POLICY.md
├── LICENSE
├── PROJECTS.md
├── README.md
├── SECURITY.md
├── archive
│   ├── private-local
│   └── umsd-package
├── docs
│   └── migration
├── projects
│   ├── adaptive-autosar
│   ├── autonomous-driving-stack
│   ├── time-synchronization-daemon
│   └── tsn-communication-stack
└── scripts
    ├── 10base-t1s
    ├── build_axon_integration.sh
    ├── build_axon_platform.sh
    ├── env
    └── refactor_iso26262_layout.sh

13 directories, 8 files
```

## Tech Stack
- Language: C/C++, Python, Shell
- Framework / Library: Platform and module-specific dependencies
- Build Tool: CMake

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
