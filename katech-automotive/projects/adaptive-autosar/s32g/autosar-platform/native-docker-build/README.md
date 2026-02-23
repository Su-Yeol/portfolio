# Native Docker Build

## Overview
This module contains Adaptive AUTOSAR platform, integration, and sample-application assets.
It is structured for platform bring-up, integration, and module-level validation.

## Features
- Clear module boundaries with directory-level ownership
- Relative-path based navigation and execution flow
- Documentation aligned with current repository layout
- Platform/integration separation for AUTOSAR workflows

## Architecture
```text
native-docker-build
├── LICENSE
├── Makefile
├── README.md
├── apd_startup
├── catalog.xml
├── dlt_logstorage.conf
├── docker
│   ├── Dockerfile
│   ├── apdup-0.4.2.vsix
│   ├── apx-completion
│   ├── download-vs-code-server.sh
│   └── settings.json
├── docker_build.sh
├── enable-vsomeip-tracing.patch
├── hawkbit
│   ├── baseurl
│   ├── createTarget.sh
│   ├── setPollingAndAuth.sh
│   └── upload_artifacts.sh
├── images
│   ├── dlt_addecu.png
│   ├── vscode_arxml.png
│   ├── vscode_attach_container.png
│   ├── vscode_coredump_analyze.png
│   ├── vscode_coredump_select.png
│   ├── vscode_debug_attach.png
│   └── vscode_version.png
├── itm
│   ├── docker_itm.sh
│   ├── itm_test.py
│   └── run_itm.sh
└── run_docker.sh

4 directories, 28 files
```

## Tech Stack
- Language: Python, Shell
- Framework / Library: Platform and module-specific dependencies
- Build Tool: Make

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/native-docker-build
make
```

### Run
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/native-docker-build
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
