# Common

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
common
├── README.md
├── config
│   └── can_spec.ini
├── include
│   ├── CanSpecProvider.h
│   ├── CrcProvider.h
│   ├── SignalCodec.h
│   └── modules
└── src
    ├── CanSpecConfigLoader.cpp
    ├── ConfigParser.cpp
    ├── CrcProviderPrivate.cpp
    ├── CrcProviderPublic.cpp
    └── private

5 directories, 9 files
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
cd katech-automotive/projects/autonomous-driving-stack/modules/common
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/common
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
