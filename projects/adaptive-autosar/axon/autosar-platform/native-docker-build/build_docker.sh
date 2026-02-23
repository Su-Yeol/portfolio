#!/bin/bash
unalias -a
set -eEuo pipefail
trap 's=$?; echo "$0: Error $s on line "$LINENO": $BASH_COMMAND"; exit $s' ERR
IFS=$'\n\t'
shopt -s failglob

WORKSPACE="${PWD}"
TARGET_MACHINE="${TARGET_MACHINE:-RadarFusionMachine}"
TARGET_MACHINE="RadarFusionDDSMachine"
if [ ${TARGET_MACHINE} == "MinimalMachine" ]; then
    MACHINE_DIR="${WORKSPACE}/../ara-api/apd/minimal-machine/machines/${TARGET_MACHINE}/native"
elif [ ${TARGET_MACHINE} == "RadarMachine" ] || [ ${TARGET_MACHINE} == "FusionMachine" ] || [ ${TARGET_MACHINE} == "RadarFusionMachine" ] || [ ${TARGET_MACHINE} == "RadarFusionWRSOMEIPMachine" ]; then
    MACHINE_DIR="${WORKSPACE}/../sample-applications/machines/${TARGET_MACHINE}/native"
elif [ ${TARGET_MACHINE} == "RadarFusionDDSMachine" ]; then
    MACHINE_DIR="${WORKSPACE}/../sample-applications/machines/${TARGET_MACHINE}/native"
else
    echo "Unsupported machine ${TARGET_MACHINE}"
    exit 1
fi
echo "Building for ${TARGET_MACHINE}"
ARA_API="${WORKSPACE}/../ara-api"
SAMPLE_APP="${WORKSPACE}/../sample-applications"
YOCTO="${WORKSPACE}/../yocto-layers"
BUILD_DIR="/mnt/build_docker/${TARGET_MACHINE}"
INSTALL_DIR="/tmp/apd"
# Benchmarks show "Ninja" is faster than "Unix Makefiles" in multicore setups (>=2)
# and minimally slower in single core setups
BUILD_GENERATOR="Ninja"
ENABLE_CLIENTSIDELOGGING="0"
# TODO: temporary workaround for TFAPD-5855, to be removed when proper fix is implemented
ENABLE_DLTNETWORKLOGGING="1"
ENABLE_VSOMEIPTRACE="0"

cmake -DCMAKE_BUILD_TYPE=Debug \
      -DARA_ENABLE_TESTS=ON \
      -DTARGET_MACHINE=${TARGET_MACHINE} \
      -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} \
      -S ${MACHINE_DIR} \
      -B ${BUILD_DIR} \
      -G ${BUILD_GENERATOR}

# minimal set needed for UCM operation in radarfusionmachine
BUILD_TARGETS=( \
apd-radarfusionmachine-statemanager \
apd-radarfusionmachine-smtool \
apd-radarfusionmachine-artifacts \
)

# complete machine content (comment to build only the subset above)
BUILD_TARGETS=(all)

if [ ${TARGET_MACHINE} = "RadarFusionMachine" ]; then
    DIAGNOSIS_BYTE="01"
elif [ ${TARGET_MACHINE} = "RadarMachine" ]; then
    DIAGNOSIS_BYTE="46"
elif [ ${TARGET_MACHINE} = "FusionMachine" ]; then
    DIAGNOSIS_BYTE="52"
elif [ ${TARGET_MACHINE} = "RadarFusionDDSMachine" ]; then
    DIAGNOSIS_BYTE="01"
fi
nice -n 19 cmake --build ${BUILD_DIR} --target ${BUILD_TARGETS[*]} -j $(nproc)
#nice -n 19 env DESTDIR="${INSTALL_DIR}" cmake --build "${BUILD_DIR}" --target ${BUILD_TARGETS[*]} -j "$(nproc)"
#Copy dlt configuration file.
sudo cp -uv ${INSTALL_DIR}/etc/dlt.conf /etc/dlt.conf
#Without these paths access-manager, phm and secdemofc are terminating.This can be fixed by using their CMake files.
for dir in /var/state_client \
    /var/redirected \
    /var/fc_firewall \
    /usr/run/access-manager/ \
    /usr/run/ara-phm/ \
    /usr/run/ara-exec/ \
    /usr/run/secdemofc/; do
    sudo mkdir -p $dir
    mkdir -p ${INSTALL_DIR}/$dir
    sudo chown builduser:builduser $dir
