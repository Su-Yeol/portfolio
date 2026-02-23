#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${ROOT_DIR}/artifacts/decision-module"
CONFIG_SRC="${ROOT_DIR}/config/decision-module.ini"
CONFIG_DST="${OUT_DIR}/config.ini"
REPO_ROOT="$(cd "${ROOT_DIR}/../.." && pwd)"
COMMON_DIR="${ROOT_DIR}/modules/common"

MODULE_DIR_PUBLIC="${ROOT_DIR}/modules/decision-module"
MODULE_DIR_PRIVATE="${REPO_ROOT}/archive/private-local/non-public/projects/autonomous-driving-stack/modules/decision-module"
MODULE_DIR="${MODULE_DIR_PUBLIC}"
if [[ ! -d "${MODULE_DIR_PUBLIC}" && -d "${MODULE_DIR_PRIVATE}" ]]; then
  MODULE_DIR="${MODULE_DIR_PRIVATE}"
  echo "[INFO] Using private decision-module source: ${MODULE_DIR_PRIVATE}"
fi

CXX_CANDIDATE="${CXX:-aarch64-linux-gnu-g++}"
if ! command -v "${CXX_CANDIDATE}" >/dev/null 2>&1; then
  if command -v g++ >/dev/null 2>&1; then
    CXX_CANDIDATE="g++"
  else
    echo "[ERROR] No C++ compiler found (tried aarch64-linux-gnu-g++ and g++)." >&2
    exit 1
  fi
fi

if [[ ! -d "${MODULE_DIR}/source_kcity" ]]; then
  echo "[ERROR] Missing source directory: ${MODULE_DIR}/source_kcity" >&2
  exit 1
fi
if [[ ! -d "${MODULE_DIR}/include" ]]; then
  echo "[ERROR] Missing include directory: ${MODULE_DIR}/include" >&2
  exit 1
fi

mkdir -p "${OUT_DIR}"
if [[ -f "${CONFIG_SRC}" ]]; then
  cp -f "${CONFIG_SRC}" "${CONFIG_DST}"
fi

echo "[INFO] Compiler: ${CXX_CANDIDATE}"
echo "[INFO] Output: ${OUT_DIR}/run_kcity"
"${CXX_CANDIDATE}" -g \
  "${MODULE_DIR}"/source_kcity/*.cpp \
  "${COMMON_DIR}/src/ConfigParser.cpp" \
  "${COMMON_DIR}/src/CanSpecConfigLoader.cpp" \
  "${COMMON_DIR}/src/CrcProviderPublic.cpp" \
  "${COMMON_DIR}/src/CrcProviderPrivate.cpp" \
  -lpthread \
  -I"${MODULE_DIR}/include" \
  -I"${COMMON_DIR}/include" \
  -o "${OUT_DIR}/run_kcity"

if [[ -n "${DEPLOY_TARGET_DIR:-}" ]]; then
  if ! command -v scp >/dev/null 2>&1; then
    echo "[ERROR] DEPLOY_TARGET_DIR is set but scp is not available." >&2
    exit 1
  fi
  if [[ -z "${DEPLOY_TARGET_HOST:-}" ]]; then
    echo "[ERROR] DEPLOY_TARGET_HOST is required when DEPLOY_TARGET_DIR is set." >&2
    exit 1
  fi
  echo "[INFO] Deploying run_kcity to ${DEPLOY_TARGET_HOST}:${DEPLOY_TARGET_DIR}"
  scp "${OUT_DIR}/run_kcity" "${DEPLOY_TARGET_HOST}:${DEPLOY_TARGET_DIR}"
  scp "${CONFIG_DST}" "${DEPLOY_TARGET_HOST}:${DEPLOY_TARGET_DIR}"
fi

echo "[DONE] build_decision_kcity.sh"
