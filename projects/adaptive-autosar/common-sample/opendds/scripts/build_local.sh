#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT_DIR="${ROOT_DIR}/bin"
mkdir -p "${OUT_DIR}"

if ! command -v g++ >/dev/null 2>&1; then
  echo "[ERROR] g++ not found" >&2
  exit 1
fi

if [[ -z "${DDS_ROOT:-}" ]]; then
  echo "[ERROR] DDS_ROOT is not set. Export OpenDDS root first." >&2
  exit 1
fi

COMMON_FLAGS=(
  -I"${ROOT_DIR}/generated"
  -I"${DDS_ROOT}"
  -I"${ACE_ROOT:-${DDS_ROOT}/ACE_wrappers}"
  -std=c++17
)

LIB_FLAGS=(
  -L"${DDS_ROOT}/lib"
  -lOpenDDS_Dcps
  -lOpenDDS_Rtps
  -lOpenDDS_Udp
  -lTAO
  -lACE
)

g++ "${COMMON_FLAGS[@]}" \
  "${ROOT_DIR}/src/publisher.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportImpl.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportC.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportS.cpp" \
  "${ROOT_DIR}/generated/MessengerC.cpp" \
  "${ROOT_DIR}/generated/MessengerS.cpp" \
  "${LIB_FLAGS[@]}" \
  -o "${OUT_DIR}/publisher"

g++ "${COMMON_FLAGS[@]}" \
  "${ROOT_DIR}/src/subscriber.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportImpl.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportC.cpp" \
  "${ROOT_DIR}/generated/MessengerTypeSupportS.cpp" \
  "${ROOT_DIR}/generated/MessengerC.cpp" \
  "${ROOT_DIR}/generated/MessengerS.cpp" \
  "${LIB_FLAGS[@]}" \
  -o "${OUT_DIR}/subscriber"

echo "[DONE] built binaries at ${OUT_DIR}" 
