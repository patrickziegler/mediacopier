# :camera: mediacopier

[![Build Status](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml/badge.svg?branch=master)](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml?query=branch%3Amaster)

This is an app that searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format.
The original creation date is used to **generate a folder structure** and unique filenames.
It supports a wide variety of image and videos formats (including raw) and features **lossless on-the-fly auto-rotation of JPEG files**.

<!--gif was created with 'ffmpeg -i capture.mp4 -r 10 -vf "fps=10,scale=830:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 demo.gif'-->
The app focusses on integrating into the native [KDE Plasma](https://kde.org/de/) notification system and context menu ([screenshot](https://i.imgur.com/LF5Vnj9.mp4)) but other desktop environments are supported as well.

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

### :factory: Containerized build environment

Build the container image as specified in the `Dockerfile`

```sh
docker build \
    --build-arg USER_NAME=$(id -nu) \
    --build-arg USER_UID=$(id -u) \
    --build-arg USER_GID=$(id -g) \
    -t mediacopier-build .
```

Start the build environment with the following command

```sh
docker run -it --rm -v ${PWD}:/usr/src/mediacopier -u $(id -nu) mediacopier-build
```

Alternatively, with rootless podman you don't have to specify the user

```sh
podman run -it --rm -v ${PWD}:/usr/src/mediacopier mediacopier-build
```

Inside the container, run the test suite with the following commands

```sh
cmake -DENABLE_TEST=ON /usr/src/mediacopier/ && make -j $(nproc) && make test
```

Alternatively, create a test coverage report like this (result can also be found [here](https://coveralls.io/github/patrickziegler/MediaCopier))

```sh
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

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