done


for i in $(find ${INSTALL_DIR}/lib -type l -name \*.so\*); do
    sudo cp -vP $i /lib
done


for i in $(find "${INSTALL_DIR}/lib" -type f -name '*.so*'); do
    target="/lib/$(basename "$i")"
    if [ -e "$target" ]; then
        echo "Skip (already exists): $target"
    else
        echo "Copying: $i -> $target"
        sudo cp -vl $i /lib
    fi
done


mkdir -p ${INSTALL_DIR}/usr/bin/
ln -sf /usr/bin/dlt-daemon ${INSTALL_DIR}/usr/bin/dlt-daemon

if [ -n "${TARGET_MACHINE##*WRSOMEIP*}" ]; then
    # if wr someipDaemon exists, EM will use it instead of vsomeip routingmanagerd, so delete it on non WRSOMEIP machines
    sudo rm -f ${INSTALL_DIR}/bin/someipDaemon
    ln -sf /usr/bin/routingmanagerd ${INSTALL_DIR}/usr/bin/routingmanagerd
fi

if [ ${ENABLE_VSOMEIPTRACE} -eq 1 ]; then
    # trace options to inject into the vsomeip cfg
    TRACE_FILTER=". |= . + {\"tracing\":{\"sd_enable\":\"true\",\"enable\":\"true\"}} | "
else
    # no tracing
    TRACE_FILTER=""
fi

# modify vsomeip.json
if ! [ -z ${DIAGNOSIS_BYTE+x} ]; then
    # Setting diagnosis byte is mandatory in multi-machine setup, see AR-117829
    TMPFILE_VSOMEIP=$(mktemp)
    jq "${TRACE_FILTER}. + {\"diagnosis\" : \"0x${DIAGNOSIS_BYTE}\", \"diagnosis_mask\":\"0xFF00\"}" ${INSTALL_DIR}/etc/machine_artifacts/vsomeip.json > ${TMPFILE_VSOMEIP}
    chmod +w ${INSTALL_DIR}/etc/machine_artifacts/vsomeip.json
    cat ${TMPFILE_VSOMEIP} > ${INSTALL_DIR}/etc/machine_artifacts/vsomeip.json
    rm ${TMPFILE_VSOMEIP}
fi

# Post processing for application manifest files
for manifest in $(find ${INSTALL_DIR}/opt -name MANIFEST.json); do
    tmpfile=$(mktemp)
    if [ ${ENABLE_CLIENTSIDELOGGING} -eq 1 ]; then
        CLIENTSIDELOGGING='.startup_configs[].environment |= . + ["VSOMEIP_CLIENTSIDELOGGING=\"\""] | '
    else
        CLIENTSIDELOGGING=""
    fi
    if [ ${ENABLE_DLTNETWORKLOGGING} -eq 1 ]; then
        DLTNETWORKLOGGING='.logtrace_log_modes |= . + ["NETWORK"] | '
    else
        DLTNETWORKLOGGING=""
    fi
    jq ". ${CLIENTSIDELOGGING} ${DLTNETWORKLOGGING} \
        (if (.logtrace_file_path != null and .logtrace_file_path != \"\") then .logtrace_file_path = \"${INSTALL_DIR}\" + .logtrace_file_path else . end) | \
        .startup_configs[]?.environment[]? |= sub(\"(?<k>.*_CONFIGURATION)=(?<v>.+)\"; \"\(.k)=${INSTALL_DIR}\(.v)\") | \
        .persistency_port_prototype_to_key_value_database_mapping[]?.key_value_database.uri |= \"${INSTALL_DIR}\" + ." \
        $manifest > $tmpfile
    sudo rm -f $manifest
    mv $tmpfile $manifest
done

# fix some UCM test data paths
if [ -f ${INSTALL_DIR}/opt/pkgmgr_sample/etc/MANIFEST.json ]; then
    tmpfile=$(mktemp)
    jq ".startup_configs[].arguments |= . + [\"--path\", \"${INSTALL_DIR}/share/apdtest/ucm/\"]" ${INSTALL_DIR}/opt/pkgmgr_sample/etc/MANIFEST.json > ${tmpfile}
    mv ${tmpfile} ${INSTALL_DIR}/opt/pkgmgr_sample/etc/MANIFEST.json
fi

