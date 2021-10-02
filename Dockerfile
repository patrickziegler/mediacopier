FROM ubuntu:20.04

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
    libjpeg-dev \
    libjpeg-progs \
    libspdlog-dev \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

ARG USER_NAME=dev
ARG USER_UID=1000
ARG USER_GID=1000
ARG BUILD_DIR=/tmp/build

RUN groupadd -g ${USER_GID} ${USER_NAME}; \
    useradd -l --uid ${USER_UID} --gid ${USER_GID} ${USER_NAME}; \
    mkdir -p ${BUILD_DIR}; \
    chown ${USER_NAME} ${BUILD_DIR}

USER ${USER_NAME}

WORKDIR ${BUILD_DIR}
