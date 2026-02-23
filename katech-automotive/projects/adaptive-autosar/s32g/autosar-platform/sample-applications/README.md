# Sample Applications

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
sample-applications
├── CMakeLists.txt
├── README.md
├── applications
│   ├── AdiOsiAdapter
│   ├── CMValidatorPublisher
│   ├── CMValidatorSubscriber
│   ├── CMakeLists.txt
│   ├── DemonstratorTutorial
│   ├── EmoIntegrationTest
│   ├── EmoSampleApplication
│   ├── ExtendedTutorial
│   ├── FaultyCalibrationApplication
│   ├── FaultyProbingApplication
│   ├── FaultyReportingApplication
│   ├── GenericAdasApplication
│   ├── KATECH
│   ├── LICENSE
│   ├── OEM_App
│   ├── OsiAdiAdapter
│   ├── PersistencyDemo
│   ├── PersistencyRedundancyDemo
│   ├── PhmDemo
│   ├── PiCar_Actor
│   ├── PiCar_Controller
│   ├── PiCar_Linesensor
│   ├── PiCar_Ultrasonicsensor
│   ├── RES_TimingRefApp
│   ├── RES_TimingRefServiceApp
│   ├── RES_TraceIntegrationTest
│   ├── RES_TraceIntegrationTest2
│   ├── ST_CMApp01
│   ├── ST_CMApp02
│   ├── ST_CMApp03
│   ├── ST_CMApp09
│   ├── ST_CMApp10
│   ├── ST_CMApp11
│   ├── ST_CMApp12
│   ├── ST_CMApp13
│   ├── ST_CMApp14
│   ├── ST_DDSApp01
│   ├── ST_DDSApp02
│   ├── ST_DDSApp03
│   ├── ST_DIAGApp01
│   ├── ST_EMOApps
│   ├── ST_utils
│   ├── SecDemoFc
│   ├── SecIamTestApp
│   ├── SensorDemoApplication
│   ├── SmartCameraSensor
│   ├── StateManager
│   ├── TlvDemoReceiver
│   ├── TlvDemoSender
│   ├── UCM_ara_api_applications
│   ├── UCM_flashing_adapter
│   ├── UCM_ota_client
│   ├── UCM_pkgmgr_sample
│   ├── UCM_transfer_performance_check
│   ├── UCM_update_adapter
│   ├── UCM_vehicle_driver_application
│   ├── UCM_vehicle_state_manager_application
│   ├── UpdatableApp
│   ├── fc_firewall
│   ├── fusion
│   ├── radar
│   ├── shmDemo
│   ├── smtool
│   └── system_components.arxml
├── clusters
│   ├── Actor
│   ├── CMakeLists.txt
│   ├── Classic
│   ├── Controller
│   ├── LICENSE
│   ├── Linesensor
│   ├── SWCL_BASE
│   ├── Ultrasonicsensor
│   ├── UpdatableApp
│   ├── VehiclePackages
│   ├── cluster_documentations.arxml
...
```

## Tech Stack
- Language: C/C++, Shell
- Framework / Library: Platform and module-specific dependencies
- Build Tool: CMake

## Getting Started
### Prerequisites
- Compiler/toolchain and shell environment for this module

### Build / Installation
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/sample-applications
cmake -S . -B build
cmake --build build
```

### Run
```bash
cd katech-automotive/projects/adaptive-autosar/s32g/autosar-platform/sample-applications
# Follow module scripts and integration workflow
```

## License
- Refer to the repository-level `LICENSE` and policy documents.
