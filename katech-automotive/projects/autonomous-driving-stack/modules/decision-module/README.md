# Decision Module

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
decision-module
├── README.md
├── build
│   ├── run_gateway
│   └── run_kcity
├── build.sh
├── bulid.sh
├── cangateway.sh
├── config.ini -> ../../config/decision-module.ini
├── source_gateway
│   ├── CANGateway.cpp
│   └── CRC.cpp
└── source_kcity
    ├── Communicator.cpp
    ├── MainModule.cpp
    ├── PathManager.cpp
    ├── PathViewer.py
    ├── Viewer.py
    └── __pycache__

4 directories, 14 files
```

## Tech Stack
- Language: C/C++, Python, Shell
- Framework / Library: Platform and module-specific dependencies
- Build Tool: Project-specific scripts

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/decision-module
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/decision-module
./build.sh
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
