# Modules

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
modules
├── README.md
├── aeb-control
│   ├── README.md
│   ├── build
│   ├── build.sh
│   ├── conf
│   └── src
├── common
│   ├── README.md
│   ├── config
│   ├── include
│   └── src
├── control-module
│   ├── README.md
│   ├── build
│   ├── build.sh
│   ├── conf
│   └── src
├── decision-module
│   ├── README.md
│   ├── build
│   ├── build.sh
│   ├── bulid.sh
│   ├── cangateway.sh
│   ├── config.ini -> ../../config/decision-module.ini
│   ├── source_gateway
│   └── source_kcity
└── remote-control
    ├── README.md
    ├── output
    └── src

19 directories, 12 files
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
cd katech-automotive/projects/autonomous-driving-stack/modules
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
