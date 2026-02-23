#!/usr/bin/env bash
set -euo pipefail

if [[ "${BASH_SOURCE[0]}" == "$0" ]]; then
  echo "[ERROR] Source this script to persist environment variables:" >&2
  echo "        source scripts/env/setup_axon_yocto_env.sh [build-dir]" >&2
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/../.." && pwd)"
AXON_PLATFORM_ROOT="${REPO_ROOT}/projects/adaptive-autosar/axon/autosar-platform"
INTEGRATION_LAYER="${REPO_ROOT}/projects/adaptive-autosar/axon/integration/meta-integration"
BUILD_DIR_NAME="${1:-build-axon}"
AXON_PLATFORM_PRIVATE_ROOT="${REPO_ROOT}/archive/private-local/non-public/projects/adaptive-autosar/axon/autosar-platform"

YOCTO_INIT_PUBLIC="${AXON_PLATFORM_ROOT}/yocto-layers/ara-init-build-env"
YOCTO_INIT_PRIVATE="${AXON_PLATFORM_PRIVATE_ROOT}/yocto-layers/ara-init-build-env"

ACTIVE_PLATFORM_ROOT="${AXON_PLATFORM_ROOT}"
if [[ ! -f "${YOCTO_INIT_PUBLIC}" && -f "${YOCTO_INIT_PRIVATE}" ]]; then
  ACTIVE_PLATFORM_ROOT="${AXON_PLATFORM_PRIVATE_ROOT}"
  echo "[INFO] Using private yocto-layers source: ${AXON_PLATFORM_PRIVATE_ROOT}/yocto-layers"
fi

if [[ ! -f "${ACTIVE_PLATFORM_ROOT}/yocto-layers/ara-init-build-env" ]]; then
  echo "[ERROR] Missing yocto init script: ${ACTIVE_PLATFORM_ROOT}/yocto-layers/ara-init-build-env" >&2
  return 1
fi

if [[ ! -d "${INTEGRATION_LAYER}" ]]; then
  echo "[ERROR] Missing integration layer: ${INTEGRATION_LAYER}" >&2
  return 1
fi

pushd "${ACTIVE_PLATFORM_ROOT}" >/dev/null
export MACHINE="${MACHINE:-tcn1000}"
source yocto-layers/ara-init-build-env "${BUILD_DIR_NAME}"
popd >/dev/null

if [[ -n "${BUILDDIR:-}" && -f "${BUILDDIR}/conf/bblayers.conf" ]]; then
  if ! grep -Fq "${INTEGRATION_LAYER}" "${BUILDDIR}/conf/bblayers.conf"; then
    sed -i "/^BBLAYERS/s|\"$| \\\\n  ${INTEGRATION_LAYER}\"|" "${BUILDDIR}/conf/bblayers.conf"
    echo "[INFO] Added layer to bblayers.conf: ${INTEGRATION_LAYER}"
  else
    echo "[INFO] Layer already present in bblayers.conf"
  fi
else
  echo "[WARN] bblayers.conf not found. Add this layer manually: ${INTEGRATION_LAYER}"
fi

echo "[DONE] Yocto environment ready. Example: bitbake core-image-katech-devel"
