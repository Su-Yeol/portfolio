# Firmware Profiles

This directory stores profile-specific firmware behavior variants used by `korec_rehab_cli`.

## Structure

```text
src/firmware_profiles/
├── profile_default/
├── profile_double_lock/
├── profile_lock_mode/
├── profile_offset/
└── transport_serial/
```

## Naming Convention

- `<profile>_control.c`: control-flow oriented variant
- `<profile>_diag_board*.c`: diagnostic-board oriented variant
- `transport_serial_*.c`: transport profile variant

## How It Is Used

- Catalog entries are defined in `src/profile_catalog.c`.
- `korec_rehab_cli list` prints `[category] name` from the catalog.
- `korec_rehab_cli show <name>` prints the source content of the selected profile file.
- `korec_rehab_cli validate` runs token checks against these files.

## Adding a New Profile

1. Add source file under the correct category directory.
2. Register it in `src/profile_catalog.c`.
3. Rebuild and run validation:

```bash
cd korec-rehab
make
./korec_rehab_cli validate
```
