# PROJECTS

| 프로젝트 | SoC/환경 | 빌드 체계 | 핵심 기여 | 링크 |
|---|---|---|---|---|
| Adaptive AUTOSAR (AXON + S32G) | AXON, NXP S32G | CMake(ara-api/apps) + Yocto/BitBake(meta layers) | AXON/S32G를 동일한 `autosar-platform`/`integration` 구조로 통합하고 `ara-api`/`yocto-layers`는 비공개 분리 | `projects/adaptive-autosar` |
| Autonomous Driving Stack | Embedded Linux + CAN/CAN-FD | C++ 모듈 + Bash | CRC/CAN ID 포함 모듈 코드를 비공개 분리하고 빌드 스크립트 fallback 구성 | `projects/autonomous-driving-stack` |
| Time Synchronization Daemon | Embedded Linux + TSN/AVB | CMake/Make | Open-AVB/gPTP/linuxptp 자산 구조화 | `projects/time-synchronization-daemon` |
| TSN Communication Stack | TSN 검증 환경 | Bash + INI | 시험 시나리오/설정 분리 구성 | `projects/tsn-communication-stack` |
