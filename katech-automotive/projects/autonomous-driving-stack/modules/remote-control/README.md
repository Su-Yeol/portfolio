# Remote Control Module

Remote control examples for UDP/CAN communication.

## Contents

- `src/remotePC.cpp`: sender-side style sample
- `src/remoteS32G.cpp`: receiver-side style sample
- `output/remotePC`, `output/remoteS32G`: prebuilt binaries currently included in this snapshot

## Build (Manual)

No dedicated build script is provided in this folder. Example manual build:

```bash
cd katech-automotive/projects/autonomous-driving-stack/modules/remote-control
g++ -std=c++17 -g src/remotePC.cpp -I../common/include -o output/remotePC
g++ -std=c++17 -g src/remoteS32G.cpp -I../common/include -o output/remoteS32G
```

## Notes

- Source files include hardcoded IP/port constants; adjust before runtime testing.
- Existing `output/*` binaries may be architecture-specific build artifacts from previous environments.
