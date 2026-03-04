# Common Module

Shared headers and source files used by `aeb-control`, `control-module`, and `decision-module`.

## Key Responsibilities

- INI-style key/value config parsing (`ConfigParser.cpp`)
- CAN specification provider and typed getters (`CanSpecProvider.h`, `CanSpecConfigLoader.cpp`)
- CRC helpers (`CrcProviderPublic.cpp`, `CrcProviderPrivate.cpp`)
- Bit/byte packing helpers (`SignalCodec.h`)

## Configuration

Default CAN spec file path:

- `./common/config/can_spec.ini`

Override at runtime:

```bash
export AUTODRIVE_CAN_SPEC=/path/to/can_spec.ini
```

## Structure

```text
common/
├── config/can_spec.ini
├── include/
│   ├── CanSpecProvider.h
│   ├── CrcProvider.h
│   └── SignalCodec.h
└── src/
    ├── CanSpecConfigLoader.cpp
    ├── ConfigParser.cpp
    ├── CrcProviderPublic.cpp
    ├── CrcProviderPrivate.cpp
    └── private/
```

## Private Extension Point

`src/private/` contains implementation files used when modules are built with `USE_PRIVATE_IMPL=1`.
