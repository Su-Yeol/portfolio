FROM nativebuildapd:jammy

USER root

# install tools for interactive use of the container and add builduser to the sudo group without password
RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y \
        bash-completion \
        clang-format\
        cppcheck \
        curl \
        iputils-ping \
        iproute2 \
        sudo \
        subversion\
        xmlstarlet \
    && apt-get autoremove -y \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && usermod -aG sudo builduser \
    && echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# change to builduser so that the vs code components will be installed into the buildusers home
USER builduser

COPY --chown=builduser:builduser settings.json download-vs-code-server.sh apdup-0.4.2.vsix /tmp/

COPY apx-completion /etc/bash_completion.d/apx-completion

RUN /tmp/download-vs-code-server.sh

RUN <<EOF
echo 'PS1=${PS1}'\''$(__git_ps1 "\[\e[0;92m\](%s) \[\e[0m\]")'\''
export CLANG_FORMAT_CMD=clang-format
export PATH=$PATH:/tmp/apd/sbin:/mnt/ara-api/tools/bin' >> /home/builduser/.bashrc
EOF

CMD ["/bin/bash"]
