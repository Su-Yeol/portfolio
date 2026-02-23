# meta-katech-apd/recipes-ara/exec/ara-exec_%.bbappend

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
inherit systemd
SRC_URI += "file://ara-exec.service"

SYSTEMD_SERVICE:${PN} = "ara-exec.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install:append() {
    install -d ${D}${systemd_unitdir}/system
    install -m 0644 ${WORKDIR}/ara-exec.service ${D}${systemd_unitdir}/system/

    # 원 레시피가 만든 /sbin/init 제거
    rm -f ${D}${base_sbindir}/init || true

    # 🔧 이 레시피가 만든 빈 /sbin 디렉터리도 정리(다른 패키지가 다시 만들므로 여기서 지워도 안전)
    if [ -d ${D}${base_sbindir} ]; then
        rmdir --ignore-fail-on-non-empty ${D}${base_sbindir} 2>/dev/null || true
    fi
}

FILES:${PN} += "${systemd_unitdir}/system/ara-exec.service"

# 서브패키지 쪽 정리(virtual/init/alternatives 제거, /sbin 제외)
RPROVIDES:${PN}-executables:remove = "virtual/init"
PROVIDES:${PN}-executables:remove  = "virtual/init"
RCONFLICTS:${PN}-executables = ""
ALTERNATIVE:${PN}-executables = ""
ALTERNATIVE_LINK_NAME[init] = ""
ALTERNATIVE_TARGET[init] = ""

# ${base_sbindir}를 아예 포함하지 않도록 재정의
FILES:${PN}-executables = "\
    ${bindir} \
    ${sbindir} \
    ${libdir} \
    ${libexecdir} \
    ${sysconfdir} \
    ${datadir} \
    ${localstatedir} \
"
