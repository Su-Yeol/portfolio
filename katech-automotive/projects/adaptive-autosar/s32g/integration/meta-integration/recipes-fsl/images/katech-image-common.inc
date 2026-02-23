DEPENDS += " jansson"

TOOLCHAIN_TARGET_TASK:append = " \
    libstdc++-dev \
    glibc \
    poco-dev \
    poco \
    jansson-dev \
    libstdc++-staticdev \
    openssl-dev \
    opencv-dev \
    opencv-staticdev \
    protobuf-c-dev \
    rapidjson-dev \
    googletest-staticdev \
"

TOOLCHAIN_HOST_TASK += " \
    nativesdk-python3-jinja2 \
    nativesdk-python3-lxml \
    nativesdk-python3-markupsafe \
    nativesdk-cmake \
    nativesdk-protobuf-dev \
    nativesdk-rapidjson-dev \
    nativesdk-rapidjson \
"

IMAGE_INSTALL:append = " \
    python3-jinja2 \
    python3-lxml \
    python3-markupsafe \
    psmisc \
    procps \
    net-snmp \
    net-tools \
    iproute2 \
    poco \
    iputils \
    jansson-dev  \
    poco-dev \
    libstdc++-dev \
    glibc \
    openssl \
    libstdc++-staticdev \
    cgroup-lite \
    boost  \
    boost-dev  \
    protobuf-c \
"

