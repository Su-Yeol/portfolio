#!/bin/bash
set -e

CXX_BIN=${CXX:-aarch64-fsl-linux-g++}
AR_BIN=${AR:-aarch64-fsl-linux-ar}

SRC_DIR=$(realpath $(dirname "$0"))
GEN_INC="${SRC_DIR}/../build/gen/includes"
COMMON_ROOT="${SRC_DIR}/../../common"

SYSROOT_INC="${SDKTARGETSYSROOT}/usr/include"

echo "Cleaning old objects..."
rm -vf LIB.o LIB_KATECH.a

echo "Compiling..."
CMD="${CXX_BIN} -c -D_GNU_SOURCE -fPIC"

for INC in "$SYSROOT_INC" "$GEN_INC" "$COMMON_ROOT" "$COMMON_ROOT/service/inc" "$COMMON_ROOT/logger/inc" "$COMMON_ROOT/sensor/gps"; do
  if [ -d "$INC" ]; then
    CMD="$CMD -I$INC"
  else
    echo "⚠️  Warning: Include path not found: $INC"
  fi
done

CMD="$CMD ${CFLAGS} ${CPPFLAGS} ADCM_KATECH.cpp -o LIB.o"
echo "Executing: $CMD"
eval $CMD

echo "Archiving..."
${AR_BIN} rcs LIB_KATECH.a LIB.o
echo "Done: LIB_KATECH.a created"
