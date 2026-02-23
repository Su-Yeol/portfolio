#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

mkdir -p "${REPO_ROOT}/projects/adaptive-autosar/axon" "${REPO_ROOT}/projects/adaptive-autosar/s32g"

move_if_exists() {
  local src="$1"
  local dst="$2"
  if [[ -e "${src}" && ! -e "${dst}" ]]; then
    mkdir -p "$(dirname "${dst}")"
    mv "${src}" "${dst}"
    echo "[MOVE] ${src} -> ${dst}"
  fi
}

move_if_exists "${REPO_ROOT}/platforms/axon/adaptive-platform" "${REPO_ROOT}/projects/adaptive-autosar/axon/autosar-platform"
move_if_exists "${REPO_ROOT}/platforms/axon/integration" "${REPO_ROOT}/projects/adaptive-autosar/axon/integration"
move_if_exists "${REPO_ROOT}/platforms/s32g/source/adaptive-platform-24-11" "${REPO_ROOT}/projects/adaptive-autosar/s32g/autosar-platform"
move_if_exists "${REPO_ROOT}/platforms/s32g/source/s32g-integration" "${REPO_ROOT}/projects/adaptive-autosar/s32g/integration"
move_if_exists "${REPO_ROOT}/projects/adaptive-autosar-platform" "${REPO_ROOT}/projects/adaptive-autosar/common-sample"

# Autonomous driving stack normalization
ADS_ROOT="${REPO_ROOT}/projects/autonomous-driving-stack"
if [[ -d "${ADS_ROOT}" ]]; then
  mkdir -p "${ADS_ROOT}/src/modules" "${ADS_ROOT}/config" "${ADS_ROOT}/scripts" "${ADS_ROOT}/data" "${ADS_ROOT}/logs" "${ADS_ROOT}/artifacts"
  move_if_exists "${ADS_ROOT}/AEBControl" "${ADS_ROOT}/src/modules/aeb-control"
  move_if_exists "${ADS_ROOT}/ControlModule" "${ADS_ROOT}/src/modules/control-module"
  move_if_exists "${ADS_ROOT}/DecisionModule" "${ADS_ROOT}/src/modules/decision-module"
  move_if_exists "${ADS_ROOT}/RemoteControl" "${ADS_ROOT}/src/modules/remote-control"
  move_if_exists "${ADS_ROOT}/src/modules/decision-module/Log" "${ADS_ROOT}/logs/decision-module"
  move_if_exists "${ADS_ROOT}/src/modules/decision-module/output" "${ADS_ROOT}/artifacts/decision-module"
  move_if_exists "${ADS_ROOT}/src/modules/control-module/Reference" "${ADS_ROOT}/data/control-module-reference"
fi

echo "[DONE] refactor_iso26262_layout.sh"
