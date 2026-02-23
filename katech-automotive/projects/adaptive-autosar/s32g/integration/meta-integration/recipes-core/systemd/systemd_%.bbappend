FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI += "file://10-pfe0.network \
            file://10-pfe1.network \
            file://10-pfe2.network \
            file://10-eth0.network"

do_install:append () {
    install -d ${D}${sysconfdir}/systemd/network
    install -m 0644 ${WORKDIR}/10-pfe0.network ${D}${sysconfdir}/systemd/network/10-pfe0.network
    install -m 0644 ${WORKDIR}/10-pfe1.network ${D}${sysconfdir}/systemd/network/10-pfe1.network
    install -m 0644 ${WORKDIR}/10-pfe2.network ${D}${sysconfdir}/systemd/network/10-pfe2.network
    install -m 0644 ${WORKDIR}/10-eth0.network ${D}${sysconfdir}/systemd/network/10-eth0.network
}