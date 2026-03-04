# Time Synchronization Daemon

Source mirror and packaged artifact workspace for deterministic time synchronization stacks used in automotive networking. Covers `linuxptp`, `gPTP`, and Open-AVB variants.

## Stack

![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![gPTP](https://img.shields.io/badge/gPTP%2FIEEE_802.1AS-FF8C00?style=flat)
![TSN](https://img.shields.io/badge/TSN-FF8C00?style=flat)

---

## Directory Highlights

```text
time-synchronization-daemon/
├── artifacts/tar/
│   └── 240527_open-avb_RF.tar.gz    ← packaged reference artifact
└── module/
    ├── linuxptp/
    ├── gptp-master/
    ├── Open-AVB-asi-master/
    └── open-avb-final/
```

## What Is Included

| Component | Description |
|---|---|
| `module/linuxptp/` | Source mirror of the `linuxptp` PTP stack |
| `module/gptp-master/` | gPTP reference implementation |
| `module/Open-AVB-asi-master/` | Open-AVB fork (ASI variant) |
| `module/open-avb-final/` | Open-AVB finalized integration snapshot |
| `artifacts/tar/*.tar.gz` | Pre-packaged Open-AVB/gPTP content for reference redeploy |

## Quick Inspection

```bash
cd katech-automotive/projects/time-synchronization-daemon
tar -tzf artifacts/tar/240527_open-avb_RF.tar.gz | head
```

## Notes

- This folder is primarily a source/artifact workspace. Service orchestration and deployment are handled by environment-specific scripts outside this directory.
- Most content under `module/` originates from upstream open-source projects. Keep upstream licenses and attribution files intact when modifying or redistributing.
