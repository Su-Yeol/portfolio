#  --------------------------------------------------------------------------
#  |              _    _ _______     .----.      _____         _____        |
#  |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
#  |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
#  |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
#  |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
#  |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
#  |                              . _ _  .                                  |
#  --------------------------------------------------------------------------
#
#  All Rights Reserved.
#  Any use of this source code is subject to a license agreement with the
#  AUTOSAR development cooperation.
#  More information is available at www.autosar.org.
#
#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------

# Please keep this list alphabetically sorted for convenience.
IMAGE_INSTALL = " \
    ${CORE_IMAGE_EXTRA_INSTALL} \
    packagegroup-ara-base \
"

IMAGE_LINGUAS = " "

LICENSE = "AUTOSAR"

inherit core-image

IMAGE_ROOTFS_SIZE = "8192"

# for creating the process json file below
DEPENDS += " \
    jq-native\
"

# ':append' syntax has to be used according to
# https://www.yoctoproject.org/docs/2.1.3/ref-manual/ref-manual.html#var-IMAGE_INSTALL.
# Please keep this list alphabetically sorted for convenience.
IMAGE_INSTALL:append = " \
    ara-exec \
    ara-exec-executables \
    ara-iam \
    apd-minimalmachine-systemrestart \
    apd-minimalmachine-systemshutdown \
    ara-phm \
    rootfs-open \
    vsomeip \
	diagnostic-manager \
    vsomeip-examples \
"

write_init_link() {
    ln -sf /usr/sbin/execution-manager ${IMAGE_ROOTFS}/sbin/init
}

create_processes_json() {
  # find all application manifests and add to the process list
  for process in $(cd ${IMAGE_ROOTFS}; find var/lib/apd_ucm/swcls -path \*/etc/MANIFEST.json | sed "s:/etc/MANIFEST.json::"); do
    # create a working copy, because jq cannot operate in place
    cp ${IMAGE_ROOTFS}/var/lib/apd_ucm/PROCESSES.json ${WORKDIR}/PROCESSES.json
    # add process application folder to the json file
    jq ".value.string |= .+ [\"/$process\"]" ${WORKDIR}/PROCESSES.json > ${IMAGE_ROOTFS}/var/lib/apd_ucm/PROCESSES.json
    rm ${WORKDIR}/PROCESSES.json
  done
  sed -i "s:/:\\\/:g" ${IMAGE_ROOTFS}/var/lib/apd_ucm/PROCESSES_A.json
}

# TODO: temporary workaround for TFAPD-5855, to be removed when proper fix is implemented
fix_dlt_remote() {
  for MANIFEST in $(cd ${IMAGE_ROOTFS}; find opt -path \*/etc/MANIFEST.json); do
      TMPFILE=$(mktemp)
      # redirect into tmpfile, because jq cannot operate in place
      jq '.logtrace_log_modes |= . + ["NETWORK"]' ${IMAGE_ROOTFS}/$MANIFEST > $TMPFILE
      # replace original content with modified content and remove tmpfile
      cat $TMPFILE > ${IMAGE_ROOTFS}/$MANIFEST
      rm $TMPFILE
  done
}

#ROOTFS_POSTPROCESS_COMMAND += "write_init_link; fix_dlt_remote; "
ROOTFS_POSTPROCESS_COMMAND += " fix_dlt_remote; "
