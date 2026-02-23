#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

INSTALL_PREFIX="${INSTALL_PREFIX:-${REPO_ROOT}/build/axon/install}"
BUILD_ROOT="${BUILD_ROOT:-${REPO_ROOT}/build/axon/integration}"
TARGET_MACHINE="${TARGET_MACHINE:-AdaptiveMachine}"

for tool in cmake; do
  if ! command -v "${tool}" >/dev/null 2>&1; then
    echo "[ERROR] Required tool not found: ${tool}" >&2
    exit 1
  fi
done

INTEG_ROOT="${REPO_ROOT}/projects/adaptive-autosar/axon/integration/meta-integration/KATECH"
if [[ ! -f "${INTEG_ROOT}/applications/CMakeLists.txt" ]]; then
  echo "[ERROR] Missing source: ${INTEG_ROOT}/applications/CMakeLists.txt" >&2
  exit 1
fi

if [[ ! -d "${INSTALL_PREFIX}" ]]; then
  echo "[ERROR] Install prefix not found: ${INSTALL_PREFIX}" >&2
  echo "        Run scripts/build_axon_platform.sh first." >&2
  exit 1
fi

export CMAKE_PREFIX_PATH="${INSTALL_PREFIX}${CMAKE_PREFIX_PATH:+:${CMAKE_PREFIX_PATH}}"
mkdir -p "${BUILD_ROOT}"

build_and_install() {
  local name="$1"
  local src="$2"
  shift 2
  echo "[INFO] Configure: ${name}"
  cmake -S "${src}" -B "${BUILD_ROOT}/${name}" \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}" \
    "$@"

  echo "[INFO] Build+Install: ${name}"
  cmake --build "${BUILD_ROOT}/${name}" --parallel
  cmake --install "${BUILD_ROOT}/${name}"
}

build_and_install katech-applications "${INTEG_ROOT}/applications"
build_and_install katech-interfaces "${INTEG_ROOT}/interfaces"
build_and_install katech-network "${INTEG_ROOT}/network"
build_and_install katech-common-machine "${INTEG_ROOT}/machines/common"
build_and_install katech-adaptivemachine-arxmls "${INTEG_ROOT}/machines/AdaptiveMachine/model"
build_and_install katech-adaptivemachine-artifacts "${INTEG_ROOT}/machines/AdaptiveMachine" -DTARGET_MACHINE="${TARGET_MACHINE}"

echo "[DONE] AXON integration artifacts installed to: ${INSTALL_PREFIX}"
echo "[NEXT] Yocto image flow:"
echo "       source scripts/env/setup_axon_yocto_env.sh build-axon"
echo "       bitbake core-image-katech-devel"
