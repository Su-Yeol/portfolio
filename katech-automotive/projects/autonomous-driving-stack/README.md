# Autonomous Driving Stack

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
autonomous-driving-stack
├── README.md
├── artifacts
│   └── decision-module
├── config
│   ├── aeb-control.ini
│   ├── control-module.ini
│   └── decision-module.ini
├── logs
│   └── decision-module
├── modules
│   ├── README.md
│   ├── aeb-control
│   ├── common
│   ├── control-module
│   ├── decision-module
│   └── remote-control
└── scripts
    ├── build_decision_gateway.sh
    └── build_decision_kcity.sh

12 directories, 7 files
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
cd katech-automotive/projects/autonomous-driving-stack
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
