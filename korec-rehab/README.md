# Korec Rehab — Firmware Profile Catalog

Firmware-profile catalog and validation CLI for EMG-based prosthetic control variants. Written in C11 against embedded MCU constraints, with a single-step Make build and no external dependencies.

## Stack

![C](https://img.shields.io/badge/C11-00599C?style=flat&logo=c&logoColor=white)
![Make](https://img.shields.io/badge/Make-427819?style=flat)

---

## What This Project Does

- Registers firmware behavior variants (offset-control, lock-mode, double-lock, transport-serial) in a compile-time catalog.
- Provides a CLI (`korec_rehab_cli`) to list, inspect, and validate each profile without requiring target hardware.
- `validate` performs token-check on each profile source file, confirming required control-flow markers are present.

## Directory Structure

```text
korec-rehab/
├── src/
│   ├── main.c                  ← CLI entry point
│   ├── profile_catalog.c       ← catalog registry and validation logic
│   └── firmware_profiles/      ← profile source variants
│       ├── profile_default/
│       ├── profile_double_lock/
│       ├── profile_lock_mode/
│       ├── profile_offset/
│       └── transport_serial/
├── include/
│   └── profile_catalog.h       ← catalog API
└── Makefile
```

## Build

```bash
cd korec-rehab
make
```

Output: `korec_rehab_cli`

## CLI Reference

| Command | Description |
|---|---|
| `./korec_rehab_cli list` | Print all registered profiles with their category |
| `./korec_rehab_cli show <name>` | Print source content of the named profile |
| `./korec_rehab_cli validate` | Token-check all profiles for required control markers |

**Validation tokens checked:**
- `init_uart(`
- `stop_control(`

If a profile file is missing or unreadable, the result is reported as `unable to validate`.

## Clean

```bash
make clean
```

## Public Snapshot Note

Several `*_v2` entries are registered in `profile_catalog.c` but their source files are not included in the public snapshot. Those entries are expected to report validation failures.
