# Modules (Reserved)

`src/modules/` is intentionally reserved for reusable support modules.

## Current State

- Public snapshot does not include additional module source files here.
- Active implementation currently lives in:
  - `src/main.c`
  - `src/profile_catalog.c`
  - `src/firmware_profiles/`

## Intended Use

Use this directory when shared logic grows beyond the current two-file core. Keep profile assets in `src/firmware_profiles/` and reusable code in `src/modules/`.
