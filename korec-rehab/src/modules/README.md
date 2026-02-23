# Firmware Profiles

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
firmware_profiles
├── README.md
├── profile_default
│   ├── profile_default_control.c
│   └── profile_default_diag_board.c
├── profile_double_lock
│   ├── profile_double_lock_control.c
│   └── profile_double_lock_diag_board.c
├── profile_lock_mode
│   ├── profile_lock_mode_control.c
│   └── profile_lock_mode_diag_board.c
├── profile_offset
│   ├── profile_offset_control_baseline.c
│   ├── profile_offset_control_overlap.c
│   └── profile_offset_diag_board.c
└── transport_serial
    └── transport_serial_profile.c

5 directories, 11 files
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
cd korec-rehab/src/firmware_profiles
# Use project-level build procedure
```

### Run
```bash
cd korec-rehab/src/firmware_profiles
# Follow module scripts and integration workflow
```

## Design Notes
- Profile files are grouped by functional behavior for easier comparison.
- Naming uses role-first semantics to reduce ambiguity in profile selection.

## License
- Refer to the repository-level `LICENSE` and policy documents.
