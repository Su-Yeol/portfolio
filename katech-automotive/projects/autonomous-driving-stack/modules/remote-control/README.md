# Remote Control

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
remote-control
├── README.md
├── output
│   ├── remotePC
│   └── remoteS32G
└── src
    ├── remotePC.cpp
    └── remoteS32G.cpp

2 directories, 5 files
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
cd katech-automotive/projects/autonomous-driving-stack/modules/remote-control
# Use project-level build procedure
```

### Run
```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/remote-control
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
