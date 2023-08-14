FROM mcr.microsoft.com/devcontainers/base:jammy

RUN apt update && apt install -y \
    build-essential \
    pkg-config \
    cmake \
    libboost-dev \
    libboost-program-options-dev \
    lsb-release \
    software-properties-common \
    gnupg \
    apt-transport-https

# Install dkp-pacman
RUN mkdir -p /usr/local/share/keyring/ && \
    wget -O /usr/local/share/keyring/devkitpro-pub.gpg https://apt.devkitpro.org/devkitpro-pub.gpg && \
    echo "deb [signed-by=/usr/local/share/keyring/devkitpro-pub.gpg] https://apt.devkitpro.org stable main" \
        > /etc/apt/sources.list.d/devkitpro.list && \
    apt update && \
    apt install -y devkitpro-pacman

# Install devkitPPC
RUN bash -c '[[ ! -f /etc/mtab ]] && ln -s /proc/mounts /etc/mtab || true' && \
    dkp-pacman -S gamecube-dev --noconfirm
ENV DEVKITPPC=/opt/devkitpro/devkitPPC

# Install latest clangd
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)" && \
    ln -s /usr/bin/clangd-* /usr/local/bin/clangd
