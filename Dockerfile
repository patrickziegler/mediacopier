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
