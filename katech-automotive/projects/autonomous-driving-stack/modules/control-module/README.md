# Control Module

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
control-module
├── README.md
├── build
│   └── run_control
├── build.sh
├── conf
│   └── config.ini -> ../../../config/control-module.ini
└── src
    ├── Communicator.cpp
    ├── ControlModule.cpp
    ├── Controller.cpp
    └── PathManager.cpp

3 directories, 8 files
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
cd katech-automotive/projects/autonomous-driving-stack/modules/control-module
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/control-module
./build.sh
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
