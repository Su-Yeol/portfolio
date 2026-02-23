#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MODULE_ROOT="${SCRIPT_DIR}"
COMMON_ROOT="${SCRIPT_DIR}/../common"
PRIVATE_ROOT="${COMMON_ROOT}/src/private/decision-module"
OUT_DIR="${MODULE_ROOT}/build"
mkdir -p "${OUT_DIR}"

CXX_CANDIDATE="${CXX:-g++}"
if ! command -v "${CXX_CANDIDATE}" >/dev/null 2>&1; then
  echo "[ERROR] C++ compiler not found: ${CXX_CANDIDATE}" >&2
  exit 1
fi

PRIVATE_FLAG=()
PRIVATE_SRCS=()
if [[ "${USE_PRIVATE_IMPL:-0}" == "1" ]]; then
  PRIVATE_FLAG=(-DUSE_PRIVATE_IMPL)
  PRIVATE_SRCS=("${PRIVATE_ROOT}/CANGateway_impl.cpp")

  for src in "${PRIVATE_SRCS[@]}"; do
    if [[ ! -f "${src}" ]]; then
      echo "[ERROR] Missing private implementation: ${src}" >&2
      exit 1
    fi
  done
fi

"${CXX_CANDIDATE}" -std=c++17 -g "${PRIVATE_FLAG[@]}" \
  "${MODULE_ROOT}/source_gateway/CANGateway.cpp" \
  "${COMMON_ROOT}"/src/ConfigParser.cpp \
  "${PRIVATE_SRCS[@]}" \
  "${COMMON_ROOT}"/src/CanSpecConfigLoader.cpp \
  "${COMMON_ROOT}"/src/CrcProviderPublic.cpp \
  "${COMMON_ROOT}"/src/CrcProviderPrivate.cpp \
  -I"${MODULE_ROOT}/include" \
  -I"${COMMON_ROOT}/include" \
  -lpthread \
  -o "${OUT_DIR}/run_gateway"

echo "[DONE] ${OUT_DIR}/run_gateway"
