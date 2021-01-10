# :camera: MediaCopier

### Features
This software searches for **tagged media files** in a given directory and copies or moves those files to another directory while renaming them according to the specified format. The original creation date is used to **generate a folder structure** and unique filenames. Raw image files and videos are supported as well.

This library supports **lossless auto-rotation of JPEG files with known orientation** (on the fly) when image dimensions are appropriate (divisible by 16). If the orientation is unkown or the dimensions are inconvenient they will be copied as they are.

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

### :whale: Build and test inside Docker container

```sh
docker build -t mediacopier-build .
docker run -it --rm -v ${PWD}:/usr/src/mediacopier mediacopier-build /bin/bash
mkdir /tmp/build && cd /tmp/build
cmake /usr/src/mediacopier && make -j$(nproc)
make test
```

## Authors

* Patrick Ziegler

## License

This project is licensed under the GPL - see the [LICENSE](LICENSE) file for details
