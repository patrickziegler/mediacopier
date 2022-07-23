# :camera: MediaCopier

[![Build Status](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml/badge.svg?branch=master)](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml?query=branch%3Amaster)
[![Coverage Status](https://coveralls.io/repos/github/patrickziegler/MediaCopier/badge.svg?branch=master)](https://coveralls.io/github/patrickziegler/MediaCopier?branch=master)
![C++ Version](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)

## Features
This software searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format.
The original creation date is used to **generate a folder structure** and unique filenames.
Raw image files and videos are supported as well.

This library supports **lossless auto-rotation of JPEG files with known orientation** (on the fly) when image dimensions are appropriate (divisible by 16).
If the orientation is unkown or the dimensions are inconvenient they will be copied as they are.

## Getting Started

### :hammer: Build and Install

Direct dependencies (library):
- spdlog (https://github.com/gabime/spdlog) >=[1.9.2](https://github.com/gabime/spdlog/releases/tag/v1.9.2)
- Exiv2 (https://exiv2.org/)
- libjpeg-turbo (https://libjpeg-turbo.org/)
- ffmpeg (https://www.ffmpeg.org/download.html)

Direct dependencies (cli):
- range-v3 (https://github.com/ericniebler/range-v3)
- Qt5 (https://doc.qt.io/qt-5/)

Clone this repository and create a build directory
```sh
git clone --recursive https://github.com/patrickziegler/MediaCopier.git
cd MediaCopier && mkdir build && cd build
```

Build and install the package
```sh
cmake -DCMAKE_INSTALL_PREFIX=/usr .. && make -j$(nproc) && sudo make install
```

Available cmake flags

| Flag                   | Description                               | Default   |
|------------------------|-------------------------------------------|-----------|
| `ENABLE_CLI`           | Build Qt based cli tool                   | `ON`      |
| `ENABLE_KDE`           | Enable KDE integration for cli tool       | `OFF`     |
| `ENABLE_SHARED_LIB`    | Build shared instead of static lib        | `OFF`     |
| `ENABLE_TEST`          | Enable test targets                       | `OFF`     |
| `ENABLE_TEST_COVERAGE` | Enable test and coverage targets          | `OFF`     |
| `INSTALL_DEV_FILES`    | Install library headers and cmake targets | `OFF`     |

### :factory: Build and Test with Docker

Build an image and run it as a containerized build environment.
```sh
docker build \
    --build-arg USER_NAME=$(whoami) \
    --build-arg USER_UID=$(id -u) \
    --build-arg USER_GID=$(id -g) \
    -t mediacopier-build .
docker build \
    --build-arg USER_NAME=root \
    --build-arg USER_UID=0 \
    --build-arg USER_GID=0 \
    -t mediacopier-build .
docker run -it --rm -v ${PWD}:/usr/src/mediacopier mediacopier-build
```

Inside the container, run the following commands.
All build-time dependencies are solved already.
```sh
cmake -DENABLE_TEST=ON /usr/src/mediacopier/ && make -j $(nproc) && make test
cmake -DENABLE_TEST_COVERAGE=ON /usr/src/mediacopier/ && make -j $(nproc) && make coverage
```

### :paperclip: Build Instructions for Windows

Prepare the [vcpkg](https://github.com/microsoft/vcpkg#using-vcpkg-with-cmake) environment like described in a very helpful [article by Sam Elborai](https://sam.elborai.me/articles/vscode-cpp-dev-environment-2020/)
```sh
.\bootstrap-vcpkg.bat -disableMetrics
.\vcpkg.exe install spdlog exiv2 libjpeg-turbo ffmpeg range-v3 qt5 --triplet=x64-windows
.\vcpkg.exe list --triplet=x64-windows # check installed packages
```

Use the vcpkg toolchain file with cmake
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -B build -S .
cmake --build build --config Release
```

### :speech_balloon: Translations

```sh
linguist-qt5 mediacopier-cli/lang/lang_de.ts
```

## Demo

The tool can be built with seamless integration into the KDE desktop environment (`ENABLE_KDE=ON`).
It is then available in the context menu for any given folder and will report its progress via the native notification system.

[comment]: https://imgur.com/a/8vp34Q7
![](https://i.imgur.com/lPUpAyN.gif)

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
