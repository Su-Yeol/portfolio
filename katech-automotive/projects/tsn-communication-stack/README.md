# TSN Communication Stack

PHC/clock-synchronization verification utilities for automotive target boards, focused on measuring and validating TSN timing precision.

## Stack

![C](https://img.shields.io/badge/C-00599C?style=flat&logo=c&logoColor=white)
![TSN](https://img.shields.io/badge/TSN%2FgPTP-FF8C00?style=flat)
![aarch64](https://img.shields.io/badge/aarch64-333333?style=flat)

---

## Layout

```text
tsn-communication-stack/
├── src/gptp/
│   └── daemon_cl_log      ← gPTP daemon log tool
└── tools/
    ├── get_phc100us        ← PHC timing check (100 µs resolution)
    └── get_phc10us         ← PHC timing check (10 µs resolution)
```

## Tools

| Binary | Resolution | Description |
|---|---|---|
| `tools/get_phc100us` | 100 µs | Read PHC clock offset from network interface |
| `tools/get_phc10us` | 10 µs | Read PHC clock offset at higher precision |

Prebuilt as `aarch64` ELF binaries for direct deployment on ARM64 automotive targets.

## Usage (On Target Board)

```bash
sudo ./get_phc100us -d <interface-name>
sudo ./get_phc10us  -d <interface-name>
```

**Options:**

| Flag | Description |
|---|---|
| `-d <interface>` | Network interface name (required) |
| `-v` | Verbose output |
| `-h` | Help |

## Host vs Target

These binaries require `/lib/ld-linux-aarch64.so.1` and will not execute on non-aarch64 hosts. Use an ARM64 target board or a matching container/chroot for execution.
