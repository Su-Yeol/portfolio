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

SECTION = "devel"
LICENSE = "AUTOSAR"
#LIC_FILES_CHKSUM = "file://LICENSE;md5=0bb7e2542f1265cbbd98aca0b3c139ea"

inherit apd_application externalsrc

EXTERNALSRC := "${THISDIR}/../../../../axon-integration/meta-integration/KATECH/applications/Perception_Camera"

EXTRA_OECMAKE = " \
    -DTARGET_MACHINE=AdaptiveMachine \
    -DAPP_NAME=Perception_Camera \
    -DPROC_NAME=Perception_Camera \
"

# Please keep this list alphabetically sorted for convenience.
DEPENDS += " \
    katech-adaptivemachine-arxmls \
    katech-applications-arxmls \
    katech-common-machine-arxmls \
    katech-interfaces-arxmls \
    katech-network-arxmls \
    ara-arxmls \
    ara-com-lib \
    ara-core \
    ara-exec \
    ara-log \
    libeigen \
    python3-numpy \
    opencv \
    nats-c \
    libhailort \
"
