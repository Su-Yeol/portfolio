#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

TARGET_MACHINE="${TARGET_MACHINE:-MinimalMachine}"
INSTALL_PREFIX="${INSTALL_PREFIX:-${REPO_ROOT}/build/axon/install}"
BUILD_ROOT="${BUILD_ROOT:-${REPO_ROOT}/build/axon}"
BUILD_SAMPLE_APPS="${BUILD_SAMPLE_APPS:-1}"

for tool in cmake; do
  if ! command -v "${tool}" >/dev/null 2>&1; then
    echo "[ERROR] Required tool not found: ${tool}" >&2
    exit 1
  fi
done

ARA_API_SRC_PUBLIC="${REPO_ROOT}/projects/adaptive-autosar/axon/autosar-platform/ara-api"
ARA_API_SRC_PRIVATE="${REPO_ROOT}/archive/private-local/non-public/projects/adaptive-autosar/axon/autosar-platform/ara-api"
SAMPLE_APPS_SRC="${REPO_ROOT}/projects/adaptive-autosar/axon/autosar-platform/sample-applications"

ARA_API_SRC="${ARA_API_SRC_PUBLIC}"
if [[ ! -f "${ARA_API_SRC_PUBLIC}/CMakeLists.txt" && -f "${ARA_API_SRC_PRIVATE}/CMakeLists.txt" ]]; then
  ARA_API_SRC="${ARA_API_SRC_PRIVATE}"
  echo "[INFO] Using private ara-api source: ${ARA_API_SRC_PRIVATE}"
fi

if [[ ! -f "${ARA_API_SRC}/CMakeLists.txt" ]]; then
  echo "[ERROR] Missing source: ${ARA_API_SRC}/CMakeLists.txt" >&2
  exit 1
fi
if [[ ! -f "${SAMPLE_APPS_SRC}/CMakeLists.txt" ]]; then
  echo "[ERROR] Missing source: ${SAMPLE_APPS_SRC}/CMakeLists.txt" >&2
  exit 1
fi

mkdir -p "${BUILD_ROOT}" "${INSTALL_PREFIX}"

echo "[INFO] Configure: ara-api"
cmake -S "${ARA_API_SRC}" \
  -B "${BUILD_ROOT}/ara-api" \
  -DTARGET_MACHINE="${TARGET_MACHINE}" \
  -DARA_ENABLE_TESTS=Off \
  -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"

echo "[INFO] Build+Install: ara-api"
cmake --build "${BUILD_ROOT}/ara-api" --parallel
cmake --install "${BUILD_ROOT}/ara-api"

if [[ "${BUILD_SAMPLE_APPS}" == "1" ]]; then
  echo "[INFO] Configure: sample-applications"
  cmake -S "${SAMPLE_APPS_SRC}" \
    -B "${BUILD_ROOT}/sample-applications" \
    -DTARGET_MACHINE="${TARGET_MACHINE}" \
    -DARA_ENABLE_TESTS=Off \
    -DCMAKE_INSTALL_PREFIX="${INSTALL_PREFIX}"

  echo "[INFO] Build+Install: sample-applications"
  cmake --build "${BUILD_ROOT}/sample-applications" --parallel
  cmake --install "${BUILD_ROOT}/sample-applications"
fi

echo "[DONE] AXON platform artifacts installed to: ${INSTALL_PREFIX}"
