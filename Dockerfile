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
    libavformat-dev \
    libexiv2-dev \
    libgtest-dev \
    libimage-exiftool-perl \
    libjpeg-turbo-progs \
    libspdlog-dev \
    libturbojpeg0-dev \
    qtbase5-dev \
    qttools5-dev \
    libcli11-dev \
    libtoml11-dev \
    && rm -rf /var/lib/apt/lists/*

# workaround for https://bugs.launchpad.net/ubuntu/+source/toml11/+bug/1978418
RUN ln -s /usr/lib/share/cmake/toml11/ /usr/lib/x86_64-linux-gnu/cmake/toml11

WORKDIR /tmp/build
