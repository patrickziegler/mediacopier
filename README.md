# :camera: MediaCopier

[![Build Status](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml/badge.svg?branch=master)](https://github.com/patrickziegler/MediaCopier/actions/workflows/build-and-test.yml?query=branch%3Amaster)
[![Coverage Status](https://coveralls.io/repos/github/patrickziegler/MediaCopier/badge.svg?branch=master)](https://coveralls.io/github/patrickziegler/MediaCopier?branch=master)
![C++ Version](https://img.shields.io/badge/C++-17-blue.svg?style=flat&logo=c%2B%2B)

### Features
This software searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format.
The original creation date is used to **generate a folder structure** and unique filenames.
Raw image files and videos are supported as well.

This library supports **lossless auto-rotation of JPEG files with known orientation** (on the fly) when image dimensions are appropriate (divisible by 16).
If the orientation is unkown or the dimensions are inconvenient they will be copied as they are.

## Getting Started

### :hammer: Build and Install

Clone this repository and create a build directory
```sh
git clone --recursive https://github.com/patrickziegler/MediaCopier.git
cd MediaCopier && mkdir build && cd build
```

Build for installing to `/usr/local` *(default)*
```sh
cmake .. && make -j$(nproc)
```

Build for installing to another destination *(alternative)*
```sh
export INSTALL_PREFIX=~/.local

cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX .. && make -j$(nproc)

# optional
export $PATH=$PATH:$INSTALL_PREFIX/bin
export $LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PREFIX/lib
```

Execute installation
```sh
make install
```

### :factory: Build and Test with Docker

Build an image and run it as a containerized build environment.
```sh
docker build \
    --build-arg USER_NAME=$(whoami) \
    --build-arg USER_UID=$(id -u) \
    --build-arg USER_GID=$(id -g) \
    -t mediacopier-build .
docker run -it --rm -v ${PWD}:/usr/src/mediacopier mediacopier-build
```

Inside the container, run the following commands.
All build-time dependencies are solved already.
```sh
cmake -DENABLE_TEST=ON /usr/src/mediacopier/ && make -j $(nproc) && make test
```

## Translations

```sh
linguist-qt5 mediacopier-cli/lang/lang_de.ts
```

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
