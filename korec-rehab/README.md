# Korec Rehab

## Overview
This project contains EMG prosthetic firmware profiles and supporting tooling for inspection and validation.
It focuses on profile-level behavior differences and maintainable naming conventions.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout
- Firmware profile grouping by behavior and role

## Architecture
```text
korec-rehab
├── Makefile
├── README.md
├── include
│   └── profile_catalog.h
└── src
    ├── firmware_profiles
    ├── main.c
    └── profile_catalog.c

3 directories, 5 files
```

## Tech Stack
- Language: C/C++
- Framework / Library: Platform and module-specific dependencies
- Build Tool: Make

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd korec-rehab
make
```

### Run
```bash
cd korec-rehab
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.

## Private Components
- No mandatory private source is currently required for public build.
- If private firmware customization is needed, place it under `include/private/` or `src/firmware_profiles/private/` (excluded by `.gitignore`).
