# <img width="64px" src="https://raw.githubusercontent.com/patrickziegler/mediacopier/refs/heads/master/app/mediacopier/mediacopier.svg"> mediacopier

[![Build Status](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml/badge.svg?branch=master)](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml?query=branch%3Amaster)
[![Coverage Status](https://coveralls.io/repos/github/patrickziegler/mediacopier/badge.svg?branch=master)](https://coveralls.io/github/patrickziegler/mediacopier?branch=master)

This is an app that searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format.
The original creation date is used to **generate a folder structure** and unique filenames.
It supports a wide variety of image and videos formats (including raw) and features **lossless on-the-fly auto-rotation of JPEG files**.

<!--gif was created with 'ffmpeg -i capture.mp4 -r 10 -vf "fps=10,scale=830:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" -loop 0 demo.gif'-->
The app focusses on integrating into the native [KDE Plasma](https://kde.org/de/) notification system and context menu ([screenshot](https://i.imgur.com/LF5Vnj9.mp4)) but other desktop environments are supported as well.

Valid format specifiers for the renaming pattern can be found [here](https://en.cppreference.com/w/cpp/chrono/system_clock/formatter.html).

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
- Qt5 (https://doc.qt.io/qt-5/)
- KJobWidgets (https://api.kde.org/frameworks/kjobwidgets/html/index.html)

For example on openSUSE, these dependencies can be installed via

```sh
zypper install spdlog-devel toml11-devel libexiv2-devel libjpeg8-devel ffmpeg-7-libavformat-devel ffmpeg-7-libavutil-devel # for the core library
zypper install cli11-devel # for the pure command line interface
zypper install libQt5Widgets-devel libqt5-linguist-devel ki18n-devel kjobwidgets-devel # for the Qt5 based graphical user interface
zypper install ki18n-devel kjobwidgets-devel # for the KDE Plasma integration with Qt5
zypper install qt6-core-devel qt6-widgets-devel qt6-statemachine-devel qt6-linguist-devel # for Qt6 based graphical user interface 
zypper install kf6-ki18n-devel kf6-kjobwidgets-devel # for the KDE Plasma integration with Qt6
zypper install gtest lcov clang exiftool ImageMagick # for testing
```

Clone this repository and create a build directory

```sh
git clone https://github.com/patrickziegler/MediaCopier.git
cd MediaCopier && mkdir build && cd $_
```

Build and install the package
```sh
export CC=gcc-14 CXX=g++-14 # set specific compiler (optional)
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
| `ENABLE_CLANG_TIDY`    | Enable static code analysis with `clang-tidy`    | `OFF`   |

### :factory: Containerized build environment

Build und run the container image as specified in the `Dockerfile` with the following commands

```sh
docker build -t mediacopier-build .
docker run -it --rm -v ${PWD}:/run/src/mediacopier -u $(id -u):$(id -g) mediacopier-build
```

Inside the container, run the test suite or generate coverage reports with the following commands

```sh
cmake -DUSE_QT5=ON -DENABLE_CLANG_TIDY=ON /run/src/mediacopier/ && make formatcheck && make -j $(nproc)
cmake -DUSE_QT5=ON -DENABLE_TEST=ON /run/src/mediacopier/ && make -j $(nproc) && make test
cmake -DUSE_QT5=ON -DENABLE_TEST_COVERAGE=ON /run/src/mediacopier/ && make formatcheck && make -j $(nproc) && make test && make coverage
```

### :paperclip: Build Instructions for Windows

Prepare the [vcpkg](https://github.com/microsoft/vcpkg#using-vcpkg-with-cmake) environment like described in a very helpful [article by Sam Elborai](https://sam.elborai.me/articles/vscode-cpp-dev-environment-2020/)
```sh
.\bootstrap-vcpkg.bat -disableMetrics
.\vcpkg.exe install spdlog exiv2 libjpeg-turbo ffmpeg qt5 --triplet=x64-windows
.\vcpkg.exe list --triplet=x64-windows # check installed packages
```

Use the vcpkg toolchain file with cmake
```sh
cmake -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows -DSKIP_KDE=ON -B build -S .
cmake --build build --config Release
```

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
