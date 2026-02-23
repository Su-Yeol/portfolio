# Aeb Control

## Overview
This module provides autonomous-driving functional components and integration scripts.
It is organized for incremental module build and verification workflows.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout
- Module-oriented build and runtime organization

## Architecture
```text
aeb-control
├── README.md
├── build
│   └── run_aeb
├── build.sh
├── conf
│   └── config.ini -> ../../../config/aeb-control.ini
└── src
    ├── AEBControl_S32G_v2.cpp
    └── Communicator.cpp

3 directories, 6 files
```

## Tech Stack
- Language: C/C++, Shell
- Framework / Library: Platform and module-specific dependencies
- Build Tool: Project-specific scripts

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/aeb-control
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/aeb-control
./build.sh
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
