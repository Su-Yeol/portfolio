# Control Module

Control runtime module that links shared utilities from `../common` and produces `run_control`.

## Build

```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/control-module
./build.sh
```

Output:

- `build/run_control`

## Dependencies

- Common sources from `../common/src/`
- Headers from `../common/include/`
- Optional private implementations from `../common/src/private/control-module/`

## Private/Public Behavior

- Default build (`USE_PRIVATE_IMPL` unset): binary is a public stub and reports missing private implementation at runtime.
- Full build:

```bash
USE_PRIVATE_IMPL=1 ./build.sh
```

## Config Link

- `conf/config.ini` is a symlink to `../../../config/control-module.ini`.
