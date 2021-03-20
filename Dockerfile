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
    libimage-exiftool-perl \
    libjpeg-dev \
    libjpeg-progs \
    liblog4cplus-dev \
    qtbase5-dev \
    && rm -rf /var/lib/apt/lists/*

ARG USER_NAME=dev
ARG USER_ID=1000
ARG USER_GID=1000

RUN groupadd -g ${USER_GID} ${USER_NAME}; \
    useradd -l --uid ${USER_ID} --gid ${USER_GID} ${USER_NAME}

USER ${USER_NAME}

WORKDIR /tmp/build
