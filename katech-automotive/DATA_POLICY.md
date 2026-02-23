# Data Policy

## Data Inclusion
This repository is configured for public portfolio use.
Only documentation, integration guides, and non-sensitive source assets are intended for public tracking.

## External Data Usage
External datasets, third-party binaries, and licensed bundles are not included by default unless ownership and redistribution rights are confirmed.

## Public Scope
The public scope includes:
- Project structure and module-level documentation
- Build/run entry guidance
- Sanitized configuration examples

The public scope excludes:
- Raw driving logs and raw sensor captures
- Private implementation sources and proprietary vendor payloads
- Secrets, credentials, certificates, and key materials

## Sensitive Information Handling
- Sensitive content must be isolated from tracked paths.
- Secret files and private implementation paths must remain ignored by Git.
- Any item with unclear ownership must be removed from public tracking until verified.

## Pre-Publish Checklist
1. Verify that `.gitignore` excludes sensitive assets and generated outputs.
2. Confirm no credentials, private keys, or raw logs are staged.
3. Confirm legal ownership and redistribution rights for included files.
4. Re-check documentation links and remove machine-local references.
