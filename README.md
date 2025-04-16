# :camera: mediacopier

[![Build Status](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml/badge.svg?branch=master)](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml?query=branch%3Amaster)

This is an app that searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format.
The original creation date is used to **generate a folder structure** and unique filenames.
It supports a wide variety of image and videos formats (including raw) and features **lossless on-the-fly auto-rotation of JPEG files**.

<!--gif was created with 'ffmpeg -i capture.mp4 -r 10 -vf "fps=10,scale=830:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 demo.gif'-->
The app focusses on integrating into the native [KDE Plasma](https://kde.org/de/) notification system and context menu ([screenshot](https://i.imgur.com/LF5Vnj9.mp4)) but other desktop environments are supported as well.

## Getting Started

### :anchor: Install via Package Manager

#### openSUSE

[![build result](https://build.opensuse.org/projects/home:zipat:mediacopier/packages/mediacopier/badge.svg?type=default)](https://build.opensuse.org/package/show/home:zipat:mediacopier/mediacopier)

There are packages being built with the [Open Build Service](https://openbuildservice.org/). You can add the repostory to your distribution and install `mediacopier` easily with the following commands:

```
zypper ar -f https://download.opensuse.org/repositories/home:/zipat:/mediacopier/openSUSE_Tumbleweed/ mediacopier
zypper in mediacopier-plasma
```

### :hammer: Build and Install

Direct dependencies (library):
- spdlog (https://github.com/gabime/spdlog) >=[1.9.2](https://github.com/gabime/spdlog/releases/tag/v1.9.2)
- toml11 (https://github.com/ToruNiina/toml11)
- Exiv2 (https://exiv2.org/)
- libjpeg-turbo (https://libjpeg-turbo.org/)
- ffmpeg (https://www.ffmpeg.org/download.html)

Direct dependencies (tools):
- CLI11 (https://github.com/CLIUtils/CLI11)
- range-v3 (https://github.com/ericniebler/range-v3)
- Qt5 (https://doc.qt.io/qt-5/)
- KJobWidgets (https://api.kde.org/frameworks/kjobwidgets/html/index.html)

For example on openSUSE, these dependencies can be installed via

```sh
zypper install spdlog-devel toml11-devel libexiv2-devel libjpeg8-devel ffmpeg-6-libavformat-devel ffmpeg-6-libavutil-devel # for the core library
zypper install cli11-devel range-v3-devel # for the pure command line interface
zypper install libQt5Widgets-devel libqt5-linguist-devel ki18n-devel kjobwidgets-devel # for the Qt5 based graphical user interface
zypper install ki18n-devel kjobwidgets-devel # for the KDE Plasma integration with Qt5
zypper install qt6-core-devel qt6-widgets-devel qt6-statemachine-devel qt6-linguist-devel # for Qt6 based graphical user interface 
zypper install kf6-ki18n-devel kf6-kjobwidgets-devel # for the KDE Plasma integration with Qt6
zypper install gtest lcov exiftool ImageMagick # for testing
```

Clone this repository and create a build directory

```sh
git clone --recursive https://github.com/patrickziegler/MediaCopier.git
cd MediaCopier && mkdir build && cd build
```

Build and install the package
```sh
export CXX=/usr/bin/g++-13 # set specific compiler (optional)
cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=release .. && make -j$(nproc) && sudo make install
```

Available cmake flags

| Flag                   | Description                                      | Default |
|------------------------|--------------------------------------------------|---------|
| `SKIP_GUI`             | Don't build graphical user interface             | `OFF`   |
| `SKIP_CLI`             | Don't build plain command line interface         | `OFF`   |
| `SKIP_KDE`             | Don't build KDE Plasma integration               | `OFF`   |
| `USE_QT5`              | Build against Qt5 libraries (legacy)             | `OFF`   |
| `ENABLE_TEST`          | Enable test targets                              | `OFF`   |
| `ENABLE_TEST_COVERAGE` | Enable test and coverage targets                 | `OFF`   |
| `INSTALL_DEV_FILES`    | Install library headers and cmake targets        | `OFF`   |

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
cmake -DUSE_QT5=ON -DSKIP_GUI=ON -DSKIP_KDE=ON -DENABLE_TEST=ON /usr/src/mediacopier/ && make -j $(nproc) && make test
```

Alternatively, create a test coverage report like this (result can also be found [here](https://coveralls.io/github/patrickziegler/MediaCopier))

```sh
cmake -DUSE_QT5=ON -DSKIP_GUI=ON -DSKIP_KDE=ON -DENABLE_TEST_COVERAGE=ON /usr/src/mediacopier/ && make -j $(nproc) && make coverage
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
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DSKIP_KDE=ON -B build -S .
cmake --build build --config Release
```

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
