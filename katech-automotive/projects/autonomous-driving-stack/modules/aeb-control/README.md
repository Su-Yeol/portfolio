# AEB Control Module

AEB runtime module that links shared common sources and emits `run_aeb`.

## Build

```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/aeb-control
./build.sh
```

Output:

- `build/run_aeb`

## Dependencies

- Common sources from `../common/src/`
- Common headers from `../common/include/`
- Optional private implementation from `../common/src/private/aeb-control/`

## Private/Public Behavior

- Default build compiles a public stub binary that reports missing private implementation.
- Full behavior build:

```bash
USE_PRIVATE_IMPL=1 ./build.sh
```

## Config Link

- `conf/config.ini` points to `../../../config/aeb-control.ini`.
