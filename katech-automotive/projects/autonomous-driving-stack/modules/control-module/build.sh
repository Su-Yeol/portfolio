#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MODULE_ROOT="${SCRIPT_DIR}"
COMMON_ROOT="${SCRIPT_DIR}/../common"
PRIVATE_ROOT="${COMMON_ROOT}/src/private/control-module"
OUT_DIR="${MODULE_ROOT}/build"
mkdir -p "${OUT_DIR}"

CXX_CANDIDATE="${CXX:-g++}"
if ! command -v "${CXX_CANDIDATE}" >/dev/null 2>&1; then
  echo "[ERROR] C++ compiler not found: ${CXX_CANDIDATE}" >&2
  exit 1
fi

COMMON_SRCS=(
  "${COMMON_ROOT}/src/ConfigParser.cpp"
  "${COMMON_ROOT}/src/CanSpecConfigLoader.cpp"
  "${COMMON_ROOT}/src/CrcProviderPublic.cpp"
  "${COMMON_ROOT}/src/CrcProviderPrivate.cpp"
)
PUBLIC_SRCS=(
  "${MODULE_ROOT}/src/ControlModule.cpp"
  "${MODULE_ROOT}/src/Communicator.cpp"
  "${MODULE_ROOT}/src/Controller.cpp"
  "${MODULE_ROOT}/src/PathManager.cpp"
)
PRIVATE_SRCS=(
  "${PRIVATE_ROOT}/ControlModule_impl.cpp"
  "${PRIVATE_ROOT}/Communicator_impl.cpp"
  "${PRIVATE_ROOT}/Controller_impl.cpp"
  "${PRIVATE_ROOT}/PathManager_impl.cpp"
)

CXXFLAGS=(-std=c++17 -g)
if [[ "${USE_PRIVATE_IMPL:-0}" == "1" ]]; then
  for src in "${PRIVATE_SRCS[@]}"; do
    if [[ ! -f "${src}" ]]; then
      echo "[ERROR] Missing private implementation: ${src}" >&2
      exit 1
    fi
  done
  CXXFLAGS+=(-DUSE_PRIVATE_IMPL)
  SOURCES=("${PUBLIC_SRCS[@]}" "${PRIVATE_SRCS[@]}" "${COMMON_SRCS[@]}")
else
  SOURCES=("${PUBLIC_SRCS[@]}" "${COMMON_SRCS[@]}")
fi

"${CXX_CANDIDATE}" "${CXXFLAGS[@]}" \
  "${SOURCES[@]}" \
  -I"${MODULE_ROOT}/include" \
  -I"${COMMON_ROOT}/include" \
  -lpthread \
  -o "${OUT_DIR}/run_control"

echo "[DONE] ${OUT_DIR}/run_control"
