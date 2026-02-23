# adaptive-autosar

S32G와 AXON 포팅 자산을 하나의 프로젝트 컨텍스트로 통합한 Adaptive AUTOSAR 루트.

## 구조
- `axon/autosar-platform`
  - `ara-api/README.md` (원본은 비공개 경로 보관)
  - `sample-applications/CMakeLists.txt`
  - `yocto-layers/README.md` (원본은 비공개 경로 보관)
- `axon/integration`
  - `meta-integration/conf/layer.conf`
  - `meta-integration/KATECH/applications/CMakeLists.txt`
- `s32g/autosar-platform`
  - `ara-api/README.md` (원본은 비공개 경로 보관)
  - `sample-applications/CMakeLists.txt`
  - `yocto-layers/README.md` (원본은 비공개 경로 보관)
- `s32g/integration`
  - `meta-integration/conf/layer.conf`
- `common-sample`
  - OpenDDS 샘플
  - `src/sample-application/opendds/interfaces`: IDL
  - `src/sample-application/opendds/src`: publisher/subscriber 소스
  - `src/sample-application/opendds/generated`: TypeSupport 생성 코드
  - `src/sample-application/opendds/bin`: 실행 바이너리
  - `src/sample-application/opendds/scripts/build_local.sh`: 로컬 빌드 스크립트

## 빌드 진입점
```bash
cd /home/suyeol/KATECH
scripts/build_axon_platform.sh
scripts/build_axon_integration.sh
source scripts/env/setup_axon_yocto_env.sh build-axon
```

## 전제조건
- `bash`, `cmake`, `python3`
- Yocto/BitBake 툴체인(실제 풀빌드 시)
- AXON/S32G SDK 환경 변수(플랫폼별 스크립트에서 사용)

## 공개 제외 경로
- `archive/private-local/non-public/projects/adaptive-autosar/axon/autosar-platform/ara-api`
- `archive/private-local/non-public/projects/adaptive-autosar/axon/autosar-platform/yocto-layers`
- `archive/private-local/non-public/projects/adaptive-autosar/s32g/autosar-platform/ara-api`
- `archive/private-local/non-public/projects/adaptive-autosar/s32g/autosar-platform/yocto-layers`

## 포팅 스택 배치 원칙
- AXON: `autosar-platform` + `integration`
- S32G: `autosar-platform` + `integration`
- 공통 샘플: `common-sample`
