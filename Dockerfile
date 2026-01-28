FROM ubuntu:24.04

# preventing interactive tzinfo config on `apt-get install cmake`
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    gcc-14 \
    g++-14 \
    pkg-config \
    cmake \
    ffmpeg \
    imagemagick \
    lcov \
    clang-tidy \
    clang-format \
    libavformat-dev \
    libexiv2-dev \
    libgtest-dev \
    libimage-exiftool-perl \
    libjpeg-turbo-progs \
    libspdlog-dev \
    libturbojpeg0-dev \
    libcli11-dev \
    libtoml11-dev \
    qtbase5-dev \
    qttools5-dev \
    libkf5i18n-dev \
    libkf5jobwidgets-dev \
    && rm -rf /var/lib/apt/lists/*

# workaround for https://bugs.launchpad.net/ubuntu/+source/toml11/+bug/1978418
RUN ln -s /usr/lib/share/cmake/toml11/ /usr/lib/x86_64-linux-gnu/cmake/toml11

ARG USER_NAME=dev
ARG USER_ID=1000
ARG GROUP_ID=1000

RUN groupadd -g ${GROUP_ID} ${USER_NAME} || true \
 && useradd -l -u ${USER_ID} -g ${GROUP_ID} ${USER_NAME} || true

# Use docker run -u <USER_NAME> instead of setting the user here
#
# Not setting the user here allows to use the same Dockerfile with
# rootless podman and not having to modify the shared folders with
# `podman unshare chown ...`
#
# USER ${USER_NAME}

ENV CC=gcc-14 \
    CXX=g++-14 \
    LCOV_GCOV_TOOL=gcov-14

WORKDIR /tmp/build
