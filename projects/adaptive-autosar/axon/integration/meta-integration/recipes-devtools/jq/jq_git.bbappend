# nativesdk-jq rpath QA fix
do_install:append:class-nativesdk () {
    if [ -f "${D}${bindir}/jq" ]; then
        # chrpath 있으면 그걸로 제거
        if command -v chrpath >/dev/null 2>&1; then
            chrpath -d "${D}${bindir}/jq" || true
        fi
        # patchelf가 있으면 그것도 시도
        if command -v patchelf >/dev/null 2>&1; then
            patchelf --remove-rpath "${D}${bindir}/jq" || true
        fi
    fi
}

