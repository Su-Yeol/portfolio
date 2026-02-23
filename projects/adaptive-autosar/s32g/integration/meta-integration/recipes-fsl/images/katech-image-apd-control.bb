#
# Copyright 2017-2024 NXP
#

require recipes-fsl/images/fsl-image-pfe.inc
require recipes-fsl/images/fsl-image-base.bb
require recipes-fsl/images/fsl-image-core-common.inc
include recipes-fsl/images/fsl-image-s32-common.inc

require ../../../../adaptive-platform-24-11/yocto-layers/meta-ara/recipes-core/images/core-image-adcm-devel.bb
require katech-image-apd-debug.inc
include katech-image-common.inc

# copy the manifest and the license text for each package to image
COPY_LIC_MANIFEST = "1"
COPY_LIC_DIRS = "1"

IMAGE_INSTALL += " \
    dtc \
    kernel-devicetree \
    packagegroup-core-buildessential \
    packagegroup-core-full-cmdline \
    packagegroup-core-nfs-server \
    packagegroup-core-tools-debug \
    vim \
"

# Benchmark tools
IMAGE_INSTALL += "dhrystone fio"

IMAGE_INSTALL:append:s32cc = " perf"

# PCIe demos
IMAGE_INSTALL:append:s32cc = "${@bb.utils.contains('DISTRO_FEATURES', 'pcie-demos', \
	' kernel-pcitest demo-pcie-shared-mem', '', d)}"


# Support for accessing MDIO bus for GMAC phys
IMAGE_INSTALL:append:s32cc = " mdio-proxy "

# Tool for flashing the AQR107 firmware using mdio-proxy
IMAGE_INSTALL:append:s32cc = " aquantia-firmware-utility "

# Supporting complex evaluation scenarios
IMAGE_INSTALL += "openssl-misc"
IMAGE_INSTALL:append:s32 = " openssl openssl-dev libcrypto libssl openssl-conf openssl-engines openssl-bin"
IMAGE_INSTALL:remove:s32 = "ipsec-tools"

# Increase the freespace
IMAGE_ROOTFS_EXTRA_SPACE ?= "54000"

# Enable LXC features.
# On LS2 enable it by default. On s32, only by DISTRO_FEATURE
LXC_INSTALL_PACKAGES = "lxc debootstrap"
IMAGE_INSTALL:append:s32 = "${@bb.utils.contains('DISTRO_FEATURES', 'lxc', ' ${LXC_INSTALL_PACKAGES}', '', d)}"
IMAGE_INSTALL:append:ls2 = " ${LXC_INSTALL_PACKAGES}"

# SFTP server
IMAGE_INSTALL:append = " openssh openssh-sftp openssh-sftp-server "

# Other useful tools
IMAGE_INSTALL:append = " rsync irqbalance i2c-tools linuxptp"

# sysfs gpio interface is deprecated, include gpiod tools, lib and headers
IMAGE_INSTALL:append = " libgpiod libgpiod-tools libgpiod-dev"

# add cpufrequtils package 
IMAGE_INSTALL:append = " cpufrequtils"

IMAGE_INSTALL:append = " init-ifupdown"

IMAGE_INSTALL:append = " ppp minicom "
IMAGE_INSTALL:append = " python3-pysocks "
IMAGE_INSTALL:append = " linuxptp "

TOOLCHAIN_TARGET_TASK:append = " \
 proj \
 nats-c \
 poco \
"

IMAGE_INSTALL:append = " \
 proj \
 nats-c \
 ppp \
 poco \
"

do_ethernet_setup_script() {
	# execution script
	install -m 0755 ${THISDIR}/files/eth_setup ${IMAGE_ROOTFS}/etc/

	# copy service file
	install -m 0644 ${THISDIR}/files/eth_setup.service ${IMAGE_ROOTFS}/lib/systemd/system/

	# enable service
	ln -sf /lib/systemd/system/eth_setup.service ${IMAGE_ROOTFS}/etc/systemd/system/multi-user.target.wants/eth_setup.service
}
addtask do_ethernet_setup_script after do_rootfs before do_image


IMAGE_INSTALL:remove = " openssh \
                        packagegroup-core-ssh-openssh \
                        testappl2 \
                        linux-qspi-tool \
"


