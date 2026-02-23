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
#LIC_FILES_CHKSUM = "file://LICENSE;md5=a724db7aa5fc5686d473b9838d849cfb"

inherit apd_application externalsrc

EXTERNALSRC := "${THISDIR}/../../../../axon-integration/meta-integration/KATECH/applications/V2X"

EXTRA_OECMAKE = " \
    -DTARGET_MACHINE=AdaptiveMachine \
    -DAPP_NAME=V2X \
    -DPROC_NAME=V2X \
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
"

do_install:append() {
    install -d ${D}${libdir} 
    install -m 0755 ${S}/lib/librvpj.so ${D}${libdir}
}

FILES:${PN} += "${libdir}/*"
FILES_SOLIBSDEV = ""
INSANE_SKIP:${PN} += "dev-so"