if [ -f ${INSTALL_DIR}/opt/ota_client/etc/MANIFEST.json ]; then
    tmpfile=$(mktemp)
    jq ".startup_configs[].arguments[]? |= sub(\"/usr(?<p>/share/apdtest/ucm)\"; \"${INSTALL_DIR}\(.p)\")" ${INSTALL_DIR}/opt/ota_client/etc/MANIFEST.json > ${tmpfile}
    mv ${tmpfile} ${INSTALL_DIR}/opt/ota_client/etc/MANIFEST.json
fi

if [ -f ${INSTALL_DIR}/opt/package_manager/etc/MANIFEST.json ]; then
    tmpfile=$(mktemp)
    jq ".startup_configs[].arguments |= . + [\"--root\", \"${INSTALL_DIR}\"]" ${INSTALL_DIR}/opt/package_manager/etc/MANIFEST.json > ${tmpfile}
    mv ${tmpfile} ${INSTALL_DIR}/opt/package_manager/etc/MANIFEST.json
fi

if [ -f ${INSTALL_DIR}/opt/vehicle_package_manager/etc/MANIFEST.json ]; then
    tmpfile=$(mktemp)
    jq ".startup_configs[].arguments |= . + [\"--root\", \"${INSTALL_DIR}\"]" ${INSTALL_DIR}/opt/vehicle_package_manager/etc/MANIFEST.json > ${tmpfile}
    mv ${tmpfile} ${INSTALL_DIR}/opt/vehicle_package_manager/etc/MANIFEST.json
fi

if [ -f ${INSTALL_DIR}/opt/ara_tsync_daemon/etc/MANIFEST.json ]; then
    tmpfile=$(mktemp)
    jq ".startup_configs[]?.environment[]? |= sub(\"TSYNC_APP_INTERFACE=(?<v>.+)\"; \"TSYNC_APP_INTERFACE=${INSTALL_DIR}\(.v)\")" ${INSTALL_DIR}/opt/ara_tsync_daemon/etc/MANIFEST.json > ${tmpfile}
    mv ${tmpfile} ${INSTALL_DIR}/opt/ara_tsync_daemon/etc/MANIFEST.json
fi

#Install UCM application related files
if [ -f ${INSTALL_DIR}/share/ara-ucm/oemca_crt.pem ]; then
    mkdir -p ${INSTALL_DIR}/etc/ssl/certs
    cp -uv ${INSTALL_DIR}/share/ara-ucm/oemca_crt.pem ${INSTALL_DIR}/etc/ssl/certs/
fi

# create empty processes file
cat <<EOF > ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES_A.json
{
  "key": "processes",
  "value": {
    "string": [
    ]
  },
  "checksum": 870226170
}
EOF

rm -f ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES_B.json

ln -sf PROCESSES_A.json ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES.json

# find all application manifests and add to the process list
for process in $(cd /; find ${INSTALL_DIR}/var/lib/apd_ucm/swcls -path \*/etc/MANIFEST.json | sed "s:/etc/MANIFEST.json::"); do
    # create a working copy, because jq cannot operate in place
    TMPFILE=$(mktemp)
    cp ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES.json ${TMPFILE}
    # add process application folder to the json file
    jq ".value.string |= .+ [\"$process\"]" ${TMPFILE} > ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES_A.json
    rm ${TMPFILE}
done
sed -i "s:/:\\\/:g" ${INSTALL_DIR}/var/lib/apd_ucm/PROCESSES_A.json
if [ -x ${INSTALL_DIR}/sbin/smtool ]; then
    sed -i "s:^(cd /opt/smtool:(cd ${INSTALL_DIR}/opt/smtool:" ${INSTALL_DIR}/sbin/smtool
fi

# fix hardcoded unit test directories

for file in \
    apd-exec-testexecution-dependency \
    apd-exec-testexecution-manager \
    apd-exec-teststate-client \
    apd-exec-testupdate-reparsing \
    apd-sm-testcalibrationdata \
    apd-sm-teststatemachinehandler \
    apd-sm-teststatemachineparser
do
    if [ -x ${INSTALL_DIR}/bintest/$file ]; then
        sed -i "s:/usr/bintest:/tmp/apd/bintest:g" ${INSTALL_DIR}/bintest/$file
    fi
done


sudo cp -vrf /tmp/apd/sbin/execution-manager /usr/sbin

