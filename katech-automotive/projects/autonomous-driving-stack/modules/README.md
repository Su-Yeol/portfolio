# Autonomous Driving Modules

This directory contains the functional modules used by the autonomous-driving stack.

## Modules

| Module | Role | Build Command | Output |
| --- | --- | --- | --- |
| `aeb-control` | AEB control runtime | `./build.sh` | `build/run_aeb` |
| `control-module` | Lateral/longitudinal control runtime | `./build.sh` | `build/run_control` |
| `decision-module` | K-City and gateway decision binaries | `./build.sh`, `./cangateway.sh` | `build/run_kcity`, `build/run_gateway` |
| `common` | Shared config, CRC, signal codec, common headers | linked by module builds | static source linkage |
| `remote-control` | UDP/CAN remote communication example binaries | prebuilt in `output/` | `output/remotePC`, `output/remoteS32G` |

## Important Runtime Flag

For full behavior, private implementation is expected:

```bash
USE_PRIVATE_IMPL=1 ./build.sh
```

Without that flag, several binaries are intentionally stubbed for public release and report missing private implementation.

## Compatibility Note

- `decision-module/build.sh` delegates to `decision-module/bulid.sh` (spelling kept for backward compatibility).
