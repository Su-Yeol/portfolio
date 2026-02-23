OPENDDS_NSDK_REAL_PREFIX:class-nativesdk = "${RECIPE_SYSROOT}/opt/poky-telechips-systemd/nodistro.0/sysroots/x86_64-oesdk-linux/usr"

do_configure:prepend:class-nativesdk () {
    echo "### KATECH: opendds nativesdk sysroot fix RUN ###"
    echo "### KATECH: real_prefix=${OPENDDS_NSDK_REAL_PREFIX} ###"

    # include/openssl
    install -d ${RECIPE_SYSROOT}/usr/include
    rm -rf ${RECIPE_SYSROOT}/usr/include/openssl
    ln -sf ${OPENDDS_NSDK_REAL_PREFIX}/include/openssl ${RECIPE_SYSROOT}/usr/include/openssl

    # include/xercesc (xerces3)
    rm -rf ${RECIPE_SYSROOT}/usr/include/xercesc
    ln -sf ${OPENDDS_NSDK_REAL_PREFIX}/include/xercesc ${RECIPE_SYSROOT}/usr/include/xercesc

    ls -l ${RECIPE_SYSROOT}/usr/include/xercesc/dom/DOM.hpp || true

    # lib (필요 시)
    install -d ${RECIPE_SYSROOT}/usr
    if [ -d "${OPENDDS_NSDK_REAL_PREFIX}/lib" ]; then
        rm -rf ${RECIPE_SYSROOT}/usr/lib
        ln -sf ${OPENDDS_NSDK_REAL_PREFIX}/lib ${RECIPE_SYSROOT}/usr/lib
    fi
    if [ -d "${OPENDDS_NSDK_REAL_PREFIX}/lib64" ]; then
        rm -rf ${RECIPE_SYSROOT}/usr/lib64
        ln -sf ${OPENDDS_NSDK_REAL_PREFIX}/lib64 ${RECIPE_SYSROOT}/usr/lib64
    fi

    # 디버그(없어도 됨)
    ls -l ${RECIPE_SYSROOT}/usr/include/openssl/opensslv.h || true
}
