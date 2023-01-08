FROM ubuntu:22.04

# preventing interactive tzinfo config on `apt-get install cmake`
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    ffmpeg \
    imagemagick \
    lcov \
    libavformat-dev \
    libexiv2-dev \
    libgtest-dev \
    libimage-exiftool-perl \
    libjpeg-turbo-progs \
    librange-v3-dev \
    libspdlog-dev \
    libturbojpeg0-dev \
    pkg-config \
    qtbase5-dev \
    qttools5-dev \
    && rm -rf /var/lib/apt/lists/*

ARG USER_NAME=dev
ARG USER_UID=1000
ARG USER_GID=1000

RUN groupadd -g ${USER_GID} ${USER_NAME}; \
    useradd -l --uid ${USER_UID} --gid ${USER_GID} ${USER_NAME}

# Use docker run -u <USER_NAME> instead of setting the user here
#
# Not setting the user here allows to use the same Dockerfile with
# rootless podman and not having to modify the shared folders with
# `podman unshare chown ...`
#
# USER ${USER_NAME}

WORKDIR /tmp/build
