# Decision Module

Decision module provides two binaries:

- `run_kcity` (from `source_kcity/*.cpp`)
- `run_gateway` (from `source_gateway/CANGateway.cpp`)

## Build Commands

```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/decision-module
./build.sh       # builds run_kcity (wrapper to bulid.sh)
./cangateway.sh  # builds run_gateway
```

Outputs:

- `build/run_kcity`
- `build/run_gateway`

## Dependencies

- Shared sources from `../common/src/`
- Shared headers from `../common/include/`
- Optional private sources from `../common/src/private/decision-module/`

## Private/Public Behavior

- Default build compiles public stubs that print missing private implementation and exit.
- Full behavior requires:

```bash
USE_PRIVATE_IMPL=1 ./build.sh
USE_PRIVATE_IMPL=1 ./cangateway.sh
```

## Extra Project-Level Script

From project root, `scripts/build_decision_gateway.sh` can cross-compile and emit `artifacts/decision-module/run_gateway`.
