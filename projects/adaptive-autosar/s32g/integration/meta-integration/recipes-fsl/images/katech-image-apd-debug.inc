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

LICENSE = "AUTOSAR"

# ':append' syntax has to be used according to
# https://www.yoctoproject.org/docs/2.1.3/ref-manual/ref-manual.html#var-IMAGE_INSTALL.
# Please keep this list alphabetically sorted for convenience.
IMAGE_INSTALL:append = " \
apd-crc-test \
    apd-manifestreader-test \
    apd-testutils-test \
    ara-com-lib-test \
    ara-core-test \
    ara-core-types-test \
    ara-dropbear \
    ara-exec-test \
    ara-gen \
    ara-getty \
    ara-iam-test \
    ara-log-test \
    ara-per-test \
    ara-phm-test \
    ara-syslogd \
    ara-tsync-test \
    ara-unit-test-runner \
    diag-calculator-example \
    diag-integration-test \
    diag-seedkey-example \
    diagnostic-manager-test \
    gdbserver \
    glibc-utils \
    libe2exf-test \
    localedef \
    openssh-sftp-server \
    osi \
    ucm-libs-test \
    valgrind \
"