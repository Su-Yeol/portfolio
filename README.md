# KATECH Automotive SW Portfolio

서로 다른 SoC(S32G, AXON)에서 Adaptive AUTOSAR 포팅 결과를 `projects/adaptive-autosar` 단일 루트로 통합했다.
포트폴리오는 CMake 기반 플랫폼 빌드와 Yocto/BitBake 통합 흐름을 스크립트 기준으로 재현 가능하게 구성했다.

## 핵심 구조
- `projects/adaptive-autosar/axon/autosar-platform`
- `projects/adaptive-autosar/axon/integration`
- `projects/adaptive-autosar/s32g/autosar-platform`
- `projects/adaptive-autosar/s32g/integration`

## 근거 파일
- AXON Platform build entry: `scripts/build_axon_platform.sh`
- AXON Integration build entry: `scripts/build_axon_integration.sh`
- AXON Yocto env setup: `scripts/env/setup_axon_yocto_env.sh`
- AXON Integration layer: `projects/adaptive-autosar/axon/integration/meta-integration/conf/layer.conf`
- S32G Integration layer: `projects/adaptive-autosar/s32g/integration/meta-integration/conf/layer.conf`
- Autonomous decision build entry: `projects/autonomous-driving-stack/scripts/build_decision_kcity.sh`

## Featured Projects
- `projects/adaptive-autosar`: AXON/S32G 포팅 구조와 통합 빌드 동선
- `projects/autonomous-driving-stack`: 공개 제한 모듈 분리 후 빌드 래퍼 유지
- `projects/time-synchronization-daemon`: gPTP/Open-AVB/linuxptp 자산 구조화
- `projects/tsn-communication-stack`: TSN 테스트/설정 분리

## Quick Commands
```bash
cd /home/suyeol/KATECH
scripts/build_axon_platform.sh
scripts/build_axon_integration.sh
source scripts/env/setup_axon_yocto_env.sh build-axon

cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/aeb-control && ./build.sh
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/control-module && ./build.sh
cd /home/suyeol/KATECH/projects/autonomous-driving-stack/modules/decision-module && ./build.sh

cmake -S /home/suyeol/KATECH/projects/time-synchronization-daemon/module/gptp-master -B /tmp/katech-gptp-config-check
```

## Public Scope
- 민감정보/키/토큰/로컬 계정 데이터는 `archive/private-local/`로 격리하고 Git 추적에서 제외한다.
- `projects/adaptive-autosar/**/ara-api`, `projects/adaptive-autosar/**/yocto-layers`는 자리표시자 README만 유지한다.
- `projects/autonomous-driving-stack/modules/common/src/private/**`는 비공개 구현 경로로 유지하고 Git 추적에서 제외한다.
- 보관 파일 `archive/umsd-package/UMSD.tar`는 분석/실행하지 않는다.
